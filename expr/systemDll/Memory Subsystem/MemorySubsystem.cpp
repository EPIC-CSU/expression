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
 *file  : MemorySubsystem.cpp                                *
 *created : Feb 18, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Memory subsystem class     *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

 
#include "MemorySubsystem.h"
 
int memJunk = 0;
bool imemJunk = false;

void read_access(long addr, Val & value, int opcode, int & latency);
void write_access(long addr, Val & value, int opcode, int & latency);
void init_mem_hier();

MemorySubsystem :: MemorySubsystem()
{
	_stallBit = 0;
	init_mem_hier();
}

MemorySubsystem :: ~MemorySubsystem()
{
}

void MemorySubsystem :: SendReadRequest(long addr, int opcode)
{
	// Put the read request in the Read Queue
	MemData mData;
	mData.setAddress(addr);
	mData.setOpcode(opcode);

	ReadQueue.push_back(mData);
}


void MemorySubsystem :: SendWriteRequest(long addr, Val & value, int opcode)
{
	// Put the write request in the Write Queue
	MemData mData;
	mData.setAddress(addr);
	mData.setValue(value);
	mData.setOpcode(opcode);

	WriteQueue.push_back(mData);
}
	  

long MemorySubsystem :: ReceiveData(Val & readValue, int & opcode, long address)
{
	// Check if any data is available in this time. 
	// Valid data with latency == 0, which is ready
	// to go out of memory subsystem

	// Each call returns (sets the readValue) the first available 
	// data from the begining and deletes the entry.

	
	vector<MemData>::iterator i1, i2, i3;
	i1 = ReadQueue.begin();
	i2 = ReadQueue.end();
	 
	while (i1 != i2)
	{
		MemData mData = (*i1);

		Val & value = mData.getValue();
		long addr = mData.getAddress();

		// Check if ready to take-off (latency == 0)
		if ((mData.getLatency() == 0) && (mData.getState() == _DONE_) && (address == addr))
		{
			// Set readValue, delete it from the queue and return
			readValue = value;
			opcode = mData.getOpcode();
			
			// Remove from the ReadQueue				
			if (ReadQueue.size() == 1)
			{
				ReadQueue.clear();
				BankReadAccess.clear();
				return addr;
			}
			else
			{
				i3 = i1;
				i1 = ReadQueue.erase(i3);
				int bankNum = getBank(address);
				BankReadAccess[bankNum] = BankReadAccess[bankNum] - 1;
			}
			
			return addr;
		}
		else
			i1++;
	}

	return -1;
}


bool MemorySubsystem :: hasDataWritten(long address)
{
	// Check if this data is written or not	
	bool written = true;

	vector<MemData>::iterator i1, i2;
	i1 = WriteQueue.begin();
	i2 = WriteQueue.end();
	 
	while (i1 != i2)
	{
		MemData mData = (*i1);

		long addr = mData.getAddress();

		// Yet to be written, still in queue
		if (address == addr)
		{
			written = false;
		}

		i1++;
	}

	return written;
}


int MemorySubsystem :: IsMemStalled()
{
	return _stallBit;
}

void MemorySubsystem :: preprocess()
{
}

