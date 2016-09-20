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

/**
* @file TcpUser.cpp
* @author Daniele Berto
*/

#include "DefineGeneral.h"
#include "TcpUser.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

TcpUser::TcpUser ()
{
  user_sockid = -1;
  bzero (command_sent_by_user, STANDARDBUFFERLIMIT);
}
