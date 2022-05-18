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
// $Id: CompilerRTStatus.cc,v 1.13 1998/03/27 00:02:49 wtang Exp $
//
// File:  		CompilerRTStatus.cc
// Created:		Fri Nov 07, 97
// Last modified: 	Fri Nov 07, 97
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: STATUS

#include "stdafx.h"

#include "CompilerRTStatus.h"
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

extern void initTest();
extern void initInstrSel();
extern void	newInstrSel(Routine *);
extern void initOpt();
extern void doOptimizations(Routine *);

//global variable stating whether we are inside Tbz or not (used by PipelineFree function)
int insideTbz;

void SpreadInstrsRoutine(Routine *routine);
void storage_allocation(Routine * routine);


Array<StaticStatusWord> CompilerRTStatus :: _staticStatusTable(0, 1);

int CompilerRTStatus :: _staticCounter = 0;

// Array<DynamicStatusWord> CompilerRTStatus :: _dynamicStatusTable(0, 1);

ostream & operator << (ostream & out, const StaticStatusWord & s)
{
   out << "Transformation Name: " << s._transformationName << endl;
   out << "Transform ID: " << s._transformID << endl;
   out << "Num Times Called: " << s._numTimesCalled << endl;
   return out;
}

ostream & operator << (ostream & out, const DynamicStatusWord & s)
{
   out << "Transform ID: " << s._staticID << endl;
   out << "Status: " << s._status << endl;
   return out;
}

StaticStatusWord & CompilerRTStatus::getStaticObj(int staticID)
{
   try 
      {
	 return _staticStatusTable.elementAt(staticID - 1);
      }
   catch (Exception *e)
      {
	 cout << "Panic: Invalid static ID" << endl;
#ifdef WIN32
	 cout << "Exception: " << *e;
#endif
	 assert(0);
      }
   // SAH, Nov 03, 99 : This path should never be taken.
   //
   return _staticStatusTable.elementAt(0);
}
   
DynamicStatusWord & CompilerRTStatus::getDynamicObj(int dynamicID)
{
   try 
      {
	 return _dynamicStatusTable.elementAt(dynamicID - 1);
      }
   catch (Exception *e)
      {
	 cout << "Panic: Invalid Dynamic ID" << endl;
#ifdef WIN32
	 cout << "Exception: " << *e;
#endif
	 assert(0);
      }
   // SAH, Nov 03, 99 : This path should never be taken.
   //
   return _dynamicStatusTable.elementAt(0);
}

int & CompilerRTStatus::getNumTimesCalledObj(int staticID)
{
   try 
      {
	 return _numTimesCalledTable.elementAt(staticID - 1);
      }
   catch (Exception *e)
      {
	 cout << "Panic: Invalid static ID" << endl;
#ifdef WIN32
	 cout << "Exception: " << *e;
#endif
	 assert(0);
      }
   // SAH, Nov 03, 99 : This path should never be taken.
   //
   return _numTimesCalledTable.elementAt(0);
}

void CompilerRTStatus::init(char *fileName)
{
  
  FILE *fp;
  
  if ((fp = fopen(fileName, "r")) == NULL)
    {
      cout << "Panic: Could not open transform file" << endl;
      assert(0);
    }
  
  int k;
  int i;
  
  if (fscanf(fp, "%d\n", &k) == EOF)
    {
      cout << "Panic: Transformation file is empty" << endl;
      assert(0);
    }
  
  _staticStatusTable.setSize(k);
  
  StaticStatusWord tempStatus;
  for (i = 0; i < k; i++)
    _staticStatusTable.insertElement(tempStatus);
  
  char line[200],buffer[80],*ptr;
  StaticStatusWord tempStaticStatus;
  int tempInt;
  for (i = 0; i < k; i++)
    {
      if(fscanf(fp,"%s :",buffer)<1)
	{
	  cout << "Panic: Wrong transformation format (: missing)" << endl;
	  assert(0);
	}
      _staticCounter++;
      (_staticStatusTable.elementAt(i))._transformationName =
	strdup(buffer);
      (_staticStatusTable.elementAt(i))._transformID =
	_staticCounter; 
      if (fgets(line,200,fp)==NULL)
	{
	  cout << "Panic: Wrong number of transformations" << endl;
	  assert(0);
	}
      ptr=strtok(line," \n");
      if(ptr!=NULL){
	do{
	  tempInt=getStaticID(ptr);
	  if(tempInt<0){
	    cout << "Panic: Transformation depndence inexistent (has to be previously declared)";
	    assert(0);
	  }
	  (_staticStatusTable.elementAt(i))._dependences.appendElement(tempInt);
	}while((ptr=strtok(NULL," \n"))!=NULL);
      }
    }
  fclose(fp);
}

