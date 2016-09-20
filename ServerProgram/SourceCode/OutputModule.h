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

#ifndef __OUTPUTMODULEH
#define __OUTPUTMODULEH

/**
* @file OutputModule.h 
*/

/**
* @brief The OutputModule class provides an useful way to manage the output of the program.
* The program has to send output not only to stdout but also to the tcp users. The OutputModule class
* takes note of the output target with the "OutputModuleStdoutOn/Off" and the "OutputModuleSockidOn/Off"
* methods. It also takes note of the TCP users active on the server storing their sockid in sockid_array.
* It is possible to insert/delete a sockid with "TcpUserArrayInsert"/"Delete" methods.
*
* @author Daniele Berto
*/

#include "DefineGeneral.h"
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

using namespace std;

class OutputModule
{

private:
  /**
   * @brief The OutputModule constructor sets to -1 the "sockid_array", to 0 the "buffer" attribute and calls the
   * "OutputModuleStdoutOn" and the "OutputModuleSockidOff" methods in order to be able to send the output to the stdout and not
   * via TCP/IP. It is reasonable because, when the program starts, no TCP/IP user is connected to the server but the program has to 
   * print a start message to the stdout.
   */
  OutputModule ();

  /**
  * @brief The "outputmodule_pInstance" attribute is used to implement the singleton design pattern.
  */
  static OutputModule *outputmodule_pInstance;

  /**
  * @brief The "output_module_stdout" attribute takes note of the output mode. It is modified by the "OutputModuleStdoutOn"/"Off" methods.
  */
  int output_module_stdout;

  /**
  * @brief The "output_module_sockid" attribute takes note of the output mode. It is modified by the "OutputModuleSockidOn"/"Off" methods.
  */
  int output_module_sockid;

public:

  /**
  * @brief The "output_module_application_setup" is a pointer to the ApplicationSetup singleton.
  */
  ApplicationSetup * output_module_application_setup;

  /**
  * @brief The "Instance ()" method is used to implement the singleton design pattern.
  */
  static OutputModule *Instance ();

  /**
  * @brief The "buffer" array stores the output being printed. It is filled by the "Output(const char * string)" method.
  */
  char buffer[STANDARDBUFFERLIMIT];

  /**
  * @brief The "StdOutInsert" method copies the parameter "string" to the "buffer" attribute.
  */
  void StdOutInsert (const char *string);

  /**
  * @brief The "StdOutPrint" method prints buffer to the stdout.
  */
  void StdOutPrint ();

  /**
  * @brief The "OutputModuleStdoutOn" method enables the object to print "buffer" to the stdout.
  */
  void OutputModuleStdoutOn();

  /**
  * @brief The "OutputModuleStdoutOff" method disables the object to print "buffer" to the stdout.
  */
  void OutputModuleStdoutOff();

  /**
  * @brief The "OutputModuleSockidOn" method enables the object to send "buffer" to the TCP/IP user identificated by the "sockid" parameter.
  * The "sockid" parameter is copied to the "output_module_sockid" attribute.
  */
  void OutputModuleSockidOn(int sockid);

  /**
  * @brief The "OutputModuleSockidOn" method disables the object to send "buffer" via TCP/IP.
  */
  void OutputModuleSockidOff();

  /**
  * @brief The "sockid_array" array takes note of the users connected to the server via TCP/IP.
  */
  int sockid_array[MAXIMUM_CLIENT];

  /**
  * @brief The "TcpUserArrayInsert" method inserts the parameter "sockid" in the "sockid_array".
  */
  int TcpUserArrayInsert (int sockid);

  /**
  * @brief The "TcpUserArrayDelete" method deletes the parameter "sockid" from the "sockid_array".
  */
  int TcpUserArrayDelete (int sockid);

  /**
  * @brief The "TcpUserArraySendStdOut" method sends the "buffer" attribute to the TCP/IP user identified by the output_module_sockid attribute.
  */
  int TcpUserArraySendStdOut ();

  /**
  * @brief The "Output" method prints the "string" parameter to the stdout or send it via TCP/IP in accordance with the "output_module_stdout"
  * and the "output_module_sockid" (these variable could be modified by the "OutputModuleStdoutOn"/"Off" and "OutputModuleSockidOn"/"Off" methods).
  */
  void Output(string string_arg);

  void OutputAll(string string_arg);

  /**
  * @brief The "OutputRead" method. You have to change this name...
  */
  void OutputReadLogFile(string string_arg);
  
  static mutex mtx_constructor;
  
  mutex mtx_output;
  
};

#endif
