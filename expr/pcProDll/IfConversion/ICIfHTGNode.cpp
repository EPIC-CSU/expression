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


//#include "GlobalIncludes.h"

//#include "BaseArgument.h"
//#include "BaseHTGNode.h"
//#include "GlobalIncludes.h"
//#include "BasicBlockNode.h"

#include "IfHTGNode.h"
#include "OpPredicate.h"
//#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "SimpleHTGNode.h"
#include "ControlOp.h"
#include "ControlOpSlot.h"
#include "IfOp.h"

#include "GotoOp.h"
#include "JumpOp.h"
#include "ComputeOp.h"
#include "BasicBlockNode.h"

#ifdef WIN32
#include "crtdbg.h"
#endif

#include "FlagsDefs.h"
#include "BBIterator.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern void insertAsLastInstr(ComputeOp *moveOp, BasicBlockNode *bb);

const char* makePred(char* newPred, BaseOpProperty* oldPred, bool state)
{
#ifndef __UNIX__
 static std::string text;
 
 text="";
 
 if (oldPred==NULL)
 {
  if (state==false) text+="!";
  //text+="(";
  text+=newPred;
  //text+=")";
 }
 else //oldPred!=NULL
 {
  text+="(";
  text+=((OpPredicate*)oldPred)->text;
  text+=" & ";
  if (state==false) text+="!";
  //text+="(";
  text+=newPred;
  text+=")"; //text+="))";
 }
 
 return text.c_str();
#else  // for unix, std::string is not working because there is a previous
 // declaration of string in std_types.h as char *
 static string text;
 
 text = strdup("");
 
 if (oldPred==NULL)
 {
  if (state==false) 
   strcat(text, strdup("!"));
  //text+="(";
  strcat(text, newPred);
  //text+=")";
 }
 else //oldPred!=NULL
 {
  strcat(text, strdup("("));
  strcat(text, strdup(((OpPredicate*)oldPred)->text));
  strcat(text, strdup(" & "));
  if (state==false) 
   strcat(text, strdup("!"));
  //text+="(";
  strcat(text, strdup(newPred));
  strcat(text, strdup(")")); //text+="))";  
 }
 
 return text;
#endif
}

BaseInstruction* getLastInstrEx(BasicBlockNode* bb, BaseInstruction* stopInstr)
{
 BaseInstruction* instr=bb->getFirstInstr();
 BaseInstruction* retInstr=NULL;
 
 ASSERT(instr!=NULL);
 
 while (instr!=stopInstr)
 {
  retInstr=instr;
  instr = ((NormalInstr*)instr)->getDefaultTargetInstr();
 }
 
 return retInstr;
}

static BasicBlockNode* getLastBBEx(BaseHTGNode* htgNode, BasicBlockNode* stopBB=NULL)
{
 //quite a few assumptions go with this code
 
 //see if SimpleHTGNode
 SimpleHTGNode* sn=dynamic_cast<SimpleHTGNode*>(htgNode);
 if (sn!=NULL)
 {
  return sn->getLastBB();
 }
 
 IfHTGNode* in=dynamic_cast<IfHTGNode*>(htgNode);
 if (in!=NULL)
 {
  if (in->getNext()!=NULL)
  {
   return in->getNext()->getFirstBB();
  }
  
  //the assumption is that even though the node is a IFHTGNode 
  //it has been converted to straight line code by ifConvert
  //so, just walk the BB chain from firstBB till the joinBB
  //the one before the join is the last BB
  
  BasicBlockNode* bb=in->getIfHead()->getFirstBB();
  if (stopBB!=NULL)
  {
   ASSERT(0);
   BasicBlockNode* bingo=NULL;
   while (bb!=stopBB)
   {
    bingo=bb;
    bb=bb->getFirstSucc();
   }
   
   return bingo;
  }
  
  if (in->getNext()!=NULL)
  {
   BasicBlockNode* end=in->getNext()->getFirstBB();
   ASSERT(end!=NULL);
   BasicBlockNode* bingo=NULL;
   
   while (bb!=end)
   {
    bingo=bb;
    bb=bb->getFirstSucc();
   }
   
   return bingo;
  }
  else //the last BB in this chain is also the last BB of this node
  {
   while (bb->getNextSucc()!=NULL)
   {
    //bb=bb->getNextSucc();
    bb=bb->getFirstSucc();
   }
   
   return bb;
  }
 }
 
 //OOPS! other nodes not currently dealt with
 
 return NULL;
}

