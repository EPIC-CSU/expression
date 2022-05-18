/*************************************************************
 *file  : ISelParse.lex										 *
 *created : 6 Oct, 2001.                                     *
 *authors : Partha Biswas									 *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

digit		[0-9]
alpha		[a-zA-Z_]
alphanum	[a-zA-Z_0-9]

%{

#include "stdafx.h"
#include <stdio.h>
#include <iostream.h>
  
  extern int ISellex();
  extern int ISelparse();
    
#include "ISelParse.h"

#include "ISelParse.cpp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


    #define ISelwrap() 1
	#ifdef WIN32
        #define isatty _isatty
    #endif


	
	// to get rid of isatty() invocation in win32
	#ifdef WIN32
	    #define YY_NEVER_INTERACTIVE 1
	#endif
	

	int ISelLexDebug = 0;
%}

%%

GENERIC		{ 
                if( ISelLexDebug )
				   cout << ISeltext << endl;
				return ISEL_GENERIC; 
			}
TARGET		{ 
                if( ISelLexDebug )
				   cout << ISeltext << endl;
                return ISEL_TARGET; 
			}
LHS			{
                if( ISelLexDebug )
				   cout << ISeltext << endl;
				return ISEL_LHS; 
			}
RHS			{
                if( ISelLexDebug )
				   cout << ISeltext << endl;
				return ISEL_RHS; 
			}				
DST			{ 
                if( ISelLexDebug )
				   cout << ISeltext << endl;
                return ISEL_DST; 
		    }
SRC			{
                if( ISelLexDebug )
				   cout << ISeltext << endl; 
                return ISEL_SRC; 
			}
TREE_MAPPING			{
							if( ISelLexDebug )
								cout << ISeltext << endl; 
							return ISEL_TREE_MAPPING; 
						}

END_TREE_MAPPING			{
								if( ISelLexDebug )
									 cout << ISeltext << endl; 
								return ISEL_END_TREE_MAPPING; 
							}


REG|IMM|LAB		{
                         if( ISelLexDebug )
				              cout << ISeltext << endl; 
                         ISellval.type = (char *) malloc(strlen(ISeltext)+1);
                         strcpy(ISellval.type, ISeltext);
			             return ISEL_TYPE;
			    }

{alpha}{alphanum}*		{ 
                            if( ISelLexDebug )
				                 cout << ISeltext << endl;
                            ISellval.opc = (char *) malloc(strlen(ISeltext)+1);
			                strcpy(ISellval.opc, ISeltext);
			                return ISEL_OPCODE;
			             }
-?{digit}+			{ 
                            if( ISelLexDebug )
				                 cout << ISeltext << endl;
                            ISellval.number = atoi(ISeltext);
			                return ISEL_NUM;
			        }

"("         {  
               if( ISelLexDebug )
				   cout << ISeltext << endl; 
				return ISEL_LBRACE;
            }
")"         {  
               if( ISelLexDebug )
				   cout << ISeltext << endl; 
				return ISEL_RBRACE;
            }
"="         {  
               if( ISelLexDebug )
				   cout << ISeltext << endl; 
				return ISEL_EQUAL;
            }
"\["         {  
               if( ISelLexDebug )
				   cout << ISeltext << endl; 
				return ISEL_LPAREN;
            }
"\]"         {  
               if( ISelLexDebug )
				   cout << ISeltext << endl; 
				return ISEL_RPAREN;
            }
":"         { 
               if( ISelLexDebug )
				   cout << ISeltext << endl; 
				return ISEL_COLON; 
            }
\;.*	    {
                if( ISelLexDebug )
				{
				    cout << "Identified comments .. " << endl;
					cout << ISeltext << endl;
					cout << "end comments" << endl;
				}
			}

\;\*(\;|[^;*]|(\*)+[^;*])*(\*)+\;	{
                                          if( ISelLexDebug )
										  {
										       cout << "Identified comments .. " << endl;
					                           cout << ISeltext << endl;
					                           cout << "end comments" << endl;
										  }
									}
"+"			{
               if( ISelLexDebug )
				   cout << ISeltext << endl; 
				return ISEL_ADD_OPERATOR; 

			}
"-"			{
               if( ISelLexDebug )
				   cout << ISeltext << endl; 
				return ISEL_SUB_OPERATOR; 

			}
[\t ]       {						
                if( ISelLexDebug )
				   cout << ISeltext << endl;
            }

.|\n      {
                if( ISelLexDebug )
				   cout << ISeltext << endl;
            }

%%