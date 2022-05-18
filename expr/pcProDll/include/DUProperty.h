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
// File         DUProperty.h
// Purpose      the class of DUChainProperty
// Created      Oct. 24, 97
// Author       Xiaomei Ji
//=======================================================================

#ifndef __DUCHAINPROPERTY_H__
#define __DUCHAINPROPERTY_H__

#include "MyLinkedList.h"
#include "BaseOperation.h"
#include "BaseArgProperty.h"
#include "PropertyIncludes.h"
#include "BaseArgument.h"


class DUChainProperty : public BaseArgProperty {
	MyLinkedList<BaseOperation*> _useList;
public:
	DUChainProperty() { };
	DUChainProperty(MyLinkedList<BaseOperation*>& uses)  {
		_useList.removeAll();
		MyLinkedListIterator<BaseOperation*>* iter = uses.elementsForward();
		while ( iter->hasMoreElements() ) {
			this->appendUse(iter->currentElement());
			iter->nextElement();
		}
		delete iter;
	}
	DUChainProperty(DUChainProperty& a){
		MyLinkedListIterator<BaseOperation*> *iter = (a._useList).elementsForward();
		while (iter->hasMoreElements()){
			_useList.append(iter->currentElement());
			iter->nextElement();
		}
		delete iter;
	}
	
	//   DUChainProperty& operator=(const DUChainProperty&);
	virtual ~DUChainProperty() {
	};
	
	void appendUse(BaseOperation* use) { _useList.append(use) ; }
	void addUse(BaseOperation* use) { _useList.add(use) ; } // check for unique
	MyLinkedList<BaseOperation*>& getUseList() { return _useList; }
	
	MyLinkedListIterator<BaseOperation*>* useListIteratorForw() {
		return _useList.elementsForward();
	}
	
	MyLinkedListIterator<BaseOperation*>* useListIteratorBack() {
		return _useList.elementsBackward();
	}
	
	BaseArgProperty* copy();
	
	int is(PropertyNames name) { if (name==_DUCHAIN_) return _YES_; return _NO_; }
	
	void print();
	void print(ostream& out);
	
	friend ostream& operator<<(ostream& out, DUChainProperty& p) {
		out << " The Use List: " << p.getUseList() << endl;
		return out;
	}
	
	int isEq(BaseArgProperty* a) {
		if ( a->is(_DUCHAIN_) ) {
			if ( _useList.numItems() == ((DUChainProperty*)a)->getUseList().numItems() ) {
				MyLinkedListIterator<BaseOperation*>* iter = ((DUChainProperty*)a)->useListIteratorForw();
				while ( iter->hasMoreElements() ) {
					if ( _useList.contains(iter->currentElement()) == _NO_ )
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
		_useList.remove(op);
	}

	void removeAllUses() {
		_useList.removeAll();
	}
};

#endif

