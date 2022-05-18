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
// $Id: DataOpSlot.h,v 1.12 1998/03/21 01:18:39 ahalambi Exp $
//
// File:  		DataOpSlot.h
// Created:		Fri Nov 22, 96
// Last modified:       Fri Nov 22, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION INSTRUCTION

#ifndef __DATAOPSLOT_H__
#define __DATAOPSLOT_H__

#include "BaseOpSlot.h"
#include "DataOp.h"

// Note that the copy constructor makes a copy of the
// operation being pointed to.
// Also, the destructor destroys the operation which is contained in
// the slot.

class DataOpSlot : public BaseOpSlot
{
private:
protected:
 
 BaseOpSlot *_next;
 
 BaseOperation *_dataOpPtr;
 //
 // Although _dataOpPtr is of type BaseOperation *, the only objects
 // that will be attached to it will be objects of classes derived
 // from DataOp.
 // The reason _dataOpPtr is of type BaseOperation * and not 
 // DataOp *, is because the copy method of any object in the Operation
 // hierarchy returns an object (or ptr) of type BaseOperation.
 // See the copy constructor of this class to understand how the copy method
 // is used.
 
public:
 
 DataOpSlot()
 {
	 _next = _NULL_;
	 _dataOpPtr = _NULL_;
 }
 
 DataOpSlot(const DataOpSlot & someDataOpSlot);
 
 virtual ~DataOpSlot();
 
 virtual int isDataOpSlot(void)
 {
  return 1;
 }
 
 void addOperation(BaseOperation *someOperation, bool noCopy = false);
 void deleteOperation();
 
 void setOperation(BaseOperation *someOperation)
 {
	 if (someOperation == _NULL_)
  {
   // throw an exception.
   
   return;
  }
	 
	 _dataOpPtr = someOperation;
 }
 
 void resetOperation()
 {
	 _dataOpPtr = _NULL_;
 }
 
 BaseOperation *getPtrToOperation() const
 {
	 return _dataOpPtr;
 }
 
 void setNext(BaseOpSlot *nextSlotPtr)
 {
	 _next = nextSlotPtr;
 }
 
 BaseOpSlot *getPtrToNextSlot() const
 {
	 return _next;
 }
 
 BaseOpSlot *copy();
 
 int hasOper();
 
 friend const int operator == ( const DataOpSlot & a,  const DataOpSlot & b);
 
 friend ostream & operator << (ostream & out, const DataOpSlot & s);
 
 void print(ostream& out);
 void shortPrint(ostream& out);
 void printASMInstr(ostream&);  // for ASM gen
 void createListOfOperands(Symtab *operandSymTable);

 void printProperties(ostream & out, PropNameList & pL);
};

#endif





