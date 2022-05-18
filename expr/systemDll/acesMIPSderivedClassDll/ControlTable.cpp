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
 *file  : ControlTable.cpp                                   *
 *created : 1 Nov,1998                                       *
 *authors : Asheesh Khare                                    *
 *last modified :                                            *
 *                                                           *
 *contents:contains class definition for classes derived from*
 *         Unit class.                                       *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#include "Unit.h"
#include "ControlTable.h"
#include "CompName.h"
#include "DynamicScheduler.h"

// THIS VERSION IS FOR THE SUPERSCALAR VERSION OF THE R10K PROCESSOR

void decimalToBin(deque<int> & bitstr, int decimalValue);
#define MAX_DECODE 2

ControlTableEntry ::   ControlTableEntry(Unit * unit)
{
	_unit = unit;
	_destPropagated = 0;
	_destWritten = 0;
}

ControlTableEntry ::   ~ControlTableEntry(){}
ControlTableEntry :: ControlTableEntry(const ControlTableEntry & a):_listOfDest(a._listOfDest)
{
	_unit = a._unit;
	for(int i =0; i< (a._properties).size(); i++)
		_properties.push_back(a._properties[i]);
	/*
	BasePtrListIterator i1,i2;
	ctor<BaseType*> tempList;
	a.getDest(tempList);
	i2 = tempList.end();
	i1 = tempList.begin();
	while (i1 != i2)
	{
		_listOfDest.push_back(*i1);
		i1++;
	}
	*/
	_destPropagated = a._destPropagated;
	_destWritten = a._destWritten;
}

ControlTableEntry * ControlTableEntry :: copy()
{
  ControlTableEntry * retPtr = new ControlTableEntry(*this);
  return retPtr;
}

void ControlTableEntry :: setUnit(Unit * unit){ _unit = unit;}
void ControlTableEntry :: setProperty(int pos, int value)
 {
	 int i = _properties.size();

	 while (i <= pos)
	 {
		 _properties.push_back(-1);
		 i++;
	 }
	 _properties[pos] = value;
 }
void ControlTableEntry :: setDest(BaseType * dest)
{
	if (dest)
		_listOfDest.push_back(dest);
}

void ControlTableEntry :: deleteDest(BaseType * dest)
{
	if (dest == 0)
		return;
	BasePtrListIterator i1,i2;
	i1 = _listOfDest.begin();
	i2 = _listOfDest.end();
	while (i1 != i2)
	{
		if (*i1 == dest)
			_listOfDest.erase(i1);
		i1++;
	}
}

void ControlTableEntry :: resetDest()
{
	_listOfDest.clear();
}

void ControlTableEntry :: setDestWritten(BaseType * dest){_destWritten = dest;}
void ControlTableEntry :: setDestPropagated(BaseType * dest){_destPropagated = dest;}
BaseType * ControlTableEntry :: getDestPropagated(){return _destPropagated;}

BaseType * ControlTableEntry :: getDestWritten(){return _destWritten;}

Unit * ControlTableEntry :: getUnit(){return _unit;}
int ControlTableEntry :: getProperty(int pos) {return _properties[pos];}
void ControlTableEntry :: getDest(Vector<BaseType*> & tempList)
{
	BasePtrListIterator i1,i2;
	i1 = _listOfDest.begin();
	i2 = _listOfDest.end();
	while(i1 !=i2)
	{
		tempList.push_back(*i1);
		i1++;
	}
}

int ControlTableEntry :: checkDest(BaseType * dest,Unit ** retunit)
{
	BasePtrListIterator i1,i2;
	i1 = _listOfDest.begin();
	i2 = _listOfDest.end();
	while (i1 != i2)
	{
		if (*((AddrOperand *)(*i1)) == *((AddrOperand *)dest))
		{
			*retunit = _unit;
			return 1;
		}
		i1++;
	}
	return 0;
}

ControlTableEntry & ControlTableEntry::operator =(const ControlTableEntry & a)
{
	_unit = a._unit;
	for(int i =0; i< (a._properties).size(); i++)
		_properties.push_back(a._properties[i]);

	BasePtrListIterator i1,i2;
	Vector<BaseType*> tempList = a._listOfDest;
	i2 = tempList.end();
	i1 = tempList.begin();
	while (i1 != i2)
	{
		_listOfDest.push_back(*i1);
		i1++;
	}
	_destPropagated = a._destPropagated;
	_destWritten = a._destWritten;
	return *this;
}

