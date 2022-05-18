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
// $Id: InterferenceGraph.cpp,v 1.6 1998/03/21 01:18:26 radu Exp $
//
// File:  		InterferenceGraph.cpp
// Created:		Mon Oct 18, 99
// Last modified: 	Mon Oct 18, 99
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// This class implements the register interference graph
// 

#include "stdafx.h"

#ifdef WIN32
#pragma warning(disable:4786)
#endif

//#define FAST_MODE 1

#include "InterferenceGraph.h"
#include "STLIncludes.h"
#include "BaseInstruction.h"
#include "BaseArgument.h"
#include "ControlFlowGraph.h"
#include "Routine.h"
#include "BBIterator.h"
#include "IGNode.h"
#include "MyStack.h"
#include "SetList.h"
#include "RegNode.h"
#include "MultiChain.h"
#include "MCNode.h"
#include "DoubleMCNode.h"
#include "MetaMCNode.h"
#include "GToTRegMapProperty.h"
#include "IRTraversal.h"
#include "RoutParmProperty.h"
#include "RegisterMapping.h"
#include "IRHelper.h"
#include "Spills.h"
#include "RegCoalesce.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "RegisterFileList.h"

#include "RegClassArgProperty.h"

//#include "DebugSwitches.h"

#include "NoGUIMain.h"

//#include "DebugRA.h"

#include <fstream.h>
#include <float.h>
#include <algorithm>



//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: FAST_MODE

static int debugRA = 0;//debugRA_InterferenceGraph;
#define FAST_MODE	1

//bool allowRegisterReadWriteSameCycle = false;
bool serializeOperInInstr = false;

// In case of forwarding, when some reads are done at the end of the cycle,
// after the write
extern bool allowDelayedReads;

extern void print_RAproperties(PropertyNames prop);
extern void print_BBproperties(PropertyNames prop);
extern void print_LiveDeadProperties(Routine *rout = NULL);

extern RegisterMapping *globalRegisterMapping;
extern RegisterFileList *globalRegFileList;
extern SymbolTable* globalSymbolTable;
extern RegArchModel globalRegModel;
//extern int getIDOfArg(BaseArgument *arg);
//extern MyLinkedList<BaseArgument *> * getAllDefsInInstr(BaseInstruction *instr);
extern Routine *currRoutine;
extern void removeOldProps(Routine *routine, bool removeUseDef = false);
extern void print_RAproperties(PropertyNames prop);
//extern int nestingDepth(NormalInstr *instr);

InterferenceGraph::~InterferenceGraph()
{
	MyLinkedList<MultiChain *> *mcList = getMCList();
	MyLinkedListIterator<MultiChain *> *mcIter = mcList->elementsForward();

	while (mcIter->hasMoreElements()) {
		MultiChain *mc = mcIter->currentElement();

		delete mc;

		mcIter->nextElement();
	}
	delete mcIter;
	delete mcList;

	MyLinkedListIterator<MCNode *> *mcNodeIter = _mcNodes.elementsForward();

	while (mcNodeIter->hasMoreElements()) {
		MCNode *mcNode = mcNodeIter->currentElement();

		if (mcNode->isDoubleMCNode())
			delete mcNode;

		mcNodeIter->nextElement();
	}
	delete mcNodeIter;

	_iGraph.removeAllNodes();
}

char *regs[] = {

	"R0", "R1", "R2", "R3", "R4", "R5", "R6", "R7",
	"R8", "R9", "R10", "R11", "R12", "R13", "R14", "R15",
	"R16", "R17", "R18", "R19", "R20", "R21", "R22", "R23",
	"R24", "R25", "R26", "R27", "R28", "R29", "R30", "R31",

	"cc0", //"cc1", "cc2", "cc3",

	"sp0", //"sp1", //"sp2",

	"pc0",

	"f0", "f1", "f2", "f3", //"f4", "f5", "f6", "f7",
	"f8", "f9", "f10", "f11", "f12", "f13", "f14", "f15",
	"f16", "f17", "f18", "f19", "f20", "f21", "f22", "f23",
	"f24", "f25", "f26", "f27", "f28", "f29", "f30", "f31",

	"fp0",

	//"temp0", "temp1", "temp2", "temp3", "temp4", "temp5", "temp6", "temp7",
	//"temp8", "temp9", "temp10", "temp11", "temp12", "temp13", "temp14", "temp15",
	//"temp16", "temp17", "temp18", "temp19", "temp20", "temp21", "temp22", "temp23",
	//"temp24", "temp25", "temp26", "temp27", "temp28", "temp29", "temp30", "temp31",

	"hilo0", "hilo1"
};

//#define TESTING

void InterferenceGraph::addRegNodes()
{
	// commented for testing purposes

#ifdef TESTING

	// just for testing purposes
	char regName[10];
	for (int i = 0; i < sizeof(regs)/sizeof(char *); i++) {

		strcpy(regName, regs[i]);
		char *crt = regName;

		while (isalpha(*crt))
			crt++;

		int regNum;
		sscanf(crt, "%d", &regNum);

		regName[crt - regName] = '\0';
		

		int rfID = globalRegFileList->getBaseID(regName);

		RegNode *reg = new RegNode(rfID, regNum);
		addReg(reg);
	}

#else  // !TESTING

	/*
	ArrayIterator<RegisterFile> *rfIter = globalRegFileList->getRegFileListIter();
	while (rfIter->hasMoreElements()) {
		RegisterFile& rf = rfIter->currentElement();
		int rfID = rf.getID();

		for (int regNum = 0; regNum < rf.size(); regNum++) {
			RegNode *reg = new RegNode(rfID, regNum);
			addReg(reg);
		}

		rfIter->nextElement();
	}
	delete rfIter;
	*/

	RegFileVectorType regFileVector;
	globalRegFileList->getAllTargetRFs(regFileVector);

	RegFileVectorType::iterator it;
	for (it = regFileVector.begin(); it != regFileVector.end(); it++) {

		RegisterFile& rf = (*it);
		int rfID = rf.getID();

		for (int regNum = 0; regNum < rf.size(); regNum++) {
			RegNode *reg = new RegNode(rfID, regNum);
			addReg(reg);
		}
	}

#endif // TESTING

}

// Warning: does not update _mcNodes and _regNodes. Use the corresponding method for that
void InterferenceGraph::addNode(IGNode *node)
{
	_iGraph.addNode(node);
}

void InterferenceGraph::addInterference(IGNode *n1, IGNode *n2)
{
	if (n1 != n2)
		_iGraph.addEdge(n1, n2);
}

void InterferenceGraph::addMultiChain(MultiChain *mChain)
{
	MCNode *node = new MCNode(mChain);

	addMCNode(node);
	//addMCArchInterferences(node);
	//_mcToNodeMap[mChain] = node;
}

