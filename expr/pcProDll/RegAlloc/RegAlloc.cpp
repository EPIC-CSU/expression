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
// RegAlloc.cpp : Implements the Register Allocation Phase of the Compiler
//
/////////////////////////////////////////////////////////////////////////////
//
// Description : 
//
/////////////////////////////////////////////////////////////////////////////
//
// $Header: /express/pcProDll/RegAlloc/RegAlloc.cpp 42    11/06/02 2:33p Pmishra $
// $Modtime: 11/06/02 2:14p $
//
/////////////////////////////////////////////////////////////////////////////
//
// This file is copyrighted by the Regents of the
// University of California, Irvine. The following terms apply to
// all files associated with the description unless explicitly
// disclaimed in individual files.
// The authors hereby grant permission to use this description and
// its documentation for educational and non-commercial purpose.
// No written agreement, license, or royalty fee is required for
// authorized use of this software.
//
/////////////////////////////////////////////////////////////////////////////
//
// IN NO EVENT SHALL THE AUTHORS OR DISTRIBUTORS BE
// LIABLE TO ANY PARTY FOR DIRECT, INDIRECT, SPECIAL,
// INCIDENTAL, OR CONSEQUENTIAL DAMAGES ARISING OUT
// OF THE USE OF THIS DESCRIPTION, ITS DOCUMENTATION,
// OR ANY DERIVATIVES THEREOF, EVEN IF THE AUTHORS HAVE
// BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
// THE AUTHORS AND DISTRIBUTORS SPECIFICALLY
// DISCLAIM ANY WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE,
// AND NON-INFRINGEMENT. THIS DESCRIPTION IS PROVIDED
// ON AN "AS IS" BASIS, AND THE AUTHORS AND
// DISTRIBUTORS HAVE NO OBLIGATION TO PROVIDE
// MAINTENANCE, SUPPORT, UPDATES, ENHANCEMENTS, OR
// MODIFICATIONS.
/////////////////////////////////////////////////////////////////////////////
//
// ACES Lab - University of California, Irvine - USA
//
// Created:		Sat Dec 4, 1999
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, aces@ics.uci.edu
//
/////////////////////////////////////////////////////////////////////////////


//========================================================================
// Routine::regAlloc
//
// perform register allocation for the current routine
//========================================================================

#include "stdafx.h"
#include "Routine.h"
#include "IConstArg.h"
#include "OpCode.h"
#include "OpCodeTable.h"
#include "NormalInstr.h"
#include "InterferenceGraph.h"
#include "MultiChain.h"
#include "JumpOp.h"
#include "RAProperty.h"
#include "IRTraversal.h"
#include "RegNode.h"
#include "MCNode.h"
//#include "DebugSwitches.h"
#include "GToTRegMapProperty.h"
#include "DUProperty.h"
#include "UDProperty.h"
#include "SSAArgProperty.h"
#include "MultiChainProperty.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"
#include "ComputeOp.h"
#include "RoutParmProperty.h"
#include "IRHelper.h"
#include "RegArchModel.h"

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


static int debugRA = 0;//debugRA_RegAlloc;

extern SymbolTable* globalSymbolTable;
extern void print_RAproperties(PropertyNames prop, Routine *rout);
extern void print_BBproperties(PropertyNames prop);
extern void print_LiveDeadProperties(Routine *rout = NULL);
extern int spills;			// number of spilled arguments
extern int coalesced;		// number of coalesced registers
extern int reloads;			// number of reloads added by spilling
extern void initRegisterMapping();

extern RegArchModel globalRegModel;

// In case of forwarding, when some reads are done at the end of the cycle,
// after the write
bool allowDelayedReads = false;

//Routine *currRoutine;

// just for debugging
extern void spillMultiChain(MultiChain *mc, bool processDoubles = false);



NormalInstr *dummyInstr;
NormalOp *dummyOp;

#define FORWARD_DIR		1
#define BACKWARD_DIR	0

extern void processReloads();

void printRegNameAfterRA(RegisterArg *reg, ostream& out)
{
	RAProperty *raProp = (RAProperty *) reg->getPtrToProperty(_RA_);
	int regID = raProp->getRegID();
	RegisterFile& regFile = globalRegFileList->getRegFile(regID);

	out << regFile.name() << (regID - regFile.getID());
}

