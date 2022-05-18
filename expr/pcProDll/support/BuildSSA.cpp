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
/* the algorithm for building _SSA_ is from the paper:
  "Efficienty Computing Static Single Assignment Form and the Control 
   Dependence Graph"  
   in ACM Transactions on Programming Languages and Systems, Vol.13,No4,
   OCt, 1991, Pages 451-490*/

/*Note: The building of _SSA_ can build only once. Modification is needed to build
_SSA_ form from existing _SSA_ form*/

#include "stdafx.h"

#include "BuildSSA.h"
#ifndef WIN32
#include <strstream.h>
#else
#include <strstrea.h>
#endif
#include <ctype.h>
#include <stdlib.h>

#include "RegisterFile.h"
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

extern int numFlowOpsPerInstr;

_instrBBMap instrBBMap;
_defInstrMap defInstrMap;
_sameNameMap sameNameMap;

static int numOfReg; 
static int numBBs;
BasicBlockNode **dfsToBB;
intSet *DOMSets;
static intSet *DFSets;
static DTnode *DTree;
static Routine *curRoutine;
static int *counts;
static intStack *istack;
static int  **Assignment;
static int  **PhiPlace;
static PhiInstr **Phis;

void printSet(intSet &iSet)
{
  cout<<endl;
  for (intSet::iterator i = iSet.begin(); i!=iSet.end(); i++)
    cout<<(*i);
  cout<<endl;
}

void printNameSet()
{
  _sameNameMap::iterator iter;
  _argSet::iterator ti1, ti2;
  int i = 0;

  RegisterArg *arg;
  cout << "Printing Same Name Map" << endl;
  cout << "Size : " << sameNameMap.size() << endl; 
  cout<<endl;
  for (iter = sameNameMap.begin(); iter!=sameNameMap.end(); iter++){
    cout<<endl;
    cout<<(*iter).first;
	cout << endl;
	cout << "\t";
	cout << "Size : " << ((*iter).second).size() << endl;
	cout << "\t";
	ti1 = ((*iter).second).begin();
	ti2 = ((*iter).second).end();
	while (ti1 != ti2)
	{
		
		if (*ti1 != NULL)
		{
			arg = (RegisterArg *)(*ti1);
			arg->print();
		}
		cout << " ";
		ti1++;
	}
  }
  cout<<endl;
  cout << "End Printing Same Name Map" << endl;
}

void printError(char *s) {
  cout<<s<<endl;
  assert(0);
}

int dominate(int dominator, int dominatee)
{
  if (dominator==-1)
    return 1;

	if (dominatee==-1)
		return 0;

  while ((dominator!=0)&&(dominatee!=0)) {
    if (DTree[dominatee].getIDOM()==dominator)
      return 1;
    dominatee = DTree[dominatee].getIDOM();
  }
  return 0;
}

int anyDominate(intSet& dominatorSet, intSet& dominateeSet)
{
  intSet::iterator iter1, iter2;

  for (iter1=dominatorSet.begin(); iter1!=dominatorSet.end(); iter1++) 
    for (iter2=dominateeSet.begin(); iter2!=dominateeSet.end(); iter2++) 
      if (dominate(*iter1,*iter2))
        return 1;
  return 0;
}
/*Dominator tree can be represent in:
    DTnode: {
	int idom; immediate dominator of this node
	set<int> children; the nodes immediate dominated by it
        getIDOM();
        dominateeIterator();
    }
    if there are N CFG nodes in the CFG graph (including ENTRY and EXIT)
    use : DTNODE DTREE[N] to represent dominator tree.
*/

/*Dominator Frontier can be represent in array, each element in the array
is an integer set contains the id of CFG nodes in its DF*/


/*compute the intersection of D(p) for every predecessor of i */
void getNewDomSet(int i,intSet& intersectionSet)
{
  intSet resultSet;
  int dfsNum;

  BasicBlockNode *curBB = dfsToBB[i];
  BasicBlockPredecessors& preds = curBB->getCFPred();
  MyLinkedListIterator<BasicBlockNode *> *preIter=preds.createPredListIterForw();

  if (preIter==NULL)
    return;

  if (preIter->hasMoreElements()){
    dfsNum = (preIter->currentElement())->getDFSNum();
    for (intSet::iterator k=DOMSets[dfsNum].begin(); k!=DOMSets[dfsNum].end();
      k++)
        resultSet.insert(*k);
    preIter->nextElement();
    while (preIter->hasMoreElements()){
//perform one intersection of the predecessors 
      dfsNum = (preIter->currentElement())->getDFSNum();
	  //NOS
#ifndef WIN32
      int interResult[resultSet.size()];
#else
	  int *interResult=new int[resultSet.size()];
#endif
      int *endPos = set_intersection(resultSet.begin(),resultSet.end(),
	DOMSets[dfsNum].begin(), DOMSets[dfsNum].end(),interResult);
      resultSet.erase(resultSet.begin(),resultSet.end());
      for (int *j=interResult; j!=endPos; j++)
        resultSet.insert(*j);
      preIter->nextElement();

	  // SAH : Memory leak
#ifdef WIN32
      delete interResult;
#endif
    }
  }

  delete preIter;

  if (resultSet!=intersectionSet)
  {
    intersectionSet.erase(intersectionSet.begin(),intersectionSet.end());
    for (intSet::iterator k=resultSet.begin();k!=resultSet.end();
       k++)
         intersectionSet.insert(*k);
  }
  intersectionSet.insert(i);

}

/*  build the dominator set for every BasicBlock:
    D(Entry) = {Entry}
    for n in N - {Entry} do D(n) = N;
    while changes to any D(n) occur do:
	for n in N - {Entry} do
	  D(n) = {n} or (intersection of all the D(p} where p is a predecessor
			    of n)
*/

void buildDomSet()
{
  intSet intersectionSet;

  DOMSets[1].insert(1);
  for ( int i = 2; i<=numBBs; i++)
    for (int j=1; j<=numBBs;j++)
	DOMSets[i].insert(j);
/*end of initialization*/

  int change = 1;
  while (change) {
    change = 0;
    for (int i=2; i<=numBBs;i++) {
      getNewDomSet(i, intersectionSet);
      if (intersectionSet != DOMSets[i]){
        change = 1;
        DOMSets[i].erase( DOMSets[i].begin(),DOMSets[i].end());
        DOMSets[i].insert(i);
        for (intSet::iterator k=intersectionSet.begin(); 
	    k!=intersectionSet.end(); k++)
          DOMSets[i].insert(*k);
     }
    }
  }
}

/*
    build of dominator tree from dominator set:
	the immediate dominator of n is the biggest number(excluding itself) 
	in set D(n).
proof: if A and B are in D(n) and A > B. From definition of dominator, every
	path from Entry go through A and go through B, so that A and B are 
	on every path to n. There are two relation between A and B:
          B is the ancestor of A (its dfs number is smaller) or
	  B and A are in different branch.
   	  if B and A are in different branch, there will exist path from Entry
	  to n that go through only one of A, B to n, which is contradict to
	  that all path to n pass both A and B.
*/
void buildDomTree()
{
//no IDOM for the ENTRY block
  DTree[1].setIDOM(0);
  for ( int i=2; i<=numBBs; i++){
    intSet::iterator j = DOMSets[i].end();
    j--;
    j--;
// now j point to second largest element in the set which is the IDOM of i
    int idom = *j;
    DTree[i].setIDOM(idom);
    DTree[idom].addChild(i);
  }
}

