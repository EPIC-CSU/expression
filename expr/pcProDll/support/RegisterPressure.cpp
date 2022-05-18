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
// $Id$
//
// File:  		RegisterPressure.cpp
// Created:		Mon Nov 1, 99
// Last modified: 	Mon Nov 1, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "RegisterFileList.h"
#include "SymbolTable.h"

#include "RegPressure.h"
#include "LiveDeadBBProperty.h"
#include "RegArchModel.h"

#include "BBIterator.h"
#include "BasicBlockNode.h"
#include "Routine.h"

extern SymbolTable *globalSymbolTable;
extern RegisterFileList *globalRegFileList;
extern Routine *currRoutine;
extern RegArchModel globalRegModel;

inline int getRFIndex(BaseArgument *arg)
{
	if ((arg == NULL) || (!arg->isRealRegArg())) return _INVALID_;

	return (((RealRegArg *)arg)->regFileIndex());
}

inline int getRFIndex(int symVal)
{
	// First get the register index in the symbol table
	// e.g. register index of R4 in the symbol table.
	//
	// Next, get the RF index from the register file list.

	int regIndex = globalSymbolTable->getIndex(symVal);
	int rfIndex = globalRegFileList->getIndex((globalRegFileList->getRegFile(regIndex)).name());

	return rfIndex;
}

// given a symbol table id, this first gets the Multi-arg corresponding to it,
// and then returns the class (i.e classtype+datatype+regfile) corresponding to the multiarg.
//
void getClassIndices(int symVal, Vector<int> & indices)
{
	MultiArgList & marg = (globalIDToArgsMap[currRoutine]).getArgs(symVal);
	Vector<RegClassEntryNode2> clist;
	globalRegModel.createListOfClassTypesForArgs(marg._defList, clist, _GENERIC_);
	globalRegModel.createListOfClassTypesForArgs(marg._useList, clist, _GENERIC_);

	Vector<RegClassEntryNode2>::iterator i;
	for (i = clist.begin(); i != clist.end(); i++)
	{
		int tempIndex = globalRegModel.getIndexForRegClassToRegsMapping((*i)._dType, (*i)._cType, _GENERIC_);
		indices.push_back(tempIndex);
	}
}

void calcRegPressFromLDProp(LiveDeadBBProperty & ldProp, RegPressProperty & rpProp)
{
	// iterate over the ld prop.
	//
	MyLinkedListIterator<int> *iter = ldProp.liveListIterForw();

	Vector<int>::iterator ii;
	while (iter->hasMoreElements())
	{
		Vector<int> indices;
		getClassIndices(iter->currentElement(), indices);
		int vsize = indices.size();
		for (ii = indices.begin(); ii != indices.end(); ii++)
		{
			rpProp.addToRegisterPressure(*ii, (float)1/vsize);
		}
		//indices.clear();
		iter->nextElement();
	}
	delete iter;
}

void collectDefsFromOper(BaseOperation *oper, SetList<int> & collectorList)
{
	ArgList & dL = oper->destOperandList();
	ArgListIter diter;

	for (diter = dL.begin(); diter != dL.end(); diter++)
	{
		if (*diter == NULL) continue;

		if (!(*diter)->isRegisterArg()) continue;

		int id = (*diter)->getUniqID();
		collectorList.append(id);
	}
}

void collectUsesFromOper(BaseOperation *oper, SetList<int> & collectorList)
{
	ArgList & sL = oper->sourceOperandList();
	ArgListIter siter;

	for (siter = sL.begin(); siter != sL.end(); siter++)
	{
		if (*siter == NULL) continue;

		if (!(*siter)->isRegisterArg()) continue;

		int id = (*siter)->getUniqID();
		collectorList.append(id);
	}
}