void printRegNameAfterRA(RegisterArg *reg)
{
	printRegNameAfterRA(reg, cout);
}

void printRegNameAfterRAToString(RegisterArg *reg, char *str)
{
	char buf[20];
	RAProperty *raProp = (RAProperty *) reg->getPtrToProperty(_RA_);

	if (raProp == NULL) // error condition
	{
		// print a warning!
		//
		cout << "Warning: Recieved NULL RAProperty while printing Reg Names after RA" << endl;
		return;
	}
	int regID = raProp->getRegID();
	RegisterFile& regFile = globalRegFileList->getRegFile(regID);

	strcat(str, regFile.name());
	strcat(str, itoa((regID - regFile.getID()), buf, 10));
}

void renameMCArgs(MultiChain *mc, InterferenceGraph& ig)
{
	SetList<BaseArgument *> *args = mc->getAllArgs();
	MCNode *mcNode = ig.getMCNode(mc);
	int color = mcNode->getColor();

	// Added by SAH on 25th Feb, 2001
	//
	if (color < 0) // then there's a problem???
	{
		cout << "Warning : In renameMCArgs : recieved invalid color" << endl;
		return;
	}

	MyLinkedListIterator<BaseArgument *> *argIter = args->elementsForward();
	while (argIter->hasMoreElements()) {
		RealRegArg *reg = (RealRegArg *) argIter->currentElement();

		//char *txt = reg->getText();
		//txt[1] = '\0';
		//reg->setText(txt);
		//reg->setVal(color);
		//reg->setRegFile(_INVALID_);
		RegNode *realReg = ig.getRegOfColor(color);

		//RAProperty raProp(color);
		RAProperty raProp(realReg->getIndex());
		reg->addProperty(raProp);

		argIter->nextElement();
	}
	delete argIter;

	delete args;
}

void renameRegisters(InterferenceGraph& ig)
{
	MyLinkedList<MultiChain *> *mcList = ig.getMCList();
	MyLinkedListIterator<MultiChain *> *mcIter = mcList->elementsForward();

	while (mcIter->hasMoreElements()) {
		MultiChain *mc = mcIter->currentElement();

		renameMCArgs(mc, ig);

		mcIter->nextElement();
	}
	delete mcIter;
	delete mcList;
}

void removeOldProps(Routine *routine, bool removeUseDef = false)
{

  if (removeUseDef) {
	MyLinkedListIterator<BaseInstruction *> *instrIter = routine->instrListIteratorForw();
	while (instrIter->hasMoreElements()) {
		BaseInstruction *instr = instrIter->currentElement();
		
		MyLinkedList<BaseArgument *> *defList = IRHelper::getAllDefsInInstr(instr);
		MyLinkedListIterator<BaseArgument *> *defIter = defList->elementsForward();
		
		while (defIter->hasMoreElements()) {
			BaseArgument *def = defIter->currentElement();
			
			if (def && def->isRegisterArg()) {
				def->removeProperty(def->getPtrToProperty(_DUCHAIN_));
				def->removeProperty(def->getPtrToProperty(_UDCHAIN_));
			}
			
			defIter->nextElement();
		}
		
		delete defIter;
		delete defList;
		
		MyLinkedList<BaseArgument *> *useList = IRHelper::getAllUsesInInstr(instr);
		//MyLinkedList<BaseArgument *> *argUseList = new MyLinkedList<BaseArgument *>();
		
		MyLinkedListIterator<BaseArgument *> *useIter = useList->elementsForward();
		
		while (useIter->hasMoreElements()) {
			BaseArgument *use = useIter->currentElement();
			
			if (use && use->isRegisterArg()) {
				use->removeProperty(use->getPtrToProperty(_DUCHAIN_));
				use->removeProperty(use->getPtrToProperty(_UDCHAIN_));
			}
			
			useIter->nextElement();
		}
		
		delete useIter;
		delete useList;
		
		instrIter->nextElement();
	}
	delete instrIter;
  }

	LinkedListIterator<BasicBlockNode> *bbIter = routine->getCFG().createBBIterForw();
	while (bbIter->hasMoreElements()) {
		BasicBlockNode& bb = bbIter->currentElement();
		
		BaseBBProperty *bbProp = bb.getPtrToProperty(_LIVEDEAD_);
		if (bbProp != NULL)
			bb.removeProperty(*bbProp);

		bbProp = bb.getPtrToProperty(_COMPDEF_);
		if (bbProp != NULL)
			bb.removeProperty(*bbProp);

		bbIter->nextElement();
	}
	delete bbIter;
}

