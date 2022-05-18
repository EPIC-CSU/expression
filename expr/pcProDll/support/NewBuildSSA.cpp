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
//
// File:  		NewBuildSSA.cpp
// Created:		Tue Nov 21, 00
// Last modified: 	Tue Nov 21, 00
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: MISC

#include "stdafx.h"

#include "STLIncludes.h"

#include "RegisterFileList.h"
#include "SymbolTable.h"
#include "Routine.h"
#include "BasicBlockIncludes.h"
#include "BBIterator.h"
#include "Program.h"
#include "JumpOp.h"
#include "ComputeOp.h"
#include "CallOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW:

// returns _YES_ if arc from predBB to bb is a loop back arc.
// else returns _NO_;
//
extern int isBackArcPred(BasicBlockNode *predBB, BasicBlockNode *bb);

extern Routine *currRoutine;
extern SymbolTable *globalSymbolTable;

int debugNewSSA = 0;

typedef struct VarToSSANumMapNode
{
	int uniqID;
	int latestSSANum;
} VarToSSANumMapNode;

int operator ==(VarToSSANumMapNode a, VarToSSANumMapNode b)
{
	if (a.uniqID == b.uniqID)
		return _YES_;

	return _NO_;
}

typedef SetList<VarToSSANumMapNode> VarSSAList;

typedef Map<BasicBlockNode *, VarSSAList> SSANumbersMap;

SSANumbersMap globalSSANumbersINSet;

SSANumbersMap globalMoveCandidatesSet;

Vector<BasicBlockNode *> listOfSSAizedBBs;

Deque<BasicBlockNode *> queueOfBBsToBeSSAized;

void setSymbolTable(BasicBlockNode *bb)
{
	if (bb == NULL) return;

	int uniqID, ssaNum;

	VarSSAList & list = globalSSANumbersINSet[bb];

	MyLinkedListIterator<VarToSSANumMapNode> *iter = list.elementsForward();

	while (iter->hasMoreElements())
	{
		uniqID = (iter->currentElement()).uniqID;
		ssaNum = (iter->currentElement()).latestSSANum;

        globalSymbolTable->setEntry(uniqID, ssaNum, -1);

		iter->nextElement();
	}
	delete iter;
}

int hasBeenSSAized(BasicBlockNode *bb)
{
	Vector<BasicBlockNode *>::iterator i;

	for (i = listOfSSAizedBBs.begin(); i != listOfSSAizedBBs.end(); i++)
	{
		if ((*i) == bb) return _YES_;
	}
	return _NO_;
}

void markAsSSAized(BasicBlockNode *bb)
{
	listOfSSAizedBBs.push_back(bb);
}

int canBeDestSSAized(BasicBlockNode *bb)
{
	BasicBlockNode *succBB;
	BasicBlockEdge *succEdge;

	succEdge = bb->getFirstEdge();

	if (succEdge != NULL)
	{
		if (!succEdge->isBackArc())
		{
			succBB = succEdge->getNext();

			if (!hasBeenSSAized(succBB)) 
				return _NO_;
		}
	}

	succEdge = bb->getNextEdge();

	if (succEdge != NULL)
	{
		if (!succEdge->isBackArc())
		{
			succBB = succEdge->getNext();

			if (!hasBeenSSAized(succBB)) 
				return _NO_;
		}
	}

	return _YES_;
}

int canBeSrcSSAized(BasicBlockNode *bb)
{
	BasicBlockNode *predBB;

	// get a list of its predecessors, and return _YES_ if all predecessors have been SSAized.
	//
	BasicBlockPredecessors & pList = bb->getCFPred();

	MyLinkedListIterator<BasicBlockNode *> *iter = pList.createPredListIterForw();

	if (iter == NULL) return _YES_;

	while (iter->hasMoreElements())
	{
		predBB = iter->currentElement();

		if (predBB == NULL)
		{
			iter->nextElement();
			continue;
		}

		if (!isBackArcPred(predBB, bb))
		{
			if (!hasBeenSSAized(predBB)) 
			{
				delete iter;
				return _NO_;
			}
		}
		iter->nextElement();
	}
	delete iter;

	return _YES_;
}

// adds the variable specified by the first argument into the list specified by the second argument.
//
// if the variable already exists, it overwrites the latestSSANumber to the one specified by the variable.
// Note that this is different from mergeIntoInSet which only writes the largest SSA number into the list.
//
void addIntoInSet(VarToSSANumMapNode & nd, VarSSAList & list)
{
	MyLinkedListIterator<VarToSSANumMapNode> *iter = list.elementsForward();

	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).uniqID == nd.uniqID)
		{
			(iter->currentElement()).latestSSANum = nd.latestSSANum;

			delete iter;
			return;
		}
		iter->nextElement();
	}
	delete iter;

	// Variable does not exist in IN set; append it.
	//
	list.append(nd);
}

// adds the variable specified by the first argument into the list specified by the second argument.
// 
// if the variable already exists, changes its latestSSANumber to the largest (or smallest) number.
//
void mergeIntoInSet(VarToSSANumMapNode & nd, VarSSAList & list, int largest)
{
	MyLinkedListIterator<VarToSSANumMapNode> *iter = list.elementsForward();

	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).uniqID == nd.uniqID)
		{
			if (largest)
			{
				if ((iter->currentElement()).latestSSANum < nd.latestSSANum)
					(iter->currentElement()).latestSSANum = nd.latestSSANum;
			}
			else 
			{
				if ((iter->currentElement()).latestSSANum > nd.latestSSANum)
					(iter->currentElement()).latestSSANum = nd.latestSSANum;
			}
			delete iter;
			return;
		}
		iter->nextElement();
	}
	delete iter;

	// Variable does not exist in IN set; append it.
	//
	list.append(nd);
}

// Adds the variables in the INset of frommerge to the INset of tomerge.
//
void mergeInSets(BasicBlockNode *tomerge, BasicBlockNode *frommerge, int largest)
{
	VarSSAList & flist = globalSSANumbersINSet[frommerge];
	VarSSAList & tlist = globalSSANumbersINSet[tomerge];
	
	MyLinkedListIterator<VarToSSANumMapNode> *iter = flist.elementsForward();

	while (iter->hasMoreElements())
	{
		mergeIntoInSet(iter->currentElement(), tlist, largest);

		iter->nextElement();
	}
	delete iter;
}

