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
#include <math.h>

#include "exprs.h"
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

expr_list everything=NULL;
list_list * all_pairs=NULL; 


expr_pair::expr_pair(expr h, expr_pair *t) : head(h), tail(t)
{
	all_pairs=new list_list(this,all_pairs); 
}

plus_expr::~plus_expr(void){
	/*
	expr_list l,temp;
	expr e;

    for(l=operands;l!=NULL;){
		e=l->head;
		temp=l;
		l=l->tail;
		delete e;
		delete temp;
	}
	*/
} 

mult_expr::~mult_expr(void){
	/*
	expr_list l,temp;
	expr e;
	
	for(l=operands;l!=NULL;){
		e=l->head;
		temp=l;
		l=l->tail;
		delete e;
		delete temp;
	}*/
}

or_expr::~or_expr(void){
	/*
	expr_list l,temp;
	expr e;
	
	for(l=operands;l!=NULL;){
		e=l->head;
		temp=l;
		l=l->tail;
		delete e;
		delete temp;
	}
	*/
}
div_expr::~div_expr(void){
	/*
	delete left;
	delete right;
	*/
}

pow_expr::~pow_expr(void){
	/*
	delete left;
	delete right;
	*/
}

un_minus_expr::~un_minus_expr(void){
	//delete operand;
}


void expr_block::setExpr_block(int &i)
{
	i=1;
}


expr_block::expr_block(expr_kind k): kind(k)
{
	everything=new expr_pair(this,everything); 
}


void plus_expr::add_operand(expr e)
{
	operands=new expr_pair(e,operands);
}


void mult_expr::add_operand(expr e)
{
	operands=new expr_pair(e,operands);
}


void or_expr::add_operand(expr e)
{
	operands=new expr_pair(e,operands);
}


void plus_expr::add_operands(expr_list l)
{
	if (operands==NULL) operands=l;
	else
	{
		expr_list p;
		for(p=operands;p->tail!=NULL;p=p->tail);
		p->tail=l;
	}
}


void mult_expr::add_operands(expr_list l)
{
	if (operands==NULL) operands=l;
	else
	{
		expr_list p;
		for(p=operands;p->tail!=NULL;p=p->tail);
		p->tail=l;
	}
}


void or_expr::add_operands(expr_list l)
{
	if (operands==NULL) operands=l;
	else
	{
		expr_list p;
		for(p=operands;p->tail!=NULL;p=p->tail);
		p->tail=l;
	}
}


expr plus_expr::unify(expr e)
{
	expr_list l;
	if (e->kind==Plus_expr)
	{
		for(l=PlusE(e)->operands;l!=NULL;l=l->tail)
			add_operand(multiply_with_minus_one(l->head));
	}
	else
	{
		add_operand(multiply_with_minus_one(e));
	}
	return this;
}

expr expr_block::unify(expr e)
{
	expr re=NULL;
	if (e->kind==Plus_expr)
	{
		PlusE(e)->add_operand(multiply_with_minus_one(this));
		return e;
	}
	else
	{
		re=new plus_expr(NULL);
		PlusE(re)->add_operand(this);
		PlusE(re)->add_operand(multiply_with_minus_one(e));
		return re;
	}
}

disambType expr_block::roots(void)
{
	error("expr_block::roots shouln't be called");
	return DONTKNOW;
}

disambType identifier_expr::roots(void)
{
	cout << "\nwhen" ,ident, "==0\n";
	return OVERLAPING;
}

disambType constant_expr::roots(void)
{
	if (value==0) return EQUAL;
	else return DISJOINT;
}

