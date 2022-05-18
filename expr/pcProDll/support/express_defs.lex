/********************************************/
/* file: express.lex						*/
/* created: Wed Jun 30 1999					*/
/* authors: Gabi Grun    					*/
/* last modified:							*/
/* contents: flex scanner for EXPRESS		*/
/********************************************/

%{
//#include "stdafx.h"

//#include "expressParser.h"

#include <ctype.h>
#include <string.h>
#ifdef __UNIX__
#include <stdio.h>
#else
#include <io.h>
#endif

extern int defslex();
extern int defsparse();


#define EXPRESS_DEFS_PARSER_DEBUG 0

#include "defs_node.h"
extern char *copy(char *s);
extern char * strip_quotes_and_copy(char *s, int len);
extern void incrementLineNumber(void);
extern long lineNumber(void);
extern void setFileName(char *fileName);
extern void syntaxError(char * msg);
extern char * copy_and_replace_underscore(char *s);


#include "express_defs_parser.cpp.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#define defswrap() 1

#ifdef WIN32
#define isatty _isatty
#endif


/*
DECL 			return TT_DECL;

GLOBAL	 		return TT_GLOBAL;
PROC_BEGIN		return TT_PROCBEGIN;

LOCATION		return TT_LOCATION;
LABEL			return TT_LABEL;
PSEUDOS	 		return TT_PSEUDOS;
parm			return TT_parm;

BEGIN_SCOPE		return TT_BEGINSCOPE;
END_SCOPE		return TT_ENDSCOPE;

NAME			return TT_NAME;
BASETYPENAME	return TT_BASETYPENAME;
VALUE			return TT_VALUE;

*/


%}

DIGIT	[0-9]
LETTER	[A-Za-z]
ALFA	{LETTER}|{DIGIT}

%%


DEF	 			return TT_DEF;
TYPE_DEF	 	return TT_TYPE_DEF;
DECL		 	return TT_DECL;


PROLOGUE		{
					if (EXPRESS_DEFS_PARSER_DEBUG)
					{
						printf(" PROLOGUE ");
						fflush(stdout);
					}
					return TT_PROLOGUE;
				}
EPILOGUE		return TT_EPILOGUE;
END				{
					if (EXPRESS_DEFS_PARSER_DEBUG)
					{
						printf(" END ");
						fflush(stdout);
					}
					return TT_END;
				}


\$				return TT_DOLLAR;


-?{DIGIT}+[.]{DIGIT}+e[-+]?{DIGIT}+	{
								defslval.st = copy(yytext);
								return TT_FLOAT;
							}

-?{DIGIT}+e[-+]?{DIGIT}+	{
								defslval.st = copy(yytext);
								return TT_FLOAT;
							}

-?{DIGIT}+[.]{DIGIT}+		{
								defslval.st = copy(yytext);
								return TT_FLOAT;
							}

-?0x[0-9A-Fa-f]+			{ 
								defslval.in = strtol(yytext,NULL,0); 
								return TT_INTEGER;
							}

-?{DIGIT}+					{ 
								defslval.in = atoi(yytext); 
								return TT_INTEGER;
							}

[-()*+,/:<=>\[\]\\]         {
								defslval.ch=yytext[0];
								if(EXPRESS_DEFS_PARSER_DEBUG) printf("%c",yytext[0]);
								return yytext[0];
							}


(_|{ALFA}|\$)+				   {
                               defslval.st=copy(yytext);
							   if(EXPRESS_DEFS_PARSER_DEBUG) printf("%s\n",yytext);
                               return	TT_IDENTIFIER;
                           }


(_|{ALFA}|\$)+[.]{DIGIT}*	   {//recognize the "var.17" and transform it into "var_17"
							   defslval.st=copy_and_replace_underscore(yytext);
							   if(EXPRESS_DEFS_PARSER_DEBUG) printf("%s\n",yytext);
                               return	TT_IDENTIFIER;
                           }


\"([^"]|\"\"|\\\")*\"		   {
								defslval.st=strip_quotes_and_copy(yytext, yyleng);
								return	TT_CHARSTRING;
						   }

\;.*						{/*nothing*/}
[ \t\b]+					{ /* gobble whitespace */ }
\n							{incrementLineNumber();}
.							{ syntaxError("unexpected character in source program"); }

%%


