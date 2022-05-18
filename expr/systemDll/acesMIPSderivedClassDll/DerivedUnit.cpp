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

 *file  : DerivedUnit.cpp                                    *

 *created : 3 May, 2000                                      *

 *authors : Prabhat, Srikanth                                *

 *last modified :                                            *

 *                                                           *

 *contents:contains class methods of classes derived from the*

 *         Unit class.                                       *

 *compiled with : g++                                        *

 *for more info :                                            *

 *************************************************************/



#include "DerivedUnit.h"

#include "ControlTable.h"

#include "ExecSemSupportFuncs.h"

#include "NormalInstr.h"

#include "MyString.h"



extern BaseSimObject *getComp(MyString & str);

MemorySubsystem* MemorySubsystemPtr;



extern vector<AddrOperand> IntDestTable;

extern vector<AddrOperand> FltDestTable;

extern bool _END_OF_PROGRAM_;



extern int endOfSim;

extern int BranchMispredicted;

extern OperationLatchData BranchOperation;



extern void InitStackPointer(AddrOperand & origReg, AddrOperand & renamedReg);



 

int StartUp = 0;

MyInt global_pred;

 __declspec( dllexport ) long StartPC;

Map<Unit*,UnitList> ForwardingTable; // Destination Unit, Source Unit for forwarding



int globalMiscRegNum = 31;

map<MyString, long> GlobalLabels;





void FetchUnit::preprocess()

{

	// Nothing so far

}



void globalCompleteOperation(MyInt id, AddrOperand & dest)

{



}



/*************************************************************

*Decode Unit                                                *

*************************************************************/



DecodeUnit::DecodeUnit(int instBufSize, int numInstFetch, int numInstDecode)

{

	InstBufSize = instBufSize;

	NumInstFetch = numInstFetch;

	NumInstDecode = numInstDecode;

}



DecodeUnit::~DecodeUnit()

{ 

}



void DecodeUnit::flushBuffer()

{

	InstBuffer.clear();

}





void DecodeUnit::preprocess()

{

	InstStrLatchData tempData;



	IN_LATCH_ELEM(0)->read(tempData);

	InstrStr tempInstr = tempData.getLatchData();            

	vector<MyString> instrns = tempInstr.getStr();



	// Count the total number of valid operations (not NOPs)

	// and check whether all of them can fit into Completion 

	// Buffer and Instruction Buffer. If not, stall the Decode Unit.



	int totalOps = instrns.size();



	int currBufSize = InstBuffer.size();



	if ((InstBufSize - currBufSize) < (NumInstFetch + totalOps) )

	{

		// Stall the Fetch Unit...so set busy bit 

		globalControlTable.setProperty(this, BusyBit, 1);		

	}

	else

	{

		// Mark stallBit 0 (if it's stalled in the earlier cycle)

		globalControlTable.setProperty(this, BusyBit, 0);

	}	

}



void DecodeUnit::init()

{		

}



/*************************************************************

*OpReadUnit                                                  *

*************************************************************/



OpReadUnit::OpReadUnit(int bufSize, int numInstIn, int numInstOut)

{

	BufSize = bufSize;

    NumInstIn = numInstIn;

	NumInstOut = numInstOut;

}





void OpReadUnit::flushBuffer()

{

	ReservationStation.clear();

}





OpReadUnit::~OpReadUnit()

{

}





void OpReadUnit::preprocess()

{

	InstructionLatchData tempData1;

	

	IN_LATCH_ELEM(0)->read(tempData1);

	

	int currBufSize = ReservationStation.size();



	MyOperList & list1 = tempData1.getLatchData();

	int currLatchSize = (list1._operList).size();



	if ((BufSize - currBufSize) < currLatchSize)

	{

		globalControlTable.setProperty(this, BusyBit, 1);		

	}

	else

	{

		globalControlTable.setProperty(this, BusyBit, 0);

	}

}





/*************************************************************

*ExecuteUnit                                                  *

*************************************************************/



