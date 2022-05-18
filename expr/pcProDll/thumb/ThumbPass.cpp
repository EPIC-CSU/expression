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
// File:  			ThumbPass.cpp
// Created:			March 15, 2001.
// Last modified: 	July 30, 2001.
// Author:			Aviral Shrivastava
// Email:			aviral@ics.uci.edu
//
////////////////////////////////////////////////////////

#include "stdafx.h"
#include <ctype.h>
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
#include "SymbolTable.h"
#include "RegPressure.h"
#include "BaseArgument.h"
#include "RegisterFileList.h"
#include "CallOp.h"
#include "RegArchModel.h"
#include "RegSetMapping.h"
#include "RegClassArgProperty.h"
#include "profit_function.h"


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

extern char *prefix(char *s);
extern char *sufix(char *s);

//END MFC DEBUG NEW:  

/*
Notes about the current implementation

	1. Thumb is the technique of recoding instructions into 16 bits, 
	so that the code size is reduced.
  	
	3. Inside the basic block, I collect a list of instructions 
	that can be converted into thumb, and then find the size of the collection.
	If it seems profitable, I convert the entire collection of instructions
	into thumb instructions. Currently a block of 4 or more instructions is 
	considered to be profitable to be converted into thumb mode.
	  
	4. I need to instructions that can be converted into thumb mode into thumb mode,
	so I need a mapping of target instructions and the corresponding thumb instruction.
	I assume I have them in a table right now. Later I need to read the 
	expression description and dump this information in a file and later read that 
	file into this table
		
		  
	Still have to formally decide upon and build a model.
	Complete the flow from EXPRESSION to EXPRESS.

*/

#define TPF 4

// Extern function definitions
//
extern void appendInstrToBB(NormalInstr *newInstr, BasicBlockNode *bb);
extern void prependInstrToBB(NormalInstr *newInstr, BasicBlockNode *bb);
extern void insertInstrBetween(NormalInstr *newInstr, NormalInstr *prevInstr, NormalInstr *nextInstr);
extern void getRegisterPressure(RegPressProperty & rpProp, BaseInstruction *instr, BasicBlockNode *bb);
extern void setRegisterPressures(Routine *rout);
extern BaseArgument *createTempArg(BaseArgument *);
extern BaseArgument *createRISAArg(BaseArgument *);

extern void recompute_USEDEF_properties(Routine *rout);
extern void recompute_LIVEDEAD_properties(Routine *rout);

extern void setRISATypeInRegClassEntryNode2ForArg(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op);
extern void setRISATypeInRegClassEntryNode2ForArg(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op, int classType);
extern void setTempRISATypeInRegClassEntryNode2ForArg(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op);
extern void ReadRegSetMapFile(FILE *);

extern void scheduleForRISA(Routine *rout);

// Extern Data
//
extern Routine *currRoutine;
extern SymbolTable *globalSymbolTable;
extern RegisterFileList *globalRegFileList;
extern RegArchModel globalRegModel;
extern int doneRA;

// Global Data
int indexForTempVars;
int indexForRs;
// Local Data
//
NormalToThumbInstrMap NtoTMap;
thumbInfo GlobalThumbInfo; 
thumbInfoIter titer;
rISABlockList rISABlocks;
int debugThumb = 1;
int ISFormatNumber = 2;
int strcmpci(char *first, char *second);

/* ThumbPass works in four stages

	// 1st Phase: ThumbInit : Create the mapping from normal to thumb instructions
	//
	// 2nd phase: Possible RISA candidate variables are marked.
	//            A priority among the RISAizable blocks is defined.
	//
	// 3rd phase: According to priority, each block is analyzed for profitability.
	//            All RISA candidate variables in the block are converted to actual 
	//            RISA instrs and variables. Also make the transition data structure.
	//
	// 4th phase: Make the TransitionInfo structure, change it to fix the transitions 
	//
*/

int rISASchedule = 0;

void Routine::ThumbPass()
{
	
	//make sure program mappings are up to date
	extern void buildProgramMappings();
	buildProgramMappings();

	indexForTempVars = globalRegFileList->getIndex("temp");
	
	if( !doneRA )
	{
		indexForRs = globalRegFileList->getIndex("gR");
	}
	else
	   {
		cout << "Warning: Target Register set is not yet specified !" << endl;
		indexForRs = globalRegFileList->getIndex("R");
	}

	// phase 0 : Make the Thumb to normal instructions mapping
	//
	if (ThumbInit() == 0)
	{
		printf("Could not build the Thumb to Normal Instruction Mapping quitting...\n");
		exit(0);
	}

	currRoutine = this;
	
	if (debugThumb)
	{
		printf("\n\n");
		printf("###########################################\n");
		printf("Printing the NtoTMap\n");
		printf("###########################################\n");
		printOpcodeToSizeMap();
		printRegSetMappingVector();
		printGenericToTargetMap();
	}
	

	// 1st phase: Possible RISA candidate variables are marked.
	//            A priority among the RISAizable blocks is defined.
	//			  Make the TransitionInfo structure
	//
	rISAphase1(currRoutine);
	if (debugThumb)
	{
		printf("\n\n");
		printf("###########################################\n");
		printf("rISA Phase 2: Converting regs in candidate rISA blocks to temp\n");
		printf("###########################################\n");
		currRoutine->printHTG();
	}

	// added by SAH on 10/01/01
	//
	if (rISASchedule)
	{
		scheduleForRISA(currRoutine);
		//
		// redo rISAphase1 after scheduling.
		//
		rISAphase1(currRoutine);

		if (debugThumb)
		{
			printf("\n\n");
			printf("###########################################\n");
			printf("rISA Scheduling: Finished scheduling bbs for rISA\n");
			printf("###########################################\n");
			currRoutine->printHTG();
		}
	}


	// 2nd phase: According to priority, each block is analyzed for profitability.
	//            All RISA candidate variables in the block are converted to actual 
	//            RISA instrs and variables.
	//			  Initialize the transition structure
	//

	// Added by SAH, AVS on 07/31/01
	//
	// Note: This is a temporoary fix. Remove the following three lines,
	// if and when we migrate to register class type properties for arguments.
	//
	//globalSymbolTable->setStartNums();
	//recompute_USEDEF_properties(currRoutine);
	//recompute_LIVEDEAD_properties(currRoutine);

	rISAphase2(currRoutine);
	
	if (debugThumb)
	{	
		printf("\n\n");
		printf("###########################################\n");
		printf("Printing HTG after converting the profitable blocks to rISA\n");
		printf("###########################################\n");
		currRoutine->printHTG();

		printf("\n\n");
		printf("###########################################\n");
		printf("Printing the ThumbInfo before the fixing the interfaces\n");
		printf("###########################################\n");
		printThumbInfo(currRoutine);
	}


/*
	// 3th phase: Correct the TransitionInfo structure
	//
	//  but even after this a block may lead to a block in different mode
	// we have to correct the transitions, so we mark the blocks
	// do tree transformations on the entry and exits
	//

	rISAphase3(currRoutine);
	if (debugThumb)
	{	
		printf("\n\n");
		printf("###########################################\n");
		printf("Printing the ThumbInfo after fixing the interfaces\n");
		printf("###########################################\n");
		printThumbInfo(currRoutine);
	}


	// 4th phase: Fix the BB's
	//
	// Now actually do the necessary work... could put a check before this
//	rISAphase4(currRoutine);
	if (debugThumb)
	{
		printf("\n\n");
		printf("###########################################\n");
		printf("Printing the routine after fixing the interfaces\n");
		printf("###########################################\n");
		currRoutine->printHTG();
	}
*/	

	// finally, correctly set the global symbol table.
	//
	globalSymbolTable->setStartNums();
}


void rISAphase1(Routine *rout)
{
	// First build the list of rISAizable blocks.
	//
	rISABlocks.clear();

	LinkedListIterator<BasicBlockNode> *iter = (rout->getCFG()).createBBIterForw();

	while (iter->hasMoreElements())
	{
		createRISABlockList(&(iter->currentElement()));

		iter->nextElement();
	}
	delete iter;

	if (debugThumb)
	{
		printRisaInfo(rISABlocks);
	}


	// Next, go thru the rISA block list and change the variables to "can be rISAized"
	//
	rISABlockListIter i1;

	for (i1 = rISABlocks.begin(); i1 != rISABlocks.end(); i1++)
	{
		addRISAProp(*i1, 1);
	}


//	rISABlockListIter i1;
	// Finally, set the priority for each block (and order the list by priority).
	//
	for (i1 = rISABlocks.begin(); i1 != rISABlocks.end(); i1++)
	{
		setPriority(*i1);
	}
	orderRISABlocksByPriority();

}

