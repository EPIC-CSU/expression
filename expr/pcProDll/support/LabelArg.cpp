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
// $Id: LabelArg.cc,v 1.5 1998/03/27 00:02:52 wtang Exp $
//
// File:  		LabelArg.cc
// Created:		Wed Jan 15, 97
// Last modified: 	Wed Jan 15, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#include "stdafx.h"

#include "LabelArg.h"
#include "expressParser.h"
#include "Program.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern void printIndent(FilePointer & fp, int indent);

#ifdef COUNT_FLOWOPS
int LabelArg::_numLabelArgs = 0;	
#endif

LabelArg::LabelArg()
{
 _labelName = NULL;
#ifdef COUNT_LABELARGS
 _numLabelArgs++;	
#endif
}

LabelArg::LabelArg(char *lName)
{
 _labelName = strdup(lName);
 
#ifdef COUNT_LABELARGS
 _numLabelArgs++;	
#endif
}

LabelArg::LabelArg(const LabelArg & s)
{
 _labelName = strdup(s._labelName);
 
#ifdef COUNT_LABELARGS
 _numLabelArgs++;	
#endif
}	

LabelArg::~LabelArg()
{
#ifdef COUNT_LABELARGS
 _numLabelArgs--;
#endif
 if (_labelName!=NULL)
  free(_labelName);
} 

BaseArgument *LabelArg::copyBody()
{
 BaseArgument *retPtr;
 retPtr = new LabelArg(*this);
 
 // Currently, we do not copy any properties of the argument.
 /*
 MyLinkedList<PropertyNames> propList;
 propList.appendElement(<insert_property_name_here>
 
   (retPtr->getPropertyList()).makeACopyOf(_propertyPtr, propList);
 */
 
 return retPtr;
}  

void LabelArg::initLabel(char *lName)
{
 if (_labelName!=NULL)
  free(_labelName);
 _labelName = strdup(lName);
}

int LabelArg::isEq(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;

	if (!(arg->isLabelArg())) return _NO_;
	return isEqual(((LabelArg *)arg)->lName());
}

int LabelArg::isEqual(char *lName)
{
	if ((lName == NULL) || (_labelName == NULL)) return _NO_;

 if (strcmp(lName, _labelName) == 0)
  return _YES_;
 return _NO_;
}

const int operator == (const LabelArg & a, const LabelArg & b)
{
 if (strcmp(a._labelName, b._labelName) == 0)
 {
  return _YES_;
 }
 return _NO_;
}

const int operator == (const LabelArg & a, const char *b)
{
 if (strcmp(a._labelName, b) == 0)
 {
  return _YES_;
 }
 return _NO_;
}

void LabelArg::print()
{
 cout << _labelName;
}

// for ASM

int LabelArg::testIfLibraryFunction()
{
	if(curProgram->getRoutine(_labelName) == NULL)
		return 1;  // is a library function
	else
		return 0;
}

void LabelArg::print(ostream& out)
{
	if(_labelName[0] == '_')
		out<< _labelName+1;  // discard the leading underscore in cases of library calls
	else
		out << _labelName;
}

void LabelArg::printC(ostream& out)
{
 out << _labelName;
}

void LabelArg::printAsm(ostream& out)
{
	out << _labelName << ":";
}

void LabelArg::irToC(FilePointer & fp, int indent)
{
	printIndent(fp, indent);

	MyString s;
	printCToString(s);
	fprintf(fp, "%s", s.getStr());
}

#include "CallNameArgProperty.h"

void LabelArg::printCToString(MyString & s, char *tval)
{
	// first check to see if it has a CallNameArgProperty.
	//
	char buf[20];
	CallNameArgProperty *cnp;
	if ((cnp = (CallNameArgProperty *)(this->getPtrToProperty(_CALLNAME_))) != NULL)
	{
		sprintf(buf, "%d", cnp->justGetIndex());
		s.concat(buf);
	}
	else
		s.concat(_labelName);
}

void LabelArg::printToString(char *buf)
{
	strcat(buf, _labelName);
}

ostream & operator << (ostream & out, const LabelArg & s)
{
 out << "Label Name: " << s._labelName;
 return out;
}

void LabelArg::printProperties(ostream & out, PropNameList & pL)
{
	out << _labelName;
	BaseArgument::printProperties(out, pL);
}

char* LabelArg::lName()
{
 return _labelName;
}

BaseArgument *LabelArg::copy()
{
 BaseArgument *retPtr;
 
 retPtr = new LabelArg(*this);
 //
 // Initialize member data here.
 
 return retPtr;
}

int newLabelVal = 0;

char *getNewLabel()
{
	char *buf = new char[EXPRESS_LABEL_MAX_STRLEN];
	strcpy(buf, "EX");

 char str[20];
	strcat(buf,itoa(newLabelVal,str,10));
	newLabelVal++;

	return buf;
}
