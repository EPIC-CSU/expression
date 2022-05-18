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
 *file  : IssueUnit.cpp										 *
 *created : 3 May, 2000.                                     *
 *authors : Prabhat, Srikanth                                *
 *last modified :                                            *
 *                                                           *
 *contents:contains class methods for Unit class & other     *
 *         classes derived from it.                          *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#include "IssueUnit.h"
#include "DerivedUnit.h"
#include "ControlTable.h"
#include "DerivedStorage.h"

extern Storage *GPRFile;
extern Storage *FPRFile;
extern Storage *MiscRegFile;

BusyRegsList regsBusyInCurrCycle;

int isBusyInCurrCycle(AddrOperand addr, MyBusyElem & elem)
{
	BusyRegsListIter i1;
	for (i1 = regsBusyInCurrCycle.begin(); i1 != regsBusyInCurrCycle.end(); i1++)
	{
		if (addr == (*i1)._addr)
		{
			elem = (*i1)._elem;
			return _YES_;
		}
	}
	return _NO_;
}

void markDestBusyInCurrCycle(AddrOperand & addr, MyBusyElem & elem)
{
	BusyRegsElem tmp;
	tmp._addr = addr;
	tmp._elem = elem;

	regsBusyInCurrCycle.push_back(tmp);
}

// Pseudo code for Issue Unit.

/*

while (canIssue())
	oper = getOneOper();

	getUnitsToExecuteThisOper(oper, listOfFUs);

	eliminateBusyUnits(listOfFUs);

	foreach
		src = getSrcOperand(oper);
		readSrcFromRegFile(busybit, src);
		
		if (busybit)
			getListOfPossibleDests(src, listOfDests);
			if (noPathExists(listOfDests, listOfFUs)
				doNotIssue = 1;
			else
				eliminateIncompatibleFUs(listOfDests, listOfFUs);

	if (!doNotIssue)
		unitIssued = issueOper(oper, listOfFUs);
		changeIssueStatus(oper, unitIssued);
	else

*/

void FUandRFConnTable::addFUToMap(Unit *unit)
{
	// We do not perform any error checking.

	_fuToIntMap[unit] = _tempIndex;
	_tempIndex++;
}

void FUandRFConnTable::addRFToMap(Storage *stor)
{
	// We do not perform any error checking.

	_rfToIntMap[stor] = _tempIndex;
	_tempIndex++;
}

