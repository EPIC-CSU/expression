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
// $Id: LatchRegArg.h,v 1.3 1998/01/22 23:58:57 pgrun Exp $
//
// File:  		LatchRegArg.h
// Created:		Fri Nov 01 , 96
// Last modified: 	Fri Nov 01 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __LATCHREGARG_H__
#define __LATCHREGARG_H__

#include "PseudoRegArg.h"

class LatchRegArg : public PseudoRegArg
{
private:
 
#ifdef COUNT_LATCHREGS
 static int _numLatchRegs;	// See Comment 1 below.
#endif
 
protected:
public:
 LatchRegArg()
 {
#ifdef COUNT_LATCHREGS
	 _numLatchRegs++;	
#endif
 }
 
 virtual ~LatchRegArg()
 {
#ifdef COUNT_LATCHREGS
	 _numLatchRegs--;
#endif
 } 
 
#ifdef COUNT_LATCHREGS
 static int numLatchRegs()
 {
	 return _numLatchRegs;
 }
#endif
 
 BaseArgument *copy();
 
 int isLatchRegArg()
 {
	 return _YES_;
 }
 
};

#endif

// Comment 1:
//
// _numLatchRegs is used to keep a count of the number of LatchRegArg
// objects in existence at any point. COUNT_LATCHREGS can be either
// defined or undefined in the file "ArgumentIncludes.h". The count
// information will be used for debugging (and/or statistical)
// purposes. It might be a good idea to allow the user to specify
// whether he needs this info. 
//
