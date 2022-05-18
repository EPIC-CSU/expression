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
// $Id: RegisterArg.h,v 1.21 1998/04/15 00:32:59 xji Exp $
//
// File:  		RegisterArg.h
// Created:		Fri Nov 01 , 96
// Last modified: 	Fri Nov 01 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __REGISTERARG_H__
#define __REGISTERARG_H__

#ifndef WIN32
#include <assert.h>
#endif

#include "BaseArgument.h"
#include "disamb.h"
#include "SSAArgProperty.h"
#include "PropertyIncludes.h"
#include <string.h>

extern int doneRA;

class RegisterArg : public BaseArgument
{
 private:
 protected:
   
   int _symVal; // symbol number
   char *_regText;
      
 public:
   RegisterArg()
      {
  	_symVal = 0;
	_regText = NULL;
      }

   RegisterArg(int symVal, char *regText)
      {
	 _symVal = symVal;
	 _regText = strdup(regText);
      }
	 
   virtual ~RegisterArg()
      {
	if (_regText!=NULL)
	  free(_regText);
      }

   BaseArgument *copy()
      {
       // we don't need it now
	assert(1==0);
	//NOS
	return NULL;
      }
   
   int isRegisterArg()
      {
	 return _YES_;
      }

   void setVal(int someVal)
      {
	 _symVal = someVal;
      }
   
   void setText(char *someText)
      {
	 if (_regText!=NULL)
	   free(_regText);
	 _regText = strdup(someText);
      }
   
   int getVal()
      {
	 return _symVal;
      }
   
   char *getText()
      {
	 return _regText;
   }
   
   virtual int isReturnRegArg()
   {
	   cout << "Do not use this method. Use isReturnParameter instead" << endl;
	   assert(0);

	   if (!doneRA)
	   {
		   if ((strcmp(_regText, "gR") == 0) && (_symVal == 4))
			   return _YES_;
	   }
	   else
	   {
		   cout << "Warning: Target Register set is not yet specified !" << endl;
		   if ((strcmp(_regText, "R") == 0) && (_symVal == 4))
			   return _YES_;
	   }

	   return _NO_;
   }

   virtual int getIDWithoutSSA()
   {
	   return _INVALID_;
   }

   virtual int getUniqID()
   {
	   return _INVALID_;
   }

   virtual int getValType() { return _INVALID_; }



   friend const int operator == (const RegisterArg & a, const RegisterArg & b)
      {
	 // perform the required comparisions. I have not yet figured
	 // out what the final data members of this class are going to
	 // be. i.e. do I need to store the register name, or should I
	 // just store the symval.

	 return _YES_;
      }

// This method should be removed.
//
   virtual void print(int type = 0);
   virtual void print(int type, ostream& out);
   virtual void printAsm(ostream &);

   virtual void irToC(FilePointer & fp, int indent);
   virtual void printCToString(MyString & s, char *tval = NULL);
   virtual void printC(int type = 0);
   char *createSymbol();

   virtual void printToString(char *buf);
   void printProp(ostream& out);
   virtual void print(ostream& out);
   virtual void printC(ostream& out);

   virtual void printProperties(ostream & out, PropNameList & pL);

   friend ostream & operator << (ostream & out, const RegisterArg & s);
   virtual expr BuildExpr(BaseOperation *o);

};

#endif
