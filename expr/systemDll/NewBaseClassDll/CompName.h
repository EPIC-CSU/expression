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
 *file  : CompName.h                                         *
 *authors : ashok				                             *
 *last modified :                                            *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef _COMPNAME_H_
#define _COMPNAME_H_

#include "Unit.h"
#include "Storage.h"
#include "Port.h"
#include "Connection.h"
#include "Latch.h"

#include "..\..\pcProDll\include\MyString.h"

#ifdef __UNIX__
	#define __declspec(dllexport) 
#endif	// of #ifdef __UNIX__

extern Map<Unit *, MyString> globalUnitNameList;
extern Map<Storage *, MyString> globalStorageNameList;
extern Map<Latch *, MyString> globalLatchNameList;
extern Map<Port *, MyString> globalPortNameList;
extern Map<Connection *, MyString> globalConnectionNameList;

extern void addName(Unit *comp, MyString & str);
extern void addName(Latch *comp, MyString & str);
extern void addName(Storage *comp, MyString & str);
extern void addName(Port *comp, MyString & str);
extern void addName(Connection *comp, MyString & str);

extern void addName(BaseSimObject *comp, MyString & str);

extern MyString & getName(Unit *comp);
extern MyString & getName(Latch *comp);
extern MyString & getName(Storage *comp);
extern MyString & getName(Port *comp);
extern MyString & getName(Connection *comp);

extern MyString & getName(BaseSimObject *comp);

extern BaseSimObject *getComp(MyString & str);
extern __declspec( dllexport ) BaseSimObject *getComp(char* testStr);

extern void printName(ostream & out, Unit *comp);
extern void printName(ostream & out, Latch *comp);
extern void printName(ostream & out, Storage *comp);
extern void printName(ostream & out, Port *comp);
extern void printName(ostream & out, Connection *comp);

extern void printName(ostream & out, BaseSimObject *comp);

#endif
