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

#include "../acesMIPSderivedClassDll/DerivedClassIncludes.h"
#include "../../pcProDll/include/NormalInstr.h"
#include "../../pcProDll/include/Array.h"
#include "../../pcProDll/include/ArrayIterator.h"
#include "../acesMIPSderivedClassDll/DerivedAddrData.h"
#include "../acesMIPSderivedClassDll/ExpressAddrData.h"

const int NUM_PIPE_STAGES_FLOW_PATH = 5;

typedef BaseInstruction* PcType;

class SimFlowList
{
 private:
   Array<PcType> _flowList;

 public:
   SimFlowList() : _flowList(numFlowOpsPerInstr)
      {
	 PcType pc = NULL;
	 
	 for (int i = 0; i < numFlowOpsPerInstr; i++)
	    {
	       _flowList.setElementAt(pc, i);
	    }
      }

   SimFlowList(const SimFlowList & a) : _flowList(a._flowList)
      {
      }
   
   ~SimFlowList()
      {
      }
   
   SimFlowList *copy()
      {
	 SimFlowList *retPtr;
	 retPtr = new SimFlowList(*this);
	 
	 return retPtr;
      }
   
   void reset()
      {
	 ArrayIterator<PcType> *listIter;
	 listIter = _flowList.elements();
	 
	 while (listIter->hasMoreElements())
	    {
	       listIter->currentElement() = NULL;
	       listIter->nextElement();
	    }
	 delete listIter;
      }
   
   void initSimFlowList(BaseInstruction *instr)
      {
	 if (!(instr->isNormalInstr()))
	    {
	       // error condition.
		  cout << "Panic: Wrong instruction type in SimFlowList::initFlowList" << endl;
	       assert(0);
	    }
	 
	 ArrayIterator<PcType> *listIter;
	 listIter = _flowList.elements();
	 
	 ArrayIterator<FlowOpSlot> *flowSlotIter;
	 flowSlotIter = ((NormalInstr *)instr)->getFlowOpSlotIter();

	 BaseInstruction *tempTargetInstr;
	 
	 while (flowSlotIter->hasMoreElements())
	    {
	       if ((flowSlotIter->currentElement()).hasOper())
		  {
		     // Get the target instruction pointed to by the
		     // flow operation in the current FlowOpSlot. 
		     // 
		     tempTargetInstr = ((FlowOp *)(flowSlotIter->currentElement()).getPtrToOperation())->getTargetInstr();
		     
		     listIter->currentElement() = tempTargetInstr;
		  }
	       listIter->nextElement();
	       flowSlotIter->nextElement();
	    }
	 delete listIter;
	 delete flowSlotIter;
      }

   ArrayIterator<PcType> *getSimFlowListIter()
      {
	 ArrayIterator<PcType> *retPtr;
	 retPtr = _flowList.elements();
	 
	 return retPtr;
      }

   PcType getTarget(int pos)
   {
    try
    {
     return _flowList.elementAt(pos);
    }
    catch (Exception *e)
    {
     cout << "Panic: In SimFlowList::getTarget(int pos)" <<
      endl;
     cout << *e;
     delete e;
    }

    ASSERT(0);
    return _flowList.elementAt(pos);
   }

   friend int operator == (const SimFlowList & a, const SimFlowList &
			   b)
      {
/*
         if (a._flowList == b._flowList)
	    {
	       return _YES_;
	    }
*/
	 return _NO_;
      }
   
   friend ostream & operator << (ostream & out, SimFlowList & s)
      {
	 out << "Flow List: " << endl;
	 out << "\t" << s._flowList;
		return out;
      }
};

class FlowMechanism
{
 private:
   
   Array<SimFlowList> _branchTargetList;
   Array<PcType> _predictedPath;

   PcType _predictionBuffer;
   //
   // This is needed because _predictedPath is of size decode - wb.
   // However, we start the prediction from the fetch stage onwards.
   // If we write our prediction directly into the Array entry and not
   // to the buffer, we have to be careful in the order in which we
   // call Fetch::doStep and WB::doStep.
   // By writing to the buffer and updating the entry only at the end
   // of the cycle, we save on having to keep another set of variables
   // for the head and the tail of the circular array.
   // See _head and _tail for further comments.
   
   int _head; // This is the postion where the entries for the
              // branchtargetlist and the predictedpath are added.

   int _tail; // This is the position where the entries for the
              // branchtargetlist and the predicted path corresponding
	      // to the instruction currently in the writeback stage
	      // should be read from.

   int _writeNumber;

   int _currPriority;

   int _callPriority;
   int _retPriority;

   PcType _nextTarget;
   IRPtr _callTarget;

   int _fetchCalled;
   int _decodeCalled;
   int _wbCalled;

   inline void addOne(int & a)
      {
	 if (a == NUM_PIPE_STAGES_FLOW_PATH - 1)
	    a = 0;
	 a++;
      }
   