void InterferenceGraph::updateMCToNodeMap(MCNode *mcNode) 
{
	if (mcNode->isMetaMCNode()) {
		
		MyLinkedListIterator<MultiChain *> *mcListIter = ((MetaMCNode *)mcNode)->getMCList().elementsForward();
		while (mcListIter->hasMoreElements()) {
			MultiChain *mChain = mcListIter->currentElement();
			
			_mcToNodeMap[mChain] = mcNode;
			
			mcListIter->nextElement();
		}
		delete mcListIter;		
		
	} else {
		
		MultiChain *mChain = mcNode->getMC();
		_mcToNodeMap[mChain] = mcNode;
	}
}

void InterferenceGraph::addMCNode(MCNode *mcNode)
{
	addNode(mcNode);

	_mcNodes.add(mcNode);

	updateMCToNodeMap(mcNode);	
}

void InterferenceGraph::addReg(RegNode *regNode)
{
	// add the new node
	addNode(regNode);

	_regNodes.add(regNode);

	_indexToRegNodeMap[regNode->getIndex()] = regNode;

	regNode->setColor(_noOfColors);

	//if (debugRA)
	//	cout << "Color node " << *regNode << " with: " << _noOfColors << endl;

	_colorToRegMap[_noOfColors] = regNode;

	_noOfColors++;

	/* removed for now
	// add interferences w/ existing registers
	MyLinkedListIterator<RegNode *> *regIter = _regNodes.elementsForward();
	while (regIter->hasMoreElements()) {
		RegNode *reg = regIter->currentElement();
		
		if (reg != regNode)
			addInterference(reg, regNode);

		regIter->nextElement();
	}
	delete regIter;*/
}

DoubleMCNode * InterferenceGraph::makeDoubleMCNode(MCNode *firstMCNode, MCNode *secondMCNode)
{

	if (firstMCNode->isMetaMCNode() || secondMCNode->isMetaMCNode()) {
		ASSERT_TRACE(0, "We do not yet support MetaMCNodes inside a DoubleMCNode");
		return NULL;
	}

	MCNode *dblNode = new DoubleMCNode(firstMCNode, secondMCNode);
	_mcNodes.remove(firstMCNode);
	_mcNodes.remove(secondMCNode);
	_mcNodes.add(dblNode);

	return (DoubleMCNode *) dblNode;
}

DoubleMCNode * InterferenceGraph::makeDoubleMCNode(MultiChain *firstMC, MultiChain *secondMC)
{
	return makeDoubleMCNode(getMCNode(firstMC), getMCNode(secondMC));
}

MetaMCNode * InterferenceGraph::makeMetaMCNode(MCNode *firstMCNode, MCNode *secondMCNode)
{
	
	if (firstMCNode->isDoubleMCNode() || secondMCNode->isDoubleMCNode()) {
		ASSERT_TRACE(0, "We do not yet support DoubleMCNodes inside a MetaMCNode");
		return NULL;
	}

	if (firstMCNode == secondMCNode) {

		if (firstMCNode->isMetaMCNode()) {
			// metaMCNode was already created
			return (MetaMCNode *) firstMCNode;
		} else {
			
			ASSERT_TRACE(0, "Cannot create a MetaMCNode from two identical MCNodes");
			return NULL;
		}
	}
	
	if (firstMCNode->isMetaMCNode() ) {
		// just add the nodes from second one to first one and delete second meta node
		
		((MetaMCNode *) firstMCNode)->addMCNode(secondMCNode);
		updateMCToNodeMap(firstMCNode);

		_mcNodes.remove(secondMCNode);
		removeNode(secondMCNode);
		
		return (MetaMCNode *) firstMCNode;
	} else {

		if (secondMCNode->isMetaMCNode()) {

			((MetaMCNode *) secondMCNode)->addMCNode(firstMCNode);
			updateMCToNodeMap(secondMCNode);

			_mcNodes.remove(firstMCNode);
			removeNode(firstMCNode);

			return (MetaMCNode *) secondMCNode;

		} else {
			// create a new meta node and add both given nodes

			MCNode *metaNode = new MetaMCNode(firstMCNode);
			((MetaMCNode *) metaNode)->addMCNode(secondMCNode);

			addMCNode(metaNode);

			_mcNodes.remove(firstMCNode);
			removeNode(firstMCNode);

			_mcNodes.remove(secondMCNode);
			removeNode(secondMCNode);

			return (MetaMCNode *) metaNode;
		}
	}

	// should not reach this point!
	return NULL;
}

MetaMCNode * InterferenceGraph::makeMetaMCNode(MultiChain *firstMC, MultiChain *secondMC)
{
	return makeMetaMCNode(getMCNode(firstMC), getMCNode(secondMC));
}

void joinUDDUChainsForCopyOpArgs(BaseArgument *oldDefArg, int defArgID, BaseArgument *oldUseArg, int useArgID, BaseOperation *op)
{
    
    // iterate through the UD chain of oldUseArg
	UDChainProperty *oldUdProp = (UDChainProperty *) oldUseArg->getPtrToProperty(_UDCHAIN_);
	ASSERT_TRACE(oldUdProp, "Argument does not have a UD property");

	MyLinkedListIterator<BaseOperation*> *defListIter = oldUdProp->defListIteratorForw();
	while (defListIter->hasMoreElements()) {
		BaseOperation *defOp = defListIter->currentElement();

		BaseArgument *defArg = IRHelper::getDefOfArgInOper(useArgID, defOp);
		ASSERT_TRACE(defArg, "Cannot find definition of arg in operation");
		DUChainProperty *duProp = (DUChainProperty *) defArg->getPtrToProperty(_DUCHAIN_);
		ASSERT_TRACE(duProp, "Argument does not have a DU property");

		duProp->remove(op);

		// we now have the defs, next we'll iterate trough the DU chain of oldDefArg
		DUChainProperty *oldDuProp = (DUChainProperty *) oldDefArg->getPtrToProperty(_DUCHAIN_);
		// we may have definition that are never used
		if (oldDuProp == NULL) {
			defListIter->nextElement();
			continue;
		}
		ASSERT_TRACE(oldDuProp, "Argument does not have a DU property");

		MyLinkedListIterator<BaseOperation*> *useListIter = oldDuProp->useListIteratorForw();
		while (useListIter->hasMoreElements()) {
			BaseOperation *useOp = useListIter->currentElement();

			// we don't want to add the op that is to be removed
			// this can happen in a loop, where there is a cycle (def and use in the same op,
			// connected through a DU/UD chain)
			if (useOp == op) {
				useListIter->nextElement();
				continue;
			}

			duProp->addUse(useOp);

			MyLinkedList<BaseArgument *> *useArgList = IRHelper::getUsesOfArgInOper(defArgID, useOp);
			ASSERT_TRACE(!useArgList->isEmpty(), "Cannot find uses of arg in operation");
			MyLinkedListIterator<BaseArgument *> *useArgListIter = useArgList->elementsForward();
			while (useArgListIter->hasMoreElements()) {
				BaseArgument *useArg = useArgListIter->currentElement();

				UDChainProperty *udProp = (UDChainProperty *) useArg->getPtrToProperty(_UDCHAIN_);
				ASSERT_TRACE(udProp, "Argument does not have a UD property");

				udProp->remove(op);

				udProp->addDef(defOp);

				useArgListIter->nextElement();
			}
			delete useArgListIter;
			delete useArgList;

			useListIter->nextElement();
		}
		delete useListIter;

		defListIter->nextElement();
	}
	delete defListIter;

}

