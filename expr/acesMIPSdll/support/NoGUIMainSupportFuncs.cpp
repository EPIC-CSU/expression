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
//$Id$
//
// File:  		NoGUIMainSupportFuncs.cpp
// Created:		Mon Dec 28, 98
// Last modified: 	Mon Dec 28, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module:

#ifdef WIN32
#pragma warning(disable:4786)
#include <map>
#include <vector>
#endif

#ifdef WIN32
#include "defs_node.h"
#include "symbols.h"
#include "symtabs.h"
#endif

// above section added for ASM
#include "stdafx.h"

#include "NoGUIMain.h"
#include "MyString.h"

#include "Routine.h"

//defined in TbzNormalInstr
//if 1, it performs pipelined tbz, othewise non-pipelined. 
extern int pipelinedTbz;
extern int stalledTbz;

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//externals 
extern int linkEXPRESSION;
extern int dbEnabled;
extern int doStorageAlloc;
extern char *EXPR_NAME;
extern char *DEFS_FILE;
extern bool debugSIMULATOR;
int debugSIMULATORL2=0;
int debugSIMULATORL1=0;
int debugSIMULATORL3=0;
int noSystemCalls=0;
//procedure by procedure
int pbp=0;
//extern int memSubsystemTiming;


char start_routine[100] = "_main";

MyString globalProgramName;
MyString memConfigFile;
extern ofstream globalDumpASMFout;

extern bool debugSIMULATOR;

extern void start_time();
extern void reset_time();
extern void pause_time();
extern void print_time();

//prototypes for ASM
void size_all_symbols(void);
void print_labels(ostream &);
void printDataSection();
void printDataSectionHeader(ostream &);
void dumpDataSection(ostream &);
bool PrintingASM;
bool DumpingIR;

Vector<MyString> globalProcsFileNamesList;
Vector<MyString> globalDefsFileNamesList;

int doPassFor(char *rpass, char *rname)
{
	return (globalControlParms.hasEntry(rpass, rname));
}

int doPassFor(MyString & pass, MyString & rout)
{
	return (globalControlParms.hasEntry(pass, rout));
}

unsigned long simulateProgram(Program *curP, char *mesg = "SIMULATING")
{
	clock_count = 0;
	lockstep_clock_count = 0;
	
	/*start simulation*/ 
	
	cout << "**********************************************" << endl;
	cout << " Start simulation : " << mesg << endl <<flush;
	
	
	openDebugFile();	//open the debug.output file in which it outputs the results of simulation
	
	
	// first find the "main" routine.
	//	Routine *curR = curP->getMainRoutine();
	//	currRoutine = curP->getMainRoutine();
	
	currRoutine = curP->getRoutine(start_routine);
	Routine *curR = currRoutine;
	
	//	cout << endl << curP->getName();
	
	//	currRoutine->printName(cout);
	
	if (curR == NULL)
	{
		// error condition.
		//
		cout << "Could not find the main routine to simulate" << endl;
		return 0;
	}
	if (doPassFor("SIMULATOR_PRINT", curR->getName()))
	{
		debugSIMULATOR = 1;
	}
	else
	{
		//debugSIMULATOR = 0;
	}
	
	MyString tmpFileName = globalProgramName.getStr();
	tmpFileName.concat(mesg);
	reset_time();
	start_time();
	startSim(tmpFileName);
	continueSim();
	finishSim(globalProgramName.getStr());
	pause_time();
	
	closeDebugFile();	//close the debug.output file in which it outputs the results of simulation
	
	print_time();
	
	cout << " End simulation : " << mesg << endl;
	cout << "**********************************************" << endl <<flush;
	
	/*end simulation*/
	
	return clock_count;
}

unsigned long numLockStepCycles()
{
	return lockstep_clock_count;
}

