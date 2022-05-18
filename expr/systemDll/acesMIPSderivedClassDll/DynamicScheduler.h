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
 *file  : DynamicScheduler.h                                 *
 *created : 3 May,2000                                       *
 *authors : Prabhat, Srikanth                                *
 *last modified :                                            *
 *                                                           *
 *contents:contains class definition and functions for       *
 *         objects that are needed for DynamicScheduling in  *
 *         Elf processor                                     *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#ifndef _DYNAMICSCHEDULER_H_
#define _DYNAMICSCHEDULER_H_

#include "DerivedAddrData.h"
#include "DerivedLatchData.h"
#include "Storage.h"
#include "..\..\pcProDll\DebugSwitches.h"

#define MAX_ADDRQUE_SIZE 16

#define MAX_QUEUE_SIZE 32

#define MAX_REGLIST_SIZE 1024

#define MAX_GPRQUE_SIZE 16
 
//following values may be assigned to _prediction 
#define _PREDICTED_TAKEN_ 0
#define _PREDICTED_NOT_TAKEN_ 1
//following values may be assigned to _done on branch operation completion
#define _ASPREDICTED_ 1
#define _MISPREDICTED_ 2


class MyMapBusyReg{
private:
	/*
	inline void setTypeOfVal(MyBool & value, AddrOperand & address)
	{
		if (value._valType != _ANY) return;
		
		value._valType = _INT;
		value._as.integer = 0;
	}
	*/	
public:
	map<int,MyBusyElem> _mymap;
	
	MyMapBusyReg() {}
	MyMapBusyReg(const MyMapBusyReg & a) 
	{
		map<int, MyBusyElem>::const_iterator i1;
		i1 = (a._mymap).begin();
		while (i1 != (a._mymap).end())
		{
			_mymap[i1->first] = i1->second;
			i1++;
		}
	}

	~MyMapBusyReg() {}
	MyMapBusyReg(int size):_mymap(){};
	void read(MyBusyElem & value, AddrOperand & addr){
		if (addr.getType() != _REG_)
		{
			value._id = false;
			return;
		}

		int address = addr.convertAddrMechanism();
		
		value = _mymap[address];
	/*					
		if (debugSIMULATOR)
		{
			cout << "Printing BUSYREGTABLE READ values: " << endl;
			cout << "Addr: " ;
			addr.print(cout);
			cout << endl;
			cout << "Data: ";
			cout << value._id;
			cout << endl;	 
		}
		*/
	}
	
	void write(MyBusyElem & value, AddrOperand & addr){
		if (addr.getType() != _REG_)
			return;

		int address = addr.convertAddrMechanism();
		_mymap[address] = value;
		
		/*
		if (debugSIMULATOR)
		{
			cout << "Printing BUSYREGTABLE WRITE values: " << endl;
			cout << "Addr: " ;
			addr.print(cout);
			cout << endl;
			cout << "Data: ";
			cout << value._id;
			cout << endl;	 
		} 
		*/

	}
	
	void dump()
	{
	}
	
	void print(ostream & out)
	{
		map<int, MyBusyElem>::iterator i;
		for (i=_mymap.begin(); i!= _mymap.end(); i++)
		{
			out << "REG " << (*i).first << " : " ;
			out << ((*i).second)._id;
			out << endl;
		}
	}

	void reset()
	{
		_mymap.clear();
	}
};

/*************************************************************
 *class : BusyRegTable                                       *
 *description : maintains info about the available registers *
 *		registers are available (for opread by an operation) *
 *		when they have been written into                     *
 *		registers are marked unavailable when they are       *
 *		removed from the freelist and added to the mapping   *
 *		table                                                *
 *Methods:                                                   *
 *		getBusyBit, setBusyBit                               *
 *Data Members :		                                     *
 *		map of AddrOperands and boolean value                *
 *************************************************************/

typedef RegFile<MyMapBusyReg,AddrOperand,MyBusyElem> BusyRegTable;




/*************************************************************
 *class : CircularQueue                                      *
 *description : used to model activeList - a list that       *
 *		maintains info about instrs in the pipeline          *
 *		an entry gets added when a new physical reg is       *
 *		assigned to a logical register                       *
 *		an Element gets deleted when an instruction graduates*
 *		entries may be referenced for changing the done flag *
 *		while the entries are still in the active List       *
 *Methods:                                                   *
 *		isFull, isEmpty, enQueue, deQueue, getElement, print *
 *Data Members :		                                     *
 *		array of templated objects, head, tail and size      *
 *************************************************************/

