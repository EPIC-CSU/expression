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
#ifndef __CNSTPROPERTY_H__
#define __CNSTPROPERTY_H__

#include "BaseArgProperty.h"
#include "PropertyIncludes.h"
#include "ComputeOpCodeStruct.h"

union multiVal{
	int _ival;
        float _fval;
        double _dval;
};


class CnstProperty : public BaseArgProperty {
   union multiVal  _val;
   enum constType  _type;

   union multiVal& getVal() { return _val; }

public:
   CnstProperty() { };
   CnstProperty(int val): _type(ICONSTTYPE) { _val._ival = val; } 
   CnstProperty(float val): _type(FCONSTTYPE) {_val._fval = val; }
   CnstProperty(double val): _type(DCONSTTYPE) {_val._dval = val; }
   CnstProperty(union multiVal val,enum constType type):_val(val),_type(type){};

   CnstProperty(CnstProperty& p) {
      _type = p.getType();
      _val = p.getVal(); 
   } 
   
    virtual ~CnstProperty() { };

   CnstProperty& operator=(CnstProperty& p ) {
      if ( *this == p )
         return *this;
      _type = p.getType();
      _val = p.getVal(); 
      return *this;
   }

   int operator == (CnstProperty& p ) {
      if ( _type != p.getType() )
         return _NO_;
      switch ( _type ) {
	case ICONSTTYPE:
           if ( _val._ival = p.getIConst() )
		return _YES_;
	case FCONSTTYPE:
           if ( _val._fval = p.getFConst() )
		return _YES_;
        case DCONSTTYPE:
           if ( _val._dval = p.getDConst() )
		return _YES_;
      }
      return _NO_;
   }

   enum constType getType() { return _type; }
   int getIConst() { return _val._ival; }
   float getFConst() { return _val._fval; }
   double getDConst() { return _val._dval; }

   BaseArgProperty* copy();

   int is(PropertyNames name) { if (name==_CNSTPROP_) return _YES_; return _NO_; }

   void print() {
      switch ( _type ) {
	case ICONSTTYPE:
   	   cout << "IVal:  " << _val._ival << endl;
           break;
	case FCONSTTYPE:
   	   cout << "FVal:  " << _val._fval << endl;
           break;
        case DCONSTTYPE:
   	   cout << "DVal:  " << _val._dval << endl;
           break;
      }
   }

   void print(ostream& out) {
      switch ( _type ) {
	case ICONSTTYPE:
   	   out << "IVal:  " << _val._ival << endl;
           break;
	case FCONSTTYPE:
   	   out << "FVal:  " << _val._fval << endl;
           break;
        case DCONSTTYPE:
   	   out << "DVal:  " << _val._dval << endl;
           break;
      }
   }

   friend ostream& operator<<(ostream&out, CnstProperty& p ) {
      switch ( p.getType() ) {
	case ICONSTTYPE:
   	   out << "IVal:  " << p.getIConst() << endl;
           break;
	case FCONSTTYPE:
   	   out << "FVal:  " << p.getFConst() << endl;
           break;
        case DCONSTTYPE:
   	   out << "DVal:  " << p.getDConst() << endl;
           break;
      }
	  return out;
   }

   int isEq(BaseArgProperty* a ) {
      if ( a->is(_CNSTPROP_) ) 
         return this->operator==((CnstProperty&)*a);
      return _NO_;
   }
};

#endif
