/*
 * Copyright (C) 1996 by Steve Novack and The Regents of the University of California.
 * All Rights Reserved.
 *
 * This is a pre-release version of the EVE compiler for internal use only and
 * is not for public distribution.
 */
/* $Header: /i0/dh/ub/ilp/pcPro/support/RCS/parser.y,v 1.6 1998/04/07 20:14:03 wtang Exp $

  Part of the EVE Parallelizing Compiler by Steve Novack

  Module: Input

  Code for parsing the target specification and input N-addr code.
  Control parameters may ultimately be parsed here as well.

  This code was designed and developed by Steve Novack in partial
  fulfillment of the degree of Doctor of Philosophy at the University
  of California, Irvine.

 */ 


%{
#include "stdafx.h"

int globali=0;

#ifndef WIN32
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <assert.h>
#endif

#include <malloc.h>

#ifdef WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif

#include "Routine.h"
#include "NormalInstr.h"
#include "BaseOperation.h"
#include "Program.h"
#include "PropertyList.h"
#include "BaseOpProperty.h"
#include "PartofProperty.h"



//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


#define STRLEN_MAX 200
#define STRLEN_NOR 50

extern Program *curProgram;
extern char *yytext;


static char instrStr[STRLEN_MAX];
static int has_label;
static char labelName[STRLEN_NOR];
static char assertBuf[STRLEN_MAX];

static int tempVarCount;
static char tempVarName[] ="tempVar";
static char tempFileName[] = "_temp";
static char tempRoutFileName[] = "_temp1";
static char exeSemFileName[] = "exeSemGen.cc";
static char indentStr[] = "  ";
static FILE *exeSemFile, *tempFile,*tempRoutFile;
void printMode(ostream&out, char *mode);
void printModeVal(ostream&out, char *mode);
void gen_routine_header(char *, char*);
void gen_routine_content();
void gen_routine_tail();
void start_gen_sem();
void end_gen_sem();
void gen_assign(char *, char *, char *);
void gen_ld(char *, char *, char *, char *);
void gen_st(char *, char *, char *);
char *read_file(char *);
int is_ld(char *);
int is_st(char *);

Routine *curRoutine;

int yylex(void);
extern int yylineno;

                /* define size of largest program supported */
#define YYMAXDEPTH 100000

void clear_stab_vars();

int source_line_no;

int in_scope, in_routine, begin_prologue, begin_epilogue, scope_var;

int ignore_stab, stab_val, stab_scl, stab_size, stab_type;

extern int looking_for_tt_op;
extern int looking_for_sim_op;
extern int looking_for_semspec;

extern NormalInstr *buildMIPSInstr(char *, char * = NULL);
extern BaseOperation *buildMIPSOp(char *, char * = NULL);
extern void addToParmList(char *);
extern void clearParmList(void);

void addAssertion(NormalInstr *instr,char *assertBuf);
int formatStrings(char *op);
void clearAllStrings(void);

int current_phi_map;

void yyerror(char *s)
{
  printf("%s at line %d\n",s,yylineno);
}

%}

%start program

%union {
    char *t_str;
    char *t_initializer;
    int t_intval;
    double t_fltval;
    char *t_arg;
    char *t_fake_if;
    char *t_arg_list;
    char t_inst;
    char *t_sim_instr;
    Routine *t_routine;
    char t_value_type;
}

/* tokens for routines */
%token <t_str> TT_IDENT TT_OP TT_MODE

%token TT_ERROR TT_PROCBEGIN TT_PSEUDOS
%token TT_DECL TT_GLOBAL TT_IS_GLOBAL TT_ALIGN TT_WORD TT_HALF
%token TT_BYTE TT_ASCII TT_SPACE 
%token TT_STAB TT_VAL TT_SCL TT_DIM TT_TYPE TT_TAG TT_SIZE 
%token TT_LOCATION TT_RANGE

%token TT_UI

