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
*file  : Instruction.h                                      *
*created : 11th nov,97.                                     *
*authors : Wei Yu Tang,vgee                                 *
*last modified :                                            *
*                                                           *
*contents:contains class definitions for instruction class &*
*         other classes derived from it.                    *
*compiled with : g++                                        *
*for more info :                                            *
*************************************************************/
#ifndef _INSTRUCTION_H_
#define _INSTRUCTION_H_

#include "GlobalIncludes.h"
//#include "ArchClassIncludes.h"
#include "Unit.h"

#include "NormalOp.h"

extern 	int numDataOpsPerInstr;
extern	int numControlOpsPerInstr;
extern	int numFlowOpsPerInstr;

/*	We view the instructions as VLIW instructions.
An instruction is composed of slots.
SlotElem is one slot.
Corresponding to each slot is the HW unit, and a slot number.
Thus the operations that can go in a slot are 
decided by the operations that are supported by that unit.
*/
class SlotElem
{
public:
	
	Unit *_unit;
	int _slotNum;
	
	SlotElem(Unit *u = NULL, int s = -1) : _unit(u), _slotNum(s) { } 
	SlotElem(const SlotElem & a)
	{
		_unit = a._unit;
		_slotNum = a._slotNum;
	}
	
	virtual ~SlotElem() { }
};

typedef vector<SlotElem> SlotList;

typedef  vector<SlotElem>::iterator SlotListIterator;

enum SlotType 
{
	DATA_OP_SLOT, CONTROL_OP_SLOT, FLOW_OP_SLOT, ANY_OP_SLOT
};

class InstructionWord
{
};

class Simple_VLIW_Instr : public InstructionWord
{
	SlotList _dataSlotList;
	SlotList _controlSlotList;
	SlotList _anySlotList;
	
	int _anyDataSlotStartNum;
	
	inline SlotList & _getSlotList(SlotType a)
	{
		switch (a)
		{
		case DATA_OP_SLOT:
			return _dataSlotList;
		case CONTROL_OP_SLOT:
			return _controlSlotList;
		case ANY_OP_SLOT:
			return _anySlotList;
		default:
			{
				cout << "Error: Unrecognizable slot type. " << endl;
				assert(0);
				return _dataSlotList;
			}
		}
	}
	
public:
	Simple_VLIW_Instr() : _dataSlotList(), _controlSlotList(), _anySlotList(), _anyDataSlotStartNum(0) { }
	
	SlotList & getDataSlotList() { return _dataSlotList; }
	
	SlotList & getControlSlotList() { return _controlSlotList; }
	
	SlotList & getAnySlotList() { return _anySlotList; }
	
	void setAnyDataSlotStartNum(int sNum)
	{
		_anyDataSlotStartNum = sNum;
	}
	
	void addFU(SlotType a, int slotIndex, Unit *ptr, int slotNum = -1)
	{
		if (((_getSlotList(a))[slotIndex])._unit != NULL)
		{
			cout << "Warning: Unit already exists in the slot" << endl;
			return;
		}
		((_getSlotList(a))[slotIndex])._unit = ptr;
		((_getSlotList(a))[slotIndex])._slotNum = slotNum;
	}
	
	void addSlot(SlotType a, Unit *ptr, int slotNum = -1)
	{
		SlotElem b(ptr, slotNum);
		_getSlotList(a).push_back(b);
	}
	
	void getSlotListForOpCode(SlotType a, char *opName, Vector<int> & slotArray)
	{
		int count = 0;
		for (SlotListIterator i = _getSlotList(a).begin();
		i < _getSlotList(a).end(); i++)
		{
			if ((((*i)._unit)->getOpList()).hasOpCode(opName))
			{
				slotArray.push_back((*i)._slotNum);
			}
			count++;
		}
	}
	
	void getSlotListForOpCode(SlotType a, int opIndex, Vector<int> & slotArray)
	{
		for (SlotListIterator i = _getSlotList(a).begin();
		i != _getSlotList(a).end(); i++)
		{
			/*
			// debugging purposes only
			cout << "Unit Name = " << (*i)._unit->print(cout) << endl;
			OpCodeList j = (*i)._unit->getOpList();
			for (OpCodeList::iterator k = j.begin(); k != j.end(); k++)
				cout << "OpCode = " << (*k) << endl;
			// end debugging
			*/

			if ((((*i)._unit)->getOpList()).hasOpCode(opIndex))
			{
				slotArray.push_back((*i)._slotNum);
			}
		}
	}
	
	void addAnySlots(SlotType a, int opIndex, Vector<int> & slotArray)
	{
		int addNum = 0;
		if (a == DATA_OP_SLOT) addNum = _anyDataSlotStartNum;
		
		for (SlotListIterator i = _getSlotList(ANY_OP_SLOT).begin();
		i != _getSlotList(ANY_OP_SLOT).end(); i++)
		{
			if ((((*i)._unit)->getOpList()).hasOpCode(opIndex))
			{
				
				slotArray.push_back((*i)._slotNum + addNum);
			}
		}
	}
	
	void getSlotListForOper(BaseOperation *a, Vector<int> & slotArray)
	{
		int index = ((NormalOp *)a)->getOpCodeIndex();
		
		if (a->isDataOp())
		{
			getSlotListForOpCode(DATA_OP_SLOT, index, slotArray);
			addAnySlots(DATA_OP_SLOT, index, slotArray);
			
		}
		else if (a->isControlOp())
		{
			getSlotListForOpCode(CONTROL_OP_SLOT, index, slotArray);
			addAnySlots(CONTROL_OP_SLOT, index, slotArray);
		}
	}
	
	Unit *getFU(SlotType a, int slotNum)
	{
		for (SlotListIterator i = _getSlotList(a).begin(); i != _getSlotList(a).end(); i++)
		{
			if ((*i)._slotNum == slotNum)
				return (*i)._unit;
		}
		// possible error condtion.
		
		return NULL;
	}
};

extern Simple_VLIW_Instr *simpleVLIWInstrTemplate;

#endif

