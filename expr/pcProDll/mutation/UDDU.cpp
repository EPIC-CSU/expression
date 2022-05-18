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

#include "BaseOperation.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "IburgToIR.h"
#include "MemoryOp.h"

#include "STLIncludes.h"

#include <set>
#include <map>


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 



MyLinkedList<BaseOperation*> *GetDefList(BaseArgument *arg){
	MyLinkedList<BaseOperation*> *reachDefList;
	BaseArgProperty * prop;

	prop=arg->getPtrToProperty(_UDCHAIN_);
	if(prop!=NULL) 
	{
		reachDefList=&((UDChainProperty*)prop)->getDefList();
		return reachDefList;
	}
	return NULL;
	//return new MyLinkedList<BaseOperation*>();
}

MyLinkedList<BaseOperation*> *GetUseList(BaseArgument *arg){
	BaseArgProperty * prop;
	MyLinkedList<BaseOperation*> *useList;

	prop=arg->getPtrToProperty(_DUCHAIN_);
	if(prop!=NULL){
		useList=&((DUChainProperty*)prop)->getUseList();
		return useList;
	}
	return NULL;
	//return new MyLinkedList<BaseOperation*>();
}

void GatherUsesDests(set<BaseOperation *> *Ops,
					 set<BaseArgument *> *Uses,
					 set<BaseArgument *> *Dest){
	
}

int ContainsArg(set<BaseArgument *> *argset,BaseArgument *arg){
	set<BaseArgument*>::iterator i;
	if((i=argset->find(arg))==argset->end())
		return 0;
	else
		return 1;
}

BaseArgument * FindEqualArg(set<BaseArgument *> *argset,BaseArgument *arg){
	set<BaseArgument*>::iterator i;
	for(i=argset->begin();i!=argset->end();i++){
		if(equalRegs(arg,*i)) return *i;
	}
	return NULL;
}


void LinkUseDef(BaseArgument *dest,BaseOperation *dest_op,BaseArgument *src,BaseOperation *src_op){
	DUChainProperty *prop1,*prop2;
	UDChainProperty *prop3,*prop4;

	prop1=(DUChainProperty*)dest->getPtrToProperty(_DUCHAIN_);
	if(prop1==NULL){
		prop2=new DUChainProperty();
		prop2->addUse(src_op);
		dest->addProperty(*prop2);
	}
	else{
		prop1->addUse(src_op);
	}

	prop3=(UDChainProperty*)src->getPtrToProperty(_UDCHAIN_);
	if(prop3==NULL){
		prop4=new UDChainProperty();
		prop4->addDef(dest_op);
		src->addProperty(*prop4);
	}
	else{
		prop3->addDef(dest_op);
	}
}

void RemoveUse(BaseArgument *dest,BaseOperation *op){
	DUChainProperty *prop;
	prop=(DUChainProperty*)dest->getPtrToProperty(_DUCHAIN_);
	if(prop!=NULL){
		// cout << "Removing DU for " << dest << " with op: " << op << endl;
		prop->remove(op);
	}
}

void RemoveDef(BaseArgument *src,BaseOperation *op){
	UDChainProperty *prop;
	prop=(UDChainProperty*)src->getPtrToProperty(_UDCHAIN_);
	if(prop!=NULL){
		// cout << "Removing UD for " << src << " with op: " << op << endl;
		prop->remove(op);
	}
}

void RemoveUseDef(BaseArgument *dest,BaseOperation *dest_op,BaseArgument *src,BaseOperation *src_op){
	RemoveUse(dest,src_op);
	RemoveDef(src,dest_op);
}

void RemoveAllDef(BaseArgument *arg, BaseOperation *arg_op)
{
	MyLinkedListIterator<BaseOperation*> *iter;
	BaseOperation *op;
	MyLinkedList<BaseOperation*> *deflist;
	deflist=GetDefList(arg);
	if((deflist!=NULL) && (!deflist->isEmpty())){
		//for all the definitions of arg
		iter=deflist->elementsForward();
		while (iter->hasMoreElements()){
			//op is the definition which defines arg
			op=iter->currentElement();
			iter->nextElement();

			// Changed by SAH : Fri, 18th August, 2000
			//
			// MULTIPLE_DESTS_AND_SOURCES
			//
			ArgList & a = op->destOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					RemoveUseDef((*ai), op, arg, arg_op);
				}
			}
			//
			// end change for MULTIPLE_DESTS_AND_SOURCES

		}
		delete iter;
	}
}