/* tokens for target spec */
%token TT_TSPEC TT_NCONDS TT_RFDEFS TT_RFDEF TT_FUDEFS TT_FUDEF
%token TT_INPUTS TT_OUTPUTS TT_STAGES TT_UNITS TT_OPCODEDEFS TT_OPCODEDEF
%token TT_FU TT_LATENCY TT_MEMBERS 
%token TT_REGARG TT_IMMARG TT_ARGTYPES TT_SYNTAX TT_SEMANTICS TT_LABELARG TT_FLTARG
%token TT_SET_VAL TT_OVERRIDE_SD TT_ADD_PATH_SD TT_NOTE TT_LABEL TT_BEGIN_SCOPE TT_END_SCOPE 

%token TT_ASSERT TT_STRINGARG

%token TT_OP TT_PARM

%token TT_TOP TT_BOTTOM TT_TARCH

%type <t_intval> inputs outputs stages units latency
%type <t_str> fu members syntax expr rhs comp_expr


%token TT_MEM TT_DEST TT_SRC 
%token TT_DEXP TT_DSQRT
%token TT_ASHIFT TT_LSHIFT
%token TT_PRINT
%token <t_intval> TT_INT_VAL 
%token <t_fltval> TT_FLT_VAL 

%token TT_RETURN TT_CALL TT_CGOTO TT_SAVE TT_BREAK
%token TT_PHI_MAPS
%token TT_VIRTUAL_REG_DEF

%token TT_CASE TT_CASETABLE

/* typed nonterminals */
%type<t_routine> routine
%type <t_intval> alignment
%type <t_intval> is_global
%type <t_initializer> initializer

%type <t_arg> Case
%type <t_fake_if> case_list, caselist

%type <t_arg_list> arg_list, args
%type <t_arg> arg
%type <t_sim_instr> instr_type

%%

program: /* nothing */
		{ yyerror("premature EOF\n"); }
	| routines 
		{ 
		   source_line_no = 0;
		   return(0); 
		}
	| targetspec 
		{ return(1);   }
	| simcode 
		{ return(1);   }
	;

target_arch:  '(' TT_TARCH TT_IDENT ')' 
		{ 
		}
	;

simcode:  target_arch siminstrs ;

virtual_reg_def: '(' TT_VIRTUAL_REG_DEF 
		      TT_IDENT { 
		      }
		      possibly_empty_virtual_regs
		  ')'
	;

possibly_empty_virtual_regs: '(' virtual_regs ')' 
			   | '(' ')'
			   ;

virtual_regs:     virtual_reg 
		| virtual_reg virtual_regs
		;

virtual_reg: TT_IDENT 
		{ 
		}
	   ;
	

siminstrs: siminstr
	| siminstr siminstrs
	| label siminstrs
	| global_pseudos siminstrs
	| virtual_reg_def siminstrs
	;

label:  TT_IDENT ':'
	{
	}
	;

siminstr: '{' 
	     TT_IDENT 
	     control_part
		{
		}
	     zero_or_more_simops
	     optional_phimaps
	  '}'
	;

optional_phimaps: '(' TT_PHI_MAPS { current_phi_map = 0; } phimaps ')'
		| { /* nothing */ }
		;

phimaps:  phimap
	| phimap phimaps
	;

phimap:   '(' phicopies ')' 
		{ current_phi_map++; } 
	| '(' ')' 
		{ current_phi_map++; } 
	;

phicopies:  phicopy
	|   phicopy phicopies
	;

phicopy: '(' TT_IDENT ',' TT_IDENT ')'
	    {
	    }
	;

instr_type: TT_TOP {  }
	  | TT_SAVE arg_list 
	  	{ 
	  	}
	  | TT_BREAK {  }
	  | {  }
	  ;

control_part: TT_IDENT instr_type
		{
		}
	   | arg_list 
	     '(' 
		{ 
		}
		targ_list 
	     ')'
	     instr_type
		{
		}

	   | TT_RETURN arg_list arg_list 
		{
		}
	   | TT_IDENT TT_CALL arg arg arg_list arg arg_list 
		{
		}
	   | TT_CGOTO arg arg arg_list 
		{
		}
	   | TT_IDENT TT_BOTTOM
		{
		}
	   ;
	
arg_list:  '(' args ')'
		{ $$ = $2; }
	|  '(' ')'
		{  }
	;

args:      arg
		{  }
	|  args arg
		{  }
	;

