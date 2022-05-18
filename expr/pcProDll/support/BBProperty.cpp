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
//===========================================================================
// File:		BBProperty.cc
// Purpose:		implementation of member funtions of bb's properties
// Created:		Oct. 21, 97
// Author:		Xiaomei Ji
//
//==========================================================================

#include "stdafx.h"

#include <iostream.h>
#include "MyLinkedList.h"
#include "BBProperty.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


//========================================================================
 
/***** following two should also be offered, but, with out 
 ****  operator of removeAll and == in MyLinkedList, 
 ****  so, first omit them

CompDefBBProperty::CompDefBBProperty(const CompDefBBProperty& cdProp) {
	_defList.removeAll();
	MyLinkedListIterator<BaseArgument*>* iter=cdProp.defListIterForw();	
	while ( iter->hasMoreElements() ) {
		this->addDefinition(iter->currentElement());
		iter->nextElement();
	}
}


CompDefBBProperty&
CompDefBBProperty::operator=(const CompDefBBProperty& prop) {
	if ( *this == prop )
		return;
	_defList.removeAll();
	MyLinkedListIterator<BaseArgument*>* iter=prop.defListIterForw();	
	while ( iter->hasMoreElements() ) {
		this->addDefinition(iter->currentElement());
		iter->nextElement();
	}
	return *this;
}
***********/


inline 
MyLinkedListIterator<BaseArgument*>* CompDefBBProperty::defListIterForw() {
	return _defList.elementsForward();
}


inline 
MyLinkedListIterator<BaseArgument*>* CompDefBBProperty::defListIterBack() {
	return _defList.elementsBackward();
}

// add a copy member function in MyLinkedList
BaseBBProperty*
CompDefBBProperty::copy() {
	CompDefBBProperty* prop = new CompDefBBProperty(); 
	MyLinkedListIterator<BaseArgument*>* iter = _defList.elementsForward();
	while ( iter->hasMoreElements() ) {
		prop->appendDefinition(iter->currentElement());
		iter->nextElement();
	}
	return prop;
}


ostream&
operator<< (ostream& out, CompDefBBProperty& prop) {
	out << "CompDefBBProperty: " << endl;
	return out << prop._defList;
}


void
CompDefBBProperty::print() {
	cout << "CompDefBBProperty: " << endl;
	cout << _defList;
}


void
CompDefBBProperty::print(ostream & out) {
	out << "CompDefBBProperty: " << endl;
	out << _defList;
}

//========================================================================

/*********

LiveDeadBBProperty::LiveDeadBBProperty(const LiveDeadBBProperty& prop) {
	_liveList.removeAll();
	MyLinkedListIterator<BaseArgument*>* iter=prop.LiveListIterForw();	
	while ( iter->hasMoreElements() ) {
		this->addDefinition(iter->currentElement());
		iter->nextElement();
	}
}


LiveDeadBBProperty&
LiveDeadBBProperty::operator=(const LiveDeadBBProperty& prop) {
	if ( *this == prop )
		return;
	_liveList.removeAll();
	MyLinkedListIterator<BaseArgument*>* iter=prop.LiveListIterForw();	
	while ( iter->hasMoreElements() ) {
		this->addDefinition(iter->currentElement());
		iter->nextElement();
	}
	return *this;
}

**********/


inline 
MyLinkedListIterator<BaseArgument*>* LiveDeadBBProperty::liveListIterForw() {
	return _liveList.elementsForward();
}


inline 
MyLinkedListIterator<BaseArgument*>* LiveDeadBBProperty::liveListIterBack() {
	return _liveList.elementsBackward();
}


ostream&
operator << (ostream& out, LiveDeadBBProperty& prop) {
	out << "LiveDeadBBProperty: " << endl;
	return out << prop._liveList;
}

BaseBBProperty*
LiveDeadBBProperty::copy() {

	//NOS
	return NULL;
}

void
LiveDeadBBProperty::print(){
	cout << "LiveDeadBBProperty: " << endl;
	cout << _liveList;
}

void
LiveDeadBBProperty::print(ostream& out){
	out << "LiveDeadBBProperty: " << endl;
	out << _liveList;
}

