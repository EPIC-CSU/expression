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

#ifndef WIN32
#include <stdio.h>
#include <stdlib.h>
#endif

#include <math.h>

#include "NormalInstr.h"
#include "UDDU.h"

#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "SemActPrimitives.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//Counting the mutations
long ConstFold=0;
long CopyProp=0;
long THR=0;
long MultToShift=0;
long THRCF=0;
long TempCount=0;

void PrintMutationCounts(void){
	cout << endl;
	cout << "Constant folding: " << ConstFold << endl;
	cout << "Copy Propagation: " << CopyProp << endl;
	cout << "Tree Height Reduction: " << THR << endl;
	cout << "Multiply to Shift: " << MultToShift << endl;
	cout << "THR with CF: " << THRCF << endl;
	cout << "Temporary Counter: " << TempCount << endl;
}


void ADDOMITTEDOPS(NODEPTR_TYPE p){
	BaseOperation *op;
	if((op=GetIburgToOpMap(p))!=NULL){
		AddOmittedOps(op);
	}
}

extern TEMP_CREATION_NUM;

char *NEW_TEMP(void){
	char *c=new char[8];
	sprintf(c,"temp%d",TEMP_CREATION_NUM++);
	return c;
}

int index_of(NODEPTR_TYPE p,NODEPTR_TYPE kids[10],int eruleno){
	int i;
	short *nts = burm_nts[eruleno];

	for(i=0;nts[i];i++){
		if(p==kids[i]) return i;
	}
	return -1;
}

NODEPTR_TYPE coresp_kid(NODEPTR_TYPE p,NODEPTR_TYPE old_kids[10],NODEPTR_TYPE new_kids[10],int eruleno){
	int i;
	NODEPTR_TYPE r;
	if(p==NULL) return NULL;
	if((i=index_of(p,old_kids,eruleno))!=-1){
		r=new_kids[i];
		return r;
	}
	return NULL;
}

NODEPTR_TYPE duplicate_tree(NODEPTR_TYPE p,NODEPTR_TYPE old_kids[10],NODEPTR_TYPE new_kids[10],int eruleno){
	NODEPTR_TYPE r;
	int i;
	if(p==NULL) return NULL;
	if((i=index_of(p,old_kids,eruleno))!=-1){
		r=new_kids[i];
		return r;
	}

	r=new_tree(OP_LABEL(p),DUPLICATE_TREE(LEFT_CHILD(p)),
								DUPLICATE_TREE(RIGHT_CHILD(p)));

	set_attr(r,get_attr(p));
	// for thumb
	set_attr_type(r, get_attr_type(p));

	if(GetIburgToOpMap(p)!=NULL){
		AddIburgToOpMap(r,GetIburgToOpMap(p));
		DeleteIburgToOpMap(p);
	}

	if(GetIburgToArgMap(p)!=NULL){
		AddIburgToArgMap(r,GetIburgToArgMap(p));
		DeleteIburgToArgMap(p);
	}

	return r;
}


int instr_no(NODEPTR_TYPE p,NODEPTR_TYPE old_kids[10],NODEPTR_TYPE new_kids[10],int eruleno){
	const int _CONST_=-1000000;
	BaseOperation *op;
	BaseArgument *arg;
	BaseInstruction *instr;
	NODEPTR_TYPE r;
	r=coresp_kid(p,old_kids,new_kids,eruleno);
	if(r!=NULL) p=r;
	if((op=GetIburgToOpMap(p))!=NULL){
		return op->getInstr()->idValue();
	}
	if((arg=GetIburgToArgMap(p))!=NULL){
		MyLinkedList<BaseOperation*> *reachDefList=GetDefList(arg);
		MyLinkedListIterator<BaseOperation *> *iter=reachDefList->elementsForward();
		int min=1000000;
		while(iter->hasMoreElements()){
			instr=iter->currentElement()->getInstr();
			if(instr!=NULL && instr->idValue()<min) 
				min=iter->currentElement()->getInstr()->idValue();
			iter->nextElement();
		}
		delete iter;
		if(min==1000000) return _CONST_;
		else return min;
	}
	if(OP_LABEL(p)==get_term_num("INT"))
		return _CONST_;
	abort();
	return -1;
}