void interpretCmdLineGUI(int argc, char **argv){
	printIList = 0;
	printASM = 0; // Added for ASM
	dumpIR = 0; // Added for DumpIR on 15th Feb, 2001
	printCFG = 0;
	printHTG = 0;
	afterTbz = 0;
	doTbz	  = 0;
	doRA = 0;
	doMultiChain = 0;
	pipelinedTbz = 0;
	stalledTbz = 0;
	doMut = 0;
	doUnroll = 0;
	doThumb = 0;
	simHTG = 0;
	linkEXPRESSION = 0;
	EXPR_NAME = NULL;
	DEFS_FILE = NULL;
	dbEnabled=0;
	doStorageAlloc=0;
}

void interpretCmdLineNoGUI(int argc, char **argv)
{
	MyString a;
	
	for (int i = 1; i < argc; i++)
    {
		if (strstr(argv[i], "procs") != NULL)
		{
			a = argv[i];
			globalProcsFileNamesList.push_back(a);
			printf("\nProcs file name: %s\n",argv[i]);
			continue;
		}
		if (strstr(argv[i], "defs") != NULL)
		{
			a = argv[i];
			globalDefsFileNamesList.push_back(a);
			continue;
		}
		if (strcmp(argv[i], "-name") == 0)
		{
			i++;
			globalProgramName = argv[i];
			continue;
		}
		if (strcmp(argv[i], "-memCfg") == 0)
		{
			i++;
			memConfigFile = argv[i];
			continue;
		}
		if (strcmp(argv[i], "-pASM") == 0)
		{
			printASM = 1;
			continue;
		} // Added for ASM
		if (strcmp(argv[i], "-pDUMP") == 0)
		{
			dumpIR = 1;
			continue;
		} // Added for ASM
		if (strcmp(argv[i], "-pIList") == 0)
		{
			printIList = 1;
			continue;
		}
		if (strcmp(argv[i], "-pCFG") == 0)
		{
			printCFG = 1;
			continue;
		}
		if (strcmp(argv[i], "-pHTG") == 0)
		{
			printHTG = 1;
			continue;
		}
		if (strcmp(argv[i], "-pC") == 0)
		{
			printC = 1;
			continue;
		}
		if (strcmp(argv[i], "-pTbz") == 0)
		{
			afterTbz = 1;
			continue;
		}
		if (strcmp(argv[i], "-sNoSSA") == 0)
		{
			simNoSSA = 1;
			continue;
		}
		if (strcmp(argv[i], "-sSSA") == 0)
		{
			simSSA = 1;
			continue;
		}
		if (strcmp(argv[i], "-sHTG") == 0)
		{
			simHTG = 1;
			continue;
		}
		if (strcmp(argv[i], "-sISel") == 0)
		{
			simISel = 1;
			continue;
		}
		if (strcmp(argv[i], "-sTbz") == 0)
		{
			simTbz = 1;
			continue;
		}
		if (strcmp(argv[i], "-sRA") == 0)
		{
			simRA = 1;
			continue;
		}
		if (strcmp(argv[i], "-fsNoSSA") == 0)
		{
			funcSimNoSSA = 1;
			continue;
		}
		if (strcmp(argv[i], "-fsSSA") == 0)
		{
			funcSimSSA = 1;
			continue;
		}
		if (strcmp(argv[i], "-fsHTG") == 0)
		{
			funcSimHTG = 1;
			continue;
		}
		if (strcmp(argv[i], "-fsISel") == 0)
		{
			funcSimISel = 1;
			continue;
		}
		if (strcmp(argv[i], "-fsTbz") == 0)
		{
			funcSimTbz = 1;
			continue;
		}
		if (strcmp(argv[i], "-fsRA") == 0)
		{
			funcSimRA = 1;
			continue;
		}
		if (strcmp(argv[i], "-Tbz") == 0)
		{
			doTbz = 1;
			continue;
		}
		if (strcmp(argv[i], "-RA") == 0)
		{
			doRA = 1;
			continue;
		}
		if (strcmp(argv[i], "-MultiChain") == 0)
		{
			doMultiChain = 1;
			continue;
		}
		if (strcmp(argv[i], "-PreSch") == 0)
		{
			doPreSchTrans = 1;
			continue;
		}
		if (strcmp(argv[i], "-PostSch") == 0)
		{
			doPostSchTrans = 1;
			continue;
		}
		if (strcmp(argv[i], "-IfC") == 0)
		{
			doIfC = 1;
			continue;
		}
		if (strcmp(argv[i], "-Perc") == 0)
		{
			doPerc = 1;
			continue;
		}
		if (strcmp(argv[i], "-ISel") == 0)
		{
			doISel = 1;
			continue;
		}
		if (strcmp(argv[i], "-pipeTbz") == 0)
		{
			pipelinedTbz = 1;
			continue;
		}
		if (strcmp(argv[i], "-stalledTbz") == 0)
		{
			stalledTbz = 1;
			continue;
		}
		if (strcmp(argv[i], "-Mut") == 0)
		{
			doMut = 1;
			continue;
		}
		if (strcmp(argv[i], "-Unroll") == 0)
		{
			doUnroll = 1;
			continue;
		}
		// Thumb instructions
		if (strcmp(argv[i], "-Thumb") == 0)
		{
			doThumb = 1;
			continue;
		}

		if (strcmp(argv[i], "-pMC") == 0)
		{
			printMC = 1;
			continue;
		}
		if (strcmp(argv[i], "-EXPR") == 0)
		{
			linkEXPRESSION = 1;
			printf("\n-EXPR Expression Linked\n");
			continue;
		}
		if (strcmp(argv[i], "-DB") == 0)
		{
			dbEnabled = 1;
			continue;
		}
		if (strcmp(argv[i], "-REM") == 0)
		{
			dbEnabled = 2;
			continue;
		}
		if (strcmp(argv[i], "-SA") == 0)
		{
			doStorageAlloc = 1;
			continue;
		}
		if (strcmp(argv[i], "-ENAME") == 0)
		{
			EXPR_NAME=strdup(argv[i+1]);
			i++;
			continue;
		}
		if (strcmp(argv[i], "-debugSimulator") == 0)
		{
			debugSIMULATOR = 1;
			continue;
		}
		if (strcmp(argv[i], "-debugSimulatorL2") == 0)
		{
			debugSIMULATORL2 = 1;
			continue;
		}
		if (strcmp(argv[i], "-debugSimulatorL1") == 0)
		{
			debugSIMULATORL1 = 1;
			continue;
		}
		if (strcmp(argv[i], "-SimRout") == 0)
		{
			strcpy(start_routine, argv[++i]);
			continue;
		}
		if (strcmp(argv[i], "-debugSimulatorL3") == 0)
		{
			debugSIMULATORL3 = 1;
			continue;
		}
		/*if (strcmp(argv[i], "-memSubsystemTiming") == 0)
		{
			memSubsystemTiming = 1;
			printf("\n-memSubsystem timing ON\n");
			continue;
		}*/
		if (strcmp(argv[i], "-SimRout") == 0)
		{
			strcpy(start_routine, argv[++i]);
			continue;
		}
		if (strcmp(argv[i], "-noSystemCalls") == 0)
		{
			noSystemCalls=1;
			continue;
		}
		if (strcmp(argv[i], "-pbp") == 0) //procedure by procedure
		{
			pbp=1;
			continue;
		}
		
		cout << "Can't understand the option "  << argv[i] << endl;
		cout << "The following options are supported : " << endl;
		cout << "\t-pIList" << endl;
		cout << "\t-pCFG" << endl;
		cout << "\t-pHTG" << endl;
		cout << "\t-pC" << endl;
		cout << "\t-pTbz" << endl;
		cout << "\t-sNoSSA" << endl;
		cout << "\t-sSSA" << endl;
		cout << "\t-sHTG" << endl;
		cout << "\t-sISel" << endl;
		cout << "\t-sTbz" << endl;
		cout << "\t-sRA" << endl;
		cout << "\t-fsNoSSA" << endl;
		cout << "\t-fsSSA" << endl;
		cout << "\t-fsHTG" << endl;
		cout << "\t-fsISel" << endl;
		cout << "\t-fsTbz" << endl;
		cout << "\t-fsRA" << endl;
		cout << "\t-Tbz" << endl;
		cout << "\t-RA" << endl;
		cout << "\t-ISel" << endl;
		cout << "\t-pipeTbz" << endl;
		cout << "\t-stalledTbz" << endl;
		cout << "\t-Mut" << endl;
		cout << "\t-Unroll" << endl;
		cout << "\t-pMC" << endl;
		cout << "\t-EXPR" << endl;
		cout << "\t-DB" << endl;
		cout << "\t-REM" << endl;
		cout << "\t-SA" << endl;
		cout << "\t-ENAME" << endl;
		cout << endl;
		//also update CreateCommandLineParams() in pcProGUIView.cpp
    }
} // end of void interpretCmdLineNoGUI(int argc, char **argv)


