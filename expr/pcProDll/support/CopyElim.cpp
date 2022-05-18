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
//===========================================================================
// File:          CopyElim.cc
// Purpose:       copy elimination 
// Author:        Xiaomei Ji
// 
//=========================================================================

#include "BaseHTGNode.h"
#include "LoopHTGNode.h"
#include "IfHTGNode.h"
#include "SimpleHTGNode.h"
#include "CallHTGNode.h"
//#include "ComplexHTGNode.h"
#include "BaseOperation.h"
#include "DataOp.h"
#include "ComputeOp.h"
#include "MemoryOp.h"
#include "JumpOp.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "DataOpSlot.h"
#include "FlowOpSlot.h"
#include "BaseArgument.h"
#include "RegisterArg.h"
#include "SetList.h"
#include "MyLinkedList.h"
#include "BasicBlockNode.h"
#include "BBIterator.h"
#include "Routine.h"
#include "DeleteProperty.h"
#include "RegOrder.h"




#define INVALID -1
#define REGNUM 32
#define OPERNUMINLOOP 100

struct oneRegMap {
	int value ; // the instruction id that defined this register
	int age;    // the age of this register
}; 

struct OpTableStruct {
	BaseOperation** Operations;
	int	num;
	int 	maxNum;
};

static oneRegMap** RegMapTemplate = 0;
static oneRegMap** RegMaps = 0;
static OpTableStruct OperationTable;


static NormalInstr* nextInstr;
static NormalInstr* theLastInstr;




extern OpCodeTable globalOpCodeTable;
extern void compLiveVarOfInstr(SetList<int>&, BaseInstruction*, BasicBlockNode* bb=0);


BaseInstruction*
getTailInstrOfLoop(LoopHTGNode* loop) {
   SimpleHTGNode* tail = (SimpleHTGNode*)loop->getLoopList().getTail();
   return tail->getBasicBlock()->getLastInstr();
}


void
initRegMap(LoopHTGNode* loop, int firstID, oneRegMap**& arg) {
   int lastID = getTailInstrOfLoop(loop)->idValue();

   arg = new oneRegMap* [lastID-firstID+1];  // for all instructions
   
   for ( int i=0; i<=lastID-firstID; i++ ) {
	// each instr, for all register
	arg[i] = new oneRegMap [REGNUM];	
        int j = 0;
	for ( ; j<REGNUM; j++ ) {
	   arg[i][j].value = INVALID;
	   arg[i][j].age = INVALID;
	}
   }
}


int
isIMov(BaseOperation* op) {
    char* name=globalOpCodeTable[((NormalOp*)op)->getOpCodeIndex()].opName();
    if ( !strcmp( name, "IASSIGN" ) )
	return 1;
    return 0;
}



void
copyRegMap(oneRegMap* targ, oneRegMap* source) {
    for ( int i=0; i<REGNUM; i++ ) {
	targ[i].value = source[i].value;
	targ[i].age = source[i].age;
    }  
}


BaseInstruction* 
getHeadInstrOfLoop(LoopHTGNode* loop) {
 return ((SimpleHTGNode*)loop->getLoopHead())->getBasicBlock()->getFirstInstr();
}



void
addInOperationTable(BaseOperation* op) {
   if ( OperationTable.num == OperationTable.maxNum ) {
	OperationTable.maxNum = 2*OperationTable.maxNum;
	// reallocate
	OperationTable.Operations = (BaseOperation**)realloc(
	OperationTable.Operations,OperationTable.maxNum*sizeof(BaseOperation*)); 
   } 
   OperationTable.Operations[OperationTable.num] = op;
   OperationTable.num++;
}



// return 0 indicates equal

int
compare(oneRegMap* source, oneRegMap* targ) {
   for ( int i=0; i<REGNUM; i++ ) {
	if (source[i].value != targ[i].value || source[i].age != targ[i].age)
		return 1;
   }
   return 0;
}

int
findOpValue(BaseOperation* op) {
	for ( int i=0; i<OperationTable.num; i++ ) {
	   if ( OperationTable.Operations[i] == op )
		return i;
	}
#ifdef DEBUG
	assert(0>1);
#endif
}


