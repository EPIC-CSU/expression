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

#include <string.h>
#include <list>
#include "MyString.h"
#include "node.h"


class ClassTypeDataTypePair
{
	MyString _targetClassType;
	MyString _targetDataType;

public:

	ClassTypeDataTypePair(void)
	{}

	~ClassTypeDataTypePair(void)
	{}

	TargetTypeClassType(void)
	{
		return _targetClassType;
	}

	TargetTypeDataType(void)
	{
		return _targetDataType;
	}

};

#ifdef  __UNIX__
typedef list<ClassTypeDataTypePair> ClassTypeDataTypePairList;
#else
typedef std::list<ClassTypeDataTypePair> ClassTypeDataTypePairList;
#endif

#endif