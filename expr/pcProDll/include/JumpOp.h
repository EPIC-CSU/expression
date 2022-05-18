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
// $Id: JumpOp.h,v 1.12 1998/03/21 01:18:56 ahalambi Exp $
//
// File:  		JumpOp.h
// Created:		Tue Oct 29, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __JUMPOP_H__
#define __JUMPOP_H__

#include "FlowOp.h"
#include "NormalInstr.h"
#include "PropertyIncludes.h"

class JumpOp : public FlowOp 
{
private:
 
#ifdef COUNT_JUMPOPS
 static int _numJumpOps;	// See comment 1 below.
#endif
 
protected:
 BaseInstruction * _targetInstr;
 //
 // Could store ID of target instruction too.
 
public:
 JumpOp()
 {
	 _targetInstr = NULL;
  
#ifdef COUNT_JUMPOPS
	 _numJumpOps++;	
#endif
 }
 
 JumpOp(BaseInstruction *tgt) : _targetInstr(tgt)
 {
 }
 
 JumpOp(const JumpOp & a)
 {
	 _targetInstr = a._targetInstr;

// Currently, we do not copy any properties of the operation.
 /*
 MyLinkedList<PropertyNames> propList;
 propList.appendElement(<insert_property_name_here>
 
   _opProperties.makeACopyOf(a._opProperties, propList);
 */
	 
#ifdef COUNT_JUMPOPS
	 _numJumpOps++;	
#endif
 }
 
 virtual ~JumpOp()
 {
#ifdef COUNT_JUMPOPS
	 _numJumpOps--;
#endif
 }
 
 void setTargetInstr(BaseInstruction *someInstr)
 {
	 _targetInstr = someInstr;
 }
 
 /*
 IDType getTargetInstrID()
 {
 if (_targetInstr != NULL)
 return targetInstrID->getID();
 }
 */
 
 BaseInstruction *getTargetInstr()
 {
	 return _targetInstr;
 }
 
 // The initializeID method will have to be redefined here.
 
 void initializeID();
 
#ifdef COUNT_JUMPOPS
 static int numJumpOps()
 {
	 return _numJumpOps;
 }
#endif
 
 BaseOperation *copy();
 
 virtual void getOpcodeName(MyString & s)
 {
	 // Since Jump does not have a valid opcode name, create one.
	 //
	 s = "JUMP";
 }

 virtual BaseOperation *copyBody(bool copyProp=false);
 
 int isJumpOp()
 {
	 return _YES_;
 }
 
 void print();
 void print(int indent);
 void printC(int indent);

 void print(ostream& out);
 void shortPrint(ostream& out);
 void printToString(char *buf);
 
   void printProperties(ostream & out, PropNameList & pL);

   friend int operator == (const JumpOp & a, const JumpOp & b);

   virtual int isEq(BaseOperation *op);
   void printASMInstr(ostream& out);
   
 friend ostream& operator << (ostream & out, const JumpOp & s);
};

#endif

// Comment 1:
//
// _numJumpOps is used to keep a count of the number of JumpOp objects
// in existence at any point. COUNT_JUMPOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