void ControlTableEntry :: print(ostream & out)
{
	//out << "Unit: " << _unit << endl ;
	//out << "Properties: " << endl;
	out << _unit << " ";
	if (_unit != 0)
	{
		vector<int> :: iterator i1, i2;
		i1 = _properties.begin();
		i2 = _properties.end();
		while (i1 != i2)
		{
			out << *i1;
			i1++;
		}
		out << " Dest: ";
		BasePtrListIterator i3,i4;
		i3 = _listOfDest.begin();
		i4 = _listOfDest.end();
		while (i3 != i4)
		{
			(*i3)->print(out);
			i3++;
		}
	}
	out << endl;
	//out << "Dest: " << endl;
	//if (_unit != 0)
	//{
	//	_dest.print(out);
	//}
}

/*************************************************************
 * class ControlTable                                        *
 *************************************************************/
ControlTable :: ControlTable(int numRows)
{
	Row temp;

	for (int i = 0 ; i < numRows ; i++)
	{
		_currentRowList.push_back(temp);
	}
}

ControlTable :: ~ControlTable()
{
	_currentRowList.clear();
}

int ControlTable::search(Unit * unit, int & rowNum, int & colNum)
{
	int found = 0; 
	int row = 0, col =  0;
	int numRows = _currentRowList.size();
	for (int i = 0; i < numRows; i++, row++)
	{
		RowIterator i1,i2;
		i1 = _currentRowList[i].begin();
		i2 = _currentRowList[i].end();
		col = 0;
		while (i1 != i2)
		{
			if ((i1->getUnit()) == unit)
			{
				found = 1;
				break;
			}
			i1++;
			col++;
		}
		if (found == 1)
			break;
	}
	if (found == 1)
	{
		rowNum = row;
		colNum = col;
	}
	else 
	{
		rowNum = -1;
		colNum = -1;
	}
	return found;
}

ControlTableEntry & ControlTable :: getEntry(int rowNum, int columnNum)
{
	ControlTableEntry a;
	if ((rowNum < _currentRowList.size()) && (columnNum < (_currentRowList[rowNum]).size()))
	{
		Row & rowRef = _currentRowList[rowNum];
		return rowRef[columnNum];
	}
	else 
	{
		cout << "ControlTable :: getEntry(int, int) : Entry not present in the table" << endl;
		assert(0);
		return a;
	}
}

ControlTableEntry & ControlTable :: getEntry(Unit *unit)
{
	int rowNum, colNum;
	ControlTableEntry a;
	if (search(unit,rowNum,colNum))
		return getEntry(rowNum,colNum);
	else
	{
		cout << "ControlTable :: getEntry(Unit *) : Entry not present in the table" << endl;
		assert(0);
		return a;
	}
}

void ControlTable :: addEntry(ControlTableEntry & entry, int rowNum, int colNum)
{
	int numRows = _currentRowList.size();
	if (rowNum >= numRows)
	{
		cout << "ControlTable :: addUnitToTable : Row number exceeds table size" << endl;
		assert(0);
	}
	else
	{
		ControlTableEntry blankEntry;
		Row & currentRowRef = _currentRowList[rowNum];
		while (colNum >= currentRowRef.size())
		{
			currentRowRef.push_back(blankEntry);
		}
		currentRowRef[colNum] = entry;
	}
}

void ControlTable :: addUnitToTable(Unit * unit, int rowNum, int colNum)
{
	int numRows = _currentRowList.size();
	if (rowNum >= numRows)
	{
		cout << "ControlTable :: addUnitToTable : Row number exceeds table size" << endl;
		assert(0);
	}
	else
	{
		ControlTableEntry blankEntry;
		ControlTableEntry tempEntry(unit);
		Row & currentRowRef = _currentRowList[rowNum];
		while (colNum >= currentRowRef.size())
		{
			currentRowRef.push_back(blankEntry);
		}
		currentRowRef[colNum] = tempEntry;
	}
}

