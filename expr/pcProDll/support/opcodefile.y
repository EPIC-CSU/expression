/********************************************************/
/* file: GenericOpCodeSpecfile.y						*/
/* created: October 30, 2000							*/
/* authors: Aviral Shrivastava							*/
/* last modified:										*/
/* contents: bison grammar for GenericOpCodeSpecFile	*/
/********************************************************/

%token	TT_OPEN_PAREN
%token	TT_CLOSE_PAREN
%token	TT_OPCODE
%token	TT_DATATYPES
%token	TT_DST_LIST_
%token	TT_SRC_LIST_
%token	TT_AND
%token	TT_TYPE
%token	TT_STRING
%token	TT_INTEGER
%token	TT_FLOAT
%token	TT_ANY
%token	TT_DOUBLE

%{
//#include "stdafx.h"

#include <malloc.h>
#include <string.h>

#ifdef __UNIX__
#include <strstream.h>
#else
#include <strstrea.h>
#endif

#include "ArchitectureIncludes.h"
#include "BaseOperation.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#define YYDEBUG 1

extern void syntaxError(char * msg);
extern int opcodelex();
extern int opcodeparse();
#define opcodeerror syntaxError

int destOrSrc = 0;
int currOpcodeIndex = _INVALID_;
int currOperandNum = _INVALID_;

extern int getOpcodeIndex(char *);
extern int getOperandNum(char *);
extern void addOperandToDataTypeArray(int, int, int, int);

%}

/* Declare the alternative types of the grammar symbols. */

%union {
	int in;
	float fl;
	char ch;
	char *st;
}

/* Types for terminals */

%type <st> TT_STRING
%type <ch> TT_INTEGER TT_FLOAT TT_DOUBLE TT_ANY

/* Types for non-terminals */

%type <st> opcode
%type <st> operand
%type <in> dtype

%start opcodefile

%%

opcodefile			: /* nothing */
					| opcode_def_list 
					;

opcode_def_list		: opcode_def
					|
					  opcode_def opcode_def_list
					;

opcode_def			: TT_OPEN_PAREN TT_OPCODE opcode 
						{
//							cout << "Opcode : " << $3 << endl;
							currOpcodeIndex = getOpcodeIndex($3);
							free($3);
						}
					datatypes TT_CLOSE_PAREN
					;


datatypes			: TT_OPEN_PAREN TT_DATATYPES destination_list source_list TT_CLOSE_PAREN
					|
					TT_OPEN_PAREN TT_DATATYPES source_list TT_CLOSE_PAREN
					|
					TT_OPEN_PAREN TT_DATATYPES destination_list TT_CLOSE_PAREN
					|
					TT_OPEN_PAREN TT_DATATYPES TT_CLOSE_PAREN
					;


destination_list	: TT_OPEN_PAREN TT_DST_LIST_ 
						{
							destOrSrc = _DEST_LIST_;
						}
					operand_type_list TT_CLOSE_PAREN
					;


source_list			: TT_OPEN_PAREN TT_SRC_LIST_ 
						{
							destOrSrc = _SRC_LIST_;
						}
					operand_type_list TT_CLOSE_PAREN
					;

operand_type_list	: TT_OPEN_PAREN operand_types TT_CLOSE_PAREN
					;

operand_types		: operand_type TT_AND operand_types
					|
					operand_type
					;

operand_type		: operand TT_TYPE dtype
						{
							currOperandNum = getOperandNum($1);
							free($1);
							addOperandToDataTypeArray(currOpcodeIndex, currOperandNum, $3, destOrSrc);
						}
					;

opcode				: TT_STRING
						{
							$$ = $1;
						}
					;


operand				: TT_STRING
						{
							$$ = $1;
						}
					;


dtype				: TT_INTEGER
						{
							$$ = _INT_;
						}
					| TT_DOUBLE
						{
							$$ = _DOUBLE_;
						}
					| TT_FLOAT
						{
							$$ = _FLOAT_;
						}
					| TT_ANY
						{
							$$ = _ANY_;
						}
					;					

%%