disambType plus_expr::roots(void)
{
	expr_list l=NULL;
	int i=0,val1,val2; 
	expr con=NULL,term1=NULL,term2=NULL,term2prime=NULL;
	term2prime=new plus_expr(NULL);
	//find the constant and var terms
	for(l=operands;l!=NULL;l=l->tail)
		if (l->head->kind==Constant_expr) con=l->head;
		else 
			if (i==0)//the first variable term
			{
				term1=l->head;
				i++; 
			}
			else//all the rest of variable terms
			{
				i++; 
				term2=l->head;
				PlusE(term2prime)->add_operand(l->head); 
			}
			if (con==NULL) con=new constant_expr(0); 
			switch(i)
			{
			case 0://zero var terms
				return con->roots(); 
			case 1://one variable term: the constants in term1 have to divide con
				if (term1->kind==Mult_expr)
				{
					val1=find_constant_in_list(MultE(term1)->operands);
					if (ConE(con)->value % val1==0) return OVERLAPING;
					else return DISJOINT; 
				}
				else if (term1->kind==Identifier_expr) return OVERLAPING;  
				else return DONTKNOW;
			case 2://2 var terms
				{
					if (term1->kind==Mult_expr)
						val1=find_constant_in_list(MultE(term1)->operands);
					else if (term1->kind==Identifier_expr) return OVERLAPING;  
					else return DONTKNOW; 
					if (term2->kind==Mult_expr)
						val2=find_constant_in_list(MultE(term2)->operands);
					else if (term2->kind==Identifier_expr) return OVERLAPING;  
					else return DONTKNOW; 
					if (ConE(con)->value % gcd(val1,val2)==0) 
						return OVERLAPING;
					else return DISJOINT; 
				}
			default://3 or more var terms
				{
					int v,first_time=1;
					if (term1->kind==Mult_expr)
						val1=find_constant_in_list(MultE(term1)->operands);
					else if (term1->kind==Identifier_expr) return OVERLAPING;  
					else return DONTKNOW;
					for(l=PlusE(term2prime)->operands;l!=NULL;l=l->tail)
						if (l->head->kind==Mult_expr)
						{
							v=find_constant_in_list(MultE(l->head)->operands); 
							if (first_time)
							{
								val2=v;
								first_time=0; 
							}
							else
								val2=gcd(val2,v);
						}
						else if (l->head->kind==Identifier_expr) return OVERLAPING;  
						else return DONTKNOW;
						if (ConE(con)->value % gcd(val1,val2)==0) 
							return OVERLAPING;
						else return DISJOINT; 
				}
			}
			
			
}

disambType mult_expr::roots(void)
{
	disambType a,ra=DISJOINT;
	expr_list l=NULL;
	for(l=operands;l!=NULL;l=l->tail)
	{
		a=l->head->roots();
		switch(a)
		{
		case DISJOINT:
			break; 
			
		case OVERLAPING:
			if (ra==DISJOINT) ra=OVERLAPING; //if it was DONTKNOW, remains DONTKNOW
			break; 
			
		case EQUAL:
			return EQUAL; 
			
		case DONTKNOW:
			if (ra==DISJOINT) ra=DONTKNOW;
			break; 
			
		default:
			error("Unknown disambType in mult_expr::roots"); 
		}
	}
	return ra; 
}

disambType or_expr::roots(void)
{
	error("or_expr::roots shouldn't be called"); 
	return DONTKNOW;
}

disambType div_expr::roots(void)
{
	return left->roots();
}

disambType un_minus_expr::roots(void)
{
	return DONTKNOW;
}


disambType pow_expr::roots(void)
{
	return left->roots();
}

expr expr_block::copy(void)
{
	return new expr_block(kind);
}

expr identifier_expr::copy(void)
{
	return new identifier_expr(ident);
}

expr constant_expr::copy(void)
{
	return new constant_expr(value);
}

expr plus_expr::copy(void)
{
	expr_list l=NULL;
	expr re=NULL;
	re=new plus_expr(NULL);
	for(l=operands;l!=NULL;l=l->tail)
		PlusE(re)->add_operand(l->head->copy());
	return re;
}

expr mult_expr::copy(void)
{
	expr_list l=NULL;
	expr re=NULL;
	re=new mult_expr(NULL);
	for(l=operands;l!=NULL;l=l->tail)
		MultE(re)->add_operand(l->head->copy());
	return re;
}

expr or_expr::copy(void)
{
	expr_list l=NULL;
	expr re=NULL;
	re=new or_expr(NULL);
	for(l=operands;l!=NULL;l=l->tail)
		OrE(re)->add_operand(l->head->copy());
	return re;
}

expr div_expr::copy(void)
{
	return new div_expr(left->copy(),right->copy());
}

expr pow_expr::copy(void)
{
	return new pow_expr(left->copy(),right->copy());
}

expr un_minus_expr::copy(void)
{
	return new un_minus_expr(operand->copy());
}


