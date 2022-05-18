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
// $Id: JumpOp.cc,v 1.4 1998/03/21 01:16:29 ahalambi Exp $
//
// File:  		JumpOp.h
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#include "stdafx.h"

#include "JumpOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_JUMPOPS
int JumpOp::_numJumpOps = 0;	
#endif

// The initializeID method will have to be redefined here.

void JumpOp::initializeID()
{
 _uniqueID = 0;
}

BaseOperation *JumpOp::copyBody(bool copyProp)
{
 BaseOperation *retPtr;
 retPtr = new JumpOp(*this);
 
 if (copyProp)
 {
  // Currently, we do not copy any properties of the argument.
  /*
  MyLinkedList<PropertyNames> propList;
  propList.appendElement(<insert_property_name_here>
  
    (retPtr->getPropertyList()).makeACopyOf(_propertyPtr, propList);
  */
 }
 
 return retPtr;
}

int JumpOp::isEq(BaseOperation *op)
{
	if (op == NULL) 
		return _NO_;

	if (!op->isJumpOp())
		return _NO_;
	
	if (!NormalOp::isEq(op))
		return _NO_;

	if (_targetInstr != ((JumpOp *)op)->_targetInstr)
		return _NO_;

	return _YES_;
}

void JumpOp::print()
{
 cout << "\t Tgt: " << _targetInstr;
 cout << endl;
 printOpProperty(cout);
}

void JumpOp::print(ostream& out)
{
 out << " Tgt: " << _targetInstr;
 out << endl;
}

void JumpOp::printToString(char* buf)
{
 MyString str;
 getOpcodeName(str);
 strcat(buf,str.getStr());
}

void JumpOp::print(int indent)
{
 cout << "(* " << _targetInstr << " *)";
}

void JumpOp::printC(int indent)
{
  // cout << "(* " << _targetInstr << " *)";
}

void JumpOp::shortPrint(ostream& out)
{
 out << _targetInstr;
}

void JumpOp::printProperties(ostream& out, PropNameList & pL)
{
 out << _targetInstr;
 BaseOperation::printProperties(out, pL);
}

int operator == (const JumpOp & a, const JumpOp & b)
{
 return _YES_;
}

ostream& operator << (ostream & out, const JumpOp & s)
{
 return out;
}
//   
// Later on, the above operators, == and <<, will have to be rewritten.


BaseOperation *JumpOp::copy()
{
 BaseOperation *retPtr;
 
 retPtr = new JumpOp(*this);
 
 return retPtr;
}

void JumpOp::printASMInstr(ostream& out)  // for ASM
{
	//out << "inside JumpOp " << endl;
	out << _targetInstr;
	//setPrintOrderAndFormatStr(opCode, dstVector, srcVector, printOrder, formatStr);
	
	//out << "----------------------------------------------------" << endl;	
 
}