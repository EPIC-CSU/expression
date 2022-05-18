%{
#include "stdafx.h"

#ifndef WIN32
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#endif
#include "Routine.h"
#include "Program.h"

#ifdef WIN32
#include "parser.cpp.h"
#include <io.h>
#else
#include "parser.cc.h"
#endif


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 



int looking_for_tt_op = 0;
int looking_for_sim_op = 0;
int looking_for_semspec = 0;

int yylineno=1;

const int max_tt_idents = 8;
int tt_ident_p = 0;

%}

%option noyywrap

%s ttop 
%s simop 
%s semspec 


hexno		0x[0-9]+

ident		[^][ \t\b\n(){},":;+]+

symbol		[^ \t\b\na-zA-Z0-9]

quoted_string   \"("\\\""|[^"])*\" 

%%
		{   
		   if (looking_for_tt_op)
		      BEGIN(ttop);
		   else if (looking_for_sim_op)
		      BEGIN(simop);
		   else if (looking_for_semspec)
		      BEGIN(semspec);
		   else
		      BEGIN(INITIAL); 
		}

[ \t\b]*;.*$	{ /* gobble */ }

<semspec>mem	return TT_MEM;
<semspec>dest	return TT_DEST;
<semspec>src	return TT_SRC;
<semspec>symbol	{ return yytext[0]; }
<semspec>dexp	return TT_DEXP; 
<semspec>dsqrt	return TT_DSQRT; 
<semspec>ashift	return TT_ASHIFT; 
<semspec>lshift	return TT_LSHIFT; 
<semspec>print	return TT_PRINT; 

-?0x[0-9A-Fa-f]+ { 
		   yylval.t_intval = strtol(yytext,NULL,0); 
		   return TT_INT_VAL;
		}


-?[0-9]+	{ 
		   yylval.t_intval = strtol(yytext,NULL,0); 
		   return TT_INT_VAL;
		}

<semspec>[0-9]+[.][0-9]+ {
		   yylval.t_fltval = atof(yytext); 
		   return TT_FLT_VAL;
		}

<semspec>w1	{ yylval.t_str = strdup("w1"); return TT_MODE; }
<semspec>w2	{ yylval.t_str = strdup("w2");  return TT_MODE; }
<semspec>i	{ yylval.t_str = strdup("i");  return TT_MODE; }
<semspec>u	{ yylval.t_str = strdup("u");  return TT_MODE; }
<semspec>ui	{ yylval.t_str = strdup("ui"); return TT_UI; }
<semspec>s	{ yylval.t_str = strdup("s"); return TT_MODE; }
<semspec>f	{ yylval.t_str = strdup("f");  return TT_MODE; }
<semspec>d	{ yylval.t_str = strdup("d");  return TT_MODE; }
<semspec>b	{ yylval.t_str = strdup("b");  return TT_MODE; }
<semspec>a	{ yylval.t_str = strdup("a");  return TT_MODE; }

PROC_BEGIN	return TT_PROCBEGIN;
PSEUDOS	 	return TT_PSEUDOS;
DECL	 	return TT_DECL;
GLOBAL	 	return TT_GLOBAL;
IS_GLOBAL	return TT_IS_GLOBAL;
ALIGN	 	return TT_ALIGN;
WORD	 	return TT_WORD;
HALF	 	return TT_HALF;
BYTE	 	return TT_BYTE;
ASCII	 	return TT_ASCII;
SPACE	 	return TT_SPACE;
STAB	 	return TT_STAB;
val	 	return TT_VAL;
scl	 	return TT_SCL;
dim	 	return TT_DIM;
type	 	return TT_TYPE;
tag	 	return TT_TAG;
size	 	return TT_SIZE;
parm		return TT_PARM;
CASETABLE	return TT_CASETABLE;
CASE		return TT_CASE;
LOCATION	return TT_LOCATION;
LABEL		return TT_LABEL;
BEGIN_SCOPE	return TT_BEGIN_SCOPE;
END_SCOPE	return TT_END_SCOPE;
target_specification	 return TT_TSPEC;
num_conditionals	 return TT_NCONDS;
rf_defs	 	return TT_RFDEFS;
rf_def	 	return TT_RFDEF;
fu_defs	 	return TT_FUDEFS;
fu_def	 	return TT_FUDEF;
inputs	 	return TT_INPUTS;
outputs	 	return TT_OUTPUTS;
stages	 	return TT_STAGES;
units	 	return TT_UNITS;
opcode_defs	return TT_OPCODEDEFS;
opcode_def	return TT_OPCODEDEF;
fu	 	return TT_FU;
latency	 	return TT_LATENCY;
members	 	return TT_MEMBERS;
argtypes	return TT_ARGTYPES;
regarg	 	return TT_REGARG;
immarg	 	return TT_IMMARG;
fltarg	 	return TT_FLTARG;
range	 	return TT_RANGE;
syntax	 	return TT_SYNTAX;
semantics	return TT_SEMANTICS;
labelarg	return TT_LABELARG;
stringarg	return TT_STRINGARG;
ASSERT		return TT_ASSERT;
set_val		return TT_SET_VAL;
override_sd	return TT_OVERRIDE_SD;
add_path_sd	return TT_ADD_PATH_SD;
note		return TT_NOTE;
RETURN		return TT_RETURN;
CALL		return TT_CALL;
CGOTO		return TT_CGOTO;
SAVE		return TT_SAVE;
BREAK		return TT_BREAK;
TOP		return TT_TOP;
BOTTOM		return TT_BOTTOM;
phi_maps	return TT_PHI_MAPS;
virtual_reg_def	return TT_VIRTUAL_REG_DEF;
target_architecture	 return TT_TARCH;

<simop>[ \t\b]*[^ \t\b\n].*$ { 
		   // ignore trailing spaces
		   for(yyleng--; yyleng && isspace(yytext[yyleng]); yytext[yyleng--] = 0)
			;

		   // ignore leading spaces
		   int i;
		   for(i=0; (i < yyleng) && isspace(yytext[i]); i++ )
			;
		   
		   // ignore trailing comment
		   int j = i;
		   for(; j < yyleng; j++ )
		      if (yytext[j] == ';')
		      {
			 yytext[j] = 0;
			 break;
		      }
			 
		   yylval.t_str = strdup(yytext+i); 

		   return TT_OP;
		}
			

<ttop>^[\t][ \t\b]*[^ \t\b\n].*$ { 

		   // ignore trailing spaces
		   for(yyleng--; yyleng && isspace(yytext[yyleng]); yytext[yyleng--] = 0)
			;

		   // ignore leading spaces
		   int i;
		   for(i=0; (i < yyleng) && isspace(yytext[i]); i++ )
			;
		   
		   // ignore trailing comment
		   int j = i;
		   for(; j < yyleng; j++ )
		      if (yytext[j] == ';')
		      {
			 yytext[j] = 0;
			 break;
		      }
			 
		   yylval.t_str = strdup(yytext+i); 

		   return TT_OP;
		}

{quoted_string} { 
		   yytext[yyleng-1] = 0; // ignore leading and trailing quote

		   yylval.t_str = strdup(yytext+1); 

		   return TT_IDENT;
		} 

<INITIAL,simop,ttop>{ident}	{
		   yylval.t_str = strdup(yytext); 
		   return TT_IDENT; 
		}

[ \t\b]+	{ /* gobble whitespace */ }

[\n]		{  
		   yylineno ++ ; // do this ourselves since flex's yylineno wasn't 
		       		 // being correctly updated 
		}

.		{ return yytext[0]; }

%%