/** Coalesces two multichains, defMC with useMC (into useMC), and updates the UD-DU chains
 ** of the arguments linked to the ones in the move operation op (instruction instr)
 **/
MultiChain * InterferenceGraph::coalesceMCs(MultiChain *defMC, MultiChain *useMC, BaseOperation *op, NormalInstr *instr, bool keepUDDU)
{
	if (defMC->isDoubleFloatArg() || useMC->isDoubleFloatArg()) {
		ASSERT_TRACE(0, "We do not yet support coalescing double floats");
		return NULL;
	}

	// find the old def argument in the move operation
	int defArgID = defMC->getID();
	BaseArgument *oldDefArg = IRHelper::getDefOfArgInOper(defArgID, op);

	// find the old use argument (first if more than one) in the move operation
	int useArgID = useMC->getID();
	MyLinkedList<BaseArgument *> *oldUseArgList = IRHelper::getUsesOfArgInOper(useArgID, op);
	BaseArgument *oldUseArg = oldUseArgList->getHead();
	delete oldUseArgList;

    // update UDDU to reflect that the copy op was removed
	if (!keepUDDU)
		joinUDDUChainsForCopyOpArgs(oldDefArg, defArgID, oldUseArg, useArgID, op);

	// the UD and DU chains are updated, now change all arguments in defMC to have same argID as the ones in useMC (useArgID)
	
	// def arguments
	MyLinkedListIterator<BaseInstruction *> *defIter = defMC->defListIteratorForw();
	while (defIter->hasMoreElements()) {
		NormalInstr *defInstr = (NormalInstr *) defIter->currentElement();
		
		BaseArgument *defArg = IRHelper::getDefOfArgInInstr(defArgID, defInstr);

		// change defArg to have an ID of useArgID
		IRHelper::changeRegWithSSA(defArg, oldUseArg);
		
		defIter->nextElement();	
	}
	delete defIter;
	
	// use arguments
 	MyLinkedListIterator<BaseInstruction *> *useIter = defMC->useListIteratorForw();
	while (useIter->hasMoreElements()) {
		NormalInstr *useInstr = (NormalInstr *) useIter->currentElement();

		// for all uses
		MyLinkedList<BaseArgument *> *useArgList = IRHelper::getUsesOfArgInInstr(defArgID, useInstr);
		MyLinkedListIterator<BaseArgument *> *useArgListIter = useArgList->elementsForward();
		while (useArgListIter->hasMoreElements()) {
			BaseArgument *useArg = useArgListIter->currentElement();

			// change useArg to have an ID of useArgID
			IRHelper::changeRegWithSSA(useArg, oldUseArg);

			useArgListIter->nextElement();
		}
		delete useArgListIter;	
		delete useArgList;

		useIter->nextElement();
	}
	delete useIter;

	// add all defs/uses from defMC to useMC
	useMC->addDefList(defMC->getDefList());
	useMC->addUseList(defMC->getUseList());

	// move all interferences from defMC to useMC
	MCNode *defMCNode = getMCNode(defMC);
	MCNode *useMCNode = getMCNode(useMC);

	MyLinkedList<IGNode *> & defMCAdjNodeList = defMCNode->getAdjNodeList();
	MyLinkedListIterator<IGNode *> *defMCAdjNodeListIter = defMCAdjNodeList.elementsForward();
	while (defMCAdjNodeListIter->hasMoreElements()) {
		IGNode *adjNode = defMCAdjNodeListIter->currentElement();
		defMCAdjNodeListIter->nextElement();

		removeInterference(adjNode, defMCNode);
		addInterference(adjNode, useMCNode);
	}
	delete defMCAdjNodeListIter;

	// delete defMC from IG
	removeMCNode(getMCNode(defMC));

	useMC->resetSpilled();

	return useMC;
}

int InterferenceGraph::existInterference(MCNode *firstMCNode, MCNode *secondMCNode)
{
	return _iGraph.existEdge(firstMCNode, secondMCNode);
}


int InterferenceGraph::existInterference(MultiChain *firstMC, MultiChain *secondMC)
{
	MCNode *node1 = _mcToNodeMap[firstMC];
	MCNode *node2 = _mcToNodeMap[secondMC];

	return existInterference(node1, node2);
}


void InterferenceGraph::addInterference(MultiChain *mc1, MultiChain *mc2)
{
	if (mc1 != mc2) {

		MCNode *node1 = _mcToNodeMap[mc1];
		MCNode *node2 = _mcToNodeMap[mc2];

		addInterference(node1, node2);
	}
}

// #if 0

//NEW VERSION, WORKS WITH THE NEW REGISTER ARCHITECTURE LEVEL
// add interferences between the multichain and the architected registers
// based on the register mapping
void InterferenceGraph::addMCArchInterferences(MultiChain *mChain)
{
	MCNode *mcNode = getMCNode(mChain);

	// find the list of compatible targets for the multichain's argument

	// create list of all args
	ArgList argList;

	// get the target register list that satisfy the classes found above
	Vector<int> regIndexList;
	globalRegModel.getRegsThatSatisfyAllMappings(_GENERIC_, mChain->getRegClass(), _TARGET_, regIndexList);

	if (regIndexList.empty()) {
		cout << "MULTICHAIN: " << *mChain << endl;
		ASSERT_TRACE(0, "No compatible target registers found!!");
	}

	// go through all physical register nodes
	MyLinkedListIterator<RegNode *> *regIter = _regNodes.elementsForward();
	while (regIter->hasMoreElements()) {
		RegNode *reg = regIter->currentElement();

		// if the argument is not compatible with the register, add an interference
		if (std::find(regIndexList.begin(), regIndexList.end(), reg->getIndex()) == regIndexList.end()) {

			addInterference(mcNode, reg);
		}

		regIter->nextElement();
	}
	delete regIter;
}