/* algorithm for computing dominator frontier for every node:
  for each X in a bottom-up traversal of the dominator tree do
    DF(X) = {};
    for each Y in Succ(X) do
local: if idom(Y) != X
       then DF(X) = DF(Y) or {Y}
    end;
    for each Z in Children(X) do
	for each Y in DF(Z) do
	  if idom(Y) != X
	  then DF(X) = DF(X) or {Y}
  	end
    end
   end
*/

void computeDomFrontier()
{
/*bottom-up traversal of CFG, is also a kind of bottom-up traversal of dominator
  tree*/
  for ( int i = numBBs; i>=1; i--) {
    DFSets[i].erase(DFSets[i].begin(),DFSets[i].end());
    BasicBlockNode *curBB = dfsToBB[i];

//get "local" Dominator Frontier
    BasicBlockNode *firstSucc = curBB->getFirstSucc();
    if (firstSucc!=NULL) {
      int dfsNum = firstSucc->getDFSNum();
      if (DTree[dfsNum].getIDOM() != i)
        DFSets[i].insert(dfsNum);
    }
    BasicBlockNode *nextSucc = curBB->getNextSucc();
    if (nextSucc!=NULL) {
      int dfsNum = nextSucc->getDFSNum();
      if (DTree[dfsNum].getIDOM() != i)
        DFSets[i].insert(dfsNum);
    }

//get "up" Dominator Frontier
    intSet dominateeSet=DTree[i].getChildren();
    for (intSet::iterator z=dominateeSet.begin(); z!=dominateeSet.end(); z++) {
      for (intSet::iterator y=DFSets[*z].begin();y!=DFSets[*z].end(); y++) {
        if (DTree[*y].getIDOM() != i)
          DFSets[i].insert(*y);
      }
    }//end of adding "up" Dominator Frontier
  }//end of bottom-up traversal

}

/* find the CFG nodes that have assignment to variable v, and put them in set.
This can be done by linear search of the Basic Block*/
/* the variables here in the context are all registers, and they are in
the operation of type ComputeOp,and are the dest argument for ComputeOp */
void initAssignmentSet()
{
  int regID;
  BaseArgument *aArg;
  ArgListIter di;

  ControlFlowGraph& cfg = curRoutine->getCFG();

  LinkedListIterator<BasicBlockNode> *BBIterator = cfg.createBBIterForw();
  MyLinkedListIterator<BaseInstruction *> *instrIterator = curRoutine->
	instrListIteratorForw();

  if (!BBIterator->hasMoreElements())
    printError(" There is no BB in the Routine!");

  BasicBlockNode *curBB=&BBIterator->currentElement();
  BaseInstruction *firstInstr = curBB->getFirstInstr();
  BaseInstruction *lastInstr = curBB->getLastInstr();
  BaseInstruction *curInstr;
  int dfsNum = curBB->getDFSNum();
    
  if (!instrIterator->hasMoreElements())
    printError(" There is no instruction in the Routine");

  while (instrIterator->hasMoreElements()){
    curInstr = instrIterator->currentElement();
//build the instruction to basicblock map
    instrBBMap[curInstr] = curBB;

	//find the assignment place below for a variable(register)
    if (curInstr->isNormalInstr()){//only deal with Normal instructions
		//need change for the following line
		ArrayIterator<DataOpSlot> *dataSlotIterator = ((NormalInstr *)curInstr)->getDataOpSlotIter();
		while (dataSlotIterator->hasMoreElements()){
			DataOpSlot& curDataSlot = dataSlotIterator->currentElement();
			BaseOperation *curDataOp = curDataSlot.getPtrToOperation();
			
			if (curDataOp == NULL)
			{
				dataSlotIterator->nextElement();
				continue;
			}
			
			// Now, iterate over the list of destinations, and add it to the Assignment set.
			//
			ArgList & a = curDataOp->destOperandList();
			
			for (di = a.begin(); di != a.end(); di++)
			{
				aArg = (*di);
				if (aArg != NULL)
				{
					if ( aArg->isRegisterArg()==_NO_) 
						printError("the destination variable must be a register!\n");
					regID =globalRegFileList->getID(((RegisterArg *)aArg)->getVal(), ((RegisterArg *)aArg)->getText());
					Assignment[regID][dfsNum] = 1;
				}
			}
			dataSlotIterator->nextElement();
		}
		delete dataSlotIterator;
		
		ArrayIterator<ControlOpSlot> *controlSlotIterator = ((NormalInstr *)curInstr)->getControlOpSlotIter();
		while (controlSlotIterator->hasMoreElements()){
			ControlOpSlot& curControlSlot = controlSlotIterator->currentElement();
			BaseOperation *curControlOp = curControlSlot.getPtrToOperation();
			
			if (curControlOp == NULL)
			{
				controlSlotIterator->nextElement();
				continue;
			}
			
			// Now, iterate over the list of destinations, and add it to the Assignment set.
			//
			ArgList & a = curControlOp->destOperandList();
			
			for (di = a.begin(); di != a.end(); di++)
			{
				aArg = (*di);
				if (aArg != NULL)
				{
					if ( aArg->isRegisterArg()==_NO_) 
						printError("the destination variable must be a register!\n");
					regID =globalRegFileList->getID(((RegisterArg *)aArg)->getVal(), ((RegisterArg *)aArg)->getText());
					Assignment[regID][dfsNum] = 1;
				}
			}
			controlSlotIterator->nextElement();
		}
		delete controlSlotIterator;
    }

    if ( curInstr==lastInstr){//the end of one BB,proceed to next BB
      BBIterator->nextElement();
      if (!(BBIterator->hasMoreElements()))
	break;
      curBB = &BBIterator->currentElement();
      firstInstr = curBB->getFirstInstr();
      lastInstr = curBB->getLastInstr();
      dfsNum = curBB->getDFSNum();
      instrIterator->nextElement();
      assert(firstInstr==instrIterator->currentElement());
    }
    else
      instrIterator->nextElement();
  }
 
  delete BBIterator;
  delete instrIterator;
}


