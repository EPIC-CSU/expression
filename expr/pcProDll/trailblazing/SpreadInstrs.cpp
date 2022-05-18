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
// $Id: SpreadInstrs.cc,v 1.25 1998/04/14 18:29:07 ahalambi Exp $
//
// File:  		TbzRoutine.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		G. Grun
// Email:		pgrun@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#include "stdafx.h"

#include "Routine.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"
#include "LoopHTGNode.h"
#include "BasicBlockNode.h"
#include "BBIterator.h"
#include "UDDU.h"
#include "IburgToIR.h"
#include "Instruction.h"

#ifdef __UNIX__
	#define _ASSERT	assert
#endif

#ifndef __UNIX__
#include <process.h>
#else
#define __declspec(dllexport)
#define __stdcall
#define _ASSERT assert
#include <stdio.h>
#endif

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


//prototypes
void SpreadInstrsList(MyLinkedList<BaseHTGNode*> *list);
extern void insertInstructionIntoIR(BaseInstruction *newInstr, BaseInstruction *prevInstr, BasicBlockNode *thisBB,  int nextInstrDir);
extern NormalInstr *buildMIPSInstr(BaseOperation *op = NULL);
int ReadTime(BaseOperation *oper,int unit,SlotType optype,int pos);
int WriteTime(BaseOperation *oper,int unit,SlotType optype,int pos);
typedef map<int,vector<int>*> ResTabType;
void GetReservationTable(BaseOperation *oper,int unit,SlotType optype,ResTabType * restab);
__declspec(dllimport) int PIPELENGTH;
int load_dest_reg_field(void);
int store_src_reg_field(void);
int load_src_mem_field(void);
int store_dest_mem_field(void);
int reg_dest_field(BaseOperation *oper);
int NoPipelineConflicts(NormalInstr *instr,BasicBlockNode *bb);
void delete_express_rt(ResTabType * restab);
void FeedbackLatencies(BaseOperation *oper1,int unit1,SlotType optype1,
					  BaseOperation *oper2,int unit2,SlotType optype2,int*latencies);
int ExistsQueueBetween(BaseOperation *oper1,int unit1,SlotType optype1,
						   BaseOperation *oper2,int unit2,SlotType optype2,int latency);

//set this to one in order to insert explicit NOPs in the code in empty instructions
int insertNOPs=0;

//set this to one for write-through register files, or to 0 if 
//we need one cycle between write and read in the RFs
int write_through_register_file = 0;

//this is to inform find_unifiable_ops_dest to stop at the next instruction
extern int LastDependecy;

//set this to 1 when we are in the final spread instrs
int inFinalSpreadInstrs=0;


