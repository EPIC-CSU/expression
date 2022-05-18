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
// File:			LiveDeadBBProperty.h
// Created:			Oct. 21, 97
// Author:			Xiaomei Ji
//
//=========================================================================

#ifndef __LIVEDEADBBPROPERTY_H__
#define __LIVEDEADBBPROPERTY_H__

#ifndef WIN32
#include <iostream.h>
#endif

#include "PropertyIncludes.h"
//#include "MyLinkedList.h"
#include "BaseBBProperty.h"
#include "SetList.h"
#include "SymbolTable.h"

extern SymbolTable *globalSymbolTable;

//=========================================================================
// the live-dead property of basic block
// which records the live variables of a basic block 
//=========================================================================

// ** IMPORTANT **
//
// The LiveDeadBBProperty object records the 
// variables that are live at the exit of the basic block
//

class LiveDeadBBProperty : public BaseBBProperty {
private: 
	// record the _symVal of register's
	//MyLinkedList<int> _liveList;
	SetList<int> _liveList;
public:
	LiveDeadBBProperty() { }
	virtual ~LiveDeadBBProperty() { }
	// omit const
	LiveDeadBBProperty(LiveDeadBBProperty& prop) {
		_liveList.removeAll();
		MyLinkedListIterator<int>* iter=prop.liveListIterForw();	
		while ( iter->hasMoreElements() ) {
			this->appendLiveVar(iter->currentElement());
			iter->nextElement();
		}
	}
	/******** no == in MyLinkedList
	LiveDeadBBProperty& operator = (const LiveDeadBBProperty& prop ) {
	if ( *this == prop )
	return;
	_liveList.removeAll();
	MyLinkedListIterator<int>* iter=prop.liveListIterForw();	
	while ( iter->hasMoreElements() ) {
	this->appendLiveVar(iter->currentElement());
	iter->nextElement();
	}
	return *this;
	}
	********/
	
	//MyLinkedList<int>& getLiveList() { return _liveList; }
	SetList<int>& getLiveList() { return _liveList; }
	void appendLiveVar(int live) { _liveList.append(live); }
	MyLinkedListIterator<int>* liveListIterForw() {
		return _liveList.elementsForward();
	}
	MyLinkedListIterator<int>* liveListIterBack() {
		return _liveList.elementsBackward();
	}
	int is(PropertyNames prop) {if(prop==_LIVEDEAD_)return _YES_;return _NO_;} 
	
	BaseBBProperty* copy();
	
	// the second parameter should be const
	friend ostream& operator << (ostream& out, LiveDeadBBProperty& prop) {
		out << "LiveDeadBBProperty: " << endl;
		return out << prop._liveList;
	}
	void print() {
		//cout << "LiveDeadBBProperty: " << _liveList << endl;
		cout << "LiveDeadBBProperty: " << endl;
		MyLinkedListIterator<int>* iter = liveListIterForw();
		while ( iter->hasMoreElements() ) {
			int id = iter->currentElement();
			int index = globalSymbolTable->getIndex(id);
			// SSANum is from 1, not 0, even used before defined
			int ssaNum = id - globalSymbolTable->getStartNum(index) + 1;
			cout << "( " << id << ", " << index << ", " << ssaNum <<")   ";
			iter->nextElement();
		}
		cout << " end of LiveDeadBBProperty" << endl;
	}
	void print(ostream& out) {
		//out << "LiveDeadBBProperty: " << _liveList << endl;
		out << "LiveDeadBBProperty: " << endl;
		MyLinkedListIterator<int>* iter = liveListIterForw();
		while ( iter->hasMoreElements() ) {
			int id = iter->currentElement();
			int index = globalSymbolTable->getIndex(id);
			int ssaNum = id - globalSymbolTable->getStartNum(index) + 1;
			out << "( " << id << ", " << index << ", " << ssaNum <<")   ";
			iter->nextElement();
		}
		out << " end of LiveDeadBBProperty" << endl;
	}
	
	int isEq(BaseBBProperty* a) {
		if ( a->is(_LIVEDEAD_) ) {
			MyLinkedList<int>& temp_list = ((LiveDeadBBProperty*)a)->getLiveList();
			if ( temp_list.numItems() == _liveList.numItems() ) {
				MyLinkedListIterator<int>* iter=liveListIterForw();
				while ( iter->hasMoreElements() ) {
					if ( temp_list.contains(iter->currentElement()) == _NO_ ) {
						delete iter;
						return _NO_;
					}
					iter->nextElement();
				}
				delete iter;
				return _YES_;
			}
		}
		return _NO_;
	}
};

#endif