arg:    TT_IDENT
		{ 
		}
	;

targ_list: targ_spec
	|  targ_spec targ_list
	;

targ_spec: '(' TT_IDENT ',' TT_IDENT ')'
		{
		}
	;

zero_or_more_simops: { /* zero ops */ }
		   | simops
		   ;

simops:	  simop
	| simop simops
	;


simop:	'[' TT_INT_VAL ']' 
		{ looking_for_sim_op = 1; } 
	TT_OP
		{
		}
	;


targetspec: '(' TT_TSPEC nconds rfdefs fudefs argtypes opcodedefs ')'
	;

nconds:	 '(' TT_NCONDS TT_INT_VAL ')' 
	{

	}
	;

rfdefs:  '(' TT_RFDEFS rfdeflist ')'
	;

rfdeflist:  rfdef
	|   rfdef rfdeflist
	;

rfdef:  '(' TT_RFDEF TT_IDENT members ')'
	{
	}
	;

members: '(' TT_MEMBERS TT_IDENT ')' 
		{ 
		   $$ = $3; 
		}
	;

fudefs:  '(' TT_FUDEFS fudeflist ')'
	;

fudeflist:  fudef
	|   fudef fudeflist
	;

fudef:	'(' TT_FUDEF TT_IDENT inputs outputs stages units ')'
	{
	}
	;

inputs:	 '(' TT_INPUTS TT_INT_VAL ')' { $$ = $3; }
	;

outputs: '(' TT_OUTPUTS TT_INT_VAL ')' { $$ = $3; }
	;

stages: '(' TT_STAGES TT_INT_VAL ')' { $$ = $3; }
	;

units: '(' TT_UNITS TT_INT_VAL ')' { $$ = $3; }
	;

opcodedefs: '(' TT_OPCODEDEFS 
	{
	start_gen_sem();
	}
	opcodedeflist 
	{  
	end_gen_sem();
	} 
	')'
	;

opcodedeflist:  opcodedef 
	     |  opcodedef opcodedeflist
	     ;

opcodedef:  '(' TT_OPCODEDEF TT_IDENT fu latency syntax 
		'(' TT_SEMANTICS TT_IDENT 
	 { 
	     looking_for_semspec = 1; 
	     gen_routine_header($3,$9);
	     free($3);
             free($9);
	 }
		    semspec 
		    { looking_for_semspec = 0; }
		')' 
	    ')'
	{ 
	}
	;

fu: '(' TT_FU TT_IDENT ')' 
	{ 
	   $$ = $3; 
	}
	;

latency:  '(' TT_LATENCY TT_INT_VAL ')' { $$ = $3; }
	      ;

syntax:  '(' TT_SYNTAX TT_IDENT ')' { $$ = $3; } ;

semspec:    '(' assignment_list')' 
	{
	gen_routine_content();
	gen_routine_tail();
	}	
	| '(' ')' 
   	{
	gen_routine_tail();
   	}
	;

assignment_list:  assignment
			{  }
	| assignment ';' assignment_list
	{  }
	;

assignment: TT_MEM '[' rhs ']' '.' TT_MODE '=' rhs
	{
	gen_st($3, $6, $8);
	free($3);
	free($6);
	free($8);
	}
	| TT_DEST '.' TT_MODE '=' TT_MEM '[' rhs ']' '.' TT_MODE
	{
	gen_ld("dest",$3,$7,$10);
	free($3);
	free($7);
	free($10);
	}
	| TT_DEST '.' TT_MODE '=' rhs
	{
	gen_assign("dest",$3,$5);
	free($3);
	free($5);
	}
	
	;

rhs:	  expr  
	{
	$$ = $1; 
	}
	| comp_expr
	{
	$$ = $1;
	}
	;