void addSSAProp(BaseArgument *arg, int ssaNum)
{
	SSAArgProperty SSAP;
	SSAArgProperty *pro =(SSAArgProperty *) (arg->getPtrToProperty(_SSA_));

	if (pro!=NULL) 
		pro->setSSANum(ssaNum);
	else {
		SSAP.setSSANum(ssaNum);
		arg->addProperty(SSAP);
	}
}

int getSSANumber(VarSSAList & vlist, int id)
{
	int retVal = _INVALID_;

	// Iterate over vlist and search for a node with uniqID == id.
	// If node exists return the latestSSANum. Else return 1.

	MyLinkedListIterator<VarToSSANumMapNode> *iter = vlist.elementsForward();

	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).uniqID == id)
		{
			retVal = (iter->currentElement()).latestSSANum;
			break;
		}
		iter->nextElement();
	}
	delete iter;

	if (retVal != _INVALID_) return retVal;

	// If reached here, implies that the id was not present in vlist.
	//
	return 1;
}

int getAndSetNextSSANumber(VarSSAList & vlist, int id)
{
	int retVal = _INVALID_;

	// Iterate over vlist and search for a node with uniqID == id.
	// If node exists, add 1 to latestSSANum and return latestSSANum.
	// If node does not exist, create node and initialize it to 2+1.

	MyLinkedListIterator<VarToSSANumMapNode> *iter = vlist.elementsForward();

	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).uniqID == id)
		{
			retVal = (iter->currentElement()).latestSSANum;
			(iter->currentElement()).latestSSANum += 1;
			break;
		}
		iter->nextElement();
	}
	delete iter;

	if (retVal != _INVALID_) return retVal;

	// If reached here, implies that the id was not present in vlist.
	// Create a new node.
	//
	VarToSSANumMapNode tmp; tmp.uniqID = id; tmp.latestSSANum = 3;
	vlist.append(tmp);
	return 2;
}

// first, iterate over the list of destination args.
// then, assign them the ssa numbers as derived from the IN set. 
// also update IN set (i.e. add 1).
//
void setDestSSANumbers(BaseOperation *oper, BasicBlockNode *bb)
{
	int uniqID, ssaNum;

	// iterate over the list of destination args.
	//
	ArgList & a = oper->destOperandList();
	ArgListIter dI;

	VarSSAList & vlist = globalSSANumbersINSet[bb];

	for (dI = a.begin(); dI != a.end(); dI++)
	{
		if ((*dI) != NULL)
		{
			if (!(*dI)->isRegisterArg()) // something's wrong. All dest args have to be registers.
			{
				// signal error.
				//
				cout << "Panic: Recieved a non register arg in destination. Aborting." << endl;
				assert(0);
			}

			// get the next ssa number for that particular variable from the vlist
			uniqID = ((RegisterArg*)(*dI))->getIDWithoutSSA();
			ssaNum = getAndSetNextSSANumber(vlist, uniqID);

			addSSAProp((*dI), ssaNum);
		}
	}
}

void setDestSSANumbers(BaseInstruction *instr, BasicBlockNode *bb)
{
	if (instr == NULL) return;

	// iterate over the list of operations in the instruction.
	// 
	// We assume that all operations in an instruction are independent.
	// Hence, we can traverse the instruction structure in any order.

	// First, iterate over the data op array.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			setDestSSANumbers((dIter->currentElement()).getPtrToOperation(), bb);
		}
		dIter->nextElement();
	}
	delete dIter;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			setDestSSANumbers((cIter->currentElement()).getPtrToOperation(), bb);
		}
		cIter->nextElement();
	}
	delete cIter;
}

void setDestSSANumbers(BasicBlockNode *bb)
{
	// Iterate backwards over each instruction in the basic-block.
	//
	// For each instruction, set the ssa numbers for destination args based on the IN set.
	//
	BBBackIterator bbIter(bb);

	while (bbIter.hasMoreElements())
	{
		setDestSSANumbers(bbIter.currentElement(), bb);

		bbIter.prevElement();
	}

	// mark as done.
	//
	listOfSSAizedBBs.push_back(bb);
}

void setInSetsOfPreds(BasicBlockNode *bb)
{
	BasicBlockNode *predBB;

	// get a list of its predecessors, and set (or add to) their IN sets.
	//
	BasicBlockPredecessors & pList = bb->getCFPred();

	MyLinkedListIterator<BasicBlockNode *> *iter = pList.createPredListIterForw();

	if (iter == NULL) return;

	while (iter->hasMoreElements())
	{
		predBB = iter->currentElement();

		if (!isBackArcPred(predBB, bb))
		{
			mergeInSets(predBB, bb, 1);
		}

		iter->nextElement();
	}
	delete iter;
}

void addDestEnabledNodesToQueue(BasicBlockNode *bb)
{
	BasicBlockNode *predBB;

	// get a list of its predecessors, and add those predecessors whose successors have all been SSAized.
	//
	BasicBlockPredecessors & pList = bb->getCFPred();

	MyLinkedListIterator<BasicBlockNode *> *iter = pList.createPredListIterForw();

	if (iter == NULL) return;

	while (iter->hasMoreElements())
	{
		predBB = iter->currentElement();

		if (predBB == NULL)
		{
			iter->nextElement();
			continue;
		}

		if (!isBackArcPred(predBB, bb))
		{
			if (hasBeenSSAized(predBB)) // error condition
			{
				// what's going on? No bb can be SSAized before it's successors have been.
				//
				cout << "Panic: Tried to SSAize a previously SSAized basic block" << endl;
				assert(0);
			}

			if (canBeDestSSAized(predBB))
				queueOfBBsToBeSSAized.push_back(predBB);
		}
		iter->nextElement();
	}
	delete iter;
}

Vector<BasicBlockNode *> terminalNodesList;

void addTerminalNodesToQueueOfBBsToBeSSAized()
{
	Vector<BasicBlockNode *>::iterator i;

	for (i = terminalNodesList.begin(); i != terminalNodesList.end(); i++)
	{
		queueOfBBsToBeSSAized.push_back(*i);
	}
}

// Start with the last Basic Block (i.e. one with no successors).
//
// Process a Basic Block only if all its successors have been processed.
// Do not consider back-arcs as leading to successors.

