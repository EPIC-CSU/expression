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
////////////////////////////////////////////////////////
// File:  			profit_function.cpp
// Created:			July 25, 2000.
// Last modified: 	July 25, 2000.
// Author:			Aviral Shrivastava
// Email:			aviral@ics.uci.edu
//
////////////////////////////////////////////////////////

#include "stdafx.h"
#include "Routine.h"
#include "ControlFlowGraph.h"
#include "BasicBlockNode.h"
#include "ThumbPass.h"
#include "MyString.h"
#include "BBIterator.h"
#include "NormalInstr.h"
#include "ComputeOp.h"
#include "IburgToIR.h"
#include "SemActPrimitives.h"
#include "BaseOperation.h"
#include "profit_function.h"
#include "RegPressure.h"
#include "RegArchModel.h"

#include <map>

#ifdef map
#undef map
#endif 
#define map std::map

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


/*
The cost to Thumbize a block is in three parts
1. Code size change due to rISAizing
	= Number of instructions converted*(Size of orignal instruction - size of rISA instruction)

2. Code size change due to rISA block interfaces
	= 1+1+1 (one mx instruction at top, + one mx_r instruction at the bottom, + one nop instruction)

3. Code size due to spill/reload insertion
	
*/

// extern functions
//
extern void getRegisterPressure(RegPressProperty & rpProp, BaseInstruction *instr, BasicBlockNode *bb);

// extern data
//
extern int debugThumb;
extern int indexForTempVars;
extern int indexForRs;
extern RegArchModel globalRegModel;
extern int classTypeIndexTempRISAGeneric;
extern int classTypeIndexNormalGeneric;
extern int dataTypeIndexIntGeneric;

int CS1(rISABlock rb)
{
	// This gives the change in size due to compression of the instructions only
	// 
	return rb.numInstrs * (SIZE_OF_NORMAL_INSTR_IN_BYTES - SIZE_OF_RISA_INSTR_IN_BYTES);
}


int CS2(rISABlock rb)
{
	/* Each internal block will have a mx and an mx_r extra instr
	   When a rISABlock boundary is aligned with the block boundary, then it might not be necessary.
	   A nop may be needed in any of the above cases.
	   So I average out these 
	*/

	return (SIZE_OF_NORMAL_INSTR_IN_BYTES + SIZE_OF_RISA_INSTR_IN_BYTES) * NEED_MODE_CHANGE_INSTR
		+ SIZE_OF_RISA_INSTR_IN_BYTES;
}


int CS3(rISABlock rb)
{
	// Index of rISA and normal variables in the register pressure property of instruction
	//
	int tempRISAIndex = globalRegModel.getIndexForRegClassToRegsMapping(dataTypeIndexIntGeneric, classTypeIndexTempRISAGeneric, _GENERIC_);
	int normalIndex = globalRegModel.getIndexForRegClassToRegsMapping(dataTypeIndexIntGeneric, classTypeIndexNormalGeneric, _GENERIC_);

	// Find the average rISA register pressure value, if the block is converted
	cout << "\n\n################################################" << endl;
	cout << "Calculating average pressure of rISA variables" << endl;
	int arp = getAvgRegPress(rb, tempRISAIndex);

	// Find the normal register values, if the block is converted
	cout << "Calculating average pressure of normal variables" << endl;
	int arp_nr = getAvgRegPress(rb, normalIndex);

	// Find the average number of uses per definition of the variables inside the rISA block
	cout << "Calculating average number of uses per definition" << endl;
	double aupd = getAvgUsesPerDef(rb);

	// Find the average distance between a definition and it's first use inside the rISA block
	cout << "Calculating average distance between a definition and it's first use" << endl;
	int adul = getAvgDefUseLen(rb);

	if (debugThumb)
	{
		cout << "\nRisaBlock Start = " << rb.startInstr << "\t RisaBlock End = " << rb.endInstr << endl;
		cout << "arp = " << arp << endl;
		cout << "arp_nr = " << arp_nr << endl;
		cout << "aupd = " << aupd << endl;
		cout << "adul = " << adul << endl;

	}

	// estimate the number of spills If rISA
	//
	int noOfRisaSpills = getNoOfRisaSpills(arp, aupd, rb);
	int noOfRisaReloads = getNoOfRisaReloads(arp, rb);
	int noOfNonRisaSpills = getNoOfNonRisaSpills(noOfRisaSpills, arp, arp_nr, rb);
	int noOfNonRisaReloads = noOfNonRisaSpills;
	int noOfSpillsIfRisa = noOfRisaSpills + noOfNonRisaSpills;
	int noOfReloadsIfRisa = noOfRisaReloads + noOfNonRisaReloads;

	// estimate the number of spills If Normal
	//
	int noOfSpillsIfNormal = getNoOfSpillsIfNormal(arp+arp_nr, adul, rb);
	int noOfReloadsIfNormal = getNoOfReloads(noOfSpillsIfNormal, aupd, rb);

	//increase in size if risa
	int incInSizeIfRisa = (noOfSpillsIfRisa + CONST4 * noOfReloadsIfRisa) * SIZE_OF_NORMAL_INSTR_IN_BYTES;
	
	// increase in size if normal
	int incInSizeIfNormal = (noOfSpillsIfNormal + CONST4 * noOfReloadsIfNormal) * SIZE_OF_NORMAL_INSTR_IN_BYTES;
	// return the difference in increase in size

	if (debugThumb)
	{
		cout << "noOfRisaSpills = " << noOfRisaSpills << endl;
		cout << "noOfRisaReloads = " << noOfRisaReloads << endl;
		cout << "noOfNonRisaSpills = " << noOfNonRisaSpills << endl;
		cout << "noOfNonRisaReloads = " << noOfNonRisaReloads << endl;
		cout << "noOfSpillsIfRisa = " << noOfSpillsIfRisa << endl;
		cout << "noOfReloadsIfRisa = " << noOfReloadsIfRisa << endl;
		cout << "noOfSpillsIfNormal = " << noOfSpillsIfNormal << endl;
		cout << "noOfReloadsIfNormal = " << noOfReloadsIfNormal << endl;
		cout << "\nincInSizeIfRisa = " << incInSizeIfRisa << endl;
		cout << "incInSizeIfNormal = " << incInSizeIfNormal << endl;
	}
	return incInSizeIfRisa - incInSizeIfNormal;
}

