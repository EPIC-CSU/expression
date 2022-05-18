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
 *file  : ExpressCallSupport.cpp                             *
 *created : Jan 08, 99                                       *
 *authors : Ashok Halambi                                    *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/


#include "ExpressIncludes.h"
#include "SimCall.h"
#include "ExecSemantics.h"
#include "DynamicScheduler.h"

extern Routine *simCurrRoutine;
extern Program *curProgram;

extern Vector<Val> sysCallInputArgs;
extern Vector<Val> sysCallOutputArgs;
extern Vector<SysSideEffect> sysCallSideEffects;

extern int debugSIMULATORL1;

long int STACK_START_ADDRESS = 1000000;
int tempArgVStrLoc = ARGV_STRING_START_LOCATION;

extern Map<int, int> sysCallArgToTypeMap;
//
// The first int is the operand number, and the second is the type of the operand.
//
// E.g. printf: 1st operand is of type CHAR_POINTER

Vector<SimCallObject> simCallStack;

void simCallPop()
{
	simCallStack.pop_back();
}

void simCallPush(SimCallObject & a)
{
	simCallStack.push_back(a);
}

SimCallObject & simCallTop()
{
	return simCallStack.back();
}

bool simCallEmpty()
{
	return simCallStack.empty();
}

extern Storage *toyRegFile;
extern Storage *savedRegFile;

extern int convertReg(AddrOperand & a, BaseArgument *argPtr);

extern int isSystemCall(MyString & s);
extern void makeSystemCall(MyString & s);
extern int checkSysCallAndSetOperands(MyString & s);

extern int getSystemCallOpCode();

int isSystemCall(int opcode)
{
	if (opcode == getSystemCallOpCode())
		return _YES_;
	return _NO_;
}

int isSystemCall(BaseOperation *op)
{
	// We assume that it is a CallOp.

	char *callName;

	if ((callName = op->getTargetLabelStr()) != NULL)
	{
		MyString s(callName);

		return (isSystemCall(s));
	}
	return _NO_;
}

int isDynamicCall(BaseOperation *op)
{
	// We assume that it is a CallOp.

	if (((CallRetOp *)op)->isDynamicAddrCall()) return _YES_;
	return _NO_;
}

int isSystemCall(AddrDataUnion & a)
{
	if (a.getType() != _VALTYPE_) return _NO_;

	if ((a.getData())._valType != _INTERNAL_LABEL) return _NO_;

	MyString s((a.getData())._as.label);

	return isSystemCall(s);
}

int isDynamicCall(IRPtr & ptr)
{
	BaseOperation *oper = ptr.getOper();

	if (((CallRetOp *)oper)->isDynamicAddrCall()) return _YES_;
	return _NO_;
}

extern int isSysSourceType(MyString & s, int position);
extern int hasSideEffects(MyString & s);

void makeSystemCall(MyString & s, Connection *conn)
{
	Val a1;
	AddrOperand a;

	SignalClass lat;

	while (!checkSysCallAndSetOperands(s))
	{
		for (Map<int, int>::iterator i = sysCallArgToTypeMap.begin(); i != sysCallArgToTypeMap.end(); i++)
		{
			if ((*i).second == CHAR_POINTER)
			{
				a.setAddrOperand(_DIRECTMEMORY_, sysCallInputArgs[(*i).first]._as.integer);		
				// Now, we need to access the memory location and get the actual value.
				//
				conn->read(a1, a, lat); // ingore the latency for now.
			
				sysCallInputArgs[(*i).first] = a1;
			}
			a.reset();
			a1.reset();
		}
		sysCallArgToTypeMap.clear();
	}
		
	makeSystemCall(s);
}

void makeSystemCall(BaseOperation *op, Connection *conn)
{
	char *callName;

	if ((callName = op->getTargetLabelStr()) != NULL)
	{
		MyString s(callName);
		makeSystemCall(s, conn);
	}
}

Val & getValueOfReg(AddrOperand & a, Connection *conn)
{
	Val a1;

	if (conn == NULL)
	{
		// error condition.
		//
		cout << "Panic: Tried to get value of a register from a NULL connection" << endl;
		assert(0);
	}

	if(a.getType() == _REG_)
	{
		conn->read(a1, a);
	}

	return a1;
}

void setValueOfReg(AddrOperand & a, Val & b, Connection *conn)
{
	if (conn == NULL)
	{
		// error condition.
		//
		cout << "Panic: Tried to get value of a register from a NULL connection" << endl;
		assert(0);
	}

	if (a.getType() == _REG_)
	{
		conn->write(b, a);
	}
}

