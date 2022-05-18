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
// SpillSupport.cpp
//
// File:  		SpillSupport.cpp
// Created:		Mon Jul 30, 2001
// Author:		Partha Biswas
// Email:		partha@ics.uci.edu, ilp@ics.uci.edu
//
// Routines to provide support for spilling 
// 

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
#include "RegArchModel.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

typedef map<MyString, MyString> stringToStringMapType;

stringToStringMapType GenericToTargetMap;
extern Routine *currRoutine;
extern int _LAST_FLOW_POS_;
extern int freeSpillMemoryStart;
extern RegArchModel globalRegModel;

// Types and variables for reloads saving
typedef struct {
	BaseOperation *oper;
	BaseInstruction *instr;
	MultiChain *mc;
} ReloadInfo;
Vector<ReloadInfo *> reloads;
bool saveReloads = false;

// Global function declarations
MyString getTargetSpillReloadInstr(MyString generic);
BaseOperation * createReloadOp(BaseArgument *, BaseArgument *);
BaseOperation * createSpillOp(BaseArgument *, BaseArgument *);
extern void addDoubleProperties(BaseArgument *arg1, BaseArgument *arg2);

const int FROM_SAME_MC = 7709;
const int NOT_FROM_SAME_MC = 7404;

void readSpillReloadFile()
{
	FILE *fp;
	char tempStr[200];
	char generic[10], target[10];

	if((fp = fopen("SpillReloadTarget.txt", "r")) == NULL )
	{
		ASSERT("Unable to open file spillReloadTarget.txt\n");
	}

	while(fgets(tempStr, 200, fp) != NULL)
	{
		if(strstr(tempStr, "spillReloadInstr") != NULL)
		{
			while(strstr(fgets(tempStr, 200, fp), "endSpillReloadInstr") == NULL)
			{
				strcpy(generic, strtok(tempStr, " =>"));
				// debug
				// cout << generic << endl;
				strcpy(target, strtok(NULL, " =>\n"));
				// debug
				// cout << target << endl;
				GenericToTargetMap[(MyString)generic] = (MyString)target;
			}
		}
	}
	fclose(fp);
}


MyString getTargetSpillReloadInstr(MyString generic)
{
	stringToStringMapType::iterator si;
	MyString temp = "ILLOP";
	
	while( (si = GenericToTargetMap.find(generic)) != GenericToTargetMap.end() )
	{
		return (*si).second;
	}

	return temp;
}

// Following routines are modified version of Radu's spills.cpp

// initialize reloadInstr with the actual load operation for reloading useArg
// returns newly created reload instruction

NormalInstr * createReloadInstr(BaseArgument *useArg, BaseArgument *sibArg = NULL, MultiChain *mc = NULL)
{
	BaseOperation *loadOp = createReloadOp(useArg, sibArg);

	BaseArgument *dstArg = ((MemoryOp *)loadOp)->ptrToDestOperand();

	// create the reload instruction here
	NormalInstr *reloadInstr = new NormalInstr();

	reloadInstr->initScheduleAndAdd(loadOp, true);
	currRoutine->oMapper->addOper(loadOp, reloadInstr);
	reloadInstr->setID(0);

	// Added by Radu, 09/05/2001
	// Save a list of all reloads introduced by spilling
	// for merging later
	// Also, added mc parameter, only used when counting reloads (NULL otherwise)
	if (saveReloads) {
		ReloadInfo *reloadInfo = new ReloadInfo();

		reloadInfo->oper = loadOp;
		reloadInfo->instr = reloadInstr;
		reloadInfo->mc = mc;

		reloads.push_back(reloadInfo);
	}

	// Added by SAH on 06/13/01.
	//
	// We also create a flow op and insert it into the instruction.
	//
	JumpOp tempJumpOp;
	int index = reloadInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	(reloadInstr->getRootOpSlot()).setNext(&(reloadInstr->getFlowOpSlot(index)));

	// create MultiChain and DU properties
	MultiChainProperty tmpMCProp;
	DUChainProperty tmpDUProp;

	
	dstArg->addProperty(tmpMCProp);
	dstArg->addProperty(tmpDUProp);

	// check for doubles
	BaseArgument *newSibArg = IRHelper::getSiblingArg(dstArg);

	if (newSibArg != NULL) {
		newSibArg->addProperty(tmpMCProp);
		newSibArg->addProperty(tmpDUProp);
	}

	return reloadInstr;
}

