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
// File:  		Program.cpp
// Created:		Mon Dec 28, 98
// Last modified: 	Mon Dec 28, 98
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#include "stdafx.h"

#include "Program.h"
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

#ifdef COUNT_PROGRAMS
   int NormalOp::_numPrograms = 0;	
#endif

Program::~Program()
{
	free(_name);

	MyLinkedListIterator<Routine *> *routineListIter;
	routineListIter = _routineList.elementsForward();

	while (routineListIter->hasMoreElements())
	{
	   if ((routineListIter->currentElement()) != NULL)
	  {
		   delete (routineListIter->currentElement());
	  }
	   routineListIter->nextElement();
	}
	delete routineListIter;
	
   _filesToRoutinesMap.clear();

#ifdef COUNT_PROGRAMS
	_numPrograms--;
#endif
}

Routine * Program::getRoutine(char *name)
{
	MyLinkedListIterator<Routine *> *routineListIter;
	routineListIter = _routineList.elementsForward();

	Routine *retPtr = NULL;
	while (routineListIter->hasMoreElements())
	{
	   if ((*(routineListIter->currentElement())) == name)
	  {
		   retPtr = routineListIter->currentElement();
	  }
	   routineListIter->nextElement();
	}
	delete routineListIter;

	return retPtr;
}
   
Routine * Program::getMainRoutine()
{
	MyLinkedListIterator<Routine *> *routineListIter;
	routineListIter = _routineList.elementsForward();

	Routine *retPtr = NULL;
	while (routineListIter->hasMoreElements())
	{
	   if ((*(routineListIter->currentElement())) == "_main")
	  {
		   retPtr = routineListIter->currentElement();
	  }
	   routineListIter->nextElement();
	}
	delete routineListIter;

	return retPtr;
}

void Program::doPass(char *transformName)
{
	MyLinkedListIterator<Routine *> *routineListIter;
	routineListIter = _routineList.elementsForward();

	while (routineListIter->hasMoreElements())
	{
		(routineListIter->currentElement())->doPass(transformName);
		routineListIter->nextElement();
	}
	delete routineListIter;
}

void Program::printHTG()
{
	MyLinkedListIterator<Routine *> *routineListIter;
	routineListIter = _routineList.elementsForward();

	while (routineListIter->hasMoreElements())
	{
		cout << endl;
		(routineListIter->currentElement())->printHTG();
		cout << endl << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
		routineListIter->nextElement();
	}
	delete routineListIter;
}

#include "defs_node.h"

extern vector<defs_node *> decl_list;
typedef vector<defs_node *>::iterator Defs_node_iter;

extern defs_node *get_ascii(defs_node *node);
extern Vector<int> *get_word_list(defs_node *node);
extern Vector<short> *get_half_list(defs_node *node);
extern Vector<char> *get_byte_list(defs_node *node);

extern long get_memory_assig(char *name);

const int _WORD_SIZE_ = 4;
const int _HALF_SIZE_ = 2;
const int _BYTE_SIZE_ = 1;

extern MyString globalProgramName;
extern int getNumRegs();
extern void printIndent(FilePointer & fp, int indent);

