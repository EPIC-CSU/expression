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
//=================================================================
// dead code elimanation, maintain: cfg, bb, and instrFlow
//
// first: delete operations, 
// then:  delete instructions, maintain bb, istrFlow 
// last:  delete BB, maintain cfg
//
// to if struction: first mark the if statement and the true or false 
// branch as deleted, then, delete.
//======================================================================

#include "stdafx.h"

#include "BranchProperty.h"
#include "Routine.h"
#include "BasicBlockNode.h"
#include "BaseOperation.h"
#include "JumpOp.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "BaseOpSlot.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "FlowOpSlot.h"
#include "BBIterator.h"
#include "DeleteProperty.h"
#include "DUProperty.h"
#include "UDProperty.h"
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

extern int maintainIRWalker;
extern Routine *currRoutine;

OpList deletedOperList;

Vector<int> numDeletedArray;

static
int 
isValid(ControlFlowGraph& cfg) {
  return _YES_;
}


// because of the restriction of the deleted instruction, 
// that is ; only delete instructions that has one flow op
// so, the deleting bb only has one succ.
// that is the limitation.

static
void
delBB(BasicBlockNode* bb, Routine* routine) {
	// pred and succ
	// dfst order, and dfst number
	// does delete affect the bbs' order in DFST ???????????
	
	// first, find this bb's only succ
	BasicBlockNode* succ = bb->getFirstSucc();
	if ( succ == _NULL_ )
		succ = bb->getNextSucc();
	
	// delete the bb from succ's predecessors
	if (succ != NULL)
		succ->removeCFPred(bb);
	
	// preds
	MyLinkedListIterator<BasicBlockNode*>* predIter = bb->getCFPred().createPredListIterForw();
	if ( predIter != _NULL_ ) {
		while ( predIter->hasMoreElements() ) {
			BasicBlockNode* pred = predIter->currentElement();
			// check bb is pred's first succ or next succ 
			BasicBlockEdge edge;
			edge.setNext(succ);
			// ?????????? how to set the edge's type
			edge.setToTreeArc();
			if ( pred->getFirstSucc() == bb ) {
				pred->setFirstEdge(edge);
				if ( pred->getNextSucc() == succ )
					pred->delNextSucc();
			} else {
				if ( pred->getFirstSucc() != succ )
					pred->setNextEdge(edge);
				else // delete the second edge
					pred->delNextSucc();
			}
			if (succ != NULL)
			{
				succ->addUniqCFPred(pred);
			}
			predIter->nextElement();
		}
		delete predIter; 
	} 

  // delete this bb from routine's cfg,
  // and change the dfs order of other bbs
  ControlFlowGraph& cfg = routine->getCFG();
  if ( cfg.getStartNode() == bb )
     cfg.setStartNodePtr(succ);
  ArrayIterator<BasicBlockNode*>* bbIter = cfg.createDFSBBIter();
  int find = 0;
  while ( bbIter->hasMoreElements() ) {
     BasicBlockNode* tmp = bbIter->currentElement();
     if ( find ) // change dfsOrder to minus 1
        tmp->setDFSNum(tmp->getDFSNum()-1); 
     if ( tmp == bb ) 
        find = 1;
     bbIter->nextElement();
  }
  delete bbIter;
  // remove this bb from dfsList and bbList
  cfg.removeBBFromBBList(bb);
  cfg.removeBBFromDFSList(bb);

  // keep count.
  numDeletedArray[0]++;
  //
  // 4 == BB
}




static
int
findIndex(NormalInstr* instr, BaseOpSlot* slot, int type) {
   int index = 0;
   switch ( type ) {
      case DATA_OP:
      {
         ArrayIterator<DataOpSlot>* iter = instr->getDataOpSlotIter();
         while ( iter->hasMoreElements() ) {
            if ( iter->currentElement().getPtrToOperation() == slot->getPtrToOperation() )
               return index;
            iter->nextElement();
            index++;
         }
         delete iter;
         break;
      }
      case CONTROL_OP:
      {
         ArrayIterator<ControlOpSlot>* iter = instr->getControlOpSlotIter();
         while ( iter->hasMoreElements() ) {
            if ( iter->currentElement().getPtrToOperation() == slot->getPtrToOperation() )
               return index;
            iter->nextElement();
            index++;
         }
         delete iter;
         break;
      }
      case FLOW_OP:
      {
         ArrayIterator<FlowOpSlot>* iter = instr->getFlowOpSlotIter();
         while ( iter->hasMoreElements() ) {
			 if ( iter->currentElement().getPtrToOperation() == slot->getPtrToOperation() ) {
				 delete iter;
               return index;
			 }
            iter->nextElement();
            index++;
         }
         delete iter;
         break;
      }
   }
#ifdef DEBUG
   assert(0>1);
#endif

   //NOS
   ASSERT_TRACE(0,"Forced EXIT!");
   return 0;
}


