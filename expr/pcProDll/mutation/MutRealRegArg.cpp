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

#include "RealRegArg.h"
#include "BaseOperation.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "UDDU.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern int doneRA;

NODEPTR_TYPE RealRegArg::BuildIburgTree(BaseOperation *o,int essential)
{
  MyLinkedList<BaseOperation*> *reachDefList;
  BaseOperation * reachDef;
  NormalInstr *instr1,*instr2;
  NODEPTR_TYPE p;


  if( !doneRA )
  {
	  if(strcmp(regFilename(),"gsp")!=0)
	  { //stack pointer
		  reachDefList=GetDefList(this);
		  if(reachDefList!=NULL && reachDefList->numItems()<=1 && !reachDefList->isEmpty())
		  {
			  reachDef=reachDefList->getHead();
			  instr1=o->getInstr();
			  instr2=reachDef->getInstr();
			  if(reachDef!=o && instr1!=NULL && instr2!=NULL && 
				  instr1->idValue()>instr2->idValue())
			  {
				  p=reachDef->BuildIburgTree(0,essential);
				  if(p!=NULL)
				  {
					  AddIburgToArgMap(p,this);
					  return p;
				  }
			  }
		  }
	  }
  }
  else
  {
	  cout << "Warning: Target Register set is not yet specified !" << endl;
	  if(strcmp(regFilename(),"sp")!=0){ //stack pointer
		  reachDefList=GetDefList(this);
		  if(reachDefList!=NULL && reachDefList->numItems()<=1 && !reachDefList->isEmpty()){
			  reachDef=reachDefList->getHead();
			  instr1=o->getInstr();
			  instr2=reachDef->getInstr();
			  if(reachDef!=o && instr1!=NULL && instr2!=NULL && 
				  instr1->idValue()>instr2->idValue()){
				  p=reachDef->BuildIburgTree(0,essential);
				  if(p!=NULL){
					  AddIburgToArgMap(p,this);
					  return p;
				  }
			  }
		  }
	  }
  }
  p=new_tree(get_term_num(Name()),0,0);
  AddIburgToArgMap(p,this);
  return p;
}