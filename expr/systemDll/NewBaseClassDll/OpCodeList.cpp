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
 *file  : OpCodeList.cpp                                     *
 *authors : ashok, vgee                                      *
 *last modified :                                            *
 *                                                           *
 *contents:contains the OpCodeListEntry Class defs.This class*
 *         defines an OpCode Entry in the List associated    *
 *         with each Unit.                                   *
 *         contains the class OpCodeList which contains the  *
 *         list of actual opcodes.                           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#include "stdafx.h"

#include "OpCodeList.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//__declspec( dllimport ) OpCodeTable globalOpCodeTable;
//__declspec( dllimport ) Array<OpCodeTableEntry> _opCodeTable;

/*************************************************************
 *OpCodeListEntry Class                                      *
 *************************************************************/

extern OpCodeTable globalOpCodeTable;

OpCodeTable & OpCodeList::_opTable = globalOpCodeTable;

OpCodeListEntry::OpCodeListEntry(BaseOpExecSemantics & execOp):_opCodeIndex(-1), _executeOp(execOp){}
OpCodeListEntry::OpCodeListEntry(int opIndex, BaseOpExecSemantics & execOp):_opCodeIndex(opIndex), _executeOp(execOp){}
OpCodeListEntry::OpCodeListEntry(int opIndex, BaseOpExecSemantics & execOp, int timing):_opCodeIndex(opIndex), _executeOp(execOp), _timing(timing){}

OpCodeListEntry & OpCodeListEntry::operator =(const OpCodeListEntry& a)
{
  _opCodeIndex = a._opCodeIndex;
  _executeOp = a._executeOp;
  _timing = a._timing;
  return *this;
}

bool operator ==(const OpCodeListEntry & a, int someIndex)
{
  return (a._opCodeIndex == someIndex);
}

bool operator ==(const OpCodeListEntry& a,const OpCodeListEntry& b)
{
   return (a._opCodeIndex == b._opCodeIndex);
}
 
bool operator <(const OpCodeListEntry & a, int someIndex)
{
  return (a._opCodeIndex < someIndex);
}

bool operator !=(const OpCodeListEntry & a, int someIndex)
{
  return (a._opCodeIndex != someIndex);
}

bool operator <(const OpCodeListEntry& a,const OpCodeListEntry& b)
{
  return (a._opCodeIndex < b._opCodeIndex);
}

/*************************************************************
 *OpCodeList Class                                           *
 *************************************************************/
OpCodeList::OpCodeList():_sorted(_NO_){}

// Currently, we do not add opcodes in a sorted manner. This can be
// changed later.
void OpCodeList::addOpCode(int opCodeIndex, BaseOpExecSemantics & execOp)
{
// Note that we assume that the opCodeIndex that we recieve
// is unique. This assumption seems reasonable, seeing that
// each FU can have only one execution semantics associated
// with any particular opcode.

    OpCodeListEntry a(opCodeIndex, execOp, 1);
    _listOfOpCodes.push_back(a);
}
   
void OpCodeList::addOpCode(const char *opName, BaseOpExecSemantics & execOp)
{
// Here, we assume that the same opName is not added more
// than once to this list. If necessary, a check can be made
// by the function which call this member method to ensure
// uniqueness. 

     OpCodeListEntry a(_opTable.getIndex(opName), execOp, 1);
     _listOfOpCodes.push_back(a);
}

void OpCodeList::addOpCode(const char *opName, BaseOpExecSemantics & execOp, int timing)
{
// Here, we assume that the same opName is not added more
// than once to this list. If necessary, a check can be made
// by the function which call this member method to ensure
// uniqueness. 

     OpCodeListEntry a(_opTable.getIndex(opName), execOp, timing);
     _listOfOpCodes.push_back(a);
}
   
void OpCodeList::sortOpCodes()
{
   if(!_sorted)
     {
      sort(_listOfOpCodes.begin(), _listOfOpCodes.end());
     }
     _sorted = _YES_;
}
   
bool OpCodeList::hasOpCode(int opIndex)
{
    OpCodeArrayIter first, last, i;
	
    first = _listOfOpCodes.begin();
     last = _listOfOpCodes.end();
	 
    i = _getOpCode(first, last, opIndex);
	 
    return ((i != last) && (opIndex == i->_opCodeIndex));
}

bool OpCodeList::hasOpCode(char * opName)
{
    int opIndex = _opTable.getIndex(opName);

    OpCodeArrayIter first, last, i;
	 
    first = _listOfOpCodes.begin();
    last = _listOfOpCodes.end();
	 
    i = _getOpCode(first, last, opIndex);
	 
    return ((i != last) && (opIndex == i->_opCodeIndex));
}

int OpCodeList::getTiming(int opIndex)
{
    OpCodeArrayIter first, last, i;
	 
    first = _listOfOpCodes.begin();
    last = _listOfOpCodes.end();
	 
    i = _getOpCode(first, last, opIndex);

	if ((i != last) && (opIndex == i->_opCodeIndex))
		return i->_timing;
	else
		return -1;
}

BaseOpExecSemantics & OpCodeList::getExecuteFunc(int opIndex)
{
    OpCodeArrayIter first, last, i;
	
    first = _listOfOpCodes.begin();
    last = _listOfOpCodes.end();
	 
    i = _getOpCode(first, last, opIndex);
	 
    if ((i != last) && (opIndex == i->_opCodeIndex))
       {
	return (i->_executeOp);
       }
	else
	{
		// Error condition.
		cout << "Error: wrong index for the opcodelist" << endl;
		return (i->_executeOp);
	}
}

BaseOpExecSemantics & OpCodeList::getExecuteFunc(const char *opName)
{
    int opIndex = _opTable.getIndex(opName);

    OpCodeArrayIter first, last, i;
	 
    first = _listOfOpCodes.begin();
     last = _listOfOpCodes.end();
	 
     i = _getOpCode(first, last, opIndex);
	 
     if ((i != last) && (i->_opCodeIndex == opIndex))
	return (i->_executeOp);
	else
	{
		// Error condition.
		cout << "Error: wrong index for the opcodelist" << endl;
		return (i->_executeOp);
	}
}
//TODO : we may need a print routine here to print the OpCodeList. this routine
// will inturn be used by Unit class.
