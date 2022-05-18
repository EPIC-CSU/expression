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
// File:  		storagealloc.cc
// Created:		Tue, Jul 27, 99
// Author:		G. Grun
// Email:		pgrun@ics.uci.edu
//
//   
// Description: life range routines

#include "stdafx.h"

#include "ControlFlowGraph.h"


//build the life ranges for this basic block
int build_lr_in_bb(BasicBlockNode *bb,NormalInstr *instr,BaseOperation *oper){
	
	return 0;
}

//builds the life ranges for the whole cfg
//cfg: input, the CFG to build the life ranges for
void build_life_ranges(ControlFlowGraph *cfg){
	_ASSERT(cfg);
	cfg->Traverse(build_lr_in_bb);
}