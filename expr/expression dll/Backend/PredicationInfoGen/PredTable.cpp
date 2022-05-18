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
/*************************************************************
 *file  :		PredTable.cpp							     *
 *authors :		Aviral Shrivastava                           *
 *last modified : 07/25/2000                                 *
 *                                                           *
 *contents:		Predication Table Definition				 *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info : aviral@ics.uci.edu                         *
 *************************************************************/

#include "PredTableNode.h"
#include "PredTable.h"

//Default constructor
PredTable::PredTable()
{
}

//Default destructor
PredTable::~PredTable()
{
}

// 3) Copy Constructor
PredTable::PredTable(const PredTable & a)
{
	_predTable = a._predTable;
}

// 4) operator =
PredTable::operator = (const PredTable & a)
{
	_predTable = a._predTable;
}

void PredTable::buildPredTable(NodePtr ir)
{
	/*
	If a function has PRED_SRC as one of it's operands the function is said to be predicated. 
	
	If the behaviour of an operation "X" is defined in terms of operation "Y",
	using the "if-then-else" construct, then we say that X is "predicated counterpart" of Y.
	And Also that Y is unpredicated counterpart of X.
	*/
	
	if(ir->Kind() == nke_expression)
	{
		NodePtr operationSectionNode = NULL;
		
		operationSectionNode = ir->Child(nke_operation_section);
		
		
		// Now we are in the operations section
		NodeList::iterator opGroupIter;

		// We can have a problem if the operationSectionNode has no children
		/*
		NodeList *tempPtr;
		if ((tempPtr = operationSectionNode->Children()) == NULL)
		{
			error condition ??
			
			cout << "Panic: Recieved a Operation Section node with NULL Children List" << endl;
			assert(0);
		}
		*/

		for(opGroupIter = (operationSectionNode->Children())->begin(); opGroupIter != (operationSectionNode->Children())->end();opGroupIter++)
		{
			if ((*opGroupIter)->Kind() == nke_op_group)
			{
			/* Both the predicated and unpredicated versions of the functions must be
			present in the same group.
			Our Algorithm is a two pass.
			1st pass - make a node for each unpredicated operation
			2nd pass - add the predicated versions if the unpredicated exists, or make a new node.
			
				*/
				// First pass
				NodeList::iterator opCodeIter;
				for (opCodeIter = ((*opGroupIter)->Children())->begin(); opCodeIter != ((*opGroupIter)->Children())->end();opCodeIter++)
				{
					if(isUnPredicatedOperation(*opCodeIter))
						addUnPredicatedOperation(*opCodeIter);
				}
				
				
				//Second Pass
				for (opCodeIter = ((*opGroupIter)->Children())->begin(); opCodeIter != ((*opGroupIter)->Children())->end();opCodeIter++)
				{
					if(isPredicatedOperation(*opCodeIter))
						addPredicatedOperation(*opCodeIter);
				}
				
			}
		}
	}
}

void PredTable::addUnPredicatedOperation(NodePtr operationNode)
{
	PredTableNode newPredTableNode;
	
	newPredTableNode.addUnPredicatedOperation(operationNode);
	_predTable.push_back(newPredTableNode);
}

void PredTable::addPredicatedOperation(NodePtr operationNode)
{
	/* I have to
	1. Find out if this behavior has the if-then-else construct
	2. If it dosn't then return 0
	3. If it does, then find the operation name
	4. Search the operation, and add this operation there. 
    */

	// First find out the behavior of this node
	NodePtr behaviorNode = operationNode->Child(nke_behavior);


    /* 
      I assume that all the predicated instructions have the behavior in the format
      IF ( ****** ) THEN ***** ELSE *****
    */   

	char *behavior_string = (char*) malloc(strlen(behaviorNode->Name())+1);
	strcpy(behavior_string, behaviorNode->Name());

	if (strcmp(strtok(behavior_string, " ()"), "IF") != 0)
		printf("error in IF\n");

	char *predicate_source = strtok(NULL, " ()");

	if (strcmp(strtok(NULL, " ()"), "THEN") != 0)
		printf("error in THEN\n");
	
	char *operation_in_pred_true = strtok(NULL, " ()");
	
	if (strcmp(strtok(NULL, " ()"), "ELSE") != 0)
		printf("error in ELSE\n");
	
	char *operation_in_pred_false = strtok(NULL, " ()");

	if (strtok(NULL, " ()") != NULL)
		printf("Syntax Error\n");

	// Now I have the predicate source, operation in true predicate and false predicate.
	
	char *operation;
	if (strcmp(operation_in_pred_true, "NOP") != 0)
		operation = operation_in_pred_true;
	
	if (strcmp(operation_in_pred_false, "NOP") != 0)
		operation = operation_in_pred_false;
	
	if(operation == NULL) printf("operation is still not defined\n");
	
	// Search the operation in the predicate table formed so far
	PredNodeListIter predTableIter;
	int found_unpredicated_counterpart = 0;
	
	for(predTableIter = _predTable.begin(); predTableIter != _predTable.end(); predTableIter++)
	{
		if (strcmp(((*predTableIter).unPredicatedOperation())->Name(), operation) == 0)
		{
			(*predTableIter).addPredicatedOperation(operationNode);
			found_unpredicated_counterpart = 1;
		}
	}
	// the unpredicated counterpart is not present, then just add this operation seperately 
	if (found_unpredicated_counterpart == 0)
	{
		PredTableNode newPredTableNode;
		
		newPredTableNode.addPredicatedOperation(operationNode);
		_predTable.push_back(newPredTableNode);
	}
}


// Beware : Function Overloading.
// The next function print the table to a file
 
void PredTable::printPredTable(ostream & out)
{	
	out << "Printing Predication Table" << endl;
	
	PredNodeListIter predTableIter;
	
	for (predTableIter = _predTable.begin(); predTableIter != _predTable.end(); predTableIter++)
	{
		(*predTableIter).printPredTableNode(out);
	}		

	out << "End printing Predication Table" << endl;

}


void PredTable::printPredTable(ostream & out, FILE *predInfoFilePtr)
{	
	out << "Printing Predication Table" << endl;

	PredNodeListIter predTableIter;
	
	for (predTableIter = _predTable.begin(); predTableIter != _predTable.end(); predTableIter++)
		(*predTableIter).printPredTableNodeToFile(predInfoFilePtr);

	out << "End printing Predication Table" << endl;
}


int isPredicatedOperation(NodePtr operationNode)
{
	if (operationNode->Kind() == nke_opcode)
	{
		NodePtr operandNode = operationNode->Child(nke_operands);
		if(operandNode == NULL)
		{ 
			// printf ("operation : %s, operand code = NULL\n", operationNode->Name()); 
		}
		else
		{
			// printf ("operation : %s, operand code = NOT NULL\n", operationNode->Name());

			NodeList::iterator operandIter;
			for (operandIter = (operandNode->Children())->begin(); operandIter != (operandNode->Children())->end(); operandIter++)
			{
				if (strcmp((*operandIter)->Name(), "_PRED_SRC_") == 0)
					return 1;
			}
			return 0;
		}
	}
	return 0;
}



int isUnPredicatedOperation(NodePtr operationNode)
{
	return 1 - (isPredicatedOperation(operationNode));
}
