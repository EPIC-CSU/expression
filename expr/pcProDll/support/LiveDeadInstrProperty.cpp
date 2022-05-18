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
#include "stdafx.h"

#include "LiveDeadInstrProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

BaseInstrProperty* LiveDeadInstrProperty::copy()
{
	LiveDeadInstrProperty* prop = new LiveDeadInstrProperty(); 
	MyLinkedListIterator<int>* iter = (MyLinkedListIterator<int>*)(_liveList_beg.elementsForward());
	while ( iter->hasMoreElements() ) {
		prop->appendLiveVarBeg(iter->currentElement());
		iter->nextElement();
	}
	delete iter;

	prop = new LiveDeadInstrProperty(); 
	iter = (MyLinkedListIterator<int>*)(_liveList_end.elementsForward());
	while ( iter->hasMoreElements() ) {
		prop->appendLiveVarEnd(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
	return prop;
}




LiveDeadInstrProperty::LiveDeadInstrProperty(){
}


LiveDeadInstrProperty::~LiveDeadInstrProperty(){

}


// omit const
LiveDeadInstrProperty::LiveDeadInstrProperty(LiveDeadInstrProperty& prop) {
	_liveList_beg.removeAll();
	_liveList_end.removeAll();
	MyLinkedListIterator<int>* iter = (MyLinkedListIterator<int>*)(prop._liveList_beg.elementsForward());
	while ( iter->hasMoreElements() ) {
		appendLiveVarBeg(iter->currentElement());
		iter->nextElement();
	}
	delete iter;

	iter = (MyLinkedListIterator<int>*)(prop._liveList_end.elementsForward());
	while ( iter->hasMoreElements() ) {
		appendLiveVarEnd(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
}


SetList<int>& LiveDeadInstrProperty::getLiveListBeg() { 
	return _liveList_beg; 
}

SetList<int>& LiveDeadInstrProperty::getLiveListEnd() { 
	return _liveList_end;
}

SetList<int> *LiveDeadInstrProperty::getLiveListPtrBeg() { 
	return &_liveList_beg; 
}

SetList<int> *LiveDeadInstrProperty::getLiveListPtrEnd() { 
	return &_liveList_end;
}


void LiveDeadInstrProperty::appendLiveVarBeg(int live) { 
	_liveList_beg.append(live); 
}

void LiveDeadInstrProperty::appendLiveVarEnd(int live) { 
	_liveList_end.append(live); 
}

int LiveDeadInstrProperty::is(PropertyNames prop) {
	if(prop==_LIVEDEADINSTR_) return _YES_;
	return _NO_;
} 

// the second parameter should be const
ostream& operator << (ostream& out, LiveDeadInstrProperty& prop) {
	out << "LiveDeadInstrProperty: " << endl;
	return out << prop._liveList_beg << prop._liveList_end ;
}


MyLinkedListIterator<int>* LiveDeadInstrProperty::liveListBegIterForw() {
	return _liveList_beg.elementsForward();
}
MyLinkedListIterator<int>* LiveDeadInstrProperty::liveListBegIterBack() {
	return _liveList_beg.elementsBackward();
}


MyLinkedListIterator<int>* LiveDeadInstrProperty::liveListEndIterForw() {
	return _liveList_end.elementsForward();
}
MyLinkedListIterator<int>* LiveDeadInstrProperty::liveListEndIterBack() {
	return _liveList_end.elementsBackward();
}

void LiveDeadInstrProperty::print() {
	//cout << "LiveDeadInstrProperty: " << _liveList << endl;
	cout << "LiveDeadInstrProperty: Beg: " << endl;
	MyLinkedListIterator<int>* iter = liveListBegIterForw();
	while ( iter->hasMoreElements() ) {
		int id = iter->currentElement();
		int index = globalSymbolTable->getIndex(id);
		// SSANum is from 1, not 0, even used before defined
		int ssaNum = id - globalSymbolTable->getStartNum(index) + 1;
		cout << "( " << id << ", " << index << ", " << ssaNum <<")   ";
		iter->nextElement();
	}
	delete iter;

	cout << "End: : " << endl;
	iter = liveListEndIterForw();
	while ( iter->hasMoreElements() ) {
		int id = iter->currentElement();
		int index = globalSymbolTable->getIndex(id);
		// SSANum is from 1, not 0, even used before defined
		int ssaNum = id - globalSymbolTable->getStartNum(index) + 1;
		cout << "( " << id << ", " << index << ", " << ssaNum <<")   ";
		iter->nextElement();
	}
	cout << " end of LiveDeadInstrProperty" << endl;
	delete iter;
}
void LiveDeadInstrProperty::print(ostream& out) {
	//out << "LiveDeadInstrProperty: " << _liveList << endl;
	out << "LiveDeadInstrProperty: Beg: " << endl;
	MyLinkedListIterator<int>* iter = liveListBegIterForw();
	while ( iter->hasMoreElements() ) {
		int id = iter->currentElement();
		int index = globalSymbolTable->getIndex(id);
		int ssaNum = id - globalSymbolTable->getStartNum(index) + 1;
		out << "( " << id << ", " << index << ", " << ssaNum <<")   ";
		iter->nextElement();
	}
	out << " End: " << endl;
	delete iter;
	iter = liveListEndIterForw();
	while ( iter->hasMoreElements() ) {
		int id = iter->currentElement();
		int index = globalSymbolTable->getIndex(id);
		int ssaNum = id - globalSymbolTable->getStartNum(index) + 1;
		out << "( " << id << ", " << index << ", " << ssaNum <<")   ";
		iter->nextElement();
	}
	out << " end of LiveDeadInstrProperty" << endl;
	delete iter;
}

//returns YES if the two properties are equal
int LiveDeadInstrProperty::isEq(BaseInstrProperty* a) {
	if(this==a) return _YES_;

	if ( a->is(_LIVEDEADINSTR_)){
		//first test the beg live list
		MyLinkedList<int>* temp_list = ((LiveDeadInstrProperty*)a)->getLiveListPtrBeg();
		if(temp_list->numItems() == _liveList_beg.numItems()){
			//if they have the same number of elements
			MyLinkedListIterator<int>* iter=liveListBegIterForw();
			//for all the elements in "this"
			while ( iter->hasMoreElements() ) {
				if ( temp_list->contains(iter->currentElement()) == _NO_ ) {
					//if a does not contain the element from this, return NO
					delete iter;
					return _NO_;
				}
				iter->nextElement();
			}
		}
		else 
			return _NO_;
		//if a contains all the elements from "this", continue searching. This is really inclusion, not equality!!!
		//then test the end live list
		temp_list = ((LiveDeadInstrProperty*)a)->getLiveListPtrEnd();
		if(temp_list->numItems() == _liveList_beg.numItems()){
			//if they have the same number of elements
			MyLinkedListIterator<int>* iter=liveListEndIterForw();
			//for all the elements in "this"
			while ( iter->hasMoreElements() ) {
				if ( temp_list->contains(iter->currentElement()) == _NO_ ) {
					//if a does not contain the element from this, return NO
					delete iter;
					return _NO_;
				}
				iter->nextElement();
			}
			//if a contains all the elements from "this", return YES. This is really inclusion, not equality!!!
			delete iter;
			return _YES_;
		}
	}
	//otherwise, they are not equal
	return _NO_;
}