void rISAphase2(Routine *rout)
{
	// go thru the rISABlocks list and for each block, first perform the profitability analysis.
	// then, convert all the instructions in the block.
	//
	rISABlockListIter i1;

	for (i1 = rISABlocks.begin(); i1 != rISABlocks.end(); i1++)
	{
		(*i1).profitability = profitableToConvert(*i1);
	}

	for (i1 = rISABlocks.begin(); i1 != rISABlocks.end(); i1++)
	{
		if ((*i1).profitability > 0)
		{
			convertToRISA(*i1);
		}
	}
}

void rISAphase3(Routine *rout)
{
	// 3th phase: Correct the TransitionInfo structure
	//
	LinkedListIterator<BasicBlockNode> *iter = (rout->getCFG()).createBBIterForw();
	iter = rout->getCFG().createBBIterForw();
	while (iter->hasMoreElements())
	{
		SetEntryModeOfBB(&(iter->currentElement()));
		SetExitModeOfBB(&(iter->currentElement()));
		iter->nextElement();
	}
}


void rISAphase4(Routine *rout)
{
	// 4th phase: Fix the BB's
	//
	LinkedListIterator<BasicBlockNode> *iter = (rout->getCFG()).createBBIterForw();
	iter = rout->getCFG().createBBIterForw();
	while (iter->hasMoreElements())
	{
		ChangeModeOfBB(&(iter->currentElement()));
		iter->nextElement();
	}
	delete iter;

	// have to correct the entry mode of the first block
	iter = rout->getCFG().createBBIterForw();
	if (GlobalThumbInfo[&(iter->currentElement())].newEntryMode == _THUMB_)
	{
		// now prepend mx instruction 
		MyString opcode = "mx";
		NormalInstr *newInstr = createThumbInstr(opcode);
		prependInstrToBB(newInstr, &(iter->currentElement()));
	}
}

int isDummyInstr(BaseInstruction *instr)
{
	if (instr == currRoutine->getFirstInstr())
		return _YES_;
	return _NO_;
}

void createRISABlockList(BasicBlockNode *currBB)
{
	int ThumbMode = 0;
	BaseInstruction *firstThumbInstr = NULL;
	BaseInstruction *lastThumbInstr = NULL;
	int ThumbCount = 0;
	int ThumbPress = 0;
	
	rISABlock tempRISABlock;
	BaseInstruction *currInstr;

	// iterate over the list of instruction in the basic blocks.
	BBForwIterator bbIter(currBB);
	
	// Ignore the first NOP in the routine.
	//
	if (isDummyInstr(currBB->getFirstInstr()))
		bbIter.nextElement();

	// first set it's entry mode etc..
	GlobalThumbInfo[currBB].oldEntryMode = _NORMAL_;
	GlobalThumbInfo[currBB].oldExitMode = _NORMAL_;
	GlobalThumbInfo[currBB].newEntryMode = _NORMAL_;
	GlobalThumbInfo[currBB].newExitMode = _NORMAL_;

	int rISARegPressThreshold = CONST6 * NUM_RISA_REGS;
	int finishCurrRISABlock = 0;

	while (bbIter.hasMoreElements())
	{
		currInstr = bbIter.currentElement();
		if ((ThumbMode == 0)&&(CanbeThumbed(currInstr)))
		{
			int firstPress = getRegPress(currInstr, currBB, indexForRs);
			if (firstPress <= rISARegPressThreshold)
			{
				// can start a rISA block with this instruction.
				//

				// reset the variables
				ThumbMode = 1;
				ThumbCount = OperationCount(currInstr);
				ThumbPress = getRegPress(currInstr, currBB, indexForRs);
				firstThumbInstr = currInstr;
				lastThumbInstr = currInstr;
			}
			else
			{
				if (debugThumb) // print reason why we did not include it.
				{
					cout << "Instr not included: " << currInstr << " Num Live = " << firstPress << endl;
				}
				// do nothing.
			}
		}	
		else if ((ThumbMode == 0)&&(!CanbeThumbed(currInstr)))
		{
			// do nothing
		}		
		else if ((ThumbMode == 1)&&(CanbeThumbed(currInstr)))
		{
			// check to see if it could be profitable to add this instruction.
			//
			int currPress = getRegPress(currInstr, currBB, indexForRs);
			int currCount = OperationCount(currInstr);
			if ((ThumbPress + currPress) / (ThumbCount + currCount) <= (CONST6 * NUM_RISA_REGS))
			{
				// this instruction can be included in the current rISA block.
				//
				ThumbCount += currCount;
				ThumbPress += currPress;
				lastThumbInstr = currInstr;
			}
			else
			{
				// not profitable to include this instruction in this rISA block.
				//
				finishCurrRISABlock = 1;
			}
		}		
		else if ((ThumbMode == 1)&&(!CanbeThumbed(currInstr)))
		{
			finishCurrRISABlock = 1;
		}

		if (finishCurrRISABlock == 1)
		{
			tempRISABlock.startInstr = firstThumbInstr;
			tempRISABlock.endInstr = lastThumbInstr;
			tempRISABlock.bb = currBB;
			tempRISABlock.numInstrs = ThumbCount;
			tempRISABlock.totalNormalNumLive = ThumbPress;
			tempRISABlock.priority = -1;

			rISABlocks.push_back(tempRISABlock);

			ThumbMode = 0;
			ThumbCount = 0;
			firstThumbInstr = NULL;
			lastThumbInstr = NULL;
			
			finishCurrRISABlock = 0;

			if (debugThumb)
			{
				cout << "Creating new RISA Block:" << endl;
				cout << "Start = " << tempRISABlock.startInstr << "  End = " << tempRISABlock.endInstr << endl;
				cout << "NumInstrs = " << tempRISABlock.numInstrs << " Total NormalNumLive = " << tempRISABlock.totalNormalNumLive << endl;
				cout << endl;
			}

		}

		bbIter.nextElement();
	}

	// We've reached the end of the BB. Check if ThumbMode == 1.
	//
	if (ThumbMode == 1)
	{
			tempRISABlock.startInstr = firstThumbInstr;
			tempRISABlock.endInstr = lastThumbInstr;
			tempRISABlock.bb = currBB;
			tempRISABlock.numInstrs = ThumbCount;
			tempRISABlock.totalNormalNumLive = ThumbPress;

			tempRISABlock.priority = -1;

			rISABlocks.push_back(tempRISABlock);		

			if (debugThumb)
			{
				cout << "Creating new RISA Block:" << endl;
				cout << "Start = " << tempRISABlock.startInstr << "  End = " << tempRISABlock.endInstr << endl;
				cout << "NumInstrs = " << tempRISABlock.numInstrs << " Total NormalNumLive = " << tempRISABlock.totalNormalNumLive << endl;
				cout << endl;
			}
	}
}


void convertVarsToTempRISA(rISABlock & rb)
{
	// go thru each instr in the block and see if any source/dest needs to be marked as "a possible rISA variable"
	//
	// Note: We assume that each rISA block is a straight line segment of code.
	//
	//
	BaseInstruction *currInstr = rb.startInstr;

	convertVarsToTempRISA(currInstr);

	while (currInstr != rb.endInstr)
	{
		currInstr = ((NormalInstr *)currInstr)->getDefaultTargetInstr();

		convertVarsToTempRISA(currInstr);
	}
}


void convertVarsToTempRISA(BaseInstruction *instr)
{
	if (instr == NULL) return;

	convertToRISA(instr, 1, 0);
}	

void addRISAProp(rISABlock & rb, int tempRISA)
{
	// go thru each instr in the block and see if any source/dest needs to be marked.
	//
	// Note: We assume that each rISA block is a straight line segment of code.
	//
	//
	BaseInstruction *currInstr = rb.startInstr;

	addRISAProp(currInstr, tempRISA);

	while (currInstr != rb.endInstr)
	{
		currInstr = ((NormalInstr *)currInstr)->getDefaultTargetInstr();

		addRISAProp(currInstr, tempRISA);
	}
}


void setPriority(rISABlock & rb)
{
	// as of now, do nothing.
}


