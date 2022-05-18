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
#include "disamb.h"
#include "PropertyIncludes.h"
#include "UDProperty.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"

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

expr RealRegArg::BuildExpr(BaseOperation *o)
{
  MyLinkedList<BaseOperation*> *reachDefList;
  BaseOperation * reachDef;
  BaseArgProperty * prop;
  NormalInstr *instr1,*instr2;
  expr res=NULL;
  
  if( !doneRA )
  {
	  if(strcmp(regFilename(),"gsp")==0) //stack pointer
		  res=new identifier_expr("gsp");
	  else
	  {
		  prop=getPtrToProperty(_UDCHAIN_);
		  if(prop==NULL) //it's some register
		  {
			  const char* rn=regName();
			  res=new identifier_expr((char*)rn);
		  }
		  else //it's an identifier, build expression tree for it
		  {
			  reachDefList=&((UDChainProperty*)prop)->getDefList();
			  if(reachDefList->numItems()>1 || reachDefList->isEmpty()){
				  const char* rn=regName();
				  res=new identifier_expr((char*)rn);
				  return res;
			  }
			  reachDef=reachDefList->getHead();
			  instr1=o->getInstr();
			  instr2=reachDef->getInstr();
			  if(reachDef!=o && instr1!=NULL && instr2!=NULL && 
				  instr1->idValue()>instr2->idValue()){
				  res=reachDef->BuildExpr();
			  }
		  }
	  }
  }
  else
  {
	  assert("Target Register set is not yet specified !\n");
	  if(strcmp(regFilename(),"sp")==0) //stack pointer
	  {
		  res=new identifier_expr("sp");
	  }
	  else
	  {
		  prop=getPtrToProperty(_UDCHAIN_);
		  if(prop==NULL) //it's some register
		  {
			  const char* rn=regName();
			  res=new identifier_expr((char*)rn);
		  }
		  else //it's an identifier, build expression tree for it
		  {
			  reachDefList=&((UDChainProperty*)prop)->getDefList();
			  if(reachDefList->numItems()>1 || reachDefList->isEmpty()){
				  const char* rn=regName();
				  res=new identifier_expr((char*)rn);
				  return res;
			  }
			  reachDef=reachDefList->getHead();
			  instr1=o->getInstr();
			  instr2=reachDef->getInstr();
			  if(reachDef!=o && instr1!=NULL && instr2!=NULL && 
				  instr1->idValue()>instr2->idValue()){
				  res=reachDef->BuildExpr();
			  }
		  }
	  }
  }
  
  if(res==NULL) //it's some register
  {
      const char* rn=regName();
      res=new identifier_expr((char*)rn);
  }
  return res;
}




/*
void RealRegArg::BuildExpr(BaseOperation *o, charString * str){
BaseOperation * reachDef;
BaseArgProperty * prop;
char *rn;
if(strcmp(regFilename(),"sp")==0) str->write(" sp "); 
else{
prop=getPtrToProperty(_UDCHAIN_);
    if(prop==NULL){
      rn=regName();
      str->write(rn);
    }
    else{
      reachDef=((UDChainProperty*)prop)->getDef();
      str->write("(");
      reachDef->BuildExpr(str);
      str->write(")");
      //str->write(" | ");
    }
  }
}
*/
