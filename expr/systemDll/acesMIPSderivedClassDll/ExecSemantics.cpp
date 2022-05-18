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
 *file  : ExecSemantics.cpp                                  *
 *created : 3 May, 2000.                                     *
 *authors : Prabhat, Srikanth                                *
 *last modified : Sudeep (05/22)                             *
 *                                                           *
 *contents:contains class methods for Unit class & other     *
 *         classes derived from it.                          *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/


#include <map>


#include "DerivedLatchData.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "DerivedOpExecSemantics.h"
#include "ControlTable.h"
#include "ExecSemantics.h"
#include "ExecSemSupportFuncs.h"
#include "CompName.h"
#include "DerivedStorage.h"


#include "StatList.h"
#include "ArchStats.h"
#include "DynamicScheduler.h"

 

extern ArchStatsDB globalArchStatsDB;
extern OpCodeTable globalOpCodeTable;
extern MemoryModule * mainMemory;
extern Storage* MiscRegFile;
extern Storage* GPRFile;
extern Storage* FPRFile;
extern MyInt global_pred;
vector<AddrOperand> IntDestTable;
vector<AddrOperand> FltDestTable;



int Branch = _FALSE_;

int BranchMispredicted = _FALSE_;

OperationLatchData BranchOperation;



bool _END_OF_PROGRAM_ = false;

int spaceAvailable = _TRUE_;



unsigned long clock_count = 0;

unsigned long lockstep_clock_count = 0;



int global_memory_stalls = 0;



bool findDest(AddrOperand & dest, vector<AddrOperand> :: iterator & it);

MyLinkedList<AddrOperand> listOfDestinations;





void CompoundUnit :: doStep()

{

	UnitListIterator unitIterBegin, unitIterEnd;

	unitIterBegin = _units.begin();

	unitIterEnd = _units.end();

	

	while (unitIterBegin < unitIterEnd)

    {

		(*unitIterBegin)->doStep();

		unitIterBegin++;

    }

}



void FetchUnit :: doStep()

{

	// IN_LATCH_ELEM(0) is the PC latch before this unit. OUT_LATCH_ELEM(0) is the

	// latch between IF and ID stages.

	// read current pc from IN_LATCH_ELEM(0) and write to the IF/ID latch

	// compute new pc and write it to the PC latch: computation as follows:

	// if Branch is true, then do not write to PC latch, Branch Unit will write to it

	// else get next address and write that pc to the PC_latch

	

	vector<MyString> instrns;

	vector<MyString> tempInstrns;

	MyString brInstr;

	long pcaddr;

	static int done = 0, instfetched = 1;

	bool branch = false;



	

	if (debugSIMULATOR)

	{

		cout << endl << "FETCH UNIT" << endl;

		cout << "-----------" << endl;

	}





	if (StartUp)

	{

		pcaddr = StartPC;

		writeToPgmCounterLatch(StartPC);

		StartUp = 0;

	}

	else

	{

		pcaddr = readFromPgmCounterLatch();

	}

		

    tempInstrns = readFromMem(pcaddr, 4, done, instfetched);



	/* Check if any of the instructions fetched is a branch, also check

	its status in BTB if it is a branch */



	long addr = pcaddr;

	vector<MyString>::iterator instrPtr = tempInstrns.begin(); 

	MyInt prediction;



	for(int i=0; i<instfetched; i++) {

		MyString currInst = (*instrPtr).getStr();

		instrns.push_back(currInst);

		if (isBranchInstr(currInst) ) {

			addr = pcaddr + i*4; // location of branch instruction

			BranchPredictor.read(prediction, addr);

			brInstr = currInst;

			if (prediction._id == _PREDICTED_TAKEN_) branch = true;

			if (debugSIMULATOR)
				
			{
				if (!branch) cout << "Branch Not Taken predicted for the branch instruction...\n" << endl;
				else 
				cout << "Branch Taken predicted for the branch instruction...\n" << endl;
			}

		}

		else if (isGotoInstr(currInst) ) {

			brInstr = currInst;

			branch = true;

			if (debugSIMULATOR)				
			{
				cout << "Branch Taken predicted for the branch instruction...\n" << endl;
			}

		}



		if (branch) break;

		instrPtr++;

	}



	writeToFetDecLatch(instrns);



	if (done) 

	{		

		if (!branch) 

		{

			if (instfetched == 0)

			{

				// Means we have encountered an invalid address

				pcaddr = -1;

			}

			else 

				pcaddr = pcaddr + 4*instfetched;

			

			writeToPgmCounterLatch(pcaddr);

		}

		else

		{

			// Set the PC with the target address if it is not branch misprediction

			

			long tgtAddr = getTargetAddress(brInstr);

			writeToPgmCounterLatch(tgtAddr);

		}



	

		if (debugSIMULATOR)

		{

			cout << " Fetched the following instructions from I-Cache" << endl;

			vector<MyString>::iterator ins;

			for (ins=instrns.begin(); (ins!=instrns.end());ins++) 

				cout << (*ins).getStr() << endl ;







		}

		

	}

	else

	{

		if (debugSIMULATOR)

			cout << "Processing I-Cache Miss............................................" << endl;

	}





	// Let the stat collector know that this unit has executed.

	//

	doStats();

	globalControlTable.setProperty(this, ExecuteBit, 1);

}



