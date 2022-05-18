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
/*************************************************************
 *file  : DerivedAddrData.cpp                                *
 *authors : akhare                                           *
 *last modified : Sudeep (05/20)                                           *
 *                                                           *
 *contents:definitions of the methods of AddrData classes    *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "StdAfx.h"
#include <map>

#include "DerivedAddrData.h"
#include "MyString.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW:

// #include "RegisterFile.h"

int getValRType(int symTabNum);

convertToArchType(int valType)
{
	if (valType == _INT_)
		return _INT_REG_FILE;
 
	// default
	return _INT_REG_FILE;
}

int getRegFileType(int regNum)
{

	MyString str;
	getValRName(regNum,str);
	if (str == "cc")
		return _MISC_REG_FILE;
	else
//	cout << "getRegFIleType(): To Be Written" << endl;
		return (convertToArchType(getValRType(regNum)));
	//return _INT_REG_FILE;

/*
	if (regNum < 32)
		return _INT_REG_FILE;
	else
		return _FLOAT_REG_FILE;
*/

}
