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
// File:  		NoGUIMain.h
// Created:		Wed Nov 20, 96
// Last modified: 	Mon Dec 28, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module:


#include "stdafx.h"

#include "NoGUIMain.h"
#include "PropertyIncludes.h"

#ifndef __UNIX__
#include "../../pcProGUI/SIM/SIMDoc.h"
#include "conio.h"
#endif

#include "StatList.h"
#include "ArchStats.h"

#include "IRTraversal.h"
#include "Routine.h"
#include "PropertyIncludes.h"

#include "../acesMIPSFuncSimulator/FuncSim.h"
#include "../IfConversion/ReadPredicationInformation.h"

#include "../IfConversion/PredicationInformation.h"

extern ArchStatsDB globalArchStatsDB;

//#include <mapidefs.h>
#include <time.h>

#ifndef __UNIX__
#include <process.h>
#else
#define __declspec(dllexport)
#define __stdcall
#define _ASSERT assert
#include <stdio.h>
#endif

__declspec (dllexport) bool volatile lockPCPRO=false;
__declspec (dllexport) bool volatile stepPCPRO=false;
__declspec (dllexport) bool volatile donePCPRO=false;


//link expression compiler into express
#define __EXPRESSION__ 1

//ATTENTION
#define _NO_SYSTEM_CALLS_

// Added by Sumit Oct 19, 1999
#ifdef __EXPRESSION__
//prototypes from EXPRESSION
__declspec( dllimport ) int expression_main(int argc, char **argv);
//__declspec( dllimport ) void setTreadInfo(THREADINFO *t);
__declspec( dllimport ) void print_db_size(void);
#endif

//prototypes
void SpreadInstrsRoutine(Routine *routine);

//global_memory_stalls counts the memory stalls
//defined in ExecSemantics.cpp
extern int global_memory_stalls;

//procedure by procedure
extern int pbp;

//prototypes from EXPRESSION
__declspec( dllimport ) void print_db_size(void);


//other prototypes
void read_defs_file(Vector<MyString> & name_list);
void append_defs_file_names(void);
void redo_usedef_live(void);
void close_pipeline_tbz(void);

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//Variables for command line options
int printIList = 0, 
printASM = 0,  // Added for ASM
dumpIR = 0,  // Added for dumping IR
printCFG = 0, 
printHTG = 0, 
  printC = 0, 
afterTbz = 0,
simNoSSA = 0,
simSSA   = 0,
simHTG   = 0,
simISel  = 0,
simTbz   = 0,
simRA	 = 0,
funcSimNoSSA = 0,
funcSimSSA   = 0,
funcSimHTG   = 0,
funcSimISel  = 0,
funcSimTbz   = 0,
funcSimRA	 = 0,
doISel   = 0,
doTbz	   = 0,
doRA = 0,
doThumb = 0,
doMultiChain = 0,
doMut	   = 0,
doUnroll = 0,
printMC  = 0,
doIfC    = 0, 
doPerc   = 0,
linkEXPRESSION = 0,
dbEnabled=0,
doStorageAlloc=0,
doPreSchTrans=0, // Added by SAH on 20th Sept., 2000 for Prescheduling transformations
doPostSchTrans=0; // Added by SAH on 20th Sept., 2000 for Postscheduling transformations

//name of the expression file, read from the cmd line
char *EXPR_NAME = NULL;

//name of the defs file, read from the cmd line
char *DEFS_FILE = NULL;

//number of scheduling attempts
long RTinquiries=0;

//prototypes
extern int pipelinedTbz;
extern int stalledTbz;



int doneSSA = 0;
int doneISel = 0;
int doneParallelization = 0;
int doneRA = 0;

static bool dllInitialized=false;
#ifdef WIN32
__declspec( dllexport ) THREADINFO threadInfo;
#endif

//#ifndef MOTIF_GUI


#ifndef __UNIX__
#include "../../pcProGUI/ProgramParams.h"
#else
#include "ProgramParams.h"
#endif

ProgramParams* ArgcArgv;
#ifdef WIN32_GUI
SIMDoc* simDoc;
#endif

//time variables
/*SYSTEMTIME startTime,stopTime;
FILETIME startFileTime, stopFileTime;
LARGE_INTEGER start,stop;*/
clock_t start1,stop1;
double  duration=0;
//double startu, stopu,startk,stopk;

void start_time(void){
	//start counting time
	start1 = clock();
	/*
	VERIFY(GetThreadTimes(threadInfo.handle,
	&threadInfo.creationTime,
	&threadInfo.exitTime,
	&threadInfo.kernelTime,
	&threadInfo.userTime)); 
	
	  //if compile time is > 429.4967296 seconds must also use the threadInfo.*.dwHighDateTime
	  startu=(double)threadInfo.userTime.dwLowDateTime/10000000.0;
	  startk=(double)threadInfo.kernelTime.dwLowDateTime/10000000.0;
	*/
}

void reset_time(void)
{
	duration = 0;
}

void pause_time(void){
	stop1=clock();
	duration +=(double)(stop1 - start1);
	
	/*VERIFY(GetThreadTimes(threadInfo.handle,
	&threadInfo.creationTime,
	&threadInfo.exitTime,
	&threadInfo.kernelTime,
	&threadInfo.userTime)); 
	
	  //if compile time is > 429.4967296 seconds must also use the threadInfo.*.dwHighDateTime
	  stopu=(double)threadInfo.userTime.dwLowDateTime/10000000.0;
	  stopk=(double)threadInfo.kernelTime.dwLowDateTime/10000000.0;
	  double userTime=stopu-startu;
	  double kernelTime=stopk-startk;
	  
	duration += (double)(userTime);*/
}

void print_time(void){
	FILE *f;
	
	printf( "\n***TIME*** : %2.1f secs\n", duration / (double)CLOCKS_PER_SEC );
	//printf( "\nTrailblazing time: %2.2f\n", duration );
	
	//write to the file too:
	f=fopen("tbztime.txt","a");
	fprintf(f," & %2.1fs ", duration / (double)CLOCKS_PER_SEC );
	//fprintf(f," & %2.1fs ", duration );
	//close file
	fclose(f);
}

__declspec (dllexport) void buildProgramMappings()
{
	//build the instr walker and op mapper for each routine
	MyLinkedListIterator<Routine *> *i = curProgram->routineListIteratorForw();
	while (i->hasMoreElements())
	{
		Routine* r=i->currentElement();
		
		if (r->iWalker!=NULL) delete r->iWalker;
  r->iWalker=new WalkInstr(WALK_BACKWARDS, WALK_CF, NORMAL_MODE);;

		if (r->oMapper!=NULL) delete r->oMapper;
  r->oMapper=new OpToInstrMap(DC_OP, NORMAL_MODE);
		
		r->iWalker->initWalk(r);
		//r->iWalker->print(cout);
		
		r->oMapper->initOpToInstrMap(r);
		//r->oMapper->print(cout);
		
		i->nextElement();
	}
	delete i;
	
}

void print_sched_attempts(void){
	FILE *f;
	
	printf( "\nScheduling attempts: %d\n", RTinquiries );
	
	//write to the file too:
	f=fopen("tbztime.txt","a");
	fprintf(f," & %d ", RTinquiries );
	//close file
	fclose(f);
}

