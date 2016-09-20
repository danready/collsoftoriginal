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

#ifndef __APPLICATIONSETUPH
#define __APPLICATIONSETUPH

/**
* @file ApplicationSetup.h
* @author Daniele Berto
*/

/**
* @brief The ApplicationSetup class: This object collects information about LogFile path and the input modality.
* @author Daniele Berto
*/

#include <stdio.h>

class ApplicationSetup
{
private:

  /**
   * @brief The application_setup_pInstance reference is used to implement the singleton design pattern.
   */
  static ApplicationSetup *application_setup_pInstance;

  /**
   * @brief The ApplicationSetup constructor sets imset variable to zero.
   */
    ApplicationSetup ();
public:

  /**
   * @brief Instance() method is used to implement the singleton design pattern: it returns a reference to ApplicationSetup.
   */
  static ApplicationSetup *Instance ();
  
  /**
   * @brief The application_setup_serial_drv_log_file_path variable contains the path of the SerialDrvLog.txt file.
   */
  const char *application_setup_serial_drv_log_file_path;  
  
  /**
   * @brief The application_setup_file_par_log_path variable contains the path of the FileParLog.txt file.
   */
  const char *application_setup_file_par_log_path;    

  /**
   * @brief The application_setup_encoder_log_path variable contains the path of the EncoderLog.txt file.
   */
  const char *application_setup_encoder_log_path;
  
  /**
   * @brief The application_setup_general_log_path variable contains the path of the GeneralLog.txt file.
   */
  const char *application_setup_general_log_path;      

  /**
   * @brief The application_setup_input_mode variable contains the input mode.
   */
  const char *application_setup_input_mode;

  /**
   * @brief The input_mode variable contains the input mode chosed by the user.
   */
  int input_mode;

  /**
   * @brief The FetchInputMode method picks up the input mode from the DefineGeneral.h file.
   * @return void
   */
  void FetchInputMode (const char *application_setup_input_mode);
};
#endif
