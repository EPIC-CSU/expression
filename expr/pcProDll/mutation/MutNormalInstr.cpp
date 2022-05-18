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

#include "NormalInstr.h"
#include "JumpOp.h"
#include "Instruction.h"
#include "dependency.h"
#include "BasicBlockNode.h"
#include "BaseHTGNode.h"
#include "SimpleHTGNode.h"
#include "IfHTGNode.h"
#include "ComputeOp.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "DUProperty.h"
//#include "Routine.h"
#include "../DebugSwitches.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

class Routine;
extern Routine * currRoutine;

void NormalInstr::mutation(void){
	ArrayIterator<DataOpSlot> *dataIter;
	BaseOperation *op,*new_op;
	DataOpSlot *slot;

	dataIter = _dataOpList.elements();
  
	if(debugMut) print(0);

	if(debugMut) cout << " : " << flush;

	while (dataIter->hasMoreElements()){
		slot=&(dataIter->currentElement());
		dataIter->nextElement();
		if(slot->hasOper()){
			if(debugMut) cout << endl << "Found new slot" << endl;
			op=slot->getPtrToOperation();
			if(op->isComputeOp()){
				if(op->mutation(new_op)){
					/*
					cout << "1 Deleting old UDDU" << endl;
					currRoutine->deleteUDDU();
					cout << "2 Recomputing DU..." << endl;
					currRoutine->compDefs();
					cout << "3 Recomputing UD..." << endl;
					currRoutine->compUDChain();
					cout << "4 Done recomputing UD." << endl;
					*/
				}
			}
		}
	}
	delete dataIter;
}
