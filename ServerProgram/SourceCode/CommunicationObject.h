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

#ifndef __COMMUNICATIONOBJECTH
#define __COMMUNICATIONOBJECTH

/**
* @file CommunicationObject.h 
*/

/**
* @brief The CommunicationObject class is used for manage the input sent to the application by the clients.
* @details The caller has only to call "GetCommand" method in order to get the input. 
* "GetCommand" method is non-blocking: it returns no command if no input is available.
* @author Daniele Berto
*/

#include "DefineGeneral.h"
#include <pthread.h>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;


class CommunicationObject
{
public:

  /**
   * @brief The num_mex variable records the number of messages stored in the circulare buffer command[MAXCOMMAND].
   */
  int num_mex;

  /**
   * @brief The testa variable is the head pointer.
   */
  int testa;

  /**
   * @brief The coda variable is the tail pointer.
   */
  int coda;

  /**
   * @brief The go variable controls the main and the worker threads.
   */
  int go;

  /**
   * @brief The GetCommand method copies in the parameters the socket id of the sender and the command sent by it.
   * @param socketid is the socket id of the sender.
   * @param command is the command sent by the sender.
   * @return void
   */
  void GetCommand (int *socketid, int *command);

  /**
   * @brief The GetCommand method with no arguments return a command sent by someone to the application.
   * @return The command sent by someone.
   * @return TcpUser
   */
  TcpUser GetCommand ();

  /**
   * @brief The CommunicationObjectInit method creates all the things needed by the threads.
   * @details This method initializes the mutexes and the cond variables and sets go to 1 and creates the main_thread.
   * @return void
   */
  void CommunicationObjectInit ();

  /**
   * @brief The main_thread initializes a socket master and listen to connection requests.
   * @details It creates a worker_thread for each incoming connection. Where variable go is set to zero it closes the socket master 
   * and finish.
   */
  void Main ();

  /**
   * @brief The worker_thread manage the incoming input from a client and put it in the circular buffer command[MAXCOMMAND].
   * @details Every worker_thread is created by the thread_main.
   * @param socket_desc is the socket id of the client served by the worker_thread 
   */
  void Worker (void *socket_desc);

  /**
   * @brief command[MAXCOMMAND] is the circular buffer where the worker threads put the input sent by the clients.
   * @details When get_command method is called, a value is picked by the circular buffer and returned to the caller.
   * The access to the buffer is managed by a producers-consumer schema: the worker threads are the producers, the get_command
   * method is the consumer.
   */
  TcpUser command[MAXCOMMAND];

  /**
   * @brief The Finish method sets to 0 the go variable for calling forth the threads to exit.
   * @return void
   */
  void Finish ();

  /**
   * @brief The main_thread variable is the main thread handler.
   */
  thread * main_thread;

  /**
   * @brief The thread id of the Worker thread.
   */
  thread * worker_thread;

  /**
   * @brief A mutex used to implement producer-consumer model.
   */
  mutex ReservedKeyBoardInputArea;

  /**
   * @brief A cond variable used to implement producer-consumer model.
   */
  condition_variable BlockedProducerInput;

  /**
   * @brief A cond variable used to implement producer-consumer model.
   */
  condition_variable BlockedConsumerInput;

  /**
   * @brief A mutex used to implement producer-consumer model.
   */
  mutex Acquisition_Mutex;

  /**
   * @brief A cond variable used to implement producer-consumer model.
   */
  condition_variable Acquisition_Cond1;

  /**
   * @brief A cond variable used to implement producer-consumer model.
   */
  condition_variable Acquisition_Cond2;

  /**
   * @brief The ComunicationObject constructor sets to 0 num_mex and coda variables. It also sets to 1 testa variable.
   */
    CommunicationObject ();
};

#endif
