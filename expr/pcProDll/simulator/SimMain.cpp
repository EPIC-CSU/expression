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
#include "stdafx.h"

#include "SpecialInclude.h"

#include "ArchClassIncludes.h"
#include "Unit.h"
#include "Latch.h"
#include "Connection.h"

#include "Routine.h"
#include "MyLinkedList.h"

#include "simulator.h"
#include "FlowMechanism.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

FlowMechanism globalFlowMechanism;

Routine *simCurrRoutine;
extern long clock_count;
int pipePhase;
int notPredicted;

extern BaseInstruction *curPC, *curFetch;
extern ArchUnit *vliwProcessor1;
int isBreakPoint;
int endOfSim;

void finishSim();

extern void buildSystem();

void startSim(Routine *rPtr)
{
   endOfSim = 0;

   simCurrRoutine = rPtr;
   
   if (vliwProcessor1==NULL)
      buildSystem();

   vliwProcessor1->initPC((rPtr->getInstrList()).getHead());

   clock_count = 0;
   pipePhase = CYCLES_0;
}

void singleStep()
{
   notPredicted = 0;
   switch (pipePhase)
      {
       case CYCLES_0:
	 vliwProcessor1->step0();
         globalFlowMechanism.resetFlow();
	 if (vliwProcessor1->endOfFetch())
	   pipePhase = CYCLES_N_3;
 	 else
	   pipePhase = CYCLES_1;
	 break;
       case CYCLES_1:
	 vliwProcessor1->step1();
	 globalFlowMechanism.addFlow(curFetch,curPC);
	 if (vliwProcessor1->endOfFetch())
           pipePhase = CYCLES_N_2;
         else
           pipePhase = CYCLES_2;
	 break;
       case CYCLES_2:
	 vliwProcessor1->step2();
	 globalFlowMechanism.addFlow(curFetch,curPC);
         if (vliwProcessor1->endOfFetch())
           pipePhase = CYCLES_N_1;
         else
           pipePhase = CYCLES_3;
	 break;
       case CYCLES_3:
	 vliwProcessor1->step3();
	 globalFlowMechanism.addFlow(curFetch,curPC);
         if (vliwProcessor1->endOfFetch())
           pipePhase = CYCLES_N_0;
         else
	   pipePhase = PIPE_FULL;
	 break;
       case PIPE_FULL:
	 vliwProcessor1->stepPF();
         if (notPredicted)
           pipePhase = CYCLES_0;
	 else {
	   globalFlowMechanism.addFlow(curFetch,curPC);
           if (vliwProcessor1->endOfFetch())
	     pipePhase = CYCLES_N_3;
  	 }
	 break;
       case CYCLES_N_3:
	 vliwProcessor1->stepN3();
	 if (notPredicted)
           pipePhase = CYCLES_0;
         else 
 	   pipePhase = CYCLES_N_2;
	 break;
       case CYCLES_N_2:
	 vliwProcessor1->stepN2();
         if (notPredicted)
           pipePhase = CYCLES_0;
         else 
           pipePhase = CYCLES_N_1;
	 break;
       case CYCLES_N_1:
	 vliwProcessor1->stepN1();
         if (notPredicted)
           pipePhase = CYCLES_0;
         else 
           pipePhase = CYCLES_N_0;
	 break;
       case CYCLES_N_0:
	 vliwProcessor1->stepN0();
  	 endOfSim =  1;
	 break;
       default:
	 /* do nothing : possibly print error message */
	 break;
      }
    if (clock_count%100000==0)
      fprintf(stderr,"cycles=%i\n",clock_count);
}

void continueSim()
{

   while ((!endOfSim)&&(!isBreakPoint)){
	 singleStep();
   }

   if (isBreakPoint) 
      isBreakPoint = 0;
   else
     printf("The number of cycles is: %i  \n", clock_count);
}

long cycleCount()
{
   return clock_count;
}

void finishSim()
{
   simulator::current->setFinished();
}