comp_expr: expr '?' expr ':' expr
	{  
	strstream str;
	char *temp;
	str << $1 <<" ? " <<$3 <<" : " <<$5<<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	free($5);
	}
	| expr '+' expr
	{  
	strstream str;
	char *temp;
	str << $1 <<" + " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '-' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" + " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '*' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" * " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '/' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" / " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '%' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" % " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '&' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" & " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '|' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" | " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '^' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" ^ " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr TT_LSHIFT expr
	{
	strstream str;
	char *temp;
	str << $1 <<" << " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr TT_ASHIFT expr
	{
	strstream str;
	char *temp;
	str << $1 <<" >> " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '=' '=' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" == " <<$4 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($4);
	}
	| expr '!' '=' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" != " <<$4 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($4);
	}
	| expr '>' '=' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" >= " <<$4 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($4);
	}
	| expr '<' '=' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" <= " <<$4 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($4);
	}
	| expr '<' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" < " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| expr '>' expr
	{
	strstream str;
	char *temp;
	str << $1 <<" > " <<$3 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($1);
	free($3);
	}
	| '~' expr
	{
	strstream str;
	char *temp;
	str << " ~ " <<$2 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($2);
	}
	| '-' expr
	{
	strstream str;
	char *temp;
	str << " - " <<$2 <<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($2);
	}
	| TT_DEXP expr
	{
	strstream str;
	char *temp;

	str << "exp( " <<$2 << ")"<<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($2);
	}
	| TT_DSQRT expr
	{
	strstream str;
	char *temp;

	str << "sqrt( " <<$2 << ")"<<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($2);
	}
	| TT_PRINT expr 
	{
	assert(0);
	}
	| '(' TT_MODE ')' expr
	{
	strstream str;
	char *temp;

	str << "(";
        printMode(str,$2);
	str<<") (" <<$4<<")"<<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($2);
	free($4);
	}
	| '(' TT_UI ')' expr
	{
	strstream str;
	char *temp;

	str << "( unsigned integer) (" <<$4 <<")"<<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
 	free($4);
	}
	;

expr: 	'(' expr ')'
	{
	$$ = $2;
	}
	|  '(' comp_expr ')'
	{
	$$ = $2;
	}
	| TT_MEM '[' rhs ']' '.' TT_MODE
	{  
  	char *tempName,*tempPrint;
  	strstream str, retstr;

  	tempVarCount++;
  	str<<tempVarName<<"_"<<tempVarCount<<ends;
  	tempName = str.str();

  	retstr<<tempName << " = con->read("<<$3<<");\n";
  	retstr<<tempName;
  	printModeVal(retstr,$6);
        retstr<<";\n"<<ends;
        tempPrint = retstr.str();
	fprintf(tempRoutFile,"%s%s",indentStr,tempPrint);
	delete []tempPrint;

        $$ = strdup(tempName);
        delete []tempName;
	free($3);
	free($6);
	}
	| TT_SRC TT_INT_VAL '.' TT_MODE  
	{ 
	strstream str;
	char *temp;
	str<<"src"<<$2;
	printModeVal(str,$4);
	str<<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	free($4);
	}
	| TT_INT_VAL 
	{  
	strstream str;
	char *temp;
	str<<$1<<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	}
	| TT_FLT_VAL 
	{ 
	strstream str;
	char *temp;
	str<<$1<<ends;
	temp = str.str();
	$$ = strdup(temp);
	delete []temp;
	}
	;

argtypes: '(' TT_ARGTYPES argtypelist ')' 
	;

argtypelist: argtype 
	   | argtype argtypelist
	   ;

argtype:  '(' TT_REGARG TT_IDENT members ')' 
	      {
	      }
	| '(' TT_IMMARG TT_IDENT '(' TT_RANGE TT_INT_VAL TT_INT_VAL ')' ')' 
	      { 
	      }
	| '(' TT_LABELARG TT_IDENT syntax ')' 
	      {
	      }
	| '(' TT_STRINGARG TT_IDENT syntax ')' 
	      {
	      }
	| '(' TT_FLTARG TT_IDENT ')' 
	      {
	      }
	;

global_pseudos:  
	'(' TT_PSEUDOS				{looking_for_tt_op=0;}
	global_declarations ')'		{looking_for_tt_op=1;}
	| 
	decl					{}
	| 
	global					{}
	| 
	stab					{}
	| 
	location				{} 
	| 
	begin_scope				{} 
	| 
	end_scope				{} 
;