static
void
setHeadOfInstr(NormalInstr* instr, BaseOpSlot* slot) {

	if (slot == NULL)
	{
		// error condition.
		//
		cout << "Warning: Instruction does not have a valid header" << endl;
		instr->setHeaderOp(_INVALID_, NO_OP);
		return;
	}

   BaseOperation* op = slot->getPtrToOperation();
   if ( op == NULL )
	return;
   if ( op->isDataOp() ) 
      instr->setHeaderOp(DATA_OP, findIndex(instr, slot, DATA_OP));
   else if ( op->isControlOp() ) 
      instr->setHeaderOp(CONTROL_OP, findIndex(instr, slot, CONTROL_OP));
   else  // FlowOp
      instr->setHeaderOp(FLOW_OP, findIndex(instr, slot, FLOW_OP));
}


static
void
delOpInSlot(NormalInstr* instr, BaseOpSlot* prev, BaseOpSlot* curr) {

	if (curr == NULL)
	{
		// error condition.
		//
		cout << "Warning: Recieved NULL curr slot in Delete Op." << endl;
//		instr->setHeaderOp(_INVALID_, NO_OP);
		return;
	}

	BaseOperation* op =  curr->getPtrToOperation();
	if ( op == NULL )
		return;
	if ( op->isFlowOp() ) {
		// only delete this opSlot from opSlotList 
		if ( op->getPtrToProperty(_DELETE_) )
		{
			//instr->delFlowSlot((FlowOpSlot*)curr);
			curr->deleteOperation();
			
			// keep count.
			numDeletedArray[0]++;
			//
			// 0 == FLOW OP
		}
		return;
	}
	if ( op->getPtrToProperty(_DELETE_) ) {
		// delete this opSlot from instr 
		if ( op->isDataOp() ) {
			BaseOpSlot* next = ((DataOpSlot*)curr)->getPtrToNextSlot();
			
			/* IF DEBUG PRINT REQUIRED THEN UNCOMMENT THIS
			cout << " (* " << curr->getPtrToOperation() << " *) ";
			(curr->getPtrToOperation())->shortPrint(cout);
			cout << endl;
			*/
			if ( prev ) {
				if ( prev->getPtrToOperation() 
					&& prev->getPtrToOperation()->isDataOp() ) { 
					((DataOpSlot*)prev)->setNext(next);      
				} else {
					if ( ((ControlOpSlot*)prev)->getPtrToTrueBranch() == curr ) 
						((ControlOpSlot*)prev)->setTrueBranch(next);
					else if (((ControlOpSlot*)prev)->getPtrToFalseBranch() == curr)
						((ControlOpSlot*)prev)->setFalseBranch(next);
#ifdef DEBUG
					assert(0>1);
#endif
				}
			} else {  // change the head of instr
				setHeadOfInstr(instr, next);
			}
			//instr->delDataSlot((DataOpSlot*)curr); // delete curr
			//
			if (maintainIRWalker)
			{
				currRoutine->oMapper->removeOper(op);
			}
			curr->deleteOperation();
			
			// keep count.
			numDeletedArray[1]++;
			//
			// 1 == DATA OP
			delOpInSlot(instr,prev,next);
		} else {
#ifdef DEBUG
			assert(op->isIfOp()); 
#endif
			BaseOpSlot* next;
			BaseOpProperty* prop = op->getPtrToProperty(_BRANCH_); 
			if ( prop ) {
				if ( ((BranchProperty*)prop)->getCond() == _YES_ ) {
					next = ((ControlOpSlot*)curr)->getPtrToTrueBranch();
					//delOpInSlot(((ControlOpSlot*)curr)->getPtrToFalseBranch());
				} else if ( ((BranchProperty*)prop)->getCond() == _NO_  ) {
					next = ((ControlOpSlot*)curr)->getPtrToFalseBranch();
					//delOpInSlot(((ControlOpSlot*)curr)->getPtrToTrueBranch());
				}
			} else { 
				// indicates this if structure is included in a deleted structure
				// ???????????????  
				// next = ((ControlOpSlot*)curr)->jointSlot();
				
				//delOpInSlot(((ControlOpSlot*)curr)->getPtrToFalseBranch());
				//delOpInSlot(((ControlOpSlot*)curr)->getPtrToTrueBranch());
			}
			if ( prev ) {
				if ( prev->getPtrToOperation() 
					&& prev->getPtrToOperation()->isDataOp() ) { 
					((DataOpSlot*)prev)->setNext(next);
				} else { // CONTROL_OP
					
					if ( ((ControlOpSlot*)prev)->getPtrToTrueBranch() == curr ) 
						((ControlOpSlot*)prev)->setTrueBranch(next);
					else if (((ControlOpSlot*)prev)->getPtrToFalseBranch() == curr)
						((ControlOpSlot*)prev)->setFalseBranch(next);
#ifdef DEBUG
					assert(0>1);
#endif
				}
			} else { // change the head op of the instrution
				setHeadOfInstr(instr, next);
			}
			
			
			delOpInSlot(instr,prev,((ControlOpSlot*)curr)->getPtrToTrueBranch());
			delOpInSlot(instr,prev,((ControlOpSlot*)curr)->getPtrToFalseBranch());
			
			//instr->delCntlSlot((ControlOpSlot*)curr);
			//
			if (maintainIRWalker)
			{
				currRoutine->oMapper->removeOper(op);
			}
			curr->deleteOperation();

			// keep count.
			numDeletedArray[2]++;
			//
			// 2 == CONTROL OP
			
		}
	} else  {
		if ( op->isDataOp() ) {
			delOpInSlot(instr,curr,((DataOpSlot*)curr)->getPtrToNextSlot());
		} else { //CONTROL_OP 
			delOpInSlot(instr,curr,((ControlOpSlot*)curr)->getPtrToTrueBranch());
			delOpInSlot(instr,curr,((ControlOpSlot*)curr)->getPtrToFalseBranch());
		}
	}
}


