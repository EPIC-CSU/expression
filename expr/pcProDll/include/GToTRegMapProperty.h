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
//: GToTRegMapProperty.h
//
// File:  		GToTRegMapProperty.h
// Created:		Tue Feb 08, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// This property contains a map between generic arguments (virtual registers)
// and real registers, as required by the actual architecture.
// The generic and real register are specified using their index in the symbol
// table (note: this is different from the ID, after SSA).
//

#ifndef __GTOTREGMAPPROPERTY_H__
#define __GTOTREGMAPPROPERTY_H__

#include "BaseRoutineProperty.h"

//typedef map<int, int> REG_MAP;

// soft constraint means that a generic register should always be allocated to the target register
// hard constraint means that a generic register and only it should allocated to the target register (nothing else)
// e.g. of hard register constraint: R0 - R0

class GToTRegMapProperty : public BaseRoutineProperty {

	map<int, int> _regMap;
	map<int, int> _hardConstraintMap;

public:
	GToTRegMapProperty() {}
	//GToTRegMapProperty(const GToTRegMapProperty& regMapProp) : _regMap(regMapProp._regMap) {}

	virtual ~GToTRegMapProperty() {}

	BaseRoutineProperty* copy() const { return new GToTRegMapProperty(*this); }

	void setAssoc(int genReg, int realReg, int hardConstraint = 0) {
		_regMap[genReg] = realReg;
		_hardConstraintMap[genReg] = hardConstraint;
	}
	int getAssoc(int genReg)
	{ 
		if (_regMap.find(genReg) != _regMap.end())
			return _regMap[genReg];

		return -1;
	}

	int isHardConstraint(int genReg)
	{ 
		return _hardConstraintMap[genReg];
	}

	map<int, int>& getRegMap() { return _regMap; }
	map<int, int>& getHardConstraintMap() { return _hardConstraintMap; }

	int is(PropertyNames name) { if (name == _G_TO_T_REG_MAP_) return _YES_; return _NO_; }
 
	void print() { print(cout); }
	void print(ostream& out);

	friend ostream& operator<<(ostream& out, GToTRegMapProperty& mapProp) {
		mapProp.print(out);
		return out;
	}

	int isEq(BaseRoutineProperty* a) {
		if (a->is(_G_TO_T_REG_MAP_)) {
			if (_regMap == ((GToTRegMapProperty *) a)->_regMap)
				return true;
		}
		return false;
	}
};

#endif // __GTOTREGMAPPROPERTY_H__