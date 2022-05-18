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
//==================================================================================================================
//  file:	MaintainUD.cpp
//  purpose: maintaining ud and du chain after unrolling
//  date:    June 2, 98
//==================================================================================================================

#include "stdafx.h"

#include "BaseHTGNode.h"
#include "LoopHTGNode.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"
#include "MyLinkedList.h"
#include "BasicBlockNode.h"
#include "BBIterator.h"
#include "BaseArgument.h"
#include "RegisterArg.h"
#include "SSAArgProperty.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "BaseArgProperty.h"
#include "BaseOperation.h"
#include "ComputeOp.h"
#include "MemoryOp.h"
#include "IfOp.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "Array.h"
#include "NormalInstr.h"
#include "SymbolTable.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


#define  DEF   1
#define  USE   0

int debugMaintainUD = 0;

//=======================================================================================
// local data  structure
//=======================================================================================

struct defsOfOneObj{
	int registerIndex;
	int ssaNumber;
	MyLinkedList<BaseOperation*> defs;
};


MyLinkedList<defsOfOneObj> AllDefinitions;




static
defsOfOneObj*
findDefInDefList(int index, int ssaNum) {
	MyLinkedListIterator<defsOfOneObj>* iter = AllDefinitions.elementsForward();
	while ( iter->hasMoreElements() ) {
		defsOfOneObj& oneDef = iter->currentElement();
		if ( oneDef.registerIndex == index && oneDef.ssaNumber == ssaNum ) {
			delete iter;
			return &oneDef;
		}
		iter->nextElement();
	}
	delete iter;
	return 0;
}


/*
static
int
checkDUInArg(int defSsaNum, BaseOperation* useOp, int pos) {
	BaseArgument* useArg;
	
	if (pos == _DEST_)
		useArg = useOp->ptrToOperand(pos, _DEST_LIST_);
	else
		useArg = useOp->ptrToOperand(pos, _SRC_LIST_);


	if (useArg == NULL) return _NO_;

	if ( ! useArg->isRegisterArg() ) 
		return _NO_;
	SSAArgProperty* prop = (SSAArgProperty*)useArg->getPtrToProperty(_SSA_);
#ifdef _DEBUG
	assert(prop);
#endif
	int useSsaNum = prop->getSSANum();
	if ( useSsaNum == defSsaNum )
		return _YES_;
	return _NO_;
}
*/

static
int
checkDUInArg(int defSsaNum, BaseArgument *arg)
{
	if (arg == NULL) return _NO_;

	if ( ! arg->isRegisterArg() ) 
		return _NO_;
	SSAArgProperty* prop = (SSAArgProperty*)arg->getPtrToProperty(_SSA_);
#ifdef _DEBUG
	assert(prop);
#endif
	int useSsaNum = prop->getSSANum();
	if ( useSsaNum == defSsaNum )
		return _YES_;
	return _NO_;
}


static
void
checkDUChain(BaseArgument* defArg, int defSsaNum, BaseOperation* useOp) {

	if (useOp == NULL) return;

	ArgListIter ai;
	ArgList al;

	int needDel = 1;

	useOp->sourceOperandsList(al);

	for (ai = al.begin(); ai != al.end(); ai++)
	{
		if (*ai == NULL) continue;

		if (checkDUInArg(defSsaNum, *ai))
		{
			needDel = 0;
			break;
		}
	}
	if ( needDel )
		((DUChainProperty*)defArg->getPtrToProperty(_DUCHAIN_))->remove(useOp);																	
}

extern SymbolTable *globalSymbolTable;

static
void
recordDef(BaseOperation* op) {

	if (op == NULL) return;

	ArgListIter ai;
	ArgList al;

	op->destOperandsList(al);

	for (ai = al.begin(); ai != al.end(); ai++)
	{

		BaseArgument* defArg = *ai;
		int index = globalSymbolTable->getIndex(defArg);
		SSAArgProperty* prop = (SSAArgProperty*)defArg->getPtrToProperty(_SSA_);
	#ifdef _DEBUG
		assert(prop);
	#endif

		int tempDEBUG = 0;
		int ssaNum = prop->getSSANum();
		// check this definition's du chain, delete the use that has the different ssa number
		// from this definition
		DUChainProperty* duProp = (DUChainProperty*)defArg->getPtrToProperty(_DUCHAIN_);

		if (tempDEBUG)
		{
			cout << " (* ";
			op->shortPrint(cout);
		}
		
		if ( duProp  ) {
			MyLinkedListIterator<BaseOperation*>* useIter = duProp->useListIteratorForw();
			while (useIter->hasMoreElements() ) {
				BaseOperation* useOp = useIter->currentElement();
				useIter->nextElement();

				if (tempDEBUG)
				{
					cout << endl << "\t";
					useOp->shortPrint(cout);
				}
				checkDUChain(defArg, ssaNum, useOp);
			}
			delete useIter;
		}
		if (tempDEBUG)
		{
			cout << " *) " << endl;
		}
		// find the index and ssa number in all definitions list
		defsOfOneObj* oneDef = findDefInDefList(index, ssaNum);
		if ( oneDef == NULL ) {
			defsOfOneObj oneDefinition;
			AllDefinitions.append(oneDefinition);
			defsOfOneObj& newDef = AllDefinitions.getTail();
			newDef.registerIndex = index;
			newDef.ssaNumber = ssaNum;
			newDef.defs.append(op);
		} else {
			oneDef->defs.append(op);
		}
	}
}