void
compRegMap(LoopHTGNode* loop) {
  MyLinkedList<BaseInstruction*> instrList;
  int change = 1;
  BaseInstruction* header= getHeadInstrOfLoop(loop);
  BaseInstruction* tailInstr = getTailInstrOfLoop(loop);
  int firstID = header->idValue();
  initRegMap(loop, firstID, RegMapTemplate);
  int first = 1;
  while ( change ) {
     	change = 0;
	// add loop header to list	
	instrList.add(header);
	while ( instrList.numItems() != 0 ) {
	   BaseInstruction* instr =  instrList.getHead();
	   instrList.removeFirst();
	   oneRegMap* oldMap = RegMapTemplate[instr->idValue()-firstID];
	   oneRegMap  newMap[REGNUM]; 
	   copyRegMap(newMap, oldMap);
	   // for each op in instr
	   ArrayIterator<DataOpSlot>* iter=((NormalInstr*)instr)->getDataOpSlotIter();
	   while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
		   iter->nextElement();
		   continue;
		}
		if ( isIMov(op) ) {
		   // (value.age) = lookup src1_reg_of(op) in old map
		   int old=((RegOrderArgProperty*)op->ptrToOperand(_SOURCE_1_)->getPtrToProperty(_REGORDER_))->getOrder(); 
		   int newIn=((RegOrderArgProperty*)op->ptrToOperand(_DEST_, _DEST_LIST_)->getPtrToProperty(_REGORDER_))->getOrder(); 
		   newMap[newIn].value = oldMap[old].value;
		   newMap[newIn].age = oldMap[old].age;
		} else {
		   int opIndex = INVALID;
		   if ( first ) { // fill OperationTable
		      addInOperationTable(op);
		      opIndex = OperationTable.num-1;
		   } else {
		      opIndex = findOpValue(op);
		   }
		   if ( op->isComputeOp() || op->isMemoryOp() && ((MemoryOp*)op)->isLoad() ) { // def
		      // first, all others have the same op value, age increse
		      for ( int i=0; i<REGNUM; i++ )  {
			if ( newMap[i].value == opIndex ) {
			   newMap[i].age++;
			}
		      }
		      int index=((RegOrderArgProperty*)op->ptrToOperand(_DEST_, _DEST_LIST_)->getPtrToProperty(_REGORDER_))->getOrder();
		      //newMap[index].value = op->idValue();
		      newMap[index].value = opIndex;
		      newMap[index].age = 0;
		   }
		}
		iter->nextElement();
	   }
	   delete iter;
	   
	   // successors
	   // ???????????????????????
	   // what is the case of two instrs have the same succ
	   
	   int lastID = tailInstr->idValue();

	   ArrayIterator<FlowOpSlot>* flowIter = ((NormalInstr*)instr)->getFlowOpSlotIter();
	   while ( flowIter->hasMoreElements() ) {
		FlowOp* flowOp = (FlowOp*)flowIter->currentElement().getPtrToOperation();
		if ( flowOp == NULL ) {
			flowIter->nextElement();
			continue;	
		}
		BaseInstruction* succ = flowOp->getTargetInstr();
		// check this succ belongs to this loop or not
		if ( succ->idValue() > lastID || succ->idValue() < firstID ) {
			flowIter->nextElement();
			continue;
		}
		if (compare(newMap, RegMapTemplate[succ->idValue()-firstID])) {
		   change = 1;
		   copyRegMap(RegMapTemplate[succ->idValue()-firstID],newMap);
		}
		if ( succ != header )
		   instrList.add(succ);
		flowIter->nextElement();
	   }
	   delete flowIter;
	}
	first = 0;
  } 
}



void
updateArg(int index,BaseOperation* op,oneRegMap* current,oneRegMap* tempReg) 
{
	if (index == _DEST_)
		BaseArgument* arg =  op->ptrToOperand(index, _DEST_LIST_);
	else
		BaseArgument* arg =  op->ptrToOperand(index, _SRC_LIST_);
	
	if ( arg && arg->isRegisterArg() ) 
	{
		// lookup in template to determine the value and age of the operand
		int index = ((RegOrderArgProperty*)arg->getPtrToProperty(_REGORDER_))->getOrder();
		int value = tempReg[index].value;
		int age = tempReg[index].age;
		int i = 0;
		for (; i<REGNUM; i++ )  
		{
			if ( current[i].value == value && current[i].age == age ) 
			{
				// find out the Ri as a replacement candidate
				((RegOrderArgProperty*)((RegisterArg*)arg)->getPtrToProperty(_REGORDER_))->setOrder(i);
				break;
			}
		}
#ifdef DEBUG
		
		//assert(i<REGNUM);
#endif
	}
}



