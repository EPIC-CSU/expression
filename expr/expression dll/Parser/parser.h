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
/* file: parser.h							*/
/* created: Nov. 30, 1998					*/
/* authors: pgrun							*/
/* last modified:							*/
/* contents: global definitions for parser	*/
/********************************************/
#ifndef _PARSER_H_
#define _PARSER_H_

typedef int token;

extern int yylex();
extern int yyparse();

extern char * token_image(token t);

#include <stdio.h>

#include "errors.h"
#include "symbols.h"
#include "symtabs.h"
#include "node.h"


//The symbol table (unique for the project) 
extern Symtab st;


#ifndef YYDEBUG
// i.e., if we're not in grammar.y, include its definitions
#include "treeparse.h"
#include "expression.cpp.h"
#endif

#endif






















