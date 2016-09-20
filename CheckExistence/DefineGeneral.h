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

#ifndef __DEFINEGENERALH
#define __DEFINEGENERALH

/**
* @file DefineGeneral.h
* @brief This file contains same parameters used by the program.
* @author Daniele Berto
*/

//SERVER_IP defines the IPv4 address of this application
#define SERVER_IP	"127.0.0.1"

//SERVER_PORT defines the port number of this application
#define SERVER_PORT 1112

//STANDARDBUFFERLIMIT defines the length of the arrays used to general operation, like the buffer one in the MainExistence.c file 
#define STANDARDBUFFERLIMIT 5000

//INPUTMODALITY defines the method for fetching input.
//Available option are:
//tcp, that allows the input only via tcp/ip
//all, that allows the input either via tcp/ip and via stdin
//user, that allows the input only via stdin.
//If the option type is not recognized, all modality is actived.
#define INPUTMODALITY	"tcp"

//MAXIMIM_CLIENT defines the maximum number of clients connected via tcp/ip to this application.
#define MAXIMUM_CLIENT	100

//Maximum number of commands that the program can queue
#define	MAXCOMMAND	100

//Relative path of the CollSoft program. The relative path is obtained starting from the CheckExistence executable file.
#define	COLLSOFT_PATH	"../../ServerProgram/bin/CollSoft"

//Internal definition of input modality. Change INPUTMODALITY definition to manage it, not these parameters.
#define ALL 2
#define ONLYUSER 0
#define ONLYTCP 1

#endif
