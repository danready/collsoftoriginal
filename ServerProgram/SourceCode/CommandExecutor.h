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

//This file contains the declarations of the functions defined in CommandExecutor.c .
//The functions in this files call the funciotions contained in DriverFunction.c and 
//are called by Main.c  

#ifndef COMMANDEXECUTOR_H
#define COMMANDEXECUTOR_H

#include "Input.h"
#include "serialib.h"
#include "BasicModbusLibrary.h"
#include "ApplicationSetup.h"
#include "OutputModule.h"
#include "LogFile.h"
#include "TcpUser.h"
#include "CommunicationObject.h"
#include "DefineGeneral.h"
#include "Utils.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <thread>
#include <regex.h>
#include <stdbool.h>
#include <time.h>
#include <unistd.h>
#include <signal.h>
#include <iostream>
#include <vector>


//This struct is very useful to track some information about server activity.
//If GeneralStatus.assoc_file_status == 1 means the user has already executed the procedure to check
//the association between the serial numbers contained in the SerialDrvLog.txt file and the real situation.
//If GeneralStatus.par_file_status = 1 means the user has already executed the procedure to check
//the association between the parameters contained in the FileParLog.txt file and the real situation.
//If GeneralStatus.encoder_file_status = 1 means the user has already executed the procedure to check
//the association between the encoder values contained in the EncoderLog.txt file and the real situation.	

typedef struct{
bool assoc_file_status;
bool par_file_status;
bool encoder_file_status;
} file_check_status;

extern file_check_status GeneralStatus;

//These arrays record the actual serial number of the drivers.
extern unsigned int SerialNumberArray[MAXIMUM_DRIVER];
extern unsigned int SerialNumberArrayParagorn[MAXIMUM_DRIVER];
extern unsigned int SerialNumberArrayTmp[MAXIMUM_DRIVER];

//These arrays record the actual parameters of the drivers. See utils.h for the declaration of ParameterStruct.
extern ParameterStruct ParameterArray[MAXIMUM_DRIVER];
extern ParameterStruct ParameterArrayParagorn[MAXIMUM_DRIVER];
extern ParameterStruct ParameterArrayTmp[MAXIMUM_DRIVER];

//This array records the values used to accomplished the check_position procedure for each driver. 
//See utils.h for the declaration of EncoderStruct.
extern EncoderStruct EncoderArrayValue[MAXIMUM_DRIVER];

//This variable records if the user has just loaded some consistent values from the log file used to
//to accomplished the check_position procedure.
extern bool loading_encoder_from_file_okay; //Be careful: global variable!

//Help procedure. It is launched by the help command and prints the list of available commands. 
void HelpCommand ();

//All these procedure are lauched by the correspondent command in the Main.c file.
//The correspondent command will be indicated above every procedure. See CollSoftServerCommand.txt
//for the description of every command.

//check_drv_assoc
void CheckDrvAssoc (CommunicationObject& mioTCP, Input* mioinput, modbus_t* ctx);
//check_par_assoc
void CheckParAssoc (CommunicationObject& mioTCP, Input* mioinput, modbus_t* ctx);
//check_encode_assoc
void CheckEncodeAssoc (CommunicationObject& mioTCP, Input* mioinput, modbus_t* ctx);
//get_par drvnum
void GetPar (modbus_t* ctx, int get_par_value);
//set_par drvnum max_vel acceleration deceleration PhaseCurrent AnalogOutput0
void SetPar (modbus_t* ctx, int set_par_value, char* buffer);
//homing drvnum
void Homing(modbus_t* ctx, int homing_value);
//get_mov_par drvnum
void GetMovePar(modbus_t* ctx, int mov_par_value);
//move_to drvnum targetposition
void MoveTo(modbus_t* ctx, int moveto_drv_num, char* buffer);
//moveto_mult targetposition drvnum1 drvnum2 drvnum3 drvnum....
void MoveToMult(modbus_t* ctx, int moveto_drv_num, char* buffer);
//encode drvnum
void Encode(modbus_t* ctx, int encode_drv_num, EncoderStruct& drv_parameters);
//setmult_par max_vel acceleration deceleration PhaseCurrent AnalogOutput0 drvnum1 drvnum2 drvnum3 drvnum....
void SetParMult (modbus_t* ctx, int set_par_value, char* buffer);

//check_position drvnum
/////////////////////////////////////
//Return values:
//0 all okay
//-1 real position mismatch with estimated position
//-2 problem communicating with the driver
//everything > 0 the driver is blocked in an invalid state

//The output of this function is sent to all the clients connected to the applications
//This procedure is launched by get_all_parameter command. 
int CheckPositionEncoderToAll (modbus_t* ctx, int position_encoder_drv_num);

//The output of this function is sent only to the client that requires it.
int CheckPositionEncoderSingle (modbus_t* ctx, int position_encoder_drv_num);

//The output of this function is sent only to the client that requires it with a warning message.
int CheckPositionEncoderSingleWarning (modbus_t* ctx, int position_encoder_drv_num);
/////////////////////////////////////

//Send failed response for "get_par drvnum" command. 
void SendFailedGetPar(int drv_num);
//Send failed response for "get_mov_par drvnum" command.
void SendFailedGetMovPar(int drv_num);
//Send failed response for "check_position drvnum" command.
void SendFailedGetStatusPos(int drv_num);

//Fills EncoderArrayValue array whith the values found in EncoderLog.txt file.
void LoadEncoderFromFile();

//Prints the values of the EncoderArrayValue array. 
void ReadActualEncoderValue();

#endif
