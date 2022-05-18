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

#include "MemoryOp.h"
#include "TreeBuilding.h"
#include "IburgToIR.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int HasManyUses(BaseArgument *dst);
extern void SET_STR_ATTR(NODEPTR_TYPE p,char *v);

NODEPTR_TYPE MemoryOp::BuildIburgTree(int rootOfTree,int essential)
{
	char *opname;
	BaseArgument *src, *dst, *offset;
	NODEPTR_TYPE p=NULL;


	src=ptrToSourceOperand();
	dst=ptrToDestOperand();
	offset=getOffset();
	opname=globalOpCodeTable[getOpCodeIndex()].opName();

	if(ContainsOperation(_OldOps,this)) return dst->BuildIburgTree(this,essential);


	//for now, do not build IVLOAD and IVSTORE
	//return dst->BuildIburgTree(this,essential);//ATTENTION!!!!!!!!!!!!!!!!!!!!!!!!!

	//Version with uncommented line is for building the tree only for non "essential instructions"
	//Commented line = builds tree also for essential instructions. 
	//if(HasManyUses(dst) && !rootOfTree) return dst->BuildIburgTree(this); //ATTEINTION!!!!!!!!!!!!!!!!!!!!!!
	if(isLoad() && HasManyUses(dst) && !rootOfTree){
		essential=1;
	}

	//all the instrs which are essential, or children of essntial instrs, are added to this set
	if(essential) AddEssentialOps(this);


	if(opname!=NULL){
		//NODEPTR_TYPE dest=new_tree(get_term_num(dst->Name()),0,0);
		NODEPTR_TYPE dest;
		if(dst->isLabelArg()){
			//dest=dst->BuildIburgTree(this,essential);
			dest=new_tree(get_term_num("isel_label"),0,0);
			SET_STR_ATTR(dest,dst->lName());
			AddIburgToArgMap(dest,dst);
		}
		else{
			dest=new_tree(get_term_num(dst->Name()),0,0);
		}
		p=new_tree(get_term_num("DEF"),
				dest,
				new_tree(get_term_num(opname),
					src->BuildIburgTree(this,essential),
					(offset==NULL)?0:offset->BuildIburgTree(this,essential)
					)
			);
		AddIburgToOpMap(p,this);
		AddIburgToArgMap(dest,dst);
		_OldOps.insert(this);
		/*
		cout << "DEF(";
			cout << dst->Name();
			cout << ",";
			cout << opname <<"(";
				src->BuildIburgTree(this);
				if(offset != NULL){
					cout << ",";
					offset->BuildIburgTree(this);
				}
			cout << ")";
		cout << ")";
		*/
	}
	return p;
}