__declspec( dllexport ) unsigned int __stdcall run_simulator(void* _cp);

__declspec( dllexport ) void run_simulator_thread(void){
	//create a "gentle" thread
#ifdef WIN32
	unsigned thrdaddr;
	HANDLE hO=GetStdHandle(STD_OUTPUT_HANDLE);
#endif
	
	ProgramParams pp;
	
	pp.argc=0;
	pp.argv=NULL;
	
	cout << "Starting SIMPRESS..." << endl;
	
#ifdef __UNIX__
	run_simulator(&pp);
#else  // its win32
	threadInfo.handle=(HANDLE)_beginthreadex(NULL, 0, run_simulator, &pp, CREATE_SUSPENDED, &thrdaddr);
	SetThreadPriority(threadInfo.handle,THREAD_PRIORITY_BELOW_NORMAL); //THREAD_PRIORITY_IDLE);
	//SetThreadPriority(threadInfo.handle,THREAD_PRIORITY_ABOVE_NORMAL); //THREAD_PRIORITY_IDLE);
	ResumeThread(threadInfo.handle); 
	//WaitForSingleObject(threadInfo.handle,INFINITE); 
#endif // end of else of ifdef __UNIX
	//cout << endl << endl << "Leaving SIMPRESS..." << endl;
}

//runs the simulator and prints out the message
//message: input, the time when this simulation is done (e.g., after HTG, before Tbz)
//curProgram: input, the current program to simulate
__declspec( dllexport ) unsigned int __stdcall run_simulator(void *_cp){
	char buf[2000];
	char message[10]="";
	unsigned long simCycles;
	unsigned long htgCycleCount; //stores the number of cycles for the simulation before HTG
	
	sprintf(buf,"%s, LockStep",message);
	
	simCycles = simulateProgram(curProgram, message);
	
#ifdef WIN32
	VERIFY(GetThreadTimes(threadInfo.handle,
		&threadInfo.creationTime,
		&threadInfo.exitTime,
		&threadInfo.kernelTime,
		&threadInfo.userTime)); 
	
	//if compile time is > 429.4967296 seconds must also use the threadInfo.*.dwHighDateTime
	double userTime=(double)threadInfo.userTime.dwLowDateTime/10000000.0;
	double kernelTime=(double)threadInfo.kernelTime.dwLowDateTime/10000000.0;
	
	cout << endl << "==============================================================================" << endl ;
	cerr << "SIMULATION TIME: " << userTime + kernelTime << " secs (user: " << userTime << " secs) (kernel: " << kernelTime << " secs)" << endl ;
	cout << "==============================================================================" << endl ;
	
	
	//make this thread wait until we say so
	//while (lockPCPRO);
	
#endif
	
	printSimResult(simCycles, message);
	printSimResult(numLockStepCycles(), buf);
	printSimResultToFile(simCycles, message);
	htgCycleCount=numLockStepCycles();
	
	return 0;
}

//builds the SSA for the program
__declspec( dllexport ) void build_SSA(void){
	MyLinkedListIterator<Routine *> *routineIter;
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		currRoutine->doPass("BUILD_SSA");
		
		routineIter->nextElement();
	}
	delete routineIter;
}

//GG: added to try to fix the memory allocation problem, 7/17/01
char *dummy_space;

void allocate_dummy_space(void){
	dummy_space=new char[10000000];
	//dummy_space=(char*)malloc(10000000);
}

extern void buildSSA(Program *prog);

//builds the HTG for the program
__declspec( dllexport ) void build_HTG(void){
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		currRoutine->doPass("BUILD_HTG");
		currRoutine->setInstrID();
		
		routineIter->nextElement();
	}
	delete routineIter;
}


//usedef and Live analysis
__declspec( dllexport ) void usedef_live(void){
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		currRoutine->doPass("COMP_DEFS");
		currRoutine->doPass("COMP_UDCHAIN");   
		currRoutine->doPass("COMP_LIVEDEAD");   
		//	currRoutine->doPass("TEST_DISAMB");
		
		routineIter->nextElement();
	}
	delete routineIter;
}


//compute multichains

__declspec( dllexport ) void build_multichains(void){
	
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		currRoutine->doPass("COMP_MULTICHAINS");
		
		routineIter->nextElement();
	}
	delete routineIter;
}


// register allocation
__declspec( dllexport ) void regAlloc(void){
	
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		currRoutine->doPass("REG_ALLOC");
		
		routineIter->nextElement();
	}
	delete routineIter;
}

//do Trailblazing for the whole program
__declspec( dllexport ) void trailblazing(void){
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		
		//start_time();

		if(pipelinedTbz){
			//spread instrs to account for pipeline hazards
			redo_usedef_live();
			currRoutine->doPass("SPREAD_INSTRS");
			redo_usedef_live();
//			if (afterTbz) curProgram->printHTG();
		}
		//currRoutine->setInstrID();
		//do trailblazing
		
		currRoutine->doPass("TRAILBLAZING");

		//this only needed for VLIW, not for Superscalar
		if(pipelinedTbz || stalledTbz){
			//run again spread instrs to reinsert the needed empty instructions
			SpreadInstrsRoutine(currRoutine);
		}
		
		if(!dbEnabled)
			print_sched_attempts();
		
		print_time();
		
#ifdef __EXPRESSION__
		if(dbEnabled==1 || dbEnabled==2){
			print_db_size();
		}
#endif
		routineIter->nextElement();
	}
	delete routineIter;

#ifdef __EXPRESSION__
	if(dbEnabled==1 || dbEnabled==2){
		close_pipeline_tbz();
	}
#endif

	doneParallelization = 1;
}

//do ifConversion for the whole program
__declspec( dllexport ) void ifConverting(void)
{
	extern PredicationInformation GlobalPredicationInformation;

	GlobalPredicationInformation.ReadPredicationInformation();
	GlobalPredicationInformation.PrintPredicationInformation();
	
	MyLinkedListIterator<Routine *> *routineIter;

	extern PredicationInformation GlobalPredicationInformation;

	GlobalPredicationInformation.ReadPredicationInformation();
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		
		currRoutine->doPass("IFCONVERTING");

		routineIter->nextElement();
	}
	delete routineIter;
}

//do percolation for the whole program
__declspec( dllexport ) void percolating(void)
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		
		currRoutine->doPass("PERCOLATING");

		routineIter->nextElement();
	}
	delete routineIter;

	doneParallelization = 1;
}

void prescheduling_transformations()
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		currRoutine->doPass("DEAD_CODE_ELIM");
		currRoutine->deadLabelElimination();
		routineIter->nextElement();
	}
	delete routineIter;
}

void postscheduling_transformations()
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		currRoutine->doPass("COPY_ELIM");
		routineIter->nextElement();
	}
	delete routineIter;
}

extern void myInit3(void);

void initCompilerDS(void)
{
	myInit3();
}
void recompute_USEDEF_properties(Routine *rout);
void recompute_LIVEDEAD_properties(Routine *rout);

