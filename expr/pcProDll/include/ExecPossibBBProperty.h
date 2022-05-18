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
//=======================================================================
// File:		ExecPossibBBProperty.h
// Created:		Jan. 9, 98
// Author:		Xiaomei Ji
//======================================================================

#ifndef _EXECPOSSIBBBPROPERTY_H__
#define _EXECPOSSIBBBPROPERTY_H__

#ifndef WIN32
#include <iostream.h>
#endif

#include "PropertyIncludes.h"
#include "BaseBBProperty.h"

class ExecPossibBBProperty : public BaseBBProperty {
private:
	int percent;
public:
	ExecPossibBBProperty() {};
	ExecPossibBBProperty(int possib) : percent(possib) {};
	virtual ~ExecPossibBBProperty() {};

	ExecPossibBBProperty(ExecPossibBBProperty& prop) {
		percent = prop.getPossibility();
	}
	
	ExecPossibBBProperty& operator=( ExecPossibBBProperty& prop) {
		percent = prop.getPossibility();
	}

	int getPossibility() { return percent; }
	void setPossibility(int possib) { percent = possib; }

	int is(PropertyNames name) { if ( name==_EXECPOSSIBILITY_ ) return _YES_; return _NO_; }

	int isEq(BaseBBProperty* prop) {
		if ( prop->is(_EXECPOSSIBILITY_) ) {
			if ( ((ExecPossibBBProperty*)prop)->getPossibility() == percent )
				return _YES_;
			return _NO_;
		}
		return _NO_;
	} 

	BaseBBProperty* copy();

	friend ostream& operator<<(ostream& out, ExecPossibBBProperty& prop) {
		return out<<"ExecPossibility: " << prop.getPossibility() <<endl;
	}

	void print() {
		cout << "ExecPossibility: " << percent << endl;
	}

	void print(ostream& out ) {
		out << "ExecPossibility: " <<percent << endl;
	}
};


#endif