/* Algorithm for compute Phi function place for a variable
   WorkSet: contain the set of nodes has ever been added to AssignmentSet. 
	Initially set: WorkSet = {};
   PhiPlaceSet: contain the set of nodes that has been determined to need Phi 
	function for variable v. Initially set to {}.
     While AssignmentSet!={} do
       take X from AssignmentSet	
       WorkSet = WorkSet OR X;
	for each Y in DF(X) do
	  if Y not in PhiPlaceSet 
	    PhiPlaceSet = PhiPlaceSet OR Y;
	  if Y not IN WorkSet
	    AssignmentSet = AssignmentSet OR Y;
        end for;
     end While
*/
//compute the place to add Phi-funtion and store it in array Phiplace
void computePhiPlace(int variableIndex)
{
  //NOS
#ifndef WIN32
  int HasAlready[numBBs + 1];
  int Work[numBBs + 1];
#else
  int* HasAlready = new int[numBBs + 1];
  int* Work = new int[numBBs + 1];
#endif
  intSet candidateSet;
 
//initialization
  for (int i=1; i<=numBBs; i++) {
    HasAlready[i] = 0;
    Work[i] = 0;
    if (Assignment[variableIndex][i]==1)
      candidateSet.insert(i);
  }
 
  while (candidateSet.begin()!=candidateSet.end()) {
    int x = *(candidateSet.begin());
    candidateSet.erase(candidateSet.begin());
    for (intSet::iterator y = (DFSets[x]).begin();y!=(DFSets[x]).end();y++){
      if (HasAlready[*y]==0) {
        PhiPlace[variableIndex][*y] = 1;
 	HasAlready[*y] = 1;
        if (Work[*y]==0) {
          Work[*y] = 1;
          candidateSet.insert(*y);
        }
      }
    }
  }

#ifdef WIN32
  delete HasAlready;
  delete Work;
#endif
}


/* add Phi-function according to PhiPlace, if PhiPlace[i][j]==1, variable with
  index i need Phi-function at BasicBlock j*/
/*add the Phi instruction as the first instruction in the BB, and set the leader
of BB. Must take care in this procedure. It must be the first time the _SSA_ is
running*/
void addPhiFunc()
{
  BasicBlockNode *curBB ;
  BaseInstruction *firstInstr ;
  BaseInstruction *lastInstr ;
  PhiInstr *instr;
  int i,j;
  
  MyLinkedList<BaseInstruction *>& instrList = curRoutine->getInstrList();

  for (i=1; i<=numBBs; i++) {
    curBB = dfsToBB[i];
    firstInstr = curBB->getFirstInstr();
    lastInstr = curBB->getLastInstr();

    instr = NULL;
    for (j=0; j<numOfReg; j++) {
      if (PhiPlace[j][i]!=0) {//variable j need PhiOp in BasicBlock i
        if (instr==NULL)
          instr = new PhiInstr();
        PhiOp *operation = new PhiOp(curBB,j);
 	instr->addPhiOp(operation);
      }
    }

    if (instr!=NULL) {
      Phis[i] = instr;
      instrBBMap[instr] = curBB;
    }
  }
}

/* the number of argument for a Phi function: V = Phi(V,...,V) equal to the 
in-degree of the CFG node to which this Phi function is added*/
void placePhiFunc()
{
	int i,j;
	
	Assignment = new int*[numOfReg];
	PhiPlace = new int*[numOfReg];
	for (i=0;i<numOfReg;i++)
	{
		Assignment[i] = new int[numBBs+1];
		PhiPlace[i] = new int[numBBs+1];
	}
	
	for (i= 0; i<numOfReg; i++)
	{
		for (j =1; j<=numBBs; j++){
			Assignment[i][j] = 0;
			PhiPlace[i][j] = 0;
		}
	}	
	initAssignmentSet();
	
	for (i = 0; i<numOfReg; i++)
		computePhiPlace(i);
	
	addPhiFunc();
}


/* Algorithm:
   search(X):
     for each statement A in X do:
	if A is an ordinary assignment then
	  for each variable V used in RHS(A) do
	    replace use of V by use of Vi where i=Top(S(V))
 	  end
	for each V in LHS(A) do
	  i = C(V);
	  replace V by new Vi in LHS(A);
	  push i onto S(V)
	  C(V) = i+1;
	end
     end
     for each Y in Succ(Y) do
	j = WhichPred(Y,X);
	for each Phi-function F in Y do
	  replace the j-th operand V in RHS(F) by Vi where i=Top(S(V))
	end
     end
     for each Y in Children(X) [in dominator tree] do
	calll search(Y)
     end
     for each assignment A in X do
	for each V in oldLHS(A) do
	  pop S(V)
	end
     end
end search

In this phase we can keep a record of the definition place for every argument 
in the Phi-function
*/

void addToDefMap(RegisterArg *arg, BaseInstruction *instr)
{
  strstream name;
  _defInstrMap::iterator iter;
  char *tStr;

  arg->print(name);
  name<<ends;
  tStr = name.str();
  if (defInstrMap.find(tStr)==defInstrMap.end()) {
    _instrSet iSet;
    defInstrMap[strdup(tStr)] = iSet;
  }
  iter = defInstrMap.find(tStr);
  (*iter).second.insert(instr);
  //delete []tStr;
}

void removeFromDefMap(RegisterArg *arg, BaseInstruction *instr)
{
  strstream name;
  _defInstrMap::iterator iter;
  char *tStr;
  _instrSet::iterator iiter;

  arg->print(name);
  name<<ends;
  tStr = name.str();
  iter = defInstrMap.find(tStr);
  if (iter==defInstrMap.end()) //should not be empty
	assert(0);
  iiter = (*iter).second.find(instr);
  if (iiter==(*iter).second.end()) //definition should exist
	  assert(0);
  (*iter).second.erase(iiter);
}

/*merge two definitions since their names become same*/  
void mergeDefMap(char *toStr,char *fromStr)
{
	_defInstrMap::iterator from, to;
	_instrSet::iterator iiter;


	to = defInstrMap.find(toStr);
	from = defInstrMap.find(fromStr);
	if (to==defInstrMap.end()) //definition should exist
		assert(0);
	if (from==defInstrMap.end()) //definition should exist
		assert(0);
	for (iiter=(*from).second.begin(); iiter!=(*from).second.end();iiter++)
		(*to).second.insert(*iiter);
	free((*from).first);
	defInstrMap.erase(from);
}

void removeFromInstrMap(RegisterArg *arg, BaseInstruction *instr)
{
  strstream name;
  _defInstrMap::iterator iter;
  _instrSet::iterator iiter;
  char *tStr;

  arg->print(name);
  name<<ends;
  tStr = name.str();
  iter = defInstrMap.find(tStr);
  if (iter!=defInstrMap.end()) {
    iiter = (*iter).second.find(instr);
    if (iiter!=(*iter).second.end())
      (*iter).second.erase(iiter);
  }
  //delete []tStr;
}
  
void addToNameMap(RegisterArg *arg)
{
  strstream name;
  _sameNameMap::iterator iter;
  char *tStr;

  arg->print(name);
  name<<ends;
  tStr = name.str();
//  cout << " " << tStr << " ";
  if (sameNameMap.find(tStr)==sameNameMap.end()) {
    _argSet tempArgSet;
    sameNameMap[strdup(tStr)] = tempArgSet;
  }
  iter = sameNameMap.find(tStr);
  (*iter).second.insert(arg);
//  cout << "\t" << (*iter).second.size() << endl;
  
  //delete []tStr;
}
  
