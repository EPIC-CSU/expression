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
// $Id: FConstArg.cc,v 1.5 1998/03/27 00:02:51 wtang Exp $
//
// File:  		FConstArg.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#include "stdafx.h"

#include "FConstArg.h"
#include "TreeBuilding.h"
#include "IburgToIR.h"

extern void printIndent(FilePointer & fp, int indent);

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_FCONSTARGS
int FConstArg::_numFConstArgs = 0;	
#endif


BaseArgument *FConstArg::copy()
{
 BaseArgument *retPtr;
 
 retPtr = new FConstArg(_fConstVal);
 //
 // Initialize member data here.
 
 return retPtr;
}


FConstArg::FConstArg(const char *floatValue)
{
 if (floatValue == _NULL_)
	 _fConstVal = _NULL_;
 else
	 _fConstVal = strdup(floatValue);
}

BaseArgument *FConstArg::copyBody()
{
 BaseArgument *retPtr;
 retPtr = new FConstArg(_fConstVal);
 
 // Currently, we do not copy any properties of the argument.
 /*
 MyLinkedList<PropertyNames> propList;
 propList.appendElement(<insert_property_name_here>
 
   (retPtr->getPropertyList()).makeACopyOf(_propertyPtr, propList);
 */
 
 return retPtr;
}

int FConstArg::isEq(BaseArgument *arg)
{
	if (!(arg->isFConstArg())) return _NO_;
	if (strcmp(((FConstArg *)arg)->textValue(), _fConstVal) == 0)
		return _YES_;
	return _NO_;
}

const int operator == (const FConstArg & a, const FConstArg & b)
{
 if (strcmp(a._fConstVal, b._fConstVal) == 0)
 {
	 return _YES_;
 }
 return _NO_;
}

const int operator == (const FConstArg & a, const char *b)
{
 if (strcmp(a._fConstVal, b) == 0)
 {
	 return _YES_;
 }
 return _NO_;
}

ostream & operator << (ostream & out, const FConstArg & s)
{
 out << "Float Val: " << s._fConstVal;
 return out;
}

void FConstArg::print(ostream &out)
{
   out << _fConstVal;
}

void FConstArg::print()
{
   cout << _fConstVal;
}

void FConstArg::printCToString(MyString & s, char *tval)
{
	if (tval == NULL)
		s.concat("temp2.changeValue(");
	else
	{
		s.concat(tval);
		s.concat(".changeValue(");
	}
	s.concat(_fConstVal);
	s.concat(")");
}

void FConstArg::printC(ostream &out)
{
 out << _fConstVal;
}

void FConstArg::irToC(FilePointer & fp, int indent)
{
	printIndent(fp, indent);

	MyString s;
	printCToString(s);
	fprintf(fp, "%s", s.getStr());
}

void FConstArg::printToString(char *buf)
{
	strcat(buf, _fConstVal);
}

void FConstArg::printProperties(ostream & out, PropNameList & pL)
{
	out << _fConstVal;
	BaseArgument::printProperties(out, pL);
}

char* FConstArg::textValue()
{
 return _fConstVal;
}

// To return the value of the float immediate argument, we use atof
// to convert it  from a string representation to a double value.
// Note that the value returned is of type double.
//
double FConstArg::value()
{
 return atof(_fConstVal);
}

// Use changeValue method whenever you wish to change the value
// held by an FConstArg object. changeValue checks to see if there
// was any previous value. If there was, it frees the memory, and
// then allocates new memory and copies the new value.
//
void FConstArg::changeValue(char *valString)
{
 if (_fConstVal != _NULL_)
  free(_fConstVal);
 
 _fConstVal = strdup(valString);
}


