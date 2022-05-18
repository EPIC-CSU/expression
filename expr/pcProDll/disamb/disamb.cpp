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

#ifndef WIN32
#include <stdio.h>
#endif
#include "RegisterArg.h"
#include "BaseArgProperty.h"
#include "BaseArgument.h"
#include "RealRegArg.h"
#include "OperationIncludes.h"
#include "ArgumentIncludes.h"
#include "BaseOperation.h"
#include "disamb.h"
#include "exprs.h"
#include "gcd.h"
#include "disamb_parser.h"
#include "poly.h"
#include "MemoryOp.h"
#include "IConstArg.h"
#include "../support/Assertions.h"
#include "RegisterFile.h"
#include "RegisterFileList.h"
#include "RAProperty.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


int scalarDisamb=1, gcdDisamb=1, polyDisamb=1;
extern int doneRA;
extern void delete_everything(void);

extern RegisterFileList *globalRegFileList;

int isScalar(BaseArgument * op, BaseArgument *off)
{
	if(!doneRA)
	{
		if(op->isRealRegArg() && strcmp(((RealRegArg*)op)->regFilename(),"gsp")==0 && 
			off->isImmediateArg()) return 1;
		else return 0;
	}
	else
	{
		if (!op->isRealRegArg()) return 0;

		RAProperty *raProp = (RAProperty *) op->getPtrToProperty(_RA_);
		int regID = raProp->getRegID();
		RegisterFile& regFile = globalRegFileList->getRegFile(regID);
		
		if (strcmp(regFile.name(), "sp") != 0)
			return 0;

		if (!off->isImmediateArg()) return 0;

		return 1;
	}
}


void printDisamb(disambType val){
  switch(val){
  case EQUAL: cout << "EQUAL";break;
  case DISJOINT:cout << "DISJOINT";break;
  case OVERLAPING:cout << "OVERLAPING";break;
  case DONTKNOW:cout << "DONTKNOW";break;
  default:cout << "Undeclared Disamb value in printDisamb";break;
  }
  cout << endl;
}


disambType disamb(BaseOperation *o1, BaseOperation *o2){
  disambType retval=DONTKNOW;
  charString s1(200), s2(200);
  HLInfoArgAssertion *p1,*p2;

  //check if they are memory operations (LOAD/STORE)
  if(!o1->isMemoryOp() || !o2->isMemoryOp() ||
     !((MemoryOp*)o1)->isLoad() && !((MemoryOp*)o1)->isStore() ||
     !((MemoryOp*)o2)->isLoad() && !((MemoryOp*)o2)->isStore()) 
    error("Disamb:: called with non-load/store operations");
  

  //find the actual memory operands (op1+off1, op2+off2)
  BaseArgument *op1,*off1,*op2,*off2;
  //if(((MemoryOp*)o1)->isLoad()) op1=((MemoryOp*)o1)->ptrToSourceOperand();
  //else op1=((MemoryOp*)o1)->ptrToDestOperand();
  //if(((MemoryOp*)o2)->isLoad()) op2=((MemoryOp*)o2)->ptrToSourceOperand();
  //else op2=((MemoryOp*)o2)->ptrToDestOperand();

  op1=((MemoryOp*)o1)->ptrToOperand(_SRC_1_);
  op2=((MemoryOp*)o2)->ptrToOperand(_SRC_1_);

  off1=((MemoryOp*)o1)->getOffset();
  off2=((MemoryOp*)o2)->getOffset();

  //if they have NAME properties, try to see if they are different
  p1=(HLInfoArgAssertion*)(op1->getPtrToProperty(_HLINFO_));
  p2=(HLInfoArgAssertion*)(op2->getPtrToProperty(_HLINFO_));
  
  //if the two operations are from different arrays
  if(p1!=NULL && p2!=null && strcmp(p1->getName().getStr(),p2->getName().getStr())!=0){
	  //return disjoint
	  retval=DISJOINT;
	  return retval;
  }
  
  
  if(scalarDisamb){
	  if(isScalar(op1,off1) && isScalar(op2,off2)){
		  if(off1->isEq(off2)) 
			  retval=EQUAL;
		  else retval=DISJOINT;
	  }
	  else retval=DONTKNOW;
  }

  everything=NULL;//memorize everything for subsequent deletion
  all_pairs=NULL;//memorize everything for subsequent deletion
  
  if(retval==DONTKNOW && (gcdDisamb || polyDisamb)){
	  expr e1,e2,tmp1,tmp2,tmp3,tmp4;

    e1=new plus_expr(null);
    tmp1=op1->BuildExpr(o1);
    tmp2=off1->BuildExpr(o1);
    tmp3=op2->BuildExpr(o2);
    tmp4=off2->BuildExpr(o2);
    if(tmp1!=NULL && tmp2!=NULL && tmp3!=NULL && tmp4!=NULL){
      PlusE(e1)->add_operand(tmp1);
      PlusE(e1)->add_operand(tmp2);
      e1->print();

      e2=new plus_expr(null);
      PlusE(e2)->add_operand(tmp3);
      PlusE(e2)->add_operand(tmp4);
      e2->print();

      if(gcdDisamb) retval=gcd_disamb(e1,e2);

      if(retval==DONTKNOW){
	//if(polyDisamb) retval=poly_disamb(e1,e2);
      }
	  //delete e2;
	  //delete e1;
    }
	else{
		//if(tmp1!=NULL) delete tmp1;
		//if(tmp2!=NULL) delete tmp2;
		//if(tmp3!=NULL) delete tmp3;
		//if(tmp4!=NULL) delete tmp4;
	}
  }

  //delete all 
  delete_everything();

//  printDisamb(retval);
  return retval;
}