void loop_transformations(void)
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
//		currRoutine->genIBS();
		if (doUnroll)
		{
			currRoutine->doPass("TEST_UNROLL");
			doneParallelization = 1;
//			currRoutine->doPass("TEST_SHIFT");
//			recompute_USEDEF_properties(currRoutine);
//			recompute_LIVEDEAD_properties(currRoutine);
		}
		currRoutine->fixIfHead();

		routineIter->nextElement();
	}
	delete routineIter;
}
		
void ThumbPass(void)
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		currRoutine->doPass("THUMB");
		routineIter->nextElement();
	}
	delete routineIter;
}
		

void resetInstrIDs()
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		
		//set the instruction and operation IDs
		currRoutine->setInstrID();

		routineIter->nextElement();
	}
	delete routineIter;
}

const int TEMP_START_NUM = 33;
extern int TEMP_CREATION_NUM;

//do Mutations and InstrSelection
void mutation_instrsel(void){
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		
		//set the instruction and operation IDs
		currRoutine->setInstrID();

		TEMP_CREATION_NUM = TEMP_START_NUM;
		
		//printf("\nBefore Instr sel");
	//	currRoutine->printHTG();
		
		if(doMut)
			currRoutine->doPass("MUTATION");
		if(doISel){
			currRoutine->doPass("INSTRSEL");
//			curProgram->printHTG();
		}
				
		routineIter->nextElement();
	}
	delete routineIter;

	if (doISel)
		doneISel = 1;
}

/*
// Added by SAH on Wed, Jul 12, 2000.
//
// Delete the old cfg, htg data structures and rebuild them.
//
void rebuild_cfg_htg(void)
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);

		currRoutine->rebuildCFG();
		currRoutine->rebuildHTG();

		routineIter->nextElement();
	}
	delete routineIter;
}
*/

extern void buildIListCFGLoops();

void rebuild_cfg_htg(void)
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);

		(currRoutine->getCFG()).reset();
		(currRoutine->getLoopList()).deleteListOfLoops();

		routineIter->nextElement();
	}
	delete routineIter;

	buildIListCFGLoops();

	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->rebuildHTG();
		currRoutine->fixIfHead();
		routineIter->nextElement();
	}
	delete routineIter;
}

//do storage allocation
void storage_alloc(void){
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);
		if(doStorageAlloc)
			currRoutine->doPass("STORAGE_ALLOCATION");
		
		routineIter->nextElement();
	}
	delete routineIter;
}

extern int doPassFor(char *rpass, char *rname);

void build_loops(void)
{
	MyLinkedListIterator<Routine *> *routineIter;
	
	routineIter = curProgram->routineListIteratorForw();
	while (routineIter->hasMoreElements())
	{
		currRoutine = routineIter->currentElement();
		currRoutine->printName(cout);

		currRoutine->buildLoopList();
		currRoutine->getStatusObj().addDynamicTransformation("BUILD_LOOPLIST", _YES_);
		
		if (doPassFor("PRINT_LOOPS", currRoutine->getName()))
		{
			currRoutine->printLoops();
			currRoutine->getStatusObj().addDynamicTransformation("PRINT_LOOPS", _YES_);
		}
		
		routineIter->nextElement();
	}
	delete routineIter;
}

int debugProperties=0;
int debugRAProperties=0;
int debugBBProperties=0;
int debugIG = 0;


//prints out a list of properties
void print_properties(void){
	MyLinkedListIterator<Routine *> *rIter = curProgram->routineListIteratorForw();
	PropNameList pL;
	
	pL.push_back(_HLINFO_);
	
	rIter = curProgram->routineListIteratorForw();
	while (rIter->hasMoreElements())
	{
		(rIter->currentElement())->printProperties(cout, pL);
		rIter->nextElement();
	}
	delete rIter;
}


//prints out properties
void print_RAproperties(PropertyNames prop){
	MyLinkedListIterator<Routine *> *rIter = curProgram->routineListIteratorForw();
	PropNameList pL;
	
	pL.push_back(prop);
		
	while (rIter->hasMoreElements())
	{
		(rIter->currentElement())->printProperties(cout, pL);
		rIter->nextElement();
	}
	delete rIter;
}

// delete USEDEF properties and re-compute.
//
void recompute_USEDEF_properties(Routine *rout)
{
	if (rout == NULL) return;

		rout->recursiveRemoveProperty(_COMPDEF_);
		rout->recursiveRemoveProperty(_UDCHAIN_);
		rout->recursiveRemoveProperty(_DUCHAIN_);

		rout->compDefs();
		rout->compUDChain();
}

// delete USEDEF properties and re-compute.
//
void recompute_USEDEF_properties()
{
	MyLinkedListIterator<Routine *> *rIter = curProgram->routineListIteratorForw();
			
	while (rIter->hasMoreElements())
	{
		Routine *crtRoutine = rIter->currentElement();

		// delete Use-Def properties.
		crtRoutine->recursiveRemoveProperty(_COMPDEF_);
		crtRoutine->recursiveRemoveProperty(_UDCHAIN_);
		crtRoutine->recursiveRemoveProperty(_DUCHAIN_);

		crtRoutine->compDefs();
		crtRoutine->compUDChain();

		rIter->nextElement();
	}
	delete rIter;
}

void recompute_LIVEDEAD_properties(Routine *rout)
{
	if (rout == NULL) return;

	rout->recursiveRemoveProperty(_LIVEDEAD_);
	rout->compLiveDead();
}

//delete LIVEDEAD properties
void recompute_LIVEDEAD_properties(){
	MyLinkedListIterator<Routine *> *rIter = curProgram->routineListIteratorForw();
			
	while (rIter->hasMoreElements())
	{
		Routine *crtRoutine = rIter->currentElement();

		crtRoutine->recursiveRemoveProperty(_LIVEDEAD_);

		// recompute LIVEDEAD properties
		crtRoutine->compLiveDead();

		rIter->nextElement();
	}
	delete rIter;
}

void redo_usedef_live(void)
{
	recompute_USEDEF_properties();
	recompute_LIVEDEAD_properties();
}


//prints out BB properties
void print_BBproperties(PropertyNames prop){
	MyLinkedListIterator<Routine *> *rIter = curProgram->routineListIteratorForw();
			
	while (rIter->hasMoreElements())
	{
		LinkedListIterator<BasicBlockNode> *bbIter = rIter->currentElement()->getCFG().createBBIterForw();
		while (bbIter->hasMoreElements()) {
			BasicBlockNode& bb = bbIter->currentElement();
			
			bb.printProperty(prop);	

			bbIter->nextElement();
		}
		delete bbIter;

		rIter->nextElement();
	}
	delete rIter;
}