void orderRISABlocksByPriority()
{
	// need to go thru the risa Block list and sort by priority.
	//
	// as of now, do nothing.
}


void convertToRISA(rISABlock & rb)
{
	// Set the entry mode of the block to THUMB
	// or add the prologue
	//
	if (rb.startInstr == rb.bb->getFirstInstr())
	{
		GlobalThumbInfo[rb.bb].oldEntryMode = _THUMB_;
		GlobalThumbInfo[rb.bb].newEntryMode = _THUMB_;
	}
	else
		addRISAPrologue(rb);

	// go thru each instr in the block 
	// Note: We assume that each rISA block is a straight line segment of code.
	//
	BaseInstruction *currInstr = rb.startInstr;
	convertToRISA(currInstr, 2, 1);

	while (currInstr != rb.endInstr)
	{
		currInstr = ((NormalInstr *)currInstr)->getDefaultTargetInstr();
		convertToRISA(currInstr, 2, 1);
	}

	// Set the exit mode of the block to THUMB
	// or add the epilogue
	//
	if (rb.endInstr == rb.bb->getLastInstr())
	{
		GlobalThumbInfo[rb.bb].oldExitMode = _THUMB_;
		GlobalThumbInfo[rb.bb].newExitMode = _THUMB_;
	}
	else
		addRISAEpilogue(rb);
}


void convertToRISA(BaseInstruction *instr, int phase1or2, int changeOpcode)
{
	
	ArrayIterator<DataOpSlot> *diter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (diter->hasMoreElements())
	{
		if ((diter->currentElement()).hasOper())
		{
			convertToRISA((diter->currentElement()).getPtrToOperation(), phase1or2, changeOpcode);
		}
		diter->nextElement();
	}
	delete diter;

	ArrayIterator<ControlOpSlot> *citer = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (citer->hasMoreElements())
	{
		if ((citer->currentElement()).hasOper())
		{
			convertToRISA((citer->currentElement()).getPtrToOperation(), phase1or2, changeOpcode);
		}
		citer->nextElement();
	}
	delete citer;
}



void convertToRISA(BaseOperation *oper, int phase1or2, int changeOpcode)
{
	// begin partha on 08/25/01
	ArgList destArgList, srcArgList;
	ArgListIter aIter;
	BaseArgument *srcArg, *destArg;
	BaseArgProperty *prop;
	Vector<RegClassEntryNode2> regClasses;
	rmapType srcmap, dstmap;
	rmapVectType rsClassSet, rdClassSet;
	rmapVectTwoDim inputSrcRegSet, inputDstRegSet;
	MyString opc;
	int slen = 0, dlen = 0, pcount = 0;
	RegSetMappingType targetOpMap;

	oper->getOpcodeName(opc); // debug
	// set reg class properties..
	// 
	oper->destOperandsList(destArgList);
	
	// traverse all the destination arguments and set the regClasses
	for (aIter = destArgList.begin(); aIter != destArgList.end(); aIter++)
	{
		if ((*aIter) != NULL)
		{
			destArg = *aIter;
			prop = destArg->getPtrToProperty(_REGCLASS_);
			// get all the register classes associated with the argument
			regClasses = ((RegClassArgProperty *)prop)->getClasses();
			

			// there might be more than one register classes associated with 
			//  a given argument, the size being equal to the vector size


			Vector<RegClassEntryNode2>::iterator iter;
			for( iter = regClasses.begin(); iter != regClasses.end(); iter++ )
			{
				dstmap.rclass = (*iter)._cType;
				dstmap.rdata = (*iter)._dType;
				rdClassSet.push_back((rmapType)dstmap);
			}
			// insert into the two-dimensional vector the destination register classes
			inputDstRegSet.push_back((rmapVectType)rdClassSet);
			dlen++;
		}
	}
	
	oper->sourceOperandsList(srcArgList);
	// traverse all the source arguments and set the regClasses
	for (aIter = srcArgList.begin(); aIter != srcArgList.end(); aIter++)
	{
		if ((*aIter) != NULL)
		{
			srcArg = *aIter;

			if( srcArg->isRegisterArg() )
			{
				prop = srcArg->getPtrToProperty(_REGCLASS_);
				// get all the register classes associated with the argument
				regClasses = ((RegClassArgProperty *)prop)->getClasses();
				
				int classlen = regClasses.size();
				// there might be more than one register classes associated with 
				//  a given argument, the size being equal to the vector size
				
				Vector<RegClassEntryNode2>::iterator iter;
				for( iter = regClasses.begin(); iter != regClasses.end(); iter++ )
				{
					srcmap.rclass = (*iter)._cType;
					srcmap.rdata = (*iter)._dType;
					rsClassSet.push_back((rmapType)srcmap);
				}
			}
			else
			{
				srcmap.rclass = globalRegModel.getClassTypeIndex("IMM", _GENERIC_);
				srcmap.rdata = globalRegModel.getDataTypeIndex("INT", _GENERIC_);
				rsClassSet.push_back((rmapType)srcmap);
			}
			
			// insert into the two-dimensional vector the source register classes
			inputSrcRegSet.push_back((rmapVectType)rsClassSet);
			slen++;
		}
	}

	// change the opcode subject to checks for matching register classes
	//
	
	if (changeOpcode)
	{
		MyString opcode;
		char *targetOpCode;

		oper->getOpcodeName(opcode);
		char *temp = (char *) malloc(sizeof(opcode.getStr())+1);
		strcpy(temp, opcode.getStr());
		
		// Get the target opcode mapping 
		RegSetMappingVectorType RegSetMap;
		getMatchingTargets(temp, RegSetMap);
		
		// get the best targetOp for the generic Ops
		//
		
		if( ISFormatNumber == 0 )
		{
			getBestTargetOpForGeneric_444(RegSetMap, oper, targetOpMap);
		}
		else if( ISFormatNumber == 1 )
		{
			getBestTargetOpForGeneric_733(RegSetMap, oper, targetOpMap);
		}
		else if( ISFormatNumber == 2 )
		{
			getBestTargetOpForGeneric_implicit(RegSetMap, oper, targetOpMap);
		}		

		targetOpCode = strdup(targetOpMap.opcode);
		// targetOpCode = strdup(getTargetOpCode(temp));
		// oper->changeOpCode(NtoTMap[opcode].getStr());
		oper->changeOpCode(targetOpCode);
	}

	inputSrcRegSet.clear();
	inputDstRegSet.clear();

	// end partha on 08/25/01

	// now restrict the register set
	//
	// Modified by SAH on 08/21/01.
	//
	// We now have a RegClassProperties that specify the restrictions
	// on the final register mappings for each argument
	//
	// for each definition add a risa regclass argument
	// risa property can be RISA2, RISA4, RISA8 or RISA16 : PB on 11/28/01
	
	ArgList & dList = oper->destOperandList();
	ArgListIter di;
	rmapVectTwoDim srcMapVector = targetOpMap.srcMap;
	rmapVectTwoDim dstMapVector = targetOpMap.dstMap;
	int count = 0;
	for (di = dList.begin(); di != dList.end(); di++)
	{
		if ((*di) == NULL) continue;
		rmapVectType::iterator i;
		for( i = dstMapVector[count].begin(); i != dstMapVector[count].end(); i++ )
		{
			int classType = (int)(*i).rclass;
		    addNewRISARegClassProperty(*di, oper, 1, classType); // arg, oper, destOrSrc, classType
		}
		count++;
	}
	
	ArgList & sList = oper->sourceOperandList();
	ArgListIter si;
	count = 0;
	for (si = sList.begin(); si != sList.end(); si++)
	{
		if ((*si) == NULL) continue;
		rmapVectType::iterator i;
		for( i = srcMapVector[count].begin(); i != srcMapVector[count].end(); i++ )
		{
			int classType = (int)(*i).rclass;
		    addNewRISARegClassProperty(*si, oper, 0, classType); // arg, oper, destOrSrc, classType
		}
		count++;
	}

/*
	ArgList & dList = oper->destOperandList();
	ArgListIter di;

	for (di = dList.begin(); di != dList.end(); di++)
	{
		if ((*di) == NULL) continue;

		addRISARegClassProperty(*di, oper, 0, 1); // arg, oper, tempRISA, destOrSrc
	}

	ArgList & sList = oper->sourceOperandList();
	ArgListIter si;

	for (si = sList.begin(); si != sList.end(); si++)
	{
		if ((*si) == NULL) continue;

		addRISARegClassProperty(*si, oper, 0, 0); // arg, oper, tempRISA, destOrSrc
	}

	
	// first the destinations
	// 1. Collect the destination args
	
	oper->destOperandsList(destArgList);

	// 2. iterate over the destArgs and convert to temps
	
	for (aIter = destArgList.begin(); aIter != destArgList.end(); aIter++)
	{
		if ((*aIter) != NULL)
		{
			BaseArgument *destArg = *aIter;
			BaseArgProperty *prop = destArg->getPtrToProperty(_SSA_);
			
			BaseArgument *origArg = destArg->copy();

			// generate a new temporary
			BaseArgument *tempArg;
			if (phase1or2 == 1)
				tempArg = createTempArg(destArg);
			else // if (phase1or2 == 2)
				tempArg = createRISAArg(destArg);
			
			
			convertToTemp(destArg, tempArg);
			// iterate over its DU Chain, and recursively convert its sources
			//
			//
			BaseOperation *opInDUChain;
			BaseArgProperty *prop2 = destArg->getPtrToProperty(_DUCHAIN_);
			if (prop2 != NULL)
			{
				MyLinkedListIterator<BaseOperation*>* iter1 = ((DUChainProperty*)prop2)->useListIteratorForw();
				while ( iter1->hasMoreElements() )
				{
					opInDUChain = iter1->currentElement();
					recursiveConvertToTemp(opInDUChain, origArg, destArg, 0);
					
					iter1->nextElement();
				}
				delete iter1;
			}

			if(prop != NULL)
			{
				// propagate this change to the global symbol table
				//
				int ssaNum = ((SSAArgProperty *)prop)->getSSANum();
				globalSymbolTable->addNewRegs(((RealRegArg*)tempArg), ssaNum);
			}
			
			// delete the temporary.
			//
			delete tempArg;
		}
	}

		// now the sources
	// 1. Collect the source args
	ArgList srcArgList;
	oper->sourceOperandsList(srcArgList);
	
	// 2. iterate over the srcArgs and convert to temps
	for (aIter = srcArgList.begin(); aIter != srcArgList.end(); aIter++)
	{
		if ((*aIter) != NULL)
		{
			BaseArgument *srcArg = *aIter;
			BaseArgProperty *prop = srcArg->getPtrToProperty(_SSA_);
			
			BaseArgument *origArg = srcArg->copy();
			
			// generate a new temporary

			BaseArgument *tempArg;
			if (phase1or2 == 1)
				tempArg = createTempArg(srcArg);
			else // if (phase1or2 == 2)
				tempArg = createRISAArg(srcArg);

			
			convertToTemp(srcArg, tempArg);
			// iterate over its DU Chain, and recursively convert its sources
			//
			//
			BaseOperation *opInUDChain;
			BaseArgProperty *prop2 = srcArg->getPtrToProperty(_UDCHAIN_);
			if (prop2 != NULL)
			{
				MyLinkedListIterator<BaseOperation*>* iter1 = ((UDChainProperty*)prop2)->defListIteratorForw();
				while ( iter1->hasMoreElements() )
				{
					opInUDChain = iter1->currentElement();
					recursiveConvertToTemp(opInUDChain, origArg, srcArg, 1);
					
					iter1->nextElement();
				}
				delete iter1;
			}
			
			if(prop != NULL)
			{
				// propagate this change to the global symbol table
				//
				int ssaNum = ((SSAArgProperty *)prop)->getSSANum();
				globalSymbolTable->addNewRegs(((RealRegArg*)tempArg), ssaNum);
			}

			// delete the temporary.
			//
			delete tempArg;
		}
	}
*/
}