void MemorySubsystem :: doStep()
{
	// READ
	// Traverse ReadQueue, find out the pending read requests (latency == -1) and
	// initiate the read. Decrement latency for the requests which are already 
	// successful(read is done prior to this cycle)

	vector<MemData>::iterator i1, i2;
	i1 = ReadQueue.begin();
	i2 = ReadQueue.end();
	 

	if (i1 != i2)
	{
		MemData mData = (*i1);

		Val & value = mData.getValue();
		long address = mData.getAddress();
		int latency = mData.getLatency();
		int opcode = mData.getOpcode();
		MemAccessState state = mData.getState();

		if ((state == _WAITING_) && (latency <= 0)) // Initial state. Ready to initiate read
		{
			// Read it
			read_access(address, value, opcode, latency);

			if (memJunk)
			cout << clock_count << "::READ: " << value._as.integer << " from " << address << " with delay " << latency << endl;

			(*i1).setLatency(latency);
			(*i1).setValue(value);

			if (latency == 1)
			{	
				(*i1).setState(_DONE_);
			}
			else
				(*i1).setState(_STALLED_);
			
		}
		else if (latency > 0)
		{
			// Already read, waiting for take-off 
			// Decrement latency
			(*i1).setLatency(latency - 1);

			// When latency becomes 1 or less modify the _valid array of the cache
			if (latency <= 1)
			{
				(*i1).setState(_DONE_);
			}
		}
	}


	// WRITE
	i1 = WriteQueue.begin();
	i2 = WriteQueue.end();
	 
	if (i1 != i2)
	{
		MemData mData = (*i1);

		Val & value = mData.getValue();
		long address = mData.getAddress();
		int latency = mData.getLatency();
		int opcode = mData.getOpcode();
		MemAccessState state = mData.getState();

		if (state == _WAITING_) // Initial state. Ready to initiate write
		{
			write_access(address, value, opcode, latency);

			if (memJunk)
			cout << clock_count << "::WRITE: " << value._as.integer << " to " << address << " with delay " << latency << endl;

 			(*i1).setLatency(latency);
			
			if (latency == 1)
			{	
				(*i1).setState(_DONE_);
			}
			else
				(*i1).setState(_STALLED_);
		}
		else if ((latency > 0) && (state == _STALLED_))
		{
			// Decrement latency
			(*i1).setLatency(latency - 1);

			if (latency <= 1)
			{
				(*i1).setState(_DONE_);
			}			
		}

		if ((*i1).getState() == _DONE_)
		{
			// Remove from the WriteQueue				
			if (WriteQueue.size() == 1)
				WriteQueue.clear();
			else
				WriteQueue.erase(i1);
		}
	}

	
	/********* Commented by Mahesh Mamidipaka on 2/8/2002 to avoid multiple parallel access to the memory
	while (i1 != i2)
	{
		MemData mData = (*i1);

		Val & value = mData.getValue();
		long address = mData.getAddress();
		int latency = mData.getLatency();
		int opcode = mData.getOpcode();
		MemAccessState state = mData.getState();

		int bankNum = getBank(address);

		if (((state == _WAITING_) || (state == _STALLED_)) && (latency <= 0)) // Initial state. Ready to initiate read
		{
			if (IsReadDepSatisfied(bankNum))
			{
				// Read it
				read_access(address, value, opcode, latency);

				if (memJunk)
				cout << clock_count << "::READ: " << value._as.integer << " from " << address << " with delay " << latency << endl;

				(*i1).setLatency(latency);

				if (latency == 1)
				{
					(*i1).setValue(value);
					(*i1).setState(_DONE_);
				}
				else
					(*i1).setState(_WAITING_);
			}
			else
			{
				// Mark the stall bit for the operation
				(*i1).setState(_STALLED_);
			}
		}
		else if (latency > 0)
		{
			// Already read, waiting for take-off 
			// Decrement latency
			(*i1).setLatency(latency - 1);

			// When latency becomes 1 or less modify the _valid array of the cache
			if ((latency <= 1)  && ((*i1).getState() != _DONE_))
			{
				connect->get_module_for_address(address)->read(address, value, 999,latency);

				// Since this is delayed read, so read should be successful always
				if (latency != 1) assert(0);

				(*i1).setValue(value);
				(*i1).setState(_DONE_);
			}
		}

		i1++;
	}


	// WRITE
	// Traverse WriteQueue, find out the elements ready to be written (latency == 0)
	// If it's not ready to be written, decrement the latency. Note that, in general
	// latency for write is '0'.

	i1 = WriteQueue.begin();
	i2 = WriteQueue.end();
	 
	while (i1 != i2)
	{
		MemData mData = (*i1);

		Val & value = mData.getValue();
		long address = mData.getAddress();
		int latency = mData.getLatency();
		int opcode = mData.getOpcode();
		MemAccessState state = mData.getState();

		int bankNum = getBank(address);

		if (((state == _WAITING_) || (state == _STALLED_)) && (latency <= 1)) // Initial state. Ready to initiate write
		{
			if (IsWriteDepSatisfied(bankNum))
			{
				// Write it
				write_access(address, value, opcode, latency);

				if (memJunk)
				cout << clock_count << "::WRITE: " << value._as.integer << " to " << address << " with delay " << latency << endl;

 				(*i1).setLatency(latency);
			    (*i1).setState(_DONE_);
			}
			else
			{
				// Mark the stall bit for the operation
				(*i1).setState(_STALLED_);
			}
		}
		else if (latency > 0)
		{
			// Decrement latency
			(*i1).setLatency(latency - 1);
		}

		i1++;
	}

	// UPDATE write queue
	// Delete the entries which have completed writing

	i1 = WriteQueue.begin();
	i2 = WriteQueue.end();
	 
	while (i1 != i2)
	{
		MemData mData = (*i1);

		long address = mData.getAddress();
		int latency = mData.getLatency();
		MemAccessState state = mData.getState();

		if ((latency == 0) && (state == _DONE_))  
		{
			// Remove from the WriteQueue				
			if (WriteQueue.size() == 1)
			{
				WriteQueue.clear();
				BankWriteAccess.clear();
			}
			else
			{
				WriteQueue.erase(i1);
				int bankNum = getBank(address);
				BankWriteAccess[bankNum] = BankWriteAccess[bankNum] - 1;
			}
		}

		i1++;
	}
	*******/
	
	// Set the _stallBit
	if ((ReadQueue.size() + NO_READS_PER_CYCLE > MAX_READQ_SIZE) ||
		(WriteQueue.size() + NO_WRITES_PER_CYCLE > MAX_WRITEQ_SIZE))
	{
		_stallBit = 1;
	}
	else
		_stallBit = 0;
}

