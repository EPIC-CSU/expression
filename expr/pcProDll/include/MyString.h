/*****************************************************************\
*                                                                *
*  Copyright (C) Regents of University of California, 2003       *
*  This source code is a part of EXPRESSION project and is       *
*  copyrighted by the Regents of the University of California,   *
*  Irvine.                                                       *
*  The authors hereby grant permission to use this description   *
*  and its documentation for educational and non-commercial      *
*  purpose under the BSD license                                 *
*  (http://www.cecs.uci.edu/~express/BSD_License.txt). 	         *
*  The source code comes with no warranty and no author or       *
*  distributor accepts any responsibility for the consequences   *
*  of its use. Permission is granted to distribute this file in  *
*  compiled or executable form under the same conditions that    *
*  apply for source code. Permission is granted	to anyone to     *
*  make or distribute copies of this source code, either as      *
*  received or modified, in any medium, provided that all        *
*  copyright notices, permission and non warranty notices are    *
*  preserved, and that the distributor grants the recipient      *
*  permission for further redistribution as permitted by this    *
*  document. No written agreement, license, or royalty fee is    *
*  required for authorized use of this software.                 *
*                                                                *
*******************************************************************/
// $Id$
//
// File:  		MyString.h
// Created:		Mon Jan 11, 99
// Last modified: 	Mon Jan 11, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __MYSTRING_H__
#define __MYSTRING_H__

#include <stdlib.h>
#include "GlobalIncludes.h"

const int MIN_INCREMENT = 80;

class MyString
{
private:
	char *_str;
	int _cap; // current capacity.
public:
	MyString() : _str(NULL), _cap(0) {}
	MyString(char *s)
	{
		if (s == NULL)
		{
			_str = NULL;
			_cap = 0;
		}
		else
		{
			_str = strdup(s);
			_cap = strlen(s);
		}
	}

	MyString(const MyString & s)
	{
		if (s._str == NULL)
		{
			_str = NULL;
			_cap = 0;
		}
		else
		{
			_str = strdup(s._str);
			_cap = strlen(_str);
		}
	}

	~MyString()
	{
		if (_str != NULL)
		  //			delete _str;
		  free(_str);
	}

	void operator = (const char *s)
	{
		if (_str != NULL) 
		  //		  delete _str;
		  free(_str);

		if (s == NULL)
		{
			_str = NULL;
			_cap = 0;
		}
		else
		{
			_str = strdup(s);
			_cap = strlen(s);
		}
	}

	void operator = (const MyString & s)
	{
		if (_str != NULL) 
		  //delete _str;
		  free(_str);
		if (s._str == NULL)
		{
			_str = NULL;
			_cap = 0;
		}
		else
		{
			_str = strdup(s._str);
			_cap = strlen(_str);
		}
	}

	void capitalize() 
	{
		char temp[50];
		strcpy(temp, _str);
		for(int i=0;i<_cap;i++)
			toupper(temp[i]);
		strcpy(_str, temp);
	}

	char *getStr() { return _str; }

	int getStrLen() { return ((_str == NULL) ? 0 : strlen(_str)); }

	void concat(char *s)
	{
		if (s == NULL) return;

		char *a;
		int temp = 0;
		int didMalloc = 0;

		if (_str == NULL)
			temp = strlen(s);
		else
			temp = strlen(_str)+strlen(s);

		if (_cap <= temp)
		{
			a = (char *) malloc(temp + MIN_INCREMENT);
			didMalloc = 1;
			_cap = temp + MIN_INCREMENT;

			if (_str != NULL)
			{
				strcpy(a, _str);
				strcat(a, s);

				free(_str);
			}
			else
			{
				strcpy(a, s);
			}

		}
		else
		{
			a = _str;
			strcat(a, s);
		}
		_str = a;
	}

	int isEmpty()
	{
		if (_str == NULL) return _YES_;

		return _NO_;
	}

	void convertToCSyntax()
	{
		// go thru the string and convert all single (single quote or double quote) to 2 of the same kind.
		//
		char *tmp = (char *)malloc(strlen(_str) + MIN_INCREMENT);

		unsigned int i, j;
		for (i = j = 0; i <= strlen(_str); i++, j++)
		{
			tmp[j] = _str[i];
			if ((_str[i] == '\'') || (_str[i] == '\"'))
			{
				j++;
				tmp[j] = _str[i];
			}
		}
		_cap = strlen(_str) + MIN_INCREMENT;
		free(_str);
		_str = tmp;
	}

	void print(ostream & out)
	{
		out << _str;
	}
		
	bool friend operator < (const MyString & a, const char *s)
	{
		if (strcmp(a._str, s) < 0) return true;
		return false;
	}

	bool friend operator == (const MyString & a, char *s)
	{
		if (strcmp(a._str, s) == 0) return true;
		return false;
	}

	bool friend operator < (const MyString & a, const MyString & b)
	{
		if (strcmp(a._str, b._str) < 0) return true;
		return false;
	}

	bool friend operator == (const MyString & a, const MyString & s)
	{
		if (strcmp(a._str, s._str) == 0) return true;
		return false;
	}
	bool friend operator != (const MyString & a, const char *s)
	{
		if (strcmp(a._str, s) == 0) return false;
		return true;
	}
	bool friend operator != (const MyString & a, const MyString & b)
	{
		if (strcmp(a._str, b._str) == 0) return false;
		return true;
	}

	
};

#endif