void addRISAProp(BaseInstruction *instr, int tempRISA)
{
	ArrayIterator<DataOpSlot> *diter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (diter->hasMoreElements())
	{
		if ((diter->currentElement()).hasOper())
		{
			recursiveAddRISAProp((diter->currentElement()).getPtrToOperation(), tempRISA, 1);
			recursiveAddRISAProp((diter->currentElement()).getPtrToOperation(), tempRISA, 0);
		}
		diter->nextElement();
	}
	delete diter;

	ArrayIterator<ControlOpSlot> *citer = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (citer->hasMoreElements())
	{
		if ((citer->currentElement()).hasOper())
		{
			recursiveAddRISAProp((citer->currentElement()).getPtrToOperation(), tempRISA, 1);
			recursiveAddRISAProp((citer->currentElement()).getPtrToOperation(), tempRISA, 0);
		}
		citer->nextElement();
	}
	delete citer;
}

void recursiveAddRISAProp(BaseOperation *oper, int tempRISA, int destOrSrc, BaseArgument *origArg)
{
	if (oper == NULL)return;

	// destOrSrc == 1 => destination, destOrSrc == 0 => source

	if (destOrSrc == 1)
	{
		// first iterate over its destination list, and find the dest that is equal to origId.
		//
		// for each dest that is found, also go thru its DU chain, and convert the sources.
		//

		ArgList & dL = oper->destOperandList();
		ArgListIter dLiter;

		for (dLiter = dL.begin(); dLiter != dL.end(); dLiter++)
		{
			if ((*dLiter) == NULL) continue;

			if ((*dLiter)->isRealRegArg() == _NO_) continue; // error condition??

			if ((origArg == NULL) || ((*dLiter)->isEq(origArg))) // then, found one that needs to be converted.
			{
				if (addRISARegClassProperty((*dLiter), oper, tempRISA, destOrSrc) == _NO_) // the RISA property was already present
					continue;

				// iterate over its DU Chain, and recursively addProp to its sources
				//
				//
				BaseOperation *opInDUChain;
				BaseArgProperty *prop = (*dLiter)->getPtrToProperty(_DUCHAIN_);
				if (prop != NULL)
				{
					MyLinkedListIterator<BaseOperation*>* iter1 = ((DUChainProperty*)prop)->useListIteratorForw();
					while ( iter1->hasMoreElements() )
					{
						opInDUChain = iter1->currentElement();
						recursiveAddRISAProp(opInDUChain, tempRISA, 0, *dLiter);

						iter1->nextElement();
					}
					delete iter1;
				}
			}
		}
	}
	else
	{
		// first iterate over its source list, and find the source that is equal to origId.
		//
		// for each source that is found, also go thru its UD chain, and convert the dests.
		//

		ArgList & sL = oper->sourceOperandList();
		ArgListIter sLiter;

		for (sLiter = sL.begin(); sLiter != sL.end(); sLiter++)
		{
			if ((*sLiter) == NULL) continue;

			if ((*sLiter)->isRealRegArg() == _NO_) continue;

			if ((origArg == NULL) || ((*sLiter)->isEq(origArg))) // then, found one that needs to be converted.
			{
				if (addRISARegClassProperty((*sLiter), oper, tempRISA, destOrSrc) == _NO_) // the RISA property was already present
					continue;

				// iterate over its UD Chain and recursively convert its dests
				//
				//
				BaseOperation *opInUDChain;
				BaseArgProperty *prop = (*sLiter)->getPtrToProperty(_UDCHAIN_);

				if (prop != NULL)
				{
					MyLinkedListIterator<BaseOperation*>* iter2 = ((UDChainProperty*)prop)->defListIteratorForw();
					while ( iter2->hasMoreElements() )
					{
						opInUDChain = iter2->currentElement();
						recursiveAddRISAProp(opInUDChain, tempRISA, 1, *sLiter);

						iter2->nextElement();
					}
					delete iter2;
				}
			}
		}

		// if the operation is a call operation, need to go thru its parm list.
		//
		if (oper->isCallOp())
		{
			ArgList & pL = ((CallRetOp *)oper)->getParmList();
			ArgListIter pLiter;

			for (pLiter = pL.begin(); pLiter != pL.end(); pLiter++)
			{
				if ((*pLiter) == NULL) continue;

				if ((*pLiter)->isRealRegArg() == _NO_) continue;

				if ((origArg == NULL) || ((*pLiter)->isEq(origArg))) // then, found one that needs to be converted.
				{
					if (addRISARegClassProperty((*pLiter), oper, tempRISA, destOrSrc) == _NO_) // the RISA property was already present
						continue;

					// iterate over its UD Chain and recursively convert its dests
					//
					//
					BaseOperation *opInUDChain;
					BaseArgProperty *prop = (*pLiter)->getPtrToProperty(_UDCHAIN_);

					if (prop != NULL)
					{
						MyLinkedListIterator<BaseOperation*>* iter2 = ((UDChainProperty*)prop)->defListIteratorForw();
						while ( iter2->hasMoreElements() )
						{
							opInUDChain = iter2->currentElement();
							recursiveAddRISAProp(opInUDChain, tempRISA, 1, *pLiter);

							iter2->nextElement();
						}
						delete iter2;
					}
				}
			}
		}
	}
}