// #endif  // for #if 0



#if 0
// add interferences between the multichain and the architected registers
// based on the register mapping
void InterferenceGraph::addMCArchInterferences(MultiChain *mChain)
{
	MCNode *mcNode = getMCNode(mChain);

	// find the list of compatible targets for the multichain's argument
	RegisterList *compatTargets;

	compatTargets = globalRegisterMapping->getCompatibleTargets(mChain);

	if (!compatTargets) {
		cout << "MULTICHAIN: " << *mChain << endl;
		ASSERT_TRACE(0, "No compatible target registers found!!");
	}

	MyLinkedListIterator<RegNode *> *regIter = _regNodes.elementsForward();
	while (regIter->hasMoreElements()) {
		RegNode *reg = regIter->currentElement();

		// if the argument is not compatible with the register
		if (!compatTargets->contains(reg->getIndex())) {

			addInterference(mcNode, reg);
		}

		regIter->nextElement();
	}
	delete regIter;
}
#endif //  for #if 0

// add interferences between the multichain and the architected registers
// based on the register mapping
void InterferenceGraph::addMCArchInterferences(MCNode *mcNode)
{
	if (mcNode->isMetaMCNode()) {

		MyLinkedListIterator<MultiChain *> *mcListIter = ((MetaMCNode *) mcNode)->getMCList().elementsForward();
		while (mcListIter->hasMoreElements()) {
			MultiChain *mc = mcListIter->currentElement();

			addMCArchInterferences(mc);

			mcListIter->nextElement();
		}
		delete mcListIter;

	} else {
	
		addMCArchInterferences(mcNode->getMC());
	}
}

void InterferenceGraph::removeNode(IGNode *node)
{
	_iGraph.removeNode(node);
	delete node;
}

void InterferenceGraph::removeInterference(IGNode *n1, IGNode *n2)
{
	_iGraph.removeEdge(n1, n2);
}

// DOES NOT remove the MC node too
void InterferenceGraph::removeMultiChain(MultiChain *mChain)
{
	_mcToNodeMap.erase(mChain);

	int regIndex = mChain->getRegIndex();
	int ssaNum = mChain->getSSANum();

	SSANumToMCMap & ssaNumMap = _idToMCMap[regIndex];
	ssaNumMap.erase(ssaNum);
}

// also REMOVES the multichain inside
void InterferenceGraph::removeMCNode(MCNode *mcNode)
{
	if (mcNode->isDoubleMCNode()) {
		
		removeDoubleNode((DoubleMCNode *) mcNode);

	} else {
		
		if (mcNode->isMetaMCNode()) {
			
			MyLinkedListIterator<MultiChain *> *mcListIter = ((MetaMCNode *) mcNode)->getMCList().elementsForward();
			while (mcListIter->hasMoreElements()) {
				MultiChain *mChain = mcListIter->currentElement();
				
				removeMultiChain(mChain);
				
				mcListIter->nextElement();
			}
			delete mcListIter;		
			
		} else {

			MultiChain *mChain = mcNode->getMC();
			removeMultiChain(mChain);
		}

		_iGraph.removeNode(mcNode);
		_mcNodes.remove(mcNode);

		delete mcNode;
	}
}

// also REMOVES the nodes / multichains inside
void InterferenceGraph::removeDoubleNode(DoubleMCNode *node)
{
	MCNode *firstMCNode = node->getFirstNode();
	MCNode *secondMCNode = node->getSecondNode();

	removeMCNode(firstMCNode);
	removeMCNode(secondMCNode);

	_mcNodes.remove(node);
	delete node;
}

//========================================================================
// InterferenceGraph::addDefInterference
//
// add all the interferences between defArg and live variables in bb
//========================================================================

void InterferenceGraph::addDefInterference(BaseArgument *defArg, SetList<int>& liveList) 
{
	int defID = IRHelper::getIDOfArg(defArg);
	
	MultiChain *defMC = getMC(defID);
	ASSERT_TRACE(defMC, "Could not find multichain");

    Vector<RegClassEntryNode2>& defRegClass = defMC->getRegClass();

	MyLinkedListIterator<int> *liveListIter = liveList.elementsForward();
	while (liveListIter->hasMoreElements()) {
		int liveID = liveListIter->currentElement();

		MultiChain *liveMC = getMC(liveID);
		ASSERT_TRACE(liveMC, "Could not find multichain");

		if (defMC != liveMC) {
            Vector<RegClassEntryNode2>& liveRegClass = liveMC->getRegClass();

            // add an interference only if the MCs compete for same registers
            if (globalRegModel.isCompatible(defRegClass[0], liveRegClass[0]))
			    addInterference(defMC, liveMC);
		}

		liveListIter->nextElement();
	}
	delete liveListIter;
}

//========================================================================
// InterferenceGraph::addDefInterferenceInsideInstr
//
// add all the interferences between defArg and args in useList that are not 
// contained in same operation, in instr
//========================================================================

void InterferenceGraph::addDefInterferenceInsideInstr(BaseArgument *defArg, SetList<BaseArgument *> *useList, NormalInstr *instr)
{
	int defID = IRHelper::getIDOfArg(defArg);
	
	MultiChain *defMC = getMC(defID);

	MyLinkedListIterator<BaseArgument *> *useListIter = useList->elementsForward();
	while (useListIter->hasMoreElements()) {
		BaseArgument *useArg = useListIter->currentElement();

		if (IRHelper::getOperationWithArgInInstr(defArg, instr) != IRHelper::getOperationWithArgInInstr(useArg, instr)) {
			
			int useID = IRHelper::getIDOfArg(useArg);

			MultiChain *useMC = getMC(useID);
			if (defMC != useMC) {
				addInterference(defMC, useMC);
			}
		}

		useListIter->nextElement();
	}
	delete useListIter;
}

MyLinkedList<IGNode *> * InterferenceGraph::getNodeList()
{
	return &_iGraph.getNodeList();
}

MyLinkedList<MultiChain *> * InterferenceGraph::getMCList()
{
	return _iGraph.getMCList();
}

void InterferenceGraph::setMCNode(MultiChain *mChain, MCNode *mcNode)
{
	_mcToNodeMap[mChain] = mcNode;
}

MCNode * InterferenceGraph::getMCNode(MultiChain *mc)
{
	if (_mcToNodeMap.find(mc) != _mcToNodeMap.end())
		return _mcToNodeMap[mc];
	else
		return NULL;
}

MultiChain * InterferenceGraph::getMC(int ID)
{
	int symIndex = globalSymbolTable->getIndex(ID);
	int ssaNum = ID - globalSymbolTable->getStartNum(symIndex) + SSA_START_NUM;

	return getMC(symIndex, ssaNum);
}

