/********************************************/
/* file: express.lex						*/
/* created: Wed Jun 30 1999					*/
/* authors: Ashok Halambi					*/
/* last modified:							*/
/* contents: flex scanner for EXPRESS		*/
/********************************************/

%{
//#include "stdafx.h"

#include "expressParser.h"
#include <ctype.h>
#include <string.h>
#ifdef __UNIX__
	#include <iostream.h>
#else
	#include <io.h>
	#define isatty _isatty
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

#define yywrap() 1

%}

DIGIT	[0-9]
LETTER	[A-Za-z]
ALFA	{LETTER}|{DIGIT}

%%

GLOBAL	 		return TT_GLOBAL;
PROC_BEGIN		return TT_PROCBEGIN;

LOCATION		return TT_LOCATION;
LABEL			return TT_LABEL;
PSEUDOS	 		return TT_PSEUDOS;
parm			return TT_parm;

BEGIN_SCOPE		return TT_BEGINSCOPE;
END_SCOPE		return TT_ENDSCOPE;

PROLOGUE		{
					if (EXPRESS_PARSER_DEBUG)
					{
						printf(" PROLOGUE ");
						fflush(stdout);
					}
					return TT_PROLOGUE;
				}
EPILOGUE		return TT_EPILOGUE;
END				{
					if (EXPRESS_PARSER_DEBUG)
					{
						printf(" END ");
						fflush(stdout);
					}
					return TT_END;
				}

NAME			return TT_NAME;
BASETYPENAME	return TT_BASETYPENAME;
VALUE			return TT_VALUE;

\$				return TT_DOLLAR;


-?{DIGIT}+[.]{DIGIT}+e[-+]?{DIGIT}+	{
								yylval.st = copy(yytext);
								return TT_FLOAT;
							}

-?{DIGIT}+e[-+]?{DIGIT}+	{
								yylval.st = copy(yytext);
								return TT_FLOAT;
							}

-?{DIGIT}+[.]{DIGIT}+		{
								yylval.st = copy(yytext);
								return TT_FLOAT;
							}

-?0x[0-9A-Fa-f]+			{ 
								yylval.in = strtol(yytext,NULL,0); 
								return TT_INTEGER;
							}

-?{DIGIT}+					{ 
								yylval.in = atoi(yytext); 
								return TT_INTEGER;
							}

(_|{ALFA})+					{
                               yylval.st=copy(yytext);
							   if(EXPRESS_PARSER_DEBUG) printf("ident: %s\n",yytext);
                               return	TT_IDENTIFIER;
                           }


(_|{ALFA})+[+-]{DIGIT}+		{//for label+expression
                               yylval.st=copy(yytext);
							   if(EXPRESS_PARSER_DEBUG) printf("ident: %s\n",yytext);
                               return	TT_IDENTIFIER;
                           }

(_|{ALFA})+[.]{DIGIT}+	   {//recognize the "var.17" and transform it into "var_17"
                               yylval.st=copy_and_replace_underscore(yytext);
							   if(EXPRESS_PARSER_DEBUG) printf("ident: %s\n",yytext);
                               return	TT_IDENTIFIER;
                           }


(_|{ALFA})+[.]{DIGIT}+[+-]{DIGIT}+	   {//recognize the "var.17" and transform it into "var_17"
                               yylval.st=copy_and_replace_underscore(yytext);
							   if(EXPRESS_PARSER_DEBUG) printf("ident: %s\n",yytext);
                               return	TT_IDENTIFIER;
                           }


\"([^"]|\"\")*\"		   {
								yylval.st=strip_quotes_and_copy(yytext, yyleng);
								return	TT_CHARSTRING;
						   }


[-()*+,/:<=>\[\]\\]         {
								yylval.ch=yytext[0];
								if(EXPRESS_PARSER_DEBUG) printf("%c",yytext[0]);
								return yytext[0];
							}



\;.*						{/*nothing*/}
[ \t\b]+					{ /* gobble whitespace */ }
\n							{incrementLineNumber();}
.							{ syntaxError("unexpected character in source program"); }

%%


