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
 *file  : ExecSemSupportFuncs.cpp                            *
 *created : 17 Feb,99.                                       *
 *authors : akhare@ics.uci.edu                               *
 *last modified : Sudeep 05/20/03		                     *
 *                                                           *
 *contents:contains class methods for Unit class & other     *
 *         classes derived from it.                          *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#include "ExecSemSupportFuncs.h"
#include "ControlTable.h"
#include "..\..\pcProDll\DebugSwitches.h"
#include "CompName.h"
#include "fstream.h"
#include "ExecSemantics.h"
#include "StatList.h"
#include "ArchStats.h"
#include "NormalInstr.h"
#include "BaseOperation.h"
#include "BaseArgument.h"
#include "IConstArg.h"
#include "FlowOp.h"
#include "RAProperty.h"

extern ArchStatsDB globalArchStatsDB;
extern int doneSSA;
extern bool imemJunk;
#define _STRING_SIZE_ 128

// TODO: This can be passed to the readfromMemory function through the
// addname, getname funcs....
extern MemoryModule * icachePtr;
extern map<MyString, long> GlobalLabels;

vector<ComplexInsn> ComplexOpList;

//int printSimulator = 0;

MyString pcStr("pcLatch");
Latch * pc = (Latch*)getComp(pcStr);

// THIS VERSION IS FOR THE SUPERSCALAR VERSION OF THE R10K PROCESSOR

void addNOP(Latch *lPtr)
{
   if (_NULL_ == lPtr) return;
	
   OperationLatchData tempElem;

   int opCode = globalOpCodeTable.getIndex("NOP");
   tempElem.setOpCode(opCode);

	lPtr->write(tempElem);
}

inline int isOpCode(int opCode, char *str)
{
	int tempOpCode = globalOpCodeTable.getIndex(str);
	if (tempOpCode == opCode) return 1;

	return 0;
}

void add2OpNOP(Latch *lPtr)
{
   //this function creates a NOP which actually has two NULL operands
   if (_NULL_ == lPtr) return;
   
   if (lPtr->is(_COMPOUNDLATCH_)) return; // TODO: should be modified.
	
   //SS 2/28/01 For Predication use OperationLatchData
   //TwoOpLatchData tempElem;

   OperationLatchData tempElem;
   int opCode = globalOpCodeTable.getIndex("NOP");
   tempElem.setOpCode(opCode);

	lPtr->write(tempElem);
}

//SS 2/12/01 PredicateAddrDataUnion
//void operandRead(Val & opReadSrc, AddrDataUnion & src, Port* portPtr)
void operandRead(Val & opReadSrc, PredicateAddrDataUnion & src, Port* portPtr)

{
	Val a1;

	if (src.getType() == _ADDRTYPE_)
	{
		if((src.getAddr()).getType() == _REG_)
		{
			// Register Address
			portPtr->read(a1, src.getAddr());
			opReadSrc = a1;
		}
		else
		{
			// Immediate Memory Address
			a1.changeValue((src.getAddr()).getAddrOperand());
			opReadSrc = a1;
		}
	}
	else if ((src.getType()) == _VALTYPE_)
	{
		if((src.getData())._valType == _INT)
		{
			a1._valType = _INT;
			a1._as.integer = (src.getData())._as.integer;
		}
		if((src.getData())._valType == _FLT)
		{
			a1._valType = _FLT;
			a1._as.flt = (src.getData())._as.flt;
		}	 		
		if ((src.getData())._valType == _DBL)
		{
			a1._valType = _DBL;
			a1._as.dbl = (src.getData())._as.dbl;
		}
		opReadSrc = a1;
	}
	else if (src.getType() == _NOTYPE_) 
	{
		opReadSrc = a1;
	}
	else /* Currently we do not support any other type. */
	{
		assert(0);
	}
}

void removeDestFromListOfDestinations(Unit * unitPtr, BaseType & dest)
{
	/*
	// do the following:
	//	remove dest address object from listOfDestinations
	//	remove pointer to dest address(destptr) from the unit's list of destptrs
	//	set destwritten for this unit equal to the destptr for dest being written
	//	set destpropagated = 0
	listOfDestIterator i1 = listOfDestinations.elementsForward();

	while (i1->hasMoreElements())
	{
		AddrOperand & tempElem = i1->currentElement();
		if (tempElem == (AddrOperand &) dest)
		{
			ControlTableEntry & tempEntry = globalControlTable.getEntry(unitPtr);
			tempEntry.deleteDest(&tempElem);
			tempEntry.setDestWritten(&tempElem);
			tempEntry.setDestPropagated(0);
			listOfDestinations.remove(tempElem);
			delete i1;
			return;
		}
		i1->nextElement();
	}
	delete i1;
	cout << "Panic: writeBack() :Destination written back not present in the list of Destinations" << endl;
	assert(0);
	*/
}

void getValRName(int symTabNum, MyString & str)
{
	if (!doneSSA)
	{
		RegisterFile & tempRegFile = globalRegFileList->getRegFile(symTabNum);
		str = tempRegFile.name();
	}
	else
	{
		int idx = globalSymbolTable->getIndex(symTabNum);
		str = (globalRegFileList->getRegFile(idx)).name();
	}
}