void DecodeUnit :: doStep()

{
	vector<MyString> instrns;
	instrns = readFromFetDecLatch();


 	int count = 0; 	


	vector<MyString>::iterator ins1;


	for(ins1=instrns.begin(); (ins1!=instrns.end());ins1++) {
		InstBuffer.push_back((*ins1));
		count++;
	}


	if (InstBuffer.size() > InstBufSize) {
		cout << "Instruction Buffer Overflow" << endl;
		assert(0);
	}
	
	if (debugSIMULATOR) 
	{	
		cout << endl;
		cout << "DECODE UNIT" << endl;
		cout << "-----------" << endl;

		
		if ( ! InstBuffer.empty())

		{

			cout << endl << "Printing operations in Decode Buffer :" << endl;

			

			vector<MyString>::iterator p1,p2;

			p1 = InstBuffer.begin();

			p2 = InstBuffer.end();		

			

			while (p1 != p2) 

			{

				long Addr = 0;

				MyString Instr;

				

				splitAddrInstr((*p1),Instr,Addr);

				

				//printf("Addr: %d, Instr: %s \n", Addr, Instr.getStr());
				cout << "Addr: " << Addr << ", Instr: "<<Instr.getStr()<<endl;

				p1++;

			}

		}

		else

			cout << "Decode Buffer is empty." << endl;

	}



	UnitList & childrenList = globalNetList.getSuccessorList(this);

	int childSize = childrenList.size();



	vector<InstructionLatchData> tempData;

	for (int j= 0; j < childSize; j++)

	{

		InstructionLatchData ilData;

		tempData.push_back(ilData);

	}



	int i = 0;

	vector<bool> busy;

	while(i < childSize)

	{

		busy.push_back(false);

		i++;

	}



	MyString nameStr;

	Latch * lptr;

	OperationLatchData tempElem;



	int numDecoded = 0;



	vector<MyString>::iterator i1, i2, end;

	i1 = InstBuffer.begin();

	end = InstBuffer.end();



	while ((i1 != end) && (numDecoded < NumInstDecode))

	{		

		MyString currOper;

		long Addr;

		Val opreadVal;



		splitAddrInstr((*i1),currOper,Addr);

		

		// currOper has the instruction

				

		if (!currOper.isEmpty())

		{

			char tmpOpcode[40];

			char *tmpinstr;

			

			tmpinstr = currOper.getStr();



			// Get the OpCode in the instruction and its corresponding index..

			sscanf(tmpinstr,"%s",tmpOpcode);

			int tempOpcode = getOpcodeIndex(tmpOpcode);



			if (isControlOp(tempOpcode)) {

				processControlOp(tmpinstr, tempOpcode, tempElem);

			}

			else  

				processDataAndMemoryOp(tmpinstr, tempOpcode, tempElem);

			

			MyInt addr;

			addr._id = Addr;

			tempElem.setIRData(addr);

			

			bool issued = false;

			

			vector<AddrOperand>::iterator it;

			

			// Check RAW Hazard

			bool raw = false;

			bool srcReady = true;

			Vector<PredicateAddrDataUnion>::iterator s1,s2;

			

			s1 = (tempElem.getOperand2()).begin();

			s2 = (tempElem.getOperand2()).end();

			

			AddrOperand  src;

			

			while (s1 != s2)

			{

				if ( (*s1).getType() == _ADDRTYPE_ )

				{

					src = (*s1).getAddr();

					raw = findDest(src, it);

					if (raw) 

					{

						srcReady = false;

						break;

					}

				}



				s1++;

			}

			

			if (!srcReady) break;

			

			// Check WAW Hazard

			bool waw = false;

			if (tempElem.getOperand1().size() > 0)

			{

				AddrOperand dest = (tempElem.getOperand1())[0];

				waw = findDest(dest, it);	

			}

			

			if (waw) break;

		



			UnitListIterator c1 = childrenList.begin();

			i = 0;

			while(c1 != childrenList.end())

			{

				OpCodeList & opList = (*c1)->getOpList();

				if ((opList.hasOpCode(tempOpcode)) && !busy[i])

				{

					(tempData[i].getLatchData()._operList).push_back(tempElem);

					busy[i] = true;

					issued = true;

					break;

				}



				c1++;

				i++;

			}



			

			if (issued)

			{

				if (debugSIMULATOR)

				{

					if ( ! currOper.isEmpty() )

						cout << endl << "The instruction being currently decoded is : " << currOper.getStr() << endl;

					else

						cout << endl << "The instruction being currently decoded is a NOP" << endl;

				}



				// Mark the destination as used

				if (tempElem.getOperand1().size() > 0)

				{

					AddrOperand dest = (tempElem.getOperand1())[0];



					if (dest.getFieldType() == _ADDR_INT_)

					{

						IntDestTable.push_back(dest);

					}

					else if ((dest.getFieldType() == _ADDR_FLT_) || (dest.getFieldType() == _ADDR_DBL_))

					{

						FltDestTable.push_back(dest);

					}

				}



				i2 = i1;

				

				if (InstBuffer.size() == 1)

				{

					InstBuffer.clear();

					break;

				}

				else

					i1 = InstBuffer.erase(i2);

			if (isControlOp(tempOpcode)) {
				tempElem.reset();
				numDecoded++;
				break;
			} else 
			{
				tempElem.reset();
				numDecoded++;
			}

			}
			else
				break; // In-order execution
		}
		else
		{
			i2 = i1;

			
			if (InstBuffer.size() == 1)

			{

				InstBuffer.clear();

				break;

			}

			else

				i1 = InstBuffer.erase(i2);

			tempElem.reset();
			numDecoded++;
		}
			//tempElem.reset();
			//numDecoded++;
	}


	// Write in the output latches 

	i = 0;

	while(i < childSize) 

	{

		lptr = OUT_LATCH_ELEM(i);



		if (busy[i])

			lptr->write(tempData[i]);

		else

			addNOP(lptr);

		i++;

	}

  

	doStat(_NUMTIMES_);



	int executeBit;

	if (InstBuffer.size() == 0)

		executeBit = false;

	else

		executeBit = true; 



	globalControlTable.setProperty(this, ExecuteBit ,executeBit);



}





