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

#include <iostream.h>

#ifndef WIN32
#include <stdio.h>
#endif

#include "disamb.h"
#include "exprs.h"
#include "expr_utils.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

disambType gcd_one_pair(expr e1,expr e2,int &overlaping,int &disjoint,int &equal){
  disambType rez;
  interval *i=NULL;
  //int simplified_something;
  expr e=NULL;
  e=e1->unify(e2);
//  printf("\nUnified:");
//  e->print();
  ::simplify(e);
  i=e->get_interval();
//  printf("\nInterval: (%d,%d)\n",i->lower,i->upper); 
  
  if (i->lower>0 || i->upper<0){
    rez=DISJOINT;
  }
  else{
    rez=e->roots(); 
  }
  
  if (rez==OVERLAPING){
    overlaping=1;
  }
  else{
    if (rez==DONTKNOW){
      return rez;
    }
    else{
      if (rez==DISJOINT){
	disjoint=1;
      }
      else{
	if (rez==EQUAL){
	  equal=1;
	}
      }
    }
  }
  return rez;
}


disambType gcd_disamb(expr ex1,expr ex2)
{
  disambType rez;
  int overlaping,disjoint,equal;
  expr e1=NULL,e2=NULL;
  e1=ex1->copy();
  e2=ex2->copy();
  rez=gcd_one_pair(e1,e2,overlaping,disjoint,equal);
  //delete e1;
  //delete e2;
  return rez;
}

/*
//this version is for multiple expressions linked by ORs, for
//multiple definitions reaching a use (not the case in SSA). 
disambType gcd_disamb(expr ex1,expr ex2)
{
  disambType rez;
  expr e1=NULL,e2=NULL;
  int overlaping, disjoint, equal;
  expr_list l1=NULL,l2=NULL;
  overlaping=disjoint=equal=0;
  if (ex1->kind!=Or_expr || ex2->kind!=Or_expr) error("should be or_exprs"); 
  for(l1=OrE(ex1)->operands;l1!=NULL;l1=l1->tail){
    for(l2=OrE(ex2)->operands;l2!=NULL;l2=l2->tail){
      e1=l1->head->copy();
      e2=l2->head->copy();
      rez=gcd_one_pair(e1,e2,overlaping,disjoint,equal);
      if(rez==DONTKNOW) return rez;
    }
  }
 
  if (overlaping==1){
    return OVERLAPING;
  }
  else{
    if (equal==1 && disjoint==1){
      return OVERLAPING;
    }
    else{
      if (equal==1){
	return EQUAL;
      }
      else{
	if (DISJOINT==1){
	  return DISJOINT;
	}
      }
    }
  }
 
  //NS
  //error ("Case not handled!");
  return DONTKNOW;
}


*/

  