void
updateSource(DataOp* op, oneRegMap* current, oneRegMap* tempReg) {
    updateArg(_SOURCE_1_, op, current, tempReg);
    updateArg(_SOURCE_2_, op, current, tempReg);
}




int 
containsAndLive(oneRegMap* map, RegisterArg* source, BaseInstruction* instr){
	int index = ((RegOrderArgProperty*)source->getPtrToProperty(_REGORDER_))->getOrder();
	int value = map[index].value;
	if ( value==INVALID || map[index].age==INVALID ) 
		return _NO_;
	// find if the value is live at the entry of this instruction
	// OperationTable
	BaseArgument*arg=OperationTable.Operations[value]->ptrToOperand(_DEST_, _DEST_LIST_);
	int symbolVal=globalSymbolTable->getID(arg,arg->getPtrToProperty(_SSA_));
	SetList<int> liveSet;
	compLiveVarOfInstr(liveSet, instr);
        if ( liveSet.contains(symbolVal) == _NO_ )
		return _NO_;
	return _YES_;
}


void
updateDest(BaseOperation* op, oneRegMap* map, int instrNum) {
     int opValue = findOpValue(op);
     for ( int i=0; i<REGNUM; i++ ) {
	// first, find the previous allocated register 
	//if ( map[i].value == op->idValue() ) {
        if ( map[i].value == opValue ) {
	   // find the registers that has the same value as opValue
	   // in  the template,  
	   // if all the registers' age are not equal to the current age
	   // then, current reg is a free one 
	   int age = map[i].age;
           int j = 0;
	   for ( ; j<instrNum; j++ ) {
		oneRegMap* tempReg = RegMapTemplate[j];
		int k = 0;
		for ( ; k<REGNUM; k++ ) {
			if ( tempReg[k].value = opValue ) {
				if ( tempReg[k].age == age ) 
					break;
			}
		}
		if ( k < REGNUM ) 
			break;
	   }  
	   if ( j<instrNum )  { 
		// find , the Ri is a free one
		((RegOrderArgProperty*)((RegisterArg*)op->ptrToOperand(_DEST_, _DEST_LIST_))->getPtrToProperty(_REGORDER_))->setOrder(i);
		return;
	   }	
	}
     }

     // find a new one
     for  ( int i=0; i<REGNUM; i++ ) {
	if ( map[i].value == INVALID && map[i].age == INVALID ) {
		// find , the Ri is a free one
		((RegOrderArgProperty*)((RegisterArg*)op->ptrToOperand(_DEST_, _DEST_LIST_))->getPtrToProperty(_REGORDER_))->setOrder(i);
		return;
	}
     }    
#ifdef DEBUG
     cout << " no free register " << endl;
     assert(0>1);
#endif
}


void
initRegMapOfLoop( LoopHTGNode* loop, int firstID, oneRegMap** arg ) {
  MyLinkedList<BaseInstruction*> instrList;
  BaseInstruction* header= getHeadInstrOfLoop(loop);
  BaseInstruction* tailInstr = getTailInstrOfLoop(loop);
	// add loop header to list	
	instrList.add(header);
	while ( instrList.numItems() != 0 ) {
	   BaseInstruction* instr =  instrList.getHead();
	   instrList.removeFirst();
	   // for each op in instr
	   ArrayIterator<DataOpSlot>* iter=((NormalInstr*)instr)->getDataOpSlotIter();
	   while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
		   iter->nextElement();
		   continue;
		}
		   int   opIndex = findOpValue(op);
		   if ( op->isComputeOp() || op->isMemoryOp() && ((MemoryOp*)op)->isLoad() ) { // def
		      int index=((RegOrderArgProperty*)op->ptrToOperand(_DEST_, _DEST_LIST_)->getPtrToProperty(_REGORDER_))->getOrder();
		      arg[instr->idValue()-firstID][index].value = opIndex;
		      arg[instr->idValue()-firstID][index].age = 0;
		   }
		iter->nextElement();
	   }
	   delete iter;
	   
	   // successors
	   // ???????????????????????
	   // what is the case of two instrs have the same succ
	   
	   int lastID = tailInstr->idValue();

	   ArrayIterator<FlowOpSlot>* flowIter = ((NormalInstr*)instr)->getFlowOpSlotIter();
	   while ( flowIter->hasMoreElements() ) {
		FlowOp* flowOp = (FlowOp*)flowIter->currentElement().getPtrToOperation();
		if ( flowOp == NULL ) {
			flowIter->nextElement();
			continue;	
		}
		BaseInstruction* succ = flowOp->getTargetInstr();
		// check this succ belongs to this loop or not
		if ( succ->idValue() > lastID || succ->idValue() < firstID ) {
			flowIter->nextElement();
			continue;
		}
		if ( succ != header )
		   instrList.add(succ);
		flowIter->nextElement();
	   }
	   delete flowIter;
	}
}


