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
 *file  : AsmBB.cpp											 *
 *created : 3 July, 2000.                                    *
 *authors : Partha Biswas									 *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#include <stdafx.h>
#include <iostream.h>
#include <ctype.h>
#include <fstream.h>
#include <string.h>
#include <stdio.h>
#ifndef __UNIX__
#include <crtdbg.h>
#else
#define _ASSERT assert
#endif

#include "AsmBB.h"

#include "STLIncludes.h"

#include "FilePointer.h"
#include "Routine.h"

#include "IfOp.h"
#include "GotoOp.h"
#include "LabelArg.h"

#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector

#include <map>

#ifdef map
#undef map
#endif 
#define map std::map

#define JUMP_OPCODE "j"


typedef vector <AsmBB> asmBBVectorType;
typedef vector <BasicBlockNode *> BBVectorType;

asmBBVectorType globalASMBBList;

BBVectorType BBVector;
BBVectorType trueBBVector;

BasicBlockNode * fallThruNode(BasicBlockNode *);
BasicBlockNode * trueNode(BasicBlockNode *);
void processForNodeNotInBBVector(BasicBlockNode *, BasicBlockNode *);
void processForNodeInBBVector(BasicBlockNode *, BasicBlockNode *);
int isInBBVector(BasicBlockNode *);
BasicBlockNode *getNodeFromTrueBBVector();
void insertNodeInTrueBBVector(BasicBlockNode *);
void associateGotoWithCurrNode(BasicBlockNode *, LabelArg *);
void printLabel(LabelArg *, ostream &);
void printGoto(LabelArg *, ostream &);
LabelArg *createLabel(BasicBlockNode *);
void associateLabelWithSuccNode(BasicBlockNode *, LabelArg *);

void setCorrectLabels(BasicBlockNode *currNode, BasicBlockNode *succNode);
int sameLabels(LabelArg *label1, LabelArg *label2);
int hasLabel(BasicBlockNode *node, LabelArg *label = NULL);
LabelArg *getLabel(BasicBlockNode *node);
AsmBB *getListEntry(BasicBlockNode *node);
int isDummyBB(BasicBlockNode *bb);

int labelCount=0;
extern bool PrintingASM; // from NoGUIMainSupportFuncs
extern bool DumpingIR;

extern void printIndent(FilePointer & fp, int indent);

void clearASMDS()
{
	globalASMBBList.clear();
	BBVector.clear();
	trueBBVector.clear();
}

/*
// A new way to serialize CFG or insert GOTO
void serializeCFGPhase1(Routine *rout)
{
	globalASMBBList.clear();

	// First, keep a list of all the basic-blocks.
	// This list is used to keep track of changes to the basic-blocks (like adding labels, gotos).
	//
	LinkedListIterator<BasicBlockNode> *iter = (rout->getCFG()).createBBIterForw();
	AsmBB atb;
	atb.bb = NULL; atb.bbLabel = NULL; atb.gotoLabel = NULL; atb.newLabel = 0;
	while (iter->hasMoreElements())
	{
		BasicBlockNode *currNode = &(iter->currentElement());
		BasicBlockNode *succNode = currNode->getFirstSucc();
		BasicBlockNode *nextNode = &(iter->nextElement());
		if( succNode->getDFSNum() == nextNode->getDFSNum() )
		{
			// The first successor of bb is the same as the next bb in printing order
			// so no goto is needed
		}
		else if( !isDummyBB(currNode) && !isDummyBB(succNode) )
		{
			// A conditional/unconditional branch is needed from bb to firstSucc
			// Branch from bb to firstSucc is a true branch
			LabelArg *label;
			
			if (currNode->hasIf())
			{
				setCorrectLabels(currNode, succNode);
				return;
			}
			if(currNode->hasGoto())
			{
				// Check if goto points to the label of the succNode
				label = (LabelArg*)((GotoOp *)currNode->hasGoto())->ptrToLabel();
				if(!hasLabel(succNode, label)){
					assert(0); // goto should point to the correct label already
					label = getLabel(succNode);
					associateGotoWithCurrNode(currNode, label);
				}
				return;
			}
			if (hasLabel(succNode)) // currNode does not have a goto
			{
				// create goto op with label and insert in the op of currNode
				//
				associateGotoWithCurrNode(currNode, getLabel(succNode));
			}
			else // succNode does not have a label
			{
				label = getLabel(succNode);  // ??
				associateLabelWithSuccNode(succNode, label);
				associateGotoWithCurrNode(currNode, label);
			}
		}
		atb.bb = currNode;
		globalASMBBList.push_back(atb);
		// iter->nextElement();
	}
	delete iter;
}

*/

