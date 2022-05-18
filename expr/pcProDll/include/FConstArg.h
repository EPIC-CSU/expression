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
// $Id: FConstArg.h,v 1.5 1998/03/27 00:01:09 wtang Exp $
//
// File:  		FConstArg.h
// Created:		Thu Nov 14 , 96
// Last modified: 	Thu Nov 14 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

// As of now, the float value is stored as a string. Later on, we
// could also store the value as a float/double. As of now, we have
// no methods to initialize the value given a float/double value (i.e.
// we have no functions to convert a float/double to string).
// Although changeValue method has been provided, this method should
// not be used (except for debugging/testing) as it is an immediate
// value.

#ifndef __FCONSTARG_H__
#define __FCONSTARG_H__

#include "ImmediateArg.h"
#include "IburgDefs.h"
#include "PropertyIncludes.h"

class FConstArg : public ImmediateArg
{
private:
 
#ifdef COUNT_FCONSTARGS
 static int _numFConstArgs;	// See Comment 1 below.
#endif
 
protected:
 char *_fConstVal;
 //
 // EVE stores float-const arguments as strings (?)
 // As of now, the same representation is being used. Change to
 // actual float later, if needed.
 
public:
 FConstArg()
 {
#ifdef COUNT_FCONSTARGS
	 _numFConstArgs++;	
#endif
	 _fConstVal = _NULL_;
 }
 
 
 FConstArg(const char *floatValue);
 
 virtual ~FConstArg()
 {
#ifdef COUNT_FCONSTARGS
	 _numFConstArgs--;
#endif
  if (_fConstVal!=NULL)
   free(_fConstVal);
 } 
 
#ifdef COUNT_FCONSTARGS
 static int numFConstArgs()
 {
	 return _numFConstArgs;
 }
#endif
 
 BaseArgument *copy();
 
 BaseArgument *copyBody();
 
 int isFConstArg()
 {
	 return _YES_;
 }

 int isEq(BaseArgument *arg);

 friend const int operator == (const FConstArg & a, const FConstArg & b);
 friend const int operator == (const FConstArg & a, const char *b);
 
 friend ostream & operator << (ostream & out, const FConstArg & s);
 
 char *textValue();
 
 // To return the value of the float immediate argument, we use atof
 // to convert it  from a string representation to a double value.
 // Note that the value returned is of type double.
 //
 double value();
 
 // Use changeValue method whenever you wish to change the value
 // held by an FConstArg object. changeValue checks to see if there
 // was any previous value. If there was, it frees the memory, and
 // then allocates new memory and copies the new value.
 //
 void changeValue(char *valString);

 virtual void print(ostream &out);
 virtual void print();
   virtual void printCToString(MyString & s, char *tval = NULL);
 virtual void printC(ostream & out);
 virtual void irToC(FilePointer & fp, int indent);

 virtual void printToString(char *buf); 

   virtual void printProperties(ostream & out, PropNameList & pL);

 virtual char * Name(void){
  return textValue();
 }
 
 virtual NODEPTR_TYPE BuildIburgTree(BaseOperation *op,int essential);
};

#endif

// Comment 1:
//
// _numFConstArgs is used to keep a count of the number of FConstArg
// objects in existence at any point. COUNT_FCONSTARGS can be either
// defined or undefined in the file "ArgumentIncludes.h". The count
// information will be used for debugging (and/or statistical)
// purposes. It might be a good idea to allow the user to specify
// whether he needs this info. 
//