void prependDummyInstruction(Routine *routine)
{
	InterferenceGraph& ig = routine->getIG();
	
	dummyOp = new ComputeOp();				
	OpCode *opc = new OpCode("NOP");
	dummyOp->initOpCode(*opc);

	dummyInstr = new NormalInstr();
						
	dummyInstr->initScheduleAndAdd(dummyOp, true);
	currRoutine->oMapper->addOper(dummyOp, dummyInstr);
						
	BasicBlockNode& firstBB = routine->getCFG().getFirstBB();

	NormalInstr *firstInstr = (NormalInstr*) firstBB.getFirstInstr();
	currRoutine->insertInstrBefore(dummyInstr, firstInstr);

	currRoutine->iWalker->addPred(firstInstr, dummyInstr);
	dummyInstr->addSuccID(firstInstr->idValue());

	JumpOp *jOp = new JumpOp(firstInstr);
	BaseOpSlot& newOpSlot = dummyInstr->getFlowOpSlot(numFlowOpsPerInstr - 1);
		((FlowOpSlot *) &newOpSlot)->addOperation(jOp);

	// update bb leader
	firstBB.initLeader(dummyInstr);

	// now, add ud chains between all uses of call params and the dummy op
	RoutParmProperty *parm = (RoutParmProperty *) routine->getPtrToProperty(_ROUTPARM_);
	
	if (parm) {	
		
		int position = 0;
		ArgList & callParms = parm->getCallParms();
		
		// for all call parameters
		ArgListIter it;
		for (it = callParms.begin(); it != callParms.end(); it++) {
			int symTabID = (*it)->getUniqID();

			BaseArgument *dummyArg = IRHelper::createArg(symTabID);
			dummyOp->addOperand(dummyArg, position, _DEST_LIST_);
			delete dummyArg;
			position++;
			
			MultiChain *mc = routine->getIG().getMC(symTabID);
			
			mc->addDef(dummyInstr);

			// Added by SAH on 12/12/00
			//
			// Previously, after creation of the dummy op/instr, 
			// the multi-chain was updated with the newly created instr;
			// however, the def arg was not updated with the multi-chain property.
			// This caused problems if the variable was chosen to be spilled later.
			//
			MultiChainProperty tmp(mc);
			(dummyOp->ptrToOperand(position - 1, _DEST_LIST_))->addProperty(tmp);	

			// for all use instructions
			MyLinkedListIterator<BaseInstruction*> * useListIter = mc->useListIteratorForw();
			while (useListIter->hasMoreElements()) {
				NormalInstr *useInstr	= (NormalInstr *) useListIter->currentElement();
				
				// for all uses in instruction
				MyLinkedList<BaseArgument *> *uses = IRHelper::getUsesOfArgInInstr(symTabID, useInstr);
				MyLinkedListIterator<BaseArgument *> *argIter = uses->elementsForward();
				while (argIter->hasMoreElements()) {
					BaseArgument *use = argIter->currentElement();
					
					// add the ud chain to the use argument
					UDChainProperty* udProp = (UDChainProperty*)use->getPtrToProperty(_UDCHAIN_);
					if ( udProp ) {
						udProp->addDef(dummyOp);
					} else {
						UDChainProperty tmpProp;
						tmpProp.appendDef(dummyOp);
						use->addProperty(tmpProp);
					}
					
					argIter->nextElement();
				}
				delete argIter;
				delete uses;
				
				useListIter->nextElement();
			}
			delete useListIter;
		}
	}
}


void removeDummyInstruction(Routine *routine)
{
	if (dummyInstr) {
		BasicBlockNode& firstBB = routine->getCFG().getFirstBB();
		NormalInstr *initialFirstInstr = (NormalInstr *) dummyInstr->getDefaultTargetInstr();
		
		currRoutine->iWalker->delEdge(initialFirstInstr, dummyInstr);
		currRoutine->oMapper->removeOper(dummyOp);
		
		// update bb leader
		firstBB.initLeader(initialFirstInstr);

		currRoutine->removeInstruction(dummyInstr);
		
		delete dummyInstr;	// also delete dummyOp
		dummyInstr = NULL;
	}
}

