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

// If src2SSANum is specified, then, the ISEL is of the form ISEL <dest> <src1> <src2> <pred>
// else it is of the form ISEL <dest> <src> <pred>
// 
// Also, note that we currently assume that the predicate is just one argument.
// Inorder to make it a list of arguments, create the list before hand and pass it to this function.
//
ComputeOp *createISELOp(OpCode & opc, BaseArgument *regArg, BaseArgument *predArg, int destSSANum,  int src1SSANum, int src2SSANum = _INVALID_)
{
	char *regName;
	int regID;
	RegFileNum regFile;
	RegisterArg *src1Arg, *src2Arg, *destArg;
	SSAArgProperty ssaP;
	ComputeOp *iselOp;

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

	if (predArg != NULL)
		iselOp->addOperand(predArg, _PRED_SRC_BEGIN_);

	delete src1Arg;
	delete src2Arg;
	delete destArg;

	return iselOp;
}
/*
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
 BaseArgument* def=ccOp->ptrToOperand(_DEST_);
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
*/
/*
void BaseOperation::ifConvert(BaseOpProperty* pred, BaseOperation* ifOp)
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
