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
%token	TT_DEF
%token  TT_TYPE_DEF
%token  TT_DECL

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

#include "defs_node.h"
#include "symbols.h"
#include "symtabs.h"
#include <vector>


//prototypes
extern void syntaxError(char * msg);
extern int defslex();
extern int defsparse();
#define defserror syntaxError
//the list of definitions (DEF ...) from the defs file
extern vector<defs_node*> defs_list;
//the list of declarations (DECL ...) from the defs file
extern vector<defs_node*> crt_decl_list;
extern char *globaldefsfilename;


//#include "express_defs_parser.cpp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

char buf[1000]; //temporary char buffer used in parser

//the symbol table
extern Symtab st;

//the global defintions pointer
extern defs_node_list *global_definitions;

%}

/* Declare the alternative types of the grammar symbols. */

%union {
	int in;
	float fl;
	char ch;
	char *st;
	defs_node *nodet;
	defs_node_list * nodelistt;

}

/* These are the specific types of the grammar symbols */

/* Terminals */

%type <in> TT_INTEGER
%type <st> TT_FLOAT
%type <st> TT_CHARSTRING TT_IDENTIFIER 


/* Non-terminals */

%type <nodet> node 
%type <nodelistt> node_list 




%start definitions

%%

definitions: 
	node_list				{global_definitions=$1;}
	|
	/* nothing */			{}
	;

node:
	TT_CHARSTRING			{$$=new defs_node($1);}
	|
	TT_IDENTIFIER			{$$=new defs_node($1);}
	|
	TT_INTEGER				{$$=new defs_node(itoa($1,buf,10));}
	|
	'*'						{$$=new defs_node("*");}
	|
	TT_FLOAT				{$$=new defs_node($1);}
	|
	'(' TT_DEF TT_CHARSTRING node_list  ')'		
				{$$=new defs_node($3); $$->AddChildren($4);/*st.Declare($3,$$);*/ defs_list.push_back($$);}
	
	|
	'(' TT_DECL node_list TT_IDENTIFIER node_list  ')'		
				{$$=new defs_node($4);$$->AddChildren($3);$$->AddChildren($5);st.Declare($4,$$);crt_decl_list.push_back($$);}
	|
	'(' TT_DECL node_list TT_IDENTIFIER ')'		
				{$$=new defs_node($4);$$->AddChildren($3);st.Declare($4,$$);crt_decl_list.push_back($$);}

	|
	'(' TT_TYPE_DEF node_list ')'		{$$=new defs_node(); $$->AddChildren($3);}
	|
	'(' node_list  ')'		{$$=new defs_node(); $$->AddChildren($2);}
	;

node_list:
	node_list node			{$1->push_back($2);$$=$1;}
	|
	node					{$$=new defs_node_list(); $$->push_back($1);}
	;

%%