void serializeCFGPhase1(Routine *rout)
{
	clearASMDS();

	// First, keep a list of all the basic-blocks.
	// This list is used to keep track of changes to the basic-blocks (like adding labels, gotos).
	//
	LinkedListIterator<BasicBlockNode> *iter = (rout->getCFG()).createBBIterForw();
	AsmBB atb;
	atb.bb = NULL; atb.bbLabel = NULL; atb.gotoLabel = NULL; atb.newLabel = 0;
	while (iter->hasMoreElements())
	{
		atb.bb = &(iter->currentElement());
		globalASMBBList.push_back(atb);
		iter->nextElement();
	}
	delete iter;

	BasicBlockNode *currNode = (rout->getCFG()).getStartNode();

	BasicBlockNode *temp1, *temp2;
	LabelArg *bbLabel = NULL;
	LabelArg *gotoLabel = NULL;
	BBVector.clear(); // clearing needed for every routine
	trueBBVector.clear();

	// iter = (rout->getCFG()).createBBIterForw();
    while( currNode != NULL )
		// while (iter->hasMoreElements())
	{
		// currNode = &(iter->currentElement());
		if(!isDummyBB(currNode))
		{
			if(!isInBBVector(currNode) )
			{
				BBVector.push_back(currNode);  // If the basic block is not already in the BBVector
			}
			else
			{
				// curr node is in the BB vector, it must already have been processed;
				//
				// All we need to do is to get a node from the TrueBB Vector
				// and process the new node.
				//
				currNode = getNodeFromTrueBBVector();			
				continue;
			}
		}

		temp1 = currNode->getFallThruNode();

		if( temp1 != NULL )
		{
			if( isInBBVector(temp1) )
			{
				processForNodeInBBVector(currNode, temp1);
				temp1 = NULL;
			}
		}

		temp2 = currNode->getTrueNode();

		if( temp2 != NULL )
		{
			if( isInBBVector(temp2) )
			{
				processForNodeInBBVector(currNode, temp2);
				temp2 = NULL;
			}
			else if( temp1 != NULL )
			{
				processForNodeNotInBBVector(currNode, temp2);
			}
		}
		if( temp1 != NULL )
		{
			currNode = temp1;
		}
		else if( temp2 != NULL )
		{
			currNode = temp2;
		}
		else
		{
			currNode = getNodeFromTrueBBVector();
		}
		// iter->nextElement();
	}
}

int isInBBVector(BasicBlockNode *node)
{
	BBVectorType::iterator i;
	for(i = BBVector.begin(); i != BBVector.end(); i++)
	{
		if((*i) == node) return(1);
	}
	return(0);
}

BasicBlockNode *getNodeFromTrueBBVector()
{
	BasicBlockNode *retNode;
	if(trueBBVector.empty())
		//assert(0);
		return NULL;
	retNode = trueBBVector[0];
	trueBBVector.erase(&trueBBVector[0]);
	return retNode;
}

void processForNodeInBBVector(BasicBlockNode *currNode, BasicBlockNode *succNode)
{
	LabelArg *label;

	if (currNode->hasIf())
	{
		setCorrectLabels(currNode, succNode);
		return;
	}
	/*
	if(currNode->hasGoto())
	{
		// Check if goto points to the label of the succNode
		label = (LabelArg*)((GotoOp *)currNode->hasGoto())->ptrToLabel();
		if(!hasLabel(succNode, label)){
				assert(0); // goto should point to the correct label already
				label = getLabel(succNode);
				associateGotoWithCurrNode(currNode, label);
		}
		return;
	}
	*/
	if (hasLabel(succNode)) // currNode does not have a goto
	{
		// create goto op with label and insert in the op of currNode
		//
		associateGotoWithCurrNode(currNode, getLabel(succNode));
	}
	else // succNode does not have a label
	{
		label = getLabel(succNode);  // ??
		associateLabelWithSuccNode(succNode, label);
		associateGotoWithCurrNode(currNode, label);
	}
}

int isDummyBB(BasicBlockNode *bb)
{
	if (bb == NULL) return _YES_;

	if ((bb->getFirstInstr() == NULL) && (bb->getLastInstr() == NULL)) 
		return _YES_;

	return _NO_;
}