int getNoOfReloads(int noOfSpills, int aupd, rISABlock rb)
{
	if (aupd == 0)
		return 0;
	else
		return CONST5 * noOfSpills * aupd; 
}

int getNoOfRisaSpills(int arp, double aupd, rISABlock rb)
{
	int risaRegsAvail = NUM_RISA_REGS * REG_FACTOR_FOR_SPILL;
	if (arp > risaRegsAvail)
	{
		return (int) (rb.numInstrs * pow((1 - (1 / (aupd))), 2) );
	}
	return 0;
}

int getNoOfRisaReloads(int arp, rISABlock rb)
{
	int risaRegsAvail = NUM_RISA_REGS * REG_FACTOR_FOR_SPILL;
	if (arp > risaRegsAvail)
	{
		return (int) (rb.numInstrs * (1 - ((double)risaRegsAvail / (double)arp)) );
	}
	return 0;
}


int getNoOfNonRisaSpills(int noOfRisaSpills, int arp, int arp_nr, rISABlock rb)
{
	int availNonRisaRegs;

	if (arp >= NUM_RISA_REGS * REG_FACTOR_FOR_SPILL)
		availNonRisaRegs = NUM_REGS - NUM_RISA_REGS;
	else
		availNonRisaRegs = NUM_REGS - arp;

	if (arp_nr < (REG_FACTOR_FOR_SPILL * availNonRisaRegs))
		return 0;
	return arp_nr - (REG_FACTOR_FOR_SPILL * availNonRisaRegs);
}

int getNoOfSpillsIfNormal(int n_arp, int n_adul, rISABlock rb)
{
	if (n_arp < NUM_REGS * REG_FACTOR_FOR_SPILL)
		return 0;
	if (n_adul == 0)
		return n_arp - NUM_REGS * REG_FACTOR_FOR_SPILL;
	else
		return ((n_arp - NUM_REGS * REG_FACTOR_FOR_SPILL) * rb.numInstrs) / n_adul;
}


