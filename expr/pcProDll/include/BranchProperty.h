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
#ifndef __BRANCHPROPERTY_H__
#define __BRANCHPROPERTY_H__

#include "PropertyIncludes.h"
#include "BaseOpProperty.h"

class BranchProperty : public BaseOpProperty {
   int _condition; // the possibility of jumping to true branch
public:
   BranchProperty() { };
   BranchProperty(int cond):_condition(cond){};
   BranchProperty(BranchProperty& p) { _condition = p.getCond(); }
   BranchProperty& operator=(BranchProperty& p) { 
      if ( *this == p )
         return *this;
      _condition = p.getCond();
      return *this;
   }

   virtual ~BranchProperty() {};

   int getCond() { return _condition; }
   void setCond( int cond) { _condition = cond; }
   
   BaseOpProperty* copy() const;

   int is(PropertyNames name) { if ( name==_BRANCH_) return _YES_; return _NO_; }

   void print() {
      cout << " Possibility of Jumping to True: " << _condition << endl;
   }

   void print(ostream &out) {
      out << " Possibility of Jumping to True: " << _condition << endl;
   }

   friend ostream& operator << (ostream& out, BranchProperty& p) {
      out << " Possibility of Jumping to True: " << p.getCond() << endl;
	return out;
   }

   int isEq(BaseOpProperty* a) {
	if ( a->is(_BRANCH_) ) {
	   return ( ((BranchProperty*)a)->getCond() == _condition );
        }
        return _NO_;
   }
};
#endif

