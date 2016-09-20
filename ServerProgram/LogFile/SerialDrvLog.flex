/* scanner for SerialDrvLog.txt language */
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
#include "Utils.h"
#include "DefineGeneral.h"

using namespace std;

#define YY_DECL int yylex (vector<SerialCouple>& serial, int* max_log)

int Analizza1LastLog = -1;

%}

%x LASTLOG
%x SERIALOG
%x FINALOG

%%

<LASTLOG>^Log[ ][0-9]{1,10}[ \t]*$	{
	int tmp = atoi(yytext + 4);
	
	if (tmp > Analizza1LastLog)
	{
		Analizza1LastLog = tmp;
		*max_log = tmp;
	}
}

<SERIALOG>^Log[ ][0-9]{1,10}[ \t]*$	{
	int tmp = atoi(yytext + 4);
	
	if (tmp == Analizza1LastLog)
	{
		BEGIN(FINALOG);
	}
}

<FINALOG>^[0-9]{1,2}[ \t][0-9]{1,10}[ \t]*$	{
	SerialCouple tmp1;
	tmp1.drv_num = -1;
	tmp1.serial_number = -1;
	unsigned int uli;
	int num_drv = atoi(yytext);
	tmp1.drv_num = num_drv;
	char* mypunt = FindPointer(yytext);
	uli = strtoul(mypunt,NULL,10);
	tmp1.serial_number=uli;
	serial.push_back(tmp1);
}

<*>\n
<*>.

%%

int yywrap(void) {return 1;}

void Analizza1(vector<SerialCouple>& serial_list, int* max_log) //max_log here is not very useful....
{
	 Analizza1LastLog = -1;
     BEGIN(LASTLOG);
     yyin = fopen( SERIALDRVLOG, "r" ); //Hard coding problem!!!!
     
     if (yyin != NULL)
     {
		 yylex(serial_list, max_log);
		 rewind(yyin);
		 BEGIN(SERIALOG);
		 
		 yylex(serial_list, max_log);
	 }
}


