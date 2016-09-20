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

//This file contains the definitions of the procedures declared in CommandExecutor.h
//Each procedure is called by Main.c and calls the procedures defined in DriverFunction.c .

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
#include "CommandExecutor.h"
#include "DriverFunction.h"
#include "Linear.h"

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

using namespace std;

//Definition of global variables declared in CommandExecutor.h .
//See the declarions in that file for more informations.

unsigned int SerialNumberArray[MAXIMUM_DRIVER];
unsigned int SerialNumberArrayParagorn[MAXIMUM_DRIVER];
unsigned int SerialNumberArrayTmp[MAXIMUM_DRIVER];

ParameterStruct ParameterArray[MAXIMUM_DRIVER];
ParameterStruct ParameterArrayParagorn[MAXIMUM_DRIVER];
ParameterStruct ParameterArrayTmp[MAXIMUM_DRIVER];

EncoderStruct EncoderArrayValue[MAXIMUM_DRIVER];

file_check_status GeneralStatus;

bool loading_encoder_from_file_okay;

//This function sends -1 to the client who has sent "get_par drvnum" command
//when the execution of the command failed.
void SendFailedGetPar(int drv_num)
{
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	output_module->Output("getpar " + to_string(drv_num) + " MaxVel " + to_string(-1) + "\n");
	output_module->Output("getpar " + to_string(drv_num) + " VelHome " + to_string(-1) + "\n");
	output_module->Output("getpar " + to_string(drv_num) + " Acceleration " + to_string(-1) + "\n");
	output_module->Output("getpar " + to_string(drv_num) + " Deceleration " + to_string(-1) + "\n");
	output_module->Output("getpar " + to_string(drv_num) + " PhaseCurrent " + to_string(-1) + "\n");
	output_module->Output("getpar " + to_string(drv_num) + " AnalogOutput0 " + to_string(-1) + "\n");			
}

//This function sends -1 to the client who has sent "get_mov_par drvnum" command
//when the execution of the command failed.
void SendFailedGetMovPar(int drv_num)
{
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	output_module->Output("get_mov_par " + to_string(drv_num) + " CurrentPosition " + to_string(-1) + "\n");
	output_module->Output("get_mov_par " + to_string(drv_num) + " AnalogInput0 " + to_string(-1) + "\n");
}

//This function sends -2 to the client who has sent "check_position drvnum" command
//when the execution of the command failed.
void SendFailedGetStatusPos(int drv_num)
{

	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	output_module->OutputAll("get_pos_status " + to_string(drv_num) + " " + to_string(-2) + "\n");
	
}

//This function prints the list of the commands available by the server.
void HelpCommand()
{
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	ApplicationSetup* command_executor_application_setup;
	command_executor_application_setup = ApplicationSetup::Instance();
	
	output_module->Output("These are command you can execute (they are all CASE INSENSITIVE):\n\n");
	output_module->Output("-connect absoluteprogrammerpath:\nopen a new connection with the programmer. Connect command must be execute before all others.\n\n");				
	output_module->Output("-move_to drvnum val:\nset to val countTargetPos of the driver specified with drvnum. Each unity of val correspond to 1/128 of step\n\n");
	output_module->Output("-moveto_mult val drvnum1 drvnum2 drvnum3 drvnumx ...:\nset to val countTargetPos of the drivers specified with drvnum. Each unity of val correspond to 1/128 of step\n\n");
	output_module->Output("-set_par drvnum max_vel acceleration deceleration PhaseCurrent AnalogOutput0:\nset max_vel (Each unity of maxvel correspond to 0.25rpm), acceleration and deceleration (Each unity of acceleration and deceleration correspond to 1rpm/s), phasecurrent and AnalogOutput0 of the driver specified with drvnum\n\n");
	output_module->Output("-setmult_par max_vel acceleration deceleration PhaseCurrent AnalogOutput0 drvnum1 drvnum2 drvnum3 drvnumx ...:\nset max_vel (Each unity of maxvel correspond to 0.25rpm), acceleration and deceleration (Each unity of acceleration and deceleration correspond to 1rpm/s), phasecurrent and AnalogOutput0 of the drivers specified with drvnum\n\n");
	output_module->Output("-read_serial_log:\nread the content of SerialDrvLog.txt file which contains the associations between drvnum and SerialNumber\n\n");
	output_module->Output("-read_par_log:\nread the content of FileParLog.txt file which contains the associations between drvnum and max_vel, acceleration, deceleration, PhaseCurrent, AnalogOutput0\n\n");
	output_module->Output("-read_encoder_log:\nread the content of EncoderLog.txt file which contains the data of the encoder linear regression\n\n");
	output_module->Output("-read_log:\nread the content of GeneralLog.txt\n\n");
	output_module->Output("-check_drv_assoc:\ncheck the association between the content of read_serial_log and the real situation\n\n");
	output_module->Output("-check_par_assoc:\ncheck the association between the content of read_par_log and the real situation\n\n");
	output_module->Output("-check_encode_assoc:\ncheck the association between the content of read_encoder_log and the real situation\n\n");
	output_module->Output("-check_position drvnum:\ncheck if the CurrentPosition of drvnum correspond to the estimated one\n\n");
	output_module->Output("-get_par drvnum:\nreturn max_vel, acceleration, deceleration, PhaseCurrent and AnalogOutput0 of the driver specified in drvnum\n\n");
	output_module->Output("-get_mov_par drvnum:\nreturn CurrentPosition and AnalogInput0 of the driver specified in drvnum\n\n");	
	output_module->Output("-get_all_parameter:\nreturn max_vel, acceleration, deceleration, PhaseCurrent, AnalogOutput0, CurrentPosition and AnalogInput0 of all drivers\n\n");
	output_module->Output("-homing drvnum:\nstart the homing procedure of the driver specified in drvnum\n\n");
	output_module->Output("-homing_mult drvnum1 drvnum2 drvnum3 drvnum4 drvnumx ...:\nstart the homing procedure of the driver specified in drvnum\n\n");
	output_module->Output("-endode drvnum:\nstart the encoding procedure of the driver specified in drvnum\n\n");
	output_module->Output("-check_internal_status:\nthis command retrieves the content of the GeneralStatus struct\n\n");
	output_module->Output("-load_encoder_from_file:\nthis command gets the encoding parameters for each drivers from the EncoderLog.txt file and use it to accomplished the check_position command\n\n");
	output_module->Output("-read_actual_encoder_values:\nthis command prints the actual encoding parameters that will be used to accomplished the check_position command\n\n");
	output_module->Output("-device_list:\nthis command prints the device contained in /dev\n\n");		
	output_module->Output("-help:\nPrint this command list\n\n");
	output_module->Output("-exit:\nExit from the program: this command is enabled only in stdin mode.\n\n");
	
	
	if (command_executor_application_setup->input_mode != ONLYTCP) {
		fflush(stdout);
	}	
}