static BasicBlockNode* getJoinBB(BaseHTGNode* ifNode)
{
 BaseHTGNode* joinHTG=ifNode->getNext();
 while (joinHTG==NULL)
 {
  ifNode=ifNode->getUp();
  joinHTG=ifNode->getNext();
 }
 return joinHTG->getFirstBB();
}

int IfHTGNode::shouldIfConvertHeur()
{

	if(this->containsLoopNode()||this->containsCallNode())
		return _NO_;
	else
		return _YES_;
}

// This function creates true and false predicate depending on "sense"
OpPredicate *createPred(BaseOpProperty* pred, BaseOperation* ifOp, IfOp *newIfOp, bool sense)
{

	// now create the predicate with the help of old pred and the newIfOp.
	OpPredicate *newPred;

	if (pred!=NULL)
		newPred=(OpPredicate*)(pred->copy());
	else
		newPred=new OpPredicate();
	
	char str[50];
	str[0]='\0';
	newIfOp->getCC()->printToString(&str[0]);
	
	//add the new arg
	newPred->text=strdup(makePred(str, pred, sense));
	newPred->addPredicateArg(newIfOp, newIfOp->getCC(), !sense);
	
	// return the predicate thus formed
	return newPred;
}

extern int _LAST_FLOW_POS_;

// GOTO removal.
void IfHTGNode::removeBranchGOTO(BaseHTGNode* branch)
{
	// if branch is empty, then we do not need to perform GOTO removal
	if (branch!=NULL)
	{
		NormalInstr* instr=(NormalInstr*)(getLastBBEx(branch)->getLastInstr());
		
		//go thru the flowops and remove any goto's
		ArrayIterator<FlowOpSlot>* fIter = instr->getFlowOpSlotIter();
		while ( fIter->hasMoreElements() ) 
		{
			BaseOperation* oper=fIter->currentElement().getPtrToOperation();
			if (oper!=NULL)
			{
				if (oper->isGotoOp())
				{
					BaseInstruction* tgtInstr=oper->getTargetInstr();
					ASSERT(tgtInstr->hasLabel());
					
					//replace the original GOTO with a JUMP
					instr->extract(oper);     
					delete oper;
					JumpOp newOp(tgtInstr);
					instr->addFlowOpToList(&newOp,_LAST_FLOW_POS_);
				}
			}
			fIter->nextElement();
		}
		delete fIter;
	}
}


