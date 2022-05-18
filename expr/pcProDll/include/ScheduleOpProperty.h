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
#ifndef __SCHEDULEOPPROPERTY_H__
#define __SCHEDULEOPPROPERTY_H__

#include "PropertyIncludes.h"
#include "BaseOpProperty.h"
#include "BaseInstrProperty.h"

const int _NOCODEMOTION_ = 7219;
const int _LOWPRIORITY_ = 8168;
const int _HIGHPRIORITY_ = 2248;
const int _BARRIER_ = 4079;

const int _DEFAULT_ALL_ = 8086;

class ScheduleOpProperty : public BaseOpProperty 
{
private:
	int _flag; // Can be one of _NOCODEMOTION_, _LOWPRIORITY_, _HIGHPRIORITY_, _BARRIER_
	int _code; // This code variable can be used to further identify the restrictions on code motion.
	//
	// For example, the code could specify that the particular op/instr is a barrier only for those instrs/ops that contain the same code.

public:
	ScheduleOpProperty() : _flag(_INVALID_), _code(_DEFAULT_ALL_) {}
	ScheduleOpProperty(int flg, int cd = _DEFAULT_ALL_) : _flag(flg), _code(cd) {}
	ScheduleOpProperty(const ScheduleOpProperty & b) : _flag(b._flag), _code(b._code) {}

	virtual ~ScheduleOpProperty () {}
	
	BaseOpProperty* copy() const
	{
		BaseOpProperty *retPtr = new ScheduleOpProperty(*this);

		return retPtr;
	}
	
	int is(PropertyNames name) { if (name==_SCHEDULEOP_) return _YES_; return _NO_; }
	
	void setScheduleFlag(int flg) { _flag = flg; }
	int getScheduleFlag() { return _flag; }
	
	void setCode(int cd) { _code = cd; }
	int getCode() { return _code; }

	void print() { cout << " Schedule Option: " << _flag << " ; Code: " << _code << endl; }
	void print(ostream& out) { cout << " Schedule Option: " << _flag << " ; Code: " << _code << endl; }
	friend ostream& operator<<(ostream& out, ScheduleOpProperty & p) 
	{
		out << " Schedule Option: " << p._flag << " ; Code: " << p._code << endl;
		return out;
	}
	
	int isEq(BaseOpProperty* a) 
	{
		if ( a->is(_SCHEDULEOP_) )
		{
			if (((ScheduleOpProperty *)a)->_flag == _flag)
			{
				if (((ScheduleOpProperty *)a)->_code == _code)
					return _YES_;

				if ((_code == _DEFAULT_ALL_) || (((ScheduleOpProperty *)a)->_code == _DEFAULT_ALL_))
					return _YES_;
			}
		}
		return _NO_;
	}
};

class ScheduleInstrProperty : public BaseInstrProperty
{
private:
	int _flag; // Can be one of _NOCODEMOTION_, _LOWPRIORITY_, _HIGHPRIORITY_, _BARRIER_
	int _code; // This code variable can be used to further identify the restrictions on code motion.
	//
	// For example, the code could specify that the particular op/instr is a barrier only for those instrs/ops that contain the same code.
public:
	ScheduleInstrProperty() : _flag(_INVALID_), _code(_DEFAULT_ALL_)  {}
	ScheduleInstrProperty(int flg, int cd = _DEFAULT_ALL_) : _flag(flg), _code(cd)  {}
	ScheduleInstrProperty(const ScheduleInstrProperty & b) : _flag(b._flag), _code(b._code) {}

	virtual ~ScheduleInstrProperty () {}
	
	BaseInstrProperty* copy() 
	{
		BaseInstrProperty *retPtr = new ScheduleInstrProperty(*this);

		return retPtr;
	}
	
	int is(PropertyNames name) { if (name==_SCHEDULEINSTR_) return _YES_; return _NO_; }
	
	void setScheduleFlag(int flg) { _flag = flg; }
	int getScheduleFlag() { return _flag; }

	void setCode(int cd) { _code = cd; }
	int getCode() { return _code; }

	void print() { cout << " Schedule Option: " << _flag << " ; Code: " << _code << endl; }
	void print(ostream& out) { cout << " Schedule Option: " << _flag << " ; Code: " << _code << endl; }
	friend ostream& operator<<(ostream& out, ScheduleInstrProperty & p) 
	{
		out << " Schedule Option: " << p._flag << " ; Code: " << p._code << endl;
		return out;
	}
	
	int isEq(BaseOpProperty* a) 
	{
		if ( a->is(_SCHEDULEINSTR_) )
		{
			if (((ScheduleInstrProperty *)a)->_flag == _flag)
			{
				if (((ScheduleInstrProperty *)a)->_code == _code)
					return _YES_;

				if ((_code == _DEFAULT_ALL_) || (((ScheduleInstrProperty *)a)->_code == _DEFAULT_ALL_))
					return _YES_;
			}
		}
		return _NO_;
	}
};

#endif

