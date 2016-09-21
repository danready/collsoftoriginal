// GNU General Public License Agreement
// Copyright (C) 2015-2016 Daniele Berto daniele.fratello@gmail.com
//
// CollSoft is free software; you can redistribute it and/or modify it under
// the terms of the GNU General Public License as published by the Free Software Foundation.
// You must retain a copy of this licence in all copies.
//
// This program is distributed in the hope that it will be useful, but WITHOUT ANY
// WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
// PARTICULAR PURPOSE. See the GNU General Public License for more details.
// ---------------------------------------------------------------------------------

#include "enablepasswordusergui.h"
#include "ui_enablepasswordusergui.h"
#include "DefineGeneral.h"

#define LDAP_DEPRECATED 1
#include <ldap.h>

EnablePasswordUserGUI::EnablePasswordUserGUI(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::EnablePasswordUserGUI)
{
    user_name = "";
    authentication_value = 0;
    ui->setupUi(this);
}

EnablePasswordUserGUI::~EnablePasswordUserGUI()
{
    delete ui;
}

QString EnablePasswordUserGUI::GetAuthenticationUserName()
{
    return user_name;
}


int EnablePasswordUserGUI::GetAuthenticationValue ()
{
    return authentication_value;
}

void EnablePasswordUserGUI::on_buttonBox_accepted()
{
    if(ui->LineEdit_UserNameEnable->text() == "admin" &&
       ui->LineEdit_PasswordEnable->text() == "admin")
    {
        authentication_value = 1;
        user_name = "admin";
    }
    else
    {

        if (ui->LineEdit_UserNameEnable->text() == "")
            ui->LineEdit_UserNameEnable->setText(" ");

        if (ui->LineEdit_PasswordEnable->text() == "")
            ui->LineEdit_PasswordEnable->setText(" ");


        QByteArray tmp_1 = ui->LineEdit_UserNameEnable->text().toLatin1();
        char *user       = tmp_1.data();

        QByteArray tmp_2 = ui->LineEdit_PasswordEnable->text().toLatin1();
        char *pass       = tmp_2.data();

        authentication_value = ldap_authentication(user, pass);

        if (authentication_value == 1)
        {
            QString tmp(user);
            user_name = tmp;
        }
        else
        {
            user_name = "";
        }

    }

}