void printSimResult(unsigned long numCycles, char *mesg = "")
{
	cout << endl << "#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#" << endl;
	cout << "Number of Cycles (" << mesg << "): " << numCycles << endl;
	cout << "#-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-#" << endl;
}

void printSimSpeedup(unsigned long htgCycleCount,unsigned long tbzCycleCount){
	FILE *fp=fopen("express.res","a");
	
	fprintf(fp,"%9d\t%9d\t%.2f\n",htgCycleCount,tbzCycleCount,
		(float)htgCycleCount/(float)tbzCycleCount);
	
	fclose(fp);
}

void printSimResultToFile(unsigned long numCycles, char *mesg)
{
	FILE * fp;
	fp = fopen("express.res","a");
	
	if (!strcmp(mesg,"After InstrSel, Before Tbz") || !strcmp(mesg,"After HTG") 
		|| !strcmp(mesg,"Before SSA"))
		fprintf(fp,"					%9d\n",numCycles);
	else if (!strcmp(mesg,"After Tbz") || !strcmp(mesg, "After RA"))
		fprintf(fp,"%9d\n",numCycles);
	else{
		fprintf(fp,"Unknown command");
	}
	
	fclose(fp);
}

extern void initGlobalOpCodeTable(char *str, int genericOrTarget, int initGenericEnums);

