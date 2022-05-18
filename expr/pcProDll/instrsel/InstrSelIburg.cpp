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



#include "iostream.h"

#include "IburgDefs.h"
#include "InstrSel.h"
#include "TreeBuilding.h"
#include "InstrSelSA.h"





//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 



int isel_get_term_num(char * term){
	for(int i=0;i<MAX_IS_TERMS;i++){
		if(isel_opname[i]!=0 && strcmp(term,isel_opname[i])==0) return i;
	}
	if(isInt(term)) return get_term_num("INT");
	if(isFloat(term)) return get_term_num("INT");
	abort();
	return -1;
}

char * isel_get_term_str(int num){
	char *r=new char[20];
	strcpy(r,isel_opname[num]);
	return r;
}



void dumpIselCover(NODEPTR_TYPE p, int goalnt, int indent) {
	int eruleno = isel_rule(STATE_LABEL(p), goalnt);
	short *nts = isel_nts[eruleno];
	NODEPTR_TYPE kids[10];
	int i;

	for (i = 0; i < indent; i++)
		cout << " ";
	if((char *)isel_string[eruleno]==NULL){
		cerr << "Could not find pattern to cover subject tree";
		abort();
	}
	else 
		cout << (char *)isel_string[eruleno];

	isel_kids(p, eruleno, kids);

	cout << endl;

	for (i = 0; nts[i]; i++)
		dumpIselCover(kids[i], nts[i], indent + 1);
}


void printIselTree(NODEPTR_TYPE p,int indent) {
	int i;

	if(p==NULL) return;

	for (i = 0; i < indent; i++)
		cout << " ";

	int *ptr=((int*)get_attr(p));
	cout << isel_opname[OP_LABEL(p)] << " " << (int) ((ptr==NULL)?0:(*ptr)) << endl;

	printIselTree(LEFT_CHILD(p),indent+1);
	printIselTree(RIGHT_CHILD(p),indent+1);
}


NODEPTR_TYPE emitIselTree(NODEPTR_TYPE p, int goalnt) {
	int i;
	int eruleno = isel_rule(STATE_LABEL(p), goalnt);
	short *nts = isel_nts[eruleno];
	NODEPTR_TYPE old_kids[10];
	NODEPTR_TYPE new_kids[10];

	isel_kids(p, eruleno, old_kids);
	for (i = 0; nts[i]; i++)
		new_kids[i]=emitIselTree(old_kids[i], nts[i]);

	return InstrSelSA(p,old_kids,new_kids,eruleno);
}