void
scanAndReallocate(LoopHTGNode* loop) {
  	MyLinkedList<BaseInstruction*> instrList;
  	BaseInstruction* header= getHeadInstrOfLoop(loop);
	BaseInstruction* tailInstr = getTailInstrOfLoop(loop);
  	int firstID = header->idValue();
  	initRegMap(loop, firstID, RegMaps);

	//initRegMapOfLoop(loop, firstID, RegMaps);

	// add loop header to list	
	instrList.add(header);
	while ( instrList.numItems() != 0 ) {
	   BaseInstruction* instr =  instrList.getHead();
	   instrList.removeFirst();
	   oneRegMap* oldMap = RegMaps[instr->idValue()-firstID];
	   oneRegMap  newMap[REGNUM]; 
	   copyRegMap(newMap, oldMap);
	   // for each op in instr
	   ArrayIterator<DataOpSlot>* iter=((NormalInstr*)instr)->getDataOpSlotIter();
	   while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
		   iter->nextElement();
		   continue;
		}
		if ( isIMov(op) ) {
			DeleteProperty tmp;
			op->addProperty(tmp);
		} else {
		 	updateSource((DataOp*)op, oldMap, RegMapTemplate[instr->idValue() - firstID]);
		}
		iter->nextElement();
	   }
	 
	   iter->reset();

	   int lastID = tailInstr->idValue();

	   // then dest
	   while ( iter->hasMoreElements() ) {
		BaseOperation* op = iter->currentElement().getPtrToOperation();
		if ( op == NULL ) {
		   iter->nextElement();
		   continue;
		}
		if ( !isIMov(op) && op->isComputeOp() || 
		     op->isMemoryOp() && ((MemoryOp*)op)->isLoad() ) {

			int opValue = findOpValue(op);

			BaseArgument* dest = op->ptrToOperand(_DEST_, _DEST_LIST_);
			if (containsAndLive(oldMap,(RegisterArg*)dest,instr)){
				// inc others that have the same value
	
		   		for ( int i=0; i<REGNUM; i++ )  {
				   //if ( newMap[i].value == op->idValue() ) 
				   if ( newMap[i].value == opValue ) {
			   		newMap[i].age++;
				   }
		   		}
				
				// get a free one
				updateDest(op, newMap, lastID-firstID+1); 
			}
		   	int index=((RegOrderArgProperty*)op->ptrToOperand(_DEST_, _DEST_LIST_)->getPtrToProperty(_REGORDER_))->getOrder();
		   	//newMap[index].value = op->idValue();
		   	newMap[index].value = opValue;
		   	newMap[index].age = 0;
		}
		iter->nextElement();
	   }

	   delete iter;

	   // successors
	   // ???????????????????????
	   // what is the case of two instrs have the same succ

	   ArrayIterator<FlowOpSlot>* flowIter = ((NormalInstr*)instr)->getFlowOpSlotIter();
	   while ( flowIter->hasMoreElements() ) {
		FlowOp* flowOp = (FlowOp*)flowIter->currentElement().getPtrToOperation();
		if ( flowOp == NULL ) {
			flowIter->nextElement();
			continue;	
		}
		BaseInstruction* succ = flowOp->getTargetInstr();
		// check this succ belongs to this loop or not
		if ( succ->idValue() > lastID || succ->idValue() < firstID ) {
			flowIter->nextElement();
			continue;
		}
		copyRegMap(RegMaps[succ->idValue()-firstID], newMap);
		if ( succ != header )
		   instrList.add(succ);
		flowIter->nextElement();
	   }
	   delete flowIter;

	}
}