void RemoveAllUse(BaseArgument *arg, BaseOperation *arg_op)
{
	MyLinkedListIterator<BaseOperation*> *iter;
	BaseOperation *op;
	MyLinkedList<BaseOperation*> *uselist;
	uselist=GetUseList(arg);
	if((uselist!=NULL) && (!uselist->isEmpty())){
		//for all the uses of arg
		iter = uselist->elementsForward();
		while (iter->hasMoreElements()){
			//op is the use of arg
			op=iter->currentElement();
			iter->nextElement();
			//remove all the chains between arg and op
/*
			if(equalRegs(arg,op->ptrToOperand(_SOURCE_1_)))
				RemoveUseDef(arg,arg_op,op->ptrToOperand(_SOURCE_1_),op);
			if(equalRegs(arg,op->ptrToOperand(_SOURCE_2_)))
				RemoveUseDef(arg,arg_op,op->ptrToOperand(_SOURCE_2_),op);
*/

			// Changed by SAH : Fri, 18th August, 2000
			//
			// MULTIPLE_DESTS_AND_SOURCES
			//
			ArgList & a = op->sourceOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					if (equalRegs(arg, (*ai)))
					{
						RemoveUseDef(arg, arg_op, (*ai), op);
						break;
					}
				}
			}
			//
			// end change for MULTIPLE_DESTS_AND_SOURCES
		}
		delete iter;
	}
}

//removes all the UseDef chains of the argument arg
//arg: the argument to remove UDDU chains for
//arg_op: the operand continaing the arg
void RemoveAllUseDef(BaseArgument *arg,BaseOperation *arg_op){
	MyLinkedListIterator<BaseOperation*> *iter;
	BaseOperation *op;
	MyLinkedList<BaseOperation*> *deflist;
	MyLinkedList<BaseOperation*> *uselist;
	deflist=GetDefList(arg);
	if(deflist!=NULL){
		//for all the definitions of arg
		iter=deflist->elementsForward();
		while (iter->hasMoreElements()){
			//op is the definition which defines arg
			op=iter->currentElement();
			iter->nextElement();
			//remove all the chains between op and arg
/*
			RemoveUseDef(op->ptrToOperand(_DEST_, _DEST_LIST_),op,arg,arg_op);
*/

			// Changed by SAH : Fri, 18th August, 2000
			//
			// MULTIPLE_DESTS_AND_SOURCES
			//
			ArgList & a = op->destOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					RemoveUseDef((*ai), op, arg, arg_op);
				}
			}
			//
			// end change for MULTIPLE_DESTS_AND_SOURCES

		}
		delete iter;
	}
	uselist=GetUseList(arg);
	if(uselist!=NULL){
		//for all the uses of arg
		iter = uselist->elementsForward();
		while (iter->hasMoreElements()){
			//op is the use of arg
			op=iter->currentElement();
			iter->nextElement();
			//remove all the chains between arg and op
/*
			if(equalRegs(arg,op->ptrToOperand(_SOURCE_1_)))
				RemoveUseDef(arg,arg_op,op->ptrToOperand(_SOURCE_1_),op);
			if(equalRegs(arg,op->ptrToOperand(_SOURCE_2_)))
				RemoveUseDef(arg,arg_op,op->ptrToOperand(_SOURCE_2_),op);
*/

			// Changed by SAH : Fri, 18th August, 2000
			//
			// MULTIPLE_DESTS_AND_SOURCES
			//
			ArgList & a = op->sourceOperandList();
			ArgListIter ai;
			for (ai = a.begin(); ai != a.end(); ai++)
			{
				if ((*ai) != NULL)
				{
					if (equalRegs(arg, (*ai)))
						RemoveUseDef(arg, arg_op, (*ai), op);
				}
			}
			//
			// end change for MULTIPLE_DESTS_AND_SOURCES
		}
		delete iter;
	}
}


void MoveUDDUUse(BaseArgument *src1, BaseArgument *src2,map<BaseArgument*,BaseOperation*>&ArgToOp){
	MyLinkedList<BaseOperation*> *defList;
	MyLinkedListIterator<BaseOperation*> *iter;
	
	defList=GetDefList(src1);
	if(defList!=NULL){
		for(iter=defList->elementsForward();iter->hasMoreElements();iter->nextElement()){
			LinkUseDef(iter->currentElement()->ptrToOperand(_DEST_, _DEST_LIST_),iter->currentElement(),src2,ArgToOp[src2]);
			RemoveUse(iter->currentElement()->ptrToOperand(_DEST_, _DEST_LIST_),ArgToOp[src1]);
		}
		delete iter;
	}
}

void MoveUDDUDest(BaseArgument *dest1, BaseArgument *dest2,map<BaseArgument*,BaseOperation*>&ArgToOp){
	MyLinkedList<BaseOperation*> *useList;
	MyLinkedListIterator<BaseOperation*> *iter;

	useList=GetUseList(dest1);
	if(useList!=NULL){
		for(iter=useList->elementsForward();iter->hasMoreElements();iter->nextElement()){
			if(equalRegs(dest2,iter->currentElement()->ptrToOperand(_SOURCE_1_))){
				LinkUseDef(dest2,ArgToOp[dest2],iter->currentElement()->ptrToOperand(_SOURCE_1_),iter->currentElement());
				RemoveDef(iter->currentElement()->ptrToOperand(_SOURCE_1_),ArgToOp[dest1]);
			}
			else if(equalRegs(dest2,iter->currentElement()->ptrToOperand(_SOURCE_2_))){
				LinkUseDef(dest2,ArgToOp[dest2],iter->currentElement()->ptrToOperand(_SOURCE_2_),iter->currentElement());
				RemoveDef(iter->currentElement()->ptrToOperand(_SOURCE_2_),ArgToOp[dest1]);
			}
		}
		delete iter;
	}
}