local_pseudos:  
	'(' TT_PSEUDOS				{looking_for_tt_op=0;}
	local_declarations ')'		{looking_for_tt_op=1;}
	| 
	decl					{}
	| 
	global					{}
	| 
	stab					{}
	| 
	location				{} 
	| 
	begin_scope				{} 
	| 
	end_scope				{} 
;

global_declarations: 
	global_declaration
    | 
	global_declaration global_declarations
;

local_declarations: 
	local_declaration
    | 
	local_declaration local_declarations
;

global_declaration: 
	'(' TT_SET_VAL TT_IDENT TT_IDENT ')'		{}
	| 
	'(' TT_SET_VAL TT_IDENT TT_INT_VAL			{}
	')'
	| 
	'(' TT_OVERRIDE_SD TT_IDENT TT_IDENT ')'	{}
	| 
	'(' TT_ADD_PATH_SD TT_IDENT TT_IDENT ')'	{}
	| 
	'(' TT_NOTE TT_IDENT TT_IDENT ')'			{}
	| 
	'(' TT_NOTE TT_IDENT TT_INT_VAL				{}
	')'
	| 
	'(' TT_ASSERT TT_IDENT TT_IDENT ')'			{}
	| 
	begin_scope
	| 
	end_scope
;

local_declaration: 
	'(' TT_SET_VAL TT_IDENT TT_IDENT ')'		{}
	| 
	'(' TT_SET_VAL TT_IDENT TT_INT_VAL			{}
	')'
	| 
	'(' TT_OVERRIDE_SD TT_IDENT TT_IDENT ')'	{}
	| 
	'(' TT_ADD_PATH_SD TT_IDENT TT_IDENT ')'	{}
	| 
	'(' TT_NOTE TT_IDENT TT_IDENT ')'			{}
	| 
	'(' TT_NOTE TT_IDENT TT_INT_VAL				{}
	')'
	| 
	'(' TT_ASSERT TT_IDENT TT_IDENT ')'			{}
	| 
	'(' TT_PARM TT_IDENT ')'					{/*printf("parms: %d %s\n", globali++,$3);*/addToParmList($3);}
	| 
	begin_scope
	| 
	end_scope
	|
	alignment '(' TT_LABEL TT_IDENT ')' {}
;

location: '(' TT_LOCATION TT_INT_VAL ')'	{source_line_no = $3;}
;

begin_scope: '(' TT_BEGIN_SCOPE ')' 
	{ 
	if (scope_var == 0)
	{
		in_scope = 1;
		begin_prologue = 0;
		has_label = 0;
		clearParmList();
	}
	scope_var++;
	};

end_scope: '(' TT_END_SCOPE ')' 
	{
	scope_var--;
	if (scope_var == 0)
	{ 
		begin_epilogue = 1;
		in_scope = 0;
	}
	};

stab:	'(' TT_STAB TT_IDENT { clear_stab_vars(); } fields ')' 
	{
	}
	;

fields:	field fields {}
      | field {}
      ;

field:	'(' TT_VAL TT_INT_VAL ')' 
		{ 
		} 
     | 	'(' TT_VAL TT_IDENT ')' 
		{ 
		} 
      |	'(' TT_SCL TT_INT_VAL ')' { stab_scl = $3; } 
      |	'(' TT_SIZE TT_INT_VAL ')' { stab_size = $3; } 
      |	'(' TT_TYPE TT_INT_VAL ')' 
      		{
      		   stab_type = $3;
      		} 
      |	'(' TT_DIM dimlist ')' { /* NYI */ }
      |	'(' TT_TAG TT_IDENT ')' 
      		{
      		   /* NYI */ 
		}
      ;

dimlist : dimlist TT_INT_VAL {}
	| TT_INT_VAL {}
	;

decl:	'(' TT_DECL is_global alignment TT_IDENT initializer ')'
		{ 
		} 
	|
     	'(' TT_DECL is_global TT_IDENT initializer ')'
		{ 
		} 
	;

global:	'(' TT_GLOBAL TT_IDENT ')' 
	{ 
	} ;
	
is_global: '(' TT_IS_GLOBAL TT_INT_VAL ')' { $$ = $3; } ;

