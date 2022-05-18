/********************************************/
/* file: express.y							*/
/* created: Wed Jun 30 1999					*/
/* authors: Ashok Halambi					*/
/* last modified:							*/
/* contents: bison grammar for EXPRESS		*/
/********************************************/

%token	TT_GLOBAL
%token	TT_PROCBEGIN
%token	TT_LOCATION
%token	TT_LABEL
%token	TT_PSEUDOS
%token	TT_parm
%token	TT_BEGINSCOPE
%token	TT_ENDSCOPE
%token	TT_PROLOGUE
%token	TT_EPILOGUE
%token	TT_END
%token	TT_NAME
%token	TT_BASETYPENAME
%token	TT_VALUE
%token  TT_DOLLAR
%token	TT_INTEGER
%token	TT_FLOAT
%token	TT_CHARSTRING
%token	TT_IDENTIFIER

%token '(' ')' '*' '+' '-' ',' '/' ':' '<' '=' '>' '[' ']' '\\'

%{
//#include "stdafx.h"

#include <malloc.h>

#ifdef __UNIX__
#include <strstream.h>
#else
#include <strstrea.h>
#endif

//Note: It is very important that this #define should
// be before including the corresponding ".cpp.h" file.
// In this case, expressParser.h includes the express_parser.cpp.h
// file, so the #define is before it.
//
//
#define YYDEBUG 1

#include "Routine.h"
#include "Program.h"
#include "Assertions.h"
#include "expressParser.h"
#include "malloc.h"
#include "BaseOperation.h"
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

EXPRESS_PARSER_MODE curMode = _NO_MODE_;
int currLocation = 0;
int numArgs = 0;
int hasLabel = 0;
int inScope = 0;

char labelName[EXPRESS_LABEL_MAX_STRLEN];

extern Program *curProgram;

Routine *curRoutine;

extern NormalInstr *buildMIPSInstr(char *, char * = NULL);
extern BaseOperation *buildMIPSOp(char *, char * = NULL);
extern void addToParmList(char *);
extern void addToParmList(EXPRESS_ARG_TYPE aT, int num);
extern void addToParmList(EXPRESS_ARG_TYPE aT, char *str);
extern void clearParmList();

extern void insertIntoIR(Routine *curRoutine, char *opcode, int numArgs,
						EXPRESS_PARSER_MODE curMode, int hasLabel, int currLoc);

extern void addToArgArray(int in, EXPRESS_ARG_TYPE aT);
extern void addToArgArray(char *st, EXPRESS_ARG_TYPE aT);

extern Assertion * create_new_assertion(char *name,char *value);
//void add_assertion_to_stmt(Assertion *assert,BaseOperation *oper);
void add_assertion_to_stmt(BaseArgProperty *assert,BaseOperation *oper);

extern MyString globalCurrFileName;

extern BaseOperation *globalOperationAdded;

%}

/* Declare the alternative types of the grammar symbols. */

%union {
	int in;
	float fl;
	char ch;
	char *st;
	Routine *rout;
	BaseArgProperty *assert;
}

/* These are the specific types of the grammar symbols */

/* Terminals */

%type <in> TT_INTEGER
%type <st> TT_FLOAT
%type <st> TT_CHARSTRING TT_IDENTIFIER
%type <assert> assertion type1_assert type2_assert

/* Non-terminals */

%type <rout> routine

%start program

%%

program: /* nothing */
		{
			yyerror("premature EOF\n"); 
		}
		| routine_list 
		{ 
			currLocation = 0;
			return(0); 
		};

routine_list:
		routine
		{
			curProgram->appendRoutine(curRoutine);
		}
		|
		routine
		{
			curProgram->appendRoutine(curRoutine);
		}
		routine_list
		|
		global_stmts routine_list { }
		|
		global_stmts { }
		;

global_stmts:
		location_stmt
/*
	Note: In general, global_stmts may have more than 1 stmt.
	Currently, because of shift-reduce problems, it accepts
	only 1 location_stmt.
*/
/*		|
		location_stmt global_stmts
*/
		;

