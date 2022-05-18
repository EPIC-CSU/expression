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
// OptimizationsSupport.cpp
//
// File:  		OptimizationsSupport.cpp
// Created:		Fri Jul 27, 2001
// Author:		Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
// 

#include "stdafx.h"

#include "NewInstrSel.h"
#include "OptParse.h"
#include "Optimizations.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

ISelRuleType lhsRules;
ISelRuleType rhsRules;

OpcToRuleNoType OptOpcToRuleNo;

Vector<BaseInstruction *> doneOptVect;
Vector<ISelIROpNode> listOfOptIROps;

Vector<ISelIROpNode>::iterator findOperFromListOfOptIROps(BaseOperation *op)
{
	Vector<ISelIROpNode>::iterator retVal;

	for (retVal = listOfOptIROps.begin(); retVal != listOfOptIROps.end(); retVal++)
	{
		if (*retVal == op)
			return retVal;
	}
	return retVal;
}

void setAsDoneOpt(BaseOperation *op)
{
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfOptIROps.begin(); iter != listOfOptIROps.end(); iter++)
	{
		if (*iter == op)
		{
			iter->setFlg(_YES_);
			return;
		}
	}

	// error condition.
	//
	assert(0);
}

void setAsDoneOpt(ISelIROpNode & op)
{
	op.setFlg(_YES_);
}

void setAsDoneOpt(OpList & oList)
{
	// iterate over list of ops.
	//
	OpListIter i;
	for (i = oList.begin(); i != oList.end(); i++)
	{
		setAsDoneOpt(*i);
	}
}

int doneOpt(BaseOperation *op)
{
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfOptIROps.begin(); iter != listOfOptIROps.end(); iter++)
	{
		if (*iter == op)
		{
			if (iter->getFlg() == _YES_)
				return _YES_;
			break;
		}
	}
	return _NO_;
}

void resetOptFlg()
{
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfOptIROps.begin(); iter != listOfOptIROps.end(); iter++)
		iter->setFlg(_INVALID_);
}

// return _YES_ if operation is present in listOfISelIROps
// else return _NO_

int isPresentInOptList(BaseOperation *op)
{
	// iterate over listOfISelIROps.
	//
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfOptIROps.begin(); iter != listOfOptIROps.end(); iter++)
	{
		if (*iter == op)
			return _YES_;
	}
	return _NO_;
}

BaseInstruction *getOptInstrCorrespondingToOper(BaseOperation *op)
{
	// iterate over listOfISelIROps.
	//
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfOptIROps.begin(); iter != listOfOptIROps.end(); iter++)
	{
		if (*iter == op)
			return iter->getInstr();
	}
	return NULL;
}


void addOpsToOptList(BaseInstruction *instr)
{
	ISelIROpNode temp;
	temp.setInstr(instr);

	// iterate over the instruction and add each op to listOfISelIROps
	//
	ArrayIterator<DataOpSlot> *diter;
	diter = ((NormalInstr *)instr)->getDataOpSlotIter();

	while (diter->hasMoreElements())
	{
		if ((diter->currentElement()).hasOper())
		{
			temp.setOp((diter->currentElement()).getPtrToOperation());
			listOfOptIROps.push_back(temp);
		}
		diter->nextElement();
	}
	delete diter;

	ArrayIterator<ControlOpSlot> *citer;
	citer = ((NormalInstr *)instr)->getControlOpSlotIter();

	while (citer->hasMoreElements())
	{
		if ((citer->currentElement()).hasOper())
		{
			temp.setOp((citer->currentElement()).getPtrToOperation());
			listOfOptIROps.push_back(temp);
		}
		citer->nextElement();
	}
	delete citer;
}

