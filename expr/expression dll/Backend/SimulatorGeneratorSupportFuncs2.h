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
 *file  : SimulatorGeneratorSupportFuncs.h                   *
 *authors : Asheesh Khare                                    *
 *created : Jan, 18, 1999                                    *
 *last modified :											 *
 *															 *
 *modified by : Sudeep Pasricha                              *                             *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _SIMULATOR_GENERATOR_SUPPORT_FUNCS_H_
#define _SIMULATOR_GENERATOR_SUPPORT_FUNCS_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <string.h>
#include <map>
//#include "ArchSTLIncludes.h"
#include <iostream.h>
#include "MyString.h"
#include "node.h"
#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector

#ifdef map
#undef map
#endif 
#define map std::map

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifdef __UNIX__
	#include <stdio.h>
	#include <ctype.h>
#endif

typedef map<NodePtr, NodePtr>::iterator ComponentMapIterator;
typedef vector<int> intVector;
typedef vector<intVector> intMatrix;


/* class LowerTriangIntMatrix defines a lower triangular matrix of integers
   It uses vector<vector> , hence is expandable in both x and y dimensions */

class LowerTriangIntMatrix
{
	intMatrix _matrix;
public:
	LowerTriangIntMatrix(){}
	LowerTriangIntMatrix(const LowerTriangIntMatrix &){}
	~LowerTriangIntMatrix(){}

	void Element(int val, int xpos, int ypos);
	int Element(int xpos, int ypos);
	void printRow(int num);
	void print(void);
};

// We want an adjacency matrix representation of the netlist for the processor.
// We store the actual Nodeptrs of the components in a map (class ComponentIndex)
// which associates unique indices with each entry. These indices are used 
// to index the LowerTriangular matrix that specifies connectivity betwen various components

class ComponentGraph
{
	map<MyString, int> _indexMap;   // stores mapping Component - index
	vector<NodePtr> _elementArray; // stores reverse mapping: index - Component
	int _lastIndex;

	LowerTriangIntMatrix _adjMatrix;  // location[i,j] = 1 if i and j are adjacent in the graph
	
	int getIndex(NodePtr);
	NodePtr getElement(int index){return _elementArray[index];}

public:
	ComponentGraph(){_lastIndex = 0;}
	ComponentGraph(const ComponentGraph &){}
	~ComponentGraph(){}

	void Connect(NodePtr,NodePtr);
	int Connected(NodePtr,NodePtr);
	void printGraph(void);
};

extern map<NodePtr, NodePtr> ComponentMap;
extern NodeList Componentlist,Unitlist, Latchlist, Storagelist, Portlist, Connectionlist, Dtpathlist;
extern SymbolList Symbollist;
extern NodePtr ArchRootNode, PipelineRootNode, InstrSectionRootNode, StorageRootNode, OpMapRootNode;
extern ComponentGraph ConnectivityMatrix;

extern int getArchRootNode(NodePtr p);
extern int getOpMapRootNode(NodePtr p);
extern int  CreateComponentList(NodePtr, NodePtr);
extern void getSubType(NodePtr node, MyString & retstr);
extern void getComponentType(NodePtr node, MyString & retstr);
extern void getConstructorString(NodePtr node, MyString & retstr);
extern void getOpCodeList(NodePtr node, NodeList &);
extern void getLatchList(NodePtr node, NodeList &, NodeKindEnum);
extern void getElementList(NodePtr node, NodeList &, NodeKindEnum);
extern int instantiateUnits(NodeList complist);
extern int instantiateLatches(NodeList complist);
extern int instantiateStorage(NodeList complist);
extern int generateMemorySubsystem(NodePtr rootNode);
extern int instantiatePorts(NodeList complist);
extern int instantiateConnections(NodeList complist);
extern int addLatchesToUnits(NodeList & unitlist);
extern int addSubCompsToUnits(NodeList & unitlist);
extern int addOpCodesToUnits(NodeList & unitlist);
extern int addPortsToUnits(NodeList & unitlist);
extern int addConnectionsToUnits(NodeList & unitlist);
extern int addSubCompsToLatches(NodeList & latchlist);
extern int getPipelineRootNode(NodePtr p);
extern int CollectDTPaths(NodePtr parent, NodePtr child);
extern void getDTType(NodePtr p, MyString & retstr);
extern NodeList getDTComps(NodePtr dtpath);
extern NodePtr getTo(NodePtr dtpath);
extern NodePtr getFrom(NodePtr dtpath);
extern void addDTPaths(NodeList & dtpathlist, NodeList & complist);
extern void ProcessDTPath(NodeList & compList, NodePtr start, NodePtr end, NodeList & actualcomplist);
extern NodePtr getHead(NodeList & compList);
extern void deleteHead(NodeList & compList);
extern int getSymbolList(SymbolPtr sym);
extern int IsAdjacentToUnit(NodePtr p);
extern int getInstrSectionRootNode(NodePtr p);
extern int getStorageRootNode(NodePtr p);
extern void createInstructionTemplate(void);
extern void createPipelineInfo(void);
extern void printInitialDecls(NodePtr p);
extern void printEndOfFunction(NodeList & unitList);
extern void addDataTransStatements(NodeList & unitlist);
extern void addBranchPredictionStatements(void);
extern void reverseList(NodeList & nodelist);
extern char * makeLowerCase(char* inpstr);
extern char * getAddString(char *inpstr);
extern void printComponentMap();
extern void createOpcodeMapping(void);
extern void addGenericOpcodesFromFile(void);
extern void addTargetOpcodesFromTreeMapping(NodeList & unitlist);
extern void printEndOfFunction2(void);
extern void printUnitStatistics(NodeList & unitList);
extern void CreateOpMap(void);
extern void addStorageToCntrl(NodePtr storageRootNode);

// For Memory Subsystem
extern char* getParameter(NodePtr storage, NodeKindEnum kind);
extern char* getAddressRange(NodePtr storage, int start);
extern int getStorageNum(char* name);

/***********
extern int getWordSize(NodePtr p);
extern int getLineSize(NodePtr p);
extern int getNumLines(NodePtr p);
extern int getAssociativity(NodePtr p);
extern char* getReplacementPolicy(NodePtr p);
extern char*  getWritePolicy(NodePtr p);
extern int  getReadLatency(NodePtr p);
extern int  getWriteLatency(NodePtr p);
extern int  getLevel(NodePtr p);
extern int  getNextLevel(NodePtr p);
extern int  getStartAddress(NodePtr p);
extern int  getEndAddress(NodePtr p);
extern int  getWidth(NodePtr p);
extern int  getNumBanks(NodePtr p);
extern int  getAccessMode(NodePtr p);
extern int  getNumParRead(NodePtr p);
extern int  getNumParWrite(NodePtr p);
extern int  getRdWrConflict(NodePtr p);
*********/
 

extern FILE * fp;

#endif