//This function tries to read SerialDrvLog.txt file, then compares the serial numbers
//read in that file with the serial numbers obtained querying the drivers.
void CheckDrvAssoc (CommunicationObject& mioTCP, Input* mioinput, modbus_t* ctx)
{
	//This function flushes the pending datagrams to the drivers.
	modbus_flush(ctx);

	//This array will be used to save the user response from the stdin.
	char* buffer;
	buffer = (char*) malloc (STANDARDBUFFERLIMIT);
	bzero(buffer, STANDARDBUFFERLIMIT);

	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.
	int function_status = 0;
	
	//This object saves the user response from the TCP/IP communication.
	TcpUser command_received_by_user;

	//This variable records the latest log record from the SerialDrvLog.txt file
	//Ex. The last log is the number 62 (Log 62 Date ...) max_log will be equal
	//to 62. So, using this information, the next log will be the number 63.
	int max_log = 0;
	
	//This variable will contain the SerialNumber read from the drivers.
	//Ex. SerialNumber = ReadSerialNumber(ctx, &function_status);
	unsigned int SerialNumber = 0;

	//Singleton to manage the output of the program.
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	//Singleton to record the path of the LogFiles.
	ApplicationSetup* command_executor_application_setup;
	command_executor_application_setup = ApplicationSetup::Instance();
	
	//Singleton to write to the log files.
	LogFile* logfile;
	logfile = LogFile::Instance();

	//If LogFile exists.
	//Attempt to access SerialDrvLog.txt in order to determine if it exists.
	if( access( command_executor_application_setup->application_setup_serial_drv_log_file_path, F_OK ) != -1 ) {
		
		//It's well because it exists. If I can't open it, logfile attribute can_write is set to 0 so no segmentation fault will happened!
		//N.B. Also the functions contained in DriverFunction.c used the LogFile singleton so the programmer has
		//to set the path of the correct log file before writing to the log file.
		logfile->LogFileSet(command_executor_application_setup->application_setup_serial_drv_log_file_path);
		
		//This string will contained the buffered output that will be sent to the server.
		string output_tmp;
		
		//These vector will contain the serial numbers read by SerialDrvLog.txt. 
		vector<SerialCouple> serial_list;
		vector<SerialCouple>& serial_reference = serial_list;
		
		//Scanning SerialDrvLog.txt recording in serial_reference the drivers serial number
		//and in max_log the number of the latest log.
		Analizza1(serial_reference, &max_log);
		
		//Printing the serial numbers found in SerialDrvLog.txt .
		int i = 0;
		output_tmp += "Check Drv Assoc: SerialDrvLog.txt exists\n\n";
		output_tmp += "It contains these serial number:\n";
		for (i = 0; i < serial_reference.size(); i++)
		{
			output_tmp += to_string (i+1) + " drv num: " + " " + to_string(serial_reference[i].drv_num) + " ";
			output_tmp += "Serial number: " + to_string(serial_reference[i].serial_number) + "\n";
		}
		
		output_tmp += "\nThe real situation is:\n";
		
		//Printing the serial numbers found in the drivers connected to the programmer.
		int j = 0;
		for (j=0; j < MAXIMUM_DRIVER; j++)
		{
			SerialNumber = 0;
			modbus_set_slave(ctx, j+1);
			SerialNumber = ReadSerialNumber(ctx, &function_status);
			if (function_status != -1)
			{
				SerialNumberArrayParagorn[j] = SerialNumber;
				output_tmp += "drv num: " + to_string(j+1) + " Serial Number: " + to_string(SerialNumberArrayParagorn[j]) + "\n";
			}
			else
			{
				output_tmp += "drv num: " + to_string(j+1) + " not found\n";
				SerialNumberArrayParagorn[j] = 0;
			}
		}
		
		//////////////
		bzero(SerialNumberArrayTmp, MAXIMUM_DRIVER);
		
		//Reordering the information.
		for (i = 0; i < serial_reference.size(); i++)
		{
			if (serial_reference[i].drv_num <= MAXIMUM_DRIVER && serial_reference[i].drv_num > 0 )
				SerialNumberArrayTmp[serial_reference[i].drv_num - 1] = serial_reference[i].serial_number;
		}
		
		output_tmp += "\n";
		
		//Print some stats about the comparison between the values found in SerialDrvLog.txt
		//and the real situation. 
		for (i = 0; i < MAXIMUM_DRIVER; i++)
		{
			if (SerialNumberArrayParagorn[i] == 0 && SerialNumberArrayTmp[i] != 0)
				output_tmp += "Driver number " + to_string(i+1) + " not connected to the programmer but found in the log file with serial number: " + 
				to_string(SerialNumberArrayTmp[i]) + "\n";
				
			else if (SerialNumberArrayParagorn[i] != 0 && SerialNumberArrayTmp[i] == 0)
				output_tmp += "Driver number " + to_string(i+1) + " not found in the log file but connected to the programmer with serial number: " +
				to_string(SerialNumberArrayParagorn[i]) + "\n";
				
			else if (SerialNumberArrayParagorn[i] == 0 && SerialNumberArrayTmp[i] == 0)	
				output_tmp += "Driver number " + to_string(i+1) + " not found neither in the log file nor connected to the programmer\n";
			
			else if (SerialNumberArrayParagorn[i] == SerialNumberArrayTmp[i])
				output_tmp += "Driver number " + to_string(i+1) + " has the same serial number either in the log file and in reality: " + to_string(SerialNumberArrayTmp[i]) + "\n";
				
			else if (SerialNumberArrayParagorn[i] != SerialNumberArrayTmp[i])	
				output_tmp += "Driver number " + to_string(i+1) + " has the serial number " + to_string(SerialNumberArrayTmp[i]) + " in the log_file and the serial number " + 
				to_string(SerialNumberArrayParagorn[i]) + "in reality\n";
		
		}
		
		//////////////
		
		//Sending the information to the user. He will decide if write the new serial number to SerialDrvLog.txt or not.
		output_tmp += "\nIs this situation okay (the serial number of the drivers connected to the programmer will be written to the log file)?\n";
		
		output_module->Output(output_tmp);
		
		int count = 0;
		
		//There is a timeout: if the user does not send any response in 1 minute,
		//the server assume that the user has rejected the request.
		do {
		sleep(SLEEPCHECK);
		count ++; //count is the number of attempts (it could be LIMITCHECKLOG on all)
		command_received_by_user = mioTCP.GetCommand ();
		if (command_executor_application_setup->input_mode != ONLYTCP)
			mioinput->GetInput (buffer);
		}
		while (buffer[0] != 'y' && buffer[0] != 'n' && command_received_by_user.command_sent_by_user[0] != 'y' && command_received_by_user.command_sent_by_user[0] != 'n' && count < LIMITCHECKLOG);
		
		//If the user has accepted the situation, the program writes the
		//actual serial numbers to the log file. 
		if (buffer[0] == 'y' || command_received_by_user.command_sent_by_user[0] == 'y')
		{
			GeneralStatus.assoc_file_status = 1;
			output_module->Output("InternalStatusSerial: " + to_string(GeneralStatus.assoc_file_status)  + "\n");
			
			int i = 0;
			max_log = max_log + 1;
			
			//It's well because it exists. If I can't open it, logfile attribute can_write is set to 0 so no segmentation fault will happened!
			logfile->LogFileSet(command_executor_application_setup->application_setup_serial_drv_log_file_path);
			
			logfile->LogFileWriteString("\nLog " + to_string(max_log) + " \n");
			
			logfile->LogFileWriteString("Date: " + GetTime() + "\n");
			
			for (i=0; i < MAXIMUM_DRIVER; i++)
			{
				if (SerialNumberArrayParagorn[i] != 0)
					{
						logfile->LogFileWriteString(to_string(i+1) + "\t" + to_string(SerialNumberArrayParagorn[i]) + "\n");
					}
			}				
		}
		else
		{
			GeneralStatus.assoc_file_status = 0;
			output_module->Output("InternalStatusSerial: " + to_string(GeneralStatus.assoc_file_status)  + "\n");
			
			if (count == LIMITCHECKLOG)
				output_module->Output("Exp: check_drv_assoc timeout: you waited for a time longer then 60 secs\n");
		}
		
	} else {
		
		//If SerialDrvLog.txt does not exist, the program collects the serial numbers from the
		//drivers actually connected to the programmer and asks the user if he approves the
		//situation.
		
		//This string will contained the buffered output that will be sent to the server.
		string output_tmp;
		
		output_tmp += "Check Drv Assoc: File SerialDrvLog.txt does not exist:\nThese drivers are found to be connected to the programmer:\n";
		
		//Collecting and printing the drivers serial number.
		int i = 0;
		for (i=0; i < MAXIMUM_DRIVER; i++)
		{
			SerialNumber = 0;
			modbus_set_slave(ctx, i+1);
			SerialNumber = ReadSerialNumber(ctx, &function_status);
			if (function_status != -1)
			{
				SerialNumberArray[i] = SerialNumber;
				output_tmp += " Driver number " + to_string(i+1) + " Serial Number: " + to_string(SerialNumberArray[i]) + "\n";
			}
			else
			{
				output_tmp += "Driver number " + to_string(i+1) + " non trovato\n";
				SerialNumberArray[i] = -1;
			}
		}
		
		//Sending the information to the user. He will decide if write the new serial number to SerialDrvLog.txt or not.		
		output_tmp += "\nIs this situation okay (the serial number of the drivers connected to the programmer will be written to the log file)?\n";
		output_module->Output(output_tmp);
		
		//There is a timeout: if the user does not send any response in 1 minute,
		//the server assume that the user has rejected the request.		
		int count = 0;
		do {
		sleep(SLEEPCHECK);
		count ++;  //count is the number of attempts (it could be LIMITCHECKLOG on all)
		command_received_by_user = mioTCP.GetCommand ();
		if (command_executor_application_setup->input_mode != ONLYTCP)
			mioinput->GetInput (buffer);
		}
		while (buffer[0] != 'y' && buffer[0] != 'n' && command_received_by_user.command_sent_by_user[0] != 'y' && command_received_by_user.command_sent_by_user[0] != 'n' && count < LIMITCHECKLOG);
		
		//If the user has accepted the situation, the program writes the
		//actual serial numbers to the log file. 		
		if (buffer[0] == 'y' || command_received_by_user.command_sent_by_user[0] == 'y')
		{
			logfile->LogFileSet(command_executor_application_setup->application_setup_serial_drv_log_file_path);
			GeneralStatus.assoc_file_status = 1;
			output_module->Output("InternalStatusSerial: " + to_string(GeneralStatus.assoc_file_status)  + "\n");
			
			int i = 0;
			max_log = max_log + 1;
			logfile->LogFileWriteString("\nLog " + to_string(max_log) + " \n");
			
			logfile->LogFileWriteString("Date: " + GetTime() + "\n");
			for (i=0; i < MAXIMUM_DRIVER; i++)
			{
				if (SerialNumberArray[i] != 0)
					{
						logfile->LogFileWriteString(to_string(i+1) + "\t" + to_string(SerialNumberArray[i]) + "\n");
					}
			}
		}
		else
		{
			GeneralStatus.assoc_file_status = 0;
			output_module->Output("InternalStatusSerial: " + to_string(GeneralStatus.assoc_file_status)  + "\n");
			
			if (count == LIMITCHECKLOG)
				output_module->Output("Exp: check_drv_assoc timeout: you waited for a time longer then 60 secs\n");			
		}						
		
	}
	
	free(buffer);
}