// use this function to build a list of ops (belonging to the bb)
// in sequential order.
// i.e location(op1) = N, location(op2) = N+1, ...
//
// dir == 1 => go down the list, dir == 0 => go up the list
//
int buildListOfOptOpsSequential(BaseOperation *op, OpList & seqList, int size, int dir)
{
	BaseOperation *currOp = op;

	Vector<ISelIROpNode>::iterator i = findOperFromListOfOptIROps(currOp);

	if (i == listOfOptIROps.end()) return _NO_;


	int count = 0;

	if (dir == 1)
	{
		for (i; (i != listOfOptIROps.end() && count < size); i++, count++)
		{
			if (i->getFlg() == _YES_) // found an op in sequence that was already converted.
			{
				// as of now, in this situation, return _NO_.
				//
				return _NO_;
			}
			seqList.push_back((*i).getOp());
		}
	}
	else // if (dir == 0)
	{
		count++;
		if (i != listOfOptIROps.begin())
		{
			do
			{
				if (count >= size)
					break;

				i--;
				count++;
			}
			while (i != listOfOptIROps.begin());
		}

		if (count == size)
		{
			for (int k = 0; k < size; k++)
			{
				if (i->getFlg() == _YES_) // found an op in sequence that was already converted.
				{
					// as of now, in this situation, return _NO_.
					//
					return _NO_;
				}
				seqList.push_back((*i).getOp());
				i++;
			}
		}
	}

	if (count < size)
		return _NO_;
	return _YES_;
}

inline void createEntriesInOpList(OpList & oList, int size)
{
	for (int i = 0; i < size; i++)
		oList.push_back(NULL);
}

void addNodeToOpList(OpList & oList, BaseOperation *op, int opPos)
{
	// If oList[opPos] != NULL, flag error.
	// also, 
	// TBD: Check if op is not already present in oList.

	if (oList[opPos] == NULL)
		oList[opPos] = op;
	else
	{
		// error.
		assert(0);
	}
}

void markOpAsChecked(Vector<int> & checkList, BaseOperation *currOp, int opPos)
{
	checkList.push_back(opPos);
}

int getOpPosOfNextUnCheckedOp(Vector<int> & checkList, OpList & oList)
{
	// iterate over oList. If any entry is non-NULL, return it if its pos does not exist in checkList.
	//
	int pos = 0;
	for (OpListIter i = oList.begin(); i != oList.end(); i++)
	{
		if (*i != NULL)
		{
			int hasBeenChecked = 0;
			for (Vector<int>::iterator j = checkList.begin(); j != checkList.end(); j++)
			{
				if (pos == *j) // this position has already been marked as checked.
					hasBeenChecked = 1;
			}
			if (!hasBeenChecked) // found an unchecked operation
				return pos;
		}
	}
	return _INVALID_;
}

BaseOperation *getNextUnCheckedOp(OpList & oList, int opPos)
{
	if (opPos == _INVALID_) return NULL;

	return oList[opPos];
}

BaseOperation *getMatchingDefOper(BaseArgument *arg, int defOpPos, ISelOpListType *rule)
{
	BaseOperation *retOp = NULL;
	BaseArgProperty *prop;

	if ((prop = arg->getPtrToProperty(_UDCHAIN_)) != NULL)
	{
		// the operation present in the def list that is:
		//
		// 1) also present in the BB, and
		// 2) mathces the operation in rule[defOpPos]
		//
		// is chosen as the matching definition.
		//

		MyLinkedListIterator<BaseOperation *> *iter = ((UDChainProperty *)prop)->defListIteratorForw();

		while (iter->hasMoreElements())
		{
			BaseOperation *toper = iter->currentElement();

			if ((isPresentInOptList(toper) == _YES_) && (matchesOpInRule(toper, defOpPos, rule) == _YES_))
			{
				retOp = toper;
				break;
			}
			iter->nextElement();
		}
		delete iter;
	}
	return retOp;
}

