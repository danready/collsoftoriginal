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

#include "logfileusergui.h"
#include <cstdlib>
#include <cstring>
#include <mutex>

//The LogFileUserGui singleton is used to write to the log file the most important running information
//of the program.

LogFileUserGui *
  LogFileUserGui::log_file_pInstance = NULL;

mutex LogFileUserGui::mtx_constructor;


LogFileUserGui *
LogFileUserGui::Instance ()
{
  mtx_constructor.lock();
  if (!log_file_pInstance)	// Only allow one instance of class to be generated.
    log_file_pInstance = new LogFileUserGui ();
  mtx_constructor.unlock();

  return log_file_pInstance;
}

void LogFileUserGui::SetUserHeader(string user_header_arg)
{
    mtx_log.lock();
    user_header = user_header_arg;
    mtx_log.unlock();
}

LogFileUserGui::LogFileUserGui()
{
    log_file_stream.open("../../UserGUI/LogFile/logfile_usergui.txt", ofstream::app);

    user_header = "";

    if (log_file_stream.is_open())
    {

        log_file_stream << "\n#### User gui starting: " << GetTime() << " ####" << endl;

        /* ok, proceed with output */

    }
}

//The function GetTime returns a string that represent the current date.
string LogFileUserGui::GetTime()
{
    char* ts = (char*)malloc(100*sizeof(char));
    time_t t;
    t = time (NULL);
    ctime_r (&t, ts);
    ts[strlen (ts) - 1] = '\0';

    string tmp_string = ts;

    free(ts);

    return tmp_string;
}


void LogFileUserGui::LogFileUserGui_WriteOnLogFile(string text_being_written)
{
    mtx_log.lock();
    if (log_file_stream.is_open())
    {

    log_file_stream  << GetTime() << " " << user_header << ": " << text_being_written << endl;

    /* ok, proceed with output */

    }
    mtx_log.unlock();
}
