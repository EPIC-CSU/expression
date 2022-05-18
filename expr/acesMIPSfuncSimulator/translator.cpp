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
/*: translator.cpp
 *
 * File:  		translator.cpp
 * Created:		Sun May 21, 2000
 * Author:		Radu Cornea
 * Email:		radu@ics.uci.edu, ilp@ics.uci.edu
 *
 * Functions to translate Express IR to FuncSim structures
 */ 

#include "machine.h"
#include "call.h"
#include "syscall.h"
#include "translator.h"
#include "MemoryModule.h"

#include "FuncSim.h"
#include "BaseInstruction.h"
#include "IConstArg.h"
#include "FConstArg.h"
#include "RAProperty.h"
#include "NormalOp.h"
#include "CallOp.h"
#include "IfOp.h"
#include "Program.h"
#include "Routine.h"
#include "AddrData.h"
#include "CallNameArgProperty.h"
#include "DerivedClassIncludes.h"
#include "DerivedLatchData.h"
#include "ExecSemSupportFuncs.h"
#include "ArchClassIncludes.h"
#include "Dram.h"


/* variable and functions defined in Express */
extern OpCodeTable globalOpCodeTable;
extern int isSystemCall(BaseOperation *op);
extern int isDynamicCall(BaseOperation *op);
extern int getSystemCallOpCode();
extern int getDynamicCallOpCode();
extern int getRAddress(int symval);
extern int getSPAddress();
extern int getSystemCallNameAsInt(char *name);
extern Vector<MyString> systemCallsIndex;
typedef Vector<AddrOperand> RegAddrList;
Map<Routine *, RegAddrList> routSaveRegList;

int			instr_no;

/* RF index for SP */
int sp_reg;

/* Express to FuncSim mappings */
map<int, md_opcode> exprToFuncSimOpMap;
map<int, md_syscall_code> exprToFuncSimSyscallCodeMap;
map<BaseInstruction*, int> instrPtrToIndexMap;
map<long, int> instrAddrToIndexMap;

/* build Express to FuncSim opcode mapping */
void createOpCodeMap() {
	int exprIndex;

#define DEFINST(OP, NAME, FLAGS)							\
	exprIndex = globalOpCodeTable.getIndex(NAME);			\
	if (exprToFuncSimOpMap.find(exprIndex) == exprToFuncSimOpMap.end())		\
		exprToFuncSimOpMap[exprIndex] = (OP);
#include "machine.def"
#undef DEFINST
}

/* build Express to FuncSim syscall code mapping */
void createSyscallCodeMap() {
	int exprCode;

#define DEFSYSCALL(SYSCALL_CODE, SYSCALL_NAME)				\
	if ((exprCode = getSystemCallNameAsInt(SYSCALL_NAME)) != _INVALID_)	\
		exprToFuncSimSyscallCodeMap[exprCode] = (SYSCALL_CODE);
#include "syscall.def"
#undef DEFSYSCALL
}


/* build the mapping from call to saved registers */
void createSavedRegsMap(Program *prog)
{
	MyLinkedListIterator<Routine *> *rIter = prog->routineListIteratorForw();

	while (rIter->hasMoreElements()) {

		Routine *rout = rIter->currentElement();

		RegAddrList *rList;

		if (routSaveRegList.find(rout) != routSaveRegList.end()) {
			rList = &routSaveRegList[rout];
		} else {
			rList = NULL;
		}

		int index = instrPtrToIndexMap[rout->getFirstInstr()];
		
		md_saved_regs_t *saved = (md_saved_regs_t *) malloc(sizeof(md_saved_regs_t));

		if (rList) {
			saved->saved_reg_no = rList->size() + 2;
		} else {
			saved->saved_reg_no = 2;
		}

		saved->saved_regs = (int *) malloc(saved->saved_reg_no * sizeof(int) + 2);
		if (!saved->saved_regs) {
			printf("Not enough memory\n");
			exit(1);
		}

		int i = 0;

		if (rList) {
			for (i = 0; i < rList->size(); i++) {
				AddrOperand& addrOp = (*rList)[i];
				saved->saved_regs[i] = addrOp.getAddrOperand();
			}
		}

		/* R4 will hold first argument, so it should be saved */
		param_regs[0] = getRAddress(4);
		saved->saved_regs[i++] = param_regs[0];

		/* R5 will hold first argument, so it should be saved */
		param_regs[1] = getRAddress(5);
		saved->saved_regs[i++] = param_regs[1];

		indexToSavedRegsMap[index] = saved;

		rIter->nextElement();
	}
}