 public:
   FlowMechanism() : _branchTargetList(NUM_PIPE_STAGES_FLOW_PATH),
   _predictedPath(NUM_PIPE_STAGES_FLOW_PATH), 
   _currPriority(-1), _callPriority(-1), _retPriority(-1), _head(0), _tail(0), _writeNumber(0), _fetchCalled(0), _decodeCalled(0), _wbCalled(0)
      {
	   // Note: SimFlowList constructor takes numFlowOpsPerInstr as a parameter.
	   // Therefore, this piece of code cannot be in the constructor for FlowMechanism.
	   //
	   // We call initFlow instead.

/*
	 SimFlowList a;
	 PcType b = NULL;

	 for (int i = 0; i < NUM_PIPE_STAGES_FLOW_PATH; i++)
	    {
	       _branchTargetList.setElementAt(a, i);
	       _predictedPath.setElementAt(b, i);
	    }
	 
//	 addOne(_tail);
*/
      }
   
   ~FlowMechanism()
      {
      }
   
   void initFlow()
   {
	 SimFlowList a;
	 PcType b = NULL;

	 for (int i = 0; i < NUM_PIPE_STAGES_FLOW_PATH; i++)
	    {
	       _branchTargetList.setElementAt(a, i);
	       _predictedPath.setElementAt(b, i);
	    }
	 
//	 addOne(_tail);
   }

   void resetFlow()
   {
	   _head = 0;
	   _tail = 0;
	   _writeNumber = 0;
	   _currPriority = -1;
	   _callPriority = _retPriority = -1;
	   _fetchCalled = _decodeCalled = _wbCalled = 0;

	   ArrayIterator<SimFlowList> *iter;
	   iter = _branchTargetList.elements();
	   while (iter->hasMoreElements())
	   {
		   (iter->currentElement()).reset();
		   iter->nextElement();
	   }
	   delete iter;

	   _nextTarget = NULL;

//	   addOne(_tail);
   }


   // This function is used by the decode stage to initialize all the
   // branch targets.
   //
   void setBranchTargets(BaseInstruction *instr)
   { 
	 	(_branchTargetList.elementAt(_head)).initSimFlowList(instr);
		_decodeCalled = 1;
//		cout << "Set Targets: " << (_branchTargetList.elementAt(_head)).getTarget(_LAST_POS_) << " Num: " << _head << " Instr: " << instr << endl;
   }
	    
   void setPredictedPath(PcType nextPc)
   {
	 _predictionBuffer = nextPc;
	 _fetchCalled = 1;

//	 cout << "Predicted path: " << nextPc << " Num: " << _writeNumber << endl;
   }

   // This function is used by the writeback sub units to set the next
   // target.
   //
   void setNextTarget(int priority, int pos)
   {
	 if (priority > _currPriority)
	 {
		 _nextTarget = (_branchTargetList.elementAt(_tail)).getTarget(pos);
		 _currPriority = priority;
	 }

//	 cout << " NextTarget: " << _nextTarget << " Num: " << _tail << " Pos: " << pos << endl;
   }
   
   // This function is used by the branch writeback sub units to set the call.
   //
   void setCall(int priority, int pos, IRPtr & ptr)
   {
	 if (priority > _callPriority)
	 {
		 _callTarget = ptr;
		 _callPriority = priority;
	 }

//	 cout << " NextTarget: " << _nextTarget << " Num: " << _tail << " Pos: " << pos << endl;
   }

   void setRet(int priority, int pos)
   {
	   if (priority > _retPriority)
	   {
		   _retPriority = priority;
	   }
   }

   void setNoCall(int priority, int pos)
   {
	   if (priority > _callPriority)
	   {
		   _callPriority =  -1;
	   }
   }

   void rotateOne()
   {
	   if (_decodeCalled)
		   addOne(_head);
	   if (_fetchCalled)
	   {
		   _predictedPath.setElementAt(_predictionBuffer,_writeNumber);
		   addOne(_writeNumber);
	   }
	 if (_wbCalled)
		 addOne(_tail);

	 _fetchCalled = _decodeCalled = _wbCalled = 0; 

//	 cout << "RotateOne: " << " H: " << _head << " T: " << _tail << endl;
	 
	 resetPriority();
   }

   PcType & getNextTarget()
   {
	 return _nextTarget;
   }
   
   IRPtr & getCallTarget()
   {
	   return _callTarget;
   }

   PcType & getPredCorrespToInstrInWB()
   {
	 _wbCalled = 1;
	 
//	 cout << "PredInstrWB: " << _predictedPath.elementAt(_tail) << " Num: " << _tail << endl;

	 return _predictedPath.elementAt(_tail);
   }
   
   int tookPathNotPredicted()
   {
	   PcType predTarget = getPredCorrespToInstrInWB();
	   if (predTarget != _nextTarget)
		   return _YES_;
	   if ((tookCall()) || (tookRet())) 
		   return _YES_;
	   return _NO_;
   }

   int tookCall()
   {
	   if ((_callPriority != -1) && (_callPriority == _currPriority))
		   return _YES_;
	   return _NO_;
   }

   int tookRet()
   {
	   if ((_retPriority != -1) && (_retPriority == _currPriority))
		   return _YES_;
	   return _NO_;
   }

   void resetPriority()
   {
	 _currPriority = -1;
	 _callPriority = -1;
	 _retPriority = -1;
   }
   
};

#endif

