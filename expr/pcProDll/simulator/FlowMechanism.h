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
 *file  : FlowMechanism.h                                    *
 *created : 11th nov,97.                                     *
 *authors : Wei Yu Tang,vgee                                 *
 *last modified :                                            *
 *                                                           *
 *contents:contains class definitions for Unit class & other *
 *         classes derived from it.                          *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _FLOWMECHANISM_H_
#define _FLOWMECHANISM_H_

#include "GlobalIncludes.h"
#include "ArchClassIncludes.h"
#include "NormalInstr.h"

const int NUM_PIPE_STAGES_FLOW_PATH=4;

class SimFlowList
{
 private:
   PcType  *_flowList;
   PcType _predictPath;
 public:
   SimFlowList();
   SimFlowList(SimFlowList&);
   ~SimFlowList();
   void init();
   void initSimFlowList(BaseInstruction *instr,PcType);
   PcType getTarget(int pos);
   PcType getPrediction();
   void print(ostream&);
};

class FlowMechanism
{
 private:
   int _currPriority;
   PcType _nextTarget;
   int _head, _tail;
   SimFlowList *_branchTargetList;
 public:
   FlowMechanism();
   ~FlowMechanism();
   void resetFlow();
   void addFlow(BaseInstruction *, BaseInstruction *);
   void removeFlow();
   void setNextTarget(int priority, int pos);
   int tookPathNotPredicted();
   void print(ostream&);

   PcType & getNextTarget()
      {
	 return _nextTarget;
      }
   
   void resetPriority()
      {
	 _currPriority = -1;
      }
};
#endif