/* initialize memory */
#include "defs_node.h"

/* extern declarations */
extern vector<defs_node *> decl_list;
typedef vector<defs_node *>::iterator Defs_node_iter;
extern defs_node *get_ascii(defs_node *node);
extern Vector<int> *get_word_list(defs_node *node);
extern Vector<short> *get_half_list(defs_node *node);
extern Vector<char> *get_byte_list(defs_node *node);
extern void read_defs_file(Vector<MyString> & name_list);
extern long get_assigned_size(char *);
extern long get_memory_assig(char *name);
extern Vector<MyString> globalDefsFileNamesList;  // global declarations
extern long int MEM_CRT;

/* memory initialization function */
void my_initialize_memory(mem_t mem)
{
	/* First go thru the EXPRESS memory initialize datastructure.
	 *
	 * For each location, store the initialization content.
	 */

	Defs_node_iter i;

	defs_node *currNode, *asciiVal;
	long size;
	char *symbName;

	char *asciiStr;

	// long memAddr;
	AddrOperand a;

	Val a1;
	Vector<int> *wordList;
	Vector<int>::iterator wI;
	Vector<short> *halfList;
	Vector<short>::iterator hI;
	Vector<char> *byteList;
	Vector<char>::iterator bI;

	read_defs_file(globalDefsFileNamesList);

	// Assigning memories to the global variables
	for (i = decl_list.begin(); i != decl_list.end(); i++)
	{
		if ((currNode = (*i)) != NULL) /* i.e. is a valid node. */
		{
			symbName = currNode->Name();
			if ((asciiVal = get_ascii(currNode)) != NULL)
			{	/* i.e. the decl has an ascii value that needs to be initialized.
				 *
				 * First get the memory location that needs to be initialized.
				 *
				 * Then, get the initialization string.
				 */

				// memAddr = get_memory_assig(currNode->Name());
				
				size = get_assigned_size(symbName);
				asciiStr = asciiVal->Name();
				

				sprintf((char *) &(mem[MEM_CRT]), "%s", asciiStr);
				// increment the MEM_CRT by the size of this global variable
				MEM_CRT = MEM_CRT + size;

				/* Now, we have the address and the initialization string.
				 *
				 * Create the addrOperand and the Val objects inorder to write to memory.
                 *
				 * a.setAddrOperand(_DIRECTMEMORY_, memAddr);
				 */
			}
			else if ((wordList = get_word_list(currNode)) != NULL)
			{
				/* Iterate over wordlist, and initialize memory. */

				// memAddr = get_memory_assig(currNode->Name());

				for (wI = wordList->begin(); wI != wordList->end(); wI++)
				{
					MEM_WRITE_W(MEM_CRT, *wI);

					MEM_CRT += _WORD_SIZE_;
				}
				delete wordList;
			}
			else if ((halfList = get_half_list(currNode)) != NULL)
			{
				/* Iterate over halflist, and initialize memory.*/
				
				// memAddr = get_memory_assig(currNode->Name());
				for (hI = halfList->begin(); hI != halfList->end(); hI++)
				{
					MEM_WRITE_H(MEM_CRT, *hI);

					MEM_CRT += _HALF_SIZE_;
				}
				delete halfList;
			}
			else if ((byteList = get_byte_list(currNode)) != NULL)
			{
				/* Iterate over bytelist, and initialize memory.*/
				
				// memAddr = get_memory_assig(currNode->Name());
				for (bI = byteList->begin(); bI != byteList->end(); bI++)
				{
					MEM_WRITE_B(MEM_CRT, *bI);

					MEM_CRT += _BYTE_SIZE_;
				}
				delete byteList;
			}
		}
	}
}