void OpReadUnit::doStep()

{

	InstructionLatchData tempData1;

	

	IN_LATCH_ELEM(0)->read(tempData1);

	IN_LATCH_ELEM(0)->resetOld();



	OperationLatchData tempData;

	OUT_LATCH_ELEM(0)->write(tempData);



	vector<OperationLatchData>::iterator i1,i2;

	

	MyOperList & list1 = tempData1.getLatchData();

	i1 = (list1._operList).begin();

	i2 = (list1._operList).end();

	

	int count = 0;



	// Insert into reservation station

	while (i1 != i2)

	{

		if ((*i1).getOpCode() != 0)

			ReservationStation.push_back(*i1);

		count++;

		i1++;

	}



	vector<OperationLatchData>::iterator j1, j2, end;



	if (debugSIMULATOR)

	{

		cout << endl;

		cout << "OPREAD UNIT" << endl;

		cout << "-----------" << endl;

		

		if ( ! ReservationStation.empty())

		{		

			cout << "Contents of OpRead buffer :" << endl;

			for (j1 = ReservationStation.begin(); j1 != ReservationStation.end(); j1++)

			{

				(*j1).print(cout);

				cout << endl;

			}

			cout << endl;

		}

		else

			cout << "OpRead Buffer is empty." << endl;

	}

	



	j1 = ReservationStation.begin();

	end = ReservationStation.end();



	int dispatched = 0;



	while (j1 != end)

	{

		tempData = *j1;

		

		int tempOpcode = tempData.getOpCode();

		MyInt opId = tempData.getIRData();



		// Debug FIX, should be removed eventually

		if (tempOpcode == 0) return;



		Vector<PredicateAddrDataUnion>::iterator i1,i2, t1;



		t1 = ((*j1).getOperand2()).begin();

		i1 = (tempData.getOperand2()).begin();

		i2 = (tempData.getOperand2()).end();



		AddrOperand  src;

		MyBusyElem srcbusy;

		int toBeRead = 0; // Source operands to be read

		Val opreadVal;



		while (i1 != i2)

		{

			if ( (*i1).getType() == _ADDRTYPE_ )

			{

				

				src = (*i1).getAddr();

				

				if (src.getFieldType() == _ADDR_INT_)

					((VirtualRegFile *)GPRFile)->read(opreadVal, src);

				else if ((src.getFieldType() == _ADDR_FLT_) || (src.getFieldType() == _ADDR_DBL_))

					((VirtualRegFile *)FPRFile)->read(opreadVal, src);

				else

					assert(0);

				

				(*i1).setData(opreadVal);

				(*t1).setData(opreadVal); // Update the src value in reservation station

			}



			opreadVal.reset();

			i1++;

			t1++;

		}



		Latch * lptr = OUT_LATCH_ELEM(0);

	

		j2 = j1;

		

		lptr->write(tempData);

		

		if (ReservationStation.size() == 1)

		{

			ReservationStation.clear();

			break;

		}

		else

			j1 = ReservationStation.erase(j2);

			 

		tempData.reset();

		dispatched++;



		break;  // One issue per cycle

	}



	if (ReservationStation.empty())

		globalControlTable.setProperty(this, ExecuteBit, false);

	else

		globalControlTable.setProperty(this, ExecuteBit, true);

}



void ExecuteUnit::doStep()

