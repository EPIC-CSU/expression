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
 *file  : DerivedUnit.h                                      *
 *created : 3 May,2000                                    *
 *authors : Prabhat, Srikanth                                *
 *last modified :                                            *
 *                                                           *
 *contents:contains class definition for classes derived from*
 *         Unit class.                                       *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef _DERIVED_UNIT_H_
#define _DERIVED_UNIT_H_

#include "Unit.h"

#include "ExecSemantics.h"
#include "ExecSemSupportFuncs.h"

#include "DerivedAddrData.h"
#include "DerivedLatchData.h"

#include "DynamicScheduler.h"
#include "MemorySubsystem.h"

#include "IssueUnit.h"

extern int StartUp;
extern __declspec( dllexport ) long StartPC;
extern Map<Unit*,UnitList> ForwardingTable;
extern int getOpcodeIndex(char *str);


//Fetch Unit passes the current instruction pointer in the pc latch down in
//the pipeline, and updates the pcLatch withthe new instruction pointer

class FetchUnit : public SimpleUnit
{
private:
	int NumInstFetch; // Number of instruction fetch per cycle (default 4)

public:
	FetchUnit(int instBufSize=0, int numInstIn=0, int numInstOut=0) {};
	~FetchUnit(){};
	
	virtual void doStep();
	void preprocess();
};

//Decode Unit does the meat of the job. It looks at the instruction, sees which
//pipe the instruction belongs to (e.g. ALU, FPAdd etc.), checks to see if the
//top unit in that pipe is ready to accept a new instruction, and passes it on 
//to the correct pipe if the path is clear. (path clear means that the unit will
//get its source operands through forwarding paths etc.) 

class DecodeUnit : public SimpleUnit
{
private:

	// Instruction buffer between Fetch and Decode Units, default size 12
    // vector<BaseOperation *> InstBuffer;
	vector<MyString> InstBuffer;

	int InstBufSize;    // Instruction Buffer Size

	int NumInstFetch;   // Number of instruction fetched per cycle (default 4)
	                    // Read those many from the Fetch to Decode Latch

	int NumInstDecode;  // Number of instruction decode per cycle (default 2) 

public:
	DecodeUnit(int instBufSize, int numInstFetch, int numInstDecode);
	~DecodeUnit();

	void init();
	virtual void doStep();
	void flushBuffer();
	void preprocess();
};


class OpReadUnit : public SimpleUnit
{
private:
	// Dispatch Buffer
	vector<OperationLatchData> ReservationStation;

	// Size of the reservation station
	int BufSize;

    // Number of instructions coming in per cycle
	int NumInstIn;

	// Number of instructions going out per cycle
	int NumInstOut;

public:
	OpReadUnit(int bufSize, int numInstIn, int numInstOut);
	~OpReadUnit();

	virtual void doStep();
	void flushBuffer();
	void preprocess();
};


class BranchUnit : public SimpleUnit
{	
private:
	
	void completeBranchOperation(OperationLatchData, int);
	void completeSpecialOperation(MyInt, OperationLatchData &);

	vector<OperationLatchData> ReservationStation;
	int ResStationSize;
	void completeOperation(MyInt, AddrOperand &);

	void completeOperation(MyInt);

public:
	BranchUnit(int resStationSize, int numInstIn=0, int numInstOut=0);
	~BranchUnit();
	
	virtual void doStep();
	void flushBuffer(); 
	void preprocess();
};



// The following class will be used by both Simple Units. The Simple Units have
// ResStationSize (default 1) number of entries. 

class ExecuteUnit : public SimpleUnit
{	
private:
	int _count;
	int _prevOpcode;
	void completeOperation(MyInt, AddrOperand &);
	void completeSpecialOperation(MyInt, OperationLatchData &);
    
	vector<OperationLatchData> ReservationStation;
	int ResStationSize;

public:
	ExecuteUnit(int resStationSize, int numInstIn=0, int numInstOut=0);
	~ExecuteUnit();
	
	virtual void doStep();
	void flushBuffer(); 
	void preprocess();
};


// Load Store Unit has a reservation station of size ResStationSize(default size 2)
//Each entry in the resrvation station is of type LdStInsn, that has the
//instruction, the address, the data, and the state bit. 
class LdStInsn;

class LoadStoreUnit : public SimpleUnit
{
private:
	vector<LdStInsn> ReservationStation;

	int ResStationSize;
	
	void completeOperation(MyInt, AddrOperand &);
	void completeStoreOperation(MyInt id, AddrOperand &);

public:

	MemorySubsystem* memSubsystem;

	LoadStoreUnit(int resStationSize, int numInstIn=0, int numInstOut=0);
	~LoadStoreUnit();

	virtual void doStep();
	void flushBuffer(); 
	void preprocess();
};



// Transfers input to output in a cycle
class SimpleStageUnit : public SimpleUnit
{
private:
	int _count;

public:
	SimpleStageUnit();
	SimpleStageUnit(int x, int y, int z);
	~SimpleStageUnit();

	virtual void doStep();
	void preprocess();
};


class WriteBackUnit : public SimpleUnit
{	
private:
	void completeOperation(MyInt, AddrOperand &);

	vector<OperationLatchData> ReservationStation;
	int WbBuffSize;

public:
	WriteBackUnit(int wbBuffSize, int numInstIn=0, int numInstOut=0);
	~WriteBackUnit();

	virtual void doStep();
	void flushBuffer(); 
	void preprocess();
};


class ArchUnit : public CompoundUnit 
{
public :
	ArchUnit(){}
	~ArchUnit(){}
	ArchUnit :: ArchUnit(const ArchUnit & a);
	void initPC(long addr);


	virtual int is(ClassType ctype)
	{return((ctype == _ARCH_UNIT_) ? _YES_ : (CompoundUnit :: is(ctype)));}
	
	void singleStep();
	void reset();
	virtual void doStep();
};

class ControlUnit : public CompoundUnit 
{	
	StorageList _storage;
	MemorySubsystem* memSubsystem;

public:
	ControlUnit() ;
	ControlUnit(const ControlUnit &);
	~ControlUnit();
	virtual int is(ClassType ctype);
	friend int operator ==(const ArchUnit &a, const ArchUnit &b);
	virtual void print(ostream &out); 
	virtual int isEq(Unit * ptr);
	
	virtual void init();
	virtual void addStorage(Storage *st);
	void initPC(long addr);
	
	void updateStorageElements();
	void updateStatistics();
	
	void testStatusAtEndOfCycle();
	void checkForEndOfProg();
	void checkForTakenBranch();
	
	void preprocess();
	void reset();
	void doStep();
	
	int endOfProg();
	int endOfSimulation();

	void flush();
};

#endif
