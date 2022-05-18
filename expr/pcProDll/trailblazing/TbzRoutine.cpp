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
// $Id: Routine.cc,v 1.25 1998/04/14 18:29:07 ahalambi Exp $
//
// File:  		TbzRoutine.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		G. Grun
// Email:		pgrun@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#include "stdafx.h"

#include "Routine.h"

#include "ComputeOp.h"
#include "PhiOp.h"
#include "IfOp.h"
#include "UDProperty.h"
#include "IBSLoopProperty.h"
#include "SimpleHTGNode.h"
#include "MutPass.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


//the list of elemnts to delete from all the unifiable ops in the trailblazing pass
extern MyLinkedList<UnifiableOp*> _toDeleteUnifOps; 


//Old basic block compaction routine (not used currently)
void Routine::compact(void){
	printf("\nCompacting Basic Blocks...");
	_cfg.compact();
	printf("\nDone compaction.");
}

//Old percolation routine for conditionals
//moves conditionals as high as possible
void Routine::percolateIf(void){
	printf("\nPercolating ifs...");
	_cfg.percolateIf();
	printf("\nDone percolating ifs.");
}

//Performs trailblazing for the complete routine
void Routine::trailblaze(void){
	
	if (isEmptyBodyRoutine()) return;
	
	if (!isUsedInProgram()) return;

	MyLinkedListIterator<BaseHTGNode *> *iter;
	
	//first, perform trailblazing at this level of the HTG (the outermost code, outside any loops)

	//compute the unifiable ops for this HTG level (the outermost code, outside any loops)
	compute_unifiable_ops(&_htg);
	//schedule the instructions for this HTG level
	schedule_instructions(&_htg);

//	printHTG();

	//delete the unifiable ops
	MyLinkedListIterator<UnifiableOp*> *uiter;
	UnifiableOp *elem;
	uiter=_toDeleteUnifOps.elementsForward();
	while(uiter->hasMoreElements()){
		elem=uiter->currentElement();
		uiter->nextElement();
		_toDeleteUnifOps.remove(elem);
		delete elem;
	}
	delete uiter;
	
	//then, continue performing trailblazing at the deeper levels of the HTG
	
	//for each HTG node in this routine
	
	if (_htg.isEmpty())
	{
		cout << endl << "WARNING: Can't trailblaze routine " << this->_name << " since it does not have an HTG." << endl;
	}
	
	for(iter=_htg.elementsForward();iter->hasMoreElements();iter->nextElement()){
		//perform trailblazing inside this node
		iter->currentElement()->trailblaze();
	}
	delete iter;
}