void doSideEffectsStuff(MyString & sysName, Connection *conn)
{
	// If this system call produces a side effect, and if sysCallSideEffects is not empty,
	// write the side effects into memory.

	SignalClass lat;

	if ((hasSideEffects(sysName)) && !(sysCallSideEffects.empty()))
	{
		for (Vector<SysSideEffect>::iterator tI = sysCallSideEffects.begin(); tI != sysCallSideEffects.end(); tI++)
		{
			// write the value into memory.
			//
			conn->write((*tI).val, (*tI).addr, lat);
		}
	}
}

int isSystemCall(IRPtr & cTarg)
{
	OperPtr op = cTarg.getOper();

	if (isSystemCall(op)) return _YES_;
	return _NO_;
}

// Conn = Memory connection.
//
void doSystemCallStuff(Vector<AddrDataUnion> & srcArr, Val & destVal, Connection *conn)
{
	sysCallInputArgs.clear();
	sysCallOutputArgs.clear();
	sysCallSideEffects.clear();
	sysCallArgToTypeMap.clear();

	AddrDataUnion a;
		
	// Now, read the src array (which contains AddrDataUnions)
	//
	// the first arg is the name of the systemcall
	// while the rest are source arguments.
	//
	// Finally, place them in the sysCallInputArgs array.
	//
	Vector<AddrDataUnion>::iterator iT;
	iT = srcArr.begin();

	// Note: We do not perform any error checking here (to make sure that
	// the first addrdataunion is indeed the name of the system call)
	//
	MyString sysName(((*iT).getData())._as.label);

	iT++;

	while (iT != srcArr.end())
	{
		if ((*iT).getType() != _VALTYPE_)
		{
			// something's wrong.
			//
			// we expect that all the sources have been read (i.e no more addresses)
			//
			cout << "Panic: System call recieved address as a source operand" << endl;
			assert(0);
		}
		sysCallInputArgs.push_back((*iT).getData());
		iT++;
	}

	makeSystemCall(sysName, conn);

	doSideEffectsStuff(sysName, conn);

	// If the destination arg exists, and the sysCallOutputArgs array is not empty,
	// write the destination into the register file.
	
	if (!sysCallOutputArgs.empty())
	{	
		destVal = sysCallOutputArgs[0];
	}
}

// Conn1 : Register connection
// Conn2 : Memory connection
//
void doSystemCallStuff(IRPtr & cTarg, Connection *conn1, Connection *conn2)
{
	OperPtr op = cTarg.getOper();
	InstrPtr instr = cTarg.getInstr();
	
	if ((op == NULL) || (!op->isCallOp()))
	{
		// error condition.
		//
		cout << "Warning: The simulator recieved an invalid call oper" << endl;
		return;
	}
	
	sysCallInputArgs.clear();
	sysCallOutputArgs.clear();
	sysCallSideEffects.clear();
	
	// First read the input args and put them into a temporary array
	//
	//Changed for Predication support
	//Vector<AddrDataUnion> srcArgList;
	Vector<PredicateAddrDataUnion> srcArgList;
	//AddrDataUnion a;
	PredicateAddrDataUnion a;
	
	ArgListIter iP;
	ArgList & aLP = ((CallRetOp *)op)->getParmList(); 
	
	for (iP = aLP.begin(); iP != aLP.end(); iP++)
	{
		convertSrcToDecodeOperand(a, (*iP));
		srcArgList.push_back(a);
		a.reset();
	}
	
	// Now, read the temporary array (which contains AddrDataUnions), and,
	// if any arg is a register arg, read the value from the register file.
	//
	// Finally, place them in the sysCallInputArgs array.
	//
	//Changed for Predication support
	//Vector<AddrDataUnion>::iterator iT;
	Vector<PredicateAddrDataUnion>::iterator iT;
	
	for (iT = srcArgList.begin(); iT != srcArgList.end(); iT++)
	{
		if ((*iT).getType() == _VALTYPE_)
		{
			// Then we just place it in the sysCallInputArgs array.
			//
			sysCallInputArgs.push_back((*iT).getData());
		}
		else // if ((*i).getType() == _ADDRTYPE_)
		{
			// Read the argument from the connection.
			//
			Val tempVal = getValueOfReg((*iT).getAddr(), conn1);
			sysCallInputArgs.push_back(tempVal);
		}
	}
	
	// Now, make the system call.
	//
	makeSystemCall(op, conn2);
	
	// If the destination arg exists, and the sysCallOutputArgs array is not empty,
	// write the destination into the register file.
	
	if (op->hasOperand(_DEST_))
	{
		if (sysCallOutputArgs.empty())
		{
			// error condition.
			//
			cout << "Panic: System Call is expected to produce an output (but it didn't)" << endl;
		}
		
		BaseArgument *dArg = op->ptrToOperand(_DEST_);
		AddrOperand addr;
		convertDestToDecodeOperand(addr, dArg);
		setValueOfReg(addr, sysCallOutputArgs[0], conn1);
	}
}