//computes the nr of cycles that the two operations (oper1, oper2) have to
//be spread apart due to their flow dependencies' data hazards in the pipeline
int NrFlowDepCycles(BaseOperation *oper1,int unit1,SlotType optype1,BaseOperation *oper2,int unit2,SlotType optype2){
	MyLinkedList<BaseOperation*> *useList;
	BaseArgument *arg1;
	int t1,t2,max=0,depends=0;
	int latencies[1000],j,min=32767;
	MyLinkedListIterator<BaseOperation*> *i;
	ArgList &a = oper1->destOperandList();
	ArgListIter ai;
	//for all the destinations of oper1
	for (ai = a.begin(); ai != a.end(); ai++){
		if((*ai)==NULL) continue;
		//arg1 is the current destination of oper2
		arg1=*ai;
		useList=GetUseList(arg1);
		//for all the uses of the destination of oper1
		if(useList==NULL) return 0;
		//for all the uses of the destination of oper1
		for(i=useList->elementsForward();i->hasMoreElements();i->nextElement()){
			//if this use is in oper2
			
			if(i->currentElement()==oper2){
				//
				// MULTIPLE_DESTS_AND_SOURCES
				//
				/*
				ArgList &a = oper2->sourceOperandList();
				ArgListIter ai;
				//for all the sources of oper2
				for (ai = a.begin(); ai != a.end(); ai++){
					if ((*ai) != NULL){
						if(equalRegs(arg1,*ai)){
							t1=WriteTime(oper1,unit1,optype1,reg_dest_field(oper1));
							t2=ReadTime(oper2,unit2,optype2,_SOURCE_1_);
							if(t1-t2>max) max=t1-t2;
						}
					}
				}
				*/
				//
				// end change for MULTIPLE_DESTS_AND_SOURCES
				
				//if there is a feedback path between oper1 and oper2
				//max should be the difference between feedback_write(oper1) - feedback_read(oper2)
				//we should use the shortest feedback path or RF path between the two operations for an argument
				//between arguments we should use the longest one, but if there is a shorter feedback, use that one

				//otherwise the latency is based on the actual read/write times of oper1 and oper2
				if(equalRegs(arg1,i->currentElement()->ptrToOperand(_SRC_1_))){
					t1=WriteTime(oper1,unit1,optype1,reg_dest_field(oper1));
					t2=ReadTime(oper2,unit2,optype2,_SOURCE_1_);
					if(t1-t2>max) max=t1-t2;
					depends=1;
				}
				if(equalRegs(arg1,i->currentElement()->ptrToOperand(_SRC_2_))){
					t1=WriteTime(oper1,unit1,optype1,reg_dest_field(oper1));
					t2=ReadTime(oper2,unit2,optype2,_SOURCE_2_);
					if(t1-t2>max) max=t1-t2;
					depends=1;
				}
				if(equalRegs(arg1,i->currentElement()->ptrToOperand(_SRC_3_))){
					t1=WriteTime(oper1,unit1,optype1,reg_dest_field(oper1));
					if(oper2->isMemoryOp())
						t2=ReadTime(oper2,unit2,optype2,_REG_SRC_);
					else
						t2=ReadTime(oper2,unit2,optype2,_SOURCE_3_);
					if(t1-t2>max) max=t1-t2;
					depends=1;
				}
			}
		}
	}

	//if there is no depency between oper1 and oper2, return 0
	if(!depends) return 0;
	//if there is a dependency
	//compute the feedback paths latencies
	FeedbackLatencies(oper1,unit1,optype1,oper2,unit2,optype2,latencies);
	//find the min distance
	for(j=0;latencies[j]!=-1;j++){
		if(min>latencies[j]){
			min=latencies[j];
		}
	}
	//find the minimum between the shortest common feedback path, and the longest RF latency
	if(min<max){
		max=min;
	}
	//return the shortest latency between oper1 and oper2
	return max;
}

//computes the nr of cycles that the two operations (oper1, oper2) have to
//be spread apart due to their output dependencies' data hazards in the pipeline
int NrOutputAntiDepCycles(BaseOperation *oper1,int unit1,SlotType optype1,BaseOperation *oper2,int unit2,SlotType optype2){
	BaseArgument *dest1,*dest2,*src1,*src2,*src3,*src4;
	int t1,t2,max=0,d;

	if(oper1->hasOperand(_DEST_, _DEST_LIST_))
		dest1=oper1->ptrToOperand(_DEST_, _DEST_LIST_);
	else dest1=NULL;
	if(oper2->hasOperand(_DEST_, _DEST_LIST_))
		dest2=oper2->ptrToOperand(_DEST_, _DEST_LIST_);
	else dest2=NULL;

	src1=oper1->ptrToOperand(_SOURCE_1_);
	src2=oper1->ptrToOperand(_SOURCE_2_);

	src3=oper2->ptrToOperand(_SOURCE_1_);
	src4=oper2->ptrToOperand(_SOURCE_2_);

	//output dep
	if(equalRegs(dest1,dest2)){
		t1=WriteTime(oper1,unit1,optype1,reg_dest_field(oper1));
		t2=WriteTime(oper2,unit2,optype2,reg_dest_field(oper2));
		d=t1-t2;
		if(d<1) d=1;
		if(d>max) max=d;
	}

	//anti dep
	if(equalRegs(dest2,src1)){
		t1=ReadTime(oper1,unit1,optype1,_SOURCE_1_);
		t2=WriteTime(oper2,unit2,optype2,reg_dest_field(oper2));
		d=t1-t2;
		if(d<1) d=1;
		if(d>max) max=d;
	}

	if(equalRegs(dest2,src2)){
		t1=ReadTime(oper1,unit1,optype1,_SOURCE_2_);
		t2=WriteTime(oper2,unit2,optype2,reg_dest_field(oper2));
		d=t1-t2;
		if(d<1) d=1;
		if(d>max) max=d;
	}

	return max;
}