{

	OperationLatchData indata;

	IN_LATCH_ELEM(0)->read(indata);

	IN_LATCH_ELEM(0)->resetOld();

	

	OperationLatchData outdata;

	OUT_LATCH_ELEM(0)->write(outdata);



	if (indata.getOpCode() != 0)

		ReservationStation.push_back(indata);



	// Return if nothing to be done.

	if ( ReservationStation.empty() )

	{

		if (debugSIMULATOR)

		{

			cout << endl;

			cout << "EXECUTE UNIT" << endl;

			cout << "------------" << endl;

			cout << "Execution Buffer is empty" << endl;

		}

		

		globalControlTable.setProperty(this, ExecuteBit, 0);

		_prevOpcode = getOpcodeIndex("NOP");

		return;

	}



	if (debugSIMULATOR)

	{

		cout << endl << "EXECUTE UNIT" << endl;

		cout << "------------------" << endl;

		cout << "Contents of Execution Buffer :" << endl;

		vector<OperationLatchData>::iterator i1,i2;

		

		i1 = ReservationStation.begin();

		i2 = ReservationStation.end();



		while(i1 != i2)

		{

			(*i1).print(cout);

			i1++;

		}

		cout << endl;

	}





	// Read from reservation station	

	vector<OperationLatchData>::iterator ptr;

	ptr = ReservationStation.begin();



	OperationLatchData & tempData = (*ptr);

	int tempOpcode = tempData.getOpCode();

	MyInt opId = tempData.getIRData();



	// Debug FIX, should be removed eventually

	if (tempOpcode == 0) return;



	if (!isNOP(tempOpcode))

	{				

		MyString MiscRF("MiscRegFile");

		Storage * MiscRegFile = (VirtualRegFile *)getComp(MiscRF);

		

		AddrOperand & dest = (tempData.getOperand1())[0];

		

		outdata.setOpCode(tempOpcode);

		outdata.addOperand1(dest);

		outdata.setIRData(opId);

		

		PredicateAddrDataUnion destVal;

		destVal.setType(_VALTYPE_);

		

		if (_opList.hasOpCode(tempOpcode))

		{

			if ((_count == 0))		// new-instruction-check state

			{

				_count = _opList.getTiming(tempOpcode);

			}

			

			if (_count <= 1)

			{

				_count = 0;

				_busyBit = false;

				

				//Check for _ANY in any source operand	

				Vector<PredicateAddrDataUnion> :: iterator ptr1,ptr2;

				ptr1 = (tempData.getOperand2()).begin();

				ptr2 = (tempData.getOperand2()).end();

				

				int sizesrclist = (tempData.getOperand2()).size();

				int anyposition = 0;

				

				while (ptr1 != ptr2)

				{

					if  (((*ptr1).getData())._valType == _ANY )

					{

						assert(0);

					}

					ptr1++;

				}

				

				PredicateAddrDataUnion src1 = tempData.getOperand2()[0];



				ComplexInsn complexInst;

				

				if (isComplexOp(tempOpcode, complexInst))

				{

					/** Supports complex instructions using functionality of simple instructions

				    For example, instruction "addsub d1 s1, s2, s3" will be computed as

					"add t1 s1 s2" followed by "sub d1 t1 s3". 

					**/



					vector<SimpleInsn> & simpleInsns = complexInst._simpleInsns;

					vector<SimpleInsn>::iterator i1, i2;

					i1 = simpleInsns.begin();

					i2 = simpleInsns.end();



					Val result;

					

					while (i1 != i2)

					{

						int opcode = globalOpCodeTable.getIndex((*i1)._opcode);

						int dest = (*i1)._dest;

						

						if ((*i1)._src.size() == 1)

						{

							int src = (*i1)._src[0];

							Val src1;



							if (src == -1)

								src1 = result;

							else

								src1 = tempData.getOperand2()[src].getData();



							result = EXECUTE_FUNC(opcode)(src1);

						}

						else if ((*i1)._src.size() == 2)

						{

							int src = (*i1)._src[0];

							Val src1;

							Val src2;



							if (src == -1)

								src1 = result;

							else

								src1 = tempData.getOperand2()[src].getData();



							src = (*i1)._src[1];



							if (src == -1)

								src2 = result;

							else

								src2 = tempData.getOperand2()[src].getData();

							

							result = EXECUTE_FUNC(opcode)(src1, src2);

						}

						else

						{

							assert(0);

						}



						i1++;

					}



					destVal.setData(result);

				}

				else

				{

					// Regular operations

					

					if ( sizesrclist == 0)

					{

						cout << "Warning: Simple Unit recieved null source1" << endl;

					}

					else if ( sizesrclist == 1)

					{

						destVal.setData(EXECUTE_FUNC(tempOpcode)(src1.getData()));

					}

					else if ( sizesrclist == 2)

					{	

						PredicateAddrDataUnion src2 = tempData.getOperand2()[1];

						destVal.setData(EXECUTE_FUNC(tempOpcode)(src1.getData(), src2.getData()));	

					}

					/**********

					else if ( sizesrclist == 3)

					{

					PredicateAddrDataUnion src2 = tempData.getOperand2()[1];

					PredicateAddrDataUnion src3 = tempData.getOperand2()[2];

					destVal.setData(EXECUTE_FUNC(tempOpcode)(src1.getData(), src2.getData(), src3.getData()));	

					}

					*******/

					else

						assert(0);

				}

				

				

				//Output is written into source1 position, because of the structure of the output latch

				//which is one dest and one source

				PredicateAddrDataUnion outval;

				outval.setData(destVal.getData());

				outdata.addOperand2(outval);

				doStats();

				

				OUT_LATCH_ELEM(0)->write(outdata);

				

				if (ReservationStation.size() == 1)

					ReservationStation.clear();

				else

					ReservationStation.erase(ptr);

			}

			else

				_count--;

		}

	}

	else

	{

		// Delete NOP

		if (ReservationStation.size() == 1)

			ReservationStation.clear();

		else

			ReservationStation.erase(ptr);

	}	



	bool executeBit;



	if (ReservationStation.empty())

		executeBit = false;

	else

		executeBit = true;



	globalControlTable.setProperty(this, ExecuteBit, executeBit);

}





