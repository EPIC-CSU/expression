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
#include "stdafx.h"

#include "BaseOperation.h"
//#include "BaseArgument.h"
//#include "Routine.h"
//#include "TreeBuilding.h"
//#include "IburgToIR.h"
#include "OpPredicate.h"

#include "ComputeOp.h"
#include "UDProperty.h"
#include "DUProperty.h"

extern Routine* currRoutine;
#include "Routine.h"
#include "IRTraversal.h"
#include "PredicationInformation.h"
#include "NormalOp.h"
#include "UDDU.h"
#include "PredProperty.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern bool ICGenUDDU;


// Global declaration of predication Information
extern PredicationInformation GlobalPredicationInformation; 

extern int createNewSSANumForArg(BaseArgument *arg);


// If src2SSANum is specified, then, the ISEL is of the form ISEL <dest> <src1> <src2> <pred>
// else it is of the form ISEL <dest> <src> <pred>
// 
// Also, note that we currently assume that the predicate is just one argument.
// Inorder to make it a list of arguments, create the list before hand and pass it to this function.
//
ComputeOp *createISELOp(OpCode & opc, BaseArgument *regArg, OpPredicate* pred, int destSSANum,  int src1SSANum, int src2SSANum = _INVALID_)
{
	char *regName;
	int regID;
	RegFileNum regFile;
	RegisterArg *src1Arg = NULL, *src2Arg = NULL, *destArg = NULL;
	SSAArgProperty ssaP;
	ComputeOp *iselOp = NULL;

	regName = ((RegisterArg *)regArg)->getText();
	regID = ((RegisterArg *)regArg)->getVal();
	regFile = ((RealRegArg *)regArg)->regFileIndex();
 
	destArg = new RealRegArg(regName,regID,regFile);
	ssaP.setSSANum(destSSANum);
	destArg->addProperty(ssaP);

	src1Arg = new RealRegArg(regName,regID,regFile);
	ssaP.setSSANum(src1SSANum);
	src1Arg->addProperty(ssaP);

	if (src2SSANum != _INVALID_)
	{
		src2Arg = new RealRegArg(regName,regID,regFile);
		ssaP.setSSANum(src2SSANum);
		src2Arg->addProperty(ssaP);
	}

	iselOp = new ComputeOp();
	iselOp->initOpCode(opc);
	iselOp->addOperand(destArg, _DEST_, _DEST_LIST_);
	iselOp->addOperand(src1Arg, _SRC_1_);
	if (src2SSANum != _INVALID_)
		iselOp->addOperand(src2Arg, _SRC_2_);
	
	if (pred!= NULL)
	{
		PAPListIter iter;
		int i = _PRED_SRC_BEGIN_;
		for (iter = (((OpPredicate *)pred)->ccList).begin(); iter != (((OpPredicate *)pred)->ccList).end(); iter++)
		{
			if (*iter)
			{			
				iselOp->addOperand((BaseArgument *)((*iter)->arg), i);
				PredProperty predProp;
				if ((*iter)->negated)
				{
					if (iselOp->ptrToOperand(i))
						iselOp->ptrToOperand(i)->addProperty(predProp); 
				}
				i++;
			}
		}
	}

	delete src1Arg;
	if (src2SSANum != _INVALID_)
	{
		delete src2Arg;
	}
	delete destArg;

	return iselOp;
}