void ControlTable :: setDest(Unit * unit, BaseType * dest)
{
	int rowNum, colNum;
	if (search(unit, rowNum, colNum))
		setDest(rowNum, colNum, dest);
	else
	{
		cout << "ControlTable :: setDest(Unit *, BaseType *) : Entry not present in the table" << endl;
		assert(0);
	}
}

void ControlTable :: setDest(int rowNum, int colNum, BaseType * dest)
{
	Row & rowRef = _currentRowList[rowNum];
	rowRef[colNum].setDest(dest);
}

void ControlTable :: setDestWritten(int rowNum, int colNum, BaseType * dest)
{
	Row & rowRef = _currentRowList[rowNum];
	rowRef[colNum].setDestWritten(dest);
}

void ControlTable::setDestWritten(Unit * unit, BaseType * dest)
{
	int rowNum, colNum;
	if (search(unit, rowNum, colNum))
		setDestWritten(rowNum,colNum,dest);
	else
	{
		cout << "ControlTable :: setDestWritten(Unit *, BaseType *) : Entry not present in the table" << endl;
		assert(0);
	}
}

void ControlTable::setDestPropagated(Unit * unit, BaseType * dest)
{
	int rowNum, colNum;
	if (search(unit, rowNum, colNum))
		setDestPropagated(rowNum,colNum,dest);
	else
	{
		cout << "ControlTable :: setDestPropagated(Unit *, BaseType *) : Entry not present in the table" << endl;
		assert(0);
	}
}

void ControlTable :: setDestPropagated(int rowNum, int colNum, BaseType * dest)
{
	Row & rowRef = _currentRowList[rowNum];
	rowRef[colNum].setDestPropagated(dest);
}

BaseType * ControlTable :: getDestPropagated(int rowNum, int colNum)
{
	Row & rowRef = _currentRowList[rowNum];
	return rowRef[colNum].getDestPropagated();
}

BaseType * ControlTable :: getDestWritten(int rowNum, int colNum)
{
	Row & rowRef = _currentRowList[rowNum];
	return rowRef[colNum].getDestWritten();
}

void ControlTable :: deleteDest(int rowNum, int colNum,BaseType * dest)
{
	Row & rowRef = _currentRowList[rowNum];
	rowRef[colNum].deleteDest(dest);
}

void ControlTable :: resetDest(Unit * unit)
{
	int rowNum, colNum;
	if (search(unit, rowNum, colNum))
	{
		Row & rowRef = _currentRowList[rowNum];
		rowRef[colNum].resetDest();
	}
	else
	{
		cout << "ControlTable :: resetDest(Unit *) : Entry not present in the table" << endl;
		assert(0);
	}
}

void ControlTable :: setProperty(Unit * unit, int pos, int value)
{
	int rowNum, colNum;
	if (search(unit, rowNum, colNum))
	{
		Row & rowRef = _currentRowList[rowNum];
		rowRef[colNum].setProperty(pos,value);
	}
	else
	{
		cout << "ControlTable :: setProperty(Unit *, int, int) : Entry not present in the table" << endl;
		assert(0);
	}
}

void ControlTable :: setProperty(int row, int col, int pos, int value)
{
	Row & rowRef = _currentRowList[row];
	rowRef[col].setProperty(pos,value);
}

void ControlTable :: setDest(BaseType *dest)
{
	RowIterator i1, i2;
	int numRows = _currentRowList.size();
	for (int i = 0; i<numRows; i++)
	{
		i1 = _currentRowList[i].begin();
		i2 = _currentRowList[i].end();
		while (i1 != i2)
		{
			i1->setDest(dest);
			i1++;
		}
	}
}

void ControlTable :: setProperty(int pos, int value)
{
	RowIterator i1, i2;
	int numRows = _currentRowList.size();
	for (int i = 0; i<numRows; i++)
	{
		i1 = _currentRowList[i].begin();
		i2 = _currentRowList[i].end();
		while (i1 != i2)
		{
			i1->setProperty(pos,value);
			i1++;
		}
	}
}

void ControlTable :: getDest(Unit * unit,Vector<BaseType*> & tempList)
{
	ControlTableEntry & tempEntry = getEntry(unit);
	tempEntry.getDest(tempList);
}

