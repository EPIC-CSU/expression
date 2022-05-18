
digit		[0-9]
alpha		[a-zA-Z_]
alphanum	[a-zA-Z_0-9]

%{

//#include "stdafx.h"
#include <stdio.h>
#include <iostream.h>
  
  extern int Optlex();
  extern int Optparse();
    
#include "OptParse.h"

#include "OptParse.cpp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


    #define Optwrap() 1
	#ifdef WIN32
        #define isatty _isatty
    #endif


	
	// to get rid of isatty() invocation in win32
	#ifdef WIN32
	    #define YY_NEVER_INTERACTIVE 1
	#endif
	

	int OptLexDebug = 0;
%}

%%

GENERIC		{ 
                if( OptLexDebug )
				   cout << Opttext << endl;
				return OPT_GENERIC; 
			}
TARGET		{ 
                if( OptLexDebug )
				   cout << Opttext << endl;
                return OPT_TARGET; 
			}
LHS			{
                if( OptLexDebug )
				   cout << Opttext << endl;
				return OPT_LHS; 
			}
RHS			{
                if( OptLexDebug )
				   cout << Opttext << endl;
				return OPT_RHS; 
			}				
DST			{ 
                if( OptLexDebug )
				   cout << Opttext << endl;
                return OPT_DST; 
		    }
SRC			{
                if( OptLexDebug )
				   cout << Opttext << endl; 
                return OPT_SRC; 
			}
REG|IMM|LAB		{
                         if( OptLexDebug )
				              cout << Opttext << endl; 
                         Optlval.type = (char *) malloc(strlen(Opttext)+1);
                         strcpy(Optlval.type, Opttext);
			             return OPT_TYPE;
			    }

{alpha}{alphanum}*		{ 
                            if( OptLexDebug )
				                 cout << Opttext << endl;
                            Optlval.opc = (char *) malloc(strlen(Opttext)+1);
			                strcpy(Optlval.opc, Opttext);
			                return OPT_OPCODE;
			             }
-?{digit}+			{ 
                            if( OptLexDebug )
				                 cout << Opttext << endl;
                            Optlval.number = atoi(Opttext);
			                return OPT_NUM;
			        }

"+"			{
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_ADD_OPERATOR; 

			}
"-"			{
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_SUB_OPERATOR; 

			}
"=="			{
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_EQ_OPERATOR; 

			}
"<"			{
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_LT_OPERATOR; 

			}

"("         {  
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_LBRACE;
            }
")"         {  
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_RBRACE;
            }
"="         {  
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_EQUAL;
            }
"\["         {  
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_LPAREN;
            }
"\]"         {  
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_RPAREN;
            }
":"         { 
               if( OptLexDebug )
				   cout << Opttext << endl; 
				return OPT_COLON; 
            }
\;.*	    {
                if( OptLexDebug )
				{
				    cout << "Identified comments .. " << endl;
					cout << Opttext << endl;
					cout << "end comments" << endl;
				}
			}

\;\*(\;|[^;*]|(\*)+[^;*])*(\*)+\;	{
                                          if( OptLexDebug )
										  {
										       cout << "Identified comments .. " << endl;
					                           cout << Opttext << endl;
					                           cout << "end comments" << endl;
										  }
									}
[\t ]       {						
                if( OptLexDebug )
				   cout << Opttext << endl;
            }

.|\n      {
				incrementLineNumber();
                if( OptLexDebug )
				   cout << Opttext << endl;
            }

%%