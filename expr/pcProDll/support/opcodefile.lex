/********************************************/
/* file: GenericOpCodeSpecFile.lex			*/
/* created: Oct 30 2000					    */
/* authors: Aviral Shrivastava				*/
/* last modified:							*/
/* contents: flex scanner for EXPRESS		*/
/********************************************/

%{
//#include "stdafx.h"

#include <ctype.h>
#include <string.h>
#ifdef __UNIX__
	#include <iostream.h>
#else
	#include <io.h>
	#define isatty _isatty
#endif

extern int opcodelex();
extern int opcodeparse();

extern char *copy(char *s);

#include "opcodefile_parser.cpp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#define opcodewrap() 1

%}

DIGIT	[0-9]
LETTER	[A-Za-z]
ALFA	{LETTER}|{DIGIT}

%%
OPCODE			return TT_OPCODE;
DATATYPES		return TT_DATATYPES;
_DST_LIST_		return TT_DST_LIST_;
_SRC_LIST_		return TT_SRC_LIST_;
i				return TT_INTEGER;
d				return TT_DOUBLE;
f				return TT_FLOAT;
a				return TT_ANY;
\(				return TT_OPEN_PAREN;
\)				return TT_CLOSE_PAREN;
:				return TT_TYPE;
,				return TT_AND;			


(_|{ALFA})+					{
                               opcodelval.st=copy(yytext);
							   /* printf("ident: %s\n",yytext); */
                               return	TT_STRING;
                           }

\;.*						{ /* gobble comments */}

[ \t\b]+					{ /* gobble whitespace */ }
\n							{ /* gobble line feed */ }

.				return opcodetext[0];

%%