static
void
delOpInInstr(BaseInstruction* instr) {

	// Added by SAH on Feb 27, 2001
	//
	// Check for empty instr and NULL instr.
	//
	if ((instr != NULL) && (!((NormalInstr *)instr)->isEmpty()))
	{
      // according to the op flow
      BaseOpSlot& slot = ((NormalInstr*)instr)->getRootOpSlot();
      delOpInSlot((NormalInstr*)instr,  _NULL_, &slot);
	}
}




BasicBlockNode*
findBBOfInstr(BaseInstruction* instr, ControlFlowGraph& cfg) {
   LinkedListIterator<BasicBlockNode>* bbIter = cfg.createBBIterForw();
   while ( bbIter->hasMoreElements() ) {
      BasicBlockNode& bb = bbIter->currentElement();
      BBForwIterator instrIter(&bb);
      while ( instrIter.hasMoreElements() ) {
         if ( instrIter.currentElement() == instr ) {
            delete bbIter;
            return &bb;
         }
         instrIter.nextElement();
      }
      bbIter->nextElement();
   }
   delete bbIter;
#ifdef DEBUG
   assert(0>1);
#endif

   //NOS
   ASSERT_TRACE(0,"Forced EXIT!");
   return NULL;
}


extern void setTargetInstrOfPred(BasicBlockNode* bb, BaseInstruction* instr, BaseInstruction* target, int changeLeaderTrailer = 1);

void
delInstrInBB(BaseInstruction* instr, BaseInstruction* target, BasicBlockNode* bb, Routine* routine) {

	BaseInstruction *prevLeader, *prevTrailer;
	prevLeader = bb->getFirstInstr();
	prevTrailer = bb->getLastInstr();

    setTargetInstrOfPred(bb, instr, target, 1); // modify the leader/trailer of bb if necessary.

	// take care of the condition when the bb only has this instruction.
	// i.e. prevLeader == instr == prevTrailer
	//
	if (( prevLeader == instr ) && (prevTrailer == instr))
	{
		// make this bb a dummy bb.
		//
		bb->initLeader(NULL);
		bb->initTrailer(NULL);
	}
}