void SimpleStageUnit::doStep()

{

	OperationLatchData tempData;

	IN_LATCH_ELEM(0)->read(tempData);

	IN_LATCH_ELEM(0)->resetOld();

		 

	OUT_LATCH_ELEM(0)->write(tempData);  

}





void BranchUnit::doStep()

{

	OperationLatchData tempData;

	IN_LATCH_ELEM(0)->read(tempData);

	IN_LATCH_ELEM(0)->resetOld();

	

	if (tempData.getOpCode() != 0)

		ReservationStation.push_back(tempData);

	

	OperationLatchData outdata;

	OUT_LATCH_ELEM(0)->write(outdata);

	

	// Return if nothing to be done.

	if ( ReservationStation.empty() )

	{

		if (debugSIMULATOR)

		{

			cout << endl;

			cout << "BRANCH UNIT" << endl;

			cout << "-----------" << endl;

			cout <<  "Branch Buffer is empty" << endl;

		}

		

		globalControlTable.setProperty(this, ExecuteBit, 0);

		return;

		

	}

	

	if (debugSIMULATOR)

	{

		cout << endl;

		cout << "BRANCH UNIT" << endl;

		cout << "-----------" << endl;

		cout << "Branch Buffer contents :" << endl;

		vector<OperationLatchData>::iterator i1,i2;

		

		i1 = ReservationStation.begin();

		i2 = ReservationStation.end();



		while(i1 != i2)

		{

			(*i1).print(cout);

			i1++;

		}

		cout << endl;

		

	}

	

	vector<OperationLatchData>::iterator ptr;

	ptr = ReservationStation.begin();

	

	OperationLatchData indata = (*ptr);

	int tempOpcode = indata.getOpCode();

	outdata.setOpCode(tempOpcode);



	// Debug FIX, should be removed eventually

	if (tempOpcode == 0) return;



	Latch * lptr = OUT_LATCH_ELEM(0);

	

	if (!isNOP(tempOpcode))

	{					

		Vector<PredicateAddrDataUnion> :: iterator i1, i2;

		i1 = (indata.getOperand2()).begin();

		i2 = (indata.getOperand2()).end();

		

		//This part tries to read the source operands

		//The only instructions that will have more than 1 source

		//operand are the conditional branches in ELF. 

		//Currently, even conditional branches are only IF statements

		//whose second source comes from the defs files <--- I think:-)))

		

		int opId = (indata.getIRData())._id;		

		

		MyString MiscRF("MiscRegFile");

		Storage * MiscRegFile = (VirtualRegFile *)getComp(MiscRF);		

		

		if (isBranch(tempOpcode))

		{

			Val condEval = EXECUTE_FUNC(tempOpcode)( (indata.getOperand2())[0].getData());

			

			if (condEval._as.integer == 1)

				completeBranchOperation(indata, _YES_);		 

			else

				completeBranchOperation(indata, _NO_);

		}

		else if (isGotoOp(tempOpcode))

		{

			completeBranchOperation(indata, _YES_);

		} 

		else if (isRet(tempOpcode))

		{

			completeOperation(opId);

		}

		else  if (isSpecialOpCode(tempOpcode))

		{

			doSpecialStuff(indata);

		}

		else

		{

			// EXECUTE SINGLE CYCLE INSTRUCTIONS

			outdata.setOpCode(tempOpcode);

			

			if (!(indata.getOperand1()).empty())

			{

				AddrOperand & dest = (indata.getOperand1())[0];

				outdata.addOperand1(dest);

			}

			

			PredicateAddrDataUnion destVal;

			destVal.setType(_VALTYPE_);

			

			

			i1 = (indata.getOperand2()).begin();

			i2 = (indata.getOperand2()).end();

			

			

			int srclistsize = (indata.getOperand2()).size(); 

			

			while (i1 != i2)

			{

				if  ((*i1).getData()._valType == _ANY )

				{

					assert(0);

				}

				i1++;

			}

			

			

			if ( srclistsize == 0 )

			{

				cout << "Warning: Branch Unit recieved null source1" << endl;

			}

			else if ( srclistsize == 1)

			{

				destVal.setData(EXECUTE_FUNC(tempOpcode)((indata.getOperand2())[0].getData()));

			}

			else if ( srclistsize == 2)

			{	

				destVal.setData(EXECUTE_FUNC(tempOpcode)((indata.getOperand2())[0].getData(), ((indata.getOperand2())[1]).getData()));	

			}

			else

			{

				assert(0);

			}

			

			PredicateAddrDataUnion outval;

			outval.setData(destVal.getData());

			outdata.addOperand2(outval);			

			

			doStats();

		}



		lptr->write(outdata);

	}

	else

		addNOP(lptr);

	

	if (ReservationStation.size() == 1)

		ReservationStation.clear();

	else

		ReservationStation.erase(ptr);

	

	bool executeBit;



	if (ReservationStation.empty())

		executeBit = false;

	else

		executeBit = true;



	globalControlTable.setProperty(this, ExecuteBit, executeBit);

}