ExecuteUnit::ExecuteUnit(int resStationSize, int numInstIn, int numInstOut)

{

	ResStationSize = resStationSize;

	_count = 0;

	

	_prevOpcode = getOpcodeIndex("NOP");

}





void ExecuteUnit::flushBuffer()

{

	ReservationStation.clear();

}





ExecuteUnit::~ExecuteUnit()

{

}



void ExecuteUnit::preprocess()

{

	OperationLatchData indata;

	IN_LATCH_ELEM(0)->read(indata);

	

	int count = 0;

	int tempOpcode = indata.getOpCode();

	

	if (!isNOP(tempOpcode))

		count = 1;



	int currBufSize = ReservationStation.size();



	if (((ResStationSize - currBufSize) < count))

		globalControlTable.setProperty(this,BusyBit,1);		

	else

		globalControlTable.setProperty(this, BusyBit, 0);

}



void ExecuteUnit::completeOperation(MyInt id, AddrOperand & dest)

{

	globalCompleteOperation(id,dest);

}





void ExecuteUnit::completeSpecialOperation(MyInt id, OperationLatchData & indata)

{



}





/*************************************************************

 * SimpleStageUnit                                            *

 *************************************************************/

SimpleStageUnit::SimpleStageUnit()

{

}



SimpleStageUnit::SimpleStageUnit(int x, int y, int z)

{

}



SimpleStageUnit::~SimpleStageUnit()

{

}



void SimpleStageUnit::preprocess()

{

}







/*************************************************************

* Branch Unit                                               *

*************************************************************/



void BranchUnit::flushBuffer()

{

	ReservationStation.clear();

}



BranchUnit::BranchUnit(int resStationSize,int numInstIn, int numInstOut)

{

	ResStationSize = resStationSize;

}



BranchUnit::~BranchUnit()

{

}





void BranchUnit::preprocess()

{

	OperationLatchData indata;

	IN_LATCH_ELEM(0)->read(indata);

	

	int count = 0;

	int tempOpcode = indata.getOpCode();

	int latency = getLatency(tempOpcode);

	

	if (!isNOP(tempOpcode))

		count = 1;



	int currBufSize = ReservationStation.size();



	if ((ResStationSize - currBufSize) < count)

	{

		// Mark branch unit as busy.

		globalControlTable.setProperty(this,BusyBit,1);		

	}

	else

	{

		// Mark branch unit as not busy.

		globalControlTable.setProperty(this, BusyBit, 0);

	}

}



void BranchUnit::completeBranchOperation(OperationLatchData oper, int status)

{

	long instAddr = oper.getIRData()._id;



	MyInt prediction;

	MyInt pred;

	BranchPredictor.read(prediction, instAddr);



	int tempOpcode = oper.getOpCode();



	if (isBranch(tempOpcode) || isGotoOp(tempOpcode))

	{

		if (((prediction._id == _PREDICTED_TAKEN_) && (status == _YES_))

			|| ((prediction._id == _PREDICTED_NOT_TAKEN_) && (status == _NO_)))

		{

			// Prediction successful

			pred._id = _ASPREDICTED_;

		}

		else

		{

			// Mispredicted

			pred._id = _MISPREDICTED_;

		}

	}

	

	if (isBranch(tempOpcode) || isGotoOp(tempOpcode) )

	{

		// Update branch predictor 	

		//BranchPredictor.write(pred, instAddr);


		if (pred._id == _MISPREDICTED_)

		{

			// Inform controller to flush at the end of this cycle 4/22/03 Sudeep

			BranchMispredicted = _TRUE_;

			global_pred = pred;

			BranchOperation = oper;

		}

		else

		{
		
			BranchPredictor.write(pred, instAddr);
		
		}

	}

}





void BranchUnit::completeSpecialOperation(MyInt id, OperationLatchData & indata)

{

}



void BranchUnit::completeOperation(MyInt id)

{

}