void CompilerRTStatus::routineInit()
{
  int k = _staticStatusTable.size();
  
  if (k <=0) 
    {
      cout << "Panic: No transformations specified" << endl;
      assert(0);
    }
	 
  _numTimesCalledTable.setSize(k);
  
  for (int i = 0; i < k; i++)
    _numTimesCalledTable.insertElement(0);
}


int CompilerRTStatus::transformationExists(char *transName)
{
   int retVal = _NO_;
   
   ArrayIterator<StaticStatusWord> *staticIter;
   staticIter = _staticStatusTable.elements();
   
   while (staticIter->hasMoreElements())
      {
	 if
	    (strcmp(staticIter->currentElement()._transformationName, transName) == 0)
	       {
		  retVal = _YES_;
		  break;
	       }
	 staticIter->nextElement();
      }
   delete staticIter;
   
   return retVal;
} 
				   
int CompilerRTStatus::getStaticID(char *transName)
{
   int retVal = _INVALID_;
   
   ArrayIterator<StaticStatusWord> *staticIter;
   staticIter = _staticStatusTable.elements();
   
   while (staticIter->hasMoreElements())
      {
	 if
	    (strcmp(staticIter->currentElement()._transformationName, transName) == 0)
	       {
		  retVal =
		     staticIter->currentElement()._transformID;
		  break;
	       }
	 staticIter->nextElement();
      }
   delete staticIter;
   
   return retVal;
}

int CompilerRTStatus::staticNumTimesCalled(char *transName)
{
   int id;
   
   if ((id = CompilerRTStatus::getStaticID(transName)) == _INVALID_)
      {
	 cout << "Panic: Invalid transformation" << endl;
	 assert(0);
      }
   
   return (_staticStatusTable.elementAt(id - 1)._numTimesCalled);
}

int CompilerRTStatus::staticNumTimesCalled(int staticID)
{
   if ((staticID <= 0) || (staticID > _staticCounter))
      {
	 cout << "Panic: Invalid Static ID" << endl;
	 assert(0);
      }
   
   return (_staticStatusTable.elementAt(staticID - 1)._numTimesCalled);
}

void CompilerRTStatus::staticResetNumTimesCalled()
{
   ArrayIterator<StaticStatusWord> *staticIter;
   staticIter = _staticStatusTable.elements();
   
   while (staticIter->hasMoreElements())
      {
	 staticIter->currentElement()._numTimesCalled = 0;
	 staticIter->nextElement();
      }
   delete staticIter;
}

int CompilerRTStatus::numTimesCalled(char *transName)
{
   int id;
   
   if ((id = CompilerRTStatus::getStaticID(transName)) == _INVALID_)
      {
	 cout << "Panic: Invalid transformation " << transName << endl;
	 assert(0);
      }
   
   return (_numTimesCalledTable.elementAt(id - 1));
}

