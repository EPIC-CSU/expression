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
// $Id: Program.h,v 1.6 1998/03/27 00:01:14 wtang Exp $
//
// File:  		Program.h
// Created:		Mon Nov 11, 96
// Last modified: 	Mon Nov 11, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __PROGRAM_H__
#define __PROGRAM_H__

#include "GlobalIncludes.h"
#include "MyLinkedList.h"
#include "MyLinkedListIterator.h"
#include "InstructionIncludes.h"

#include "NormalInstr.h"
#include "MyString.h"
#include "STLIncludes.h"

//#include "Routine.h"

class Routine;

// #define COUNT_PROGRAMS

typedef Vector<MyString> RoutineNameList;
typedef Vector<MyString>::iterator RoutineNameListIter;

class Program
{
 private:
   
#ifdef COUNT_PROGRAMS
   static int _numPrograms;
#endif

 protected:
      
   char *_name;
   
   MyLinkedList<Routine *> _routineList;

   Map<MyString, RoutineNameList> _filesToRoutinesMap;

 public:
   Program()
      {
	 _name = _NULL_;
	 	 
#ifdef COUNT_PROGRAMS
	 _numPrograms++;
#endif
      }
   
   Program(char *name)
      {
	 if (name != NULL)
	    _name = strdup(name);

#ifdef COUNT_PROGRAMS
	 _numPrograms++;
#endif
      }
   
   ~Program();
   
#ifdef COUNT_PROGRAMS
   static int numPrograms()
      {
	 return _numPrograms;
      }
#endif

   void setName(char *programName)
      {
	 _name = strdup(programName);
      }
   
   const char *getName()
      {
	 return _name;
      }
   
   void appendRoutine(Routine *someRoutine)
      {
	 _routineList.append(someRoutine);
      }
   
   // Note: This method cannot handle overloaded functions.
   //In order to do that, we would also need to pass the argument list.
   //
   Routine *getRoutine(char *name);

   Routine *getMainRoutine();

   // The function routineListIterator is to be used whenever a caller
   // wishes to go thru the list of routines.
   // 
   // This function creates the iterator (and initializes it). It is upto
   // the caller function to free the memory.
   //
   MyLinkedListIterator<Routine *> *routineListIteratorForw()
      {
	 return (_routineList.elementsForward());
      }

   MyLinkedListIterator<Routine *> *routineListIteratorBack()
      {
	 return (_routineList.elementsBackward());
      }
   
   void doPass(char *transformName);

   void printHTG();

   void irToC(MyString & fileName);
   void irToC(FilePointer & fp);

   // For the next four methods:
   // f = filename,
   // r = routinename
   //
   void addFile(MyString & f);
   void addToFileRoutineMap(MyString & f, MyString & r);
   RoutineNameList & getListOfRoutines(MyString & f);
   void getFileName(MyString & r, MyString & f);

   friend int operator == (const Program & a, const Program & b)
      {
	 if (strcmp(a._name, b._name) == 0)
	    return _YES_;
	 return _NO_;
      }

   friend int operator == (const Program & a, const char *s)
      {
	 if (strcmp(a._name, s) == 0)
	    return _YES_;
	 return _NO_;
      }	 

   friend ostream& operator << (ostream & out, const Program & s)
      {
	 return out << "Program: " << s._name;
      }
//   
// Later on, the above operators, == and <<, will have to be rewritten
// when we add member data to this class.

   int countInstructions();
   int countOperations();
};

extern Program *curProgram;

extern Routine *getRoutineFromName(char *name);


#endif