//This function tries to read FileParLog.txt file, then compares the parameters
//read in that file with the parameters obtained querying the drivers.
void CheckParAssoc (CommunicationObject& mioTCP, Input* mioinput, modbus_t* ctx)
{
	//This function flushes the pending datagrams to the drivers.
	modbus_flush(ctx);

	//This array will be used to save the user response from the stdin.
	char* buffer;
	buffer = (char*) malloc (STANDARDBUFFERLIMIT);
	bzero(buffer, STANDARDBUFFERLIMIT);

	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.
	int function_status = 0;
	
	//This object saves the user response from the TCP/IP communication.	
	TcpUser command_received_by_user;

	//This variable records the latest log record from the FileParLog.txt file
	//Ex. The last log is the number 62 (Log 62 Date ...) max_log will be equal
	//to 62. So, using this information, the next log will be the number 63.
	int max_log = 0;
	
	//This variable will contain the parameters read from the drivers.
	//Ex. tmp_parameter_struct.max_vel = ReadMaxVel(ctx, &function_status, "check_drv_assoc_exp: ");
	ParameterStruct tmp_parameter_struct;

	//Singleton to manage the output of the program.
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	//Singleton to record the path of the LogFiles.	
	ApplicationSetup* command_executor_application_setup;
	command_executor_application_setup = ApplicationSetup::Instance();
	
	//Singleton to write to the log files.	
	LogFile* logfile;
	logfile = LogFile::Instance();

	//If LogFile exists.
	//Attempt to access FileParLog.txt in order to determine if it exists.
	if( access( command_executor_application_setup->application_setup_file_par_log_path, F_OK ) != -1 ) {
				
		//This string will contained the buffered output that will be sent to the server.		
		string output_tmp;
		
		//These vector will contain the parameters read by FileParLog.txt. 		
		vector<ParameterStruct> parameter_list;
		vector<ParameterStruct>& parameter_reference = parameter_list;
		
		//Scanning FileParLog.txt recording in parameter_reference the drivers parameters
		//and in max_log the number of the latest log.		
		AnalizzaFilePar(parameter_reference, &max_log);
		
		//Printing the parameters found in FileParLog.txt .		
		int i = 0;
		output_tmp += "Check Par Assoc: FileParLog.txt exists\n\n";
		output_tmp += "It contains these parameters:\n";
				
		for (i = 0; i < parameter_reference.size(); i++)
		{
			output_tmp += to_string(parameter_reference[i].drv_num) + "\t";
			output_tmp += "MaxVel: " + to_string(parameter_reference[i].max_vel) + "\t";
			output_tmp += "VelHome: " + to_string(parameter_reference[i].vel_home) + "\t";			
			output_tmp += "Acceleration: " + to_string(parameter_reference[i].acceleration) + "\t";
			output_tmp += "Deceleration: " + to_string(parameter_reference[i].deceleration) + "\t";
			output_tmp += "PhaseCurrent: " + to_string(parameter_reference[i].phase_current) + "\t";
			output_tmp += "AnalogOutput0: " + to_string(parameter_reference[i].analog_output0) + "\n";			
		}
		
		output_tmp += "\nThe real situation is:\n";
		
		//Saving and printing the parameters found in the drivers connected to the programmer.
		int j = 0;
		for (j=0; j < MAXIMUM_DRIVER; j++)
		{
			int error = 0;
			
			//modbus_set_slave return value must be checked in future
			modbus_set_slave(ctx, j+1);
			
			tmp_parameter_struct.max_vel = ReadMaxVel(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;
			tmp_parameter_struct.vel_home = ReadVelHome(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;			
			tmp_parameter_struct.acceleration = ReadAcceleration(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;
			tmp_parameter_struct.deceleration = ReadDeceleration(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;
			tmp_parameter_struct.phase_current = ReadPhaseCurrent(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;
			tmp_parameter_struct.analog_output0 = ReadAnalogOutput0(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;							
			
			if (error != -1)
			{
				ParameterArrayParagorn[j].max_vel = tmp_parameter_struct.max_vel;
				ParameterArrayParagorn[j].vel_home = tmp_parameter_struct.vel_home;				
				ParameterArrayParagorn[j].acceleration = tmp_parameter_struct.acceleration;
				ParameterArrayParagorn[j].deceleration = tmp_parameter_struct.deceleration;
				ParameterArrayParagorn[j].phase_current = tmp_parameter_struct.phase_current;
				ParameterArrayParagorn[j].analog_output0 = tmp_parameter_struct.analog_output0;
				output_tmp += "Driver number " + to_string(j+1) + " max_vel: " + to_string(ParameterArrayParagorn[j].max_vel) + "\t";
				output_tmp += "vel_home: " + to_string(ParameterArrayParagorn[j].vel_home) + "\t";				
				output_tmp += "acceleration: " + to_string(ParameterArrayParagorn[j].acceleration) + "\t";
				output_tmp += "deceleration: " + to_string(ParameterArrayParagorn[j].deceleration) + "\t";
				output_tmp += "phase_current: " + to_string(ParameterArrayParagorn[j].phase_current) + "\t";
				output_tmp += "analog_output0: " + to_string(ParameterArrayParagorn[j].analog_output0) + "\n";
			}
			else
			{
				output_tmp += "Driver number " + to_string(j+1) + " not found\n";
				ParameterArrayParagorn[j].max_vel = -1;
				ParameterArrayParagorn[j].vel_home = -1;				
				ParameterArrayParagorn[j].acceleration = -1;
				ParameterArrayParagorn[j].deceleration = -1;
				ParameterArrayParagorn[j].phase_current = -1;
				ParameterArrayParagorn[j].analog_output0 = -1;
			}
			
		}
		
		output_tmp += "\n";

		//////////////
		//Emptying the struct that will contain the log file parameters reordered.
		for (i = 0; i < MAXIMUM_DRIVER; i++)
		{
			ParameterArrayTmp[i].drv_num = -1;
			ParameterArrayTmp[i].max_vel = -1;
			ParameterArrayTmp[i].vel_home = -1;				
			ParameterArrayTmp[i].acceleration = -1;
			ParameterArrayTmp[i].deceleration = -1;
			ParameterArrayTmp[i].phase_current = -1;
			ParameterArrayTmp[i].analog_output0 = -1;			
		}
		
		//Reordering the parameters found in the log file.
		for (i = 0; i < parameter_reference.size(); i++)
		{
			if (parameter_reference[i].drv_num <= MAXIMUM_DRIVER && parameter_reference[i].drv_num > 0)
			{
				ParameterArrayTmp[parameter_reference[i].drv_num - 1].drv_num = parameter_reference[i].drv_num;
				ParameterArrayTmp[parameter_reference[i].drv_num - 1].max_vel = parameter_reference[i].max_vel;
				ParameterArrayTmp[parameter_reference[i].drv_num - 1].vel_home = parameter_reference[i].vel_home;				
				ParameterArrayTmp[parameter_reference[i].drv_num - 1].acceleration = parameter_reference[i].acceleration;
				ParameterArrayTmp[parameter_reference[i].drv_num - 1].deceleration = parameter_reference[i].deceleration;
				ParameterArrayTmp[parameter_reference[i].drv_num - 1].phase_current = parameter_reference[i].phase_current;
				ParameterArrayTmp[parameter_reference[i].drv_num - 1].analog_output0 = parameter_reference[i].analog_output0;		
			}
		}
		
		
		output_tmp += "\n";
		
		//Print some stats about the comparison between the values found in FileParLog.txt
		//and the real situation.
		for (i = 0; i < MAXIMUM_DRIVER; i++)
		{
			if (ParameterArrayParagorn[i].max_vel == -1 && ParameterArrayTmp[i].max_vel != -1)
				output_tmp += "Driver number " + to_string(i+1) + " not connected to the programmer but found in the log file\n";
			
			else if (ParameterArrayParagorn[i].max_vel != -1 && ParameterArrayTmp[i].max_vel == -1)
				output_tmp += "Driver number " + to_string(i+1) + " not found in the log file but connected to the programmer\n";
				
			else if (ParameterArrayParagorn[i].max_vel == -1 && ParameterArrayTmp[i].max_vel == -1)	
				output_tmp += "Driver number " + to_string(i+1) + " not found neither in the log file nor connected to the programmer\n";
			
			else if (ParameterArrayParagorn[i].max_vel == ParameterArrayTmp[i].max_vel && ParameterArrayParagorn[i].vel_home == ParameterArrayTmp[i].vel_home &&
					 ParameterArrayParagorn[i].acceleration == ParameterArrayTmp[i].acceleration && ParameterArrayParagorn[i].deceleration == ParameterArrayTmp[i].deceleration &&
					 ParameterArrayParagorn[i].phase_current == ParameterArrayTmp[i].phase_current && ParameterArrayParagorn[i].analog_output0 == ParameterArrayTmp[i].analog_output0)
				output_tmp += "Driver number " + to_string(i+1) + " has the same parameter either in the log file and in reality\n";
				
			else	
				output_tmp += "Driver number " + to_string(i+1) + " parameters mismatch between the log file and the reality\n";
		
		}

		//////////////
		
		//Sending the information to the user. He will decide if write the new parameters to FileParLog.txt or not.		
		output_tmp += "\nIs this situation okay (the parameters of the drivers connected to the programmer will be written to the log file)?\n";
		
		output_module->Output(output_tmp);
		
		//There is a timeout: if the user does not send any response in 1 minute,
		//the server assume that the user has rejected the request.			
		int count = 0;
		do {
		sleep(SLEEPCHECK);
		count ++;  //count is the number of attempts (it could be LIMITCHECKLOG on all)
		command_received_by_user = mioTCP.GetCommand ();
		if (command_executor_application_setup->input_mode != ONLYTCP)
			mioinput->GetInput (buffer);
		}
		while (buffer[0] != 'y' && buffer[0] != 'n' && command_received_by_user.command_sent_by_user[0] != 'y' && command_received_by_user.command_sent_by_user[0] != 'n' && count < LIMITCHECKLOG);
		
		//If the user has accepted the situation, the program writes the
		//actual parameters to the log file FileParLog.txt. 			
		if (buffer[0] == 'y' || command_received_by_user.command_sent_by_user[0] == 'y')
		{
			GeneralStatus.par_file_status = 1;
			output_module->Output("InternalStatusParameter: " + to_string(GeneralStatus.par_file_status)  + "\n");
			
			int i = 0;
			max_log = max_log + 1;
			
			//file exists
			logfile->LogFileSet(command_executor_application_setup->application_setup_file_par_log_path);
			
			logfile->LogFileWriteString("\nLog " + to_string(max_log) + " \n");
			
			logfile->LogFileWriteString("Date: " + GetTime() + "\n");
			
			logfile->LogFileWriteString("DrvNum\tMaxVel\tVelHome\tAcceleration\tDeceleration\tPhaseCurrent\tAnalogOutput0\n");
			
			for (i=0; i < MAXIMUM_DRIVER; i++)
			{
				if (ParameterArrayParagorn[i].max_vel != -1 && ParameterArrayParagorn[i].vel_home != -1 && ParameterArrayParagorn[i].acceleration != -1 && ParameterArrayParagorn[i].deceleration != -1 && ParameterArrayParagorn[i].phase_current != -1  && ParameterArrayParagorn[i].analog_output0 != -1)
					{
						logfile->LogFileWriteString(to_string(i+1) + "\t" + to_string(ParameterArrayParagorn[i].max_vel) + "\t" + to_string(ParameterArrayParagorn[i].vel_home) + "\t" + to_string(ParameterArrayParagorn[i].acceleration) + 
						"\t" + to_string(ParameterArrayParagorn[i].deceleration) + "\t" + to_string(ParameterArrayParagorn[i].phase_current) + "\t" + to_string(ParameterArrayParagorn[i].analog_output0) + "\n");
					}							
			}					
		}
		else
		{
			//output_module->Output("Non ha accettato");
			GeneralStatus.par_file_status = 0;
			output_module->Output("InternalStatusParameter: " + to_string(GeneralStatus.par_file_status)  + "\n");
			
			if (count == LIMITCHECKLOG)
			//~ output_module->Output("Exp: check_drv_assoc timeout: you waited for a time longer then 60 secs\n");
				output_module->Output("Exp: check_par_assoc timeout: you waited for a time longer then 60 secs\n");
		}
		
	} else {
		
		//file FileParLog.txt doesn't exist
		
		//If FileParLog.txt does not exist, the program collects the parameters from the
		//drivers actually connected to the programmer and asks the user if he approves the
		//situation.
		
		//This string will contained the buffered output that will be sent to the server.
		string output_tmp;
		
		//~ output_tmp += "Check Par Assoc: Ho trovato queste informazioni: \n";
		
		//Collecting and printing the parameters from the drivers connected to the programmer.
		output_tmp += "Check Par Assoc: File FileParLog.txt does not exist:\nThese drivers are found in reality:\n";		
		int i = 0;
		for (i=0; i < MAXIMUM_DRIVER; i++)
		{
			
			int error = 0;

			modbus_set_slave(ctx, i+1);
			
			tmp_parameter_struct.max_vel = ReadMaxVel(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;
			tmp_parameter_struct.vel_home = ReadVelHome(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;			
			tmp_parameter_struct.acceleration = ReadAcceleration(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;
			tmp_parameter_struct.deceleration = ReadDeceleration(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;
			tmp_parameter_struct.phase_current = ReadPhaseCurrent(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;
			tmp_parameter_struct.analog_output0 = ReadAnalogOutput0(ctx, &function_status, "check_drv_assoc_exp: ");
			if (function_status == -1) error = -1;				
			
			if (error != -1)
			{
				ParameterArray[i].max_vel = tmp_parameter_struct.max_vel;
				ParameterArray[i].vel_home = tmp_parameter_struct.vel_home;				
				ParameterArray[i].acceleration = tmp_parameter_struct.acceleration;
				ParameterArray[i].deceleration = tmp_parameter_struct.deceleration;
				ParameterArray[i].phase_current = tmp_parameter_struct.phase_current;
				ParameterArray[i].analog_output0 = tmp_parameter_struct.analog_output0;
				output_tmp += "Driver number " + to_string(i+1) + " max_vel: " + to_string(ParameterArray[i].max_vel) + "\n";
				output_tmp += "Driver number " + to_string(i+1) + " vel_home: " + to_string(ParameterArray[i].vel_home) + "\n";				
				output_tmp += "Driver number " + to_string(i+1) + " acceleration: " + to_string(ParameterArray[i].acceleration) + "\n";
				output_tmp += "Driver number " + to_string(i+1) + " deceleration: " + to_string(ParameterArray[i].deceleration) + "\n";
				output_tmp += "Driver number " + to_string(i+1) + " phase_current: " + to_string(ParameterArray[i].phase_current) + "\n";
				output_tmp += "Driver number " + to_string(i+1) + " analog_output0: " + to_string(ParameterArray[i].analog_output0) + "\n";
			}
			else
			{
				output_tmp += "Driver number " + to_string(i+1) + " not found\n";
				ParameterArray[i].max_vel = -1;
				ParameterArray[i].vel_home = -1;
				ParameterArray[i].acceleration = -1;
				ParameterArray[i].deceleration = -1;
				ParameterArray[i].phase_current = -1;
				ParameterArray[i].analog_output0 = -1;
			}
		}
		
		//Sending the information to the user. He will decide if write the new parameters to FileParLog.txt or not.		
		output_tmp += "\nIs this situation okay (the parameters of the drivers connected to the programmer will be written to the log file)?\n";
		output_module->Output(output_tmp);
		
		//There is a timeout: if the user does not send any response in 1 minute,
		//the server assume that the user has rejected the request.			
		int count = 0;
		do {
		sleep(SLEEPCHECK);
		count ++;  //count is the number of attempts (it could be LIMITCHECKLOG on all)
		command_received_by_user = mioTCP.GetCommand ();
		if (command_executor_application_setup->input_mode != ONLYTCP)
			mioinput->GetInput (buffer);
		}
		while (buffer[0] != 'y' && buffer[0] != 'n' && command_received_by_user.command_sent_by_user[0] != 'y' && command_received_by_user.command_sent_by_user[0] != 'n' && count < LIMITCHECKLOG);
		
		//If the user has accepted the situation, the program writes the
		//actual parameters to the log file FileParLog.txt.		
		if (buffer[0] == 'y' || command_received_by_user.command_sent_by_user[0] == 'y')
		{
			logfile->LogFileSet(command_executor_application_setup->application_setup_file_par_log_path);
			
			GeneralStatus.par_file_status = 1;
			output_module->Output("InternalStatusParameter: " + to_string(GeneralStatus.par_file_status)  + "\n");
			
			int i = 0;
			max_log = max_log + 1;
			logfile->LogFileWriteString("\nLog " + to_string(max_log) + " \n");
						
			logfile->LogFileWriteString("Date: " + GetTime() + "\n");
			
			logfile->LogFileWriteString("DrvNum\tMaxVel\tVelHome\tAcceleration\tDeceleration\tPhaseCurrent\tAnalogOutput0\n");
			
			for (i=0; i < MAXIMUM_DRIVER; i++)
			{
				if (ParameterArrayParagorn[i].max_vel != -1 && ParameterArrayParagorn[i].vel_home != -1 && ParameterArrayParagorn[i].acceleration != -1 && ParameterArrayParagorn[i].deceleration != -1 && ParameterArrayParagorn[i].analog_output0 != -1)
					{
						logfile->LogFileWriteString(to_string(i+1) + "\t" + to_string(ParameterArrayParagorn[i].max_vel) + "\t" + to_string(ParameterArrayParagorn[i].vel_home) + "\t" + to_string(ParameterArrayParagorn[i].acceleration) + 
						"\t" + to_string(ParameterArrayParagorn[i].deceleration) + "\t" + to_string(ParameterArrayParagorn[i].phase_current) + "\t" + to_string(ParameterArrayParagorn[i].analog_output0) + "\n");
					}
			}
		}
		else
		{
			GeneralStatus.par_file_status = 0;
			output_module->Output("InternalStatusParameter: " + to_string(GeneralStatus.par_file_status)  + "\n");
			
			if (count == LIMITCHECKLOG)
				output_module->Output("Exp: check_par_assoc timeout: you waited for a time longer then 60 secs\n");			
		}						
		
	}
	
	free(buffer);
}

//This function tries to read EncoderLog.txt file, then compares the parameters
//read in that file with the parameters obtained querying the drivers.
void CheckEncodeAssoc (CommunicationObject& mioTCP, Input* mioinput, modbus_t* ctx)
{
	//This function flushes the pending datagrams to the drivers.
	modbus_flush(ctx);
	
	//This array will be used to save the user response from the stdin.
	char* buffer;
	buffer = (char*) malloc (STANDARDBUFFERLIMIT);
	bzero(buffer, STANDARDBUFFERLIMIT);

	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.
	int function_status = 0;
	
	//This object saves the user response from the TCP/IP communication.	
	TcpUser command_received_by_user;

	//This variable records the latest log record from the EncoderLog.txt file
	//Ex. The last log is the number 62 (Log 62 Date ...) max_log will be equal
	//to 62. So, using this information, the next log will be the number 63.
	int max_log = 0;
	
	//This variable will contain the encoding values of the drivers obtained by Encode procedure.
	//Ex.	Encode(ctx, j+1, tmp_encoder_struct);
	EncoderStruct tmp_encoder_struct;

	//Singleton to manage the output of the program.
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	//Singleton to record the path of the LogFiles.		
	ApplicationSetup* command_executor_application_setup;
	command_executor_application_setup = ApplicationSetup::Instance();
	
	//Singleton to write to the log files.		
	LogFile* logfile;
	logfile = LogFile::Instance();

	//If LogFile exists.
	//Attempt to access EncoderLog.txt in order to determine if it exists.
	if( access( command_executor_application_setup->application_setup_encoder_log_path, F_OK ) != -1 ) {
		
		//This string will contained the buffered output that will be sent to the server.		
		string output_tmp;
		
		//These vector will contain the encoding values read from EncoderLog.txt. 
		vector<EncoderStruct> encoder_list;
		vector<EncoderStruct>& encoder_reference = encoder_list;
		
		//Scanning EncoderLog.txt recording in encoder_reference the encoding values
		//and in max_log the number of the latest log.				
		AnalizzaFileEncoder(encoder_reference, &max_log);
		
		//Printing the encoding values read from the EncoderLog.txt file
		int i = 0;
		output_tmp += "Check Encode Assoc: EncoderLog.txt exists\n\n";
		output_tmp += "It contains these parameters:\n";
				
		for (i = 0; i < encoder_reference.size(); i++)
		{
			output_tmp += to_string (i+1) + " drv num: " + " " + to_string(encoder_reference[i].drv_num) + "\t";
			output_tmp += "Slope: " + to_string(encoder_reference[i].slope) + "\t";
			output_tmp += "Intercept: " + to_string(encoder_reference[i].intercept) + "\t";
			output_tmp += "Coefficient: " + to_string(encoder_reference[i].coefficient) + "\n\n";	
		}
		
		//Obtaining and printing the encoding values from the drivers.
		//Be careful! This implies to execute an encoding procedure for the all drivers!!!!
		output_tmp += "\nThe real situation is:\n";
		
		int j = 0;
		for (j=0; j < MAXIMUM_DRIVER; j++)
		{
			
			if (ENCODINGHOME == 1)
			{
				Homing(ctx, j+1);
			}			
			
			Encode(ctx, j+1, tmp_encoder_struct);				
			
			if (tmp_encoder_struct.slope != -1 || tmp_encoder_struct.intercept != -1 || tmp_encoder_struct.coefficient != -1)
			{
				EncoderArrayValue[j].drv_num = tmp_encoder_struct.drv_num;
				EncoderArrayValue[j].slope = tmp_encoder_struct.slope;
				EncoderArrayValue[j].intercept = tmp_encoder_struct.intercept;
				EncoderArrayValue[j].coefficient = tmp_encoder_struct.coefficient;
				output_tmp += "Driver number " + to_string(j+1) + " slope: " + to_string(EncoderArrayValue[j].slope) + "\t";
				output_tmp += "Driver number " + to_string(j+1) + " intercept: " + to_string(EncoderArrayValue[j].intercept) + "\t";
				output_tmp += "Driver number " + to_string(j+1) + " coefficient: " + to_string(EncoderArrayValue[j].coefficient) + "\n\n";
			}
			else
			{
				output_tmp += "Driver number " + to_string(j+1) + " not found\n";
				EncoderArrayValue[j].drv_num = -1;
				EncoderArrayValue[j].slope = -1;
				EncoderArrayValue[j].intercept = -1;
				EncoderArrayValue[j].coefficient = -1;
			}
			
		}
		
		//Sending the information to the user. He will decide if write the new encoding values to EncoderLog.txt or not.			
		output_tmp += "\n";
		
		output_tmp += "\nIs this situation okay (the encoding parameters of the drivers connected to the programmer will be written to the log file)?\n";
		
		output_module->Output(output_tmp);
		
		//There is a timeout: if the user does not send any response in 1 minute,
		//the server assume that the user has rejected the request.	
		int count = 0;
		do {
		sleep(SLEEPCHECK);
		count ++; //count is the number of attempts (it could be LIMITCHECKLOG on all)
		command_received_by_user = mioTCP.GetCommand ();
		if (command_executor_application_setup->input_mode != ONLYTCP)
			mioinput->GetInput (buffer);
		}
		while (buffer[0] != 'y' && buffer[0] != 'n' && command_received_by_user.command_sent_by_user[0] != 'y' && command_received_by_user.command_sent_by_user[0] != 'n' && count < LIMITCHECKLOG);
		
		//If the user has accepted the situation, the program writes the
		//actual parameters to the log file EncoderLog.txt.				
		if (buffer[0] == 'y' || command_received_by_user.command_sent_by_user[0] == 'y')
		{
			GeneralStatus.encoder_file_status = 1;
			output_module->Output("InternalStatusEncoder: " + to_string(GeneralStatus.encoder_file_status)  + "\n");
			
			int i = 0;
			max_log = max_log + 1;
			
			//file exists
			logfile->LogFileSet(command_executor_application_setup->application_setup_encoder_log_path);
			
			logfile->LogFileWriteString("\nLog " + to_string(max_log) + " \n");
			
			logfile->LogFileWriteString("Date: " + GetTime() + "\n");
			
			logfile->LogFileWriteString("DrvNum\tSlope\tIntercept\tCoefficient\n");
			
			for (i=0; i < MAXIMUM_DRIVER; i++)
			{
				if (EncoderArrayValue[i].slope != -1 || EncoderArrayValue[i].intercept != -1 || EncoderArrayValue[i].coefficient != -1)
					{
						logfile->LogFileWriteString(to_string(i+1) + "\t" + to_string(EncoderArrayValue[i].slope) + "\t" + to_string(EncoderArrayValue[i].intercept) + 
						"\t" + to_string(EncoderArrayValue[i].coefficient) + "\n");
					}							
			}					
		}
		else
		{
			GeneralStatus.encoder_file_status = 0;
			output_module->Output("InternalStatusEncoder: " + to_string(GeneralStatus.encoder_file_status) + "\n");
			
			if (count == LIMITCHECKLOG)
				output_module->Output("Exp: check_encode_assoc timeout: you waited for a time longer then 60 secs\n");
		}
		
	} else {
		
		//file doesn't exist
		
		//file EncoderLog.txt doesn't exist
		
		//If EncoderLog.txt does not exist, the program collects the encoding values from the
		//drivers actually connected to the programmer and asks the user if he approves the
		//situation.
		
		//This string will contained the buffered output that will be sent to the server.	
		
		string output_tmp;
		
		//~ output_tmp += "Check Par Assoc: Ho trovato queste informazioni: \n";
		
		//Collecting and printing the encoding values from the drivers connected to the programmer.	
		output_tmp += "Check Encode Assoc: File EncoderLog.txt does not exist:\nThese drivers are found in reality:\n";		
		int j = 0;
		for (j=0; j < MAXIMUM_DRIVER; j++)
		{	
			
			if (ENCODINGHOME == 1)
			{
				Homing(ctx, j+1);
			}
			
			Encode(ctx, j+1, tmp_encoder_struct);				
			
			if (tmp_encoder_struct.slope != -1 || tmp_encoder_struct.intercept != -1 || tmp_encoder_struct.coefficient != -1)
			{
				EncoderArrayValue[j].drv_num = tmp_encoder_struct.drv_num;
				EncoderArrayValue[j].slope = tmp_encoder_struct.slope;
				EncoderArrayValue[j].intercept = tmp_encoder_struct.intercept;
				EncoderArrayValue[j].coefficient = tmp_encoder_struct.coefficient;
				output_tmp += "Driver number " + to_string(j+1) + " slope: " + to_string(EncoderArrayValue[j].slope) + "\t";
				output_tmp += "Driver number " + to_string(j+1) + " intercept: " + to_string(EncoderArrayValue[j].intercept) + "\t";
				output_tmp += "Driver number " + to_string(j+1) + " coefficient: " + to_string(EncoderArrayValue[j].coefficient) + "\n\n";
			}
			else
			{
				output_tmp += "Driver number " + to_string(j+1) + " not found\n";
				EncoderArrayValue[j].drv_num = -1;
				EncoderArrayValue[j].slope = -1;
				EncoderArrayValue[j].intercept = -1;
				EncoderArrayValue[j].coefficient = -1;
			}
		}
		
		//Sending the information to the user. He will decide if write the new encoding values to EncoderLog.txt or not.			
		output_tmp += "\nIs this situation okay (the encoding parameters of the drivers connected to the programmer will be written to the log file)?\n";
		output_module->Output(output_tmp);
		
		//There is a timeout: if the user does not send any response in 1 minute,
		//the server assume that the user has rejected the request.			
		int count = 0;
		do {
		sleep(SLEEPCHECK);
		count ++; //count is the number of attempts (it could be LIMITCHECKLOG on all)
		command_received_by_user = mioTCP.GetCommand ();
		if (command_executor_application_setup->input_mode != ONLYTCP)
			mioinput->GetInput (buffer);
		}
		while (buffer[0] != 'y' && buffer[0] != 'n' && command_received_by_user.command_sent_by_user[0] != 'y' && command_received_by_user.command_sent_by_user[0] != 'n' && count < LIMITCHECKLOG);
		
		//If the user has accepted the situation, the program writes the
		//actual parameters to the log file EncoderLog.txt.			
		if (buffer[0] == 'y' || command_received_by_user.command_sent_by_user[0] == 'y')
		{
			logfile->LogFileSet(command_executor_application_setup->application_setup_file_par_log_path);
			
			GeneralStatus.encoder_file_status = 1;
			output_module->Output("InternalStatusEncoder: " + to_string(GeneralStatus.encoder_file_status) + "\n");
			
			int i = 0;
			max_log = max_log + 1;
			logfile->LogFileWriteString("\nLog " + to_string(max_log) + " \n");
			
			logfile->LogFileWriteString("Date: " + GetTime() + "\n");
			
			logfile->LogFileWriteString("DrvNum\tSlope\tIntercept\tCoefficient\n");
			
			for (i=0; i < MAXIMUM_DRIVER; i++)
			{
				if (EncoderArrayValue[i].slope != -1 || EncoderArrayValue[i].intercept != -1 || EncoderArrayValue[i].coefficient != -1)
					{
						logfile->LogFileWriteString(to_string(i+1) + "\t" + to_string(EncoderArrayValue[i].slope) + "\t" + to_string(EncoderArrayValue[i].intercept) + 
						"\t" + to_string(EncoderArrayValue[i].coefficient) + "\n");
					}							
			}
		}
		else
		{
			GeneralStatus.encoder_file_status = 0;
			output_module->Output("InternalStatusEncoder: " + to_string(GeneralStatus.encoder_file_status) + "\n");
			
			if (count == LIMITCHECKLOG)
				output_module->Output("Exp: check_encode_assoc timeout: you waited for a time longer then 60 secs\n");			
		}						
		
	}
	
	free(buffer);
}

//This function collects the parameters from the driver indicated by get_par_value.
//The parameters collected are: max_vel, vel_home, acceleration, deceleration, phase_current,
//AnalogOutput0.
void GetPar (modbus_t* ctx, int get_par_value)
{
	//This function flushes the pending datagrams to the drivers.
	modbus_flush(ctx);

	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.		
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.	
	int error = 0;
	
	//Singleton to manage the output of the program.
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	//This struct collects the parameters obtained from the driver.
	ParameterStruct tmp_parameter_struct;
	
	//Try to set the get_par_value as the active driver.
	function_status = modbus_set_slave(ctx, get_par_value);
	if (function_status == -1) 
		error = -1;	
	//If success	
	else
	{
		//Try to collect the parameters. If at least one communication is not done,
		//no valid output is sent to the client.
		tmp_parameter_struct.max_vel = ReadMaxVel(ctx, &function_status, "Exp: ");
		if (function_status == -1) 
			error = -1;
			
		tmp_parameter_struct.vel_home = ReadVelHome(ctx, &function_status, "Exp: ");
		if (function_status == -1) 
			error = -1;			
			
		tmp_parameter_struct.acceleration = ReadAcceleration(ctx, &function_status, "Exp: ");
		if (function_status == -1) 
			error = -1;
			
		tmp_parameter_struct.deceleration = ReadDeceleration(ctx, &function_status, "Exp: ");
		if (function_status == -1) 
			error = -1;
			
		tmp_parameter_struct.phase_current = ReadPhaseCurrent(ctx, &function_status, "Exp: ");
		if (function_status == -1) 
			error = -1;
			
		tmp_parameter_struct.analog_output0 = ReadAnalogOutput0(ctx, &function_status, "Exp: ");
		if (function_status == -1) 
			error = -1;				
	}
	
	//If all communications are okay.
	if (error != -1)
	{
		output_module->Output("getpar " + to_string(get_par_value) + " MaxVel " + to_string(tmp_parameter_struct.max_vel) + "\n");
		//sleep(1);
		output_module->Output("getpar " + to_string(get_par_value) + " VelHome " + to_string(tmp_parameter_struct.vel_home) + "\n");
		//sleep(1);		
		output_module->Output("getpar " + to_string(get_par_value) + " Acceleration " + to_string(tmp_parameter_struct.acceleration) + "\n");
		//sleep(1);
		output_module->Output("getpar " + to_string(get_par_value) + " Deceleration " + to_string(tmp_parameter_struct.deceleration) + "\n");
		//sleep(1);
		output_module->Output("getpar " + to_string(get_par_value) + " PhaseCurrent " + to_string(tmp_parameter_struct.phase_current) + "\n");
		//sleep(1);
		output_module->Output("getpar " + to_string(get_par_value) + " AnalogOutput0 " + to_string(tmp_parameter_struct.analog_output0) + "\n");		
	}
	//If at least one communication is failed.
	else
	{
		SendFailedGetPar(get_par_value);
	}
	
}

//This function sets the parameters of driver indicated by set_par_value
//to the values contained in buffer. The values are: max_vel, vel_home,
//acceleration, deceleration, phase_current, AnalogOutput0.
//N.B. The precondition to use this function is that in buffer is
//stored a consistent set_par command. This is guaranteed by the check
//of the command in Main.c . The correct syntax of the command is:
//set_par drvnum max_vel acceleration deceleration PhaseCurrent AnalogOutput0.
void SetPar (modbus_t* ctx, int set_par_value, char* buffer)
{

	//This function flushes the pending datagrams to the drivers.
	modbus_flush(ctx);
		
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.		
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.		
	int error_status = 0;
	
	//Singleton to manage the output of the program.	
	OutputModule* output_module;
	output_module = OutputModule::Instance();	
	
	//Variables used to stored the values contained in buffer.
	int max_vel = 0;
	
	int16_t vel_home = 0;	

	int acceleration = 0;

	int deceleration = 0;
	
	int phase_current = 0;
	
	int analog_output0 = 0;
	
	//Skip the first word of the command.
	char* set_par_punt = FindPointer(buffer);
	
	output_module->Output("###############\n");
	output_module->Output("Executing SetPar function\n");
	
	//Storing max_vel values
	max_vel = (uint16_t)FindIntegerValue(set_par_punt);
	
	//This condition is dued to the limitation of the driver DS44.
	if (max_vel > MAXVEL)
	{	
		output_module->Output("Warning, you can't set max_vel > 12000. It will be set to 12000.\n");
		max_vel = MAXVEL;
	}
	
	if (max_vel == MINVEL) 
	{
		max_vel = MINVEL + 1;
		output_module->Output("Warning, you can't set to 0 max_vel. It will be set to 1. \n");
	}
	
	//Skipping one word
	set_par_punt = FindPointer(set_par_punt);

	output_module->Output("Max_vel = " + to_string(max_vel) + "\n");

	//Storing vel_home values.
	vel_home = (int16_t)FindIntegerValue(set_par_punt);
	
	//This condition is important to avoid excessive speed to return in homing position.
	if (vel_home > MAXVELHOME)
	{	
		output_module->Output("Warning, you can't set vel_home > 1000. It will be set to 1000.\n");
		vel_home = MAXVELHOME;
	}
	
	if (vel_home < MINVELHOME) 
	{
		vel_home = MINVELHOME;
		output_module->Output("Warning, you can't set vel_home < -1000. It will be set to -1000.\n");
	}
	
	//Skipping one word.
	set_par_punt = FindPointer(set_par_punt);

	output_module->Output("Vel_home = " + to_string(vel_home) + "\n");

	//Storing acceleration value.
	acceleration = (uint16_t)FindIntegerValue(set_par_punt);
	
	if (acceleration > MAXACCELERATION)
	{	
		output_module->Output("Warning, you can't set acceleration > 30000. It will be set to 30000.\n");
		acceleration = MAXACCELERATION;
	}
	
	if (acceleration == MINACCELERATION) 
	{
		acceleration = MINACCELERATION + 1;
		output_module->Output("Warning, you can't set to 0 acceleration. It will be set to 1. \n");						
	}
	
	set_par_punt = FindPointer(set_par_punt);

	output_module->Output("Acceleration = " + to_string(acceleration) + "\n");

	//Storing deceleration value.
	deceleration = (uint16_t)FindIntegerValue(set_par_punt);
	
	if (deceleration > MAXDECELERATION)
	{	
		output_module->Output("Warning, you can't set deceleration > 30000. It will be set to 30000.\n");
		deceleration = MAXDECELERATION;
	}
	
	if (deceleration == MINDECELERATION) 
	{
		deceleration = MINDECELERATION + 1;
		output_module->Output("Warning, you can't set to 0 deceleration. It will be set to 1. \n");
	}
	
	set_par_punt = FindPointer(set_par_punt);
	
	output_module->Output("deceleration = " + to_string(deceleration) + "\n");

	//Storing phase_current value.
	phase_current = (uint16_t)FindIntegerValue(set_par_punt);
	
	if (phase_current > MAXPHASECURRENT)
	{	
		output_module->Output("Warning, you can't set phase_current > 17. It will be set to 17.\n");
		phase_current = MAXPHASECURRENT;
	}
	
	if (phase_current < MINPHASECURRENT) 
	{
		phase_current = MINPHASECURRENT;
		output_module->Output("Warning, you can't set phase_current < 10. It will be set to 10. \n");
	}
	
	set_par_punt = FindPointer(set_par_punt);
	
	output_module->Output("phase_current = " + to_string(phase_current) + "\n");

	//Storing analog_output0 value.
	analog_output0 = (uint16_t)FindIntegerValue(set_par_punt);
	
	if (analog_output0 > MAXANALOGOUTPUT0)
	{	
		output_module->Output("Warning, you can't set analog_output0 > 1023. It will be set to 1023.\n");
		analog_output0 = MAXANALOGOUTPUT0;
	}
	
	if (analog_output0 < MINANALOGOUTPUT0) 
	{
		analog_output0 = MINANALOGOUTPUT0;
		output_module->Output("Warning, you can't set analog_output0 < 0. It will be set to 0. \n");
	}
	
	output_module->Output("analog_output0 = " + to_string(analog_output0) + "\n");

	//At this point, the program has collected the setting parameters and is ready to set it.
	error_status = 0;

	//Try to set the set_par_value as the active driver.
	function_status = modbus_set_slave(ctx, set_par_value);
	if (function_status == -1) 
		error_status = -1;	
	//If success
	else
	{
		//Try to set all the parameters. If one setting failed,
		//the program sends to the client failed parameters.
		function_status = SetMaxVel(ctx, max_vel, "Exp: ");
		if (function_status == -1) error_status = -1;
		
		function_status = SetVelHome(ctx, vel_home, "Exp: ");
		if (function_status == -1) error_status = -1;		

		function_status = SetAcceleration(ctx, acceleration, "Exp: ");
		if (function_status == -1) error_status = -1;

		function_status = SetDeceleration(ctx, deceleration, "Exp: ");
		if (function_status == -1) error_status = -1;
		
		function_status = SetPhaseCurrent(ctx, phase_current, "Exp: ");
		if (function_status == -1) error_status = -1;
		
		function_status = SetAnalogOutput0(ctx, analog_output0, "Exp: ");
		if (function_status == -1) error_status = -1;			
	}
	
	//If no error occurred.
	if (error_status != -1)
	{
		GetPar(ctx, set_par_value);
	}
	//If at least one error occurred.
	else
	{
		SendFailedGetPar(set_par_value);
	}
	
}

//N.B. This function is the same of the SetPar one in except of the syntax of the 
//command stored in buffer.
//This function sets the parameters of driver indicated by set_par_value
//to the values contained in buffer. The values are: max_vel, vel_home,
//acceleration, deceleration, phase_current, AnalogOutput0.
//N.B. The precondition to use this function is that in buffer is
//stored a consistent set_par command. This is guaranteed by the check
//of the command in Main.c . The correct syntax of the command is:
//set_par drvnum max_vel acceleration deceleration PhaseCurrent AnalogOutput0.
void SetParMult (modbus_t* ctx, int set_par_value, char* buffer)
{

	//This function flushes the pending datagrams to the drivers.
	modbus_flush(ctx);
		
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.		
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.		
	int error_status = 0;
	
	//Singleton to manage the output of the program.	
	OutputModule* output_module;
	output_module = OutputModule::Instance();	
	
	//Variables used to stored the values contained in buffer.
	int max_vel = 0;

	int16_t vel_home = 0;

	int acceleration = 0;

	int deceleration = 0;
	
	int phase_current = 0;
	
	int analog_output0 = 0;
	
	char* set_par_punt = buffer;
	
	output_module->Output("###############\n");
	output_module->Output("Executing the function SetParMult\n");
	
	//Storing max_vel values	
	max_vel = (uint16_t)FindIntegerValue(set_par_punt);
	
	//This condition is dued to the limitation of the driver DS44.
	if (max_vel > MAXVEL)
	{	
		output_module->Output("Warning, you can't set max_vel > 12000. It will be set to 12000.\n");
		max_vel = MAXVEL;
	}
	
	if (max_vel == MINVEL) 
	{
		max_vel = MINVEL + 1;
		output_module->Output("Warning, you can't set to 0 max_vel. It will be set to 1. \n");
	}

	//Skipping one word	
	set_par_punt = FindPointer(set_par_punt);

	output_module->Output("Max_vel = " + to_string(max_vel) + "\n");

	//Storing vel_home values.
	vel_home = (int16_t)FindIntegerValue(set_par_punt);

	//This condition is important to avoid excessive speed to return in homing position.
	if (vel_home > MAXVELHOME)
	{	
		//fprintf(stderr, "Warning, you can't set max_vel > 3000 rpm. It will be set to 3000 rpm.\n");
		output_module->Output("Warning, you can't set vel_home > 1000. It will be set to 1000.\n");
		vel_home = MAXVELHOME;
	}
	
	if (vel_home < MINVELHOME) 
	{
		vel_home = MINVELHOME;
		output_module->Output("Warning, you can't set vel_home < -1000. It will be set to -1000.\n");
	}
	
	//Skipping one word.	
	set_par_punt = FindPointer(set_par_punt);

	output_module->Output("Vel_home = " + to_string(vel_home) + "\n");

	//Storing acceleration value.
	acceleration = (uint16_t)FindIntegerValue(set_par_punt);
	
	if (acceleration > MAXACCELERATION)
	{	
		output_module->Output("Warning, you can't set acceleration > 30000. It will be set to 30000.\n");
		acceleration = MAXACCELERATION;
	}
	
	if (acceleration == MINACCELERATION) 
	{
		acceleration = MINACCELERATION + 1;
		output_module->Output("Warning, you can't set to 0 acceleration. It will be set to 1. \n");						
	}
	
	set_par_punt = FindPointer(set_par_punt);

	output_module->Output("Acceleration = " + to_string(acceleration) + "\n");

	//storing deceleration value.
	deceleration = (uint16_t)FindIntegerValue(set_par_punt);
	
	if (deceleration > MAXDECELERATION)
	{	
		output_module->Output("Warning, you can't set deceleration > 30000. It will be set to 30000.\n");
		deceleration = MAXDECELERATION;
	}
	
	if (deceleration == MINDECELERATION) 
	{
		deceleration = MINDECELERATION + 1;
		output_module->Output("Warning, you can't set to 0 deceleration. It will be set to 1. \n");
	}
	
	set_par_punt = FindPointer(set_par_punt);
	
	output_module->Output("deceleration = " + to_string(deceleration) + "\n");

	//storing phase_current value.
	phase_current = (uint16_t)FindIntegerValue(set_par_punt);
	
	if (phase_current > MAXPHASECURRENT)
	{	
		output_module->Output("Warning, you can't set phase_current > 17. It will be set to 17.\n");
		phase_current = MAXPHASECURRENT;
	}
	
	if (phase_current < MINPHASECURRENT) 
	{
		phase_current = MINPHASECURRENT;
		output_module->Output("Warning, you can't set phase_current < 10. It will be set to 10. \n");
	}
	
	set_par_punt = FindPointer(set_par_punt);
	
	output_module->Output("phase_current = " + to_string(phase_current) + "\n");

	//storing analog_output0 value.
	analog_output0 = (uint16_t)FindIntegerValue(set_par_punt);
	
	if (analog_output0 > MAXANALOGOUTPUT0)
	{	
		output_module->Output("Warning, you can't set analog_output0 > 1023. It will be set to 1023.\n");
		analog_output0 = MAXANALOGOUTPUT0;
	}
	
	if (analog_output0 < MINANALOGOUTPUT0) 
	{
		analog_output0 = MINANALOGOUTPUT0;
		output_module->Output("Warning, you can't set analog_output0 < 0. It will be set to 0. \n");
	}
	
	output_module->Output("analog_output0 = " + to_string(analog_output0) + "\n");

	//At this point, the program has collected the setting parameters and is ready to set it.
	error_status = 0;

	//Try to set the set_par_value as the active driver.
	function_status = modbus_set_slave(ctx, set_par_value);
	if (function_status == -1) 
		error_status = -1;
	//If success	
	else
	{
		//Try to set all the parameters. If one setting failed,
		//the program sends to the client failed parameters.		
		function_status = SetMaxVel(ctx, max_vel, "Exp: ");
		if (function_status == -1) error_status = -1;
		
		function_status = SetVelHome(ctx, vel_home, "Exp: ");
		if (function_status == -1) error_status = -1;		

		function_status = SetAcceleration(ctx, acceleration, "Exp: ");
		if (function_status == -1) error_status = -1;

		function_status = SetDeceleration(ctx, deceleration, "Exp: ");
		if (function_status == -1) error_status = -1;
		
		function_status = SetPhaseCurrent(ctx, phase_current, "Exp: ");
		if (function_status == -1) error_status = -1;
		
		function_status = SetAnalogOutput0(ctx, analog_output0, "Exp: ");
		if (function_status == -1) error_status = -1;			
	}
	
	//If no error occurred.	
	if (error_status != -1)
	{
		GetPar(ctx, set_par_value);
	}
	//If at least one error occurred.	
	else
	{
		SendFailedGetPar(set_par_value);
	}
	
}

//This function orders the driver indicated by homing_value to execute
//the homing procedure.
//See firmware documentation for more information about the procedure.  
void Homing(modbus_t* ctx, int homing_value)
{
	
	//This function flushes the pending datagrams to the drivers.	
	modbus_flush(ctx);
	
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.		
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.	
	int error_status = 0;
		
	//Singleton to manage the output of the program.		
	OutputModule* output_module;
	output_module = OutputModule::Instance();	
	
	//Try to set the set_par_value as the active driver.	
	function_status = modbus_set_slave(ctx, homing_value);
	
	//If success
	if (function_status == -1) 
	{	
		error_status = -1;	
		output_module->Output("Exp: error, homing not done: set slave failed\n");
		return;
	}
	
	//status_state == 3 is not a state contemplated by the firmware, so it is
	//a neutral value to initialized the variable.
	uint16_t status_state = 3;
	
	//This variable records the success following functions.
	int rc;
	
	//Try to read the actual state.
	status_state = ReadStatusState(ctx, &rc, "Exp: "); //Questo rc non va messo qui!!!!

	//status_state = 4 or status_state = 5 means that the previous operation is terminated.
	//Obviously these information are hard coded!	
	//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
	//the homing function is aborted.
	int count = 0;
	while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
	{
		usleep(SLEEPSTATUS_STATE);
		count ++;
		status_state = ReadStatusState(ctx, &rc, "Exp: ");
	}
	
	if (count == LIMITSTATUS_STATE)
	{
		output_module->Output("Exp: error, homing not done because status state is blocked to an invalid state\n");
		return;
	}
	
	//The firmware requires the software to set to 0 the status state before executing other operations.
	function_status = SetStatusState(ctx, 0, "Exp: ");
	if (function_status == -1) error_status = -1;
	
	//If no error occurred
	if (error_status != -1)
	{
		//Starting the homing procedure.
		function_status = SetRequestState(ctx, STATEHOMING, "Exp :");
		if (function_status != -1)
		{
			output_module->Output("Exp: Homing done\n");
		}
		else
		{	
			error_status = -1;
			output_module->Output("Exp: error, homing not done: set request state failed\n");
			return;	
		}
		
	}
	else
	{
		output_module->Output("Exp: error, homing not done because SetStatusState function filed\n");
		return;		
	}
}

//This function collects the movimentation parameters from the driver
//indicated by mov_par_value.
//The movimentation parameters are CurrentPosition and AnalogInput0.
//CorrentPosition is the actual position of the driver, AnalogInput0
//is the values retrieved by encoder to the driver and it can be used
//for checking the position of the engine mastered by the driver.
//Since the operation requires the driver to have already accomplished the
//previous operation, a check to the status of the driver is performed.
void GetMovePar(modbus_t* ctx, int mov_par_value)
{

	//This function flushes the pending datagrams to the drivers.	
	modbus_flush(ctx);
		
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.			
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.	
	int error = 0;
	
	//Singleton to manage the output of the program.	
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	//Initializing the variables that are storing the positions values.
	int current_position = -1;
	
	int analog_input0 = -1;
	
	//Try to set the driver indicated by the mov_par_value as the active one.
	function_status = modbus_set_slave(ctx, mov_par_value);
	
	//status_state == 3 is not a state contemplated by the firmware, so it is
	//a neutral value to initialized the variable.
	uint16_t status_state = 3;
	
	//This variable records the success following functions.	
	int rc;
	
	//Try to read the actual state.
	status_state = ReadStatusState(ctx, &rc, "Exp: ");

	//status_state = 4 or status_state = 5 means that the previous operation is terminated.
	//Obviously these information are hard coded!	
	//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
	//the homing function is aborted.
	int count = 0;
	while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
	{
		usleep(SLEEPSTATUS_STATE);
		count ++;
		status_state = ReadStatusState(ctx, &rc, "Exp: ");
	}	
	
	
	if (count == LIMITSTATUS_STATE)
	{
		SendFailedGetMovPar(mov_par_value);
		return;
	}
	
	//If no error occurred
	if (function_status == -1) 
		error = -1;	
	else
	{
		//Reading current_position and analog_input0
		//If an error occurred, a negative response is sent to the client.
		//
		current_position = ReadCurrentPosition(ctx, &function_status, "Exp: ");
		if (function_status == -1) 
			error = -1;
			
		analog_input0 = ReadAnalogInput0(ctx, &function_status, "Exp: ");
		if (function_status == -1) 
			error = -1;
					
	}
	
	//If success.
	if (error != -1)
	{   
		output_module->Output("get_mov_par " + to_string(mov_par_value) + " CurrentPosition " + to_string(current_position) + "\n");
		//sleep(1);
		output_module->Output("get_mov_par " + to_string(mov_par_value) + " AnalogInput0 " + to_string(analog_input0) + "\n");
		//sleep(1);		
	}
	//If an error occurred, a negative response is sent to the client.
	else
	{
		SendFailedGetMovPar(mov_par_value);
	}

}

//This function set the CountTargetPosition of the driver indicated by moveto_drv_num
//to the values found in buffer.
//N.B. The precondition to execute the function is that in buffer is stored a valid
//move_to command. This is guaranteed by the check performed in Main.c.
//The correct syntax of the command is: move_to drvnum val .
//In order to accomplished the movimentation is performed a check to the status of the
//driver: it has to have already terminated the previous operation.
void MoveTo(modbus_t* ctx, int moveto_drv_num, char* buffer)
{
	
	//This function flushes the pending datagrams to the drivers.	
	modbus_flush(ctx);
	
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.				
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.	
	int error_status = 0;
	
	//This variable is used to stored the TargetPosition obtained by buffer.	
	int moveto_value = 0;
	
	//This variable is useful to browse the buffer in order to find the TargetPosition val.
	char* mypunt;
	
	//Skipping "move_to" and "drvnum".
	mypunt = FindPointer(buffer);
	
	//Retrieving "val" that is the CountTargetPosition and storing it in moveto_value.
	moveto_value = FindIntegerValue(mypunt);	
	
	//Singleton to manage the output of the program.	
	OutputModule* output_module;
	output_module = OutputModule::Instance();	
	
	//Try to set the driver indicated by the moveto_drv_num as the active one.
	function_status = modbus_set_slave(ctx, moveto_drv_num);
	if (function_status == -1) 
	{	
		error_status = -1;	
		output_module->Output("Exp: error, movimentation not done: set slave failed\n");
		return;
	}
	
	//status_state == 3 is not a state contemplated by the firmware, so it is
	//a neutral value to initialized the variable.	
	uint16_t status_state = 3;
	
	//This variable records the success following functions.		
	int rc;
	
	//Try to read the actual state.	
	status_state = ReadStatusState(ctx, &rc, "Exp: "); //Questo rc non va messo qui!!!!

	//status_state = 4 or status_state = 5 means that the previous operation is terminated.
	//Obviously these information are hard coded!	
	//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
	//the homing function is aborted.
	int count = 0;
	while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
	{
		usleep(SLEEPSTATUS_STATE);
		count ++;
		status_state = ReadStatusState(ctx, &rc, "Exp: ");
	}
	
	if (count == LIMITSTATUS_STATE)
	{
		output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
		return;
	}
	
	//The firmware requires the software to set to 0 the status state before executing other operations.	
	function_status = SetStatusState(ctx, 0, "Exp: ");
	if (function_status == -1) error_status = -1;
	
	//If no error occurred.
	if (error_status != -1)
	{
		//Setting CountTargetPosition.
		function_status = SetCountTargetPosition(ctx, moveto_value, "Exp :");
		if (function_status != -1)
		{
			//Requesting a relative movimentation.
			function_status = SetRequestState(ctx, STATEMOVEREL, "Exp :");
			if (function_status != -1)
			{
				output_module->Output("Exp: Movimentation done\n");
			}
			else
			{	
				error_status = -1;
				output_module->Output("Exp: error, movimentation not done: set request state failed\n");
				return;	
			}
		}
		else
		{
			output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
			return;	
		}
	}
	else
	{
		output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
		return;		
	}
}


//N.B. This function is the of the MoveTo one in except of the syntax of the command stored
//in buffer.

//This function set the CountTargetPosition of the driver indicated by moveto_drv_num
//to the values found in buffer.
//N.B. The precondition to execute the function is that in buffer is stored a valid
//move_to command. This is guaranteed by the check performed in Main.c.
//The correct syntax of the command is: move_to drvnum val .
//In order to accomplished the movimentation is performed a check to the status of the
//driver: it has to have already terminated the previous operation.
void MoveToMult(modbus_t* ctx, int moveto_drv_num, char* buffer)
{

	//This function flushes the pending datagrams to the drivers.		
	modbus_flush(ctx);
	
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.	
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.	
	int error_status = 0;
		
	//Retrieving "val" that is the CountTargetPosition and storing it in moveto_value.		
	int moveto_value = 0;
	
	//This variable is useful to browse the buffer in order to find the TargetPosition val.
	char* mypunt;
	
	//Retrieving "val" that is the CountTargetPosition and storing it in moveto_value.	
	moveto_value = FindIntegerValue(buffer);
	
	//Singleton to manage the output of the program.	
	OutputModule* output_module;
	output_module = OutputModule::Instance();	
	
	//Try to set the driver indicated by the moveto_drv_num as the active one.	
	function_status = modbus_set_slave(ctx, moveto_drv_num);
	if (function_status == -1) 
	{	
		error_status = -1;	
		output_module->Output("Exp: error, movimentation not done: set slave failed\n");
		return;
	}
	
	//status_state == 3 is not a state contemplated by the firmware, so it is
	//a neutral value to initialized the variable.		
	uint16_t status_state = 3;
	
	//This variable records the success following functions.	
	int rc;
	
	//Try to read the actual state.		
	status_state = ReadStatusState(ctx, &rc, "Exp: "); //Questo rc non va messo qui!!!!

	//status_state = 4 or status_state = 5 means that the previous operation is terminated.
	//Obviously these information are hard coded!	
	//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
	//the homing function is aborted.
	int count = 0;
	while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
	{
		usleep(SLEEPSTATUS_STATE);
		count ++;
		status_state = ReadStatusState(ctx, &rc, "Exp: ");
	}
	
	if (count == LIMITSTATUS_STATE)
	{
		output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
		return;
	}
	
	//The firmware requires the software to set to 0 the status state before executing other operations.	
	function_status = SetStatusState(ctx, 0, "Exp: ");
	if (function_status == -1) error_status = -1;
	
	//If no error occurred.	
	if (error_status != -1)
	{
		//Setting CountTargetPosition.		
		function_status = SetCountTargetPosition(ctx, moveto_value, "Exp :");
		if (function_status != -1)
		{
			//Requesting a relative movimentation.		
			function_status = SetRequestState(ctx, STATEMOVEREL, "Exp :");
			if (function_status != -1)
			{
				output_module->Output("Exp: Movimentation done\n");
			}
			else
			{	
				error_status = -1;
				output_module->Output("Exp: error, movimentation not done: set request state failed\n");
				return;	
			}
		}
		else
		{
			output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
			return;	
		}
	}
	else
	{
		output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
		return;		
	}
}

//This function executes an encoding procedure for the driver indicated in encode_drv_num 
//and save in drv_parameters struct the values obtained from the linear regression.
//Warning: in order to execute this function the user must have already done the homing procedure.
//If this has not happened, this function returns inconsistent values. In DefineGeneral.h there is
//a definition called ENCODINGHOME: if ENCODINGHOME == 1, Main.c execute an homing procedure before
//calling the encoding procedure.
//The encoding procedure consists to reach the final position indicated by MAXEXTENSION
//by steps indicated by ENCODINGSTEP.
//Be careful: it is assumed that the path begins from 0 and ends to a negative value. You have
//to check the driver polarity before calling this procedure.
void Encode(modbus_t* ctx, int encode_drv_num, EncoderStruct& drv_parameters)
{
	//This function flushes the pending datagrams to the drivers.	
	modbus_flush(ctx);
	
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.		
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.		
	int error_status = 0;
	
	//Warning! Be careful to the return values of drv_parameters
	drv_parameters.drv_num = encode_drv_num;
	drv_parameters.slope = -1;
	drv_parameters.intercept = -1;
	drv_parameters.coefficient = -1;
	
	//vector to store the CurrentPosition
	vector<double> enc_pos_vector_x;
	
	//vector to store the AnalogInput0
	vector<double> enc_pos_vector_y;
	
	//Variable used to store a CurrentPosition value.
	double*	enc_pos_array_x; 
	
	//Variable user to store an AnalogInput0 value.
	double*	enc_pos_array_y;
	
	//Singleton to manage the output of the program.	
	OutputModule* output_module;
	output_module = OutputModule::Instance();	
	
	//Try to set the driver indicated by the moveto_drv_num as the active one.		
	function_status = modbus_set_slave(ctx, encode_drv_num);
	if (function_status == -1) 
	{	
		error_status = -1;	
		output_module->Output("Exp: error, homing not done: set slave failed\n");
		return;
	}
	
	//status_state == 3 is not a state contemplated by the firmware, so it is
	//a neutral value to initialized the variable.	
	uint16_t status_state = 3; //Warning!
	
	//This variable records the success following functions.		
	int rc;
	
	//Try to read the actual state.		
	status_state = ReadStatusState(ctx, &rc, "Exp: ");

	//status_state = 4 or status_state = 5 means that the previous operation is terminated.
	//Obviously these information are hard coded!	
	//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
	//the homing function is aborted.
	int count = 0;
	while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
	{
		usleep(SLEEPSTATUS_STATE);
		count ++;
		status_state = ReadStatusState(ctx, &rc, "Exp: ");
	}
	
	if (count == LIMITSTATUS_STATE)
	{
		output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
		return;
	}
	
	//The encoding procedure consists to reach the final position indicated by MAXEXTENSION
	//by steps indicated by ENCODINGSTEP.
	//Be careful: it is assumed that the path begins from 0 and ends to a negative value. You have
	//to check the driver polarity before calling this procedure.
	int encode_position = 0;
	
	//String to compose a move_to command in order to use the MoveTo function.
	string tmp_buffer;
	
	//Starting the encoding procedure.
	for (encode_position = 0; encode_position >= MAXEXTENSION; encode_position -= ENCODINGSTEP )	
	{
		//Composing a valid move_to command to use MoveTo function.
		tmp_buffer = "move_to " + to_string(encode_drv_num) + " " + to_string(encode_position);
		
		//Moving the engine.
		MoveTo(ctx, encode_drv_num, (char*) tmp_buffer.c_str());

		//status_state = 4 or status_state = 5 means that the previous operation is terminated.
		//Obviously these information are hard coded!	
		//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
		//the homing function is aborted.		
		int count = 0;

		status_state = ReadStatusState(ctx, &rc, "Exp: ");

		while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
		{
			usleep(SLEEPSTATUS_STATE);
			count ++;
			status_state = ReadStatusState(ctx, &rc, "Exp: ");
		}
		
		if (count == LIMITSTATUS_STATE)
		{
			output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
			return;
		}
		
		//Storing the current position to enc_pos_vector_x array.
		enc_pos_vector_x.push_back((double)encode_position);
		
		//Storing AnalogInput0 to enc_pos_vector_y array.
		enc_pos_vector_y.push_back((double)ReadAnalogInput0(ctx, &function_status, "Exp: "));
		
		if (function_status == -1)
		{
			output_module->Output("Exp: error, encoding not done in consequence of an error reading AnalogInput0\n");
			return;			
		}
	}
	
	//Printing the values obtained by the previous procedure.
	int k = 0;
	
	enc_pos_array_x = (double*)malloc(sizeof(double)*enc_pos_vector_x.size());
	enc_pos_array_y = (double*)malloc(sizeof(double)*enc_pos_vector_y.size());	
	
	for (k = 0; k < enc_pos_vector_x.size(); k++)
	{
		enc_pos_array_x[k] = enc_pos_vector_x[k];
		enc_pos_array_y[k] = enc_pos_vector_y[k];
		
		output_module->Output( to_string(enc_pos_array_x[k]) + "\n");
		
	}
	
	output_module->Output("\n");
	
	for (k = 0; k < enc_pos_vector_x.size(); k++)
	{
		output_module->Output( to_string(enc_pos_array_y[k]) + "\n");
	}
	
	//Starting the linear regression.
	Maths::Regression::Linear A(enc_pos_vector_x.size(), enc_pos_array_x, enc_pos_array_y);
 
	//Print the linear regression values
    output_module->Output("    Slope = " + to_string(A.getSlope()) + "\n");
    output_module->Output("Intercept = " + to_string(A.getIntercept()) + "\n");
 
    output_module->Output("Regression coefficient = " + to_string(A.getCoefficient()) + "\n");
	
	//Saving the values obtained by linear regression. 
	drv_parameters.slope = A.getSlope();
	drv_parameters.intercept = A.getIntercept();
	drv_parameters.coefficient = A.getCoefficient();	
	
	//Clearing memory.
	free(enc_pos_array_x);
	free(enc_pos_array_y);
	
}

//This function performs the comparison between the position declared by 
//the driver (the value of the register Position) and the position 
//retrieved by the encoder (the value of the register AnalogInput0).
//The driver is indicated by position_encoder_drv_num.
//
//Return values:
//0 all okay
//-1 real position mismatch with estimated position
//-2 problem communicating with the driver
//everything > 0 the driver is blocked in an invalid state
int CheckPositionEncoderSingle (modbus_t* ctx, int position_encoder_drv_num)
{
	//Singleton to manage the output of the program.
	OutputModule* output_module;
	output_module = OutputModule::Instance();
		
	//This function flushes the pending datagrams to the drivers.		
	modbus_flush(ctx);
	
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.		
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.			
	int error_status = 0;
	
	//Variables used to stored the current position and the AnalogInput0.
	int current_position = 0;
	int analog_input0 = 0;	
	
	//Try to set the driver indicated by the moveto_drv_num as the active one.		
	function_status = modbus_set_slave(ctx, position_encoder_drv_num);
	if (function_status == -1) 
	{	
		error_status = -1;	
		//output_module->Output("Exp: error, movimentation not done: set slave failed\n");
		output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-2)  + "\n");
		return -2;
	}
	
	//status_state == 3 is not a state contemplated by the firmware, so it is
	//a neutral value to initialized the variable.		
	uint16_t status_state = 3;
	
	//This variable records the success following functions.			
	int rc;
	
	//Try to read the actual state.		
	status_state = ReadStatusState(ctx, &rc, "Exp: "); //Questo rc non va messo qui!!!!

	//status_state = 4 or status_state = 5 means that the previous operation is terminated.
	//Obviously these information are hard coded!	
	//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
	//the homing function is aborted.
	int count = 0;
	while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
	{
		usleep(SLEEPSTATUS_STATE);
		count ++;
		status_state = ReadStatusState(ctx, &rc, "Exp: ");
	}
	
	if (count == LIMITSTATUS_STATE)
	{
		//output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
		output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(status_state)  + "\n");
		return status_state;
	}
	
	//Retrieving current position
	current_position = ReadCurrentPosition(ctx, &function_status, "Exp: ");
	if (function_status == -1) 
		error_status = -1;
		
	//Retrieving AnalogInput0	
	analog_input0 = ReadAnalogInput0(ctx, &function_status, "Exp: ");
	if (function_status == -1) 
		error_status = -1;
	
	//If no error occurred, try to estimate the value of analog_input0
	//starting from the current position of the driver
	if (error_status != -1)
	{
		double tmp_estimated_value = 0;
		double tmp_current_position_value = (double) current_position;
		int paragon = 0;
		tmp_estimated_value = EncoderArrayValue[position_encoder_drv_num - 1].slope*tmp_current_position_value + 
		EncoderArrayValue[position_encoder_drv_num - 1].intercept;
		paragon = (int)round(tmp_estimated_value);
		
		//output_module->Output("Il valore di AnalogInput(0) e': " + to_string(analog_input0) + "\n");
		//output_module->Output("Quello stimato e': " + to_string(paragon) + "\n");
		
		//Compare the value of analog_input0 retrieved from the driver
		//and the estimated one.
		
		//Position mismath
		if ( abs(paragon - analog_input0) > POSITION_TOLERATED_ERROR )
		{
			//output_module->Output("I due valori non coincidono\n");
			output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-1)  + "\n");
			
			//Position mismath
			return -1;
		}
		else
		{	
			//Position match
			//output_module->Output("I due valori coincidono!!!!\n");
			output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(0)  + "\n");
			
			//Position match
			return 0;
		}

	}
	else
	{
		output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-2)  + "\n");
		
		//Unable to accomplished the operation due to a communication error.
		return -2; 
	}

}

//This function is the same of CheckPositionEncoderSingle but CheckPositionEncoderSingleWarning
//sends a warning message to the client when loading_encoder_from_file_okay is equal to 0 
//(it means that the user has not already loaded the encoding values in EncoderArrayValue 
//using the command load_encoder_from_file).
//This function performs the comparison between the position declared by 
//the driver (the value of the register Position) and the position 
//retrieved by the encoder (the value of the register AnalogInput0).
//The driver is indicated by position_encoder_drv_num.
//
//Return values:
//0 all okay
//-1 real position mismatch with estimated position
//-2 problem communicating with the driver
//everything > 0 the driver is blocked in an invalid state
int CheckPositionEncoderSingleWarning (modbus_t* ctx, int position_encoder_drv_num)
{

	//Singleton to manage the output of the program.	
	OutputModule* output_module;
	output_module = OutputModule::Instance();

	//If the user has not loaded the encoding values from EncoderLog.txt.
	if (loading_encoder_from_file_okay == 0)
		output_module->Output("Check position warning! You have to press the button Load Encoder From File in General tab or you have to digit load_encoder_from_file command in order to accomplished the check position procedure in a consistent way!\n");
	
	//This function flushes the pending datagrams to the drivers.			
	modbus_flush(ctx);
	
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.	
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.	
	int error_status = 0;
	
	//Variables used to stored the current position and the AnalogInput0.	
	int current_position = 0;
	int analog_input0 = 0;
	
	//Try to set the driver indicated by the moveto_drv_num as the active one.			
	function_status = modbus_set_slave(ctx, position_encoder_drv_num);
	if (function_status == -1) 
	{	
		error_status = -1;	
		//output_module->Output("Exp: error, movimentation not done: set slave failed\n");
		output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-2)  + "\n");
		return -2;
	}
	
	//status_state == 3 is not a state contemplated by the firmware, so it is
	//a neutral value to initialized the variable.	
	uint16_t status_state = 3;
	
	//This variable records the success following functions.		
	int rc;
	
	//Try to read the actual state.			
	status_state = ReadStatusState(ctx, &rc, "Exp: "); //Questo rc non va messo qui!!!!

	//status_state = 4 or status_state = 5 means that the previous operation is terminated.
	//Obviously these information are hard coded!	
	//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
	//the homing function is aborted.
	int count = 0;
	while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
	{
		usleep(SLEEPSTATUS_STATE);
		count ++;
		status_state = ReadStatusState(ctx, &rc, "Exp: ");
	}
	
	if (count == LIMITSTATUS_STATE)
	{
		//output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
		output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(status_state)  + "\n");
		return status_state;
	}
	
	//Retrieving current position
	current_position = ReadCurrentPosition(ctx, &function_status, "Exp: ");
	if (function_status == -1) 
		error_status = -1;
		
	//Retrieving AnalogInput0			
	analog_input0 = ReadAnalogInput0(ctx, &function_status, "Exp: ");
	if (function_status == -1) 
		error_status = -1;
	
	//If no error occurred, try to estimate the value of analog_input0
	//starting from the current position of the driver	
	if (error_status != -1)
	{
		double tmp_estimated_value = 0;
		double tmp_current_position_value = (double) current_position;
		int paragon = 0;
		tmp_estimated_value = EncoderArrayValue[position_encoder_drv_num - 1].slope*tmp_current_position_value + 
		EncoderArrayValue[position_encoder_drv_num - 1].intercept;
		paragon = (int)round(tmp_estimated_value);
		
		//output_module->Output("Il valore di AnalogInput(0) e': " + to_string(analog_input0) + "\n");
		//output_module->Output("Quello stimato e': " + to_string(paragon) + "\n");
		
		//Compare the value of analog_input0 retrieved from the driver
		//and the estimated one.
		
		//Position mismath		
		if ( abs(paragon - analog_input0) > POSITION_TOLERATED_ERROR )
		{
			//output_module->Output("I due valori non coincidono\n");
			output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-1)  + "\n");
			
			//Position mismath	
			return -1;
		}
		else
		{
			//Position match
			//output_module->Output("I due valori coincidono!!!!\n");
			output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(0)  + "\n");
			
			//Position match
			return 0;
		}

	}
	else
	{
		output_module->Output("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-2)  + "\n");
		
		//Unable to accomplished the operation due to a communication error.		
		return -2; 
	}

}

