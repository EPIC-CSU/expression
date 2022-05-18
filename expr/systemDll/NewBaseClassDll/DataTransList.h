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
 *file  : DataTransList.h                                    *
 *authors : ashok				                             *
 *last modified :                                            *
 *                                                           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef _DATATRANSLIST_H_
#define _DATATRANSLIST_H_

#include "ArchClassIncludes.h"
#include "../../pcProDll/include/OpCodeTable.h"
#include "BaseDataTransSemantics.h"
#include "ArchSTLIncludes.h"

/*************************************************************
 *class:DataTransListEntry : this class describes one entry  *
 *          in the Opcode List which is associated with every*
 *          unit.every Unit has got a list of opcodes assoc- *
 *          iated with it which are supported by it.         *
 *************************************************************/

class DataTransListEntry
{
 public:

   int _opCodeIndex;                 //index of the OpCode
   BaseDataTransSemantics & _dataTrans; //Ref. to The DataTrans object.


   DataTransListEntry(BaseDataTransSemantics & dataTrans);
   DataTransListEntry(int opIndex, BaseDataTransSemantics & dataTrans);
   DataTransListEntry & operator =(const DataTransListEntry & a);
   
   friend bool operator ==(const DataTransListEntry & a, int someIndex);
   friend bool operator ==(const DataTransListEntry & a, const DataTransListEntry & b);
   friend bool operator <(const DataTransListEntry & a, int someIndex);
   friend bool operator !=(const DataTransListEntry & a, int someIndex);
   friend bool operator <(const DataTransListEntry & a, const DataTransListEntry & b);
};

/*************************************************************
 *class:DataTransList : this class contains the list of		 *
 *               opcodes handled by a unit.                  *
 *************************************************************/
class DataTransList
{
 private:

  typedef Vector<DataTransListEntry> DataTransArray;
  typedef Vector<DataTransListEntry>::iterator DataTransArrayIter;
   
  static OpCodeTable & _opTable; 
  bool _sorted;
  DataTransArray _listOfOpCodes;

  inline int _getDefaultOpCodeIndex()
  {
	  return (_opTable.getIndex("DEFAULT"));
  }

  inline DataTransArrayIter _getOpCode(DataTransArrayIter & i1,
			    DataTransArrayIter & i2,int opIndex)
  {
	  return (_sorted ? lower_bound(i1, i2, opIndex):std::find(i1, i2, opIndex));
  }
   
 public:
   DataTransList();

   void addDataTrans(int opCodeIndex, BaseDataTransSemantics & execOp);
   void addDataTrans(const char *opName, BaseDataTransSemantics & execOp);
   void sortDataTrans();
   bool hasDataTrans(int opIndex);
   bool hasDataTrans(char * opName);
   BaseDataTransSemantics & getDataTransFunc(int opIndex);
   BaseDataTransSemantics & getDataTransFunc(const char *opName);
   BaseDataTransSemantics & getDataTransFunc();
   //
   // returns the default (if it exists) data transfer function.
};

#endif