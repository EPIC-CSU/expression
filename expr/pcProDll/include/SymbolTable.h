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
// $Id: SymbolTable.h,v 1.9 1998/03/25 01:50:37 wtang Exp $
//
// File:  		SymbolTable.h
// Created:		Fri Oct 31, 97
// Last modified: 	Fri Oct 31, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARCHITECTURE

#ifndef __SYMBOLTABLE_H__
#define __SYMBOLTABLE_H__

#include "GlobalIncludes.h"
#include "Array.h"
#include "ArrayIterator.h"
#include "BaseArgument.h"
#include "RegisterFileList.h"
#include "SSAArgProperty.h"

const int START_NUM = 0;
const int START_POS = 0;
const int SSA_START_NUM = 1;

extern RegisterFileList *globalRegFileList;
extern int doneSSA, doneRA;

class STabEntry
{
 private:
   int _numSSAVars;		// number of SSA variables in this ST entry
   int _startNum;		// index of the first SSA variable (from this entry)

 public:
   
   STabEntry(int numVars = _INVALID_, int startNum = _INVALID_) :
      _numSSAVars(numVars), _startNum(startNum)
	 {
	 }
   
   STabEntry(const STabEntry & a)
      {
	 _numSSAVars = a._numSSAVars;
	 _startNum = a._startNum;
      }
   
   ~STabEntry() { }
   
   void setNumVars(int numVars) { _numSSAVars = numVars; }
   
   int getNumVars() { return _numSSAVars; }
   
   void setStartNum(int startNum) { _startNum = startNum; }
   
   int getStartNum() { return _startNum; }

   void setAll(int numVars, int startNum);
   void setAllMax(int numVars, int startNum);

   int nextStartNum();   
   int contains(int someNum);

   friend int operator == (const STabEntry & a, const STabEntry & b);
   
   friend ostream & operator << (ostream & out, const STabEntry & s);
   
   void print(ostream & out);   
};

class SymbolTable
{
 private:
   static Array<STabEntry> _symTab;
   
 public:

	 SymbolTable()
	 {
	 }
	 
	 ~SymbolTable(){ }
	 
	 void setSize (int k);
	 int getSize();

	 void initialize();

	 void setEntry(int index, int numSSACopies, int startNum);
	 
	 int getIndex(BaseArgument *ssaReg);
	 int getIndex(int regNum, char *regText);
	 
	 int getStartNum(int index);
	 
	 int getNumVars(int index);
	 
	 int getID(int regNum, char *regText, int ssaNum);	 
	 int getID(int idFromGlobalRegFileList, int ssaNum);

	 int getSSANum(int regNum, char *regText );	 
	 
	 int getID(BaseArgument *regArg, int ssaNum = 1);
	 int getID(BaseArgument *regArg, BaseArgProperty *ssaProp);
	 
	 // This function returns the index number given the ID.
	 // This is useful for performing a reverse mapping from ID to the
	 // actual register name and number.
	 //
	 int getIndex(int symTabID);
	    
	 static void setStartNums();
	 
	 char *getRegName(int symTabID);

	 void print(ostream & out);
	 
	 void addNewRegs(int index, int numSSAVars);
	 void addNewRegs(BaseArgument *arg, int numSSAVars);

	 int getTotNumVars();
};

typedef struct MultiArgList
{
	ArgList _defList;
	ArgList _useList;
} MultiArgList;

// The class IDToArgsMap can be used to map a unique number to a list of arguments.
//
// Typically, the symbol table number (which is a uniq ID for each arg) is used. 
// This allows querying the IR for the list of arguments with the same name
// (multiple defs and uses of the same variable).
//
class IDToArgsMap
{
private:
	Map<int, MultiArgList> _idToArgsMap;

public:
	IDToArgsMap(){}
	~IDToArgsMap(){ _idToArgsMap.erase(_idToArgsMap.begin(), _idToArgsMap.end()); }

	// destOrSrc == 1 => destination, destOrSrc == 0 => source
	void addArg(int index, BaseArgument *arg, int destOrSrc);
	void addDefArg(int index, BaseArgument *arg);
	void addUseArg(int index, BaseArgument *arg);

	// use these functions only if the index is to be gotten using the uniqID method of BaseArgument.
	// i.e. by using symbol table id as the index
	//
	// destOrSrc == 1 => destination, destOrSrc == 0 => source
	void addArg(BaseArgument *arg, int destOrSrc);
	void addDefArg(BaseArgument *arg);
	void addUseArg(BaseArgument *arg);

	MultiArgList & getArgs(int index);
	ArgList & getDefArgs(int index);
	ArgList & getUseArgs(int index);

	// use these functions only if the index is to be gotten using the uniqID method of BaseArgument.
	// i.e. by using symbol table id as the index
	//
	MultiArgList & getArgs(BaseArgument *arg);
	ArgList & getDefArgs(BaseArgument *arg);
	ArgList & getUseArgs(BaseArgument *arg);

	void eraseAll();
};

extern Map<Routine *, IDToArgsMap> globalIDToArgsMap;

#endif