void setDestSSANumbers(Routine *rout)
{
	BasicBlockNode *currBB;

	// first add the list of terminal nodes from rout to queueOfBBsToBe SSAized.
	//
	addTerminalNodesToQueueOfBBsToBeSSAized();

	while (!queueOfBBsToBeSSAized.empty())
	{
		currBB = queueOfBBsToBeSSAized.front();
		queueOfBBsToBeSSAized.pop_front();

		// First, set the ssa numbers for operations inside the bb.
		//
		setDestSSANumbers(currBB);

		// Next, set the IN sets of all predecessor blocks.
		//
		setInSetsOfPreds(currBB);

		// Finally, add the bbs who have now been enabled to the queue.
		//
		addDestEnabledNodesToQueue(currBB);
	}

	// now, the start node of the CFG is the last node to be processed.
	// The VarSSAList of this node contains the max ssa number for each variable.
	//
	// set the symbol table based on this.
	//
	setSymbolTable(&((rout->getCFG()).getFirstBB()));
}

// type : INT or FLOAT or DOUBLE
//
BaseOperation *createMoveOp(int destSSANum, int regID)
{
	OpCode opCode;
	SSAArgProperty ssaP;
	BaseOperation *movOp;

	movOp = new ComputeOp();

	BaseArgument *arg = globalRegFileList->getRealReg(regID);
	
	if(arg->isRealRegArg())
	{
		char *regFileName;
		regFileName = strdup(((RealRegArg*)arg)->regFilename());
		if(strcmp(regFileName, "gR") == 0)
		{
			// type = _INT_;
			opCode.init("IASSIGN");
		}
		else if(strcmp(regFileName, "gf") == 0)
		{
			// type = _FLOAT_;
			opCode.init("FASSIGN");
		}
		else
		{
			opCode.init("IASSIGN");
		}
	}
	else
	{
		assert(0);
	}

	
	((ComputeOp *)movOp)->initOpCode(opCode);
	movOp->addOperand(arg, _DEST_, _DEST_LIST_);
	movOp->addOperand(arg, _SOURCE_1_);

	// Now add the ssa property to the destination operand.

	BaseArgument *dstArg = movOp->ptrToOperand(_DEST_, _DEST_LIST_);
	ssaP.setSSANum(destSSANum);
	dstArg->addProperty(ssaP);
  
	delete arg;

	return movOp;
}

void convertGotosToJumps(NormalInstr *instr)
{
	BaseOperation *oper, *tempOper;
	JumpOp jOp;

	ArrayIterator<FlowOpSlot> *iter = instr->getFlowOpSlotIter();

	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).hasOper())
		{
			oper = (iter->currentElement()).getPtrToOperation();

			if (oper->isGotoOp())
			{
				jOp.setTargetInstr(oper->getTargetInstr());

				tempOper = &(jOp);

				(iter->currentElement()).resetOperation();
				(iter->currentElement()).addOperation(tempOper);

				delete oper;
			}
		}
		iter->nextElement();
	}
	delete iter;
}

void removeLabel(NormalInstr *instr)
{
	if (instr == NULL) return;

	if (instr->hasLabel())
	{
		instr->deleteLabel();
	}
}

extern BasicBlockNode *insertDummyBBBetween(BasicBlockNode *currBB, BasicBlockNode *succBB, Routine *curR);

BasicBlockNode *checkAndSetNewBB(BasicBlockNode *currBB, BasicBlockNode *succBB)
{
	BasicBlockNode *retPtr = NULL;

	// if currBB has multiple succs (i.e. is an if)
	// and succBB has multiple preds (i.e. is a join)
	// then, add a dummy basic-block between the two.
	//
	// while counting preds for succBB, neglect all back arcs.
	// also if succBB is not the first succ of currBB, then we do not need to add a dummy basicblock.
	//
	if (succBB != currBB->getFirstSucc()) return retPtr;

	if ((succBB->getCFPred()).numItems() > 1)
	{
		MyLinkedListIterator<BasicBlockNode *> *iter = (succBB->getCFPred()).createPredListIterForw();

		int count = 0;
		while (iter->hasMoreElements())
		{
			if (!isBackArcPred(iter->currentElement(), succBB))
				count++;

			iter->nextElement();
		}
		delete iter;
		if (count <= 1) return retPtr;

		if ((currBB->getFirstSucc() != NULL) && (currBB->getNextSucc() != NULL))
		{
			retPtr = insertDummyBBBetween(currBB, succBB, currRoutine);

			mergeInSets(retPtr, currBB, 1);
		}
	}
	return retPtr;
}

extern int _LAST_FLOW_POS_;

