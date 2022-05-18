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

#ifndef _ASSERTIONS_H_
#define _ASSERTIONS_H_


#include "GlobalIncludes.h"
#include "BaseArgProperty.h"
#include "MyString.h"


//holds the assertions (name+value)
struct Assertion{
	char *name;
	char *value;
};


class HLInfoArgAssertion : public BaseArgProperty 
{
protected:
	MyString _name;
	MyString _value;
public:
	HLInfoArgAssertion() { }
	HLInfoArgAssertion(char *name, char *value) : _name(name), _value(value) { }
	HLInfoArgAssertion(const HLInfoArgAssertion & a) : _name(a._name), _value(a._value) { }
	~HLInfoArgAssertion() { }
	
	int isHLInfoArgAssertion() { return _YES_; }

	BaseArgProperty *copy()
	{
		BaseArgProperty *retptr = new HLInfoArgAssertion(*this);
		return retptr;
	}

	int is(PropertyNames aName)
	{
		if (aName == _HLINFO_) return _YES_;
		return _NO_;
	}

	void print(ostream & out)
	{
		out << " (** HLInfo: ";
		_name.print(out);
		out << " ";
		_value.print(out);
		out << " **) ";
	}

	void setName(char *str) { _name = str; }
	void setValue(char *str) { _value = str; }

	void setName(MyString & str) { _name = str; }
	void setValue(MyString & str) { _value = str; }

	MyString & getName() { return _name; }
	MyString & getValue() { return _value; }

};

typedef int RT_TYPE;
typedef int RT_VALUE;

class RTInfoAssertion
{
protected:
	RT_TYPE _type; // hard real time constraint, hint, soft, etc.
	RT_VALUE _time; // this could be either interms of processor clock cycles or actual real time.

public:
	RTInfoAssertion() : _time(_INVALID_) { }
	RTInfoAssertion(RT_TYPE tp, RT_VALUE ti) : _type(tp), _time(ti) { }
	~RTInfoAssertion() { }

	void setRTType(RT_TYPE tp) { _type = tp; }
	void setRTValue(RT_VALUE ti) { _time = ti; }

	RT_TYPE getRTType() { return _type; }
	RT_VALUE getRTValue() { return _time; }

	void print(ostream & out)
	{
		out << "RTInfo: Type: " << _type << " Value: " << _time;
	}
};

#endif