expr expr_block::simplify(int &simplified_something)
{
	return new expr_block(kind);
}

expr identifier_expr::simplify(int &simplified_something)
{
	return new identifier_expr(ident);
}

expr constant_expr::simplify(int &simplified_something)
{
	return new constant_expr(value);
}



expr plus_expr::simplify(int &simplified_something)
{
	int passed_through_if=0; 
	expr_list l=NULL,k=NULL;
	expr re1=NULL,re2=NULL,re3=NULL,factorized_expr=NULL;
	int val=0;
	//constr of re1 = simplify recursively
	re1=new plus_expr(NULL);
	for(l=operands;l!=NULL;l=l->tail){
		PlusE(re1)->add_operand(l->head->simplify(simplified_something));
		//delete l->head;
	}
	//constr of re2 = unify (+ ...(+ ...) ...)
	re2=new plus_expr(NULL);
	for(l=PlusE(re1)->operands;l!=NULL;l=l->tail)
	{
		if (l->head->kind==Plus_expr)
		{
			PlusE(re2)->add_operands(PlusE(l->head)->operands);
			setExpr_block(simplified_something); 
		}
		else
			PlusE(re2)->add_operand(l->head);
	}
	//constr of re3 = unify constant terms
	val=0; 
	re3=new plus_expr(NULL);
	for(l=PlusE(re2)->operands;l!=NULL;l=l->tail)
	{
		if (l->head->kind==Constant_expr)
			val+=ConE(l->head)->value;
		else
			PlusE(re3)->add_operand(l->head);
	}
	if (val!=0)
		PlusE(re3)->add_operand(new constant_expr(val));
	//modify re3 = factorization of terms which differ by a constant
	for(l=PlusE(re3)->operands;l!=NULL;)
	{
		for(k=l->tail;k!=NULL;k=k->tail)
			if (l->head->differ_by_constant_multiplier(k->head,factorized_expr))
			{
				//replace k->head with factorized form of l->head and k->head
				k->head=factorized_expr;
				//		  printf("\nThe factorized expr:\n");
				//		  factorized_expr->print(); 
				//remove l from list of terms
				remove(l);
				passed_through_if=1; 
				setExpr_block(simplified_something); 
				break; //goes to next l
			}
			if (passed_through_if) passed_through_if=0;
			else l=l->tail;
	}
	//if there is no term, return constant_expr(0)
	if (PlusE(re3)->operands==NULL) return new constant_expr(0); 
	//if there is only one term, return it without the addition
	if (PlusE(re3)->operands->tail==NULL) {
		setExpr_block(simplified_something); 
		return PlusE(re3)->operands->head; 
	}
	return re3;
}

expr mult_expr::simplify(int &simplified_something)
{
	expr_list l=NULL;
	expr re1=NULL,re2=NULL,re3=NULL;
	int val=1;
	//constr of re1 = simplify recursively
	re1=new mult_expr(NULL);
	for(l=operands;l!=NULL;l=l->tail){
		MultE(re1)->add_operand(l->head->simplify(simplified_something));
		//delete l->head;
	}
	//constr of re2 = unify (* ... (* ...) ...)
	re2=new mult_expr(NULL);
	for(l=MultE(re1)->operands;l!=NULL;l=l->tail)
	{
		if (l->head->kind==Mult_expr)
		{
			MultE(re2)->add_operands(MultE(l->head)->operands);
			setExpr_block(simplified_something); 
		}
		else
			MultE(re2)->add_operand(l->head);
	}
	//constr of re3 = unify the constants
	re3=new mult_expr(NULL);
	for(l=MultE(re2)->operands;l!=NULL;l=l->tail)
	{
		if (l->head->kind==Constant_expr)
			if (ConE(l->head)->value==0) return new constant_expr(0);
			else val*=ConE(l->head)->value;
			else
				MultE(re3)->add_operand(l->head);
	}
	if (val!=1)
		MultE(re3)->add_operand(new constant_expr(val));
	else setExpr_block(simplified_something);
	//if there is no factor, return constant_expr(0)
	if (MultE(re3)->operands==NULL) return new constant_expr(0); 
	//if there is only one factor, return it without the multiplication
	if (MultE(re3)->operands->tail==NULL) {
		setExpr_block(simplified_something); 
		return MultE(re3)->operands->head; 
	}
	re3=MultE(re3)->defactorize(simplified_something);
	return re3;
}



