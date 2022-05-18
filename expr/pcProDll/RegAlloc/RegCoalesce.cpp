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

//: RegCoalesce.cpp
//
// File:  		RegCoalesce.cpp
// Created:		Thu Feb 22, 2001
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Register coalescing (our implementation) tries to assign same physical registers
// to virtual registers used in a copy instruction. Later phases may remove the 
// redundant copy instructions.
// 

#include "stdafx.h"
#include "RegCoalesce.h"
#include "MultiChain.h"
#include "InterferenceGraph.h"
#include "ArrayIterator.h"
#include "DataOpSlot.h"
#include "NormalInstr.h"
#include "Routine.h"
#include "RegisterMapping.h"
#include "RegArchModel.h"
#include "IRHelper.h"

//#include "DebugRA.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

static int debugRA = 0;//debugRA_RegCoalesce;

class BaseArgument;

extern Routine *currRoutine;
extern isMove(BaseOperation * oper);
extern BaseArgument * getMoveSource(BaseOperation * oper);
extern BaseArgument * getMoveDest(BaseOperation * oper);
extern int SameSymbol(BaseArgument * source, BaseArgument * targ);
extern RegisterMapping *globalRegisterMapping;
extern RegArchModel globalRegModel;
extern void joinUDDUChainsForCopyOpArgs(BaseArgument *oldDefArg, int defArgID, BaseArgument *oldUseArg, int useArgID, BaseOperation *op);

extern void deleteUselessIR(Routine *rout);
extern void markOp(BaseOperation *oper);
extern int hasBeenMarked(BaseOperation *oper);
int maintainIRWalker = 0;

bool mergeDestSrc = false;


int coalesced = 0;

int canCoalesce(MultiChain * srcMC, MultiChain * destMC, InterferenceGraph& ig) {
	
	// the current heuristic is that we coalesce registers if there is 
	// no interference between them and if they have the same set
	// of compatible targets

	if (srcMC->isDoubleFloatArg() && destMC->isDoubleFloatArg())
		return false;

#if 0
	if (!ig.existInterference(srcMC, destMC) &&
		((*globalRegisterMapping->getCompatibleTargets(srcMC)).operator==(
			 *globalRegisterMapping->getCompatibleTargets(destMC))))


		return true;
	else
		return false;
#endif

//#if 0
	if (!ig.existInterference(srcMC, destMC)) {
		// fill argList with all arguments (uses and defs) from te multichain
		SetList<BaseArgument *> *srcArgs = srcMC->getAllArgs();

		// get list of register classes for the generic register arguments
		Vector<RegClassEntryNode2> srcRegClass;
		globalRegModel.createListOfClassTypesForArgs(srcArgs, srcRegClass, _GENERIC_);
		delete srcArgs;
		
		// fill argList with all arguments (uses and defs) from te multichain
		SetList<BaseArgument *> *destArgs = destMC->getAllArgs();

		// get list of register classes for the generic register arguments
		Vector<RegClassEntryNode2> destRegClass;
		globalRegModel.createListOfClassTypesForArgs(destArgs, destRegClass, _GENERIC_);
		delete destArgs;

		if (globalRegModel.isEqualClassSets(srcRegClass, destRegClass))
			return true;
	}
//#endif
		
	return false;
}


