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

#include "ComputeOp.h"
#include "OpCodeTable.h"
#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "PropertyIncludes.h"
#include "DUProperty.h"
#include "UDDU.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int HasManyUses(BaseArgument *dst){
	MyLinkedList<BaseOperation*> *useList;

	useList=GetUseList(dst);
	if(useList==NULL) return 0;
	int nr=useList->numItems();
	if(nr>1 || nr==1 && equalRegs(useList->getHead()->ptrToOperand(_SOURCE_1_),
						  useList->getHead()->ptrToOperand(_SOURCE_2_))){
		return 1;
	}
	return 0;
}

extern int isNOP(int opCode);

NODEPTR_TYPE ComputeOp::BuildIburgTree(int rootOfTree, int essential)
{
	char *opname;
	BaseArgument *src1, *src2, *dst;
	NODEPTR_TYPE p=NULL;

	src1=ptrToOperand(_SOURCE_1_);
	src2=ptrToOperand(_SOURCE_2_);
	dst=ptrToOperand(_DEST_, _DEST_LIST_);

	if(dst==NULL) return NULL;
//	if (isNOP(getOpCodeIndex())) return NULL;

	if(ContainsOperation(_OldOps,this)) return dst->BuildIburgTree(this,essential);

	//Version with uncommented line is for building the tree only for non "essential instructions"
	//Commented line = builds tree also for essential instructions. 
	//if(HasManyUses(dst) && !rootOfTree) return dst->BuildIburgTree(this); //ATTEINTION!!!!!!!!!!!!!!!!!!!!!!
	if(dst!=NULL && HasManyUses(dst) && !rootOfTree){
		essential=1;
	}

	//all the instrs which are essential, or children of essntial instrs, are added to this set
	if(essential) AddEssentialOps(this);

	opname=globalOpCodeTable[getOpCodeIndex()].opName();
	if (isNOP(getOpCodeIndex()))
	{
		p = new_tree(get_term_num("DEF"), 
			dst->BuildIburgTree(this,essential),
			new_tree(get_term_num(opname),
			0,
			0
			)
		);
	}
	else if(opname!=NULL){
		if(src2!=NULL){
			p=new_tree(get_term_num("DEF"),
				dst->BuildIburgTree(this,essential),
				new_tree(get_term_num(opname),
					src1->BuildIburgTree(this,essential),
					src2->BuildIburgTree(this,essential)
					)
				);
		}
		else{
			p=new_tree(get_term_num("DEF"),
				dst->BuildIburgTree(this,essential),
				new_tree(get_term_num(opname),
					src1->BuildIburgTree(this,essential),
					0
					)
				);
		}
		AddIburgToOpMap(p,this);
		_OldOps.insert(this);
		/*
		cout << "DEF(";
			cout << dst->Name();
			cout << ",";
			cout << opname <<"(";
				src1->BuildIburgTree(this);
				if(src2 != NULL){
					cout << ",";
					src2->BuildIburgTree(this);
				}
			cout << ")";
		cout << ")";
		*/
	}
	return p;
}