extern void setTargetOfNormalInstr(NormalInstr* predInstr, BaseInstruction* instr, BaseInstruction* target);

static
void
delInstrInRoutine(BaseInstruction* delInstr, BaseInstruction* target, Routine* routine) {
   MyLinkedListIterator<BaseInstruction*>* iter = routine->instrListIteratorForw();
   while ( iter->hasMoreElements() ) {
      BaseInstruction* instr = iter->currentElement();
      setTargetOfNormalInstr((NormalInstr*)instr, delInstr, target);
      iter->nextElement();
   }
   delete iter;
}


// return val -1: means can not be deleted
// otherwise, the return val means the index of the flow op

static
int
canDelNormalInstr(NormalInstr* instr) {
   ArrayIterator<DataOpSlot>* iter = instr->getDataOpSlotIter();
   while ( iter->hasMoreElements() ) {
      BaseOperation* op = iter->currentElement().getPtrToOperation();
      if ( op != NULL )
         return -1;
      iter->nextElement();
   }
   delete iter;
   
   ArrayIterator<ControlOpSlot>* cntlIter = instr->getControlOpSlotIter();
   while ( cntlIter->hasMoreElements() ) {
      BaseOperation* op = cntlIter->currentElement().getPtrToOperation();
      if ( op != NULL )
         return -1;
      cntlIter->nextElement();
   }
   delete cntlIter;
  
   int num = 0;
   int index = 0;
   int retVal = 0;
   ArrayIterator<FlowOpSlot>* flowIter = instr->getFlowOpSlotIter();
   while ( flowIter->hasMoreElements() ) {
      BaseOperation* op = flowIter->currentElement().getPtrToOperation();
      if ( op != NULL ) {
		  if (op->isGotoOp()) return -1;

         retVal = index;
         num++;
         if ( num > 1 ) 
            return -1;
      }
      flowIter->nextElement();
      index++;
   }
   delete flowIter;
   return retVal;
}


// only delete normal instr that 
// has no labels
// empty data and cntl and only one flow opSlot
//
// if an instr can be deleted, 
// delete  it from IList,
// maintain InstrFlow by changing all instr that points to this
// instr  point to its target instr 

void
delInstr(BaseInstruction* instr,Routine* routine,BasicBlockNode* bb=0) {
   BaseInstruction* next = 0;
   NormalInstr* normalInstr = (NormalInstr*)instr;
   if ( normalInstr->getPtrToLabelArg() )
         return;
   int index;
   if ( (index = canDelNormalInstr(normalInstr)) == -1 ) 
	return;
   next = ((JumpOp*)((FlowOpSlot&)normalInstr->getFlowOpSlot(index)).getPtrToOperation())->getTargetInstr();
   ControlFlowGraph& cfg = routine->getCFG();
   if ( isValid(cfg) ) { 
      if ( bb ) {
         delInstrInBB(instr, next, bb, routine);
      } else {
         // first find which basic block this instr belongs to
         delInstrInBB(instr, next, findBBOfInstr(instr, cfg), routine);
      }
   } else {
      delInstrInRoutine(instr, next, routine);
   }     


	if (maintainIRWalker)
	{
		currRoutine->iWalker->removeInstr(instr);
	}
	
   // delete instr from the IList
   routine->removeInstruction(instr);

  // keep count.
  numDeletedArray[3]++;
  //
  // 3 == INSTR
}

void markOp(BaseOperation *oper)
{
	DeleteProperty prop;

/* IF DEBUG PRINT REQUIRED THEN UNCOMMENT THIS

	cout << " (* " << oper << " *) ";
	oper->shortPrint(cout);
	cout << endl;
*/
	oper->addProperty(prop);

	 // When an operation is marked store its ptr location in an array.
	 //
	 // This is used later when maintaining the DU properties that were
	 // spoilt by removing this operation.
	 //
	 deletedOperList.push_back(oper);

}

