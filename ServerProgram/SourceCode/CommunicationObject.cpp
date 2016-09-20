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
* @file CommunicationObject.cpp
* @author Daniele Berto
*/

#include "DefineGeneral.h"
#include "TcpUser.h"
#include "LogFile.h"
#include "ApplicationSetup.h"
#include "Input.h"
#include "CommunicationObject.h"
#include "OutputModule.h"
#include "Utils.h"
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <thread>
#include <mutex>
#include <condition_variable>

#include <vector>
#include <sstream>
#include <utility>

#include <netinet/in.h>
#include <netinet/tcp.h>

using namespace std;


std::vector<std::string> explode(std::string const & s, char delim)
{
    std::vector<std::string> result;
    std::istringstream iss(s);

    for (std::string token; std::getline(iss, token, delim); )
    {
        result.push_back(std::move(token));
    }

    return result;
}


void
CommunicationObject::Main ()
{

  OutputModule *output_module;
  output_module = OutputModule::Instance ();
  
  ApplicationSetup* comm_object_application_setup;
  comm_object_application_setup = ApplicationSetup::Instance ();
  
  LogFile *logfile;
  logfile = LogFile::Instance ();

  int client_sock;
  int c;
  struct sockaddr_in server, client;
  int socket_desc;
  socket_desc = socket (AF_INET, SOCK_STREAM, 0);

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = inet_addr(SERVER_IP);
  server.sin_port = htons (SERVER_PORT);

  int flag1 = 1;
  int result1 = setsockopt(client_sock,   /* socket affected */
                        IPPROTO_TCP,     /* set option at TCP level */
                        TCP_NODELAY,     /* name of option */
                        (char *) &flag1,  /* the cast is historical cruft */
                        sizeof(int));    /* length of option value */

  /*Binding the socket*/
  bind (socket_desc, (struct sockaddr *) &server, sizeof (server));

  while (go)
    {

      listen (socket_desc, 1);

      c = sizeof (struct sockaddr_in);

      if (go == 0)
	{
	  break;
	}

   client_sock = 
   accept (socket_desc, (struct sockaddr *) &client, (socklen_t *) & c);

   int flag = 1;
   int result = setsockopt(client_sock,  /* socket affected */
                        IPPROTO_TCP,     /* set option at TCP level */
                        TCP_NODELAY,     /* name of option */
                        (char *) &flag,  /* the cast is historical cruft */
                        sizeof(int));    /* length of option value */

      if (go == 0)
	{
	  break;
	}

      if (client_sock >= 0)
	{
	  output_module->OutputModuleStdoutOff();
	  output_module->OutputModuleSockidOn(client_sock);
	  output_module->TcpUserArrayInsert (client_sock);
	  output_module->Output("Welcome: Comunicazione col server stabilita\n");
	  
	  logfile->LogFileSet(comm_object_application_setup->application_setup_general_log_path);
	  string tmp_address = inet_ntoa(client.sin_addr);
	  logfile->LogFileWriteString(GetTime() + ": Assigned socket id " + to_string(client_sock) + " to client with ip " + tmp_address + "\n\n");
	  
	  worker_thread = new thread (&CommunicationObject::Worker, this,
			  (void *) &client_sock);
      worker_thread->detach();
	}

    }   // while go

}


void 
CommunicationObject::Worker (void *socket_desc)
{
  OutputModule *output_module;
  output_module = OutputModule::Instance ();

  ApplicationSetup* comm_object_application_setup;
  comm_object_application_setup = ApplicationSetup::Instance ();
  
  LogFile *logfile;
  logfile = LogFile::Instance ();

  int j;
  int sock = *(int *) socket_desc;
  int read_size;
  char buffer[STANDARDBUFFERLIMIT];
  const char *my_punt;
  bzero (buffer, STANDARDBUFFERLIMIT);

  while (go)
    {
      bzero (buffer, STANDARDBUFFERLIMIT);
      if ((read_size = recv (sock, buffer, STANDARDBUFFERLIMIT, MSG_NOSIGNAL)) <= 0)
		{
		  logfile->LogFileSet(comm_object_application_setup->application_setup_general_log_path);
		  logfile->LogFileWriteString(GetTime() + ": User with socket id " + to_string(sock) + " disconnected\n\n");
		  
		  output_module->TcpUserArrayDelete (sock);
		  return;
		}

      string tmp(buffer);
      auto command_list = explode(tmp, '\n');
      int i = 0;

	  for ( i = 0; i < command_list.size() ; i ++)
	  {
		  unique_lock<mutex> ReservedKeyBoardInputAreaHandle(ReservedKeyBoardInputArea);

		  if (num_mex == MAXCOMMAND)
			BlockedProducerInput.wait(ReservedKeyBoardInputAreaHandle);
			
		  coda = (coda + 1) % MAXCOMMAND;
		  //strncpy (command[coda].command_sent_by_user, buffer, STANDARDBUFFERLIMIT - 1);
		  strncpy (command[coda].command_sent_by_user, command_list[i].c_str(), STANDARDBUFFERLIMIT - 1);
		  command[coda].user_sockid = sock;
		  
		  num_mex++;

		  BlockedConsumerInput.notify_one();
	  }


    }    // while go

      BlockedConsumerInput.notify_one();

}


void
CommunicationObject::CommunicationObjectInit ()
{

  go = 1;

  main_thread = new thread (&CommunicationObject::Main, this);

}


TcpUser CommunicationObject::GetCommand ()
{
  TcpUser
    tmp;
  bzero(tmp.command_sent_by_user, STANDARDBUFFERLIMIT);
  tmp.user_sockid = -1;
      unique_lock<mutex> ReservedKeyBoardInputAreaHandle(ReservedKeyBoardInputArea);

  if (num_mex == 0)
    {
		bzero(tmp.command_sent_by_user, STANDARDBUFFERLIMIT);
    }
  else
    {
      tmp = command[testa];

      command[testa].user_sockid = -1;
      bzero (command[testa].command_sent_by_user, STANDARDBUFFERLIMIT);

      testa = (testa + 1) % MAXCOMMAND;

      num_mex--;
      BlockedProducerInput.notify_one();
    }
  return tmp;
}


void
CommunicationObject::Finish ()
{
  int i;

  go = 0;

  for (i = 0; i < 10; i++)
      BlockedProducerInput.notify_one();

}


CommunicationObject::CommunicationObject ()
{
  num_mex = 0;
  coda = 0;
  testa = 1;
}