// create spillInstr with the actual store operation for spilling defArg
// returns newly created spill instruction

NormalInstr * createSpillInstr(BaseArgument * defArg, BaseArgument *sibArg = NULL)
{

	BaseOperation *storeOp = createSpillOp(defArg, sibArg);

	BaseArgument *srcRegArg = ((MemoryOp *)storeOp)->ptrToSourceOperand();

	NormalInstr * spillInstr = new NormalInstr();
	
	spillInstr->initScheduleAndAdd(storeOp, true);
	currRoutine->oMapper->addOper(storeOp, spillInstr);
	spillInstr->setID(0);

	// Added by SAH on 06/13/01.
	//
	// We also create a flow op and insert it into the instruction.
	//
	JumpOp tempJumpOp;
	int index = spillInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	(spillInstr->getRootOpSlot()).setNext(&(spillInstr->getFlowOpSlot(index)));

	// create MultiChain and UD properties
	MultiChainProperty tmpMCProp;
	UDChainProperty tmpUDProp;

	// add the multichain properties to the source register arg of storeOp

	
	srcRegArg->addProperty(tmpMCProp);
	srcRegArg->addProperty(tmpUDProp);

	// check for doubles
	BaseArgument *newSibArg = IRHelper::getSiblingArg(srcRegArg);

	if (newSibArg != NULL) {
		newSibArg->addProperty(tmpMCProp);
		newSibArg->addProperty(tmpUDProp);
	}

	return spillInstr;
}

void setTargetStoreOpcode(MyString & s, BaseArgument *memArg, BaseArgument *sibArg = NULL)
{
	if (memArg->isIntegerReg())
		s = getTargetSpillReloadInstr("IVSTORE"); // gets the target equivalent of IVSTORE.
	else
	{
		if (sibArg == NULL)
			s = getTargetSpillReloadInstr("FVSTORE");
		else
			s = getTargetSpillReloadInstr("DVSTORE"); // if (sibArg != NULL) => it is a double argument.
	}
	if( strcmp(s.getStr(), "ILLOP") == 0 )
		ASSERT("No target mapping found !\n");
}

void setTargetLoadOpcode(MyString & s, BaseArgument *memArg, BaseArgument *sibArg = NULL)
{
	if (memArg->isIntegerReg())
		s = getTargetSpillReloadInstr("IVLOAD"); // gets the target equivalent of IVLOAD.
	else
	{
		if (sibArg == NULL)
			s = getTargetSpillReloadInstr("FVLOAD");
		else
			s = getTargetSpillReloadInstr("DVLOAD"); // if (sibArg != NULL) => it is a double argument.
	}
	if( strcmp(s.getStr(), "ILLOP") == 0 )
		ASSERT("No target mapping found !\n");
}