void LoadStoreUnit::doStep()

{

	OperationLatchData indata;



	IN_LATCH_ELEM(0)->read(indata);

	IN_LATCH_ELEM(0)->resetOld();

	

	LdStInsn insn(indata,_FRESH_);

	

	// Write into reservation station from Issue to simple LoadStoreUnit latch 

	if (indata.getOpCode() != 0)

		ReservationStation.push_back(insn);

	

	// Make output latch NULL

	OperationLatchData outdata;

	OUT_LATCH_ELEM(0)->write(outdata);

	

	// Return if nothing to be done.

	if ( ReservationStation.empty() )

	{

		if (debugSIMULATOR)

		{

			cout << endl;

			cout << "LOAD/STORE UNIT" << endl;

			cout << "---------------" << endl;

			cout << endl << "Load/St buffer is empty." << endl;

		}

		

		globalControlTable.setProperty(this, ExecuteBit, 0);

		return;

	}



	if (debugSIMULATOR)

	{	

		cout << endl;

		cout << "LOAD/STORE UNIT" << endl;

		cout << "---------------" << endl;

		cout << "LdStUnit Buffer contents:" << endl;

		vector<LdStInsn>::iterator i1,i2;

		

		i1 = ReservationStation.begin();

		i2 = ReservationStation.end();

		

		while(i1 != i2)

		{

			(*i1).print(cout);

			i1++;

		}

		cout << endl;

		

	}





	// Read from reservation station

	vector<LdStInsn>::iterator p1, p2, end;



	p1 = ReservationStation.begin();

	end = ReservationStation.end();

    

	int AddrCalcAvail = 1;

	int DCacheAccess = 0;

	OperationLatchData tempData;

	

	while (p1 != end)

	{

		LdStInsn & currInsn = (*p1);

		tempData = (currInsn._ldstInsn);



		int tempOpcode = tempData.getOpCode();

		

		// Debug FIX, should be removed eventually

		if (tempOpcode == 0) return;

		

		if(!isNOP(tempOpcode))

		{

			State state = currInsn._statebit;

			MyInt opId = tempData.getIRData();

			

			AddrOperand & dest = (tempData.getOperand1())[0];

			

			switch(state)

			{

				case _FRESH_ : 

				{

					if(AddrCalcAvail == 1)

					{

						AddrCalcAvail = 0;

						

						int addr;

						int offset;

						

						addr = ((tempData.getOperand2())[0].getData())._as.integer;

						offset = ((tempData.getOperand2())[1].getData())._as.integer;

						

						long address = addr + offset;



						currInsn._address = address;					

						currInsn._statebit = _ADDRCALC_;

					}

					p1++;

				}

				break;

				

				case _ADDRCALC_ :

				{

					if(DCacheAccess == 0)

					{

						DCacheAccess = 1;

						

						long address = currInsn._address;

						

						// if memory subsyetm is not stalled ( the readQ or writeQ is full) 

						// send read/write request

						if (memSubsystem->IsMemStalled() == 0)

						{

							if(isStore(tempOpcode))

							{

								Val value = (tempData.getOperand2())[2].getData();



								memSubsystem->SendWriteRequest(address, value, tempOpcode);



								completeStoreOperation(opId,dest);



								currInsn._statebit = _COMPLETED_;



								p1++;

							}

							else // if isLoad(tempOpcode)

							{

								Val value;

								

								memSubsystem->SendReadRequest(address, tempOpcode);



								currInsn._statebit = _REPLAY_;



								p1++;

							}

						}

					}

					else

						p1++;

				}

				break;



				default:

				{

					p1++;

				}

				break;

			}

		}

		else

		{

			p2 = p1;



			if (ReservationStation.size() == 1)

			{

				ReservationStation.clear();

				break;

			}

			else
			{
				p1 = ReservationStation.erase(p2);
				//if (p1+1 == end)
					p1++;
			}
		}

	}

	



	// Read from reservation station

	

	p1 = ReservationStation.begin();

	end = ReservationStation.end();

	

	while (p1 != end)

	{	

		LdStInsn currInsn = (*p1);

		tempData = currInsn._ldstInsn;

		State state = currInsn._statebit;

		

		int tempOpcode = tempData.getOpCode();

		

		if(!isNOP(tempOpcode))

		{

			AddrOperand & dest = (tempData.getOperand1())[0];

			MyInt opId = tempData.getIRData();

			

			bool out = false;



			switch(state)

			{

				case _REPLAY_ :  // Only load operation gets into it

				{

					Val value;

					

					long address = currInsn._address;

					

					int addr = memSubsystem->ReceiveData(value, tempOpcode, address);	

					

					if (addr != -1) // Valid read

					{

						// Next three lines will be executed if data is not written

						// in a location but read and used.

						if ((value._valType == _ANY) || (value._valType == _INTERNAL_LABEL))

						{

							value._valType = _INT;

							value._as.integer = 1;

						}



						completeOperation(opId,dest);

						

						outdata.setOpCode(tempOpcode);

						outdata.addOperand1(dest);

						

						//Read the value and put them in source1 position

						

						PredicateAddrDataUnion outval;

						outval.setData(value);

						outdata.addOperand2(outval);

						

						OUT_LATCH_ELEM(0)->write(outdata);

						

						if (debugSIMULATOR)

						{

							cout << "Data  : ";

							value.print(cout);

							cout << " read from addr: ";

							cout << address;

							cout << " into ";

							dest.print();

							cout << endl;

						}



						p2 = p1;

						

						if (ReservationStation.size() == 1)

						{

							ReservationStation.clear();

							out = true;

							break;

						}

						else
						{
							p1 = ReservationStation.erase(p2);
							//if (p1+1 == end)
								p1++;
						}

					}

					else

						p1++;

				}

				break;



				case _COMPLETED_ :   // Only store operation gets into it.

				{

					Val value;

					

					long address = currInsn._address;

					

					bool written = memSubsystem->hasDataWritten(address);	

					

					if (written) // Already written

					{

						if (debugSIMULATOR)

						{

							cout << "Completed Store Operation" << endl;

							cout << "-------------------------" << endl;

						}

						

						//p2 = p1;



						if (ReservationStation.size() == 1)

						{

							ReservationStation.clear();

							out = true;

							break;

						}

						else
						{
							ReservationStation.erase(p1);//p1 = ReservationStation.erase(p2);
							//if (p1+1 == end)
								p1++;
						}
					}

					else

						p1++;

				}

				break;

				

				default:

				{

					p1++;

				}

				break;

			}

			

			if (out) break;

		}

		else

		{

			p2 = p1;

			

			if (ReservationStation.size() == 1)

			{

				ReservationStation.clear();

				break;

			}

			else
			{
				p1 = ReservationStation.erase(p2);
				//if (p1+1 == end)
					p1++;
			}
		}

	}

	

	bool executeBit;



	if (ReservationStation.empty())

		executeBit = false;

	else

		executeBit = true;



	globalControlTable.setProperty(this, ExecuteBit, executeBit);

}