static
void
checkUDChain(BaseArgument* useArg, int ssaNum, BaseOperation* defOp) {
	BaseArgument* defArg = defOp->ptrToOperand(_DEST_, _DEST_LIST_);
	SSAArgProperty* prop = (SSAArgProperty*)defArg->getPtrToProperty(_SSA_);
#ifdef _DEBUG
	assert(prop);
#endif
	int defSsaNum = prop->getSSANum();
	if ( ssaNum != defSsaNum )
		((UDChainProperty*)useArg->getPtrToProperty(_UDCHAIN_))->remove(defOp);																
}




static void
modifyUDDU(BaseOperation* op, int position) 
{
	BaseArgument* useArg;
	
	if (position == _DEST_)
		useArg = op->ptrToOperand(position, _DEST_LIST_);
	else
		useArg = op->ptrToOperand(position, _SRC_LIST_);


	if ((useArg != NULL) && (useArg->isRegisterArg())) {
	int index = globalSymbolTable->getIndex(useArg);
		SSAArgProperty* prop = (SSAArgProperty*)useArg->getPtrToProperty(_SSA_);
#ifdef _DEBUG
		assert(prop);
#endif
		int ssaNum = prop->getSSANum();
		UDChainProperty* udProp = (UDChainProperty*)useArg->getPtrToProperty(_UDCHAIN_);
		if ( udProp)  {
			// check some definition need to be removed or not
			MyLinkedListIterator<BaseOperation*>* defIter = udProp->defListIteratorForw();
			while (defIter->hasMoreElements() ) {
				BaseOperation* defOp = defIter->currentElement();
				defIter->nextElement();
				checkUDChain(useArg, ssaNum, defOp);
			}
			delete defIter;

		}

		// find the definitions that recorded in the Alldefinitions list
		defsOfOneObj* oneDefList = findDefInDefList(index, ssaNum);
		if ( oneDefList ) {
			// append all these definitions
			if ( !udProp ) {
				UDChainProperty tmpProp;
				useArg->addProperty(tmpProp);
				udProp = (UDChainProperty*)useArg->getPtrToProperty(_UDCHAIN_);
			}
			MyLinkedListIterator<BaseOperation*>* defOpIter = oneDefList->defs.elementsForward();
			while ( defOpIter->hasMoreElements() ) {
				BaseOperation* defOp = defOpIter->currentElement();
				udProp->appendDef(defOp);
				BaseArgument* defArg = defOp->ptrToOperand(_DEST_, _DEST_LIST_);
				DUChainProperty* duProp = (DUChainProperty*)defArg->getPtrToProperty(_DUCHAIN_);

				if ( duProp == NULL ) {
					DUChainProperty tmpProp;
					defArg->addProperty(tmpProp);
					duProp = (DUChainProperty*)defArg->getPtrToProperty(_DUCHAIN_);
				}

				duProp->appendUse(op);
				
				defOpIter->nextElement();
			}
			delete defOpIter;
		}
	}
}



static
void
recordDefUse(BaseInstruction* instr, int tag) {
	ArrayIterator<DataOpSlot>* iter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
			iter->nextElement();
			continue;
		}
		// compute operation
		if ( op->isComputeOp() )  {
			if ( tag == DEF ) {
				recordDef(op);
			}
			else {
				modifyUDDU(op, _SOURCE_1_);
				modifyUDDU(op, _SOURCE_2_); 
			}
		}
		// memory operation
		else if ( op->isMemoryOp() ) {
			if ( tag == DEF ) {
				if ( ((MemoryOp*)op)->isLoad() ) 
					recordDef(op);
			} else {
				modifyUDDU(op, _SOURCE_1_);
				if ( ((MemoryOp*)op)->isStore() ) 
					//modifyUDDU(op, _DEST_);
					modifyUDDU(op, _SOURCE_2_);

			}
		} 
		iter->nextElement();
	}	
	
	delete iter;

	if ( tag == USE ) {
		// control operation
		ArrayIterator<ControlOpSlot>* cntlIter = ((NormalInstr*)instr)->getControlOpSlotIter();
		while ( cntlIter->hasMoreElements() ) {
			BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
			if ( op && op->isIfOp() ) {
				modifyUDDU(op, _SOURCE_1_);  
			}
			cntlIter->nextElement();
		}
		delete cntlIter;
	}

}




