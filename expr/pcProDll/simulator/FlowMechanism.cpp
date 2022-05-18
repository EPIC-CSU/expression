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

SimFlowList::SimFlowList()
{
  _flowList = new PcType[numFlowOpsPerInstr];
  init();
}

SimFlowList::SimFlowList(SimFlowList& src)
{
  _flowList = new PcType[numFlowOpsPerInstr];
  for (int i=0; i<numFlowOpsPerInstr; i++)
    _flowList[i] = src._flowList[i];
  _predictPath = src._predictPath;
}

SimFlowList::~SimFlowList()
{
  delete []_flowList;
}

void SimFlowList::init()
{
  _predictPath = NULL;
  for (int i=0; i<numFlowOpsPerInstr; i++)
    _flowList[i] = NULL;
}

void SimFlowList::initSimFlowList(BaseInstruction *instr,PcType pc)
{
  _predictPath = pc;

  ArrayIterator<FlowOpSlot> *flowSlotIter;
  int i = 0;

  if (instr->isNormalInstr()) {
    BaseInstruction *tempTargetInstr;
    flowSlotIter = ((NormalInstr *)instr)->getFlowOpSlotIter();

    while (flowSlotIter->hasMoreElements()) {
      if ((flowSlotIter->currentElement()).hasOper()) {
                     // Get the target instruction pointed to by the
                     // flow operation in the current FlowOpSlot.
        tempTargetInstr = ((FlowOp *)(flowSlotIter->currentElement()).
		getPtrToOperation())->getTargetInstr();
        _flowList[i] = tempTargetInstr;
      }
      else 
        _flowList[i] = NULL;
     i++;
     flowSlotIter->nextElement();
    }
  }
  else {
    cout <<"Panic: Wrong instruction type in SimFlowList::initFlowList" << endl;
    assert(0);
  }
  delete flowSlotIter;
}

PcType  SimFlowList::getTarget(int pos)
{
  return _flowList[pos-1];
}

PcType SimFlowList::getPrediction()
{
  return _predictPath;
}

void SimFlowList::print(ostream& out)
{
  out<<"Predicted Path:"<<_predictPath;
  out << "Flow List: ";
  for (int i=0 ; i<numFlowOpsPerInstr; i++)
    out << "\t" << _flowList[i];
  out<<endl;
}

FlowMechanism::FlowMechanism() 
{
  _branchTargetList = new SimFlowList[NUM_PIPE_STAGES_FLOW_PATH];
}

FlowMechanism::~FlowMechanism()
{
  delete []_branchTargetList;
}

void FlowMechanism::resetFlow()
{
  _currPriority= 0;
  _nextTarget = NULL;
  _head = 0;
  _tail = 0;
  for (int i=0; i<NUM_PIPE_STAGES_FLOW_PATH; i++)
    _branchTargetList[i].init();
}

void FlowMechanism::addFlow(BaseInstruction *instr, PcType pc)
{
  _branchTargetList[_head].initSimFlowList(instr,pc);
  _head = (_head+1)%NUM_PIPE_STAGES_FLOW_PATH;
}

void FlowMechanism::removeFlow()
{
  _currPriority = 0;
  _tail = (_tail+1)%NUM_PIPE_STAGES_FLOW_PATH;
}

void FlowMechanism::setNextTarget(int priority ,int pos)
{
  if (priority > _currPriority) {
    _nextTarget = _branchTargetList[_tail].getTarget(pos);
    _currPriority = priority;
  }
}

int FlowMechanism::tookPathNotPredicted()
{
//if it is NULL, there is no predication, current path is sure to correct
  if (_nextTarget != _branchTargetList[_tail].getPrediction())
       return _YES_;
  return _NO_;
}

void FlowMechanism::print(ostream& out)
{
  out<<"nextTarget"<<_nextTarget<<endl;
  out<<"Flows:"<<endl;
  for (int i=0; i<NUM_PIPE_STAGES_FLOW_PATH; i++)
    _branchTargetList[i].print(out);
  out<<endl;
}