void my_readCommandLineArgs(MyString & fName)
{
	// Note: ArgC is set ot 1 becos we currently assume that the 
	// first argV parameter (i.e. the name of the executable)
	// is not read in the command line args.
	//
	// If this condition were to change, argC should be set to 0
	//
	int argC;
	Vector<MyString> argV;
	int tempArgVStrLoc = FS_ARGV_STRING_START_LOCATION;

	argC = 1;
	argV.clear();

	char line[100];
	if (!fName.isEmpty()) 
	{
		FilePointer fp;
		
		try
		{
			fp.open(fName.getStr(), "r");
		}
		catch (CouldNotOpenFileException e)
		{
			cout << e;
			assert(0);
		}
		if (fgets(line, 100, fp) == NULL)
		{
			// error condition.
			//
			cout << "Warning: CommandLineArgument File was empty" << endl;
			fp.close();
			return;
		}

		MyString buf;
		buf = strtok(line, " \n");

		while (!buf.isEmpty())
		{
			argC++;
			argV.push_back(buf);

			buf = strtok(NULL, " \n");
		}
	}
	//initStorageWithCmdLineArgs();

	/*Argc is to be stored in R4 (either R4 if no SSA or R4.0 if SSA)*/
	int r4addr = getRAddress(FS_argCRegNum);
	SET_REG_I(r4addr, argC);


// Argv is a pointer to an array of pointers to chars.
//
// Store the ** pointer in R5. R5 points to the start of the array in memory.
// The start location and each subsequent memory location contains the location
// of the argv string.
//
// Note: Argv[0] is supposed to be the name of the executable itself (which we do not store).
//
// So, altho ARGV_START_LOCATION may be x, the first parameter is x + POINTER_INCREMENT
//
	AddrOperand a;

	int r5addr = getRAddress(FS_argVRegNum);
	SET_REG_A(r5addr, FS_ARGV_START_LOCATION);

	Vector<MyString>::iterator i;

	int currLoc = FS_ARGV_START_LOCATION + FS_POINTER_INCREMENT;
	for (i = argV.begin(); i != argV.end(); i++)
	{
		/* copy the arg string */
		strcpy(MEM_POINTER_C(tempArgVStrLoc), (*i).getStr());

		/* write the pointer to arg */
		MEM_WRITE_W(currLoc, tempArgVStrLoc);

		tempArgVStrLoc += ((*i).getStrLen() + 1);

		currLoc += FS_POINTER_INCREMENT;
	}
}



/* translate an argument from express to FuncSim */
void translateArg(BaseArgument *arg, md_arg_t *simArg)
{
	if (arg->isIConstArg()) {
		simArg->ival = ((IConstArg*)arg)->value();
	} else if (arg->isFConstArg()) {
		simArg->dval = ((FConstArg*)arg)->value();
	} else if (arg->isRealRegArg()) {
		
		BaseArgument *argPtr = arg;

		/* taken from ExpressExecSemSupportFuncs.cpp : convertReg() */
		int uniqID = 0;
		BaseArgProperty *ssaProp;
		RAProperty *raProp;
		
		if ((raProp = (RAProperty *) argPtr->getPtrToProperty(_RA_)) != NULL)
		{
			uniqID = raProp->getRegID();
			//RegisterFile& regFile = globalRegFileList->getRegFile(uniqID);

			// the real reg id is the offset from the regfile id
			//uniqID -= regFile.getID();
		}
		else if ((ssaProp = argPtr->getPtrToProperty(_SSA_)) != NULL) 
		{
			uniqID = globalSymbolTable->getID(argPtr, ssaProp);
		}
		else  /* ssaProperty is not present. */
		{
			if (!doneSSA)
			{
				/* i.e. SSA has not been done yet. */
				if (argPtr->isRealRegArg())
				{
					uniqID = ((RegisterArg*)argPtr)->getIDWithoutSSA();
				}
			}
			else
			{
				/* SSA property is NULL. This means that the register must be a newly created temporary. */
				uniqID = globalSymbolTable->getID(argPtr);
			}	
		}
		/* ** */
			
		simArg->reg = uniqID;
	} else if (arg->isLabelArg()) {
		CallNameArgProperty *cnp;
		if ((cnp = (CallNameArgProperty *)(arg->getPtrToProperty(_CALLNAME_))) != NULL) {
			
			int i = cnp->justGetIndex();
			
			if (exprToFuncSimSyscallCodeMap.find(i) != exprToFuncSimSyscallCodeMap.end())
				simArg->targ = exprToFuncSimSyscallCodeMap[i];
			else
				simArg->targ = SYS_NA;
		} else {
			/* TODO?! */
			MyString s(((LabelArg*) arg)->lName());
			int i = getSystemCallNameAsInt(s.getStr());
			
			if (exprToFuncSimSyscallCodeMap.find(i) != exprToFuncSimSyscallCodeMap.end())
				simArg->targ = exprToFuncSimSyscallCodeMap[i];
			else
				simArg->targ = SYS_NA;
		}
	} else {
		printf("Wrong argument\n");
	}
}

