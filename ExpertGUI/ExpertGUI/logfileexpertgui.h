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

#ifndef LOGFILEEXPERTGUI_H
#define LOGFILEEXPERTGUI_H

#include <iostream>
#include <fstream>
#include <string>
#include <mutex>
using namespace std;

//Singleton used to write running information to the log file.
//The log file path is hard coded in the singleton constructor.

class LogFileExpertGUI
{
public:
    void LogFileExpertGUI_WriteOnLogFile(string text_being_written);

    static LogFileExpertGUI* Instance ();

    void SetUserHeader(string user_header_arg);



private:

    LogFileExpertGUI();

    static LogFileExpertGUI* log_file_pInstance;

    ofstream log_file_stream;
    string GetTime();

    string user_header;

    std::mutex mtx_log;

    static std::mutex mtx_constructor;


};

#endif // LOGFILEEXPERTGUI_H