routine:
		'(' TT_PROCBEGIN TT_IDENTIFIER
		{
//			if (EXPRESS_PARSER_DEBUG)
				printf("Name of procedure: %s\n",$3);

			curRoutine = new Routine();

			$$ = curRoutine;
			curRoutine->setName($3);

			inScope = 0;
			curMode = _INIT_STMTS_;
			
			// Add it to the filesToRoutinesMap.
			//
			MyString s($3);
			curProgram->addToFileRoutineMap(globalCurrFileName, s);

			// perform any initializations.
			//
			parserRoutInit();
			free($3);	
		}
		actual_routine
		{
			if (inScope != 0)
			{
				// error condition.
				//
				printf("Scope count for routine %s was incorrect.\n", $3);
			}
		}
		')'
		;

/* A routine is 

	init_stmts
	prologue_stmts
	[start_stmts]
	routine_body
	[end_stmts]
	epilogue_smts
*/

actual_routine:
		init_stmts
		/*prologue_stmts*/
		/*start_stmts*/
		routine_body
		/*end_stmts*/
		/*epilogue_stmts*/

/* Note: The following gave shift reduce errors. */
/*   
		|
		init_stmts
		prologue_stmts
		start_stmts
		routine_body
		epilogue_stmts
		|
		init_stmts
		prologue_stmts
		routine_body
		end_stmts
		epilogue_stmts
		|
		init_stmts
		prologue_stmts
		routine_body
		epilogue_stmts
*/
		;

init_stmts:
		'(' label_stmt
/*
	Note: In general, init_stmts may have more than 1 stmt.
	Currently, because of shift-reduce problems, it accepts
	only 1 label_stmt.
*/
/*
		|
		label_stmt init_stmts
*/
		;

start_stmts:
		{
			curMode = _START_STMTS_;
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In start_stmts\n");
				fflush(stdout);
			}

		}
		start_end_stmts
		;

end_stmts:
		{
			curMode = _END_STMTS_;
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In end_stmts\n");
				fflush(stdout);
			}
		}
		start_end_stmts

/* An end stmt typically is a location or a normal stmt stmt */
/*		start_end_stmts */
		;

start_end_stmts:
/*
		location_stmt
		| 
		normal_stmt 
*/
/*
	Note: In general, start_end_stmts may have multiple stmts.
	Currently, because of shift-reduce problems, it accepts
	only 1 location_stmt, or 1 normal_stmt or a combination
	of the two.
*/
/*
		|
		location_stmt start_end_stmts
		|
		normal_stmt start_end_stmts
*/
/*
		location_stmt 
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In start_end: Finished location stmt\n");
				fflush(stdout);
			}
		}
		normal_stmt
		|
		normal_stmt
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In start_end: Finished normal stmt\n");
				fflush(stdout);
			}
		}
		location_stmt
*/
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In start_end_stmts some instr\n");
				fflush(stdout);
			}
		}
		statement_list
		|
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In start_end_stmts empty\n");
				fflush(stdout);
			}
		}
		'(' ')'
		;

prologue_stmts:
		start_prologue
		{
			curMode = _PROLOGUE_;
			hasLabel = 0;

			if (EXPRESS_PARSER_DEBUG)
			{
				printf("Finished start_prologue\n");
				fflush(stdout);
			}
		}
		statement_list
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("Starting end_prologue\n");
				fflush(stdout);
			}
		}
		end_prologue
		;

start_prologue:
		'(' TT_PROLOGUE ')'
		;

end_prologue:
		'(' 
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In end prologue\n");
				fflush(stdout);
			}
		}
		TT_END TT_PROLOGUE ')'
		;

epilogue_stmts:
		start_epilogue
		{
			curMode = _EPILOGUE_;
//			hasLabel = 0;
		}
		statement_list
		end_epilogue
		{
				insertIntoIR(curRoutine, "RETURN", 0, curMode, hasLabel, currLocation);
		}
		;