alignment: '(' TT_ALIGN TT_INT_VAL ')' { $$ = $3; } ;

initializer: inits {  };

inits:	init inits {}
	| init {}
	;

init:	word_init {}
	| half_init {}
	| byte_init {}
	| ascii_init {}
	| space_init {}
	;

word_init: '(' TT_WORD TT_INT_VAL {  } ')' 
         | '(' TT_WORD TT_IDENT 
         	{ 
         	}
           ')' 
         | '(' TT_WORD TT_IDENT '+' TT_INT_VAL
         	{ 
         	}
           ')' 
	 ;

half_init: '(' TT_HALF TT_INT_VAL {  } ')' ;

byte_init: '(' TT_BYTE TT_INT_VAL { } ')' ;

ascii_init: '(' TT_ASCII TT_IDENT ')' 
		{
		} ;

space_init: '(' TT_SPACE TT_INT_VAL {  } ')' ;

routines: 
	routine				{/** 2Jan97 TESTING $$ = $1;*/curProgram->appendRoutine(curRoutine);}
	| 
	routine {curProgram->appendRoutine(curRoutine);} routines	
	| 
	global_pseudos routines	{}
	| 
	global_pseudos				{}
;

routine: 
	'(' TT_PROCBEGIN				{looking_for_tt_op = 1; in_routine=1; begin_prologue=1; scope_var=0; /* added 12/16  in_scope=1;*/}
	TT_IDENT 
									{
											printf("name of procedure--%s\n",$4);
											curRoutine = new Routine();

											$$ = curRoutine;
											curRoutine->setName($4);
											free($4);	
									}
	statements ')'					
	{
		NormalInstr *ins = buildMIPSInstr("RETURN");
		curRoutine->appendInstruction(ins);

		looking_for_tt_op = 0; in_routine=0; begin_epilogue=0; /* added 12/16  in_scope=0;*/
	}
;

statements:	 
	statement
	|	 
	statement statements
;

statement:	
	TT_OP
			{
			//	printf("tt_op: %d %s\n", globali++, $1);
				if (in_scope) {
					int i = formatStrings($1);
					NormalInstr *instr;
					if (has_label){
						instr = buildMIPSInstr(instrStr,labelName);
			//			 printf("%s---%s\n",instrStr,labelName); 
					}
					else{
						instr = buildMIPSInstr(instrStr);
 			//			 printf("%s\n",instrStr); 
					}
					if(i>0){
						addAssertion(instr,assertBuf);
					}	
					has_label = 0;
					curRoutine->appendInstruction(instr);
					clearAllStrings();
				}
				else // not in scope
				{
					if (has_label) // i.e. the label is the last statement in the .s file.
					{
						NormalInstr *instr;
						instr = buildMIPSInstr("NOP", labelName);
						has_label = 0;
						curRoutine->appendInstruction(instr);
					}
					else if (begin_prologue)
					{
						formatStrings($1);
						BaseOperation *oper;
						oper = buildMIPSOp(instrStr);
						/* printf("%s\n",instrStr); */

						curRoutine->addPrologueOp(oper);
						delete oper;
						clearAllStrings();
					}	
					else if (begin_epilogue)
					{
						formatStrings($1);
						BaseOperation *oper;
						oper = buildMIPSOp(instrStr);
						/* printf("%s\n",instrStr); */

						curRoutine->addEpilogueOp(oper);
						delete oper;
						clearAllStrings();
					}	
				}
				free($1);
			}
	|      
	'(' TT_LABEL TT_IDENT ')'
			{
				if (in_scope) {
					has_label = 1;
					assert(strlen(labelName)<STRLEN_NOR);
					strcpy(labelName, $3);
				}
				free($3);
			}
	|      
	'(' TT_CASETABLE '(' TT_LABEL TT_IDENT ')' ')' case_list		{}
	|      
	local_pseudos 
	;

Case:	TT_CASE TT_IDENT
		{ 
		}
	;

case_list: {  } caselist { $$ = $2; }

caselist: Case Case
		{
		}
	| Case caselist
		{
		}
	;


%%

void
clear_stab_vars() {
   ignore_stab = stab_val = stab_scl = stab_size = stab_type = 0;
}

