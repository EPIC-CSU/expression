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
//==========================================================================
// File: 		CompExecPossib.cc
// Created:		Jan. 9, 98
// Author:		Xiaomei Ji
//=========================================================================

#include "ExecPossibBBProperty.h"
#include "Routine.h"
#include "BasicBlockNode.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "BaseOpSlot.h"
#include "BaseOperation.h"
#include "BranchProperty.h"
#include "MyLinkedList.h"
#include "Array.h"


#define MINIMUL(a, b)  (a)<(b)?(a):(b)



static
void
resetThroughPossib(BasicBlockNode*pred, BasicBlockNode*bb, float& possib) {
	BaseInstruction* instr = pred->getLastInstr();
#ifdef DEBUG
	assert(instr->isNormalInstr());
#endif
	BaseOpSlot&	cntlOpSlot = ((NormalInstr*)instr)->getControlOpSlot(0);
	BaseOperation*  op = cntlOpSlot.getPtrToOperation();
        if ( op == NULL ) 
		return;
        BranchProperty* prop = (BranchProperty*)op->getPtrToProperty(_BRANCH_);
	if ( !prop ){ // no heuristic, assume 50%
		possib = 0.5;
		return;
	}
	op = cntlOpSlot.getPtrToTrueBranch()->getPtrToOperation();
	if ( op == NULL ) 
		return;
	BaseInstruction* target = op->getTargetInstr();
	if ( target == bb->getFirstInstr() )   // true branch
		possib = (prop->getCond())/100.0;	
	else // false branch
		possib = 1 - (prop->getCond())/100.0;
}


// return value indicates whether the possibility of this basic block changes,
// return value 1 indicates change,  0 indicates no-change

static
int
compEPOfBB(BasicBlockNode* bb) {
   // bb's possibility = min of ( 100, sum ( pred's possibility * 
   // 				       the possibility from pred to bb ) )

   int sum = 0;

   MyLinkedListIterator<BasicBlockNode*>* iter = bb->getCFPred().createPredListIterForw();
   while ( iter->hasMoreElements() ) {
   	int predPossib = 0;
   	float throughPossib = 1;
        BasicBlockNode* pred = iter->currentElement();
	ExecPossibBBProperty* prop = (ExecPossibBBProperty*)pred->getPtrToProperty(_EXECPOSSIBILITY_);
	if ( prop )  
           predPossib = prop->getPossibility();
   	// if pred only has one succ
        if ( pred->getFirstEdge() && pred->getNextEdge() ) {
		// reset throughPossib
		resetThroughPossib(pred, bb, throughPossib);
 	}
	sum += (int)(predPossib * throughPossib);
	iter->nextElement();
   }
   sum = MINIMUL(sum, 100);
   ExecPossibBBProperty* oldProp = (ExecPossibBBProperty*)bb->getPtrToProperty(_EXECPOSSIBILITY_);
   if ( oldProp ) {
      if ( sum == oldProp->getPossibility() ) {
        delete iter;
	return 0;
      }
      oldProp->setPossibility(sum);
      delete iter;
      return 1;
   }
   ExecPossibBBProperty newProp(sum); 
   bb->addProperty(newProp);
   delete iter;
   return 1;
}


void
Routine::compExecPossib() {
#ifdef DEBUG
	printf("Begin to compute exec possibility \n");
#endif

	ArrayIterator<BasicBlockNode*>* iter = _cfg.createDFSBBIter();
	int change = 1;
        // initialization first bb's possibility as 100
	if ( iter->hasMoreElements() == _NO_ )  {
		delete iter;
		return;
        }
	// at least one basic block
	ExecPossibBBProperty prop(100);
	iter->currentElement()->addProperty(prop);
	while ( change ) {
	   change = 0;
	   iter->nextElement(); // across the first;
	   while ( iter->hasMoreElements() ) {
		if ( compEPOfBB(iter->currentElement()) )
			change = 1;
		iter->nextElement();
	   }
	   iter->reset();
	}
 	delete iter;

#ifdef DEBUG
	printf("end of computing exec possibility \n");
#endif
}
