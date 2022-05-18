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

#include "Loop.h"
#include "LoopList.h"
#include "Routine.h"
#include "MyLinkedList.h"
#include <assert.h>
#include "BasicBlockNode.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

struct ltnode
{
  bool operator()(BasicBlockNode* node1, BasicBlockNode* node2) const
  {
    assert(node1!=NULL);
    assert(node2!=NULL); 
    return (node1->getDFSNum() < node2->getDFSNum());
  }
};


typedef set<BasicBlockNode *,ltnode > nodeSet;
typedef set<int,less<int> > intSet;

/*this is a local class definition used to build loop*/
class loopNode {
public:
  int head;
  intSet _tailSet;
  intSet _nodeSet;
  loopNode* _parentLoop;
  Loop* loop;

  loopNode(){
    head = 0;
    _parentLoop = NULL;
    loop = NULL;
  }
};

typedef  multimap<int, loopNode*, less<int> > loopMap ;

/*the definition of descendantSet here is from Paper--"The Hierarchical Task
Graph as a Universal Intermediate Representation",
find T set: T(y) denote the descendants of y in the depth first search tree,
an element x is included in T(y), only if there is a non-null path made up of
tree arcs only from y to x.
(for irreducible loops, the definition of T(x) may be changed to include forward
-arc determined on what really needed to be in a loop with more than one header.
However, the algorithm for finding the loop will remain unchanged.)*/

void BuildDescendantSet(BasicBlockNode * header, nodeSet& descendantSet)
{
  nodeSet candidateSet;
  BasicBlockNode *curBB, *nextBB;

  candidateSet.insert(header);
  while (!candidateSet.empty()){
    curBB = *(candidateSet.begin());
    candidateSet.erase(candidateSet.begin());
    if (descendantSet.find(curBB)!=descendantSet.end()) {
     printf("wrong in loop recognition");
     ASSERT_TRACE(0,"Forced EXIT!");
    }
    descendantSet.insert(curBB);
    BasicBlockEdge *firstEdge = curBB->getFirstEdge();
    if (firstEdge!=NULL)
      if (!firstEdge->isBackArc()){
        nextBB = curBB->getFirstSucc();
        if (descendantSet.find(nextBB)==descendantSet.end()) 
          candidateSet.insert(nextBB);
      }
    BasicBlockEdge *nextEdge = curBB->getNextEdge();
    if (nextEdge!=NULL)
      if (!nextEdge->isBackArc()){ 
        nextBB = curBB->getNextSucc();
        if (descendantSet.find(nextBB)==descendantSet.end()) 
          candidateSet.insert(curBB->getNextSucc());
      }
  }
}

/*build a loop node, and add it to workLoopMap with the ID of loop head as key
so that it will be easy to buildSRC later*/
void BuildSingleLoop(BasicBlockNode* header, BasicBlockNode* tail,
    loopMap& workLoopMap)
{

  nodeSet descendantSet;
  BuildDescendantSet( header,  descendantSet);

  nodeSet loopNodeSet;
  nodeSet candidateSet;
  loopNodeSet.insert(header);
  candidateSet.insert(tail);

  while (!candidateSet.empty()){
    BasicBlockNode *curNode = *(candidateSet.begin());
    candidateSet.erase(candidateSet.begin());
    BasicBlockPredecessors& preds = curNode->getCFPred();
    MyLinkedListIterator<BasicBlockNode *> *preIter=
      preds.createPredListIterForw();

	if (preIter != NULL)
	{
		while (preIter->hasMoreElements()){
			BasicBlockNode *onePred = preIter->currentElement();
			if (loopNodeSet.find(onePred)!=loopNodeSet.end()){
				/*already in the loop,discard it*/
				preIter->nextElement();
				continue;
			}
			if (descendantSet.find(onePred)!=descendantSet.end()){
				/*qualify to be in the loop*/
				/*add it to the loop*/
				loopNodeSet.insert(onePred);
				
				/* SAH : Jan 05, 99 */
				/* Bug fix:
				When the basic block node is qualified to be in the loop,
				it too should be added to the candidate set.
				*/
				candidateSet.insert(onePred);
				/* End bug fix */
				
				/*add its predecessor to the candidateSet*/
				BasicBlockPredecessors& newPreds = onePred->getCFPred();
				MyLinkedListIterator<BasicBlockNode *> *newPreIter=
					newPreds.createPredListIterForw();
				
				while(newPreIter->hasMoreElements()) {
					candidateSet.insert(newPreIter->currentElement());
					newPreIter->nextElement();
				}
				delete newPreIter;
			}
			preIter->nextElement();
		}
		delete preIter;
	}
  }

  int headID = header->getDFSNum();
  int tailID = tail->getDFSNum();
  loopNode* lpNode = new loopNode();
  lpNode->head = headID;
  (lpNode->_tailSet).insert(tailID);

  for (nodeSet::iterator i=loopNodeSet.begin(); i!=loopNodeSet.end(); i++) 
    (lpNode->_nodeSet).insert((*i)->getDFSNum());

  workLoopMap.insert(pair<const int,loopNode*> (headID,lpNode));
}

void initLoopBodies(Routine& routine,loopMap& workLoopMap)
{
	if (routine.isEmptyBodyRoutine()) return;

  ControlFlowGraph& cfg = routine.getCFG();
 
/*finding all backedges, build all loop nodes */
  LinkedListIterator<BasicBlockNode> *bbIter = cfg.createBBIterForw();
  while (bbIter->hasMoreElements()){
    BasicBlockNode& curBB = bbIter->currentElement();
    BasicBlockEdge *firstEdge = curBB.getFirstEdge();
    if (firstEdge!=NULL) 
      if (firstEdge->isBackArc()) 
        BuildSingleLoop(curBB.getFirstSucc(), &curBB,workLoopMap);
    BasicBlockEdge *nextEdge = curBB.getNextEdge();
    if (nextEdge!=NULL)
      if (nextEdge->isBackArc()) 
        BuildSingleLoop(curBB.getNextSucc(), &curBB,workLoopMap);
    bbIter->nextElement();
  }
  delete bbIter;
} 

