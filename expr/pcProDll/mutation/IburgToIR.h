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
#ifndef _IBURGTOIR_H_
#define _IBURGTOIR_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <set>
#include <map>

#include "STLIncludes.h"

#include "BaseOperation.h"

#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "BaseOpSlot.h"
#include "DataOpSlot.h"
#include "FlowOpSlot.h"
#include "RegisterFileList.h"
#include "RealRegArg.h"
#include "IConstArg.h"
#include "FConstArg.h"
#include "OpCode.h"
#include "ComputeOp.h"
#include "NormalInstr.h"
#include "DUProperty.h"
#include "UDProperty.h"
#include "UDDU.h"
#include "MutPass.h"
#include "../DebugSwitches.h"
#include "OpCodeTable.h"
#include "MemoryOp.h"
#include "BaseInstruction.h"
#include "IfOp.h"


enum {DIDNTFIT, FIT, NOTINSTR};

//IburgToOp map
void AddIburgToOpMap(NODEPTR_TYPE p,BaseOperation *o);
BaseOperation *GetIburgToOpMap(NODEPTR_TYPE p);
void DeleteIburgToOpMap(NODEPTR_TYPE p);
void ClearIburgToOpMap(void);
NODEPTR_TYPE GetOpIburgToOpMap(BaseOperation *o);

NormalInstr *GetIburgToInstr(NODEPTR_TYPE p);

//Iburg to Arg map
void AddIburgToArgMap(NODEPTR_TYPE p,BaseArgument *o);
BaseArgument *GetIburgToArgMap(NODEPTR_TYPE p);
void DeleteIburgToArgMap(NODEPTR_TYPE p);
void ClearIburgToArgMap(void);


//Transformation Iburg To IR. 
BaseOperation* IburgToIR(NODEPTR_TYPE p,BaseArgument *old_dest,BaseOperation *old_op,NormalInstr *old_root_instr);
void DisconnectOldIRTree(void);

//Connect/DisconnectOperation
void DisconnectOperation(BaseOperation *op, BaseInstruction *instr);
void ConnectOperation(BaseOperation *op,BaseOpSlot *slot);

//EssentialOps
void ClearEssentialOps(void);
void AddEssentialOps(BaseOperation *op);

//OmittedOps
void ClearOmittedOps(void);
void AddOmittedOps(BaseOperation *op);

void UndoLastMutation(void);

int equalRegs(BaseArgument *reg1, BaseArgument *reg2);

extern set<BaseOperation *> _OldOps; //operations in the ORIGINAL tree (all of them). 
extern set<BaseOperation *> _NewOps;		//all the NEWLY created operations
extern set<BaseOperation *> _ReinsertedOps;	//all ops from original tree REINSERTED in IR because they are still needed. 

int ContainsOperation(set<BaseOperation *> &OpsSet,BaseOperation*op);


#endif