void removeFromNameMap(RegisterArg *arg)
{
  strstream name;
  _sameNameMap::iterator iter;
  _argSet::iterator aiter;
  char *tStr;

  arg->print(name);
  name<<ends;
  tStr = name.str();
  iter = sameNameMap.find(tStr);
  if (iter!=sameNameMap.end()) {
    aiter = (*iter).second.find(arg);
    if (aiter!=(*iter).second.end())
      (*iter).second.erase(aiter);
  }
  //delete []tStr;
}
  
void renameRHSArg(RegisterArg *Arg)
{
  SSAArgProperty SSAP;

  if ( Arg->isRegisterArg()==_NO_) 
    printError("the variable must be a register!\n");

  int regID =globalRegFileList->getID(Arg->getVal(), Arg->getText());
  int SSAid = istack[regID].back();
  if (SSAid==0) {
//    cout<<"used before definition !" << endl;
    istack[regID].push_back(1);
    counts[regID]++;
    SSAid = 1;
  }
  SSAArgProperty *pro = (SSAArgProperty *)(Arg->getPtrToProperty(_SSA_));
  if (pro!=NULL) 
    pro->setSSANum(SSAid);
  else {
    SSAP.setSSANum(SSAid);
    Arg->addProperty(SSAP);
  }
  addToNameMap(Arg);
}

void renameRHSforComputeOp(ComputeOp *cptOp)
{
	BaseArgument *srcArg;
	ArgListIter si;
	ArgList & a = cptOp->sourceOperandList();

	for (si = a.begin(); si != a.end(); si++)
	{
		srcArg = *si;
		if (srcArg != NULL)
			if (srcArg->isRegisterArg())
				renameRHSArg((RegisterArg *)srcArg);
	}
}

void renameRHSforIfOp(IfOp *ifOp)
{
	BaseArgument *srcArg;
	ArgListIter si;
	ArgList & a = ifOp->sourceOperandList();

	for (si = a.begin(); si != a.end(); si++)
	{
		srcArg = *si;
		if (srcArg != NULL)
			if (srcArg->isRegisterArg())
				renameRHSArg((RegisterArg *)srcArg);
	}
}

void renameRHSforMemoryOp(MemoryOp *mOp)
{
	BaseArgument *srcArg;
	ArgListIter si;
	ArgList & a = mOp->sourceOperandList();

	for (si = a.begin(); si != a.end(); si++)
	{
		srcArg = *si;
		if (srcArg != NULL)
			if (srcArg->isRegisterArg())
				renameRHSArg((RegisterArg *)srcArg);
	}
}

void renameRHSforCallOp(CallRetOp *cOp)
{
	BaseArgument *srcArg;
	ArgListIter si;
	ArgList & a = cOp->sourceOperandList();

	for (si = a.begin(); si != a.end(); si++)
	{
		srcArg = *si;
		if (srcArg != NULL)
			if (srcArg->isRegisterArg())
				renameRHSArg((RegisterArg *)srcArg);
	}

  ArgList & al = cOp->getParmList();
  for (ArgListIter i = al.begin(); i != al.end(); i++)
  {
	  srcArg = *i;
	  if (srcArg!=NULL)
	  {
		if (srcArg->isRegisterArg())
		  renameRHSArg((RegisterArg *)srcArg);
	  }
  }

  ArgList & sl = cOp->getSavedRegList();
  for (ArgListIter j = sl.begin(); j != sl.end(); j++)
  {
	  srcArg = *j;
	  if (srcArg!=NULL)
	  {
		if (srcArg->isRegisterArg())
		  renameRHSArg((RegisterArg *)srcArg);
	  }
  }
}

//it is implied that two operations write to same variable can not in same instr
void renameForRHS(NormalInstr *curInstr)
{
/*Now assume there is only one operation in one instruction, which is the first
operation. Later on, will travel the tree for all the operations */
  BaseOperation *headOp = curInstr->getPtrToRootOperation();
  if (headOp->isComputeOp()) 
    renameRHSforComputeOp((ComputeOp *)headOp);
  else if (headOp->isIfOp()) 
    renameRHSforIfOp((IfOp *)headOp);
  else if (headOp->isMemoryOp())
    renameRHSforMemoryOp((MemoryOp *)headOp);
  else if (headOp->isCallRetOp())
    renameRHSforCallOp((CallRetOp *)headOp);
}

void renameLHSforArg(RegisterArg *destArg,BaseInstruction *curInstr)
{
  SSAArgProperty SSAP;

  if (destArg==0) 
    printError("No destArg");
  if ( destArg->isRegisterArg()==_NO_) 
    printError("the destination variable must be a register!\n");
//change the following line
  int regID =globalRegFileList->getID(destArg->getVal(),destArg->getText());
  counts[regID]++;
  int SSAid = counts[regID];
  SSAArgProperty *pro =(SSAArgProperty *) (destArg->getPtrToProperty(_SSA_));
  if (pro!=NULL) 
    pro->setSSANum(SSAid);
  else {
    SSAP.setSSANum(SSAid);
    destArg->addProperty(SSAP);
  }
  istack[regID].push_back(SSAid);  
  addToNameMap(destArg);
  addToDefMap(destArg,curInstr);
}

/*NOTE: this function is the complementary of popLHSforOneInstr, when you change this function,
dont' forget to change popLHSforOneInstr*/
void renameNormalForLHS(NormalInstr *curInstr)
{
/*Now assume there is only one operation in one instruction, which is the first
operation. Later on, will travel the tree for all the operations */
    BaseOperation *headOp = curInstr->getPtrToRootOperation();

	BaseArgument *destArg;
	ArgListIter di;
	ArgList & a = headOp->destOperandList();

	for (di = a.begin(); di != a.end(); di++)
	{
		destArg = *di;

		if (destArg != NULL)
			if (destArg->isRegisterArg())
				renameLHSforArg((RegisterArg *)destArg,curInstr);
	}
}

void renamePhiForLHS(PhiInstr *curInstr)
{
    MyLinkedListIterator<PhiOp *> *iter = curInstr->PhiOpsForwardIterator();
    while (iter->hasMoreElements()){
      PhiOp *operation = iter->currentElement();
      BaseArgument *destArg = operation->ptrToDestOperand();
      renameLHSforArg((RegisterArg *)destArg,curInstr);
      iter->nextElement();
    }
    delete iter;
}

/*rename Phi-function Arg in BasicBlock curBB whose definition is in or passed
from BasicBlock with id--fromBBid*/
void renamePhiArgforOneSucc(BasicBlockNode *curBB,int fromBBid)
{
  PhiOp *operation ;
  BaseArgument *srcArg ;
  int curBBid = curBB->getDFSNum();

  if (Phis[curBBid]!=NULL){
    MyLinkedListIterator<PhiOp *> *iter = Phis[curBBid]->PhiOpsForwardIterator();
    while (iter->hasMoreElements()){
      operation = iter->currentElement();
      srcArg = operation->ptrToPhiArg(fromBBid);
      if (srcArg==NULL)
	printError("destArg not exist in Phi operation");
      renameRHSArg((RegisterArg *)srcArg);
      iter->nextElement();
    }
    delete iter;
  }
}

