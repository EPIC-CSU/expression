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
// $Id: OpCode.h,v 1.4 1998/01/22 23:59:18 pgrun Exp $
//
// File:  		OpCode.h
// Created:		Thu Nov 07 , 96
// Last modified: 	Thu Nov 07 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

// From the target specification, we read in the list of opcodes for
// the machine and store them in a table.
// (The target specification also has to show a correspondance between
//  the opcode and the types recognized by the compiler)
// Now, when we read in the source code, we do not need to store the
// opcode string. Instead, we can store the table entry number that
// corresponds to that opcode. Inside that table, corresponding to
// each opcode, we will also need to store the semantics of that
// opcode. 

#ifndef __OPCODE_H__
#define __OPCODE_H__

#include "GlobalIncludes.h"
#include "OpCodeTable.h"
#include "MyString.h"

typedef int TableIndex;

extern OpCodeTable globalOpCodeTable;

class OpCode
{
private:
 TableIndex _opCodeIndex;
 
public:
 OpCode(int index = NO_OP) : _opCodeIndex(index)
 {
 }
 
 OpCode(const char *opName)
 {
	 _opCodeIndex = globalOpCodeTable.getIndex(opName);
	 
	 // Perform table lookup to get the proper index, and
	 // initialize _opCodeIndex. 
 }
 
 OpCode(const OpCode & a)
 {
	 _opCodeIndex = a._opCodeIndex;
 }
 
 ~OpCode()
 {
 }
 
 OpCode *copy();
 
 void init(char *str)
 {
	 // Perform table lookup to get the proper index, and
	 // initialize _opCodeIndex. 
	 //
	 _opCodeIndex = globalOpCodeTable.getIndex(str);
	 
 }
 
 void init(TableIndex someIndex)
 {
  // Note that we do not do any error checking.
  // The assumption is that the callee function passes a proper index as the parameter.
  //
  _opCodeIndex = someIndex;
 }
 const char *mnemonic()
 {
	 // Perform table lookup, get the opcode name.
  
  //NOS
  return NULL;
 }
 
 void mnemonic(MyString & s)
 {
	 s = globalOpCodeTable[_opCodeIndex].opName();
 }

 const TableIndex index()
 {
  return _opCodeIndex;
 }
 
 int typeOf()
 {
  return globalOpCodeTable[_opCodeIndex].typeOf();
  
 }
 
 OpCode & operator = (const OpCode & a)
 {
  if (this != &a) // To guard against s = s
  {
   _opCodeIndex = a._opCodeIndex;
  }
  return *this;
 }
 
 // Two opcodes are equal if they have the same table index number.
 //
 friend int operator == (const OpCode & a, const OpCode & b)
 {
  if (a._opCodeIndex == b._opCodeIndex)
   return _YES_;
  
  return _NO_;
 }
 
 friend ostream & operator << (ostream & out, const OpCode & s)
 {
  // Perform table lookup. Get the opcode name and then print it.
  
  //out << s._opCodeIndex << " ";
  out << globalOpCodeTable[s._opCodeIndex].opName() << " ";
  
  return out;
 }
 
// For ASM
 friend MyString getOpcode(const OpCode &s)
 {
	 MyString opcodeName = globalOpCodeTable[s._opCodeIndex].opName();
	 return opcodeName;
 }

 void printC();

 int isDoubleTypeOpcode();
};

#endif