void insertAsLastInstr(BaseOperation *movOp, BasicBlockNode *bb)
{
	NormalInstr *newInstr;
	JumpOp tempJumpOp;
	BaseOperation *headOp;
	
	int index;
		
	NormalInstr  *lastInstr = (NormalInstr *)bb->getLastInstr();
	
	if (lastInstr == NULL) // i.e. the basic block is a newly created block (or a dummy block)
	{
		newInstr = new NormalInstr();
		newInstr->initScheduleAndAdd(movOp);
		
		// Now add a flowop to the first instruction in the successor basic block.
		//
		JumpOp tempJumpOp;
	
		BaseInstruction *nextInstr = NULL;
		
		if (bb->getFirstSucc() != NULL)
		{
			nextInstr = (bb->getFirstSucc())->getFirstInstr();

			// we do not expect the bb to have two succs. 
			// So, make a check and flag an error if it does indeed have two succs.
			//
			if (bb->getNextSucc() != NULL) // error condition
			{
				cout << "Panic: Newly constructed bb (without any instrs) has two succs" << endl;
				assert(0);
			}
		}
		tempJumpOp.setTargetInstr(nextInstr);

		index = newInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
		(newInstr->getRootOpSlot()).setNext(&(newInstr->getFlowOpSlot(index)));

		// Next, we need to add a flow op from its predecessor bb's last instr to this instr.
		//
		BasicBlockPredecessors & bbPred = bb->getCFPred();

		if (bbPred.numItems() != 1) // error condition again.
		{
			cout << "Panic: Dummy bb has more than one predecessors" << endl;
			assert(0);
		}
		BaseInstruction *prevInstr = (bbPred.getFirstPred())->getLastInstr();

		if (prevInstr != NULL) // set its succ (true succ for "if" and fall-thru succ for others) to the first instr in this bb.
		{
			prevInstr->setNextInstr(newInstr);
		}

		// Finally, add this new instr to the current routine.
		//
		// We add it just before the nextInstr.
		//
		currRoutine->insertInstrBefore((BaseInstruction *)newInstr, nextInstr);
		
		bb->initLeader(newInstr);
		bb->initTrailer(newInstr);
		
		return;
	}
	
	// If the basic block is not a newly created block (or a dummy block)
	// then .....
	
	headOp = lastInstr->getPtrToRootOperation();
	
	if (!headOp->isFlowOp())
	{
		// If the last Instr contained a Goto, we do not want to retain the goto op.
		// This is becos later passes that reconstruct the cfg get confused by the
		// goto op.
		//
		// So, we iterator over the old and new instr and then convert any goto op to a corresponding
		// flow op.
		//
		// Also, if the last Instr contained a label, we want to remove the label.
		//
		convertGotosToJumps(lastInstr);
		
		newInstr = new NormalInstr(*lastInstr);
		
		convertGotosToJumps(newInstr);
		removeLabel(newInstr);
		
		currRoutine->insertInstrAfter(newInstr,lastInstr);
	}
	if (headOp->isComputeOp()||headOp->isMemoryOp()) 
	{
		//inserted instruction is inserted after the last instruction of BB
		//setup FlowOp for the old last instruction
		ArrayIterator<FlowOpSlot> *fIter = lastInstr->getFlowOpSlotIter();
		while (fIter->hasMoreElements()) {
			if (fIter->currentElement().hasOper()) {
				JumpOp *jOp = (JumpOp *)fIter->currentElement().getPtrToOperation();
				jOp->setTargetInstr(newInstr);
				break;
			}
			fIter->nextElement();
		}
		delete fIter;
		
		DataOpSlot& slot = (DataOpSlot&)newInstr->getRootOpSlot();
		slot.deleteOperation();
		
		// Now save pointer to the flow op slot that was pointed to by this slot.
		BaseOpSlot *flowSlot = slot.getPtrToNextSlot();
		
		// Now delete the next pointer of the slot (whose operation too was deleted).
		slot.setNext(NULL);
		
		newInstr->initScheduleAndAdd(movOp);
		
		(newInstr->getRootOpSlot()).setNext(flowSlot);
		bb->initTrailer(newInstr);
	}
	else if (headOp->isCallOp())
	{
		// Set the fall thru branch to the new instruction.
		
		BaseOpSlot & fslot = lastInstr->getFlowOpSlot(_LAST_FLOW_POS_);
		JumpOp *jOp = (JumpOp *)(fslot.getPtrToOperation());
		jOp->setTargetInstr(newInstr);
		
		// Now set the newInstr as the move operation.
		// Delete the old operation, clean up the flowops.
		
		ControlOpSlot & cslot = (ControlOpSlot&)newInstr->getRootOpSlot();
		cslot.deleteOperation();
		
		// Now save pointer to the flow op slot that was pointed to by this slot.
		BaseOpSlot *flowSlot = cslot.getPtrToFalseBranch();
		
		// Now reset the next pointer of the slot (whose operation too was deleted).
		cslot.setNext(NULL);
		
		// Also, reset the true branch.
		if ((cslot.getPtrToTrueBranch())->hasOper())
		{
			(cslot.getPtrToTrueBranch())->deleteOperation();
		}
		cslot.setTrueBranch(NULL);
		
		newInstr->initScheduleAndAdd(movOp);
		
		(newInstr->getRootOpSlot()).setNext(flowSlot);
		bb->initTrailer(newInstr);
	}
	else if (headOp->isIfOp())
	{
		//the inserted instruction is really inserted before the last instruction of BB
		//since the last instruction of BB is IF, there is no assignment to change the
		//value of some registers, doing so is same , and easy
		//remove old test argument from sameNameMap
				
		//insert the mov operation in the old last instruction
		lastInstr->removeAllOp();
		lastInstr->initScheduleAndAdd(movOp);
		
		//setup jump for old last instruction
		tempJumpOp.setTargetInstr(newInstr);
		index = lastInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
		(lastInstr->getRootOpSlot()).setNext(&(lastInstr->getFlowOpSlot(index)));
		
		bb->initTrailer(newInstr);
	}
	else if (headOp->isRetOp()){
		//the inserted instruction is really inserted before the last instruction of BB
		//since the last instruction of BB is RETURN, there is no assignment to change the
		//value of some registers, doing so is same , and easy
		//remove old return reg argument from sameNameMap
		
		//insert the mov operation in the old last instruction
		lastInstr->removeAllOp();
		lastInstr->initScheduleAndAdd(movOp);
		
		//setup jump for old last instruction
		tempJumpOp.setTargetInstr(newInstr);
		index = lastInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
		(lastInstr->getRootOpSlot()).setNext(&(lastInstr->getFlowOpSlot(index)));
		
		bb->initTrailer(newInstr);
	}
	else 
	{
		if (headOp->isFlowOp())
		{
			//		  cout << " Encountered FlowOp: " << headOp->isGotoOp() << endl;
			BaseOpSlot & flowSlot = lastInstr->getRootOpSlot();
			
			// DEBUG code.
			//
			/*
			cout << "Printing DEBUG Code in SSA" << endl;
			cout << endl;
			BaseOperation *op = flowSlot.getPtrToOperation();
			if (op != NULL)
			{
			op->shortPrint(cout);
			cout << endl;
			}
			cout << "*^**^**^**^**^**^**^**^**^**^**^**^**^**^**^**^**^*" << endl;
			//
			*/
			lastInstr->initScheduleAndAdd(movOp);
			lastInstr->getRootOpSlot().setNext(&(flowSlot));
		}
		else // headOp->isControlOp()
		{
			cout << " Encountered unknown type of operation" << endl;
			assert(0);
		}
	}
}

int isPresentInList(int uniqID, VarSSAList & list)
{
	// iterate over list. return 1 if uniqId exists in list.
	//
	MyLinkedListIterator<VarToSSANumMapNode> *iter = list.elementsForward();

	while (iter->hasMoreElements())
	{
		if (uniqID == (iter->currentElement()).uniqID)
		{
			delete iter;
			return _YES_;
		}
		iter->nextElement();
	}
	delete iter;

	return _NO_;
}

