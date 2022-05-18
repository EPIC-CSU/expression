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
//: Spills.cpp
//
// File:  		Spills.cpp
// Created:		Wed Aug 16, 2000
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// Handles the spilling of live ranges in register allocation
// Refer to SpillSupport.cpp for the support functions 
// which create spill and reload instructions

#include "stdafx.h"
#include "BaseArgument.h"
#include "IConstArg.h"
#include "MemoryOp.h"
#include "NormalInstr.h"
#include "Routine.h"
#include "IRTraversal.h"
#include "IRHelper.h"
#include "MultiChain.h"
#include "MultiChainProperty.h"
#include "DUProperty.h"
#include "UDProperty.h"
#include "JumpOp.h"
#include "MCNode.h"
#include "DoubleMCNode.h"
#include "MetaMCNode.h"
#include "Spills.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


static int debugRA = 0;

class RegisterFileList;
class OpCode;


#define SPILL_MEMORY_BYTES 4		// !! modify it with care, spilling of doubles depends on it
//int spillMemoryStart = 20000000;
int spillMemoryStart = 9000000;
//int freeSpillMemoryStart = spillMemoryStart;
int spills = 0;
int reloads = 0;

extern bool saveReloads;
extern void processReloads();

extern Routine *currRoutine;
extern RegisterFileList *globalRegFileList;
//extern NormalInstr *dummyInstr;
extern NormalOp *dummyOp;
//InterferenceGraph *crtIG;
//MultiChain *crtMC;

extern void addDoubleProperties(BaseArgument *arg1, BaseArgument *arg2);

// for getting the target spill reload instructions

extern void readSpillReloadFile();
NormalInstr * createReloadInstr(BaseArgument *useArg, BaseArgument *sibArg = NULL, MultiChain *mc = NULL);
NormalInstr * createSpillInstr(BaseArgument * defArg, BaseArgument *sibArg = NULL);


// link the def of arg in defInstr to all uses of arg in useInstr (UD and DU chains)
// and add them to mc
// there is a special case when defInstr is dummyInstr, for which we just simulate defs
void linkDefUseOfArgInInstrMC(BaseArgument *arg, NormalInstr *defInstr, NormalInstr *useInstr, MultiChain *mc)
{
	// get def of arg
	BaseArgument *defArg = IRHelper::getDefOfArgInInstr(arg, defInstr);

	// get op of defArg
	BaseOperation *defOp = defArg ? 
		IRHelper::getOperationWithArgInInstr(defArg, defInstr) :
		dummyOp;	// assume that if defArg not found, we are in dummyInstr

	// add defArg to mc
	mc->addDef(defInstr);
	if (defArg)
		((MultiChainProperty *) defArg->getPtrToProperty(_MULTICHAIN_))->setMC(mc);

	// get DU prop of defArg
	DUChainProperty *duProp = defArg ?
		(DUChainProperty *) defArg->getPtrToProperty(_DUCHAIN_) :
		NULL;
	
	// get uses of arg
	MyLinkedList<BaseArgument *> *useArgList = IRHelper::getUsesOfArgInInstr(arg, useInstr);
	MyLinkedListIterator<BaseArgument *> *useArgListIter = useArgList->elementsForward();
	while (useArgListIter->hasMoreElements()) {
		BaseArgument *useArg = useArgListIter->currentElement();

		// get op of useArg
		BaseOperation *useOp = IRHelper::getOperationWithArgInInstr(useArg, useInstr);

		// add useArg to mc
		mc->addUse(useInstr);
		((MultiChainProperty *) useArg->getPtrToProperty(_MULTICHAIN_))->setMC(mc);

		// get UD prop of defArg
		UDChainProperty *udProp = (UDChainProperty *) useArg->getPtrToProperty(_UDCHAIN_);

		// link UDDUs
		if (duProp)
			duProp->appendUse(useOp);
		udProp->appendDef(defOp);
		
		useArgListIter->nextElement();
	}
	delete useArgListIter;
	delete useArgList;
}

