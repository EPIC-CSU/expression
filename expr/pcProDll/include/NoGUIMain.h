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
//$Id: NoGUIMain.cc,v 1.5 1998/04/17 19:12:01 pgrun Exp $
//
// File:  		NoGUIMain.cc
// Created:		Mon Dec 28, 98
// Last modified: 	Mon Dec 28, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module:

#ifndef __NOGUIMAIN_H__
#define __NOGUIMAIN_H__

//#include "stdafx.h"

#include "GlobalIncludes.h"

//#include "Routine.h"
//#include "simulator.h"
#include "Program.h"
#include "MyLinkedListIterator.h"
#include "SemActPrimitives.h"
#include "ControlParms.h"
#include "FilePointer.h"
#include "../DebugSwitches.h"
//#include "RedirectOutput.h"

#include "SymbolTable.h"

class Routine;

enum PRINT_WHAT {_ASM_, _IList_, _CFG_, _HTG_, _C_, _DUMP_};

typedef char *PRINT_WHEN;
typedef char *PRINT_MESG;

extern Routine *currRoutine;
extern Program *curProgram;

extern ControlParmTable globalControlParms;

extern void myInit1(void);
extern void myInit2(void);
extern void myBuild(char *);
extern void buildIListCFGLoops();

extern void buildSystem();

extern void resetCompiler();

extern void continueSim();
extern void startSim(Routine *);
extern void singleStep();
extern void finishSim();

extern unsigned long clock_count;
extern unsigned long lockstep_clock_count;
extern unsigned long numLockStepCycles();

extern void openDebugFile(void);
extern void closeDebugFile(void);

extern int printASM, dumpIR, printIList, printCFG, printHTG, printC, afterTbz;
extern int simNoSSA, simSSA, simHTG, simISel, simTbz, simRA;
extern int funcSimNoSSA, funcSimSSA, funcSimHTG, funcSimISel, funcSimTbz, funcSimRA;
extern int doISel, doTbz, doTransMut, doUnroll, doThumb, printMC, doRA;
extern int doIfC, doPerc, doMultiChain;
extern int doPreSchTrans, doPostSchTrans;

extern unsigned long simulateRoutine(Routine *curR, char *mesg);
extern unsigned long simulateProgram(Program *curP, char *mesg);
extern void printSimResult(unsigned long numCycles, char *mesg);
extern void printSimResultToFile(unsigned long numCycles, char *mesg = "");
extern void printCountResultToFile(int numOpers, int numInstrs, char *mesg = "");
void printSimSpeedup(unsigned long htgCycleCount,unsigned long tbzCycleCount);

extern void interpretCmdLineGUI(int argc, char **argv);
extern void interpretCmdLineNoGUI(int argc, char **argv);

extern void buildArchModel();
extern void printFUandOpcodeDS(ostream & out);
extern void initFUandOpcodeDS(char *fileName);

extern void irToC(MyString & s);
extern void outputC(PRINT_WHEN pW, PRINT_MESG pM);
extern void outputC(PRINT_WHEN pW, FilePointer & fp);
 
extern void printProg(PRINT_WHAT a, PRINT_WHEN b);

extern SymbolTable *globalSymbolTable;

extern void addCallNameProperty(Program *prog);
extern void firstPassCallPatch(Program *prog);
extern void secondPassCallPatch(Program *prog);

extern void patchForDoubles(Program *prog);
extern void addCorrectionOps(Program *prog);
extern void remCorrectionOps(Program *prog);
extern void setCallPointers(Program *prog);

extern void setScheduleProperties();

extern void removeGCCSaveRestores(Routine *rout);
extern void addSavesRestores(Routine *rout);

extern MyString globalProgramName;

#endif
