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

#include "Routine.h"

//#include "ComputeOp.h"
//#include "PhiOp.h"
//#include "IfOp.h"
//#include "UDProperty.h"
//#include "IBSLoopProperty.h"
//#include "SimpleHTGNode.h"
//#include "MutPass.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


extern Routine* currRoutine;
__declspec (dllexport) bool ICGenUDDU;


//performs if-conversion for the complete routine
void Routine::ifConvert(BaseOpProperty* pred, BaseOperation* ifOp)
{
 currRoutine=this;

 // DEBUG code added by SAH on Aug 31, 2000
 //
 // globalOpCodeTable.print(cout);

 //make sure program mappings are up to date
 extern void buildProgramMappings();
 buildProgramMappings();

 //do a pass thru the CFG and mark the BBs to convert base on startBlock, stopBlock
 //this->markBlocks();
 // This was the older method.

 // The decision whether to IfConvert will be taken inside the loop.

 MyLinkedListIterator<BaseHTGNode *> *iter;

 //for each HTG node in this routine
 for(iter=_htg.elementsForward();iter->hasMoreElements();iter->nextElement())
 {
	 //if-conversion inside this node
	 OpList IselList;
	 OpList OperationsList;
 	 iter->currentElement()->ifConvert(pred, IselList, ifOp, OperationsList);
 }
 delete iter;
}

void Routine::markBlocks()
{
 BasicBlockNode* bb=getCFG().getStartNode();
 bb->markBlock(startBlock==NULL); //false if there's a start node set
}
