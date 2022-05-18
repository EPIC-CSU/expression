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
// $Id: LabelArg.h,v 1.7 1998/03/21 01:18:59 ahalambi Exp $
//
// File:  		LabelArg.h
// Created:		Mon Jan 13, 97
// Last modified: 	Mon Jan 13, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __LABELARG_H__
#define __LABELARG_H__

#include "BaseArgument.h"
#include "PropertyIncludes.h"

class LabelArg : public BaseArgument
{
private:
 
#ifdef COUNT_LABELARGS
 static int _numLabelArgs;	// See Comment 1 below.
#endif
 
protected:
 char * _labelName;
 
public:
 LabelArg();
 LabelArg(char *lName);
 LabelArg(const LabelArg & s);
 virtual ~LabelArg();
 
#ifdef COUNT_LABELARGS
 static int numLabelArgs()
 {
	 return _numLabelArgs;
 }
#endif
 
 BaseArgument *copy();
 
 BaseArgument *copyBody();
 
 void initLabel(char *lName);
 
 int isEq(BaseArgument *arg);

 int isEqual(char *lName);
 
 int isLabelArg()
 {
  return _YES_;
 }
 
 friend const int operator == (const LabelArg & a, const LabelArg & b);
 friend const int operator == (const LabelArg & a, const char *b);
 
 void print();
 // for ASM
 int testIfLibraryFunction(void);
 void print(ostream& out);
 void printAsm(ostream& out);
   virtual void printCToString(MyString & s, char *tval = NULL);
 void printC(ostream& out);
 virtual void irToC(FilePointer & fp, int indent);

 virtual void printToString(char *buf);

   virtual void printProperties(ostream & out, PropNameList & pL);

   friend ostream & operator << (ostream & out, const LabelArg & s);
 
 char *lName();
 char *Name()
 {
	 return _labelName;
 }

 NODEPTR_TYPE BuildIburgTree(BaseOperation *o,int essential);
};

#endif

// Comment 1:
//
// _numLabelArgs is used to keep a count of the number of LabelArg
// objects in existence at any point. COUNT_LABELARGS can be either
// defined or undefined in the file "ArgumentIncludes.h". The count
// information will be used for debugging (and/or statistical)
// purposes. It might be a good idea to allow the user to specify
// whether he needs this info. 
//
