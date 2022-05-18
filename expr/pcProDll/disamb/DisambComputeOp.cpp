/*****************************************************************\
*                                                                *
*  Copyright (C) Regents of University of California, 2003       *
*  This source code is a part of EXPRESSION project and is       *
*  copyrighted by the Regents of the University of California,   *
*  Irvine.                                                       *
*  The authors hereby grant permission to use this description   *
*  and its documentation for educational and non-commercial      *
*  purpose under the BSD license                                 *
*  (http://www.cecs.uci.edu/~express/BSD_License.txt). 	         *
*  The source code comes with no warranty and no author or       *
*  distributor accepts any responsibility for the consequences   *
*  of its use. Permission is granted to distribute this file in  *
*  compiled or executable form under the same conditions that    *
*  apply for source code. Permission is granted	to anyone to     *
*  make or distribute copies of this source code, either as      *
*  received or modified, in any medium, provided that all        *
*  copyright notices, permission and non warranty notices are    *
*  preserved, and that the distributor grants the recipient      *
*  permission for further redistribution as permitted by this    *
*  document. No written agreement, license, or royalty fee is    *
*  required for authorized use of this software.                 *
*                                                                *
*******************************************************************/
#include "stdafx.h"

#include "GlobalIncludes.h"

#include "ComputeOp.h"
#include "OpCodeTable.h"
#include "disamb.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

expr ComputeOp::build_subexpr(char*& s)
{
 BaseArgument *arg;
 expr r=null;

 //remove wrapper ( ) if any
 if ((*s=='(') && (s[strlen(s)]==')'))
   {
     s[strlen(s)]='\0';
     s++;
   }

 while (*s!='\0')
 {
  switch (*s)
  {
       case '$':
            switch (*(s+1))
            {
             case '1':
                  arg=ptrToOperand(_SOURCE_1_);
                  r=arg->BuildExpr(this);
                  break;

             case '2':
                  arg=ptrToOperand(_SOURCE_2_);
				  if(arg==NULL){
					  cout <<"Semantic of opcode is not correct" <<endl;
					  abort();
				  }
                  r=arg->BuildExpr(this);
                  break;

             default:
                  error("Illegal operand number in ComputeOp::build_subexpr()");
            }
            s+=2;
            break;

       case '(':
            {
             r=build_expr(s);
            }
            break;

       default:
            error("Illegal operand number in ComputeOp::build_subexpr()");
            break;
  }
 }
 return r;
}



expr ComputeOp::build_expr(char*& p)
{
    char *oldp;
    expr l=null,e1,e2;
    int level=0;
    BaseArgument *arg;

    oldp=p;
    while(*p!='\0')
    {
      switch (*p) //look for first level-zero operator
      {
       case '(':
            level++;
            break;

       case ')':
            level--;
            break;

       case '$':
            switch (*(p+1))
            {
             case '1':
                  arg=ptrToOperand(_SOURCE_1_);
                  l=arg->BuildExpr(this);
                  break;

             case '2':
                  arg=ptrToOperand(_SOURCE_2_);
                  l=arg->BuildExpr(this);
                  break;

             default:
                  error("Illegal operand number in ComputeOp::build_subexpr()");
            }
			p++;
			break;

       case '+':
            if (level==0)
            {
				*p++='\0';
				l=new plus_expr(null);
				e1=build_subexpr(oldp);
				e2=build_subexpr(p);
				if(e1==NULL || e2==NULL) return NULL;
				PlusE(l)->add_operand(e1);
				PlusE(l)->add_operand(e2);
            }
            break;

       case '-':
            if (level==0)
            {
				*p++='\0';
				l=new plus_expr(null);
				e1=build_subexpr(oldp);
				e2=build_subexpr(p);
				if(e1==NULL || e2==NULL) return NULL;
				PlusE(l)->add_operand(e1);
				PlusE(l)->add_operand(e2);
            }
            break;

       case '*':
            if (level==0)
            {
				*p++='\0';
				l=new mult_expr(null);
				e1=build_subexpr(oldp);
				e2=build_subexpr(p);
				if(e1==NULL || e2==NULL) return NULL;
				MultE(l)->add_operand(e1);
				MultE(l)->add_operand(e2);
            }
            break;

       case '/':
		   return NULL;
            //if (level==0)
            //{
            // *p++='\0';
            // l=new plus_expr(null);
            // DivE(l)->add_operand(build_subexpr(oldp));
            // DivE(l)->add_operand(build_subexpr(p));
            //}
            break;

       case '<': // means <<
		   return NULL;
            //if (level==0)
            //{
            // *p++='\0';
            // p++;
            // l=new plus_expr(null);
            // PlusE(l)->add_operand(build_expr(oldp));
            // PlusE(l)->add_operand(build_expr(p));
            //}
            break;

       default:
            //ignore anything else
            error("Invalid operator in ComputeOp::BuildExpr");
			return NULL;
            break;
      }
	  if(*p=='\0') break;
      p++;
    }

  return l;
}


expr ComputeOp::BuildExpr()
{
  char *opsem;
  //char *s=(char*)malloc(400);
  char s[400];
  char *ps=s;
  expr r=null;

  opsem=globalOpCodeTable[getOpCodeIndex()].opSemantic();
  //char *s = new char[10];
  //for (int i = 0; i < 10; i++){
  //s[i] = '\0';
  //}
  if(opsem!=NULL)
  {
    strcpy(s,opsem);
    r=build_expr(ps);
  }

  return r;
}