expr or_expr::simplify(int &simplified_something)
{
	error("Or_expr::simplify shouldn't be called");
	return NULL;
}

expr div_expr::simplify(int &simplified_something)
{
	expr re1=NULL;
	re1=new div_expr(left->simplify(simplified_something),right->simplify(simplified_something));
	if (DivE(re1)->left->kind==Constant_expr)
		if (ConE(DivE(re1)->left)->value==0) return new constant_expr(0);
		else if (DivE(re1)->right->kind==Constant_expr)
			if (ConE(DivE(re1)->right)->value==0) error("Division by 0");
			else return new constant_expr(ConE(DivE(re1)->left)->value/
				ConE(DivE(re1)->right)->value);
			else return re1;
			else if (DivE(re1)->right->kind==Constant_expr)
				if (ConE(DivE(re1)->right)->value==0) error("Division by 0");
				else return re1;
				else if (DivE(re1)->left->equivalent(DivE(re1)->right)) return new constant_expr(1);
				else return re1;
				
				//NS
				error ("Case not handled!");
				return NULL;
}

expr pow_expr::simplify(int &simplified_something)
{
	expr re1=NULL;
	re1=new pow_expr(left->simplify(simplified_something),right->simplify(simplified_something));
	if (PowE(re1)->left->kind==Constant_expr)
		if (ConE(PowE(re1)->left)->value==0) return new constant_expr(0);
		else if (ConE(PowE(re1)->left)->value==1) return new constant_expr(1);
		else if (PowE(re1)->right->kind==Constant_expr)
			if (ConE(PowE(re1)->right)->value==0) return new constant_expr(1);
			else return new constant_expr((int)pow(ConE(PowE(re1)->left)->value,
				ConE(PowE(re1)->right)->value));
			else return re1;
			else if (PowE(re1)->right->kind==Constant_expr)
				if (ConE(PowE(re1)->right)->value==0) return new constant_expr(1);
				else if (ConE(PowE(re1)->right)->value==1) PowE(re1)->left;
				else return re1; 
				else return re1;
				
				//NS
				error ("Case not handled!");
				return NULL;
}

expr un_minus_expr::simplify(int &simplified_something)
{
	expr re1=NULL;
	re1=new un_minus_expr(operand->simplify(simplified_something));
	if (Un_minusE(re1)->operand->kind==Constant_expr)
		return new constant_expr(-ConE(Un_minusE(re1)->operand)->value);
	else if (Un_minusE(re1)->operand->kind==Un_minus_expr)
		return Un_minusE(Un_minusE(re1)->operand)->operand; 
	else return re1;   
}


int expr_block::differ_by_constant_multiplier(expr e,expr &factorized_expr)
{
	expr e1=NULL,e2=NULL;
	factorized_expr=NULL;
	e1=new mult_expr(new expr_pair(new constant_expr(1),
		new expr_pair(copy(),NULL)));
	e2=e->copy();
	if (e1->differ_by_constant_multiplier(e2,factorized_expr)) return 1;
	return 0;
}


int mult_expr::differ_by_constant_multiplier(expr e,expr &factorized_expr)
{
	int val;
	factorized_expr=NULL;
	if (e->kind==Mult_expr)
	{
		expr e1=NULL,e2=NULL;
		expr_list removed_list=NULL;
		e1=copy();
		e2=e->copy();
		if (equivalent_lists(MultE(e1)->operands,MultE(e2)->operands,removed_list))
			//if both e1 and e2 have either 0 operands left or one constant at most
		{
			val=0;
			if (MultE(e1)->operands!=NULL && 
				MultE(e1)->operands->head->kind==Constant_expr) 
				val+=ConE(MultE(e1)->operands->head)->value;
			if (MultE(e2)->operands!=NULL &&
				MultE(e2)->operands->head->kind==Constant_expr) 
				val+=ConE(MultE(e2)->operands->head)->value;
			if (val!=0) factorized_expr=new mult_expr(new expr_pair(
				new constant_expr(val),removed_list));
			else factorized_expr=new constant_expr(0);
			return 1; 
		}
		else return 0; 
	}
	else return 0;
}