// return _NO_ if the property was already present in arg.
// else add property and return _YES_
//
int addRISARegClassProperty(BaseArgument *arg, BaseOperation *op, int tempRISA, int destOrSrc)
{
	RegClassArgProperty tmp;
	RegClassEntryNode2 cls;

	// if tempRISA == 1 add tempRISA property; else add RISA property.
	//
	// destOrSrc == 1 => DEST, else SOURCE.
	//
	if (tempRISA == 1)
	{
		if (destOrSrc == 1)
			setTempRISATypeInRegClassEntryNode2ForArg(cls, arg, op);
		else
			setTempRISATypeInRegClassEntryNode2ForArg(cls, arg, op);
	}
	else
	{
		if (destOrSrc == 1)
			setRISATypeInRegClassEntryNode2ForArg(cls, arg, op);
		else
			setRISATypeInRegClassEntryNode2ForArg(cls, arg, op);
	}

	BaseArgProperty *prop;
	if ((prop = arg->getPtrToProperty(_REGCLASS_)) != NULL)
	{
		// merge the set of properties already present in arg.
		//
		// merge should return 1 if the particular reg class (TempRISA) was not already present in arg.
		// else returns 0.
		//
		Vector<RegClassEntryNode2> tempVect;
		tempVect.push_back(cls);
		return ((globalRegModel.mergeListsOfRegClassEntryNode2(((RegClassArgProperty *)prop)->getClasses(), tempVect, _GENERIC_) == 1) ? _YES_ : _NO_);
	}
	else
	{
		tmp.addClass(cls);
		arg->addProperty(tmp);
		return _YES_;
	}
}

// takes care of whether the classType is risa2, risa4, risa8 or risa16
int addNewRISARegClassProperty(BaseArgument *arg, BaseOperation *op, int destOrSrc, int classType)
{
	RegClassArgProperty tmp;
	RegClassEntryNode2 cls;
	
	// if tempRISA == 1 add tempRISA property; else add RISA property.
	//
	// destOrSrc == 1 => DEST, else SOURCE.
	//
	
	if (destOrSrc == 1)
		setRISATypeInRegClassEntryNode2ForArg(cls, arg, op, classType);
	else
		setRISATypeInRegClassEntryNode2ForArg(cls, arg, op, classType);
	
	BaseArgProperty *prop;
	if ((prop = arg->getPtrToProperty(_REGCLASS_)) != NULL)
	{
		// merge the set of properties already present in arg.
		//
		// merge should return 1 if the particular reg class (TempRISA) was not already present in arg.
		// else returns 0.
		//
		Vector<RegClassEntryNode2> tempVect;
		tempVect.push_back(cls);
		return ((globalRegModel.mergeListsOfRegClassEntryNode2(((RegClassArgProperty *)prop)->getClasses(), tempVect, _GENERIC_) == 1) ? _YES_ : _NO_);
	}
	else
	{
		tmp.addClass(cls);
		arg->addProperty(tmp);
		return _YES_;
	}
}

void addRISAPrologue(rISABlock & rb)
{
 	// add a BX instruction before the first Thumb instruction
	MyString opcode = "mx";
	NormalInstr *MXInstr = createThumbInstr(opcode);
	if (rb.startInstr == (rb.bb)->getFirstInstr())
	{
		prependInstrToBB(MXInstr, rb.bb);
	}
	else
	{
		// find the previous instr.
		//
		BaseInstruction *prevInstr;
		BBBackIterator iter(rb.bb);
		//
		while (iter.hasMoreElements())
		{
			if (iter.currentElement() == rb.startInstr)
			{
				iter.prevElement();
				prevInstr = iter.currentElement();
				break;
			}
			iter.prevElement();
		}

		// Need to insert BXInstr between prevInstr and firstThumbInstr
		//
		insertInstrBetween((NormalInstr *)MXInstr, (NormalInstr *)prevInstr, (NormalInstr *)(rb.startInstr)); 
	}
}


void addRISAEpilogue(rISABlock & rb)
{
	// add a BXT instruction  at the last
	MyString opcode = "mx_r";
	NormalInstr *MX_RInstr = createThumbInstr(opcode);
	if (rb.endInstr == (rb.bb)->getLastInstr())
	{
		appendInstrToBB(MX_RInstr, rb.bb);
	}
	else
	{
		// find the next instr.
		//
		BaseInstruction *nextInstr;
		BBForwIterator iter(rb.bb);
		//
		while (iter.hasMoreElements())
		{
			if (iter.currentElement() == rb.endInstr)
			{
				iter.nextElement();
				nextInstr = iter.currentElement();
				break;
			}
			iter.nextElement();
		}
		insertInstrBetween((NormalInstr *)MX_RInstr, (NormalInstr *)(rb.endInstr), (NormalInstr *)nextInstr);
	}
}


void SetEntryModeOfBB(BasicBlockNode *currBB)
{
	Mode currBlockEntryMode = GlobalThumbInfo[currBB].newEntryMode;
	MyLinkedListIterator<BasicBlockNode *> *predListIter = currBB->getCFPred().createPredListIterForw();
	int thumbModeCount = 0;
	int normalModeCount = 0;

	// first count the number of thumb and normal predecessors
	if (predListIter)
	{
		while (predListIter->hasMoreElements())
		{	
			if (GlobalThumbInfo[predListIter->currentElement()].newExitMode == _THUMB_)
				thumbModeCount++;
			else
				normalModeCount++;
			predListIter->nextElement();
		}
		
		
		if (GlobalThumbInfo[currBB].newEntryMode == _THUMB_)
		{
			if (thumbModeCount == 0)
			{
				// N N -> T   --->  N N -> N
				GlobalThumbInfo[currBB].newEntryMode = _NORMAL_;
			}
			
			else if (normalModeCount == 0)
			{
				// T T -> T  ---> T T -> T
				// do nothing
			}
			
			else
			{
				// N T -> T  ----> N N -> N
				GlobalThumbInfo[currBB].newEntryMode = _NORMAL_;
				
				MyLinkedListIterator<BasicBlockNode *> *predIter = currBB->getCFPred().createPredListIterForw();
				while(predIter->hasMoreElements())
				{
					if (GlobalThumbInfo[predIter->currentElement()].newExitMode == _THUMB_)
						GlobalThumbInfo[predIter->currentElement()].newExitMode = _NORMAL_;
					predIter->nextElement();
				}
			}
		}
		else // (GlobalThumbInfo[currBB].newEntryMode == _NORMAL_)
		{
			if (thumbModeCount == 0)
			{
				// N N -> N  ---> N N -> N
				// do nothing
			}
			
			else if (normalModeCount == 0)
			{
				// T T -> N  ---> T T -> T
				GlobalThumbInfo[currBB].newEntryMode = _THUMB_;
			}
			
			else
			{
				// N T -> N  ---> N N -> N
				MyLinkedListIterator<BasicBlockNode *> *predIter = currBB->getCFPred().createPredListIterForw();
				while(predIter->hasMoreElements())
				{
					if (GlobalThumbInfo[predIter->currentElement()].newExitMode == _THUMB_)
						GlobalThumbInfo[predIter->currentElement()].newExitMode = _NORMAL_;
					predIter->nextElement();
				}
			}
		}
	}
}