MultiChain * InterferenceGraph::getMC(int symIndex, int ssaNum)
{
	if (_idToMCMap.find(symIndex) != _idToMCMap.end()) {
		SSANumToMCMap & ssaNumToMCMap = _idToMCMap[symIndex];
		
		if (ssaNumToMCMap.find(ssaNum) != ssaNumToMCMap.end())
		return ssaNumToMCMap[ssaNum];
	}
	
	return NULL;
}

RegNode * InterferenceGraph::getRegNode(int index)
{
	if (_indexToRegNodeMap.find(index) != _indexToRegNodeMap.end())
		return _indexToRegNodeMap[index];
	else
		return NULL;
}

void InterferenceGraph::setMC(int ID, MultiChain *mc)
{
	int symIndex = globalSymbolTable->getIndex(ID);
	int ssaNum = ID - globalSymbolTable->getStartNum(symIndex) + SSA_START_NUM;

	setMC(symIndex, ssaNum, mc);
}

void InterferenceGraph::setMC(int symIndex, int ssaNum, MultiChain *mc)
{
	_idToMCMap[symIndex][ssaNum] = mc;
}

RegNode * InterferenceGraph::getRegOfColor(int color)
{
	if (_colorToRegMap.find(color) != _colorToRegMap.end())
		return _colorToRegMap[color];
	else
		return NULL;
}

void InterferenceGraph::reset()
{
	//_iGraph.removeAllNodes();
	//_regNodes.removeAll();

	MyLinkedList<IGNode *> *nodes = getNodeList();
	MyLinkedListIterator<IGNode *> *nodeIter = nodes->elementsForward();
	while (nodeIter->hasMoreElements()) {
		IGNode *node = nodeIter->currentElement();

		if (node->isMC()) {

			// delete all adj. MCs from adj. list
			MyLinkedList<IGNode *>& adjList = node->getAdjNodeList();
			MyLinkedListIterator<IGNode *> *adjIter = adjList.elementsForward();
			while (adjIter->hasMoreElements()) {
				IGNode *adjNode = adjIter->currentElement();

				adjIter->nextElement();

				if (adjNode->isMC()) {
					adjList.remove(adjNode);
				}
			}
			delete adjIter;

			//node->setColor(UNCOLORED);

			//MultiChain *mc = ((MCNode *) node)->getMC();
			//mc->getDefList().removeAll();
			//mc->getUseList().removeAll();
			//_iGraph.removeNode(node);
		}
		
		nodeIter->nextElement();
	}
	delete nodeIter;
	
	//_colorToRegMap.clear();
	//_mcToNodeMap.clear();
	//_idToMCMap.clear();

	//_noOfColors = 0;
}

void InterferenceGraph::resetColors()
{
	MyLinkedListIterator<MCNode *> *mcNodesIter = _mcNodes.elementsForward();
	while (mcNodesIter->hasMoreElements()) {
		MCNode *node = mcNodesIter->currentElement();

		node->setColor(UNCOLORED);
		if (node->isDoubleMCNode()) {
			((DoubleMCNode *) node)->getFirstNode()->setColor(UNCOLORED);
			((DoubleMCNode *) node)->getSecondNode()->setColor(UNCOLORED);
		}

		mcNodesIter->nextElement();
	}
	delete mcNodesIter;

}

//========================================================================
// InterferenceGraph::build 
//
// build the interference graph associated with routine
//========================================================================

void InterferenceGraph::build(Routine *routine)
{
	// add the edges (interferences between multichains)
	ControlFlowGraph& cfg = routine->getCFG();
	
	LinkedListIterator<BasicBlockNode> *bbListIter = cfg.createBBIterForw();

	while (bbListIter->hasMoreElements()) {
		BasicBlockNode& bb = bbListIter->currentElement();

		BasicBlockNode *temp = &bb;

		// _LIVEDEAD_ property at the exit of block
		LiveDeadBBProperty *ldProp = (LiveDeadBBProperty *) bb.getPtrToProperty(_LIVEDEAD_);
		
		// live variable list from the exit of the block
		// we'll have to compute and maintain the actual live variable list
		SetList<int>& liveList = ldProp->getLiveList();

		// now compute the live list at the start of BB = 
		//	union of live lists at end of previous BBs
		SetList<int> startLiveList;
		MyLinkedListIterator<BasicBlockNode *> *predIter = bb.getCFPred().createPredListIterForw();
		if (predIter) {
			while (predIter->hasMoreElements()) {
				BasicBlockNode *predBB = predIter->currentElement();

				LiveDeadBBProperty *predLDProp = (LiveDeadBBProperty *) predBB->getPtrToProperty(_LIVEDEAD_);
				SetList<int>& predLiveList = predLDProp->getLiveList();

				startLiveList.set_union(predLiveList);

				predIter->nextElement();
			}
			delete predIter;
		}

		BBBackIterator bbIter(&bb);
		while (bbIter.hasMoreElements()) {
			BaseInstruction *instr = bbIter.currentElement();

			// now the live list contains the live list at this point, except last uses

			/*
			if (!allowRegisterReadWriteSameCycle) {
				//next, add the last uses (actually all uses) to live list
				SetList<BaseArgument *> *uses = IRHelper::getAllUsesInInstr(instr);
				
				// now, take out all args in move ops (they are treated separately)
				SetList<BaseArgument *> *moveOpUses = IRHelper::getAllMoveOpUsesInInstr(instr);
				uses->minus(*moveOpUses);

				IRHelper::addArgsToLiveList(uses, liveList);

				delete uses;
			}
			*/

			// In case of forwarding (e.g. ST120)
			if (allowDelayedReads) {
				MyLinkedList<BaseArgument *> *uses = IRHelper::getAllDelayedReadsInInstr(instr);
				IRHelper::addArgsToLiveList(uses, liveList);
			}

			// so we can add the interferences with all defs in instr
			MyLinkedList<BaseArgument *> *defList = IRHelper::getAllDefsInInstr(instr);
			MyLinkedListIterator<BaseArgument *> *defIter = defList->elementsForward();

			while (defIter->hasMoreElements()) {
				BaseArgument *def = defIter->currentElement();
				defIter->nextElement();

				// we don't want to add interferences for variable that are just defined, not used
				//int defID = MultiChain::getIDOfArg(def);
				//MultiChain *defMC = getMC(defID);
				//if (defMC->getUseList().numItems() > 0)

				/// 04/01/2002 Commented out by RCC, it generates infinite spills in some cases
				//if (startLiveList.contains(IRHelper::getIDOfArg(def)))
				//	defList->remove(def); // was defined also before this bb
				
				addDefInterference(def, liveList);

				// added July 27, 2001 - takes care of def and uses in the same operation (don't add interferences)
				if (serializeOperInInstr) {
					// add interferences only between defs and uses in different operations from instr
					SetList<BaseArgument *> *uses = IRHelper::getAllUsesInInstr(instr);
					addDefInterferenceInsideInstr(def, uses, (NormalInstr *) instr);
					delete uses;
				}
			}
			delete defIter;

			// get all the defs in instruction and delete the variables from the live list
			IRHelper::delArgsFromLiveList(defList, liveList);
			delete defList;

			/*if (allowRegisterReadWriteSameCycle) {*/
				//next, add the last uses (actually all uses) to live list
				MyLinkedList<BaseArgument *> *uses;
				if (!allowDelayedReads)
					uses = IRHelper::getAllUsesInInstr(instr);
				else /* allowDelayedReads */
					uses = IRHelper::getAllNotDelayedReadsInInstr(instr);

				IRHelper::addArgsToLiveList(uses, liveList);

				delete uses;
			/*} else {
				
				// always allow read and write in same cycle for move operations
				MyLinkedList<BaseArgument *> *moveOpUses = IRHelper::getAllMoveOpUsesInInstr(instr);
				IRHelper::addArgsToLiveList(moveOpUses, liveList);
				delete moveOpUses;
			}*/
			
			
			bbIter.prevElement();
		}

		bbListIter->nextElement();
	}
	delete bbListIter;

	cleanupDoubleCrossInterferences();
}

