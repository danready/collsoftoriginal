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
* @file OutputModule.cpp 
*/

#include "DefineGeneral.h"
#include "ApplicationSetup.h"
#include "OutputModule.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <thread>
#include <mutex>

#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace std;

OutputModule *
  OutputModule::outputmodule_pInstance = NULL;

mutex OutputModule::mtx_constructor;

OutputModule *
OutputModule::Instance ()
{
  mtx_constructor.lock();	
  if (!outputmodule_pInstance)	// Only allow one instance of class to be generated.
    outputmodule_pInstance = new OutputModule ();
  mtx_constructor.unlock();

  return outputmodule_pInstance;
}

OutputModule::OutputModule ()
{
  OutputModule::OutputModuleSockidOff();
  OutputModule::OutputModuleStdoutOn();

  bzero (buffer, STANDARDBUFFERLIMIT);

  int i;
  for (i = 0; i < MAXIMUM_CLIENT; i++)
    sockid_array[i] = -1;
}

void
OutputModule::StdOutInsert (const char *string_arg)
{
  bzero (buffer, STANDARDBUFFERLIMIT);
  strncpy (buffer, string_arg, STANDARDBUFFERLIMIT);
}

void
OutputModule::StdOutPrint ()
{
  output_module_application_setup = ApplicationSetup::Instance ();

  if ((output_module_application_setup->input_mode == 2
       || output_module_application_setup->input_mode == 0)
      && output_module_stdout == 1)
    {
      fprintf (stdout, "%s", buffer);
    }
}  //void OutputModule::StdOutPrint()

int
OutputModule::TcpUserArrayInsert (int sockid)
{
  mtx_output.lock();
  int i;
  for (i = 0; i < MAXIMUM_CLIENT; i++)
    {
      if (sockid_array[i] == -1)
	{
	  sockid_array[i] = sockid;
	  mtx_output.unlock();
	  return 0;
	}
    }
  mtx_output.unlock();  
  return 1;
}  //int OutputModule::TcpUserArrayInsert(int sockid)



int
OutputModule::TcpUserArrayDelete (int sockid)
{
  mtx_output.lock();	
  int i;
  for (i = 0; i < MAXIMUM_CLIENT; i++)
    {
      if (sockid_array[i] == sockid)
	{
	  sockid_array[i] = -1;
	  mtx_output.unlock();
	  return 0;
	}
    }
  mtx_output.unlock();
  return 1;
}  //int OutputModule::TcpUserArrayDelete(int sockid)


int
OutputModule::TcpUserArraySendStdOut ()
{
  int i,j = 0;
  output_module_application_setup = ApplicationSetup::Instance ();

  if (output_module_application_setup->input_mode == 2
      || output_module_application_setup->input_mode == 1)
    {
      for (i = 0; i < MAXIMUM_CLIENT; i++)
	{
	  if (sockid_array[i] > -1
	      && sockid_array[i] == output_module_sockid)
	    {
			send (sockid_array[i], buffer, STANDARDBUFFERLIMIT, MSG_NOSIGNAL);
			usleep(SLEEPTCP);

			if (FLUSH_OUTPUTMODULETCP == 1)
			{
				//Setting TCP_NODELAY implies an implicit flush of the socket
				int flag = 1;
				setsockopt(sockid_array[i],  /* socket affected */
						IPPROTO_TCP,     /* set option at TCP level */
						TCP_NODELAY,     /* name of option */
						(char *) &flag,  /* the cast is historical cruft */
						sizeof(int));    /* length of option value */
			}
	    }	//if(sockid_array[i]>-1)
	}   //for(i=0; i<MAXIMUM_CLIENT; i++)
    }

  return 0;
}   //int OutputModule::TcpUserArraySendStdOut(StdOut * stdout_arg)

 void OutputModule::Output(string string_arg){

mtx_output.lock();
OutputModule::StdOutInsert (string_arg.c_str()); 
OutputModule::StdOutPrint(); 
OutputModule::TcpUserArraySendStdOut ();
mtx_output.unlock();

}

 void OutputModule::OutputAll(string string_arg){

mtx_output.lock();
OutputModule::StdOutInsert (string_arg.c_str()); 

output_module_application_setup = ApplicationSetup::Instance ();

if ((output_module_application_setup->input_mode == 2
   || output_module_application_setup->input_mode == 0))
{
  fprintf (stdout, "%s", buffer);
}

if (output_module_application_setup->input_mode == 2
  || output_module_application_setup->input_mode == 1)
{
  for (int i = 0; i < MAXIMUM_CLIENT; i++)
	{
	  if (sockid_array[i] > -1)
		{
		  send (sockid_array[i], buffer, STANDARDBUFFERLIMIT, MSG_NOSIGNAL);
		  usleep(SLEEPTCP);
		}	//if(sockid_array[i]>-1)
	}   //for(i=0; i<100; i++)
}

mtx_output.unlock();

}



void OutputModule::OutputReadLogFile(string string_arg){
	
mtx_output.lock();
string tmp = "Reading LogFile...\n" + string_arg;
OutputModule::StdOutInsert (tmp.c_str());
OutputModule::StdOutPrint(); 
OutputModule::TcpUserArraySendStdOut ();
mtx_output.unlock();

}

void OutputModule::OutputModuleStdoutOn(){
mtx_output.lock();
output_module_stdout = 1;
mtx_output.unlock();
}

void OutputModule::OutputModuleStdoutOff(){
mtx_output.lock();
output_module_stdout = -1;
mtx_output.unlock();
}

void OutputModule::OutputModuleSockidOn(int sockid)
{
mtx_output.lock();	
output_module_sockid = sockid;
mtx_output.unlock();
}

void OutputModule::OutputModuleSockidOff()
{
mtx_output.lock();
output_module_sockid = -1;
mtx_output.unlock();
}
