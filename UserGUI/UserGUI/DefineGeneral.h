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

#ifndef DEFINEGENERAL_H
#define DEFINEGENERAL_H

//Ip of the CollSoft server.
//#define COLLSOFT_IP "192.84.144.245"
#define COLLSOFT_IP "127.0.0.1"

//Port to communicate with the CollSoft server
#define COLLSOFT_PORT 1111

//Ip of the CheckExistence servre.
#define EXISTENCE_IP "127.0.0.1"

//Port to communicate with the CheckExistence server
#define EXISTENCE_PORT 1112

//Ldap server Ip
#define LDAP_HOST "127.0.0.1"
//Ldap server Port
#define LDAP_PORT 389
//Ldap admin path
#define LDAP_DN "cn=admin, dc=elinp, dc=com"
//Ldap admin password
#define LDAP_PW "fantinodivaren"
//Ldap base dn
#define LDAP_BASE_DN "ou=wp09,dc=elinp,dc=com"

//Sled conversion factor
#define SLED_CONVERSION_1 12.8f //Each rotation correspond to 200*128 step and 2 mm. So 12.8 is the number of step per micron.
#define SLED_CONVERSION_2 12.8f
#define SLED_CONVERSION_3 12.8f
#define SLED_CONVERSION_4 12.8f
#define SLED_CONVERSION_5 12.8f
#define SLED_CONVERSION_6 12.8f
#define SLED_CONVERSION_7 12.8f
#define SLED_CONVERSION_8 12.8f
#define SLED_CONVERSION_9 12.8f
#define SLED_CONVERSION_10 12.8f
#define SLED_CONVERSION_11 12.8f
#define SLED_CONVERSION_12 12.8f
#define SLED_CONVERSION_13 12.8f
#define SLED_CONVERSION_14 12.8f

#define MAX_OPENING_MICRON 30000 //30000 micron

//Time to wait for a message. DEPRECATED
#define WAITING_TIME 100000

#define TCPINLOGFILE 0 //1==yes tcp message in log file
                       //0 or other == no tcp message in log file

#endif // DEFINEGENERAL_H
