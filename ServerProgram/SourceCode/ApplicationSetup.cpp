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
* @file ApplicationSetup.cpp
* @author Daniele Berto
*/

#include "ApplicationSetup.h"
#include "OutputModule.h"
#include "DefineGeneral.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

//This object collects information about LogFile path and the input modality.

//In the constructor there are not mutex like in the other objects because its values are not modified
//during the execution of the program.
ApplicationSetup *
  ApplicationSetup::application_setup_pInstance = NULL;

ApplicationSetup *
ApplicationSetup::Instance ()
{
  if (!application_setup_pInstance)   // Only allow one instance of class to be generated.
    application_setup_pInstance = new ApplicationSetup ();

  return application_setup_pInstance;
}


ApplicationSetup::ApplicationSetup ()
{
/////Init the memory for SerialDrvLog.txt file path  
  application_setup_serial_drv_log_file_path = (const char *)malloc ((strlen (SERIALDRVLOG) + 1));
  strcpy ((char *) application_setup_serial_drv_log_file_path, SERIALDRVLOG);

/////Init the memory for FileParLog.txt file path
  application_setup_file_par_log_path = (const char *)malloc ((strlen (FILEPARLOG) + 1));
  strcpy ((char *) application_setup_file_par_log_path, FILEPARLOG);

/////Init the memory for EncoderLog.txt file path
  application_setup_encoder_log_path = (const char *)malloc ((strlen (ENCODERLOG) + 1));
  strcpy ((char *) application_setup_encoder_log_path, ENCODERLOG);
  
/////Init the memory for GeneralLog.txt file path
  application_setup_general_log_path = (const char *)malloc ((strlen (GENERALOG) + 1));
  strcpy ((char *) application_setup_general_log_path, GENERALOG);

//Save the input modality
//N.B. The INPUTMODALITY definition is in DefineGeneral.h file.
  application_setup_input_mode = (const char *) malloc ((strlen (INPUTMODALITY) + 1));
  strcpy ((char *) application_setup_input_mode, INPUTMODALITY);
}

//Return the Input modality:
//0 = ONLYUSER
//1 = ONLYTCP
//2 = ALL (the default choice).
//See the DefineGeneral.h file in order to get information about the input modality and
//the definitions. 
void
ApplicationSetup::FetchInputMode (const char *application_setup_input_mode)
{

  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  if (!strcmp ("user", application_setup_input_mode))
    {
      input_mode = 0;
      output_module->Output("User command mode activated\n");
    }

  else if (!strcmp ("tcp", application_setup_input_mode))
    {
      input_mode = 1;
      output_module->Output("Tcp command mode activated\n");
    }

  else if (!strcmp ("all", application_setup_input_mode))
    {
      input_mode = 2;
      output_module->Output("Tcp and User command mode activated\n");
    }

  else
    {
      output_module->Output("Tcp and User command mode activated\n");
      input_mode = 2;
    }
}