void markOpsInInstr(BaseInstruction *instr)
{
	// Iterate over data ops, and mark each op.
	//
	ArrayIterator<DataOpSlot> *diter;
	diter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (diter->hasMoreElements())
	{
		if ((diter->currentElement()).hasOper())
			markOp((diter->currentElement()).getPtrToOperation());
		diter->nextElement();
	}
	delete diter;
}

int numMarkedInThisStage;

int hasBeenMarked(BaseOperation *oper)
{
	if (oper->getPtrToProperty(_DELETE_) != NULL)
		return _YES_;

	return _NO_;
}

int allOpsMarked(MyLinkedList<BaseOperation *> & useList)
{
	MyLinkedListIterator<BaseOperation *> *iter = useList.elementsForward();

	while (iter->hasMoreElements())
	{
		if (!hasBeenMarked(iter->currentElement()))
			return _NO_;

		iter->nextElement();
	}
	return _YES_;
}

extern int isReturnParameter(BaseArgument *arg);
extern int isSPArg(BaseArgument *arg);
extern int isGenericArchDefinedArg(BaseArgument *arg);
extern int isNOP(int opCode);

int isNOP(BaseOperation *oper)
{
	return (isNOP((oper->getOpCode()).index()));
}

void markIfNoDefUse(BaseOperation *oper)
{
	if (hasBeenMarked(oper)) // has already been marked to be deleted. do nothing.
		return;

	// First check to see if it has a DEST_LIST.
	// 
	// If it does not, do not mark it.
	// Note: This holds for Store, If and Flow ops.
	// 
	// Then, do not mark if it is a call or return op.
	//
	// Then, do not mark if it the dest is a return parameter.
	// (i.e. R4)
	//
	// Finally, if it does not have a def-use property, mark it.

	if (oper == NULL) return;
	
	if (oper->isCallRetOp()) return;

	// if the oper is a NOP, it has probably been introduced to handle pipeline/operation latencies.
	//
	if (isNOP(oper)) return;

	// if oper does not define any argument, return.
	//
	if ((oper->destOperandList()).empty()) return;

	ArgListIter ai;
	ArgList al;
	BaseArgument *arg;

	oper->destOperandsList(al);

	int hasProp = 0;
	for (ai = al.begin(); ai != al.end(); ai++)
	{
		if (*ai == NULL) continue;
		
		arg = *ai;

		// if arg is not of register type, do not mark.
		// Note: If we have come this far, 
		// Dest args are not supposed to be of any other type.
		// So, print a warning.

		if (!arg->isRegisterArg())
		{
			// error condition.
			//
			cout <<"Warning: Recieved a non-register Destination argument while marking useless operations (for DCE)" << endl;
			return;
		}

		if (isReturnParameter(arg)) return;
		if (isSPArg(arg)) return;

		BaseArgProperty *prop = arg->getPtrToProperty(_DUCHAIN_);
		
		if (prop != NULL)
		{
			// Added by SAH on 09/24/01
			//
			// even if the operation has a DU property,
			// it may be the case that the DU list is empty.
			// in that case, treat it like it doesnt have a DU property.
			//
			if (!(((DUChainProperty *)prop)->getUseList()).isEmpty())
			{
				// also, if it has a NON NULL DU property, make sure that all the ops in 
				// the use list are valid. For ex. if all the ops in use list have also been marked as 
				// dead, then this op should me marked for deletion too.
				//
				if (!allOpsMarked(((DUChainProperty *)prop)->getUseList()))
				{
					hasProp = 1;
					break;
				}
			}

		}
	}
	if (!hasProp)
	{
		markOp(oper);

		numMarkedInThisStage++;
	}
}