void
directTailToHead(NormalInstr* tail, BaseInstruction* head) {
    // find tail's flowOp
  
    for ( int i=0; i<numFlowOpsPerInstr; i++ ) {
	JumpOp* jumpOp=(JumpOp*)tail->getFlowOpSlot(i).getPtrToOperation();
	if ( jumpOp )  {
           jumpOp->setTargetInstr(head);
	   break;
	}
    }
}

oneRegMap*
getRegMapOfInstr(BaseInstruction* instr, int firstID) {
	return RegMaps[instr->idValue()-firstID];
}



// currently, assume the loop has  only one backedge

int
match(MyLinkedList<BaseInstruction*>& headerList, MyLinkedList<BaseInstruction*>& backEdges) {
   BaseInstruction* tail = backEdges.getTail();
   int firstID = headerList.getHead()->idValue();
   oneRegMap*  regMapOfTail = getRegMapOfInstr(tail, firstID);
   MyLinkedListIterator<BaseInstruction*>* iter = headerList.elementsForward();
   while ( iter->hasMoreElements() ) {
	BaseInstruction* header = iter->currentElement();
	oneRegMap* regMapOfHead = getRegMapOfInstr(header, firstID);
        if ( !compare(regMapOfTail, regMapOfHead) ) {
		directTailToHead((NormalInstr*)tail, header);
		//???????????????
	        // modHTGNode();
		delete iter;
		return 1;
	}
        iter->nextElement();
   }
   delete iter;
   return 0;
}



NormalInstr* 
copyNormalInstr(NormalInstr* instr) {
    NormalInstr* newInstr = new NormalInstr();
 
    if ( instr->getPtrToLabelArg() )
       newInstr->addLabel(instr->getPtrToLabelArg());
    ArrayIterator<DataOpSlot>* iter = instr->getDataOpSlotIter();
    while ( iter->hasMoreElements() ) {
	newInstr->addDataOpToList((DataOp*)iter->currentElement().getPtrToOperation());
	iter->nextElement();
    }
    delete iter;

/***
    ArrayIterator<FlowOpSlot>* fIter = instr->getFlowOpSlotIter();
    while ( fIter->hasMoreElements() ) {
	newInstr->addFlowOpToList(fIter->currentElement().getPtrToOperation());
	fIter->nextElement();
    }
    delete fIter;
***/

    ArrayIterator<ControlOpSlot>* cIter = instr->getControlOpSlotIter();
    while ( cIter->hasMoreElements() ) {
	newInstr->addControlOpToList((ControlOp*)cIter->currentElement().getPtrToOperation());
	cIter->nextElement();
    }
    delete cIter;

    newInstr->setHeaderOp(instr->getRootOpType(), instr->getRootOpIndex());


//?????????????????????????????????????????????????
// listOfSuccessorID, uniqueID, flowOp

    return newInstr;

}


BasicBlockNode*
copyBasicBlock(BasicBlockNode* bb) {
	BasicBlockNode* newBB = new BasicBlockNode();
	NormalInstr* newInstr = copyNormalInstr((NormalInstr*)bb->getLastInstr());
	newBB->initTrailer(newInstr);
	// traverse backwards for the convenient next field
	BBBackIterator iter(bb);
	iter.prevElement(); // across the tail
	while ( iter.hasMoreElements() ) {
		NormalInstr* curr = copyNormalInstr((NormalInstr*)iter.currentElement());
		JumpOp* flowOp = new JumpOp();
		flowOp->setTargetInstr(newInstr);
		curr->addFlowOpToList(flowOp);
		iter.prevElement();
		newInstr = curr;
	}
	newBB->initLeader(newInstr);
}


