/* scanner for FileParLog.txt language */
%{

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

#include <stdlib.h>
#include <vector>
#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <stdint.h>
#include "Utils.h"
#include "DefineGeneral.h"

using namespace std;

#define YY_DECL int yylex (vector<ParameterStruct>& parameter_vector, int* max_log)

int AnalizzaParLastLog = -1;

%}

%x LASTLOG
%x SERIALOG
%x FINALOG

%%

<LASTLOG>^Log[ ][0-9]{1,10}[ \t]*$	{
	int tmp = atoi(yytext + 4);
	if (tmp > AnalizzaParLastLog)
	{
		AnalizzaParLastLog = tmp;
		*max_log = tmp;
	}
}

<SERIALOG>^Log[ ][0-9]{1,10}[ \t]*$	{
	int tmp = atoi(yytext + 4);
	if (tmp == AnalizzaParLastLog)
	{ 
		BEGIN(FINALOG);
	}
}

<FINALOG>^[0-9]{1,2}[ \t][0-9]{1,10}[ \t]-{0,1}[0-9]{1,10}[ \t][0-9]{1,10}[ \t][0-9]{1,10}[ \t][0-9]{1,10}[ \t][0-9]{1,10}[ \t]*$	{
	ParameterStruct tmp1;
	tmp1.drv_num = -1;
	tmp1.max_vel = 0;
	tmp1.vel_home = 0;
	tmp1.acceleration = 0;
	tmp1.deceleration = 0;
	tmp1.phase_current = 0;
	tmp1.analog_output0 = 0;
	int num_drv = atoi(yytext);
	tmp1.drv_num = num_drv;
	char* mypunt = FindPointer(yytext);
	tmp1.max_vel = atoi(mypunt);
	mypunt = FindPointer(mypunt);
	tmp1.vel_home = (int16_t)atoi(mypunt);
	mypunt = FindPointer(mypunt);
	tmp1.acceleration = atoi(mypunt);
	mypunt = FindPointer(mypunt);
	tmp1.deceleration = atoi(mypunt);
	mypunt = FindPointer(mypunt);
	tmp1.phase_current = atoi(mypunt);
	mypunt = FindPointer(mypunt);
	tmp1.analog_output0 = atoi(mypunt);
	parameter_vector.push_back(tmp1);
}

<*>\n
<*>.

%%

int yywrap(void) {return 1;}

void AnalizzaFilePar(vector<ParameterStruct>& parameter_arg, int* max_log)
{
	 AnalizzaParLastLog = -1;
     BEGIN(LASTLOG);
     yyin = fopen( FILEPARLOG, "r" ); //Hard coding problem!!!!
     
     if (yyin != NULL)
     {
		 yylex(parameter_arg, max_log);
		 rewind(yyin);
		 BEGIN(SERIALOG);
		 yylex(parameter_arg, max_log);
	 }
}