void doCallStuff(IRPtr & cTarg, Routine *nR)
{
	/**********************
	OperPtr op = cTarg.getOper();
	InstrPtr instr = cTarg.getInstr();
	
	if ((op == NULL) || (!op->isCallOp()))
	{
		// error condition.
		//
		cout << "Warning: The simulator recieved an invalid call oper" << endl;
		return;
	}
	
	// First, push the IRPtr into the sim stack.
	//
	SimCallObject a(instr, op);
	
	AddrOperand dest;
	if (op->hasOperand(_DEST_))
	{
		convertReg(dest, op->ptrToOperand(_DEST_));
		a.setDest(dest);
	}
	a.setCallerRoutine(simCurrRoutine);
	simCallPush(a);
	
	// Important: The model we follow here is to asynchronously perform
	// the actions needed in a call (e.g. saving registers etc).
	//
	// The correct way to do this would be to go thru a stream of instructions
	// which perform the actions.
	
	// Save the registers:
	//
	ArgListIter i;
	ArgList & aL = ((CallRetOp *)op)->getSavedRegList(); 
	
	AddrOperand addr;
	Val val;
	for (i = aL.begin(); i != aL.end(); i++)
	{
		if (!convertReg(addr, (*i)))
		{
			// error condition.
			//
			cout << "Warning: The argument to be saved (in a call) was not of register type" << endl;
			continue;
		}
		toyRegFile->read(val, addr);
		savedRegFile->write(val, addr);
	}
	savedRegFile->doUpdate();

	// finally set the current routine to the called routine.
	//
	if (nR != NULL)
		simCurrRoutine = nR;
	else
		simCurrRoutine = ((CallRetOp *)op)->ptrToRoutine();

	cout << "Calling: " << simCurrRoutine->getName() << endl;

	char debugName[] = "_addnode";
	if (strcmp(simCurrRoutine->getName(), debugName) == 0)
	{
		int dbrake = 1;
	}
	if (strcmp(simCurrRoutine->getName(), debugName) == 1)
	{
		int dbrake = 1;
	}
	***************************/
}

extern int getRAddress(int symval);
extern int getRIDWithoutSSA(int symval);

void setCallArguments(IRPtr & cTarg)
{
	/**********************
	// Save the previous values of R4.1, R5.1 and R6.1 in savedRegFile.
	//
	// Then write the arguments into R4.1, R5.1 and R6.1.
	//
	// This is presuming that SSA has been performed.
	//
	if (doneSSA)
	{
		OperPtr op = cTarg.getOper();

		if ((op == NULL) || (!op->isCallOp()))
		{
			// error condition.
			//
			cout << "Warning: The simulator recieved an invalid call oper" << endl;
			return;
		}

		AddrOperand addr, addr4, addr5, addr6;
		Val val;

		// Save R4.1, R5.1, R6.1
		//
		int r4addr, r5addr, r6addr;

		r4addr = getRAddress(4);
		addr4.setAddrOperand(_REG_, r4addr);
		toyRegFile->read(val, addr4);
		savedRegFile->write(val, addr4);

		r5addr = getRAddress(5);
		addr5.setAddrOperand(_REG_, r5addr);
		toyRegFile->read(val, addr5);
		savedRegFile->write(val, addr5);

//		r6addr = getRAddress(6);
//		addr6.setAddrOperand(_REG_, r6addr);
//		toyRegFile->read(val, addr6);
//		savedRegFile->write(val, addr6);

		ArgListIter i;
		ArgList & aL = ((CallRetOp *)op)->getParmList();

		int rid;

		for (i = aL.begin(); i != aL.end(); i++)
		{
			if (!convertReg(addr, (*i)))
			{
				// error condition.
				//
				cout << "Warning: The argument to be saved (in a call) was not of register type" << endl;
				continue;
			}

			toyRegFile->read(val, addr);

			// Now, we know that the argument is a register.
			// Determine if it is R4, R5, or R6.
			//
			if ((rid = ((RegisterArg *)(*i))->getIDWithoutSSA()) != _INVALID_)
			{
				if (rid == getRIDWithoutSSA(4))
				{
					toyRegFile->write(val, addr4);
				}
				else if (rid == getRIDWithoutSSA(5))
				{
					toyRegFile->write(val, addr5);
				}
//				else if (rid == getRIDWithoutSSA(6))
//				{
//					toyRegFile->write(val, addr6);
//				}
				else
				{
					// error condition
					//
					cout << "Warning: Recieved a call argument that was not R4, R5 or R6" << endl;
				}
			}
		}
	}
	************************/
}