//renaming for the arguments for Phi-functions in the succ-BBs of BBid
int renameForPhiArg(int BBid)
{

  BasicBlockNode *curBB = dfsToBB[BBid];
//currently only 2 successors
  BasicBlockNode *first = curBB->getFirstSucc();
  BasicBlockNode *next = curBB->getNextSucc();
  if (first!=NULL)
    renamePhiArgforOneSucc(first,BBid);
  if (next!=NULL)
    renamePhiArgforOneSucc(next,BBid);

  //NOS
  return 0;
}

void popLHSforArg(RegisterArg *Arg)
{
    if (Arg==0) 
	{
      // printError("No Arg");
	  return;
	}
    if ( Arg->isRegisterArg()==_NO_) 
      printError("the variable must be a register!\n");
//change the following line
    int regID =globalRegFileList->getID(Arg->getVal(), Arg->getText());
    istack[regID].pop_back();
}

/*NOTE: this function is the complementary of renameNormalForLHS, when you change renameNormalForLHS,
you should change this function also*/
void popLHSforOneInstr(BaseInstruction *curInstr)
{

  if (curInstr->isNormalInstr()) {
/*Now assume there is only one operation in one instruction, which is the first
operation. Later on, will travel the tree for all the operations */
    BaseOperation *headOp = ((NormalInstr *)curInstr)->getPtrToRootOperation();
  
	BaseArgument *destArg;
	ArgListIter di;
	ArgList & a = headOp->destOperandList();

	for (di = a.begin(); di != a.end(); di++)
	{
		destArg = *di;

		if (destArg != NULL)
			if (destArg->isRegisterArg())
				popLHSforArg((RegisterArg *)destArg);
	}
  }
  else if (curInstr->isPhiInstr()){
    MyLinkedListIterator<PhiOp *> *iter = ((PhiInstr *)curInstr)->PhiOpsForwardIterator();
    while (iter->hasMoreElements()){
      PhiOp *operation = iter->currentElement();
      BaseArgument *destArg = operation->ptrToDestOperand();
      popLHSforArg((RegisterArg *)destArg);
      iter->nextElement();
    }
    delete iter;
  }
  else
    printError("Wrong Instruction type");
}

//restore the stack to the state before the renaming enter BBid
int restoreStack(int BBid)
{
  
  MyLinkedList<BaseInstruction *>& instrList = curRoutine->getInstrList();
  BasicBlockNode *curBB = dfsToBB[BBid];
  BaseInstruction *firstInstr = curBB->getFirstInstr();
  BaseInstruction *lastInstr = curBB->getLastInstr();

  MyListNode<BaseInstruction *> *container = instrList.findElem(firstInstr);
  if (container==NULL)
    printError("Instruction not found 1...");

//if there is definition in the BB, pop it from the stack
  while (container!=NULL) {
    BaseInstruction *curInstr=container->getItem();
    popLHSforOneInstr(curInstr);
    if (curInstr==lastInstr)
      break;
    container = container->nextItem();
  }
  
  //remove for definition by Phi functions, since they are not explicitly inserted into instruction stream
  if (Phis[BBid])
	popLHSforOneInstr(Phis[BBid]);

  //NOS
  return 0;
}

void searchForSSARename(int BBid)
{
  
  MyLinkedList<BaseInstruction *>& instrList = curRoutine->getInstrList();
  BasicBlockNode *curBB = dfsToBB[BBid];
  BaseInstruction *firstInstr = curBB->getFirstInstr();
  BaseInstruction *lastInstr = curBB->getLastInstr();

  MyListNode<BaseInstruction *> *container = instrList.findElem(firstInstr);
  if (container==NULL)
    printError("Instruction not found 1...");

//renaming every statement in current BB in the following loop
  if (Phis[BBid]!=NULL) 
    renamePhiForLHS(Phis[BBid]);

  while (container!=NULL) {
    NormalInstr *curInstr=(NormalInstr *)container->getItem();

//rename for reference point
    renameForRHS(curInstr);
//rename for definition point
    renameNormalForLHS(curInstr);

    if (curInstr==lastInstr)
      break;
    container = container->nextItem();
  }

  renameForPhiArg(BBid);

//recursively call this function for the Children BBs in DTree of this BB
  intSet& children = DTree[BBid].getChildren();
  for (intSet::iterator child=children.begin();child!=children.end();child++)
    searchForSSARename( *child );

  restoreStack(BBid);
}


/*renaming is done for all the variables at the same time. Since it will 
travel the program linear, it will cost much high if we do it for every 
variable separately
Algorithm:

    for every variabe V do
	C(V) = 0;
	S(V) = EmptyStack;
    end
    call searchForSSARename(Entry);
*/
void renameForSSA()
{

// 1 is the dfsNum of first BB
  searchForSSARename(1);
}

void initSSAStruct(ControlFlowGraph& cfg)
{
	int i;
  istack = new intStack[numOfReg];
  numBBs = cfg.numBBs();

  dfsToBB = new BasicBlockNode*[numBBs+1];
  LinkedListIterator<BasicBlockNode> *bbIter = cfg.createBBIterForw();
  while (bbIter->hasMoreElements()){
    BasicBlockNode& curBB = bbIter->currentElement();
    dfsToBB[curBB.getDFSNum()] = &curBB;
    bbIter->nextElement();
  }
  delete bbIter;

  instrBBMap.erase(instrBBMap.begin(),instrBBMap.end());
  defInstrMap.erase(defInstrMap.begin(),defInstrMap.end());
  sameNameMap.erase(sameNameMap.begin(),sameNameMap.end());

  DOMSets = new intSet[numBBs+1];
  DFSets = new intSet[numBBs+1];
  DTree = new DTnode[numBBs+1];
  counts = new int[numOfReg];
  Phis = new PhiInstr*[numBBs+1];

  for (i=0; i<numOfReg; i++)
    istack[i].push_back(0);

  for ( i=0;i<numOfReg;i++)
    counts[i] = 0;

  for ( i=1; i<=numBBs; i++)
    Phis[i] = NULL;
}

void clearSSAStruct()
{
	int i;

  delete []dfsToBB;

  for (i=0; i<numBBs+1; i++)
	  DOMSets[i].clear();
  delete []DOMSets;

  delete []DTree;
  delete []DFSets;
  delete []counts;

	
  for (i=0; i<numOfReg; i++)
    istack[i].clear();
  delete []istack;

  
  for (i=0;i<numOfReg;i++)
    delete [](Assignment[i]);
  delete []Assignment;
  for (i=0;i<numOfReg;i++)
    delete [](PhiPlace[i]);
  delete []PhiPlace;
  for (i=1; i<=numBBs;i++)
    if (Phis[i]!=NULL)
      delete Phis[i];
  delete []Phis;

  instrBBMap.erase(instrBBMap.begin(),instrBBMap.end());
  for (_defInstrMap::iterator iter1=defInstrMap.begin(); iter1!=defInstrMap.end();iter1++)
    free((*iter1).first);
  defInstrMap.erase(defInstrMap.begin(),defInstrMap.end());
  for (_sameNameMap::iterator iter=sameNameMap.begin(); iter!=sameNameMap.end();iter++)
    free((*iter).first);
  sameNameMap.erase(sameNameMap.begin(),sameNameMap.end());
}