// Use this function to build a list of ops (belonging to the bb)
// based on the dependence tree structure.
// Assume that the input op is at the root of the tree.
//
int buildTreeOfOptOpsFromDepChains(BaseOperation *op, OpList & treeList, int ruleIndex)
{
	int tempPos;
	Vector<int> markAsCheckedList;

	ISelOpListType *thisRule = lhsRules[ruleIndex];

	// First, create "size" number of entries in treeList.
	//
	createEntriesInOpList(treeList, numOps(thisRule));
	
	BaseOperation *currOp = op;
	int opPos = getRootTreePos(thisRule);

	// Add the root node (in its position in the rule) to treeList.
	//
	addNodeToOpList(treeList, currOp, opPos);
	
	ISelArgListType::iterator i1, i2;
	while (currOp != NULL)
	{
		i1 = i2 = NULL;

		if (sourceList(thisRule, opPos) != NULL)
		{
			i1 = sourceList(thisRule, opPos)->begin();
			i2 = sourceList(thisRule, opPos)->end();
		}

		while (i1 != i2)
		{
			if ((tempPos = hasDefInRuleTree(*i1, thisRule)) != _INVALID_)
			{
				// get the source number.
				//
				int srcNum = getIndex(i1->getArgNum(), _SRC_LIST_);
				BaseArgument *sarg = currOp->ptrToOperand(srcNum, _SRC_LIST_);
				BaseOperation *tempOper = getMatchingDefOper(sarg, tempPos, thisRule);

				if (tempOper == NULL) // no match.
					return _NO_;

				addNodeToOpList(treeList, tempOper, tempPos);
			}
			i1++;
		}

		markOpAsChecked(markAsCheckedList, currOp, opPos);

		opPos = getOpPosOfNextUnCheckedOp(markAsCheckedList, treeList);
		currOp = getNextUnCheckedOp(treeList, opPos);
	}

	// finally, check to see that treeList does not contain any NULL entries.
	//
	OpListIter i;
	for (i = treeList.begin(); i != treeList.end(); i++)
		if (*i == NULL) 
			return _NO_;

	return _YES_;
}

extern int equalRegs(BaseArgument *reg1, BaseArgument *reg2);
extern void transferDUUDChains(BaseArgument *fromArg, BaseOperation *fromOp, BaseArgument *toArg, BaseOperation *toOp, int deleteFrom);
extern void transferUDDUChains(BaseArgument *fromArg, BaseOperation *fromOp, BaseArgument *toArg, BaseOperation *toOp, int deleteFrom);
extern void addUDDUChains(BaseArgument *fromArg, BaseOperation *fromOp, BaseArgument *toArg, BaseOperation *toOp);

// argIdx is the index of the map.
// create a tuple with the rest of the arguments to this function.
//
void addTupleToMap(ArgTuplesMap & aMap, int argIdx, int genericOrTarget, int opPos, int dstOrSrc, int argNum)
{
	// create a tuple.
	//
	ArgTuple newTuple;
	newTuple._genericOrTarget = genericOrTarget;
	newTuple._opPos = opPos;
	newTuple._dstOrSrc = dstOrSrc;
	newTuple._argNum = argNum;

	// add the tuple to the map.
	//
	(aMap[argIdx]).push_back(newTuple);
}

void createMapOfArgTuples(ISelOpListType *rule, ArgTuplesMap & aMap)
{
	ISelOpListType::iterator li;
	ISelOp *tempOp;

	int opPos;
	ISelArgListType::iterator tempArgI;

	for (li = rule->begin(), opPos = 0; li != rule->end(); li++, opPos++)
	{
		tempOp = *li;

		ISelArgListType *dargList = tempOp->getDstList();

		if (dargList != NULL)
		{
			// iterate over dargList.
			//
			for (tempArgI = dargList->begin(); tempArgI != dargList->end(); tempArgI++)
			{
				if (!(tempArgI->getOperand()).isRegType())
					continue;

				int argIdx = (tempArgI->getArgIdent()).getIdent();
				addTupleToMap(aMap, argIdx, _GENERIC_, opPos, _DEST_LIST_, tempArgI->getArgNum());
			}
		}

		ISelArgListType *sargList = tempOp->getSrcList();

		if (sargList != NULL)
		{
			// iterate over sargList.
			//
			for (tempArgI = sargList->begin(); tempArgI != sargList->end(); tempArgI++)
			{
				if (!(tempArgI->getOperand()).isRegType())
					continue;

				int argIdx = (tempArgI->getArgIdent()).getIdent();
				addTupleToMap(aMap, argIdx, _GENERIC_, opPos, _SRC_LIST_, tempArgI->getArgNum());
			}
		}
	}
}