void FUandRFConnTable::createConnTable()
{
	// _tempIndex gives us the number of FUs+RFs that need entries in the table.

	FUandRFConnRow row;
	FUandRFConnTableEntry ent;
	ent._hasPath = _NO_;

	for (int i = 0; i < _tempIndex; i++)
	{
		_fuAndRFConnTable.push_back(row);

		FUandRFConnRow & tmpRow = _fuAndRFConnTable.back();
		for (int j = 0; j < _tempIndex; j++)
		{
			tmpRow.push_back(ent);
		}
	}
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
void FUandRFConnTable::addPath(int fromIndex, int toIndex)
{
	// Note: We do not perform any error checking.
	//
	((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath = _YES_;
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
void FUandRFConnTable::addPath(Unit *fromU, Unit *toU)
{
	int fromIndex, toIndex;

	fromIndex = _fuToIntMap[fromU];
	toIndex = _fuToIntMap[toU];

	((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath = _YES_;
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
void FUandRFConnTable::addPath(Unit *fromU, Storage *toS)
{
	int fromIndex, toIndex;

	fromIndex = _fuToIntMap[fromU];
	toIndex = _rfToIntMap[toS];

	((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath = _YES_;
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
void FUandRFConnTable::addPath(Storage *fromS, Unit *toU)
{
	int fromIndex, toIndex;

	fromIndex = _rfToIntMap[fromS];
	toIndex = _fuToIntMap[toU];

	((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath = _YES_;
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
void FUandRFConnTable::addPath(Storage *fromS, Storage *toS)
{
	int fromIndex, toIndex;

	fromIndex = _rfToIntMap[fromS];
	toIndex = _rfToIntMap[toS];

	((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath = _YES_;
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
int FUandRFConnTable::hasPath(int fromIndex, int toIndex)
{
	// Note: We do not perform any error checking.
	//
	return (((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath);
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
int FUandRFConnTable::hasPath(Unit *fromU, Unit *toU)
{
	int fromIndex, toIndex;

	fromIndex = _fuToIntMap[fromU];
	toIndex = _fuToIntMap[toU];

	return (((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath);
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
int FUandRFConnTable::hasPath(Unit *fromU, Storage *toS)
{
	int fromIndex, toIndex;

	fromIndex = _fuToIntMap[fromU];
	toIndex = _rfToIntMap[toS];

	return (((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath);
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
int FUandRFConnTable::hasPath(Storage *fromS, Unit *toU)
{
	int fromIndex, toIndex;

	fromIndex = _rfToIntMap[fromS];
	toIndex = _fuToIntMap[toU];

	return (((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath);
}

// Note: fromIndex is the column index, while toIndex is the row index.
//
int FUandRFConnTable::hasPath(Storage *fromS, Storage *toS)
{
	int fromIndex, toIndex;

	fromIndex = _rfToIntMap[fromS];
	toIndex = _rfToIntMap[toS];

	return (((_fuAndRFConnTable[toIndex])[fromIndex])._hasPath);
}

// Instantiate the globalFUandRFConnTable
//
FUandRFConnTable globalFUandRFConnTable;

#ifdef ISSUE_FOR_PIPELINE_EXPLORATION

void IssueUnit::initializeIssueUnitDS()
{
	// Set the number of operations issued in current cycle to 0
	//
	_numIssuedInCurrCycle = 0;

	// Set the flag for every execution unit (FU) to has-not-been-issued-an-oper (INIT_FULISTELEM)
	//
	FUListIter i;
	for (i = globalFUList.begin(); i != globalFUList.end(); i++)
	{
		(*i)._flag = INIT_FULISTELEM;
	}

	// Reset all operations in issue buffer as OPER_INIT.
	// This is a new cycle, a new beginning for all the operations in the issue buffer.
	// Perhaps they will be lucky this cycle, who knows?

	IssueBufferDSIter i1;
	
	for (i1 = IssueBuffer.begin(); i1 != IssueBuffer.end(); i1++)
	{
		(*i1)._status = OPER_INIT;
	}

	// Now, reset the regsBusyInCurrCycle DS.
	//
	regsBusyInCurrCycle.clear();
}

int IssueUnit::_hasBeenIssued(IssueBufferDSIter i)
{
	if ((*i)._status == OPER_ISSUED)
		return _YES_;
	return _NO_;
}

int IssueUnit::_doNotIssue(IssueBufferDSIter i)
{
	if ((*i)._status == OPER_STALLED)
		return _YES_;
	return _NO_;
}

int IssueUnit::_getAnOperation(ThreeOpLatchData & oper)
{
	// SAH : 05/22/2000
	// Currently, we look only at the top operation in the buffer.

	// First, check to see if we have already issued the max. allowable number of operations per cycle

	if (_numIssuedInCurrCycle >= NumInstIssue) return _INVALID_;

	// Now, get the topmost non-issued operation in the buffer

	if (IssueBuffer.empty()) return _INVALID_;

	IssueBufferDSIter i1;
	int count = 0;

	for (i1 = IssueBuffer.begin(); i1 != IssueBuffer.end(); i1++, count++)
	{
		if (_hasBeenIssued(i1)) continue;

		// Now, determine if this operation can be issued.
		// An operation can be issued if:
		// 1) The operation has not been looked at previously (in the same cycle) and
		// 2) It does not depend on other operations.
		//
		// SAH : 05/22/2000 : Note that as we are looking only at the topmost operation,
		// we currently do not need to check for condition 2.

		if (_doNotIssue(i1)) return _INVALID_;

		// This operation can be issued, so return count (i.e. the index in the buffer).
		//
		oper = (*i1)._oper;
		return count;
	}
	return _INVALID_;
}

void IssueUnit::_getListOfCandidateFUsForOper(ThreeOpLatchData & oper, ListOfFUIndicesDS & listOfFUs)
{
	// First, get the opcode of the operation

	int opcode = oper.getOpCode();

	// Now, iterate over the list of FUs,and determine which ones can execute this operation.

	for (int i = 0; i < globalFUList.size(); i++)
	{
		if ((((globalFUList[i])._unit)->getOpList()).hasOpCode(opcode))
		{
			// enter this FU into the listOfFUs.
			listOfFUs.push_back(i);
		}
	}
}

int IssueUnit::_unitIsBusy(int unitIndex)
{
	// First get the pointer to the unit from the unitIndex.

	Unit *unit = globalFUList[unitIndex]._unit;

	// Now, look at the control table to determine if the unit is supposed to be busy this cycle.
	int isBusy = globalControlTable.getProperty(unit, BusyBit);

	return isBusy;
}

int IssueUnit::_operHasBeenIssuedToUnit(int unitIndex)
{
	// Check the hasBeenIssuedAnOper table.

	if ((globalFUList[unitIndex])._flag == HAS_BEEN_ISSUED_AN_OPER) return _YES_;
	return _NO_;
}

void IssueUnit::_eliminateBusyFUs(ListOfFUIndicesDS & listOfFUs)
{
	// Candidate FUs can be eliminated for two reasons:
	//
	// 1) They are currently busy,
	// 2) An operation has already been issued to that FU.

	// First, iterator over the listOfFUs and determine if that FU is busy or not.

	// We maintain a list of non-busy FUs.

	ListOfFUIndicesDS notBusyFUs;
	ListOfFUIndicesDSIter i;

	for (i = listOfFUs.begin(); i != listOfFUs.end(); i++)
	{
			
		// Handling reason 1:
		if (_unitIsBusy(*i))
			continue;

		// Handling reason 2:
		if (_operHasBeenIssuedToUnit(*i))
			continue;

		notBusyFUs.push_back(*i);
	}

	listOfFUs.clear();

	// Now, transfer the list of non-busy FUs to listOfFUs.

	for (i = notBusyFUs.begin(); i != notBusyFUs.end(); i++)
	{
		listOfFUs.push_back(*i);
	}
}

MyBusyElem IssueUnit::_determineIfSrcBusy(AddrDataUnion & s)
{
	MyBusyElem srcbusy;
	AddrOperand src;

	if (s.getType() == _ADDRTYPE_)
	{
		src = s.getAddr();

		// first check currMiscBusyRegs and currGPBusyRegs
		//
		if (isBusyInCurrCycle(src, srcbusy))
			return srcbusy;
		
		if (src.getFieldType() == _ADDR_MISC_)
			MiscBusyRegTable->read(srcbusy,src);
		else
			GPBusyRegTable->read(srcbusy,src);
	}

	
	return srcbusy;
}

void IssueUnit::_readSrc(AddrDataUnion & s, Val & v)
{
	AddrOperand src;
	if (s.getType() == _ADDRTYPE_)
	{
		src = s.getAddr();
		if (src.getFieldType() == _ADDR_MISC_)
			PORT_ELEM(2)->read(v,src); // Note: PORT_ELEM(2) is the MiscRegFIle.
		else
			operandRead(v, s, PORT_ELEM(0));
	} 
	else	
	{
		operandRead(v, s, PORT_ELEM(0));
	}
}

void IssueUnit::getListOfPossibleDests(AddrDataUnion & s, ListOfCompIndicesDS & listOfDests)
{
	AddrOperand src;
	MyBusyElem srcbusy;

	if (s.getType() == _ADDRTYPE_)
	{
		src = s.getAddr();
		
		if (src.getFieldType() == _ADDR_MISC_)
		{
			MiscBusyRegTable->read(srcbusy,src);

			// Now, add it to the list of dests.
			//
			if (srcbusy._fu != _INVALID_)
				listOfDests.push_back(srcbusy._fu);

/*****************************************************************************/
/*** SAH, 05/26/2000
 ***
 *** I commented out this section of the code becos of errors in accessing the globalFUandRFConnTable.
 *** While building this table, the RFs are not added as of now (becos FUs do not connect to RFs).
 *** However, here we try to access the index for a RF and this creates an error.
 ***
 *** 
			// Also, push the reg file in the list of dests.
			//
			int regFileID = globalFUandRFConnTable.getIndex(MiscRegFile);
			listOfDests.push_back(regFileID);

  ***
  ***
  ***/
		}
		else
		{
			GPBusyRegTable->read(srcbusy,src);

			// Now, add it to the list of dests.
			//
			if (srcbusy._fu != _INVALID_)
				listOfDests.push_back(srcbusy._fu);

/*****************************************************************************/
/*** SAH, 05/26/2000
 ***
 *** I commented out this section of the code becos of errors in accessing the globalFUandRFConnTable.
 *** While building this table, the RFs are not added as of now (becos FUs do not connect to RFs).
 *** However, here we try to access the index for a RF and this creates an error.
 ***
 *** 
			// Also, push the reg file in the list of dests.
			//
			int regFileID = globalFUandRFConnTable.getIndex(GPRegFile);
			listOfDests.push_back(regFileID);

  ***
  ***
  ***/
		}
	}
}

void IssueUnit::eliminateNoPathFUs(ListOfCompIndicesDS & listOfDests, ListOfFUIndicesDS & listOfFUs)
{
	// check to see if there is a path from any of the list of dests to any of the list of fus.
	//

	int toIndex, fromIndex;

	ListOfFUIndicesDSIter i1, i2, k1, k2;
	i2 = listOfFUs.end();

	ListOfCompIndicesDSIter j1, j2;
	j2 = listOfDests.end();

	ListOfFUIndicesDS tempList;
	int hasPath;

	for (i1 = listOfFUs.begin(); i1 != i2; i1++)
	{
		toIndex = *i1;

		hasPath = 0;
		for (j1 = listOfDests.begin(); j1 != j2; j1++)
		{
			fromIndex = *j1;

			if (globalFUandRFConnTable.hasPath(fromIndex, toIndex))
			{
				hasPath = 1;
				break;
			}
		}
		if (hasPath)
		{
			tempList.push_back(toIndex);
		}
	}
	// finally copy tempList to listOfFUs;
	//
	listOfFUs.clear();
	k2 = tempList.end();
	for (k1 = tempList.begin(); k1 != k2; k1++)
	{
		listOfFUs.push_back(*k1);
	}
}

void IssueUnit::markAsNotIssued(int issueBufferIndex)
{
	// Set the bit in IssueBuffer to OPER_STALLED

	(IssueBuffer[issueBufferIndex])._status = OPER_STALLED;
}

int IssueUnit::getBestFU(ListOfFUIndicesDS & listOfFUs, int issueBufferIndex)
{
	// Current heuristic : Simple.
	// Return the first element in the list.

	return listOfFUs.front();
}

void IssueUnit::markAsIssued(int issueBufferIndex, int fuIndex)
{
	// Set the bit in IssueBuffer to OPER_ISSUED

	(IssueBuffer[issueBufferIndex])._status = OPER_ISSUED;

	// Set the bit in globalFUList to HAS_BEEN_ISSUED_AN_OPER

	(globalFUList[fuIndex])._flag = HAS_BEEN_ISSUED_AN_OPER;

	// Increment the number of operations issued in the current cycle

	_numIssuedInCurrCycle++;
}

void IssueUnit::eraseAllIssuedOpers()
{
	// iterate over the issue buffer.
	//
	IssueBufferDSIter i1, i2, i3;
	
	i1 = IssueBuffer.begin(); 
	i2 = IssueBuffer.end();

	int IBsize = IssueBuffer.size();
	int numDeleted = 0;

	// Store all the non-issued ops in a temporary list
	// Then, erase IssueBuffer, and fill it with only the non-issued ops (from the temporary list).
	//
	IssueBufferDS tempList;
	while (i1 != i2)
	{
		if ((*i1)._status != OPER_ISSUED)
		{
			tempList.push_back(*i1);
		}
		i1++;
	}
	IssueBuffer.clear();
	for (i1 = tempList.begin(); i1 != tempList.end(); i1++)
	{
		IssueBuffer.push_back(*i1);
	}
}

void IssueUnit::setDestRegToBusy(AddrOperand & dest, int fuIndex)
{
	MyBusyElem destbusy;
	destbusy._fu = fuIndex;
	destbusy._id = true;

	markDestBusyInCurrCycle(dest, destbusy);

/*
	if (dest.getFieldType() == _ADDR_MISC_)
	{
		MiscBusyRegTable->write(destbusy,dest);
//		MiscBusyRegTable->doUpdate();
	}
	else
	{
		GPBusyRegTable->write(destbusy,dest);
//		GPBusyRegTable->doUpdate();
	}
*/
}

void IssueUnit::updateBusyRegTable()
{
	AddrOperand addr;
	MyBusyElem elem;

	BusyRegsListIter i1;
	for (i1 = regsBusyInCurrCycle.begin(); i1 != regsBusyInCurrCycle.end(); i1++)
	{
		addr = (*i1)._addr;
		elem = (*i1)._elem;

		if (addr.getFieldType() == _ADDR_MISC_)
		{
			MiscBusyRegTable->write(elem,addr);
		}
		else
		{
			GPBusyRegTable->write(elem,addr);
		}
	}
}

void IssueUnit::doStep()
{

}

#endif  // ISSUE_FOR_PIPELINE_EXPLORATION	
	
