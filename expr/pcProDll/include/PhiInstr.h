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
// $Id: PhiInstr.h,v 1.13 1998/02/27 22:54:27 xji Exp $
//
// File:  		phiInstr.h
// Created:		Fri Nov 08, 96
// Last modified: 	Thu Nov 14, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#ifndef __PHIINSTR_H__
#define __PHIINSTR_H__

#include "BaseInstruction.h"
#include "PhiOp.h"
#include "Array.h"
#include "FlowOpSlot.h"
#include "GotoOp.h"

extern int numFlowOpsPerInstr;

class PhiInstr : public BaseInstruction
{
private:
 
#ifdef COUNT_PHIINSTRS
 static int _numPhiInstrs;	
 //
 // _numPhiInstrs is used to keep a count of the number of
 // PhiInstr objects in existence at any point. COUNT_PHIINSTRS can
 // be either defined or undefined in the file "InstructionIncludes.h".
 // The count information will be used for debugging (and/or
 // statistical) purposes. It might be a good idea to allow the user to
 // specify whether he needs this info.
 //
#endif
 
protected:
 MyLinkedList<PhiOp *> _phiOps;
 Array<FlowOpSlot> _flowOpList;
 
public:
 PhiInstr() : _flowOpList(numFlowOpsPerInstr)
 {
#ifdef COUNT_PHIINSTRS
	 _numPhiInstrs++;	
#endif
  FlowOpSlot fc;
  for (int i = 0; i < numFlowOpsPerInstr; i++)
  {
   _flowOpList.insertElement(fc);
  }
 }
 
 ~PhiInstr()
 {
#ifdef COUNT_PHIINSTRS
	 _numPhiInstrs--;
#endif
  MyLinkedListIterator<PhiOp *> *iter = _phiOps.elementsForward();
  while (iter->hasMoreElements()) {
   PhiOp *operation = iter->currentElement();
   delete operation;
   iter->nextElement();
  }
  delete iter;
  
 }    
 
 int addFlowOpToList(FlowOp *someOpPtr, int pos = _INVALID_);
 
 BaseOpSlot & getFlowOpSlot(int index)
 {
  return _flowOpList.elementAt(index);
 }
 
 ArrayIterator<FlowOpSlot> *getFlowOpSlotIter()
 {
  ArrayIterator<FlowOpSlot> *retPtr;
  retPtr = _flowOpList.elements();
  
  return retPtr;
 }
 
 BaseInstruction *getFirstSucc();
 
 BaseInstruction *getNextSucc()
 {
  return NULL;
 }
 
 void setNext(BaseInstruction *next);
 
 BaseInstruction *getDefaultTargetInstr();
 
 void addPhiOp(PhiOp *operation)
 {
  _phiOps.append(operation);
 }
 
 void removePhiOp(PhiOp* operation) {
  _phiOps.remove(operation);
 }
 
 
 MyLinkedListIterator<PhiOp *> *PhiOpsForwardIterator()
 {
  MyLinkedListIterator<PhiOp *> *iter = _phiOps.elementsForward();
  return iter;
 }
 
#ifdef COUNT_PHIINSTRS
 static int numPhiInstrs()
 {
	 return _numPhiInstrs;
 }
#endif
 
 void initializeID()
 {
 }
 
 BaseInstruction *copy();
 
 int isPhiInstr()
 {
	 return _YES_;
 }
 
 friend int operator == (const PhiInstr & a, const PhiInstr & b)
 {
	 return _YES_;
 }
 
 void print(ostream& out) ;
 void shortPrint(ostream& out) ;
 
 friend ostream& operator << (ostream & out, PhiInstr & s)
 {
	 MyLinkedListIterator<PhiOp *> *printIter;
	 printIter = (s._phiOps).elementsForward();
	 
	 out << "Printing Phi Instr: " << endl;
	 while (printIter->hasMoreElements())
  {
   printIter->currentElement()->print();
   printIter->nextElement();
  }
	 
	 delete printIter;
	 
	 return out;
 }
 
 MyLinkedList<PhiOp*>* getOpList() {
  return &_phiOps;
 }
 
 //   
 // Later on, the above operators, == and <<, will have to be rewritten
 // when we add member data to this class.
 
 void setTargetInstr(BaseInstruction *instr){
  setNext(instr);
 }
 
 

 void printArgsProperty(ostream & out)
 {
	 out << "Args  Properties:" << endl;
	 MyLinkedListIterator<PhiOp*>* iter = _phiOps.elementsForward();
	 while ( iter->hasMoreElements() ) {
   out << " ------------  op ----------------------- " << endl;
   PhiOp* op = iter->currentElement();
   out << " ------------  dest ----------------------- " << endl;
   (op->ptrToOperand(_DEST_))->print(out);
   MyLinkedListIterator<BaseArgument*>* argIter=op->phiArgIterForw();
   while ( argIter->hasMoreElements() )  {
    out << " ------------  source ----------------------- " << endl;
    argIter->currentElement()->print(out);
    argIter->nextElement();
   }
   delete argIter;
   iter->nextElement();
	 }
	 delete iter;
 }
};

#endif