// A problem: 
// 
// What happens if two moves need to be added because the node has 2 successors (each with multiple preds)
// which both have a variable with different SSA numbering?
// In this case, the 2 moves should be added.
//
void addMoves(BasicBlockNode *currBB, BasicBlockNode *succBB, VarSSAList & inclusionList)
{
	BaseOperation *mvoper;

	// Compare the IN sets of currBB with succBB. 
	// For every variable in currBB with different ssa number from succBB IN set,
	// Add a move (with dest ssa number = succBB IN set number).
	//
	VarSSAList & currlist = globalSSANumbersINSet[currBB];
	VarSSAList & succlist = globalSSANumbersINSet[succBB];

	MyLinkedListIterator<VarToSSANumMapNode> *i1, *i2;
	i1 = currlist.elementsForward();

	while (i1->hasMoreElements())
	{
		VarToSSANumMapNode & elem = (i1->currentElement());

		i2 = succlist.elementsForward();

		while (i2->hasMoreElements())
		{
			VarToSSANumMapNode & cmpelem = (i2->currentElement());

			if (cmpelem.uniqID == elem.uniqID) // found a match.
			{
				if (isPresentInList(cmpelem.uniqID, inclusionList))
				{
					if (cmpelem.latestSSANum != elem.latestSSANum) // the ssa nums do not match. need to insert move
					{
						// create a move operation.
						//
						mvoper = createMoveOp(cmpelem.latestSSANum, elem.uniqID); 
						
						// next, insert move as last instr.
						//
						insertAsLastInstr(mvoper, currBB);

						// finally, change the latestSSANum in elem to incorporate the move destination.
						//
						elem.latestSSANum = cmpelem.latestSSANum;
					}
				}
			}
			i2->nextElement();
		}
		delete i2;

		i1->nextElement();
	}
	delete i1;
}

// A problem: 
// 
// What happens if two moves need to be added because the node has 2 successors (each with multiple preds)
// which both have a variable with different SSA numbering?
// In this case, the 2 moves should be added.
//
void addMoves(BasicBlockNode *currBB, BasicBlockNode *succBB)
{
	// First, check to see if we need to add a dummy basic block between currBB and succBB.
	//
	BasicBlockNode *nd = checkAndSetNewBB(currBB, succBB);

	if (nd != NULL)
		currBB = nd;

	BaseOperation *mvoper;

	// Compare the IN sets of currBB with succBB. 
	// For every variable in currBB with different ssa number from succBB IN set,
	// Add a move (with dest ssa number = succBB IN set number).
	//
	VarSSAList & currlist = globalSSANumbersINSet[currBB];
	VarSSAList & succlist = globalSSANumbersINSet[succBB];

	MyLinkedListIterator<VarToSSANumMapNode> *i1, *i2;
	i1 = currlist.elementsForward();

	while (i1->hasMoreElements())
	{
		VarToSSANumMapNode & elem = (i1->currentElement());

		i2 = succlist.elementsForward();

		while (i2->hasMoreElements())
		{
			VarToSSANumMapNode & cmpelem = (i2->currentElement());

			if (cmpelem.uniqID == elem.uniqID) // found a match.
			{
				if (cmpelem.latestSSANum != elem.latestSSANum) // the ssa nums do not match. need to insert move
				{
					// create a move operation.
					//
					mvoper = createMoveOp(cmpelem.latestSSANum, elem.uniqID); 
					
					// next, insert move as last instr.
					//
					insertAsLastInstr(mvoper, currBB);

					// finally, change the latestSSANum in elem to incorporate the move destination.
					//
					elem.latestSSANum = cmpelem.latestSSANum;
				}
			}
			i2->nextElement();
		}
		delete i2;

		i1->nextElement();
	}
	delete i1;
}

void setInSet(BaseOperation *oper, BasicBlockNode *bb)
{
	int uniqID, ssaNum;

	// iterate over the list of destination args.
	//
	ArgList & a = oper->destOperandList();
	ArgListIter dI;

	VarSSAList & vlist = globalSSANumbersINSet[bb];

	for (dI = a.begin(); dI != a.end(); dI++)
	{
		if ((*dI) != NULL)
		{
			if (!(*dI)->isRegisterArg()) // something's wrong. All dest args have to be registers.
			{
				// signal error.
				//
				cout << "Panic: Recieved a non register arg in destination. Aborting." << endl;
				assert(0);
			}

			// get the ssa number for this dest and add it to the vlist 
			//
			uniqID = ((RegisterArg*)(*dI))->getIDWithoutSSA();
			ssaNum = ((SSAArgProperty *)((*dI)->getPtrToProperty(_SSA_)))->getSSANum();

			VarToSSANumMapNode tmp; tmp.uniqID = uniqID; tmp.latestSSANum = ssaNum;
			addIntoInSet(tmp, vlist);
		}
	}
}

void setInSet(BaseInstruction *instr, BasicBlockNode *bb)
{
	if (instr == NULL) return;

	// iterate over the list of operations in the instruction.
	// 
	// We assume that all operations in an instruction are independent.
	// Hence, we can traverse the instruction structure in any order.

	// First, iterate over the data op array.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			setInSet((dIter->currentElement()).getPtrToOperation(), bb);
		}
		dIter->nextElement();
	}
	delete dIter;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			setInSet((cIter->currentElement()).getPtrToOperation(), bb);
		}
		cIter->nextElement();
	}
	delete cIter;
}

