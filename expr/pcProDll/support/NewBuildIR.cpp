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
// File:  		NewBuildIR.cpp
// Created:		Tue Jul 06, 99
// Last modified: 	Tue Jul 06, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "expressParser.h"
#include "BuildIR.h"
#include "CodeLocProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern int doneRA;

BaseArgument *buildArg(EXPRESS_ARG_TYPE aT, int num, BaseArgProperty *asrt)
{
   BaseArgument *retPtr = NULL;

   if (aT == _EXPRESS_REG_INT_)
   {
	   if( !doneRA )
	   {
		   retPtr = new RealRegArg("gR", num);
		   retPtr->setRegFile(globalRegFileList->getIndex("gR"));
	   }
	   else
	   {
		   cout << "Warning: Target Register set is not yet specified !" << endl;
		   retPtr = new RealRegArg("R", num);
		   retPtr->setRegFile(globalRegFileList->getIndex("R"));
	   }
	}
	else if (aT == _EXPRESS_IMM_INT_)
	{
		retPtr = new IConstArg(num);
	}
	else
	{
		// error condition
		//
		cout << "Warning: buildArg recieved incompatible type with integer arg" << endl;
	}

	if (asrt != NULL)
	{
		retPtr->addProperty(*asrt);
	}

	return retPtr;
}

BaseArgument *buildArg(EXPRESS_ARG_TYPE aT, char *st, BaseArgProperty *asrt)
{
   BaseArgument *retPtr = NULL;

	if (aT == _EXPRESS_REG_STRING_)
	{
		char *tmp = st;
		int i = 0;
		while (isalpha(*tmp))
		{
			tmp++;
			i++;
		}
		int num = 0;
		if (*tmp != '\0')
			num = atoi(tmp);
		char tmpStr[EXPRESS_ARG_MAX_STRLEN];
		strcpy(tmpStr, "g");
		i++; // accounting for "g" standing for generic
		strcat(tmpStr, st);
		tmpStr[i] = '\0';

		retPtr = new RealRegArg(tmpStr, num);
		retPtr->setRegFile(globalRegFileList->getIndex(tmpStr));
	}
	else if (aT == _EXPRESS_IMM_FLOAT_)
	{
		retPtr = new FConstArg(st);
	}
	else if (aT == _EXPRESS_LABEL_)
	{
		retPtr = new LabelArg(st);
	}
	else
	{
		// error condition
		//
		cout << "Warning: buildArg recieved incompatible type with integer arg" << endl;
	}

	if (asrt != NULL)
	{
		retPtr->addProperty(*asrt);
	}

	return retPtr;
}

BaseArgument *buildArg(int pos)
{
	ArgNode & arg = argArray[pos];
	EXPRESS_ARG_TYPE a = arg.argT;

	// Also, check for assertions and add them.
	//
	// Right now, we only accept one assertion per arg, in the original .procs file.
	//
	BaseArgProperty *asrt;
	asrt = assertionArray[pos];

	if ((a == _EXPRESS_REG_INT_) || (a == _EXPRESS_IMM_INT_))
	{
		return (buildArg(a, arg.argV.iarg, asrt));
	}
	else if ((a == _EXPRESS_REG_STRING_) || (a == _EXPRESS_IMM_FLOAT_) || (a == _EXPRESS_LABEL_))
	{
		return (buildArg(a, arg.argV.sarg, asrt));
	}

	// default: error condtion.
	//
	return NULL;
}

BaseArgument *makeNewNextReg(BaseArgument *arg)
{
	if (!arg->isRegisterArg()) return NULL;

	RegisterArg *tmp = (RegisterArg *)arg;
	BaseArgument *retPtr;
	retPtr = new RealRegArg(tmp->getText(), tmp->getVal()+1);
	retPtr->setRegFile(globalRegFileList->getIndex(tmp->getText()));

	return retPtr;
}

