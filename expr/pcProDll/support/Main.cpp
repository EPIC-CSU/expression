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
#include "stdafx.h"

#include "GlobalIncludes.h"
#include "FilePointer.h"
#include "NormalInstr.h"
#include "OpCodeTable.h"
#include "Routine.h"
#include "Program.h"
#include "RegisterFileList.h"
#include "SymbolTable.h"
#include "CompilerRTStatus.h"
//#include "simulator.h"
#include "IfOppositesTable.h"
#include "RenameTable.h"
#include "ControlParms.h"
#include "MyString.h"
#include "CallOp.h"
#include "RealRegArg.h"
#include "OpCodeFileDS.h"


// SAH Thu Jun 17, 99: Toy VLIW Simulator specific.
//
#include "FlowMechanism.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern NormalInstr *buildMIPSInstr(char *str, char *label = NULL);

RenameTable globalRenameTable;

extern OpCodeTable globalOpCodeTable;

extern RegisterFileList *globalRegFileList;

extern SymbolTable *globalSymbolTable;

extern IfOppositesTable globalIfOppositesTable;

extern FlowMechanism globalFlowMechanism;

extern ControlParmTable globalControlParms;

extern void initCallPass(Program *curProgram);

extern int yyparse();

extern int noSystemCalls;

extern int printCFG;

Routine* currRoutine;

Program *curProgram;
#ifdef WIN32
__declspec( dllexport ) Program* GetCurrentProgram()
{
  //NOS: DO NOT REMOVE
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
  //NOS: DO NOT REMOVE
 return curProgram;
}
#endif


extern void simulateRoutine(Routine *rPtr);

// Note the below two registers are MIPS R4000 specific.
//
RealRegArg globalHI;
RealRegArg globalLO;

void initGlobalRegFileList(char *fileName, int genericOrTarget)
{
   FilePointer fp(fileName, "r");
   char buffer[100];
   int intVar, valType;
  
// Commented out by SAH on 10/02/01.
// Now that initGlobalRegFileList is being called two times (once for generic, once for target).
//   
   //   globalRegFileList = new RegisterFileList();
   
   try
   {
	   while (fscanf(fp, "%s %d %d\n", buffer, &intVar, &valType) != EOF)
		   globalRegFileList->createRegFile(buffer, intVar, valType, genericOrTarget);
   }
   catch (CouldNotOpenFileException e)
   {
	   cout << "Panic: Could Not Open File" << fileName << endl;
	   assert(0);
   }
   
   // Added by SAH on 10/03/00
   if( genericOrTarget == _GENERIC_ )
   {
	   globalHI.setText("ghilo");
	   globalHI.setVal(0);
	   globalHI.setRegFile(globalRegFileList->getIndex("ghilo"));
	   globalLO.setText("ghilo");
	   globalLO.setVal(1);
	   globalLO.setRegFile(globalRegFileList->getIndex("ghilo"));
   }
}

void initGlobalFlowMechanism()
{
	globalFlowMechanism.initFlow();
}

void initControlParmTable(char *fileName)
{
	globalControlParms.addEntriesFromFile(fileName);
}

void setIfOppositesTable(char *fileName)
{
//	cout << endl << "*** Setting IfOpposites Table ***" << endl;

	FilePointer fp(fileName, "r");
	char op1[100], op2[100];
	char line[80];
	int res;

	while (fgets(line, 80, fp) != NULL)
	{
		res = sscanf(line, "%s %s", op1, op2);
		if (res != 2)
		{
			// error condition.
			cout << "Panic: " << fileName << " is not in the proper format" << endl;
			assert(0);
		}
		globalIfOppositesTable.addEntry(op1, op2);
	}
	fp.close();
//	cout << endl << "*** Done setting IfOpposites Table ***" << endl;
}