int ControlTable :: checkDest(Unit * unit, BaseType * dest, Unit** retunit)
{
	RowIterator i1, i2;
	for (int i = 0; i < _currentRowList.size(); i++)
	{
		i1 = _currentRowList[i].begin();
		i2 = _currentRowList[i].end();
		while(i1 != i2)
		{
			// the if takes care of holes in the table that are created because 
			// all pipes may not be  of the same length
			if ((i1->getUnit() != 0) && (i1->getUnit() != unit))
			{
				if (i1->checkDest(dest,retunit))
					return 1;
			}
			i1++;
		}
	}
	return 0;
}

int ControlTable :: getProperty(Unit * unit, int pos)
{
	ControlTableEntry & tempEntry = getEntry(unit);
	return tempEntry.getProperty(pos);
}

int ControlTable :: getProperty(int row, int col, int pos)
{
	Row & rowRef = _currentRowList[row];
	return rowRef[col].getProperty(pos);
}

void ControlTable :: getIthRow(int rowNum, Row & unitlist)
{
	RowIterator i1, i2;
	i1 = _currentRowList[rowNum].begin();
	i2 = _currentRowList[rowNum].end();
	while(i1 != i2)
	{
		unitlist.push_back(*i1);
		i1++;
	}
}

void ControlTable :: initGlobalFUList()
{
	// add all the Units in the EXECUTE_STAGE to the globalFUList data structure.

	Row & currentRowRef = _currentRowList[EXECUTE_STAGE_NUM];

	Unit *currUnit;
	RowIterator i1;

	FUListElem elem;
	elem._flag = INIT_FULISTELEM;

	for (i1 = currentRowRef.begin(); i1 != currentRowRef.end(); i1++)
	{
		if ((currUnit = (*i1).getUnit()) != NULL)
		{
			elem._unit = currUnit;
			globalFUList.push_back(elem);
		}
	}
}

int isEmptyIntersection(LatchList *l1, LatchList *l2)
{
	if ((l1 == NULL) || (l2 == NULL)) return _YES_;
	if ((l1->empty()) || (l2->empty())) return _YES_;

	LatchList::iterator i1, i2;
	LatchList::iterator j1, j2;

	i2 = l1->end();
	j2 = l2->end();

	for (i1 = l1->begin(); i1 != i2; i1++)
	{
		for (j1 = l2->begin(); j1 != j2; j1++)
		{
			if ((*i1) == (*j1)) return _NO_;
		}
	}
	return _YES_;
}

void ControlTable :: createFUandRFConnTable()
{
	// first, add all the units in the EXECUTE_STAGE to the table data structure.
	// then, add all the rfs to the table data structure.

	Row & currentRowRef = _currentRowList[EXECUTE_STAGE_NUM];

	Unit *currUnit, *fromUnit, *toUnit;
	RowIterator i1, i2;
	for (i1 = currentRowRef.begin(); i1 != currentRowRef.end(); i1++)
	{
		if ((currUnit = (*i1).getUnit()) != NULL)
		{
			globalFUandRFConnTable.addFUToMap(currUnit);
		}
	}

	// TBD : Add all the RFs to the table data structure.

	// Once all the FUs and RFs have been added, create the (zero-initialized) Connectivity Table.
	//
	globalFUandRFConnTable.createConnTable();

	// next, determine all the feed-back paths between fus.
	//
	for (i1 = currentRowRef.begin(); i1 != currentRowRef.end(); i1++)
	{
		// We designate a unit as the "from" unit.
		// Next, we get the list of its output latches.
		// (Note that in most cases there is only one output latch).
		// Then, we determine if any fu has one of the output latches as one of its other latches.
		// If so, then that fu is a "to" unit.

		if ((fromUnit = (*i1).getUnit()) != NULL)
		{
			// get its output latch list.
			LatchList * outLatchList = fromUnit->getLatchList(_OUT_L_);

			// Now, for each unit, check if its other latch list contains any of the output latches.

			for (i2 = currentRowRef.begin(); i2 != currentRowRef.end(); i2++)
			{
				if ((toUnit = (*i2).getUnit()) != NULL)
				{
					LatchList *otherLatchList = toUnit->getLatchList(_OTHER_L_);

					if (!isEmptyIntersection(outLatchList, otherLatchList))
					{
						// i.e. there exists a feedback path.
						//
						globalFUandRFConnTable.addPath(fromUnit, toUnit);
					}
				}
			}
		}
	}

	// TBD : add from and to between FUs and RFs.
}