BaseHTGNode*
copyHTGNode(BaseHTGNode* node) {

   // NOTE: the next field of BaseHTGNode
   // NOTE: the nextInstr is the first instr of current HTG Node,
   // 		that is the target instr of previous Instruction,
   //		the previous instruction handled later, because of backwards 

   if ( node->isCallNode() || node->isComplexNode() ) {
	return 0;
   } else if ( node->isSimpleNode() ) {
	BasicBlockNode* bb = copyBasicBlock(((SimpleHTGNode*)node)->getBasicBlock());

	directTailToHead((NormalInstr*)bb->getLastInstr(), nextInstr);
	nextInstr = (NormalInstr*)bb->getFirstInstr();
	theLastInstr = (NormalInstr*)bb->getLastInstr();

	return new SimpleHTGNode(bb);
   } else if ( node->isIfNode() ) {
	IfHTGNode* newNode = new IfHTGNode();
	newNode->setIfHead(copyHTGNode(((IfHTGNode*)node)->getIfHead()));
	newNode->setIfJoin(copyHTGNode(((IfHTGNode*)node)->getIfJoin()));

	// backwards for next field in BaseHTGNode
        MyLinkedListIterator<BaseHTGNode*> * iter=((IfHTGNode*)node)->createTrueBranchIterBack();
	BaseHTGNode* next = 0;

	BaseInstruction* realJoinInstr = nextInstr;

	while ( iter->hasMoreElements() ) {
		BaseHTGNode* curr = copyHTGNode(iter->currentElement());
		newNode->appendTrueNode(curr);
		curr->setNext(next);

		if ( next == 0 ) { // the last in true branch
		    JumpOp* jumpOp = new JumpOp();
		    jumpOp->setTargetInstr(realJoinInstr);
		    theLastInstr->addFlowOpToList(jumpOp);
		}

		iter->nextElement();
		next = curr;
	}

	// set the first instr's target in true branch as join
	BasicBlockNode* ifBlock = ((SimpleHTGNode*)newNode->getIfHead())->getBasicBlock();
	NormalInstr* ifInstr = (NormalInstr*)ifBlock->getLastInstr();
	JumpOp* jumpOp = new JumpOp();
	jumpOp->setTargetInstr(nextInstr);
        ifInstr->addFlowOpToList(jumpOp);  
	
	delete iter;

	// reset nextInstr
	nextInstr =  (NormalInstr*)realJoinInstr;

	next = 0;
	// backwards for next field in BaseHTGNode
        iter=((IfHTGNode*)node)->createFalseBranchIterBack();
	while ( iter->hasMoreElements() ) {
		BaseHTGNode* curr = copyHTGNode(iter->currentElement());
		newNode->appendFalseNode(curr);
		curr->setNext(next);

		if ( next == 0 ) { // the last in false branch
		    JumpOp* jumpOp = new JumpOp();
		    jumpOp->setTargetInstr(realJoinInstr);
		    theLastInstr->addFlowOpToList(jumpOp);
		}

		iter->nextElement();
		iter->nextElement();
		next = curr;
	}
	delete iter;
   	
	// set if instr to its false branch
	jumpOp = new JumpOp();
	jumpOp->setTargetInstr(nextInstr);
        ifInstr->addFlowOpToList(jumpOp);  
	
	nextInstr = (NormalInstr*)ifBlock->getFirstInstr();

	return newNode;
   } else {  // LoopNode
	// create a dummy head
	LoopHTGNode*	newNode = new LoopHTGNode();

	//???????????????????? make dummy as pointer,
	// or add operator= in BaseHTGNode
	//newNode->addDummyHead(new SimpleHTGNode(0));

	newNode->setPtrToLoopDS(0); // invalid 

	// traverse backwards
	MyLinkedListIterator<BaseHTGNode*>* iter = ((LoopHTGNode*)node)->
							createLoopIterBack();
	BaseHTGNode* next = 0;
	BaseInstruction* tailOfLoop = 0;
	while ( iter->hasMoreElements() ) {
		BaseHTGNode* curr = copyHTGNode(iter->currentElement());
		curr->setNext(next);
		newNode->appendNode(curr);
		if ( next == 0 ) { // the last one
			tailOfLoop = theLastInstr;	
		}
		iter->nextElement();
		next = curr;
	}
	// curr, ( also next ) is the first one, loop's real header
	directTailToHead((NormalInstr*)tailOfLoop, nextInstr);

	theLastInstr = (NormalInstr*)tailOfLoop;

	delete iter;

	// the last new htg node is loop head
	newNode->setNewLoopHead(next);  // next==curr or 0
	
	return newNode;
   }
}


LoopHTGNode*
unroll(LoopHTGNode* loop) {
	nextInstr = 0;
	theLastInstr = 0;
	LoopHTGNode* newLoop = (LoopHTGNode*)copyHTGNode(loop);

}