//computes the nr of cycles that the two operations (oper1, oper2) have to
//be spread apart due to their memory dependencies' data hazards in the pipeline
int NrMemoryDepCycles(BaseOperation *oper1,int unit1,SlotType optype1,BaseOperation *oper2,int unit2,SlotType optype2){
	int d,dep,max=0;
	int t1,t2;
	MemoryOp *op1,*op2;
	if((op1=dynamic_cast<MemoryOp*>(oper1))==NULL) return 0;
	if((op2=dynamic_cast<MemoryOp*>(oper2))==NULL) return 0;

	if(op1->isStore() || op2->isStore()){
		d=disamb(op1,op2);
		switch(d){
		case EQUAL:
		case OVERLAPING:
		case DONTKNOW:
			dep=1;
			break;
		case DISJOINT:
			dep=0;
			break;
		}
		if(dep){
			//find the distance required between the two memory operations
			if(op1->isStore() && op2->isStore()){
				t1=WriteTime(op1,unit1,optype1,store_dest_mem_field());
				t2=WriteTime(op2,unit2,optype2,store_dest_mem_field());
				if(t1-t2>max) max=t1-t2;
			}
			else if(op1->isStore()){
				t1=WriteTime(op1,unit1,optype1,store_dest_mem_field());
				t2=ReadTime(op2,unit2,optype2,load_src_mem_field());
				if(t1-t2>max) max=t1-t2;
			}
			else if(op2->isStore()){
				t1=ReadTime(op1,unit1,optype1,load_src_mem_field());
				t2=WriteTime(op2,unit2,optype2,store_dest_mem_field());
				if(t1-t2>max) max=t1-t2;
			}
			return max;
		}
	}
	return 0;
}

//computes the nr of cycles that the two operations (oper1, oper2) have to
//be spread apart due to their dependencies' data hazards in the pipeline
int ComputeNrDepCycles(BaseOperation *oper1,int unit1,SlotType optype1,BaseOperation *oper2,int unit2,SlotType optype2){
	int curr,max=0,latency;
	if(ExistsQueueBetween(oper1,unit1,optype1,oper2,unit2,optype2,latency)){
		//set this to inform the find_unifiable_ops_dest to stop at the next instruction
		//LastDependecy=1;
		return latency;
	}
	curr=NrFlowDepCycles(oper1,unit1,optype1,oper2,unit2,optype2);
	if(curr>max) max=curr;
	curr=NrOutputAntiDepCycles(oper1,unit1,optype1,oper2,unit2,optype2);
	if(curr>max) max=curr;
	curr=NrMemoryDepCycles(oper1,unit1,optype1,oper2,unit2,optype2);
	if(curr>max) max=curr;
	//if we do not have write-through register files, need to increment max with 1, 
	//to account for the 1 cycle delay between write to RF and read from RF
	if(!write_through_register_file) max++;

	return max;
}