template <class T, int MAX_SIZE>
class CicularQueue
{
	private:
		T _queue[MAX_SIZE];
		int _head;
		int _tail;
		int _curr;
		int _size;

	public:
		CicularQueue(){_head = 0; _tail = -1; _size = 0; _curr = 0;}
		~CicularQueue(){}

		bool isFull()
		{
			if (_size == MAX_SIZE)
				return true;
			return false;
		}

		bool isEmpty()
		{
			if (_size == 0)
				return true;
			return false;
		}

		int Head()
		{
			return _head;
		}

		int Tail()
		{
			return _tail;
		}

		int Size()
		{
			return _size;
		}

		int enQueue(T &a)
		{
			if (_size == MAX_SIZE)
			{
				cout << "Error: CircularQueue - overflow" << endl;
				assert(0);
				return 1;
			}
			else
			{
				_tail = (_tail+1) % MAX_SIZE;
				_queue[_tail] = a;
				_size++;
				return _tail;
			}
		}

		T & getElement(int pos)
		{
			T a;
			if (_size >0)
			{
				if (((pos <= _tail) && (pos >= _head) && (_tail >=_head)) 
					|| ((((pos >= _head) && (pos < MAX_SIZE)) 
					||((pos >=0) && (pos <=_tail))) && (_tail < _head)))
					return _queue[pos];
				else
				{
					cout << "Error: CicularQueue - Element does not exist at this position" << endl;
					assert(0);
					return a;
				}
			}
			else
			{
				cout << "CicularQueue::getElement() - Searching for an element in empty queue" << endl;
				assert(0);
				return a;
			}
		}

		T & getHeadElement()
		{
			T a;
			if (_size == 0)
				return a;
			else
				return _queue[_head];
		}
		
		T deQueue()
		{
			T a;
			if (_size == 0)
			{			
				cout << "Error: CircularQueue - underflow" << endl;
				assert(0);
				return a;
			}
			else
			{
				_size--;
				T temp = _queue[_head];
				_head = (_head + 1) % MAX_SIZE;
				return temp;
			}
		}


		T * getCurr()
		{
			T* temp;
			if (_size == 0)
			{			
				cout << "Error: CircularQueue - underflow" << endl;
				assert(0);
				return temp;
			}
			else
			{
				temp = &(_queue[_curr]);
				_curr = (_curr + 1) % MAX_SIZE;
				return temp;
			}
		}

		void sync()
		{
			_curr = _head;
		}

		void flush()
		{
			T temp;
			while (_size > 0)
			{
				temp = deQueue();
				//delete temp;
			}

			_head = 0;
			_tail = -1;
			_size = 0;
		}

	
		void print(ostream & out)
		{
			if (_size == 0)
				return;

			if (_tail >= _head)
			{
				for (int i = _head; i <= _tail; i++)
				{
					out << "[" << i << "] :";
					_queue[i].print(out);
				}
			}
			else if (_size > 0)
			{
				for (int i = _head; i < MAX_SIZE; i++)
				{
					out << "[" << i << "] :";
					_queue[i].print(out);
				}
				
				for (i = 0; i <= _tail; i++)
				{
					out << "[" << i << "] :";
					_queue[i].print(out);
				}
			}
		}
};

 



/*************************************************************
 *class : BranchPrediction                                   *
 *description : maintains history of branch instrs in the    *
 *              prog. Used to predict branches according to  *
 *              2-bit predictor principle                    *
 *Methods: predict(currOper)                                 *
 *Data Members:                                              *
 *************************************************************/

class BranchPrediction:public Storage
{
private:
	enum PREDICTION_STATE {TAKEN1,TAKEN2,NOTTAKEN1,NOTTAKEN2};
	map<long,PREDICTION_STATE> _branchHistoryMap;
public:

	BranchPrediction(){}
	~BranchPrediction(){}

	void read(BaseType &, long);
	void write(BaseType &, long);
	void doUpdate();
	void print(ostream &);	
};


//extern BusyRegTable * IntBusyRegTable;
//extern BusyRegTable * TempIntBusyRegTable;

//extern BusyRegTable * FltBusyRegTable;
//extern BusyRegTable * TempFltBusyRegTable;

extern BranchPrediction BranchPredictor;
//extern vector<BusyRegTable *> ListOfIntBusyRegTables;
//extern vector<BusyRegTable *> ListOfFltBusyRegTables;

extern BusyRegTable * MiscBusyRegTable;

extern void writeToPCLatch(InstrPtr tgt);

 
#endif