//return status.
//status = -1 : unable to query the LDAP server.
//status = 0: password mismath/the user does not exist.
//status = 1: okay.
//Input argument: the username (char* user) and the password (char* pass)
//to check.
int EnablePasswordUserGUI::ldap_authentication(char* user, char* pass)
{
    int status = -1;

    LDAP *ldap;
    LDAPMessage *answer, *entry;
    BerElement *ber;

    int  result;
    int  auth_method    = LDAP_AUTH_SIMPLE;
    int  ldap_version   = LDAP_VERSION3;
    char *ldap_host     = LDAP_HOST;
    int   ldap_port     = LDAP_PORT;
    char *ldap_dn       = LDAP_DN;
    char *ldap_pw       = LDAP_PW;
    char *base_dn       = LDAP_BASE_DN;

    // The search scope must be either LDAP_SCOPE_SUBTREE or LDAP_SCOPE_ONELEVEL
    int  scope          = LDAP_SCOPE_SUBTREE;
    // The search filter, "(objectClass=*)" returns everything. Windows can return
    // 1000 objects in one search. Otherwise, "Size limit exceeded" is returned.
    //~ char *filter        = "(&(objectClass=user)(sAMAccountName=frank4dd))";
    QString user_tmp(user);

    QString filter_tmp = "(&(objectClass=posixAccount)(uid=" + user_tmp + "))";

    QByteArray tmp = filter_tmp.toLatin1();
    char *filter       = tmp.data();

    // The attribute list to be returned, use {NULL} for getting all attributes
    //char *attrs[]       = {"memberOf", NULL};
    char *attrs[]       = {"userPassword"};
    // Specify if only attribute types (1) or both type and value (0) are returned
    int  attrsonly      = 0;
    // entries_found holds the number of objects found for the LDAP search
    int  entries_found  = 0;
    // dn holds the DN name string of the object(s) returned by the search
    char *dn            = "";
    // attribute holds the name of the object(s) attributes returned
    char *attribute     = "";
    // values is  array to hold the attribute values of the object(s) attributes
    char **values;
    // i is the for loop variable to cycle through the values[i]
    int  i              = 0;

    /* First, we print out an informational message. */
    //printf( "Connecting to host %s at port %d...\n\n", ldap_host, ldap_port );

    /* STEP 1: Get a LDAP connection handle and set any session preferences. */
    /* For ldaps we must call ldap_sslinit(char *host, int port, int secure) */
    if ( (ldap = ldap_init(ldap_host, ldap_port)) == NULL ) {
      //perror( "ldap_init failed" );
      return status;
    } else {
      //printf("Generated LDAP handle.\n");
    }

    /* The LDAP_OPT_PROTOCOL_VERSION session preference specifies the client */
    /* is an LDAPv3 client. */
    result = ldap_set_option(ldap, LDAP_OPT_PROTOCOL_VERSION, &ldap_version);

    if ( result != LDAP_OPT_SUCCESS ) {
        ldap_perror(ldap, "ldap_set_option failed!");
        return status;
    } else {
      //printf("Set LDAPv3 client version.\n");
    }

    /* STEP 2: Bind to the server. */
    // If no DN or credentials are specified, we bind anonymously to the server */
    // result = ldap_simple_bind_s( ldap, NULL, NULL );
    result = ldap_simple_bind_s(ldap, ldap_dn, ldap_pw );

    if ( result != LDAP_SUCCESS ) {
      //fprintf(stderr, "ldap_simple_bind_s: %s\n", ldap_err2string(result));
      return status;
    } else {
      //printf("LDAP connection successful.\n");
    }

    /* STEP 3: Do the LDAP search. */
    result = ldap_search_s(ldap, base_dn, scope, filter,
                           attrs, attrsonly, &answer);

    if ( result != LDAP_SUCCESS ) {
      //fprintf(stderr, "ldap_search_s: %s\n", ldap_err2string(result));
      return status;
    } else {
      //printf("LDAP search successful.\n");
    }

    /* Return the number of objects found during the search */
    entries_found = ldap_count_entries(ldap, answer);
    if ( entries_found == 0 ) {
      //fprintf(stderr, "LDAP search did not return any data.\n");

      ////////////////////
      //Very important to return 0 (incorrect username and/or password) in case of invalid username
      status = 0;
      ////////////////////

      return status;
    } else {
      //printf("LDAP search returned %d objects.\n", entries_found);
    }

    /* cycle through all objects returned with our search */
    for ( entry = ldap_first_entry(ldap, answer);
          entry != NULL;
          entry = ldap_next_entry(ldap, entry)) {

      /* Print the DN string of the object */
      dn = ldap_get_dn(ldap, entry);
      //printf("Found Object: %s\n", dn);

      // cycle through all returned attributes
      for ( attribute = ldap_first_attribute(ldap, entry, &ber);
            attribute != NULL;
            attribute = ldap_next_attribute(ldap, entry, ber)) {

        /* Print the attribute name */
        //printf("Found Attribute: %s\n", attribute);
        if ((values = ldap_get_values(ldap, entry, attribute)) != NULL) {

          /* cycle through all values returned for this attribute */
          for (i = 0; values[i] != NULL; i++) {

            QString passtocheck_tmp(pass);
            QString passtocheckLDAP_tmp(values[i]);

            if (passtocheck_tmp == passtocheckLDAP_tmp)
            {
                status = 1;
                return status;
            }
            else
            {
                status = 0;
                return status;
            }
            /* print each value of a attribute here */
            //printf("%s: %s\n", attribute, values[i] );
          }
          ldap_value_free(values);
        }
      }
      ldap_memfree(dn);
    }

    ldap_msgfree(answer);
    ldap_unbind(ldap);

    return status;

}