void SetExitModeOfBB(BasicBlockNode *currBB)
{
	BasicBlockNode *firstSucc = currBB->getFirstSucc();
	BasicBlockNode *secondSucc = currBB->getNextSucc();
	
	if (firstSucc)
	{
		if (secondSucc)
		{
			// has both the successors
			if (GlobalThumbInfo[currBB].newExitMode == _THUMB_)
			{
				if ((GlobalThumbInfo[firstSucc].newEntryMode == _THUMB_)&&(GlobalThumbInfo[secondSucc].newEntryMode == _THUMB_))
				{
					// T -> T T  ---> T -> T T
					// do nothing
				}
				
				else if ((GlobalThumbInfo[firstSucc].newEntryMode == _THUMB_)&&(GlobalThumbInfo[secondSucc].newEntryMode == _NORMAL_))
				{
					// T -> T N  ---> N -> N N
					GlobalThumbInfo[currBB].newExitMode = _NORMAL_;
					GlobalThumbInfo[firstSucc].newEntryMode = _NORMAL_;
				}
				
				else if ((GlobalThumbInfo[firstSucc].newEntryMode == _NORMAL_)&&(GlobalThumbInfo[secondSucc].newEntryMode == _THUMB_))
				{
					// T -> N T  ---> N -> N N
					GlobalThumbInfo[currBB].newExitMode = _NORMAL_;
					GlobalThumbInfo[secondSucc].newEntryMode = _NORMAL_;
				}
				
				else
				{
					// T -> N N  ---> N -> N N
					GlobalThumbInfo[currBB].newExitMode = _NORMAL_;
				}
			}

			else// if (GlobalThumbInfo[currBB].newExitMode == _NORMAL_)
			{
				if ((GlobalThumbInfo[firstSucc].newEntryMode == _THUMB_)&&(GlobalThumbInfo[secondSucc].newEntryMode == _THUMB_))
				{
					// N -> T T  ---> N -> N N
					GlobalThumbInfo[firstSucc].newEntryMode = _NORMAL_;
					GlobalThumbInfo[secondSucc].newEntryMode = _NORMAL_;
				}
				
				else if ((GlobalThumbInfo[firstSucc].newEntryMode == _THUMB_)&&(GlobalThumbInfo[secondSucc].newEntryMode == _NORMAL_))
				{
					// N -> T N  ---> N -> N N
					GlobalThumbInfo[firstSucc].newEntryMode = _NORMAL_;
				}
				
				else if ((GlobalThumbInfo[firstSucc].newEntryMode == _NORMAL_)&&(GlobalThumbInfo[secondSucc].newEntryMode == _THUMB_))
				{
					// N -> N T  ---> N -> N N
					GlobalThumbInfo[secondSucc].newEntryMode = _NORMAL_;
				}
				
				else
				{
					// N -> N N  ---> N -> N N
					// do nothing
				}
			}
		}
		else // has only one successor
		{
			if (GlobalThumbInfo[currBB].newExitMode == _THUMB_)
			{
				if (GlobalThumbInfo[currBB->getFirstSucc()].newEntryMode == _THUMB_)
				{
					// T -> T   ---> T -> T
					// do nothing
				}
				else
				{
					/// T -> N   ---> N -> N
					GlobalThumbInfo[currBB].newExitMode = _NORMAL_;
				}
			}
			else //if (GlobalThumbInfo[currBB].newExitMode == _NORMAL_)
			{
				if (GlobalThumbInfo[currBB->getFirstSucc()].newEntryMode == _THUMB_)
				{
					// N -> T   ---> N -> N
					GlobalThumbInfo[firstSucc].newEntryMode = _NORMAL_;
				}
				else
				{
					/// N -> N   ---> N -> N
					// do nothing
				}
			}
		}
	}
}


void ChangeModeOfBB(BasicBlockNode *currBB)
{
	if (GlobalThumbInfo[currBB].oldEntryMode == GlobalThumbInfo[currBB].newEntryMode)
	{
		// do nothing
	}
	else 
	{
		if (GlobalThumbInfo[currBB].oldEntryMode == _NORMAL_)
		{
			// change entry mode from NORMAL to THUMB
			MyString opcode = "mx_r";
			NormalInstr *newInstr = createThumbInstr(opcode);
			prependInstrToBB(newInstr, currBB);
		}
		else
		{
			// change entry mode from THUMB to NORMAL
			MyString opcode = "mx";
			NormalInstr *newInstr = createThumbInstr(opcode);
			prependInstrToBB(newInstr, currBB);
		}
	}

	if (GlobalThumbInfo[currBB].oldExitMode == GlobalThumbInfo[currBB].newExitMode)
	{ 
		// do nothing 
	}
	else
	{
		if (GlobalThumbInfo[currBB].oldExitMode == _THUMB_)
		{
			// convert the last instruction to normal
			// insert a mx_r before the last instruction

			NormalInstr *lastInstr = (NormalInstr *) currBB->getLastInstr();
			ConvertInstrToNormal(lastInstr);

			NormalInstr *firstInstr = (NormalInstr *) currBB->getFirstInstr();
			if (firstInstr == lastInstr)
			{
				printf("I had hoped this condition does not come\n");
			}
			else
			{
				NormalInstr *secondLastInstr = (NormalInstr *)getPrev(lastInstr, firstInstr);
				MyString opcode = "mx_r";
				NormalInstr *newInstr = createThumbInstr(opcode);
				insertInstrBetween(newInstr, secondLastInstr, lastInstr);
			}

		}
		else
		{
			printf("There's an error. This condition cannnot occur\n");
		}
	}
}


// if dstOrSrc == 1, iterate over the destination list only.
// else iterate over the source list only.
//
void recursiveConvertToTemp(BaseOperation *oper, BaseArgument *origArg, BaseArgument *tempArg, int destOrSrc)
{
	if ((oper == NULL) || (origArg == NULL) || (tempArg == NULL)) return;

	if (origArg->isEq(tempArg)) return;

	if (destOrSrc == 1)
	{
		// first iterate over its destination list, and find the dest that is equal to origId.
		//
		// for each dest that is found, also go thru its DU chain, and convert the sources.
		//

		ArgList & dL = oper->destOperandList();
		ArgListIter dLiter;

		for (dLiter = dL.begin(); dLiter != dL.end(); dLiter++)
		{
			if ((*dLiter) == NULL) continue;

			if ((*dLiter)->isRealRegArg() == _NO_) continue; // error condition??

			if ((*dLiter)->isEq(origArg)) // then, found one that needs to be converted.
			{
				convertToTemp((*dLiter), tempArg);

				// iterate over its DU Chain, and recursively convert its sources
				//
				//
				BaseOperation *opInDUChain;
				BaseArgProperty *prop = (*dLiter)->getPtrToProperty(_DUCHAIN_);
				if (prop != NULL)
				{
					MyLinkedListIterator<BaseOperation*>* iter1 = ((DUChainProperty*)prop)->useListIteratorForw();
					while ( iter1->hasMoreElements() )
					{
						opInDUChain = iter1->currentElement();
						recursiveConvertToTemp(opInDUChain, origArg, tempArg, 0);

						iter1->nextElement();
					}
					delete iter1;
				}
			}
		}
	}
	else
	{
		// first iterate over its source list, and find the source that is equal to origId.
		//
		// for each source that is found, also go thru its UD chain, and convert the dests.
		//

		ArgList & sL = oper->sourceOperandList();
		ArgListIter sLiter;

		for (sLiter = sL.begin(); sLiter != sL.end(); sLiter++)
		{
			if ((*sLiter) == NULL) continue;

			if ((*sLiter)->isRealRegArg() == _NO_) continue;

			if ((*sLiter)->isEq(origArg)) // then, found one that needs to be converted.
			{
				convertToTemp((*sLiter), tempArg);

				// iterate over its UD Chain and recursively convert its dests
				//
				//
				BaseOperation *opInUDChain;
				BaseArgProperty *prop = (*sLiter)->getPtrToProperty(_UDCHAIN_);

				if (prop != NULL)
				{
					MyLinkedListIterator<BaseOperation*>* iter2 = ((UDChainProperty*)prop)->defListIteratorForw();
					while ( iter2->hasMoreElements() )
					{
						opInUDChain = iter2->currentElement();
						recursiveConvertToTemp(opInUDChain, origArg, tempArg, 1);

						iter2->nextElement();
					}
					delete iter2;
				}
			}
		}

		// if the operation is a call operation, need to go thru its parm list.
		//
		if (oper->isCallOp())
		{
			ArgList & pL = ((CallRetOp *)oper)->getParmList();
			ArgListIter pLiter;

			for (pLiter = pL.begin(); pLiter != pL.end(); pLiter++)
			{
				if ((*pLiter) == NULL) continue;

				if ((*pLiter)->isRealRegArg() == _NO_) continue;

				if ((*pLiter)->isEq(origArg)) // then, found one that needs to be converted.
				{
					convertToTemp((*pLiter), tempArg);

					// iterate over its UD Chain and recursively convert its dests
					//
					//
					BaseOperation *opInUDChain;
					BaseArgProperty *prop = (*pLiter)->getPtrToProperty(_UDCHAIN_);

					if (prop != NULL)
					{
						MyLinkedListIterator<BaseOperation*>* iter2 = ((UDChainProperty*)prop)->defListIteratorForw();
						while ( iter2->hasMoreElements() )
						{
							opInUDChain = iter2->currentElement();
							recursiveConvertToTemp(opInUDChain, origArg, tempArg, 1);

							iter2->nextElement();
						}
						delete iter2;
					}
				}
			}
		}
	}
}

