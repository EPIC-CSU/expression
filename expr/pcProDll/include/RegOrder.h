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
#ifndef __REGORDER_H__
#define __REGORDER_H__

#include "PropertyIncludes.h"
#include "BaseArgProperty.h"


class RegOrderArgProperty : public BaseArgProperty {
private:
	int order;
public:
	RegOrderArgProperty() {};
	RegOrderArgProperty(int num) : order(num) {};
	~RegOrderArgProperty() {};
	
	RegOrderArgProperty(RegOrderArgProperty& prop) {
		order = prop.getOrder();
	}
 
	RegOrderArgProperty& operator=(RegOrderArgProperty& prop) {
		order = prop.getOrder();
		return *this;
	}
	
	BaseArgProperty* copy();
 
	int is(PropertyNames name) {
		if ( name==_REGORDER_) return _YES_; return _NO_;
	}
 
	void print(ostream& out) {
		out << " the reg alloc is R" << order << endl;
	}
 
	void print() {
		cout << " the reg alloc is R" << order << endl;
	}
 
	int isEq(BaseArgProperty* prop) {
		if ( prop->is(_REGORDER_) ) {
			if ( order == ((RegOrderArgProperty*)prop)->getOrder() )
				return _YES_;
			return _NO_;
		}	
		return _NO_;
	}
 
	void setOrder(int num) { order = num; }
	int getOrder() { return order; }
};

#endif