void
copyElimInLoop(LoopHTGNode* loop) {
   // init OperationTable
   OperationTable.num = 0;
   OperationTable.maxNum = OPERNUMINLOOP;
   OperationTable.Operations = new BaseOperation* [OperationTable.maxNum];

   // first, compute the initial register maps of loop
   compRegMap(loop);

   // then, slpit nodes having multiple defs, recalc register maps ???????????

   // add header into headers_list
   MyLinkedList<BaseInstruction*> headerList;
   BaseInstruction* header = getHeadInstrOfLoop(loop);
   headerList.append(header);

   // add backedges into backedges_list
   MyLinkedList<BaseInstruction*> backEdges;
   BaseInstruction* tail = getTailInstrOfLoop(loop);

   backEdges.append(tail);

   // register allocation
   scanAndReallocate(loop);
   
   while ( backEdges.isEmpty() == _NO_ ) {
	// unroll along backedge 
        //?????????????????  
	// reset the operationTable in unroll
        LoopHTGNode* newLoop = unroll(loop);

	// add new iteration headers and backedges to respective lists
	header = getHeadInstrOfLoop(newLoop);
 	headerList.append(header);

        // link the previous tail to this unrolled loop's header
	directTailToHead((NormalInstr*)backEdges.getTail(), header);

        tail = getTailInstrOfLoop(newLoop);
	backEdges.append(tail);

	scanAndReallocate(newLoop);
	
	if ( match(headerList, backEdges) ) {
		break;	// just for one backEdge;
	}
   }
}

int
hasMoveInInstr(BaseInstruction* instr) {

      // data operation
      ArrayIterator<DataOpSlot>*iter=((NormalInstr*)instr)->getDataOpSlotIter();
      while ( iter->hasMoreElements() ) {
        BaseOperation* op = iter->currentElement().getPtrToOperation();
	if ( op == NULL ) {
		iter->nextElement();
		continue;
	}
        if ( isIMov(op) )  {
		delete iter;
		return _YES_;
	}
        iter->nextElement();
      }
      delete iter;
      return _NO_;
}


int
hasMoveInBB(BasicBlockNode* bb) {
	BBForwIterator bbIter(bb);
	while ( bbIter.hasMoreElements() ) {
	   if ( hasMoveInInstr(bbIter.currentElement()) ) 
		return _YES_;
	   bbIter.nextElement();
	}
}



int
hasMoveInLoop(LoopHTGNode* node) {
   MyLinkedListIterator<BaseHTGNode*>* iter = node->createLoopIterForw();
   while ( iter->hasMoreElements() ) {
	BaseHTGNode* inner = iter->currentElement();
	if (inner->isComplexNode() || inner->isCallNode() || inner->isIfNode()) 
		;
	else if ( inner->isSimpleNode() ) {
		if ( hasMoveInBB(((SimpleHTGNode*)inner)->getBasicBlock()) ) {
			delete iter;
			return _YES_;
		}
	} else { // LoopNode again
		if ( hasMoveInLoop((LoopHTGNode*)inner) )
			copyElimInLoop((LoopHTGNode*)inner);	
	}
	iter->nextElement();
   }
   delete iter;
}



void traverseNode(BaseHTGNode*);


void
travBranch(MyLinkedListIterator<BaseHTGNode*>* iter) {
   while ( iter->hasMoreElements() ) {
	traverseNode(iter->currentElement());
	iter->nextElement();
   }
   delete iter;
}



void
traverseNode(BaseHTGNode* node) {
      if ( node->isSimpleNode() || node->isComplexNode() || node->isCallNode() )
	;
      else if ( node->isLoopNode() ) {
	if ( hasMoveInLoop((LoopHTGNode*)node) ) {
		copyElimInLoop((LoopHTGNode*)node);
	}
      } else { // ifNode
	travBranch(((IfHTGNode*)node)->createTrueBranchIterForw());
	travBranch(((IfHTGNode*)node)->createFalseBranchIterForw());
      }
}

/*

void
Routine::copyElim() {

#ifdef DEBUG
   printf(" begin to copy elimination \n");
#endif

   BaseHTGNode* node = this->getHTG()->getHead();
   do {
      traverseNode(node);
      node = node->getNext();
   } while ( node );

#ifdef DEBUG
   printf(" end to copy elimination \n");
#endif

}
*/