void CompilerRTStatus::incNumTimesCalled(char *transName)
{
   int id;
   
   if ((id = CompilerRTStatus::getStaticID(transName)) == _INVALID_)
      {
	 cout << "Panic: Invalid transformation " << transName << endl;
	 assert(0);
      }
   
	(_numTimesCalledTable.elementAt(id - 1))++;
}

int CompilerRTStatus::numTimesCalled(int staticID)
{
   if ((staticID <= 0) || (staticID > _staticCounter))
      {
	 cout << "Panic: Invalid Static ID" << endl;
	 assert(0);
      }
   
   return (_numTimesCalledTable.elementAt(staticID - 1));
}

void CompilerRTStatus::incNumTimesCalled(int staticID)
{
   if ((staticID <= 0) || (staticID > _staticCounter))
      {
	 cout << "Panic: Invalid Static ID" << endl;
	 assert(0);
      }
   
	(_numTimesCalledTable.elementAt(staticID - 1))++;
}

void CompilerRTStatus::resetNumTimesCalled()
{
   ArrayIterator<int> *numTimesIter;
   numTimesIter = _numTimesCalledTable.elements();
   
   while (numTimesIter->hasMoreElements())
      {
	 numTimesIter->currentElement() = 0;
	 numTimesIter->nextElement();
      }
   delete numTimesIter;
}

const char * CompilerRTStatus::transformationName(int staticID)
{
   if ((staticID <= 0) || (staticID > _staticCounter))
      {
	 cout << "Panic: Invalid Static ID" << endl;
	 assert(0);
      }
   
   return 
      (_staticStatusTable.elementAt(staticID - 1)._transformationName);
}

	 
// Returns a dynamic ID which can be used to refer into the dynamic
// status table.
//
int CompilerRTStatus::addDynamicTransformation(char *name, int status)
{
   int id;
   if ((id = getStaticID(name)) == _INVALID_)
      {
	 cout << "Panic: Invalid transformation" << endl;
	 assert(0);
      }
	 
   _addDynamicElement(id, status);
   
   return _dynamicCounter;
}

// Returns a dynamic ID which can be used to refer into the dynamic
// status table.
//
int CompilerRTStatus::addDynamicTransformation(int staticID, int status)
{
   if ((staticID <= 0) || (staticID > _staticCounter)) 
      {
	 cout << "Panic: Invalid Static ID" << endl;
	 assert(0);
      }
   
   _addDynamicElement(staticID, status);
   
   return _dynamicCounter;
}

void CompilerRTStatus::changeStatus(int dynamicID, int status)
{
   if ((dynamicID <= 0) || (dynamicID > _dynamicCounter))
      {
	 cout << "Panic: Invalid dynamic ID" << endl;
      }
   _dynamicStatusTable.elementAt(dynamicID - 1)._status =
      status;
}

void CompilerRTStatus::printStaticStats(ostream & out)
{
   ArrayIterator<StaticStatusWord> *statIter;
   statIter = _staticStatusTable.elements();
   
   out << "Printing the Compiler Transformations: " << endl;
   out << "*******************************************" << endl;
   out << endl;
   
   while (statIter->hasMoreElements())
      {
	 out << statIter->currentElement();
	 out << endl;
	 
	 statIter->nextElement();
      }
   delete statIter;
}

void CompilerRTStatus::printDynStats(ostream & out)
{
   ArrayIterator<DynamicStatusWord> *dynIter;
   dynIter = _dynamicStatusTable.elements();
   
   out << "Printing the Dynamic Status: " << endl;
   out << "*******************************************" << endl;
   out << endl;
   
   while (dynIter->hasMoreElements())
      {
	 out << "Transformation Name: " << transformationName(dynIter->currentElement()._staticID);
	 out << endl;
	 
	 out << dynIter->currentElement();
	 out << endl;
	 
	 dynIter->nextElement();
      }
   delete dynIter;
}