void initGlobalOpCodeTable(char *fileName, int genericOrTarget, int initGenericEnums)
{
	cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	cout << endl << "Reading Compiler Opcodes" << endl;

	FilePointer fp(fileName, "r");
   char buffer[100];
   char semant[200];
   char line[200];
   char classtype[200];
   int res;
   int opType;
   EnumGenericOpCode genericOpCode;
   try
      {
	while (fgets(line,200,fp)!=NULL){
	  res=sscanf(line, "%s %d %s %s", buffer, &opType, semant, classtype);
	  if (initGenericEnums)
		  genericOpCode = getGenericOpCodeEnumIndex(buffer);
		// added by Sudeep to accomodate changes
		// made in the genericOpCodeFile which now
		// has a new field for class name corresponding
		// to each generic opcode
	  if (!strcmp(fileName,"genericOpCodeFile"))
	  {
	  if (res==2)
		  globalOpCodeTable.addOpCode(buffer, genericOrTarget);

	  else if(res==3)
	    globalOpCodeTable.addOpCode(buffer, genericOrTarget, opType, genericOpCode); 
				//no semantic
	  else if(res==4)
	    globalOpCodeTable.addOpCode(buffer, genericOrTarget, opType, semant, genericOpCode);
	  else if(res==5)
	    globalOpCodeTable.addOpCode(buffer, genericOrTarget, opType, semant, genericOpCode);
	  } else
	  {


	  if (res==1)
		  globalOpCodeTable.addOpCode(buffer, genericOrTarget);

	  else if(res==2)
	    globalOpCodeTable.addOpCode(buffer, genericOrTarget, opType, genericOpCode); 
				//no semantic
	  else if(res==3)
	    globalOpCodeTable.addOpCode(buffer, genericOrTarget, opType, semant, genericOpCode);
	  else if(res==4)
	    globalOpCodeTable.addOpCode(buffer, genericOrTarget, opType, semant, genericOpCode);
	  }
  }
      }
   catch (CouldNotOpenFileException e)
      {
	 cout << "Panic: Could Not Open File" << endl;
	 assert(0);
      }
   fp.close();

	cout << endl << "Done Reading Compiler Opcodes" << endl;
	cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
}

void initGlobalSymbolTable()
{
	cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
	cout << endl << "Intializing Symbol Table" << endl;

	globalSymbolTable = new SymbolTable();
   globalSymbolTable->setSize(RegisterFileList::totNumRegs());
   
	cout << endl << "Done Initializing Symbol Table" << endl;
	cout << endl << "#++++++++++++++++++++++++++++++++#" << endl;
}

void initGlobalRenameTable()
{
	globalRenameTable.init();
}

void setSpecialCallTable(char *filename)
{
	FilePointer fp(filename, "r");
	char *buffer;
	char *opcode;
	char line[200];
	char *optype;
	int temptype = 0;
	MyString str;

	try
	{
		while (fgets(line,200,fp)!=NULL){
			optype = strtok(line, " ");			
			temptype = atoi(optype);
			buffer = strtok(NULL, " ");
			str = buffer;
			CallRetOp::addSpecialCall(temptype, str);
			if ((opcode = strtok(NULL, "\n")) != NULL)
				CallRetOp::addSpecialOpcode(temptype, globalOpCodeTable.getIndex(opcode));
		}
	}
	catch (CouldNotOpenFileException e)
	{
		cout << "Panic: Could Not Open File" << endl;
		assert(0);
	}
	fp.close();
}

void myInit3()
{
	  // First go thru all the routines in the program and set the call address
	  // (routine) pointers appropriately.
	  //
	  initCallPass(curProgram);

	  // Now set up the routines inside the globalControlParms table.
	  //
	  MyLinkedListIterator<Routine *> *rIter;
	  rIter = curProgram->routineListIteratorForw();

	  while (rIter->hasMoreElements())
	  {
		  globalControlParms.addToColIndex((rIter->currentElement())->getName());
		  rIter->nextElement();
	  }
	  delete rIter;

   //initializes numDeletedArray[] which keeps track of # deleted ops/instrs/bb
   extern void initDCEStatsObject();
   initDCEStatsObject();
}

extern int doPassFor(char *rpass, char *rname);

extern void yyrestart(FILE *filePtr);

void resetLineNumber(void);

MyString globalCurrFileName;

extern int _LAST_FLOW_POS_;

#ifdef WIN32
__declspec( dllexport ) void myBuild(char *fileName)
#else
void myBuild(char *fileName)
#endif
{
  //NOS: DO NOT REMOVE
#ifdef WIN32
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
  //NOS: DO NOT REMOVE

		_LAST_FLOW_POS_ = numFlowOpsPerInstr - 1;
  try 
    {
      //NormalInstr *currInstr;
      
      //char buffer[40], *tempLabel, label[10];
      
      FilePointer pf(fileName, "r");
      
	  // Add the file name to the program data structure.
	  // Do this by removing the ".procs" from it.
	  //
	  char *token = strtok(fileName, ".");
	  globalCurrFileName = token;

	  // 
      /*	 dup2(fileno(stdin), fileno(pf)); */
      
	  //Added GG, 061599
	  //yyrestart(pf);
	  yyrestart(pf.file_ptr());
	
	  resetLineNumber();
      
      yyparse();
      
//for test simulator in memory leak,
/*
      extern void buildSystem();
      extern void continueSim();
      extern void startSim(Routine *);

      routineIter = curProgram->routineListIteratorForw();
      while (routineIter->hasMoreElements()){
	Routine *curR = routineIter->currentElement();
        simulator *sim = new simulator(curR);
  	simulator::current = sim;
        sim->init();
        startSim(curR);
	continueSim();
	delete sim;
        routineIter->nextElement();
      }
      delete routineIter;      
*/
    }
  catch (CouldNotOpenFileException e)
    {
      cout << "Panic: Could Not Open File" << endl;
      assert(0);
    }
}