void BranchUnit::completeOperation(MyInt id, AddrOperand & dest)

{

	globalCompleteOperation(id,dest);

}















/*************************************************************

*LdSt Unit                                               *

*************************************************************/



LoadStoreUnit::LoadStoreUnit(int resStationSize, int numInstIn, int numInstOut)

{

	memSubsystem = MemorySubsystemPtr;



	ResStationSize = resStationSize;

}





void LoadStoreUnit::flushBuffer()

{

	ReservationStation.clear();

	memSubsystem->flushQueues();

}





LoadStoreUnit::~LoadStoreUnit()

{

}



void LoadStoreUnit::preprocess()

{

	OperationLatchData indata;

	

	IN_LATCH_ELEM(0)->read(indata);

	

	int count = 0;

	int tempOpcode = indata.getOpCode();

	int latency = getLatency(tempOpcode);

	

	if (!isNOP(tempOpcode))

		count = 1;



	int currBufSize = ReservationStation.size();



	if ((ResStationSize - currBufSize) < count)

	{

		globalControlTable.setProperty(this, BusyBit, 1);		

	}

	else

	{

		globalControlTable.setProperty(this, BusyBit, 0);

	}

}





void LoadStoreUnit::completeOperation(MyInt id, AddrOperand & dest)

{

	globalCompleteOperation(id,dest);

}





void LoadStoreUnit::completeStoreOperation(MyInt id, AddrOperand & dest)

{

}



/*************************************************************

*Completion Unit                                               *

*************************************************************/



WriteBackUnit::WriteBackUnit(int wbBuffSize, int numInstIn, int numInstOut)

{

     WbBuffSize = wbBuffSize;

}





void WriteBackUnit::flushBuffer()

{

	ReservationStation.clear();

}





WriteBackUnit::~WriteBackUnit()

{

}



void WriteBackUnit::preprocess()

{

	

}





void WriteBackUnit::completeOperation(MyInt id, AddrOperand & dest)

{

	globalCompleteOperation(id,dest);

}





/*************************************************************

*Arch Unit                                                  *

*************************************************************/

ArchUnit :: ArchUnit(const ArchUnit & a)

{

	CompoundUnit::CompoundUnit(a);

}



void ArchUnit::initPC(long addr)

{

	((ControlUnit *)UNIT_ELEM(0))->initPC(addr);

}



void ArchUnit::singleStep()

{  

	ControlUnit *cntrl = ((ControlUnit *)UNIT_ELEM(0));

	

	cntrl->doStep();

	

	if ((cntrl->endOfSimulation()) && (cntrl->endOfProg()))

		endOfSim = 1;

	

	

	if (clock_count%10000==0)

		fprintf(stderr,"cycles=%i\n",clock_count);

}



void ArchUnit :: reset()

{

	// Call the reset of the control unit.

	//

	UNIT_ELEM(0)->reset();

}







/*************************************************************

*Control Unit                                               *

*************************************************************/



ControlUnit::  ControlUnit(){

	memSubsystem = new MemorySubsystem();

	MemorySubsystemPtr = memSubsystem;



}



ControlUnit::~ControlUnit(){}



ControlUnit::ControlUnit(const ControlUnit & a)

{

	CompoundUnit::CompoundUnit(a);

}



int ControlUnit::is(ClassType ctype)

{

	return ((ctype == _CONTROL_UNIT_)?_YES_:(CompoundUnit::is(ctype)));

}



int operator ==(const ControlUnit &a, const ControlUnit &b)

{ 

	return _NO_; 

}



void ControlUnit :: print(ostream & out)

{

	out << endl << "Printing the contents of ControlUnit Unit: " << endl;

	CompoundUnit :: print(out);

	out << endl << "#********************#" << endl;

}



int ControlUnit :: isEq(Unit *ptr)

{

	if (_NULL_ != ptr)

	{

		if (ptr->is(_CONTROL_UNIT_)) return _YES_;

		

		/* to do: redefine this portion */

	}

	return _NO_;

}



