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
//
// File:  			PredicationInformation.cpp
// Created:			August 20, 2000.
// Last modified: 	August 20, 2000.
// Author:			Aviral Shrivastava
// Email:			aviral@ics.uci.edu
//
////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PredicationInformation.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


// Global Op Code Table
extern OpCodeTable globalOpCodeTable;


PredicationInformation GlobalPredicationInformation;

// 1. Default Constructor
PredicationInformation::PredicationInformation()
{
}


// 2. Default Destructor
PredicationInformation::~PredicationInformation()
{
	_iselInfoList.clear();
	_predInfoList.clear();
}


// 3. Add a unpredicated operation 
void PredicationInformation::AddUnPredicatedOperation(MyString unpredOp)
{
	// printf("Adding a unpredicated operation\n");
	/*
	I see through the list to find if this operation is already present
	If it is then I do not add this function
	else I add this to the list
	*/
	
	predIter pIter;

	for(pIter = _predInfoList.begin(); pIter != _predInfoList.end(); pIter++)
		if ((*pIter)._unpredicatedOperation == unpredOp) return;

	PredInfo predInfo;
	predInfo._unpredicatedOperation = unpredOp;
	_predInfoList.push_back(predInfo);
}

// 4. Add a predicated operation corresponding to a unpredicated operation
void PredicationInformation::AddPredicatedOperation(MyString predOp, MyString unpredOp)
{
	// printf("Adding a predicated operation\n");
	/*
	Find the unpredicated operation (We may Search opposite in the list)
	Add this operation a that place
	*/
	predIter pIter;

	for(pIter = _predInfoList.begin(); pIter != _predInfoList.end(); pIter++)
	{
		if ((*pIter)._unpredicatedOperation == unpredOp)
			(*pIter)._predicatedOperation = predOp;
	}
}


// 5. Add a predicated operation which does not have a predicated counterpart
void PredicationInformation::AddPredicatedOperation(MyString predOp)
{
	// printf("Adding a predicated operation with no unpredicated counterpart!!\n");
	/*
	I see through the list to find if this operation is already present
	If it is then I do not add this function
	else I add this to the list
	*/
	
	predIter pIter;

	for(pIter = _predInfoList.begin(); pIter != _predInfoList.end(); pIter++)
		if ((*pIter)._predicatedOperation == predOp) return;

	PredInfo predInfo;
	predInfo._predicatedOperation = predOp;
	_predInfoList.push_back(predInfo);
}


// 6. Add a generic isel
void PredicationInformation::AddGenericIsel(MyString gIsel)
{
	// printf("Adding a generic Isel\n");
	/*
	I see through the list to find if this gIsel is already present
	If it is then I do not add this function
	else I add this to the list
	*/
	
	iselIter iIter;

	for(iIter = _iselInfoList.begin(); iIter != _iselInfoList.end(); iIter++)
		if ((*iIter)._genericIsel == gIsel) return;

	IselInfo iselInfo;
	iselInfo._genericIsel = gIsel;
	_iselInfoList.push_back(iselInfo);
}


// 7. Add a target specific isel corresponding to generic isel
void PredicationInformation::AddTargetIsel(MyString tIsel, MyString gIsel)
{
	// printf("Adding a target Isel\n");
	/*
	Find the generic Isel (We may Search opposite in the list)
	Add this operation a that place
	*/
	iselIter iIter;

	for(iIter = _iselInfoList.begin(); iIter != _iselInfoList.end(); iIter++)
	{
		if ((*iIter)._genericIsel== gIsel)
		{
			// Now add this tIsel to the vector _targetIselList
			((*iIter)._targetIselList).push_back(tIsel);
		}
	}
}
	

// 8. Tells whether the operation is predicated.
bool PredicationInformation::IsPredicatedOperation(MyString Op)
{
	/*
	I search through the predInfoList and search for the Op
	in the predicatedOperation field
	*/

	predIter pIter;

	for(pIter = _predInfoList.begin(); pIter != _predInfoList.end(); pIter++)
	{
		if ((!((*pIter)._predicatedOperation.isEmpty())) && ((*pIter)._predicatedOperation == Op)) 
			return true;
	}
	return false;
}


// 9. Tells whether the operation is unpredicated.
bool PredicationInformation::IsUnPredicatedOperation(MyString Op)
{
	/*
	I search through the predInfoList and search for the Op
	in the unpredicatedOperation field
	*/
	predIter pIter;

	for(pIter = _predInfoList.begin(); pIter != _predInfoList.end(); pIter++)
	{
		if ((*pIter)._unpredicatedOperation == Op) 
			return true;
	}
	return false;
}


// 10. Give Predicated counterpart of an operation
MyString PredicationInformation::GetPredicatedCounterpart(MyString unpredOp)
{
	/*
	I search through the predInfoList and search for the unpredOp
	in the unpredicatedOperation field, if I find that,
	I return the predicatedOperation
	*/
	predIter pIter;

	for(pIter = _predInfoList.begin(); pIter != _predInfoList.end(); pIter++)
	{
		if ((*pIter)._unpredicatedOperation == unpredOp) 
			return (*pIter)._predicatedOperation;
	}
	return NULL;
}


// 11. Reads the predication information from a file called PredInfoFile
//     and makes the GlobalPredicationInformation
   