void print_RoutLiveDeadProperties(Routine *rout)
{
	if (rout == NULL) return;

	BaseBBProperty *ldProp;

	LinkedListIterator<BasicBlockNode> *bbIter = rout->getCFG().createBBIterForw();
	while (bbIter->hasMoreElements()) {
		BasicBlockNode& bb = bbIter->currentElement();
		
		cout << "BasicBlockNode DFS:" << bb.getDFSNum() << " (* " << &bb << " *) " << endl;

		ldProp = bb.getPtrToProperty(_LIVEDEAD_);

		if (ldProp != NULL)
		{
			MyLinkedListIterator<int> *iter = ((LiveDeadBBProperty *)ldProp)->liveListIterForw();

			int count = 0;
			cout << "\t";
			while (iter->hasMoreElements())
			{
				cout << iter->currentElement() << " ";
				count++;

				if (count > 8)
				{
					count = 0;
					cout << endl;
					cout << "\t";
				}
				iter->nextElement();
			}
			delete iter;
		}
		bbIter->nextElement();
		cout << endl;
	}
	delete bbIter;
}

//prints out BB properties
void print_LiveDeadProperties(Routine *rout){

	if (rout == NULL)
	{
		MyLinkedListIterator<Routine *> *rIter = curProgram->routineListIteratorForw();
				
		while (rIter->hasMoreElements())
		{
			print_RoutLiveDeadProperties(rIter->currentElement());
			rIter->nextElement();
		}
		delete rIter;
	}
	else
		print_RoutLiveDeadProperties(rout);
}


//deallocates the data structures for the routines
void delete_routines(void){
	MyLinkedListIterator<Routine *> *routineIter;
	Routine *currRoutine;
	LinkedListIterator<BasicBlockNode> *bbIter;
		
	//for each routine
	for(routineIter = curProgram->routineListIteratorForw();routineIter->hasMoreElements();routineIter->nextElement())
	{
		//this is the current routine
		currRoutine = routineIter->currentElement();

		// delete LIVEDEAD properties
		bbIter = currRoutine->getCFG().createBBIterForw();
		while (bbIter->hasMoreElements()) {
			BasicBlockNode& bb = bbIter->currentElement();	
			bb.removeProperty(bb.getProperty(_LIVEDEAD_));
			bbIter->nextElement();
		}
		delete bbIter;
	}
	delete routineIter;

	//for each routine in the current file
	for(routineIter = curProgram->routineListIteratorForw();routineIter->hasMoreElements();routineIter->nextElement())
	{
		currRoutine = routineIter->currentElement();
		(currRoutine->getCFG()).deleteBBList();
		(currRoutine->getLoopList()).deleteListOfLoops();
	}
	delete routineIter;

	//delete curProgram;
}

extern Vector<MyString> globalProcsFileNamesList;
extern Vector<MyString> globalDefsFileNamesList;

extern void global_memory_alloc(void);

/*
#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

  __declspec( dllexport ) int pcProGUI(int,char**);
  
	__declspec( dllexport ) unsigned int __stdcall pcProGUIMain(void* _pp)
	{
	//NOS: DO NOT REMOVE
	#ifdef WIN32
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
	#endif
	//NOS: DO NOT REMOVE
	
	  ArgcArgv=(ProgramParams*)_pp;
	  cout=ArgcArgv->cout;
	  cerr=ArgcArgv->cerr;
	  cerr << endl << "-- EXPRESS: compiling " << ArgcArgv->filename << " ---------------------------------------------------" << endl << endl ;
	  //fake it for now!
	  int t=pcProGUI(2,NULL);
	  cerr << endl << "-- EXPRESS: finished " << ArgcArgv->filename << " ----------------------------------------------------" << endl;
	  //delete the params passed in
	  //delete ArgcArgv;
	  return t;
	  }
	  #else  //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
*/

#ifdef WIN32_GUI
//__declspec( dllexport ) int pcProConsole(int,char**);
__declspec( dllexport ) int pcPro(int,char**);

//__declspec( dllexport ) unsigned int __stdcall pcProConsoleMain(void* _cp)
__declspec( dllexport ) unsigned int __stdcall pcProMain(void* _cp)
{
	//NOS: DO NOT REMOVE
#ifdef WIN32
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	//NOS: DO NOT REMOVE
	
	ArgcArgv=(ProgramParams*)_cp;
	if (ArgcArgv->cout!=NULL) 
	{	
		cout=ArgcArgv->cout;
	}
	if (ArgcArgv->cerr!=NULL) 
	{
		cerr=ArgcArgv->cerr;
	}

	simDoc=(SIMDoc*)(ArgcArgv->simDoc);
	cout << endl << "-- EXPRESS: Started" << " ---------------------------------------------------" << endl << endl ;
	//fake it for now!
	//int t=pcProConsole(ArgcArgv->argc,ArgcArgv->argv);
	int t=pcPro(ArgcArgv->argc,ArgcArgv->argv);
	cout << endl << "-- EXPRESS: Finished " << " ----------------------------------------------------" << endl;
	//delete the params passed in
	//delete ArgcArgv;
	//exit(0);
	return t;
}

#endif // WIN32_GUI
//#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

#ifdef WIN32
//#ifdef WIN32_GUI
//__declspec( dllexport ) int pcProGUI(int argc, char **argv)
//#else
//__declspec( dllexport ) int pcProConsole(int argc, char **argv)
//#endif //WIN32_GUI

__declspec( dllexport ) int pcPro(int argc, char **argv)

