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
//========================================================================
// File:		ExecNumBBProperty.h
// Created:		Jan. 9, 98
// Author:		Xiaomei Ji
//========================================================================

#ifndef __EXECNUMBBPROPERTY_H__
#define __EXECNUMBBPROPERTY_H__

#ifndef WIN32
#include <iostream.h>
#endif

#include "PropertyIncludes.h"
#include "BaseBBProperty.h"

class ExecNumBBProperty : public BaseBBProperty {
private:
	int num;
public:
	ExecNumBBProperty() {};
	ExecNumBBProperty(int execNum):num(execNum) {};
	virtual ~ExecNumBBProperty() {};
 
	ExecNumBBProperty(ExecNumBBProperty& prop) {
		num = prop.getExecNumOfBB();
 }

 ExecNumBBProperty& operator=( ExecNumBBProperty& prop) {
		num = prop.getExecNumOfBB();
		return *this;
 }
 
	int getExecNumOfBB() { return num; }
	void setExecNumOfBB(int execNum) { num = execNum; }
 
	int is(PropertyNames prop){if (prop==_EXECNUM_) return _YES_;return _NO_;}
 
	int isEq(BaseBBProperty* prop) {
		if ( prop->is(_EXECNUM_) ) {
			if ( num == ((ExecNumBBProperty*)prop)->getExecNumOfBB() )
				return _YES_;
			return _NO_;
		}
		return _NO_;
	}
 
 BaseBBProperty* copy();
 
	friend ostream& operator<< (ostream& out, ExecNumBBProperty& prop) {
		return out << "ExecNumBBProperty: " << prop.getExecNumOfBB() << endl;
	}
 
	void print() {
		cout << "ExecNumBBProperty: " << num << endl; 
  
	}
	
	void print( ostream& out ) {
		out << "ExecNumBBProperty: " << num << endl;
	}
};

#endif





