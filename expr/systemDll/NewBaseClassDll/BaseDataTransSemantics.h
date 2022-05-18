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
 *file  : BaseDataTransSemantics.h                           *
 *authors : ahalambi                                         *
 *last modified :			                                 *
 *                                                           *
 *contents:												     *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef __BASEDATATRANSSEMANTICS_H__
#define __BASEDATATRANSSEMANTICS_H__

#include "ArchClassIncludes.h"
#include "AddrData.h"
#include "Connection.h"

class BaseDataTransSemantics
{
 public:
	 virtual SignalClass operator()()
	 {
		 // Error: this function should never be called.
		 cout << "Error: Base class method (BaseDataTransSemantics::operator()) should not be called" << endl;
		 return (SignalClass(-1));
	 }
	 
	 virtual SignalClass operator()(Val & a, AddrOperand & b, Connection *conn)
	 {
		 // Error: this function should never be called.
		 cout << "Error: Base class method (BaseDataTransSemantics::operator(Val, AddrOperand, Connection)) should not be called" << endl;
		 return (SignalClass(-1));
	 }   
};

#endif