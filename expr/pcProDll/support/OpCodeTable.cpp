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
// $Id: OpCodeTable.cc,v 1.4 1998/01/22 23:46:26 pgrun Exp $
//
// File:  		CallOpCodetable.cc
// Created:		Sun Nov 24, 96
// Last modified: 	Sun Nov 24, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: 

#include "stdafx.h"

#include "OpCodeTable.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//int numOfOpCodes = 200;
//
// Make this user settable.

// Modified by AVS on 5-24-2001
int numOfOpCodes = 300;


Array<OpCodeTableEntry> OpCodeTable::_opCodeTable(numOfOpCodes);

OpCodeTable globalOpCodeTable;

OpCodeTableEntry::OpCodeTableEntry(const OpCodeTableEntry & s)
{
 if (s._opName != NULL)
 {
  _opName = new char[strlen(s._opName) +1];
  strcpy(_opName, s._opName);
 }
 
 _genericOrTarget = s._genericOrTarget;

 _opType = s._opType;

 if(s._opSemantic != NULL)
 {
  _opSemantic = new char[strlen(s._opSemantic)+1];
  strcpy(_opSemantic, s._opSemantic);
 }
 else _opSemantic=NULL;
 _genericOpCodeEnum = s._genericOpCodeEnum;
 //
 // later on, check to see if the type is valid.
}



OpCodeTableEntry::OpCodeTableEntry(char *name, int genericOrTarget, int type)
{
 if (name != NULL)
 {
  _opName = new char[strlen(name) +1];
  strcpy(_opName, name);
 }
 
 _genericOrTarget = genericOrTarget;

 _opType = type;

 _opSemantic = NULL;
 //
 // later on, check to see if the type is valid.
 
}

OpCodeTableEntry::OpCodeTableEntry(char *name, int genericOrTarget, int type, char *sem)
{
 if (name != NULL)
 {
  _opName = new char[strlen(name) +1];
  strcpy(_opName, name);
 }
 
 _opType = type;

 _genericOrTarget = genericOrTarget;
 
 if(sem!=NULL){
  _opSemantic=new char[strlen(sem)+1];
  strcpy(_opSemantic,sem);
 }
}

OpCodeTableEntry::OpCodeTableEntry(char *name, int genericOrTarget, int type,
				   EnumGenericOpCode genericOpCodeEnum)
{
 if (name != NULL)
 {
  _opName = new char[strlen(name) +1];
  strcpy(_opName, name);
 }
 
	_genericOrTarget = genericOrTarget;
 _opType = type;
 _opSemantic = NULL;
 _genericOpCodeEnum = genericOpCodeEnum;
 //
 // later on, check to see if the type is valid.
 
}

OpCodeTableEntry::OpCodeTableEntry(char *name, int genericOrTarget, int type, char *sem,
				   EnumGenericOpCode genericOpCodeEnum)
{
 if (name != NULL)
 {
  _opName = new char[strlen(name) +1];
  strcpy(_opName, name);
 }

	_genericOrTarget = genericOrTarget;
 _opType = type;

 if(sem!=NULL){
  _opSemantic=new char[strlen(sem)+1];
  strcpy(_opSemantic,sem);
 }
 _genericOpCodeEnum = genericOpCodeEnum;
}
