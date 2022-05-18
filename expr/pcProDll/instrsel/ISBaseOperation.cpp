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

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "BaseOperation.h"
#include "BaseArgument.h"
//#include "Routine.h"
#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "UDDU.h"
#include "InstrSel.h"
#include "InstrSelIburg.h"
#include "../DebugSwitches.h"
#include "NormalOp.h"
#include "ComputeOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int debugISel = 0;
extern BaseArgument *GenerateArgument(NODEPTR_TYPE p);
extern void CopyProperties(BaseOperation *op1,BaseOperation *op2);
extern NormalInstr *globalLastInstrInserted;
extern map<BaseOperation *,NormalInstr*> _OpToInstrMap;
extern int InsertOp(NormalOp *op,NormalInstr *instr);
extern int globalRecomputeUDDU;
extern NormalInstr *buildMIPSInstr(BaseOperation *op = NULL);
extern void insertInstructionIntoIR(BaseInstruction *newInstr, BaseInstruction *prevInstr, BasicBlockNode *thisBB,  int nextInstrDir);

int BaseOperation::InstrSel(BaseOperation *&new_op){
	NODEPTR_TYPE p,q;
	MyString s;
	int replaceTree=0;
	NormalInstr *instr=this->getInstr();
	this->getOpcodeName(s);
	if(strcmp(s.getStr(),"NOP")==0){
		//for NOPs do not do ISEL
		return 0;
	}
   	if(debugISel) cout << endl;
	ClearIburgToOpMap();
	ClearIburgToArgMap();
	ClearEssentialOps();
	ClearOmittedOps();
	_OldOps.clear();
	p=BuildIburgTree(1,0);
	do{
		if(debugISel) cout << endl;
		if(debugISel) printIselTree(p,0);
		if(debugISel) cout << "\n\tCovers:\n" <<flush;
		setPerformedMut(0);
		isel_label(p);
		if(debugISel) dumpIselCover(p, 1, 0);
		q=emitIselTree(p,1);
		if(PerformedMut()) replaceTree=1;
		if(debugISel) cout << endl;
		if(debugISel) printIselTree(q,0);
		if(debugISel) cout << endl;
		// delete p;
		// purify fix
		free(p);
		p=q;
	}while(PerformedMut());
	if(replaceTree){
		if(this->hasOperand(_DEST_, _DEST_LIST_)) new_op=IburgToIR(p,this->ptrToOperand(_DEST_, _DEST_LIST_),this,instr);
		else new_op=IburgToIR(p,NULL,this,instr);
		if(new_op!=this){
			//update UDDU chains
			//_ReinsertedOps= ops reinserted from original
			//_NewOps= newly created ops
			//_OldOps= original ops
			//_OmittedOps= ops from the original tree left out in the new tree (may be reinserted if in ReinsertedOps) 
			UpdateUDDU(&_OldOps,&_NewOps,&_ReinsertedOps);
		}
	}
	else{ 
		new_op=NULL;
		if(debugISel) cout << endl << "Not replaced (no mutation performed)/////////////////////////" << endl << endl;
	}
	if(debugISel) cout << endl;
	return replaceTree && new_op!=this;//return 1 if there was any change made (any mutation)
}

