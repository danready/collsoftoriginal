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

#ifndef __TCPUSERH
#define __TCPUSERH

#include "DefineGeneral.h"

class TcpUser
{
public:

  /**
   * @brief The command_sent_by_user array store the command sent by a client of the application.
   * STANDARDBUFFERLIMIT definition can be found in DefineGeneral.h file
   */
  char command_sent_by_user[STANDARDBUFFERLIMIT];

  /**
   * @brief The user_sockid variable contains the sockid of the user who sent the command.
   */
  int user_sockid;

  /**
   * @brief The TcpUser constructor sets to 0 the attribute "command_sent_by_user".
   */
  TcpUser ();
};

#endif
