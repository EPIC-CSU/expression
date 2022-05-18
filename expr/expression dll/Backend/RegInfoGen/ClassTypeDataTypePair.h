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
/*************************************************************
*file  :		ClassTypeDataTypePair.h						 *
*authors :		Aviral Shrivastava                           *
*last modified : 09/10/2001                                  *
*                                                            *
*contents:		Register Information Generation Header		 *
*comments:-                                                  *
*compiled with : g++                                         *
*for more info : aviral@ics.uci.edu                          *
**************************************************************/

#ifndef _CLASS_DATA_TYPE_PAIR_H_
#define _CLASS_DATA_TYPE_PAIR_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <list>
#include <vector>
#include "MyString.h"
#include "node.h"


class ClassTypeDataTypePair
{
	MyString _targetClassType;
	MyString _targetDataType;

public:

	ClassTypeDataTypePair(void)
	{}

	// The cdPair constructor with one operand - targetDataType
	ClassTypeDataTypePair(char *targetDataType)
	{
		_targetDataType = targetDataType;
	}

	ClassTypeDataTypePair(MyString targetClassType, MyString targetDataType)
	{
		_targetDataType = targetDataType;
		_targetClassType = targetClassType;
	}

	~ClassTypeDataTypePair(void)
	{}

	MyString TargetClassType(void)
	{
		return _targetClassType;
	}

	void TargetClassType(MyString targetClassType)
	{
		_targetClassType = targetClassType;
	}

	void TargetClassType(char *targetClassType)
	{
		_targetClassType = targetClassType;
	}

	MyString TargetDataType(void)
	{
		return _targetDataType;
	}

	void TargetDataType(char *targetDataType)
	{
		_targetDataType = targetDataType;
	}

	void TargetDataType(MyString targetDataType)
	{
		_targetDataType = targetDataType;
	}

	ClassTypeDataTypePair copy()
	{
		ClassTypeDataTypePair newCDPair;
		newCDPair.TargetClassType(_targetClassType);
		newCDPair.TargetDataType(_targetDataType);
		return newCDPair;
	}

	void Print()
	{
		printf("<");
		printf("targetClassType = %s", _targetClassType.getStr());
		printf("\ttargetDataType = %s", _targetDataType.getStr());
		printf(">\n");
	}


	int PrintForTargetRegClassToRegsMapping(FILE *fptr)
	{
		/*fprintf(fptr, "(DATA_TYPE ");
		fprintf(fptr, "%s", _targetDataType.getStr());
		fprintf(fptr, ") ");
		
		fprintf(fptr, "(CLASS_TYPE ");
		fprintf(fptr, "%s", _targetClassType.getStr());
		fprintf(fptr, ")");*/
		
	}

	//  ATRI MANDAL 
	MyString CollectDataForTargetRegClasses()
	{
		//fprintf(fptr, "Data=%s\n", _targetDataType.getStr());
		return(_targetDataType);

	}

	MyString CollectClassForTargetRegClasses()
	{
	    //fprintf(fptr, "Target=%s\n", _targetClassType.getStr());
		return(_targetClassType);
	}

	void PrintForRegClassToRegClassMapping(FILE *fptr, MyString genericDataType, MyString genericClassType)
	{
		fprintf(fptr, " (GENERIC  ");

		fprintf(fptr, "(DATA_TYPE ");
		fprintf(fptr, "%s", genericDataType.getStr());
		fprintf(fptr, ") ");

		fprintf(fptr, "(CLASS_TYPE ");
		fprintf(fptr, "%s", genericClassType.getStr());
		fprintf(fptr, ") ");

		fprintf(fptr, ") ");

		fprintf(fptr, "(TARGET  ");

		fprintf(fptr, "(DATA_TYPE ");
		fprintf(fptr, "%s", _targetDataType.getStr());
		fprintf(fptr, ") ");
		
		fprintf(fptr, "(CLASS_TYPE ");
		fprintf(fptr, "%s", _targetClassType.getStr());
		fprintf(fptr, ") ");

		fprintf(fptr, ")");

		fprintf(fptr, "\n");
	}
};

#ifdef  __UNIX__
typedef list<ClassTypeDataTypePair> ClassTypeDataTypePairList;
#else
typedef std::list<ClassTypeDataTypePair> ClassTypeDataTypePairList;
#endif

#ifdef  __UNIX__
typedef vector<ClassTypeDataTypePairList> ClassTypeDataTypePairListVector;
#else
typedef std::vector<ClassTypeDataTypePairList> ClassTypeDataTypePairListVector;
#endif


#endif