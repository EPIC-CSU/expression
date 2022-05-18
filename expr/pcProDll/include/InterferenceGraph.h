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
// $Id: InterferenceGraph.h,v 1.6 1998/03/21 01:18:26 radu Exp $
//
// File:  		InterferenceGraph.h
// Created:		Mon Oct 18, 99
// Last modified: 	Mon Oct 18, 99
// Author:		Radu Cornea
// Email:		radu@ics.uci.edu, ilp@ics.uci.edu
//
// This class implements the register interference graph
// 

#ifndef __INTERFERENCEGRAPH_H__
#define __INTERFERENCEGRAPH_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "IGraph.h"
#include "STLIncludes.h"

class IGNode;
class MultiChain;
class MCNode;
class DoubleMCNode;
class MetaMCNode;
class RegNode;
class BaseArgument;
class BasicBlockNode;
class Routine;
class NormalInstr;
class BaseOperation;
class BaseInstruction;

template <class T>
class SetList;

class InterferenceGraph
{
	IGraph _iGraph;

	MyLinkedList<RegNode *> _regNodes;
	MyLinkedList<MCNode *> _mcNodes;

	typedef map<int, MultiChain *> SSANumToMCMap;

	map<int, RegNode *> _colorToRegMap;
	map<MultiChain *, MCNode *> _mcToNodeMap;
	map<int, SSANumToMCMap> _idToMCMap;	// maps [index, ssaNum] to MC
	map<int, RegNode *> _indexToRegNodeMap;

	int _noOfColors;

public:
	InterferenceGraph()	: _iGraph() { _noOfColors = 0; };

	~InterferenceGraph();

	void addNode(IGNode *node);
	void addInterference(IGNode *n1, IGNode *n2);

	void addMultiChain(MultiChain *mChain);
	void updateMCToNodeMap(MCNode *mcNode);
	void addMCNode(MCNode *mcNode);
	void addReg(RegNode *regNode); 
	void addRegNodes();

	DoubleMCNode * makeDoubleMCNode(MCNode *firstMCNode, MCNode *secondMCNode);
	DoubleMCNode * makeDoubleMCNode(MultiChain *firstMC, MultiChain *secondMC);

	MetaMCNode * makeMetaMCNode(MCNode *firstMCNode, MCNode *secondMCNode);
	MetaMCNode * makeMetaMCNode(MultiChain *firstMC, MultiChain *secondMC);

	MultiChain * coalesceMCs(MultiChain *defMC, MultiChain *useMC, BaseOperation *op, NormalInstr *instr, bool keepUDDU = false);

	int existInterference(MCNode *firstMCNode, MCNode *secondMCNode);
	int existInterference(MultiChain *firstMC, MultiChain *secondMC);

	void addInterference(MultiChain *mc1, MultiChain *mc2);
	void addMCArchInterferences(MultiChain *mChain);
	void addMCArchInterferences(MCNode *mcNode);
	void addGenericToTargetCompatibility(int genericRegIndex, int targetRegIndex, int hardConstraint);
	void addGToTRegMapInterferences(Routine *routine);
	void addMCToTargetCompatibility(MultiChain *mc, int targetRegIndex);
	void addConstrainedRegInterferences(Routine *routine);

	void removeNode(IGNode *node);
	void removeInterference(IGNode *n1, IGNode *n2);

	void removeMultiChain(MultiChain *mChain);
	void removeMCNode(MCNode *node);
	//void removeInterference(MultiChain *mc1, MultiChain *mc2);

	void removeDoubleNode(DoubleMCNode *node);

	void addDefInterference(BaseArgument *defArg, SetList<int>& liveList);
	void addDefInterferenceInsideInstr(BaseArgument *defArg, SetList<BaseArgument *> *useList, NormalInstr *instr);

	MyLinkedList<IGNode *> * getNodeList();

	MyLinkedList<MultiChain *> * getMCList();

	void setMCNode(MultiChain *mc, MCNode *mcNode);

	MCNode * getMCNode(MultiChain *mc);

	MultiChain * getMC(int ID);

	MultiChain * getMC(int symIndex, int ssaNum);

	RegNode * getRegNode(int index);

	void setMC(int ID, MultiChain *mc);

	void setMC(int symIndex, int ssaNum, MultiChain *mc);

	void setNoOfColors(int colNo) { _noOfColors = colNo; }

	int getNoOfColors() { return _noOfColors; }

	RegNode * getRegOfColor(int color);

	// just deletes edges between multichains and _colorToRegMap
	void reset();

	void resetColors();
	
	void build(Routine *routine);

	void cleanupDoubleCrossInterferences();

	void addMappingInterferences();

	void listNeighbours();

	//int InterferenceGraph::nestingDepth(NormalInstr *instr);

	IGNode * InterferenceGraph::bestNodeToSpill();

	int colorMCNode(MCNode *mcNode);

	int colorDoubleMCNode(DoubleMCNode *doubleMCNode);

	void color();

	void listColors();

	void printIDsAndNames();

	void print(ostream& out);

	void print() { print(cout); }

	void printNodes(ostream& out);

	void printMCNodes(ostream& out);

	void printEdges(ostream& out);

	void printMCEdges(ostream& out);

	void printMCNodeCompatEdges(ostream& out, MCNode *mcNode);

	void printMCCompatEdges(ostream& out);

    void printStatistics(ostream& out);
	
	friend ostream& operator<<(ostream& out, InterferenceGraph& ig)
	{
		ig.print(out);
		return out;
	}

	void printGML(ostream& out);

	friend void regCoalesceOper(BaseOperation * oper, InterferenceGraph& ig);
	friend void regCoalesceInstr(BaseInstruction * instr, InterferenceGraph& ig);
	friend void registerCoalescing(InterferenceGraph& ig);

};

#endif

