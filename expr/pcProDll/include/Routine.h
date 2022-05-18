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
// $Id: Routine.h,v 1.30 1998/04/11 00:35:44 xji Exp $
//
// File:  		Routine.h
// Created:		Mon Nov 11, 96
// Last modified: 	Mon Nov 11, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include
//

#ifndef __ROUTINE_H__
#define __ROUTINE_H__

#include <fstream.h>
#include "GlobalIncludes.h"
#include "STLIncludes.h"
#include "MyString.h"

#include "MyLinkedList.h"
#include "MyLinkedListIterator.h"
#include "InstructionIncludes.h"

#include "NormalInstr.h"

#include "ControlFlowGraph.h"
#include "Loop.h"
#include "LoopList.h"
#include "BinGraphDFS.h"
#include "BaseHTGNode.h"
#include "MemoryOp.h"

#include "PropertyIncludes.h"
#include "PropertyList.h"
#include "BaseRoutineProperty.h"

#include "CompilerRTStatus.h"

#include "InterferenceGraph.h"

extern ofstream globalDumpASMFout; // from NoGUIMainSupportFuncs.cpp
class WalkInstr;
class OpToInstrMap;

class MultiChainProperty;
//extern bool PrintingASM;   from NoGUIMainSupportFuncs.cpp
//extern bool DumpingIR;     from NoGUIMainSupportFuncs.cpp

//class InterferenceGraph;

// #define COUNT_ROUTINES

#ifdef WIN32
typedef struct
{
	HANDLE handle;
	FILETIME creationTime;
	FILETIME exitTime;
	FILETIME kernelTime;
	FILETIME userTime;
}
THREADINFO;
#endif

// For ASM gen
void createOpcodeToOpndMapTable(FILE *);
void printTextSectionHeader(char *, ostream &);
void printTextSectionFooter(char *, ostream &);

// for ASM gen : CFG traversal
void serializeCFGPhase1(Routine *);
void asmGenPhase2(ostream &);
void irToCPhase2(FilePointer & fp, int indent);

class Routine
{
private:
	
#ifdef COUNT_ROUTINES
	static int _numRoutines
#endif
		
		void genIBSForLoop(Loop *aLoop);
	
	void genIBSForLoopList(LoopList & aList);
	
protected:
	
	char *_name;
	
	MyLinkedList<BaseInstruction *> _instrList;
		
	ControlFlowGraph _cfg;
	
	LoopList _loops;
	
	CompilerRTStatus _routineStatus;
	
	MyLinkedList<BaseHTGNode *> _htg;
	
	OpList _prologue;
	OpList _epilogue;
	
	InterferenceGraph *_ig;
	
	int _isUnstructured;
	
	// property list
	PropertyList<BaseRoutineProperty> _propList;

	int _isUsedInProgram;
	
	void _genListOfUsedLabels();
	void _deleteUselessLabels();
	
public:
	WalkInstr* iWalker;
	OpToInstrMap* oMapper;
	
	Routine() : _routineStatus(), _cfg(), _loops(), _isUnstructured(0), _isUsedInProgram(0)
	{
		_name = _NULL_;
		
		iWalker = _NULL_;
		oMapper = _NULL_;
		
		startBlock=NULL;
		stopBlock=NULL;
		
#ifdef COUNT_ROUTINES
		_numRoutines++;
#endif
	}
	
	Routine(char *name) : _routineStatus(), _cfg(), _loops(), _isUnstructured(0), _isUsedInProgram(0)
	{
		if (name != NULL)
			_name = strdup(name);
		
		iWalker = _NULL_;
		oMapper = _NULL_;
		
		startBlock=NULL;
		stopBlock=NULL;
		
#ifdef COUNT_ROUTINES
		_numRoutines++;
#endif
	}
	
	~Routine();
	
#ifdef COUNT_ROUTINES
	static int numRoutines()
	{
		return _numRoutines;
	}
#endif
	
	void doPass(char *transformName)
	{
		_routineStatus.doPass(transformName,this);
	}
	
	void setName(char *routineName)
	{
		_name = strdup(routineName);
	}
	
	char *getName()
	{
		return _name;
	}
	
	int isMainRoutine()
	{
		if ((_name != NULL) && (strcmp(_name, "_main") == 0))
			return _YES_;
		return _NO_;
	}

	void printName(ostream & out)
	{
		out << endl << "#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#*#" << endl;
		//	 out << endl << "#++++++++++++++++++++++++++++++++#" << endl;
		if (_name != NULL) out << "Routine: " << _name << endl;
	}
	
	void appendInstruction(BaseInstruction *someInstr)
	{
		_instrList.append(someInstr);
	}
	
