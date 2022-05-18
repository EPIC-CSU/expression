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
//========================================================================
// File         UDProperty.h
// Purpose      the class of UDChainProperty 
// Created      Oct. 24, 97
// Author       Xiaomei Ji
//=======================================================================

#ifndef __UDCHAINPROPERTY_H__
#define __UDCHAINPROPERTY_H__

#include "MyLinkedList.h"
#include "BaseOperation.h"
#include "BaseArgProperty.h"
#include "PropertyIncludes.h"

class UDChainProperty : public BaseArgProperty {
	MyLinkedList<BaseOperation*> _defList;
public:
	UDChainProperty() { };
	UDChainProperty(MyLinkedList<BaseOperation*>& defs) {
		_defList.removeAll();
		MyLinkedListIterator<BaseOperation*>* iter = defs.elementsForward();
		while ( iter->hasMoreElements() ) {
			this->appendDef(iter->currentElement());
			iter->nextElement();
		}
		delete iter;
	}
	
	UDChainProperty(UDChainProperty & b){
		MyLinkedListIterator<BaseOperation*> *iter = (b._defList).elementsForward();
		while (iter->hasMoreElements()){
			_defList.append(iter->currentElement());
			iter->nextElement();
		}
		delete iter;
	}
	
	virtual ~UDChainProperty() { };
	
	MyLinkedList<BaseOperation*>& getDefList() { return _defList; }
	void appendDef(BaseOperation* def) {
		int hasDef = 0;
		
		MyLinkedListIterator<BaseOperation *> *iter = _defList.elementsForward();
		while (iter->hasMoreElements()){
			if (iter->currentElement() == def){
				hasDef = 1;
				break;
			}
			iter->nextElement();
		}
		delete iter;
		
		if (!hasDef)
			_defList.append(def); 
	}
	
	void addDef(BaseOperation* def) { 
		int hasDef = 0;
		
		MyLinkedListIterator<BaseOperation *> *iter = _defList.elementsForward();
		while (iter->hasMoreElements()){
			if (iter->currentElement() == def){
				hasDef = 1;
				break;
			}
			iter->nextElement();
		}
		delete iter;
		
		if (!hasDef)
			_defList.add(def); 
	} 
	// check for unique
	// 
	// SAH, Aug 09, 99: Added code to check for uniqueness of the def to be added.
	
	MyLinkedListIterator<BaseOperation*>* defListIteratorForw() {
		return _defList.elementsForward();
	}
	
	MyLinkedListIterator<BaseOperation*>* defListIteratorBack() {
		return _defList.elementsBackward();
	}
	
	BaseArgProperty* copy();
	
	int is(PropertyNames name) { if (name==_UDCHAIN_) return _YES_; return _NO_; }
	
	void print();
	void print(ostream& out);
	
	friend ostream& operator<<(ostream& out, UDChainProperty& p) {
		out << " the Def List: " << p.getDefList() <<endl;
		return out;
	}
	
	
	int isEq(BaseArgProperty* a) {
		if ( a->is(_UDCHAIN_) ) {
			if ( _defList.numItems() == ((UDChainProperty*)a)->getDefList().numItems() ) {
				MyLinkedListIterator<BaseOperation*>* iter = ((UDChainProperty*)a)->defListIteratorForw();
				while ( iter->hasMoreElements() ) {
					if ( _defList.contains(iter->currentElement()) == _NO_ )
						return _NO_;
					iter->nextElement();
				}
				delete iter;
				return _YES_;
			}
		}
		return _NO_;
	} 
	
	void remove(BaseOperation* op) {
		_defList.remove(op);
	}

	void removeAllDefs() {
		_defList.removeAll();
	}
};

#endif

