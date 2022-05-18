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
/*************************************************************
 *file  : ExecSemSupportFuncs.h                              *
 *created : 17 Feb, 1999.                                    *
 *authors : akhare@ics.uci.edu                               *
 *last modified :                                            *
 *                                                           *
 *contents:contains classes for base read, write execute     *
 *         functions which constitute the dosteps of units   *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/
#ifndef _EXECSEMSUPPORT_FUNCTIONS_H_
#define _EXECSEMSUPPORT_FUNCTIONS_H_

// THIS VERSION IS FOR THE SUPERSCALAR VERSION OF THE R10K PROCESSOR

#include "stdafx.h"
#include "DerivedUnit.h"
#include "DerivedLatch.h"
#include "Port.h"

#include "IConstArg.h"
#include "FConstArg.h"
#include "CallOp.h"
#include "MemoryOp.h"
#include "DerivedAddrData.h"
#include "DerivedLatchData.h"

#define _SRC1_ 1
#define _SRC2_ 2

extern unsigned long clock_count;
extern unsigned long lockstep_clock_count;

extern void addNOP(Latch *lPtr);
extern void add2OpNOP(Latch *lPtr);

//SS 12/06/00 Add Predication support
//extern void operandRead(Val & opReadSrc, AddrDataUnion & src, Port* portPtr);
extern void operandRead(Val & opReadSrc, PredicateAddrDataUnion & src, Port* portPtr);

extern void writeBack(Unit * unitPtr, Port * portPtr, BaseType & value, BaseType & dest, SignalClass & latency);

extern int convertReg(AddrOperand & a, char *str, int tempOpcode);
extern void convertDestToDecodeOperand(AddrOperand & a, BaseArgument *argPtr);

//SS 12/06/00
//extern void convertSrcToDecodeOperand(AddrDataUnion & a, BaseArgument *argPtr);
extern void convertSrcToDecodeOperand(PredicateAddrDataUnion & a, BaseArgument *argPtr);

extern void convertDestToDecodeOperand(AddrOperand & a, char *oper, int tempOpcode);
extern void convertSrcToDecodeOperand(PredicateAddrDataUnion & a, char *oper, int tempOpcode);
extern void processOp(char *instr, int opindex, OperationLatchData & tempElem);
extern void processCallOp(char *instr, int opindex, OperationLatchData & tempElem);
extern void processMemoryOp(char *instr, int opindex, OperationLatchData & tempElem);
extern void processControlOp(char *instr, int opindex, OperationLatchData & tempElem);
extern void processDataAndMemoryOp(char *instr, int opindex, OperationLatchData & tempElem);

extern long getBranchAddress(char * instr);
//extern InstrPtr getFallThruAddress(BaseOperation *currOper);
extern void executeBranchOp(BaseOperation * currOper);

extern vector<MyString> readFromMem(long addr, int numfetches, int &done, int &instfetched);
extern long readFromPgmCounterLatch(void);
extern vector<MyString> readFromFetDecLatch(void);
extern void writeToFetDecLatch(vector<MyString> a);
extern void writeToPgmCounterLatch(long addr);
extern void splitAddrInstr(MyString InsStr, MyString &Inst, long & Addr) ;
extern long getTargetAddress(MyString InsStr);
extern int isBranchInstr(MyString InsStr);
extern int isGotoInstr(MyString InsStr);

// extern void writeToPCLatch(InstrPtr tgt);
extern int CountOperInList(char *oplist);
extern int isIDENTIFIER(char *oper);
extern int isRegisterArg(int &UniqID, char *oper);
extern int isIConstArg(char *oper);
extern int isFConstArg(char *oper);
extern void OpInList(char *list, int i, char *oper);
extern int countOperInList(char *oplist);

extern Unit * getOpConsumptionUnit(int opcode,int & reqdTime);
extern UnitList * getForwardingUnitsList(Unit * unit);
extern int dataAvailableFromForwPaths(Unit * src, Unit * dest, int reqdTime);
extern int getNumCyclesToReachForwSrcUnit(Unit * start, Unit * end);

extern int isLdStOp(int opCode);
extern int isControlOp(int opCode);
extern int isCompareOp(int opCode);

extern int getPipeNumber(int opcode);
extern int isStore(int opCode);
extern int isLoad(int opCode);
extern int isDouble(int opCode);
extern int isDebugPrint(int opCode);
extern int isOperation(int opCode, char* op);

extern int getGotoOpIndex(void);

extern int isGotoOp(int opCode);

extern int isSpecialOpCode(int opCode);
extern int isLibFuncCall(int opcode);

//SS 2/28/01 For Predication
//extern void doSpecialStuff(TwoOpLatchData & elem);

extern void doSpecialStuff(OperationLatchData & elem);

extern void openDebugFile(void);
extern void closeDebugFile(void);

extern MyLinkedList<AddrOperand> listOfDestinations;
typedef MyLinkedListIterator<AddrOperand>* listOfDestIterator;

extern int getLatency(int opcode);
extern void removeDestFromListOfDestinations(Unit *, BaseType &);

extern int convertReg(int & a, BaseArgument *argPtr);
extern int convertReg(AddrOperand & a, BaseArgument *argPtr);

enum State 
{
	_FRESH_,
	_ADDRCALC_,
	_READ_WRITE_,
	_REPLAY_,
	_COMPLETED_
};


class LdStInsn
{
public:

	//SS 12/05/00
	//ThreeOpLatchData _ldstInsn;
	OperationLatchData _ldstInsn;
	State _statebit;
	long _address;
	Val _value;
	int _latency;

	//LdStInsn(ThreeOpLatchData ldstInsn, State statebit): _value()
	LdStInsn(OperationLatchData ldstInsn, State statebit): _value()
	{
		_ldstInsn = ldstInsn;
   		_statebit = statebit;
		_address = -999;
		//_value = NULL;
		_latency = -1;
	}
	~LdStInsn(){}
	void print(ostream & out);
};


class SimpleInsn
{
public:
	char _opcode[20];
	int _dest;
	vector<int> _src;

	SimpleInsn(char * opcode, int dest, vector<int> src)
	{
		strcpy(_opcode, opcode);
		_dest = dest;
		_src = src;

	}

	SimpleInsn()
	{
	}

	~SimpleInsn(){}
	void print(ostream & out);
};


class ComplexInsn
{
public:
	char _opcode[20];
	vector<SimpleInsn> _simpleInsns;

	ComplexInsn(char * opcode, vector<SimpleInsn> simpleInsns)
	{
		strcpy(_opcode, opcode);
		_simpleInsns = simpleInsns;
	}

	ComplexInsn()
	{
	}

	~ComplexInsn(){}
	void print(ostream & out);
};

extern bool isComplexOp(int opCode, ComplexInsn & complexInst);
void initializeComplexOpList(char* fname);

#endif