void ControlTable :: update()
{
	RowIterator i1, i2;
	for (int i = 0; i < _currentRowList.size(); i++)
	{
		i1 = _currentRowList[i].begin();
		i2 = _currentRowList[i].end();
		while(i1 != i2)
		{
			// the if takes care of holes in the table that are created because 
			// all pipes may not be  of the same length
			if (i1->getUnit() != 0)
			{
				//BaseType * destWritten = i1->getDestWritten();
				//if (destWritten)
				//	i1->deleteDest(destWritten);
				i1->setDestWritten(0);
				// i1->setProperty(StallBit,0);

				if (i1->getProperty(NextCycleExecuteBit))
					i1->setProperty(ExecuteBit, 1);

				i1->setProperty(NextCycleExecuteBit,0);

				//i1->setProperty(Src1ForwBit,0);
				//i1->setProperty(Src2ForwBit,0);
				i1->setProperty(PipeNumBit,0);

				// set the dest that will be propagated in the next cycle 
				Vector<BaseType*> tempList;
				i1->getDest(tempList);
				BaseType * destNextCycle=0;
				if (!(tempList.empty()))
					destNextCycle = tempList.front();
				i1->setDestPropagated(destNextCycle);
				//i1->setDestPropagated(0);
			}
			i1++;
		}
	}
	setPipeNumBit(DECODE_STAGE_NUM,0,0);
}


/*
void ControlTable :: updateProperty(int pos)
{
	RowIterator i1, i2, i3;
	for (int i = 0; i < _currentRowList.size(); i++)
	{
		i1 = _currentRowList[i].begin();
		i2 = _currentRowList[i].end();
		while(i1 != i2)
		{
			// the if takes care of holes in the table that are created because 
			// all pipes may not be  of the same length
			if (i1->getUnit())
				i1->setProperty(pos,i3->getProperty(pos)); 
			i1++;
		}
	}
}
*/

void ControlTable :: print(ostream & out)
{
	RowIterator i1, i2;
	int col = 0;

	for (int i = 0; i < _currentRowList.size(); i++)
	{
		i1 = _currentRowList[i].begin();
		i2 = _currentRowList[i].end();
		col = 0;
		while (i1 != i2)
		{
			out << " Row,Col " << i << "," << col << "  ";
			i1->print(out);
			out << endl;
			i1++;col++;
		}
	}
	//out << endl;
}

// instantiate the globalControlTable 

ControlTable globalControlTable(MaxNumStages);

// instantiate the globalFUList

FUList globalFUList;

/*************************************************************
 * class NetList                                       *
 *************************************************************/

NetList::NetList(){}
NetList::~NetList(){} 

void NetList::addUnit(Unit *unit, Unit * pred, Unit * succ)
{
	graphIterator i1 = _graph.find(unit);
	if (i1 == _graph.end())
	{
		// Element does not exist in the graph. needs to be added
		struct PredSucc newElement;
		if (pred)
			(newElement._predlist).push_back(pred);
		if (succ)
			(newElement._succlist).push_back(succ);
		_graph[unit] = newElement;
	}
	else
	{
		// Element exists, lists need to be updated
		UnitList & tempList = (i1->second)._predlist;
		UnitListIterator i2,i3;
		i3 = tempList.end();
		i2 = std::find(tempList.begin(), tempList.end(), pred);
		if ((i2 == i3) && pred)
			tempList.push_back(pred);
		tempList = (i1->second)._succlist;
		i2 = std::find(tempList.begin(), tempList.end(), succ);
		i3 = tempList.end();
		if ((i2 == i3) && succ)
			tempList.push_back(succ);
	}
}