// Note: This function is very similar to the initializeMemory function in ExpressCallSupport.cpp file.
//
void initCMem(FilePointer & fp)
{
	// First go thru the EXPRESS memory initialize datastructure.
	//
	// For each location, store the initialization content.

	// First print the function header :
	//
	fprintf(fp, "void initProgMem()\n");
	fprintf(fp, "{\n");

	// now print the temporary variables.
	//
	printIndent(fp, 2);
	fprintf(fp, "Val memAssignTemp;\n\n");

	Defs_node_iter i;

	defs_node *currNode, *asciiVal;

	char *asciiStr;

	long memAddr;

	Vector<int> *wordList;
	Vector<int>::iterator wI;
	Vector<short> *halfList;
	Vector<short>::iterator hI;
	Vector<char> *byteList;
	Vector<char>::iterator bI;

	for (i = decl_list.begin(); i != decl_list.end(); i++)
	{
		if ((currNode = (*i)) != NULL) // i.e. is a valid node.
		{
			if ((asciiVal = get_ascii(currNode)) != NULL)
			{ // i.e. the decl has an ascii value that needs to be initialized.

				// First get the memory location that needs to be initialized.
				//
				// Then, get the initialization string.

				memAddr = get_memory_assig(currNode->Name());
				asciiStr = asciiVal->Name();

				MyString tstring(asciiStr);

				tstring.convertToCSyntax();

				// Now, we have the address and the initialization string.
				//
				// Create the addrOperand and the Val objects inorder to write to memory.

				printIndent(fp, 2);
				fprintf(fp, "writeString(\"%s\", %d);\n", tstring.getStr(), memAddr);
			}
			else if ((wordList = get_word_list(currNode)) != NULL)
			{
				// Iterate over wordlist, and initialize memory.
				//
				memAddr = get_memory_assig(currNode->Name());

				// A small optimization : Typically, these wordlists contain the same value stored in different locations.
				// Do not make a call to "memAssignTemp.changeValue if the current value is the same
				// as the prev value.
				//
				int prevValue = 0;
				printIndent(fp, 2);
				fprintf(fp, "memAssignTemp.changeValue(%d);\n", prevValue);

				for (wI = wordList->begin(); wI != wordList->end(); wI++)
				{
					if (*wI != prevValue)
					{
						printIndent(fp, 2);
						prevValue = *wI;
						fprintf(fp, "memAssignTemp.changeValue(%d);\n", prevValue);
					}
					printIndent(fp, 2);
					fprintf(fp, "initMemory(memAssignTemp, %d);\n", memAddr);

					memAddr += _WORD_SIZE_;
				}
				delete wordList;
			}
			else if ((halfList = get_half_list(currNode)) != NULL)
			{
				// Iterate over halflist, and initialize memory.
				//
				memAddr = get_memory_assig(currNode->Name());

				// A small optimization : Typically, these wordlists contain the same value stored in different locations.
				// Do not make a call to "memAssignTemp.changeValue if the current value is the same
				// as the prev value.
				//
				short prevValue = 0;
				printIndent(fp, 2);
				fprintf(fp, "memAssignTemp.changeValue(%d);\n", prevValue);
				printIndent(fp, 2);
				fprintf(fp, "memAssignTemp.changeType(_SHORT);\n");

				for (hI = halfList->begin(); hI != halfList->end(); hI++)
				{
					if (*hI != prevValue)
					{
						printIndent(fp, 2);
						prevValue = *hI;
						fprintf(fp, "memAssignTemp.changeValue(%d);\n", prevValue);
						printIndent(fp, 2);
						fprintf(fp, "memAssignTemp.changeType(_SHORT);\n");
					}
					printIndent(fp, 2);
					fprintf(fp, "initMemory(memAssignTemp, %d);\n", memAddr);

					memAddr += _HALF_SIZE_;
				}
				delete halfList;
			}
			else if ((byteList = get_byte_list(currNode)) != NULL)
			{
				// Iterate over halflist, and initialize memory.
				//
				memAddr = get_memory_assig(currNode->Name());

				for (bI = byteList->begin(); bI != byteList->end(); bI++)
				{
					printIndent(fp, 2);
					fprintf(fp, "memAssignTemp.changeValue(\'%c\');\n", *bI);
					printIndent(fp, 2);
					fprintf(fp, "memAssignTemp.changeType(_CHAR);\n");
					printIndent(fp, 2);
					fprintf(fp, "initMemory(memAssignTemp, %d);\n", memAddr);

					memAddr += _BYTE_SIZE_;
				}
				delete byteList;
			}
		}
	}
	fprintf(fp, "}\n");
}

int isMainRoutineForOutC = 0;

void Program::irToC(MyString & fileName)
{
	MyString fN;
	
	if (globalProgramName.isEmpty())
	{
		// give a generic name
		//
		fN = "irToC";
	}
	else
		fN = globalProgramName;
	
	fN.concat("_");
	fN.concat(fileName.getStr());
	fN.concat(".cc");
		
	FilePointer fp;
	
	try 
	{
		fp.open(fN, "w");
	}
	catch (CouldNotOpenFileException e)
	{
		cout << e;
		assert(0);
	}
	irToC(fp);
	
	fp.close();
}