void InterferenceGraph::cleanupDoubleCrossInterferences()
{
	// first, let's find all pairs of double nodes
	MyLinkedListIterator<MCNode *> *firstNodeIter = _mcNodes.elementsForward();
	while (firstNodeIter->hasMoreElements()) {
		MCNode *firstNode = firstNodeIter->currentElement();

		if (firstNode->isDoubleMCNode()) {

			// remove interference between nodes in double node
			removeInterference(((DoubleMCNode *) firstNode)->getFirstNode(),
							   ((DoubleMCNode *) firstNode)->getSecondNode());

			MyLinkedListIterator<MCNode *> *secondNodeIter = _mcNodes.elementsForward();
			while (secondNodeIter->hasMoreElements()) {
				MCNode *secondNode = secondNodeIter->currentElement();

				if (secondNode->isDoubleMCNode()) {
					
					// now we have two double nodes, let's remove the cross interferences
					removeInterference(((DoubleMCNode *) firstNode)->getFirstNode(),
									   ((DoubleMCNode *) secondNode)->getSecondNode());
					removeInterference(((DoubleMCNode *) firstNode)->getSecondNode(),
									   ((DoubleMCNode *) secondNode)->getFirstNode());

				}

				secondNodeIter->nextElement();
			}
			delete secondNodeIter;
		}

		firstNodeIter->nextElement();
	}
	delete firstNodeIter;
}

void InterferenceGraph::addMappingInterferences()
{
	MyLinkedListIterator<MCNode *> *mcNodesIter = _mcNodes.elementsForward();
	while (mcNodesIter->hasMoreElements()) {
		MCNode *mcNode = mcNodesIter->currentElement();

		if (mcNode->isDoubleMCNode()) {
			addMCArchInterferences(((DoubleMCNode *) mcNode)->getFirstNode());
			addMCArchInterferences(((DoubleMCNode *) mcNode)->getSecondNode());
		} else {
			addMCArchInterferences(mcNode);
		}

		mcNodesIter->nextElement();
	}
	delete mcNodesIter;
}

void InterferenceGraph::listNeighbours() 
{

	cout << "Neighbours: " << endl;

	MyLinkedList<IGNode *> *nodes = getNodeList();

	MyLinkedListIterator<IGNode *> *nodeIter = nodes->elementsForward();
	while (nodeIter->hasMoreElements()) {
		IGNode *n = nodeIter->currentElement();

		cout << "Node " << *n << " has ";
		cout << n->getNoOfAdjNodes() << " neighbours: " << endl << " ";

		MyLinkedList<IGNode *>& neighbours = n->getAdjNodeList();

		MyLinkedListIterator<IGNode *> *neighboursIter = neighbours.elementsForward();
		while (neighboursIter->hasMoreElements()) {
			IGNode *crt = neighboursIter->currentElement();

			cout << (*crt) << " ";

			neighboursIter->nextElement();
		}
		delete neighboursIter;

		cout << endl;

		nodeIter->nextElement();
	}
	delete nodeIter;
	delete nodes;
}

IGNode * InterferenceGraph::bestNodeToSpill()
{
	double bestCost = DBL_MAX;
	IGNode *bestNode = NULL;

	MyLinkedListIterator<MCNode *> *mcNodeIter = _mcNodes.elementsForward();
	while (mcNodeIter->hasMoreElements()) {
		MCNode *crtNode = mcNodeIter->currentElement();

			double cost;
			if ((cost = crtNode->computeSpillCost()) < bestCost) {
				bestCost = cost;
				bestNode = crtNode;
			}

		mcNodeIter->nextElement();
	}
	delete mcNodeIter;

	return bestNode;
}

int InterferenceGraph::colorMCNode(MCNode *mcNode) {
	MyLinkedList<IGNode *>& neighbours = mcNode->getAdjNodeList();
				
	MyLinkedListIterator<IGNode *> *neighboursIter = neighbours.elementsForward();
	
	// go through available colors, in preffered order
	//for (int i = 0; i < _noOfColors; i++) {
	for (int i = _noOfColors - 1; i >= 0; i--) {

		// don't use preferred order for now
		//int crtColor = regAllocPrefferedOrder[i];
		int crtColor = i;
		
		int canColor = 1;
		
		neighboursIter->reset();
		
		while (neighboursIter->hasMoreElements()) {
			IGNode *neighbourNode = neighboursIter->currentElement();
			
			if (neighbourNode->getColor() == crtColor)
				canColor = 0;
			
			neighboursIter->nextElement();
		}
		
		if (canColor) {
			
			if (debugRA)
				cout << "Color node " << mcNode << " with: " << crtColor << " (" << getRegOfColor(crtColor) << ")" << endl;
			
			mcNode->setColor(crtColor);

			delete neighboursIter;
			return crtColor;
		} 
	}
	delete neighboursIter;

	return UNCOLORED;
}

