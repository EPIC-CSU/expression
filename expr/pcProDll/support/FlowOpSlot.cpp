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
// $Id: FlowOpSlot.cc,v 1.1 1998/03/21 01:16:12 ahalambi Exp $
//
// File:  		FlowOpSlot.cc
// Created:		Wed Mar 11, 98
// Last modified: 	Wed Mar 11, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "FlowOpSlot.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

FlowOpSlot::FlowOpSlot(const FlowOpSlot & someFlowOpSlot)
{
 if (someFlowOpSlot._flowOpPtr != _NULL_)
 {
	 _flowOpPtr = someFlowOpSlot._flowOpPtr->copy();
 }
 else
 {
	 _flowOpPtr = _NULL_;
 }
}

void FlowOpSlot::addOperation(BaseOperation *someOperation, bool noCopy)
{
 if (someOperation == _NULL_)
 {
	 // throw an exception.
  
  return;
 }
 
 if (noCopy)
  _flowOpPtr = someOperation;
 else
  _flowOpPtr = someOperation->copy();
}

void FlowOpSlot::changeOperation(BaseOperation *someOp)
{
 _flowOpPtr = someOp;
}

void FlowOpSlot::deleteOperation()
{
 delete _flowOpPtr;
 _flowOpPtr = _NULL_;
}

int FlowOpSlot::hasOper()
{
 if (_flowOpPtr != _NULL_) return _YES_;
 
 return _NO_;
}

const int operator == (const FlowOpSlot & a, const FlowOpSlot & b)
{
 // perform the required comparisions.
 // As of now, just returning _NO_
 
 return (a.getPtrToOperation() == b.getPtrToOperation() );
}

ostream & operator << (ostream & out, const FlowOpSlot & s)
{
 if (s._flowOpPtr != _NULL_)
  s._flowOpPtr->print();
 return out;
}

void FlowOpSlot::print(ostream& out)
{
 if (_flowOpPtr != _NULL_)
  _flowOpPtr->print(out);
}

void FlowOpSlot::shortPrint(ostream& out)
{
 if (_flowOpPtr != _NULL_)
 {
	 out << "  ";
  _flowOpPtr->shortPrint(out);
 }
}

void printC(int indent) {
}

void FlowOpSlot::printProperties(ostream & out, PropNameList & pL)
{
 if (_flowOpPtr != _NULL_)
 {
	 out << "  ";
	 _flowOpPtr->printProperties(out, pL);
 }
}

BaseOpSlot *FlowOpSlot::copy()
{
 BaseOpSlot *retPtr;
 
 retPtr = new FlowOpSlot(*this);
 
 return retPtr;
}