#else
int main(int argc, char **argv)
#endif
{
	//NOS: DO NOT REMOVE
#ifdef WIN32
	AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
	//NOS: DO NOT REMOVE
	
	unsigned long simCycles = 0;
	unsigned long noSSACycleCount = 0; //stores the number of cycles for the simulation before SSA
	unsigned long htgCycleCount = 0; //stores the number of cycles for the simulation before HTG
	unsigned long tbzCycleCount = 0; //stores the number of cycles for the simulation after Tbz
	unsigned long raCycleCount = 0; //stores the number of cycles for the simulation after RA
	
	//MyLinkedListIterator<Routine *> *routineIter;
	
	//#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
	//	if (argc < 2)
	//	{
	//		cout << "Usage is pcPro <filename>.s [options]" << endl;
	//		exit(1);
	//	}
	//#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

	//perform this before initializing the memory subsystem
	interpretCmdLineNoGUI(argc,argv);
	
#ifdef WIN32
	if (dllInitialized==false)
	{
		dllInitialized=true;
		myInit1();
		buildArchModel();
		buildSystem();
		myInit2();
	}
#else
	myInit1();
	buildArchModel();
	buildSystem();	
	myInit2();
#endif
	
#ifdef WIN32_GUI
	//hook up the simulator to the GUI
	if (simDoc!=NULL)
	{
	}
#endif
	
	
	//#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
	//	myBuild(ArgcArgv->filename);
	//	interpretCmdLineGUI(argc,argv);
	//#else

	//interpretCmdLineNoGUI(argc,argv);

	if(pbp){
		resetCompiler();

		//procedure by procedure
		Vector<MyString>::iterator sIter;

		//read in the defs file (if set in cmd_line)
		read_defs_file(globalDefsFileNamesList);
		
		// SAH : 07/21/2000 : Moved this from inside read_defs_file routine.
		//
		// Perform global memory allocation.
		//
		// Call this pass only if a simulation run has been specified
		//
		// Note: currently, we only check to see if the structural simulator needs to be invoked.
		// We may also need to see if the functional simulator is to be invoked.
		//
		// We need memory allocation only if we are going to simulate the program.
		// However, assembly code generation pass needs the labels in symbolic form.
		//
		// So, currently, simulation and assembly code generation are mutually exclusive.
		//
		if (printASM)
		{
			append_defs_file_names();
			simNoSSA = simSSA = simHTG = simISel = simTbz = simRA = printC = 0;
		}
		else
		{
			global_memory_alloc();
		}

		
		// we are going to print out the IR as C (using the Program::irToC method)
		// So, open a file pointer to the required output file.
		//
		FilePointer irtocPtr;
		MyString fN;
		
		if (globalProgramName.isEmpty())
		{
			// give a generic name
			//
			fN = "irToC";
		}
		else
			fN = globalProgramName;
		
		fN.concat(".cc");
		
		try 
		{
			irtocPtr.open(fN, "w");
		}
		catch (CouldNotOpenFileException e)
		{
			cout << e;
			assert(0);
		}
		
#ifdef __EXPRESSION__
		if(doTbz && linkEXPRESSION){
			//call the expression compiler
			//if this ASSERT fails, you forgot to include the -ENAME file.exp in the comand line
			_ASSERT(EXPR_NAME);
			if(dbEnabled==1){
				// char *buf[8]={"expression",EXPR_NAME,"-RT","-DB","-FMFILENAME","c62_mem.fmd","-IFFILENAME","c62_mem.ifd"};
				// char *buf[8]={"expression",EXPR_NAME,"-RT","-DB","-FMFILENAME","elf.fmd","-IFFILENAME","elf.ifd"};
				char *buf[8]={"expression",EXPR_NAME,"-RT","-DB","-FMFILENAME","acesMIPS.fmd","-IFFILENAME","acesMIPS.ifd"};
				//first set the thread info, to be able to count the time inside EXPRESSION
				//setTreadInfo(&threadInfo);
				expression_main(8,(char**)buf);
			}
			else if(dbEnabled==2){
				// char *buf[9]={"expression",EXPR_NAME,"-RT","-DB","-REM","-FMFILENAME","c62_mem.fmd","-IFFILENAME","c62_mem.ifd"};
				// char *buf[9]={"expression",EXPR_NAME,"-RT","-DB","-REM","-FMFILENAME","elf.fmd","-IFFILENAME","elf.ifd"};
				char *buf[9]={"expression",EXPR_NAME,"-RT","-DB","-REM","-FMFILENAME","acesMIPS.fmd","-IFFILENAME","acesMIPS.ifd"};
				//first set the thread info, to be able to count the time inside EXPRESSION
				//setTreadInfo(&threadInfo);
				expression_main(9,(char**)buf);
			}
			else{
				// char *buf[7]={"expression",EXPR_NAME,"-RT","-FMFILENAME","c62_mem.fmd","-IFFILENAME","c62_mem.ifd"};
				// char *buf[7]={"expression",EXPR_NAME,"-RT","-FMFILENAME","elf.fmd","-IFFILENAME","elf.ifd"};
				char *buf[7]={"expression",EXPR_NAME,"-RT","-FMFILENAME","acesMIPS.fmd","-IFFILENAME","acesMIPS.ifd"};
				//first set the thread info, to be able to count the time inside EXPRESSION
				//setTreadInfo(&threadInfo);
				expression_main(7,(char**)buf);
			}
		}
#endif // #ifdef __EXPRESSSION__

		//for all the input .procs files in the command line
		for (sIter = globalProcsFileNamesList.begin(); sIter != globalProcsFileNamesList.end(); sIter++)
		{

			//read in the file
			cout << "-- EXPRESS: Reading ";
			sIter->print(cout);
			cout << endl;
			myBuild((*sIter).getStr());
			
			//build the instruction list for all the routines in the file 
			buildIListCFGLoops();

			if (printASM)
			{
				append_defs_file_names();
			}

#ifndef _NO_SYSTEM_CALLS_
			//add call name property for all the routines in the file 
			addCallNameProperty(curProgram);
#endif

			firstPassCallPatch(curProgram);

			//perform SSA for the file
			doneSSA = 0;
			doneISel = 0;
			// build_SSA();

			allocate_dummy_space();

			buildSSA(curProgram);
			doneSSA = 1;

			// Note: May need to check if this is really necessary.
			// For example, MIPS architectures need this pass.
			//
			patchForDoubles(curProgram);

			secondPassCallPatch(curProgram);

			//UD, DU, live analysis for the file
			usedef_live();
			
			//build the HTG for the file
			build_HTG();
			
			//output C for the file
			outputC("AFTER_HTG_BEFORE_PRESCH", irtocPtr);
			
			// NOSTEMP
			//
			if (doPreSchTrans)
				 prescheduling_transformations();
		
			outputC("AFTER_PRESCH_BEFORE_ISEL", "PreSch_NoISel");
		
			if(debugProperties) print_properties();
		
			// loop transformations.
			//
			loop_transformations();

			// Purify fix : remove UMR
			resetInstrIDs();

			//perform mutations and instruction selection
			mutation_instrsel();
			
			//output C after isel, mut
			outputC("AFTER_ISEL_BEFORE_TBZ", irtocPtr);
			
			
			if(doIfC)
			{
				ifConverting();
			}
			
			//	rebuild_cfg_htg();
			
			if(doPerc)
			{
				percolating();
			}
			
			//do trailblazing for the whole program
			if(doTbz)
			{
				redo_usedef_live();
				trailblazing();
			}
			
			
			outputC("AFTER_TBZ", irtocPtr);
			
			if (doRA) {
				//perform RA for the file
				buildProgramMappings();
				doneRA = 0;
				regAlloc();
				doneRA = 1;
			}
			//now deallocate all the datastructures for all the routines, files, 
			//don't deallocate the memory allcoation from the defs files

			delete_routines();
			resetCompiler();
		}                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                             
	}
	else{
		//not procedure by procedure (all at once)

		resetCompiler();
		
		Vector<MyString>::iterator sIter;
		for (sIter = globalProcsFileNamesList.begin(); sIter != globalProcsFileNamesList.end(); sIter++)
		{
			cout << "--------------------------------------------------------" << endl;
			cout << endl << "-- EXPRESS: Reading ";
			sIter->print(cout);
			cout << " ------------" << endl << endl;
			cout << "--------------------------------------------------------" << endl;
			myBuild((*sIter).getStr());
		}
		
		initCompilerDS();

		// Now, build the IList, CFG and Loop structures
		//
		buildIListCFGLoops();
		
		cout << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << " Code Size Before All Transformations: " << endl;
		cout << "\t Num. Instructions = " << curProgram->countInstructions() << endl;
		cout << "\t Num. Operations = " << curProgram->countOperations() << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << endl;
		
		// add callname property to all label args that refer to routine calls.
		//
#ifndef _NO_SYSTEM_CALLS_
		addCallNameProperty(curProgram);
#endif
		
		firstPassCallPatch(curProgram);

		//#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
		
		
		//read in the defs file (if set in cmd_line)
		read_defs_file(globalDefsFileNamesList);
		
		// SAH : 07/21/2000 : Moved this from inside read_defs_file routine.
		//
		// Perform global memory allocation.
		//
		// Call this pass only if a simulation run has been specified
		//
		// Note: currently, we only check to see if the structural simulator needs to be invoked.
		// We may also need to see if the functional simulator is to be invoked.
		//
		// We need memory allocation only if we are going to simulate the program.
		// However, assembly code generation pass needs the labels in symbolic form.
		//
		// So, currently, simulation and assembly code generation are mutually exclusive.
		//
		if (printASM)
		{
			append_defs_file_names();
			simNoSSA = simSSA = simHTG = simISel = simTbz = simRA = printC = 0;
		}
		else
		{
			global_memory_alloc();
		}

		if(debugProperties) print_properties();
		
		doneSSA = 0;
		
	printProg(_IList_, "BEFORE_SSA");
	printProg(_CFG_, "BEFORE_SSA");
	//	outputC("BEFORE_SSA", "NoSSA");
	//	printProg(_DUMP_, "BEFORE_SSA");  // dump ir to file
		
		start_time();
		
		if (simNoSSA)
		{
			simCycles = simulateProgram(curProgram, "_DUMP_IR_BEFORE_SSA.txt");
			printSimResult(simCycles, "Before SSA");
			printSimResult(numLockStepCycles(), "Before SSA, LockStep");
			printSimResultToFile(simCycles, "Before SSA");
			noSSACycleCount=simCycles;
		}
		pause_time();
		print_time();
		
		if (funcSimNoSSA)
		{
			simCycles = funcSimulateProgram(curProgram, "Before SSA");
			printSimResult(simCycles, "Before SSA");
			printSimResult(numLockStepCycles(), "Before SSA, LockStep");
		}
		
		/*
		WalkInstr wIObj(WALK_BACKWARDS, WALK_CF, NORMAL_MODE);
		OpToInstrMap oIObj(DC_OP, NORMAL_MODE);
		
		  MyLinkedListIterator<Routine *> *r1Iter = curProgram->routineListIteratorForw();
		  while (r1Iter->hasMoreElements())
		  {
		  wIObj.initWalk(r1Iter->currentElement());
		  wIObj.print(cout);
		  
			oIObj.initOpToInstrMap(r1Iter->currentElement());
			oIObj.print(cout);
			
			  r1Iter->nextElement();
			  }
			  delete r1Iter;
		*/

		// Note: May need to check if this is really necessary.
		// For example, MIPS architectures need this pass.
		//
		patchForDoubles(curProgram);

		// At the begining of each routine, add a NOP with the sp, and the arguments of the 
		// routine as source of operands
		//
		//addCorrectionOps(curProgram);

		//build the SSA for the program
		//build_SSA();

		allocate_dummy_space();

		buildSSA(curProgram);
		
		build_loops();

		doneSSA = 1;
		
		cout << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << " Code Size After SSA Before All Other Transformations: " << endl;
		cout << "\t Num. Instructions = " << curProgram->countInstructions() << endl;
		cout << "\t Num. Operations = " << curProgram->countOperations() << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << endl;


		secondPassCallPatch(curProgram);

	//	printProg(_IList_, "AFTER_SSA_BEFORE_UDDU");
	//	printProg(_CFG_, "AFTER_SSA_BEFORE_UDDU");
	//	printProg(_DUMP_, "AFTER_SSA_BEFORE_UDDU");  // dump ir to file
		
		//globalSymbolTable->print(cout);
		if (simSSA)
		{
			simCycles = simulateProgram(curProgram, "_DUMP_IR_AFTER_SSA_BEFORE_UDDU.txt");
			printSimResult(simCycles, "After SSA, Before HTG");
			printSimResult(numLockStepCycles(), "After SSA, Before HTG, LockStep");
		}
		
		if (funcSimSSA)
		{
			simCycles = funcSimulateProgram(curProgram, "After SSA, Before HTG");
			printSimResult(simCycles, "After SSA, Before HTG");
			printSimResult(numLockStepCycles(), "After SSA, Before HTG, LockStep");
		}
				
		//print_properties();
		
		//if(debugRAProperties) print_RAproperties(_INSTR_TYPE_);
		
		//if(debugBBProperties) print_BBproperties(_LIVEDEAD_);
		
		//build the HTG for the program
		build_HTG();
		
		//Analysis
		usedef_live();

	//	printProg(_IList_, "AFTER_HTG_BEFORE_PRESCH");
	//	printProg(_CFG_, "AFTER_HTG_BEFORE_PRESCH");
	//	printProg(_HTG_, "AFTER_HTG_BEFORE_PRESCH");
	//	printProg(_C_, "AFTER_HTG_BEFORE_PRESCH");
	//	printProg(_DUMP_, "AFTER_HTG_BEFORE_PRESCH");  // dump ir to file
		
		outputC("AFTER_HTG_BEFORE_PRESCH", "HTG_NoPreSch");
		
		if (simHTG)
		{
			simCycles = simulateProgram(curProgram, "_DUMP_IR_AFTER_HTG_BEFORE_PRESCH.txt");
			printSimResult(simCycles, "After HTG, Before InstrSel");
			printSimResult(numLockStepCycles(), "After HTG, Before InstrSel, LockStep");
			printSimResultToFile(simCycles, "After HTG");
			htgCycleCount=numLockStepCycles();
		}
		
		if (funcSimHTG)
		{
			simCycles = funcSimulateProgram(curProgram, "After HTG, Before InstrSel");
			printSimResult(simCycles, "After HTG, Before InstrSel");
			printSimResult(numLockStepCycles(), "After HTG, Before InstrSel, LockStep");
		}
		
		// NOSTEMP
		//
		if (doPreSchTrans)
			 prescheduling_transformations();

	//	printProg(_IList_, "AFTER_PRESCH_BEFORE_ISEL");
	//	printProg(_CFG_, "AFTER_PRESCH_BEFORE_ISEL");
	//	printProg(_HTG_, "AFTER_PRESCH_BEFORE_ISEL");
	//	printProg(_C_, "AFTER_PRESCH_BEFORE_ISEL");
	//	printProg(_DUMP_, "AFTER_PRESCH_BEFORE_ISEL");  // dump ir to file
		
		outputC("AFTER_PRESCH_BEFORE_ISEL", "PreSch_NoISel");
		
		if(debugProperties) print_properties();
		
		// loop transformations.
		//
		loop_transformations();		
		
		// Temporary addition by SAH on 21st Feb, 2001
		//
		redo_usedef_live();

		if(debugProperties) print_properties();
		/*
		rIter = curProgram->routineListIteratorForw();
		while (rIter->hasMoreElements())
		{
		(rIter->currentElement())->printProperties(cout, pL);
		rIter->nextElement();
		}
		delete rIter;
		*/


		// for debugging
	printProg(_IList_, "AFTER_PRESCH_BEFORE_ISEL");
	//	printProg(_CFG_, "AFTER_PRESCH_BEFORE_ISEL");
	printProg(_HTG_, "AFTER_PRESCH_BEFORE_ISEL");
	//	printProg(_C_, "AFTER_PRESCH_BEFORE_ISEL");
	//	printProg(_DUMP_, "AFTER_PRESCH_BEFORE_ISEL");  // dump ir to file
		//perform mutations and instruction selection
		resetInstrIDs();
		
		mutation_instrsel();

	printProg(_IList_, "AFTER_ISEL_BEFORE_TBZ");
	//	printProg(_CFG_, "AFTER_ISEL_BEFORE_TBZ");
	printProg(_HTG_, "AFTER_ISEL_BEFORE_TBZ");
	//	printProg(_C_, "AFTER_ISEL_BEFORE_TBZ");
	//	printProg(_DUMP_, "AFTER_ISEL_BEFORE_TBZ");  // dump ir to file
		
		outputC("AFTER_ISEL_BEFORE_TBZ", "ISel_NoTbz");
		
		if (simISel)
		{
			simCycles = simulateProgram(curProgram, "_DUMP_IR_AFTER_ISEL_BEFORE_TBZ.txt");
			printSimResult(simCycles, "After InstrSel, Before Tbz");
			printSimResult(numLockStepCycles(), "After InstrSel, Before Tbz, LockStep");
			printSimResultToFile(simCycles, "After InstrSel, Before Tbz");
		}
		
		if (funcSimISel)
		{
			simCycles = funcSimulateProgram(curProgram, "After InstrSel, Before Tbz");
			printSimResult(simCycles, "After InstrSel, Before Tbz");
			printSimResult(numLockStepCycles(), "After InstrSel, Before Tbz, LockStep");
		}
		
		if (doStorageAlloc)
		{
			storage_alloc();
		}
		
#ifdef __EXPRESSION__
		if(doTbz && linkEXPRESSION){
			//call the expression compiler
			//if this ASSERT fails, you forgot to include the -ENAME file.exp in the comand line
			_ASSERT(EXPR_NAME);
			if(dbEnabled==1){
				// char *buf[8]={"expression",EXPR_NAME,"-RT","-DB","-FMFILENAME","c62_mem.fmd","-IFFILENAME","c62_mem.ifd"};
				// char *buf[8]={"expression",EXPR_NAME,"-RT","-DB","-FMFILENAME","elf.fmd","-IFFILENAME","elf.ifd"};
				char *buf[8]={"expression",EXPR_NAME,"-RT","-DB","-FMFILENAME","acesMIPS.fmd","-IFFILENAME","acesMIPS.ifd"};
				//first set the thread info, to be able to count the time inside EXPRESSION
				//setTreadInfo(&threadInfo);
				expression_main(8,(char**)buf);
			}
			else if(dbEnabled==2){
				// char *buf[9]={"expression",EXPR_NAME,"-RT","-DB","-REM","-FMFILENAME","c62_mem.fmd","-IFFILENAME","c62_mem.ifd"};
				// char *buf[9]={"expression",EXPR_NAME,"-RT","-DB","-REM","-FMFILENAME","elf.fmd","-IFFILENAME","elf.ifd"};
				char *buf[9]={"expression",EXPR_NAME,"-RT","-DB","-REM","-FMFILENAME","acesMIPS.fmd","-IFFILENAME","acesMIPS.ifd"};
				//first set the thread info, to be able to count the time inside EXPRESSION
				//setTreadInfo(&threadInfo);
				expression_main(9,(char**)buf);
			}
			else{
				// char *buf[7]={"expression",EXPR_NAME,"-RT","-FMFILENAME","c62_mem.fmd","-IFFILENAME","c62_mem.ifd"};
				// char *buf[7]={"expression",EXPR_NAME,"-RT","-FMFILENAME","elf.fmd","-IFFILENAME","elf.ifd"};
				char *buf[7]={"expression",EXPR_NAME,"-RT","-FMFILENAME","acesMIPS.fmd","-IFFILENAME","acesMIPS.ifd"};
				//first set the thread info, to be able to count the time inside EXPRESSION
				//setTreadInfo(&threadInfo);
				expression_main(7,(char**)buf);
			}
		}
#endif // #ifdef __EXPRESSSION__
		
		//if(debugRAProperties) print_RAproperties(_DUCHAIN_);
		if(debugBBProperties) print_BBproperties(_LIVEDEAD_);
		
		// Temporary addition by SAH on 21st Feb, 2001
		//
		redo_usedef_live();

		if(doPerc)
		{
			percolating();
		}
		
		if(doIfC)
		{
			ifConverting();

			rebuild_cfg_htg();
			redo_usedef_live();
		}

		// NOSTEMP
		//
		// if (doPreSchTrans)
		//	prescheduling_transformations();

	//	printProg(_IList_, "AFTER_IFCONVERT");
	//	printProg(_CFG_, "AFTER_IFCONVERT");
	//	printProg(_HTG_, "AFTER_IFCONVERT");
	//	printProg(_C_, "AFTER_IFCONVERT");
	//	printProg(_DUMP_, "AFTER_IFCONVERT");  // dump ir to file
		
		outputC("AFTER_IFCONVERT", "Tbz");

		//do trailblazing for the whole program
		if(doTbz)
		{
			setScheduleProperties();
			trailblazing();
		}
		
		if (doThumb)
		{
			ThumbPass();
		}

		if(debugProperties) print_properties();
		
	//	printProg(_IList_, "AFTER_TBZ");
	//	printProg(_CFG_, "AFTER_TBZ");
	//	printProg(_HTG_, "AFTER_TBZ");
	//	printProg(_C_, "AFTER_TBZ");
	//	printProg(_DUMP_, "AFTER_TBZ");  // dump ir to file
		
		outputC("AFTER_TBZ", "Tbz");
		
		start_time();
		if (simTbz)
		{
			simCycles = simulateProgram(curProgram, "_DUMP_IR_AFTER_TBZ.txt");
			printSimResult(simCycles, "After Tbz");
			printSimResult(simCycles+global_memory_stalls, "After Tbz (including memory stalls)");
			printSimResult(numLockStepCycles(), "Tbz, LockStep");
			printSimResultToFile(simCycles, "After Tbz");
			tbzCycleCount=simCycles;
		}
		pause_time();
		print_time();
		
		if(simTbz && simHTG){
			printSimSpeedup(htgCycleCount,tbzCycleCount);
		}
		
		if(simNoSSA && simTbz){
			printSimSpeedup(noSSACycleCount,tbzCycleCount);
		}
		
		if(printMC)
			PrintMutationCounts(); //prints out statistic information from mutations
		
		if (funcSimTbz)
		{
			simCycles = funcSimulateProgram(curProgram, "After Tbz");
			printSimResult(simCycles, "After Tbz");
			printSimResult(numLockStepCycles(), "Tbz, LockStep");
		}
		
		if (doRA) {
			
			// delete routineIter;
			
			buildProgramMappings();
			
			doneRA = 0;
			
			regAlloc();
						
			doneRA = 1;
		}


		// If RA inserts new instrs, then we call this to ensure that our prints look nice.
		//
		// resetInstrIDs();

		//Correction ops were added before SSA. Now, we can delete them.
		//
//		remCorrectionOps(curProgram);

		if (doPostSchTrans)
			postscheduling_transformations();
		
		//curProgram->printHTG();

		//delete [10000000] dummy_space;
		//free(dummy_space);
		
	/*	{
			int i;
			char *s;
			for(i=0;i<1000000;i++){
				//cout << i << endl;
				s=new char[100];
			}
		} */

//		curProgram->printHTG();

	printProg(_IList_, "AFTER_REGALLOC");
	printProg(_CFG_, "AFTER_REGALLOC");
	printProg(_HTG_, "AFTER_REGALLOC");
	//	printProg(_C_, "AFTER_REGALLOC");
	printProg(_ASM_, "AFTER_REGALLOC");  // Generate Assembly
	printProg(_DUMP_, "AFTER_REGALLOC");  // dump ir to file
	//	outputC("AFTER_REGALLOC", "RegAlloc");
		
		
		if (simRA)
		{
			simCycles = simulateProgram(curProgram, "_DUMP_IR_AFTER_REGALLOC.txt");
			printSimResult(simCycles, "After RA");
			printSimResult(numLockStepCycles(), "After RA, LockStep");
			printSimResultToFile(simCycles, "After RA");
			raCycleCount=simCycles;
		}
		
		if (funcSimRA)
		{
			simCycles = funcSimulateProgram(curProgram, "_DUMP_IR_AFTER_REGALLOC.txt");
			printSimResult(simCycles, "After RA");
			printSimResult(numLockStepCycles(), "After RA, LockStep");
		}

		if(simNoSSA && simRA){
			printSimSpeedup(noSSACycleCount,raCycleCount);
		}
		
		cout << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << " Code Size After All Transformations: " << endl;
		cout << "\t Num. Instructions = " << curProgram->countInstructions() << endl;
		cout << "\t Num. Operations = " << curProgram->countOperations() << endl;
		cout << "--------------------------------------------------------" << endl;
		cout << endl;
	}
	
/*#ifdef WIN32
	VERIFY(GetThreadTimes(threadInfo.handle,
		&threadInfo.creationTime,
		&threadInfo.exitTime,
		&threadInfo.kernelTime,
		&threadInfo.userTime)); 
	
	double userTime;
	double kernelTime;
	
#if 0
	//if compile time is > 429.4967296 seconds must also use the threadInfo.*.dwHighDateTime
	if ((startTime.userTime.dwHighDateTime!=stopTime.userTime.dwHighDateTime) ||
		(startTime.kernelTime.dwHighDateTime!=stopTime.kernelTime.dwHighDateTime))
	{
		cout << endl << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl ;
		cout << "WARNING: Compile time reported below might not be reliable." << endl ;
		cout << "         See the file containing this comment for details." << endl ;
		cout << "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl ;
	}
	
	userTime=(double)threadInfo.userTime.dwLowDateTime/10000000.0;
	kernelTime=(double)threadInfo.kernelTime.dwLowDateTime/10000000.0;
	
#else
	
#define MAKE64BIT(val64,val32hi,val32lo) \
	{ \
	unsigned __int64 temp=val32hi; \
	temp=Int64ShllMod32(temp,31); temp=Int64ShllMod32(temp,1); \
	val64=temp; \
	temp=val32lo; \
	val64+=temp; \
	}
	
	unsigned __int64 userTime64;
	MAKE64BIT(userTime64,threadInfo.userTime.dwHighDateTime,threadInfo.userTime.dwLowDateTime);
	
	unsigned __int64 kernelTime64;
	MAKE64BIT(kernelTime64,threadInfo.kernelTime.dwHighDateTime,threadInfo.kernelTime.dwLowDateTime);
	
	__int64 userTime64s=(__int64)userTime64;
	__int64 kernelTime64s=(__int64)kernelTime64;
	
	userTime=userTime64s;
	userTime=userTime/10000000.0;
	kernelTime=kernelTime64s;
	kernelTime=kernelTime64s/10000000.0;
	
#endif
	
	cout << endl << "==============================================================================" << endl ;
	cerr << "COMPILE TIME: " << userTime + kernelTime << " secs (user: " << userTime << " secs) (kernel: " << kernelTime << " secs)" << endl ;
	cout << "==============================================================================" << endl ;
	
	
	//make this thread wait until we say so
	//while (lockPCPRO);
	
	//	getch();
	
	return 0;
#endif

*/
	
	//CloseFiles();
}

