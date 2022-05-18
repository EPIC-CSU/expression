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

#ifndef TREEBUILDING_H
#define TREEBUILDING_H

#include "IburgDefs.h"

#define MAX_GM_TERMS 403

// for thumb
#define INTEGER 2
#define STRING 1
#define NOATTRIB 0

struct TERM_MAP_TYPE{
	int term;
	char str[20];
};

int PerformedMut(void);
void setPerformedMut(int v);

void set_attr(NODEPTR_TYPE p,void *attr);
void * get_attr(NODEPTR_TYPE p);
void * copy_attr(void *attr,int size);
int GET_INT_ATTR(NODEPTR_TYPE p);
void SET_INT_ATTR(NODEPTR_TYPE p,int v);

int get_term_num(char * term);
char * get_term_str(int num);
int isInt(char *str);
int isFloat(char *str);

// for thumb
void set_attr_type(NODEPTR_TYPE p, int type);
int get_attr_type(NODEPTR_TYPE p);



NODEPTR_TYPE new_tree(int op, NODEPTR_TYPE l, NODEPTR_TYPE r);
STATE_TYPE burm_label(NODEPTR_TYPE p);
void dumpCover(NODEPTR_TYPE p, int goalnt, int indent);
NODEPTR_TYPE emitTree(NODEPTR_TYPE p, int goalnt);
void printTree(NODEPTR_TYPE p,int indent);


NODEPTR_TYPE *burm_kids(NODEPTR_TYPE p, int eruleno, NODEPTR_TYPE kids[]);
int burm_rule(STATE_TYPE state, int goalnt);
extern char *burm_string[];
extern short *burm_nts[];
extern char *burm_opname[];


NODEPTR_TYPE SemanticAction(NODEPTR_TYPE p,NODEPTR_TYPE old_kids[10],NODEPTR_TYPE new_kids[10],int eruleno);

int burm_op_label(NODEPTR_TYPE p);
STATE_TYPE burm_state_label(NODEPTR_TYPE p);
NODEPTR_TYPE burm_child(NODEPTR_TYPE p, int index);


#endif