void setInSetAndAddMoves(BasicBlockNode *bb)
{
	// First, get the IN set of its predecessors.
	// If it has just one predecessor (not counting back-arcs) use this IN set as its starting IN set.
	//
	// If it has more than one predecessor, then try to merge the IN Sets together.
	// Then, if any predecessor's IN set is different from the merged set add moves to compensate.
	//

	BasicBlockNode *predBB;

	VarSSAList & vlist = globalSSANumbersINSet[bb];

	// get a list of its predecessors.
	//
	BasicBlockPredecessors & pList = bb->getCFPred();

	MyLinkedListIterator<BasicBlockNode *> *iter = pList.createPredListIterForw();


	if (iter != NULL) // also, check for the case when it has only one predecessor.
	{
		while (iter->hasMoreElements())
		{
			predBB = iter->currentElement();
			mergeInSets(bb, predBB, 0);

			iter->nextElement();
		}
		delete iter;

		// Now, go thru the list of predecessors. Compare the merged IN set with each predecessor's IN set.
		// Then, add moves if necessary.
		//
		iter = pList.createPredListIterForw();

		// While adding moves, we might also insert basic blocks which would change the pred list.
		// So, we create a separate list of all the predecessor bbs.
		//
		Vector<BasicBlockNode *> predVect;

		while (iter->hasMoreElements())
		{
			predBB = iter->currentElement();
			predVect.push_back(predBB);

			iter->nextElement();
		}
		delete iter;

		Vector<BasicBlockNode *>::iterator whyohwhy, ohmy;

		ohmy = predVect.end();
		for (whyohwhy = predVect.begin(); whyohwhy != ohmy; whyohwhy++)
		{
			addMoves((*whyohwhy), bb);
		}
	}

	// Moves have been added. Now, we can proceed with the business of going thru this basic block, 
	// and changing its IN set (due to the definitions in this basic block).
	//
	// Iterate forward over the list of instructions.
	//
	BBForwIterator newiter(bb);

	while (newiter.hasMoreElements())
	{
		setInSet(newiter.currentElement(), bb);

		newiter.nextElement();
	}

	// mark as done.
	//
	listOfSSAizedBBs.push_back(bb);
}

void setSrcSSANumbers(BaseOperation *oper, BasicBlockNode *bb)
{
	int uniqID, ssaNum;
	
	// iterate over the list of source args.
	//
	ArgList & b = oper->sourceOperandList();
	ArgListIter sI;

	VarSSAList & vlist = globalSSANumbersINSet[bb];

	for (sI = b.begin(); sI != b.end(); sI++)
	{
		if ((*sI) != NULL)
		{
			if ((*sI)->isRegisterArg()) 
			{
				// get the next ssa number for that particular variable from the vlist
				uniqID = ((RegisterArg*)(*sI))->getIDWithoutSSA();
				ssaNum = getSSANumber(vlist, uniqID);

				addSSAProp((*sI), ssaNum);
			}
		}
	}

	if (oper->isCallRetOp())
	{
		ArgList & al = ((CallRetOp *)oper)->getParmList();
		for (ArgListIter i = al.begin(); i != al.end(); i++)
		{
			if ((*i) !=NULL)
			{
				if ((*i)->isRegisterArg())
				{
					// get the next ssa number for that particular variable from the vlist
					uniqID = ((RegisterArg*)(*i))->getIDWithoutSSA();
					ssaNum = getSSANumber(vlist, uniqID);
					
					addSSAProp((*i), ssaNum);
				}
			}
		}
		
		ArgList & sl = ((CallRetOp *)oper)->getSavedRegList();
		for (ArgListIter j = sl.begin(); j != sl.end(); j++)
		{
			if ((*j) !=NULL)
			{
				if ((*j)->isRegisterArg())
				{
					// get the next ssa number for that particular variable from the vlist
					uniqID = ((RegisterArg*)(*j))->getIDWithoutSSA();
					ssaNum = getSSANumber(vlist, uniqID);
					
					addSSAProp((*j), ssaNum);
				}
			}
		}
	}
	
	// add the dest args to the IN Set.
	//
	ArgList & a = oper->destOperandList();
	ArgListIter dI;

	for (dI = a.begin(); dI != a.end(); dI++)
	{
		if ((*dI) != NULL)
		{
			if (!(*dI)->isRegisterArg()) // something's wrong. All dest args have to be registers.
			{
				// signal error.
				//
				cout << "Panic: Recieved a non register arg in destination. Aborting." << endl;
				assert(0);
			}

			// get the ssa number for this dest and add it to the vlist 
			//
			uniqID = ((RegisterArg*)(*dI))->getIDWithoutSSA();
			ssaNum = ((SSAArgProperty *)((*dI)->getPtrToProperty(_SSA_)))->getSSANum();

			VarToSSANumMapNode tmp; tmp.uniqID = uniqID; tmp.latestSSANum = ssaNum;
			addIntoInSet(tmp, vlist);
		}
	}
}

void setSrcSSANumbers(BaseInstruction *instr, BasicBlockNode *bb)
{
	if (instr == NULL) return;

	// iterate over the list of operations in the instruction.
	// 
	// We assume that all operations in an instruction are independent.
	// Hence, we can traverse the instruction structure in any order.

	// First, iterate over the data op array.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			setSrcSSANumbers((dIter->currentElement()).getPtrToOperation(), bb);
		}
		dIter->nextElement();
	}
	delete dIter;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			setSrcSSANumbers((cIter->currentElement()).getPtrToOperation(), bb);
		}
		cIter->nextElement();
	}
	delete cIter;
}

void setSrcSSANumbers(BasicBlockNode *bb)
{
	// Iterate forwards over each instruction in the basic-block.
	//
	// For each instruction, set the ssa numbers for src args based on the IN set.
	//
	BBForwIterator bbIter(bb);

	while (bbIter.hasMoreElements())
	{
		setSrcSSANumbers(bbIter.currentElement(), bb);

		bbIter.nextElement();
	}

	// set the in sets of the successors.
	//
	BasicBlockNode *succBB;

	succBB = bb->getFirstSucc();
	if (succBB != NULL)
	{
		mergeInSets(succBB, bb, 0);
	}

	succBB = bb->getNextSucc();
	if (succBB != NULL)
	{
		mergeInSets(succBB, bb, 0);
	}

	// mark as done.
	//
	listOfSSAizedBBs.push_back(bb);	
}

void removeFromMoveCandidatesSet(BaseArgument *arg, VarSSAList & list)
{
	if (arg == NULL) return;

	// get the uniqId from the arg.
	//
	if (!arg->isRegisterArg()) // something's wrong. 
	{
		// signal error.
		//
		cout << "Panic: Recieved a non register arg in destination. Aborting." << endl;
		assert(0);
	}

	int uniqID = ((RegisterArg*)(arg))->getIDWithoutSSA();

	// iterate over the list. If uniqIDs match, remove that element.
	//
	MyLinkedListIterator<VarToSSANumMapNode> *iter = list.elementsForward();

	while (iter->hasMoreElements())
	{
		if (uniqID == (iter->currentElement()).uniqID)
		{
			list.remove(iter->currentElement());
			break;
		}
		iter->nextElement();
	}
	delete iter;
}