void  setSymbolTable()
{
  if (globalSymbolTable==NULL) {
    cout<< "No symbol table" <<endl;
  }
  else {
    for (int i = 0; i<numOfReg; i++)
    {
        globalSymbolTable->setEntry(i, counts[i], -1);
//      cout<<i<< "= "<<counts[i]<<endl;
    }
    globalSymbolTable->setStartNums();
  }
}

int domBottom(int index, vector<intSet>&useBBVec)
{
  intSet dominator = useBBVec[index];
  intSet dominatee;

  for (int i=0; i<useBBVec.size(); i++)
    if (i==index)
      continue;
    else {
      dominatee = useBBVec[i];
      if (anyDominate(dominator,dominatee))
        return 0; 
    }
  return 1;
}

//a position in freeVec is free, means we don't need to add additional move
//operation to eliminate a PhiOp
//a position is free, if the corresponding BasicBlockNode is at the bottom
//of the dominate tree consisted of all the BasicBlockNode appear in the Phiop
void computeFreeVec(PhiOp *op,vector<int>& freeVec)
{
  RegisterArg *srcArg;
  MyLinkedListIterator<BaseArgument *> *argIter ;
  _defInstrMap::iterator DIter;
  _instrBBMap::iterator IBIter;
  BaseInstruction *curInstr;
  BasicBlockNode *curBB;
  int bbID;
  vector< intSet > useBBVec;
  intSet iset;
  _instrSet::iterator iiter;

  argIter = op->phiArgIterForw();
  while(argIter->hasMoreElements()) {
//find in which BBs this srcArg is defined
    srcArg = (RegisterArg *)argIter->currentElement();
    strstream name;
    char *tStr;
    srcArg->print(name);
    name<<ends;
    tStr = name.str();
    DIter = defInstrMap.find(tStr);
    iset.erase(iset.begin(),iset.end());

    if (DIter!=defInstrMap.end()) {
      for (iiter=(*DIter).second.begin(); iiter!=(*DIter).second.end();iiter++){
        curInstr = (BaseInstruction *)(*iiter);
        IBIter = instrBBMap.find(curInstr);
        assert(IBIter!=instrBBMap.end());
        curBB = (BasicBlockNode *)(*IBIter).second;
        bbID = curBB->getDFSNum();
        iset.insert(bbID);
      }
      useBBVec.push_back(iset);
    }
    else { //possible reasons:
		//1. this variable is used before def, 
		//2. or maybe it is a parameter
	   //we can assume it is defined in the entry of the CFG, thus it will
	   //dominate every basicblock, so this variable is not free
      iset.insert(-1);
      useBBVec.push_back(iset);
    }
    //delete []tStr;
    argIter->nextElement();
  }

  freeVec.erase(freeVec.begin(),freeVec.end());
  for (int i=0; i<useBBVec.size(); i++)
    if (!domBottom(i, useBBVec))
      freeVec.push_back(0);
    else
      freeVec.push_back(1);
}

ComputeOp *createMoveOp(int destSSANum, BaseArgument *regArg, int srcSSANum, OpCode & opc)
{
	char *regName;
	int regID;
	RegFileNum regFile;
	RegisterArg *srcArg, *destArg;
	SSAArgProperty ssaP;
	ComputeOp *movOp;

	regName = ((RegisterArg *)regArg)->getText();
	regID = ((RegisterArg *)regArg)->getVal();
	regFile = ((RealRegArg *)regArg)->regFileIndex();
 
	srcArg = new RealRegArg(regName,regID,regFile);
	ssaP.setSSANum(srcSSANum);
	srcArg->addProperty(ssaP);
	destArg = new RealRegArg(regName,regID,regFile);
	ssaP.setSSANum(destSSANum);
	destArg->addProperty(ssaP);
  
	movOp = new ComputeOp();
	movOp->initOpCode(opc);
	movOp->addOperand(destArg, _DEST_, _DEST_LIST_);
	movOp->addOperand(srcArg, _SOURCE_1_);

	delete srcArg;
	delete destArg;

	return movOp;
}

//add an move instructino: move arg.?, arg.destSSANum, to the basicblock
//where the definition of arg comes from
ComputeOp *createMoveOp(int destSSANum, RegisterArg *arg)
{
  OpCode opCode;
  int srcSSANum;
  SSAArgProperty SSAP;
  SSAArgProperty *ssaProp;

  ssaProp = (SSAArgProperty *)(arg->getPtrToProperty(_SSA_));
  srcSSANum = ssaProp->getSSANum();

  // Now, decide whether it should be IASSIGN or FASSIGN
  //
  if (arg->getValType() == _INT_)
	  opCode.init("IASSIGN");
  else if (arg->getValType() == _FLOAT_)
	  opCode.init("FASSIGN");
  else
	  opCode.init("ASSIGN");

  if (srcSSANum==destSSANum) //don't move to itself
    return NULL;
	
  BaseArgument *baseArg = arg;
  return (createMoveOp(destSSANum, baseArg, srcSSANum, opCode));

}

void realMergeName(_argSet& to, _argSet& from)
{
  _argSet::iterator iter;

  RegisterArg *arg;

  for (iter = from.begin(); iter!=from.end(); iter++)
  {
	  arg = (RegisterArg *)*iter;
    to.insert(*iter);
  }
  from.erase(from.begin(),from.end());
}

void renameArgSet(int toSSAnum, _argSet& aSet)
{
  _argSet::iterator iter;
  RegisterArg *arg;
  SSAArgProperty *ssaProp;

  for (iter = aSet.begin(); iter!=aSet.end(); iter++) {
    arg = (RegisterArg *)(*iter);
    ssaProp = (SSAArgProperty *)(arg->getPtrToProperty(_SSA_));
    ssaProp->setSSANum(toSSAnum);
  }
}

//the arg.? and arg.destSSANum will share the same name
//merge their entries in sameNameMap
void mergeName(int destSSANum, RegisterArg *arg)
{
  char regName[100];
  strstream fromName,toName;
  _sameNameMap::iterator fromIter,toIter;
  char *ptr1, *ptr2,*totStr,*fromtStr;

  arg->print(fromName);
  fromName<<ends;
  ptr1 = regName;
  fromtStr = ptr2 = fromName.str();
  while(*ptr2!='.')
    *ptr1++ = *ptr2++; 
  *ptr1 = 0;
  toName<<regName<<'.'<<destSSANum<<ends;
  totStr = toName.str();

  fromIter = sameNameMap.find(fromtStr);
  toIter = sameNameMap.find(totStr);
  assert(fromIter!=sameNameMap.end());
  assert(toIter!=sameNameMap.end());
//  cout << (*fromIter).first << endl;
//  cout << (*toIter).first << endl;
  if (fromIter!=toIter){
	mergeDefMap(totStr,fromtStr);
    renameArgSet(destSSANum,(*fromIter).second);
    realMergeName((*toIter).second,(*fromIter).second);
    free((*fromIter).first);
    sameNameMap.erase(fromIter);
  }
  //delete []totStr;
  //delete []fromtStr;
}

extern Routine *currRoutine;