	void insertInstrAfter(BaseInstruction *newInstr, BaseInstruction *currInstr)
	{
		_instrList.insertAfter(newInstr, currInstr);
	}

	void insertInstrBefore(BaseInstruction *newInstr, BaseInstruction *currInstr)
	{
		_instrList.insertBefore(newInstr, currInstr);
	}
	
	void removeInstruction(BaseInstruction *someInstr)
	{
		_instrList.remove(someInstr);
	}
	
	BaseInstruction *getInstruction(IDType someID)
	{
		MyLinkedListIterator<BaseInstruction *> *instrListIterator;
		
		instrListIterator = _instrList.elementsForward();
		
		// what about the case when someID is not a valid ID ? 
		// i.e. what if there is no such instruction in the list ?
		
		while (instrListIterator->hasMoreElements())
		{
			// make this a try, catch statement.
			
			if ((instrListIterator->currentElement())->idValue() == someID)
			{
				return instrListIterator->currentElement();
			}
			instrListIterator->nextElement();
		}
		delete instrListIterator;
		
		//NOS
		ASSERT_TRACE(0,"Forced EXIT!");
		return NULL;
	}
	
	MyLinkedList<BaseInstruction *>& getInstrList()
	{
		return _instrList;
	}
	
	// The function instrListIterator is to be used whenever a caller
	// wishes to go thru the list of instruction (in a particular
	// routine).
	// This function creates the iterator (and initializes it). It is upto
	// the caller function to free the memory.
	//
	MyLinkedListIterator<BaseInstruction *> *instrListIteratorForw()
	{
		return (_instrList.elementsForward());
	}
	
	MyLinkedListIterator<BaseInstruction *> *instrListIteratorBack()
	{
		return (_instrList.elementsBackward());
	}
	
	int isEmptyBodyRoutine();
	
	void buildInstrFlow();
	
	__declspec (dllexport) void buildCFG();
	
	__declspec (dllexport) void buildHTG();
	
	__declspec (dllexport) void rebuildCFG();

	__declspec (dllexport) void rebuildHTG();

	void deleteCFG()
	{
		_cfg.deleteBBList();
	}
	
	MyLinkedList<BaseHTGNode *> *getHTG()
	{
		return &_htg;
	}
	
	__declspec (dllexport) void printHTG(void);
	void printC(void);
	void createListOfOperands();	// list of operators/variables so that 
				// they can be used by printC 
	
	
	// I do not want to provide all the ControlFlowGraph methods here.
	// Instead, I have provided a reference to _cfg. Using this, other
	// functions can call the methods of the ControlFlowGraph class.
	// Although it is against the OOP philosophy (i.e. do not provide
	// direct access to member data), there is no danger of misuse here
	// because of the fact that the member data which is being provided
	// is also a class object (and hence, only its member functions can
	// be used). 
	//
	ControlFlowGraph & getCFG()
	{
		return _cfg;
	}
	
	void buildLoopList();
	//
	// This method is to be written by Weiyu. (The routine for
	// determining loops from a basic block CFG).
	
	void deleteLoopList()
	{
		_loops.deleteListOfLoops();
	}
	
	// See comment made above for the method getCFG.
	//
	LoopList & getLoopList()
	{
		return _loops;
	}
	
	void genIBS();
	//
	// Generate the Index, Bounds, Step properties for each loop.
	
	void setTargetMask();
	
	void buildCFPreds();
	
	__declspec (dllexport) void buildSSA();
	
	void initStatus()
	{
		_routineStatus.routineInit();
	}
	
	CompilerRTStatus & getStatusObj()
	{
		return _routineStatus;
	}
	
	void printStatus(ostream & out)
	{
		_routineStatus.printDynStats(out);
	}

	void setIsUsedInProgram() { _isUsedInProgram = 1; }

	int isUsedInProgram() { return _isUsedInProgram; }

	friend int operator == (const Routine & a, const Routine & b)
	{
		if (strcmp(a._name, b._name) == 0)
			return _YES_;
		return _NO_;
	}
	
	friend int operator == (const Routine & a, const char *s)
	{
		if (strcmp(a._name, s) == 0)
			return _YES_;
		return _NO_;
	}	 
	
	friend ostream& operator << (ostream & out, const Routine & s)
	{
		return out << "Routine: " << s._name;
	}
	//   
	// Later on, the above operators, == and <<, will have to be rewritten
	// when we add member data to this class.
	
	__declspec (dllexport) void printCFG();
	
	void printLoops()
	{
		cout << "Printing Loops for Routine: ";
		if (_name != NULL) cout << _name;
		cout << endl;
		
		_loops.print();
		cout << endl;
	}
	