// replace all the connections from instr to oldInstr with connections to newInstr
// also update iWalker with the new connections
void replaceReferences(NormalInstr *instr, NormalInstr *oldInstr, NormalInstr *newInstr)
{
	BaseOperation *flowOpPtr;
	
	BaseOperation *oper;
	BaseInstruction *tgt = NULL;
	
	if ((oper = instr->hasCallRetOp()) != NULL)
	{
		// We do not want to return a pointer to an instruction in another routine.
		// So, we ignore the instr that is the target of a call.
		
		tgt = ((CallRetOp *)oper)->getTargetInstr();
	}
	BaseInstruction *succ;
	
	ArrayIterator<FlowOpSlot> *_succListIter;
	_succListIter = instr->getFlowOpSlotIter();
	while (_succListIter->hasMoreElements())
	{
		flowOpPtr = (_succListIter->currentElement()).getPtrToOperation();
		
		if (flowOpPtr != NULL)
		{
			succ = flowOpPtr->getTargetInstr();
			
			if (succ != tgt) // i.e. if succ is not the target of a call/ret
				if (succ == oldInstr) {
					flowOpPtr->setTargetInstr(newInstr);
				}
		}
		_succListIter->nextElement();
	}
	delete _succListIter;

	instr->removeSuccID(oldInstr->idValue());
	instr->addSuccID(newInstr->idValue());

	currRoutine->iWalker->delEdge(oldInstr, instr);
	currRoutine->iWalker->addPred(newInstr, instr);
}

// the next function only works if the basic block has only one succesor
// in case of multiple successors, try to insert the instruction at the beggining
// of each successor basic block
void appendInstrToBB(NormalInstr *newInstr, BasicBlockNode *bb)
{
	NormalInstr *lastInstr = (NormalInstr*) bb->getLastInstr();
	NormalInstr *nextInstr = (NormalInstr*) lastInstr->getDefaultTargetInstr();

	ASSERT_TRACE(bb->getNextSucc() == NULL, "The basic block has more than one successor. Read the comments above the function!");
	ASSERT_TRACE(bb->isCallRetType() == _NO_, "The basic block (to which an instruction is to be appended) has a call op as its trailer.");

	// insert the newInst in routine and iWalker
	currRoutine->insertInstrAfter(newInstr, lastInstr);
	//currRoutine->iWalker->addNewInstr(newInstr);
	
	// handle connections between newInstr and nextInstr
	currRoutine->iWalker->addPred(nextInstr, newInstr);
	newInstr->addSuccID(nextInstr->idValue());
	// set default target of newInstr to nextInstr
	//newInstr->setTargetInstr(nextInstr);
	JumpOp *jOp = new JumpOp(nextInstr);
	BaseOpSlot& newOpSlot = newInstr->getFlowOpSlot(numFlowOpsPerInstr - 1);
		((FlowOpSlot *) &newOpSlot)->addOperation(jOp);

	// handle connections between lastInstr and newInstr
	replaceReferences(lastInstr, nextInstr, newInstr);

	// update bb trailer
	bb->initTrailer(newInstr);
}

// does exactly what it says :)
void prependInstrToBB(NormalInstr *newInstr, BasicBlockNode *bb)
{
	NormalInstr *firstInstr = (NormalInstr*) bb->getFirstInstr();

	// insert the newInst in routine and iWalker
	currRoutine->insertInstrBefore(newInstr, firstInstr);
	//currRoutine->iWalker->addNewInstr(newInstr);

	// move label from firstInstr to newInstr
	if (firstInstr->hasLabel()) {
		BaseArgument *label = firstInstr->getPtrToLabelArg();
		newInstr->addLabel(label);
		firstInstr->deleteLabel();
	}
	
	// handle connections between instructions before firstInstr and newInstr
	InstrList pred = currRoutine->iWalker->getNext(firstInstr);
	InstrListIter i;
	for (i=pred.begin(); i != pred.end(); i++) {
		NormalInstr *prevInstr = (NormalInstr *) *i;

		// handle connections between prevInstr and newInstr
		replaceReferences(prevInstr, firstInstr, newInstr);
	}

	// handle connections between newInstr and firstInstr
	currRoutine->iWalker->addPred(firstInstr, newInstr);
	newInstr->addSuccID(firstInstr->idValue());
	// set default target of newInstr to firstInstr
	//newInstr->setTargetInstr(firstInstr);
	JumpOp *jOp = new JumpOp(firstInstr);
	BaseOpSlot& newOpSlot = newInstr->getFlowOpSlot(numFlowOpsPerInstr - 1);
		((FlowOpSlot *) &newOpSlot)->addOperation(jOp);

	// update bb leader
	bb->initLeader(newInstr);

	//currRoutine->iWalker->addNewInstr(newInstr);
}

