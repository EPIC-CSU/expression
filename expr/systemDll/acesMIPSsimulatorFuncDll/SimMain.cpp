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
#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifndef __UNIX__
#include "StdAfx.h"
#else
#ifndef ASSERT_TRACE
#define ASSERT_TRACE(c,e) if (!(c)) { cerr << endl << "WOA, NELLIE! " << __FILE__ << "(" << __LINE__ << "): " << e << endl; assert(0); }
#endif
#endif

#include <map>

#include "Routine.h"

#include "SpecialInclude.h"    
#include "ArchClassIncludes.h"
#include "../acesMIPSderivedClassDll/DerivedUnit.h"

#include "FlowMechanism.h"

#include "StatList.h"
#include "ArchStats.h"
#include "../acesMIPSderivedClassDll/DataHazardDetector.h"

extern bool volatile lockPCPRO;
extern bool volatile stepPCPRO;
extern bool volatile donePCPRO;

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


Routine *simCurrRoutine;

int notPredicted;
int endOfSim;

int isBreakPoint;

extern Unit *toyArch;
extern MemoryModule *mainMemory;
extern Connectivity *connect;
ArchUnit *vliwProcessor1;
extern ArchStatsDB globalArchStatsDB;
extern MyString globalProgramName;

FlowMechanism globalFlowMechanism;
DataHazardDetector globalDataHazardDetector;

void finishSim();

extern void buildSystem();

int _primaryCacheAccess_;
int _primaryCacheMiss_;
int _secondaryCacheAccess_;
int _secondaryCacheMiss_;


extern void initializeMemory(long, char *, char *);

void startSim(MyString pgmFileName)
{
   endOfSim = 0;
   
   _primaryCacheAccess_ = 0;
   _primaryCacheMiss_ = 0;
   _secondaryCacheAccess_ = 0;
   _secondaryCacheMiss_ = 0;

   
   vliwProcessor1 = (ArchUnit*)toyArch;
   
   if (vliwProcessor1==NULL)
      buildSystem();
   else
   {
	   vliwProcessor1->reset();
	   connect->initializeMemorySubsystem();
	   initializeComplexOpList("combined_ops.txt");
	   globalFlowMechanism.resetFlow();
	   globalDataHazardDetector.reset();
	   globalArchStatsDB.resetAllStats();
   }

   long pgmstrtaddr = 0;
   initializeMemory(pgmstrtaddr, "both", pgmFileName.getStr());
//   mainMemory->initmem(pgmstrtaddr,"both",pgmFileName.getStr());

   //vliwProcessor1->initPC((rPtr->getInstrList()).getHead());
   vliwProcessor1->initPC(pgmstrtaddr);
   clock_count = 0;

}

void singleStep()
{
	if (vliwProcessor1 != NULL)
		vliwProcessor1->singleStep();
	else
	{
		// Error condition. Startsim has to be called before calling singleStep.
		cout << "Panic: SingleStep cannot be called before the processor pointer has been set properly" << endl;
		assert(0);
	}
}

void continueSim()
{

   while ((!endOfSim)&&(!isBreakPoint))
   {
    while (lockPCPRO); //wait for user action
	   singleStep();
    if (stepPCPRO) lockPCPRO=true; //stop after this step
   }

   donePCPRO=true;

   if (isBreakPoint) 
      isBreakPoint = 0;
   else
     printf("The number of cycles is: %i  \n", clock_count);
}

long cycleCount()
{
   return clock_count;
}

extern void displayAtEndOfSimulation();
void print_mem_statistics(FILE *);

void finishSim(char* statFileName)
{
//	endOfSim = 1;
//	dumpRegFile();

 	displayAtEndOfSimulation();

	//print the memory hierarchy statistics
	char fileName[100];
	strcpy(fileName, statFileName);
	strcat(fileName, ".pwrStats");
	FILE* statPtr;
	statPtr = fopen(fileName, "w");
	fprintf(statPtr, "\nTotal Cycles: %d\n", clock_count);
	print_mem_statistics(statPtr);
	fclose(statPtr);

	//exit(0);
	// If any D.S. need to be destroyed, this is the place to do it.
}