//returns 1 if the transformations that transfName depends on where already
//executed, 0 otherwise. It does not automaticaly execute them
int CompilerRTStatus::verifyDependences(char *transfName){
  int tempInt=getStaticID(transfName);
  ArrayIterator<int> *iter;
  iter=(getStaticObj(tempInt))._dependences.elements();
  while(iter->hasMoreElements()){
    if(numTimesCalled(iter->currentElement())==0) return 0;
    iter->nextElement();
  }
  return 1;
}

//executes the transformations on which transfName depends 
//curR is the routine for which it has to execute the depending passes
void CompilerRTStatus::executeDependences(char *transfName,Routine *curR){
  int tempInt=getStaticID(transfName);
  ArrayIterator<int> *iter;
  iter=(getStaticObj(tempInt))._dependences.elements();
  while(iter->hasMoreElements()){
    if(numTimesCalled(iter->currentElement())==0) 
      doPass((char *)transformationName(iter->currentElement()),curR);
    iter->nextElement();
  }
}

static int onlyOncePerRoutine(char *transfName)
{
	if (
		(strcmp(transfName, "PRINT_ROUTINE") == 0)
		|| (strcmp(transfName, "PRINT_CFG") == 0)
		|| (strcmp(transfName, "PRINT_LOOPS") == 0)
		|| (strcmp(transfName, "PRINT_HTG") == 0)
		|| (strcmp(transfName, "PRINT_C") == 0)
		|| (strcmp(transfName, "DUMP_IR") == 0)
//		|| (strcmp(transfName, "BUILD_INSTRFLOW") == 0) // Added by SAH on 14th Sept, 2000 for rebuild_cfg_htg
		|| (strcmp(transfName, "DEAD_CODE_ELIM") == 0)  // Added by SAH on 15th Sept, 2000 for after IfC.
		|| (strcmp(transfName, "COPY_ELIM") == 0)  // Added by SAH on 13th Dec, 2000 for post scheduling transformations.
		|| (strcmp(transfName, "TRAILBLAZING") == 0)  // Added by SAH on 13th Dec, 2000 for post scheduling transformations.
		|| (strcmp(transfName, "SPREAD_INSTRS") == 0)  // Added by SAH on 13th Dec, 2000 for post scheduling transformations.
		|| (strcmp(transfName, "REG_ALLOC") == 0)  // Added by SAH on 13th Dec, 2000 for post scheduling transformations.
		)
	{
		return _NO_;
	}
	return _YES_;
}

extern void buildSSA(Routine *);
		