BaseOperation *buildMIPSOp(char *opcode, int numArgs, int hasLabel)
{
	BaseOperation *retPtr = NULL;
	
	BaseArgument *tempArgArray[EXPRESS_MAX_ARGS];
	int i;
	
	OpCode opc(opcode);
	
	if ((numArgs < 0) || (numArgs > EXPRESS_MAX_ARGS))
	{
		// error condition.
		//
		cout << " Panic: Operation has invalid number of arguments: " << numArgs << endl;
		assert(0);
	}
	
	for (i = 0; i < numArgs; i++)
		tempArgArray[i] = buildArg(i);
	
	IfOp tempIfOp;
	JumpOp tempJumpOp;
	GotoOp tempGotoOp;
	CallRetOp tempCallOp;
	MemoryOp tempMemoryOp;
	ComputeOp tempComputeOp;
	
	switch (numArgs)
	{
	case 0:
		// The operation is a NOP or a RETURN
		switch (opc.typeOf())
		{
		case COMPUTE_OP:
			tempComputeOp.initOpCode(opc);
			
			retPtr = tempComputeOp.copy();
			tempComputeOp.reset();			
			break;
			
		case RET_OP:
			tempCallOp.initOpCode(opc);
			
			retPtr = tempCallOp.copy();
			tempCallOp.reset();
			break;
		}
		break;
		
		case 1:
			switch (opc.typeOf())
			{
			case GOTO_OP:
				tempGotoOp.initOpCode(opc);
				tempGotoOp.addOperand(tempArgArray[FIRST_POS], _SOURCE_1_);
				
				retPtr = tempGotoOp.copy();
				tempGotoOp.reset();				
				break;
				
			case CALL_OP:
				tempCallOp.initOpCode(opc);
				tempCallOp.addOperand(tempArgArray[FIRST_POS], _SOURCE_1_);
				
				if (isSpecialCall(tempCallOp))
					initSpecialParms(tempCallOp);
				else
					initParms(tempCallOp);
				
				retPtr = tempCallOp.copy();
				tempCallOp.reset();
				break;
			}
			break;
			
			case 2:
				switch (opc.typeOf())
				{
				case IF_OP:
					tempIfOp.initOpCode(opc);
					tempIfOp.addCCReg(tempArgArray[FIRST_POS]);
					tempIfOp.addLabelArg(tempArgArray[SECOND_POS]);
					
					retPtr = tempIfOp.copy();
					tempIfOp.reset();
					break;
					
				case CALL_OP:
					tempCallOp.initOpCode(opc);
					tempCallOp.addOperand(tempArgArray[FIRST_POS], _SOURCE_1_);
					tempCallOp.addOperand(tempArgArray[SECOND_POS], _DEST_, _DEST_LIST_);
					
					if (isSpecialCall(tempCallOp))
						initSpecialParms(tempCallOp);
					else
						initParms(tempCallOp);
					
					retPtr = tempCallOp.copy();
					
					tempCallOp.reset();
					break;
					
				case COMPUTE_OP:
					tempComputeOp.initOpCode(opc);
					tempComputeOp.addOperand(tempArgArray[FIRST_POS], _DEST_, _DEST_LIST_);
					tempComputeOp.addOperand(tempArgArray[SECOND_POS], _SOURCE_1_);
					
					retPtr = tempComputeOp.copy();
					tempComputeOp.reset();
					break;
					
				case STORE_OP:
				case LOAD_OP:
					tempMemoryOp.initOpCode(opc);
					tempMemoryOp.addDestOperand(tempArgArray[FIRST_POS]);
					tempMemoryOp.addSourceOperand(tempArgArray[SECOND_POS]);
					
					// We need to always specify the offset.
					// So, create a new immediate argument with 0 value and addit.
					
					BaseArgument *offsetArg;
					offsetArg = new IConstArg(0);
					
					tempMemoryOp.setOffset(offsetArg);
					retPtr = tempMemoryOp.copy();
					tempMemoryOp.reset();
					delete offsetArg;
					break;
				}
				break;
				
				case 3:
					switch (opc.typeOf())
					{
					case COMPUTE_OP:
						tempComputeOp.initOpCode(opc);
						tempComputeOp.addOperand(tempArgArray[FIRST_POS], _DEST_, _DEST_LIST_);
						tempComputeOp.addOperand(tempArgArray[SECOND_POS], _SOURCE_1_);
						tempComputeOp.addOperand(tempArgArray[THIRD_POS], _SOURCE_2_);
						
						retPtr = tempComputeOp.copy();
						tempComputeOp.reset();
						break;
						
					case LOAD_OP:
						tempMemoryOp.initOpCode(opc);
						tempMemoryOp.addDestOperand(tempArgArray[FIRST_POS]);
						tempMemoryOp.setOffset(tempArgArray[SECOND_POS]);
						tempMemoryOp.addSourceOperand(tempArgArray[THIRD_POS]);
						
						retPtr = tempMemoryOp.copy();
						tempMemoryOp.reset();
						break;
						
					case STORE_OP:
						tempMemoryOp.initOpCode(opc);
						
						// In the case where the Store address is specified as label + int
						// the order of the args may be reversed (for First and Second)
						// So, check to ensure that the offset is always the immediate.
						//
						if ((tempArgArray[FIRST_POS])->isImmediateArg())
						{
							tempMemoryOp.addDestOperand(tempArgArray[SECOND_POS]);
							tempMemoryOp.setOffset(tempArgArray[FIRST_POS]);
						}
						else if ((tempArgArray[SECOND_POS])->isImmediateArg())
						{
							tempMemoryOp.addDestOperand(tempArgArray[FIRST_POS]);
							tempMemoryOp.setOffset(tempArgArray[SECOND_POS]);
						}
						else
						{
							// error condition. Either the First or the Second argument 
							// has to be an immediate
							//
							// However, in some cases, store takes the form : <opcode> <label> <reg> <reg>
							// In such cases, make the label as the offset.
							//
							cout << "Warning: In Building Store operation with 3 args, neither first or second argument was immediate" << endl;
							
							if ((tempArgArray[FIRST_POS])->isLabelArg())
							{
								tempMemoryOp.addDestOperand(tempArgArray[SECOND_POS]);
								tempMemoryOp.setOffset(tempArgArray[FIRST_POS]);
							}
							else if ((tempArgArray[SECOND_POS])->isLabelArg())
							{
								tempMemoryOp.addDestOperand(tempArgArray[FIRST_POS]);
								tempMemoryOp.setOffset(tempArgArray[SECOND_POS]);
							}
							else
							{
								// we don't know how to handle such cases, assert
								//
								assert(0);
							}
						}
						tempMemoryOp.addSourceOperand(tempArgArray[THIRD_POS]);
						
						retPtr = tempMemoryOp.copy();
						tempMemoryOp.reset();
						break;
					case CALL_OP:
						tempCallOp.initOpCode(opc);
						//the first_pos operand is omited, since it is the returd address, which we don't need
						tempCallOp.addOperand(tempArgArray[SECOND_POS], _SOURCE_1_);
						tempCallOp.addOperand(tempArgArray[THIRD_POS], _DEST_, _DEST_LIST_);
						
						if (isSpecialCall(tempCallOp))
							initSpecialParms(tempCallOp);
						else
							initParms(tempCallOp);
						
						retPtr = tempCallOp.copy();
						
						tempCallOp.reset();
						break;
					case IMPLIED_DEST_OP:
						// as of now, we only handle compute ops with multiple dests (like DDIV)
						//
						tempComputeOp.initOpCode(opc);
						tempComputeOp.addOperand(tempArgArray[FIRST_POS], _DEST_, _DEST_LIST_);
						tempComputeOp.addOperand(makeNewNextReg(tempArgArray[FIRST_POS]), _IMPLIED_DEST_, _DEST_LIST_);
						tempComputeOp.addOperand(tempArgArray[SECOND_POS], _SOURCE_1_);
						tempComputeOp.addOperand(tempArgArray[THIRD_POS], _SOURCE_2_);
						
						retPtr = tempComputeOp.copy();
						tempComputeOp.reset();
						break;

					}
					break;
      }
	  
	  
	  // Now, delete the objects attached to the pointers. (Memory
	  // Management). breakStr function creates new objects that have to
	  // be destroyed.
	  //
	  
	  for (i=0; i<numArgs;i++)
	  {
		  if (tempArgArray[i] != NULL) 
			  delete tempArgArray[i];
	  }	  
	  return retPtr;
}