/*
int CS3(rISABlock rb)
{
	// Index of rISA and normal variables in the register pressure property of instruction
	//
	int tempRISAIndex = globalRegModel.getIndexForRegClassToRegsMapping(dataTypeIndexIntGeneric, classTypeIndexTempRISAGeneric, _GENERIC_);
	int normalIndex = globalRegModel.getIndexForRegClassToRegsMapping(dataTypeIndexIntGeneric, classTypeIndexNormalGeneric, _GENERIC_);

	// Find the average rISA register pressure value, if the block is converted
	cout << "\n\n################################################" << endl;
	cout << "Calculating average pressure of rISA variables" << endl;
	int arp = getAvgRegPress(rb, tempRISAIndex);

	// Find the normal register values, if the block is converted
	cout << "Calculating average pressure of normal variables" << endl;
	int arp_nr = getAvgRegPress(rb, normalIndex);

	// Find the average number of uses per definition of the variables inside the rISA block
	cout << "Calculating average number of uses per definition" << endl;
	int aupd = getAvgUsesPerDef(rb);

	// Find the average distance between a definition and it's first use inside the rISA block
	cout << "Calculating average distance between a definition and it's first use" << endl;
	int adul = getAvgDefUseLen(rb);

	if (debugThumb)
	{
		cout << "\nRisaBlock Start = " << rb.startInstr << "\t RisaBlock End = " << rb.endInstr << endl;
		cout << "arp = " << arp << endl;
		cout << "arp_nr = " << arp_nr << endl;
		cout << "aupd = " << aupd << endl;
		cout << "adul = " << adul << endl;

	}

	// estimate the number of spills If rISA
	//
	int noOfRisaSpills = getNoOfRisaSpills(arp, adul, rb);
	int noOfNonRisaSpills = getNoOfNonRisaSpills(noOfRisaSpills, arp, arp_nr, rb);
	int noOfSpillsIfRisa = noOfRisaSpills + noOfNonRisaSpills;
	int noOfReloadsIfRisa = getNoOfReloads(noOfSpillsIfRisa, aupd, rb);

	// estimate the number of spills If Normal
	//
	int noOfSpillsIfNormal = getNoOfSpillsIfNormal(arp+arp_nr, adul, rb);
	int noOfReloadsIfNormal = getNoOfReloads(noOfSpillsIfNormal, aupd, rb);

	//increase in size if risa
	int incInSizeIfRisa = (noOfSpillsIfRisa + CONST4 * noOfReloadsIfRisa) * SIZE_OF_NORMAL_INSTR_IN_BYTES;
	
	// increase in size if normal
	int incInSizeIfNormal = (noOfSpillsIfNormal + CONST4 * noOfReloadsIfNormal) * SIZE_OF_NORMAL_INSTR_IN_BYTES;
	// return the difference in increase in size

	if (debugThumb)
	{
		cout << "noOfRisaSpills = " << noOfRisaSpills << endl;
		cout << "noOfNonRisaSpills = " << noOfNonRisaSpills << endl;
		cout << "noOfSpillsIfRisa = " << noOfSpillsIfRisa << endl;
		cout << "noOfReloadsIfRisa = " << noOfReloadsIfRisa << endl;
		cout << "noOfSpillsIfNormal = " << noOfSpillsIfNormal << endl;
		cout << "noOfReloadsIfNormal = " << noOfReloadsIfNormal << endl;
		cout << "\nincInSizeIfRisa = " << incInSizeIfRisa << endl;
		cout << "incInSizeIfNormal = " << incInSizeIfNormal << endl;
	}
	return incInSizeIfRisa - incInSizeIfNormal;
}


int getNoOfReloads(int noOfSpills, int aupd, rISABlock rb)
{
	if (aupd == 0)
		return 0;
	else
		return CONST5 * noOfSpills * aupd; 
}



int getNoOfRisaSpills(int arp, int adul, rISABlock rb)
{
	int risaRegsAvail = NUM_RISA_REGS * REG_FACTOR_FOR_SPILL;

	if (arp < risaRegsAvail)
		return 0;

	if (adul == 0)
		return arp - NUM_RISA_REGS * REG_FACTOR_FOR_SPILL;
	else
		return arp - risaRegsAvail;
}


int getNoOfNonRisaSpills(int noOfRisaSpills, int arp, int arp_nr, rISABlock rb)
{
	int availNonRisaRegs;

	if (arp >= NUM_RISA_REGS * REG_FACTOR_FOR_SPILL)
		availNonRisaRegs = NUM_REGS - NUM_RISA_REGS;
	else
		availNonRisaRegs = NUM_REGS - arp;

	if (arp_nr < (REG_FACTOR_FOR_SPILL * availNonRisaRegs))
		return 0;
	return arp_nr - (REG_FACTOR_FOR_SPILL * availNonRisaRegs);
}

int getNoOfSpillsIfNormal(int n_arp, int n_adul, rISABlock rb)
{
	if (n_arp < NUM_REGS * REG_FACTOR_FOR_SPILL)
		return 0;
	if (n_adul == 0)
		return n_arp - NUM_REGS * REG_FACTOR_FOR_SPILL;
	else
		return ((n_arp - NUM_REGS * REG_FACTOR_FOR_SPILL) * rb.numInstrs) / n_adul;
}
*/