void ControlUnit::init()

{

	CompoundUnit::init();

	

//	IntBusyRegTable = new BusyRegTable;

//	FltBusyRegTable = new BusyRegTable;



	MiscBusyRegTable = new BusyRegTable;

}



void ControlUnit :: reset()

{

	// Currently, we only reset the storage elements (i.e. register file, memory).



	StorageListIterator s1, s2;

	s1 = _storage.begin();

	s2 = _storage.end();



	while (s1 != s2)

	{

		(*s1)->reset();

		s1++;

	}

}



void ControlUnit::addStorage(Storage *st)

{

	_storage.push_back(st);

}   



void ControlUnit::initPC(long addr)

{

	// set global variables for the Program Address Generate Unit

	StartUp = 1;

	

	StartPC = addr;



	// set Execute Bit of the Program Address Generate Unit, so that its doStep is called

	resetAllExecuteBits;

	resetAllNextCycleExecuteBits;

	resetAllStallBits;

	resetAllBusyBits;

	resetAllPipeNumBits;

	

	//globalControlTable.print(cout);

	

	ControlTableEntry & tempEntry = globalControlTable.getEntry(FETCH_STAGE_NUM,0);

	Unit * fetchUnit = tempEntry.getUnit();

	globalControlTable.setProperty(fetchUnit,ExecuteBit,1);

	//globalControlTable.print(cout);

}



void ControlUnit :: updateStorageElements()

{

	// Get PC Latch

	MyString pcStr("pcLatch");

	Latch * pc = (Latch*)getComp(pcStr);

	pc->doUpdate();



	// Update all the storage elements



	StorageListIterator sIterBegin, sIterEnd;

	sIterBegin = _storage.begin();

	sIterEnd = _storage.end();

	

	while (sIterBegin != sIterEnd)

	{

		(*sIterBegin)->doUpdate();

		sIterBegin++;

	}



//	IntBusyRegTable->doUpdate();

//	FltBusyRegTable->doUpdate();

	MiscBusyRegTable->doUpdate();



	// Update old Busy reg tables

//	vector<BusyRegTable *>::iterator i1 = ListOfIntBusyRegTables.begin();

//	while(i1 != ListOfIntBusyRegTables.end())

//	{

//		(*i1)->doUpdate();

//		i1++;

//	}



//	vector<BusyRegTable *>::iterator i2 = ListOfFltBusyRegTables.begin();

//	while(i2 != ListOfFltBusyRegTables.end())

//	{

//		(*i2)->doUpdate();

//		i2++;

//	}

	

	// ADVANCE THE GLOBAL CLOCK

	clock_count++;

}





void ControlUnit::updateStatistics()

{

	//globalArchStatsDB.execAllTrackStats();

}





void ControlUnit::testStatusAtEndOfCycle()

{

	// This probably is not needed anymore once the branch prediction 

	// is moved to the fetch unit. Still need to verify this.

//	checkForTakenBranch();

	checkForEndOfProg();

	globalControlTable.update();

}



void ControlUnit :: checkForEndOfProg()

{

	// the next address to be fetched from is present in the PC Latch Use this to check for end of prog.

	if (endOfProg())

	{

		setNextCycleExecuteBit(FETCH_STAGE_NUM,0,0);

		

		OTHER_LATCH_ELEM(0)->reset();

	}

	else 

		setNextCycleExecuteBit(FETCH_STAGE_NUM,0,1);

}



void ControlUnit :: checkForTakenBranch()

{

	// If a branch occurs, then all instructions in the ISSUE-Execute_first_stage latches, 

	// Decode-ISSUE latches and Fetch-Decode latches have to be squashed 

	// The above is accomplished by setting the the nextCycleExecute

	// bits of units from Decode to the branch stage 0

	// Further, remove the destinations for these instructions from the globalControlTable,

	// and listOfDestinations

	

	if (Branch)

	{

		// Do not run the decode unit next cycle

		setNextCycleExecuteBit(1,0,0);

		

		resetAllStallBits;

	}

}