int GenerateOperation(NODEPTR_TYPE p,NormalOp *&new_op)
{ //ComputeOp
	BaseOperation *old_op;
	NormalOp* dummy_oper; //ComputeOp
	NormalInstr *temp;
	char *opname;
	int opcodeind;
	int r;
	if(p==NULL) return DIDNTFIT;

	if(LEFT_CHILD(p)!=0){
		r=GenerateOperation(LEFT_CHILD(p),dummy_oper);
		if(r==DIDNTFIT) return r;
	}
	if(RIGHT_CHILD(p)!=0){
		r=GenerateOperation(RIGHT_CHILD(p),dummy_oper);
		if(r==DIDNTFIT) return r;
	}
	char *name;
	name=Opname(OP_LABEL(p));
	if(strcmp(name,"DEF")==0){
		if((old_op=GetIburgToOpMap(p))!=NULL){
			opname=Opname(OP_LABEL(RIGHT_CHILD(p)));
			opcodeind=globalOpCodeTable.getIndex(opname);
			if(globalOpCodeTable.isType(opcodeind,COMPUTE_OP)){
				new_op=new ComputeOp();
				new_op->initOpCode(*(new OpCode(opcodeind)));
				new_op->addOperand(GenerateArgument(LEFT_CHILD(RIGHT_CHILD(p))), _SOURCE_1_);
				new_op->addOperand(GenerateArgument(RIGHT_CHILD(RIGHT_CHILD(p))), _SOURCE_2_);
				new_op->addOperand(GenerateArgument(LEFT_CHILD(p)),_DEST_, _DEST_LIST_);
			}
			else if(globalOpCodeTable.isType(opcodeind,MEMORY_OP)){
				new_op=new MemoryOp();
				new_op->initOpCode(*(new OpCode(opcodeind)));
				((MemoryOp*)new_op)->addSourceOperand(GenerateArgument(LEFT_CHILD(RIGHT_CHILD(p))));
				((MemoryOp*)new_op)->setOffset(GenerateArgument(RIGHT_CHILD(RIGHT_CHILD(p))));
				((MemoryOp*)new_op)->addDestOperand(GenerateArgument(LEFT_CHILD(p)));
			}
			else if(globalOpCodeTable.isType(opcodeind,IF_OP)){
				new_op=new IfOp();
				new_op->initOpCode(*(new OpCode(opcodeind)));
				new_op->addOperand(GenerateArgument(LEFT_CHILD(RIGHT_CHILD(p))), _COND_REG_);
				new_op->addOperand(GenerateArgument(RIGHT_CHILD(RIGHT_CHILD(p))), _SRC_LABEL_);
				//new_op->addOperand(GenerateArgument(LEFT_CHILD(p)),_DEST_, _DEST_LIST_);
			}
			else 
				_ASSERT(0);
			new_op->setID(old_op->idValue());
			_NewOps.insert(new_op);
			CopyProperties(old_op,new_op);
			if (debugMut)
			{
				cout << "Replaced: " << endl;
				old_op->print();
				cout << "With: " << endl;
				new_op->print();
			}
			new_op->updateUDDU(old_op,NULL);
			deleteUDDU(old_op);

			if (debugMut)
			{
				cout << "Replaced: " << endl;
				old_op->print();
				cout << "With: " << endl;
				new_op->print();
			}

			if (debugMut)
				new_op->print();
			globalLastInstrInserted=_OpToInstrMap[old_op];
			return InsertOp(new_op,globalLastInstrInserted);
		}
		else{
			opname=Opname(OP_LABEL(RIGHT_CHILD(p)));
			opcodeind=globalOpCodeTable.getIndex(opname);
			if(globalOpCodeTable.isType(opcodeind,COMPUTE_OP)){
				new_op=new ComputeOp();
				new_op->initOpCode(*(new OpCode(opcodeind)));
				new_op->addOperand(GenerateArgument(LEFT_CHILD(RIGHT_CHILD(p))),_SOURCE_1_);
				new_op->addOperand(GenerateArgument(RIGHT_CHILD(RIGHT_CHILD(p))),_SOURCE_2_);
				new_op->addOperand(GenerateArgument(LEFT_CHILD(p)),_DEST_, _DEST_LIST_);
			}
			else if(globalOpCodeTable.isType(opcodeind,MEMORY_OP)){
				new_op=new MemoryOp();
				new_op->initOpCode(*(new OpCode(opcodeind)));
				((MemoryOp*)new_op)->addSourceOperand(GenerateArgument(LEFT_CHILD(RIGHT_CHILD(p))));
				((MemoryOp*)new_op)->setOffset(GenerateArgument(RIGHT_CHILD(RIGHT_CHILD(p))));
				((MemoryOp*)new_op)->addDestOperand(GenerateArgument(LEFT_CHILD(p)));
			}
			_NewOps.insert(new_op);
			//new_op->updateUDDU(old_op,NULL);
			globalRecomputeUDDU=1;
			insertInstructionIntoIR(temp=buildMIPSInstr(NULL),globalLastInstrInserted,
				globalLastInstrInserted->getBasicBlock(),_FALSE_BRANCH_);
			globalLastInstrInserted=temp;
			return InsertOp(new_op,globalLastInstrInserted);
		}
	}
	else{
		new_op=NULL;
	}
	return NOTINSTR;
}