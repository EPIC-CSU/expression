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
// $Id: CompilerRTStatus.h,v 1.11 1998/03/27 00:01:07 wtang Exp $
//
// File:  		CompilerRTStatus.h
// Created:		Thu Nov 06 , 97
// Last modified: 	Thu Nov 06 , 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   

#ifndef __COMPILERRTSTATUS_H__
#define __COMPILERRTSTATUS_H__

#include "GlobalIncludes.h"
#include "FilePointer.h"
#include "Array.h"
#include "ArrayIterator.h"
#include "Exceptions.h"

class StaticStatusWord
{
 public:
   
   char *_transformationName;
   int _transformID;
   int _numTimesCalled;
   Array<int> _dependences;
   
   StaticStatusWord():_dependences(0,1)
      {
	 _transformationName = NULL;
	 _transformID = _INVALID_;
	 _numTimesCalled = 0;
      }
   
   StaticStatusWord(const StaticStatusWord & a)
      {
	 if (a._transformationName != NULL)
	    _transformationName = strdup(a._transformationName);
	 
	 _transformID = a._transformID;
	 _numTimesCalled = a._numTimesCalled;
	 _dependences = a._dependences;
      }
   
   ~StaticStatusWord()
      {
	 if (_transformationName != NULL)
	    free(_transformationName);
	 // purify fix : remove the array entries
	 _dependences.removeAllElements();
      }
   
   friend int operator == (const StaticStatusWord & a, const
			   StaticStatusWord & b)
      {
	 if ((a._transformID == b._transformID) &&
	     (a._numTimesCalled == b._numTimesCalled))
	    return _YES_;
	 
	 return _NO_;
      }
   
   friend ostream & operator << (ostream & out, const StaticStatusWord
				 & s);
   
};

class DynamicStatusWord
{
 public:
   
   int _staticID;
   int _status;
   
   DynamicStatusWord(int sID = _INVALID_, int status =
		     _INVALID_) : _staticID(sID), _status(status)
			{
			}
   
   DynamicStatusWord(const DynamicStatusWord & a)
      {
	 _staticID = a._staticID;
	 _status = a._status;
      }
	 
   ~DynamicStatusWord()
      {
      }
   
   friend int operator == (const DynamicStatusWord & a, const
			   DynamicStatusWord & b)
      {
	 if ((a._staticID == b._staticID) &&
	     (a._status == b._status))
	    return _YES_;
	 
	 return _NO_;
      }
   
   friend ostream & operator << (ostream & out, const DynamicStatusWord
				 & s);
   
};

class Routine;
	 
class CompilerRTStatus
{
 private:

   static Array<StaticStatusWord> _staticStatusTable;
   
   Array<int> _numTimesCalledTable;
   
   Array<DynamicStatusWord> _dynamicStatusTable;

   static int _staticCounter;
   int _dynamicCounter;
   
   inline void _addDynamicElement(int ID, int status)
      {
	 DynamicStatusWord tempStatus(ID, status);
	 
	 _dynamicCounter++;
	 _dynamicStatusTable.appendElement(tempStatus);
	 
	 _staticStatusTable.elementAt(ID - 1)._numTimesCalled++;
	 (_numTimesCalledTable.elementAt(ID - 1))++;
      }

   StaticStatusWord & getStaticObj(int staticID);
   DynamicStatusWord & getDynamicObj(int dynamicID);
   
   int & getNumTimesCalledObj(int staticID);
   
 public:
   
   CompilerRTStatus() : _dynamicCounter(0), _numTimesCalledTable(0, 1), _dynamicStatusTable(0, 1)
      {
      }

   ~CompilerRTStatus()
      {
      }
   
   static void init(char *fileName);

   void routineInit();
	 
   static int numberOfStaticTransformations() { return _staticCounter; }

   int numberOfDynamicTransformations() { return _dynamicCounter; }

   static int transformationExists(char *transName);
				   
   static int getStaticID(char *transName);
   static int staticNumTimesCalled(char *transName);
   static int staticNumTimesCalled(int staticID);
   static void staticResetNumTimesCalled();
   int numTimesCalled(char *transName);
   void incNumTimesCalled(char *transName);

   int numTimesCalled(int staticID);
   void incNumTimesCalled(int staticID);

   void resetNumTimesCalled();
   static const char *transformationName(int staticID);
   // Returns a dynamic ID which can be used to refer into the dynamic
   // status table.
   //
   int addDynamicTransformation(char *name, int status);
   // Returns a dynamic ID which can be used to refer into the dynamic
   // status table.
   //
   int addDynamicTransformation(int staticID, int status);
   void changeStatus(int dynamicID, int status);
   static void printStaticStats(ostream & out);
   void printDynStats(ostream & out);
   int verifyDependences(char * transfName);
   void executeDependences(char *transfName,Routine *curR);
   void doPass(char *transfName,Routine * curR);
};

#endif

	       
	       
	 



