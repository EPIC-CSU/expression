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
// $Id: IConstArg.h,v 1.5 1998/03/21 01:18:52 ahalambi Exp $
//
// File:  		IConstArg.h
// Created:		Thu Nov 14 , 96
// Last modified: 	Thu Nov 14 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __ICONSTARG_H__
#define __ICONSTARG_H__

#include "ImmediateArg.h"
#include "IburgDefs.h"
#include "PropertyIncludes.h"

class IConstArg : public ImmediateArg
{
private:
 
#ifdef COUNT_ICONSTARGS
 static int _numIConstArgs;	// See Comment 1 below.
#endif
 
protected:
 int _iConstVal;
 
public:
 IConstArg() : _iConstVal(0)
 {
#ifdef COUNT_ICONSTARGS
	 _numIConstArgs++;	
#endif
 }
 
 IConstArg(int integerValue)
 {
	 _iConstVal = integerValue;
  
#ifdef COUNT_ICONSTARGS
	 _numIConstArgs++;	
#endif
 }
 
 IConstArg(const IConstArg & a) : _iConstVal(a._iConstVal)
 {
 }
 
 virtual ~IConstArg()
 {
#ifdef COUNT_ICONSTARGS
	 _numIConstArgs--;
#endif
 } 
 
#ifdef COUNT_ICONSTARGS
 static int numIConstArgs()
 {
	 return _numIConstArgs;
 }
#endif
 
 BaseArgument *copy();
 
 BaseArgument *copyBody();
 
 int isIConstArg()
 {
	 return _YES_;
 }
 
 friend const int operator == (const IConstArg & a, const IConstArg & b);
 friend const int operator == (const IConstArg & a, const int b);
 
 virtual int isEq(BaseArgument *eqPtr);
 
 friend ostream & operator << (ostream & out, const IConstArg & s);
 
 int value()
 {
	 return _iConstVal;
 }
 
 void changeValue(int iVal)
 {
	 _iConstVal = iVal;
 }
 
 virtual void print(ostream &out);
   virtual void printCToString(MyString & s, char *tval = NULL);
 virtual void printC(ostream &out);
 virtual void irToC(FilePointer & fp, int indent);

 virtual void print();
 virtual void printToString(char *buf);

   virtual void printProperties(ostream & out, PropNameList & pL);
 
 virtual expr BuildExpr(BaseOperation *o); //for disamb. Code in disamb.
 
 virtual char * Name(void);
 
 virtual NODEPTR_TYPE BuildIburgTree(BaseOperation *op,int essential);
};

#endif

// Comment 1:
//
// _numIConstArgs is used to keep a count of the number of IConstArg
// objects in existence at any point. COUNT_ICONSTARGS can be either
// defined or undefined in the file "ArgumentIncludes.h". The count
// information will be used for debugging (and/or statistical)
// purposes. It might be a good idea to allow the user to specify
// whether he needs this info. 
//
