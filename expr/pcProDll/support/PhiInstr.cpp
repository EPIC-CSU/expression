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
// $Id: PhiInstr.cc,v 1.8 1998/01/22 23:46:28 pgrun Exp $
//
// File:  		PhiInstr.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#include "stdafx.h"

#include "PhiInstr.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_PHIINSTRS
int PhiInstr::_numPhiInstrs = 0;	
#endif

extern int numFlowOpsPerInstr;

BaseInstruction *PhiInstr::getDefaultTargetInstr()
{
 BaseOperation *tempOp;
 
 if ((tempOp = _flowOpList.elementAt(numFlowOpsPerInstr - 1).getPtrToOperation()) == NULL) 
  // This could mean that the default next instr
  // has not been set yet, or that it is the last
  // instr.
  assert(0);
 
 if (!tempOp->isFlowOp()) 
  assert(0);
 return (((FlowOp *)tempOp)->getTargetInstr());
}

int PhiInstr::addFlowOpToList(FlowOp *someOpPtr, int pos)
{
 if (pos == _INVALID_) {
  ArrayIterator<FlowOpSlot> *fIter;
  fIter = _flowOpList.elements();
  while (fIter->hasMoreElements()) {
   if ((fIter->currentElement()).hasOper() == _NO_) {
    (fIter->currentElement()).addOperation(someOpPtr);
    return fIter->currentIndex();
   }
   fIter->nextElement();
  }
  delete fIter;
 }
 else {
  _flowOpList.elementAt(pos).addOperation(someOpPtr);
  return pos;
 }
 return _INVALID_;
}

BaseInstruction *PhiInstr::getFirstSucc()
{
 return getDefaultTargetInstr();
}


void PhiInstr::setNext(BaseInstruction *next)
{
 GotoOp tempGotoOp;
 LabelArg tempLabelArg;
 tempLabelArg.initLabel("ForPhi");
 tempGotoOp.initOpCode(GOTO_OP);
 tempGotoOp.addLabelArg(&tempLabelArg);
 tempGotoOp.setTargetInstr(next);
 addFlowOpToList(&tempGotoOp,numFlowOpsPerInstr-1);
}


void PhiInstr::print(ostream& out)
{
 MyLinkedListIterator<PhiOp *> *printIter;
 printIter = (_phiOps).elementsForward();
 
 out << "Phi Instr: " << endl;
 while (printIter->hasMoreElements())
 {
  printIter->currentElement()->print(out);
  printIter->nextElement();
 }
 out<<"tgt:"<<getDefaultTargetInstr();
 
 delete printIter;
}

void PhiInstr::shortPrint(ostream& out)
{
 MyLinkedListIterator<PhiOp *> *printIter;
 printIter = (_phiOps).elementsForward();
 
 out << "Phi Instr: " << endl;
 while (printIter->hasMoreElements())
 {
  printIter->currentElement()->shortPrint(out);
  printIter->nextElement();
 }
 
 delete printIter;
}


BaseInstruction *PhiInstr::copy()
{
 BaseInstruction *retPtr;
 
 retPtr = new PhiInstr;
 //
 // As of now there is nothing in here to initialize
 
 return retPtr;
}