//computes the nr of cycles that the two instructions (instr and nextInstr) have to 
//be spread out due to their dependencies' data hazards in the pipeline
int ComputeNrDepCycles(NormalInstr *instr,NormalInstr *nextInstr){
	ArrayIterator<DataOpSlot> *iter1,*iter2;
	ArrayIterator<ControlOpSlot> *iter3,*iter4;
	DataOpSlot *slot1,*slot2;
	ControlOpSlot *slot3,*slot4;
	BaseOperation *oper1,*oper2,*oper3,*oper4;
	int curr,max=0,i,j;

	if(instr==NULL || nextInstr==NULL) return 0;
	
	i=0;
	//for all the operations in instr
	iter1=instr->getDataOpSlotIter();
	while(iter1->hasMoreElements()){
		slot1=&(iter1->currentElement());
		if(slot1->hasOper()){
			oper1=slot1->getPtrToOperation();
			j=0;
			//for all the data operations in nextInstr
			iter2=nextInstr->getDataOpSlotIter();
			while(iter2->hasMoreElements()){
				slot2=&(iter2->currentElement());
				if(slot2->hasOper()){
					oper2=slot2->getPtrToOperation();
					curr=ComputeNrDepCycles(oper1,i,DATA_OP_SLOT,oper2,j,DATA_OP_SLOT);
					if(curr>max) max=curr;
				}
				iter2->nextElement();j++;
			}
			delete iter2;
			j=0;
			//for all the control operations in nextInstr
			iter3=nextInstr->getControlOpSlotIter();
			while(iter3->hasMoreElements()){
				slot3=&(iter3->currentElement());
				if(slot3->hasOper()){
					oper3=slot3->getPtrToOperation();
					curr=ComputeNrDepCycles(oper1,i,DATA_OP_SLOT,oper3,j,CONTROL_OP_SLOT);
					if(curr>max) max=curr;
				}
				iter3->nextElement();j++;
			}
			delete iter3;
		}
		iter1->nextElement();i++;
	}
	delete iter1;

	i=0;
	//for all the control operations in instr
	iter4=instr->getControlOpSlotIter();
	while(iter4->hasMoreElements()){
		slot4=&(iter4->currentElement());
		if(slot4->hasOper()){
			oper4=slot4->getPtrToOperation();
			j=0;
			//for all the data operations in nextInstr
			iter2=nextInstr->getDataOpSlotIter();
			while(iter2->hasMoreElements()){
				slot2=&(iter2->currentElement());
				if(slot2->hasOper()){
					oper2=slot2->getPtrToOperation();
					curr=ComputeNrDepCycles(oper4,i,CONTROL_OP_SLOT,oper2,j,DATA_OP_SLOT);
					if(curr>max) max=curr;
				}
				iter2->nextElement();j++;
			}
			delete iter2;
			j=0;
			//for all the control operations in nextInstr
			iter3=nextInstr->getControlOpSlotIter();
			while(iter3->hasMoreElements()){
				slot3=&(iter3->currentElement());
				if(slot3->hasOper()){
					oper3=slot3->getPtrToOperation();
					curr=ComputeNrDepCycles(oper4,i,CONTROL_OP_SLOT,oper3,j,CONTROL_OP_SLOT);
					if(curr>max) max=curr;
				}
				iter3->nextElement();j++;
			}
			delete iter3;
		}
		iter4->nextElement();i++;
	}
	delete iter4;

	return max;
}

//computes the nr of cycles that the oper and instr have to 
//be spread out due to their dependencies' data hazards in the pipeline
int ComputeNrDepCycles(BaseOperation *oper,int unit,SlotType optype,NormalInstr *instr){
	ArrayIterator<DataOpSlot> *iter1;
	ArrayIterator<ControlOpSlot> *iter2;
	DataOpSlot *slot1;
	ControlOpSlot *slot2;
	BaseOperation *oper1,*oper2;
	int curr,max=0,i;

	if(instr==NULL || oper==NULL) return 0;
	
	i=0;
	//for all data operations in instr
	iter1=instr->getDataOpSlotIter();
	while(iter1->hasMoreElements()){
		slot1=&(iter1->currentElement());
		if(slot1->hasOper()){
			oper1=slot1->getPtrToOperation();
			curr=ComputeNrDepCycles(oper1,i,DATA_OP_SLOT,oper,unit,optype);
			if(curr>max) max=curr;
		}
		iter1->nextElement();i++;
	}
	delete iter1;

	i=0;
	//for all control operations in instr
	iter2=instr->getControlOpSlotIter();
	while(iter2->hasMoreElements()){
		slot2=&(iter2->currentElement());
		if(slot2->hasOper()){
			oper2=slot2->getPtrToOperation();
			curr=ComputeNrDepCycles(oper2,i,CONTROL_OP_SLOT,oper,unit,optype);
			if(curr>max) max=curr;
		}
		iter2->nextElement();i++;
	}
	delete iter2;

	return max;
}

