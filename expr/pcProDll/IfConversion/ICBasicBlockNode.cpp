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
#include "BasicBlockNode.h"
#include "BBIterator.h"
//#include "NormalInstr.h"
//#include "PropertyIncludes.h"
//#include "UDProperty.h"
//#include "SimpleHTGNode.h"
//#include "dependency.h"
//#include "JumpOp.h"
//#include "IfOppositesTable.h"

#include "FlagsDefs.h"

#ifdef WIN32
#include "crtdbg.h"
#endif

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

//schedule this basic block
void BasicBlockNode::ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList)
{ 
//	if (tobeConverted)
//	{
#ifdef WIN32
		_CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "!", "ifConvert for BB: 0x%08X\n", (long)this);
#endif
	BBForwIterator iter(this);
	
	//for all the instruction inside this basic block
	while(iter.hasMoreElements())
	{
		iter.currentElement()->ifConvert(pred, IselList, ifOp, OperationsList);
		iter.nextElement();
	}
//}
//now move all the contents to the bb
}

// This routine need to be removed.
void BasicBlockNode::markBlock(bool val)
{
 if (currRoutine->startBlock!=NULL) //start ifConversion
 {
  if (currRoutine->startBlock==this)
  {
   flags.setFlag(IFCV_FLAG,true);
   val=true;
  }
  else
  {
   flags.setFlag(IFCV_FLAG,val);
  }
 }
 else
 {
  flags.setFlag(IFCV_FLAG,val);
 }
 
 if (currRoutine->stopBlock!=NULL) //stop ifConversion
 {
  if (currRoutine->stopBlock==this)
  {
   val=false;
   if (this!=currRoutine->startBlock) flags.setFlag(IFCV_FLAG,false);
  }
 }
 
 //also mark this block's instructions' PERC_FLAG
	BBForwIterator it(this);
 
	while (it.hasMoreElements())
	{
		it.currentElement()->flags.setFlag(PERC_FLAG,flags.getFlag(IFCV_FLAG));
		it.nextElement();
	}
 
 //now propagate to predecessors
 BasicBlockNode* bb;
 
 if (this->getFirstEdge()!=NULL)
 {
  if (!this->getFirstEdge()->isBackArc())
  {
   bb=getFirstSucc();
   if (bb!=NULL) bb->markBlock(val);
  }
 }
 
 if (this->getNextEdge()!=NULL)
 {
  if (!this->getNextEdge()->isBackArc())
  {
   bb=getNextSucc();
   if (bb!=NULL) bb->markBlock(val);
  }
 }
}