// use this function to build an operation with empty list of dest/source args.
// these can be filled in later.
//
BaseOperation *buildShellOperation(char *opcode)
{
	BaseOperation *retPtr;

	IfOp tempIfOp;
	JumpOp tempJumpOp;
	GotoOp tempGotoOp;
	CallRetOp tempCallOp;
	MemoryOp tempMemoryOp;
	ComputeOp tempComputeOp;
	
	OpCode opc(opcode);

	switch (opc.typeOf())
	{
		case COMPUTE_OP:
			tempComputeOp.initOpCode(opc);
			
			retPtr = tempComputeOp.copy();
			tempComputeOp.reset();			
			break;
			
		case IF_OP:
			tempIfOp.initOpCode(opc);
			
			retPtr = tempIfOp.copy();
			tempIfOp.reset();
			break;
			
		case RET_OP:
		case CALL_OP:
			tempCallOp.initOpCode(opc);
			
			retPtr = tempCallOp.copy();
			tempCallOp.reset();
			break;
						
		case STORE_OP:
		case LOAD_OP:
			tempMemoryOp.initOpCode(opc);

			retPtr = tempMemoryOp.copy();
			tempMemoryOp.reset();
			break;

		case IMPLIED_DEST_OP:
			tempComputeOp.initOpCode(opc);
			
			retPtr = tempComputeOp.copy();
			tempComputeOp.reset();			
			break;

		case GOTO_OP:
			tempGotoOp.initOpCode(opc);
			
			retPtr = tempGotoOp.copy();
			tempGotoOp.reset();				
			break;
	}
	return retPtr;
}



//Stores the last operation just added by buildMIPSInstr
//needed in the parser to add the assertion to this operation
BaseOperation *globalOperationAdded;