void insertMovIntoMaps(NormalInstr *instr, BasicBlockNode *bb)
{

	BaseOperation *headOp;
	BaseArgument *destArg, *srcArg;

    instrBBMap[instr] = bb;

    headOp = instr->getPtrToRootOperation();
    if (!headOp->isComputeOp())
		assert(0);

    destArg = ((ComputeOp *)headOp)->ptrToOperand(_DEST_, _DEST_LIST_);
    if (!destArg->isRegisterArg())
		assert(0);
	addToNameMap((RegisterArg *)destArg);
	addToDefMap((RegisterArg *)destArg,instr);

	srcArg = ((ComputeOp *)headOp)->ptrToOperand(_SOURCE_1_);
	if (!srcArg->isRegisterArg())
		assert(0);
	addToNameMap((RegisterArg *)srcArg);
}

/*
void convertGotosToJumps(NormalInstr *instr)
{
	BaseOperation *oper, *tempOper;
	JumpOp jOp;

	ArrayIterator<FlowOpSlot> *iter = instr->getFlowOpSlotIter();

	while (iter->hasMoreElements())
	{
		if ((iter->currentElement()).hasOper())
		{
			oper = (iter->currentElement()).getPtrToOperation();

			if (oper->isGotoOp())
			{
				jOp.setTargetInstr(oper->getTargetInstr());

				tempOper = &(jOp);

				(iter->currentElement()).resetOperation();
				(iter->currentElement()).addOperation(tempOper);

				delete oper;
			}
		}
		iter->nextElement();
	}
	delete iter;
}

void removeLabel(NormalInstr *instr)
{
	if (instr == NULL) return;

	if (instr->hasLabel())
	{
		instr->deleteLabel();
	}
}
*/

extern void convertGotosToJumps(NormalInstr *instr);
extern void removeLabel(NormalInstr *instr);

void removeFromSameNameMap(BaseArgument *arg)
{
	if ((arg == NULL) || (!arg->isRegisterArg()))
		return;

	removeFromNameMap((RegisterArg *)arg);
}

extern int _LAST_FLOW_POS_;

void insertAsLastInstr(ComputeOp *movOp, BasicBlockNode *bb)
{
  NormalInstr *newInstr;
  JumpOp tempJumpOp;
  BaseOperation *headOp;

  int index;
  
  BaseOperation *a = movOp;
  NormalInstr  *lastInstr = (NormalInstr *)bb->getLastInstr();
  
  if (lastInstr == NULL) // i.e. the basic block is a newly created block (or a dummy block)
  {
	  newInstr = new NormalInstr();
	  newInstr->initScheduleAndAdd(a);

	  // Now add a flowop to nowhere.
	  //
	  JumpOp tempJumpOp;
	  index = newInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
	  (newInstr->getRootOpSlot()).setNext(&(newInstr->getFlowOpSlot(index)));

	  // add this new instr to the current routine.
	  //
	  currRoutine->appendInstruction((BaseInstruction *)newInstr);

	  bb->initLeader(newInstr);
	  bb->initTrailer(newInstr);

	  insertMovIntoMaps(newInstr,bb);
	  return;
  }

  // If the basic block is not a newly created block (or a dummy block)
  // then .....

  headOp = lastInstr->getPtrToRootOperation();

  if (!headOp->isFlowOp())
  {
   // If the last Instr contained a Goto, we do not want to retain the goto op.
   // This is becos later passes that reconstruct the cfg get confused by the
   // goto op.
   //
   // So, we iterator over the old and new instr and then convert any goto op to a corresponding
   // flow op.
   //
   // Also, if the last Instr contained a label, we want to remove the label.
   //
   convertGotosToJumps(lastInstr);

   newInstr = new NormalInstr(*lastInstr);

   convertGotosToJumps(newInstr);
   removeLabel(newInstr);

   currRoutine->insertInstrAfter(newInstr,lastInstr);
  }
  if (headOp->isComputeOp()||headOp->isMemoryOp()) {
	  //inserted instruction is inserted after the last instruction of BB
	  //setup FlowOp for the old last instruction
	  ArrayIterator<FlowOpSlot> *fIter = lastInstr->getFlowOpSlotIter();
	  while (fIter->hasMoreElements()) {
		  if (fIter->currentElement().hasOper()) {
			  JumpOp *jOp = (JumpOp *)fIter->currentElement().getPtrToOperation();
			  jOp->setTargetInstr(newInstr);
			  break;
		  }
		  fIter->nextElement();
	  }
	  delete fIter;
	  
	  DataOpSlot& slot = (DataOpSlot&)newInstr->getRootOpSlot();
	  slot.deleteOperation();
	  
	  // Now save pointer to the flow op slot that was pointed to by this slot.
	  BaseOpSlot *flowSlot = slot.getPtrToNextSlot();
	  
	  // Now delete the next pointer of the slot (whose operation too was deleted).
	  slot.setNext(NULL);
	  
	  newInstr->initScheduleAndAdd(a);
	  
	  (newInstr->getRootOpSlot()).setNext(flowSlot);
	  bb->initTrailer(newInstr);
	  
	  insertMovIntoMaps(newInstr,bb);
  }
  else if (headOp->isCallOp())
  {
	  // Set the fall thru branch to the new instruction.

	  BaseOpSlot & fslot = lastInstr->getFlowOpSlot(_LAST_FLOW_POS_);
	  JumpOp *jOp = (JumpOp *)(fslot.getPtrToOperation());
	  jOp->setTargetInstr(newInstr);

	  // Now set the newInstr as the move operation.
	  // Delete the old operation, clean up the flowops.
	  
	  ControlOpSlot & cslot = (ControlOpSlot&)newInstr->getRootOpSlot();
	  cslot.deleteOperation();
	  
	  // Now save pointer to the flow op slot that was pointed to by this slot.
	  BaseOpSlot *flowSlot = cslot.getPtrToNextSlot();
	  
	  // Now reset the next pointer of the slot (whose operation too was deleted).
	  cslot.setNext(NULL);

	  // Also, reset the true branch.
	  if ((cslot.getPtrToTrueBranch())->hasOper())
	  {
		  (cslot.getPtrToTrueBranch())->deleteOperation();
	  }
	  cslot.setTrueBranch(NULL);

	  newInstr->initScheduleAndAdd(a);
	  
	  (newInstr->getRootOpSlot()).setNext(flowSlot);
	  bb->initTrailer(newInstr);
	  
	  insertMovIntoMaps(newInstr,bb);
  }
  else if (headOp->isIfOp()){
//the inserted instruction is really inserted before the last instruction of BB
//since the last instruction of BB is IF, there is no assignment to change the
//value of some registers, doing so is same , and easy
//remove old test argument from sameNameMap

	// removing old test argument from sameNameMap.
	// Added by SAH : Wed, Aug 16, 2000
	//
	removeFromSameNameMap(headOp->ptrToOperand(_COND_REG_));

//insert the mov operation in the old last instruction
    lastInstr->removeAllOp();
    lastInstr->initScheduleAndAdd(a);

//setup jump for old last instruction
    tempJumpOp.setTargetInstr(newInstr);
    index = lastInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
    (lastInstr->getRootOpSlot()).setNext(&(lastInstr->getFlowOpSlot(index)));
	
	bb->initTrailer(newInstr);

	insertMovIntoMaps(lastInstr,bb);

	//since we use newly copied IfOp, the CC register doesn't exist in the name map, add it.
	headOp = newInstr->getPtrToRootOperation();
    BaseArgument *ifArg = ((IfOp *)headOp)->ptrToCCReg();
	if (!ifArg->isRegisterArg())
		assert(0);
	addToNameMap((RegisterArg *)ifArg);
  }
  else if (headOp->isRetOp()){
//the inserted instruction is really inserted before the last instruction of BB
//since the last instruction of BB is RETURN, there is no assignment to change the
//value of some registers, doing so is same , and easy
//remove old return reg argument from sameNameMap

	// removing old test argument from sameNameMap.
	// Added by SAH : Wed, Aug 16, 2000
	//
	  removeFromSameNameMap(headOp->ptrToOperand(_SOURCE_1_));

//insert the mov operation in the old last instruction
    lastInstr->removeAllOp();
    lastInstr->initScheduleAndAdd(a);

//setup jump for old last instruction
    tempJumpOp.setTargetInstr(newInstr);
    index = lastInstr->addFlowOpToList(&(tempJumpOp), _LAST_FLOW_POS_);
    (lastInstr->getRootOpSlot()).setNext(&(lastInstr->getFlowOpSlot(index)));
	
	bb->initTrailer(newInstr);

	insertMovIntoMaps(lastInstr,bb);

	//since we use newly copied IfOp, the CC register doesn't exist in the name map, add it.
	headOp = newInstr->getPtrToRootOperation();
    BaseArgument *retArg = ((CallRetOp *)headOp)->ptrToOperand(_SOURCE_1_);
	if ((retArg != NULL) && (!retArg->isRegisterArg()))
		assert(0);
	addToNameMap((RegisterArg *)retArg);
  }
  else 
  {
	  if (headOp->isFlowOp())
	  {
//		  cout << " Encountered FlowOp: " << headOp->isGotoOp() << endl;
		  BaseOpSlot & flowSlot = lastInstr->getRootOpSlot();

		  // DEBUG code.
		  //
/*
		  cout << "Printing DEBUG Code in SSA" << endl;
		  cout << endl;
		  BaseOperation *op = flowSlot.getPtrToOperation();
		  if (op != NULL)
		  {
			  op->shortPrint(cout);
			  cout << endl;
		  }
		  cout << "*^**^**^**^**^**^**^**^**^**^**^**^**^**^**^**^**^*" << endl;
		  //
*/
		  lastInstr->initScheduleAndAdd(a);
		  lastInstr->getRootOpSlot().setNext(&(flowSlot));

		  insertMovIntoMaps(lastInstr, bb);
	  }
	  else // headOp->isControlOp()
	  {
		  cout << " Encountered unknown type of operation" << endl;
		  assert(0);
	  }
  }
}