static
void
traverseSimpleHTG(SimpleHTGNode* node, int tag) {
	BasicBlockNode* bb = node->getBasicBlock();
	BBForwIterator bbIter(bb);
	while ( bbIter.hasMoreElements() ) {
		BaseInstruction* instr = bbIter.currentElement();
		recordDefUse(instr, tag);
		bbIter.nextElement();
	}

}


static void traverseNode(BaseHTGNode*, int);



static
void
traverseBranch(MyLinkedListIterator<BaseHTGNode*>* iter, int tag) {
	while ( iter->hasMoreElements() ) {
		traverseNode(iter->currentElement(), tag);
		iter->nextElement();
	}
	delete iter;
}




static
void
traverseLoop(LoopHTGNode* loop, int tag) {
	MyLinkedListIterator<BaseHTGNode*>* iter = loop->createLoopIterForw();
	while ( iter->hasMoreElements() ) {
		traverseNode(iter->currentElement(), tag);
		iter->nextElement();
	}
	delete iter;
}



static
void
traverseNode(BaseHTGNode* node, int tag) {
		if ( node->isComplexNode() || node->isCallNode() )
			;
		else if ( node->isIfNode() ) {
			BaseHTGNode* ifHead = ((IfHTGNode*)node)->getIfHead();
#ifdef _DEBUG
			assert(ifHead->isSimpleNode() );
#endif
			// SAH: Wed, Aug 04, 99
			//
			// During unrolling, we do not keep the IfHead uptodate.
			// 
			// 
			if (ifHead != NULL)
			{
				traverseSimpleHTG((SimpleHTGNode*)ifHead, tag);
			}
			traverseBranch(((IfHTGNode*)node)->createTrueBranchIterForw(), tag);
			traverseBranch(((IfHTGNode*)node)->createFalseBranchIterForw(), tag);
		} else if ( node->isSimpleNode() ) {
			traverseSimpleHTG((SimpleHTGNode*)node, tag); 
		} else { // loopNode
			traverseLoop((LoopHTGNode*)node, tag);
		}

}




void
maintainUD(BaseHTGNode* loop) {
	if (!loop->isLoopNode())
	{
		// error condition.
		cout << "Panic: Recieved non loop node in maintainUD" << endl;
		assert(0);
	}

	traverseLoop((LoopHTGNode *)loop, DEF);
	traverseLoop((LoopHTGNode *)loop, USE);
}

extern int equalRegs(BaseArgument *reg1, BaseArgument *reg2);

// Deletes all USE - DEF references from the fromOp to the toOp.
//
// if refArg != NULL, check to make sure that you only delete chains that involve refArg.
//
void deleteUseDefsFromTo(BaseOperation *fromOp, BaseOperation *toOp, BaseArgument *refArg)
{
	if ((fromOp == NULL)  || (toOp == NULL)) return;

	if (debugMaintainUD)
	{
		cout << " Deleting UD Chains from: " << endl << "\t";
		fromOp->shortPrint(cout);
		cout << endl << " to: " << endl << "\t";
		toOp->shortPrint(cout);
		cout << endl;
	}

	ArgListIter ai;
	BaseArgument *arg;

	ArgList & al = fromOp->sourceOperandList();

	for (ai = al.begin(); ai != al.end(); ai++)
	{
		if (*ai == NULL) continue;

		arg = *ai;

		if (!arg->isRegisterArg()) continue;
		if ((refArg != NULL) && (!equalRegs(refArg, arg))) continue;

		((UDChainProperty *)arg->getPtrToProperty(_UDCHAIN_))->remove(toOp);
	}
}