int getValRType(int symTabNum)
{
	if (!doneSSA)
	{
		RegisterFile & tempRegFile = globalRegFileList->getRegFile(symTabNum);
		return tempRegFile.valType();
	}
	else
	{
		int idx = globalSymbolTable->getIndex(symTabNum);
		return (globalRegFileList->getRegFile(idx)).valType();
	}
}

	 
void processOp(char *instr, int opindex, OperationLatchData & tempElem)
{
	//this is for Data Units
	tempElem.setOpCode(opindex);
	char opcode[_STRING_SIZE_]="", dstlist[_STRING_SIZE_]= "", srclist[_STRING_SIZE_]= "";

	sscanf(instr,"%s%s%s",opcode,dstlist,srclist);

	if (countOperInList(dstlist) > 0)
	{
		char dst[_STRING_SIZE_];
		AddrOperand a;

		OpInList(dstlist,1,dst);
		convertDestToDecodeOperand(a, dst, opindex);
		(tempElem.getOperand1()).push_back(a);
	}

	int srcCount = countOperInList(srclist);

	int i = 1;
	while (i <= srcCount )
	{
		char src[_STRING_SIZE_];
		PredicateAddrDataUnion tmp;

		OpInList(srclist,i,src);
		convertSrcToDecodeOperand(tmp, src, opindex);
		(tempElem.getOperand2()).push_back(tmp);

		i++;
	}
}


void processControlOp(char *instr, int opindex, OperationLatchData & tempElem)
{
	char opcode[_STRING_SIZE_]="", dstlist[_STRING_SIZE_]= "", srclist[_STRING_SIZE_]= "";
	sscanf(instr,"%s%s%s", opcode, dstlist, srclist);

	if (isCall(opindex))
	{
		processCallOp(instr, opindex, tempElem);
	}
    else if (isRet(opindex))
	{
		tempElem.setOpCode(opindex);
	}
	else
	{
		// If its not call/ret it must be branch.
		//NOTE: For branches we need to push the instruction address location also..

		tempElem.setOpCode(opindex);

		if (strcmp("()",dstlist)) {
			printf("WARNING: Control intructions should NOT have any DEST list....");
			assert(0);
		}

		int srcCount = countOperInList(srclist);
		int i = 1;
		while (i <= srcCount)
		{
			PredicateAddrDataUnion tmp;
			char src1[_STRING_SIZE_];
			
			OpInList(srclist,i,src1);
			convertSrcToDecodeOperand(tmp, src1, opindex);
			(tempElem.getOperand2()).push_back(tmp);

			i++;
		}
	}
}


void processCallOp(char *instr, int opindex, OperationLatchData & tempElem)
{
	
	if (isCall(opindex) != _YES_)
	{
		// error condition.
		cout << "Expected Call Op in simulator, got something else" << endl;
		assert(0);
	}	

	// Not needed currently...
	//if ((specCallType = ((CallRetOp *)currOper)->getSpecialCallType()) != _INVALID_)

	
	char opcode[_STRING_SIZE_]="", dstlist[_STRING_SIZE_]= "", srclist[_STRING_SIZE_]= "";
	int count = 0;

	sscanf(instr,"%s%s%s",opcode,dstlist,srclist);

	if (countOperInList(srclist) > 0) {
		char src[_STRING_SIZE_];
		int tmpOpcodeIndex = 0;
		
		OpInList(srclist,1,src);
	
		if (!strcmp(src,"DEBUG_PRINT")) {
			char instr[_STRING_SIZE_];

			tmpOpcodeIndex = getOpcodeIndex("DEBUG_PRINT");

			OpInList(srclist,2,src);
			sprintf(instr,"DEBUG_PRINT () (%s)",src);
			processOp(instr, tmpOpcodeIndex, tempElem);
		}

		else if (isIDENTIFIER(src))	{
			cout << "Panic: It is not possible to currently simulate a call to a physical memory location" << endl;
			assert(0);
		}
	}
}


void processMemoryOp(char *instr, int opindex, OperationLatchData & tempElem)
{
	if (isLdStOp(opindex) != _YES_)
	{
		// error condition.
		cout << "Expected Memory Op in simulator, got something else" << endl;
		assert(0);
	}

	// if it is a load op, no special processing is needed.
	// call the normal processing function.

	if (isLoad(opindex))
	{
		processOp(instr, opindex, tempElem);
		return;
	}

	// else it is a store op.
	// for a store op, the destination arg contains the offset,
	// the source2 contains the address.
	// source1 contains the value that is to be stored.

	tempElem.setOpCode(opindex);
	char opcode[_STRING_SIZE_]="", dstlist[_STRING_SIZE_]= "", srclist[_STRING_SIZE_]= "";
	int count = 0;
	sscanf(instr,"%s%s%s",opcode,dstlist,srclist);

	// For store ops :
	// _SRC_1_ : source register
	//
	// _OFFSET_ : offset
	//
	// _SRC_3_ : base address
	//
	char src[_STRING_SIZE_];
	PredicateAddrDataUnion tmp;

	if (countOperInList(srclist) != 3) {
		// error condition.
		cout << "Panic: A store must always have 3 operands: SRC1,SRC2 and OFFSET (even if it is Zero)" << endl;
		assert(0);
	}

	int srcCount = countOperInList(srclist);
	int i = srcCount;

	while (i > 0)
	{
		OpInList(srclist,i,src);
		convertSrcToDecodeOperand(tmp, src, opindex);
		(tempElem.getOperand2()).push_back(tmp);

		i--;
	}
	
}