int totalSpills = 0;

// main register allocation procedure
void doRegAlloc(Routine *routine)
{	
	spills = 0;
	coalesced = 0;
	reloads = 0;

	currRoutine = routine;

	// If the routine is not "reachable" (i.e. is not used in the program)
	// then, we are not guaranteed that the Call initialization would have been done.
	// (For ex. with GCC Front-end, we need to add dummy ops at the beginning of each
	// routine. If the routine is never called, then the dummy NOP will not have the
	// input parameters of the routine.
	// 
	if (!routine->isUsedInProgram())
		return;

	// Made Conditional by PB and AVS
	if (debugRA) {
	//	globalSymbolTable->print(cout);
		//globalRegModel.printCompatibilityArray(cout);
	}

	int tempDEBUG = 0;

	if (strcmp(routine->getName(), "_output") == 0)
		tempDEBUG = 1;

	// not needed after the new register mapping approach
	//initRegisterMapping();
	
	InterferenceGraph& ig = routine->getIG();
	
	if (debugRA)
		cout << "Adding real registers to IG" << endl;
	
	ig.addRegNodes();

	//if (!allowDelayedReads) {
		removeOldProps(currRoutine, true);
	//} else {
		// keep existing DU/UD chains
	//	removeOldProps(currRoutine, false);
	//}
	
	cout << "Recomputing properties... " << endl;

	/// Next 2 if-ed out on 02/04/2002, because of delayed reads
	/// (DU/UD and livedead analysis do not know how to handle them)
	//if (!allowDelayedReads) {
		currRoutine->compDefs();
		currRoutine->compUDChain();
	//}

	// should before prependDummyInstruction, because multichains are used there
	currRoutine->compMultiChains();

	//Commented out by SAH on Jan 12th, 2001
	// The Dummy Instruction is added before SSA.
	//
	// if there are parameters, add the dummy instruction (hack for correct handling of parameters)
/*
	if (routine->getPtrToProperty(_ROUTPARM_) != NULL)
		prependDummyInstruction(currRoutine);
*/

	if (debugRA)
		currRoutine->print();

	if (debugRA) {
		ig.printMCNodes(cout);
	}

	ig.addMappingInterferences();

	if (debugRA) {
		ig.printNodes(cout);
		//ig.printMCCompatEdges(cout);
	}

	// should be after prependDummyInstruction, to correctly compute livedead for the first basic block
	// because of param args
	currRoutine->compLiveDead();

	currRoutine->iWalker->reset();
	currRoutine->iWalker->initWalk(currRoutine);

	if (debugRA) {
		//print_RAproperties(_DUCHAIN_, currRoutine);
		//print_RAproperties(_UDCHAIN_, currRoutine);
		//print_LiveDeadProperties(currRoutine);
	}

	if (debugRA) {
		currRoutine->print();
	}

	//MultiChain *mc = ig.getMC(82);
	//spillMultiChain(mc);
	//return;

	cout << "Coloring interference graph..." << endl;
	ig.color();

	if (debugRA) {
		cout << "Spills: " << spills << endl;
		cout << "Coalesced: " << coalesced << endl;
		cout << "Reloads: " << reloads << endl;
	}

		totalSpills += spills;

	if (debugRA)
		ig.printIDsAndNames();

	if (debugRA)
		routine->print();

	if (debugRA)
		cout << endl << "Renaming registers..." << endl << endl;
	
	renameRegisters(ig);

	//Commented out by SAH on Jan 12th, 2001
	// The Dummy Instruction is removed at the end of compilation.
	//
/*
	removeDummyInstruction(currRoutine);
*/

	currRoutine->iWalker->reset();
	currRoutine->iWalker->initWalk(currRoutine);
	processReloads();

	if (debugRA)
		routine->print();

	//ig.~InterferenceGraph();
}

void Routine::registerAllocation()
{

#ifdef DEBUG
	printf("begin register allocation\n");
#endif
	
	createIG();
	doRegAlloc(this);
	deleteIG();

#ifdef DEBUG
	printf("finished register allocation \n");
#endif
}