// this function works corectly only when both prevInstr and nextInstr belong to the same masic block
// in special cases, between blocks, use appendInstrToBB and prependInstrToBB
void insertInstrBetween(NormalInstr *newInstr, NormalInstr *prevInstr, NormalInstr *nextInstr)
{
	// insert the newInst in routine and iWalker
	currRoutine->insertInstrAfter(newInstr, prevInstr);
	currRoutine->iWalker->addNewInstr(newInstr);
	
	// handle connections between newInstr and nextInstr
	currRoutine->iWalker->addPred(nextInstr, newInstr);
	newInstr->addSuccID(nextInstr->idValue());
	// set default target of newInstr to nextInstr
	//newInstr->setTargetInstr(nextInstr);
	JumpOp *jOp = new JumpOp(nextInstr);
	BaseOpSlot& newOpSlot = newInstr->getFlowOpSlot(numFlowOpsPerInstr - 1);
		((FlowOpSlot *) &newOpSlot)->addOperation(jOp);

	// handle connections between prevInstr and newInstr
	replaceReferences(prevInstr, nextInstr, newInstr);
}

//This routine has been extensively modified by SAH on 01/17/2001
//
// The modification was to take care of the situation wherein spill ops were to be inserted after call ops.
//
// insert the spill of arg just after defInstr
void insertSpillOfArgAfterInstr(BaseArgument *arg, NormalInstr *defInstr, MultiChain *mc, int spillMemoryStart,
								BaseArgument *sibArg = NULL, MultiChain *sibMC = NULL)
{
	// sanity check
	if (!arg->isRealRegArg())
		ASSERT_TRACE(0, "Trying to reload a non-register...");

	if (sibArg && !sibArg->isRealRegArg())
		ASSERT_TRACE(0, "Trying to reload a non-register...");

	BaseOperation *oper;
	BaseInstruction *tgt = NULL;
	
	// for all successor instructions
	ArrayIterator<FlowOpSlot> *_succListIter = defInstr->getFlowOpSlotIter();

	if ((oper = defInstr->hasCallRetOp()) != NULL)
	{
		// then, do not add a spill op in between the call op and its first succ.
		//its first succ is an instr in a different routine.

		_succListIter->nextElement();
	}

	while (_succListIter->hasMoreElements())
	{
		BaseOperation *flowOpPtr = (_succListIter->currentElement()).getPtrToOperation();

		if (flowOpPtr != NULL)
		{
			NormalInstr *nextInstr = (NormalInstr *) flowOpPtr->getTargetInstr();

			if (nextInstr) {	

				// now, insert spill between defInstr and nextInstr
				// first, create spill instruction
				
				// get the pointer to the newly created spilled instruction
				NormalInstr *newUseInstr = createSpillInstr(arg, sibArg);

				BasicBlockNode *defBB = defInstr->getBasicBlock();

				// insert newUseInstr in routine, just after defInstr
				if (defBB->getLastInstr() != defInstr) {
					// defInstr is not the last in basic block
					// just insert newUseInstr after defInstr
					insertInstrBetween(newUseInstr, defInstr, nextInstr);
				}
				// modified by SAH on 01/17/01
				// previously it was : else if (defBB->getNextSucc() == NULL) {
				//
				else if ((defBB->getNextSucc() == NULL) && (defBB->isCallRetType() == _NO_)) {
					// defBB has only one successor and defBB has no call/ret
					// append newUseInstr to defBB
					appendInstrToBB(newUseInstr, defBB);
				} else {
					// defBB has multiple successors or has call/ret op
					// prepend newUseInstr to next bb
					BasicBlockNode *nextBB = nextInstr->getBasicBlock();

					// if nextBB == NULL, then, there is an error
					//
					if (nextBB == NULL) 
					{
						cout << "Panic: Recieved NULL BB while trying to insert spill node" << endl;
						assert(0);
					}
					prependInstrToBB(newUseInstr, nextBB);
				}

				// update UDDU chains and MC between the two instructios
				linkDefUseOfArgInInstrMC(arg, defInstr, newUseInstr, mc);
				if (sibArg)
					linkDefUseOfArgInInstrMC(sibArg, defInstr, newUseInstr, sibMC);
			}
		}

		_succListIter->nextElement();
	}
	delete _succListIter;
}

