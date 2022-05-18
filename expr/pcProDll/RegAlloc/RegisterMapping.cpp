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
//: RegisterMapping.cpp
//
// File:  		RegisterMapping.cpp
// Created:		Wed Aug 16, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Provides the mapping between generic registers (used by compiler) and target registers (architecture dependent)
// 

#include "stdafx.h"

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "RegisterMapping.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


RegisterMapping::~RegisterMapping() {
	map<int, RegisterList *>::iterator iter;
	
	// general register mappings
	for (iter = _generalRegisterMapping.begin(); iter != _generalRegisterMapping.end(); iter++) {
		RegisterList *targets = (*iter).second;

		delete targets;
	}

	// call address register mappings
	if (_callAddressRegisterMapping) {
		delete _callAddressRegisterMapping;
	}

	// call ret register mappings
	for (iter = _callReturnRegisterMapping.begin(); iter != _callReturnRegisterMapping.end(); iter++) {
		RegisterList *targets = (*iter).second;

		delete targets;
	}

	// call param register mappings
	for (iter = _callParamsRegisterMapping.begin(); iter != _callParamsRegisterMapping.end(); iter++) {
		RegisterList *targets = (*iter).second;

		delete targets;
	}

	// stack pointer register mappings
	for (iter = _stackPointerRegisterMapping.begin(); iter != _stackPointerRegisterMapping.end(); iter++) {
		RegisterList *targets = (*iter).second;

		delete targets;
	}


	// condition code register mappings
	for (iter = _conditionCodeRegisterMapping.begin(); iter != _conditionCodeRegisterMapping.end(); iter++) {
		RegisterList *targets = (*iter).second;

		delete targets;
	}

	// double float register mappings
	for (iter = _doubleFloatRegisterMapping.begin(); iter != _doubleFloatRegisterMapping.end(); iter++) {
		RegisterList *targets = (*iter).second;

		delete targets;
	}
}

void RegisterMapping::print(ostream& out) {
	map<int, RegisterList *>::iterator iter;
	RegisterListIterator *targetIter;
	int genericRegIndex, targetRegIndex;
	
	// general register mappings
	out << "General register mappings:" << endl;

	for (iter = _generalRegisterMapping.begin(); iter != _generalRegisterMapping.end(); iter++) {
		genericRegIndex = (*iter).first;
		RegisterList *targets = (*iter).second;

		out << genericRegIndex << " -> ";

		targetIter = targets->elementsForward();
		while (targetIter->hasMoreElements()) {
			targetRegIndex = targetIter->currentElement();

			out << targetRegIndex << " ";

			targetIter->nextElement();
		}
		delete targetIter;
		
		out << endl << endl;
	}


	// call address register mappings
	if (_callAddressRegisterMapping) {
		out << "Call address register mappings:" << endl;
		
		RegisterList *targets = _callAddressRegisterMapping;
		
		out << "?" << " -> ";
		
		targetIter = targets->elementsForward();
		while (targetIter->hasMoreElements()) {
			targetRegIndex = targetIter->currentElement();
			
			out << targetRegIndex << " ";
			
			targetIter->nextElement();
		}
		delete targetIter;
		
		out << endl << endl;
	}


	// call ret register mappings
	out << "Call ret register mappings:" << endl;

	for (iter = _callReturnRegisterMapping.begin(); iter != _callReturnRegisterMapping.end(); iter++) {
		genericRegIndex = (*iter).first;
		RegisterList *targets = (*iter).second;

		out << genericRegIndex << " -> ";

		targetIter = targets->elementsForward();
		while (targetIter->hasMoreElements()) {
			targetRegIndex = targetIter->currentElement();

			out << targetRegIndex << " ";

			targetIter->nextElement();
		}
		delete targetIter;
		
		out << endl << endl;
	}


	// call param register mappings
	out << "Call params register mappings:" << endl;

	for (iter = _callParamsRegisterMapping.begin(); iter != _callParamsRegisterMapping.end(); iter++) {
		genericRegIndex = (*iter).first;
		RegisterList *targets = (*iter).second;

		out << genericRegIndex << " -> ";

		targetIter = targets->elementsForward();
		while (targetIter->hasMoreElements()) {
			targetRegIndex = targetIter->currentElement();

			out << targetRegIndex << " ";

			targetIter->nextElement();
		}
		delete targetIter;
		
		out << endl << endl;
	}


	// stack pointer register mappings
	out << "Stack pointer register mappings:" << endl;

	for (iter = _stackPointerRegisterMapping.begin(); iter != _stackPointerRegisterMapping.end(); iter++) {
		genericRegIndex = (*iter).first;
		RegisterList *targets = (*iter).second;

		out << genericRegIndex << " -> ";

		targetIter = targets->elementsForward();
		while (targetIter->hasMoreElements()) {
			targetRegIndex = targetIter->currentElement();

			out << targetRegIndex << " ";

			targetIter->nextElement();
		}
		delete targetIter;
		
		out << endl << endl;
	}


	// condition code register mappings
	out << "Condition code register mappings:" << endl;

	for (iter = _conditionCodeRegisterMapping.begin(); iter != _conditionCodeRegisterMapping.end(); iter++) {
		genericRegIndex = (*iter).first;
		RegisterList *targets = (*iter).second;

		out << genericRegIndex << " -> ";

		targetIter = targets->elementsForward();
		while (targetIter->hasMoreElements()) {
			targetRegIndex = targetIter->currentElement();

			out << targetRegIndex << " ";

			targetIter->nextElement();
		}
		delete targetIter;
		
		out << endl << endl;
	}

	// double float register mappings
	out << "Double float register mappings:" << endl;

	for (iter = _doubleFloatRegisterMapping.begin(); iter != _doubleFloatRegisterMapping.end(); iter++) {
		genericRegIndex = (*iter).first;
		RegisterList *targets = (*iter).second;

		out << genericRegIndex << " -> ";

		targetIter = targets->elementsForward();
		while (targetIter->hasMoreElements()) {
			targetRegIndex = targetIter->currentElement();

			out << targetRegIndex << " ";

			targetIter->nextElement();
		}
		delete targetIter;
		
		out << endl << endl;
	}
}