void CompilerRTStatus::doPass(char *transfName, Routine * curR){
  if ((onlyOncePerRoutine(transfName)) && (numTimesCalled(transfName))){
    cout << "Warning: A pass that is executed only once per routine was called once too many for routine: " << curR->getName() << endl;
    return;
  }

  executeDependences(transfName,curR);

	incNumTimesCalled(transfName);
  addDynamicTransformation(transfName, 1);

  if(strcmp(transfName,"PRINT_ROUTINE")==0){
    curR->print();
  }
  else if(strcmp(transfName, "PRINT_ASM") == 0){
	 curR->printASMByCFG();
  } // print ASM
  else if(strcmp(transfName, "DUMP_IR") == 0){
	 curR->dumpIR();
  } // print ASM
  else if(strcmp(transfName,"PRINT_CFG")==0){
    curR->printCFG();
  }
  else if(strcmp(transfName,"PRINT_LOOPS")==0){
    curR->printLoops();
  }
  else if(strcmp(transfName,"PRINT_HTG")==0){
    curR->printHTG();
  }
  else if(strcmp(transfName,"PRINT_C")==0){
    curR->printC();
  }
  else if(strcmp(transfName,"BUILD_INSTRFLOW")==0){
    curR->buildInstrFlow();
  }
  else if(strcmp(transfName,"BUILD_CFG")==0){
    curR->buildCFG();
  }
  else if(strcmp(transfName,"BUILD_CFPREDS")==0){
    curR->buildCFPreds();
  }
  else if(strcmp(transfName,"BUILD_LOOPLIST")==0){
    curR->buildLoopList();
  }
  else if(strcmp(transfName,"BUILD_SSA")==0){

	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Building Static Single Assignment (SSA) Form." << endl;
	  buildSSA(curR);
	  cout << endl << "Done Building SSA." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"TEST_DISAMB")==0){
	  curR->testDisamb();
  }
  else if(strcmp(transfName,"COMP_DEFS")==0){
	  cout << endl << "Performing Def-Use (DU) Analysis." << endl;
	  curR->compDefs();
//	  cout << "Done computing def-use chains." << endl;
  }
  else if(strcmp(transfName,"COMP_UDCHAIN")==0){
	  cout << endl << "Performing Use-Def (UD) Analysis." << endl;
	  curR->compUDChain();
//	  cout << "Done computing use-def chains." << endl;
  }
  else if(strcmp(transfName,"COMP_LIVEDEAD")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Performing Live-Dead (LD) Analysis." << endl;
	  curR->compLiveDead();
	  cout << endl << "Done Performing LD Analysis." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"COMP_MULTICHAINS")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Building MultiChains." << endl;
	  curR->compMultiChains();
	  cout << endl << "Done Building MultiChains." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"REG_ALLOC")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Performing Register Allocation." << endl;
	  curR->registerAllocation();
	  cout << endl << "Done Performing Register Allocation." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"BB_COMPACTION")==0){
    curR->compact();
  }
  else if(strcmp(transfName,"IF_PERCOLATION")==0){
    curR->percolateIf();
  }
  else if(strcmp(transfName,"BUILD_HTG")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Building Hierarchical Task Graph (HTG)." << endl;
    curR->buildHTG();
	  cout << endl << "Done Building HTG." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"TRAILBLAZING")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Performing Trailblazing Percolation Scheduling (TiPS)." << endl;

	  insideTbz=1;
	  curR->trailblaze();
	  insideTbz=0;

	  cout << endl << "Done Trailblazing." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"PERCOLATING")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Performing Percolation Scheduling." << endl;

	  curR->percolate();

	  cout << endl << "Done Percolating." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"IFCONVERTING")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Performing IF Converting." << endl;

	  curR->ifConvert(NULL);

	  cout << endl << "Done IF Converting." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"TEST_UNROLL")==0){
	//  curR->genIBS();
    curR->testUnroll(1);
  }
  else if(strcmp(transfName, "TEST_SHIFT")==0){
	  curR->testShift(1);
  }
  else if(strcmp(transfName,"MUTATION")==0){
    curR->mutation();
  }
  else if(strcmp(transfName,"INSTRSEL")==0){
	 initInstrSel();
	newInstrSel(curR);
    // curR->InstrSel();
  }
  else if(strcmp(transfName,"TRANSMUTATION")==0){
	  initOpt();
	  doOptimizations(curR);
  }
  else if(strcmp(transfName,"THUMB")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "THUMB Conversion Starts" << endl;

	  curR->ThumbPass();

	  cout << endl << "THUMB Conversion Ends" << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else if(strcmp(transfName,"COPY_ELIM")==0){
    curR->copyElim();
  }
  else if(strcmp(transfName,"SPREAD_INSTRS")==0){
    SpreadInstrsRoutine(curR);
  }
  else if(strcmp(transfName,"SET_ID")==0){
    curR->setInstrID();
  }
  else if(strcmp(transfName,"STORAGE_ALLOCATION")==0){
    storage_allocation(curR);
  }
  else if(strcmp(transfName,"DEAD_CODE_ELIM")==0){
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	  cout << endl << "Performing Dead Code Elimination" << endl;
    curR->deadCodeElimination();
	  cout << endl << "Done Dead Code Elimination." << endl;
	  cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
  }
  else{
    cout <<"Panic: Inexistent compiler pass";
    assert(0);
  }
}


