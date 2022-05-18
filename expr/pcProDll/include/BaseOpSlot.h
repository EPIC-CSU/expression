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
// $Id: BaseOpSlot.h,v 1.15 1998/03/09 23:55:53 ahalambi Exp $
//
// File:  		OpSlot.h
// Created:		Fri Nov 22, 96
// Last modified: 	Fri Nov 22, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION INSTRUCTION

#ifndef __OPSLOT_H__
#define __OPSLOT_H__

#include "STLIncludes.h"
#include "OperationIncludes.h"
#include "InstructionIncludes.h"

#include "BaseOperation.h"
#include "PropertyIncludes.h"

class BaseOpSlot
{
 private:
 protected:
 public:
   BaseOpSlot()
      {
      }
   
   virtual ~BaseOpSlot()
      {
      }
   
   virtual void setNext(BaseOpSlot *nextPtr)
      {
      }
   
   virtual void setTrueBranch(BaseOpSlot *truePtr)
      {
      }
   
   virtual void setFalseBranch(BaseOpSlot *falsePtr)
      {
      }
   
   virtual BaseOpSlot *getPtrToNextSlot() const
      {
	   //NOS
	   return NULL;
      }
  
   virtual BaseOpSlot *getPtrToTrueBranch() const
      {
	   //NOS
	   return NULL;
      }
   
   virtual BaseOpSlot *getPtrToFalseBranch() const
      {
	   //NOS
	   return NULL;
      }

   virtual BaseOperation *getPtrToOperation() const
      {
	   //NOS
	   return NULL;
      }

   virtual int hasOper()
      {
	 return _NO_;
      }

   virtual void addOperation(BaseOperation *someOperation, bool noCopy = false)
   {
   }
   
   virtual void setOperation(BaseOperation *someOperation)
   {
   }

   virtual void print(ostream & out)
      {
      }

   virtual void shortPrint(ostream & out)
      {
      }

   virtual void printProperties(ostream & out, PropNameList & pL)
   {
   }

   virtual void deleteOperation() { 
   }

   virtual int isControlOpSlot(void)
      {
	 return 0;
      }
   
   virtual int isFlowOpSlot(void)
     {
       return 0;
     }

   virtual int isDataOpSlot(void)
     {
       return 0;
     }

   friend const int operator == ( const BaseOpSlot & a,  const BaseOpSlot & b)
      {
	 // perform the required comparisions.
         // As of now, just returning _NO_

	 return _NO_;
      }
   friend ostream & operator << (ostream & out, const BaseOpSlot & s)
      {
	 return out;
      }

   BaseInstruction *getInstr(void){
     return (BaseInstruction*)(getPtrToOperation()->getInstr());
   }
};

typedef Vector<BaseOpSlot *> OpSlotList;
typedef OpSlotList::iterator OpSlotListIter;

#endif




