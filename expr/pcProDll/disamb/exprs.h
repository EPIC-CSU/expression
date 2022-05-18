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
#ifndef _EXPRSH_
#define _EXPRSH_ 1
#define MINUSINFINITY -1000000
#define PLUSINFINITY  1000000

#ifndef WIN32
#include <malloc.h>
#endif

#include <stdlib.h>

#include "std_types.h"
#include "disamb.h"
#include "intMatrix.h"

typedef struct expr_block *expr;
typedef struct expr_pair * expr_list;

// *** LIST_LIST

struct list_list
{
 expr_list head;
 list_list *tail;
 list_list(expr_list h, list_list *t) :
 head(h), tail(t)
 {
 }
};
extern list_list * all_pairs; 
// *** EXPR_LIST ***

struct expr_pair
{
 expr head;
 expr_pair *tail;
 expr_pair(expr h, expr_pair *t);
};

extern expr_list everything; 

// *** interval ******
struct interval
{
 int lower,upper;
 interval(int i,int j):lower(i),upper(j){}
};

// *** EXPR_BLOCK, et al **

typedef enum
{
 Identifier_expr, Constant_expr, Plus_expr, Mult_expr, Div_expr, Pow_expr, 
  Un_minus_expr, Or_expr
} expr_kind;

struct expr_block 
{
 expr_kind kind;
 expr_block(expr_kind k);

 virtual ~expr_block(void){} 
 virtual void print(void){}
 virtual disambType roots(void);
 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual expr unify(expr e);
 virtual int equivalent(expr e){return 0;}
 virtual int differ_by_constant_multiplier(expr e,expr &factorized_expr);
 void setExpr_block(int &i); 
 virtual interval * get_interval(void);
 virtual int count_vars(void){error("Too complicated expr");return 0;}
 virtual void build_matrix(intMatrix *coord,intMatrix *constr){error("Shouldn't be called");}
 virtual void build_matrix(intMatrix *coord,intMatrix *constr,int i){error("Shouldn't be called");}
 virtual int too_complicated(void){ return 1;}
};

struct identifier_expr : expr_block 
{
 char * ident;
 
 identifier_expr(char *id) :
 expr_block(Identifier_expr)
 {
  ident=(char*)malloc(strlen(id)+1);
  strcpy(ident,id);
 }
 virtual ~identifier_expr(void){free(ident);} 
 
 virtual void print(void)
 {
	 printf(" %s ",ident);
 }

 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual disambType roots(void);
 virtual int equivalent(expr e); 
 virtual interval *get_interval(void);
 virtual int count_vars(void);
 virtual void build_matrix(intMatrix *coord,intMatrix *constr,int i);
 virtual int too_complicated(void);
};

struct constant_expr : expr_block {
 integer value;
 
 constant_expr(int val) :
 expr_block(Constant_expr), value(val)
 {
 }

 virtual ~constant_expr(void){} 
 virtual void print(void)
 {
  printf(" %d ",value);
 }

 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual disambType roots(void);
 virtual int equivalent(expr e); 
 virtual interval *get_interval(void);
 virtual int count_vars(void);
 virtual void build_matrix(intMatrix *coord,intMatrix *constr,int i);
 virtual int too_complicated(void);
};

struct plus_expr : expr_block 
{
 expr_list operands;
 
 plus_expr(expr_list t):expr_block(Plus_expr),operands(t){}
 virtual ~plus_expr(void); 
 void add_operand(expr e);
 void add_operands(expr_list l);
 virtual void print(void)
 {
  expr_list l=operands;
  printf(" (+");
  for(;l!=NULL;l=l->tail)
   l->head->print();
  printf(") ");
 }
 virtual disambType roots(void);
 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual expr unify(expr e);
 virtual int equivalent(expr e); 
 virtual interval * get_interval(void);
 virtual int count_vars(void);
 virtual void build_matrix(intMatrix *coord,intMatrix *constr);
 virtual int too_complicated(void);
};

struct mult_expr : expr_block 
{
 expr_list operands;
 
 mult_expr(expr_list t):expr_block(Mult_expr),operands(t){}
 virtual ~mult_expr(void);
 void add_operand(expr e);
 void add_operands(expr_list l);
 virtual void print(void)
 {
  expr_list l=operands;
  printf(" (*");
  for(;l!=NULL;l=l->tail)
   l->head->print();
  printf(") ");
 }
 virtual disambType roots(void);
 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual int differ_by_constant_multiplier(expr e,expr &factorized_expr);
 virtual int equivalent(expr e); 
 //defactorization:
 expr defact(expr_list &l,int & factorized_something); 
 expr defactorize(int & simplified_something); 
 virtual interval *get_interval(void);
 virtual int count_vars(void);
 virtual void build_matrix(intMatrix *coord,intMatrix *constr,int i);
 virtual int too_complicated(void);
};


struct or_expr : expr_block {
 expr_list operands;
 
 or_expr(expr_list t):expr_block(Or_expr),operands(t){}
 virtual ~or_expr(void);
 void add_operand(expr e);
 void add_operands(expr_list l);
 virtual void print(void)
 {
  expr_list l=operands;
  printf(" (|");
  for(;l!=NULL;l=l->tail)
   l->head->print();
  printf(") \n");
 }
 virtual disambType roots(void);
 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual int equivalent(expr e); 
 virtual interval *get_interval(void);
};

struct div_expr : expr_block {
 expr left, right;
 
 div_expr(expr l, expr r):expr_block(Div_expr),left(l), right(r){}
 virtual ~div_expr(void);
 virtual void print(void)
 {
  printf(" (/");
  left->print();
  right->print();
  printf(") ");
 }
 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual disambType roots(void);
 virtual int equivalent(expr e); 
 virtual interval *get_interval(void);
};


struct pow_expr : expr_block {
 expr left, right;
 
 pow_expr(expr l, expr r):expr_block(Pow_expr),left(l), right(r){}
 virtual ~pow_expr(void);
 virtual void print(void)
 {
  printf(" (^");
  left->print();
  right->print();
  printf(") ");
 }
 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual disambType roots(void);
 virtual int equivalent(expr e); 
 virtual interval *get_interval(void);
};

struct un_minus_expr : expr_block {
 expr operand;
 
 un_minus_expr(expr o): expr_block(Un_minus_expr),operand(o)
 {error("Un_minus shouldn't be used");}
 virtual ~un_minus_expr(void);
 virtual void print(void)
 {
  printf(" (-");
  operand->print();
  printf(") ");
 }
 virtual expr copy(void);
 virtual expr simplify(int &simplified_something);
 virtual disambType roots(void);
 virtual int equivalent(expr e); 
 virtual interval *get_interval(void);
};

#define PlusE(e) ((plus_expr*)(e))
#define MultE(e) ((mult_expr*)(e))
#define OrE(e) ((or_expr*)(e))
#define DivE(e) ((div_expr*)(e))
#define Un_minusE(e) ((un_minus_expr*)(e))
#define ConE(e) ((constant_expr*)(e))
#define IdE(e) ((identifier_expr*)(e))
#define PowE(e) ((pow_expr*)(e))

#endif