void markIfNoUseDef(BaseOperation *oper)
{
	if (hasBeenMarked(oper)) // has already been marked to be deleted. do nothing.
		return;

	// First check to see if it has a SOURCE_LIST.
	// 
	// If it does not, do not mark it.
	// 
	// Then, do not mark if it is a call or return op.
	//
	// Then, do not mark it if any source is a call parameter, return, or stack-pointer.
	// (i.e. R4)
	//
	// Finally, if it does not have a def-use property, mark it.

	if (oper == NULL) return;
	
	if (oper->isCallRetOp()) return;

	// if oper does not have any source argument, return.
	//
	if ((oper->sourceOperandList()).empty()) return;

	ArgListIter ai;
	ArgList al;
	BaseArgument *arg;

	oper->sourceOperandsList(al);

	int hasProp = 0;
	for (ai = al.begin(); ai != al.end(); ai++)
	{
		if (*ai == NULL) continue;
		
		arg = *ai;

		// if arg is not of register type, do not mark.
		//
		if (!arg->isRegisterArg())
		{
			hasProp = 1;
			break;
		}

		if (isGenericArchDefinedArg(arg)) return;

		BaseArgProperty *prop = arg->getPtrToProperty(_UDCHAIN_);
		
		if (prop != NULL)
		{
			hasProp = 1;
			break;
		}
	}
	if (!hasProp)
	{
		markOp(oper);

		numMarkedInThisStage++;
	}
}

void markNonDUOpsInInstr(BaseInstruction *instr)
{
	// Go thru the data-ops and search for those without any DU properties.
	//
	ArrayIterator<DataOpSlot> *iter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).hasOper())
		{
			markIfNoDefUse((iter->currentElement()).getPtrToOperation());
		}
		iter->nextElement();
	}
	delete iter;
}

void markNonUDOpsInInstr(BaseInstruction *instr)
{
	// Go thru the data-ops and search for those without any UD properties.
	//
	ArrayIterator<DataOpSlot> *iter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).hasOper())
		{
			markIfNoUseDef((iter->currentElement()).getPtrToOperation());
		}
		iter->nextElement();
	}
	delete iter;
}

// This is the basicblocknode counterpart of markDeadOperations(Routine *)
//
// See that function for comments.
//
void markDeadOperations(BasicBlockNode *bb)
{
	BBForwIterator bbIter(bb);
	while (bbIter.hasMoreElements())
	{
		BaseInstruction *instr = bbIter.currentElement();
		markNonDUOpsInInstr(instr);
		markNonUDOpsInInstr(instr);

		bbIter.nextElement();
	}
}
void markDeadOperations(Routine *rout)
{
	// There are various reasons why an operation can be marked as dead.
	//
	// (1) It has no def-use property. i.e. it does not define a useful value
	// (2) It will never be executed. e.g. an if expression that evaluates to
	//     a constant will kill one of its paths.
	// (3) It has no use-def property. i.e. none of its sources have a definition.
	//     note: in this case, we have to make sure that the source is not a call parameter, or return,
	//     or the stack pointer.

	// Currently we only handle the first situation.
	//
	// Code for the third situation has been implemented and tested, but has been commented out.
	// This is becos it was virtually useless.

   MyLinkedListIterator<BaseInstruction*>* iter = rout->instrListIteratorForw();
   while ( iter->hasMoreElements() ) 
   {
      BaseInstruction* instr = iter->currentElement();
	  markNonDUOpsInInstr(instr); // CASE 1
//	  markNonUDOpsInInstr(instr); // CASE 3
	  iter->nextElement();
   }
   delete iter;

}

void deleteUselessOps(BasicBlockNode *bb)
{
	BBForwIterator instrIter(bb);
	while ( instrIter.hasMoreElements() ) {
		BaseInstruction* instr = instrIter.currentElement();
		instrIter.nextElement();
	    delOpInInstr(instr);
	}
}

// if delEmptyInstrs == 1, then remove all empty instructions in the bb, else keep them.
// default: remove the instrs.
//
void deleteUselessIR(BasicBlockNode & bb, Routine *rout, int delEmptyInstrs)
{
	BBForwIterator instrIter(&bb);
	while ( instrIter.hasMoreElements() ) {
		BaseInstruction* instr = instrIter.currentElement();
		instrIter.nextElement();
	    delOpInInstr(instr);
	}
	if (delEmptyInstrs)
	{
		instrIter.reset();
		while ( instrIter.hasMoreElements() ) {
			BaseInstruction* instr = instrIter.currentElement();
			instrIter.nextElement();
			delInstr(instr, rout, &bb);
		}
	}
}