void convertToTemp(BaseArgument *arg, BaseArgument *tempArg)
{
	if (arg->isRealRegArg() == _NO_) return;

	((RealRegArg *)arg)->changeReg(tempArg);
}


// Is a misnomer. 
// This function creates and returns a BX operation, or a BXT operation, or a NOPT operation.
NormalInstr *createThumbInstr(MyString opcode)
{
	OpCode *opc;
		
	ComputeOp *mxOp= new ComputeOp();
	opc = new OpCode(opcode.getStr());
	mxOp->initOpCode(*opc);
	mxOp->setID(0);
	
	NormalInstr *mxInstr = new NormalInstr();
	mxInstr->initScheduleAndAdd(mxOp, true);
	mxInstr->setID(0);
	return mxInstr;
}


int CanbeThumbed(BaseInstruction *instr);

int OperationCount(BaseInstruction *instr)
{
	int retVal = ((NormalInstr *)instr)->countDataOps();
	retVal += ((NormalInstr *)instr)->countControlOps();

	return retVal;
}

// This function should look at the feasibility of conversion 
// by checking the arguments also.

int CanbeThumbed(BaseOperation *op)
{
	NtoTIter i;
	MyString opcode;
	op->getOpcodeName(opcode);

	return isFeasibleToConvert(op);
}

int isFeasibleToConvert(BaseOperation *oper)
{
	// begin partha on 08/25/01
	ArgList destArgList, srcArgList;
	ArgListIter aIter;
	BaseArgument *srcArg, *destArg;
	BaseArgProperty *prop;
	Vector<RegClassEntryNode2> regClasses;
	rmapType srcmap, dstmap;
	
	rmapVectTwoDim inputSrcRegSet, inputDstRegSet;
	MyString opc;
	int slen = 0, dlen = 0, pcount = 0;

	oper->getOpcodeName(opc); // debug
	// set reg class properties..
	// 
	oper->destOperandsList(destArgList);
	
	// traverse all the destination arguments and set the regClasses
	for (aIter = destArgList.begin(); aIter != destArgList.end(); aIter++)
	{
		rmapVectType rdClassSet;
		if ((*aIter) != NULL)
		{
			destArg = *aIter;
			prop = destArg->getPtrToProperty(_REGCLASS_);
			// get all the register classes associated with the argument
			regClasses = ((RegClassArgProperty *)prop)->getClasses();
			

			// there might be more than one register classes associated with 
			//  a given argument, the size being equal to the vector size


			Vector<RegClassEntryNode2>::iterator iter;
			for( iter = regClasses.begin(); iter != regClasses.end(); iter++ )
			{
				dstmap.rclass = (*iter)._cType;
				dstmap.rdata = (*iter)._dType;
				rdClassSet.push_back((rmapType)dstmap);
			}
			// insert into the two-dimensional vector the destination register classes
			inputDstRegSet.push_back((rmapVectType)rdClassSet);
			rdClassSet.clear();
			dlen++;
		}
	}
	
	oper->sourceOperandsList(srcArgList);
	// traverse all the source arguments and set the regClasses
	for (aIter = srcArgList.begin(); aIter != srcArgList.end(); aIter++)
	{
		rmapVectType rsClassSet;
		if ((*aIter) != NULL)
		{
			srcArg = *aIter;

			if( srcArg->isRegisterArg() )
			{
				prop = srcArg->getPtrToProperty(_REGCLASS_);
				// get all the register classes associated with the argument
				regClasses = ((RegClassArgProperty *)prop)->getClasses();
				

				int classlen = regClasses.size();
				// there might be more than one register classes associated with 
				//  a given argument, the size being equal to the vector size
				
				Vector<RegClassEntryNode2>::iterator iter;
				for( iter = regClasses.begin(); iter != regClasses.end(); iter++ )
				{
					srcmap.rclass = (*iter)._cType;
					srcmap.rdata = (*iter)._dType;
					rsClassSet.push_back((rmapType)srcmap);
				}
			}
			else
			{
				srcmap.rclass = globalRegModel.getClassTypeIndex("IMM", _GENERIC_);
				srcmap.rdata = globalRegModel.getDataTypeIndex("INT", _GENERIC_);
				rsClassSet.push_back((rmapType)srcmap);
			}
			
			// insert into the two-dimensional vector the source register classes
			
			inputSrcRegSet.push_back((rmapVectType)rsClassSet);
			rsClassSet.clear();
			slen++;
		}
	}
	
	// change the opcode subject to checks for matching register classes
	//
	
	char *temp = (char *) malloc(sizeof(opc.getStr())+1);
	strcpy(temp, opc.getStr());
	// NOPs are rISAizable
	if( strcmp(temp, "NOP") == 0 )
	{
		return true;
	}

	// Get the target opcode mapping 		
	if(ifMapExists(temp, inputSrcRegSet, slen, inputDstRegSet, dlen))
	{
		return 1;
	}
	else
	{
		return 0;
	}
	
	
	inputSrcRegSet.clear();
	inputDstRegSet.clear();
	
	// end partha on 08/25/01
}

/*
bool isFeasibleToConvert(BaseOperation *op)
{
	if (testAll)
		return true;


	// the destination registers in rISA instruction use the rISA register set
	BaseArgument *destArg, *srcArg;
	ArgListIter aIter;
	
	ArgList destArgs = op->destOperandList();
	for (aIter = destArgs.begin(); aIter != destArgs.end(); aIter++)
	{
		destArg = *aIter;
		if( destArg != NULL )
		{
			if( destArg->isRegisterArg() )
			{
				// always the case
				if( strcmpci(((RegisterArg *)destArg)->getText(), "cc") == 0 )
					return false;
				if( strcmpci(((RegisterArg *)destArg)->getText(), "hilo") == 0 )
					return false;
				if( strcmpci(((RegisterArg *)destArg)->getText(), "sp") == 0 )
					return false;
				if( strcmpci(((RegisterArg *)destArg)->getText(), "fp") == 0 )
					return false;
				// R0 cannot be a destination register.. That's why not included
			}
		}
	}
	ArgList srcArgs = op->sourceOperandList();
	for (aIter = srcArgs.begin(); aIter != srcArgs.end(); aIter++)
	{
		srcArg = *aIter;
		if( srcArg != NULL )
		{
			if( srcArg->isRegisterArg() )
			{
				
				if( strcmpci(((RegisterArg *)srcArg)->getText(), "cc") == 0 )
					return false;
				if( strcmpci(((RegisterArg *)srcArg)->getText(), "hilo") == 0 )
					return false;
				if( strcmpci(((RegisterArg *)srcArg)->getText(), "sp") == 0 )
					return false;
				if( strcmpci(((RegisterArg *)srcArg)->getText(), "fp") == 0 )
					return false;
			
			}
		}
	}
	return true;
}
*/
// case insensitive strcmp

int strcmpci(char *first, char *second)
{
	if( strlen(first) > strlen(second) )
		return 1;
	else if( strlen(first) > strlen(second) )
		return -1;
	else
	{
		if( strcmp( first, second ) == 0 )
			return 0;
		char trav1 = *first, trav2 = *second;
		while( trav1 != '\0' )
		{
			if( toupper(trav1) != toupper(trav2) )
				return 2;
			trav1++;
			trav2++;
		}
		return 0;
	}
	return 2;
}

