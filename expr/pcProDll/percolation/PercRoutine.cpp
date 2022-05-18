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
//#include "IRTraversal.h"

#include "FlagsDefs.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


bool isBackArc(NormalInstr* instr, NormalInstr* nextInstr)
{
 BasicBlockNode* bb=instr->getBasicBlock();
 BasicBlockNode* bbNext=nextInstr->getBasicBlock();

 return bb->getDFSNum()>=bbNext->getDFSNum();
}

void percolateEx(Routine* routine, NormalInstr* instr)
{
 ASSERT(instr!=NULL);
 if (instr->flags.getFlag(PERC_DONE_FLAG)==false)
 {
  if (instr->flags.getFlag(PERC_FLAG)==true) //migrate the data ops in the instruction
  { 
   instr->flags.setFlag(PERC_DONE_FLAG,true);
   
   ArrayIterator<DataOpSlot> *dIter = instr->getDataOpSlotIter(); 
   while (dIter->hasMoreElements())
   {
    if ((dIter->currentElement()).hasOper())
    {
     (dIter->currentElement()).getPtrToOperation()->percMigrate(routine);
    }  
    dIter->nextElement();
   }
   delete dIter;
  }
 }

 //NOSFIX what do we do with the control ops?!

 //then migrate instr's successor(s) if any
 NormalInstr* firstSucc=(NormalInstr*)(instr->getFirstSucc());
 NormalInstr* nextSucc=(NormalInstr*)(instr->getNextSucc());
 bool canBeBackArc=(firstSucc!=NULL) && (nextSucc!=NULL);

 if (firstSucc!=NULL)
 {
  if (!isBackArc(instr, firstSucc) || !canBeBackArc)
  {
   percolateEx(routine, firstSucc);
  }
 }
 if (nextSucc!=NULL)
 {
  if (!isBackArc(instr, nextSucc) || !canBeBackArc)
  {
   percolateEx(routine, nextSucc);
  }
 }
}

void Routine::percolate()
{
 NormalInstr* instr=(NormalInstr*)(getFirstInstr());

 //make sure program mappings are up to date
 extern void buildProgramMappings();
 buildProgramMappings();

 //no reason to migrate ops from the first instr
 instr=(NormalInstr*)instr->getFirstSucc();  

 percolateEx(this, instr); 
}