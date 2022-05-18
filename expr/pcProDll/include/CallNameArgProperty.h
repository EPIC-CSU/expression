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
// File:  		CallNameArgProperty.h
// Created:		Mon Jan 24, 00
// Last modified: 	Mon Jan 24, 00
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT
// Module: PROPERTY

#ifndef __CALLNAMEARGPROPERTY_H__
#define __CALLNAMEARGPROPERTY_H__

#include "PropertyIncludes.h"
#include "BaseArgProperty.h"
#include "Routine.h"

class CallNameArgProperty : public BaseArgProperty
{
private:
protected:
	int _isSysCall;
	int _index;
public:
	CallNameArgProperty() : _isSysCall(-1) { }
	CallNameArgProperty(Routine *rout) : _isSysCall(_NO_) { _index = (int)rout; }
	CallNameArgProperty(int sysI) : _isSysCall(_YES_), _index(sysI) { }
	CallNameArgProperty(const CallNameArgProperty & a) : _isSysCall(a._isSysCall), _index(a._index) { }
	~CallNameArgProperty(){ }

	virtual int isCallNameArgProperty() { return _YES_; }

	virtual int is(PropertyNames aName)
      {
		return ((aName == _CALLNAME_) ? _YES_ : _NO_);
      }

   virtual void print(ostream& out)
   {
	   if (_isSysCall == _YES_)
		   out << "SysCall : " << " : " << _index;
	   else if (_isSysCall == _NO_)
		   out << "Call : " << ((Routine *)_index)->getName() << " : " << _index;
	   cout << endl;
   }

   friend int operator == (const BaseArgProperty & a, const
			   BaseArgProperty & b)
      {
	 return _YES_;
      }
   //
   // TODO

   friend ostream & operator << (ostream & out, const BaseArgProperty & s)
	 {
	    return out;
	 }
   //
   // TODO

   virtual int isEq(BaseArgProperty* a) {
      return _NO_;
   } 
   //
   // TODO

   void setCallNamePtr(Routine *r) { _isSysCall = _NO_; _index = (int)r; }
   Routine *getCallNamePtr() { if (_isSysCall == _NO_) return ((Routine *)_index); return NULL; }
   char *getCallName() { if (_isSysCall == _NO_) return (((Routine *)_index)->getName()); return NULL; }

   void setSysCallIndex(int sysI) { _isSysCall = _YES_; _index = sysI; }
   int getSysCallIndex() { if (_isSysCall == _YES_) return _index; return _INVALID_; }

   int justGetIndex() { return _index; }

   BaseArgProperty *copy()
   {
	   BaseArgProperty *retVal = new CallNameArgProperty(*this);
	   return retVal;
   }
};

#endif

