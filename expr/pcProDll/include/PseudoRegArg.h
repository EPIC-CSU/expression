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
// $Id: PseudoRegArg.h,v 1.5 1998/01/22 23:59:30 pgrun Exp $
//
// File:  		PseudoRegArg.h
// Created:		Fri Nov 01 , 96
// Last modified: 	Fri Nov 01 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __PSEUDOREGARG_H__
#define __PSEUDOREGARG_H__

#include "RegisterArg.h"

class PseudoRegArg : public RegisterArg
{
 private:

#ifdef COUNT_PSEUDOREGS
   static int _numPseudoRegs;  // See Comment 1 below.
#endif
   
 protected:
 public:
   PseudoRegArg()
      {
#ifdef COUNT_PSEUDOREGS
	 _numPseudoRegs++;	
#endif
      }

   virtual ~PseudoRegArg()
      {
#ifdef COUNT_PSEUDOREGS
	 _numPseudoRegs--;	
#endif
      }

#ifdef COUNT_PSEUDOREGS
   static int numPseudoRegs()
      {
	 return _numPseudoRegs;
      }
#endif

   int isPseudoRegArg()
      {
	 return _YES_;
      }


   friend int operator == (const PseudoRegArg & a, const PseudoRegArg & b)
      {
	 return _YES_;
      }
   
   friend ostream& operator << (ostream & out, const PseudoRegArg & s)
      {
	 out << "Register Num: " << s._symVal << endl;
	 return out;
      }

	void print() {
	 cout << "Register Num: " << this->_symVal << endl;
	}
//
// Rewrite the operators == and << later.

};

#endif

// Comment 1:
//
// _numPseudoRegs is used to keep a count of the number of PseudoRegArg
// objects in existence at any point. COUNT_PSEUDOREGS can be either
// defined or undefined in the file "ArgumentIncludes.h". The count
// information will be used for debugging (and/or statistical)
// purposes. It might be a good idea to allow the user to specify
// whether he needs this info. 
//