double getAvgUsesPerDef(rISABlock rb)
{
	// for each instr in block
	// list the operations, and then list all the defs
	// find the no. of uses for each def and return that
	//
	// Note: We assume that each rISA block is a straight line segment of code.
	//
	//
	int totUses= 0;
	int totDefs = 0;

	BaseInstruction *currInstr = rb.startInstr;
	OpList opsInInstr;

	currInstr->createListOfOperations(opsInInstr);
	for (OpListIter oIter = opsInInstr.begin(); oIter != opsInInstr.end(); oIter++)
	{
		if (*oIter)
		{
			ArgList destArgs = (*oIter)->destOperandList();
			for (ArgListIter aIter = destArgs.begin(); aIter != destArgs.end(); aIter++)
			{
				if (*aIter)
				{
					totUses += getNoOfUses((*aIter));
					totDefs++;
					if (debugThumb)
					{
						cout << "currInstr = " << currInstr << "\ttotUses = " << totUses << "\ttotDefs = " << totDefs << endl;
					}
				}
			}
		}
	}

	while (currInstr != rb.endInstr)
	{
		currInstr = ((NormalInstr *)currInstr)->getDefaultTargetInstr();
		opsInInstr.clear();
		currInstr->createListOfOperations(opsInInstr);
		for (OpListIter oIter = opsInInstr.begin(); oIter != opsInInstr.end(); oIter++)
		{
			if (*oIter)
			{
				ArgList destArgs = (*oIter)->destOperandList();
				for (ArgListIter aIter = destArgs.begin(); aIter != destArgs.end(); aIter++)
				{
					if (*aIter)
					{
						totUses += getNoOfUses((*aIter));
						totDefs++;
						if (debugThumb)
						{
							cout << "currInstr = " << currInstr << "\ttotUses = " << totUses << "\ttotDefs = " << totDefs << endl;
						}
					}
				}
			}
		}
	}
	
	// Now just do the average
	if (totDefs)
		return (double)totUses/(double)totDefs;
	else
		return 0;
}



int getNoOfUses(BaseArgument *destArg)
{
	BaseArgProperty *duProp = destArg->getPtrToProperty(_DUCHAIN_);
	if (duProp)
		return ((DUChainProperty *) duProp)->getUseList().numItems();
	else
		return 0;
}



int getAvgDefUseLen(rISABlock rb)
{
	// for each instr in block
	// list the operations, and then list all the defs
	// find the defUseLen of each def, average it and return
	//
	// Note: We assume that each rISA block is a straight line segment of code.
	//
	//
	int totDefUseLen = 0;
	int totDefs = 0;

	BaseInstruction *currInstr = rb.startInstr;
	
	{
		OpList opsInInstr;
		currInstr->createListOfOperations(opsInInstr);
		for (OpListIter oIter = opsInInstr.begin(); oIter != opsInInstr.end(); oIter++)
		{
			if (*oIter)
			{
				ArgList destArgs = (*oIter)->destOperandList();
				for (ArgListIter aIter = destArgs.begin(); aIter != destArgs.end(); aIter++)
				{
					if (*aIter)
					{
						totDefUseLen += getDefUseLen(*aIter, currInstr, rb);
						totDefs++;
						if (debugThumb)
						{
							cout << "currInstr = " << currInstr << "\ttotDefUseLen = " << totDefUseLen<< endl;
						}
					}
				}
			}
		}
	}

	while (currInstr != rb.endInstr)
	{
		currInstr = ((NormalInstr *)currInstr)->getDefaultTargetInstr();
		OpList opsInInstr;
		currInstr->createListOfOperations(opsInInstr);
		for (OpListIter oIter = opsInInstr.begin(); oIter != opsInInstr.end(); oIter++)
		{
			if (*oIter)
			{
				//if (debugThumb)
				//{
				//	(*oIter)->shortPrint(cout);
				//	cout << endl;
				//}

				ArgList destArgs = (*oIter)->destOperandList();
				for (ArgListIter aIter = destArgs.begin(); aIter != destArgs.end(); aIter++)
				{
					if (*aIter)
					{
						totDefUseLen += getDefUseLen((*aIter), currInstr, rb);
						totDefs++;
						if (debugThumb)
						{
							cout << "currInstr = " << currInstr << "\ttotDefUseLen = " << totDefUseLen<< endl;
						}
					}
				}
			}
		}
	}

	// Now just do the average
	if (totDefs)
		return totDefUseLen/totDefs;
	else
		return 0;
}