extern int _LAST_FLOW_POS_;

NormalInstr *buildMIPSInstr(char *opcode, int numArgs, int hasLabel, int currLoc)
{
	int index;
	JumpOp tempJumpOp;

	NormalInstr *retPtr;
	retPtr = new NormalInstr;


	BaseOperation *op;

	// Changed by SAH on : 07/18/2000 : We convert IGOTO's to return statements.
	//
	if (isIGOTO(opcode))
	{
		op = buildMIPSOp("RETURN", 0, hasLabel);

		// Note: At this point we do not know the register that holds the return.
		// We figure that out after the IList has been built, and then we "patch" the return
		// with the appropriate argument as the source.
	}
	else
	{
		op = buildMIPSOp(opcode, numArgs, hasLabel);
	}

	LabelArg tempLabelArg;
	if (hasLabel != 0)
	{
		tempLabelArg.initLabel(labelName);
		retPtr->addLabel(&(tempLabelArg));
		
	}
   
   if (op == NULL)
	{
		// error condition.
		//
		cout << "Warning: Tried to insert NULL op into instruction" << endl;
		return retPtr;
	}

   // Now add the source code location number to the instruction.
   //
   CodeLocProperty locP(currLoc);
   retPtr->addProperty(locP);

	switch ((op->getOpCode()).typeOf())
	{

	case STORE_OP:
	case LOAD_OP:
	case COMPUTE_OP:

		retPtr->initScheduleAndAdd(op);
		
		index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
		
		(retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
		break;

	case RET_OP:
	case CALL_OP:

		retPtr->initScheduleAndAdd(op);

		index = retPtr->addFlowOpToList(&(tempJumpOp));
		(retPtr->getRootOpSlot()).setTrueBranch(&(retPtr->getFlowOpSlot(index)));
		
		index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);	       
		(retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
		break;

	case IF_OP:

		retPtr->initScheduleAndAdd(op);

		index = retPtr->addFlowOpToList(&(tempJumpOp));
		(retPtr->getRootOpSlot()).setTrueBranch(&(retPtr->getFlowOpSlot(index)));
	       
		index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
		(retPtr->getRootOpSlot()).setFalseBranch(&(retPtr->getFlowOpSlot(index)));
		break;

	case GOTO_OP:

		retPtr->initAddFlowOp((FlowOp *)op, _LAST_FLOW_POS_);
		break;

	default:

		// error condition.
		//
		cout << "Panic: Recieved invalid operation type while building Instr" << endl;
		assert(0);
	}

	globalOperationAdded=retPtr->getRootOpSlot().getPtrToOperation();

	return retPtr;
}

BaseInstruction *buildInstrWithOper(BaseOperation *op)
{
	int index;
	JumpOp tempJumpOp;

	BaseInstruction *retPtr;
	retPtr = new NormalInstr;
	NormalInstr *retNPtr = (NormalInstr *)retPtr;

   if (op == NULL)
	{
		// error condition.
		//
		cout << "Warning: Tried to insert NULL op into instruction" << endl;
		return retPtr;
	}
	switch ((op->getOpCode()).typeOf())
	{

	case STORE_OP:
	case LOAD_OP:
	case COMPUTE_OP:

		retNPtr->initScheduleAndAdd(op);
		
		index = retNPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
		
		(retNPtr->getRootOpSlot()).setNext(&(retNPtr->getFlowOpSlot(index)));
		break;

	case RET_OP:
	case CALL_OP:

		retNPtr->initScheduleAndAdd(op);

		index = retNPtr->addFlowOpToList(&(tempJumpOp));
		(retNPtr->getRootOpSlot()).setTrueBranch(&(retNPtr->getFlowOpSlot(index)));
		
		index = retNPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);	       
		(retNPtr->getRootOpSlot()).setNext(&(retNPtr->getFlowOpSlot(index)));
		break;

	case IF_OP:

		retNPtr->initScheduleAndAdd(op);

		index = retNPtr->addFlowOpToList(&(tempJumpOp));
		(retNPtr->getRootOpSlot()).setTrueBranch(&(retNPtr->getFlowOpSlot(index)));
	       
		index = retNPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
		(retNPtr->getRootOpSlot()).setFalseBranch(&(retNPtr->getFlowOpSlot(index)));
		break;

	case GOTO_OP:

		retNPtr->initAddFlowOp((FlowOp *)op, _LAST_FLOW_POS_);
		break;

	default:

		// error condition.
		//
		cout << "Panic: Recieved invalid operation type while building Instr" << endl;
		assert(0);
	}
	return retPtr;
}

BaseInstruction *buildInstrWithShellOper(char *opcode)
{
	BaseOperation *op = buildShellOperation(opcode);

	return buildInstrWithOper(op);
}
