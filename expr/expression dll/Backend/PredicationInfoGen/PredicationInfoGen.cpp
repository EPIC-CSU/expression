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
 *file  :		PredicationInfoGen.cpp		                 *
 *authors :		Aviral Shrivastava                           *
 *last modified : 07/25/2000                                 *
 *                                                           *
 *contents:		Predication Information Generation			 *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info : aviral@ics.uci.edu                         *
 *************************************************************/


/*
  Predication information generation
  ----------------------------------

  We generate two files for generating the information 
  required for predication.

  1. predInfoFile :  This file contains two sections
     A) Isel information :
	 genericIsel : targetIsel1 targetIsel2 ....
	 
	 
	 B) Predication Information :
	 unpredicatedOperation : predicatedOperation



  2. targetOpCodeFile : This file contains all the target Opcodes
     targetOpcode : Index : string 

*/


#include "PredTable.h"
#include "PredicationInfoGen.h"
#include "string.h"

PredTable globalPredTable;

// I am printing the predication information in a hardcoded file
// called predInfoFile


void predicationInfoGen(NodePtr ir)
{
	printTargetOpCodeFile(ir);
	printPredInfoFile(ir);
}


void printTargetOpCodeFile(NodePtr ir)
{
	FILE *targetOpCodeFilePtr;

	/*
	 We have to go in the operation_section --> op_group  of the file
	 And then print the function name
	*/

	if ((targetOpCodeFilePtr = fopen("targetOpCodeFile", "w")) == NULL)
	{
		printf("Cannot write to targetOpCodeFile\n");
		exit(2);
	}

	if(ir->Kind() == nke_expression)
	{
		NodePtr operationSectionNode = NULL;
		
		operationSectionNode = ir->Child(nke_operation_section);
		
		
		// Now we are in the operations section
		NodeList::iterator opGroupIter;
	
		for(opGroupIter = (operationSectionNode->Children())->begin(); opGroupIter != (operationSectionNode->Children())->end();opGroupIter++)
		{
			if ((*opGroupIter)->Kind() == nke_op_group)
			{

				NodeList::iterator opCodeIter;
				for (opCodeIter = ((*opGroupIter)->Children())->begin(); opCodeIter != ((*opGroupIter)->Children())->end();opCodeIter++)
					printOpInFile(*opCodeIter, targetOpCodeFilePtr);
			}
		}
	}
	fclose(targetOpCodeFilePtr);
}


void printOpInFile(NodePtr operationNode, FILE *targetOpCodeFilePtr)
{
	if (operationNode->Kind() == nke_opcode)
	{
		fprintf(targetOpCodeFilePtr, "%s\n", operationNode->Name());
	}
}


void printPredInfoFile(NodePtr ir)
{
	FILE *predInfoFilePtr;

	globalPredTable.buildPredTable(ir);
	

	/* Format of the output file
	Each line could be like :
	gIsel = generic assign function		tIsel = corresponding target functions
    unpredOp = unpredicated function	predOp = predicated counterparts
	predOp = predicated function
    unpredOp = unpredicated function		
	*/
	
	if ((predInfoFilePtr = fopen("predInfoFile", "w")) == NULL)
	{
		printf("Cannot write to predInfoFile\n");
		exit(2);
	}

	// First print the predication info
	globalPredTable.printPredTable(cout, predInfoFilePtr);

	// Now print the Isel information
	printCorresTarAssign(ir, predInfoFilePtr);
	
	fclose(predInfoFilePtr);
}


void printCorresTarAssign(NodePtr ir, FILE *predInfoFilePtr)
{
	if(ir->Kind() == nke_expression)
	{
		NodePtr operationMappingNode = NULL;
		NodePtr opcodeMappingNode = NULL;;
		
		operationMappingNode = ir->Child(nke_opmapping_section);
		opcodeMappingNode = operationMappingNode->Child(nke_opcode_mapping);

		// Now we are in the operationMapping section
		NodeList::iterator opMappingIter;

		for (opMappingIter = (opcodeMappingNode->Children())->begin(); opMappingIter != (opcodeMappingNode->Children())->end(); opMappingIter++)
		{
			if ((*opMappingIter)->Kind() == nke_opcode_map)
			{
				// If the generic operation is ASSIGN, then print the ASSIGN
				// and the corresponding function/s 

				print_isels(*opMappingIter, predInfoFilePtr);
			}
		}
	}
}


void print_isels(NodePtr opMappingNode, FILE *predInfoFilePtr)
{
	// If the generic function is assign then

	NodePtr genericNode = opMappingNode->Child(nke_generic);
	NodePtr targetNode = opMappingNode->Child(nke_target);

	NodePtr genericOperation = genericNode->Child(nke_operation);

	if ((strcmp(genericOperation->Name(), "ASSIGN" ) == 0) ||
		(strcmp(genericOperation->Name(), "IASSIGN") == 0) ||
		(strcmp(genericOperation->Name(), "FASSIGN") == 0) ||
		(strcmp(genericOperation->Name(), "DASSIGN") == 0))
	{
		// When you find an assign statement
		// first print the generic operation
		fprintf(predInfoFilePtr, "gIsel = %s\ttIsel =", genericOperation->Name());

		// Now print the corresponding target specific operations

		NodeList::iterator targetOperIter;
		for (targetOperIter = (targetNode->Children())->begin(); targetOperIter != (targetNode->Children())->end(); targetOperIter++)
		{
			if ((*targetOperIter)->Kind() == nke_operation)
				fprintf(predInfoFilePtr, " %s", (*targetOperIter)->Name());
			fprintf(predInfoFilePtr, "\n");				
		}
	}
}