void processForNodeNotInBBVector(BasicBlockNode *currNode, BasicBlockNode *succNode)
{
	LabelArg *label;
	if(!currNode->hasIf())
	{
		if(succNode != currNode->getTrueNode())
			assert(0);
		if(!succNode->getLabel())
			assert(0);
	}
	else
	{
		if(!hasLabel(succNode))   // should never be satisfied
		{
			//Note: SAH, 01/24/01
			//
			// This situation is now possible. This is due to SSA which might create dummy bbs
			// along the true branch without also creating the labels.
			//
			// However, if the bb is an empty dummy bb, then just ignore it.
			//
			if (isDummyBB(succNode))
			{
				insertNodeInTrueBBVector(succNode);
				return;
			}
			// create label and insert succNode into trueBBVector
			label = getLabel(succNode); // ??
			associateLabelWithSuccNode(succNode, label);
			setCorrectLabels(currNode, succNode);
//			assert(0);
		}
		/*
		if(!currNode->hasGoto()) // another impossible scenario
		{
			// create goto op with label and insert in the op of currNode
			label = (LabelArg *)succNode->getLabel();
			associateGotoWithCurrNode(currNode, label);	
			assert(0);
		}
		*/
		insertNodeInTrueBBVector(succNode); // Note: We do not add succNode if it is an empty dummy bb
	}
	return;		
}

AsmBB *getListEntry(BasicBlockNode *node)
{
	asmBBVectorType::iterator i;

	for (i = globalASMBBList.begin(); i != globalASMBBList.end(); i++)
	{
		if ((*i).bb == node)
			return i;
	}
	return NULL;
}

LabelArg *createLabel(BasicBlockNode *node)
{
	char labelName[10];
	LabelArg *newLabel;
	sprintf(labelName, "NL%d", labelCount++);
	newLabel = new LabelArg(labelName);
	return newLabel;
}

LabelArg *getLabel(BasicBlockNode *node)
{
	BaseArgument *retPtr = NULL;

	if ((retPtr = node->getLabel()) != NULL)
		return ((LabelArg *)retPtr);

	AsmBB *idx = getListEntry(node);

	if ((idx != NULL) && (idx->bbLabel != NULL))
		return (idx->bbLabel);

	return createLabel(node);
}

int hasLabel(BasicBlockNode *node, LabelArg *label)
{
	LabelArg *tempPtr = NULL;

	if ((tempPtr = (LabelArg *)(node->getLabel())) == NULL)
	{
		AsmBB *idx = getListEntry(node);

		if ((idx != NULL) && (idx->bbLabel != NULL))
			tempPtr = idx->bbLabel;
	}
	if (tempPtr == NULL) return _NO_;

	if (label != NULL)
	{
		return sameLabels(label, tempPtr);
	}
	return _YES_;
}

int sameLabels(LabelArg *label1, LabelArg *label2)
{
	if (strcmp(label1->lName(), label2->lName()) == 0)
			return _YES_;
	return _NO_;
}

// This function ensures that currNode points to succNode.
// i.e. it ensures that the goto (or if) stmt in currNode has the label that corresponds to succNode
//
void setCorrectLabels(BasicBlockNode *currNode, BasicBlockNode *succNode)
{
	if ((currNode == NULL) || (succNode == NULL)) return;

	if (hasLabel(succNode) == _NO_) // error condition as of now.
	{
		cout << "Panic: Error while trying to set correct labels" << endl;
		assert(0);
	}

	LabelArg *succLabel = getLabel(succNode);

	GotoOp *top;
	IfOp *tif;
	if ((top = (GotoOp *)(currNode->hasGoto())) != NULL)
	{
		if (sameLabels(succLabel, (LabelArg *)(top->ptrToLabel())) == _NO_)
		{
			// as of now, error condition
			assert(0);
		}
	}
	else if ((tif = (IfOp *)(currNode->hasIf())) != NULL)
	{
		if (sameLabels(succLabel, (LabelArg *)(tif->ptrToLabel())) == _NO_)
		{
			// check if the true branch (i.e. the if-goto) needs to be modified.
			// if so, then change the label arg inside the if.
			// 
			// if the false branch (i.e. fall-thru) does not point to the correct label
			// then, we need to insert a goto-op right after the if-stmt.
			//
			if (succNode == currNode->getTrueNode()) // we need to change the label inside if.
			{
				tif->addLabelArg(succLabel);
			}
			else //if (succNode == currNode->getFallThruNode())
			{
				associateGotoWithCurrNode(currNode, succLabel);
			}

		}
	}
}