void WriteBackUnit :: doStep()

{

	int inpSize = _inputLatches.size();

	OperationLatchData tempData;



	// Read all the input latches and write to reservation station

	LatchListIterator lIterBegin, lIterEnd;

	lIterBegin = _inputLatches.begin();

	lIterEnd = _inputLatches.end();

	

	while (lIterBegin != lIterEnd)

	{

		(*lIterBegin)->read(tempData);

		(*lIterBegin)->resetOld();

		if ((tempData.getOpCode() != 0) && !isNOP(tempData.getOpCode()))

			ReservationStation.push_back(tempData);

		lIterBegin++;

	}



	if (debugSIMULATOR) 

	{

		

		cout << endl;

		cout << "WRITEBACK UNIT" << endl;

		cout << "--------------" << endl;

		

		if ( ! ReservationStation.empty())

		{

			cout << endl << "Printing operations in WriteBack Buffer :" << endl;

			

			vector<OperationLatchData>::iterator p1,p2;

			p1 = ReservationStation.begin();

			p2 = ReservationStation.end();		

			

			while (p1 != p2) 

			{

				(*p1).print(cout);

				p1++;

			}

			cout << endl;

		}

		else

			cout << "WriteBack Buffer is empty." << endl;

	}

	

	// Read from reservation station and writeback to register file.

	vector<OperationLatchData>::iterator i1,i2, end;

	

	i1 = ReservationStation.begin();

	end = ReservationStation.end();

	

	while(i1 != end)

	{

		tempData = (*i1);

		int tempOpcode = tempData.getOpCode();

		MyInt opId = tempData.getIRData();



		// Debug FIX, should be removed eventually

		if (tempOpcode == 0) return;



		// Detect the end of program

		if (tempOpcode == globalOpCodeTable.getIndex("jr"))

			_END_OF_PROGRAM_ = true;



		if ((tempData.getOperand1()).size() > 0)

		{

			AddrOperand & dest = (tempData.getOperand1())[0];

			PredicateAddrDataUnion & destVal = (tempData.getOperand2())[0];

			

			if (dest.getType() == _REG_)

			{	

				if (dest.getFieldType() == _ADDR_INT_)

					((VirtualRegFile *)GPRFile)->write(destVal.getData(), dest);

				else if ((dest.getFieldType() == _ADDR_FLT_) || (dest.getFieldType() == _ADDR_DBL_))

					((VirtualRegFile *)FPRFile)->write(destVal.getData(), dest);

				else

					assert(0);	

				

				

				// Delete the destination

				vector<AddrOperand> :: iterator it;

				bool exists = findDest(dest, it);

				

				if (exists)

				{

					if (dest.getFieldType() == _ADDR_INT_)

					{

						IntDestTable.erase(it);

					}

					else if ((dest.getFieldType() == _ADDR_FLT_) || (dest.getFieldType() == _ADDR_DBL_))

					{

						FltDestTable.erase(it);

					}

				}

				else

				{

					// Warning, since the destination got deleted incorrectly

				}

				

				completeOperation(opId,dest);				 

				

				if (debugSIMULATOR)

				{

					cout << "Completed Operation in Clk : " << clock_count << endl;

					cout << "-----------------------------------" << endl;

					cout << "Opcode : " << tempOpcode << endl;

					cout << " Register :";

					dest.print(cout);

					cout <<  " and Value  : ";

					destVal.print(cout);

					cout << endl;

				}

			}

		}

			

		i2 = i1;

		

		if (ReservationStation.size() == 1)

		{

			ReservationStation.clear();

			break;

		}

		else

			i1 = ReservationStation.erase(i2);

	}

}