//This function is the same of CheckPositionEncoderSingle but CheckPositionEncoderToAll
//sends the response to all the clients connected to the server. It is used by Main.c
//in order to perform a periodical check and send the response to the clients.
//This function performs the comparison between the position declared by 
//the driver (the value of the register Position) and the position 
//retrieved by the encoder (the value of the register AnalogInput0).
//The driver is indicated by position_encoder_drv_num.
//
//Return values:
//0 all okay
//-1 real position mismatch with estimated position
//-2 problem communicating with the driver
//everything > 0 the driver is blocked in an invalid state
int CheckPositionEncoderToAll (modbus_t* ctx, int position_encoder_drv_num)
{
	
	//Singleton to manage the output of the program.		
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	//This function flushes the pending datagrams to the drivers.		
	modbus_flush(ctx);
	
	//This variable will be used to record the success status of the
	//the functions interacting with the drivers.
	int function_status = 0;
	
	//This variable records the presence of an error in the communication
	//with the driver.	
	int error_status = 0;
	
	//Variables used to stored the current position and the AnalogInput0.	
	int current_position = 0;
	int analog_input0 = 0;
	
	//Try to set the driver indicated by the moveto_drv_num as the active one.		
	function_status = modbus_set_slave(ctx, position_encoder_drv_num);
	if (function_status == -1) 
	{	
		error_status = -1;	
		//output_module->Output("Exp: error, movimentation not done: set slave failed\n");
		output_module->OutputAll("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-2) + "\n");
		return -2;
	}
	
	//status_state == 3 is not a state contemplated by the firmware, so it is
	//a neutral value to initialized the variable.		
	uint16_t status_state = 3;
	
	//This variable records the success following functions.	
	int rc;
	
	//Try to read the actual state.	
	status_state = ReadStatusState(ctx, &rc, "Exp: "); //Questo rc non va messo qui!!!!

	//status_state = 4 or status_state = 5 means that the previous operation is terminated.
	//Obviously these information are hard coded!	
	//count is a timeout: if the operation is not ultimated in the times specified by LIMITSTATUS_STATE,
	//the homing function is aborted.
	int count = 0;
	while (status_state != 4 && status_state != 5 && status_state != 0 && count < LIMITSTATUS_STATE)
	{
		usleep(SLEEPSTATUS_STATE);
		count ++;
		status_state = ReadStatusState(ctx, &rc, "Exp: ");
	}
	
	if (count == LIMITSTATUS_STATE)
	{
		//output_module->Output("Exp: error, movimentation not done because status state is blocked to an invalid state\n");
		output_module->OutputAll("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(status_state)  + "\n");
		return status_state;
	}
	
	//Retrieving current position
	current_position = ReadCurrentPosition(ctx, &function_status, "Exp: ");
	if (function_status == -1) 
		error_status = -1;
		
	//Retrieving AnalogInput0		
	analog_input0 = ReadAnalogInput0(ctx, &function_status, "Exp: ");
	if (function_status == -1) 
		error_status = -1;
	
	//If no error occurred, try to estimate the value of analog_input0
	//starting from the current position of the driver.	
	if (error_status != -1)
	{
		double tmp_estimated_value = 0;
		double tmp_current_position_value = (double) current_position;
		int paragon = 0;
		tmp_estimated_value = EncoderArrayValue[position_encoder_drv_num - 1].slope*tmp_current_position_value + 
		EncoderArrayValue[position_encoder_drv_num - 1].intercept;
		paragon = (int)round(tmp_estimated_value);
		
		//output_module->Output("Il valore di AnalogInput(0) e': " + to_string(analog_input0) + "\n");
		//output_module->Output("Quello stimato e': " + to_string(paragon) + "\n");
		
		//Compare the value of analog_input0 retrieved from the driver
		//and the estimated one.
		
		//Position mismath		
		if ( abs(paragon - analog_input0) > POSITION_TOLERATED_ERROR )
		{
			//output_module->Output("I due valori non coincidono\n");
			output_module->OutputAll("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-1)  + "\n");
			
			//Position mismath				
			return -1;
		}
		else
		{
			//output_module->Output("I due valori coincidono!!!!\n");
			output_module->OutputAll("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(0)  + "\n");
			
			//Position match			
			return 0;
		}

	}
	else
	{
		output_module->OutputAll("get_pos_status " + to_string(position_encoder_drv_num) + " " + to_string(-2)  + "\n");
		
		//Unable to accomplished the operation due to a communication error.		
		return -2; 
	}

}

//This function reads the encoder values from the EncoderLog.txt file and
//load them in EncoderArrayValue. These values will be used to perform the 
//comparison between the position declared by the driver (the value of the 
//register Position) and the position retrieved by the encoder (the value
//of the register AnalogInput0). 
void LoadEncoderFromFile()
{
	//Variable used to store the latest log saved in EncoderLog.txt
	int max_log = 0;
	
	//Struct used to save the encoding values retrieved from the EncoderLog.txt file.
	EncoderStruct tmp_encoder_struct;

	//Singleton used to manage outpu_module
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	//Singleton used to retrieve the EncoderLog.txt file path.
	ApplicationSetup* command_executor_application_setup;
	command_executor_application_setup = ApplicationSetup::Instance();
	
	//Singleton used to write in the log files
	LogFile* logfile;
	logfile = LogFile::Instance();

	//If the file EncoderLog.txt can be accessed.  
	if( access( command_executor_application_setup->application_setup_encoder_log_path, F_OK ) != -1 ) {
		
		//~ //file exists
		//~ logfile->LogFileSet(command_executor_application_setup->application_setup_encoder_log_path);
		
		//Finding the last values from the EncoderLog.txt file
		vector<EncoderStruct> encoder_list;
		vector<EncoderStruct>& encoder_reference = encoder_list;
		AnalizzaFileEncoder(encoder_reference, &max_log);
		
		//Loading the values found in the array used to perform the 
		//comparison between the position declared by the driver (the value of the 
		//register Position) and the position retrieved by the encoder (the value
		//of the register AnalogInput0). 
		int i = 0;
				
		for (i = 0; i < encoder_reference.size(); i++)
		{
			EncoderArrayValue[i].drv_num = encoder_reference[i].drv_num;			
			EncoderArrayValue[i].slope = encoder_reference[i].slope;
			EncoderArrayValue[i].intercept = encoder_reference[i].intercept;
			EncoderArrayValue[i].coefficient = encoder_reference[i].coefficient;
		}
		
		//Setting to 1 the global variable that records the accomplished of the operation.
		loading_encoder_from_file_okay = 1;
		
		output_module->Output("Loading encoder values from file: okay\n");
		
	}
	else
	{
		output_module->Output("Loading encoder values from file: EncoderLog.txt not exists.\nClick check_encoder_assoc then repeat the command\n");
		
		//Setting to 1 the global variable that records the failure of the operation.
		loading_encoder_from_file_okay = 0;
	}
}

//This function prints the actual values used to compare the position declared by the driver
//(the value of the register Position) and the position retrieved by the encoder (the value
//of the register AnalogInput0).
void ReadActualEncoderValue()
{
	//Singleton to manage the output of the server.
	OutputModule* output_module;
	output_module = OutputModule::Instance();
	
	//String to bufferize the output of the server
	string output_tmp;
	
	//If the user has not loaded the encoding values from EncoderLog.txt
	if (loading_encoder_from_file_okay == 0)
		output_tmp += "Loading encoder values from file: NOT DONE, click Load Encoder From File Button\n";
	else
		output_tmp += "Loading encoder values from file: DONE\n";
	
	int i = 0;
	
	//Anyhow, sent to the client the required values.
	for (i = 0; i < MAXIMUM_DRIVER; i++)
	{
		output_tmp += "Encoder values, drv num " + to_string(i+1) +  " -> Slope: " + to_string(EncoderArrayValue[i].slope) + " Intercept: " + 
		to_string(EncoderArrayValue[i].intercept) + " Coefficient: " + to_string(EncoderArrayValue[i].coefficient) + "\n";
	}
	
	output_module->Output(output_tmp);
}
