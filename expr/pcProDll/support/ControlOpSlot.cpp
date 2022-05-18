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
// $Id: ControlOpSlot.cc,v 1.1 1998/03/21 01:16:09 ahalambi Exp $
//
// File:  		ControlOpSlot.cc
// Created:		Wed Mar 11, 98
// Last modified: 	Wed Mar 11, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "ControlOpSlot.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


//GG: set this to 1, to generate VLIW-like code, with the NOPs explicitly in the code
int vliwASMGEN=0;

void ControlOpSlot::addOperation(BaseOperation *someOperation, bool noCopy)
{
 if (someOperation == _NULL_)
 {
	 // throw an exception.
  
  return;
 }
 
 if (noCopy)
  _controlOpPtr = someOperation;
 else
  _controlOpPtr = someOperation->copy();
}

void ControlOpSlot::deleteOperation()
{
 delete _controlOpPtr;
 _controlOpPtr = _NULL_;
}

int ControlOpSlot::hasOper()
{
 if (_controlOpPtr != _NULL_) return _YES_;
 
 return _NO_;
}

const int operator == (const ControlOpSlot & a, const ControlOpSlot & b)
{
 // perform the required comparisions.
 // As of now, just returning _NO_
 
 return ( a.getPtrToOperation() == b.getPtrToOperation()
  &&  a.getPtrToTrueBranch() == b.getPtrToTrueBranch()
  &&  a.getPtrToFalseBranch() == b.getPtrToFalseBranch() );
}

ostream & operator << (ostream & out, const ControlOpSlot & s)
{
 if (s._controlOpPtr != _NULL_)
  s._controlOpPtr->print();
 return out;
}

void ControlOpSlot::print(ostream& out)
{
 if (_controlOpPtr != _NULL_)
  _controlOpPtr->print(out);
}  

void ControlOpSlot::shortPrint(ostream& out)
{
 if (_controlOpPtr != _NULL_)
 {
	 out << "  ";
  _controlOpPtr->shortPrint(out);
  //NOSTEMP
	 out << "  (* " << _controlOpPtr << " *)";
 }
}  

void ControlOpSlot::printProperties(ostream & out, PropNameList & pL)
{
 if (_controlOpPtr != _NULL_)
 {
	 out << "  ";
	 _controlOpPtr->printProperties(out, pL);
 }
}

BaseOpSlot *ControlOpSlot::copy()
{
 BaseOpSlot *retPtr;
 
 retPtr = new ControlOpSlot(*this);
 
 return retPtr;
}

void ControlOpSlot::printASMInstr(ostream& out)
{
 if (_controlOpPtr != _NULL_)
 {
  _controlOpPtr->printASMInstr(out);
 }
 else{
	 if(vliwASMGEN){
		//GG: modified to print out also the NOPs, for VLIW-like ASM
		out << "\t4\tnop\t()\t()\n";
	 }
 }
}  
