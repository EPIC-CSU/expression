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
// $Id: VirtualRegArg.h,v 1.4 1998/01/22 23:59:49 pgrun Exp $
//
// File:  		VirtualRegArg.h
// Created:		Fri Nov 01 , 96
// Last modified: 	Fri Nov 01 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __VIRTUALREGARG_H__
#define __VIRTUALREGARG_H__

#include "PseudoRegArg.h"

class VirtualRegArg : public PseudoRegArg
{
private:
 
#ifdef COUNT_VIRTUALREGS
 static int _numVirtualRegs;	// See Comment 1 below.
#endif
 
protected:
public:
 VirtualRegArg()
 {
#ifdef COUNT_VIRTUALREGS
	 _numVirtualRegs++;	
#endif
 }
 
 VirtualRegArg(int symVal, char *regText)
 {
  RegisterArg::RegisterArg(symVal, regText);
 }
 
 virtual ~VirtualRegArg()
 {
#ifdef COUNT_VIRTUALREGS
	 _numVirtualRegs--;
#endif
 } 
 
#ifdef COUNT_VIRTUALREGS
 static int numVirtualRegs()
 {
	 return _numVirtualRegs;
 }
#endif
 
 BaseArgument *copy();
 
 BaseArgument *copyBody();
 
 int isVirtualRegArg()
 {
	 return _YES_;
 }
 
 friend ostream & operator << (ostream & out, const VirtualRegArg& s)
 {
	 out << "Register Number: " << s._symVal;
	 
	 return out;
 }
 
	void print() {
	 cout << "Register Number: " << this->_symVal;
	}
};

#endif

// Comment 1:
//
// _numVirtualRegs is used to keep a count of the number of VirtualRegArg
// objects in existence at any point. COUNT_VIRTUALREGS can be either
// defined or undefined in the file "ArgumentIncludes.h". The count
// information will be used for debugging (and/or statistical)
// purposes. It might be a good idea to allow the user to specify
// whether he needs this info. 
//