void doRetStuff(PCType & pc)
{
	/**********************
	// First, pop the IRPtr from the sim stack.
	//
	if (simCallEmpty())
	{
		// if the call stack is empty, then this return 
		//must be the return of the main function.

		if (simCurrRoutine != curProgram->getMainRoutine())
		{
			// error condition.
			//
			cout << "Error: Something wrong with the call/return pattern in the program" << endl;
			assert(0);
		}
		return;
	}

	SimCallObject a = simCallTop();
	simCallPop();

	OperPtr op = a.ptrToOperation();
	InstrPtr instr = a.ptrToInstruction();

	if ((op == NULL) || (!op->isCallOp()))
	{
		// error condition.
		//
		cout << "Warning: The simulator recieved an invalid call oper" << endl;
		return;
	}

	// Important: The model we follow here is to asynchronously perform
	// the actions needed in a return (e.g. restoring registers etc).
	//
	// The correct way to do this would be to go thru a stream of instructions
	// which perform the actions.

	// Restore the registers:
	//
	ArgListIter i;
	ArgList & aL = ((CallRetOp *)op)->getSavedRegList(); 

	AddrOperand addr;
	Val val;
	for (i = aL.begin(); i != aL.end(); i++)
	{
		if (!convertReg(addr, (*i)))
		{
			// error condition.
			//
			cout << "Warning: The argument to be saved (in a call) was not of register type" << endl;
			continue;
		}
		savedRegFile->read(val, addr);
		toyRegFile->write(val, addr);
	}
	toyRegFile->doUpdate();

	// Then set the pc to the correct instr.
	//
	BaseInstruction *nextInstr = ((NormalInstr *)instr)->getDefaultTargetInstr();
	pc.setPC(nextInstr);

	// finally set the current routine to the calling routine.
	simCurrRoutine = a.getCallerRoutine();
	*******************/
}

#include "defs_node.h"

extern vector<defs_node *> decl_list;
typedef vector<defs_node *>::iterator Defs_node_iter;

extern defs_node *get_ascii(defs_node *node);
extern Vector<int> *get_word_list(defs_node *node);

extern long get_memory_assig(char *name);

void initializeMemory(Storage *mem)
{
	// First go thru the EXPRESS memory initialize datastructure.
	//
	// For each location, store the initialization content.

	if ((mem == NULL) || !(mem->is(_MEM_)))
	{
		// error condition.
		//
		cout << "Warning: Tried to initialize a non-Memory Object in function initializeMemory" << endl;
		return;
	}

	Defs_node_iter i;

	defs_node *currNode, *asciiVal;

	char *asciiStr;

	long memAddr;
	AddrOperand a;

	Val a1;
	Vector<int> *wordList;
	Vector<int>::iterator wI;

	for (i = decl_list.begin(); i != decl_list.end(); i++)
	{
		if ((currNode = (*i)) != NULL) // i.e. is a valid node.
		{
			if ((asciiVal = get_ascii(currNode)) != NULL)
			{ // i.e. the decl has an ascii value that needs to be initialized.

				// First get the memory location that needs to be initialized.
				//
				// Then, get the initialization string.

				memAddr = get_memory_assig(currNode->Name());
				asciiStr = asciiVal->Name();

				// Now, we have the address and the initialization string.
				//
				// Create the addrOperand and the Val objects inorder to write to memory.

				a.setAddrOperand(_DIRECTMEMORY_, memAddr);

				a1.changeValue(asciiStr);

				mem->initialize(a1, a);
			}
			else if ((wordList = get_word_list(currNode)) != NULL)
			{
				// Iterate over wordlist, and initialize memory.
				//
				memAddr = get_memory_assig(currNode->Name());
				for (wI = wordList->begin(); wI != wordList->end(); wI++)
				{
					a.setAddrOperand(_DIRECTMEMORY_, memAddr);
					a1.changeValue(*wI);

					mem->initialize(a1, a);

					memAddr += _WORD_SIZE_;
				}
				delete wordList;
			}
		}
	}
}