// Deletes all DEF - USE references from the fromOp to the toOp.
//
// if refArg != NULL, check to make sure that you only delete chains that involve refArg.
//
void deleteDefUsesFromTo(BaseOperation *fromOp, BaseOperation *toOp, BaseArgument *refArg = NULL)
{
	if ((fromOp == NULL)  || (toOp == NULL)) return;

	if (debugMaintainUD)
	{
		cout << " Deleting DU Chains from: " << endl << "\t";
		fromOp->shortPrint(cout);
		cout << endl << " to: " << endl << "\t";
		toOp->shortPrint(cout);
		cout << endl;
	}

	ArgListIter ai;
	BaseArgument *arg;

	ArgList & al = fromOp->destOperandList();

	for (ai = al.begin(); ai != al.end(); ai++)
	{
		if (*ai == NULL) continue;

		arg = *ai;

		if (!arg->isRegisterArg()) continue;
		if ((refArg != NULL) && (!equalRegs(refArg, arg))) continue;

		((DUChainProperty *)arg->getPtrToProperty(_DUCHAIN_))->remove(toOp);
	}
}

// Deletes all DEF-USE chains from "arg" and all USE-DEF chains to "arg".
//
// "arg" is a destination argument in "op"
//
void deleteDestArgChains(BaseArgument *arg, BaseOperation *op)
{
	if (arg == NULL) return;

	DUChainProperty *prop;
	if ((prop = (DUChainProperty *)arg->getPtrToProperty(_DUCHAIN_)) == NULL) return;

	if (debugMaintainUD)
	{
		cout << " Deleting Dest Arg Chains from: " << endl << "\t";
		op->shortPrint(cout);
		cout << endl;
	}

	MyLinkedListIterator<BaseOperation *> *iter = prop->useListIteratorForw();

	while (iter->hasMoreElements())
	{
		deleteUseDefsFromTo(iter->currentElement(), op, arg);

		iter->nextElement();
	}
	delete iter;

	prop->removeAllUses();
}

// Deletes all USE-DEF chains from "arg" and all DEF-USE chains to "arg".
//
// "arg" is a source argument in "op"
//
void deleteSourceArgChains(BaseArgument *arg, BaseOperation *op)
{
	if (arg == NULL) return;

	UDChainProperty *prop;
	if ((prop = (UDChainProperty *)arg->getPtrToProperty(_UDCHAIN_)) == NULL) return;

	if (debugMaintainUD)
	{
		cout << " Deleting Source Arg Chains from: " << endl << "\t";
		op->shortPrint(cout);
		cout << endl;
	}

	MyLinkedListIterator<BaseOperation *> *iter = prop->defListIteratorForw();

	while (iter->hasMoreElements())
	{
		deleteDefUsesFromTo(iter->currentElement(), op, arg);

		iter->nextElement();
	}
	delete iter;

	prop->removeAllDefs();
}

extern void linkUDDU(BaseArgument *def, BaseArgument *use, BaseOperation *defSt, BaseOperation *useSt);

// fromArg is a dest and toArg is a source
//
void addDUUDChains(BaseArgument *fromArg, BaseOperation *fromOp,
				   BaseArgument *toArg, BaseOperation *toOp)
{
	if ((fromArg == NULL) || (!fromArg->isRegisterArg())) return;
	if ((toArg == NULL) || (!toArg->isRegisterArg())) return;

	if (debugMaintainUD)
	{
		cout << " Adding DUUD Chains from: " << endl << "\t";
		fromOp->shortPrint(cout);
		cout << endl << " to: " << endl << "\t";
		toOp->shortPrint(cout);
		cout << endl;
	}

	linkUDDU(fromArg, toArg, fromOp, toOp);
}

// fromArg is a source and toArg is a dest
//
// Note: This is the same as addDUUDChains(toArg, toOp, fromArg, fromOp)
//
void addUDDUChains(BaseArgument *fromArg, BaseOperation *fromOp,
				   BaseArgument *toArg, BaseOperation *toOp)
{
	if ((fromArg == NULL) || (!fromArg->isRegisterArg())) return;
	if ((toArg == NULL) || (!toArg->isRegisterArg())) return;

	if (debugMaintainUD)
	{
		cout << " Adding UDDU Chains from: " << endl << "\t";
		fromOp->shortPrint(cout);
		cout << endl << " to: " << endl << "\t";
		toOp->shortPrint(cout);
		cout << endl;
	}

	linkUDDU(toArg, fromArg, toOp, fromOp);
}