start_epilogue:
		'(' TT_EPILOGUE ')'
		;

end_epilogue:
		'(' TT_END TT_EPILOGUE ')'
		;

routine_body:
		/* start_body */
		{
			curMode = _BODY_;
			clearParmList();
			hasLabel = 0;
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In routine_body\n");
				fflush(stdout);
			}
		}
		statement_list
		/* end_body */
		{
		/* Modified by SAH on Feb 26, 2001. */
			if (curRoutine->isMainRoutine())
			{
			/*	insertIntoIR(curRoutine, "RETURN", 0, curMode, hasLabel, currLocation);*/
			/*  hasLabel = 0; */
			}
			if (hasLabel)
			{
				insertIntoIR(curRoutine, "RETURN", 0, curMode, hasLabel, currLocation);
			}
			hasLabel = 0;
		}
		;

start_body:
		begin_scope
		;

end_body:
		end_scope
		;

statement_list:
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In statement_list many stmts\n");
				fflush(stdout);
			}
		}
		/*'(' */
		'(' statement 
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("Parsed a statement\n");
				fflush(stdout);
			}
		}
		stmt_list
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("Finished statement_list\n");
				fflush(stdout);
			}
		}
		/* ')' */

|
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In statement_list single stmt\n");
				fflush(stdout);
			}
		}
		/* '(' */
		'(' statement
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("Parsed a statement\n");
				fflush(stdout);
			}
		}
		/* ')' */
		;

stmt_list:
		'(' statement stmt_list
		|
		'(' statement
		;


statement:
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In location_stmt\n");
				fflush(stdout);
			}
		}
		location_stmt
		|
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In normal_stmt\n");
				fflush(stdout);
			}
		}
		normal_stmt
		|
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In label_stmt\n");
				fflush(stdout);
			}
		}
		label_stmt
		|
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In pseudos_stmt\n");
				fflush(stdout);
			}
		}
		pseudos_stmt
		/* 
		|
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In misc_stmt\n");
				fflush(stdout);
			}
		}
		'(' misc_stmt
		*/
		;

label_stmt:
		TT_LABEL TT_IDENTIFIER ')'
		{
			if (EXPRESS_PARSER_DEBUG)
				printf("Label: %s\n", $2); 
			hasLabel = 1;
			strcpy(labelName, $2);
			free($2);
		}
		;

location_stmt:
		TT_LOCATION TT_INTEGER ')'
		{
			if (EXPRESS_PARSER_DEBUG)
				printf("Loc: %d\n", $2);
			currLocation = $2;
		}
		;

pseudos_stmt:
		TT_PSEUDOS 
		{
			if (EXPRESS_PARSER_DEBUG)
				printf("In Pseudos_stmt\n");
		}
		')'
		|
		TT_PSEUDOS 
		{
			if (EXPRESS_PARSER_DEBUG)
				printf("In Pseudos_stmt\n");
		}
		parm_list
		')'
		;

parm_list:
		parm_one
		|
		parm_one parm_list
		;

parm_one:
		'(' 
		/*{
			if (EXPRESS_PARSER_DEBUG)
				printf("In Parm\n");
		}*/
		TT_parm TT_DOLLAR TT_IDENTIFIER ')'
		{
			if (EXPRESS_PARSER_DEBUG)
				printf("Parm: %s\n", $4);

			addToParmList(_EXPRESS_REG_STRING_,$4);
			free($4);
		}
		|
		'(' TT_parm TT_DOLLAR TT_INTEGER ')'
		{
			if (EXPRESS_PARSER_DEBUG)
				printf("Parm: %d\n", $4);

			addToParmList(_EXPRESS_REG_INT_, $4);
		}

/*
		|
 		'(' 
		TT_parm TT_IDENTIFIER ')'
		{
			if (EXPRESS_PARSER_DEBUG)
				printf("Parm: %s\n", $3);

			addToParmList($3);
			free($3);
		}
*/
		;

misc_stmt:
		begin_scope
		|
		end_scope
		;