void addPredicateUse(BaseOperation* ifOp, BaseOperation* predOp) 
{
 if (!ICGenUDDU) return;

 //_DEST_ arg of op that IF depends on is the CC (always true)
 ComputeOp* ccOp;
 BaseArgument* use=ifOp->ptrToOperand(_SOURCE_1_);
 
 //ifOp->print();

	UDChainProperty* udProp = (UDChainProperty*)use->getPtrToProperty(_UDCHAIN_);
 ASSERT(udProp!=NULL);
 ASSERT(udProp->getDefList().numItems()==1);
	MyLinkedListIterator<BaseOperation*>* iter = udProp->defListIteratorForw();
 //we found the op that computes the CC
	ccOp = (ComputeOp*)(iter->currentElement());
	delete iter;

 //ccOp->print();
// cout << "Adding dependency arcs between " << currRoutine->oMapper->getInstr(ccOp) << "(" << ccOp << ") to " << currRoutine->oMapper->getInstr(predOp) << "(" << predOp << ")" << endl;

 //predOp->print();

 //add the def
 BaseArgument* def=ccOp->ptrToOperand(_DEST_, _DEST_LIST_);
	DUChainProperty* duProp=(DUChainProperty*)def->getPtrToProperty(_DUCHAIN_);
	if ( duProp ) 
	{
  duProp->addUse(predOp);
 }
	else 
 {
		DUChainProperty tmpProp;
		tmpProp.appendUse(predOp);
		def->addProperty(tmpProp);
	}

 //add the use
 use=predOp->ptrToOperand(_SOURCE_1_);
	udProp = (UDChainProperty*)use->getPtrToProperty(_UDCHAIN_);
	if ( udProp ) 
 {
		udProp->addDef(ccOp);
	}
 else 
 {
		UDChainProperty tmpProp;
		tmpProp.appendDef(ccOp);
		use->addProperty(tmpProp);
	}
}

extern int getNewSSANumForArg(BaseArgument *arg);


void NormalOp::ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList)
{
	if (pred!=NULL)
	{
		OpPredicate* opPred=(OpPredicate*)pred;
		for (PAPList::iterator i=(PredicateArg**)(opPred->ccList.begin());i!=(opPred->ccList.end());i++) //for each predicate argument
		{
			PredicateArg* pa=*i;
			addPredicateUse(pa->op, this);
		}
	}
	
	if (pred!=NULL) //attach the predicate if not NULL (no predicate)
	{		
		MyString opcode_name;
		getOpcodeName(opcode_name);
		// If the operation is a predicated operation, then just add the predicate source
		if (GlobalPredicationInformation.IsPredicatedOperation(opcode_name.getStr()))
		{
			// just add the operands
			// addOperand((*iter), _PRED_SRC_START_ + i);
			PAPListIter iter;
			int i = _PRED_SRC_BEGIN_;
			for (iter = (((OpPredicate *)pred)->ccList).begin(); iter != (((OpPredicate *)pred)->ccList).end(); iter++)
			{
				if (*iter != NULL)
				{
					addOperand((*iter)->arg, i);
					i++;
				}
			}		


			/*
			This has to be changed... this should only change the operand
			and not create a new one.
			*/
		}
		else
			//If the operation is unpredicated operation then
		{
			MyString newopcode_name = GlobalPredicationInformation.GetPredicatedCounterpart(opcode_name);
			if (newopcode_name.isEmpty())
				// This has to be predicated using Isel
			{
				printf("This instruction has to be predicated using Isel\n");
				
				ArgListIter dIter;
				for (dIter = _destOperandList.begin(); dIter != _destOperandList.end(); dIter++)
				{
					if (*dIter)
					{
						int NeedIsel = 0;
						MyLinkedList <BaseOperation *> *UseList = GetUseList(*dIter);
						
						// Do we need an Isel
						MyLinkedListIterator <BaseOperation *> *uIter;
						uIter = UseList->elementsForward();
						while(uIter->hasMoreElements())
						{
							if (!(isPresentOperation(uIter->currentElement(), OperationsList)))
								NeedIsel++;
							uIter->nextElement();
						}
						delete (uIter);

						if (NeedIsel)
						{
							/*
							OpListIter oIter;
							for (oIter = OperationsList.begin(); oIter != OperationsList.end(); oIter++)
							{
								(*oIter)->shortPrint(cout);
							}
							*/

							BaseArgProperty *prop;
							prop = (*dIter)->getPtrToProperty(_SSA_);
							int OldSSANum = ((SSAArgProperty*)prop)->getSSANum();
							
							int NewSSANum = getNewSSANumForArg(*dIter);
							
														
							
							MyLinkedListIterator <BaseOperation *> *uIter;
							uIter = UseList->elementsForward();
							while(uIter->hasMoreElements())
							{
								if (uIter->currentElement() != this)
								{
									if (isPresentOperation(uIter->currentElement(), OperationsList))
									((NormalOp *)(uIter->currentElement()))->ChangeReg(OldSSANum, NewSSANum, *dIter);
								}
								uIter->nextElement();
							}
							delete (uIter);
							
							
							// Now change the destination register
							((SSAArgProperty*)prop)->setSSANum(NewSSANum);

														
							// Generate the isel
							OpCode opc;
							if (GlobalPredicationInformation.GetIselOpCode(opc))
							{	
								// first create the Isel
								ComputeOp *tempOp = createISELOp(opc, (*dIter), (OpPredicate *)pred, OldSSANum, NewSSANum);

								// now add this to the Isel list
								IselList.push_back((BaseOperation *)tempOp);
							}

						}

					}
				}
			}
			else
				// If the operation has a predicated counterpart then
			{
				changeOpCode(newopcode_name.getStr());
				
				PAPListIter iter;
				int i = _PRED_SRC_BEGIN_;
				for (iter = (((OpPredicate *)pred)->ccList).begin(); iter != (((OpPredicate *)pred)->ccList).end(); iter++)
				{
					if (*iter)
					{
						addOperand((*iter)->arg, i);

						PredProperty predProp;
						if ((*iter)->negated)
							this->ptrToOperand(i)->addProperty(predProp); 
						
						i++;
					}
				}
			}
		}
	}
	else
	{
		OpPredicate* newPred=new OpPredicate("TRUE");
		addProperty(*newPred);
		
	}
}