/* translates a data operation from Express to FuncSim */
void translateDataOp(BaseOperation *op, md_oper_t *simOper)
{
	BaseArgument *arg;

	int exprOpCode = ((NormalOp *) op)->getOpCodeIndex();
	simOper->opcode = exprToFuncSimOpMap[exprOpCode];
	simOper->dest.ival = simOper->src1.ival = simOper->src2.ival = simOper->flags = 0;

	if( simOper->opcode == 0 ) 
	{
		((NormalOp *) op)->shortPrint(cout);
		ASSERT_TRACE(0, "Opcode not defined in machine.def");
	}

	if (op == NULL) {
		simOper->opcode = NOP;
	} else if (op->isComputeOp() || op->isMemoryOp()) {

		/* destination operand */
		if (op->isMemoryOp() && ((MemoryOp *)op)->isStore())
		{
		//	arg = ((MemoryOp *) op)->getOffset();
		}
		else
		{
			arg = op->ptrToOperand(_DEST_, _DEST_LIST_);
			if (arg)
				translateArg(arg, &simOper->dest);
		}


		/* source 1 operand */
		arg = op->ptrToOperand(_SOURCE_1_, _SRC_LIST_);
		if (arg)
			translateArg(arg, &simOper->src1);
		if (arg && arg->isImmediateArg()) {
			simOper->flags |= OPFLAG_IMM1;
		}

		/* source 2 operand */
		arg = op->ptrToOperand(_SOURCE_2_, _SRC_LIST_);
		if (arg) 
			translateArg(arg, &simOper->src2);
		if (arg && arg->isImmediateArg()) {
			simOper->flags |= OPFLAG_IMM2;
		}

		// This is specifically for STORE operations 
		// 
		/* source 3 operand */
		if (op->isMemoryOp() && ((MemoryOp *)op)->isStore())
		{
			// OFFSET => SRC2
			arg = ((MemoryOp *) op)->getOffset();
			arg = op->ptrToOperand(_SOURCE_3_, _SRC_LIST_);
			if (arg) 
				translateArg(arg, &simOper->src3);
		}



	} else {
		printf("Wrong operation\n");
	}
}