void addDUUDChains(BaseOperation *defOp, BaseOperation *useOp, BaseArgument *refArg)
{
	if ((defOp == NULL) || (useOp == NULL)) return;

	ArgList & dL = defOp->destOperandList();
	ArgList & sL = useOp->sourceOperandList();

	ArgListIter di, si;
	BaseArgument *darg, *sarg;

	for (di = dL.begin(); di != dL.end(); di++)
	{
		if (*di == NULL) continue;

		darg = *di;
		if (!darg->isRegisterArg()) continue;

		if ((refArg != NULL) && (!equalRegs(refArg, darg))) continue;

		for (si = sL.begin(); si != sL.end(); si++)
		{
			if (*si == NULL) continue;

			sarg = *si;
			if (!sarg->isRegisterArg()) continue;

			if ((refArg != NULL) && (!equalRegs(refArg, sarg))) continue;

			if (!equalRegs(darg, sarg)) continue;

			// Finally, we know that darg and sarg are equal and need DU, UD chains.
			//
			addDUUDChains(darg, defOp, sarg, useOp);
		}
	}
}

// Note: This is the same as:
//
// addDUUDChains(defOp, useOp, refArg);
//
void addUDDUChains(BaseOperation *useOp, BaseOperation *defOp, BaseArgument *refArg)
{
	if ((useOp == NULL) || (defOp == NULL)) return;

	ArgList & sL = useOp->sourceOperandList();
	ArgList & dL = defOp->destOperandList();

	ArgListIter si, di;
	BaseArgument *sarg, *darg;

	for (si = sL.begin(); si != sL.end(); si++)
	{
		if (*si == NULL) continue;

		sarg = *si;
		if (!sarg->isRegisterArg()) continue;

		if ((refArg != NULL) && (!equalRegs(refArg, sarg))) continue;

		for (di = dL.begin(); di != dL.end(); di++)
		{
			if (*di == NULL) continue;

			darg = *di;
			if (!darg->isRegisterArg()) continue;

			if ((refArg != NULL) && (!equalRegs(refArg, darg))) continue;

			if (!equalRegs(darg, sarg)) continue;

			// Finally, we know that darg and sarg are equal and need DU, UD chains.
			//
			addUDDUChains(sarg, useOp, darg, defOp);
		}
	}
}

// if deleteFrom == 1, then delete the DU chain (and UD chains) from (to) fromArg
//
// Transfers the DU chains from one dest arg (fromArg) to another dest arg (toArg)
//
void transferDUUDChains(BaseArgument *fromArg, BaseOperation *fromOp, 
						BaseArgument *toArg, BaseOperation *toOp, int deleteFrom)
{
	if ((fromArg == NULL) || (!fromArg->isRegisterArg())) return;
	if ((toArg == NULL) || (!toArg->isRegisterArg())) return;

	DUChainProperty *prop = (DUChainProperty *)fromArg->getPtrToProperty(_DUCHAIN_);

	if (prop == NULL) return;

	if (debugMaintainUD)
	{
		cout << " Transfering DUUD Chains from: " << endl << "\t";
		fromOp->shortPrint(cout);
		cout << endl << " to: " << endl << "\t";
		toOp->shortPrint(cout);
		cout << endl;
	}

	MyLinkedListIterator<BaseOperation *> *iter = prop->useListIteratorForw();

	while (iter->hasMoreElements())
	{
		BaseOperation *useOp = iter->currentElement();

		addDUUDChains(toOp, useOp, toArg);

		iter->nextElement();
	}
	delete iter;

	if (deleteFrom) 
		deleteDestArgChains(fromArg, fromOp);
}

// if deleteFrom == 1, then delete the UD chain (and DU chains) from (to) fromArg
//
// Transfers the UD chains from one source arg (fromArg) to another source arg (toArg)
//
void transferUDDUChains(BaseArgument *fromArg, BaseOperation *fromOp, 
						BaseArgument *toArg, BaseOperation *toOp, int deleteFrom)
{
	if ((fromArg == NULL) || (!fromArg->isRegisterArg())) return;
	if ((toArg == NULL) || (!toArg->isRegisterArg())) return;

	UDChainProperty *prop = (UDChainProperty *)fromArg->getPtrToProperty(_UDCHAIN_);

	if (prop == NULL) return;

	MyLinkedListIterator<BaseOperation *> *iter = prop->defListIteratorForw();

	if (debugMaintainUD)
	{
		cout << " Transfering UDDU Chains from: " << endl << "\t";
		fromOp->shortPrint(cout);
		cout << endl << " to: " << endl << "\t";
		toOp->shortPrint(cout);
		cout << endl;
	}

	while (iter->hasMoreElements())
	{
		BaseOperation *defOp = iter->currentElement();

		addUDDUChains(toOp, defOp, toArg);

		iter->nextElement();
	}
	delete iter;

	if (deleteFrom) 
		deleteSourceArgChains(fromArg, fromOp);
}