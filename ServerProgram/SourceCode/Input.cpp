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
* @file Input.cpp 
*/

#include "DefineGeneral.h"
#include "Input.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <thread>
#include <mutex>
#include <pthread.h>
#include <condition_variable>

//Limite della lunghezza dell'input inserito da tastiera
//#define STANDARDBUFFERLIMIT 5000

using namespace std;

Input::Input ()
{
  int i;
  num_mex = 0;
  go = 1;
  input_buffer = (char *) malloc (STANDARDBUFFERLIMIT);
  for (i = 0; i < STANDARDBUFFERLIMIT; i++)
    input_buffer[i] = '\0';
  producer_thread = new thread (&Input::Producer, this);
}

void
Input::Producer ()
{
  int i;
  char input_buffer_private[STANDARDBUFFERLIMIT];

  //go is set to 1 by the costructor. The finish() method set it to 0.
  while (go)
    {
      for (i = 0; i < STANDARDBUFFERLIMIT; i++)
	{
	  input_buffer_private[i] = '\0';
	}

      fgets (input_buffer_private, STANDARDBUFFERLIMIT-1, stdin);	//getting the input from stdin

      for (i = 0; i < STANDARDBUFFERLIMIT; i++)
	{
	  if (input_buffer_private[i] == '\n')
	    input_buffer_private[i] = '\0';
	}

      unique_lock<mutex> lk(mutex1);	//assure data consistency

      for (i = 0; i < STANDARDBUFFERLIMIT; i++)
	{
	  input_buffer[i] = input_buffer_private[i];
	}

      num_mex = 1;  // a message is ready

      lk.unlock();
    }


}


void
Input::GetInput (char *input_buffer_sending)
{
  int i;
      unique_lock<mutex> lk(mutex1);	//assure data consistency

  if (num_mex == 0)	// if no message set the input_buffer_sending to zero
    {
      for (i = 0; i < STANDARDBUFFERLIMIT; i++)
	input_buffer_sending[i] = '\0';

    }

  else		// if there is a message, copy it into input_buffer_sending
    {
      for (i = 0; i < STANDARDBUFFERLIMIT; i++)
	{
	  input_buffer_sending[i] = input_buffer[i];
	}
      num_mex = 0;	//no message ready
    }
}

void
Input::Finish ()
{
  go = 0;	// stop the producer thread
}