BaseOperation *createStoreOp(BaseArgument *storeSource, BaseArgument *index, BaseArgument *offset, BaseArgument *sibArg = NULL)
{
	BaseOperation *retOp = new MemoryOp();

	// first initialize the opcode.
	//
	MyString opcodeStr;
	setTargetStoreOpcode(opcodeStr, storeSource, sibArg);
	OpCode opc(opcodeStr.getStr());
	((NormalOp *)retOp)->initOpCode(opc);

	// Next, add the destinations.

	// Finally add the sources
	//
	retOp->addOperand(storeSource, _SRC_3_, _SRC_LIST_);
	retOp->addOperand(index, _SRC_1_, _SRC_LIST_);
	retOp->addOperand(offset, _OFFSET_, _SRC_LIST_);

	if (sibArg) 
	{
		retOp->addOperand(sibArg, _DOUBLE_SRC_BEGIN_, _SRC_LIST_);
		addDoubleProperties(retOp->ptrToOperand(_SRC_3_, _SRC_LIST_),
							retOp->ptrToOperand(_DOUBLE_SRC_BEGIN_, _SRC_LIST_));
	}
	return retOp;
}

BaseOperation *createLoadOp(BaseArgument *loadDest, BaseArgument *index, BaseArgument *offset, BaseArgument *sibArg = NULL)
{
	BaseOperation *retOp = new MemoryOp();

	// first initialize the opcode.
	//
	MyString opcodeStr;
	setTargetLoadOpcode(opcodeStr, loadDest, sibArg);
	OpCode opc(opcodeStr.getStr());
	((NormalOp *)retOp)->initOpCode(opc);

	// Next, add the destinations.
	//
	retOp->addOperand(loadDest, _DST_, _DEST_LIST_);

	// Finally add the sources
	//
	retOp->addOperand(index, _SRC_1_, _SRC_LIST_);
	retOp->addOperand(offset, _OFFSET_, _SRC_LIST_);

	if (sibArg) 
	{
		retOp->addOperand(sibArg, _DOUBLE_DST_BEGIN_, _DEST_LIST_);
		addDoubleProperties(retOp->ptrToOperand(_DST_, _DEST_LIST_),
							retOp->ptrToOperand(_DOUBLE_DST_BEGIN_, _DEST_LIST_));
	}
	return retOp;
}


// returns a pointer to the newly created reload operation
BaseOperation * createReloadOp(BaseArgument * useArg, BaseArgument *sibArg = NULL)
{
	// Create the base-address-register argument.
	//
	BaseArgument *baseArg = new IConstArg(freeSpillMemoryStart);

	// Create the offset argument
	//
	BaseArgument *offsetArg = new IConstArg(0);

	BaseOperation *reloadOp = createLoadOp(useArg, baseArg, offsetArg, sibArg);

	reloadOp->setID(0);


// NEW VERSION : Works with RegArchModel
// #if 0
	globalRegModel.setRegClassPropertyForArg(((MemoryOp *)reloadOp)->ptrToDestOperand(), reloadOp);
// #endif
	
	delete baseArg;
	delete offsetArg;

	return reloadOp;
}

// initialize storeOp with the necessary arguments for spilling defArg
// returns the pointer to the newly created operation

BaseOperation * createSpillOp(BaseArgument *defArg, BaseArgument *sibArg = NULL)
{
	// Create the base-address-register argument.
	//
	BaseArgument *baseArg = new IConstArg(freeSpillMemoryStart);

	// Create the offset argument
	//
	BaseArgument *offsetArg = new IConstArg(0);

	BaseOperation *spillOp = createStoreOp(defArg, baseArg, offsetArg, sibArg);

	spillOp->setID(0);


// NEW VERSION : Works with RegArchModel
// #if 0
	globalRegModel.setRegClassPropertyForArg(((MemoryOp *)spillOp)->ptrToSourceOperand(), spillOp);
// #endif
	
	delete baseArg;
	delete offsetArg;

	return spillOp;
}

extern int equalRegs(BaseArgument *reg1, BaseArgument *reg2);