int plus_expr::equivalent(expr e)
{
	if (e->kind!=Plus_expr) return 0; 
	expr e1=NULL,e2=NULL;
	expr_list removed_list=NULL; 
	e1=copy();
	e2=e->copy();
	if (!equivalent_lists(PlusE(e1)->operands,PlusE(e2)->operands,removed_list)) return 0;
	if (PlusE(e1)->operands==NULL && PlusE(e2)->operands==NULL) return 1;
	if (PlusE(e1)->operands!=NULL && PlusE(e2)->operands!=NULL  &&
		PlusE(e1)->operands->head->kind==Constant_expr &&
		PlusE(e2)->operands->head->kind==Constant_expr &&
		ConE(PlusE(e1)->operands->head)->value==ConE(PlusE(e2)->operands->head)->value) return 1;
	return 0;
}

int mult_expr::equivalent(expr e)
{
	if (e->kind!=Mult_expr) return 0;
	expr e1=NULL,e2=NULL;
	expr_list removed_list=NULL; 
	e1=copy();
	e2=e->copy();
	if (!equivalent_lists(MultE(e1)->operands,MultE(e2)->operands,removed_list)) return 0;
	if (MultE(e1)->operands==NULL && MultE(e2)->operands==NULL) return 1;
	if (MultE(e1)->operands!=NULL && MultE(e2)->operands!=NULL  &&
		MultE(e1)->operands->head->kind==Constant_expr &&
		MultE(e2)->operands->head->kind==Constant_expr &&
		ConE(MultE(e1)->operands->head)->value==ConE(MultE(e2)->operands->head)->value) return 1;
	return 0;
}

int un_minus_expr::equivalent(expr e)
{
	if (e->kind!=Un_minus_expr) return 0; 
	return operand->equivalent(Un_minusE(e)->operand);
}

int div_expr::equivalent(expr e)
{
	if (e->kind!=Div_expr) return 0; 
	return left->equivalent(DivE(e)->left) && right->equivalent(DivE(e)->right);
}

int pow_expr::equivalent(expr e)
{
	if (e->kind!=Pow_expr) return 0; 
	return left->equivalent(PowE(e)->left) && right->equivalent(PowE(e)->right);
}

int or_expr::equivalent(expr e)
{
	error("equivalent for or's shouldn't be called...");
	return 0;
}

int identifier_expr::equivalent(expr e)
{
	if (e->kind==Identifier_expr && strcmp(ident,IdE(e)->ident)==0) return 1;
	return 0;
}

int constant_expr::equivalent(expr e)
{
	if (e->kind==Constant_expr && value==ConE(e)->value) return 1;
	return 0;
}


expr mult_expr::defact(expr_list &l,int & factorized_something)
{
	expr e=NULL,auxe=NULL;
	expr_list el=NULL;
	if (l==NULL) return NULL;
	if (l->head->kind==Plus_expr)
	{
		setExpr_block(factorized_something);
		e=l->head;
		l=l->tail;
		for(el=PlusE(e)->operands;el!=NULL;el=el->tail)
		{
			auxe=copy();
			MultE(auxe)->add_operand(el->head); 
			el->head=auxe;
		}
		return e; 
	}
	else return defact(l->tail,factorized_something); 
}

expr mult_expr::defactorize(int & simplified_something)
{
	expr auxe=NULL,sum=NULL;
	int factorized_something=0;
	auxe=defact(operands,factorized_something);
	if (factorized_something) {
		setExpr_block(simplified_something); 
		return auxe; 
	}
	else return this;
}



interval *expr_block::get_interval(void)
{
	error("expr_block::get_interval shouldn't be called");
	return NULL;
}

interval *identifier_expr::get_interval(void)
{
	interval *i=NULL;
	if (ident[0]=='$' && ident[1]=='L') i=new interval(0,PLUSINFINITY);
	else i=new interval(MINUSINFINITY,PLUSINFINITY);
	return i;
}


interval *constant_expr::get_interval(void)
{
	interval *i=NULL;
	i=new interval(value,value);
	return i;
}