void associateGotoWithCurrNode(BasicBlockNode *currNode, LabelArg *label)
{
	asmBBVectorType::iterator i;
	for(i = globalASMBBList.begin(); i != globalASMBBList.end(); i++)
	{
		if((*i).bb == currNode)
		{
			if ((*i).gotoLabel != NULL) // error condition.
			{
				cout << "Panic: Tried to associate a goto label with a node that already had one" << endl;
				assert(0);
			}
			(*i).gotoLabel = label;
			return;
		}
	}
	assert(0);  // node doesn't exist in the globalASMBBlist list : impossible
	return;
}

void associateLabelWithSuccNode(BasicBlockNode *succNode, LabelArg *label)
{
	asmBBVectorType::iterator i;
	for(i = globalASMBBList.begin(); i != globalASMBBList.end(); i++)
	{
		if((*i).bb == succNode)
		{
			if ((*i).bbLabel != NULL) // error condition.
			{
				cout << "Panic: Tried to associate a bb label with a node that already had one" << endl;
				assert(0);
			}
			(*i).bbLabel = label;
			(*i).newLabel = 1;  // is a new label
			return;
		}
	}
	assert(0);  // node doesn't exist in the globalASMBBlist list : impossible
	return;
}

void insertNodeInTrueBBVector(BasicBlockNode *node)
{
	BBVectorType::iterator i;
	for(i = trueBBVector.begin(); i != trueBBVector.end(); i++)
	{
		if((*i) == node) return;
	}
	trueBBVector.push_back(node);
	return;
}

void asmGenPhase2(ostream &out) // print the assembly by traversing BBVector
{
	BBVectorType::iterator i;
	BasicBlockNode *thisBasicBlock;
	AsmBB *idx;
	for(i = BBVector.begin(); i!= BBVector.end(); i++)
	{
		//out << "# basic block pointer -> " << (*i).bb;
		//out << "# -----------------" << endl;
		//out << "# basic block begin" << endl;
		//out << "# -----------------" << endl;
		// now for every basic block, print all the instructions followed by a goto if reqd
		thisBasicBlock = (*i);

		idx = getListEntry(thisBasicBlock);

		if((*idx).newLabel)  // if the basic block has a new label
		{
			printLabel((*idx).bbLabel, out);
		}
		thisBasicBlock->printASMByCFG(out);  // print the basic block
		// now print the goto if needed
		if((*idx).gotoLabel) // there is a goto op
		{
			printGoto((*idx).gotoLabel, out);  // ??
		}
		//out << "# -----------------" << endl;
		//out << "# basic block end" << endl;
		//out << "# -----------------" << endl;
	}
}

void irToCLabel(LabelArg *arg, FilePointer & fp)
{
	fprintf(fp, "%s :\n", arg->Name());
}

void irToCGoto(LabelArg *arg, FilePointer & fp)
{
	fprintf(fp, "/* GOTO %s */\n", arg->Name());
	fprintf(fp, " goto %s;\n", arg->Name());
}

void irToCPhase2(FilePointer & fp, int indent)
{
	BBVectorType::iterator i;
	BasicBlockNode *thisBasicBlock;
	AsmBB *idx;

	for(i = BBVector.begin(); i!= BBVector.end(); i++)
	{
		// now for every basic block, print all the instructions followed by a goto if reqd
		thisBasicBlock = (*i);
		idx = getListEntry(thisBasicBlock);
		if((*idx).newLabel)  // if the basic block has a new label
		{
			printIndent(fp, indent);
			irToCLabel((*idx).bbLabel, fp);
		}
		thisBasicBlock->irToC(fp, indent);  // print the basic block
		// now print the goto if needed
		if((*idx).gotoLabel) // there is a goto op
		{
			irToCGoto((*idx).gotoLabel, fp);  // ??
		}
	}
}

void printLabel(LabelArg *label, ostream &out)
{
	out <<label->Name() << ":" << endl;
}

#include "OpCodeTable.h"
#include "GotoOp.h"

extern int getGotoOpIndex();
extern OpCodeTable globalOpCodeTable;
extern int doneISel;

TableIndex getGotoOpcodeIndex(void)
{
	if (!doneISel) // return generic goto.
	{
		return globalOpCodeTable.getIndex("GOTO");
	}
	else
		return getGotoOpIndex();
}

void printGoto(LabelArg *label, ostream &out)
{
	// first create a goto op, with the label inserted.
	// Also, get the correct opcode.
	// 
	GotoOp gop;

	TableIndex opc = getGotoOpcodeIndex();
	OpCode someOp(opc);

	gop.addLabelArg((BaseArgument *)label);
	gop.initOpCode(someOp);

	// both DumpingIR and PrintingASM use the same method to print the operation.
	//
	gop.printASMInstr(out);
}