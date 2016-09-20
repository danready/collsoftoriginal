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

#include "Utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <regex.h>
#include <stdbool.h>

using std::string;

//The FindPointer function returns a pointer to the word that follow the initial one.
//Ex. : FindPointer("moveto 1000") returns a pointer to 1 
char * FindPointer (char *yytext)
{
	int i = 0;
	char *punt;
	while (yytext[i] != ' ' && yytext[i] != '\t' && i<strlen(yytext)-1)
		i++;
	while ((yytext[i] == ' ' || yytext[i] == '\t') && i<strlen(yytext)-1)
		i++;
	punt = yytext + i;
	return punt;
}

//The FindIntegerValue function returns the value of the integer that follow the initial word of the string yytext.
//Ex. : FindIntegerValue("moveto 1000") returns 1000
int FindIntegerValue (char *yytext)
{
	char *punt;
	int integer_value;
	punt = FindPointer (yytext);
	integer_value = atoi (punt);
	return integer_value;
}

//The reg_matches function tries to find "pattern" in the string pointed by "str".
//It returns true if the string is found or returns false if the string is not found.
bool reg_matches (const char *str, const char *pattern)
{
	regex_t re;
	int ret;

	if (regcomp (&re, pattern, REG_EXTENDED) != 0)
		return false;

	ret = regexec (&re, str, (size_t) 0, NULL, 0);
	regfree (&re);

	if (ret == 0)
		return true;

	return false;
}

//The function GetTime returns a string that represent the current date.
string GetTime()
{
	char* ts = (char*)malloc(100*sizeof(char));
	time_t t;
	t = time (NULL);
	ctime_r (&t, ts);
	ts[strlen (ts) - 1] = '\0';
	
	string tmp_string = ts;

	free(ts);
	
	return tmp_string;		
}
