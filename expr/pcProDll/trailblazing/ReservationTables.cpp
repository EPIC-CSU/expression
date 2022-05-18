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
#include "NormalInstr.h"
#include "MemoryOp.h"
#include "InstrTypeProperty.h"
#include "BBIterator.h"
#include "UDDU.h"

#ifndef __UNIX__
	#include "crtdbg.h"
#else
	#define _ASSERT assert
#endif

#ifndef __UNIX__
#include <process.h>
#else
#define __declspec(dllimport)
#define __stdcall
#define _ASSERT assert
#include <stdio.h>
#endif

//includes for the unit name computation
#include "Instruction.h"
#include "Unit.h"
#include "CompName.h"
//#include "../../expression dll/IR/node.h"

#include <map>
#include <vector>

//global variables saying whether we are doing pipelined tbz, 
//and whether we are inside tbz
extern int pipelinedTbz;
extern int insideTbz;
extern long RTinquiries;
extern int stalledTbz;

//flag for debugging the RT generation and usage
int debugRT=1;

//flag for debugging the Pipelined scheduling (the RT testing part)
int debugPipelineSched=1;

extern int equalRegs(BaseArgument*, BaseArgument*);

extern int equalRegs(BaseArgument*, BaseArgument*);


//define this guy if need to link EXPRESSION into the project
#ifndef __UNIX__
#define __EXPRESSION__
#endif

//if 1, the reservation tables and read/write times are provided from the EXPRESSION descrion
//otherwise, the hard-coded version is used
#ifdef __EXPRESSION__
#define expression_linked 1
#else
#define expression_linked 0
#endif

//the pipeline length
//for c6x_mem
//int PIPELENGTH = 40;
//for elf only 8
// int PIPELENGTH = 40;
//
// Modified by SAH on 09/21/01
//
__declspec (dllimport) int PIPELENGTH;

//the max size of the instruction map
#define MAXMAP 200


typedef map<int,vector<int>*> ResTabType;


//the target
enum target_type {c6x,dlx,c6x_mem,elf,minimips, minimips_fpcoproc,st100_fpcoproc};
enum target_type target=minimips;


#if expression_linked
//EXPRESSION prototypes
__declspec( dllimport ) void GetReservationTableEXPRESSION(char *opcode,char **instruction,char *instr_type,int format,int *rt,int &computed);
__declspec( dllimport ) int get_capacity(char * unit);
__declspec( dllimport ) int GetReadWriteTimeEXPRESSION(char *opcode,char **instruction,char *instr_type,int format,int argument,int &computed);
__declspec( dllimport ) int get_unit_nr(char * unit);
__declspec( dllimport ) char * get_unit_name(int unit);
__declspec( dllimport ) void delete_db(void);
__declspec( dllimport ) void GetFeedbackLatenciesEXPRESSION(char *opcode1,char **instruction1,char *instr_type1,int format1,int &computed1,
							char *opcode2,char **instruction2,char *instr_type2,int format2,int &computed2,int *latencies);
__declspec( dllimport ) int ExistsQueueBetweenEXPRESSION(char *opcode1,char **instruction1,char *instr_type1,int format1,int &computed1,
			char *opcode2,char **instruction2,char *instr_type2,int format2,int &computed2,int latency);

#endif

//EXPRESS prototypes
void GetReservationTableHC(BaseOperation *oper,int unit,ResTabType * restab);
void start_time(void);
void pause_time(void);
void close_pipeline_tbz(void);


//////////////////////////// Field name conversion functions ////////////////////////////////////


//the register destination in a load
int load_dest_reg_field(void){
	switch(target){
	case c6x: 
		return _MEM_SRC_;
	case dlx:
		return _DEST_;
	case c6x_mem: 
	case elf:
	case minimips:
	case minimips_fpcoproc:
	case st100_fpcoproc:
		return _REG_DEST_;
	}
	_ASSERT(0);
	return -1;
}

//the register source in a store
int store_src_reg_field(void){
	switch(target){
	case c6x: 
		return _MEM_DEST_;
	case dlx:
		return _SOURCE_2_;
	case c6x_mem: 
	case elf:
	case minimips:
	case minimips_fpcoproc:
	case st100_fpcoproc:
		return _REG_SRC_;
	}
	_ASSERT(0);
	return -1;
}

//the memory block source in a load
int load_src_mem_field(void){
	switch(target){
	case c6x: 
		return _MEM_BYTE_;
	case dlx:
		return _MEM_BYTE_;
	case c6x_mem: 
	case elf: 
	case minimips:
	case minimips_fpcoproc:
	case st100_fpcoproc:
		return _MEM_SRC_;
	}
	_ASSERT(0);
	return -1;
}

//the memory block dest in a store
int store_dest_mem_field(void){
	switch(target){
	case c6x: 
		return _MEM_BYTE_;
	case dlx:
		return _MEM_BYTE_;
	case c6x_mem: 
	case elf: 
	case minimips:
	case minimips_fpcoproc:
	case st100_fpcoproc:
		return _MEM_DEST_;
	}
	_ASSERT(0);
	return -1;
}

