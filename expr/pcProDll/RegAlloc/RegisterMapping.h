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
//: RegisterMapping.h
//
// File:  		RegisterMapping.h
// Created:		Wed Aug 16, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Provides the mapping between generic registers (used by compiler) and target registers (architecture dependent)
// 

#ifndef __REGISTER_MAPPING__
#define __REGISTER_MAPPING__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "MyLinkedList.h"
#include "STLIncludes.h"
#include "MultiChain.h"
#include "SymbolTable.h"

typedef SetList<int> RegisterList;
typedef MyLinkedListIterator<int> RegisterListIterator;

typedef MyLinkedList<int> IDList;


extern SymbolTable * globalSymbolTable;
extern int regAllocPrefferedOrder[];

class RegisterMapping {
	
	map<int, RegisterList *> _generalRegisterMapping;
	RegisterList * _callAddressRegisterMapping;
	map<int, RegisterList *> _callReturnRegisterMapping;
	map<int, RegisterList *> _callParamsRegisterMapping;
	map<int, RegisterList *> _stackPointerRegisterMapping;
	map<int, RegisterList *> _conditionCodeRegisterMapping;
	map<int, RegisterList *> _doubleFloatRegisterMapping;
	
public:
	RegisterMapping() {
		_callAddressRegisterMapping = NULL;
	}

	~RegisterMapping();

	RegisterList * getGeneralTargets(int symTabID) {
		return _generalRegisterMapping[symTabID];
	}

	RegisterList * getTargetsCallAddress() {
		return _callAddressRegisterMapping;
	}

	RegisterList * getTargetsCallReturn(int symTabID) {
		return _callReturnRegisterMapping[symTabID];
	}

	RegisterList * getTargetsCallParam(int symTabID) {
		return _callParamsRegisterMapping[symTabID];
	}

	RegisterList * getTargetsStackPointer(int symTabID) {
		return _stackPointerRegisterMapping[symTabID];
	}

	RegisterList * getTargetsConditionCode(int symTabID) {
		return _conditionCodeRegisterMapping[symTabID];
	}

	RegisterList * getTargetsDoubleFloat(int symTabID) {
		return _doubleFloatRegisterMapping[symTabID];
	}

	
	RegisterList * getCompatibleTargets(MultiChain * mChain) {
		
		RegisterList *compatTargets;
		int argID = mChain->getID();
		int regIndex = globalSymbolTable->getIndex(argID);

		// check to see if the multichain is a special one
		if (mChain->isCallAddress()) {
			compatTargets = getTargetsCallAddress();
		} else if (mChain->isCallParamArg()) {
			compatTargets = getTargetsCallParam(regIndex);
		} else if (mChain->isCallReturnArg()) {
			compatTargets = getTargetsCallReturn(regIndex);
		} else if (mChain->isStackPointerArg()) {
			compatTargets = getTargetsStackPointer(regIndex);
		} else if (mChain->isConditionCodeArg()) {
			compatTargets = getTargetsConditionCode(regIndex);
		} else if (mChain->isDoubleFloatArg()) {
			compatTargets = getTargetsDoubleFloat(regIndex);
		} else {	// just a normal argument
			compatTargets = getGeneralTargets(regIndex);
		}

		return compatTargets;
	}
	
	
	// all the set methods require a RegisterList reference as argument
	// the given Register List IS copied

	void setGeneralTargets(int symTabID, RegisterList *targets) {
		_generalRegisterMapping[symTabID] = new RegisterList(*targets);
	}

	void setGeneralTargets(IDList *ids, RegisterList *targets) {
		MyLinkedListIterator<int> *idIter = ids->elementsForward();
		while (idIter->hasMoreElements()) {
			int id = idIter->currentElement();

			setGeneralTargets(id, targets);

			idIter->nextElement();
		}
		delete idIter;
	}

	void setTargetsCallAddress(RegisterList *targets) {
		_callAddressRegisterMapping = new RegisterList(*targets);
	}

	void setTargetsCallReturn(int symTabID, RegisterList *targets) {
		_callReturnRegisterMapping[symTabID] = new RegisterList(*targets);
	}

	void setTargetsCallParam(int symTabID, RegisterList *targets) {
		_callParamsRegisterMapping[symTabID] = new RegisterList(*targets);
	}

	void setTargetsStackPointer(int symTabID, RegisterList *targets) {
		_stackPointerRegisterMapping[symTabID] = new RegisterList(*targets);
	}

	void setTargetsConditionCode(int symTabID, RegisterList *targets) {
		_conditionCodeRegisterMapping[symTabID] = new RegisterList(*targets);
	}

	void setTargetsConditionCode(IDList *ids, RegisterList *targets) {
		MyLinkedListIterator<int> *idIter = ids->elementsForward();
		while (idIter->hasMoreElements()) {
			int id = idIter->currentElement();

			setTargetsConditionCode(id, targets);

			idIter->nextElement();
		}
		delete idIter;
	}

	void setTargetsDoubleFloat(int symTabID, RegisterList *targets) {
		_doubleFloatRegisterMapping[symTabID] = new RegisterList(*targets);
	}

	void setTargetsDoubleFloat(IDList *ids, RegisterList *targets) {
		MyLinkedListIterator<int> *idIter = ids->elementsForward();
		while (idIter->hasMoreElements()) {
			int id = idIter->currentElement();

			setTargetsDoubleFloat(id, targets);

			idIter->nextElement();
		}
		delete idIter;
	}

	void print(ostream& out);

	void print() { print(cout); }
};

#endif // __REGISTER_MAPPING__
