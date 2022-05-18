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

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#endif

#include <iostream.h>
#include <assert.h>
#include <map>

#include "TreeBuilding.h"
#include "MutPass.h"
#include "InstrSelIburg.h"

#include "STLIncludes.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


int isInt(char *str){
	char *p;
	for(p=str;*p!='\0';p++){
		if(!isdigit(*p) && *p!='-' && *p!='+') return 0;
	}
	return 1;
}

int isFloat(char *str){
	char *p;
	for(p=str;*p!='\0';p++){
		if(!isdigit(*p) && *p!='-' && *p!='+' && *p!='.' && *p!='e' && *p!='E') return 0;
	}
	return 1;
}


int get_gm_term_num(char * term){
	for(int i=0;i<MAX_GM_TERMS;i++){
		if(burm_opname[i]!=0 && strcmp(term,burm_opname[i])==0) return i;
	}
	if(isInt(term)) return get_term_num("INT");
	if(isFloat(term)) return get_term_num("INT");
	abort();
	return -1;
}

char * get_gm_term_str(int num){
	char *r=new char[20];
	strcpy(r,burm_opname[num]);
	return r;
}

int get_term_num(char *num){
	if(GetGenericMutations())
		return get_gm_term_num(num);
	else if(GetInstructionSelection())
		return isel_get_term_num(num);
	else
		abort();
	return -1;
}

char *get_term_str(int num){
	if(GetGenericMutations())
		return get_gm_term_str(num);
	else if(GetInstructionSelection())
		return isel_get_term_str(num);
	else
		abort();
	return NULL;
}


NODEPTR_TYPE new_tree(int op, NODEPTR_TYPE l, NODEPTR_TYPE r) {
	// NODEPTR_TYPE p = (NODEPTR_TYPE)malloc(sizeof *p);
	// purify fix
	NODEPTR_TYPE p = (NODEPTR_TYPE)malloc(sizeof(NODE_TYPE));

	assert(p);
	p->op = op;
	p->kids[0] = l; p->kids[1] = r;
	return p;
}

//////////////////// attributes ////////////////////////////////////

map<NODEPTR_TYPE,void *> attr_map;
// added for thumb
map<NODEPTR_TYPE,int> attr_type_map;


void set_attr(NODEPTR_TYPE p,void *attr){
	attr_map[p]=attr;
}

void set_attr_type(NODEPTR_TYPE p,int type){
	attr_type_map[p]=type;
}

void * get_attr(NODEPTR_TYPE p){
	if(p==NULL) abort();
	if(attr_map.find(p)==attr_map.end()){
		return NULL;
	}
	else
		return attr_map[p];
}

int get_attr_type(NODEPTR_TYPE p){
	if(p==NULL) abort();
	if( p->op == 252 ) 
		return INTEGER;
	if(attr_type_map.find(p)==attr_type_map.end()){
		return NOATTRIB;
	}
	else
		return attr_type_map[p];
}


void * copy_attr(void *attr,int size){
	if(attr==NULL) return NULL;
	else{
		void *r=(void *)malloc(size);
		memcpy(r,attr,size);
		return r;
	}
}

void SET_INT_ATTR(NODEPTR_TYPE p,int v){
	set_attr(p,new int(v));
	// for thumb
	set_attr_type(p, INTEGER);
}

int GET_INT_ATTR(NODEPTR_TYPE p){
	return *(int *)get_attr(p);
}


void SET_STR_ATTR(NODEPTR_TYPE p,char *v){
	set_attr(p,strdup(v));
	// for thumb
	set_attr_type(p, STRING);
}

char *GET_STR_ATTR(NODEPTR_TYPE p){
	return (char *)get_attr(p);
}
///////////////////////////////////////////////////////////////////


void dumpCover(NODEPTR_TYPE p, int goalnt, int indent) {
	int eruleno = burm_rule(STATE_LABEL(p), goalnt);
	short *nts = burm_nts[eruleno];
	NODEPTR_TYPE kids[10];
	int i;

	for (i = 0; i < indent; i++)
		cout << " ";
	if((char *)burm_string[eruleno]==NULL){
		cerr << "Could not find pattern to cover subject tree";
		abort();
	}
	else 
		cout << (char *)burm_string[eruleno];

	burm_kids(p, eruleno, kids);

	cout << endl;

	for (i = 0; nts[i]; i++)
		dumpCover(kids[i], nts[i], indent + 1);
}

void printTree(NODEPTR_TYPE p,int indent) {
	int i;

	if(p==NULL) return;

	for (i = 0; i < indent; i++)
		cout << " ";

	int *ptr=((int*)get_attr(p));
	cout << burm_opname[OP_LABEL(p)] << " " << (int) ((ptr==NULL)?0:(*ptr)) << endl;

	printTree(LEFT_CHILD(p),indent+1);
	printTree(RIGHT_CHILD(p),indent+1);
}


NODEPTR_TYPE emitTree(NODEPTR_TYPE p, int goalnt) {
	int i;
	int eruleno = burm_rule(STATE_LABEL(p), goalnt);
	short *nts = burm_nts[eruleno];
	NODEPTR_TYPE old_kids[10];
	NODEPTR_TYPE new_kids[10];

	burm_kids(p, eruleno, old_kids);
	for (i = 0; nts[i]; i++)
		new_kids[i]=emitTree(old_kids[i], nts[i]);

	return SemanticAction(p,old_kids,new_kids,eruleno);
}



int performedMut=0;

int PerformedMut(void){
	return performedMut;
}

void setPerformedMut(int v){
	performedMut=v;
}

