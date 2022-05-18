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
// $Id: BuildIR.cc,v 1.12 1998/03/27 00:02:45 wtang Exp $
//
// File:  		BuildIR.cc
// Created:		Thu Jan 02, 97
// Last modified: 	Thu Jan 02, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"
#include "BuildIR.h"

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
BaseArgument *buildArg(char *argStr)
{
   BaseArgument *retPtr;
   char str[200], regName[200];
   char *tempStr, *regNamePtr;   
   int regNum;
   
   strcpy(str,argStr);
   tempStr = str;
   regNamePtr = regName;
   while (isspace(*tempStr))
	   tempStr++;

   if (*tempStr == '$') // register argument
      {
	 tempStr += SIZE_OF_CHAR;
	 while (isalpha(*tempStr))
	       *regNamePtr++ = *tempStr++;
	 
   	 *regNamePtr = 0;
	 regNum = 0;
	 if (tempStr != NULL)
		 regNum = atoi(tempStr);
	 
	 if (regNamePtr==regName )
	 {
		 if( !doneRA )
		 {
			 retPtr = new RealRegArg("gR", regNum);
		 }
		 else
		 {
//			 cout << "Warning: Target Register set is not yet specified !" << endl;
			 assert(0); // The section of code below is wrong. Need to check RA property.
			 retPtr = new RealRegArg("R", regNum);
		 }
	 }
	 else 
	 {
	    retPtr = new RealRegArg(regName, regNum);
	 }

	 retPtr->setRegFile(globalRegFileList->getIndex(regName));
      }
   else // immediate argument or label argument
      { 
	 if (*tempStr == 'L') // Note: This has to be extended to include any character.
	 { 
	       retPtr = new LabelArg(tempStr);
	 }
	 else if (*tempStr == '_') // it is most likely a symbolic address
	 {
//		 int num = addToSymbolicAddressTable(tempStr);
//		 retPtr = new IConstArg(num);

		 retPtr = new LabelArg(tempStr);
	 }
	 else if (strstr(tempStr, "e+") != NULL || strstr(tempStr, "e-") != NULL) // floating point.
	 {
	       retPtr = new FConstArg(tempStr);
	 }
	 else // integer immediate
	 {
	       retPtr = new IConstArg(atoi(tempStr));
	 }
   }
   
   return retPtr;
}

int breakStr(char *pStr, OpCode & opCode, BaseArgument ** args)
{
   char *tempStr;
   int done;
   
   int numArgs = 0;
   
   tempStr = strtok(pStr, "\t ");

   opCode.init(tempStr);

   done = 0;
   while ((!done) && (numArgs < MAX_NUM_OF_ARGS))
      {
	 tempStr = strtok(NULL, " ");
	 
	 if (tempStr == NULL)
	    done = 1;
	 else
	    {
	       args[numArgs] = buildArg(tempStr);
	       numArgs++;
	    }
      }
   
   return numArgs;
}

extern int _LAST_FLOW_POS_;

