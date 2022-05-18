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
/*************************************************************
 *file  : OpCodeList.h                                       *
 *authors : ashok,vgee			                             *
 *last modified :                                            *
 *                                                           *
 *contents:contains the OpCodeListEntry Class defs.This class*
 *         defines an OpCode Entry in the List associated    *
 *         with each Unit.                                   *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef _OPCODELIST_H_
#define _OPCODELIST_H_

#include "ArchClassIncludes.h"
#include "OpCodeTable.h"
#include "BaseOpExecSemantics.h"
#include "ArchSTLIncludes.h"

/*************************************************************
 *class:OpCodeListEntry : this class describes one entry in  *
 *          the Opcode List which is associated with every   *
 *          unit.every Unit has got a list of opcodes assoc- *
 *          iated with it which are supported by it.         *
 *************************************************************/

class OpCodeListEntry
{
 public:

   int _opCodeIndex;                 //index of the OpCode
   BaseOpExecSemantics & _executeOp; //Ref. to The OpCode object.
   int _timing;


   OpCodeListEntry(BaseOpExecSemantics & execOp);
   OpCodeListEntry(int opIndex, BaseOpExecSemantics & execOp);
   OpCodeListEntry(int opIndex, BaseOpExecSemantics & execOp, int timing);
   OpCodeListEntry & operator =(const OpCodeListEntry & a);
   
   friend bool operator ==(const OpCodeListEntry & a, int someIndex);
   friend bool operator ==(const OpCodeListEntry & a, const OpCodeListEntry & b);
   friend bool operator <(const OpCodeListEntry & a, int someIndex);
   friend bool operator !=(const OpCodeListEntry & a, int someIndex);
   friend bool operator <(const OpCodeListEntry & a, const OpCodeListEntry & b);
};

/*************************************************************
 *class:OpCodeList : this class contains the list of opcodes *
 *               handled by a unit.                          *
 *************************************************************/
class OpCodeList
{
 private:

  typedef Vector<OpCodeListEntry> OpCodeArray;
  typedef Vector<OpCodeListEntry>::iterator OpCodeArrayIter;
   
  static OpCodeTable & _opTable; 
  bool _sorted;
  OpCodeArray _listOfOpCodes;
  inline OpCodeArrayIter _getOpCode(OpCodeArrayIter & i1,
			    OpCodeArrayIter & i2,int opIndex)
  {
	  return (_sorted ? lower_bound(i1, i2, opIndex):std::find(i1, i2, opIndex));
  }
   
 public:
   OpCodeList();

   void addOpCode(int opCodeIndex, BaseOpExecSemantics & execOp);
   void addOpCode(int opCodeIndex, BaseOpExecSemantics & execOp, int timing);
   void addOpCode(const char *opName, BaseOpExecSemantics & execOp);
   void addOpCode(const char *opName, BaseOpExecSemantics & execOp, int timing);
   void sortOpCodes();
   bool hasOpCode(int opIndex);
   bool hasOpCode(char * opName);
   int getTiming(int opIndex);
   BaseOpExecSemantics & getExecuteFunc(int opIndex);
   BaseOpExecSemantics & getExecuteFunc(const char *opName);
};

#endif

	 
