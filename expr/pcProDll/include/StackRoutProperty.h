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
// $Id$
//
// File:  		StackRoutProperty.h
// Created:		Wed Nov 05, 97
// Last modified: 	Wed Nov 05, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ROUTINE, PROPERTY

#ifndef __STACKROUTPROPERTY_H__
#define __STACKROUTPROPERTY_H__

#include "PropertyIncludes.h"
#include "BaseRoutineProperty.h"

const int _GCC_STACK_ = 1997;
const int _EXPRESS_STACK_ = 2002;

class CallStackInfo
{
public:
	int _stackSize;

	int _saveRegsStartAddr;
	int _saveRegsEndAddr;
	int _numSaveInstrs;

	int _spillStartAddr;
	int _spillEndAddr;

	CallStackInfo() : _stackSize(_INVALID_), 
		_saveRegsStartAddr(_INVALID_), _saveRegsEndAddr(_INVALID_), _numSaveInstrs(_INVALID_), 
		_spillStartAddr(_INVALID_), _spillEndAddr(_INVALID_) {}
};

class StackRoutProperty : public BaseRoutineProperty
{
private:
	CallStackInfo _gccInfo;
	CallStackInfo _expressInfo;

#define GET_STACK(p) ((p == _GCC_STACK_) ? _gccInfo : _expressInfo)

protected:
public:
	StackRoutProperty() {}
	
	StackRoutProperty(const StackRoutProperty & someRProperty) : 
		_gccInfo(someRProperty._gccInfo), _expressInfo(someRProperty._expressInfo) {}
	
	virtual ~StackRoutProperty() {}
	
	BaseRoutineProperty *copy() const;
	
	void print(ostream&);
		
	friend ostream & operator << (ostream & out, const BaseRoutineProperty & s)
	{
		return out;
	}
	
   int is(PropertyNames name) { if (name==_STACKROUT_) return _YES_; return _NO_; }
		
	int isEq(BaseRoutineProperty* a) {
		return _NO_;
	}

	void setStackSize(int size, int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		st._stackSize = size;
	}

	void setSaveRegsStartAddr(int addr, int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		st._saveRegsStartAddr = addr;
	}

	void setSaveRegsEndAddr(int addr, int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		st._saveRegsEndAddr = addr;
	}

	void setNumSaveInstrs(int numSaves, int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		st._numSaveInstrs = numSaves;
	}

	void setSpillStartAddr(int addr, int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		st._spillStartAddr = addr;
	}

	void setSpillEndAddr(int addr, int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		st._spillEndAddr = addr;
	}

	int getNumSavesFromAddresses(int whichStack); // calculates the number of registers that can be saved
	// based on the start and end addresses. assuming that each register is of size 4 bytes.

	int getSaveRegsStartAddr(int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		return st._saveRegsStartAddr;
	}

	int getSaveRegsEndAddr(int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		return st._saveRegsEndAddr;
	}

	int getNumSaveInstrs(int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		return st._numSaveInstrs;
	}

	int getSpillStartAddr(int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		return st._spillStartAddr;
	}

	int getSpillEndAddr(int whichStack) // can be one of: _GCC_STACK_, or _EXPRESS_STACK_
	{
		CallStackInfo & st = GET_STACK(whichStack);
		return st._spillEndAddr;
	}
};

#endif