void processDataAndMemoryOp(char *instr, int opindex, OperationLatchData & tempElem)
{
	if (instr == NULL)
	{
		int opCode = globalOpCodeTable.getIndex("NOP");
		tempElem.setOpCode(opCode);
	}
	else if (isLdStOp(opindex))
	{
		processMemoryOp(instr, opindex, tempElem);
	}
	else
	{
		processOp(instr, opindex, tempElem);
	}
}

long getBranchAddress(char *instr)
{
	// execute branch operations
	// Assuming that the Target address always is the second operand of src list!!
	// Might have to be changed...

	char opcode[_STRING_SIZE_]="", dstlist[_STRING_SIZE_]= "", srclist[_STRING_SIZE_]= "";
	int count = 0;
	sscanf(instr,"%s%s%s",opcode,dstlist,srclist);

	char src[_STRING_SIZE_];
	int tempOpcode = getOpcodeIndex(opcode);

	if (isBranch(tempOpcode))
		OpInList(srclist,2,src);
	else if (isGotoOp(tempOpcode))
		OpInList(srclist,1,src);
	
	if (!isIDENTIFIER(src)) {
		cout << "getBranchAddress() - Address not found" << endl;
		printf(" SECOND OPERAND IN SOURCE LIST SHOULD HAVE BEEN A IDENTIFIER.\n");
		printf(" IDENTIFIER : %s , in INSTR : %s is not RECOGNISED \n", src, instr);
		assert(0);
		return 0;
	} else  {
		return GlobalLabels[(MyString) src];
	}
}

InstrPtr getFallThruAddress(BaseOperation *currOper)
{
	InstrPtr pcInstr = currOper->getInstr();
	InstrPtr nextPcInstr = ((NormalInstr *)pcInstr)->getDefaultTargetInstr();
	return nextPcInstr;
}

int isCompareOp(int opCode)
{
	int tempOpCode;

	/**** These are compare instructions for ELF ***/
	tempOpCode = globalOpCodeTable.getIndex("CMPW");
	if (tempOpCode == opCode) return 1;

	tempOpCode = globalOpCodeTable.getIndex("CMPWI");
	if (tempOpCode == opCode) return 1;

	return 0;
}


int isLdStOp(int opCode)
{
	MyString nameStr("LDST_EX");
	Unit * ldst = (Unit*)getComp(nameStr);

	OpCodeList & opList = ldst->getOpList();
	if (opList.hasOpCode(opCode) && !isNOP(opCode))
		return 1;
	else
		return 0;
}

int isControlOp(int opCode)
{
	MyString nameStr("BR_EX");
	Unit * branch = (Unit*)getComp(nameStr);

	OpCodeList & opList = branch->getOpList();
	// NOP is not a valid control instruction.
	if (opList.hasOpCode(opCode) && !isNOP(opCode) )
		return 1;
	else
		return 0;
}


int isStore(int opCode)
{
	int tempOpCode = globalOpCodeTable.getIndex("IVSTORE");
	if (tempOpCode == opCode) return 1;

	if (opCode == globalOpCodeTable.getIndex("sw")) return 1;
	if (opCode == globalOpCodeTable.getIndex("QIVSTOREU")) return 1;
	if (opCode == globalOpCodeTable.getIndex("sbu")) return 1;
	if (opCode == globalOpCodeTable.getIndex("QIVSTORE")) return 1;
	if (opCode == globalOpCodeTable.getIndex("sb")) return 1;
	if (opCode == globalOpCodeTable.getIndex("HIVSTOREU")) return 1;
	if (opCode == globalOpCodeTable.getIndex("shu")) return 1;
	if (opCode == globalOpCodeTable.getIndex("HIVSTORE")) return 1;
	if (opCode == globalOpCodeTable.getIndex("sh")) return 1;
	if (opCode == globalOpCodeTable.getIndex("FVSTORE")) return 1;
	if (opCode == globalOpCodeTable.getIndex("s_s")) return 1;
	if (opCode == globalOpCodeTable.getIndex("DVSTORE")) return 1;
	if (opCode == globalOpCodeTable.getIndex("s_d")) return 1;

	return 0;
}

int isLoad(int opCode)
{
	int tempOpCode = globalOpCodeTable.getIndex("IVLOAD");
	if (tempOpCode == opCode) return 1;

	if (opCode == globalOpCodeTable.getIndex("lw")) return 1;
	if (opCode == globalOpCodeTable.getIndex("QIVLOADU")) return 1;
	if (opCode == globalOpCodeTable.getIndex("lbu")) return 1;
	if (opCode == globalOpCodeTable.getIndex("QIVLOAD")) return 1;
	if (opCode == globalOpCodeTable.getIndex("lb")) return 1;
	if (opCode == globalOpCodeTable.getIndex("HIVLOADU")) return 1;
	if (opCode == globalOpCodeTable.getIndex("lhu")) return 1;
	if (opCode == globalOpCodeTable.getIndex("HIVLOAD")) return 1;
	if (opCode == globalOpCodeTable.getIndex("lh")) return 1;
	if (opCode == globalOpCodeTable.getIndex("FVLOAD")) return 1;
	if (opCode == globalOpCodeTable.getIndex("l_s")) return 1;
	if (opCode == globalOpCodeTable.getIndex("DVLOAD")) return 1;
	if (opCode == globalOpCodeTable.getIndex("l_d")) return 1;

	return 0;
}

