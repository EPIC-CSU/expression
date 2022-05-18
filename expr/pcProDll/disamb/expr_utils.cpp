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

#include <iostream.h>

#ifndef WIN32
#include <ctype.h>
#endif

#include "gcd.h"
#include "expr_utils.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#define MAXST 400

static union
{
 char st[MAXST];
 int in;
}
 yylval;

static char tok;
static int NumOrExpressions=0;

void simplify(expr &e){
  expr temp;
  int simplified_something;
  do{
    simplified_something=0; 
	temp=e;
    e=e->simplify(simplified_something);
//    cout << endl << "***Simplified:" << endl; 
//    e->print();
  }while(simplified_something); 
}


int gcd(int a,int b)
{
 if (a<0) a=-a;
 if (b<0) b=-b;
 if (a==0 || b==0) return 0;
 if (a==1 || b==1) return 1;
 if (a==b) return a;
 if (a>b) 
  return gcd(a-b,b);
 else
  return gcd(b-a,a);
}


int get_token(char *&s)
{
 int i;
 
 while (*s==' ' || 
        *s=='\t' || 
        *s=='\n')
 {
  s++;
 }

 if (isdigit(*s))
 {
	 //constant
	 yylval.in=0;
	 while(isdigit(*s))
  {
   yylval.in=yylval.in*10+*(s++)-'0'; 
  }
	 return Const;
 }
 else if (isalpha(*s) || *s=='$')
 {
	 //ident
	 i=0;
	 while(*s!=' ' && 
        *s!='\t' && 
        *s!='\n' && 
        *s!=')' && 
        *s!='(' && 
        *s!='+' && 
        *s!='-' && 
        *s!='+' && 
        *s!='*' &&
        i<MAXST-1)
  {
   yylval.st[i++]=*s++;
  }
	 if (i>=MAXST-1)
  {
   error("Identifier too long in expr_utils.cc"); 
  }
	 yylval.st[i]='\0';
	 return Ident;
 }
 else {
  return *s++;
 }
}



char* get_subexpr(char** s)
{
 char* r=new char[100];

 return r;
}



expr expression(char** s)
{
 expr l=NULL;
 expr t=NULL;
 expr r=NULL;
 expr re=NULL;
/*
 char* ss;
 switch(tok)
 {
        case '(':
             //subexpr == ( ... )
             ss=get_subexpr(s);
             l=expression(&ss);
             delete[] ss;
             break; 

        case '+':
             r=expression(s);
             t=new plus_expr(null);
             PlusE(t)->add_operand(l);
             PlusE(t)->add_operand(r);
             l=t;
             break;
              
        case '*':
             r=expression(s);
             t=new mult_expr(null);
             PlusE(t)->add_operand(l);
             PlusE(t)->add_operand(r);


             tok=get_token(s); 
             while(tok!=')')
             {
              MultE(re)->add_operand(expression(s));
             }
             break;
        
        case '-':
             tok=get_token(s);
             //re=new un_minus_expr(expression(s));
             re=multiply_with_minus_one(expression(s)); 
             break;

        //case '-':
        //     tok=get_token(s);
        //     re=new minus_expr(expression(s), expression(s));
        //     break;
        
        case '/':
             tok=get_token(s);
             re=new div_expr(expression(s), expression(s));
             break;
        
        case '^':
             tok=get_token(s);
             re=new pow_expr(expression(s), expression(s)); 
             break;
        
        case Ident:
             re=new identifier_expr(yylval.st);
             tok=get_token(s); 
             break;
        
        case Const:
             re=new constant_expr(yylval.in); 
             tok=get_token(s); 
             break;
        
  case Ident:
       re=new identifier_expr(yylval.st);
       break;
  
  case Const:
       re=new constant_expr(yylval.in); 
       break;

  case '-':
       tok=get_token(s);
       if (tok!=Const) error("There should be a constant here");
       re=new constant_expr(-yylval.in); 
       break; 
  
  default:
       error("invalid token");
       break;
 }
 tok=get_token(s); 
*/
 return re;
} 



expr build_expr_tree(char *&s)
{
 expr e=NULL,re=NULL;
/*
 re=new or_expr(NULL);
 tok=get_token(s);
 e=expression(s);
 OrE(re)->add_operand(e);
 while (tok!=0)
 {
  while(tok=='|')
  {
   tok=get_token(s); 
	  e=expression(s);
	  OrE(re)->add_operand(e);
  }
 }
*/
 return re;
}


void remove(expr_list l)
{
 if (l->tail==NULL) error("l shouldn't be the last in list");
 l->head=l->tail->head;
 l->tail=l->tail->tail;
}

int equivalent_lists(expr_list &l1,expr_list &l2,expr_list &removed_list)
{
 //destroys the l1 and l2!!!!!!!!!!!!!!!!!!
 //removes the equivalent exprs from the 2 lists and returns 1 if there are 
 //only constants left in l1 and l2.
 remove_equivalents(l1,l2,removed_list);
 return ((l1==NULL || (l1->head->kind==Constant_expr && l1->tail==NULL)) &&
  (l2==NULL || (l2->head->kind==Constant_expr && l2->tail==NULL)));
}


void remove_equivalents(expr_list &l1,expr_list &l2,expr_list &removed_list)
{
 expr_list t1=NULL,t2=NULL,t1aux=NULL,t2aux=NULL;
 removed_list=NULL; 
 for(t1aux=t1=l1;t1;t1=t1->tail)
 {
  for(t2aux=t2=l2;t2;t2=t2->tail)
  {
   if (t1->head->equivalent(t2->head))
   {
    if (t1aux!=t1) 
     t1aux->tail=t1->tail;
    else 
    {
     l1=t1->tail;
     t1aux=l1;
    }
    if (t2aux!=t2) 
     t2aux->tail=t2->tail;
    else 
    {
     l2=t2->tail; 
     t2aux=l2; 
    }
    removed_list=new expr_pair(t1->head,removed_list); 
    break; 
   }
   else
   {
    t1aux=t1;
    t2aux=t2;
   }
  }
 } 
}


