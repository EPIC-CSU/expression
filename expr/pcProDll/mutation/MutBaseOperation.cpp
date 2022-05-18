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

#include "BaseOperation.h"
#include "BaseArgument.h"
//#include "Routine.h"
#include "TreeBuilding.h"
#include "IburgToIR.h"
#include "UDProperty.h"
#include "DUProperty.h"
#include "UDDU.h"
#include "../DebugSwitches.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 



void printOps(set<BaseOperation*> *ops){
	set<BaseOperation*>::iterator i;
	BaseOperation *oper;

	cout << "Printing old ops: " << endl;
	for(i=ops->begin();i!=ops->end();i++){
		oper=*i;
		cout << oper->idValue() << endl;
	}
}

int BaseOperation::mutation(BaseOperation *&new_op){
	NODEPTR_TYPE p,q;
	int replaceTree=0;
	NormalInstr *instr=this->getInstr();
   	if(debugMut) cout << endl;
	ClearIburgToOpMap();
	ClearIburgToArgMap();
	ClearEssentialOps();
	ClearOmittedOps();
	_OldOps.clear();
	p=BuildIburgTree(1,0);
	if(debugMut) printOps(&_OldOps);
	if(p==NULL) return 0;
	do{
		if(debugMut) cout << endl;
		if(debugMut) printTree(p,0);
		if(debugMut) cout << "\n\tCovers:\n" <<flush;
		setPerformedMut(0);
		burm_label(p);
		if(debugMut) dumpCover(p, 1, 0);
		q=emitTree(p,1);
		if(PerformedMut()) replaceTree=1;
		if(debugMut) cout << endl;
		if(debugMut) printTree(q,0);
		if(debugMut) cout << endl;
		// delete p;
		// purify fix
		free(p);
		p=q;
	}while(PerformedMut());
	if(replaceTree){
		new_op=IburgToIR(p,this->ptrToOperand(_DEST_, _DEST_LIST_),this,instr);
		if(new_op!=this){
			//update UDDU chains
			//_ReinsertedOps= ops reinserted from original
			//_NewOps= newly created ops
			//_OldOps= original ops
			//_OmittedOps= ops from the original tree left out in the new tree (may be reinserted if in ReinsertedOps) 
			if(debugMut) printOps(&_OldOps);
			UpdateUDDU(&_OldOps,&_NewOps,&_ReinsertedOps);
		}
	}
	else{ 
		new_op=NULL;
		if(debugMut) cout << endl << "Not replaced (no mutation performed)/////////////////////////" << endl << endl;
	}
	if(debugMut) cout << endl;
	return replaceTree && new_op!=this;//return 1 if there was any change made (any mutation)
}
