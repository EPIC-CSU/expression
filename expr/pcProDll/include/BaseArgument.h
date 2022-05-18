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
// $Id: BaseArgument.h,v 1.13 1998/03/27 00:01:06 wtang Exp $
//
// File:  		BaseArgument.h
// Created:		Fri Nov 01 , 96
// Last modified: 	Fri Nov 01 , 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#ifndef __BASEARGUMENT_H__
#define __BASEARGUMENT_H__

#include "ArgumentIncludes.h"
#include "BaseArgProperty.h"
#include "PropertyList.h"
#include "BaseOperation.h"
#include "disamb.h"
#include "IburgDefs.h"
#include "PropertyIncludes.h"
#include "FilePointer.h"

class BaseArgument
{
 private:
 protected:
   PropertyList<BaseArgProperty> _propertyPtr;

 public:
   BaseArgument()
      {
      }
   
   virtual ~BaseArgument()
      {
      }
 
   friend const int operator == (const BaseArgument & a,
			   const BaseArgument & b)
      {
	 // As of now, there is nothing in here to compare.
	 return _TRUE_;
      }

   virtual void print()
      {
	 /* if (_propertyPtr != NULL) */
	    _propertyPtr.print();
      }

   virtual int testIfLibraryFunction() // labelArg defines it
   {
	   return 0;
   }

   virtual void print(ostream& out)
      {
	 /* if (_propertyPtr != NULL) */
	    _propertyPtr.print(out);
      }

   virtual void printCToString(MyString & s, char *tval = NULL){
   }

   virtual void printC(ostream& out)
      {
	 /* if (_propertyPtr != NULL) */
	    _propertyPtr.print(out);
      }

   virtual void irToC(FilePointer & fp, int indent){
   }

   virtual void printProperties(ostream & out, PropNameList & pL)
   {
	   BaseArgProperty *prop;

	   // out << " {<";
	   PropNameListIter i;
	   for (i = pL.begin(); i != pL.end(); i++)
	   {
		   prop = _propertyPtr.getPtrToProperty(*i);
		   if (prop != NULL)
			   prop->print(out);
	   }
	   // out << " >} ";
   }

   virtual void printToString(char *buf)
      {
      }
   
   friend ostream & operator << (ostream & out, const BaseArgument & s)
      {
	 return out;
      }
                                         
   virtual BaseArgument *copy() = 0;

   virtual BaseArgument *copyBody()
      {
	 return NULL;
      }
   
   virtual int isEq(BaseArgument *eqPtr)
      {
	 return _NO_;
      }
   
   virtual void addProperty(BaseArgProperty & someProp)
      {
	   BaseArgProperty * prop;
	   prop=&someProp;
	   _propertyPtr.addProperty(someProp);
      }

   virtual void removeProperty(BaseArgProperty & someProp)
      {
	 _propertyPtr.removeProperty(someProp);
      }

   virtual void removeProperty(BaseArgProperty* someProp)
      {
         if ( someProp )
	    _propertyPtr.removeProperty(*someProp);
      }


   virtual void setRegFile(int regFileIndex)
      {
      }
   
   virtual BaseArgProperty & getProperty(PropertyNames aName)
      {
/*	 if (_propertyPtr == NULL)
	    {
	       //signal error condition.
		  
	       return NULL;
	    }
*/
	 return _propertyPtr.getProperty(aName);
      }

   virtual BaseArgProperty *getPtrToProperty(PropertyNames aName)
     {
       return _propertyPtr.getPtrToProperty(aName);
     }
   
   virtual char *lName()
      {
	 return NULL;
      }

   virtual int getIDWithoutSSA()
   {
	   return _INVALID_;
   }

   virtual int getUniqID()
   {
	   return _INVALID_;
   }
   
   virtual int isEqual(char *lName)
      {
	 return _NO_;
      }
   
   virtual int isRegisterArg()
      {
	 return _NO_;
      }

   virtual int isLabelArg()
      {
	 return _NO_;
      }

   virtual int isImmediateArg()
      {
	 return _NO_;
      }

   virtual int isRealRegArg()
      {
	 return _NO_;
      }

   virtual int isPseudoRegArg()
      {
	 return _NO_;
      }

   virtual int isVirtualRegArg()
      {
	 return _NO_;
      }

   virtual int isLatchRegArg()
      {
	 return _NO_;
      }

   virtual int isIConstArg()
      {
	 return _NO_;
      }

   virtual int isFConstArg()
      {
	 return _NO_;
      }

   virtual int isIntegerReg()
      {
	 return _NO_;
      }

   virtual int isFloatReg()
      {
	 return _NO_;
      }
   
   virtual expr BuildExpr(BaseOperation *o)
     {
	   //NOS
	   return NULL;
     } 

   LinkedListIterator<BaseArgProperty>* propIterForw() {
      return _propertyPtr.elementsForward();
   }

   PropertyList<BaseArgProperty> & getPropertyList() {
	   return _propertyPtr;
   }

   virtual void recursiveRemoveProperty(PropertyNames name)
   {
	   _propertyPtr.removeProperty(name);
   }

   virtual char * Name(void){
	   return NULL;
   }

   virtual void renameValue(){}
   
   virtual NODEPTR_TYPE BuildIburgTree(BaseOperation *op,int essential){
	   cout << "This function shouldn't be called";
	   abort();
	   return NULL;
   }
};

typedef Vector<BaseArgument *> ArgList;
typedef Vector<BaseArgument *>::iterator ArgListIter;

#endif