void addTargetOpcodes(void)
{
	// This is information required by certain passes (such as if-conversion, register-allocation, etc)
	//
	// This is, obviously, not on a routine-by-routine basis
	// 
	// Currently, only the targetOpcodeFile is read in here.
	//
	initGlobalOpCodeTable("targetOpCodeFile", _TARGET_, _NO_);
}

void addTargetRegFileList(void)
{
}

extern void initRegArchModel();

void buildArchModel()
{
	initFUandOpcodeDS("opLatencyFile");

	addTargetOpcodes();
	//	printFUandOpcodeDS(cout);

	initRegArchModel();
}

void resetCompiler()
{
	if (curProgram != NULL)
		delete curProgram;
	curProgram = new Program();
}

void irToC(MyString & s)
{
	curProgram->irToC(s);
}

ofstream globalDumpASMFout;

void printProg(char *pName, MyString & pW, ofstream & fout)
{
	MyString rName;
	
	MyLinkedListIterator<Routine *> *routineIter;
	routineIter = curProgram->routineListIteratorForw();

	globalDumpASMFout = fout;

	if (PrintingASM || DumpingIR)
	{
		printDataSection();

		if( PrintingASM )
			fout << "\t.text\t" << endl;  // EXPR text file
	}
	
	while (routineIter->hasMoreElements())
	{ 
		rName = (routineIter->currentElement())->getName();
		if (doPassFor(pW, rName))
		{
			(routineIter->currentElement())->doPass(pName);
		}
		routineIter->nextElement();
	}
	delete routineIter;
}