int MemorySubsystem :: IsReadDepSatisfied(int bankNum)
{
	return 1;

	// Check how many read access are currently active for this bank
	// and how many are allowed.

	int readAccessNum;
	if (BankReadAccess.size() == 0)
		readAccessNum =0;
	else
		readAccessNum = BankReadAccess[bankNum];
 
	if (readAccessNum < NO_PARALLEL_READ_ACCESS)
	{
		// Now check if whether READ-WRITE conflict exists
		int writeAccessNum;
		if (BankWriteAccess.size() == 0)
			writeAccessNum = 0;
		else
			writeAccessNum = BankWriteAccess[bankNum];

		if (READ_WRITE_CONFLICT) // Bank uses same data path for read/write
		{
			if ((readAccessNum + writeAccessNum) < NO_PARALLEL_READ_ACCESS)
			{
				// Update bank info
				if (BankReadAccess.size() == 0)
					BankReadAccess[bankNum] = 1;
				else
					BankReadAccess[bankNum] = BankReadAccess[bankNum] + 1;

				return 1;
			}
		}
	}
 
	// This read operation needs to be stalled
	return 0;
	 
}


int MemorySubsystem :: IsWriteDepSatisfied(int bankNum)
{
	return 1;

	// Check how many write access are currently active for this bank
	// and how many are allowed.

	int writeAccessNum;
	if (BankWriteAccess.size() == 0)
		writeAccessNum = 0;
	else
		writeAccessNum = BankWriteAccess[bankNum];

	if (writeAccessNum < NO_PARALLEL_WRITE_ACCESS)
	{
		// Now check if whether READ-WRITE conflict exists

		int readAccessNum;
		if (BankReadAccess.size() == 0)
			readAccessNum = 0;
		else
			readAccessNum = BankReadAccess[bankNum];

		if (READ_WRITE_CONFLICT) // Bank uses same data path for read/write
		{
			if ((readAccessNum + writeAccessNum) < NO_PARALLEL_WRITE_ACCESS)
			{
				// Update bank info
				if (BankWriteAccess.size() == 0)
					BankWriteAccess[bankNum] = 1;
				else
					BankWriteAccess[bankNum] = BankWriteAccess[bankNum] + 1;

				return 1;
			}
		}
	}
 
	// This read operation needs to be stalled
	return 0;	 
}


int MemorySubsystem :: getBank(long address)
{
	return ((( address /2) % 2) + 2);
	// Find the address range.
	long addressRange = HIGH_ADDRESS - LOW_ADDRESS;

	// Determine the bank size based on total number of banks
	int bankSize = addressRange/NO_OF_BANKS;

	int bankNo = (int)((address - LOW_ADDRESS) / bankSize);

	return bankNo;
}

void MemorySubsystem :: flushQueues()
{
	ReadQueue.clear();
	WriteQueue.clear();
}
 