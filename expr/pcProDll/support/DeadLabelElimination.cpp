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
// $Id$
//
// File:  		DeadLabelElimination.cpp
// Created:		Mon Nov 1, 99
// Last modified: 	Mon Nov 1, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "MyLinkedListIterator.h"
#include "ArrayIterator.h"

#include "STLIncludes.h"
#include "MyString.h"

#include "NormalInstr.h"
#include "BaseOperation.h"
#include "Routine.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

Vector<MyString> globalReferencedLabels;

// This function assumes that it is given a label arg.
// It then inserts the label (as a MyString) into the Labels Vector.
//
void addLabelToList(LabelArg *arg)
{
	MyString str(arg->lName());

	globalReferencedLabels.push_back(str);
}

// Given an operation, goes thru all its dest/source operands and adds any label
// to the list.
//
void addLabelsToList(BaseOperation *oper)
{
	// First get the list of all operands in the oper.

	if (oper == NULL) return;

	ArgList a;
	oper->allOperandsList(a);

	for (ArgListIter i = a.begin(); i != a.end(); i++)
	{
		if ((*i)->isLabelArg())
		{
			addLabelToList((LabelArg *)(*i));
		}
	}
}

// Procedure to go thru all the instructions and generate a list of Labels 
// that are referenced in the program.
// Note: Here we do not do any data- or control- analysis 
// (such as determining if the if branch will/will not be taken.)
//
//
void Routine::_genListOfUsedLabels()
{
	BaseInstruction *currInstr;

	ArrayIterator<DataOpSlot> *dIter;
	ArrayIterator<ControlOpSlot> *cIter;
	ArrayIterator<FlowOpSlot> *fIter;
	
	MyLinkedListIterator<BaseInstruction *> *iter = _instrList.elementsForward();
	
	while (iter->hasMoreElements())
	{
		currInstr = iter->currentElement();

//  currInstr->print();
		
		if (currInstr == NULL)
		{
			iter->nextElement();
			continue;
		}
		
		dIter = ((NormalInstr *)currInstr)->getDataOpSlotIter();
		
		// Now go thru each data op and see if it has a label argument.
		//
		while (dIter->hasMoreElements())
		{
			if ((dIter->currentElement()).hasOper())
			{
				addLabelsToList((dIter->currentElement()).getPtrToOperation());
			}
			dIter->nextElement();
		}
		delete dIter;

		cIter = ((NormalInstr *)currInstr)->getControlOpSlotIter();

		// Now go thru each control op and see if it has a label argument.
		//
		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
			{
				addLabelsToList((cIter->currentElement()).getPtrToOperation());
			}
			cIter->nextElement();
		}
		delete cIter;

		fIter = ((NormalInstr *)currInstr)->getFlowOpSlotIter();

		// Now go thru each flow op and see if it has a label argument.
		//
		while (fIter->hasMoreElements())
		{
			if ((fIter->currentElement()).hasOper())
			{
				addLabelsToList((fIter->currentElement()).getPtrToOperation());
			}
			fIter->nextElement();
		}
		delete fIter;

		iter->nextElement();
	}
	delete iter;
}

int isLabelReferenced(BaseArgument *arg)
{
	if (!arg->isLabelArg())
	{
		// error condition.
		//
		cout << "Warning: Expected label argument in the isLabelReferenced function" << endl;
		return _YES_;
	}

	MyString str(((LabelArg *)arg)->lName());
	
	for (Vector<MyString>::iterator i = globalReferencedLabels.begin(); i != globalReferencedLabels.end(); i++)
	{
		if ((*i) == str) return _YES_;
	}
	return _NO_;
}

// Procedure to delete the un-referenced labels.
//
// This procedure relies on geListOfUsedLabels() to get the list of labels that
// are statically referenced (and hence assumed useful).
//
//
void Routine::_deleteUselessLabels()
{
	BaseInstruction *currInstr;
	
	MyLinkedListIterator<BaseInstruction *> *iter = _instrList.elementsForward();
	
	while (iter->hasMoreElements())
	{
		currInstr = iter->currentElement();
		
		if (currInstr == NULL)
		{
			iter->nextElement();
			continue;
		}

		if (currInstr->hasLabel())
		{
			if (!isLabelReferenced(((NormalInstr *)currInstr)->getPtrToLabelArg()))
			{
				((NormalInstr *)currInstr)->deleteLabel();
			}
		}
		
		iter->nextElement();
	}
	delete iter;
}

// Method to eliminate label arguments that are not used in the program.
//
// This is needed for example to avoid building spurious basic-blocks while
// building the CFG.
//
//
void Routine::deadLabelElimination()
{
	_genListOfUsedLabels();
	_deleteUselessLabels();

	globalReferencedLabels.clear();
}