extern int isNOP(int opCode);

//returns the size of the longest reservation table
//of an operation in the instr
int GetMaxReservationTableSize(NormalInstr *instr){
	ArrayIterator<DataOpSlot> *iter1;
	ArrayIterator<ControlOpSlot> *iter2;
	DataOpSlot *slot1;
	ControlOpSlot *slot2;
	BaseOperation *oper1,*oper2;
	int i,j,max=0;
	ResTabType restab;
	
	i=0;
	//for all the operations in instr
	iter1=instr->getDataOpSlotIter();
	while(iter1->hasMoreElements()){
		slot1=&(iter1->currentElement());
		if(slot1->hasOper()){
			oper1=slot1->getPtrToOperation();

			// Hack by SAH on 09/20/01 to handle NOP operations
			// that were inserted due as correction ops.
			//
			if (!isNOP((oper1->getOpCode()).index()))
			{
				GetReservationTable(oper1,i,DATA_OP_SLOT,&restab);
				for(j=0;j<PIPELENGTH;j++){
					if(j>max && !restab[j]->empty()){
						max=j;
					}
				}
			}
			//delete_express_rt(&restab);
		}
		iter1->nextElement();i++;
	}
	delete iter1;

	i=0;
	//for all the control operations in instr
	iter2=instr->getControlOpSlotIter();
	while(iter2->hasMoreElements()){
		slot2=&(iter2->currentElement());
		if(slot2->hasOper()){
			oper2=slot2->getPtrToOperation();
			if(!oper2->isRetOp()){
				GetReservationTable(oper2,i,CONTROL_OP_SLOT,&restab);
				for(j=0;j<PIPELENGTH;j++){
					if(j>max && !restab[j]->empty()){
						max=j;
					}
				}
				delete_express_rt(&restab);
			}
		}
		iter2->nextElement();i++;
	}
	delete iter2;
	
	return max;
}

//the set of opcodes that need empty instructions to account for static scheduling latencies 
set<int> static_delay_ops;


//read the operation which need explicit empty instructions (NOPs) in SpreadInstrs
void readStaticDelayOpers(void){
  char line[1000],word[1000];
  int n;
  FILE *f;

  f=fopen("StaticDelayOpers.txt","r");

  if(f==NULL){
    printf("Coulnd not open SlotMappings.txt\n");
    exit(1);
  }

  while(!feof(f)){
    fgets(line,1000,f);
    if(line[0]!='#'){
      n=sscanf(line,"%s\n",word);
      if(n==1){
		  static_delay_ops.insert(globalOpCodeTable.getIndex(word));
      }
    }
  }
  fclose(f);
}


int needsStaticDelaysOper(BaseOperation *oper){
	// char *opname;
	int opcode;
	set<int>::iterator i;
	//opname=globalOpCodeTable[dynamic_cast<NormalOp*>(oper)->getOpCodeIndex()].opName();
	opcode=dynamic_cast<NormalOp*>(oper)->getOpCodeIndex();
	for(i=static_delay_ops.begin();i!=static_delay_ops.end();i++){
		if(opcode==(*i)){
			return 1;
		}
	}
	return 0;
}

