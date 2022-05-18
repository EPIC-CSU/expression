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
#ifndef __REGPRESSURE_H__
#define __REGPRESSURE_H__

#include "PropertyIncludes.h"
#include "BaseInstrProperty.h"


// The RegisterPressure property records the number of live variables of each class.
// For example, the number of floating-point variables, integer variables, compiler intermediates etc.
//
// Note that the register pressure property records the register pressure at the end of each instruction.
// i.e. it also includes the registers that are live within that instruction.
//
//
class RegPressProperty : public BaseInstrProperty {
private:

	Map<int, float> _regPressures;
	//
	// The index of the map indicates the regsiter file (or type).
	// This index is derived from the globalRegisterFile data object.
	//
	// The current register classes supported in the RegisterFile data structure are:
	// integer (R), condition code (cc), stack pointer (sp), frame pointer (fp), floating-point (f),
	// and compiler temporaries (temp)
	//
	// The value contained in the map indicates the current register pressure for that particular
	// register file type.

public:
	RegPressProperty() {};
	RegPressProperty(const RegPressProperty& prop) : _regPressures(prop._regPressures){}
	~RegPressProperty() {};
 
	

	void clear() { _regPressures.clear(); }
	
	RegPressProperty& operator=(RegPressProperty& prop);
	
	BaseInstrProperty* copy();
 
//	RegPressProperty(RegPressProperty& prop) {
//		unAllocVarNum = prop.getVarNum();
//		availRegNum = prop.getRegNum();
//	}
 
 
	int is(PropertyNames name) {
		if ( name == _REGPRESSURE_ )
			return _YES_;
		return _NO_;
	}
 
	void setRegisterPressure(int index, int numLive)
	{
		_regPressures[index] = (float)numLive;
	}

	void setRegisterPressure(int index, float press)
	{
		_regPressures[index] = press;
	}

	void addToRegisterPressure(int index, int change)
	{
		_regPressures[index] += (float)change;
	}

	void addToRegisterPressure(int index, float change)
	{
		_regPressures[index] += change;
	}

	void subFromRegisterPressure(int index, int change)
	{
		_regPressures[index] -= (float)change;
	}

	void subFromRegisterPressure(int index, float change)
	{
		_regPressures[index] -= (float)change;
	}

	int getNumLive(int index)
	{
		Map<int, float>::iterator i;
		if ((i = _regPressures.find(index)) != _regPressures.end())
			return ((int)((*i).second));

		return _INVALID_;
	}

	float getPressure(int index);

	void print() {
		// TBD
		//
		cout << "Method to be completed" << endl;
	}
 
	void print(ostream& out) {
		// TBD
		//
		out << "Method to be completed" << endl;
	}
 
 
	int isEq(BaseInstrProperty* prop) {

		// TBD
		//
		assert(0);

		if ( prop->is(_REGPRESSURE_) ) {
			return _NO_;
		}
		return _NO_;
	}
};

#endif