void addToMoveCandidatesSet(int uniqID, VarSSAList & list)
{
	int needToAdd = 1;

	// iterate over the list. If uniqIDs match, do not add that element.
	//
	MyLinkedListIterator<VarToSSANumMapNode> *iter = list.elementsForward();

	while (iter->hasMoreElements())
	{
		if (uniqID == (iter->currentElement()).uniqID)
		{
			needToAdd = 0;
			break;
		}
		iter->nextElement();
	}
	delete iter;

	if (needToAdd)
	{
		VarToSSANumMapNode tmp; tmp.uniqID = uniqID; tmp.latestSSANum = _INVALID_;
		list.prepend(tmp);
	}
}

void addToMoveCandidatesSet(BaseArgument *arg, VarSSAList & list)
{
	if (arg == NULL) return;

	// get the uniqId from the arg.
	//
	if (!arg->isRegisterArg())  
		return;

	int uniqID = ((RegisterArg*)arg)->getIDWithoutSSA();

	addToMoveCandidatesSet(uniqID, list);
}

void mergeMoveCandidatesSets(VarSSAList & fromlist, VarSSAList & tolist)
{
	// iterate over fromlist. Add each element to tolist

	MyLinkedListIterator<VarToSSANumMapNode> *iter = fromlist.elementsForward();

	while (iter->hasMoreElements())
	{
		addToMoveCandidatesSet((iter->currentElement()).uniqID, tolist);

		iter->nextElement();
	}
	delete iter;
}

void mergeMoveCandidatesSets(BasicBlockNode *fromBB, BasicBlockNode *toBB)
{
	// get the move candidates sets for each bb.
	//
	VarSSAList & flist = globalMoveCandidatesSet[fromBB];
	VarSSAList & tlist = globalMoveCandidatesSet[toBB];

	mergeMoveCandidatesSets(flist, tlist);
}

void removeDestsFromMoveCandidatesSet(BaseOperation *oper, BasicBlockNode *bb)
{
	// iterate over the list of dests, and remove each dest from the move candidates list.

	VarSSAList & vlist = globalMoveCandidatesSet[bb];

	ArgList & a = oper->destOperandList();
	ArgListIter diter;

	for (diter = a.begin(); diter != a.end(); diter++)
	{
		removeFromMoveCandidatesSet(*diter, vlist);
	}
}

void addSrcsToMoveCandidatesSet(BaseOperation *oper, BasicBlockNode *bb)
{
	// iterate over the list of srcs, and add each src to the move candidates list.

	VarSSAList & vlist = globalMoveCandidatesSet[bb];

	ArgList & a = oper->sourceOperandList();
	ArgListIter siter;

	for (siter = a.begin(); siter != a.end(); siter++)
	{
		addToMoveCandidatesSet(*siter, vlist);
	}
}

void setMoveCandidates(BaseInstruction *instr, BasicBlockNode *bb)
{
	if (instr == NULL) return;

	// iterate over the list of operations in the instruction.
	// 
	// We assume that all operations in an instruction are independent.
	// Hence, we can traverse the instruction structure in any order.

	// First, iterate over the data op array.
	//
	ArrayIterator<DataOpSlot> *dIter;
	dIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			removeDestsFromMoveCandidatesSet((dIter->currentElement()).getPtrToOperation(), bb);
		}
		dIter->nextElement();
	}
	delete dIter;

	ArrayIterator<ControlOpSlot> *cIter;
	cIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			removeDestsFromMoveCandidatesSet((cIter->currentElement()).getPtrToOperation(), bb);
		}
		cIter->nextElement();
	}
	delete cIter;

	// First, iterate over the data op array.
	//
	dIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			addSrcsToMoveCandidatesSet((dIter->currentElement()).getPtrToOperation(), bb);
		}
		dIter->nextElement();
	}
	delete dIter;

	cIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			addSrcsToMoveCandidatesSet((cIter->currentElement()).getPtrToOperation(), bb);
		}
		cIter->nextElement();
	}
	delete cIter;
}

void setMoveCandidates(BasicBlockNode *bb)
{
	// iterate backwards over the bb, subtract all dests,and add all sources.

	BBBackIterator bbIter(bb);

	while (bbIter.hasMoreElements())
	{
		setMoveCandidates(bbIter.currentElement(), bb);

		bbIter.prevElement();
	}

	// mark as done.
	//
	listOfSSAizedBBs.push_back(bb);
}

void setMoveCandidatesOfPreds(BasicBlockNode *bb)
{
	BasicBlockNode *predBB;

	// get a list of its predecessors, and set (or add to) their IN sets.
	//
	BasicBlockPredecessors & pList = bb->getCFPred();

	MyLinkedListIterator<BasicBlockNode *> *iter = pList.createPredListIterForw();

	if (iter == NULL) return;

	while (iter->hasMoreElements())
	{
		predBB = iter->currentElement();

		if (!isBackArcPred(predBB, bb))
		{
			mergeMoveCandidatesSets(bb, predBB);
		}

		iter->nextElement();
	}
	delete iter;
}

int addMoveHeur = 1;

// bb is the loop header, and predBB is the loop tail.
// Insert moves to all the predecessors of loop header, 
// to ensure that all the destination vars have the same ssa number
//
void addMovesForLoops(BasicBlockNode *bb, BasicBlockNode *predBB)
{
	BasicBlockNode *currBB, *refBB;

	// First, get the list of move candidates.
	//

	// First get the set of all registers that have a use before a def in the loop.
	//
	VarSSAList & addOnlyIfList = globalMoveCandidatesSet[bb];

	// get a list of its predecessors.
	//
	BasicBlockPredecessors & pList = bb->getCFPred();
	MyLinkedListIterator<BasicBlockNode *> *iter = pList.createPredListIterForw();

	if (addMoveHeur == 1)
		refBB = predBB;
	else
		refBB = iter->currentElement();

	while (iter->hasMoreElements())
	{
		currBB = iter->currentElement();
		if (refBB != currBB)
			addMoves(currBB, refBB, addOnlyIfList);

		iter->nextElement();
	}
	delete iter;
}