void getArgAndOperCorrespToArgTuple(OpList & aList, ArgTupleList & args, int dstOrSrc, ArgAndOper & ao)
{
	// find an arg in args that corresponds to dstOrSrc
	//
	ArgTupleList::iterator i;

	for (i = args.begin(); i != args.end(); i++)
	{
		if (i->_dstOrSrc == dstOrSrc)
		{
			BaseOperation *op = aList[i->_opPos];
			BaseArgument *arg = op->ptrToOperand(getIndex(i->_argNum, dstOrSrc), dstOrSrc);

			ao._arg = arg;
			ao._op = op;

			break;
		}
	}
}

BaseArgument *getArgCorrespToTuple(OpList & aList, ArgTuple & at)
{
	BaseArgument *retPtr = NULL;

	retPtr = (aList[at._opPos])->ptrToOperand(getIndex(at._argNum, at._dstOrSrc), at._dstOrSrc);

	return retPtr;
}

BaseOperation *getOperCorrespToTuple(OpList & aList, ArgTuple & at)
{
	BaseOperation *retPtr = NULL;

	retPtr = aList[at._opPos];

	return retPtr;
}

extern void deleteDefUsesFromTo(BaseOperation *fromOp, BaseOperation *toOp, BaseArgument *refArg);
extern void deleteUseDefsFromTo(BaseOperation *fromOp, BaseOperation *toOp, BaseArgument *refArg);

extern void addUDDUChains(BaseOperation *useOp, BaseOperation *defOp, BaseArgument *refArg);

void transferUniqUDDUChains(BaseOperation *doNotMatchThisOp, BaseArgument *fromArg, BaseOperation *fromOp, BaseArgument *toArg, BaseOperation *toOp)
{
	// for each UD chain from fromArg, if the op is not "doNotMatchThisOp" then add this UD chain to toArg.
	//
	// This is done by first just adding all UD/DU chains and then using "deleteDefUsesFromTo" to delete all use/defs to "doNotMatchThisOp"
	//
	transferUDDUChains(fromArg, fromOp, toArg, toOp, 0);
	deleteDefUsesFromTo(doNotMatchThisOp, toOp, toArg);
	deleteUseDefsFromTo(toOp, doNotMatchThisOp, toArg);
}

void resetUDDUChains_internal1(OpList & gList, ArgTupleList & gArgs, OpList & tList, ArgTupleList & tArgs)
{
	int hasDest = 0, hasSrc = 0;
	ArgTupleList::iterator i;

	ArgAndOper gdao, gsao, tdao, tsao;
	gdao._arg = gsao._arg = tdao._arg = tsao._arg = NULL;

	getArgAndOperCorrespToArgTuple(gList, gArgs, _DEST_LIST_, gdao);
	getArgAndOperCorrespToArgTuple(gList, gArgs, _SRC_LIST_, gsao);
	getArgAndOperCorrespToArgTuple(tList, tArgs, _DEST_LIST_, tdao);
	getArgAndOperCorrespToArgTuple(tList, tArgs, _SRC_LIST_, tsao);

	BaseArgument *thisArg;
	BaseOperation *thisOper;

	for (i = tArgs.begin(); i != tArgs.end(); i++)
	{
		thisArg = getArgCorrespToTuple(tList, *i);
		thisOper = getOperCorrespToTuple(tList, *i);

		if (i->_dstOrSrc == _DEST_LIST_)
		{

			if (gdao._arg != NULL)
				transferDUUDChains(gdao._arg, gdao._op, thisArg, thisOper, 1);
			else 
			{
				// as of now, this is an error condition.
				// TBD: for each source in gList corresponding to gArgs, add a DU (and UD) chain from thisArg to the source
				//
				assert(0);
			}
		}
		else // if (i->_dstOrSrc == _SRC_LIST_)
		{
			if ((gsao._arg != NULL) && (gdao._arg != NULL))
				transferUniqUDDUChains(gdao._op, gsao._arg, gsao._op, thisArg, thisOper);
			else if (gsao._arg != NULL)
				transferUDDUChains(gsao._arg, gsao._op, thisArg, thisOper, 0);

			if (tdao._arg != NULL)
				addUDDUChains(thisOper, tdao._op, thisArg);
		}
	}

}