NormalInstr *buildMIPSInstr(char *parseStr, char *label = NULL)
{
   OpCode opCode;
   int i;
   BaseArgument * argArray[MAX_NUM_OF_ARGS];
   for (i=0; i<MAX_NUM_OF_ARGS;i++)
     argArray[i] = NULL;
   
   NormalInstr *retPtr;
   
   int numArgs;
   numArgs = breakStr(parseStr, opCode, argArray);
   
   if ((numArgs < 0) || (numArgs > MAX_NUM_OF_ARGS))
      {
	 cout << " Panic: Operation has invalid number of arguments: " << numArgs << endl;
	 assert(0);
	 //
	 // Later on make this an exception.
      }
   
   retPtr = new NormalInstr;
   
   LabelArg tempLabelArg;
   
   if (label != NULL)
      {
	 tempLabelArg.initLabel(label);
	 retPtr->addLabel(&(tempLabelArg));
	 
      }
   
   int index;
   
   IfOp tempIfOp;
   JumpOp tempJumpOp;
   GotoOp tempGotoOp;
   CallRetOp tempCallOp;
   MemoryOp tempMemoryOp;
   ComputeOp tempComputeOp;

	BaseOperation *a; 

	switch (numArgs)
	{
	case 0:
		// The operation is a NOP:
		switch (opCode.typeOf())
		 {
		case COMPUTE_OP:
			tempComputeOp.initOpCode(opCode);

			a = &(tempComputeOp);
	       retPtr->initScheduleAndAdd(a);
	       
	       index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	       
	       (retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
	       
	       tempComputeOp.reset();

		   break;
		case RET_OP:
			 tempCallOp.initOpCode(opCode);

			 a = &(tempCallOp);
			retPtr->initScheduleAndAdd(a);

	       index = retPtr->addFlowOpToList(&(tempJumpOp));
	       (retPtr->getRootOpSlot()).setTrueBranch(&(retPtr->getFlowOpSlot(index)));
			
			index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);	       
			(retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
	       
			tempCallOp.reset();

		 }

	 break;
	 
       case 1:
	 switch (opCode.typeOf())
	    {
	     case GOTO_OP:
	       tempGotoOp.initOpCode(opCode);
	       tempGotoOp.addLabelArg(argArray[FIRST_POS]);
	       
	       retPtr->initAddFlowOp(&(tempGotoOp), _LAST_FLOW_POS_);
	       
	       tempGotoOp.reset();
	       
	       break;
		 case CALL_OP:
			 tempCallOp.initOpCode(opCode);
			 tempCallOp.addOperand(argArray[FIRST_POS], _SOURCE_1_);

			 if (isSpecialCall(tempCallOp))
			 {
				 initSpecialParms(tempCallOp);
			 }
			 else
			 {
				initParms(tempCallOp);
			 }

			 a = &(tempCallOp);
			retPtr->initScheduleAndAdd(a);

	       index = retPtr->addFlowOpToList(&(tempJumpOp));
	       (retPtr->getRootOpSlot()).setTrueBranch(&(retPtr->getFlowOpSlot(index)));
			
			index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);	       
			(retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
	       
			tempCallOp.reset();
			break;
		}
		break;
       case 2:
		switch (opCode.typeOf())
	    {
	     case IF_OP:
	       tempIfOp.initOpCode(opCode);
	       tempIfOp.addCCReg(argArray[FIRST_POS]);
	       tempIfOp.addLabelArg(argArray[SECOND_POS]);

		   retPtr->initAddControlOp(&(tempIfOp));
	       
	       index = retPtr->addFlowOpToList(&(tempJumpOp));
	       (retPtr->getRootOpSlot()).setTrueBranch(&(retPtr->getFlowOpSlot(index)));
	       
	       index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	       (retPtr->getRootOpSlot()).setFalseBranch(&(retPtr->getFlowOpSlot(index)));
	       
	       tempIfOp.reset();
	       
	       break;
		 case CALL_OP:
			 tempCallOp.initOpCode(opCode);
			 tempCallOp.addOperand(argArray[FIRST_POS], _SOURCE_1_);
			 tempCallOp.addOperand(argArray[SECOND_POS], _DEST_, _DEST_LIST_);

			 if (isSpecialCall(tempCallOp))
			 {
				 initSpecialParms(tempCallOp);
			 }
			 else
			 {
				initParms(tempCallOp);
			 }

			 a = &(tempCallOp);
	       retPtr->initScheduleAndAdd(a);
	       
	       index = retPtr->addFlowOpToList(&(tempJumpOp));
	       (retPtr->getRootOpSlot()).setTrueBranch(&(retPtr->getFlowOpSlot(index)));

		   index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	       (retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
	       
	       tempCallOp.reset();
		   break;
		 case COMPUTE_OP:
	       tempComputeOp.initOpCode(opCode);
	       tempComputeOp.addOperand(argArray[FIRST_POS], _DEST_, _DEST_LIST_);
	       tempComputeOp.addOperand(argArray[SECOND_POS], _SOURCE_1_);
	     
			a = &(tempComputeOp);
	       retPtr->initScheduleAndAdd(a);
	       
	       index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	       
	       (retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
	       
	       tempComputeOp.reset();
	       
	       break;
		 case STORE_OP:
		 case LOAD_OP:
			 tempMemoryOp.initOpCode(opCode);
			 tempMemoryOp.addDestOperand(argArray[FIRST_POS]);
			 tempMemoryOp.addSourceOperand(argArray[SECOND_POS]);
			 
			 a = &(tempMemoryOp);
			 retPtr->initScheduleAndAdd(a);
			 
			 index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
			 
			 (retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
			 
			 tempMemoryOp.reset();
			 
			 break;
	 }
	 break;
	 
       case 3:
	 switch (opCode.typeOf())
	    {
	     case COMPUTE_OP:
	       tempComputeOp.initOpCode(opCode);
	       tempComputeOp.addOperand(argArray[FIRST_POS], _DEST_, _DEST_LIST_);
	       tempComputeOp.addOperand(argArray[SECOND_POS], _SOURCE_1_);
	       tempComputeOp.addOperand(argArray[THIRD_POS], _SOURCE_2_);
	       
			a = &(tempComputeOp);
		
			retPtr->initScheduleAndAdd(a);
	       
	       index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	       
	       (retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));
	       
	       tempComputeOp.reset();
	 
	       break;
	       
	     case LOAD_OP:
	       tempMemoryOp.initOpCode(opCode);
	       tempMemoryOp.addDestOperand(argArray[FIRST_POS]);
	       tempMemoryOp.setOffset(argArray[SECOND_POS]);
	       tempMemoryOp.addSourceOperand(argArray[THIRD_POS]);
	       
		   a = &(tempMemoryOp);
		
	       retPtr->initScheduleAndAdd(a);

	       index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	       
	       (retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));

	       tempMemoryOp.reset();

	       break;
		 case STORE_OP:
	       tempMemoryOp.initOpCode(opCode);
	       tempMemoryOp.addDestOperand(argArray[SECOND_POS]);
	       tempMemoryOp.setOffset(argArray[FIRST_POS]);
	       tempMemoryOp.addSourceOperand(argArray[THIRD_POS]);
	       
		   a = &(tempMemoryOp);
		   retPtr->initScheduleAndAdd(a);

	       index = retPtr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	       
	       (retPtr->getRootOpSlot()).setNext(&(retPtr->getFlowOpSlot(index)));

	       tempMemoryOp.reset();

	       break;
	    }
	 
	 break;
      }
      
   // Now, delete the objects attached to the pointers. (Memory
   // Management). breakStr function creates new objects that have to
   // be destroyed.
   //

   for (i=0; i<numArgs;i++)
     if (argArray[i] != NULL) {
/*
       if (argArray[i]->isLabelArg())
	 delete ((LabelArg *)argArray[i]);
       if (argArray[i]->isFConstArg())
	 delete ((FConstArg *)argArray[i]);
       if (argArray[i]->isRealRegArg())
	 delete ((RealRegArg *)argArray[i]);
       else
*/
	delete argArray[i];
     }

   return retPtr;
}

BaseOperation *buildMIPSOp(char *parseStr, char *label = NULL)
{
   OpCode opCode;
   int i;
   BaseArgument * argArray[MAX_NUM_OF_ARGS];
   for (i=0; i<MAX_NUM_OF_ARGS;i++)
     argArray[i] = NULL;
   
   BaseOperation *retPtr;
   
   int numArgs;
   numArgs = breakStr(parseStr, opCode, argArray);
   
   if ((numArgs < 0) || (numArgs > MAX_NUM_OF_ARGS))
      {
	 cout << " Panic: Operation has invalid number of arguments: " << numArgs << endl;
	 assert(0);
	 //
	 // Later on make this an exception.
      }
   
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
		switch (opCode.typeOf())
		 {
		case COMPUTE_OP:
			tempComputeOp.initOpCode(opCode);

	       retPtr = tempComputeOp.copy();
	       
	       tempComputeOp.reset();

		   break;
		case RET_OP:
			tempCallOp.initOpCode(opCode);
			 retPtr = tempCallOp.copy();

			tempCallOp.reset();
			break;

		 }
	 break;
	 
       case 1:
	 switch (opCode.typeOf())
	    {
	     case GOTO_OP:
	       tempGotoOp.initOpCode(opCode);
	       tempGotoOp.addOperand(argArray[FIRST_POS], _SOURCE_1_);
	       
	       retPtr = tempGotoOp.copy();

	       tempGotoOp.reset();
	       
	       break;
		 case CALL_OP:
			 tempCallOp.initOpCode(opCode);
			 tempCallOp.addOperand(argArray[FIRST_POS], _SOURCE_1_);

			 if (isSpecialCall(tempCallOp))
			 {
				 initSpecialParms(tempCallOp);
			 }
			 else
			 {
				initParms(tempCallOp);
			 }

			 retPtr = tempCallOp.copy();

			tempCallOp.reset();
			break;
		}
		break;
       case 2:
		switch (opCode.typeOf())
	    {
	     case IF_OP:
	       tempIfOp.initOpCode(opCode);
	       tempIfOp.addCCReg(argArray[FIRST_POS]);
	       tempIfOp.addLabelArg(argArray[SECOND_POS]);

	       retPtr = tempIfOp.copy();
		   
		   tempIfOp.reset();
	       
	       break;
		 case CALL_OP:
			 tempCallOp.initOpCode(opCode);
			 tempCallOp.addOperand(argArray[FIRST_POS], _SOURCE_1_);
			 tempCallOp.addOperand(argArray[SECOND_POS], _DEST_, _DEST_LIST_);

			 if (isSpecialCall(tempCallOp))
			 {
				 initSpecialParms(tempCallOp);
			 }
			 else
			 {
				initParms(tempCallOp);
			 }

	       retPtr = tempCallOp.copy();
	       
	       tempCallOp.reset();
		   break;
		 case COMPUTE_OP:
	       tempComputeOp.initOpCode(opCode);
	       tempComputeOp.addOperand(argArray[FIRST_POS], _DEST_, _DEST_LIST_);
	       tempComputeOp.addOperand(argArray[SECOND_POS], _SOURCE_1_);
	     
	       retPtr = tempComputeOp.copy();
	       
	       tempComputeOp.reset();
	       
	       break;
		 case STORE_OP:
		 case LOAD_OP:
			 tempMemoryOp.initOpCode(opCode);
			 tempMemoryOp.addDestOperand(argArray[FIRST_POS]);
			 tempMemoryOp.addSourceOperand(argArray[SECOND_POS]);
			 
	       retPtr = tempMemoryOp.copy();

		   tempMemoryOp.reset();
			 
			 break;
	 }
	 break;
	 
       case 3:
	 switch (opCode.typeOf())
	    {
	     case COMPUTE_OP:
	       tempComputeOp.initOpCode(opCode);
	       tempComputeOp.addOperand(argArray[FIRST_POS], _DEST_, _DEST_LIST_);
	       tempComputeOp.addOperand(argArray[SECOND_POS], _SOURCE_1_);
	       tempComputeOp.addOperand(argArray[THIRD_POS], _SOURCE_2_);
	       
	       retPtr = tempComputeOp.copy();
	       tempComputeOp.reset();
	 
	       break;
	       
	     case LOAD_OP:
	       tempMemoryOp.initOpCode(opCode);
	       tempMemoryOp.addDestOperand(argArray[FIRST_POS]);
	       tempMemoryOp.setOffset(argArray[SECOND_POS]);
	       tempMemoryOp.addSourceOperand(argArray[THIRD_POS]);
	       
	       retPtr = tempMemoryOp.copy();

		   tempMemoryOp.reset();

	       break;
		 case STORE_OP:
	       tempMemoryOp.initOpCode(opCode);
	       tempMemoryOp.addDestOperand(argArray[SECOND_POS]);
	       tempMemoryOp.setOffset(argArray[FIRST_POS]);
	       tempMemoryOp.addSourceOperand(argArray[THIRD_POS]);
	       
	       retPtr = tempMemoryOp.copy();

	       tempMemoryOp.reset();

	       break;
	    }
	 
	 break;
      }
   
   
   // Now, delete the objects attached to the pointers. (Memory
   // Management). breakStr function creates new objects that have to
   // be destroyed.
   //

   for (i=0; i<numArgs;i++)
     if (argArray[i] != NULL) {
		delete argArray[i];
     }

   return retPtr;
}