void NetList::setPredecessor(Unit * unit, Unit * pred)
{
	graphIterator i1 = _graph.find(unit);
	if (i1 == _graph.end())
	{
		cout << "Error : NetList::setPredecessor - cannot set predecessor for an unknown unit" << endl;
		assert(0);
	}
	else
	{
		UnitList & tempList = (i1->second)._predlist;
		UnitListIterator i2;
		i2 = std::find(tempList.begin(), tempList.end(), pred);
		if ((i2 == tempList.end()) && pred)
			tempList.push_back(pred);
	}
}


void NetList::setSuccessor(Unit * unit, Unit * succ)
{
	graphIterator i1 = _graph.find(unit);
	if (i1 == _graph.end())
	{
		cout << "Error : NetList::setPredecessor - cannot set predecessor for an unknown unit" << endl;
		assert(0);
	}
	else
	{
		UnitList & tempList = (i1->second)._succlist;
		UnitListIterator i2;
		i2 = std::find(tempList.begin(), tempList.end(), succ);
		if ((i2 == tempList.end()) && succ)
			tempList.push_back(succ);
	}
}


void NetList :: print(ostream & out)
{
	graphIterator mapi1,mapi2;
	Unit * tempUnit;
	struct PredSucc tempStruct;

	mapi1 = _graph.begin();
	mapi2 = _graph.end();
	while (mapi1 != mapi2)
	{
		tempUnit = mapi1->first;
		tempStruct = mapi1->second;
		out << "Unit :" ;
		getName(tempUnit).print(out);
		cout << endl;
		out << "Pred :" ;
		UnitList & tempList = tempStruct._predlist;
		UnitListIterator i1, i2;
		i1 = tempList.begin();
		i2 = tempList.end();
		while (i1 != i2)
		{
			getName(*i1).print(out);
			out << "  ";
			i1++;
		}
		out << endl;

		out << "Succ :" ;
		tempList = tempStruct._succlist;
		i1 = tempList.begin();
		i2 = tempList.end();
		while (i1 != i2)
		{
			getName(*i1).print(out);
			out << "  ";
			i1++;
		}
		out << endl;
		out << endl;
		mapi1++;
	}
}

UnitList & NetList::getSuccessorList(Unit * unit)
{
	return (_graph[unit])._succlist;
}

UnitList & NetList::getPredecessorList(Unit *unit)
{
	return (_graph[unit])._predlist;
}

// instantiate the global NetList
NetList globalNetList;


/*************************************************************
 * class StallMechanism                                      *
 *************************************************************/
StallMechanism :: StallMechanism(){}
StallMechanism :: ~StallMechanism(){}
void StallMechanism :: setStall()
{
	int rowNum, busyBit, stallBit;
	Row tempList;
	RowIterator i1, i2;
	int colNum = 0;

	for(rowNum = MaxNumStages-1; rowNum >= 0; rowNum--)
	{
		// Copy an entire row (ith stage units of all pipes) from the globalControlTable into a temporary list.
		globalControlTable.getIthRow(rowNum,tempList);
		
		i1 = tempList.begin();
		i2 = tempList.end();
		colNum = 0;
		while(i1 != i2)
		{		
			Unit * currUnit = i1->getUnit();
			
			if (currUnit)
			{
				UnitList & childrenList = globalNetList.getSuccessorList(currUnit);
				
				UnitListIterator c1 = childrenList.begin();
				bool busy_stall = false;

				while(c1 != childrenList.end())
				{
					Unit *child = (*c1);

					busyBit = globalControlTable.getProperty(child, BusyBit);
					stallBit = globalControlTable.getProperty(child, StallBit);

					if (busyBit || stallBit)
					{
						busy_stall = true;
						globalControlTable.setProperty(rowNum, colNum, StallBit, 1);
						break;
					}
					c1++;
				}

				if (busy_stall == false)
				{
					// Unstall, if stalled earlier
					globalControlTable.setProperty(rowNum, colNum, StallBit, 0);
					
					// Update the output latches for the units which are not stalled				
					LatchListIterator lIterBegin, lIterEnd;
					lIterBegin = currUnit->getLatchList(_OUT_L_)->begin();
					lIterEnd = currUnit->getLatchList(_OUT_L_)->end();
					
					while (lIterBegin != lIterEnd)
					{
						(*lIterBegin)->doUpdate();
						(*lIterBegin)->resetNew();
						lIterBegin++;
					}
				}
			}
			i1++;colNum++;
		}
		tempList.clear();
	}
}