expr multiply_with_minus_one(expr e)
{
 return 
  new mult_expr(new expr_pair(new constant_expr(-1), new expr_pair(e,NULL)));
}



int find_constant_in_list(expr_list l)
{
 for(;l!=NULL;l=l->tail)
 {
  if (l->head->kind==Constant_expr) 
   return ConE(l->head)->value;
 }
 return 1;
}


int min4(int a, int b, int c, int d)
{
	if(a<=b && a<=c && a<=d) return a; 
	if(b<=a && b<=c && b<=d) return b;
	if(c<=a && c<=b && c<=d) return c;
	return d;
}

int max4(int a, int b, int c, int d)
{
	if(a>=b && a>=c && a>=d) return a;
	if(b>=a && b>=c && b>=d) return b;
	if(c>=a && c>=b && c>=d) return c;
	return d;
}


int my_add(int a,int b)
{
 if (a==PLUSINFINITY || b==PLUSINFINITY) return PLUSINFINITY;
 if (a==MINUSINFINITY || b==MINUSINFINITY) return MINUSINFINITY;
 return a+b;
}

int my_mult(int a,int b)
{
 if (a==PLUSINFINITY && b>0 || a==MINUSINFINITY && b<0) return PLUSINFINITY;
 if (a==PLUSINFINITY && b<0 || a==MINUSINFINITY && b>0) return MINUSINFINITY;
 if (b==PLUSINFINITY && a>0 || b==MINUSINFINITY && a<0) return PLUSINFINITY;
 if (b==PLUSINFINITY && a<0 || b==MINUSINFINITY && a>0) return MINUSINFINITY;
 return a*b;
}

int my_div(int a,int b)
{
 if (a==PLUSINFINITY && b>0 || a==MINUSINFINITY && b<0) return PLUSINFINITY;
 if (a==PLUSINFINITY && b<0 || a==MINUSINFINITY && b>0) return MINUSINFINITY;
 if (b==PLUSINFINITY && a>0 || b==MINUSINFINITY && a<0) return 0;
 if (b==PLUSINFINITY && a<0 || b==MINUSINFINITY && a>0) return 0;
 return a/b;
}


void delete_everything(void)
{
 expr_list l=NULL,aux=NULL;
 list_list * l1=NULL,*aux1=NULL; 
 for(l=everything;l!=NULL;)
 {
	 aux=l->tail; 
	 delete l->head;
	 l=aux; 
 }
 for(l1=all_pairs;l1!=NULL;)
 {
	 aux1=l1->tail;
	 delete l1->head;
	 delete l1; 
	 l1=aux1; 
 }
}

void _parse_expression_list(string* result,string head,string tail)
{
 //try to find a |
 int i=0;
 while (tail[i]!='|' && 
        tail[i]!='\0')
 {
  i++;
 }

 if (tail[i]=='\0') //no ORs, easy case
 { 
  //concatenate head and tail and return that as one of the OR exprs
  NumOrExpressions++;
  string t1=concat(head, tail);
  string t2=*result;
  *result=concat(*result,t1);;
  free(t2);
  free(t1);
  return;
 }
 else //ORs, needs further processing
 {
  //new head
  string newhead=(string)malloc(strlen(head)+i);
  strcpy(newhead,head);
  strncpy(&newhead[strlen(head)],tail,i-1);
  newhead[strlen(head)+i-1]='\0';

  //identify the OR part and the new tail
  int j=i+1; //start scanning past the '|'
  int level=0;
  while ((tail[j]!='\0') && level>=0) //match ( and )
  {
   if (tail[j]=='(') level++;
   if (tail[j]==')') level--;
   j++;
  }

  //the OR part
  string or=(string)malloc(j-i-1);
  strncpy(or,&tail[i+1],j-i-2);
  or[j-i-2]='\0';

  //part tail
  string parttail=(string)malloc(strlen(tail)-j+1);
  strcpy(parttail,&tail[j+1]);

  i=0;
  //now repeat for each subexpression in or
  while (or[i]!='\0')
  {
   while (or[i]==' ') i++; //skip blanks until '('
   if (or[i]!='\0')
   {
    j=1;
    while (or[i]!=')' && or[i]!='\0')
    {
     i++;
     j++;
    }
    j++;

    //found a subexpression
    string newtail=(string)malloc(strlen(parttail)+j+1);
    strncpy(newtail,&or[i-j+1],j);
    newtail[j]='\0';
    strcat(newtail,parttail);
    _parse_expression_list(result,newhead,newtail);  
    free(newtail);
   }
   if (or[i]!='\0') i++;
  }

  free(newhead);
  free(or);
  free(parttail);
 }
}


// receives something like:
//
//    (+ (* a 2) ( | (* b 3) (* b 5) )
//
// generates something like:
//
//    ( | (+ (* a 2) (* b 3) ) | ( + (* a 2) (* b 5) ) )
//
// function is just a wrapper around _parse_expression_list()
// user must explicitly deallocate the return string (or does he/she?)

void parse_expression_list(string* result,string head,string tail)
{
  NumOrExpressions=0;
  _parse_expression_list(result,head,tail);
  if (NumOrExpressions>1)
  {
   string t1;

   // the \n is for debugging formatting only, to be removed in final version
   t1=concat("( | \n", *result);
   free(*result);
   *result=concat(t1," )");
   free(t1);
  }
}
