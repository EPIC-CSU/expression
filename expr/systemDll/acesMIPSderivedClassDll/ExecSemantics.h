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
 *file  : ExecSemantics.h                                    *
 *created : May 03,2000                                     *
 *authors : Prabhat, Srikanth                               *
 *last modified :                                            *
 *                                                           *
 *contents:contains class definition for classes derived from*
 *         Unit class.                                       *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _EXEC_SEMANTICS_H_
#define _EXEC_SEMANTICS_H_

#include <map>

#include "stdafx.h"
//#include "DerivedUnit.h"
#include "DerivedAddrData.h"
#include "OpCodeTable.h"
#include "MyLinkedList.h"
#include "DataHazardDetector.h"

//These macros have been commented since there are now functions
//declared here and defined in ExecSemSupportFuncs.cpp 

//#define NOP globalOpCodeTable.getIndex("NOP")
//#define RETURN globalOpCodeTable.getIndex("RETURN")
//#define CALL globalOpCodeTable.getIndex("CALL")
//#define ASSIGN globalOpCodeTable.getIndex("ASSIGN")
//#define DCONSTANT globalOpCodeTable.getIndex("DCONSTANT")
//#define FCONSTANT globalOpCodeTable.getIndex("FCONSTANT")
//#define ICONSTANT globalOpCodeTable.getIndex("ICONSTANT")
//#define EXP globalOpCodeTable.getIndex("EXP")
//#define SQRT globalOpCodeTable.getIndex("SQRT")
//#define BRANCH globalOpCodeTable.getIndex("IF")
//#define MTC1 globalOpCodeTable.getIndex("MTC1")
//#define DMTC1 globalOpCodeTable.getIndex("DMTC1")
//#define MFC1 globalOpCodeTable.getIndex("MFC1")
//#define DMFC1 globalOpCodeTable.getIndex("DMFC1")
//#define IVSTORE globalOpCodeTable.getIndex("IVSTORE")
//#define DVSTORE globalOpCodeTable.getIndex("DVSTORE")
//#define FVSTORE globalOpCodeTable.getIndex("FVSTORE")
//#define IVLOAD globalOpCodeTable.getIndex("IVLOAD")
//#define DVLOAD globalOpCodeTable.getIndex("DVLOAD")
//#define FVLOAD globalOpCodeTable.getIndex("FVLOAD")

#define EXECUTE_FUNC(p) (_opList.getExecuteFunc(p))
#define EXEC_FUNC(p) (unit->_opList.getExecuteFunc(p))


extern int Branch;
extern int endOfSim;
extern unsigned long clock_count;
extern unsigned long lockstep_clock_count;

extern int pipePhase;

extern DataHazardDetector globalDataHazardDetector;
//extern FUandOpcodeDS globalFUandOpcodeDS;

extern int isStore(int opCode);
extern int isLoad(int opCode);
//The following five functions have been modified/added by
//Srikanth on 5/17/2000
extern int isCall(int opCode);
extern int isNOP(int opCode);
extern int isEXP(int opCode);
extern int isSQRT(int opCode);
extern int isBranch(int opCode);

extern int isRet(int opCode);
extern int isCallRet(int opCode);
extern int isDynamicCall(int opCode);

extern int getSpecialOpCode(MyString & s);
extern int getDynamicCallOpCode();

extern int isSpecialOpCode(int opCode);
extern int isSpecialDataOpCode(int opCode);

extern void setSrcValOperand(Val & src, Val & tmp);

extern void lockStepModeInitTemp();
extern void lockStepModeCalc();
extern void lockStepModeCompute(int opcode);

extern void getStringFromSrcArg(AddrDataUnion & a, MyString & s);
extern int getOpcodeIndex(char *str);


extern void printRegName(int addr);

#endif