void IfHTGNode::ifConvert(BaseOpProperty* pred, 
						  OpList & IselList,
						  BaseOperation* ifOp,
						  OpList OperationsList)
{
	MyLinkedListIterator<BaseHTGNode *> *iter;
	


	// 1.	operInTrueBranch = A vector of all the operations in the true branch 
	OpList operInTrueBranch;

	iter=_trueBranch.elementsForward();
	while(iter->hasMoreElements())
	{
		(iter->currentElement())->createListOfOperations(operInTrueBranch);
		iter->nextElement();
	}
	delete iter;
	
	
	// 2.	operInFalseBranch = A vector of all the operations in the false branch 
	OpList operInFalseBranch;
	OpList &false_oplist_ref = operInFalseBranch;
	
	iter=_falseBranch.elementsForward();
	while(iter->hasMoreElements())
	{
		(iter->currentElement())->createListOfOperations(false_oplist_ref);
		iter->nextElement();
	} 
	delete iter;

	
	
	//if (ShouldIfConvert(IfHTGNode))
	if (this->shouldIfConvertHeur())
	{
		NormalInstr* instr=(NormalInstr*)(_ifHead->getLastBB())->getLastInstr();
		ArrayIterator<ControlOpSlot> *cIter = instr->getControlOpSlotIter(); 
		IfOp *newIfOp;
		
		// first find the newIfOp
		while (cIter->hasMoreElements())
		{
			if ((cIter->currentElement()).hasOper())
			{
				newIfOp=((IfOp*)(cIter->currentElement()).getPtrToOperation()); //assume there's only one control op
			}  
			cIter->nextElement();
		}
		delete cIter;
		assert(newIfOp!=NULL);
		
		// Now create the predicates
		OpPredicate* predTrue;
		OpPredicate* predFalse;
		
		// 1.	truePred = MakeTruePredicate (IfHeadNode, predicate);
		predTrue = createPred(pred, ifOp, newIfOp, true);
		
		// 2.	falsePred = MakeFalsePredicate (IfHeadNode, predicate);		
		predFalse = createPred(pred, ifOp, newIfOp, false);




		// 3.	Remove the if Operation
			


		// 4.	ifConvert the true branch
		OpList trueIselList;
		iter=_trueBranch.elementsForward();
		while(iter->hasMoreElements())
		{
			iter->currentElement()->ifConvert(predTrue, trueIselList, newIfOp, operInTrueBranch);
			iter->nextElement();
		}
		delete iter;

		
		/*
		// 5.	IfHTGNode::AppendIsels (trueIselList, _trueBranch);
		OpListIter oIter;
		for (oIter = trueIselList.begin(); oIter != trueIselList.end(); oIter++)
			insertAsLastInstr((ComputeOp *)(*oIter), createOrGetLastTrueNode()->getLastBB());
		*/


		// 5	ifConvert the false branch
		OpList falseIselList;
		iter=_falseBranch.elementsForward();
		while(iter->hasMoreElements())
		{
			iter->currentElement()->ifConvert(predFalse, falseIselList, newIfOp, operInFalseBranch);
			iter->nextElement();
		}
		delete iter;


		/*
		Initially we appended the trueIsel list at the end of the true node. 
		But that is unoptimal. Now since we append the true and then the false branch below it, 
		we have appended both the Isel lists after the false branch
		*/

		// 6	IfHTGNode::AppendIsels (trueIselList, _trueBranch);
		OpListIter oIter;
		for (oIter = trueIselList.begin(); oIter != trueIselList.end(); oIter++)
			insertAsLastInstr((ComputeOp *)(*oIter), createOrGetLastFalseNode()->getLastBB());
	
		// 7.	IfHTGNode::AppendIsels (falseIselList, falsePred, false);
		for (oIter = falseIselList.begin(); oIter != falseIselList.end(); oIter++)
			insertAsLastInstr((ComputeOp *)(*oIter), createOrGetLastFalseNode()->getLastBB());


		// Modify the Ilist

		// remove the if instruction
		instr->extract(newIfOp);
		instr->setHeaderOp(FLOW_OP,_LAST_FLOW_POS_);
		delete newIfOp;
		
		
		MyLinkedListIterator<BaseHTGNode *>* ix;

		BaseHTGNode *lastTrueNode = NULL;

		// remove the true branch GOTO
		ix = _trueBranch.elementsForward();
		if (ix->hasMoreElements())
		{
			lastTrueNode = ix->currentElement();
		}
		removeBranchGOTO(lastTrueNode );

		
		// remove the false branch GOTO
		BaseHTGNode *lastFalseNode = NULL;
		ix = _falseBranch.elementsForward();
		if (ix->hasMoreElements())
		{
			lastFalseNode = ix->currentElement();
		}
		removeBranchGOTO(lastFalseNode);


		// Now patch the instructions correctly
		if ((lastTrueNode != NULL) && (lastFalseNode != NULL))
		{
			// insert code for patching true and false nodes with header and join.
			BasicBlockEdge firstEdge(NULL,_FORWARD_ARC_);
			BasicBlockEdge nextEdge(NULL, _FORWARD_ARC_);
			
			BasicBlockNode* joinBB=getJoinBB(this);
			//NormalInstr* instr=(NormalInstr*)(getLastBBEx(_ifHead)->getLastInstr());
			
			NormalInstr* lastHeadInstr=(NormalInstr*)(getLastBBEx(_ifHead)->getLastInstr());
			NormalInstr* firstTrueInstr=(NormalInstr*)(lastTrueNode->getFirstBB()->getFirstInstr());
			NormalInstr* firstFalseInstr=(NormalInstr*)(lastFalseNode->getFirstBB()->getFirstInstr());
			NormalInstr* lastTrueInstr=(NormalInstr*)(getLastInstrEx(lastTrueNode->getFirstBB(),joinBB->getFirstInstr()));
			NormalInstr* lastFalseInstr=(NormalInstr*)(getLastInstrEx(lastFalseNode->getFirstBB(),joinBB->getFirstInstr()));
			
			cout << "Last Head Instr : ";
			lastHeadInstr->shortPrint(cout);
			cout << endl;

			cout << "Last True Instr : ";
			lastTrueInstr->shortPrint(cout);
			cout << endl;

			cout << "Last False Instr : ";
			lastFalseInstr->shortPrint(cout);
			cout << endl;
			
			//NormalInstr* lastTrueInstr=(NormalInstr*)(getLastBBEx(lastTrueNode,joinBB)->getLastInstr());
			
			//point head to true branch only
			//CFG
			firstEdge.setNext(lastTrueNode->getFirstBB());
			getLastBBEx(_ifHead)->setFirstEdge(firstEdge);
			getLastBBEx(_ifHead)->setNextEdge(nextEdge);
			//ILG
			//first remove the first flow op
			ArrayIterator<FlowOpSlot>* fIter = lastHeadInstr->getFlowOpSlotIter();
			ASSERT(fIter->hasMoreElements());
			BaseOperation* oper=fIter->currentElement().getPtrToOperation();
			BaseInstruction* trueInstr=oper->getTargetInstr();
			ASSERT(trueInstr==firstTrueInstr);
			lastHeadInstr->extract(oper); 
			delete oper;  
			//next replace other flow op with a JUMP to true branch
			fIter->nextElement();
			ASSERT(fIter->hasMoreElements());
			oper=fIter->currentElement().getPtrToOperation();
			BaseInstruction* falseInstr=oper->getTargetInstr();
			while (falseInstr==NULL) //skip empty slots
			{
				fIter->nextElement();
				oper=fIter->currentElement().getPtrToOperation();
				falseInstr=oper->getTargetInstr();
			}
			ASSERT(falseInstr==firstFalseInstr);
			lastHeadInstr->extract(oper);     
			delete oper;
			JumpOp newOp1(firstTrueInstr);
			lastHeadInstr->addFlowOpToList(&newOp1,_LAST_FLOW_POS_);
			delete fIter;
			
			//now point true to false
			//CFG
			firstEdge.setNext(lastFalseNode->getFirstBB());
			getLastBBEx(lastTrueNode)->setFirstEdge(firstEdge);
			//getLastBBEx(_ifHead)->setNextEdge(nextEdge);
			//ILG
			fIter = lastTrueInstr->getFlowOpSlotIter();
			ASSERT(fIter->hasMoreElements());
			//skip true flow op
			fIter->nextElement();
			ASSERT(fIter->hasMoreElements());
			oper=fIter->currentElement().getPtrToOperation();
			lastTrueInstr->extract(oper);     
			delete oper;
			JumpOp newOp2(falseInstr);
			lastTrueInstr->addFlowOpToList(&newOp2,_LAST_FLOW_POS_);
			fIter->nextElement();
			ASSERT(!fIter->hasMoreElements());
			delete fIter;
			
			//false already points to join

			firstFalseInstr=(NormalInstr*)(lastFalseNode->getFirstBB()->getFirstInstr());
			lastTrueInstr=(NormalInstr*)(getLastInstrEx(lastTrueNode->getFirstBB(),joinBB->getFirstInstr()));
			lastFalseInstr=(NormalInstr*)(getLastInstrEx(lastFalseNode->getFirstBB(),joinBB->getFirstInstr()));
			
			cout << "Last True Instr : ";
			lastTrueInstr->shortPrint(cout);
			cout << endl;

			cout << "Last False Instr : ";
			lastFalseInstr->shortPrint(cout);
			cout << endl;

		}
		else if (lastFalseNode == NULL)
		{
			assert(0);
		}
		else if (lastTrueNode == NULL)
		{
			BasicBlockNode* joinBB=getJoinBB(this);
			NormalInstr* lastFalseInstr=(NormalInstr*)(getLastInstrEx(lastFalseNode->getFirstBB(),joinBB->getFirstInstr()));
			
			cout << "Last False Instr : ";
			lastFalseInstr->shortPrint(cout);
			cout << endl;

			// insert code for patching false nodes with header and join.
			instr=(NormalInstr*)(getLastBBEx(_ifHead)->getLastInstr());

			//remove the flow op pointing from head to join
			ArrayIterator<FlowOpSlot>* fIter = instr->getFlowOpSlotIter();
			ASSERT(fIter->hasMoreElements());
			BaseOperation* oper=fIter->currentElement().getPtrToOperation();
			instr->extract(oper);     
			
			//   instr->print();
			
			delete oper;
			delete fIter;
		}
	}
	else
	{
		// 1. IfConvert the true branch
		OpList trueIselList;
		iter=_trueBranch.elementsForward();
		while(iter->hasMoreElements())
		{
			iter->currentElement()->ifConvert(pred, trueIselList, ifOp, OperationsList);
			iter->nextElement();
		}
		delete iter;
		


		// 2.	IfHTGNode::AppendIsels (trueIselList, _trueBranch);
		OpListIter oIter;
		for (oIter = trueIselList.begin(); oIter != trueIselList.end(); oIter++)
			insertAsLastInstr((ComputeOp *)(*oIter), createOrGetLastTrueNode()->getLastBB());



		// 3. IfConvert the false branch
		OpList falseIselList;
		iter=_falseBranch.elementsForward();
		while(iter->hasMoreElements())
		{
			iter->currentElement()->ifConvert(pred ,falseIselList, ifOp, OperationsList);
			iter->nextElement();
		}
		delete iter;


		// 4.	IfHTGNode::AppendIsels (falseIselList, predicate, false);
		for (oIter = falseIselList.begin(); oIter != falseIselList.end(); oIter++)
			insertAsLastInstr((ComputeOp *)(*oIter), createOrGetLastTrueNode()->getLastBB());

	}
}
	

void IfHTGNode::AppendIsels(OpList IselList, BaseOpProperty *predicate, bool sense)
{
	MyLinkedListIterator<BaseHTGNode *> *iter;

	if (sense)
		iter=_trueBranch.elementsForward();
	else
		iter=_trueBranch.elementsForward();

		

	delete iter;
	

}

void IfHTGNode::createListOfOperations(OpList & oplist)
{
	MyLinkedListIterator<BaseHTGNode *> *iter;
	iter=_trueBranch.elementsForward();
	while(iter->hasMoreElements())
	{
		(iter->currentElement())->createListOfOperations(oplist);
		iter->nextElement();
	}
	delete iter;
	iter=_falseBranch.elementsForward();
	while(iter->hasMoreElements())
	{
		(iter->currentElement())->createListOfOperations(oplist);
		iter->nextElement();
	}
	delete iter;
}



