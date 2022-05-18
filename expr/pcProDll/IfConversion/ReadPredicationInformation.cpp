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
// File:  			ReadPredicationInformation.cpp
// Created:			August 20, 2000.
// Last modified: 	August 20, 2000.
// Author:			Aviral Shrivastava
// Email:			aviral@ics.uci.edu
//
////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PredicationInformation.h"
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


/*
	We have to read the predication information from a file "PredInfoFile"
	Example of PredInfoFile

    gIsel = assign	tIsel = mov	isel
	gIsel = iassign	tIsel = imov
	unpredOp = add	predOp = padd
	unpredOp = sub
	predOp = pmul

*/


// Global declaration of predication Information
// PredicationInformation GlobalPredicationInformation; 

/*
void PredicationInformation::PrintPredicationInformation()
{
	// First print the isel information

	// Next print the predication info
	predIter pIter;
	for (pIter = _predInfoList.begin(); pIter != _predInfoList.begin(); pIter++)
	{
		if (pIter != NULL)
		{
			if (!((*pIter)._unpredicatedOperation).isEmpty())
				printf("unpredOp = %s", ((*pIter)._unpredicatedOperation).getStr());
			
			if (!((*pIter)._predicatedOperation).isEmpty())
				printf("\tpredOp = %s", ((*pIter)._predicatedOperation).getStr());
			
			printf("\n");

		}
	}

}

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
		if (strtok(line_buffer_copy, " \t=") == NULL)
		{
			printf("Sorry the line is empty\n");
			exit(2);
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
				//
				// gIsel = assign tIsel = mov isel copy

				// first word should be "gIsel",					--> Check
				// second should be name of generic Isel			--> Add the generic Isel
				// third word should be "tIsel"					--> check
				// rest of the words are target Isels				--> Add the target Isels
				
				MyString gIsel = strtok(NULL, " \t=");
				AddGenericIsel(gIsel);

				if (strcmp(strcpy(word_buffer, strtok(NULL, " \t=")), "tIsel") != 0)
					printf("Error : Third word in lineno %d should be tIsel\n", lineno);
				else
				{
					for(int i=4;i<=word_count;i++)
					{
						MyString tIsel = strtok(NULL, " \t=");
						AddTargetIsel(tIsel, gIsel);
					}
				}
				continue;
			}

			if (strcmp(word_buffer, "unpredOp") == 0)
			{
				// If the first word in line is unpredOp
				//
				// unpredOp = add predOp = padd
				// 		or
				// unpredOp = add

				// First word is unpredOp							--> Check
				// second word is unpredicated function name		--> Add the unpredOp

				// If word count > 2 then							--> Check
				// 3rd word is predOp								--> check
				// 4th word is the name of the predOp				--> Add the predicated counterpart
				
				MyString unpredOp = strtok(NULL, " \t=");
				AddUnPredicatedOperation(unpredOp);

				if (word_count > 2)
				{
					// word_count should be 4  -- Assumption
					if (strcmp(strtok(NULL, " \t="), "predOp") != 0)
						printf("Error : Third word in lineno %d should be predOp\n", lineno);
					else
					{
						MyString predOp = strtok(NULL, " \t=");
						AddPredicatedOperation(predOp, unpredOp);
					}
				}
				continue;
			}
			
			if (strcmp(word_buffer, "predOp") == 0)
			{
				// If the first word in line is predOp 
				// 
				// predOp = pmul
				
				//   First word is predOp							--> check
				//   Second word is a predicated operation name	--> Add the predicated operation
				
				MyString predOp = strtok(NULL, " \t=");
				AddPredicatedOperation(predOp);	
				continue;
			}
		}
	}
	fclose(predInfoFilePtr);
}
*/