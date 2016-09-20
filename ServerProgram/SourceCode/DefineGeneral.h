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
* @brief This file contains same parameters used by the program. In particular, it contains the dimension of the queues used by the aquisition threads.
* @author Daniele Berto
*/

//SERVER_IP defines the IPv4 address of this application
#define SERVER_IP	"127.0.0.1"

//SERVER_PORT defines the port number of this application
#define SERVER_PORT 1111

//MAXIMUM_DRIVER indicates the maximum number of drivers which could be connected to the driver
#define MAXIMUM_DRIVER 14

//STANDARDBUFFERLIMIT defines the length of the arrays used to general operation, like the buffer one in the Main.c file 
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

//If FLUSH_OUTPUTMODULETCP == 1, TCP_NODELAY is set at every TCP/IP output
//This implies an implicit flush of the TCP socket.
//If FLUSH_OUTPUTMODULETCP != 1, TCP_NODELAY is not set at every TCP/IP output.
#define FLUSH_OUTPUTMODULETCP 1

//Definition of the addresses used in the program.
//The address of the registers could be checked using the manual of the drivers,
//the address of the variables could be checked at the beginning of the software firmware of the drivers.
#define			StopAddr				0xA000
#define			StatusAddr 				0xA102
#define 		ControlModeAddress		0xA104
#define			CurrentPositionAddress	0xA10B
#define			TargetPositionAddress	0xA301
#define			AddressCounterA			0xA10F
#define			AddressMaxVel			0xA107
#define			AddressVelHome			0xA00A
#define			AddressAcceleration		0xA109
#define			AddressDeceleration		0xA10A
#define			AddressRefVal			0xA300
#define			AddressAnalogInput0		0xA202
#define			AddressAnalogOutput0	0xA204
#define			SerialNumberAddress		0x9D05
#define			PhaseCurrentAddress		0xA103
#define			StatusStateAddress		0xA005
#define			RequestStateAddress		0xA008
#define			count_TargetPosAddress	0xA003

//Check the firmware documentation to obtain a detailed explanation of these definitions
#define			STATEHOMING				4
#define			STATEMOVEREL			17

//Number of the default slave driver.
#define			DEFAULT_SLAVE			1

//Maximum number of commands that the program can queue
#define			MAXCOMMAND				100

//Relative path of the log files
#define SERIALDRVLOG	"../../ServerProgram/LogFile/SerialDrvLog.txt"
#define FILEPARLOG	"../../ServerProgram/LogFile/FileParLog.txt"
#define ENCODERLOG	"../../ServerProgram/LogFile/EncoderLog.txt"
#define GENERALOG	"../../ServerProgram/LogFile/GeneralLog.txt"

//Max driver extension used to obtain the encoding values
#define MAXEXTENSION	-102400

//The dimension of every step to obtain the encoding values
#define ENCODINGSTEP	6400

//Definition of the time spent waiting for some actions.
#define	SLEEPCOMMAND	10000
#define	SLEEPTCP	5000
#define SLEEPMODBUS	5000
#define	SLEEPSTATUS_STATE	10000
#define SLEEPCHECK 1

//Check position tolerated error
#define POSITION_TOLERATED_ERROR	5

//Maximum number of attempt to obtain a valid state from the driver. The attempts are separated each others by SLEEPSTATUS_STATE useconds.
#define	LIMITSTATUS_STATE	20

//Maximum number of attempt to obtain a valid answer to write the log file. The attempts are separated each others by SLEEPCHECK seconds.
#define LIMITCHECKLOG	60

////////////////////////////////////////////////////////////////////////////////////////
//Be careful: these parameter modify the behaviour of the modbus library!
//
//The time to wait for a modbus answer before the timeout
#define _RESPONSE_TIMEOUT    50000

//Maximum time between the bytes of a modbus answer
#define _BYTE_TIMEOUT        50000
////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////
//WARNING! This parameter is very important because it determines if an homing procedure
//is executed at the begininng of an encoding procedure.
#define ENCODINGHOME	0


//These parameters indicate the limits of the drivers settings. For example,
//the value of the acceleration must be included in [MINACCELERATION, MAXACCELERATION] interval.
#define MAXVEL	12000
#define MINVEL	0
#define MAXVELHOME	1000
#define MINVELHOME	-1000
#define MAXACCELERATION	30000
#define MINACCELERATION	0
#define MAXDECELERATION	30000
#define MINDECELERATION	0
#define MAXPHASECURRENT	17
#define MINPHASECURRENT	10
#define MAXANALOGOUTPUT0	1023
#define MINANALOGOUTPUT0	0

//The number of fetch-execute cycles between the automatic position checking.
//The time is: SLEEPCOMMAND*usec * COUNTCHECK_LIMIT = 10000*6000usec = 60000000usec = 60sec = 1m
//SLEEPCOMMAND is defined above in this file.
#define COUNTCHECK_LIMIT	6000

//Internal definition of input modality. Change INPUTMODALITY definition to manage it, not these parameters.
#define ALL 2
#define ONLYUSER 0
#define ONLYTCP 1

#endif
