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
#ifndef _EXPRESSINCLUDES_H_
#define _EXPRESSINCLUDES_H_

#include "ArchSTLIncludes.h"
#include "DerivedClassIncludes.h"

#include "DerivedLatchData.h"
#include "DerivedLatch.h"

#include "ExpressAddrData.h"

#include "IConstArg.h"
#include "FConstArg.h"
#include "CallOp.h"
#include "MemoryOp.h"
//#include "Routine.h"
#include "Program.h"
#include "Instruction.h"

#include "CallNameArgProperty.h"

const int _ALL_ = 220;
const int _DEST_AND_SOURCE1_ = 221;

const int argCRegNum = 4;
const int argVRegNum = 5;

const int POINTER_INCREMENT = 4;
const int ARGV_START_LOCATION = 100000;
const int ARGV_STRING_START_LOCATION = 101000;

extern OpCodeTable globalOpCodeTable;

extern int getRegName(RegisterArg *tempArg);

extern void convertDestToDecodeOperand(AddrOperand & a, BaseArgument *argPtr);

//SS 3/2/01 Change AddrDataUnion to PredAddrDataUnion
//extern void convertSrcToDecodeOperand(AddrDataUnion & a, BaseArgument *argPtr);
extern void convertSrcToDecodeOperand(PredicateAddrDataUnion & a, BaseArgument *argPtr);

//extern int processOpAndSetDelay(BaseInstruction *currInstr, BaseOperation *currOper, DecOpSubLatchData & tempElem, int srcDelay, int whichOperands = _ALL_);

//extern int processCallOpAndSetDelay(BaseInstruction *currInstr, BaseOperation *currOper, DecOpSubLatchData & tempElem, int srcDelay);
//extern int processMemoryOpAndSetDelay(BaseInstruction *currInstr, BaseOperation *currOper, DecOpSubLatchData & tempElem, int srcDelay);

extern InstrPtr getNextPC(InstrPtr pcInstr);
//extern int decodeInstr(BaseInstruction *instrPtr, Vector<DecOpSubLatchData> & tempDataArr, Vector<BrCallLatchData> & tempControlArr, int srcDelay);

extern int isSystemCall(IRPtr & cTarg);
extern int isSystemCall(int opcode);
extern int isSystemCall(BaseOperation *oper);
extern int isSystemCall(AddrDataUnion & a);
extern int isSystemCall(MyString & s);
extern int isDynamicCall(IRPtr & ptr);
extern int isDynamicCall(BaseOperation *op);

extern int getSystemCallOpCode();

extern void doSystemCallStuff(Vector<AddrDataUnion> & srcArr, Val & destVal, Connection *conn);
extern void doSystemCallStuff(IRPtr & cTarg, Connection *conn1 = NULL, Connection *conn2 = NULL);
extern void doCallStuff(IRPtr & cTarg, Routine *nR = NULL);
extern void doRetStuff(PCType & pc);
 
extern void printAReg(int regNum, char *regText, int ssaNum);
extern void getRegFromUserAndPrint();

extern void setCallArguments(IRPtr & cTarg);

#endif