int isCall(int opCode)
{
	if (isOpCode(opCode, "CALL")) return 1;
	if (isOpCode(opCode, "SYSCALL")) return 1;
	if (isOpCode(opCode, "DYNCALL")) return 1;
	if (isOpCode(opCode, "jal")) return 1;

	return 0;
}


int isBranch(int opCode)
{
	if (isOpCode(opCode, "IF")) return 1;

	if (isOpCode(opCode, "bnez")) return 1;

	return 0;
}


int getSystemCallOpCode()
{
	return (globalOpCodeTable.getIndex("SYSCALL"));
}

int getDynamicCallOpCode()
{
	return (globalOpCodeTable.getIndex("DYNCALL"));
}

int isDynamicCall(int opCode)
{
	return (isOpCode(opCode, "DYNCALL"));
}

int isRet(int opCode)
{
	if (isOpCode(opCode, "RETURN") == _YES_) 
		return _YES_;
	if (isOpCode(opCode, "jr") == _YES_) 
		return _YES_;
	return _NO_;
}

int isCallRet(int opCode)
{
	return ((isCall(opCode)) || (isRet(opCode)));
}

int isDebugPrint(int opCode)
{
	int tempOpCode = globalOpCodeTable.getIndex("DEBUG_PRINT");
	if (tempOpCode == opCode) return 1;
	return 0;
}

int isNOP(int opCode)
{ 
	// return (isOpCode(opCode, "NOP"));
	if( isOpCode(opCode, "NOP") || isOpCode(opCode, "nop") )
		return 1;
	return 0;
}

int getGotoOpIndex()
{
	return (globalOpCodeTable.getIndex("j"));
}

int isGotoOp(int opCode)
{
	if ( isOpCode(opCode, "GOTO") || isOpCode(opCode, "j") )
		return 1;
	return 0;
}


int isPRINTF(int opCode)
{
	return (isOpCode(opCode, "PRINTF"));
}

int isEXP(int opCode)
{
	return (isOpCode(opCode, "EXP"));
}

int isSQRT(int opCode)
{
	return (isOpCode(opCode, "SQRT"));
}


int isOperation(int opCode, char* op)
{
	return (isOpCode(opCode, op));
}

int isSpecialOpCode(int opCode)
{
	return isDebugPrint(opCode);
}

int isLibFuncCall(int opCode)
{
	if ((isEXP(opCode)) || (isSQRT(opCode)))
		return 1;
	else 
		return 0;
}

inline int isDebugPrint(MyString & s)
{
	return ((s == "_DEBUG_PRINT") ? 1 : 0);
}

inline int isPRINTF(MyString & s)
{
	return ((s == "_PRINTF") ? 1 : 0);
}


int isSpecialOpCode(MyString & s)
{
	if (isDebugPrint(s)) return 1;

	return 0;
}

int getSpecialOpCode(MyString & s)
{
	if (isDebugPrint(s)) return (globalOpCodeTable.getIndex("DEBUG_PRINT"));

	return -1;
}

 
void getStringFromSrcArg(AddrDataUnion & a, MyString & s)
{
	if (a.getType() == _VALTYPE_)
	{
		if ((a.getData())._valType == _INTERNAL_LABEL)
		{
			s = (a.getData())._as.label;
		}
	}
}

fstream *debug_print_file=NULL;

void openDebugFile(void){
	debug_print_file=new fstream("debug.output",ios::out);
	if(debug_print_file==NULL){
		cout << "Cannot open debug.output file..." << endl;
		abort();
	}
}

void closeDebugFile(void){
	debug_print_file->close();
}

//SS 2/28/01 Support predication
//void doSpecialStuff(TwoOpLatchData & elem)
void doSpecialStuff(OperationLatchData & elem)
{
	if (isDebugPrint(elem.getOpCode()))
	{
		cout << "#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#";
		cout << endl << "DEBUG_PRINT: ";
		// Operands 1 and 2 have been exchanged (from c6x) because now the first
		// operand is an address and the second is AddrDataUnion which contains the value to print
		
		//if (!((elem.getOperand1()).isEmpty()))
		//	(elem.getOperand1().print(cout));

		if ( (elem.getOperand1()).size() > 0  )
			((elem.getOperand1())[0]).print(cout);

		((elem.getOperand2())[0]).print(cout);
		((elem.getOperand2())[0]).print(*debug_print_file);
		(*debug_print_file) << endl;
		cout << endl;
		cout << "#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#";
		cout << endl;
	}
}

int getLatency(int opCode)
{
	int tempOpCode = globalOpCodeTable.getIndex("IMUL");
	if (tempOpCode == opCode) return 4;

	return 1;
}

extern int getSPAddress();
extern long int STACK_START_ADDRESS;
extern long int STACK_POINTER_REG_NUMBER;
extern Storage* GPRFile;
extern Storage* FPRFile;