void printProg(char *pName, MyString & pW)
{
	PrintingASM = false;
	DumpingIR = false;

	MyString tempName = globalProgramName;

	ofstream fout;
	
	if( strcmp(pName, "PRINT_ASM") == 0)
	{
		PrintingASM = true;
	}
	else if (strcmp(pName, "DUMP_IR") == 0)
	{
		DumpingIR = true;
	}

	if (PrintingASM || DumpingIR)
	{
		MyString expressAll("EXPRESS_all");

		if (!doPassFor(pW, expressAll))
			return;

		tempName.concat("_");
		tempName.concat(pW.getStr());
		tempName.concat(".txt");
		fout.open(tempName.getStr());
		if( !fout )
		{
			cerr << "Unable to open" << tempName.getStr() << "in append mode " << endl;
			assert(0);
		}
	}
	printProg(pName, pW, fout);
	if (( strcmp(pName, "PRINT_ASM") == 0) || (strcmp(pName, "DUMP_IR") == 0))
	{
		fout.close();
	}
}

void printProg(PRINT_WHAT pT, PRINT_WHEN pW)
{
	if ((printIList) && (pT == _IList_))
	{
		MyString a("PRINT_ILIST_");
		a.concat(pW);
		printProg("PRINT_ROUTINE", a);
	}
	else if ((printCFG) && (pT == _CFG_))
	{
		MyString a("PRINT_CFG_");
		a.concat(pW);
		printProg("PRINT_CFG", a);
	}
	else if ((printASM) && (pT == _ASM_))
	{
		MyString a("PRINT_ASM_");
		a.concat(pW);
		printProg("PRINT_ASM", a);
	} // Added for ASM
	else if ((dumpIR) && (pT == _DUMP_))
	{
		MyString a("DUMP_IR_");
		a.concat(pW);
		printProg("DUMP_IR", a);
	} // Added for dump ir
	if ((printHTG) && (pT == _HTG_))
	{
		MyString a("PRINT_HTG_");
		a.concat(pW);
		printProg("PRINT_HTG", a);
	}
	if ((printC) && (pT == _C_))
	{
		MyString a("PRINT_C_");
		a.concat(pW);
		printProg("PRINT_C", a);
	}
}

void outputC(PRINT_WHEN pW, PRINT_MESG pM)
{
	if (!printC)
		return;

	MyString s(pM);

	MyString pass("OUTPUT_C_");
	pass.concat(pW);

	if (doPassFor(pass, allRoutines))
	{
		curProgram->irToC(s);
	}
}

void outputC(PRINT_WHEN pW, FilePointer & fp)
{
	if (!printC)
		return;

	MyString pass("OUTPUT_C_");
	pass.concat(pW);

	if (doPassFor(pass, allRoutines))
	{
		curProgram->irToC(fp);
	}
}

extern void dumpDataSection(ostream &);

void printDataSection()
{
	if( PrintingASM == true )
	{
		printDataSectionHeader(globalDumpASMFout);  // EXPR text file
		print_labels(globalDumpASMFout);
	}
	else if( DumpingIR == true )
	{
		dumpDataSection(globalDumpASMFout);
	}
}

void printDataSectionHeader(ostream &out)
{
	out << "\t.data\t" << endl;
	out << "\t.globl\t" << "register_alloc" << endl;
	out << "\t.size\t" << "register_alloc, 148" << endl;
	out << "register_alloc :" << endl;
	out << "\t.byte\t148" << endl;
}

extern void addCorrectionOps(Routine *rout);
extern void remCorrectionOps(Routine *rout);

void addCorrectionOps(Program *prog)
{
	MyLinkedListIterator<Routine *> *routineIter;
	routineIter = curProgram->routineListIteratorForw();

	while (routineIter->hasMoreElements())
	{
		addCorrectionOps(routineIter->currentElement());
		routineIter->nextElement();
	}
	delete routineIter;
}

void remCorrectionOps(Program *prog)
{
	MyLinkedListIterator<Routine *> *routineIter;
	routineIter = curProgram->routineListIteratorForw();

	while (routineIter->hasMoreElements())
	{
		remCorrectionOps(routineIter->currentElement());
		routineIter->nextElement();
	}
	delete routineIter;
}