int InterferenceGraph::colorDoubleMCNode(DoubleMCNode *doubleMCNode) {
	
	MyLinkedList<IGNode *>& neighboursFirst = doubleMCNode->getFirstNode()->getAdjNodeList();
	MyLinkedList<IGNode *>& neighboursSecond = doubleMCNode->getSecondNode()->getAdjNodeList();
				
	MyLinkedListIterator<IGNode *> *neighboursFirstIter = neighboursFirst.elementsForward();
	MyLinkedListIterator<IGNode *> *neighboursSecondIter = neighboursSecond.elementsForward();
	
	for (int i = 0; i < (_noOfColors -1); i++) {
		
		// don't use a preferred order for now
		//int crtColor = regAllocPrefferedOrder[i];
		//int nextColor = regAllocPrefferedOrder[i + 1];

		int crtColor = i;
		int nextColor = i + 1;

		int canColor = 1;
		
		neighboursFirstIter->reset();
		neighboursSecondIter->reset();
		
		// try to color first node with crtColor
		while (neighboursFirstIter->hasMoreElements()) {
			IGNode *neighbourFirstNode = neighboursFirstIter->currentElement();
			
			if (neighbourFirstNode->getColor() == crtColor) {
				canColor = 0;
				break;
			}
			
			neighboursFirstIter->nextElement();
		}

		// try to color first node with (crtColor +1)
		if (canColor)
			while (neighboursSecondIter->hasMoreElements()) {
				IGNode *neighbourSecondNode = neighboursSecondIter->currentElement();
			
				if (neighbourSecondNode->getColor() == (nextColor)) {
					canColor = 0;
					break;
				}
			
				neighboursSecondIter->nextElement();
			}
		
		if (canColor) {
			
			if (debugRA) {
				cout << "Color double node:" << "[" << endl;
				cout << " Color node " << doubleMCNode->getFirstNode() << " with: " << crtColor << " (" << getRegOfColor(crtColor) << ")" << endl;
				cout << " Color node " << doubleMCNode->getSecondNode() << " with: " << crtColor + 1 << " (" << getRegOfColor(nextColor) << ")" << endl;
				cout << "]" << endl;
			}
			
			doubleMCNode->getFirstNode()->setColor(crtColor);
			doubleMCNode->getSecondNode()->setColor(nextColor);
			doubleMCNode->setColor(crtColor);

			delete neighboursFirstIter;
			delete neighboursSecondIter;
			return crtColor;
		} 
	}
	delete neighboursFirstIter;
	delete neighboursSecondIter;

	return UNCOLORED;
}

int registerCoalesceHeur = 1;

// Briggs' algorithm
void InterferenceGraph::color()
{

	MyStack<MCNode *> nodeStack;
	MyStack<MCNode *> nodesToSpill;

	int tryAgain;

	while (1) {
		
///// build phase
		if (debugRA)
			cout << "Building int graph - adding interferences" << endl;
		
		if (debugRA) {
			//cout << "IG:" << endl;
			printMCNodes(cout);
			//printEdges(cout);
		}

		// add interferences to ig
		build(currRoutine);

		// perform register coalescing
		if (registerCoalesceHeur)
			registerCoalescing(*this);
		
		if (debugRA) {
			cout << "IG:" << endl;
			printMCNodes(cout);
			printMCEdges(cout);
		}

        printStatistics(cout);

		int numRegs = _regNodes.numItems();
		
///// simplify phase

		while (_iGraph.numItems() > numRegs) {
			
			// try to remove as many MC nodes with degree < _noOfColors as possible
			tryAgain = 1;
			while (tryAgain) {
				tryAgain = 0;
				
				MyLinkedListIterator<MCNode *> *mcNodeIter = _mcNodes.elementsForward();
				while (mcNodeIter->hasMoreElements()) {
					MCNode *mcNode = mcNodeIter->currentElement();
					
					// should be here because next block modifies the list of nodes
					mcNodeIter->nextElement();
					
					if (mcNode->getNoOfAdjNodes() < _noOfColors) {
						
						if (debugRA)
							cout << "Pushing node: " << mcNode << " on stack..." << endl;
						
						// extract the node from IG
						if (mcNode->isDoubleMCNode()) {
							_iGraph.extractNode(((DoubleMCNode *) mcNode)->getFirstNode());
							_iGraph.extractNode(((DoubleMCNode *) mcNode)->getSecondNode());
						} else {
							_iGraph.extractNode(mcNode);
						}
						_mcNodes.remove(mcNode);
						nodeStack.push(mcNode);
			
						// we have just removed a node, try again, maybe others can be removed
						tryAgain = 1;
					}
				}
				delete mcNodeIter;
			}
			
			// all MC nodes have degree > _noOfColors, mark one node for spilling
			if (_iGraph.numItems() > numRegs) {

				if (debugRA)
					cout << "Cannot remove node ... nodes left: " << (_iGraph.numItems() - numRegs) << " ... find node for spilling" << endl;
					
				
				MCNode *mcNode = (MCNode *) bestNodeToSpill();
				
				ASSERT_TRACE(mcNode, "Cannot find a spill candidate...");

				if (debugRA) {
					cout << "Spill candidate for pushing to stack: " << mcNode << " (";
					cout << mcNode->getNoOfAdjNodes() << " neighbours)" << endl;
				}

				// extract the node from IG
				if (mcNode->isDoubleMCNode()) {
					_iGraph.extractNode(((DoubleMCNode *) mcNode)->getFirstNode());
					_iGraph.extractNode(((DoubleMCNode *) mcNode)->getSecondNode());
				} else {
					_iGraph.extractNode(mcNode);
				}
				_mcNodes.remove(mcNode);
				nodeStack.push(mcNode);
			}
		}
		
///// select phase
		while (nodeStack.size() > 0) {
			MCNode *mcNode = (MCNode *) nodeStack.top();
			nodeStack.pop();
			// insert the node into IG
			if (mcNode->isDoubleMCNode()) {
				_iGraph.insertNode(((DoubleMCNode *) mcNode)->getSecondNode());
				_iGraph.insertNode(((DoubleMCNode *) mcNode)->getFirstNode());
			} else {
				_iGraph.insertNode(mcNode);
			}
			_mcNodes.add(mcNode);
			
			if (mcNode->isDoubleMCNode()) {
				colorDoubleMCNode((DoubleMCNode *) mcNode);
			} else {
				colorMCNode(mcNode);
			}
			
			if ((mcNode->getColor() == UNCOLORED)) { // && node->isMC()) {
				if (debugRA)
					cout << "Cannot color node " << mcNode << " ... pushing it to spill stack" << endl;

				nodesToSpill.push(mcNode);
			}
		}
		
		// if there are no nodes to spill, we are done
		if (nodesToSpill.isEmpty())
			break;
		
///// spill phase
		while (nodesToSpill.size() > 0) {

			if (debugRA) {
				cout << nodesToSpill.size() << " nodes to spill..." << endl;
			}

			MCNode *mcNode = nodesToSpill.top();
			nodesToSpill.pop();
			
			if (debugRA) {
				cout << "Spilling node: " << mcNode << endl;
			}
			spillNode(mcNode);

			if (debugRA) {
				cout << "Node spilled..." << endl;
			}
			
			//if (debugRA)
				//currRoutine->print();
			
			//if(debugRA) 
			//	currRoutine->iWalker->print(cout);
			
			//if(debugRA) 
			//	print_RAproperties(_DUCHAIN_);
			//if(debugRA) 
			//	print_RAproperties(_UDCHAIN_);

			//if(debugRA)
			//	print_RAproperties(_MULTICHAIN_);
		}
		
		if (debugRA)
			currRoutine->printHTG();
		
		if (debugRA) {
			cout << "Rebuild graph..." << endl;
		}
		if (debugRA)
			cout << "Resetting IG..." << endl;
		
		//reset();
		resetColors();
		
		if (debugRA)
			cout << "Computing multichains" << endl;
		
		removeOldProps(currRoutine);

		currRoutine->compDefs();

		currRoutine->compLiveDead();
		
		//if(debugRA) 
		//	print_RAproperties(_DUCHAIN_);
		//if(debugRA) 
		//	print_RAproperties(_UDCHAIN_);
		if(debugRA) 
			print_LiveDeadProperties(currRoutine);
		
	}
}