int needsStaticDelaysInstr(NormalInstr *instr){
	ArrayIterator<DataOpSlot> *iter1;
	ArrayIterator<ControlOpSlot> *iter2;
	DataOpSlot *slot1;
	ControlOpSlot *slot2;
	BaseOperation *oper1,*oper2;
	int i,max=0;
	// int j;
	i=0;
	//for all the operations in instr
	iter1=instr->getDataOpSlotIter();
	while(iter1->hasMoreElements()){
		slot1=&(iter1->currentElement());
		if(slot1->hasOper()){
			oper1=slot1->getPtrToOperation();
			if(needsStaticDelaysOper(oper1))
				return 1;
		}
		iter1->nextElement();i++;
	}
	delete iter1;

	i=0;
	//for all the control operations in instr
	iter2=instr->getControlOpSlotIter();
	while(iter2->hasMoreElements()){
		slot2=&(iter2->currentElement());
		if(slot2->hasOper()){
			oper2=slot2->getPtrToOperation();
			if(needsStaticDelaysOper(oper2))
				return 1;
		}
		iter2->nextElement();i++;
	}
	delete iter2;
	return 0;
}


//spread out instructions in simpleHtgNode (representing a basic block)
void SpreadInstrsSimple(SimpleHTGNode * node){
	NormalInstr *instr,*prev,*tmp;
	NormalOp *op;
	OpCode *opc;
	int nrInstr,i,j,max=0,saveJ;
	//this should be infered from EXPRESSION
	int lookahead=6;
	int currentInstrNeedsDelayed=0,nextInstrNeedsDelayed=0;
	BasicBlockNode *bb=node->getBasicBlock(),*nextBB;
	BBForwIterator iter1(bb),iter2;
	//BBForwIterator iter(bb);
	BBBackIterator iter(bb);

	cout << endl << "PIPELENGTH = " << PIPELENGTH << endl;

	while(iter.hasMoreElements())
	{
		if(iter.currentElement()->isNormalInstr())
		{
			instr=(NormalInstr*)(iter.currentElement());
			currentInstrNeedsDelayed=needsStaticDelaysInstr(instr);
			
			//first make iter1 point to instr
			for(iter1.reset();iter1.hasMoreElements() && 
				iter1.currentElement()!=instr;iter1.nextElement())
				;
		
			_ASSERT(iter1.hasMoreElements());
			
			//make iter1 point to the next after instr
			iter1.nextElement();
			//go to the previous element
			iter.prevElement();
			//iter1=iter;
			max=0;
			//first find the number of instructions we have to look ahead for data hazards
			lookahead=GetMaxReservationTableSize(instr);
			
			
			//first find the number of empty instructions we need to add after 'instr' to account for pipeline
			//hazards
			
			NormalInstr *instr2;
			//check for the following lookahead instructions in the same bb 
			//to see if they generate any spread need
			//j starts from 1 to account for the already present 1 cycle delay between instr and nextInstr
			//the ComputeNrDepCycles returns dependency_length or dependency_length + 1 depending on write-through RF or not
			for(j=1;j<lookahead && iter1.hasMoreElements();j++)
			{
				instr2=dynamic_cast<NormalInstr*>(iter1.currentElement());
				nextInstrNeedsDelayed=needsStaticDelaysInstr(instr2);

				if(currentInstrNeedsDelayed || nextInstrNeedsDelayed || !inFinalSpreadInstrs)
				{
					nrInstr=ComputeNrDepCycles(instr,instr2);
					iter1.nextElement();
					if(nrInstr-j>max)
						max=nrInstr-j;
				}
				else 
					iter1.nextElement();
			}
			
			if(j<lookahead)
			{
				//check in the following basic blocks for pipeline hazards with this 'instr'
				
				//save j, to restart it for each following basic block
				saveJ=j;
				
				//for the first following bb
				nextBB=bb->getFirstSucc();
				if(nextBB!=NULL)
				{
					//make iter2 point to the first instr in nextBB
					iter2.setBB(nextBB);
					for(;j<lookahead && iter2.hasMoreElements();j++)
					{
						instr2=dynamic_cast<NormalInstr*>(iter2.currentElement());
						nextInstrNeedsDelayed=needsStaticDelaysInstr(instr2);
						if(currentInstrNeedsDelayed || nextInstrNeedsDelayed || !inFinalSpreadInstrs)
						{
							nrInstr=ComputeNrDepCycles(instr,instr2);
							iter2.nextElement();
							if(nrInstr-j>max) max=nrInstr-j;
						}
						else 
							iter2.nextElement();
					}
				}
				//for the second following bb
				nextBB=bb->getNextSucc();
				//restore the j to restart for the next following bb
				j=saveJ;
				if(nextBB!=NULL)
				{
					//make iter2 point to the first instr in nextBB
					iter2.setBB(nextBB);
					for(;j<lookahead && iter2.hasMoreElements();j++)
					{
						instr2=dynamic_cast<NormalInstr*>(iter2.currentElement());
						nextInstrNeedsDelayed=needsStaticDelaysInstr(instr2);
						if(currentInstrNeedsDelayed || nextInstrNeedsDelayed  || !inFinalSpreadInstrs)
						{
							nrInstr=ComputeNrDepCycles(instr,instr2);
							iter2.nextElement();
							if(nrInstr-j>max) max=nrInstr-j;
						}
						else 
							iter2.nextElement();
					}
				}
			}
			
			//insert 'max' empty instructions after 'instr', and before the next instruction
			for(prev=instr,i=0;i<max;i++)
			{
				//op=buildMIPSOp(opcode, numArgs, hasLabel);
				if(insertNOPs && inFinalSpreadInstrs)
				{
					op = new ComputeOp();
					opc = new OpCode("X_NOP");
					op->initOpCode(*opc);
					//op->clearDestOperand();
				}
				else 
					op=NULL;

				insertInstructionIntoIR(tmp=buildMIPSInstr(op),prev,bb,_FALSE_BRANCH_);
				prev=tmp;
			}
			//insert empty instructions after instr, until no more pipeline resource conflicts
			for(i=0,prev=instr;!NoPipelineConflicts(instr,bb) && i<PIPELENGTH;i++)
			{
				//op=buildMIPSOp(opcode, numArgs, hasLabel);
				if(insertNOPs && inFinalSpreadInstrs)
				{
					op = new ComputeOp();
					opc = new OpCode("X_NOP");
					op->initOpCode(*opc);
				}
				else 
					op=NULL;

				insertInstructionIntoIR(tmp=buildMIPSInstr(op),prev,bb,_FALSE_BRANCH_);
				prev=tmp;
			}

			//if i>=PIPELENGTH, means there is a conflict in one reservation table for an instruction
			//due to a resource with a too small capacity
			_ASSERT(i<PIPELENGTH);
		}
		else 
			iter.prevElement();
	}	
}