void InitStackPointer(AddrOperand & origReg, AddrOperand & renamedReg)
{
	// Check if the original Register is stack pointer location. Then
	// initialize renamed Register with stack pointer value.

	if (STACK_POINTER_REG_NUMBER == origReg.getAddrOperand())
	{
		int newSpAddr = renamedReg.getAddrOperand();

		AddrOperand addr;
		addr.setAddrOperand(_REG_, newSpAddr);
		
		Val value(STACK_START_ADDRESS);
		GPRFile->initialize(value, addr);
	}
}

void LdStInsn :: print (ostream & out) 
{
	cout << "<IR:" << _ldstInsn.getIRData()._id << ">";; 
	cout << "State: " << _statebit;
	
	// Changed by SAH & SS on 03/03/01
	//
	// Make everything list based. So, iterate over the dest and src operand lists and print the values.
	//

	// PRINTING DESTS
	//
	Vector<AddrOperand>::iterator d1, d2;
	d1 = (_ldstInsn.getOperand1()).begin();
	d2 = (_ldstInsn.getOperand1()).end();

	while (d1 != d2)
	{
		AddrOperand &dest = *d1;
		if (dest.getAddrOperand() ==  _ADDRTYPE_)
			cout << " Dest: R" << dest.getAddrOperand();
		d1++;

	}

	
	// PRINTING SOURCES
	//
	Vector<PredicateAddrDataUnion> :: iterator i1,i2;

	i1 = (_ldstInsn.getOperand2()).begin;
	i2 = (_ldstInsn.getOperand2()).end;

	int i = 1; //<----- This is only a counter for printing out sources
	while (i1 != i2)
	{
		if ((*i1).getType() == _ADDRTYPE_)
			cout << " Src " << i << ": R" << (*i1).getAddr().getAddrOperand();
		else
			cout << " Src " << i << ": " << (*i1).getData()._as.integer;
		i1++;
		i++;
	}

	cout << " Opcode: " << _ldstInsn.getOpCode() << endl;
}


// Note: this function must vary for each architecture.
//
int isMove(BaseOperation *oper)
{
	int opCode = (oper->getOpCode()).index();

	if (isOpCode(opCode, "IASSIGN")) return 1;
	if (isOpCode(opCode, "FASSIGN")) return 1;
	if (isOpCode(opCode, "DASSIGN")) return 1;
	if (isOpCode(opCode, "ASSIGN")) return 1;
	if (isOpCode(opCode, "MFHI")) return 1;
	if (isOpCode(opCode, "MFLO")) return 1;

	//GG 01/02/02: These are not needed for ST100
	//if (isOpCode(opCode, "mflo")) return 1;
	//if (isOpCode(opCode, "mfhi")) return 1;
	if (isOpCode(opCode, "move")) return 1;
	//if (isOpCode(opCode, "mov_d")) return 1;
	//if (isOpCode(opCode, "mov_s")) return 1;
	//if (isOpCode(opCode, "mflo_r")) return 1;
	//if (isOpCode(opCode, "mfhi_r")) return 1;
	//if (isOpCode(opCode, "move_r")) return 1;

	return 0;
}

vector<MyString> readFromMem(long addr, int numfetches, int &done, int &instfetched)
{

	static int busy = 0;
	static int i=0;
	static int access_count=0;
	int dum = 0, lat;
	Val a;
	char tmpstr[100];
	MyString strname;
	static vector<MyString> instrns;
	vector<MyString> temp;
	static long prev_addr = 0;

	if (prev_addr != addr) {
		// Need to abort the access if the addr is changed.
		busy = 0;
	}

	prev_addr = addr;
	if (!busy) {
		instrns.clear();
		access_count++;
		icachePtr->read(addr, a, dum, lat);
		if (imemJunk)
		{
			printf("%d: fetching from addr: %d", access_count, addr);
			if (a._valType == _INTERNAL_LABEL)
				printf("\t%s", getStringValue(a));
			printf("\n");
		}
		if (!a.isEmpty()) {
			sprintf(tmpstr,"%ld =%s",addr,getStringValue(a));
			instrns.push_back((MyString) tmpstr);
		} else {
			done = 1;
			instfetched = 0;
			return temp;
		}

		busy = lat;

	// read from ICACHE till theres a miss.
	// This should be end of cache line. 
	// TODO: The ICACHE read has to do take care of this. 
	// (depends on the size of the bus from ICACHE to fetch unit)

		lat = 1;
		i = 1;
		while(i < numfetches) {

			icachePtr->read((addr+i*4), a, dum, lat);
			if ((a.isEmpty()) || (lat!=1)) break;

			// This is not a fresh read access.
			// reduce the read access count in ICACHE for these accesses
			icachePtr->setReadHits(icachePtr->getReadHits() - 1);
			icachePtr->setCacheAccesses(icachePtr->getCacheAccesses() - 1);

	//		printf("%ld : %s\n",(addr+i*4), getStringValue(a));
			sprintf(tmpstr,"%ld=%s",(addr+i*4),getStringValue(a));
			instrns.push_back((MyString) tmpstr);
			i++;
		}
	}

	if (busy == 1) {
		busy = 0;
		done = 1;
		instfetched = i;
		return instrns;
	} else if (busy > 1) {
		busy = busy - 1;
		done = 0;
		instfetched = 0;
		return temp;
	} else 
		return temp;
}