/* translate a control operation from Express to FuncSim */
void translateControlOp(BaseOperation *op, BaseInstruction *targetInstr, md_oper_t *simOper)
{
	BaseArgument *arg;

	int exprOpCode = ((NormalOp *) op)->getOpCodeIndex();
	simOper->opcode = exprToFuncSimOpMap[exprOpCode];
	simOper->dest.ival = simOper->src1.ival = simOper->src2.ival = simOper->flags = 0;
	
	if( simOper->opcode == 0 ) 
	{
		((NormalOp *) op)->shortPrint(cout);
		ASSERT_TRACE(0, "Opcode not defined in machine.def");
	}

	if (op == NULL) {
		simOper->opcode = NOP;

	} 
	else if (op->isCallOp()) 
	{
		/* destination address for result */
		if (op->hasOperand(_DEST_, _DEST_LIST_)) {
			arg = op->ptrToOperand(_DEST_, _DEST_LIST_);
			translateArg(arg, &simOper->dest);
		}
		
		/* call target */
		if (op->hasOperand(_SOURCE_1_, _SRC_LIST_))
		{
			arg = op->ptrToOperand(_SOURCE_1_, _SRC_LIST_);
			translateArg(arg, &simOper->src1);
		}

		/* parameter list */
		int i;
		ArgListIter iT;
		ArgList & srcList = ((CallRetOp *)op)->getParmList();

		if ((simOper->src2.plist = (md_plist_t *) malloc(sizeof(md_plist_t))) == NULL)
			printf("Not enough memory\n");

		simOper->src2.plist->parno = srcList.size();
		
		if ((simOper->src2.plist->params = (md_arg_t *) malloc(srcList.size() * sizeof(md_arg_t))) == NULL)
			printf("Not enough memory\n");

		for (iT = srcList.begin(), i = 0; iT != srcList.end(); iT++, i++) {
			arg = *iT;
			translateArg(arg, &simOper->src2.plist->params[i]);
		}	
		
		/* more special processing */
		if (((CallRetOp *) op)->isSpecialCall()) { /* DEBUG_PRINT */
			int specCallType = ((CallRetOp *) op)->getSpecialCallType();
			int exprOpCode = ((CallRetOp *) op)->getSpecialOpcode(specCallType);
			
			simOper->opcode = exprToFuncSimOpMap[exprOpCode];
				
		} else if (isSystemCall(op)) { /* SYSCALL */
			simOper->opcode = exprToFuncSimOpMap[getSystemCallOpCode()];
			
		} else {
			if (isDynamicCall(op)) { /* DYNCALL */
				simOper->opcode = exprToFuncSimOpMap[getDynamicCallOpCode()];
				
			} else { /* normal call (CALL) */
				simOper->opcode = exprToFuncSimOpMap[((NormalOp *) op)->getOpCodeIndex()];
				simOper->src1.targ = instrPtrToIndexMap[targetInstr];
					
			}
		}
	} else if (op->isIfOp()) {
		/* source 1 operand */
		arg = ((IfOp *) op)->ptrToCCReg();
		if (arg)
			translateArg(arg, &simOper->src1);

		/* target operand */
		simOper->src2.targ = instrPtrToIndexMap[targetInstr];

	} else if (op->isRetOp()) {
		

	} else {
		printf("Wrong operation\n");
	}
}

/* translate an instruction from Express to FuncSim */
void translateInstr(BaseInstruction *instr, md_instr_t *simInstr)
{
	BaseOpSlot& rootSlot = instr->getRootOpSlot();
	BaseOpSlot *crtSlot;	// = &rootSlot;
	BaseOperation *op;
	BaseInstruction *targetInstr;
	
	int crtOper = 0;

	/* instr->shortPrint(cout); */

	//while (crtSlot && (crtSlot->isDataOpSlot())) {

	ArrayIterator<DataOpSlot>* dataOpSlotIter = ((NormalInstr*)instr)->getDataOpSlotIter();
	while (dataOpSlotIter->hasMoreElements()) {
		crtSlot = (BaseOpSlot *) &(dataOpSlotIter->currentElement());

		if (crtSlot->hasOper()) {

			op = crtSlot->getPtrToOperation();
			translateDataOp(op, &(simInstr->oper[crtOper]));
		
			crtOper++;
			if (crtOper >= OPER_PER_INSTR)
				printf("Increase OPER_PER_INSTR !\n");
		}

		//crtSlot = crtSlot->getPtrToNextSlot();
		dataOpSlotIter->nextElement();
	}
	delete dataOpSlotIter;

	//while (crtSlot && (crtSlot->isControlOpSlot())) {
	ArrayIterator<ControlOpSlot>* controlOpSlotIter = ((NormalInstr*)instr)->getControlOpSlotIter();
	while (controlOpSlotIter->hasMoreElements()) {
		crtSlot = (BaseOpSlot *) (&controlOpSlotIter->currentElement());

		if (crtSlot->hasOper()) {

			op = crtSlot->getPtrToOperation();		
			targetInstr = crtSlot->getPtrToTrueBranch()->getPtrToOperation()->getTargetInstr();

			translateControlOp(op, targetInstr, &(simInstr->oper[crtOper]));
		
			crtOper++;
			if (crtOper >= OPER_PER_INSTR)
				printf("Increase OPER_PER_INSTR !\n");
		}

		//crtSlot = crtSlot->getPtrToFalseBranch();
		controlOpSlotIter->nextElement();
	}
	delete controlOpSlotIter;

	simInstr->oper_no = crtOper;

	//if (crtSlot && (crtSlot->isFlowOpSlot())) {
		//BaseInstruction *nextInstr = crtSlot->getPtrToOperation()->getTargetInstr();
		BaseInstruction *nextInstr = ((NormalInstr *) instr)->getDefaultTargetInstr();
		if (nextInstr)
			simInstr->next = instrPtrToIndexMap[nextInstr];
		//} else {
		//	printf("Error in instruction structure\n");
		//}
}