interval *plus_expr::get_interval(void){
	interval *i=NULL,*j=NULL;
	expr_list l=NULL;
	j=new interval(0,0);
	for(l=operands;l!=NULL;l=l->tail)
	{
		i=l->head->get_interval();
		j->lower=my_add(j->lower,i->lower);
		j->upper=my_add(j->upper,i->upper);
		
	}
	return j;
}

interval *mult_expr::get_interval(void){
	interval *i=NULL,*j=NULL;
	expr_list l=NULL;
	int v1,v2; 
	j=new interval(1,1);
	for(l=operands;l!=NULL;l=l->tail){
		i=l->head->get_interval();
		v1=min4(my_mult(j->lower,i->lower),my_mult(j->lower,i->upper),
			my_mult(j->upper,i->lower),my_mult(j->upper,i->upper));
		v2=max4(my_mult(j->lower,i->lower),my_mult(j->lower,i->upper),
			my_mult(j->upper,i->lower),my_mult(j->upper,i->upper));
		j->lower=v1;
		j->upper=v2; 
	}
	return j;
}

interval *un_minus_expr::get_interval(void){
	interval *i=NULL,*j=NULL;
	i=operand->get_interval();
	j=new interval(-i->upper,-i->lower);
	return j;
}


interval *div_expr::get_interval(void)
{
	interval *i=NULL,*j=NULL,*k=NULL;
	i=left->get_interval();
	j=right->get_interval();
	k=new interval(min4(my_div(i->lower,j->lower),my_div(i->lower,j->upper),
		my_div(i->upper,j->lower),my_div(i->upper,j->upper)),
		max4(my_div(i->lower,j->lower),my_div(i->lower,j->upper),
		my_div(i->upper,j->lower),my_div(i->upper,j->upper)));
	return k;
}

interval *pow_expr::get_interval(void)
{
	error("forgot to paste over from div");
	return NULL;
}

interval *or_expr::get_interval(void)
{
	error("or_expr::get_interval shouldn't be called");
	return NULL;
}


//////////////////////////////// count_vars //////////////////////////////


int identifier_expr::count_vars(void){
	return 1;
}

int constant_expr::count_vars(void){
	return 0;
}

int plus_expr::count_vars(void){
	expr_list l=NULL;
	int s=0;
	for(l=operands;l!=NULL;l=l->tail){
		s+=l->head->count_vars();
	}
	return s;
}

int mult_expr::count_vars(void){
	expr_list l=NULL;
	int s=0;
	for(l=operands;l!=NULL;l=l->tail){
		s+=l->head->count_vars();
	}
	if(s>1) error("Too complicated expr (second degree)");
	return s;
}


//////////////////////////////// build_matrix /////////////////////////////


void plus_expr::build_matrix(intMatrix *coord,intMatrix *constr){
	expr_list l=NULL;
	int i=0;
	for(l=operands;l!=NULL;l=l->tail){
		l->head->build_matrix(coord,constr,i++);
	}
}	

/////////////////////// build_matrix //////////////////////////

void identifier_expr::build_matrix(intMatrix *coord,intMatrix *constr,int i){
	constr->Set(i*2,i,1);
	constr->Set(i*2,constr->nCol()-1,100);
	constr->Set(i*2+1,i,-1);
	constr->Set(i*2+1,constr->nCol()-1,-100);
}

void constant_expr::build_matrix(intMatrix *coord,intMatrix *constr,int i){
	coord->Set(0,i,value);
}

void mult_expr::build_matrix(intMatrix *coord,intMatrix *constr,int i){
	expr_list l=NULL;
	for(l=operands;l!=NULL;l=l->tail){
		l->head->build_matrix(coord,constr,i);
	}
}


////////////////////////// too_complicated /////////////////////

int identifier_expr::too_complicated(void){
	return 0;
}

int constant_expr::too_complicated(void){
	return 0;
}

int plus_expr::too_complicated(void){
	expr_list l=NULL;
	for(l=operands;l!=NULL;l=l->tail){
		if(l->head->too_complicated()) return 1;
	}  
	return 0;
}

int mult_expr::too_complicated(void){
	expr_list l=NULL;
	int nr_vars=0;
	for(l=operands;l!=NULL;l=l->tail){
		if(l->head->too_complicated()) return 1;
		if(l->head->kind==Identifier_expr) nr_vars++;
	}  
	if(nr_vars>1) return 1;
	return 0;
}





