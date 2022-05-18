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
//: MultiChainProperty.h
//
// File:  		MultiChainProperty.h
// Created:		Fri Oct 01, 1999
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// MultiChainProperty implements the multichain property.
// 

#ifndef __MULTICHAINPROPERTY_H__
#define __MULTICHAINPROPERTY_H__

#include "BaseArgProperty.h"

class MultiChain;

class MultiChainProperty : public BaseArgProperty {
	MultiChain *_pMC;

public:
	MultiChainProperty() {}
	MultiChainProperty(MultiChain *pMC) { _pMC = pMC; }
	MultiChainProperty(MultiChain& mChain) { _pMC = &mChain; }
	MultiChainProperty(MultiChainProperty& mcProp) { _pMC = mcProp.getMC(); }
	
	virtual ~MultiChainProperty() {}

	int isEq(BaseArgProperty* prop) {
		if (prop->is(_MULTICHAIN_)) {
			return _pMC->isEq(((MultiChainProperty *) prop)->getMC());
		} else
			return _NO_;
	}

	BaseArgProperty* copy()
	{
		return new MultiChainProperty(*this);
	}

	void setMC(MultiChain *pMC) {
		_pMC = pMC;
	}

	MultiChain * getMC() {
		return _pMC;
	}

	int is(PropertyNames name) { if (name==_MULTICHAIN_) return _YES_; return _NO_; }
 
	void print() { cout << *_pMC; }
	void print(ostream& out) { out << *_pMC; }

	friend ostream& operator<<(ostream& out, MultiChainProperty& mcProp) {
		out << *(mcProp.getMC());
		return out;
	}

};

#endif // __MULTICHAINPROPERTY_H__