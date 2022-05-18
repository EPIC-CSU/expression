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
// $Id: OpCodeTable.h,v 1.10 1998/03/27 00:01:13 wtang Exp $
//
// File:  		OpCodeTable.h
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARCHITECTURE, include

#ifndef __OPCODETABLE_H__
#define __OPCODETABLE_H__

#include "GlobalIncludes.h"
#include "ArchitectureIncludes.h"
#include "Array.h"
#include "GenericOpCodes.h"
#include "MyString.h"

extern int numOfOpCodes;
//
// Assuming that the number of OpCodes is known in advance.
// See RegisterFileList.h for more explanation.

const int INVALID_OP = -1;
const int NO_OP = 0;
const int IF_OP = 1;
const int JUMP_OP = 2;
const int GOTO_OP = 3;
const int CALLRET_OP = 4;
const int MEMORY_OP = 5;
const int COMPUTE_OP = 6;

const int LOAD_OP = 7;
const int STORE_OP = 8;

const int CALL_OP = 9;
const int RET_OP = 10;
const int IMPLIED_DEST_OP = 11;

class OpCodeTableEntry
{
private:
 char *_opName;
 int _genericOrTarget;
 int _opType;
 char * _opSemantic;
 EnumGenericOpCode _genericOpCodeEnum; // defined in GenericOpCodes.h

public:
 
 OpCodeTableEntry()
 {
	 _opName = NULL;
	 _genericOrTarget = _INVALID_;
	 _opType = INVALID_OP;
	 _opSemantic = NULL;
 }
 
 OpCodeTableEntry(const OpCodeTableEntry & s);
 
 OpCodeTableEntry(char *name, int genericOrTarget = _GENERIC_, int type = INVALID_OP);
 
 OpCodeTableEntry(char *name, int genericOrTarget, int type, char *sem);
 
 OpCodeTableEntry(char *name, int genericOrTarget, int type, EnumGenericOpCode genericOpCodeEnum);
 OpCodeTableEntry(char *name, int genericOrTarget, int type, char *sem, 
		  EnumGenericOpCode genericOpCodeEnum);
 ~OpCodeTableEntry()
 {
	 delete []_opName;
	 delete []_opSemantic;
 }
 
 friend int opCmp(OpCodeTableEntry & s, const char *someName)
 {
	 if (strcmp(s._opName, someName) == 0)
   return _YES_;
	 return _NO_;
 }
 
 int typeOf()
 {
	 return _opType;
 }
 
 int isGeneric()
 {
	 if (_genericOrTarget == _GENERIC_) return _YES_;
	 return _NO_;
 }

 int isTarget()
 {
	 if (_genericOrTarget == _TARGET_) return _YES_;
	 return _NO_;
 }

 char *opName()
 {
	 return _opName;
 }
 
 char *opSemantic()
 {
  return _opSemantic;
 }
 
  EnumGenericOpCode getGenericOpCodeEnum() {
    return _genericOpCodeEnum;
  }
 
 friend int operator == (const OpCodeTableEntry & a, const
  OpCodeTableEntry & b)
 {
	 if (strcmp(a._opName, b._opName) == 0)
   return _YES_;
	 return _NO_;
 }
 
 friend ostream & operator << (ostream & out, const OpCodeTableEntry
  & a) 
 {
	 out << "OpCode Name: " << a._opName << endl;
	 out << "OpCode Type: " << a._opType << endl;
	 return out;
 }
 
};

class OpCodeTable
{
private:
protected:
	static Array<OpCodeTableEntry> _opCodeTable;
	
public:
 OpCodeTable()
 {
 }
 
 // Note that I have changed OpCodeTable to store OpCodeTableEntry
 // objects rather than char * strings. Thus, the following comment no
 // longer applies. I have left it here because I feel that there is
 // some info. to be gained out of this comment.
 
 // Imp : Disregard the following comment.
 
 // The destructor is a problem, because of the fact that we are trying
 // to store strings. This means that the array destructor cannot be
 // called because the array stores pointers to char * type.
 // We want to destroy the strings as well as the pointers that point
 // to them.
 //
 // We construct an iterator that is going to run thru the table
 // freeing up space. Note that since the size of the table is known
 // already, we could just have a for loop, instead of the while loop
 // which is being used now.
 //
 ~OpCodeTable()
 {
	 _opCodeTable.removeAllElements();
 }
 
