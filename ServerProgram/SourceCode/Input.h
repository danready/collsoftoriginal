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

#ifndef __INPUTH
#define __INPUTH

/**
* @file Input.h 
*/

/**
* @brief The Input class gets the input from stdin.
*
* @author Daniele Berto
*/

#include <thread>
#include <mutex>
#include <condition_variable>

using namespace std;

class Input
{
public:

  /**
   * @brief The input_buffer variable contains the informations shared by the threads.
   */
  char *input_buffer;

  /**
   * @brief The GetInput method copies the input from the stdin to the parameter input_buffer_sending.
   * @details The caller of this method gets the user input: the method copies it in the parameter.
   * @param input_buffer_sending is where the informations are copied
   * @return void
   */
  void GetInput (char *input_buffer_sending);

  /**
   * @brief The Finish method shuts down the object threads.
   * @return void 
   */
  void Finish ();

  /**
   * @brief The producer_thread gets the input from the stdin and puts it in the input_buffer attribute.
   */
  thread * producer_thread;

  /**
   * @brief The mutex1 mutex guarantees the consistency of the informations retrieved by the get_input method.
   */
  mutex mutex1;

  /**
   * @brief The go variable controls the threads.
   */
  int go;

  /**
   * @brief The num_mex variable indicates if a message is ready to be picked by the get_input method.
   */
  int num_mex;

  /**
   * @brief The Producer method gets the input from the stdin and puts it in the input_buffer attribute.
   * @return void
   */
  void Producer ();

  /**
   * @brief The Input constructor sets num_mex to 0 and go to 1. 
   * @details It also initializates input_buffer and creates the producer thread.
   */
  Input ();
};

#endif