int registerReused(BaseArgument *reloadArg, BaseInstruction *instr, MultiChain *mc)
{
	int retVal = _NO_;

	// assume that all error checking has been done.

	// First look at all the defs in the instruction.
	//
	SetList<BaseArgument *> *defList = IRHelper::getAllDefsInInstr(instr);

	MyLinkedListIterator<BaseArgument *> *iter1 = defList->elementsForward();

	while (iter1->hasMoreElements())
	{
		if (equalRegs(reloadArg, iter1->currentElement()))
		{
			// found an argument that is the same as reloadArg. Now, see if that argument also exists
			// in the multi-chain.
			//
			if (mc->containsDef(instr)) // yes.
				retVal = FROM_SAME_MC;
			else
				retVal = NOT_FROM_SAME_MC;

			break;
		}
		iter1->nextElement();
	}
	delete iter1;
	delete defList;
	if (retVal != _NO_) return retVal;

	// Next look at all the uses in the instruction.
	//
	SetList<BaseArgument *> *useList = IRHelper::getAllUsesInInstr(instr);

	MyLinkedListIterator<BaseArgument *> *iter2 = useList->elementsForward();

	while (iter2->hasMoreElements())
	{
		if (equalRegs(reloadArg, iter2->currentElement()))
		{
			// found an argument that is the same as reloadArg. Now, see if that argument also exists
			// in the multi-chain.
			//
			if (mc->containsUse(instr)) // yes.
				retVal = FROM_SAME_MC;
			else
				retVal = NOT_FROM_SAME_MC;

			break;
		}
		iter2->nextElement();
	}
	delete iter2;
	delete useList;

	return retVal;
}

extern void markOp(BaseOperation *oper);
extern void deleteUselessIR(Routine *rout);
extern void initDCEStatsObject();
extern void displayDCEStats();

// go up the instruction list. if we encounter an operation that defines/uses the same argument,
// and if that operation is not present in the multichain mc, do not delete the reload op.
// if we encounter a join, do not delete the reload op.
//
void mergeReloads(BaseOperation *reloadOp, BaseInstruction *reloadInstr, MultiChain *mc)
{
	// assume that reloadop is the only operation in reload instr.
	//
	BaseInstruction *prevInstr = reloadInstr;
	int canDelOper = 0;

	// get a pointer to the store argument.
	//
	if ((reloadOp == NULL) || (!reloadOp->isMemoryOp())) // error condition ?
		return;

	if (!((MemoryOp *)reloadOp)->isLoad()) // error condition ?
		return;

	// now, get the argument corresponding to the reloaded register.
	//
	BaseArgument *arg = reloadOp->ptrToOperand(_DEST_, _DEST_LIST_);

	if ((arg == NULL) || (!arg->isRegisterArg())) // error condition ?
		return;

	while (1)
	{
		InstrList & prevList = currRoutine->iWalker->getNext(prevInstr);

		if (prevList.empty()) // error condition??
			break;

		if (prevList.size() > 1) // reached a join
			break;

		prevInstr = prevList.front();

		int temp = registerReused(arg, prevInstr, mc);
		if (temp == FROM_SAME_MC) // the reload op is redundant.
		{
			canDelOper = 1;
			break;
		}
		else if (temp == NOT_FROM_SAME_MC) // the reload op is necessary
		{
			break;
		}
		// else move to the previous operation.
	}

	if (canDelOper) // is a redundant reload.
	{
		markOp(reloadOp);
		mc->removeDef(reloadInstr);
	}
}

extern int doneRA;

void processReloads() {

	Vector<ReloadInfo *>::iterator it;

	// for purposes of merging reloads, set doneRA = 1;
	//
	int tempDoneRA = doneRA;
	doneRA = 1;

	for (it = reloads.begin(); it != reloads.end(); it++) {
		ReloadInfo *reloadInfo = (*it);

		mergeReloads(reloadInfo->oper, reloadInfo->instr, reloadInfo->mc);

		// free up the space
		delete reloadInfo;
	}

	initDCEStatsObject();
	deleteUselessIR(currRoutine);
	displayDCEStats();
	
	doneRA = tempDoneRA;

	// also need to maintain the Multichains by deleting them from the mc chains too.

	// delete the reloads
	reloads.clear();
}

