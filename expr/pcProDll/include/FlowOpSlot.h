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
// $Id: FlowOpSlot.h,v 1.13 1998/03/21 01:18:43 ahalambi Exp $
//
// File:  		FlowOpSlot.h
// Created:		Fri Nov 22, 96
// Last modified:       Fri Nov 22, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION INSTRUCTION

#ifndef __FLOWOPSLOT_H__
#define __FLOWOPSLOT_H__

#include "BaseOpSlot.h"
#include "FlowOp.h"

// Note that the copy constructor makes a copy of the
// operation being pointed to.
// Also, the destructor destroys the operation which is contained in
// the slot.

class FlowOpSlot : public BaseOpSlot
{
private:
protected:
 
 /*   BaseOpSlot *_next; */
 //
 // This is not needed. 
 // It might be a good idea to have a pointer to an instruction tho.
 
 BaseOperation *_flowOpPtr;
 //
 // See DataOpSlot.h for an explanation as to why BaseOperation *
 // (and not FlowOp *) is being used.
 
public:
 
 FlowOpSlot()
 {
	 _flowOpPtr = _NULL_;
 }
 
 FlowOpSlot(const FlowOpSlot & someFlowOpSlot);
 
 virtual ~FlowOpSlot()
 {
	 delete _flowOpPtr;
 }
 
 virtual int isFlowOpSlot(void)
 {
  return 1;
 }
 
 void resetOperation()
 {
	 _flowOpPtr = _NULL_;
 }
 
 void addOperation(BaseOperation *someOperation, bool noCopy = false);
 void changeOperation(BaseOperation *someOp);
 void deleteOperation();
 
 BaseOperation *getPtrToOperation() const
 {
	 return _flowOpPtr;
 }
 
 BaseOpSlot *copy();
 
 int hasOper();
 
 friend const int operator == (const FlowOpSlot & a, const FlowOpSlot & b);
 
 friend ostream & operator << (ostream & out, const FlowOpSlot & s);
 
 void print(ostream& out);
 void shortPrint(ostream& out);
 void printC(int indent);
 
 void printProperties(ostream & out, PropNameList & pL);
};

#endif