void writeToFetDecLatch(vector<MyString> a)
{

	InstrStr tempinstr;
	tempinstr.setInstrStr(a);
	
	InstStrLatchData tmpinstrlatdata;
	tmpinstrlatdata.setLatchData(tempinstr);
	
	MyString nameStr("FetDecLatch");
	Latch * FetDeclatch = (Latch*)getComp(nameStr);
	FetDeclatch->write(tmpinstrlatdata);

}

vector<MyString> readFromFetDecLatch(void)
{

	MyString nameStr("FetDecLatch");
	Latch * FetDeclatch = (Latch*)getComp(nameStr);
	InstStrLatchData tmpinstrlatdata;
	InstrStr tempinstr;

	FetDeclatch->read(tmpinstrlatdata);
	FetDeclatch->resetOld();

	tempinstr = tmpinstrlatdata.getLatchData();
	return (tempinstr.getStr());
	
}

long readFromPgmCounterLatch(void)
{
	
	MyString nameStr("pcLatch");
	Latch * PCLatch = (Latch*)getComp(nameStr);
	PCLatchData templatdata;
	PgmAddr temp;

	PCLatch->read(templatdata);
	temp = templatdata.getLatchData();
	long addr = temp.getAddr();
	return addr;

}

void writeToPgmCounterLatch(long addr)
{
	PgmAddr temp;
	temp.setPgmAddr(addr);

	PCLatchData templatdata;
	templatdata.setLatchData(temp);

	MyString nameStr("pcLatch");
	Latch * pcLatch = (Latch*)getComp(nameStr);
	pcLatch->write(templatdata);
	// This is just for testing ... comment it later..
	// PCLatch->doUpdate();
}

void splitAddrInstr(MyString InsStr, MyString &Inst, long & Addr) 
{
	long tmpaddr;
	char *tmpstr;

	char *tmpinstr;
	tmpinstr = InsStr.getStr();

	sscanf(tmpinstr,"%ld",&tmpaddr);
	Addr = tmpaddr;

	// instruction starts after the character '='
	tmpstr = strchr(tmpinstr,'=');
	MyString temp(tmpstr+1);
	Inst = temp;
}

long getTargetAddress(MyString InsStr) 
{
	MyString Inst;
	long tgtAddr;
	char *tmpInstr;

	splitAddrInstr(InsStr, Inst, tgtAddr);
	tmpInstr = Inst.getStr();
	tgtAddr = getBranchAddress(tmpInstr);

	return tgtAddr;
}

int isBranchInstr(MyString InsStr)
{
	MyString Inst;
	long tgtAddr;
	char tmpOpcode[40];
	char *tmpInstr;

	splitAddrInstr(InsStr, Inst, tgtAddr);
	tmpInstr = Inst.getStr();
	
	// Get the OpCode in the instruction and its corresponding index..
	sscanf(tmpInstr,"%s",tmpOpcode);
	int tmpOpcodeIndex = getOpcodeIndex(tmpOpcode);
	return isBranch(tmpOpcodeIndex);
}

int isGotoInstr(MyString InsStr)
{
	MyString Inst;
	long tgtAddr;
	char tmpOpcode[40];
	char *tmpInstr;

	splitAddrInstr(InsStr, Inst, tgtAddr);
	tmpInstr = Inst.getStr();
	
	// Get the OpCode in the instruction and its corresponding index..
	sscanf(tmpInstr,"%s",tmpOpcode);
	int tmpOpcodeIndex = getOpcodeIndex(tmpOpcode);
	return isGotoOp(tmpOpcodeIndex);
}


int convertReg(int & a, BaseArgument *argPtr)
{
   if (argPtr->isRegisterArg())
   {
	   int uniqID = 0;
	   BaseArgProperty *ssaProp;
	   RAProperty *raProp;

	   if ((raProp = (RAProperty *) argPtr->getPtrToProperty(_RA_)) != NULL)
	   {
		   //int regFile = ((RealRegArg *) argPtr)->regFileIndex();
		   //int rfIndex = globalRegFileList->getBaseID(regFile);
		   uniqID = raProp->getRegID();
		   //uniqID = rfIndex + regId;
	   }
 	   else if ((ssaProp = argPtr->getPtrToProperty(_SSA_)) != NULL) 
	   {
		   uniqID = globalSymbolTable->getID(argPtr, ssaProp);
	   }
	   else  // ssaProperty is not present.
	   {
//		   cout << "Warning: The register argument does not have any SSA property" << endl;
		   if (!doneSSA)
		   {
			   // i.e. SSA has not been done yet.
			   if (argPtr->isRealRegArg())
			   {
				   uniqID = ((RegisterArg*)argPtr)->getIDWithoutSSA();
			   }
		   }
		   else
		   {
			   // SSA property is NULL. This means that the register must be a newly created temporary.
			   uniqID = globalSymbolTable->getID(argPtr);
		   }
	   }
	   a = uniqID;

	 return 1;
   }
   return 0;
}

int convertReg(AddrOperand & a, BaseArgument *argPtr)
{
	int uniqID;
	int retVal = convertReg(uniqID, argPtr);
	
	if (retVal == 0) return retVal;
	
	a.setAddrOperand(_REG_, uniqID); 
	
	int regfile = getRegFileType(uniqID);

	switch(regfile)
	{
		case _INT_REG_FILE: a.setFieldType(_ADDR_INT_); break;
		case _FLT_REG_FILE: a.setFieldType(_ADDR_FLT_); break;
		case _MISC_REG_FILE: a.setFieldType(_ADDR_MISC_); break;
		default:;
	}
	
	return retVal;
}


