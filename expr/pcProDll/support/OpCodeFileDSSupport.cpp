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
/********************************************/
/* file: OpCodeFileDSSupport.cpp			*/
/* created: Wed Oct 25 2000					*/
/* authors: Ashok Halambi					*/
/* last modified:							*/
/* contents: support routines for			*/
/* lexer, parser					 		*/
/********************************************/
#include "stdafx.h"

#include "GlobalIncludes.h"
#include "OpCodeFileDS.h"
#include "OpCodeTable.h"
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

Name2DefineList GlobalName2DefineList;

void initName2DefineList()
{
	MyString name;

	name = "_DST_";
	GlobalName2DefineList[name] = _DST_;

	name = "_SRC_1_";
	GlobalName2DefineList[name] = _SRC_1_;

	name = "_SRC_2_";
	GlobalName2DefineList[name] = _SRC_2_;

	name = "_SRC_3_";
	GlobalName2DefineList[name] = _SRC_3_;
}

void printName2DefineList()
{
	N2DIter i;
	MyString s;
	for (i = GlobalName2DefineList.begin(); i != GlobalName2DefineList.end(); i++)
	{
		s = (*i).first;
		printf("Name %s \t Code %d\n", s.getStr(), (*i).second);
	}
}

int getDefineFromName(MyString & s)
{
	return GlobalName2DefineList[s];
}

OperandTypesForOperations globalOperandTypesForOperations;

void addOperandToDataTypeArray(int opcodeIndex, int operandNum, int operandType, int dstOrSrc)
{
	if ((opcodeIndex == _INVALID_) || (operandNum == _INVALID_))
	{
		// error condition. Should we assert?
		//
		return;
	}
	if (dstOrSrc == _DEST_LIST_) // this means destination list
		(globalOperandTypesForOperations[opcodeIndex]).dstList[operandNum] = operandType;
	else if (dstOrSrc == _SRC_LIST_) // this means source list
		(globalOperandTypesForOperations[opcodeIndex]).srcList[operandNum] = operandType;
}

int getOperandNum(char *str)
{
	if (str == NULL) 	return _INVALID_;

	MyString s(str);
	return getDefineFromName(s);
}

extern OpCodeTable globalOpCodeTable;

int getOpcodeIndex(char *str)
{
	if (str != NULL)
		return globalOpCodeTable.getIndex(str);
	return _INVALID_;
}

int getDataTypeForArgFromList(OperandTypeList & l, int pos)
{
	OperandTypeList::iterator i;

	for (i = l.begin(); i != l.end(); i++)
	{
		if ((*i).first == pos) return (*i).second;
	}
	return _INVALID_;
}

int getDataTypeForArgInOper(int opcode, int pos, int dstOrSrc)
{
	if ((opcode == _INVALID_) || (pos == _INVALID_))
	{
		return _INVALID_;
	}
	if (dstOrSrc == _DEST_LIST_) // this means destination list
		return (getDataTypeForArgFromList(globalOperandTypesForOperations[opcode].dstList, pos));
	else if (dstOrSrc == _SRC_LIST_) // this means source list
		return (getDataTypeForArgFromList(globalOperandTypesForOperations[opcode].srcList, pos));

	return _INVALID_;
}

int is2ndArgOfDouble(int pos, int dstOrSrc)
{
	if (dstOrSrc == _DEST_LIST_)
	{
		if ((pos >= _DOUBLE_DST_BEGIN_) && (pos <= _DOUBLE_DST_END_))
			return _YES_;
	}
	if (dstOrSrc == _SRC_LIST_)
	{
		if ((pos >= _DOUBLE_SRC_BEGIN_) && (pos <= _DOUBLE_SRC_END_))
			return _YES_;
	}

	return _NO_;
}

void opcoderestart( FILE *input_file );
int opcodeparse(void);

void readOpCodeFile(MyString & fName)
{
	if (fName.isEmpty()) return;

	FILE *f = fopen(fName.getStr(), "r");

	if(f==NULL)
	{
		printf("Could not open file %s\n",fName.getStr());
		assert(0);
	}

	opcoderestart(f);
	opcodeparse();
}