// Adds UD/DU chains between args in "args"
//
// If "args" size > 1, and if it contains atleast 1 dst arg and atleast 1 src arg,
// this function adds UD/DU chains between all srcs and dsts.
//
void resetUDDUChains_internal2(OpList & aList, ArgTupleList & args)
{
	ArgTupleList::iterator i, j;

	BaseArgument *dstArg, *srcArg;
	BaseOperation *dstOper, *srcOper;

	if (args.size() < 1) return;

	for (i = args.begin(); i != args.end(); i++)
	{
		if (i->_dstOrSrc == _SRC_LIST_)
		{
			srcArg = getArgCorrespToTuple(aList, *i);
			srcOper = getOperCorrespToTuple(aList, *i);

			for (j = args.begin(); j != args.end(); j++)
			{
				if (j->_dstOrSrc == _DEST_LIST_)
				{
					dstArg = getArgCorrespToTuple(aList, *j);
					dstOper = getOperCorrespToTuple(aList, *j);

					addUDDUChains(srcArg, srcOper, dstArg, dstOper);
				}
			}
		}
	}
}

void resetUDDUChains(OpList & gList, OpList & tList, int ruleIndex)
{
	// assume that gList & tList match the rule specified by ruleIndex.
	//
	// Now, we need to detach the gList ops from the IR and insert the tList ops into the IR.
	// So, we remove the D-U chains of the destinations in gList and attach them to tList destinations.
	// Similarly with the sources (except we don't remove the chains from the gList ops).
	// However, if in the gList, a DU/UD chain existed between two ops in the gList, need to make the corresponding
	// DU/UD chain in the tList.
	//

	// For the rule, create a list of the register arg indices.
	// For each arg index, create a list of tupes (<generic_or_target>, <op_pos>, <dst_or_src> <arg_pos>)
	//
	// Note: Due to the way gList and tList are created, we are guaranteed that the ops in these lists are exactly in the order in which
	// they are specified in the rule. (For ex. a tree : a -> b <- c (i.e. b is the root) can be specified as a, c, b or c, a, b).
	//

	ArgTuplesMap lhsArgIndexTuples, rhsArgIndexTuples;

	ISelOpListType *lRule = lhsRules[ruleIndex];
	ISelOpListType *rRule = rhsRules[ruleIndex];

	// First create the map for lhs.
	//
	createMapOfArgTuples(lRule, lhsArgIndexTuples);

	// Next, create the map for rhs
	//
	createMapOfArgTuples(rRule, rhsArgIndexTuples);

	// Now, that we have the maps, go thru the rhsArgIndexTuples, and for each argument, add the du/ud chains from
	// the corresponding lhsArgIndexTuples.
	//
	ArgTuplesMap::iterator atkr, atkl;
	// BaseArgument *targ;
	for (atkr = rhsArgIndexTuples.begin(); atkr != rhsArgIndexTuples.end(); atkr++)
	{
		int argIdx = atkr->first;

		if ((atkl = lhsArgIndexTuples.find(argIdx)) != lhsArgIndexTuples.end()) // i.e. match on lhs
			resetUDDUChains_internal1(gList, atkl->second, tList, atkr->second);
		else
		{
			// no match on lhs, but there might be a ud-du match on the rhs itself.
			//
			// e.g. : rhs : op1 DST[1] = REG (3) ...
			//            : op2 .... SRC[1] = REG(3) ...
			//
			resetUDDUChains_internal2(tList, atkr->second);
		}
			
	}
}