void InterferenceGraph::listColors()
{
	cout << endl << "FINAL COLORS:" << endl;

	MyLinkedList<IGNode *> *nodeList = getNodeList();

	MyLinkedListIterator<IGNode *> *nodeIter = nodeList->elementsForward();
	while (nodeIter->hasMoreElements()) {
		IGNode *node = nodeIter->currentElement();

		if (node->isMC() && ((MCNode *) node)->isDoubleMCNode()) {
			cout << "DoubleMCNode:" << endl;
			cout << "IGNode: " << *(((DoubleMCNode *) node)->getFirstNode()) << " color: "
				<< ((DoubleMCNode *) node)->getFirstNode()->getColor() << endl;
			cout << "IGNode: " << *(((DoubleMCNode *) node)->getSecondNode()) << " color: "
				<< ((DoubleMCNode *) node)->getSecondNode()->getColor() << endl;
		} else {
			cout << "IGNode: " << *node << " color: " << node->getColor() << endl;
		}

		nodeIter->nextElement();
	}
	delete nodeIter;

	//delete nodeList;
}

void InterferenceGraph::printIDsAndNames()
{
	map<int, SSANumToMCMap>::iterator iter;

	cout << "IDs, names and indexes:" << endl;
	for (iter = _idToMCMap.begin(); iter != _idToMCMap.end(); iter++) {
		int symIndex = iter->first;

		SSANumToMCMap & ssaNumToMCMap = iter->second;
		SSANumToMCMap::iterator ssaNumIter;

		for (ssaNumIter = ssaNumToMCMap.begin(); ssaNumIter != ssaNumToMCMap.end(); ssaNumIter++) {
			int id = globalSymbolTable->getStartNum(symIndex) + ssaNumIter->first - SSA_START_NUM;

			char *regName = globalSymbolTable->getRegName(id);
			cout << id << " - " << regName;
			cout << " - " << globalSymbolTable->getIndex(id) << endl;
			delete regName;
		}
	}
}

void InterferenceGraph::print(ostream& out) {
	out << _iGraph;
}

void InterferenceGraph::printNodes(ostream& out) {
	_iGraph.printNodes(out);
}

void InterferenceGraph::printMCNodes(ostream& out) {
	
	out << "List of MC nodes:" << endl;
	
	MyLinkedListIterator<MCNode *> *mcNodeIter = _mcNodes.elementsForward();
	while (mcNodeIter->hasMoreElements()) {
		IGNode *mcNode = mcNodeIter->currentElement();
		
		out << *mcNode;
		//out << "  ";
		//node->print(out);
		out << endl;
		
		mcNodeIter->nextElement();
	}
	delete mcNodeIter;	
}

void InterferenceGraph::printEdges(ostream& out) {
	_iGraph.printEdges(out);
}

void InterferenceGraph::printMCEdges(ostream& out) {
	_iGraph.printMCEdges(out);
}

void InterferenceGraph::printMCNodeCompatEdges(ostream &out, MCNode *mcNode) {
	out << " ";
	mcNode->print(out);
	out << " -> ";
	
	MyLinkedListIterator<RegNode *> *regNodesIter = _regNodes.elementsForward();
	while (regNodesIter->hasMoreElements()) {
		RegNode *regNode = regNodesIter->currentElement();
		
		if (!_iGraph.existEdge(mcNode, regNode)) {
			regNode->print(out);
			out << ", ";
		}	
		
		regNodesIter->nextElement();
	}
	delete regNodesIter;
	
	out << endl;
}

void InterferenceGraph::printMCCompatEdges(ostream& out) {

	out << "List of MC compatibilities edges:" << endl;

	MyLinkedListIterator<MCNode *> *mcNodesIter = _mcNodes.elementsForward();
	while (mcNodesIter->hasMoreElements()) {
		MCNode *mcNode = mcNodesIter->currentElement();

		if (mcNode->isDoubleMCNode()) {
			out << "Double :" << endl << "[" << endl;
			printMCNodeCompatEdges(out, ((DoubleMCNode *) mcNode)->getFirstNode());
			printMCNodeCompatEdges(out, ((DoubleMCNode *) mcNode)->getSecondNode());
			out << "]" << endl;
		} else {
			printMCNodeCompatEdges(out, mcNode);
		}

		mcNodesIter->nextElement();
	}
	delete mcNodesIter;
}

// prints number of nodes and number of edges
void InterferenceGraph::printStatistics(ostream& out) {
    out << "IG Statistics: " << endl;
    out << "    " << _iGraph.numItems() << " nodes: " 
        << _regNodes.numItems() << " registers, "
        << _mcNodes.numItems() << " multichains, " << endl;
    
    int edges = 0;
    MyLinkedList<IGNode *>& nodes = _iGraph.getNodeList();

    MyLinkedListIterator<IGNode *> *nodeIter = nodes.elementsForward();
    while (nodeIter->hasMoreElements()) {
        IGNode *node = nodeIter->currentElement();

        edges += node->getNoOfAdjNodes();

        nodeIter->nextElement();
    }
    delete nodeIter;

    // edges are reported twice, by nodes at both ends
    edges /= 2;

    cout << "    edges: " << edges << endl;
}

void InterferenceGraph::printGML(ostream& out) {
	_iGraph.printGML(out);
}


//========================================================================
// Routine::buildIG
//
// build the interference graph associated with the current routine
//========================================================================


void Routine::buildIG()
{

#ifdef DEBUG
	printf("begin computing the interference graph\n");
#endif

	_ig->build(this);

#ifdef DEBUG
	printf("finished computing the interference graph \n");
#endif
}