void PredicationInformation::ReadPredicationInformation()
{
	FILE *predInfoFilePtr;


	if ((predInfoFilePtr = fopen("predInfoFile", "r")) == NULL)
	{
		printf("Cannot read predInfoFile\n");
		exit(2);
	}
	
	char line_buffer[200];
	int lineno = 0;
	while (fgets(line_buffer, 200, predInfoFilePtr) != NULL)
	{
		lineno++;
		// First I make a copy of line_buffer
		char line_buffer_copy[200];
		strcpy(line_buffer_copy, line_buffer);
		int word_count = 1;

		// Check if the line is not empty
		if (strtok(line_buffer_copy, " \t=\n") == NULL)
		{
			// printf("Sorry the line is empty\n");
			//
			// move on to the next line.
		}
		else
		{
			// Now I count the number of words in this line
			while(strtok(NULL, " \t=") != NULL)
				word_count++;

			char word_buffer[50];
			// Now we read the first word
			strcpy(word_buffer, strtok(line_buffer, " \t="));

			if (strcmp(word_buffer, "gIsel") == 0)
			{
				// If the first word in line is gIsel
				/*
				gIsel = assign tIsel = mov isel copy

				first word should be "gIsel",					--> Check
				second should be name of generic Isel			--> Add the generic Isel
				third word should be "tIsel"					--> check
				rest of the words are target Isels				--> Add the target Isels
				*/
				MyString gIsel = strtok(NULL, " \t=\n");
				AddGenericIsel(gIsel);
				if (strcmp(strcpy(word_buffer, strtok(NULL, " \t=\n")), "tIsel") != 0)
					printf("Error : Third word in lineno %d should be tIsel\n", lineno);
				else
				{
					for(int i=4;i<=word_count;i++)
					{
						MyString tIsel = strtok(NULL, " \t=\n");
						// Now find the predicated target Isel
						MyString predIsel = GlobalPredicationInformation.FindPredTargetIsel(tIsel);
						// And add that if it exists
						if (predIsel.isEmpty())
						{}
						else
							AddTargetIsel(predIsel, gIsel);
					}
				}
				continue;
			}

			if (strcmp(word_buffer, "unpredOp") == 0)
			{
				// If the first word in line is unpredOp
				/*
				unpredOp = add predOp = padd
						or
				unpredOp = add

				First word is unpredOp							--> Check
				second word is unpredicated function name		--> Add the unpredOp

				If word count > 2 then							--> Check
				3rd word is predOp								--> check
				4th word is the name of the predOp				--> Add the predicated counterpart
				*/
				MyString unpredOp = strtok(NULL, " \t=\n");
				AddUnPredicatedOperation(unpredOp);

				if (word_count > 2)
				{
					// word_count should be 4  -- Assumption
					if (strcmp(strtok(NULL, " \t=\n"), "predOp") != 0)
						printf("Error : Third word in lineno %d should be predOp\n", lineno);
					else
					{
						MyString predOp = strtok(NULL, " \t=\n");
						AddPredicatedOperation(predOp, unpredOp);
					}
				}
				continue;
			}
			
			if (strcmp(word_buffer, "predOp") == 0)
			{
				// If the first word in line is predOp 
				/*
				predOp = pmul
				
				  First word is predOp							--> check
				  Second word is a predicated operation name	--> Add the predicated operation
				*/
				MyString predOp = strtok(NULL, " \t=");
				AddPredicatedOperation(predOp);	
				continue;
			}
		}
	}
	fclose(predInfoFilePtr);
}


// 12. Prints the predicate information
void PredicationInformation::PrintPredicationInformation()
{
	// First print the isel information
	printf("\n\n########### Printing the Isel information #################\n");
	iselIter iIter;
	myStringIter tIter;

	for(iIter = _iselInfoList.begin(); iIter != _iselInfoList.end(); iIter++)
	{
		printf("gIsel = %s\tIsel =", ((*iIter)._genericIsel).getStr());

		for (tIter = ((*iIter)._targetIselList).begin(); tIter != ((*iIter)._targetIselList).end(); tIter++)
			printf(" %s", (*tIter).getStr());
			
		printf("\n");
	}

	// Next print the predication info
	printf("########### Printing the predication information #################\n\n");

	predIter pIter;
	for (pIter = _predInfoList.begin(); pIter != _predInfoList.end(); pIter++)
	{
		printf("unpredOp = %s", ((*pIter)._unpredicatedOperation).getStr());
		printf("\tpredOp = %s", ((*pIter)._predicatedOperation).getStr());
			
		printf("\n");
	}
}


// 13. Finds the predicated target Isel
MyString PredicationInformation::FindPredTargetIsel(MyString tIsel)
{
	if (GlobalPredicationInformation.IsPredicatedOperation(tIsel))
		return tIsel;

	return GlobalPredicationInformation.GetPredicatedCounterpart(tIsel);

}

// Fills the opc field with the Isel Operation opcode.
// Returns 0 if not possible.
int PredicationInformation::GetIselOpCode(OpCode & opc)
{
	/*
	There could be integer operations for which I should return the predicated counterpart of IASSIGN
	And if the operation is on doubles then I should return the predicated counterpart of DASSIGN
	etc etc...

    But right now I am returning only the Isel corresponding to ASSIGN command, assuming there is only one.

	*/

	iselIter iIter;
	myStringIter tIter;

	for(iIter = _iselInfoList.begin(); iIter != _iselInfoList.end(); iIter++)
	{
		if (strcmp( ((*iIter)._genericIsel).getStr(), "ASSIGN") == 0)
		{
			for (tIter = ((*iIter)._targetIselList).begin(); tIter != ((*iIter)._targetIselList).end(); tIter++)
			{
				opc.init((*tIter).getStr());
				return 1;
			}

		}
	}
	return 0;
}
