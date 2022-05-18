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
#include "parser.h"

static long Line_number = 1;

void increment_line_number(void)
{
    Line_number++;
}

long line_number(void)
{
    return Line_number;
}

static int debugging = true;

static int syntax_errors;
static int semantic_errors;
static int compiler_errors;
static int warnings;

static char file_name[100];

void set_file_name(char * s)
{
    strcpy(file_name, s);
    Line_number = 1;
    syntax_errors = 0;
    semantic_errors = 0;
    compiler_errors = 0;
    warnings=0;
}

void gen_comment(void)
{
    printf("\n### ");
}

void debug(char * msg)
{
    if (debugging)
        printf("DEBUG: %s\n",msg);
}

void syntax_error(char * msg)
{
    gen_comment();
    printf("%s (line: %d) Syntax Error: %s\n",file_name,line_number(),msg);
    syntax_errors++;
	assert(0);
	exit(1);
}

void compiler_error(char * msg)
{
    gen_comment();
    printf("%s (line: %d) Compiler Error: %s\n",file_name,line_number(),msg);
    compiler_errors++;
}

void semantic_error(char * msg)
{
    gen_comment();
    printf("%s (line: %d) Semantic Error: %s\n",file_name,line_number(),msg);
    semantic_errors++;
	exit(1);
}

void warning(char * msg)
{
    gen_comment();
    printf("%s (line: %d) Warning: %s\n",file_name,line_number(),msg);
    warnings++;
}


static char * sip(int n)
{
        return n==1?"":"S";
}

static char * prefix = "> ";

void report_errors(char * filename)
{
	printf("\n%s:\n",filename);
    if (syntax_errors>0) {
        gen_comment();
        printf("%s%d SYNTAX ERROR%s",prefix,syntax_errors,sip(syntax_errors));
    }
    if (semantic_errors>0) {
        gen_comment();
        printf("%s%d SEMANTIC ERROR%s",prefix,semantic_errors,sip(semantic_errors));
        }
    if (compiler_errors>0) {
        gen_comment();
        printf("%s%d COMPILER ERROR%s",prefix,compiler_errors,sip(compiler_errors));
        }
    if (warnings>0) {
        gen_comment();
        printf("%s%d WARNING%s",prefix,warnings,sip(warnings));
    }
}


void printIndent(int indent){
	int i;
	printf("\n");
	for(i=0;i<indent;i++)
		printf(" ");
}