//the destination in data and memory operations
int reg_dest_field(BaseOperation *oper){
	if(oper->isMemoryOp()){
		switch(target){
		case c6x:
			return _DEST_;
		case dlx:
			return _DEST_;
		case c6x_mem:
		case elf: 
		case minimips:
		case minimips_fpcoproc:
		case st100_fpcoproc:
			return _REG_DEST_;
		}
	}
	else
		return _DEST_;
	_ASSERT(0);
	return -1;
}


//initializes the express reservation table
void init_express_rt(ResTabType * restab){
	int j;
	for(j=0;j<PIPELENGTH;j++){
		(*restab)[j]=new vector<int>();
	}
}

//delete the express reservation table
//(only the internal lists)
void delete_express_rt(ResTabType * restab){
	int j;
	for(j=0;j<PIPELENGTH;j++){
		(*restab)[j]->clear();
		delete (*restab)[j];
	}
}

//delete the express reservation table
//(the whole thing)
void delete_whole_express_rt(ResTabType * restab){
	int j;
	for(j=0;j<PIPELENGTH;j++){
		(*restab)[j]->clear();
		delete (*restab)[j];
	}
	delete restab;
}

//delete the pipetab pipeline table 
//(only the internal lists)
void delete_pipetab_rt(ResTabType * restab){
	int j;
	for(j=-PIPELENGTH+1;j<PIPELENGTH;j++){
		(*restab)[j]->clear();
		delete (*restab)[j];
	}
}
extern int isNOP(int opCode);

/////////////////////////////////// EXPRESSION interface /////////////////////////////////////
//these functions provide the interface with the reservation tables generation 
//from EXPRESSION