StallMechanism globalStallMechanism;

void getForwardingSrc(Latch * latch,UnitList * forwSrcList)
{
	RowIterator i1, i2;
	Row tempList;

	for (int i = MaxNumStages-1; i>=0; i--)
	{
		globalControlTable.getIthRow(i,tempList);
		i1 = tempList.begin();
		i2 = tempList.end();
		while (i1 != i2)
		{
			Unit * currunit = i1->getUnit();
			// to take care of holes in the control table where no units are stored
			if (currunit)
			{
				LatchList * outputLatchList = currunit->getLatchList(_OUT_L_);

				LatchList::iterator i3,i4;
				i3 = outputLatchList->begin();
				i4 = outputLatchList->end();

				for(i3;i3!=i4;i3++)
				{
					if (*i3 == latch)
						forwSrcList->push_back(currunit);
				}
			}
			i1++;
		}
		tempList.clear();
	}
}

void createForwardingTable()
{
	RowIterator i1, i2;
	Row tempList;

	for (int i = MaxNumStages-1; i>=0; i--)
	{
		globalControlTable.getIthRow(i,tempList);

		i1 = tempList.begin();
		i2 = tempList.end();
		while (i1 != i2)
		{
			Unit * currunit = i1->getUnit();
			// to take care of holes in the control table where no units are stored
			if (currunit)
			{
				LatchList * inputLatchList = currunit->getLatchList(_IN_L_);

				LatchList::iterator i3,i4;
				if (!(inputLatchList->empty()))
				{
					i3 = inputLatchList->begin();
					i4 = inputLatchList->end();
				
					UnitList unitlist;
					i3++;

					if (i3 != i4)
					{
						ForwardingTable[currunit] = unitlist;
					}

					for(i3;i3!=i4;i3++)
					{
						UnitList forwSrcList;
						getForwardingSrc(*i3,&forwSrcList);
						UnitList::iterator i5,i6;
						i5 = forwSrcList.begin();
						i6 = forwSrcList.end();
						for(i5; i5!=i6; i5++)
						{
							(ForwardingTable[currunit]).push_back(*i5);
						}
					}
				}
			}
			i1++;
		}
		tempList.clear();
	}
}

void printForwardingTable(ostream& out)
{
	Map<Unit*,UnitList>::iterator i1,i2;
	i1 = ForwardingTable.begin();
	i2 = ForwardingTable.end();

	MyString unitName;

	for(i1;i1!=i2;i1++)
	{
		out << "Dest: " << (getName(i1->first)).getStr() << " ";
		UnitList * forwSrcsList = &(i1->second);
		out << "Srcs: " ;
		UnitListIterator i3,i4;
		i3 = forwSrcsList->begin();
		i4 = forwSrcsList->end();
		for(i3;i3!=i4;i3++)
		{
			out << (getName(*i3)).getStr() << "  ";
		}
		out << endl;
	}
}

void decimalToBin(deque<int> & bitstr, int decimalValue)
{
	int temp = decimalValue;
	while (temp != 0)
	{
		bitstr.push_front(temp%2);
		temp = temp/2;
	}
	while (bitstr.size() != numPipes)
	{
		bitstr.push_front(0);
	}

	deque<int>:: iterator i1 = bitstr.begin();
	deque<int>:: iterator i2 = bitstr.end();
	/*
	while(i1 != i2)
	{
		cout << (*i1) ;
		i1++;
	}
	cout << endl;
	*/
	
}

int binaryToDecimal(deque<int> & bitstr)
{
	deque<int>:: iterator i1 = bitstr.begin();
	deque<int>:: iterator i2 = bitstr.end();
	int placeValue = 1;
	int binCode = 0;

	/*while(i1 != i2)
	{
		cout << (*i1) ;
		i1++;
	}
	cout << endl;
	*/
	
	
	for(int i=numPipes-1; i>=0; i--)
	{
		if (bitstr[i] == 1)
			binCode+=placeValue;
		placeValue*=2;
	}
	return binCode;
}