//spread out instrutions in the if node
void SpreadInstrsIf(IfHTGNode * node){
	SpreadInstrsList(&node->getTrueBranch());
	SpreadInstrsList(&node->getFalseBranch());
}

//spread out instructions in the loop HTG node
void SpreadInstrsLoop(LoopHTGNode * node){
	SpreadInstrsList(&node->getLoopList());
}


//spread out the instructions in a list of HTG nodes
void SpreadInstrsList(MyLinkedList<BaseHTGNode*> *list){
	MyLinkedListIterator<BaseHTGNode *> *iter;
	SimpleHTGNode *simpnode;
	IfHTGNode *ifnode;
	LoopHTGNode *loopnode;

	iter=list->elementsForward();
	while(iter->hasMoreElements()){
		if((ifnode=dynamic_cast<IfHTGNode *>(iter->currentElement()))!=NULL)
			SpreadInstrsIf(ifnode);
		else if((simpnode=dynamic_cast<SimpleHTGNode *>(iter->currentElement()))!=NULL){
			SpreadInstrsSimple(simpnode);
		}
		else if((loopnode=dynamic_cast<LoopHTGNode *>(iter->currentElement()))!=NULL)
			SpreadInstrsLoop(loopnode);
		else
			abort();
		iter->nextElement();
	}
	delete iter;
}



//Spreads out the instructions in a routine
//routine: the routine
void SpreadInstrsRoutine(Routine *routine){

	if (!routine->isUsedInProgram()) return;

	printf("\nSpreading out instrs for pipelining\n...");
	SpreadInstrsList(routine->getHTG());
	printf("Done spreading out instrs for pipelining\n...");
}
