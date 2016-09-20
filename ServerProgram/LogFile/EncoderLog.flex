/* scanner for EncoderLog.txt language */
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
#include <string>
#include "Utils.h"
#include "DefineGeneral.h"

using namespace std;

#define YY_DECL int yylex (vector<EncoderStruct>& encoder_vector, int* max_log)

int AnalizzaEncoderLastLog = -1;

%}

%x LASTLOG
%x SERIALOG
%x FINALOG

%%

<LASTLOG>^Log[ ][0-9]{1,10}[ \t]*$	{
	int tmp = atoi(yytext + 4);
	if (tmp > AnalizzaEncoderLastLog)
	{
		AnalizzaEncoderLastLog = tmp;
		*max_log = tmp;
	}
}

<SERIALOG>^Log[ ][0-9]{1,10}[ \t]*$	{
	int tmp = atoi(yytext + 4);
	if (tmp == AnalizzaEncoderLastLog)
	{ 
		BEGIN(FINALOG);
	}
}

<FINALOG>^[0-9]{1,2}[ \t]-{0,1}[0-9]{1,10}.{0,1}[0-9]{0,10}[ \t]-{0,1}[0-9]{1,10}.{0,1}[0-9]{0,10}[ \t]-{0,1}[0-9]{1,10}.{0,1}[0-9]{0,10}[ \t]*$	{
	EncoderStruct tmp1;
	tmp1.drv_num = -1;
	tmp1.slope = -1;
	tmp1.intercept = -1;
	tmp1.coefficient = -1;
	int num_drv = atoi(yytext);
	tmp1.drv_num = num_drv;
	char* mypunt = FindPointer(yytext);
	tmp1.slope = std::stod(mypunt);
	mypunt = FindPointer(mypunt);
	tmp1.intercept = std::stod(mypunt);
	mypunt = FindPointer(mypunt);
	tmp1.coefficient = std::stod(mypunt);
	encoder_vector.push_back(tmp1);
}

<*>\n
<*>.

%%

int yywrap(void) {return 1;}

void AnalizzaFileEncoder(vector<EncoderStruct>& encoder_arg, int* max_log)
{
	AnalizzaEncoderLastLog = -1;
	BEGIN(LASTLOG);
	yyin = fopen( ENCODERLOG, "r" ); //Hard coding problem!!!!

	if (yyin != NULL)
	{
		yylex(encoder_arg, max_log);
		rewind(yyin);
		BEGIN(SERIALOG);
		yylex(encoder_arg, max_log);
	}
}
