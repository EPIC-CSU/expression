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
/* file: parserSupport.cpp					*/
/* created: Wed Jun 30 1999					*/
/* authors: Ashok Halambi					*/
/* last modified:							*/
/* contents: support routines for			*/
/* lexer, parser					 		*/
/********************************************/
#include "stdafx.h"

#include "GlobalIncludes.h"
#include "expressParser.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

char * copy(char * s)
{
      char * r;
      r=strdup(s);
      return r;
}

char * copy_and_replace_underscore(char * s)
{
	//replaces "." with "_" for the var.4 generated from gcc, to transform them into var_4
      char * r,*p;
      r=strdup(s);
	  for(p=r;*p!='\0';p++){
		  if(*p=='.') *p='_';
	  }
      return r;
}

char * strip_quotes_and_copy(char *s,int len)
{
       char * rez,*r;
       r=rez=new char[len-1];
       s++;
       do{
            if(*s=='"') s++;
            if(*s!='\0') *rez++=*s++;
            else *rez=*s;
       }
	   while(*s!='\0');
       return r;
}

static long lineNum = 1;


void resetLineNumber(void){
	lineNum = 1;
}

void incrementLineNumber(void)
{
    lineNum++;
}

long lineNumber(void)
{
    return lineNum;
}

static char parseFileName[100];

void setFileName(char *fileName)
{
	strcpy(parseFileName, fileName);
}

void syntaxError(char * msg)
{
 cout << parseFileName << " (line: " << lineNumber() << ") Syntax Error: " << msg << endl;
	assert(0);
}

ArgNode argArray[EXPRESS_MAX_ARGS];
BaseArgProperty *assertionArray[EXPRESS_MAX_ARGS];
extern int numArgs;

void addToArgArray(int num, EXPRESS_ARG_TYPE aT)
{
	if (!(numArgs < EXPRESS_MAX_ARGS))
	{
		// error condition.
		//
		cout << "Panic: Received erroneous position to insert arg into argArray" << endl;
		assert(0);
	}
	argArray[numArgs].argV.iarg = num;
	argArray[numArgs].argT = aT;
}

void addToArgArray(char *str, EXPRESS_ARG_TYPE aT)
{
	if (!(numArgs < EXPRESS_MAX_ARGS))
	{
		// error condition.
		//
		cout << "Panic: Revieved erroneous position to insert arg into argArray" << endl;
		assert(0);
	}
	strcpy(argArray[numArgs].argV.sarg, str);
	argArray[numArgs].argT = aT;
}


void addToAssertionArray(BaseArgProperty *asrt)
{
	if (!(numArgs < EXPRESS_MAX_ARGS))
	{
		// error condition.
		//
		cout << "Panic: Revieved erroneous position to insert arg into argArray" << endl;
		assert(0);
	}
	assertionArray[numArgs] = asrt;
}

void clearAssertionArray()
{
	for (int i = 0; i < EXPRESS_MAX_ARGS; i++)
	{
		assertionArray[i] = NULL;
	}
}

void clearArgArray()
{
	// ToDo, TBD : Clear the arg array (if necessary).
}

void parserRoutInit()
{
	clearArgArray();
	clearAssertionArray();
}

extern BaseOperation *buildMIPSOp(char *opcode, int numArgs, int hasLabel);
extern NormalInstr *buildMIPSInstr(char *opcode, int numArgs, int hasLabel, int currLoc);

void insertIntoIR(Routine *curRoutine, char *opcode, int numArgs, 
				  EXPRESS_PARSER_MODE curMode, int hasLabel, int currLoc)
{
//	BaseOperation *oper;
	BaseInstruction *instr;

//	if (curMode == _PROLOGUE_)
//	{
//		oper = buildMIPSOp(opcode, numArgs, hasLabel);

//		curRoutine->addPrologueOp(oper);
//		delete oper;
//	}
	if ((curMode == _BODY_) || (curMode == _PROLOGUE_) || (curMode == _EPILOGUE_) 
		|| (curMode == _START_STMTS_) || (curMode == _END_STMTS_))
	{
		instr = buildMIPSInstr(opcode, numArgs, hasLabel, currLoc);
		//instr = buildMIPSInstr(buildMIPSOp(opcode, numArgs, hasLabel));

		if (instr != NULL)
			curRoutine->appendInstruction(instr);
	}
	else
	{
		// The other portions of the routine are not included in the IR
		// as of now.
	}
}
