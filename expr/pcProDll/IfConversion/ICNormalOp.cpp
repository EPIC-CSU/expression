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

#include "NormalOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

void NormalOp::ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList)
{
	if (pred!=NULL)
	{
		OpPredicate* opPred=(OpPredicate*)pred;
		for (PAPList::iterator i=(PredicateArg**)(opPred->ccList.begin());i!=(opPred->ccList.end());i++) //for each predicate argument
		{
			PredicateArg* pa=*i;
			addPredicateUse(pa->op, this);
		}
	}
	
	if (pred!=NULL) //attach the predicate if not NULL (no predicate)
	{		
		MyString opcode_name;
		getOpcodeName(opcode_name);
		// If the operation is a predicated operation, then just add the predicate source
		if (GlobalPredicationInformation.IsPredicatedOperation(opcode_name.getStr()))
		{
			// just add the operands
			// addOperand((*iter), _PRED_SRC_START_ + i);
			PAPListIter iter;
			int i = _PRED_SRC_START_;
			for (iter = (((OpPredicate *)pred)->ccList).begin(); iter != (((OpPredicate *)pred)->ccList).end(); iter++)
			{
				if (*iter != NULL)
				{
					addOperand((*iter)->arg, i);
					i++;
				}
			}		
		}
		else
			//If the operation is unpredicated operation then
		{
			MyString newopcode_name = GlobalPredicationInformation.GetPredicatedCounterpart(opcode_name);
			if (newopcode_name.isEmpty())
				// This has to be predicated using Isel
			{
				printf("This instruction has to be predicated using Isel\n");
				
				
			}
			else
				// If the operation has a predicated counterpart then
			{
				changeOpCode(newopcode_name.getStr());
				
				PAPListIter iter;
				int i = _PRED_SRC_START_;
				for (iter = (((OpPredicate *)pred)->ccList).begin(); iter != (((OpPredicate *)pred)->ccList).end(); iter++)
				{
					if (*iter != NULL)
					{
						addOperand((*iter)->arg, i);
						i++;
					}
				}
			}
		}
		
	}
	else
	{
		OpPredicate* newPred=new OpPredicate("TRUE");
		addProperty(*newPred);
		
	}
}