void translateCallOpFromDump(md_oper_t *simOper, OperationLatchData &opPacket)
{
	/* destination address for result */
	int opcode = opPacket.getOpCode();
	MyString s;
	int i;
	
	simOper->opcode = exprToFuncSimOpMap[opcode];

	// destination
	if(opPacket.getOperand1().size() > 0)
	{
		AddrOperand dest = (opPacket.getOperand1())[0];
		simOper->dest.reg = dest.getAddrOperand();
	}
	
    int numOfSrcs = (opPacket.getOperand2()).size();
	
	if( numOfSrcs > 0 )
	{
		
		vector<PredicateAddrDataUnion> srcList = opPacket.getOperand2();
		// call target
		PredicateAddrDataUnion src = srcList[0];
		
		if( opcode == getOpcodeIndex("DEBUG_PRINT") )
		{
			i = opcode;
		}
		else
		{
			// src contains the function name
			s = src.getData()._as.label;
			i = getSystemCallNameAsInt(s.getStr());
		}
		
		if (exprToFuncSimSyscallCodeMap.find(i) != exprToFuncSimSyscallCodeMap.end())
			simOper->src1.targ = exprToFuncSimSyscallCodeMap[i];
		else
			simOper->src1.targ = SYS_NA;
		
		// parameter list

		if( opcode == getOpcodeIndex("DEBUG_PRINT") )
		{
			// src contains the parameter of debug_print
			simOper->opcode = exprToFuncSimOpMap[opcode];
			simOper->src2.plist = (md_plist_t *) malloc(sizeof(md_plist_t));
			simOper->src2.plist->parno = 1;
			simOper->src2.plist->params = (md_arg_t *) malloc(sizeof(md_arg_t));
			simOper->src2.plist->params[0].reg = src.getAddr().getAddrOperand();
			return;
		}

		vector<PredicateAddrDataUnion>::iterator iT;
		
		if ((simOper->src2.plist = (md_plist_t *) malloc(sizeof(md_plist_t))) == NULL)
			printf("Not enough memory\n");
		
		simOper->src2.plist->parno = srcList.size() - 1;
		
		if ((simOper->src2.plist->params = (md_arg_t *) malloc((srcList.size()-1) * sizeof(md_arg_t))) == NULL)
			printf("Not enough memory\n");
		
		for (iT = srcList.begin()+1, i = 0; iT != srcList.end(); iT++, i++) {
			src = *iT;
			simOper->src2.plist->params[i].reg = src.getAddr().getAddrOperand();
		}	
	}
	
	/*if (isSystemCall(opcode))
	{ // SYSCALL 
		simOper->opcode = exprToFuncSimOpMap[getSystemCallOpCode()];
		
	} else 
	{
		if (isDynamicCall(opcode)) 
		{ // DYNCALL 
			simOper->opcode = exprToFuncSimOpMap[getDynamicCallOpCode()];
			
		} else
		{ // normal call (CALL) 
			simOper->opcode = exprToFuncSimOpMap[getOpcodeIndex(opcode)];
			// simOper->src1.targ = instrAddrToIndexMap[targetInstr];
			
		}
	}
	*/
	return; 
}