void calcLDPropForPrevInstr(BaseInstruction *instr, LiveDeadBBProperty & ldProp)
{
	BaseOperation *currOper;

	if (instr == NULL) return;

	// for each operation in the instr:
	// go thru the dest list first. Delete all the args from ldProp.
	//
	// next, for each operation in the instr:
	// go thru the src list. Union all the register args to ldProp.
	//
	// Note: the order is quite important.
	//

	// Now, we collect all the destinations within this instruction.
	//
	// Note that this is very similar to the compDefs function in CompDefs.cpp

	SetList<int> collectDestSymVals;

	// First, go thru the data ops.
	//
	ArrayIterator<DataOpSlot> *dOpIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dOpIter->hasMoreElements())
	{
		if ((dOpIter->currentElement()).hasOper())
		{
			currOper = (dOpIter->currentElement()).getPtrToOperation();
			
			collectDefsFromOper(currOper, collectDestSymVals);
		}
		dOpIter->nextElement();
	}
	delete dOpIter;

	// Next, go thru the control ops.
	//
	ArrayIterator<ControlOpSlot> *cOpIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cOpIter->hasMoreElements())
	{
		if ((cOpIter->currentElement()).hasOper())
		{
			currOper = (cOpIter->currentElement()).getPtrToOperation();
			
			collectDefsFromOper(currOper, collectDestSymVals);
		}
		cOpIter->nextElement();
	}
	delete cOpIter;

	// We have finished collecting the destinations. Subtract them from the ld property.
	//
	(ldProp.getLiveList()).minus(collectDestSymVals);

	// Now, we collect all the register sources within this instruction.
	//
	SetList<int> collectSourceSymVals;

	// First, go thru the data ops.
	//
	dOpIter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (dOpIter->hasMoreElements())
	{
		if ((dOpIter->currentElement()).hasOper())
		{
			currOper = (dOpIter->currentElement()).getPtrToOperation();
			
			collectUsesFromOper(currOper, collectSourceSymVals);
		}
		dOpIter->nextElement();
	}
	delete dOpIter;

	// Next, go thru the control ops.
	//
	cOpIter = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (cOpIter->hasMoreElements())
	{
		if ((cOpIter->currentElement()).hasOper())
		{
			currOper = (cOpIter->currentElement()).getPtrToOperation();
			
			collectUsesFromOper(currOper, collectSourceSymVals);
		}
		cOpIter->nextElement();
	}
	delete cOpIter;

	// We have finished collecting the sources. Add (set_union) them to the ld property.
	//
	(ldProp.getLiveList()).set_union(collectSourceSymVals);
				
}

void setRegisterPressures(BasicBlockNode *bb)
{
	LiveDeadBBProperty *ldprop = (LiveDeadBBProperty *)(bb->getPtrToProperty(_LIVEDEAD_));

	if (ldprop == NULL) return;

	LiveDeadBBProperty currInstrLDProp = *(((LiveDeadBBProperty *)ldprop->copy()));

	// now, we know the live variables at the end of the bb. 
	// So, we can calculate the reg pressure at the last instr of the bb.
	//
	RegPressProperty currInstrPressProp;
	

	BBBackIterator biter(bb);

	BaseInstruction *currInstr;
	while (biter.hasMoreElements())
	{
		currInstr = biter.currentElement();
	
		calcRegPressFromLDProp(currInstrLDProp, currInstrPressProp);
		currInstr->addProperty(currInstrPressProp);

		// Now, for each instr, add its uses and subtract its defs from the ld prop 
		// to get the live info for the previous instr.
		//
		calcLDPropForPrevInstr(currInstr, currInstrLDProp);

		biter.prevElement();
	}
}

void setRegisterPressures(Routine *rout)
{
	// iterate over the basic blocks and set the register pressures for each instruction.

	if (rout == NULL) return;

	LinkedListIterator<BasicBlockNode> *iter = (rout->getCFG()).createBBIterForw();

	while (iter->hasMoreElements())
	{
		setRegisterPressures(&(iter->currentElement()));
		iter->nextElement();
	}
	delete iter;
}

// Note: This function returns the register pressure at the end of the instruction.
//
void getRegisterPressure(RegPressProperty & rpProp, BaseInstruction *instr, BasicBlockNode *bb = NULL)
{
	RegPressProperty *tempProp;

	if ((tempProp = (RegPressProperty *)(instr->getPtrToProperty(_REGPRESSURE_))) != NULL)
	{
		rpProp = *tempProp;

		return;
	}

	if (bb == NULL) return;

	// else we need to compute the register pressure for that particular instr.
	//
	LiveDeadBBProperty *ldprop = (LiveDeadBBProperty *)(bb->getPtrToProperty(_LIVEDEAD_));

	if (ldprop == NULL) return;

	LiveDeadBBProperty currInstrLDProp = *(((LiveDeadBBProperty *)ldprop->copy()));	

	BBBackIterator biter(bb);

	BaseInstruction *currInstr;
	while (biter.hasMoreElements())
	{
		currInstr = biter.currentElement();

		if (currInstr == instr)
		{
			calcRegPressFromLDProp(currInstrLDProp, rpProp);
			break;
		}

		// Now, for each instr, add its uses and subtract its defs from the ld prop 
		// to get the live info for the previous instr.
		//
		calcLDPropForPrevInstr(currInstr, currInstrLDProp);

		biter.prevElement();
	}
}