void ArchUnit :: doStep()

{

	CompoundUnit::doStep();

}



void ControlUnit::doStep()

{	

	int rowNum, executeThisUnit, stallThisUnit;

	Row tempList;

	RowIterator i1, i2;

	int colNum = 0;

	

	preprocess();	// Sets the stall bits for the required units to '1'.	

	

	int foobar=0;

	

	// Perform the memory subsystem doStep()

 	memSubsystem->doStep();



	if (debugSIMULATOR)

		cout << endl << "Clk: " << clock_count << " ******************** BEGIN CYCLE *****************   " << endl;



	if (foobar)

		globalControlTable.print(cout);	

				

	for(rowNum = MaxNumStages-1; rowNum >= 0; rowNum--)

	{

		// Copy an entire row (ith stage units of all pipes) from the 

		// globalControlTable into a temporary list. Since this is a copy

		// use it just for reading (not writing) to the table

		globalControlTable.getIthRow(rowNum,tempList);

		

		i1 = tempList.begin();

		i2 = tempList.end();

		colNum = 0;

		while(i1 != i2)

		{		

			Unit * currUnit = i1->getUnit();

			// If execute bit is 1, then call the doStep

			if (currUnit)

			{

				executeThisUnit = i1->getProperty(ExecuteBit);

				stallThisUnit = i1->getProperty(StallBit);

				//if (executeThisUnit && !stallThisUnit)

				if (!stallThisUnit)

				{

					currUnit->doStep();					

				

					// schedule the successor units of all units that executed this cycle

					// by making their NextCycleExecuteBit 1. In addition, as the instr. proceeds 

					// ahead in the pipeline, the dest associated with it is also moved down the pipe

					

					UnitList & succUnitList = globalNetList.getSuccessorList(currUnit);

					// units in the last stage will not have successor units

					UnitListIterator i3, i4;

					i3 = succUnitList.begin();

					i4 = succUnitList.end();

					

					while (i3 != i4)

					{

						Unit * nextUnit = *i3;

						int col=-1;

						if (nextUnit)

						{

							globalControlTable.setProperty(nextUnit, NextCycleExecuteBit, 1);

						}

						i3++;

					}

				}

				else if ((i1->getProperty(ExecuteBit) == 1) && (i1->getProperty(StallBit) == 1))

				{

					setNextCycleExecuteBit(rowNum,colNum,1); // try running it in the next cycle

				}

			}

			i1++;colNum++;

		}

		tempList.clear();

	}



	if (BranchMispredicted)

	{

		BranchMispredicted = _FALSE_;

		if (debugSIMULATOR)
			cout << "\nBranch Mispredicted .. flushing instructions in pipeline\n";

		flush();



		// Restore old BusyReg Tables

		IntDestTable.clear();

		FltDestTable.clear();

		

		long instAddr = BranchOperation.getIRData()._id;

		

		MyInt prediction;

		MyInt pred;

		BranchPredictor.read(prediction, instAddr);

		

		// Put correct target address in the PCLatch

		long tgt = ((BranchOperation.getOperand2())[1]).getData()._as.integer;

		

		if (prediction._id == _PREDICTED_TAKEN_)

		{

			// Get fall through address.

			tgt = instAddr + 4;

			writeToPgmCounterLatch(tgt);

		}

		else if (prediction._id == _PREDICTED_NOT_TAKEN_)

		{

			// Get the branch address

			writeToPgmCounterLatch(tgt);

		}

		

		

		// set next cycle execute bots for all units to 0

		resetAllNextCycleExecuteBits;

		resetAllBusyBits;

		BranchPredictor.write(global_pred, instAddr);
	}


	updateStorageElements();

	updateStatistics();

	globalStallMechanism.setStall();

	testStatusAtEndOfCycle();



	if (debugSIMULATOR)

		cout << endl <<" ******************************** END CYCLE *************************   " << endl << endl;

}



bool findDest(AddrOperand & dest, vector<AddrOperand> :: iterator & it)

{

	if (dest.getFieldType() == _ADDR_INT_)

	{

		it = IntDestTable.begin();

		while (it != IntDestTable.end())

		{

			if (dest == (*it)) 

			{

				return true;

			}

			it++;

		}

	}

	else if ((dest.getFieldType() == _ADDR_FLT_) || (dest.getFieldType() == _ADDR_DBL_))

	{

		it = FltDestTable.begin();

		while (it != FltDestTable.end())

		{

			if (dest == (*it)) 

			{

				return true;

			}

			it++;

		}

	}



	return false;

}