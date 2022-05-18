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
/* file: expressParser.h					*/
/* created: Thu Jul 01 1999					*/
/* authors: Ashok Halambi					*/
/* last modified:							*/
/* contents: global definitions for			*/
/* express parser							*/
/********************************************/
#ifndef _EXPRESSPARSER_H_
#define _EXPRESSPARSER_H_

extern int yylex();
extern int yyparse();

#include <stdio.h>
//#include "Routine.h"
#include "Assertions.h"

#include "Routine.h"

#ifndef YYDEBUG
// i.e., if we're not in grammar.y, include its definitions
#ifdef __UNIX__
	#include "../support/express_parser.cpp.h"
#else
#include "..\support\express_parser.cpp.h"
#endif // end of else of #ifdef __UNIX__
#endif

//Control some debug prints in the lexer and parser
//
#define EXPRESS_PARSER_DEBUG 0

enum EXPRESS_PARSER_MODE
{
	_INIT_STMTS_, _START_STMTS_, _END_STMTS_, 
	_PROLOGUE_, _EPILOGUE_, _BODY_, _NO_MODE_
};

enum EXPRESS_ARG_TYPE
{
	_EXPRESS_REG_INT_, _EXPRESS_REG_STRING_, 
	_EXPRESS_IMM_INT_, _EXPRESS_IMM_FLOAT_, 
	_EXPRESS_LABEL_
};

#define EXPRESS_MAX_ARGS 3
#define EXPRESS_ARG_MAX_STRLEN 50
#define EXPRESS_LABEL_MAX_STRLEN 50

typedef struct
{
	union
	{
		char sarg[EXPRESS_ARG_MAX_STRLEN];
		int iarg;
	} argV;
	EXPRESS_ARG_TYPE argT;
} ArgNode;

extern char *copy(char *s);
extern char * strip_quotes_and_copy(char *s, int len);
extern char * copy_and_replace_underscore(char *s);
extern void incrementLineNumber(void);
extern long lineNumber(void);
extern void setFileName(char *fileName);
extern void syntaxError(char * msg);

#define yyerror syntaxError

extern ArgNode argArray[EXPRESS_MAX_ARGS];
extern BaseArgProperty *assertionArray[EXPRESS_MAX_ARGS];
extern char labelName[EXPRESS_LABEL_MAX_STRLEN];

extern void parserRoutInit();
extern void clearArgArray();
extern void clearAssertionArray();
extern void addToAssertionArray(BaseArgProperty *asrt);
extern BaseArgProperty *createHLInfoAssertion(char *name, char *value);

#endif