/* translates a data operation from Express to FuncSim */
void translateOpFromDump(md_oper_t *simOper, OperationLatchData &opPacket)
{
	int opcode = opPacket.getOpCode();
	
	simOper->opcode = exprToFuncSimOpMap[opcode];
	simOper->dest.ival = simOper->src1.ival = simOper->src2.ival = simOper->flags = 0;
	
	if(opPacket.getOperand1().size() > 0)
	{
		AddrOperand dest = (opPacket.getOperand1())[0];
		simOper->dest.reg = dest.getAddrOperand();
	}

	
	int numOfSrcs = (opPacket.getOperand2()).size();
	
	/* source 1 operand */
	
	if( numOfSrcs > 0 )
	{
		
		PredicateAddrDataUnion src = (opPacket.getOperand2())[0];
		if(src.getType() == _ADDRTYPE_)
		{
			simOper->src1.reg = src.getAddr().getAddrOperand();
		}
		else
		{
			if( src.getData().getType() == _INT )
			{
				simOper->src1.ival = src.getData()._as.integer;
			}
			else if( src.getData().getType() == _FLT )
			{
				simOper->src1.fval = src.getData()._as.flt;
			}
			else if( src.getData().getType() == _DBL )
			{
				simOper->src1.dval = src.getData()._as.dbl;
			}
			simOper->flags |= OPFLAG_IMM1;
		}
	}
	
	if(isBranch(opcode))
	{
		/* target operand */
		long addr = (opPacket.getOperand2())[1].getData()._as.integer;
		simOper->src2.targ = instrAddrToIndexMap[addr];
		return;
	}
	

		/* source 2 operand */
	if( numOfSrcs > 1 )
	{
		PredicateAddrDataUnion src = (opPacket.getOperand2())[1];
		if(src.getType() == _ADDRTYPE_)
		{
			simOper->src2.reg = src.getAddr().getAddrOperand();
		}
		else
		{
			if( src.getData().getType() == _INT )
			{
				simOper->src2.ival = src.getData()._as.integer;
			}
			else if( src.getData().getType() == _FLT )
			{
				simOper->src2.fval = src.getData()._as.flt;
			}
			else if( src.getData().getType() == _DBL )
			{
				simOper->src2.dval = src.getData()._as.dbl;
			}
			simOper->flags |= OPFLAG_IMM2;
		}
	}
	

		/* source 3 operand */
	if( numOfSrcs > 2 )
	{
		PredicateAddrDataUnion src = (opPacket.getOperand2())[2];
		if(src.getType() == _ADDRTYPE_)
		{
			simOper->src3.reg = src.getAddr().getAddrOperand();
		}
	}
	return;
}


extern MemoryModule *mainMemory;