// insert the reload of arg just before useInstr
void insertReloadOfArgBeforeInstr(BaseArgument *arg, NormalInstr *useInstr, MultiChain *mc, int spillMemoryStart,
								  BaseArgument *sibArg = NULL, MultiChain *sibMC = NULL)
{
	// sanity check
	if (!arg->isRealRegArg())
		ASSERT_TRACE(0, "Trying to reload a non-register...");

	if (sibArg && !sibArg->isRealRegArg())
		ASSERT_TRACE(0, "Trying to reload a non-register...");
	
	InstrList pred = currRoutine->iWalker->getNext(useInstr);
	NormalInstr *prevInstr = (NormalInstr *) *(pred.begin());

	// now, insert reload between prevInstr and useInstr
	// first, create reload instruction
	
	NormalInstr *newDefInstr = createReloadInstr(arg, sibArg, mc);
	reloads++;
	
	BasicBlockNode *useBB = useInstr->getBasicBlock();
				
	// insert newDefInstr in routine, just before useInstr
	if (useBB->getFirstInstr() != useInstr) {
		// useInstr is not the first in basic block
		// just insert newDefInstr before useInstr
		insertInstrBetween(newDefInstr, prevInstr, useInstr);
	} else {
		// useBB has multiple predecessors
		// prepend newDefInstr to useBB
		prependInstrToBB(newDefInstr, useBB);
	}

	// update UDDU chains and MC between the two instructios
	linkDefUseOfArgInInstrMC(arg, newDefInstr, useInstr, mc);
	if (sibArg)
		linkDefUseOfArgInInstrMC(sibArg, newDefInstr, useInstr, sibMC);
}

void resetDUProperty(BaseArgument *defArg) {
	// clear DU property of def argument
	//if (defArg) { // this may be the dummy instr, so defArg may not really exist
		DUChainProperty * duProp = (DUChainProperty *) defArg->getPtrToProperty(_DUCHAIN_);
		if (duProp)
			duProp->removeAllUses();
		else {
			DUChainProperty tmp;
			defArg->addProperty(tmp);
		}
	//}
}

void resetUDProperty(BaseArgument *useArg) {
	// clear UD property of use argument
	//if (useArg) { // this may be the dummy instr, so defArg may not really exist
		UDChainProperty * udProp = (UDChainProperty *) useArg->getPtrToProperty(_UDCHAIN_);
		if (udProp)
			udProp->removeAllDefs();
		else {
			UDChainProperty tmp;
			useArg->addProperty(tmp);
		}
	//}
}


