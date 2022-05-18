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
#include "stdafx.h"

#include "OpCode.h"
#include <string.h>
#include <ctype.h>

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 



 OpCode *OpCode::copy()
 {
	 OpCode *retPtr;
	 
	 retPtr = new OpCode(_opCodeIndex);
	 
	 return retPtr;
 }

void OpCode::printC() 
{
  char *opName;
  int opNameLen;
  char *lowerCaseName;

  opName = globalOpCodeTable[_opCodeIndex].opName();
  opNameLen = strlen(opName);
  lowerCaseName = new char[opNameLen + 1];

  for (int i = 0; i < opNameLen; i++)
    lowerCaseName[i] = tolower(opName[i]);

  lowerCaseName[opNameLen] = '\0';

  cout << lowerCaseName;

  delete lowerCaseName;
}

  
// Note: This function is TBD.
//
int OpCode::isDoubleTypeOpcode()
{
	assert(0);

	// First, check to see if the opcode is a generic or a target specific opcode.
	//
	// Currently, if the opcode is target specific, we assume that it is not of double type.
	//
	if (!(globalOpCodeTable[_opCodeIndex]).isGeneric()) return _NO_;

//	if (globalOpCodeTable.isDataType(_opCodeIndex, _DOUBLE_)) return _NO_;

	return _NO_;
}

  
