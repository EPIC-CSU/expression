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
// $Id: ControlOpSlot.h,v 1.11 1998/03/21 01:18:37 ahalambi Exp $
//
// File:  		ControlOpSlot.h
// Created:		Fri Nov 22, 96
// Last modified:       Fri Nov 22, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION INSTRUCTION

#ifndef __CONTROLOPSLOT_H__
#define __CONTROLOPSLOT_H__

#include "BaseOpSlot.h"
#include "ControlOp.h"

// Note that the copy constructor makes a copy of the
// operation being pointed to.
// Also, the destructor destroys the operation which is contained in
// the slot.

class ControlOpSlot : public BaseOpSlot
{
private:
protected:
 
 BaseOpSlot *_trueBranch;
 BaseOpSlot *_falseBranch;
 
 BaseOperation *_controlOpPtr;
 //
 // See DataOpSlot.h for an explantion as to why BaseOperation * 
 // (and not ControlOp *) is being used.
 
public:
 
 ControlOpSlot()
 {
	 _trueBranch = _NULL_;
	 _falseBranch = _NULL_;
  
	 _controlOpPtr = _NULL_;
 }
 
 ControlOpSlot(const ControlOpSlot & someControlOpSlot)
 {
	 _trueBranch = someControlOpSlot._trueBranch;
	 _falseBranch = someControlOpSlot._falseBranch;
	 
	 if (someControlOpSlot._controlOpPtr != _NULL_)
  {
   _controlOpPtr = someControlOpSlot._controlOpPtr->copy();
  }
	 else
  {
   _controlOpPtr = _NULL_;
  }
 }
 
 virtual ~ControlOpSlot()
 {
	 delete _controlOpPtr;
 }
 
 int isControlOpSlot(void)
 {
	 return _YES_;
 }
 
 void addOperation(BaseOperation *someOperation, bool noCopy = false);
 void deleteOperation();

 void setOperation(BaseOperation *someOperation)
 {
	 if (someOperation == _NULL_){
		 return;
	 }
 	 
	 _controlOpPtr = someOperation;
 }
 
 BaseOperation *getPtrToOperation() const
 {
	 return _controlOpPtr;
 }
 
 void setTrueBranch(BaseOpSlot *trueSlotPtr)
 {
	 _trueBranch = trueSlotPtr;
 }
 
 void setFalseBranch(BaseOpSlot *falseSlotPtr)
 {
	 _falseBranch = falseSlotPtr;
 }
 
 // The next slot is assumed to be the default slot.
 // In this case, it is the fall thru (i.e. false) branch.
 //
 void setNext(BaseOpSlot *slotPtr)
 {
	 _falseBranch = slotPtr;
 }

 BaseOpSlot *getPtrtoNextSlot() const
 {
	 return _falseBranch;
 }

 BaseOpSlot *getPtrToTrueBranch() const
 {
	 return _trueBranch;
 }
 
 BaseOpSlot *getPtrToFalseBranch() const
 {
	 return _falseBranch;
 }
 
 BaseOpSlot *copy();
 
 int hasOper();
 
 friend const int operator == (const ControlOpSlot & a, const ControlOpSlot & b);
 friend ostream & operator << (ostream & out, const ControlOpSlot & s);
 
 void print(ostream& out);
 void shortPrint(ostream& out);
 void printASMInstr(ostream&);  // for ASM
 
 void printProperties(ostream & out, PropNameList & pL);

 void resetOperation()
 {
	 _controlOpPtr = _NULL_;
 } 

};

#endif