void addMovesForLoops(BasicBlockNode *bb)
{
	BasicBlockNode *predBB;

	// get a list of its predecessors.
	//
	BasicBlockPredecessors & pList = bb->getCFPred();
	MyLinkedListIterator<BasicBlockNode *> *iter = pList.createPredListIterForw();

	// if atleast one of its predecessors is thru a back edge, then insert moves where necessary
	// to conform to the back arc predecessor.
	//
	if (iter != NULL)
	{
		while (iter->hasMoreElements())
		{
			predBB = iter->currentElement();

			if (isBackArcPred(predBB, bb)) // found a back arc. insert moves.
			{
				addMovesForLoops(bb, predBB);
				break;
			}
			iter->nextElement();
		}
		delete iter;
	}

	// mark as done.
	//
	listOfSSAizedBBs.push_back(bb);
}

void addSrcEnabledNodesToQueue(BasicBlockNode *bb)
{
	BasicBlockNode *succBB;
	
	// go thru its successors, and add those successors whose predecessors have all been SSAized.
	//
	succBB = bb->getFirstSucc();
	
	if (succBB != NULL)
	{
		if (!isBackArcPred(bb, succBB))
			if (canBeSrcSSAized(succBB))
				queueOfBBsToBeSSAized.push_back(succBB);
	}
	
	succBB = bb->getNextSucc();
	
	if (succBB != NULL)
	{
		if (!isBackArcPred(bb, succBB))
			if (canBeSrcSSAized(succBB))
				queueOfBBsToBeSSAized.push_back(succBB);
	}
}

void cleanUp()
{
	// empty all the lists, maps and queues.
	//
	globalSSANumbersINSet.clear();
	listOfSSAizedBBs.clear();
	queueOfBBsToBeSSAized.clear();
}

void setSrcSSANumbersAndAddMoves(Routine *rout)
{
	BasicBlockNode *currBB = &((rout->getCFG()).getFirstBB());

	queueOfBBsToBeSSAized.push_back(currBB);

	while (!queueOfBBsToBeSSAized.empty())
	{
		currBB = queueOfBBsToBeSSAized.front();
		queueOfBBsToBeSSAized.pop_front();

		// First, determine its IN set (from its predecessors).
		// If necessary, add moves in the predecessors.
		//
		setInSetAndAddMoves(currBB);

		// Finally, add the bbs who have now been enabled to the queue.
		//
		addSrcEnabledNodesToQueue(currBB);
	}

	if (debugNewSSA)
		rout->print();

	listOfSSAizedBBs.clear();
	queueOfBBsToBeSSAized.clear();
	globalMoveCandidatesSet.clear();

	addTerminalNodesToQueueOfBBsToBeSSAized();

	while (!queueOfBBsToBeSSAized.empty())
	{
		currBB = queueOfBBsToBeSSAized.front();
		queueOfBBsToBeSSAized.pop_front();

		setMoveCandidates(currBB);
		
		// Set the move candidates set of the predecessors.
		//
		setMoveCandidatesOfPreds(currBB);

		// Finally, add the bbs which have now been enabled to the queue.
		//
		addDestEnabledNodesToQueue(currBB);
	}

	if (debugNewSSA)
		rout->print();

	listOfSSAizedBBs.clear();
	queueOfBBsToBeSSAized.clear();
	currBB = &((rout->getCFG()).getFirstBB());
	queueOfBBsToBeSSAized.push_back(currBB);

	while (!queueOfBBsToBeSSAized.empty())
	{
		currBB = queueOfBBsToBeSSAized.front();
		queueOfBBsToBeSSAized.pop_front();

		addMovesForLoops(currBB);

		// Finally, add the bbs who have now been enabled to the queue.
		//
		addSrcEnabledNodesToQueue(currBB);
	}

	cleanUp();

	if (debugNewSSA)
		rout->print();

	currBB = &((rout->getCFG()).getFirstBB());
	queueOfBBsToBeSSAized.push_back(currBB);

	while (!queueOfBBsToBeSSAized.empty())
	{
		currBB = queueOfBBsToBeSSAized.front();
		queueOfBBsToBeSSAized.pop_front();

		// Next, set the ssa numbers of operations inside the bb.
		//
		setSrcSSANumbers(currBB);

		// Finally, add the bbs who have now been enabled to the queue.
		//
		addSrcEnabledNodesToQueue(currBB);
	}
}

void buildSSA(Routine *rout)
{
	if (rout == NULL) return;

	if (debugNewSSA)
		rout->print();

	currRoutine = rout;

	// First set the list of terminal nodes for the routine.
	//
	(rout->getCFG()).getListOfTerminalNodes(terminalNodesList);

	// Set SSA Numbers for destination args.
	//
	// Iterate backwards over the list of instructions (in Backwards CFG order).
	// Set destination ssa numbers for each operation.
	//
	setDestSSANumbers(rout);

	if (debugNewSSA)
		rout->print();

	cleanUp();

	// Now, set SSA Numbers for source args.
	// Also, add moves where necessary.
	//
	// Iterate forwards over the list of basic blocks.
	// Compute IN sets, and merge IN sets from the predecessors.
	// If IN set contains variables with different SSA numbers, add Moves where necessary.
	// Based on the IN sets, set the source ssa numbers
	// 
	setSrcSSANumbersAndAddMoves(rout);

	if (debugNewSSA)
		rout->print();

	// set the symbol table.
	//
    globalSymbolTable->setStartNums();


	// set the cfg structure correctly.
	// Note: This is needed becos some new basic blocks might have been added during RSSAization.
	//
	(rout->getCFG()).resetDFSNums();
	BinGraphDFS<ControlFlowGraph, BasicBlockNode, BasicBlockEdge> basicBlockDFS;
	basicBlockDFS.doDFS(rout->getCFG());

//		(iter->currentElement())->print();

	(rout->getCFG()).orderDFS();


	// finally, clean up.
	cleanUp();
	terminalNodesList.clear();
}

void buildSSA(Program *prog)
{
	if (prog == NULL) return;

	globalSymbolTable->initialize();

	MyLinkedListIterator<Routine *> *iter = prog->routineListIteratorForw();

	while (iter->hasMoreElements())
	{
		(iter->currentElement())->doPass("BUILD_SSA");
//		buildSSA(iter->currentElement());

		iter->nextElement();
	}
	delete iter;

//	globalSymbolTable->print(cout);
}