int formatStrings(char *op)
{
	assert(strlen(op)<STRLEN_MAX);
	int index = 0,i=0;
	char *str = op;
	str ++; //skip the first (
	while (((*str)!='(')&&((*str)!=')')) 
		instrStr[index++] = *str++;
	if ((*str)=='(') {
		str++;
		while ((*str)!=')') 
			assertBuf[i++]=*str++; //skip the inside (...)
		str++;
		while ((*str)!=')')
			instrStr[index++] = *str++;
	}
	instrStr[index] = 0;
	assertBuf[i]='\0';
	return i;
}

void clearAllStrings(void)
{
}

void addAssertion(NormalInstr *instr,char *assertBuf)
{
  char *p,name[200],val[200];
  int i=0,j=0;
  for(p=assertBuf;*p!=' ';p++) name[i++]=*p;
  name[i]='\0';
  for(;*p==' ';p++);
  for(;*p!=')' && *p!='\0';p++) val[j++]=*p;  
  val[j]='\0';
  if(strcmp(name,"PART_OF")==0){
    BaseOpProperty *prop=new PartofProperty(val);
    BaseOperation *op=instr->getPtrToRootOperation();
    op->addProperty(*prop);
    delete prop;
  }
}

void printMode(ostream&out, char *mode)
{
  if (strcmp("b",mode)==0)
    out<<"char";
  else if (strcmp("i",mode)==0)
    out<<"int";
  else if (strcmp("u",mode)==0)
    out<<"unsigned";
  else if (strcmp("s",mode)==0)
    out<<"short";
  else if (strcmp("f",mode)==0)
    out<<"float";
  else if (strcmp("d",mode)==0)
    out<<"double";
  else if (strcmp("w1",mode)==0)
    out<<"float";
  else if (strcmp("w2",mode)==0)
    out<<"double";
  else if (strcmp("a",mode)==0)
    out<<"double";
  else 
    assert(0);
}

void printModeVal(ostream&out, char *mode)
{
  if (strcmp("b",mode)==0)
    out<<"._as.byte";
  else if (strcmp("i",mode)==0)
    out<<"._as.integer";
  else if (strcmp("u",mode)==0)
    out<<"._as.uinteger";
  else if (strcmp("s",mode)==0)
    out<<"._as.short_int";
  else if (strcmp("f",mode)==0)
    out<<"._as.flt";
  else if (strcmp("d",mode)==0)
    out<<"._as.dbl";
  else if (strcmp("w1",mode)==0)
    out<<"._as.flt";
  else if (strcmp("w2",mode)==0)
    out<<"._as.dbl";
  else if (strcmp("a",mode)==0)
    out<<"._as.dbl";
  else 
    assert(0);
}

void gen_routine_header(char *r_name, char *r_type)
{
  tempRoutFile = fopen(tempRoutFileName,"w");
  if (tempRoutFile==NULL)
    assert(0);

  if (is_ld(r_type)) {
    fprintf(exeSemFile, "void %s_EXEC(val& dest, val& src0, val& src1, Connection *con)\n" ,r_name);
    fprintf(exeSemFile, "{\n");
    fprintf(tempFile, "%snew exeMemSem(\"%s_EXEC\",%s_EXEC);\n", 
	indentStr,r_name ,r_name);
  }
  else if (is_st(r_type)) {
    fprintf(exeSemFile, "void %s_EXEC(val& src0, val& src1, val& src2, Connection *con)\n" ,r_name);
    fprintf(exeSemFile, "{\n");
    fprintf(tempFile, "%snew exeMemSem(\"%s_EXEC\",%s_EXEC);\n", 
	indentStr,r_name ,r_name);
  }
  else  {
    fprintf(exeSemFile, "void %s_SEM_EXEC(val& dest, val& src0, val& src1)\n" 
	,r_name);
    fprintf(exeSemFile, "{\n");
    fprintf(tempFile, "%snew exeCompSem(\"%s_SEM_EXEC\",%s_SEM_EXEC);\n",
               indentStr,r_name,r_name);
  }
 tempVarCount = 0;
}