begin_scope:
		'(' TT_BEGINSCOPE ')'
		{
			inScope++;
		}
		;

end_scope:
		'(' TT_ENDSCOPE ')'
		{
			inScope--;
		}
		;

normal_stmt:
		/*{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In normal_stmt 3\n");
				fflush(stdout);
			}
		}*/

		TT_IDENTIFIER assertion
		{
			numArgs = 0;
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In normal_stmt: %s\n", $1);
				fflush(stdout);
			}
		}
		arg_list 
		')'
		{
			insertIntoIR(curRoutine, $1, numArgs, curMode, hasLabel, currLocation);
			hasLabel = 0;
			add_assertion_to_stmt($2,globalOperationAdded);
			clearArgArray();
			clearAssertionArray();
		}

		|

		/*{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In normal_stmt 1\n");
				fflush(stdout);
			}
		}
		

		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In normal_stmt 2\n");
				fflush(stdout);
			}
		}*/
		
		TT_IDENTIFIER 
		{
			numArgs = 0;
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In normal_stmt: %s\n", $1);
				fflush(stdout);
			}
		}
		arg_list 
		')'
		{
			//insertIntoIR(curRoutine, $2, numArgs, curMode, hasLabel, currLocation);
			insertIntoIR(curRoutine, $1, numArgs, curMode, hasLabel, currLocation);
			hasLabel = 0;
			clearArgArray();
			clearAssertionArray();
		}
		;


arg_list:
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In arg_list\n");
				fflush(stdout);
			}
		}
		arg
		{
			numArgs++;
		}
		arg_list
		|
		;

arg:
		arg_val
		|
		arg_val 
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("After arg, before assertion\n");
				fflush(stdout);
			}
		}
		assertion
		{
			addToAssertionArray($3);
		}
		;

arg_val:
		TT_DOLLAR TT_INTEGER
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In arg_val: R %d\n", $2);
				fflush(stdout);
			}
			addToArgArray($2, _EXPRESS_REG_INT_);
		}
		|
		TT_DOLLAR TT_IDENTIFIER
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In arg_val: Reg: %s\n", $2);
				fflush(stdout);
			}
			addToArgArray($2, _EXPRESS_REG_STRING_);
			free($2);
		}
		|
		TT_INTEGER
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In arg_val: Imm: %d\n", $1);
				fflush(stdout);
			}
			addToArgArray($1, _EXPRESS_IMM_INT_);
		}
		|
		TT_FLOAT
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In arg_val: Imm: %s\n", $1);
				fflush(stdout);
			}
			addToArgArray($1, _EXPRESS_IMM_FLOAT_);
			free($1);
		}
		|
		TT_IDENTIFIER
		{
			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In arg_val: LABEL: %s\n", $1);
				fflush(stdout);
			}

			addToArgArray($1, _EXPRESS_LABEL_);
			free($1);
		}
		|
		TT_IDENTIFIER '+' TT_INTEGER
		{
//			printf("Warning: arg = IDENT operator NUM is parsed but not handled correctly\n");

			if (EXPRESS_PARSER_DEBUG)
			{
				printf("In arg_val: LABEL: %s, INT: %d\n", $1, $3);
				fflush(stdout);
			}

			addToArgArray($1, _EXPRESS_LABEL_);
			numArgs++;
			addToArgArray($3, _EXPRESS_IMM_INT_);
			free($1);
		}
		;

assertion:
		type2_assert	{$$=$1;}
		|
		type1_assert	{$$=$1;}
		;

type1_assert:
		'(' TT_NAME TT_CHARSTRING ',' TT_BASETYPENAME TT_CHARSTRING ')'
		{
			$$=createHLInfoAssertion($3,$6);
			free($3);
			free($6);
		}
		;

type2_assert:
		'(' TT_NAME TT_CHARSTRING ',' TT_VALUE TT_CHARSTRING ')'
		{
			$$=createHLInfoAssertion($3,$6);
			free($3);
			free($6);
		}
		;

%%