void ControlUnit::preprocess()

{

	int rowNum,colNum;

	Row tempList;

	

	for(rowNum=MaxNumStages-1;rowNum>=0;rowNum--)

	{

		// Copy an entire row (ith stage units of all pipes) from the 

		// globalControlTable into a temporary list.

		globalControlTable.getIthRow(rowNum,tempList);

		

		RowIterator i1,i2;

		i1 = tempList.begin();

		i2 = tempList.end();

		colNum = 0;

		while(i1 != i2)

		{

			// Dummy entries for pipes with lesser than the MaxNumStages have Unit * = 0

			// Only if the execute bit is 1, does the input latch contain meaningful data

			// using which the preprocess function is going to set the stall bit.

			if (i1->getUnit()) 

			{

				(i1->getUnit())->preprocess();

			}

			i1++;

			colNum++;

		}

		tempList.clear();

	}

}





// Modified by SAH on 21st Feb, 2001

//

// The problem was:

//

// When an end of program is detected, the following steps are to be taken:

//

// 1) Stop fetching (or fetch NULLs)

// 2) Let the pipeline drain

// 3) Test for full pipeline drain condition and then exit simulation.

//

// However, the earlier version of ELF simulator was wrong becos:

//

// either:

//

// 1) It would stop fetching while also reseting the fetch-decode latch.

//    This had the effect of removing the last instruction from the pipeline.

//

// or

//

// 2) It would stop fetching but not reset the fetch-decode latch in the next cycle.

//    This had the effect of looping over the last instruction (present in the fetch-decode latch).

//

// The solution presented is:

//

// When an end of program is detected

//

// 1) Continue fetching NOPs. However, change fetch to not increment the pc, when the pc is invalid.

// 2) Change the end of program detection condition, to check if:

//    a) The pc value is invalid.

//    b) The fetch-decode value is invalid.





int ControlUnit::endOfProg()

{

	if (_END_OF_PROGRAM_)

		return _YES_;

	else

		return _NO_;



	InstStrLatchData tempinstrData;

	OTHER_LATCH_ELEM(0)->read(tempinstrData);

	InstrStr temp = tempinstrData.getLatchData();

	if (temp.isValid() == _YES_)

		return _NO_;



	PCLatchData tmpaddrData;

	OTHER_LATCH_ELEM(1)->read(tmpaddrData);

	PgmAddr tmp = tmpaddrData.getLatchData();

	if (tmp.isValid() == _YES_)

		return _NO_;

	return _YES_;

}



int ControlUnit::endOfSimulation()

{

	int endSim = endOfProg();

	

	if  (endSim) 

		return 1;

	else 

		return 0;

}





void ControlUnit::flush()

{

	// Flush all the latches and reservation stations till the output of opread units

	int rowNum, colNum;

	Row tempList;

	RowIterator i1, i2;

				

	for(rowNum = 0; rowNum <= OPREAD_STAGE_NUM; rowNum++)

	{

		tempList.clear();



		globalControlTable.getIthRow(rowNum,tempList);

		

		i1 = tempList.begin();

		i2 = tempList.end();

		colNum =0;

		while(i1 != i2)

		{		

			Unit * currUnit = i1->getUnit();



			if (currUnit)

			{

				// Flush reservation station

				currUnit->flushBuffer();



				// Stall these units as well

				globalControlTable.setProperty(rowNum, colNum, StallBit, 1);



				// Flush output latches

				LatchList * outLatches = currUnit->getLatchList(_OUT_L_);

				LatchListIterator lIterBegin, lIterEnd;

				lIterBegin = outLatches->begin();

				lIterEnd = outLatches->end();

				

				while (lIterBegin != lIterEnd)

				{

					(*lIterBegin)->reset();

					lIterBegin++;

				}

			}

			i1++;

			colNum++;

		}	

	}

}