void buildIListCFGLoops()
{
      MyLinkedListIterator<Routine *> *routineIter;

      routineIter = curProgram->routineListIteratorForw();

      Routine *curR;

//    CompilerRTStatus :: printStaticStats(cout);
	 
      while (routineIter->hasMoreElements())
	{
	   curR = routineIter->currentElement();

	   curR->initStatus();

	   curR->printName(cout);

//	   curR->getStatusObj().addDynamicTransformation("PRINT_ROUTINE", _YES_);

//	   curR->print();

	   curR->doPass("BUILD_INSTRFLOW");
//	   curR->getStatusObj().addDynamicTransformation("BUILD_INSTRFLOW", _YES_);
//	   curR->print();

//	   curR->getStatusObj().addDynamicTransformation("PRINT_ROUTINE", _YES_);

	   curR->buildCFG();
	   curR->getStatusObj().addDynamicTransformation("BUILD_CFG", _YES_);

//	   if(printCFG) curR->printCFG();
//	   curR->getStatusObj().addDynamicTransformation("PRINT_CFG", _YES_);

	   curR->buildCFPreds();
	   curR->getStatusObj().addDynamicTransformation("BUILD_CFPREDS", _YES_);

//	   curR->printCFG();
//	   curR->getStatusObj().addDynamicTransformation("PRINT_CFG", _YES_);

//	   curR->buildLoopList();
//	   curR->getStatusObj().addDynamicTransformation("BUILD_LOOPLIST", _YES_);

/*
	   if (doPassFor("PRINT_LOOPS", curR->getName()))
	   {
		   curR->printLoops();
		   curR->getStatusObj().addDynamicTransformation("PRINT_LOOPS", _YES_);
	   }
*/

//	   curR->testDisamb();
	   
//	  curR->buildSSA();
#ifdef DEBUG
//	  curR->printCFG();
#endif
//        curR->compDefs();
//	  curR->compUDChain();

#ifdef DEBUG
//	  curR->print();
#endif

	 // (routineIter->currentElement())->compLiveDead();
#ifdef DEBUG
	  //(routineIter->currentElement())->printCFG();
#endif

// For simulation.

#ifdef EXPRESS_DEBUG
   simulateRoutine(routineIter->currentElement());
#endif
	   
	 // curR->print();

	  routineIter->nextElement();
	}
      
      delete routineIter;
}

void printStaticStatus(ostream & out)
{
   out << endl;
   CompilerRTStatus :: printStaticStats(out);
}


void initStatusTable(char *fileName)
{
   CompilerRTStatus :: init(fileName);
}


//ATTENTION! need to comment this out to include system calls
//#define _NO_SYSTEM_CALLS_

#ifndef _NO_SYSTEM_CALLS_
extern void initSystemCallsTable();
#endif

#ifdef WIN32
__declspec( dllexport ) void myInit1()
#else
void myInit1()
#endif
{
  //NOS: DO NOT REMOVE
#ifdef WIN32
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
  //NOS: DO NOT REMOVE

//   curProgram = new Program();
   
   initGlobalOpCodeTable("genericOpCodeFile", _GENERIC_, _YES_);
   setIfOppositesTable("ifOppositesFile");
   
   // genericRegFileList as well as targetRegFileList are present
   globalRegFileList = new RegisterFileList();
   initGlobalRegFileList("genericRegFileList.txt", _GENERIC_);
   initGlobalRegFileList("targetRegFileList.txt", _TARGET_);

   initGlobalSymbolTable();
   initStatusTable("transformFile");
   initGlobalRenameTable();
   setSpecialCallTable("specialCallFile");
   initControlParmTable("controlParms");

   //ATTENTION! This needs to be uncommented when running with system calls
#ifndef _NO_SYSTEM_CALLS_
	   initSystemCallsTable();
#endif

	initName2DefineList();

	MyString s("genericOpCodeSpecFile");
	readOpCodeFile(s);
}

#ifdef WIN32
__declspec( dllexport ) void myInit2()
#else
void myInit2()
#endif
{
  //NOS: DO NOT REMOVE
#ifdef WIN32
		AFX_MANAGE_STATE(AfxGetStaticModuleState());
#endif
  //NOS: DO NOT REMOVE

   // SAH Thu Jun 17, 99: Toy VLIW Simulator specific.
   //
   initGlobalFlowMechanism();
}

//#ifdef EXPRESS_DEBUG
/*
main(int argc, char** argv) {
	myInit1();
	myInit2();
	myBuild(argv[1]);
}
*/
//#endif