	void print()
	{
		MyLinkedListIterator<BaseInstruction *> *instrIter;
		
		instrIter = _instrList.elementsForward();
		
		cout << "Printing Routine: ";
		if (_name != NULL) cout << _name;
		cout << endl;
		
		// int i = 0;
		
		while (instrIter->hasMoreElements())
		{
			//  cout << "Instr " << i << ":  " << instrIter->currentElement() << endl;
			//  i++;
			
			(instrIter->currentElement())->shortPrint(cout);
			//   instrIter->currentElement()->printInstrProperty(cout);
			//   instrIter->currentElement()->printOpsProperty(cout);
			//   ((NormalInstr*)instrIter->currentElement())->printArgsProperty(cout);
			
			instrIter->nextElement();
		}
		
     	cout << "End Printing Routine" << endl;
		delete instrIter;
	}

	// Populating the data structures for final generation of the text section
	
    void printASMByCFG()  // printing ASM by CFG traversal
	{
		FILE * fp;
		
		cout << "Printing ASM..." << endl;

		if((fp = fopen("operandsMappingFile.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
		
		
        createOpcodeToOpndMapTable(fp);
		fclose(fp);

		cout << "Printing Routine: ";  // debug : Partha
		
		if (_name != NULL) 
		{
			//globalDumpASMFout << "\t#########################################################################\n";
			//globalDumpASMFout << "\t# Callee allocates the stack frame and saves the callee-saved registers #\n";
			//globalDumpASMFout << "\t#########################################################################\n";
			printTextSectionHeader(_name, globalDumpASMFout);
		}
		
		cout << endl;

		serializeCFGPhase1(this); // pass the first basic block pointer

		asmGenPhase2(globalDumpASMFout); // Print the assembly instructions by traversing the BBVector
		
		printTextSectionFooter(_name, globalDumpASMFout);
		cout << "End Printing Routine" << endl; // debug : Partha
	}


	// generating generic assembly 
	void dumpIR()  // printing ASM by CFG traversal
	{
		FILE * fp;
		if((fp = fopen("dumpIRFormat.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}

        createOpcodeToOpndMapTable(fp);
		fclose(fp);

		cout << "Printing Routine: ";  // debug : Partha
		
		if (_name != NULL) 
		{
			printTextSectionHeader(_name, globalDumpASMFout);
		}
		
		cout << endl;

		serializeCFGPhase1(this); // pass the first basic block pointer

		asmGenPhase2(globalDumpASMFout); // Print the assembly instructions by traversing the BBVector
		
		printTextSectionFooter(_name, globalDumpASMFout);
		cout << "End Printing Routine" << endl; // debug : Partha
	}

	void printProperties(ostream & out, PropNameList & pL)
	{
		MyLinkedListIterator<BaseInstruction *> *instrIter;
		
		instrIter = _instrList.elementsForward();
		
		out << "Printing Routine with properties: ";
		if (_name != NULL) out << _name;
		out << endl;
		
		while (instrIter->hasMoreElements())
		{   
			(instrIter->currentElement())->printProperties(out, pL);
			out.flush();
			
			instrIter->nextElement();
		}
		out << "End Printing Routine" << endl;
		
		delete instrIter;
	}
	
	void testDisamb(void);  //This is only for testing the Disambiguator. The code is in the disamb part.
	
	void compDefs();
	void compUDChain();
	void compMultiChains();
	void compLiveDead();
	void cnstProp();
	__declspec (dllexport) void deadCodeElimination();
	__declspec (dllexport) void deadLabelElimination();
	void valueProp();
	void copyProp();
	void copyElim();
	
	InterferenceGraph& getIG()
	{
		return *_ig;
	}
	
	void createIG()
	{
		_ig = new InterferenceGraph();
	}

	void deleteIG()
	{
		delete _ig;
	}

	void printIG() {
		cout << _ig;
	}
	
	void buildIG();
	void registerAllocation();
	
	void compExecPossib();
	void RegAlloc();
	
	int isUnstructured(int recompute = 1);
	
	void getFileName(MyString & str);

	/*  printing the instruction list
	void printASM()  // printing ASM
	{
		MyLinkedListIterator<BaseInstruction *> *instrIter;

		// debug Partha
		FILE * fp;
		if( PrintingASM == true )
		{
			if((fp = fopen("operandsMappingFile.txt", "r")) ==NULL){
				assert("Unable to open file \n");
			}
		}
		else // if( DumpingIR == true )
		{
			if((fp = fopen("operandsMappingFile.txt", "r")) ==NULL){
				assert("Unable to open file \n");
			}
		}

        createOpcodeToOpndMapTable(fp);
		fclose(fp);

// debug Partha

		
		instrIter = _instrList.elementsForward();

		cout << "Printing Routine: ";  // debug : Partha
//globalDumpASMFout << "Printing Routine: ";
		
		if (_name != NULL) 
		{
			globalDumpASMFout << "\t#########################################################################\n";
			globalDumpASMFout << "\t# Callee allocates the stack frame and saves the callee-saved registers #\n";
			globalDumpASMFout << "\t#########################################################################\n";
			printTextSectionHeader(_name, "operandsMappingFile.txt", globalDumpASMFout);
//
			cout << _name;
			globalDumpASMFout << "\t.globl\t";  // EXPR text file
			globalDumpASMFout << _name << endl;
			globalDumpASMFout << "\t.ent\t";  // EXPR text file
			globalDumpASMFout << _name << endl;
			globalDumpASMFout << _name << ":" << endl;
//
		}
		
		cout << endl;
		
		// int i = 0;
		
		while (instrIter->hasMoreElements())
		{
			//  cout << "Instr " << i << ":  " << instrIter->currentElement() << endl;
			//  i++;
			

			(instrIter->currentElement())->printASMInstr(globalDumpASMFout);  // generate ASM file
			//   instrIter->currentElement()->printInstrProperty(globalDumpASMFout);
			//   instrIter->currentElement()->printOpsProperty(globalDumpASMFout);
			//   ((NormalInstr*)instrIter->currentElement())->printArgsProperty(globalDumpASMFout);
			
			instrIter->nextElement();
		}
//
		globalDumpASMFout << "\t.end\t";  // EXPR text file
		globalDumpASMFout << _name << endl;
//
		printTextSectionFooter(_name, globalDumpASMFout);
		cout << "End Printing Routine" << endl; // debug : Partha

		delete instrIter;
	}
	*/
	
	void print(ostream& out)
	{
		MyLinkedListIterator<BaseInstruction *> *instrIter;
		
		instrIter = _instrList.elementsForward();
		
		out << "Printing Routine: ";
		if (_name != NULL) cout << _name;
		cout << endl;
		
		int i = 0;
		
		while (instrIter->hasMoreElements())
		{
			out << "Instruction " << i << ":  " << instrIter->currentElement() << endl;
			i++;
			
			(instrIter->currentElement())->print(out);
			instrIter->nextElement();
		}
		
		out << "End Printing Routine" << endl;
		
		delete instrIter;
	}

	void printReturnType(FilePointer & fp);
	void printFormalParmList(FilePointer & fp);
	void irToC(FilePointer & fp);

	void compact(void);
	void percolateIf(void);
	__declspec (dllexport) void trailblaze(void);
	void mutation(void);
	void InstrSel(void);
	void setInstrID(void);
	__declspec (dllexport) void fixIfHead(void);
	
	void testUnroll(int innerLoopOnly = 1);
	void testShift(int innerLoopOnly = 1);
	
	BaseInstruction *getFirstInstr();
	BaseInstruction *getLastInstr();
	
	void deleteUDDU(void);
	
	void initEpilogue(OpList & a);
	
	void addEpilogueOp(BaseOperation *a);
	
	OpList & getEpilogue() { return _epilogue; }
	
	void initPrologue(OpList & a);
	
	void addPrologueOp(BaseOperation *a);
	
	OpList & getPrologue() { return _prologue; }
	
	void getSavedRegList(ArgList & alist);
	BasicBlockNode* startBlock;
	BasicBlockNode* stopBlock;
	virtual void ifConvert(BaseOpProperty* pred, BaseOperation* ifOp = NULL);
	__declspec (dllexport) void markBlocks();
	__declspec (dllexport) void percolate();

	// Thumb Pass
	void ThumbPass();

	void addProperty(BaseRoutineProperty& prop) { _propList.addProperty(prop); }

	PropertyList<BaseRoutineProperty>& getPropList() { return _propList; }

	BaseRoutineProperty& getProperty(PropertyNames name) {
		return _propList.getProperty(name);
	}
	BaseRoutineProperty* getPtrToProperty(PropertyNames name) {
		return _propList.getPtrToProperty(name);
	}
	void  printProperty(PropertyNames name) {
		BaseRoutineProperty* prop = getPtrToProperty(name);
        if ( prop )
			prop->print(cout); 
	}
	void removeProperty(BaseRoutineProperty& prop) {
		_propList.removeProperty(prop);
	}

	void recursiveRemoveProperty(PropertyNames name);

	int countInstructions() { return (_instrList.numItems()); }

	int countOperations();
};

typedef Map<MyString, Routine *> RoutineTable;
typedef RoutineTable::iterator RoutineTableIter;

#endif




