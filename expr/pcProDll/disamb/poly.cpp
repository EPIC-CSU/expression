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

#include "disamb.h"
#include "exprs.h"
#include "expr_utils.h"

#include "intMatrix.h"
#include "BasicSet.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


//builds the two matrixes for the expression e
//here the expr e should be something like i1*c1+i2*c2+i3*c3+...+cn, where c are constants, and i are loop indexes
void build_coord_matrix(intMatrix *&coord,intMatrix *&constr,expr e){
	//find the matrix sizes, as the number of variables in the expression
	int nr_vars=e->count_vars();
	//create the two new matrixes (coord and constr)
	coord=new intMatrix(1,nr_vars);
	constr=new intMatrix(nr_vars*2,nr_vars);
	//build the matrixes for the expression e
	e->build_matrix(coord,constr);
}



//disambiguates between e1 and e2 representing two load/store addresses
//returns the disambiguation result
disambType poly_disamb(expr e1,expr e2){
  intMatrix *coord1,*coord2,*constr1,*constr2;
  BasicSet *bs1,*bs2;
  long int res;
  //first simplify the two expressions
  simplify(e1);
  simplify(e2);
  //if too compliated, forget it (e.g., power, div, ...)
  if(e1->too_complicated()) return DONTKNOW;
  if(e2->too_complicated()) return DONTKNOW;
  //build the coordinates matrixes
  build_coord_matrix(coord1,constr1,e1);
  build_coord_matrix(coord2,constr2,e2);
  //construct the basic sets, ie the polyhedrons
  bs1=ConstructBasicSet(*coord1,*constr1);
  bs2=ConstructBasicSet(*coord2,*constr2);
  //intersect the two basic sets
  res=*bs1 & *bs2;
  //if intersection is NULL, means they are disjoint
  if(res==0) return DISJOINT;
  //otherwise they are EQUAL, or OVERLAPING
  else if(res==bs1->nrPts() || res==bs2->nrPts()) return EQUAL;
  else if(res>0) return OVERLAPING;
  else return DONTKNOW;
}