void GatherUsesDests(set<BaseOperation*> *ops,set<BaseArgument *>* uses,
					 set<BaseArgument *> *dests,
					 map<BaseArgument *,BaseOperation*>*argtoop)
{
	set<BaseOperation*>::iterator i;
	BaseArgument *dest,*src1,*src2;
	BaseOperation *oper;

	for(i=ops->begin();i!=ops->end();i++){
		oper=*i;
		if(oper->hasOperand(_DEST_, _DEST_LIST_))
			dest=oper->ptrToOperand(_DEST_, _DEST_LIST_);
		src1=oper->ptrToOperand(_SOURCE_1_);
		src2=oper->ptrToOperand(_SOURCE_2_);
		uses->insert(src1);
		if(src2!=NULL) uses->insert(src2);
		if(oper->hasOperand(_DEST_, _DEST_LIST_))
			dests->insert(dest);
		(*argtoop)[src1]=oper;
		if(src2!=NULL) (*argtoop)[src2]=oper;
		if(oper->hasOperand(_DEST_, _DEST_LIST_))
			(*argtoop)[dest]=oper;
	}
}


void UpdateUDDU(set<BaseOperation *> *OldOps, set<BaseOperation *> *NewOps,
				set<BaseOperation *> *ReinsertedOps){
	set<BaseArgument*> OldUses;
	set<BaseArgument*> OldDest;
	set<BaseArgument*> NewUses;
	set<BaseArgument*> NewDest;
	set<BaseArgument*> ReinsUses;
	set<BaseArgument*> ReinsDest;
	map<BaseArgument*,BaseOperation*> ArgToOp;
	set<BaseArgument *>::iterator i,j;
	BaseArgument *src,*dest;

	OldUses.clear();
	OldDest.clear();
	NewUses.clear();
	NewDest.clear();
	ReinsUses.clear();
	ReinsDest.clear();
	ArgToOp.clear();

	int tempoDebug = 0;

	if (tempoDebug)
	{
		// iterate over oldops, newops and print ud, du chains.
		//
		PropNameList pL; pL.push_back(_DUCHAIN_);

		set<BaseOperation *>::iterator klm;
		cout << "Printing DU list for Old Ops: " << endl;
		for (klm = OldOps->begin(); klm != OldOps->end(); klm++)
		{
			cout << "Op : " << (*klm) << " # ";
			(*klm)->print(cout); cout << endl;
			(*klm)->printProperties(cout, pL);
			cout << endl;
		}
		cout << "Printing DU list for New Ops: " << endl;
		for (klm = NewOps->begin(); klm != NewOps->end(); klm++)
		{
			cout << "Op : " << (*klm) << " # ";
			(*klm)->print(cout); cout << endl;
			(*klm)->printProperties(cout, pL);
			cout << endl;
		}
	}

	if(OldOps!=NULL)
		GatherUsesDests(OldOps,&OldUses,&OldDest,&ArgToOp);
	GatherUsesDests(NewOps,&NewUses,&NewDest,&ArgToOp);
	GatherUsesDests(ReinsertedOps,&ReinsUses,&ReinsDest,&ArgToOp);
	
	for(i=NewUses.begin();i!=NewUses.end();i++){
		if((dest=FindEqualArg(&NewDest,*i))!=NULL){
			LinkUseDef(dest,ArgToOp[dest],*i,ArgToOp[*i]);
		}
		else if((dest=FindEqualArg(&ReinsDest,*i))!=NULL){
			LinkUseDef(dest,ArgToOp[dest],*i,ArgToOp[*i]);
		}
		else if((src=FindEqualArg(&OldUses,*i))!=NULL){
			MoveUDDUUse(src,*i,ArgToOp);
		}
	}
	for(i=NewDest.begin();i!=NewDest.end();i++){
		if((src=FindEqualArg(&NewUses,*i))!=NULL){
			//nothing, they were linked earliez
		}
		if((dest=FindEqualArg(&OldDest,*i))!=NULL){
			MoveUDDUDest(dest,*i,ArgToOp);
		}
	}
	for(i=OldUses.begin();i!=OldUses.end();i++){
		RemoveAllUseDef(*i,ArgToOp[*i]);
	}
	for(i=OldDest.begin();i!=OldDest.end();i++){
		RemoveAllUseDef(*i,ArgToOp[*i]);
	}
}

void deleteUDDU(BaseOperation *oper){
	if (oper == NULL) return;

	ArgListIter ai1, ai2;

	ai1 = (oper->destOperandList()).begin();
	ai2 = (oper->destOperandList()).end();

	while (ai1 != ai2)
	{
		if (*ai1 != NULL)
		{
			RemoveAllUse(*ai1, oper);
		}
		ai1++;
	}

	ai1 = (oper->sourceOperandList()).begin();
	ai2 = (oper->sourceOperandList()).end();

	while (ai1 != ai2)
	{
		if (*ai1 != NULL)
		{
			RemoveAllDef(*ai1, oper);
		}
		ai1++;
	}
}