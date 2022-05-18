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
#include "disamb.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

void NormalInstr::testDisamb(BaseInstruction *instr){
  ArrayIterator<DataOpSlot> *dataIter1, *dataIter2;
  BaseOperation *op1,*op2;

  if(!instr->isNormalInstr()) return;


  dataIter1 = _dataOpList.elements();
  dataIter2 = ((NormalInstr*)instr)->_dataOpList.elements();
  
  while (dataIter1->hasMoreElements())
    {
      if((dataIter1->currentElement()).hasOper()){
	op1=(dataIter1->currentElement()).getPtrToOperation();
	dataIter2->reset();
	while (dataIter2->hasMoreElements())
	  {
	    if((dataIter2->currentElement()).hasOper()){
	      op2=(dataIter2->currentElement()).getPtrToOperation();
	      if(op1->isMemoryOp() && op2->isMemoryOp()){
		disamb(op1,op2);
	      }
	    }
	    dataIter2->nextElement();
	  }
      }
      dataIter1->nextElement();
    }
  delete dataIter1;
  delete dataIter2;
}