// split mc into many small MCs containing the existent instr and the new spill/reload instr
// return the list of created MCs
MyLinkedList<MultiChain *> * insertSpillsReloads(MultiChain *mc, bool processDoubles = false)
{
	
	// Partha : 07/27
	// read the spill-reload file and get the information for the target spill, reload instr
	readSpillReloadFile();


	int argID = mc->getID();
	BaseArgument *arg = IRHelper::createArg(argID);

	BaseArgument *siblingArg = NULL;
	MultiChain *siblingMC = mc->getSiblingMC();
	if (siblingMC)
		siblingArg = IRHelper::createArg(siblingMC->getID());

	//MyLinkedList<MultiChain *> *newMCs = new MyLinkedList<MultiChain *>();

	/// make copies of defs/uses, they will be modified by spills
	MyLinkedList<BaseInstruction *> *defListCopy = new MyLinkedList<BaseInstruction *>(mc->getDefList());
	MyLinkedList<BaseInstruction *> *useListCopy = new MyLinkedList<BaseInstruction *>(mc->getUseList());

	// first, insert store instructions for spilling after defs
	MyLinkedListIterator<BaseInstruction *> *defIter = defListCopy->elementsForward();
	///MyLinkedListIterator<BaseInstruction *> *defIter = mc->defListIteratorForw();
	while (defIter->hasMoreElements()) {
		NormalInstr *defInstr = (NormalInstr *) defIter->currentElement();
		defIter->nextElement();

		//if (debugRA)
		//	defInstr->shortPrint(cout);

		BaseArgument *defArg = IRHelper::getDefOfArgInInstr(argID, defInstr);
		
		// check for doubles
		BaseArgument *sibArg = IRHelper::getSiblingArg(defArg);

		// don't do anything for doubles unless processDoubles is true
		if (sibArg && !processDoubles)
			continue;

		resetDUProperty(defArg);

		// create a new multichain for the current argument being spilled
		///MultiChain *newMC = new MultiChain(argID);
		///newMC->setSpilled();
		///newMCs->add(newMC);
		///mc->removeDef(defInstr);
		//mc->setSpilled();

		MultiChain *sibMC = NULL;

		if (sibArg) {

			resetDUProperty(sibArg);

			// create a new multichain for the current argument being spilled
			///int sibID = IRHelper::getIDOfArg(sibArg);
			///sibMC = new MultiChain(sibID);
			///sibMC->setSpilled();
			//newMC->setSiblingMC(sibMC);
			//sibMC->setSiblingMC(newMC);
			//siblingMC->removeDef(defInstr);
			//siblingMC->setSpilled();
		}
		
		// insert spill instr just after defInstr
		if (sibArg)
			insertSpillOfArgAfterInstr(arg, defInstr, mc, freeSpillMemoryStart, siblingArg, siblingMC);
		else
			insertSpillOfArgAfterInstr(arg, defInstr, mc, freeSpillMemoryStart);
			
	}
	delete defIter;
	
	// next, insert load instructions for reloading before uses
	MyLinkedListIterator<BaseInstruction *> *useIter = useListCopy->elementsForward();
	///MyLinkedListIterator<BaseInstruction *> *useIter = mc->useListIteratorForw();
	while (useIter->hasMoreElements()) {
		NormalInstr *useInstr = (NormalInstr *) useIter->currentElement();
		useIter->nextElement();

		BaseArgument *firstSibArg = NULL;

		//if (debugRA)
		//	useInstr->shortPrint(cout);

		// for all uses
		MyLinkedList<BaseArgument *> *useArgList = IRHelper::getUsesOfArgInInstr(argID, useInstr);
		MyLinkedListIterator<BaseArgument *> *useArgListIter = useArgList->elementsForward();
		while (useArgListIter->hasMoreElements()) {
			BaseArgument *useArg = useArgListIter->currentElement();

			// check for doubles
			BaseArgument *sibArg = IRHelper::getSiblingArg(useArg);

			if (sibArg) {
				if (firstSibArg == NULL) {
					firstSibArg = sibArg;
				}
				if (!processDoubles) {
					break;
				} else {
					resetUDProperty(sibArg);
				}
			}

			resetUDProperty(useArg);

			useArgListIter->nextElement();
		}
		delete useArgListIter;	
		delete useArgList;

		if (firstSibArg && !processDoubles) {
			continue;
		}

		// create a new multichain for the current argument being spilled
		///MultiChain *newMC = new MultiChain(argID);
		///newMC->setSpilled();
		///newMCs->add(newMC);
		///mc->removeUse(useInstr);
		//mc->setSpilled();

		MultiChain *sibMC = NULL;

		if (firstSibArg) {

			// create a new multichain for the current argument being spilled
			///int sibID = IRHelper::getIDOfArg(firstSibArg);
			///sibMC = new MultiChain(sibID);
			///sibMC->setSpilled();
			///newMC->setSiblingMC(sibMC);
			///sibMC->setSiblingMC(newMC);
			///siblingMC->removeUse(useInstr);
			//siblingMC->setSpilled();
		}

		// insert reload instr just before useInstr
		if (firstSibArg)
			insertReloadOfArgBeforeInstr(arg, useInstr, mc, freeSpillMemoryStart, siblingArg, siblingMC);
		else
			insertReloadOfArgBeforeInstr(arg, useInstr, mc, freeSpillMemoryStart);

	}
	delete useIter;
	
	delete defListCopy;
	delete useListCopy;

	delete arg;
	if (siblingArg)
		delete siblingArg;

	freeSpillMemoryStart += SPILL_MEMORY_BYTES;

	///return newMCs;
	return NULL;
}