void mergeLoop(loopNode* dest, loopNode* src)
{
  (dest->_tailSet).insert(*((src->_tailSet).begin()));
  intSet destNodeSet = dest->_nodeSet;
  intSet srcNodeSet = src->_nodeSet;
  for(intSet::iterator i=srcNodeSet.begin(); i!=srcNodeSet.end();i++)
    if (destNodeSet.find(*i)==destNodeSet.end())/*different node exists*/
      destNodeSet.insert(*i);
  delete src;
}
 
/*
build the strong connected region in two steps:
1. insert to the new list with headID as key, if there exists a loop with same
   headID in the list, merge them to form a SCR
2. after new list is build, build another new list. Now it is simple, just 
   change the key from headID to the number of nodes in the loop, make the 
   preparation for the building of inner loop.
*/
void buildSCRegion(loopMap& workLoopMap)
{
  loopMap tempLoopMap;
  int headID, key;
  loopMap::iterator i, j;
  loopNode *lpNode;

/*phase 1: build SRC*/
  for( i = workLoopMap.begin(); i!=workLoopMap.end(); i++){
    headID = (*i).first;
     lpNode = (*i).second;
    j = tempLoopMap.find(headID);
    if (j!=tempLoopMap.end()) 
//found a existing loop with the same head, merge them to build SRC
      mergeLoop((*j).second,lpNode);
    else
      tempLoopMap.insert(pair<const int,loopNode*>(headID,lpNode));
  }
  workLoopMap.erase(workLoopMap.begin(),workLoopMap.end());

/*phase 2: reorder in increasing order of the number of nodes in the loop*/
  for(i=tempLoopMap.begin();i!=tempLoopMap.end();i++){
    lpNode = (*i).second;
    key = (lpNode->_nodeSet).size();
    workLoopMap.insert(pair<const int,loopNode*>(key,lpNode));
  }
  tempLoopMap.erase(tempLoopMap.begin(),tempLoopMap.end());
}

void buildLoopRelations(loopMap& workLoopMap)
{
  loopMap::iterator i,j;
  int headID;
  //loopNode *iloop, *jloop;
  intSet nodeSet;

  for(i=workLoopMap.begin();i!=workLoopMap.end();i++){
    for (j = i; j!=workLoopMap.end(); j++){
      if (i==j)
        continue;
      headID = ((*i).second)->head;
      nodeSet=((*j).second)->_nodeSet;
      if (nodeSet.find(headID)!=nodeSet.end()){
/*loop i is the inner loop of loop j since its head is in the node set of loop
j, so every node of loop i is also in loop j*/

        ((*i).second)->_parentLoop = (*j).second;
        break;
      }
    }
  }
}

void formLoopNests(loopMap& workLoopMap, LoopList& _loops)
{
/*create loop node from temporary loop structure*/
  for (loopMap::iterator i=workLoopMap.begin();i!=workLoopMap.end();i++){
    loopNode* lpNode=(*i).second;
    lpNode->loop = new Loop();
    Loop* curLoop = lpNode->loop;
    curLoop->setHeadID(lpNode->head);
    for (intSet::iterator j=(lpNode->_tailSet).begin();
        j!=(lpNode->_tailSet).end();j++)
      curLoop->addTailBBID(*j);
    if ((lpNode->_tailSet).size()>1)
      curLoop->setType(_IRREDUCIBLE_);
    else 
      curLoop->setType(_REDUCIBLE_);
    for (intSet::iterator k=(lpNode->_nodeSet).begin();
        k!=(lpNode->_nodeSet).end();k++)
      curLoop->addBBToLoop(*k);
  }

/*add inner loop to outer loop*/
  for (loopMap::iterator j=workLoopMap.begin();j!=workLoopMap.end();j++){
    loopNode* lpNode=(*j).second;
    if (lpNode->_parentLoop==NULL)/*uppermost loop*/
      _loops.addLoop(lpNode->loop);
    else { /*current loop is the inner loop */
      loopNode* outerNode=lpNode->_parentLoop;
      (outerNode->loop)->addInnerLoop(lpNode->loop);
    }
  }

/*reclaim space*/
  for (loopMap::iterator k=workLoopMap.begin();k!=workLoopMap.end();k++){
    loopNode* lpNode=(*k).second;
//    delete lpNode->loop;
    delete lpNode;
  }
  workLoopMap.erase(workLoopMap.begin(),workLoopMap.end());
}

void computeLoopNestLevel(LoopList& _loops,int curLevel)
{
  _loops.setLevelOfNesting(curLevel);
  MyLinkedListIterator<Loop *> *lpList = _loops.loopIterForw();
  if (lpList==_NULL_)
    return;
  while (lpList->hasMoreElements()){
    Loop *curLoop = lpList->currentElement();
    LoopList& innerLevelList = curLoop->getRefToInnerLoopList();
    computeLoopNestLevel(innerLevelList,curLevel+1);
    lpList->nextElement();
  }
  delete lpList;
}

void Routine::buildLoopList()
{
	// added by SAH on 09/24/01
	//
	// To make this method re-entrant.
	//
	_loops.deleteListOfLoops();

  loopMap workLoopMap;

  initLoopBodies(*this,workLoopMap);
  buildSCRegion(workLoopMap);
  buildLoopRelations(workLoopMap);

  formLoopNests(workLoopMap,_loops);
  computeLoopNestLevel(_loops,0);
}

