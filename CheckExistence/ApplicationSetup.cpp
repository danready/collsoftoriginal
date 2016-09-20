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

/*
Implementazione standard dei metodi per implementare il singleton design pattern
*/
ApplicationSetup *
  ApplicationSetup::application_setup_pInstance = NULL;

ApplicationSetup *
ApplicationSetup::Instance ()
{
  if (!application_setup_pInstance)   // Only allow one instance of class to be generated.
    application_setup_pInstance = new ApplicationSetup ();

  return application_setup_pInstance;
}


///Il costruttore (privato) del singleton riempie le varibili contenenti i path dei files usati dal programma e la modalita' di input con valori di default.
ApplicationSetup::ApplicationSetup ()
{
  application_setup_input_mode = (const char *) malloc ((strlen (INPUTMODALITY) + 1));
  strcpy ((char *) application_setup_input_mode, INPUTMODALITY);
}

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
