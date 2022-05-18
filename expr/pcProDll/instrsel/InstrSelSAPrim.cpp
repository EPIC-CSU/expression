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
#include <stdlib.h>
#endif

#include <math.h>

#include "NormalInstr.h"
#include "UDDU.h"

#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "SemActPrimitives.h"
#include "InstrSel.h"
#include "InstrSelSAPrim.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


int isel_index_of(NODEPTR_TYPE p,NODEPTR_TYPE kids[10],int eruleno){
	int i;
	short *nts = isel_nts[eruleno];

	for(i=0;nts[i];i++){
		if(p==kids[i]) return i;
	}
	return -1;
}

NODEPTR_TYPE isel_coresp_kid(NODEPTR_TYPE p,NODEPTR_TYPE old_kids[10],NODEPTR_TYPE new_kids[10],int eruleno){
	int i;
	NODEPTR_TYPE r;
	if(p==NULL) return NULL;
	if((i=isel_index_of(p,old_kids,eruleno))!=-1){
		r=new_kids[i];
		return r;
	}
	return NULL;
}

NODEPTR_TYPE isel_duplicate_tree(NODEPTR_TYPE p,NODEPTR_TYPE old_kids[10],NODEPTR_TYPE new_kids[10],int eruleno){
	NODEPTR_TYPE r;
	int i;
	if(p==NULL) return NULL;
	if((i=isel_index_of(p,old_kids,eruleno))!=-1){
		r=new_kids[i];
		return r;
	}

	r=new_tree(OP_LABEL(p),IS_DUPLICATE_TREE(LEFT_CHILD(p)),
								IS_DUPLICATE_TREE(RIGHT_CHILD(p)));

	set_attr(r,get_attr(p));

	if(GetIburgToOpMap(p)!=NULL){
		AddIburgToOpMap(r,GetIburgToOpMap(p));
		DeleteIburgToOpMap(p);
	}

	if(GetIburgToArgMap(p)!=NULL){
		AddIburgToArgMap(r,GetIburgToArgMap(p));
		DeleteIburgToArgMap(p);
	}

	return r;
}