/* This function is just same as createNewSSANumForArg.
The only difference being that this does not assign a new SSANum to the new register.
*/ 
int getNewSSANumForArg(BaseArgument *arg)
{
	int symIndex = globalSymbolTable->getIndex(arg);
	int numSSAVars = globalSymbolTable->getNumVars(symIndex);
	numSSAVars++;
	int newSSANum = numSSAVars;

	globalSymbolTable->addNewRegs(symIndex, numSSAVars);
	globalSymbolTable->setStartNums();

	SSAArgProperty *ssaProp = (SSAArgProperty *) arg->getPtrToProperty(_SSA_);

	return newSSANum;
}


int createNewSSANumForArg(BaseArgument *arg)
{
	int symIndex = globalSymbolTable->getIndex(arg);
	int numSSAVars = globalSymbolTable->getNumVars(symIndex);
	numSSAVars++;
	int newSSANum = numSSAVars;

	globalSymbolTable->addNewRegs(symIndex, numSSAVars);
	globalSymbolTable->setStartNums();

	SSAArgProperty *ssaProp = (SSAArgProperty *) arg->getPtrToProperty(_SSA_);
	ssaProp->setSSANum(newSSANum);

	return newSSANum;
}

int createNewSSANumForMC(MultiChain *mc)
{
	// first, get a list of all arguments in this multichain
	SetList<BaseArgument *> *args = mc->getAllArgs();

	// compute th number 
	int argID = mc->getID();
	int symIndex = globalSymbolTable->getIndex(argID);
	int numSSAVars = globalSymbolTable->getNumVars(symIndex);
	numSSAVars++;
	int newSSANum = numSSAVars;

	// allocate a new SSANum for this mc
	globalSymbolTable->addNewRegs(symIndex, numSSAVars);
	globalSymbolTable->setStartNums();

	int newID = globalSymbolTable->getStartNum(symIndex) + newSSANum - SSA_START_NUM;
	
	MyLinkedListIterator<BaseArgument *> *argIter = args->elementsForward();
	while (argIter->hasMoreElements()) {
		BaseArgument *arg = argIter->currentElement();

		SSAArgProperty *ssaProp = (SSAArgProperty *) arg->getPtrToProperty(_SSA_);
		ssaProp->setSSANum(newSSANum);

		argIter->nextElement();
	}
	delete argIter;

	mc->setID(newID);

	delete args;

	return newID;
}

void addMCToIG(MultiChain *newMC, InterferenceGraph& ig) {
	// create entries in the symbol table for the new MCs
	int mcID = createNewSSANumForMC(newMC);
	
	//if (debugRA)
	//	currRoutine->print();
	
	// add newMC to the interference graph
	ig.setMC(mcID, newMC);
	ig.addMultiChain(newMC);
	
	// add interferences between the new MCs and the physical registers
	ig.addMCArchInterferences(ig.getMCNode(newMC));
	
	//if (debugRA)
		//ig.printMCCompatEdges(cout);
}