 void addOpCode(char *opCodeText, int genericOrTarget = _GENERIC_, int opType = INVALID_OP)
 {
	 OpCodeTableEntry tableElem(opCodeText, genericOrTarget, opType);
	 _opCodeTable.insertElement(tableElem);
 }
 
 void addOpCode(char *opCodeText, int genericOrTarget, int opType, char *opSem)
 {
	 OpCodeTableEntry tableElem(opCodeText, genericOrTarget, opType, opSem);
	 _opCodeTable.insertElement(tableElem);
 }
 
 void addOpCode(char *opCodeText, int genericOrTarget, int opType, 
		EnumGenericOpCode genericOpCodeEnum)
 {
	 OpCodeTableEntry tableElem(opCodeText, genericOrTarget, opType, genericOpCodeEnum);
	 _opCodeTable.insertElement(tableElem);
 }
 
  void addOpCode(char *opCodeText, int genericOrTarget, int opType, char *opSem,
		 EnumGenericOpCode genericOpCodeEnum)
 {
	 OpCodeTableEntry tableElem(opCodeText, genericOrTarget, opType, opSem,
				    genericOpCodeEnum);
	 _opCodeTable.insertElement(tableElem);
 }
 
 OpCodeTableEntry & operator [] (const int index)
 {
  try
  {
   return _opCodeTable.elementAt(index);
  }
  catch (ArrayOutOfBoundsException *r)
  {
   cout << "Panic: Invalid OpCode Table Index" << endl;
   cout << *r;
   assert(0);
  }
  catch (NoSuchElementException *r)
  {
   cout << "Panic: No such OpCode";
   cout << *r;
   cout << endl;
   assert(0);
  }
  // SAH, Nov 03, 99 : This path should never be taken.
  //
  return _opCodeTable.elementAt(0);
	}
 
 int isType(int opIndex, int opType)
 {
	 int tempIndex = ((*this)[opIndex]).typeOf();

	 if (opType == tempIndex) return _YES_;

	 // Additional code to check for memory ops:

	 if ((opType == MEMORY_OP) && ((tempIndex == STORE_OP) || (tempIndex == LOAD_OP)))
		 return _YES_;

	 // Additional code to check for call/ret ops:

	 if ((opType == CALLRET_OP) && ((tempIndex == CALL_OP) || (tempIndex == RET_OP)))
		 return _YES_;

	 return _NO_;
 }
 
 int isGeneric(int opIndex)
 {
	 return ((*this)[opIndex]).isGeneric();
 }

 int isTarget(int opIndex)
 {
	 return ((*this)[opIndex]).isTarget();
 }

 int getIndex(const char *opCodeName)
 {
   ArrayIterator<OpCodeTableEntry> *opCodeTableIterator;
   ArrayIterator<OpCodeTableEntry> *firstOpCodeTableIterator;
  
   opCodeTableIterator = _opCodeTable.elements();
   firstOpCodeTableIterator = opCodeTableIterator;
   while(opCodeTableIterator->hasMoreElements())
     {
       try
	 {
	   if (opCmp(opCodeTableIterator->currentElement(),
		     opCodeName) == _YES_)
	     {
	       int temp = opCodeTableIterator->currentIndex();
	       delete firstOpCodeTableIterator;
	       return temp;
	     }
	 }
       catch (NoSuchElementException *r)
	 {
	   cout << "Panic: Something wrong with OpCodeTable" << endl;
	   //
	   // Basically says that we have gone over the bounds of the array.
	   
	   cout << *r;
	   assert(0);
	 }
       
       opCodeTableIterator->nextElement();
  }
   delete firstOpCodeTableIterator;
   throw (Exception *)
     new NoSuchElementException("OpCodeTable::getIndex()");
   
   //NOS
   exit(-1);
   return 0;
 }
 

 void getOpCodeName(int opCodeIndex, MyString &opCodeName)
 {
	cout << "OpCode is : " << ((*this)[opCodeIndex]).opName() << endl;
 }
 
 void print(ostream & out)
 {
   ArrayIterator<OpCodeTableEntry> *opCodeTableIterator;
  
   opCodeTableIterator = _opCodeTable.elements();
   while(opCodeTableIterator->hasMoreElements())
   {
	   out << opCodeTableIterator->currentElement();
	   opCodeTableIterator->nextElement();
   }
   delete opCodeTableIterator;
 }

};

#endif