void deleteUselessIR(Routine *rout)
{
	if (rout == NULL) return;

	if (rout->isEmptyBodyRoutine()) return;

	int cfgValid = isValid(rout->getCFG());

   MyLinkedListIterator<BaseInstruction*>* iter = rout->instrListIteratorForw();
   while ( iter->hasMoreElements() ) {
      BaseInstruction* instr = iter->currentElement();
      iter->nextElement();
      delOpInInstr(instr);
      if ( cfgValid == _NO_ )
         delInstr(instr, rout);
   }

   delete iter;

   // if cfg is valid, delete instr according to bb
   
   LinkedListIterator<BasicBlockNode>* bbIter= (rout->getCFG()).createBBIterForw();
   while ( bbIter->hasMoreElements() ) {
      BasicBlockNode& bb = bbIter->currentElement();
      // forward first. because delete instr following perhaps 
      bbIter->nextElement();
      BBForwIterator instrIter(&bb);
      while ( instrIter.hasMoreElements() ) {
	 BaseInstruction* instr = instrIter.currentElement();
         instrIter.nextElement();
         delInstr(instr, rout, &bb);
      }
   }
   delete bbIter;
}

int hasBeenDeleted(BaseOperation *oper)
{
	for (OpListIter i = deletedOperList.begin(); i != deletedOperList.end(); i++)
	{
		if (oper == *i)
			return _YES_;
	}
	return _NO_;
}

extern void deleteUseDefsFromTo(BaseOperation *fromOp, BaseOperation *toOp, BaseArgument *refArg = NULL);
extern void deleteDefUsesFromTo(BaseOperation *fromOp, BaseOperation *toOp, BaseArgument *refArg = NULL);

void maintainDUAfterDCE(BaseOperation *oper)
{
	if (oper == NULL) return;

	// STEP 1: From this oper's du chains, delete those that point to deleted ops
	// STEP 2: If this oper is marked as deleted, delete all use-def chains from other ops to it.


/* IF DEBUG PRINT REQUIRED THEN UNCOMMENT THIS

	PropNameList pL;
	pL.push_back(_DUCHAIN_);
	cout << " (* " << oper << " *) ";
	oper->printProperties(cout, pL);
	cout << endl;
*/
	ArgListIter ai;
	ArgList & dal = oper->destOperandList();
	BaseArgument *arg;
	BaseArgProperty *prop;

	for (ai = dal.begin(); ai != dal.end(); ai++)
	{
		if (*ai == NULL) continue;
		arg = *ai;

		prop = arg->getPtrToProperty(_DUCHAIN_);
		if (prop == NULL) continue;

		MyLinkedListIterator<BaseOperation*> *iter = ((DUChainProperty *)prop)->useListIteratorForw();
		while (iter->hasMoreElements())
		{
			BaseOperation *useOp = iter->currentElement();
			iter->nextElement();

			// STEP 1
			if (hasBeenDeleted(useOp))
			{
				((DUChainProperty *)prop)->remove(useOp);
			}

			// STEP 2
			if (hasBeenDeleted(oper))
			{
				deleteUseDefsFromTo(useOp, oper);
			}
		}
		delete iter;
	}

	// Similarly for source operands.
	//
	ArgList & sal = oper->sourceOperandList();

	for (ai = sal.begin(); ai != sal.end(); ai++)
	{
		if (*ai == NULL) continue;
		arg = *ai;

		if (!arg->isRegisterArg()) continue;

		prop = arg->getPtrToProperty(_UDCHAIN_);
		if (prop == NULL) continue;

		MyLinkedListIterator<BaseOperation*> *iter = ((UDChainProperty *)prop)->defListIteratorForw();
		while (iter->hasMoreElements())
		{
			BaseOperation *defOp = iter->currentElement();
			iter->nextElement();

			// STEP 1
			//
			if (hasBeenDeleted(defOp))
			{
				((UDChainProperty *)prop)->remove(defOp);
			}

			// STEP 2
			//
			if (hasBeenDeleted(oper))
			{
				deleteDefUsesFromTo(defOp, oper, arg);
			}
		}
		delete iter;
	}
}