// if processDoubles is true, it spills both values of a double arg,
// otherwise is doesn't spill any
void spillMultiChain(MultiChain *mc, bool processDoubles = false)
{	
	
	if (mc->getUseList().isEmpty()) { // we do not spill variables that are just defined not used
		if (!mc->isDoubleFloatArg() || (processDoubles == true)) {
			cout << "MULTICHAIN: " << *mc << endl;
			ASSERT_TRACE(0, "We do not spill a variable that is never used");
			return;
		}
	}
	
	if (mc->isSpilled()) {
		cout << "MULTICHAIN: " << *mc << endl;
		ASSERT_TRACE(0, "Trying to spill an already spilled multichain");
		return;
	}

	// split mc into small MCs
	MyLinkedList<MultiChain *> *newMCs = insertSpillsReloads(mc, processDoubles);

	InterferenceGraph& ig = currRoutine->getIG();

	ig.getMCNode(mc)->resetMCAdjList();
	mc->computeRegClass();

	/*
	// for all new created MCs
	MyLinkedListIterator<MultiChain *> *mcIter = newMCs->elementsForward();
	while (mcIter->hasMoreElements()) {
		MultiChain *newMC = mcIter->currentElement();
		
		addMCToIG(newMC, ig);

		// if have a sibling, process it and create a double node
		MultiChain *sibMC = newMC->getSiblingMC();
		if (sibMC != NULL) {
		
			addMCToIG(sibMC, ig);

			// join nodes
			ig.makeDoubleMCNode(newMC, sibMC);
		}
		
		mcIter->nextElement();
	}
	delete mcIter;

	delete newMCs;
	*/
}

void spillNode(MCNode *node)
{
	InterferenceGraph& ig = currRoutine->getIG();

	if (debugRA) {
		//currRoutine->printHTG();
		//currRoutine->print();
	}

	if (debugRA) {
		cout << "Spilling node: " << node << endl;
	}

	saveReloads = true;

	if (node->isDoubleMCNode()) {

		MultiChain *firstMC = ((DoubleMCNode *) node)->getFirstNode()->getMC();
		MultiChain *secondMC = ((DoubleMCNode *) node)->getSecondNode()->getMC();

		if ((firstMC->getUseList().isEmpty()) && (secondMC->getUseList().isEmpty())) { // we do not spill variables that are just defined not used
			cout << "MULTICHAIN: " << *firstMC << " " << secondMC << endl;
			ASSERT_TRACE(0, "We do not spill a variable that is never used");
			return;
		}

		spillMultiChain(firstMC, true);  // process also double args
		//if (debugRA)
		//	currRoutine->print();

		spillMultiChain(secondMC);
		//if (debugRA)
		//	currRoutine->print();

		firstMC->setSpilled();
		secondMC->setSpilled();

		if (debugRA) {
			//currRoutine->printHTG();
			//currRoutine->print();
		}

		// delete old MCs
		//firstMC->removeUseDefList();
		//secondMC->removeUseDefList();
		//ig.removeMultiChain(firstMC);
		//ig.removeMultiChain(secondMC);
		//delete firstMC;
		//delete secondMC;

		///ig.removeDoubleNode((DoubleMCNode *) node);

	} else {
		
		if (node->isMetaMCNode()) {
			
			MyLinkedListIterator<MultiChain *> *mcListIter = ((MetaMCNode *) node)->getMCList().elementsForward();
			while (mcListIter->hasMoreElements()) {
				MultiChain *mc = mcListIter->currentElement();
				
				spillMultiChain(mc);
				//delete mc;
				mc->setSpilled();
				
				mcListIter->nextElement();
			}
			delete mcListIter;		
			
		} else {	
			
			saveReloads = true;
			MultiChain *mc = ((MCNode *) node)->getMC();
			spillMultiChain(mc);
			saveReloads = false;
			//processReloads();
			//delete mc;
			mc->setSpilled();
		}		
		
		if (debugRA) {
			//currRoutine->printHTG();
			//currRoutine->print();
		}

		// delete old mc
		//mc->removeUseDefList();
		//ig.removeMultiChain(mc);
		///ig.removeMCNode(node);
	}

	if (debugRA) {
		cout << "Node spilled..." << endl;
	}
	spills++;
}