// This function is an earlier implementation.
//
// The function above is currently being used in TransMutations.
//
// Note: The function below assumes that each definition is unique. 
// Thus, this works only if it is called before Register Allocation.
// Use the function above, which is more recent, and only sets UDDU chains based on the rules.
//
// Note: This works when all the UDDU chains are within the basicblock.
//
// The case:
//
//  BB 1 :
//         <op> <X_dest> ....
//         ......
//
//  BB 2 :
//         <op> <Y_dest> <K_src> ...
//         ......
//
//         <op> <Z_dest> <src> ...
//         ....
//
// where  BB2 is a loop and K_src has UD chains to X_dest and Z_dest
// when translated to target ops, will only have a UD chain to Z_dest.
//
void resetUDDUChains(OpList & gList, OpList & tList)
{
	// go thru each operation in gList. For each source argument that is also a source in
	// tList, add its u-d chain to the tList ops and modify the def's d-u chains to include the 
	// tList ops.
	//
	// For each destination argument in gList that is also a destination in tList, add its
	// d-u chain to the tList op and remove the d-u chain from the gList op.
	// Also, modify the use's u-d chain to point to the tList op.

	OpListIter gI, tI, ttI;
	// OpListIter ggI;
	ArgListIter dgI, dtI, sgI, stI;

	// first, reset the DUUD chains of the destinations that appear in both gList and tList.
	//
	// for each op (gO) in gList,
	//     for each dest arg (gdA) in gO
	//         for each op (tO) in tList
	//             for each dest arg (tdA) in tO
	//                 if (gdA == tdA) replace DU and UD chains
	//
	for (gI = gList.begin(); gI != gList.end(); gI++)
	{
		ArgList & dgl = (*gI)->destOperandList();
		for (dgI = dgl.begin(); dgI != dgl.end(); dgI++)
		{
			if (*dgI == NULL) continue;
			int done = 0;
			
			for (tI = tList.begin(); tI != tList.end(); tI++)
			{
				ArgList & dtl = (*tI)->destOperandList();
				for (dtI = dtl.begin(); dtI != dtl.end(); dtI++)
				{
					if (*dtI == NULL) continue;

					if (equalRegs(*dgI, *dtI))
					{
						transferDUUDChains(*dgI, *gI, *dtI, *tI, 1);
						done = 1;
						break;
					}
				}
				if (done) break;
			}
		}
	}

	// next, set the source UD chains of the tList.
	//
	for (tI = tList.begin(); tI != tList.end(); tI++)
	{
		ArgList & stl = (*tI)->sourceOperandList();
		for (stI = stl.begin(); stI != stl.end(); stI++)
		{
			if (*stI == NULL) continue;

			int done = 0;
			
			for (ttI = tList.begin(); ttI != tList.end(); ttI++)
			{
				if (*ttI == *tI) continue; // same operation
				//
				// Note: This is wrong if the operations are inside a loop.

				ArgList & dtl = (*tI)->destOperandList();
				for (dtI = dtl.begin(); dtI != dtl.end(); dtI++)
				{
					if (*dtI == NULL) continue;

					if (equalRegs(*stI, *dtI))
					{
						addUDDUChains(*stI, *tI, *dtI, *ttI);
						done = 1;
						break;
					}
				}
				if (done) break;
			}

			if (done) continue;

			for (gI = gList.begin(); gI != gList.end(); gI++)
			{
				ArgList & dgl = (*gI)->destOperandList();
				for (dgI = dgl.begin(); dgI != dgl.end(); dgI++)
				{
					if (*dgI == NULL) continue;

					if (equalRegs(*stI, *dgI))
					{
						addUDDUChains(*stI, *tI, *dgI, *gI);
						done = 1;
						break;
					}
				}
				if (done) break;
			}

			if (done) continue;

			// Note: This cannot be combined with the gList iteration above.
			//
			for (gI = gList.begin(); gI != gList.end(); gI++)
			{
				ArgList & sgl = (*gI)->sourceOperandList();
				for (sgI = sgl.begin(); sgI != sgl.end(); sgI++)
				{
					if (*sgI == NULL) continue;

					if (equalRegs(*stI, *sgI))
					{
						transferUDDUChains(*sgI, *gI, *stI, *tI, 0);
						done = 1;
						break;
					}
				}
				if (done) break;
			}
		}
	}
}

void printOptList()
{
	cout << "Printing OptIRList: " << endl;

	// iterate over listOfISelIROps.
	//
	Vector<ISelIROpNode>::iterator iter;

	for (iter = listOfOptIROps.begin(); iter != listOfOptIROps.end(); iter++)
	{
		(iter->getOp())->shortPrint(cout);
		cout << endl;
	}
}

void initOpt()
{
    try{
           FilePointer fp("OptMapping.txt", "r");
	       Optrestart(fp.file_ptr());
	       Optparse();
	}
    catch (CouldNotOpenFileException e)
    {
      cout << "Panic: Could Not Open File" << endl;
      assert(0);
    }
}