//1 if the init_compiler has been run, 0 otherwise
int global_init_compiler=0;

__declspec( dllexport ) void init_compiler(void* _cp)
{
	if(global_init_compiler==0)
	{
		global_init_compiler=1;
		
		//NOS: DO NOT REMOVE
#ifdef WIN32
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
		//NOS: DO NOT REMOVE
		
		ArgcArgv=(ProgramParams*)_cp;
		
#ifdef WIN32_GUI
		cout=ArgcArgv->cout;
		cerr=ArgcArgv->cerr;
		simDoc=(SIMDoc*)(ArgcArgv->simDoc);
#endif
		cout << endl << "-- EXPRESS: compiling " << ArgcArgv->argv[1] << " ---------------------------------------------------" << endl << endl ;
		
		//perform this before the architecture has been initialized
		interpretCmdLineNoGUI(ArgcArgv->argc,ArgcArgv->argv);

		if (dllInitialized==false)
		{
			dllInitialized=true;
			myInit1();
			buildSystem();
			buildArchModel();
			myInit2();
		}
		
		resetCompiler();
		
		//interpretCmdLineNoGUI(ArgcArgv->argc,ArgcArgv->argv);
		myBuild(ArgcArgv->argv[1]);

 	buildIListCFGLoops();

	//ATTENTION! This needs to be uncommented when running with system calls
#ifndef _NO_SYSTEM_CALLS_
		addCallNameProperty(curProgram);
#endif

  read_defs_file(globalDefsFileNamesList);

 	//build the SSA for the program
 	doneSSA = 0;
 	build_SSA();
		doneSSA = 1;

  //Analysis
	 usedef_live();
	
 	build_HTG();

 	prescheduling_transformations();

 	mutation_instrsel();
	}
}

__declspec( dllexport ) BaseStat* getStatsIter(BaseSimObject* obj, bool bInit = false)
{
	static StatsRow::iterator i;
	static StatsRow* statMap=NULL;
	
	if (bInit) //init stage
	{
		statMap=&(globalArchStatsDB.getStatList(obj));
		i = statMap->begin();
	}
	
	//iter stage
	if (i != statMap->end())
	{
		return (*i++).second;
	}
	else
	{
		return NULL;
	}
}


__declspec( dllexport ) BaseStat* getTrackStatsIter(BaseSimObject* obj, bool bInit = false)
{
	static StatsRow::iterator i;
	static StatsRow* statMap=NULL;
	
	if (bInit) //init stage
	{
		statMap=&(globalArchStatsDB.getTrackStatList(obj));
		i = statMap->begin();
	}
	
	//iter stage
	if (i != statMap->end())
	{
		return (*i++).second;
	}
	else
	{
		return NULL;
	}
}

//__declspec (dllexport) Routine* volatile currRoutine;
__declspec( dllexport ) void setCurrRoutine(Routine* routine)
{
	currRoutine=routine;
}


//#endif // MOTIF_GUI