void convertDestToDecodeOperand(AddrOperand & a, BaseArgument *argPtr)
{
   if (_NULL_ == argPtr)
      {
	a.setType(_NONE_);
      }
   else if (convertReg(a, argPtr))
	{
	}
   else if (argPtr->isIConstArg())
      {
	      a.setAddrOperand(_DIRECTMEMORY_, ((IConstArg *)argPtr)->value());
      }
   else if (argPtr->isFConstArg())
      {
	// Error condition.
	cout << "Panic: Expected address, got FConst" << endl;
	assert(0);
      }
}

//SS 12/06/01 Predication support	 
//void convertSrcToDecodeOperand(AddrDataUnion & a, BaseArgument *argPtr)
void convertSrcToDecodeOperand(PredicateAddrDataUnion & a, BaseArgument *argPtr)
{
   AddrOperand bA;
   if (_NULL_ == argPtr)
      {
	a.setType(_NOTYPE_);
      }
   else if (convertReg(bA, argPtr))
	{
		a.setAddr(bA);
		//       cout << "  Src: " << *((RegisterArg*)argPtr) << "  " <<  uniqID << endl; 
	}
   else if (argPtr->isIConstArg())
      {
	Val b(((IConstArg*)argPtr)->value());
	a.setData(b);
      }
   else if (argPtr->isFConstArg())
      {
	float f = ((FConstArg*)argPtr)->value();
	Val b(f);
	a.setData(b);
      }
}

void convertSrcToDecodeOperand(PredicateAddrDataUnion & a, char *oper, int tempOpcode)
{
   AddrOperand bA;

   if (!strcmp("",oper)) {
	   a.setType(_NOTYPE_);
   }

   // Check if this is a identifier...
   else if (isIDENTIFIER(oper)) {
	   Val b((int)GlobalLabels[oper]);
	   a.setData(b);
   }
   else if (convertReg(bA, oper, 0))  // Send a dummy parameter '0' for source operand
   {
	   a.setAddr(bA);
   }
   else if (isIConstArg(oper)) {
	   Val b(atoi(oper));
	   a.setData(b);
   }
   else if (isFConstArg(oper)) {
	   if (isDouble(tempOpcode))
	   {
		   char ** endPtr;
		   double d = strtod(oper, endPtr);

		   Val b(d);	   
		   a.setData(b);
	   }
	   else
	   {
		   Val b((float)atof(oper));
		   a.setData(b);
	   }
   }
   else {
	   printf("Couldnot intrepret the argument: %s",oper);
	   assert(0);
   }

}

void convertDestToDecodeOperand(AddrOperand & a, char *oper, int tempOpcode)
{
	if (oper == NULL) {
		a.setType(_NONE_);
	}
    else if (convertReg(a, oper, tempOpcode)) {
	}
	else if (isIConstArg(oper)) {
		a.setAddrOperand(_DIRECTMEMORY_, atoi(oper));
    }
	else if (isFConstArg(oper)) {
	// Error condition.
		cout << "Panic: Expected address, got FConst" << endl;
		assert(0);
    }
}

int convertReg(AddrOperand & a, char *oper, int tempOpcode)
{
	int uniqID;
	int retVal = isRegisterArg(uniqID, oper);
	
	if (retVal == -1) return 0;
	
	a.setAddrOperand(_REG_, uniqID); 
	
	//int regfile = getRegFileType(uniqID);

	int regfile = -1;
	char reg = oper[1];

	if (reg == 'f')
		regfile = _FLT_REG_FILE;
	else
		regfile = _INT_REG_FILE;
		
	switch(regfile)
	{
		case _INT_REG_FILE: a.setFieldType(_ADDR_INT_); break;

		case _FLT_REG_FILE: 
			if (isDouble(tempOpcode))
				a.setFieldType(_ADDR_DBL_);
			else
				a.setFieldType(_ADDR_FLT_); 
			break;

		case _MISC_REG_FILE: a.setFieldType(_ADDR_MISC_); break;

		default:;
	}

	return 1;
}


int isIDENTIFIER(char *oper) {
	map<MyString, long>::iterator i1 = GlobalLabels.find(MyString (oper));

	if (i1 == GlobalLabels.end()) return 0;
	else return 1;
}

int isRegisterArg(int &UniqID, char *oper) {
	if (oper[0]!='$') return -1;
	else if (oper[1] == 'f') {
		UniqID = atoi((oper+2));
		return 1;
	}
	else {
		UniqID = atoi((oper+1));
		return 1;
	}
}

int isIConstArg(char *oper) {
	int isINT = 1,i=0;

	// Doesnt take hexadecimal representations now.
	if (oper[i] == '0' && (oper[i+1] == 'x' || oper[i+1] == 'X')) {
		printf("WARNING: hexadecimal representation not currently supported...\n");
		assert(0);
	}

	// checking for negative number
	if (oper[i] == '-') i++;
	while(oper[i] != '\0') {
		if (!isdigit(oper[i])) isINT = 0;
		i++;
	}
	return isINT;
}

