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
// File:			BBProperty.h
// Created:			Oct. 21, 97
// Author:			Xiaomei Ji
//
//=========================================================================

#ifndef __BBPROPERTY_H__
#define __BBPROPERTY_H__

#ifndef WIN32
#include <iostream.h>
#endif

#include "PropertyIncludes.h"
#include "MyLinkedList.h"
#include "BaseArgument.h"




// the base property of basic block, this is an ABC
class BaseBBProperty {
public:	
	BaseBBProperty() { }
	virtual ~BaseBBProperty() { }
	virtual int is(PropertyNames prop) = 0;
	static int isValidProperty(PropertyNames prop) {
		switch ( prop ) {
		case _COMPDEF_:
		case _LIVEDEAD_:
			return _YES_;
		}
		return _NO_;
	}
	virtual BaseBBProperty* copy() = 0;
	virtual int operator==(const BaseBBProperty&) { return _YES_;}
	friend ostream& operator<<(ostream& out, BaseBBProperty&) { return out;}
	virtual void print() = 0;
	virtual void print(ostream&) = 0;
};


//=========================================================================
// the compute definition property of basic block,
// which records all the defined variables in the basic block
// please note that: all variables refer to Register Argument in this file
//=========================================================================

class CompDefBBProperty : public BaseBBProperty {
private:
	MyLinkedList<BaseArgument*> _defList;
public:
	CompDefBBProperty() { }
	~CompDefBBProperty() { }
//	CompDefBBProperty(const CompDefBBProperty& );
//	CompDefBBProperty& operator = (const CompDefBBProperty&);
		
	MyLinkedList<BaseArgument*>& getDefList() { return _defList; }
	void appendDefinition(BaseArgument* def) { _defList.append(def); }
	inline MyLinkedListIterator<BaseArgument*>* defListIterForw();
	inline MyLinkedListIterator<BaseArgument*>* defListIterBack(); 
	int is(PropertyNames prop){if (prop==_COMPDEF_) return _YES_;return _NO_;} 
	BaseBBProperty* copy();

	// the second parameter should be const,  
	// but now without because of the << of MyLinkedList
	friend ostream& operator<< (ostream&, CompDefBBProperty&);
	void print();
	void print(ostream&);
};


//=========================================================================
// the live-dead property of basic block
// which records the live variables of a basic block 
//=========================================================================

class LiveDeadBBProperty : public BaseBBProperty {
private: 
	MyLinkedList<BaseArgument*> _liveList;
public:
	LiveDeadBBProperty() { }
	~LiveDeadBBProperty() { }
//	LiveDeadBBProperty(const LiveDeadBBProperty& );
//	LiveDeadBBProperty& operator = (const LiveDeadBBProperty&);
		
	MyLinkedList<BaseArgument*>& getLiveList() { return _liveList; }
	void appendLiveVar(BaseArgument* live) { _liveList.append(live); }
	inline MyLinkedListIterator<BaseArgument*>* liveListIterForw();
	inline MyLinkedListIterator<BaseArgument*>* liveListIterBack(); 
	int is(PropertyNames prop) {if(prop==_LIVEDEAD_)return _YES_;return _NO_;} 

	BaseBBProperty* copy();

	// the second parameter should be const
	friend ostream& operator << (ostream&, LiveDeadBBProperty&);
	void print();
	void print(ostream&);
};

#endif