void maintainDUAfterDCE(BaseInstruction *instr)
{
	if (instr == NULL) return;

	// Go thru the data-ops and search for those without any DU properties.
	//
	ArrayIterator<DataOpSlot> *diter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (diter->hasMoreElements())
	{
		if ((diter->currentElement()).hasOper())
		{
			maintainDUAfterDCE((diter->currentElement()).getPtrToOperation());
		}
		diter->nextElement();
	}
	delete diter;

	// Go thru the control-ops and search for those without any DU properties.
	//
	ArrayIterator<ControlOpSlot> *citer = ((NormalInstr *)instr)->getControlOpSlotIter();
	while (citer->hasMoreElements())
	{
		if ((citer->currentElement()).hasOper())
		{
			maintainDUAfterDCE((citer->currentElement()).getPtrToOperation());
		}
		citer->nextElement();
	}
	delete citer;
}

void maintainDUAfterDCE(BasicBlockNode *bb)
{
	BBForwIterator bbIter(bb);
	while (bbIter.hasMoreElements())
	{
		BaseInstruction *instr = bbIter.currentElement();
		maintainDUAfterDCE(instr);
		bbIter.nextElement();
	}
}

void maintainDUAfterDCE(Routine *rout)
{
   MyLinkedListIterator<BaseInstruction*>* iter = rout->instrListIteratorForw();
   while ( iter->hasMoreElements() ) 
   {
      BaseInstruction* instr = iter->currentElement();
	  maintainDUAfterDCE(instr);
      iter->nextElement();
   }

   delete iter;

}

void initDCEStatsObject()
{
	// 0 == FLOW OP.
	// 1 == DATA OP.
	// 2 == CONTROL OP.
	// 3 == INSTR.
	// 4 == BB.

	numDeletedArray.clear();

	for (int i = 0; i < 5; i++)
	{
		numDeletedArray.push_back(0);
	}
}

void displayDCEStats()
{
	cout << "---------------------------------------" << endl;
	cout << "-- Num. Data Ops Eliminated: " << numDeletedArray[1] << " ------" << endl;
	cout << "-- Num. Control Ops Eliminated: " << numDeletedArray[2] << " ---" << endl;
//	cout << "-- Num. Flow Ops Eliminated: " << numDeletedArray[0] << " ------" << endl;
	cout << "-- Num. Instructions Eliminated: " << numDeletedArray[3] << " --" << endl;
//	cout << "-- Num. Basic Blocks Eliminated: " << numDeletedArray[4] << " --" << endl;
	cout << "---------------------------------------" << endl;
}

// Use this function, if you need to selectively DCEize sections of a routine.
//
// if delEmptyInstrs == 1, then remove all empty instructions, else keep them.
// default: remove them
//
void deadCodeElimination(BasicBlockNode *bb, int delEmptyInstrs)
{
	deletedOperList.clear();

   // Iterate over marking all operations that need to be deleted.
   //
   // Note: marking some operations to be deleted might also invalidate other operations.
   //
   // So, do this in a loop until there are no more ops.
   // This loop is guaranteed to terminate.

   numMarkedInThisStage = 1;
   while (numMarkedInThisStage > 0)
   {
	   numMarkedInThisStage = 0;

	   // First mark the operations to be deleted.
	   //
	   markDeadOperations(bb);

	   // Maintain the DU properties of all affected nodes.
	   //
	   maintainDUAfterDCE(bb);
	   deletedOperList.clear();
   }

   // Then perform actual deletion of the operations/instructions/basic-blocks.
   //
   deleteUselessIR(*bb, currRoutine, delEmptyInstrs);
}

void
Routine::deadCodeElimination() {

   deletedOperList.clear();

   // Iterate over marking all operations that need to be deleted.
   //
   // Note: marking some operations to be deleted might also invalidate other operations.
   //
   // So, do this in a loop until there are no more ops.
   // This loop is guaranteed to terminate.

   numMarkedInThisStage = 1;
   while (numMarkedInThisStage > 0)
   {
	   numMarkedInThisStage = 0;


	   // First mark the operations to be deleted.
	   //
		cout << "Marking useless operations ...." << endl;
	   markDeadOperations(this);

	   // Maintain the DU properties of all affected nodes.
	   //
	   cout << "Maintaining DU property ...." << endl;
	   maintainDUAfterDCE(this);
	   deletedOperList.clear();
   }

   // Then perform actual deletion of the operations/instructions/basic-blocks.
   //
   cout << "Performing dead code deletion ..." << endl;
   initDCEStatsObject();
   deleteUselessIR(this);
   displayDCEStats();
}