extern int getRAddress(int symval);
extern int getSPAddress();
long int STACK_POINTER_REG_NUMBER = 33;

void initializeStackPointer(Storage *regFile)
{
	if ((regFile == NULL) || !(regFile->is(_REGISTER_)))
	{
		// error condition.
		//
		cout << "Warning: Tried to initialize a non-Memory Object in function initializeStackPointer" << endl;
		return;
	}

	AddrOperand spReg;

	// This is a hack. Stack pointer is assumed to be number 33
	spReg.setAddrOperand(_REG_, STACK_POINTER_REG_NUMBER, _ADDR_INT_);
	Val a1(STACK_START_ADDRESS);
	regFile->initialize(a1, spReg);

	if (debugSIMULATOR)	{
		cout << "Stack pointer (R33) is initialized with value " << STACK_START_ADDRESS << endl;
	}
}

extern int argC;
extern Vector<MyString> argV;

// Argc is to be stored in R4 (either R4 if no SSA or R4.0 if SSA)
//
// Assume that argc has already been read in to the variable argC
//
void initializeArgc(Storage *regFile)
{
	AddrOperand a;

	int r4addr = getRAddress(argCRegNum);
	a.setAddrOperand(_REG_, r4addr);

	Val a1(argC);
	regFile->initialize(a1, a);
}

extern int tempArgVStrLoc;

int setArgVStringAndGetLoc(MyString & s, Storage *mem)
{
	AddrOperand a;
	a.setAddrOperand(_DIRECTMEMORY_, tempArgVStrLoc, _ADDR_STRING_);
	
	Val a1(s.getStr());

	mem->initialize(a1, a);

	int retval = tempArgVStrLoc;

	tempArgVStrLoc += (s.getStrLen() + 1);

	return retval;
}

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
void initializeArgv(Storage *regFile, Storage *mem)
{
	// first reset tempArgVStrLoc;
	//
	tempArgVStrLoc = ARGV_STRING_START_LOCATION;

	AddrOperand a;

	int r5addr = getRAddress(argVRegNum);
	a.setAddrOperand(_REG_, r5addr);

	Val a1(ARGV_START_LOCATION);
	regFile->initialize(a1, a);

	Vector<MyString>::iterator i;

	int currLoc = ARGV_START_LOCATION + POINTER_INCREMENT;
	for (i = argV.begin(); i != argV.end(); i++)
	{
		a.setAddrOperand(_DIRECTMEMORY_, currLoc);
		a1.changeValue(setArgVStringAndGetLoc(*i, mem));
		mem->initialize(a1, a);

		currLoc += POINTER_INCREMENT;
	}
}
/*
int getSPAddress()
{
	int uniqID = _INVALID_;

	if (!doneSSA)
	{
		uniqID = globalRegFileList->getBaseID("sp");
	}
	// After RA, SP is mapped to R1
	else if (doneRA)
	{
		uniqID = globalRegFileList->getBaseID("R") + 1;
	}
	else
	{
		uniqID = globalSymbolTable->getID(0, "sp", 1);
	}
	return uniqID;
}
*/

// The SP mapping will vary from one architecture to another.
// This information is derived from ADL. For example in MIPS,
// SP maps to $29.
extern void getSPSymVal(MyString &regText, int &symVal);

int getSPAddress()
{
	int uniqID = _INVALID_;
	MyString regText;
	int spSymVal;

	if (!doneSSA)
	{
		uniqID = globalRegFileList->getBaseID("gsp");
	}
	else if (!doneRA)
	{
		uniqID = globalSymbolTable->getID(0, "gsp", 1);
	}
	else
	{
		assert("Target Register set is not yet specified !\n");

		// SP is R29 : PB
		// From targetRegClassToRegsMapping.txt

		getSPSymVal(regText, spSymVal);
		if( spSymVal == -1 )
			assert(0);

		uniqID = globalRegFileList->getID(spSymVal, regText.getStr());
	}
	return uniqID;
}

int getRAddress(int symval)
{
	int uniqID = _INVALID_;

	if (!doneSSA)
	{
		uniqID = globalRegFileList->getBaseID("R") + symval;
	}
	else
	{
		uniqID = globalSymbolTable->getID(symval, "R", 1);
	}
	return uniqID;
}

int getRIDWithoutSSA(int symval)
{
	int uniqID = globalRegFileList->getBaseID("R") + symval;

	return uniqID;
}
