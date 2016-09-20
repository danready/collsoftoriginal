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

#ifndef ENABLEPASSWORDUSERGUI_H
#define ENABLEPASSWORDUSERGUI_H

#include <QDialog>

namespace Ui {
class EnablePasswordUserGUI;
}

class EnablePasswordUserGUI : public QDialog
{
    Q_OBJECT

public:
    explicit EnablePasswordUserGUI(QWidget *parent = 0);
    ~EnablePasswordUserGUI();
    int GetAuthenticationValue();
    QString GetAuthenticationUserName();

private slots:
    void on_buttonBox_accepted();

private:
    Ui::EnablePasswordUserGUI *ui;
    int authentication_value;

    //return status.
    //status = -1 : unable to query the LDAP server.
    //status = 0: password mismath/the user does not exist.
    //status = 1: okay.
    //Input argument: the username (char* user) and the password (char* pass)
    //to check.
    int ldap_authentication(char* user, char* pass);
    QString user_name;
};

#endif // ENABLEPASSWORDUSERGUI_H
