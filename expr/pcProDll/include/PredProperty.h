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
#ifndef __PREDPROPERTY_H__
#define __PREDPROPERTY_H__

#include "BaseArgProperty.h"

class PredProperty : public BaseArgProperty
{
	private :
		bool _isnotInverted;

	public :
		PredProperty() : _isnotInverted(false) 
		{}
		
		PredProperty(bool sense) 
		{ _isnotInverted = sense; }
		
		PredProperty (PredProperty &a)
		{ _isnotInverted = a._isnotInverted; }

		PredProperty &operator=(PredProperty &a) 
		{ 
			if (*this == a)
				return *this;
			_isnotInverted = a._isnotInverted;
			return *this;
		}
		
		virtual ~PredProperty() 
		{};
		
		bool getIsnotInverted() 
		{ return _isnotInverted; }
		
		void setIsnotInverted(bool sense) 
		{ _isnotInverted = sense; }

		virtual BaseArgProperty *copy()
		{
			BaseArgProperty *newPredProp;
			newPredProp = new PredProperty(false);
			return newPredProp;
		}
				
		virtual int is(PropertyNames aName)
		{
			if (aName == _PRED_)
				return _YES_;
			return _NO_;
		}
	
		void print() 
		{ cout << "predicate sense : " << _isnotInverted << endl; }

		void print(ostream& out) 
		{ out << "predicate sense : " << _isnotInverted << endl; }

		friend ostream& operator<<(ostream&out, PredProperty& a )
		{ 
			out << "predicate sense : " << a._isnotInverted << endl; 
			return out;
		}
		
		int isEq(BaseArgProperty* a) 
		{
			if ( a->is(_PRED_) ) {
				return ( ((PredProperty*)a)->getIsnotInverted() == _isnotInverted);
			}
			return _NO_;
		}
		
		virtual int isPredProperty()
		{
			return _YES_;
		}
		
		int operator == (PredProperty &a) 
		{ return (_isnotInverted == a._isnotInverted); }
};
#endif