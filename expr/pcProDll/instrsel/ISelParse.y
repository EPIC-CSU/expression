/*************************************************************
 *file  : ISelParse.y										 *
 *created : 6 Oct, 2000.                                     *
 *authors : Partha Biswas									 *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

%{
	  #include "stdafx.h"
	  #include <iostream.h>
		#include <stdlib.h>

		#ifndef WIN32
			#include <alloca.h>
		#endif

		#include <malloc.h>

		#ifdef __UNIX__
		#include <strstream.h>
		#else
		#include <strstrea.h>
		#endif

	  #include "ISelParse.h"
	  

	  //START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
      #ifdef WIN32
      #ifdef _DEBUG
      #define new DEBUG_NEW
      #undef THIS_FILE
      static char THIS_FILE[] = __FILE__;
      #endif
      #endif
      //END MFC DEBUG NEW: 
      	  
	  int ISelcurrentOp, ISeltempOp;

	  // declarations of functions used in this file
	  extern int getOpcodeIndex(char *opcode);
	  extern ArrayOfIntType &getRuleNumList(int currOp);
	  extern int existsCurrentOp(int currOp);

	extern void syntaxError(char * msg);
	extern int ISellex();
	extern int ISelparse();
	#define ISelerror syntaxError
	ISelOpListType *ISeltemp;

%}

%union {
    int number;
	char *opc;
	char *type;
	opndType ot;
	identType it;
	ISelOp *iselop;
	ISelArgListType *iselarg;
	ISelOpListType *iseloplist;
};


%token ISEL_GENERIC 
%token ISEL_TARGET
%token ISEL_LHS 
%token ISEL_RHS 
%token ISEL_DST 
%token ISEL_SRC 
%token ISEL_LPAREN 
%token ISEL_RPAREN
%token ISEL_LBRACE
%token ISEL_RBRACE
%token ISEL_COLON
%token ISEL_EQUAL
%token ISEL_ADD_OPERATOR
%token ISEL_SUB_OPERATOR
%token ISEL_EQ_OPERATOR
%token ISEL_LT_OPERATOR
%token ISEL_TREE_MAPPING
%token ISEL_END_TREE_MAPPING
%token <number> ISEL_NUM
%token <type> ISEL_TYPE
%token <opc> ISEL_OPCODE
%type <number> dstNum srcNum val
%type <ot> argType
%type <ot> immVal
%type <it> argIdent
%type <iselop> Op
%type <iselarg> srcList dstList
%type <iseloplist> opGenList 
%type <iseloplist> genericPart 
%type <iseloplist> opTarList 
%type <iseloplist> targetPart
%type <iseloplist> lhsPart
%type <iseloplist> rhsPart
%type <number> operator

%start description

%%
description:
      ISEL_LBRACE ISEL_TREE_MAPPING ruleList ISEL_END_TREE_MAPPING ISEL_RBRACE
	  {
	       printf("\tFinished Parsing the ISel input file...\n");
	  }
	  ;

ruleList:
      ruleList rule 
	  {
	  }
	  | rule
	  {
	  }
	  ;

rule:
      generics targets
	  {
	  }
	  |
	  lhs rhs
	  {
	  }
	  ;

generics:
      ISEL_LBRACE genericPart 
	  {
	      genericRules.push_back($2);

		  (ISelOpcToRuleNo[ISelcurrentOp]).push_back(genericRules.size() - 1);
	  }
	  ;

targets:
      targetPart ISEL_RBRACE
	  {
		  targetRules.push_back($1);
	  }
	  ;

lhs:
      ISEL_LBRACE lhsPart 
	  {
	      lhsRules.push_back($2);

		  (OptOpcToRuleNo[ISelcurrentOp]).push_back(lhsRules.size() - 1);
	  }
	  ;

rhs:
      rhsPart ISEL_RBRACE
	  {
		  rhsRules.push_back($1);
	  }
	  ;

genericPart:
      ISEL_LBRACE ISEL_GENERIC ISEL_LBRACE opGenList ISEL_RBRACE ISEL_RBRACE
	  {
           $$ = $4;
	  }	  
	  ;

targetPart:
      ISEL_LBRACE ISEL_TARGET ISEL_LBRACE opTarList ISEL_RBRACE ISEL_RBRACE
	  {
	       $$ = $4;
	  }
	  ;

lhsPart:
      ISEL_LBRACE ISEL_LHS ISEL_LBRACE opGenList ISEL_RBRACE ISEL_RBRACE
	  {
           $$ = $4;
	  }	  
	  ;

rhsPart:
      ISEL_LBRACE ISEL_RHS ISEL_LBRACE opTarList ISEL_RBRACE ISEL_RBRACE
	  {
	       $$ = $4;
	  }
	  ;

opGenList:
      opGenList Op 
	  {
	      $$->push_back($2);

		  // ISelcurrentOp = ISeltempOp;
		  //
		  // comment out this line if you want the first op in the rule to be the "root op"
	  }
	  | Op
	  {
		  ISelcurrentOp = ISeltempOp;

	      $$ = new ISelOpListType;
	      $$->push_back($1);
//		  ISeltemp = $$;
	  }
	  ;

opTarList:
      opTarList Op 
	  {
	      $$->push_back($2);
	  }
	  | Op
	  {
	      $$ = new ISelOpListType;
	      $$->push_back($1);
	  }
	  ;


Op:
      ISEL_LBRACE ISEL_OPCODE dstList srcList ISEL_RBRACE
	  {
	      $$ = new ISelOp();
		  ISeltempOp = getOpcodeIndex($2);
	      $$->addOpcode($2);
		  $$->setDstList($3);
		  $$->setSrcList($4);
	  }
	  ;


dstList:
      dstList dstNum ISEL_EQUAL argType argIdent 
	  {
	      ArgIdent idnt($5.idType, $5.ident1, $5.ident2, $5.oprtr);
	      Oprnd opnd($4.regOrImm, $4.oprtr, $4.val);
	      ISelArg dstArg($2, opnd, idnt);
		  $$->push_back(dstArg);

	  }
	  | dstNum ISEL_EQUAL argType argIdent
	  {
	      ArgIdent idnt($4.idType, $4.ident1, $4.ident2, $4.oprtr);
	      Oprnd opnd($3.regOrImm, $3.oprtr, $3.val);
	      $$ = new ISelArgListType;
	      ISelArg dstArg($1, opnd, idnt);
		  $$->push_back(dstArg);
	  }
	  |
	  {
	      // nothing
		  $$ = NULL;
	  }
	  ;

srcList:
      srcList srcNum ISEL_EQUAL argType argIdent
	  {
	      ArgIdent idnt($5.idType, $5.ident1, $5.ident2, $5.oprtr);
	      Oprnd opnd($4.regOrImm, $4.oprtr, $4.val);
	      ISelArg srcArg($2, opnd, idnt);
		  $$->push_back(srcArg);
	  }
	  | srcNum ISEL_EQUAL argType argIdent
	  {
	      ArgIdent idnt($4.idType, $4.ident1, $4.ident2, $4.oprtr);
	      Oprnd opnd($3.regOrImm, $3.oprtr, $3.val);
	      $$ = new ISelArgListType;
	      ISelArg srcArg($1, opnd, idnt);
		  $$->push_back(srcArg);
	  }
	  |
	  {
	      // nothing
		  $$ = NULL;
	  }
	  ;

dstNum:
      ISEL_DST ISEL_LPAREN val ISEL_RPAREN 
	  {
	       $$ = $3;
	  }
	  ;
srcNum:
      ISEL_SRC ISEL_LPAREN val ISEL_RPAREN
	  {
	       $$ = $3;
	  }
	  ;
val:
      ISEL_NUM
	  {
		   $$ = $1;
	  }
	  ;

argType:
      ISEL_TYPE 
      {
	       if( strcmp($1, "REG") == 0 )
		   {
	           $$.regOrImm = _REG_;
		   }
		   else if( strcmp($1, "IMM") == 0 )
		   {
	           $$.regOrImm = _IMM_;
		   }
		   else if( strcmp($1, "LAB") == 0 )
		   {
	           $$.regOrImm = _LAB_;
		   }
		   $$.oprtr = _INVALID_;
		   $$.val = _INVALID_;
	  }	  
	  | ISEL_TYPE immVal 
	  {
	       if( strcmp($1, "REG") == 0 )
		   {
	           $$.regOrImm = _REG_;
		   }
		   else if( strcmp($1, "IMM") == 0 )
		   {
	           $$.regOrImm = _IMM_;
		   }
		   else if( strcmp($1, "LAB") == 0 )
		   {
	           $$.regOrImm = _LAB_;
		   }
		   $$.oprtr = $2.oprtr;
		   $$.val = $2.val;
	  }
	  ;

argIdent:
      ISEL_LBRACE ISEL_NUM ISEL_RBRACE
	  {
		   $$.idType = _ID_SINGLE_;
		   $$.ident1 = $2;  
	  }
	  |
      ISEL_LBRACE ISEL_NUM operator ISEL_NUM ISEL_RBRACE
	  {
		   $$.idType = _ID_EXPRESSION_;  
		   $$.ident1 = $2;
		   $$.ident2 = $4;
		   $$.oprtr = $3;  
	  }
	  ;

operator:
      ISEL_ADD_OPERATOR
	  {
		   $$ = _ADD_OPERATOR_;
	  }
	  |
      ISEL_SUB_OPERATOR
	  {
		   $$ = _SUB_OPERATOR_;
	  }
	  |
      ISEL_EQ_OPERATOR
	  {
		   $$ = _EQ_OPERATOR_;
	  }
	  |
      ISEL_LT_OPERATOR
	  {
		   $$ = _LT_OPERATOR_;
	  }
	  ;

immVal:
      operator ISEL_NUM
	  {
	       $$.oprtr = $1;
		   $$.val = $2;
	  }
	  ;


%%