void gen_routine_content()
{
  if (tempVarCount!=0) {
    if (tempVarCount==1)
      fprintf(exeSemFile,"%sval %s_1;\n\n",indentStr,tempVarName);
    else {
      fprintf(exeSemFile,"%sval ",indentStr);
      for (int i=0; i<tempVarCount-1; i++)
        fprintf(exeSemFile, " %s_%i,",tempVarName,i);
      fprintf(exeSemFile, " %s_%i;\n\n",tempVarName,tempVarCount);
    }
  }
  fclose(tempRoutFile);
  char *contents = read_file(tempRoutFileName);
  fprintf (exeSemFile,"%s",contents);
  free(contents);
}

void gen_routine_tail()
{
  fprintf(exeSemFile,"}\n\n");
}

void start_gen_sem()
{
  exeSemFile = fopen(exeSemFileName,"w");
  if (exeSemFile==NULL)
    assert(0);
  tempFile = fopen(tempFileName, "w");
  if (tempFile==NULL)
    assert(0);
  fprintf(exeSemFile, "/* This file is generated automatically */\n");
  fprintf(exeSemFile,"#include <math.h> \n");
  fprintf(exeSemFile,"#include \"DataValue.h\" \n");
  fprintf(exeSemFile,"#include \"Connection.h\" \n");
  fprintf(exeSemFile,"#include \"exeSem.h\" \n\n");

  fprintf(tempFile, "void addExecSem()\n");
  fprintf(tempFile, "{\n\n");
}

void end_gen_sem()
{
  char *lclptr;

  fprintf(tempFile,"}\n");
  fclose(tempFile);

  lclptr = read_file(tempFileName);
  fprintf(exeSemFile,"\n%s",lclptr);
  fclose(exeSemFile);
  free(lclptr);
}

char *read_file(char *fileName)
{
  long fsize;
  char *lclptr;
  FILE *fptr;

  fptr = fopen(fileName,"r");
  if (fptr == NULL) {
    assert(0);
  }
  fseek(fptr,0,2);
  fsize = ftell(fptr);
  rewind(fptr);
  lclptr = (char *)malloc(fsize + 1);
  fread(lclptr,sizeof(char), fsize, fptr);
  lclptr[fsize] = '\0';
  fclose(fptr);
  return lclptr;
}

void gen_assign(char *dest, char *mode, char *rhs)
{
  strstream str;
  char *temp;

  str<<indentStr<<"dest";
  printModeVal(str,mode);
  str<<" = "<<rhs<<";\n"<<ends;
  temp =str.str();
  fprintf(tempRoutFile,"%s",temp);
  delete []temp;
}

void gen_st(char *dest_addr, char *mode, char *content)
{ 
  char *tempName;
  strstream str, retstr;

  tempVarCount++;
  str<<tempVarName<<"_"<<tempVarCount<<ends;
  tempName = str.str();
  
  retstr<<indentStr<<tempName<<".changeValue("<<content<<");\n";
  retstr<<indentStr<<"con->write("<<dest_addr<<","<<tempName<<");\n"<<ends;
  delete []tempName;
  tempName = retstr.str();
  fprintf(tempRoutFile,"%s",tempName);
  delete []tempName;
}

void gen_ld(char *dest, char *dest_mode, char *src_addr, char *src_mode)
{
  char *tempName;
  strstream str, retstr;

  tempVarCount++;
  str<<tempVarName<<"_"<<tempVarCount<<ends;
  tempName = str.str();

  retstr<<indentStr<<tempName << " = con->read("<<src_addr<<");\n";
  retstr<<indentStr<<dest;
  printModeVal(retstr,dest_mode);
  retstr<<" = "<<tempName;
  printModeVal(retstr,src_mode);
  retstr<<";\n"<<ends;
  
  delete []tempName;
  tempName = retstr.str();
  fprintf(tempRoutFile,"%s",tempName);
  delete []tempName;
}

int is_ld(char *type)
{
  if (strcmp(type,"READS_MEM")==0)
    return 1;
  return 0;
}

int is_st(char *type)
{
  if (strcmp(type,"WRITES_MEM")==0)
    return 1;
  return 0;
}
