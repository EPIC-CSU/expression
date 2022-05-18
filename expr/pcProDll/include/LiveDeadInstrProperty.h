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
//==========================================================================
// File:			LiveDeadInstrProperty.h
// Created:			Oct. 21, 97
// Author:			Xiaomei Ji
//
//=========================================================================

#ifndef __LIVEDEADINSTRPROPERTY_H__
#define __LIVEDEADINSTRPROPERTY_H__

#ifndef WIN32
#include <iostream.h>
#endif

#include "PropertyIncludes.h"
//#include "MyLinkedList.h"
#include "BaseInstrProperty.h"
#include "SetList.h"
#include "SymbolTable.h"


//#include <set>

extern SymbolTable *globalSymbolTable;

//=========================================================================
// the live-dead property of basic block
// which records the live variables of a basic block 
//=========================================================================

class LiveDeadInstrProperty : public BaseInstrProperty {
private: 
	// record the _symVal of register's
	//live list at the begining fo teh instr
	SetList<int> _liveList_beg;
	//live list at the end fo teh instr
	SetList<int> _liveList_end;
public:
	LiveDeadInstrProperty();
	virtual ~LiveDeadInstrProperty();
	// omit const
	LiveDeadInstrProperty(LiveDeadInstrProperty& prop);
	SetList<int> &getLiveListBeg();
	SetList<int> &getLiveListEnd();
	SetList<int> *getLiveListPtrBeg();
	SetList<int> *getLiveListPtrEnd();
	void appendLiveVarBeg(int live);
	void appendLiveVarEnd(int live);
	MyLinkedListIterator<int>* liveListBegIterForw();
	MyLinkedListIterator<int>* liveListBegIterBack();
	MyLinkedListIterator<int>* liveListEndIterForw();
	MyLinkedListIterator<int>* liveListEndIterBack();
	int is(PropertyNames prop);
	BaseInstrProperty* copy();
	friend ostream& operator << (ostream& out, LiveDeadInstrProperty& prop);
	void print();
	void print(ostream& out);
	int isEq(BaseInstrProperty* a);
};

#endif