//Converts an EXPRESS operation into the EXPRESSION format
//input: oper, unit
//optype: DATA or CTRL OP_SLOT
//output: instruction, opcode,format
void ConvertToExpression(BaseOperation *oper,int unit,SlotType optype,char **instruction,
						 char *&opcode,char *&unitname,int &format){
	Unit *u;
	char *rfname, *memname, *rfname1, *rfname2, *rfname3;

	switch(target){
	case c6x:
		rfname=strdup("RFA");
		memname=strdup("Memory_Bank0");
		break;
	case c6x_mem:
		rfname=strdup("RFA");
		memname=strdup("MemoryBlock");
		break;
	case elf:
		rfname=strdup("GPRFile");
		memname=strdup("ElfMemory");
		break;
	case dlx:
		rfname=strdup("REGFILE");
		memname=strdup("DLXMEMORY");
		break;
	case minimips_fpcoproc:
		rfname=strdup("GPRFile");
		rfname1=strdup("GPRFile");
		rfname2=strdup("CP_FRFile");
		memname=strdup("D_MEMORY");
		break;
	case st100_fpcoproc:
		rfname=strdup("DRF");
		rfname1=strdup("DRF");
		rfname2=strdup("CP_FRF");
		memname=strdup("D_MEMORY");
		break;
	case minimips:
		rfname=strdup("GPRFile");
		rfname1=strdup("GPRFile");
		rfname2=strdup("GPRFile");
		rfname3=strdup("IMM");
		memname=strdup("L1");
		break;
	}

	
	//first compute the unit name
	//u=simpleVLIWInstrTemplate->getFU(DATA_OP_SLOT,unit);
	u=simpleVLIWInstrTemplate->getFU(optype,unit);

	*instruction++=strdup("_UNIT_");
	unitname=*instruction++=getName(u).getStr();
	
	//get the opcode for this operation
	opcode=globalOpCodeTable[((NormalOp*)oper)->getOpCodeIndex()].opName();
	
	//find the format for this instructino (the fields that have to be filled in)
	if(oper->isComputeOp()){

		//first translate the source operands, and set the format

		// Hack by SAH on 09/20/01 to handle NOP operations
		// that were inserted due as correction ops.
		//
		int nr_dests=0;
		if (isNOP((oper->getOpCode()).index()))
		{
			// convert to a hardcoded EXPRESSION format.
			//
			*instruction++=strdup("_SOURCE_1_");
			*instruction++=rfname1;

			*instruction++=strdup("_SOURCE_2_");
			*instruction++=rfname1;

			*instruction++=strdup("_DEST_");
			*instruction++=rfname1;

			format=0;

			*instruction=NULL;
			//done
			return;
		}
		else if(oper->ptrToOperand(_SOURCE_3_)!=NULL){
			//instead of the virtual register found in all the operands, we use RFA
			//(*instruction)[strdup("_SOURCE_1_")]=((RegisterArg*)(oper->ptrToOperand(_SOURCE_1_)))->getText();
			*instruction++=strdup("_SOURCE_1_");
			if(oper->ptrToOperand(_SRC_1_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;

			*instruction++=strdup("_SOURCE_2_");
			if(oper->ptrToOperand(_SRC_2_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;

			*instruction++=strdup("_SOURCE_3_");
			if(oper->ptrToOperand(_SRC_3_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;

			format=6;
		}
		else if(oper->ptrToOperand(_SOURCE_2_)!=NULL){
			//instead of the virtual register found in all the operands, we use RFA
			//(*instruction)[strdup("_SOURCE_1_")]=((RegisterArg*)(oper->ptrToOperand(_SOURCE_1_)))->getText();
			*instruction++=strdup("_SOURCE_1_");
			if(oper->ptrToOperand(_SRC_1_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;

			*instruction++=strdup("_SOURCE_2_");
			if(oper->ptrToOperand(_SRC_2_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;

			format=0;
		}
		else{
			*instruction++=strdup("_SOURCE_1_");
			if(oper->ptrToOperand(_SRC_1_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;

			format=1;
		}

		//then translate the destination operands, and possibly set the foramt 

		if(oper->hasOperand(_IMPLIED_DEST_,_DEST_LIST_) && strcmp(opcode,"NOP")!=0){
			*instruction++=strdup("_DEST_1_");
			if(oper->ptrToOperand(_DST_,_DEST_LIST_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;

			*instruction++=strdup("_DEST_2_");
			if(oper->ptrToOperand(_IMPLIED_DEST_,_DEST_LIST_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;
			format=5;
			nr_dests=2;
		}
		else{
			*instruction++=strdup("_DEST_");
			if(oper->ptrToOperand(_DST_,_DEST_LIST_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;
			nr_dests=1;
		}

		//handle the source 3 if it is a 3-source instruction (format will be 6 or 7)
		/*  I am not sure what this was meant for: format 6 refers to mac-like operations : PB 
		if(oper->hasOperand(_SRC_3_)){
			*instruction++=strdup("_SOURCE_3_");
			if(oper->ptrToOperand(_SRC_3_)->isFloatReg())
				*instruction++=rfname2;
			else
				*instruction++=rfname1;

			if(nr_dests==2) format=6;
			else if(nr_dests==1) format=7;
			else _ASSERT(0);
		}
		*/

	}
	else if(oper->isMemoryOp()){
		if(((MemoryOp*)oper)->isLoad()){
			//loads
			*instruction++=strdup("_SOURCE_1_");
			*instruction++=rfname;

			switch(target){
			case c6x:
				*instruction++=strdup("_MEM_SRC_");
				*instruction++=rfname;
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname;
				*instruction++=strdup("_MEM_BYTE_");
				*instruction++=memname;
				format=3;
				break;
			case c6x_mem:
			case elf:
				*instruction++=strdup("_REG_DEST_");
				*instruction++=rfname;
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname;
				*instruction++=strdup("_MEM_SRC_");
				*instruction++=memname;
				format=3;
				break;
			case minimips:
				*instruction++=strdup("_REG_DEST_");
				*instruction++=rfname;
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname3;
				*instruction++=strdup("_MEM_SRC_");
				*instruction++=memname;
				format=4;
				break;
			case minimips_fpcoproc:
			case st100_fpcoproc:
				*instruction++=strdup("_REG_DEST_");
				*instruction++=rfname;
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname;
				*instruction++=strdup("_MEM_SRC_");
				*instruction++=memname;
				format=4;
				break;
			case dlx:
				*instruction++=strdup("_DEST_");
				*instruction++=rfname;
				*instruction++=strdup("_MEM_BYTE_");
				*instruction++=memname;
				format=3;
				break;
			}

		}
		else{
			//stores
			*instruction++=strdup("_SOURCE_1_");
			*instruction++=rfname;

			switch(target){
			case c6x:
				*instruction++=strdup("_MEM_DEST_");
				*instruction++=rfname;
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname;
				*instruction++=strdup("_MEM_BYTE_");
				*instruction++=memname;

				format=2;
				break;
			case c6x_mem:
			case elf:
				*instruction++=strdup("_REG_SRC_");
				*instruction++=rfname;
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname;
				*instruction++=strdup("_MEM_DEST_");
				*instruction++=memname;

				format=2;
				break;
			case minimips:
				*instruction++=strdup("_REG_SRC_");
				*instruction++=rfname;
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname3;
				*instruction++=strdup("_MEM_DEST_");
				*instruction++=memname;

				format=3;
				break;
			case minimips_fpcoproc:
			case st100_fpcoproc:
				*instruction++=strdup("_REG_SRC_");
				*instruction++=rfname;
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname;
				*instruction++=strdup("_MEM_DEST_");
				*instruction++=memname;

				format=3;
				break;
			case dlx:
				*instruction++=strdup("_SOURCE_2_");
				*instruction++=rfname;
				*instruction++=strdup("_MEM_BYTE_");
				*instruction++=memname;

				format=2;
				break;
			}
		}
	}
	else if(oper->isControlOp()){

/*
		if(oper->isCallOp()){
			*instruction++=strdup("_SOURCE_1_");
			*instruction++=rfname;

			*instruction++=strdup("_SOURCE_2_");
			*instruction++=rfname;

			*instruction++=strdup("_DEST_");
			*instruction++=rfname;

			format=0;
		}
		else 
*/
		if(oper->isIfOp()){
			*instruction++=strdup("_SOURCE_1_");
			*instruction++=rfname;

			*instruction++=strdup("_DEST_");
			*instruction++=rfname;

			format=1;
		}
		else if(oper->isCallRetOp())
		{
			*instruction++=strdup("_SOURCE_1_");
			*instruction++=rfname;

			*instruction++=strdup("_SOURCE_2_");
			*instruction++=rfname;

			format=5;
		}

		else
			_ASSERT(0);
	}
	else
		_ASSERT(0);
	*instruction=NULL;
}


//get reservation table for the operation oper
void GetReservationTable(BaseOperation *oper,int unit,SlotType optype,ResTabType * restab){
	char *instruction[MAXMAP];
	char *opcode,*unitname;
	int format,j,computed;
	int rt[1000];//buffer to receive the rt from the dll
	int i;
	BaseOpProperty *prop;
	char *instr_type=NULL;

	//find if it has an _INSTR_TYPE_ propery (e.g., PAGE MODE READ/WRITE, BURST MODE, etc.)
	prop=(InstrTypeProperty*)(oper->getPtrToProperty(_INSTR_TYPE_));
	if(prop!=NULL){
		//if it has an INSTRTYPE property
		instr_type=((InstrTypeProperty*)prop)->Value();
	}

	if(expression_linked){
		//if expression project is linked, use the reservation tables
		//generation to return this reservation table

		start_time();

		//first convert oper and unit into EXPRESSION format instruction, opcode, format
		ConvertToExpression(oper,unit,optype,instruction,opcode,unitname,format);

#if expression_linked
		//call reservation table generation from EXPRESSION
		GetReservationTableEXPRESSION(opcode,instruction,instr_type,format,rt,computed);

		//count the number of RT inquiries:
		RTinquiries++;
#endif

		//Transform the reservation table returned from EXPRESSION into the EXPRESS format

		//initialize the rt
		
		init_express_rt(restab);
		
		//convert the reservation table expr_rt to EXPRESS format
		for(i=0,j=0;j<PIPELENGTH;j++,i++){
			//-1 is the separator between the stages in the rt
			while(rt[i]!=-1){
				(*restab)[j]->push_back(rt[i]);
				i++;
			}
		}

		if(computed)
			pause_time();
	}
	else{
		//call the hard coded version of reservation table computation
		GetReservationTableHC(oper,unit,restab);
	}
}

//determine the pipeline stage in which the operand pos is read by operation oper
int ReadTime(BaseOperation *oper,int unit,SlotType optype,int pos){
	char *instruction[MAXMAP];
	char *opcode,*unitname;
	int format,rez;
	int computed;
	BaseOpProperty *prop;
	char *instr_type=NULL;

	if(debugRT){
		oper->print(0);
		printf("\n");
		fflush(stdout);
	}

	//find if it has an _INSTR_TYPE_ propery (e.g., PAGE MODE READ/WRITE, BURST MODE, etc.)
	prop=(InstrTypeProperty*)(oper->getPtrToProperty(_INSTR_TYPE_));
	if(prop!=NULL){
		//if it has an INSTRTYPE property
		instr_type=((InstrTypeProperty*)prop)->Value();
	}

	/*if(oper->isControlOp()){
		switch(pos){
		case _SOURCE_1_:
		case _SOURCE_2_:
			return 2;
		case _MEM_SRC_:
			return 4;
		}
		abort();
	}
	else */
	if(expression_linked){
		//if expression project is linked, use the EXPRESSION 

		start_time();

		//first convert oper and unit into EXPRESSION format instruction, opcode, format
		ConvertToExpression(oper,unit,optype,instruction,opcode,unitname,format);

#if expression_linked
		//count the number of RT inquiries:
		RTinquiries++;

		//get the read time from EXPRESSION
		rez=GetReadWriteTimeEXPRESSION(opcode,instruction,instr_type,format,pos,computed);

		if(computed)
			pause_time();

		return rez;
#endif
	}
	else{
		switch(pos){
		case _SOURCE_1_:
		case _SOURCE_2_:
			return 2;
		case _MEM_SRC_:
			return 4;
		}
		abort();
	}
	return -1;
}


//determine the pipeline stage in which the operand pos is written by operation oper
int WriteTime(BaseOperation *oper,int unit,SlotType optype,int pos){
	char *instruction[MAXMAP];
	char *opcode,*unitname;
	int format,rez;
	int computed;
	BaseOpProperty *prop;
	char *instr_type=NULL;

	//find if it has an _INSTR_TYPE_ propery (e.g., PAGE MODE READ/WRITE, BURST MODE, etc.)
	prop=(InstrTypeProperty*)(oper->getPtrToProperty(_INSTR_TYPE_));
	if(prop!=NULL){
		//if it has an INSTRTYPE property
		instr_type=((InstrTypeProperty*)prop)->Value();
	}


	/*if(oper->isControlOp()){
		switch(pos){
		case _DEST_:
			return 4;
		case _MEM_DEST_:
			return 4;
		}
		abort();
	}
	else */
	if(expression_linked){
		//if expression project is linked, use the EXPRESSION 

		start_time();

		//the DEST in EXPRESSION is called _MEM_SRC_. We should find some better names here. 
		if(oper->isMemoryOp() && pos==_DEST_ && target==c6x){
			pos=_MEM_SRC_;
		}

		//first convert oper and unit into EXPRESSION format instruction, opcode, format
		ConvertToExpression(oper,unit,optype,instruction,opcode,unitname,format);

#if expression_linked
		//count the number of RT inquiries:
		RTinquiries++;

		//get the write time from EXPRESSION
		rez=GetReadWriteTimeEXPRESSION(opcode,instruction,instr_type,format,pos,computed);

		if(computed)
			pause_time();

		return rez;
#endif
	}
	else{
		switch(pos){
		case _DEST_:
			return 4;
		case _MEM_DEST_:
			return 4;
		}
		abort();
	}
	return -1;
}


//determine the feedback latencies between oper1 and oper2
void FeedbackLatencies(BaseOperation *oper1,int unit1,SlotType optype1,
					  BaseOperation *oper2,int unit2,SlotType optype2,int*latencies){
	char *instruction1[MAXMAP],*instruction2[MAXMAP];
	char *opcode1,*unitname1,*opcode2,*unitname2;
	int format1,format2;
	int computed1,computed2;
	BaseOpProperty *prop;
	char *instr_type1=NULL,*instr_type2=NULL;

	//find if it has an _INSTR_TYPE_ propery (e.g., PAGE MODE READ/WRITE, BURST MODE, etc.)
	prop=(InstrTypeProperty*)(oper1->getPtrToProperty(_INSTR_TYPE_));
	if(prop!=NULL){
		//if it has an INSTRTYPE property
		instr_type1=((InstrTypeProperty*)prop)->Value();
	}

	//find if it has an _INSTR_TYPE_ propery (e.g., PAGE MODE READ/WRITE, BURST MODE, etc.)
	prop=(InstrTypeProperty*)(oper2->getPtrToProperty(_INSTR_TYPE_));
	if(prop!=NULL){
		//if it has an INSTRTYPE property
		instr_type2=((InstrTypeProperty*)prop)->Value();
	}


	if(expression_linked){
		//if expression project is linked, use the EXPRESSION 

		start_time();

		//first convert oper and unit into EXPRESSION format instruction, opcode, format
		ConvertToExpression(oper1,unit1,optype1,instruction1,opcode1,unitname1,format1);
		ConvertToExpression(oper2,unit2,optype2,instruction2,opcode2,unitname2,format2);

#if expression_linked
		//count the number of RT inquiries:
		RTinquiries++;

		//get the write time from EXPRESSION
		GetFeedbackLatenciesEXPRESSION(opcode1,instruction1,instr_type1,format1,computed1,
			opcode2,instruction2,instr_type2,format2,computed2,latencies);

		if(computed1 || computed2)
			pause_time();
#endif
	}
	else{
		latencies[0]=0;
	}
}


//get the capacity of a unit
int GetCapacity(int unit){
	char *unitname;
	//Unit *u;

	if(expression_linked){
		//if expression project is linked, use the EXPRESSION 

		//first compute the unit name
		//u=simpleVLIWInstrTemplate->getFU(DATA_OP_SLOT,unit);
		//unitname=getName(u).getStr();
#if expression_linked
		unitname=get_unit_name(unit);
		
		//get the write time from EXPRESSION
		return get_capacity(unitname);
#endif
	}
	else{
		switch(unit){
		case 100: //FETCH
		case 101: //DECODE
		case 102: //READ_OP
		case 103: //WRITE_OP
			return 6;
		default:
			return 1;
		}
	}
	abort();
	return -1;
}

//find the identifier (int) for the unit name
int GetUnitNr(char *name){
	if(expression_linked){
		//if expression project is linked, use the EXPRESSION 

#if expression_linked
		//get the write time from EXPRESSION
		return get_unit_nr(name);
#endif
	}
	else{
		if(strcmp(name,"FETCH")==0) return 100;
		if(strcmp(name,"DECODE")==0) return 101;
		if(strcmp(name,"READ_OP")==0) return 102;
		if(strcmp(name,"WRITE_OP")==0) return 103;
		abort();
	}
	return 0;
}


/////////////////////////////////// DONE EXPRESSION interface /////////////////////////////////////


//returns the reservation table for the MIPS architecture
//(hard coded version)
void GetReservationTableHC(BaseOperation *oper,int unit,ResTabType * restab){
	//This is a rough reservation table for the MIPS
	(*restab)[0]=new vector<int>();
	(*restab)[0]->push_back(GetUnitNr("FETCH"));

	(*restab)[1]=new vector<int>();
	(*restab)[1]->push_back(GetUnitNr("DECODE"));

	(*restab)[2]=new vector<int>();
	(*restab)[2]->push_back(GetUnitNr("READ_OP"));

	(*restab)[3]=new vector<int>();
	(*restab)[3]->push_back(unit);

	(*restab)[4]=new vector<int>();
	(*restab)[4]->push_back(GetUnitNr("WRITE_OP"));
}


//Add a reservation table to a pipeline table (representing all the rts for a
//region of the program). 
void AddReservationTable(ResTabType * restab,ResTabType * pipetab,int position){
	int i;
	vector<int>::iterator j;
	for(i=0;i<PIPELENGTH;i++){
		_ASSERT((*restab)[i]);
		for(j=(*restab)[i]->begin();j!=(*restab)[i]->end();j++){
			if(i+position<PIPELENGTH){
				(*pipetab)[i+position]->push_back(*j);
			}
		}
	}
}


//get the union of reservation tables for all the operations
//in instr
void GetReservationTables(NormalInstr *instr,ResTabType * pipetab,int position){
	ArrayIterator<DataOpSlot> *dataIter;
	ArrayIterator<ControlOpSlot> *controlIter;
	//ArrayIterator<FlowOpSlot> *flowIter;
	ResTabType restab;
	int i;
	
	for(dataIter=instr->getDataOpList()->elements(),i=0;dataIter->hasMoreElements();dataIter->nextElement(),i++){
		if((dataIter->currentElement()).hasOper()){
			restab.clear();
			GetReservationTable(dataIter->currentElement().getPtrToOperation(),i,DATA_OP_SLOT,&restab);
			AddReservationTable(&restab,pipetab,position);
			delete_express_rt(&restab);
		}
	}
	delete dataIter;
	
	for(controlIter=instr->getControlOpList()->elements(),i=0;controlIter->hasMoreElements();controlIter->nextElement(),i++){
		if((controlIter->currentElement()).hasOper()){
			restab.clear();
			GetReservationTable(controlIter->currentElement().getPtrToOperation(),i,CONTROL_OP_SLOT,&restab);
			AddReservationTable(&restab,pipetab,position);
			delete_express_rt(&restab);
		}
	}
	delete controlIter;

	/*
	for(flowIter = _flowOpList.elements(),i=0;flowIter->hasMoreElements();flowIter->nextElement(),i++){
		if(flowIter->currentElement().hasOper()){
			flowIter->currentElement();
		}
	}
	delete flowIter;
	*/
}


//get the union of all reservation tables for all the previous -------------------------------and following PIPELENGH instrs
//pipetab[-PIPELENGTH+1], ...pipetab[0], pipetab[1], .... pipetab[PIPELENGTH-1]
void GetPipelineTable(NormalInstr *instr,ResTabType * pipetab){
	BaseHTGNode *node;
	NormalInstr *prev;
	int i;
	//initialize the pipetab
	for(i=-PIPELENGTH+1;i<PIPELENGTH;i++){
		(*pipetab)[i]=new vector<int>();
	}
	//compute the pipeline for previous instructions
	for(i=0,node=instr;i>1-PIPELENGTH && node!=NULL;i--){
		if(node!=NULL && (prev=dynamic_cast<NormalInstr*>(node))!=NULL){
			GetReservationTables(prev,pipetab,i);
		}
		node=node->getPrev();
	}

/*	//initialize the pipetab
	for(i=-PIPELENGTH+1;i<PIPELENGTH;i++){
		(*pipetab)[i]=new vector<int>();
	}
	//compute the pipeline for previous instructions
	for(i=-1,node=instr;i>-PIPELENGTH && node!=NULL;i--){
		node=node->getPrev();
		if(node!=NULL && (prev=dynamic_cast<NormalInstr*>(node))!=NULL){
			GetReservationTables(prev,pipetab,i);
		}
	}
	//compute the pipeline for subsequent instructions
	for(i=0,node=instr;i<PIPELENGTH && node!=NULL;i++){
		if(node!=NULL && (next=dynamic_cast<NormalInstr*>(node))!=NULL){
			GetReservationTables(next,pipetab,i);
		}
		node=node->getNext();
	}
*/
}

//count the nr of instances of unit in the unitset pipe stage
int NrInstances(vector<int> *unitset,int unit){
	vector<int>::iterator i;
	int res=0;
	for(i=unitset->begin();i!=unitset->end();i++){
		if(*i==unit) res++;
	}
	return res;
}

//Check if a reservation table of a new instr fits into the pipeline table 
//of a region of the program
int FitsReservationTable(ResTabType * restab,ResTabType * pipetab){
	int i;
	vector<int>::iterator j;
	int nr_instances,capacity;
	int unit;
	char *unitname;

	//for each pipeline stage
	for(i=0;i<PIPELENGTH;i++){
		_ASSERT((*restab)[i]);
		//for each resource in this pipeline stage in the restab
		for(j=(*restab)[i]->begin();j!=(*restab)[i]->end();j++){
			unit=*j;
#if expression_linked
			unitname=get_unit_name(unit);
			capacity=GetCapacity(unit);
#endif
			nr_instances=NrInstances((*pipetab)[i],unit);
			//if no more space for this resource
			if(nr_instances+1>capacity){
				if(debugPipelineSched){
					printf("Conflict due to %s\n.",unitname);
				}
				return 0;
			}
		}
	}
	return 1;
}

//returns 1 if there is a queue between the two operations, and returns in latency the latency between them
int ExistsQueueBetween(BaseOperation *oper1,int unit1,SlotType optype1,
						   BaseOperation *oper2,int unit2,SlotType optype2,int latency){
	char *instruction1[MAXMAP],*instruction2[MAXMAP];
	char *opcode1,*unitname1,*opcode2,*unitname2;
	int format1,format2;
	int computed1,computed2;
	BaseOpProperty *prop;
	char *instr_type1=NULL,*instr_type2=NULL;
	int r;

	//find if it has an _INSTR_TYPE_ propery (e.g., PAGE MODE READ/WRITE, BURST MODE, etc.)
	prop=(InstrTypeProperty*)(oper1->getPtrToProperty(_INSTR_TYPE_));
	if(prop!=NULL){
		//if it has an INSTRTYPE property
		instr_type1=((InstrTypeProperty*)prop)->Value();
	}

	//find if it has an _INSTR_TYPE_ propery (e.g., PAGE MODE READ/WRITE, BURST MODE, etc.)
	prop=(InstrTypeProperty*)(oper2->getPtrToProperty(_INSTR_TYPE_));
	if(prop!=NULL){
		//if it has an INSTRTYPE property
		instr_type2=((InstrTypeProperty*)prop)->Value();
	}


	if(expression_linked){
		//if expression project is linked, use the EXPRESSION 

		start_time();

		//first convert oper and unit into EXPRESSION format instruction, opcode, format
		ConvertToExpression(oper1,unit1,optype1,instruction1,opcode1,unitname1,format1);
		ConvertToExpression(oper2,unit2,optype2,instruction2,opcode2,unitname2,format2);

#if expression_linked
		//count the number of RT inquiries:
		RTinquiries++;

		//get the write time from EXPRESSION
		r=ExistsQueueBetweenEXPRESSION(opcode1,instruction1,instr_type1,format1,computed1,
			opcode2,instruction2,instr_type2,format2,computed2,latency);

		if(computed1 || computed2)
			pause_time();

		return r;
#endif
	}
	else{
		return 0;
	}
}


//returns 1 if oper1 depends on oper2 (flow dependency) and the dependency latency between oper1 and oper2 is NOT satisfied when 
//they are spread at a distance of "distance" cycles
int DependencyLatencyNotSatisfied(BaseOperation *oper1,int unit1,SlotType optype1,
						   BaseOperation *oper2,int unit2,SlotType optype2,int distance){
	MyLinkedList<BaseOperation*> *useList;
	BaseArgument *arg1;
	int latencies[1000],j;
	int t1,t2,max;
	int depends=0;
	MyLinkedListIterator<BaseOperation*> *i;
	ArgList &a = oper2->destOperandList();
	ArgListIter ai;
	int latency;
	//if there is a queue between oper1 and oper2, compare the latency to the distance
	if(ExistsQueueBetween(oper1,unit1,optype1,oper2,unit2,optype2,latency))
		return (latency>distance);
	//for all the destinations of oper2
	for (ai = a.begin(); ai != a.end(); ai++){
		if((*ai)==NULL) continue;
		//arg1 is the current destination of oper2
		arg1=*ai;
		useList=GetUseList(arg1);
		//if useList NULL, oper2 has no uses, oper1 does not depend on oper2, return 0
		if(useList==NULL) return 0;
		//for all the uses of the destination of oper2
		for(i=useList->elementsForward();i->hasMoreElements();i->nextElement()){
			//if this use is in oper2
			
			if(i->currentElement()==oper1){
				//if there is a feedback path between oper1 and oper2
				//max should be the difference between feedback_write(oper2) - feedback_read(oper1)
				//we should use the shortest feedback path or RF path between the two operations for an argument
				//between arguments we should use the longest one, but if there is a shorter feedback, use that one

				//otherwise the latency is based on the actual read/write times of oper1 and oper2
				if(equalRegs(arg1,i->currentElement()->ptrToOperand(_SRC_1_))){
					t1=WriteTime(oper2,unit2,optype2,reg_dest_field(oper2));
					t2=ReadTime(oper1,unit1,optype1,_SOURCE_1_);
					if(t1-t2>max) max=t1-t2;
					depends=1;
				}
				if(equalRegs(arg1,i->currentElement()->ptrToOperand(_SRC_2_))){
					t1=WriteTime(oper2,unit2,optype2,reg_dest_field(oper2));
					t2=ReadTime(oper1,unit1,optype1,_SOURCE_2_);
					if(t1-t2>max) max=t1-t2;
					depends=1;
				}
				if(equalRegs(arg1,i->currentElement()->ptrToOperand(_SRC_3_))){
					t1=WriteTime(oper2,unit2,optype2,reg_dest_field(oper2));
					if(oper1->isMemoryOp())
						t2=ReadTime(oper1,unit1,optype1,_REG_SRC_);
					else 
						t2=ReadTime(oper1,unit1,optype1,_SOURCE_3_);
					if(t1-t2>max) max=t1-t2;
					depends=1;
				}
			}
		}
	}
	//if there is no depency between oper1 and oper2, return 0
	if(!depends) return 0;
	//if there is a dependency
	//if the distance is larger then max, then the dependency is satisfied
	if(distance>=max) return 0;
	//otherwise, if the distance is equal to the latency of a feedback path between oper1 and oper2, then its
	//satisfied
	FeedbackLatencies(oper1,unit1,optype1,oper2,unit2,optype2,latencies);
	//check if disnance is equal to any of the feedback latencies
	for(j=0;latencies[j]!=-1;j++){
		if(distance==latencies[j]){
			//if the distance is equal to a feedback latency, return 0, we're still fine
			return 0;
		}
	}
	//if the distance is not equal to any feedback latencies, return 1, there is a data hazard
	return 1;
}


//returns 1 if the flow dependencies between oper and any other opers previous PIPELENGTH
//are satisfied either by feedback paths or by the register file write/reads
int FlowDataHazardsFree(NormalInstr *instr,BaseOperation *oper1,int unit1,SlotType optype1){
	BaseHTGNode *node;
	NormalInstr *prev;
	int i,j;
	int unit2;
	ArrayIterator<DataOpSlot> *dataIter;
	ArrayIterator<ControlOpSlot> *controlIter;
	BaseOperation *oper2;
	SlotType optype2;
			

	//for all the previous operations before instr
	for(i=-1,node=instr->getPrev();i>1-PIPELENGTH && node!=NULL;i--){
		if(node!=NULL && (prev=dynamic_cast<NormalInstr*>(node))!=NULL){
			//for all the operations in prev
			for(dataIter=prev->getDataOpList()->elements(),j=0;dataIter->hasMoreElements();dataIter->nextElement(),j++){
				if((dataIter->currentElement()).hasOper()){
					unit2=prev->getSlotIndex(&(dataIter->currentElement()));
					optype2=DATA_OP_SLOT;
					oper2=(dataIter->currentElement()).getPtrToOperation();
					//check latency between oper1 and dataIter->currentElement()->getPtrToOperation();
					if(DependencyLatencyNotSatisfied(oper1,unit1,optype1,oper2,unit2,optype2,abs(i)))
						return 0;
				}
			}
			delete dataIter;
			
			for(controlIter=prev->getControlOpList()->elements(),j=0;controlIter->hasMoreElements();controlIter->nextElement(),j++){
				if((controlIter->currentElement()).hasOper()){
					optype2=CONTROL_OP_SLOT;
					unit2=prev->getSlotIndex(&(controlIter->currentElement()));
					oper2=(controlIter->currentElement()).getPtrToOperation();
					//check latency between oper and dataIter->currentElement()->getPtrToOperation();
					if(DependencyLatencyNotSatisfied(oper1,unit1,optype1,oper2,unit2,optype2,abs(i)))
						return 0;
				}
			}
			delete controlIter;


		}
		node=node->getPrev();
	}
	return 1;
}

//returns 1 if there are enough resources in the pipeline for this operation
int PipelineFree(NormalInstr *instr,BaseOperation *oper,int unit,SlotType optype){
	int r;
	//if we are not in pipelined-tbz mode, or we are not using this function
	//for trailblazing (e.g., it is called from mutations or instr sel), 
	//simply return 1. 
	//if(!pipelinedTbz || !insideTbz || !stalledTbz) return 1;
	if(!pipelinedTbz || !insideTbz) return 1;

	ResTabType restab,pipetab;
	restab.clear();
	pipetab.clear();

	//Do the pipeline comparison
	//get reservation table for the operation oper
	//restab[0], restab[1], .... restab[PIPELENGTH]
	GetReservationTable(oper,unit,optype,&restab);
	//get the union of all reservation tables for the instr (for previous and following PIPELENGH instrs)
	//pipetab[-PIPELENGTH+1], ...pipetab[0], pipetab[1], .... pipetab[PIPELENGTH-1]
	GetPipelineTable(instr,&pipetab);
	//see if restab fits into pipetab
	r=FitsReservationTable(&restab,&pipetab);
	//check if the data dependencies between oper and it's dependents and dependees are exactly matched by 
	//feedback paths, or are greater then the regular RF-based timings
	r&=FlowDataHazardsFree(instr,oper,unit,optype);
	delete_express_rt(&restab);
	delete_pipetab_rt(&pipetab);
	return r;
}

//advances the bb iterator until it reaches instr
void AdvanceBBIterTo(NormalInstr *instr,BBForwIterator *iter){
	for(iter->reset();iter->hasMoreElements() && iter->currentElement()!=instr;){
		iter->nextElement();
	}
}

void print_res_tab(ResTabType *rt){
	int i,unit;
	char *unitname;
	vector<int>::iterator j;
	printf("Reservation Table:\n");
	for(i=0;i<PIPELENGTH;i++){
		printf("%d: ",i);
		for(j=(*rt)[i]->begin();j!=(*rt)[i]->end();j++){
			unit=*j;
			unitname=get_unit_name(unit);
			printf("%s ",unitname);
		}
		printf("\n");
	}
}

//returns 1 if there is no conflict in the pipeline after instr
int NoPipelineConflicts(NormalInstr *instr,BasicBlockNode *bb){
	ResTabType restab;
	int i;
	vector<int>::iterator j;
	int nr_instances,capacity;
	int unit;
	char *unitname;
	BBForwIterator iter(bb);

	restab.clear();
	init_express_rt(&restab);

	AdvanceBBIterTo(instr,&iter);

	//for PIPELENGTH instructions following instr
	for(i=0;i<PIPELENGTH && iter.hasMoreElements();i++){
		GetReservationTables((NormalInstr *)(iter.currentElement()),&restab,i);
		iter.nextElement();
	}

	if (debugPipelineSched)
	{
		cout << endl << "Reservation Table" << endl;
		// print the reservation table
		for(i=0;i<PIPELENGTH;i++)
		{
			cout << "Units at time = " << i;
			for(j=restab[i]->begin();j!=restab[i]->end();j++)
			{
				unit=*j;
#if expression_linked
				unitname=get_unit_name(unit);
				capacity=GetCapacity(unit);
				cout << " " << unitname << " ";
#endif
			}
			cout << endl;
		}
		cout << endl;
	}


	//for each pipeline stage
	for(i=0;i<PIPELENGTH;i++){
		_ASSERT(restab[i]);
		//for each resource in this pipeline stage in the restab
		for(j=restab[i]->begin();j!=restab[i]->end();j++){
			unit=*j;
#if expression_linked
			unitname=get_unit_name(unit);
			capacity=GetCapacity(unit);
#endif
			nr_instances=NrInstances(restab[i],unit);
			//if too many instances of this resource
			if(nr_instances>capacity){
				if(debugPipelineSched){
					printf("\nConflict in SpreadInstrs at instr %d due to %s.",instr->idValue(),unitname);
					print_res_tab(&restab);
				}
				delete_express_rt(&restab);
				return 0;
			}
		}
	}
	delete_express_rt(&restab);
	return 1;
}


void close_pipeline_tbz(void){
	delete_db();
}