int isFConstArg(char *oper) {
	int isFLOAT = 1, i=0;
	
	// Different supported representations:
	// -?{DIGIT}+[.]{DIGIT}+e[-+]?{DIGIT}+							
	// -?{DIGIT}+e[-+]?{DIGIT}+	
	// -?{DIGIT}+[.]{DIGIT}+	

	if (oper[i] == '-') i++;

	while(isdigit(oper[i])) 
		i++;
	
	if (oper[i] == '.') {
		i++;
		while(isdigit(oper[i])) i++;
		if (oper[i] == '\0') return isFLOAT;
	}
	
	if (oper[i] == 'e') {
		i ++;
		// Added by Sudeep 05/20/03 -> earlier there was no check for [+-] after e
		// Bug fix
		if ((oper[i] == '-')||(oper[i] == '+')) i++;
		while (isdigit(oper[i])) i++;
		if (oper[i] == '\0') return isFLOAT;
	}

	return 0;
}

int isDouble(int opCode)
{
	if (isOpCode(opCode, "add_d")) return 1;
	if (isOpCode(opCode, "sub_d")) return 1;
	if (isOpCode(opCode, "mul_d")) return 1;
	if (isOpCode(opCode, "div_d")) return 1;
	if (isOpCode(opCode, "li_d")) return 1;
	if (isOpCode(opCode, "mov_d")) return 1;
	if (isOpCode(opCode, "cvt_d_w")) return 1;
	if (isOpCode(opCode, "neg_d")) return 1;
	if (isOpCode(opCode, "c_lt_d")) return 1;
	if (isOpCode(opCode, "c_le_d")) return 1;
	if (isOpCode(opCode, "c_gt_d")) return 1;
	if (isOpCode(opCode, "c_ge_d")) return 1;
	if (isOpCode(opCode, "c_eq_d")) return 1;
	if (isOpCode(opCode, "l_d")) return 1;
	if (isOpCode(opCode, "s_d")) return 1;
	if (isOpCode(opCode, "cvt_d_s")) return 1;

	return 0;
}

// This function returns the i-th Operand in List.
void OpInList(char *list, int i, char *oper)
{
	int j=0,k=0,count=0;

	// first char in list is always a '('
	if (list[0] != '(') {
		printf("List doesnt start with '(' \n");
		assert(0);
	}
	k++;

	while(count < i) {
		j=0;
		while (list[k] != ',' && list[k] != ')') {
			oper[j++]=list[k++];
		}
		oper[j] = '\0';
		k++;
		count++;
		if ((list[k] == ')') && (count != i) ) {
			printf("Couldn't find operand --%d-- in oper-list: %s \n",i,list);
			assert(0);
		}
	}
}

int countOperInList(char *oplist) 
{
	int count = 0;
	if (!strcmp(oplist,"()")) {
		// Nothing in list
		return 0;
	}

	int i=0;
	while (oplist[i] != ')') {
		if (oplist[i] == ',') count++;
		i++;
	}
	return count+1;
}


void initializeComplexOpList (char * fname)
{
	FILE *fp;

	if ((fp = fopen(fname, "r")) == NULL) {
		printf("File: %s cannot be opened\n", fname);
		assert(0);
	}
	else
	{
		bool complexOpcode = true;
		bool simpleOp = true;

		char line[80];
		ComplexInsn complexInst;

		while ((!feof(fp)))
		{
			fgets(line,80,fp);

			if (strcmp(line, "END\n") == 0) break;
			if (strcmp(line, "BEGIN\n") == 0) continue;

			if (complexOpcode)
			{
				char opcode[20];
				sscanf(line,"(%s",opcode);
				
				strcpy(complexInst._opcode, opcode);
				complexOpcode = false;
			}
			else if (strcmp(line, ")\n") == 0)
			{
				ComplexOpList.push_back(complexInst);

				// New complex operation starts
				complexOpcode = true;
				complexInst._simpleInsns.clear();
			}
			else
			{
				// Simple Opcodes
				char opcode[20];
				int dest;
				vector<int> src;
				bool dst = false;

				char * ptr = line;

				while ((*ptr != ')') || (*ptr != '\n'))
				{
					if (*ptr == '\t')
					{
						ptr++;
						continue;
					}

					char *oPtr = strchr(ptr, '\t');
					char *tPtr = ptr;

					if (oPtr != NULL)
					{
						tPtr = oPtr + 1;
						*oPtr = '\0';
						
						
						if (*ptr == '(')
						{
							ptr++;
							dst = true;
							strcpy(opcode, ptr);
						}
						else if (dst)
						{
							dest = atoi(ptr);
							dst = false;
						}
						else
						{
							int s = atoi(ptr);
							src.push_back(s);
						}
						
						ptr = tPtr;
					}
					else
						break;
				}

				SimpleInsn simpleInst(opcode, dest, src);
				complexInst._simpleInsns.push_back(simpleInst);
			}
		}
	}
}

bool isComplexOp(int opCode, ComplexInsn & complexInst)
{
	vector<ComplexInsn>::iterator i1, i2;
	i1 = ComplexOpList.begin();
	i2 = ComplexOpList.end();

	while (i1 != i2)
	{
		int op = globalOpCodeTable.getIndex((*i1)._opcode);
		if (op == opCode) 
		{
			complexInst = (*i1);
			return true;
		}
		i1++;
	}

	return false;
}
