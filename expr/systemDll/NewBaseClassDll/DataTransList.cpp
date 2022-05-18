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
 *file  : DataTransList.cpp                                  *
 *authors : ashok                                            *
 *last modified :                                            *
 *                                                           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#include "DataTransList.h"

/*************************************************************
 *DataTransListEntry Class                                   *
 *************************************************************/

extern OpCodeTable globalOpCodeTable;

OpCodeTable & DataTransList::_opTable = globalOpCodeTable;

DataTransListEntry::DataTransListEntry(BaseDataTransSemantics & dataTrans):_opCodeIndex(-1), _dataTrans(dataTrans){}
DataTransListEntry::DataTransListEntry(int opIndex, BaseDataTransSemantics & dataTrans):_opCodeIndex(opIndex), _dataTrans(dataTrans){}

DataTransListEntry & DataTransListEntry::operator =(const DataTransListEntry & a)
{
  _opCodeIndex = a._opCodeIndex;
  _dataTrans = a._dataTrans; 
  return *this;
}

bool operator ==(const DataTransListEntry & a, int someIndex)
{
  return (a._opCodeIndex == someIndex);
}

bool operator ==(const DataTransListEntry & a,const DataTransListEntry& b)
{
   return (a._opCodeIndex == b._opCodeIndex);
}
 
bool operator <(const DataTransListEntry & a, int someIndex)
{
  return (a._opCodeIndex < someIndex);
}

bool operator !=(const DataTransListEntry & a, int someIndex)
{
  return (a._opCodeIndex != someIndex);
}

bool operator <(const DataTransListEntry & a,const DataTransListEntry & b)
{
  return (a._opCodeIndex < b._opCodeIndex);
}

/*************************************************************
 *OpCodeList Class                                           *
 *************************************************************/
DataTransList::DataTransList():_sorted(_NO_){}

// Currently, we do not add opcodes in a sorted manner. This can be
// changed later.
void DataTransList::addDataTrans(int opCodeIndex, BaseDataTransSemantics & dataTrans)
{
// Note that we assume that the opCodeIndex that we recieve
// is unique. This assumption seems reasonable, seeing that
// each FU can have only one execution semantics associated
// with any particular opcode.

    DataTransListEntry a(opCodeIndex, dataTrans);
    _listOfOpCodes.push_back(a);
}
   
void DataTransList::addDataTrans(const char *opName, BaseDataTransSemantics & dataTrans)
{
// Here, we assume that the same opName is not added more
// than once to this list. If necessary, a check can be made
// by the function which call this member method to ensure
// uniqueness. 

     DataTransListEntry a(_opTable.getIndex(opName), dataTrans);
     _listOfOpCodes.push_back(a);
}
   
void DataTransList::sortDataTrans()
{
   if(!_sorted)
     {
      sort(_listOfOpCodes.begin(), _listOfOpCodes.end());
     }
     _sorted = _YES_;
}
   
bool DataTransList::hasDataTrans(int opIndex)
{
    DataTransArrayIter first, last, i;
	
    first = _listOfOpCodes.begin();
     last = _listOfOpCodes.end();
	 
    i = _getOpCode(first, last, opIndex);
	 
    return ((i != last) && (opIndex == i->_opCodeIndex));
}

bool DataTransList::hasDataTrans(char * opName)
{
    int opIndex = _opTable.getIndex(opName);

    DataTransArrayIter first, last, i;
	 
    first = _listOfOpCodes.begin();
    last = _listOfOpCodes.end();
	 
    i = _getOpCode(first, last, opIndex);
	 
    return ((i != last) && (opIndex == i->_opCodeIndex));
}

BaseDataTransSemantics & DataTransList::getDataTransFunc(int opIndex)
{
    DataTransArrayIter first, last, i;
	
    first = _listOfOpCodes.begin();
    last = _listOfOpCodes.end();
	 
    i = _getOpCode(first, last, opIndex);
	 
    if ((i != last) && (opIndex == i->_opCodeIndex))
	{
		return (i->_dataTrans);
	}
	else
	{
		// try returning the default data trans
		//
		i = _getOpCode(first, last, _getDefaultOpCodeIndex());
		if (i != last)
			return (i->_dataTrans);
		else
		{
			// Error condition.
			cout << "Error: wrong index for the opcodelist" << endl;
			return (i->_dataTrans);
		}
	}
}

BaseDataTransSemantics & DataTransList::getDataTransFunc(const char *opName)
{
    int opIndex = _opTable.getIndex(opName);

    DataTransArrayIter first, last, i;
	 
    first = _listOfOpCodes.begin();
     last = _listOfOpCodes.end();
	 
     i = _getOpCode(first, last, opIndex);
	 
     if ((i != last) && (i->_opCodeIndex == opIndex))
	return (i->_dataTrans);
	 else
	 {
		// try returning the default data trans
		//
		i = _getOpCode(first, last, _getDefaultOpCodeIndex());
		if (i != last)
			return (i->_dataTrans);
		else
		{
			// Error condition.
			cout << "Error: wrong index for the opcodelist" << endl;
			return (i->_dataTrans);
		}
	 }
}

BaseDataTransSemantics & DataTransList::getDataTransFunc()
{
    int opIndex = _opTable.getIndex("DEFAULT");

    DataTransArrayIter first, last, i;
	 
    first = _listOfOpCodes.begin();
     last = _listOfOpCodes.end();
	 
     i = _getOpCode(first, last, _getDefaultOpCodeIndex());
	 
     if ((i != last) && (i->_opCodeIndex == opIndex))
		return (i->_dataTrans);
	 else
	 {
		// Error condition.
		cout << "Error: No default opcode for opcodelist" << endl;
		return (i->_dataTrans);
	 }
}

//TODO : we may need a print routine here to print the OpCodeList. this routine
// will inturn be used by Unit class.