// returns 1 if the operation is present
// returns 0 if the operation is not present
int isPresentOperation(BaseOperation *op, OpList OperationsList)
{
	OpListIter oIter;
	for(oIter = OperationsList.begin(); oIter != OperationsList.end(); oIter++)
	{
		if ((*oIter) == op)
			return 1;
	}
	return 0;
}



// returns 1 if the operand is present
// returns 0 if the operand is not present
// Note :  Checks only in the sourceOperandList 
int NormalOp::isPresentOperand(int SSANum)
{
	ArgListIter sIter;
	int done = 0;
	for (sIter = _sourceOperandList.begin(); sIter != _sourceOperandList.end(); sIter++)
	{
		if (*sIter)
		{
			BaseArgProperty *prop;
			prop = (*sIter)->getPtrToProperty(_SSA_);
			if (prop)
			{
				if ( ((SSAArgProperty*)prop)->getSSANum() == SSANum)
					done = 1;
			}
		}
	}
	return done;
}

extern int equalRegs(BaseArgument*, BaseArgument*);

// Returns 1 if changeReg is succesful, else returns 0.
int NormalOp::ChangeReg(int OldSSANum, int NewSSANum, BaseArgument *reg)
{
	ArgListIter sIter;
	int done = 0;
	for (sIter = _sourceOperandList.begin(); sIter != _sourceOperandList.end(); sIter++)
	{
		if (*sIter)
		{
			BaseArgProperty *prop;
			prop = (*sIter)->getPtrToProperty(_SSA_);
			if (prop)
			{
				if (equalRegs(reg, *sIter))
				{
					((SSAArgProperty*)prop)->setSSANum(NewSSANum);
					done = 1;
				}
			}
		}
	}
	return done;
}


/* to be removed later : Aviral/Partha
void BaseOperation::ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList)
{
 //print();
 //cout << endl;

 //make this op depend on the operation computing its predicate
// if ((ifOp!=NULL) && !this->isIfOp()) 
// {
//  addPredicateUse(ifOp, this);
// }
// //and if pred!=NULL on all other predicate-computing ops
 if (pred!=NULL)
 {
  OpPredicate* opPred=(OpPredicate*)pred;
  for (PAPList::iterator i=(PredicateArg**)(opPred->ccList.begin());i!=(opPred->ccList.end());i++) //for each predicate argument
  {
   PredicateArg* pa=*i;
   addPredicateUse(pa->op, this);
  }
 }

 if (pred!=NULL) //attach the predicate if not NULL (no predicate)
 {
  OpPredicate* newPred=(OpPredicate*)(pred->copy());
  addProperty(*newPred);
 }
 else
 {
  OpPredicate* newPred=new OpPredicate("TRUE");
  addProperty(*newPred);
 }
}

*/