int CanbeThumbed(BaseInstruction *instr)
{
	ArrayIterator<DataOpSlot> *dIter = ((NormalInstr *)instr)->getDataOpSlotIter();
	ArrayIterator<ControlOpSlot> *cIter = ((NormalInstr *)instr)->getControlOpSlotIter();
	ArrayIterator<FlowOpSlot> *fIter = ((NormalInstr *)instr)->getFlowOpSlotIter();
	
	MyString goto_opcode = "GOTO";

	// There should be no FlowOps
	while (fIter->hasMoreElements())
	{
		if ((fIter->currentElement()).hasOper())
		{
			MyString opcode;
			((fIter->currentElement()).getPtrToOperation())->getOpcodeName(opcode);

			if (opcode == goto_opcode)
			{
				delete fIter;
				return 0;
			}
		}
		fIter->nextElement();
	}
	delete fIter;

	// There should be no ControlOps
	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			if (!CanbeThumbed((cIter->currentElement()).getPtrToOperation()))
			{
				delete cIter;
				return 0;
			}
		}
		cIter->nextElement();
	}
	delete cIter;

	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			if (!CanbeThumbed((dIter->currentElement()).getPtrToOperation()))
			{
				delete dIter;
				return 0;
			}
		}  
		dIter->nextElement();
	}
	delete dIter;
	
	return 1;
}



void ConvertInstrToNormal(NormalInstr *instr)
{
	ArrayIterator<DataOpSlot> *dIter = instr->getDataOpSlotIter();
	ArrayIterator<ControlOpSlot> *cIter = instr->getControlOpSlotIter();
	ArrayIterator<FlowOpSlot> *fIter = instr->getFlowOpSlotIter();
	
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
		{
			ConvertOperationToNormal((NormalOp *) (dIter->currentElement()).getPtrToOperation());
		}  
		dIter->nextElement();
	}
	delete dIter;

	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
		{
			ConvertOperationToNormal((NormalOp *) (cIter->currentElement()).getPtrToOperation());
		}  
		cIter->nextElement();
	}
	delete cIter;

	/*
	while (fIter->hasMoreElements())
	{
		if ((fIter->currentElement()).hasOper())
		{
			ConvertOperationToNormal((NormalOp *) (fIter->currentElement()).getPtrToOperation());
		}  
		fIter->nextElement();
	}
	delete fIter;
	*/
}

void ConvertOperationToNormal(NormalOp *oper)
{
	MyString opcode;
	oper->getOpcodeName(opcode);
	NtoTIter i;
	for(i = NtoTMap.begin(); i != NtoTMap.end(); i++)
	{
		if ((*i).second == opcode)
		{
			oper->changeOpCode(((MyString)((*i).first)).getStr());
			return;
		}
	}
}

void printRisaInfo(rISABlockList rbList)
{
	printf("\nPrinting the RISA Blocks\n");
	for (rISABlockListIter rIter = rbList.begin(); rIter != rbList.end(); rIter++)
	{
		printf("Start 0x00%x\t End 0x00%x\t BB 0x00%x\n", (*rIter).startInstr, (*rIter).endInstr, (*rIter).bb); 
	}
}

void printThumbInfo(Routine *rout)
{
	LinkedListIterator<BasicBlockNode> *iter = (rout->getCFG()).createBBIterForw();
	// print the entry and exit modes before setting the modes
	iter = rout->getCFG().createBBIterForw();
	while (iter->hasMoreElements())
	{
		printf ("BB%d entry mode : %d\t exit mode : %d\n", iter->currentElement().getDFSNum(), 
			GlobalThumbInfo[&(iter->currentElement())].newEntryMode, GlobalThumbInfo[&(iter->currentElement())].newExitMode);
		iter->nextElement();
	}
}


/*
int isProfitableToConvert(BasicBlockNode *currBB, BaseInstruction *firstThumbInstr, BaseInstruction *lastThumbInstr, int ThumbCount)
{	
	The profitability function depends on 
	1. Code Size due to 
	1. Added Instructions
	2. Converted Instructions
	3. Spill Reload Insertion
	2. Performance
	1. Added Instructions
	2. Spill and Reload instructions
	
	  The third factor that affects the performance is cache performance... 
	  but that's hard to estimate, so we leave it for now

	BBForwIterator bbIter(currBB);
	NormalInstr *currInstr = (NormalInstr *)bbIter.currentElement();
	while( currInstr != firstThumbInstr)
	{
		RegPressProperty rpProp;	
		getRegisterPressure(rpProp, currInstr, currBB);
		if (debugThumb)
		{
			printf("0x00%x -> %d\n", currInstr, rpProp.getNumLive(67));
		}
		bbIter.nextElement();
		currInstr = (NormalInstr *)bbIter.currentElement();
	}
	
	if (ThumbCount >= 4) return 1;
	return 0;
}
/*
  ___________________ Converting a BB _________________
 
  if ((BB.leader = mx_r)&&(BB.leader.next = firstThumbInstr))
  {
	remove BB.leader;
	make firstThumbInstr the leader;
  }
  else
	insert mx;

  if (BB.trailer = lastThumbInstr)
  {
	for each child of BB
	{
		if (BB.child.leader = mx_r)
		  do nothing;

		if (BB.child.leader = mx)
		  remove mx;

        insert mx_r;
	}
  }
  else
    insert mx_r;

*/
/*
-------------- Another Algorithm ------------------

  for each basic block
    do the conversion
	mark the entering mode
	mark the exiting mode
	if mode change in mid of a block, add mx or mx_r instructions appropiately

  for each basic block
    if (all predecessors are NT and this block is NT) then
       do nothing

    if (all predecessors are T and this block is T) then
       do nothing

    if (all predecessors are NT and this block is T) then
	   add a mx_r in the start of the basic block 
    
	if (all predecessors are T and this block is NT) then
	   add a mx in the start of the basic block 

    else
       {
	     if (currentblock.entrymode == T)
		 {
			insert mx at the top of this block
			for the predecessors that end in Thumb mode
			{
			  convert the last instruction to Normal mode
			  insert a mx_r just before this instruction
			}
		 }
		 else if (currentblock.entrymode == NT)
		 {
			for the predecessors that end in Thumb mode
			{
			  convert the last instruction to Normal mode
			  insert a mx_r just before this instruction
			}
		 }
	   }

*/

/*
-------------- Yet Another Algorithm ------------------

Data Structure Changes
  Basic Block will contain 2 more parameters
	1. entrymode, instrToBeInserted
	2. exitmode, instrToBeInserted



  // set the entry and exit modes
  for each basic block
    do the conversion
	mark the entering mode
	mark the exiting mode
	if mode change in mid of a block, add mx or mx_r instructions appropiately

  // look at the entry modes and set the instrToBeInserted
  for each basic block
  {
    if (currentBlock.entrymode = T)
	{
	   Transformations

		1.    T    T        T    T
					 --->  
				 T             T
	    
		2.    N    N        N    N
					 --->  
				 T             N
	    
		3.    N    T        N    N
					 --->  
				 T             N
	}
	else (currentBlock.entrymode = N)
	{
	    Transformations

		1.    N    N        N    N
					 --->  
				 N             N
	    
		2.    T    T        T    T
					 --->  
				 N             T
	    
		3.    N    T        N    N
					 --->  
				 N             N
	}
  }

  // look at the exit modes and set the instrToBeInserted
  for each basic block
  {
    if (currentBlock.entrymode = T)
	{
	   Transformations

		1.       T              T
					   --->  
			   T    T         T    T
	    
		2.       T              T
					   --->  
			   N    N         T    T

		3.       T              T
					   --->  
			   T    N         T    T
	}
	else (currentBlock.entrymode = N)
	{
	    Transformations

		1.       N              N
					   --->  
			   N    N         N    N
	    
		2.       N              N
					   --->  
			   T    T         N    N

		3.       N              N
					   --->  
			   N    T         N    N
	}
  }

  // how to 
    1. convert child/self from T to N
           insert mx at the top
    2. convert child/self from N to T
           insert mx_r at the top
    3. convert a parent from T to N
           convert the last branch instruction from T to N
		   insert a mx_r before this instruction
}
*/





// returns 1, if this could initialize the NtoTMap
int ThumbInit()
{
	
	FILE *fpt;
	if( (fpt = fopen("rISARegisterMapping.txt", "r")) == NULL )
	{
		ASSERT("Unable to open rISARegisterMapping.txt\n");
	}

	ReadRegSetMapFile(fpt);

	fclose(fpt);

	return 1;
}

void printNtoTMap(NormalToThumbInstrMap N2TMap)
{
	// print the thumb mapping
	printf("Printing Normal to Thumb Instruction Mapping\n");
	NtoTIter i;
	for ( i = N2TMap.begin(); i!= N2TMap.end(); i++ )
		printf("%s \t %s\n", ((MyString) (*i).first).getStr(), ((MyString) (*i).second).getStr());
	
}