// This function also takes care of some SSA business. If you just want to insert a move op
// to the basic-block, then use the function above. 

// Note: In this function, when the last instruction of the bb is a flow op, the ssa Maps are
// not updated properly.
//
void insertLastBBInstr(ComputeOp *movOp ,int BBid)
{
 
  BasicBlockNode *curBB = dfsToBB[BBid];
//  currBB->insertAsLastOp(movOp);
  insertAsLastInstr(movOp, curBB);
  
}

void elimPhiOp(PhiOp *op, PhiInstr *instr )
{
  RegisterArg *destArg, *srcArg;
  MyLinkedListIterator<BaseArgument *> *argIter ;
  int destSSAnum , predBBid;
  PredidArgProperty *predProp;
  SSAArgProperty *ssaProp;
  ComputeOp *movOp;
  vector<int> freeVec;
  vector<int>::iterator viter;

  destArg = (RegisterArg *)op->ptrToDestOperand();
  ssaProp = (SSAArgProperty *)(destArg->getPtrToProperty(_SSA_));
  destSSAnum = ssaProp->getSSANum();

  computeFreeVec(op,freeVec);
  viter = freeVec.begin();
  argIter = op->phiArgIterForw();
//the arg that is free, can be renamed without addition mov operation
  while (argIter->hasMoreElements()) {
    srcArg = (RegisterArg *)argIter->currentElement();
    if (*viter) 
      mergeName(destSSAnum,srcArg);
    else {
      movOp = createMoveOp(destSSAnum,srcArg);
	  //
	  // The next piece of code was inserted by Ashok on Jul 8, 98.
	  //
	  // This is to take care of the condition when createMoveOp returns null;
	  //	
	  if (movOp != NULL)
	  {
		predProp = (PredidArgProperty *) (srcArg->getPtrToProperty(_PREDID_));
		predBBid = predProp->getPredID();
		insertLastBBInstr(movOp,predBBid);

		// SAH : Memory leak : 07/19/2000
		delete movOp;
	  }
    }
    argIter->nextElement();
    viter++;
  }

  removeFromDefMap(destArg,instr);

  delete argIter;
}

void elimPhi()
{
  PhiOp *op ;
  MyLinkedListIterator<PhiOp *> *opIter ;

  for (int i =1; i<=numBBs; i++) 
    if (Phis[i]!=NULL) {
      opIter = Phis[i]->PhiOpsForwardIterator();
      while (opIter->hasMoreElements()){
        op= opIter->currentElement();

	elimPhiOp(op, Phis[i]);

        opIter->nextElement();
      }
	    delete opIter;
	    }
	}

	//rename variables after variables have merge
	//if we don't care variables' ssaNum be consecutive, this pass is not needed
	void renumberName()
	{
	  int i;
	  _sameNameMap::iterator iter;
	  int regID,ssaNum;
	  RegisterArg *arg;


	  for (i=0; i<numOfReg; i++)
	    counts[i] = 0;
	  for (iter=sameNameMap.begin();iter!=sameNameMap.end();iter++) {
	    arg = (RegisterArg *) (*(((*iter).second).begin()));
	    regID =globalRegFileList->getID(arg->getVal(), arg->getText());
	    counts[regID]++;
	    ssaNum = counts[regID];
	    renameArgSet(ssaNum,  (*iter).second);
	  }
	}

	void Routine::buildSSA()
	{
	  //int i;

	  curRoutine = this;
	  
	  if (isEmptyBodyRoutine()) return;

	  numOfReg = RegisterFileList::totNumRegs();

	  initSSAStruct(this->getCFG());

	// 1. First build dominate tree.
	  buildDomSet();
	  buildDomTree();

	// 2. compute the dominate frontiers of every node.
	  computeDomFrontier();

	  placePhiFunc();

	  renameForSSA();

	  printf("BEGIN ELIMINATE Phi Functions\n");
//	  print();

//	  printNameSet();
	  elimPhi();
//	  print();
//	  printNameSet();

	  renumberName();
	
	  printf("\n\nAFTER ELIMINATE Phi Functions\n");
	  //print();

	  setSymbolTable();
	  clearSSAStruct();
}
