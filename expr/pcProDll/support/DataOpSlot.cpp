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
// $Id: DataOpSlot.cc,v 1.1 1998/03/21 01:16:10 ahalambi Exp $
//
// File:  		DataOpSlot.cc
// Created:		Wed Mar 11, 98
// Last modified: 	Wed Mar 11, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "DataOpSlot.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//GG: set this to 1, to generate VLIW-like code, with the NOPs explicitly in the code
extern int vliwASMGEN;


DataOpSlot::DataOpSlot(const DataOpSlot & someDataOpSlot)
{
 _next = someDataOpSlot._next;
 
 if (someDataOpSlot._dataOpPtr != _NULL_)
 {
	 _dataOpPtr = someDataOpSlot._dataOpPtr->copy();
 }
 else
 {
	 _dataOpPtr = _NULL_;
 }
}

void DataOpSlot::addOperation(BaseOperation *someOperation, bool noCopy)
{
 if (someOperation == _NULL_)
 {
	 // throw an exception.
  
  return;
 }

 if (noCopy)
  _dataOpPtr = someOperation;
 else
  _dataOpPtr = someOperation->copy();
}

void DataOpSlot::deleteOperation()
{
 delete _dataOpPtr;
 _dataOpPtr = _NULL_;
}

int DataOpSlot::hasOper()
{
 if (_dataOpPtr != _NULL_) return _YES_;
 
 return _NO_;
}

const int operator == (const DataOpSlot & a, const DataOpSlot & b)
{
 return ( a.getPtrToNextSlot()==b.getPtrToNextSlot()
  &&  a.getPtrToOperation() == b.getPtrToOperation() );
}

ostream & operator << (ostream & out, const DataOpSlot & s)
{
 if (s._dataOpPtr != _NULL_)
  s._dataOpPtr->print();
 return out;
}

void DataOpSlot::print(ostream& out)
{
 if (_dataOpPtr != _NULL_)
  _dataOpPtr->print(out);
}

void DataOpSlot::shortPrint(ostream& out)
{
 if (_dataOpPtr != _NULL_)
 {
	 out << "  ";
  _dataOpPtr->shortPrint(out);
  //NOSTEMP
	 out << "  (* " << _dataOpPtr << " *)";
 }
}


void DataOpSlot::createListOfOperands(Symtab *operandSymTable)
{
 if (_dataOpPtr != _NULL_)
   {
     _dataOpPtr->createListOfOperands(operandSymTable);
   }
}

void DataOpSlot::printProperties(ostream & out, PropNameList & pL)
{
 if (_dataOpPtr != _NULL_)
 {
	 out << "  ";
  _dataOpPtr->printProperties(out, pL);
 }
}

BaseOpSlot *DataOpSlot::copy()
{
 BaseOpSlot *retPtr;
 
 retPtr = new DataOpSlot(*this);
 
 return retPtr;
}

DataOpSlot::~DataOpSlot(){
	 delete _dataOpPtr;
}

// for ASM gen
void DataOpSlot::printASMInstr(ostream& out)
{
 if (_dataOpPtr != _NULL_)
 {
  _dataOpPtr->printASMInstr(out);  

 }
 else{
	 if(vliwASMGEN){
		//GG: modified to print out also the NOPs, for VLIW-like ASM
		out << "\t4\tnop\t()\t()\n";
	 }
 }
}
