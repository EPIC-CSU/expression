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
// $Id: ImmediateArg.h,v 1.9 1998/01/22 23:58:51 pgrun Exp $
//
// File:  		ImmediateArg.h
// Created:		Thu Nov 14 , 96
// Last modified: 	Thu Nov 13 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __IMMEDIATEARG_H__
#define __IMMEDIATEARG_H__

#include "BaseArgument.h"
#include "disamb.h"

class ImmediateArg : public BaseArgument
{
private:
 
#ifdef COUNT_IMMEDIATEARGS
 static int _numImmediateArgs;	// See Comment 1 below.
#endif
 
protected:
 //      int _intNum;
 
public:
 ImmediateArg()
 {
#ifdef COUNT_IMMEDIATEARGS
	 _numImmediateArgs++;	
#endif
 }
 
 ImmediateArg(const ImmediateArg & a)
 {
  //	 _intNum = a._intNum;
	 
#ifdef COUNT_IMMEDIATEARGS
	 _numImmediateArgs++;	
#endif
 }
 
 virtual ~ImmediateArg()
 {
#ifdef COUNT_IMMEDIATEARGS
	 _numImmediateArgs--;
#endif
 } 
 
#ifdef COUNT_IMMEDIATEARGS
 static int numImmediateArgs()
 {
	 return _numImmediateArgs;
 }
#endif
 
 int isImmediateArg()
 {
	 return _YES_;
 }
 
 BaseArgument *copy();
 
 friend const int operator == (const ImmediateArg & a, const ImmediateArg & b)
 {
	 return _YES_;
 }
 
 virtual int isEq(BaseArgument *eqPtr)
 {
	 return _NO_;
 }
 
 friend ostream & operator << (ostream & out, const ImmediateArg & s)
 {
	 return out;
 }
 //
 // Rewrite the operators == and << later.
 
 
 virtual void print()
 {
 }
 
 virtual void print(ostream& out)
 {
 }
 
};

#endif

// Comment 1:
//
// _numImmediateArgs is used to keep a count of the number of ImmediateArg
// objects in existence at any point. COUNT_IMMEDIATEARGS can be either
// defined or undefined in the file "ArgumentIncludes.h". The count
// information will be used for debugging (and/or statistical)
// purposes. It might be a good idea to allow the user to specify
// whether he needs this info. 
//