//========================================================================
// 
// Performs register coalescing on oper
//
//========================================================================
void regCoalesceOper(BaseOperation * oper, NormalInstr *instr, InterferenceGraph& ig) {
	
	if (isMove(oper)) {
	
		BaseArgument * src = getMoveSource(oper);
		BaseArgument * dest = getMoveDest(oper);

		if (!src || !src->isRegisterArg())
			return;

		if (!dest || !dest->isRegisterArg())
			return;

		MultiChain * srcMC = MultiChain::getMCOfArgID(src, currRoutine);
		MultiChain * destMC = MultiChain::getMCOfArgID(dest, currRoutine);

		// if same MCs, don't do coalescing
        if (srcMC == destMC) {
            // useless copy operation, just remove it
            
            // get a pointer to old def arg in oper
            int defArgID = srcMC->getID();
            BaseArgument *oldDefArg = IRHelper::getDefOfArgInOper(defArgID, oper);
            
            // get a pointer to old use arg in oper
            int useArgID = srcMC->getID();
            MyLinkedList<BaseArgument *> *oldUseArgList = IRHelper::getUsesOfArgInOper(useArgID, oper);
            BaseArgument *oldUseArg = oldUseArgList->getHead();
            delete oldUseArgList;
            
            // update UDDU to reflect that the copy op was removed
            joinUDDUChainsForCopyOpArgs(oldDefArg, defArgID, oldUseArg, useArgID, oper);
            
            srcMC->removeDef(instr);
            srcMC->removeUse(instr);
            markOp(oper);

        } else if (canCoalesce(srcMC, destMC, ig)) {

			if (debugRA)
				cout << "Coalescing: " << srcMC << " and " << destMC <<  endl;

			// this was the old way of handling the coalescing
			//ig.makeMetaMCNode(srcMC, destMC);
			// new coalescing method
			MultiChain *newMC = ig.coalesceMCs(destMC, srcMC, oper, instr);

			// remove useless copy operation
			newMC->removeDef(instr);

			int newArgID = newMC->getID();
			//test if there are other operations using newArgID in instr, besides the move op
			MyLinkedList<BaseArgument *> *useArgsInInstr = IRHelper::getUsesOfArgInInstr(newArgID, instr);
			MyLinkedList<BaseArgument *> *useArgsInOper = IRHelper::getUsesOfArgInOper(newArgID, oper);
			// if all uses are the ones in oper, we can safely delete instr from newMC's useList
			if (useArgsInOper->numItems() == useArgsInInstr->numItems())
				newMC->removeUse(instr);
			delete useArgsInOper;
			delete useArgsInInstr;
			markOp(oper);

			coalesced++;

			//if (debugRA)
			//	currRoutine->printHTG();
		}
	} else if (mergeDestSrc) {
		SetList<BaseArgument *> *defs = IRHelper::getAllDefsInOper(oper);
		SetList<BaseArgument *> *uses = IRHelper::getAllUsesInOper(oper);
		MyLinkedListIterator<BaseArgument *> *defIter = defs->elementsForward();
		MyLinkedListIterator<BaseArgument *> *useIter = uses->elementsForward();
		while (defIter->hasMoreElements()) {
			BaseArgument *def = defIter->currentElement();
			MultiChain *destMC = MultiChain::getMCOfArgID(def, currRoutine);

			while (useIter->hasMoreElements()) {
				BaseArgument *use = useIter->currentElement();
				MultiChain * srcMC = MultiChain::getMCOfArgID(use, currRoutine);

				if ((destMC != srcMC) && canCoalesce(srcMC, destMC, ig)) {

					if (debugRA)
						cout << "New Coalescing: " << srcMC << " and " << destMC <<  endl;
					
					// new coalescing method
					MultiChain *newMC = ig.coalesceMCs(destMC, srcMC, oper, instr, true);
					
					coalesced++;

					//if (debugRA)
					//	currRoutine->printHTG();
				}

				useIter->nextElement();
			}
		
			useIter->reset();
			defIter->nextElement();
		}
		delete defIter;
		delete useIter;
	}
}

//========================================================================
// 
// Performs register coalescing on instr
//
//========================================================================
void regCoalesceInstr(NormalInstr *instr, InterferenceGraph& ig) {
	
	// data operations
	ArrayIterator<DataOpSlot>* dataOpIter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while (dataOpIter->hasMoreElements()) {
		BaseOperation* op = dataOpIter->currentElement().getPtrToOperation();
		
		if (op == NULL) {
			dataOpIter->nextElement();
			continue;
		}
		
		if (!hasBeenMarked(op))
			regCoalesceOper(op, instr, ig);
		
		dataOpIter->nextElement();
	}	
	
	delete dataOpIter;
	
}


//========================================================================
// 
// Performs register coalescing on current routine (with interference graph ig)
//
//========================================================================

void registerCoalescing(InterferenceGraph& ig) {

	if (currRoutine->isEmptyBodyRoutine()) return;

	MyLinkedListIterator<BaseInstruction *> *instrIter = currRoutine->instrListIteratorForw();
	while (instrIter->hasMoreElements()) {
		NormalInstr *instr = (NormalInstr *) instrIter->currentElement();

		regCoalesceInstr(instr, ig);

		instrIter->nextElement();
	}
	delete instrIter;

	// remove all marked ops
	maintainIRWalker = 1;
	deleteUselessIR(currRoutine);
	maintainIRWalker = 0;

	if (debugRA)
		currRoutine->printHTG();
}