void Program::irToC(FilePointer & fp)
{
	cout << endl << "++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++" << endl;
	cout << " Generating C code from IR " << endl;
	cout << endl << "++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++-++" << endl;
	
	MyLinkedListIterator<Routine *> *routineListIter;

   // Insert all the initial stuff, like #includes here.
   //
   fprintf(fp, "#include \"GlobalIncludes.h\"\n");
   fprintf(fp, "#include \"STLIncludes.h\"\n");   
   fprintf(fp, "#include \"Val.h\"\n\n");
   fprintf(fp, "#include \"SimMemSupport.h\"\n");
   fprintf(fp, "#include \"SimCallSupport.h\"\n\n");

   // Insert global variables (like Memory).
   //
   // fprintf(fp, "Map<int, Val> MEM; /* Memory declaration */\n\n");
   
   // now : RegisterFile
   //
   // fprintf(fp, "Vector<Val> RF; /* Register File declaration */\n\n");

   // Insert externs.
   //
   fprintf(fp, "extern void initAll(void);\n\n");
//   fprintf(fp, "extern void _DEBUG_PRINT(Val x);\n\n");

	Routine *tempR;

   // Insert declarations for each function.
   //
	routineListIter = _routineList.elementsForward();

	while (routineListIter->hasMoreElements())
	{
		tempR = routineListIter->currentElement();
		if (!tempR->isMainRoutine())
		{
			tempR->printReturnType(fp);
			fprintf(fp, " %s", (tempR->getName())+1);
			tempR->printFormalParmList(fp);
			fprintf(fp, ";\n");
		}
		routineListIter->nextElement();
	}
	delete routineListIter;

	fprintf(fp, "\n");

	// now, output the memory initialization function.
	//
	initCMem(fp);

	// then, output the callTable initialization function.
	//

   // Insert declarations for each function.
   //
	routineListIter = _routineList.elementsForward();

	fprintf(fp, "void initCallTable()\n{\n");

	while (routineListIter->hasMoreElements())
	{
		tempR = routineListIter->currentElement();
		if (!tempR->isMainRoutine())
		{
			printIndent(fp, 2);
			fprintf(fp, "addCall(\"%s\", %s);\n", (tempR->getName())+1, (tempR->getName())+1);
		}
		routineListIter->nextElement();
	}
	delete routineListIter;

	fprintf(fp, "}\n\n");

	// print each routine.
	//
	routineListIter = _routineList.elementsForward();

	while (routineListIter->hasMoreElements())
	{
		tempR = routineListIter->currentElement();
		if (tempR->isMainRoutine())
		{
			isMainRoutineForOutC = 1;
		}
		else
			isMainRoutineForOutC = 0;

		(routineListIter->currentElement())->irToC(fp);
		routineListIter->nextElement();
	}
	delete routineListIter;
}

Routine *getRoutineFromName(char *name)
{
	if ((name == NULL) || (curProgram == NULL)) return _NULL_;

	return (curProgram->getRoutine(name));
}

void Program::addFile(MyString & f)
{
	_filesToRoutinesMap[f];
}

void Program::addToFileRoutineMap(MyString & f, MyString & r)
{
	// Note: We do not perform any checking.
	// For ex.: To see if the same routine is being added twice.
	
	(_filesToRoutinesMap[f]).push_back(r);

/*
	cout << " R: ";
	r.print(cout);
	cout << " F: ";
	f.print(cout);
	cout << endl;
*/
}

RoutineNameList & Program::getListOfRoutines(MyString & f)
{
	// check if the file name exists.
	//
	Map<MyString, RoutineNameList>::iterator i;

	if ((i = _filesToRoutinesMap.find(f)) != _filesToRoutinesMap.end())
	{
		return ((*i).second);
	}

	// error condition.
	//
	assert(0);
	RoutineNameList& a = *(new RoutineNameList());
	return a;
}

void Program::getFileName(MyString & r, MyString & f)
{
	Map<MyString, RoutineNameList>::iterator i;
	RoutineNameListIter j;

	for (i = _filesToRoutinesMap.begin(); i != _filesToRoutinesMap.end(); i++)
	{
		for (j = (i->second).begin(); j != (i->second).end(); j++)
		{
			if (*j == r)
			{
				f = i->first;
				return;
			}
		}
	}

	// error condition. Could not find a file that contained the routine.
	//
	assert(0);
}

int Program::countInstructions()
{
	MyLinkedListIterator<Routine *> *routineListIter;
	routineListIter = _routineList.elementsForward();

	int retcnt = 0;

	while (routineListIter->hasMoreElements())
	{
		retcnt += (routineListIter->currentElement())->countInstructions();

		routineListIter->nextElement();
	}
	delete routineListIter;

	return retcnt;
}

int Program::countOperations()
{
	MyLinkedListIterator<Routine *> *routineListIter;
	routineListIter = _routineList.elementsForward();

	int retcnt = 0;

	while (routineListIter->hasMoreElements())
	{
		retcnt += (routineListIter->currentElement())->countOperations();

		routineListIter->nextElement();
	}
	delete routineListIter;

	return retcnt;
}

