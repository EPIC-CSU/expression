
%{
//	  #include "stdafx.h"
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

	  #include "OptParse.h"
	  

	  //START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
      #ifdef WIN32
      #ifdef _DEBUG
      #define new DEBUG_NEW
      #undef THIS_FILE
      static char THIS_FILE[] = __FILE__;
      #endif
      #endif
      //END MFC DEBUG NEW: 
      	  
	  int OptcurrentOp, OpttempOp;

	  // declarations of functions used in this file
	  extern int getOpcodeIndex(char *opcode);
	  extern ArrayOfIntType &getRuleNumList(int currOp);
	  extern int existsCurrentOp(int currOp);

	extern void syntaxError(char * msg);
	extern int Optlex();
	extern int Optparse();
	#define Opterror syntaxError
	ISelOpListType *Opttemp;

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


%token OPT_GENERIC 
%token OPT_TARGET
%token OPT_LHS 
%token OPT_RHS 
%token OPT_DST 
%token OPT_SRC 
%token OPT_LPAREN 
%token OPT_RPAREN
%token OPT_LBRACE
%token OPT_RBRACE
%token OPT_COLON
%token OPT_EQUAL
%token OPT_ADD_OPERATOR
%token OPT_SUB_OPERATOR
%token OPT_EQ_OPERATOR
%token OPT_LT_OPERATOR
%token <number> OPT_NUM
%token <type> OPT_TYPE
%token <opc> OPT_OPCODE
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
		{
			resetLineNumber();
		}
      OPT_LBRACE ruleList OPT_RBRACE
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
      OPT_LBRACE genericPart 
	  {
	      genericRules.push_back($2);

		  (ISelOpcToRuleNo[OptcurrentOp]).push_back(genericRules.size() - 1);
	  }
	  ;

targets:
      targetPart OPT_RBRACE
	  {
		  targetRules.push_back($1);
	  }
	  ;

lhs:
      OPT_LBRACE lhsPart 
	  {
	      lhsRules.push_back($2);

		  (OptOpcToRuleNo[OptcurrentOp]).push_back(lhsRules.size() - 1);
	  }
	  ;

rhs:
      rhsPart OPT_RBRACE
	  {
		  rhsRules.push_back($1);
	  }
	  ;

genericPart:
      OPT_LBRACE OPT_GENERIC OPT_LBRACE opGenList OPT_RBRACE OPT_RBRACE
	  {
           $$ = $4;
	  }	  
	  ;

targetPart:
      OPT_LBRACE OPT_TARGET OPT_LBRACE opTarList OPT_RBRACE OPT_RBRACE
	  {
	       $$ = $4;
	  }
	  ;

lhsPart:
      OPT_LBRACE OPT_LHS OPT_LBRACE opGenList OPT_RBRACE OPT_RBRACE
	  {
           $$ = $4;
	  }	  
	  ;

rhsPart:
      OPT_LBRACE OPT_RHS OPT_LBRACE opTarList OPT_RBRACE OPT_RBRACE
	  {
	       $$ = $4;
	  }
	  ;

opGenList:
      opGenList Op 
	  {
	      $$->push_back($2);

		  OptcurrentOp = OpttempOp;
		  //
		  // comment out this line if you want the first op in the rule to be the "root op"
	  }
	  | Op
	  {
		  OptcurrentOp = OpttempOp;

	      $$ = new ISelOpListType;
	      $$->push_back($1);
//		  Opttemp = $$;
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
      OPT_LBRACE OPT_OPCODE dstList srcList OPT_RBRACE
	  {
	      $$ = new ISelOp();
		  OpttempOp = getOpcodeIndex($2);
	      $$->addOpcode($2);
		  $$->setDstList($3);
		  $$->setSrcList($4);
	  }
	  ;


dstList:
      dstList dstNum OPT_EQUAL argType argIdent 
	  {
	      ArgIdent idnt($5.idType, $5.ident1, $5.ident2, $5.oprtr);
	      Oprnd opnd($4.regOrImm, $4.oprtr, $4.val);
	      ISelArg dstArg($2, opnd, idnt);
		  $$->push_back(dstArg);

	  }
	  | dstNum OPT_EQUAL argType argIdent
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
      srcList srcNum OPT_EQUAL argType argIdent
	  {
	      ArgIdent idnt($5.idType, $5.ident1, $5.ident2, $5.oprtr);
	      Oprnd opnd($4.regOrImm, $4.oprtr, $4.val);
	      ISelArg srcArg($2, opnd, idnt);
		  $$->push_back(srcArg);
	  }
	  | srcNum OPT_EQUAL argType argIdent
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
      OPT_DST OPT_LPAREN val OPT_RPAREN 
	  {
	       $$ = $3;
	  }
	  ;
srcNum:
      OPT_SRC OPT_LPAREN val OPT_RPAREN
	  {
	       $$ = $3;
	  }
	  ;
val:
      OPT_NUM
	  {
		   $$ = $1;
	  }
	  ;

argType:
      OPT_TYPE 
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
	  | OPT_TYPE immVal 
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
      OPT_LBRACE OPT_NUM OPT_RBRACE
	  {
		   $$.idType = _ID_SINGLE_;
		   $$.ident1 = $2;  
	  }
	  |
      OPT_LBRACE OPT_NUM operator OPT_NUM OPT_RBRACE
	  {
		   $$.idType = _ID_EXPRESSION_;  
		   $$.ident1 = $2;
		   $$.ident2 = $4;
		   $$.oprtr = $3;  
	  }
	  ;

operator:
      OPT_ADD_OPERATOR
	  {
		   $$ = _ADD_OPERATOR_;
	  }
	  |
      OPT_SUB_OPERATOR
	  {
		   $$ = _SUB_OPERATOR_;
	  }
	  |
      OPT_EQ_OPERATOR
	  {
		   $$ = _EQ_OPERATOR_;
	  }
	  |
      OPT_LT_OPERATOR
	  {
		   $$ = _LT_OPERATOR_;
	  }
	  ;

immVal:
      operator OPT_NUM
	  {
	       $$.oprtr = $1;
		   $$.val = $2;
	  }
	  ;


%%