int getDefUseLen(BaseArgument *destArg, BaseInstruction *currInstr, rISABlock rb)
{
	// we have find out the defuse len of each use and find the min
	int minDefUseLen = rb.numInstrs;

	BaseArgProperty *duProp = destArg->getPtrToProperty(_DUCHAIN_);
	if (duProp != NULL)
	{
		MyLinkedListIterator<BaseOperation*>* uIter = ((DUChainProperty*)duProp)->useListIteratorForw();
		while ( uIter->hasMoreElements() )
		{
			BaseOperation *opInDUChain = uIter->currentElement();
			
			int temp = getDefUseLen(currInstr, opInDUChain->getInstr(), rb);
			if (temp < minDefUseLen)
			{
				minDefUseLen = temp;
			}
			uIter->nextElement();
		}
		delete uIter;
	}

	return minDefUseLen;
}

int getDefUseLen(BaseInstruction *def, BaseInstruction *use, rISABlock rb)
{
	int len = 1;
	BaseInstruction *currInstr = def;
	while (currInstr != rb.endInstr)
	{
		currInstr = ((NormalInstr *)currInstr)->getDefaultTargetInstr();
		if (currInstr == use)
			return len;
		len++;
	}
	return len;
}

int getRegPress(BaseInstruction *instr, BasicBlockNode *bb, int index)
{
	RegPressProperty rpProp;
	getRegisterPressure(rpProp, instr, bb);

	int numLive;
	numLive = ((numLive = rpProp.getNumLive(index)) ==_INVALID_) ? 0 : numLive;

	return numLive;
}

int getAvgRegPress(rISABlock rb, int index)
{	
	// This finds out the average register pressure in a block of the register class whose index is porovided.
	// 
	// go thru each instr in the block and add the register pressures
	//
	// Note: We assume that each rISA block is a straight line segment of code.
	//
	//
	BaseInstruction *currInstr = rb.startInstr;
	RegPressProperty rpProp;
	getRegisterPressure(rpProp, currInstr, rb.bb);

	int numLive;
	numLive = ((numLive = rpProp.getNumLive(index)) ==_INVALID_) ? 0 : numLive;
	//
	// numLive = rpProp.getNumLive(). if (numLive == INVALID) numLive = 0

	int totRegPress = numLive;

	if (debugThumb)
	{
		cout << "currInstr = " << currInstr << "\ttotRegPress = " << totRegPress << endl;
	}

	while (currInstr != rb.endInstr)
	{
		rpProp.clear();
		currInstr = ((NormalInstr *)currInstr)->getDefaultTargetInstr();
		getRegisterPressure(rpProp, currInstr, rb.bb);

		numLive = ((numLive = rpProp.getNumLive(index)) ==_INVALID_) ? 0 : numLive;
		//
		// numLive = rpProp.getNumLive(). if (numLive == INVALID) numLive = 0
		totRegPress += numLive;
		
		if (debugThumb)
		{
			cout << "currInstr = " << currInstr << "\ttotRegPress = " << totRegPress << endl;
		}
	}
	if (totRegPress == -1)
		return 0;
	else
		return totRegPress/rb.numInstrs;
}

static int useHeur = 1;

int profitableToConvert(rISABlock rb)
{
	if (useHeur == 0)
	{
		// We calculate the normal - thumb change in code size
		cout << "No of instrs in rISA block = " << rb.numInstrs << endl;

		int codeSizeChange = CONST1 * CS1(rb);
		cout << "Code Size Reduction due to compression = " << CS1(rb) << endl;

		codeSizeChange -= CONST2 * CS2(rb);
		cout << "Code Size Reduction due to mx and nop = " << CS2(rb) << endl;

		codeSizeChange -= CONST3 * CS3(rb);
		cout << "Code Size Reduction due register pressure = " << CS3(rb) << endl;

		cout << "\n Final Code Size Change = " << codeSizeChange << endl;
		
		return codeSizeChange;
	}
	else if (useHeur == 1)
	{
		if (rb.numInstrs > 2)
			return 1;
		return -1;
	}
	else if (useHeur == 2)
	{
		return -1;
	}
	else return 1;
}