/* translate a program from IR dump to FuncSim */
md_addr_t translateProgramFromDump()
{
	int i;
	vector<MyString> opStringVector;
	Val val;
	long startAddr = 0;
	MyString startInstr;

	// create the mappings for translating between Express and FuncSim
	createOpCodeMap();
	createSyscallCodeMap();
	
	// memory initializations 
	my_initialize_memory(mem);
	// make sp_reg point to the target SP register
	sp_reg = getSPAddress();
	SET_REG_I(sp_reg, FS_STACK_START_ADDRESS);
	
	int instrCount = 0;
	// count the number of instructions and set the instrAddrToIndexMap
	while(1)
	{
		long addr;
		addr = startAddr+instrCount*4;
		((Dram*)mainMemory)->read(addr, val);

		MyString oper = getStringValue(val);
		
		if (!oper.isEmpty())
		{
			instrAddrToIndexMap[addr] = instrCount;
		}
		opStringVector.push_back(oper);
		instrCount++;
		char opcode[40];
		char *tmpinstr;
		
		tmpinstr = oper.getStr();
		
		// Get the OpCode in the instruction and its corresponding index..
		sscanf(tmpinstr,"%s",opcode);
		if(strcmp(opcode, "jr") == 0)
			break;
	}
	
	sim_program = (md_instr_t *) malloc(instrCount * sizeof(md_instr_t));

	assert(sim_program);

	for(i = 0; i < instrCount; i++)
	{
		MyString oper = opStringVector[i];

		if (!oper.isEmpty())
		{
			OperationLatchData opData;
			char opcode[40];
			char *tmpinstr;
			
			tmpinstr = oper.getStr();

			// Get the OpCode in the instruction and its corresponding index..
			sscanf(tmpinstr,"%s",opcode);
			int opcodeIndex = getOpcodeIndex(opcode);
			md_instr_t & simInstr = sim_program[i];
			md_oper_t & simOper = simInstr.oper[0];

			if (isControlOp(opcodeIndex)) 
			{
				processControlOp(tmpinstr, opcodeIndex, opData);
			}
			else  
			{
				processDataAndMemoryOp(tmpinstr, opcodeIndex, opData);
			}
			if (isCall(opcodeIndex))
			{
				translateCallOpFromDump(&simOper, opData);
			}
			else
			{
				translateOpFromDump(&simOper, opData);
			}
			simInstr.next = i+1;
			simInstr.oper_no = 1;  // We take one operation per instruction
		}
		
	}

	int startInstrAddr = instrAddrToIndexMap[startAddr];

	return startInstrAddr;
}


/* translate a program from IR to FuncSim */
md_addr_t translateProgram(Program *prog)
{
	int crtInstrNo = 0;

	/* create the mappings for translating between Express and FuncSim */
	createOpCodeMap();
	createSyscallCodeMap();
	
	/* memory initializations */
	my_initialize_memory(mem);
	sp_reg = getSPAddress();
	SET_REG_I(sp_reg, FS_STACK_START_ADDRESS);
	
	// ?????
	// MyString str("cmd_line");
	/* Now, read in the command line arguments. */
	// my_readCommandLineArgs(str);

	/* count the number of instruction in the program
	 * create a map between instr pointers and indices in the instr array
	 */
	MyLinkedListIterator<Routine *> *rIter = prog->routineListIteratorForw();
	while (rIter->hasMoreElements()) {
		Routine *routine = rIter->currentElement();

		MyLinkedList<BaseInstruction *>& instrList = routine->getInstrList();

		MyLinkedListIterator<BaseInstruction *> *instrIter = instrList.elementsForward();
		while (instrIter->hasMoreElements()) {
			BaseInstruction *instr = instrIter->currentElement();

			instrPtrToIndexMap[instr] = crtInstrNo;
			crtInstrNo++;

			instrIter->nextElement();
		}
		delete instrIter;

		rIter->nextElement();
	}
	delete rIter;

	createSavedRegsMap(prog);

	/* allocate space for the program instructions */
	instr_no = crtInstrNo;
	sim_program = (md_instr_t *) malloc(instr_no * sizeof(md_instr_t));

	assert(sim_program);

	crtInstrNo = 0;

	/* translate the instructions */
	rIter = prog->routineListIteratorForw();
	while (rIter->hasMoreElements()) {
		Routine *routine = rIter->currentElement();

		MyLinkedList<BaseInstruction *>& instrList = routine->getInstrList();

		MyLinkedListIterator<BaseInstruction *> *instrIter = instrList.elementsForward();
		while (instrIter->hasMoreElements()) {
			BaseInstruction *instr = instrIter->currentElement();

			translateInstr(instr, &sim_program[crtInstrNo]);
			crtInstrNo++;

			instrIter->nextElement();
		}
		delete instrIter;

		rIter->nextElement();
	}
	delete rIter;	

	/* get the start instruction of the program */
	BaseInstruction *startInstr = prog->getRoutine("_main")->getInstrList().getHead();
	int startInstrNo = instrPtrToIndexMap[startInstr];

	return startInstrNo;
}


