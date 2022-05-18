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
// $Id: IConstArg.cc,v 1.4 1998/03/21 01:16:24 ahalambi Exp $
//
// File:  		IConstArg.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#include "stdafx.h"

#include "IConstArg.h"
#include "TreeBuilding.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_ICONSTARGS
int IConstArg::_numIConstArgs = 0;	
#endif

extern void printIndent(FilePointer & fp, int indent);

BaseArgument *IConstArg::copyBody()
{
 BaseArgument *retPtr;
 retPtr = new IConstArg(*this);
 
 // Currently, we do not copy any properties of the argument.
 /*
 MyLinkedList<PropertyNames> propList;
 propList.appendElement(<insert_property_name_here>
 
   (retPtr->getPropertyList()).makeACopyOf(_propertyPtr, propList);
 */
 
 return retPtr;
}

const int operator == (const IConstArg & a, const IConstArg & b)
{
 if (a._iConstVal == b._iConstVal)
 {
	 return _YES_;
 }
 return _NO_;
}

const int operator == (const IConstArg & a, const int b)
{
 if (a._iConstVal == b)
 {
	 return _YES_;
 }
 return _NO_;
}

int IConstArg::isEq(BaseArgument *eqPtr)
{
 if ((eqPtr != NULL) && (eqPtr->isIConstArg()))
  if (_iConstVal == ((IConstArg *) eqPtr)->_iConstVal)
   return _YES_;
  return _NO_;
}

ostream & operator << (ostream & out, const IConstArg & s)
{
 out << "Int Val: " << s._iConstVal;
 return out;
}

void IConstArg::print(ostream &out)
{
 out << _iConstVal;
}

void IConstArg::printC(ostream &out)
{
 out << _iConstVal;
}

void IConstArg::irToC(FilePointer & fp, int indent)
{
	printIndent(fp, indent);

	MyString s;
	printCToString(s);
	fprintf(fp, "%s", s.getStr());
}

void IConstArg::printCToString(MyString & s, char *tval)
{
	if (tval == NULL)
		s.concat("temp2.changeValue(");
	else
	{
		s.concat(tval);
		s.concat(".changeValue(");
	}
	char str[20];
	itoa(_iConstVal, str, 10);

	s.concat(str);
	s.concat(")");
}

void IConstArg::print()
{
   cout << _iConstVal;
}

void IConstArg::printToString(char *buf)
{
 char str[20];
 strcat(buf,itoa(_iConstVal,str,10));
}

void IConstArg::printProperties(ostream & out, PropNameList & pL)
{
	out << _iConstVal;
	BaseArgument::printProperties(out, pL);
}

BaseArgument *IConstArg::copy()
{
 BaseArgument *retPtr;
 
 retPtr = new IConstArg(*this);
 //
 // Initialize member data here.
 
 return retPtr;
}

char * IConstArg::Name(void)
{
 char *s=new char[100];
 sprintf(s,"%d",value());
 return s;
}
