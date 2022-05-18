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
 *file  : Unit.cc                                            *
 *created : 11th nov,97.                                     *
 *authors : Wei Yu Tang,vgee                                 *
 *last modified :                                            *
 *                                                           *
 *contents:contains class methods for Unit class & other     *
 *         classes derived from it.                          *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#include "stdafx.h"

#include "Unit.h"
#include "simulator.h"
#include "NormalInstr.h"
#include "Instruction.h"
#include "sim_register_file.h"
#include "sim_mem.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern sim_register_file *globalSimRegFile;
extern sim_mem *globalSimMem;

extern int pipePhase;

extern Simple_VLIW_Instr *simpleVLIWInstrTemplate;

const _FIRST_PRIORITY_ = 3;
const _SECOND_PRIORITY_ = 2;
const _LAST_PRIORITY_ = 1;

const _LAST_POS_ = numFlowOpsPerInstr ;
//
// Note that this is the same as _LAST_FLOW_POS_ defined in BuildIR.cc

const _FIRST_POS_ = 1;
const _SECOND_POS_ = 2;

ListUnitList guiDisplayUnits;

BaseInstruction *fetchInstr,*decodeInstr;
//this may change to a list of instruction in execution later
BaseInstruction *opreadInstr, *executeInstr, *writebackInstr;


/*************************************************************
 *constructors begin here. other methods are defined after   *
 *the constructors have been defined.                        *
 *input null pointers are not checked for.                   *
 *************************************************************/

void Unit::init()
{
}

void Unit::addOpCode(char *opName)
{
   _opList.addOpCode(opName);
}

OpCodeList & Unit::getOpList()
{
   return _opList;
}

SimpleUnit :: SimpleUnit() : _latches(), _connections(){}
SimpleUnit :: SimpleUnit(const SimpleUnit& a) : _latches(a._latches),
  _connections(a._connections) {}

SimpleUnit :: ~SimpleUnit()
{
}

void SimpleUnit :: init()
{
 Unit::init();
}

Unit * SimpleUnit :: copy()
{
  Unit * retPtr;
  
  retPtr = new SimpleUnit(* this);
  return retPtr;
}

void SimpleUnit :: addLatch(Latch * latch)
{
  _latches.push_back(latch);
}

void SimpleUnit :: addConnection(Connection * connection)
{
  _connections.push_back(connection);
}

void SimpleUnit :: print(ostream & out)
{
   LatchListIterator latchIter;
   ConnectionListIterator connectIter;

   out << endl << "Printing Latches: " << endl;
   
   CREATE_ITER(latchIter, _latches);
   while (HAS_MORE_ELEMENTS(latchIter, _latches))
      {
/* to do: Define print method for each latch class */

/*	       CURRENT_ELEMENT(latchIter)->print(out); */
	 NEXT_ELEMENT(latchIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Connections: " << endl;

   CREATE_ITER(connectIter, _connections);
   while (HAS_MORE_ELEMENTS(connectIter, _connections))
      {
	 CURRENT_ELEMENT(connectIter)->print(out);
	 NEXT_ELEMENT(connectIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
}

CompoundUnit :: CompoundUnit() : _units(), _latches(),
   _connections(){} 

CompoundUnit :: CompoundUnit(const CompoundUnit& a) :
      _units(a._units), _latches(a._latches),
      _connections(a._connections) {} 

CompoundUnit :: ~CompoundUnit()
{
}

void CompoundUnit :: init()
{
 Unit::init();
}

Unit * CompoundUnit :: copy()
{
  Unit * retPtr;
  
  retPtr = new CompoundUnit(* this);
  return retPtr;
}

void CompoundUnit :: addUnit(Unit * unit)
{
  _units.push_back(unit);
}

void CompoundUnit :: addLatch(Latch * latch)
{
  _latches.push_back(latch);
}

void CompoundUnit :: addConnection(Connection * connection)
{
  _connections.push_back(connection);
}

void CompoundUnit :: print(ostream & out)
{
   UnitListIterator unitIter;
   LatchListIterator latchIter;
   ConnectionListIterator connectIter;

   out << endl << "Printing Units: " << endl;
   
   CREATE_ITER(unitIter, _units);
   while (HAS_MORE_ELEMENTS(unitIter, _units))
      {
	 CURRENT_ELEMENT(unitIter)->print(out);
	 NEXT_ELEMENT(unitIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Latches: " << endl;
   
   CREATE_ITER(latchIter, _latches);
   while (HAS_MORE_ELEMENTS(latchIter, _latches))
      {
/* to do: Define print method for each latch class */

/*	       CURRENT_ELEMENT(latchIter)->print(out); */
	 NEXT_ELEMENT(latchIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
   out << endl << "Printing Connections: " << endl;
   
   CREATE_ITER(connectIter, _connections);
   while (HAS_MORE_ELEMENTS(connectIter, _connections))
      {
	 CURRENT_ELEMENT(connectIter)->print(out);
	 NEXT_ELEMENT(connectIter);
      }

   out << endl << "#+-+-+-+-+-+-+#" << endl;
}

FetchUnit :: FetchUnit(const FetchUnit & a)
{
  SimpleUnit::SimpleUnit(a);
}

Unit * FetchUnit :: copy()
{
  Unit * retPtr = new FetchUnit(*this);
  return retPtr;
}

void FetchUnit::guiShortDisplay(ostream& out)
{
  out<<"Fetch Unit";
}

void FetchUnit::guiLongDisplay(ostream& out)
{
  BaseInstruction *pc = ((PcLatch *)LATCH_ELEM(0))->read();
  out<<"Fetch: "<<pc<<endl;
  if (pc!=NULL)
    ((NormalInstr *)pc)->print(out);
}

void FetchUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of Fetch Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int FetchUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_FETCH_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

DecodeUnit :: DecodeUnit(const DecodeUnit & a)
{
  SimpleUnit::SimpleUnit(a);
}

Unit * DecodeUnit :: copy()
{
  Unit * retPtr = new DecodeUnit(*this);
  return retPtr;
}

void DecodeUnit::guiShortDisplay(ostream& out)
{
  out<<"Decode Unit"<<endl;
}

void DecodeUnit::guiLongDisplay(ostream& out)
{
  BaseInstruction *instr =  ((FetchLatch *)LATCH_ELEM(0))->getLatch2().read();
  out<<"Decode: "<<instr<<endl;
  if (instr!=NULL)
    ((NormalInstr *)instr)->print(out);
}

void DecodeUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of Decode Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int DecodeUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_DECODE_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

ExecuteUnit :: ExecuteUnit(const ExecuteUnit & a)
{
  CompoundUnit::CompoundUnit(a);
}

Unit * ExecuteUnit :: copy()
{
  Unit * retPtr = new ExecuteUnit(*this);
  return retPtr;
}

void ExecuteUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of Execute Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int ExecuteUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_EXECUTE_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

AluUnit :: AluUnit(const AluUnit & a)
{
  CompoundUnit::CompoundUnit(a);
}

Unit * AluUnit :: copy()
{
  Unit * retPtr = new AluUnit(*this);
  return retPtr;
}

void AluUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of Alu Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int AluUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_ALU_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

FloatAluUnit :: FloatAluUnit(const FloatAluUnit & a)
{
  CompoundUnit::CompoundUnit(a);
}

Unit * FloatAluUnit :: copy()
{
  Unit * retPtr = new FloatAluUnit(*this);
  return retPtr;
}

void FloatAluUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of FloatAlu Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int FloatAluUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_FLOATALU_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

BranchUnit :: BranchUnit(const BranchUnit & a)
{
  CompoundUnit::CompoundUnit(a);
}

Unit * BranchUnit :: copy()
{
  Unit * retPtr = new BranchUnit(*this);
  return retPtr;
}

void BranchUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of Branch Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int BranchUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_BRANCH_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}


LoadStoreUnit :: LoadStoreUnit(const LoadStoreUnit & a)
{
  CompoundUnit::CompoundUnit(a);
}

Unit * LoadStoreUnit :: copy()
{
  Unit * retPtr = new LoadStoreUnit(*this);
  return retPtr;
}

void LoadStoreUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of LoadStore Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int LoadStoreUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_LOADSTORE_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

OpReadUnit :: OpReadUnit(const OpReadUnit & a)
{
  CompoundUnit::CompoundUnit(a);
}

Unit * OpReadUnit :: copy()
{
  Unit * retPtr = new OpReadUnit(*this);
  return retPtr;
}

void OpReadUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of OpReadUnit Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int OpReadUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_OPREAD_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

OpReadSubUnit :: OpReadSubUnit(const OpReadSubUnit & a)
{
  SimpleUnit::SimpleUnit(a);
}

Unit * OpReadSubUnit :: copy()
{
  Unit * retPtr = new OpReadSubUnit(*this);
  return retPtr;
}

void OpReadSubUnit::guiShortDisplay(ostream& out)
{
  out<<"OpRead Unit";
}

void OpReadSubUnit::guiLongDisplay(ostream& out)
{
  out<<"OpRead: "<<endl;
  ((DecodeLatch *)LATCH_ELEM(0))->getLatch1().guiDisplay(out);
}

void OpReadSubUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of OpRead Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int OpReadSubUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_OPREAD_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

AluExecuteUnit :: AluExecuteUnit(const AluExecuteUnit & a)
{
  SimpleUnit::SimpleUnit(a);
}

Unit * AluExecuteUnit :: copy()
{
  Unit * retPtr = new AluExecuteUnit(*this);
  return retPtr;
}

void AluExecuteUnit::guiShortDisplay(ostream& out)
{
  out<<"ALU";
}

void AluExecuteUnit::guiLongDisplay(ostream& out)
{
  out<<"ALU"<<endl;
  ((OpReadLatch *)LATCH_ELEM(0))->getLatch1().guiDisplay(out);
}

void AluExecuteUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of AluExecute Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int AluExecuteUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_ALUEXECUTE_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

FloatAluExecuteUnit :: FloatAluExecuteUnit(const FloatAluExecuteUnit & a)
{
  SimpleUnit::SimpleUnit(a);
}

Unit * FloatAluExecuteUnit :: copy()
{
  Unit * retPtr = new FloatAluExecuteUnit(*this);
  return retPtr;
}

void FloatAluExecuteUnit::guiShortDisplay(ostream& out)
{
  out<<"FALU";
}

void FloatAluExecuteUnit::guiLongDisplay(ostream& out)
{
  out<<"FALU"<<endl;
  ((OpReadLatch *)LATCH_ELEM(0))->getLatch1().guiDisplay(out);
}

void FloatAluExecuteUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of FloatAluExecute Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int FloatAluExecuteUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_FLOATALUEXECUTE_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

BranchExecuteUnit :: BranchExecuteUnit(const BranchExecuteUnit & a)
{
  SimpleUnit::SimpleUnit(a);
}

Unit * BranchExecuteUnit :: copy()
{
  Unit * retPtr = new BranchExecuteUnit(*this);
  return retPtr;
}

void BranchExecuteUnit::guiShortDisplay(ostream& out)
{
  out<<"BRANCH";
}

void BranchExecuteUnit::guiLongDisplay(ostream& out)
{
  out<<"BRANCH"<<endl;
  ((OpReadLatch *)LATCH_ELEM(0))->getLatch1().guiDisplay(out);
}

void BranchExecuteUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of BranchExecute Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int BranchExecuteUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_BRANCHEXECUTE_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

LoadStoreExecuteUnit :: LoadStoreExecuteUnit(const LoadStoreExecuteUnit & a)
{
  SimpleUnit::SimpleUnit(a);
}

Unit * LoadStoreExecuteUnit :: copy()
{
  Unit * retPtr = new LoadStoreExecuteUnit(*this);
  return retPtr;
}

void LoadStoreExecuteUnit::guiShortDisplay(ostream& out)
{
  out<<"LOAD/STORE";
}

void LoadStoreExecuteUnit::guiLongDisplay(ostream& out)
{
  out<<"LOAD/STORE"<<endl;
  ((OpReadLatch *)LATCH_ELEM(0))->getLatch1().guiDisplay(out);
}

void LoadStoreExecuteUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of LoadStoreExecute Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int LoadStoreExecuteUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_LOADSTOREEXECUTE_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

WriteBackSubUnit :: WriteBackSubUnit(const WriteBackSubUnit & a)
{
  SimpleUnit::SimpleUnit(a);
}

Unit * WriteBackSubUnit :: copy()
{
  Unit * retPtr = new WriteBackSubUnit(*this);
  return retPtr;
}

void WriteBackSubUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of WriteBackSub Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

void WriteBackSubUnit::guiShortDisplay(ostream& out)
{
  out<<"WRITEBACK";
}

void WriteBackSubUnit::guiLongDisplay(ostream& out)
{
  out<<"WRITEBACK"<<endl;
  ((WritebackLatch *)LATCH_ELEM(0))->getLatch1().guiDisplay(out);
}

int WriteBackSubUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_WRITEBACKSUB_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

BranchWriteBackSubUnit :: BranchWriteBackSubUnit(const BranchWriteBackSubUnit & a)
{
 WriteBackSubUnit::WriteBackSubUnit(a);
}

Unit * BranchWriteBackSubUnit :: copy()
{
  Unit * retPtr = new BranchWriteBackSubUnit(*this);
  return retPtr;
}

void BranchWriteBackSubUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of BranchWriteBackSub Unit: " << endl;
   SimpleUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int BranchWriteBackSubUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_BRANCHWRITEBACKSUB_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

WriteBackUnit :: WriteBackUnit(const WriteBackUnit & a)
{
  CompoundUnit::CompoundUnit(a);
}

Unit * WriteBackUnit :: copy()
{
  Unit * retPtr = new WriteBackUnit(*this);
  return retPtr;
}

void WriteBackUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of WriteBack Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int WriteBackUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_WRITEBACK_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

BranchWriteBackUnit :: BranchWriteBackUnit(const BranchWriteBackUnit & a)
{
 WriteBackUnit::WriteBackUnit(a);
}

Unit * BranchWriteBackUnit :: copy()
{
  Unit * retPtr = new BranchWriteBackUnit(*this);
  return retPtr;
}

void BranchWriteBackUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of BranchWriteBack Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int BranchWriteBackUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_BRANCHWRITEBACK_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

ArchUnit :: ArchUnit(const ArchUnit & a)
{
  CompoundUnit::CompoundUnit(a);
}

Unit * ArchUnit :: copy()
{
  Unit * retPtr = new ArchUnit(*this);
  return retPtr;
}

void ArchUnit :: print(ostream & out)
{
   out << endl << "Printing the contents of Arch Unit: " << endl;
   CompoundUnit :: print(out);
   out << endl << "#********************#" << endl;
}

int ArchUnit :: isEq(Unit *ptr)
{
   if (_NULL_ != ptr)
      {
	 if (ptr->is(_ARCH_UNIT_)) return _YES_;
	 
	 /* to do: redefine this portion */
      }
   return _NO_;
}

void ArchUnit::buildArch()
{

   UnitList tempGUIFetchList, tempGUIDecodeList, tempGUIOpReadList, tempGUIExecList, tempGUIWBList;;
   
// First Create all the units, then all the latches, and finally the
// storage, connection elements.

/********* Units *****************************/

  
   guiDisplayUnits.erase(guiDisplayUnits.begin(),guiDisplayUnits.end());
   init();

// control.

   Unit *cntrl;
   cntrl = new ControlUnit();
   cntrl->init();


  //fetch

  Unit * fetch;
  fetch=new FetchUnit();
  fetch->init();

  //decode

  Unit * decode;
  decode=new DecodeUnit();
  decode->init();

  //alu1, alu2

  Unit * alu1, *alu2, *aluop1, *aluop2, *aluex1, *aluex2;
  alu1=new AluUnit();
  alu2=new AluUnit();
  alu1->init();
  alu2->init();

  aluop1 = new OpReadSubUnit();
  aluop2 = new OpReadSubUnit();
  aluop1->init();
  aluop2->init();

  aluex1 = new AluExecuteUnit();
  aluex2 = new AluExecuteUnit();
  aluex1->init();
  aluex2->init();

  aluex1->addOpCode("IADD");
  aluex1->addOpCode("ISUB" );
  aluex1->addOpCode("IGE");
  aluex1->addOpCode("IGT");
  aluex1->addOpCode("ILE" );
  aluex1->addOpCode("ILT" );
  aluex1->addOpCode("IASH" );
  aluex1->addOpCode("ILSH" );
  aluex1->addOpCode("ICONSTANT" );
  aluex1->addOpCode("IASSIGN" );
  aluex1->addOpCode("ASSIGN" );
  aluex1->addOpCode("NOP" );
//this is added here to make the first function unit the general purpose unit
//since in simulation of serial programs, floating point operations may 
//occupy the slot for integer operations.
  aluex1->addOpCode("FASSIGN");
  aluex1->addOpCode("DADD");
  //aluex1->addOpCode("DSUB");
  aluex1->addOpCode("DCONSTANT");
  //aluex1->addOpCode("DMUL");
  aluex1->addOpCode("DMTC1");
  aluex1->addOpCode("TRUNCID");
  aluex1->addOpCode("MFC1");
  aluex1->addOpCode("MTC1");
  aluex1->addOpCode("CVTDI");
  aluex1->addOpCode("ILAND");
  aluex1->addOpCode("DASSIGN");
  aluex1->addOpCode("DGE");
  aluex1->addOpCode("DGT");
  aluex1->addOpCode("CVTSD");
  aluex1->addOpCode("CVTDS");
  aluex1->addOpCode("DMFC1");
  aluex1->addOpCode("FVSTORE");
  aluex1->addOpCode("DDIV");
  aluex1->addOpCode("FVLOAD");
  aluex1->addOpCode("IMUL");
  aluex1->addOpCode("IEQ");
  aluex1->addOpCode("DEQ");
  aluex1->addOpCode("INE");
  aluex1->addOpCode("ASSIGN");

  //followin are target specific opcodes for c6x
  aluex1->addOpCode("ADD");
  aluex1->addOpCode("SUB");
  aluex1->addOpCode("LDW");
  aluex1->addOpCode("STW");
  aluex1->addOpCode("MV");
  aluex1->addOpCode("SHL");
  aluex1->addOpCode("MPYDP");
  aluex1->addOpCode("ADDDP");
  aluex1->addOpCode("SUBDP");
  aluex1->addOpCode("MVK");
  aluex1->addOpCode("CMPLT");
  aluex1->addOpCode("CMPGT");
  aluex1->addOpCode("DPTRUNC");
  aluex1->addOpCode("DPINT");
  aluex1->addOpCode("CMPLTDP");
  aluex1->addOpCode("CMPGTDP");
  aluex1->addOpCode("SPDP");
  aluex1->addOpCode("DPSP");
  aluex1->addOpCode("CMPEQDP");
  aluex1->addOpCode("ADDSP");
  aluex1->addOpCode("SUBSP");
  aluex1->addOpCode("MPYSP");
  aluex1->addOpCode("LDDW");
  aluex1->addOpCode("STDW");



   
  simpleVLIWInstrTemplate->addSlot(DATA_OP_SLOT, aluex1);

  aluex2->addOpCode("IADD");
  aluex2->addOpCode("ISUB" );
  aluex2->addOpCode("IGE");
  aluex2->addOpCode("IGT");
  aluex2->addOpCode("ILE" );
  aluex2->addOpCode("ILT" );
  aluex2->addOpCode("IASH" );
  aluex2->addOpCode("ILSH" );
  aluex2->addOpCode("ICONSTANT" );
  aluex2->addOpCode("IASSIGN" );
  aluex2->addOpCode("NOP" );
   
  simpleVLIWInstrTemplate->addSlot(DATA_OP_SLOT, aluex2);

  alu1->addUnit(aluop1);
  alu1->addUnit(aluex1);
  alu2->addUnit(aluop2);
  alu2->addUnit(aluex2);

  //fpu

  Unit * float1, *floatop, *floatex;
  float1=new FloatAluUnit();
  float1->init();

  floatop = new OpReadSubUnit();
  floatex = new FloatAluExecuteUnit();
  floatop->init();
  floatex->init();

  floatex->addOpCode("IADD");
  floatex->addOpCode("FASSIGN");
  floatex->addOpCode("DADD");
  floatex->addOpCode("DSUB");
  floatex->addOpCode("DCONSTANT");
  floatex->addOpCode("DMUL");
  floatex->addOpCode("FMUL");
  floatex->addOpCode("FADD");
  floatex->addOpCode("FSUB");
  floatex->addOpCode("DMTC1");
  floatex->addOpCode("NOP");
  floatex->addOpCode("FMUL");
  floatex->addOpCode("FADD");
  floatex->addOpCode("FSUB");
  floatex->addOpCode("FDIV");
   
  simpleVLIWInstrTemplate->addSlot(DATA_OP_SLOT, floatex);

  float1->addUnit(floatop);
  float1->addUnit(floatex);

  //load/store

  Unit * ld, *ldop, *ldex;
  ld = new LoadStoreUnit();
  ldop = new OpReadSubUnit();
  ldex = new LoadStoreExecuteUnit();

  ldex->addOpCode("IVLOAD");
  ldex->addOpCode("IVSTORE");
  ldex->addOpCode("DVLOAD");
  ldex->addOpCode("DVSTORE");
  ldex->addOpCode("NOP");
   
  simpleVLIWInstrTemplate->addSlot(DATA_OP_SLOT, ldex);

  ld->init();
  ldop->init();
  ldex->init();

  ld->addUnit(ldop);
  ld->addUnit(ldex);

   
  //br1, br2

  Unit *br1, *br2, *br1op, *br2op, *br1ex, *br2ex;
  br1=new BranchUnit();
  br2=new BranchUnit();
  br1op = new OpReadSubUnit();
  br2op = new OpReadSubUnit();
  br1ex = new BranchExecuteUnit();
  br2ex = new BranchExecuteUnit();

  br1->init();
  br2->init();
  br1op->init();
  br1ex->init();
  br2op->init();
  br2ex->init();

  br1ex->addOpCode("IF");
  br1ex->addOpCode("GOTO");
  br1ex->addOpCode("NOP");
  
  simpleVLIWInstrTemplate->addSlot(CONTROL_OP_SLOT, br1ex);

  br2ex->addOpCode("IF");
  br2ex->addOpCode("GOTO");
  br2ex->addOpCode("NOP");
   
  simpleVLIWInstrTemplate->addSlot(CONTROL_OP_SLOT, br2ex);

  br1->addUnit(br1op);
  br1->addUnit(br1ex);
  br2->addUnit(br2op);
  br2->addUnit(br2ex);


   
  //wb

  // This structure may change. (i.e we may have things like
  // BranchWriteBackSubUnit and BranchWriteBackUnit).
  //

  Unit *subwb1, *subwb2,*subwb3,*subwb4,*subwb5br1,*subwb6br2;

  subwb1 = new WriteBackSubUnit();
  subwb2 = new WriteBackSubUnit();
  subwb3 = new WriteBackSubUnit();
  subwb4 = new WriteBackSubUnit();
  subwb5br1 = new BranchWriteBackSubUnit();
  subwb6br2 = new BranchWriteBackSubUnit();

  subwb1->init();
  subwb2->init();
  subwb3->init();
  subwb4->init();
  subwb5br1->init();
  subwb6br2->init();

  ((BranchWriteBackSubUnit *)subwb5br1)->setPriority(_FIRST_PRIORITY_);
  ((BranchWriteBackSubUnit *)subwb6br2)->setPriority(_SECOND_PRIORITY_);
  ((BranchWriteBackSubUnit *)subwb5br1)->setPos(_FIRST_POS_);
  ((BranchWriteBackSubUnit *)subwb6br2)->setPos(_SECOND_POS_);

  Unit * brwb;
  brwb = new BranchWriteBackUnit();
  brwb->init();

  ((BranchWriteBackUnit *)brwb)->setLastPriority(_LAST_PRIORITY_);
  ((BranchWriteBackUnit *)brwb)->setLastPos(_LAST_POS_);

  brwb->addUnit(subwb5br1);
  brwb->addUnit(subwb6br2);

//start create Compound Unit
  Unit *opread, *execute, *execU,*wb;
  opread = new OpReadUnit();
  opread->init();
  opread->addUnit(aluop1);
  opread->addUnit(aluop2);
  opread->addUnit(floatop);
  opread->addUnit(ldop);
  opread->addUnit(br1op);
  opread->addUnit(br2op);

  execute = new ExecuteUnit();
  execute->init();
  execute->addUnit(aluex1);
  execute->addUnit(aluex2);
  execute->addUnit(floatex);
  execute->addUnit(ldex);
  execute->addUnit(br1ex);
  execute->addUnit(br2ex);

  wb = new WriteBackUnit();
  wb->init();
  wb->addUnit(subwb1);
  wb->addUnit(subwb2);
  wb->addUnit(subwb3);
  wb->addUnit(subwb4);
  wb->addUnit(subwb5br1);
  wb->addUnit(subwb6br2);

  execU = new ExecuteUnit();
  execU->init();
  execU->addUnit(alu1);
  execU->addUnit(alu2);
  execU->addUnit(float1);
  execU->addUnit(ld);
  execU->addUnit(br1);
  execU->addUnit(br2);
  
  cntrl->addUnit(fetch);
  cntrl->addUnit(decode);
  cntrl->addUnit(opread);
  cntrl->addUnit(execute);
  cntrl->addUnit(wb);
  cntrl->addUnit(execU);
  cntrl->addUnit(this);
  cntrl->addUnit(aluop1);
  cntrl->addUnit(aluop2);
  cntrl->addUnit(floatop);
  cntrl->addUnit(ldop);
  cntrl->addUnit(br1op);
  cntrl->addUnit(br2op);
  cntrl->addUnit(aluex1);
  cntrl->addUnit(aluex2);
  cntrl->addUnit(floatex);
  cntrl->addUnit(ldex);
  cntrl->addUnit(br1ex);
  cntrl->addUnit(br2ex);
  cntrl->addUnit(subwb1);
  cntrl->addUnit(subwb2);
  cntrl->addUnit(subwb3);
  cntrl->addUnit(subwb4);
  cntrl->addUnit(subwb5br1);
  cntrl->addUnit(subwb6br2);
  cntrl->addUnit(alu1);
  cntrl->addUnit(alu2);
  cntrl->addUnit(float1);
  cntrl->addUnit(ld);
  cntrl->addUnit(br1);
  cntrl->addUnit(br2);
  cntrl->addUnit(brwb);
  
  addUnit(cntrl); // ControlUnit is added to ArchUnit.
  addUnit(fetch);
  addUnit(decode);
  addUnit(opread);
  addUnit(execute);
  addUnit(wb); 

//for gui
  tempGUIFetchList.push_back(fetch);
  guiDisplayUnits.push_back(tempGUIFetchList);
   
  tempGUIDecodeList.push_back(decode);
  guiDisplayUnits.push_back(tempGUIDecodeList);

  tempGUIOpReadList.push_back(aluop1);
  tempGUIOpReadList.push_back(aluop2);
  tempGUIOpReadList.push_back(floatop);
  tempGUIOpReadList.push_back(ldop);
  tempGUIOpReadList.push_back(br1op);
  tempGUIOpReadList.push_back(br2op);
  guiDisplayUnits.push_back(tempGUIOpReadList);
   
  tempGUIExecList.push_back(aluex1);
  tempGUIExecList.push_back(aluex2);
  tempGUIExecList.push_back(floatex);
  tempGUIExecList.push_back(ldex);
  tempGUIExecList.push_back(br1ex);
  tempGUIExecList.push_back(br2ex);
  guiDisplayUnits.push_back(tempGUIExecList);

  tempGUIWBList.push_back(subwb1);
  tempGUIWBList.push_back(subwb2);
  tempGUIWBList.push_back(subwb3);
  tempGUIWBList.push_back(subwb4);
  tempGUIWBList.push_back(subwb5br1);
  tempGUIWBList.push_back(subwb6br2);
  guiDisplayUnits.push_back(tempGUIWBList);

/********* Units *****************************/

/********* Latches *****************************/

  // PC.

  Latch *pc;
  pc = new PcLatch();
  
fetch->addLatch(pc);
brwb->addLatch(pc);
wb->addLatch(pc);

// 11/28 97
// Note that I am not adding the PC latch to the writeback units.
// This is a little messy right now, and it should be resloved
// properly (i.e. do not make it into a hack.).
//
// 12/12 97
//
// Well, I did turn it into a hack. But atleast it is a well organized
// hack. :-)

   addLatch(pc); // Adding pc latch to both Arch and Control.
   cntrl->addLatch(pc);

   Latch *fetchLatch;
fetchLatch = new FetchLatch();

fetch->addLatch(fetchLatch);
decode->addLatch(fetchLatch);

cntrl->addLatch(fetchLatch);

Latch *decodeLatch1,*decodeLatch2,*decodeLatch3;
Latch *decodeLatch4,*decodeLatch5,*decodeLatch6;

decodeLatch1 = new DecodeLatch();
decode->addLatch(decodeLatch1);
aluop1->addLatch(decodeLatch1);

decodeLatch2 = new DecodeLatch();
decode->addLatch(decodeLatch2);
aluop2->addLatch(decodeLatch2);

decodeLatch3 = new DecodeLatch();
decode->addLatch(decodeLatch3);
floatop->addLatch(decodeLatch3);

decodeLatch4 = new DecodeLatch();
decode->addLatch(decodeLatch4);
ldop->addLatch(decodeLatch4);

decodeLatch5 = new DecodeLatch();
decode->addLatch(decodeLatch5);
br1op->addLatch(decodeLatch5);

decodeLatch6 = new DecodeLatch();
decode->addLatch(decodeLatch6);
br2op->addLatch(decodeLatch6);

cntrl->addLatch(decodeLatch1);
cntrl->addLatch(decodeLatch2);
cntrl->addLatch(decodeLatch3);
cntrl->addLatch(decodeLatch4);
cntrl->addLatch(decodeLatch5);
cntrl->addLatch(decodeLatch6);

Latch *opread1, *opread2, *opread3, *opread4, *opread5, *opread6;

opread1 = new OpReadLatch();
aluop1->addLatch(opread1);
aluex1->addLatch(opread1);

opread2 = new OpReadLatch();
aluop2->addLatch(opread2);
aluex2->addLatch(opread2);

opread3 = new OpReadLatch();
floatop->addLatch(opread3);
floatex->addLatch(opread3);

opread4 = new OpReadLatch();
ldop->addLatch(opread4);
ldex->addLatch(opread4);

opread5 = new OpReadLatch();
br1op->addLatch(opread5);
br1ex->addLatch(opread5);

opread6 = new OpReadLatch();
br2op->addLatch(opread6);
br2ex->addLatch(opread6);

cntrl->addLatch(opread1);
cntrl->addLatch(opread2);
cntrl->addLatch(opread3);
cntrl->addLatch(opread4);
cntrl->addLatch(opread5);
cntrl->addLatch(opread6);

Latch *wb1, *wb2, *wb3, *wb4, *wb5, *wb6;

wb1 = new WritebackLatch();
aluex1->addLatch(wb1);
subwb1->addLatch(wb1);

wb2 = new WritebackLatch();
aluex2->addLatch(wb2);
subwb2->addLatch(wb2);

wb3 = new WritebackLatch();
floatex->addLatch(wb3);
subwb3->addLatch(wb3);

wb4 = new WritebackLatch();
ldex->addLatch(wb4);
subwb4->addLatch(wb4);

wb5 = new WritebackLatch();
br1ex->addLatch(wb5);
subwb5br1->addLatch(wb5);

wb6 = new WritebackLatch();
br2ex->addLatch(wb6);
subwb6br2->addLatch(wb6);

cntrl->addLatch(wb1);
cntrl->addLatch(wb2);
cntrl->addLatch(wb3);
cntrl->addLatch(wb4);
cntrl->addLatch(wb5);
cntrl->addLatch(wb6);

/********* Latches *****************************/

/********* Storage *****************************/

/* For conneciton to the register file: Currently we are assuming */
/* unlimited connectivity. The connection class structure may have to */
/* be modified to change this later. */

Storage *regfile, *mem;
Connection *opread1reg, *opread2reg, *opread3reg, *opread4reg,
*opread5reg, *opread6reg, *wb1reg, *wb2reg,  *wb3reg,  *wb4reg,
*wb5reg,  *wb6reg;

Connection *ldmem;

sim_register_file *regf;
regf = globalSimRegFile;

regfile = new RegFile(regf, 6);

sim_mem *memf;
memf = globalSimMem;

mem = new Memory(memf);

((ControlUnit *)cntrl)->addStorage(regfile);
((ControlUnit *)cntrl)->addStorage(mem);

opread1reg = new RegisterConnection(regfile, READ);
opread2reg = new RegisterConnection(regfile, READ);
opread3reg = new RegisterConnection(regfile, READ);
opread4reg = new RegisterConnection(regfile, READ);
opread5reg = new RegisterConnection(regfile, READ);
opread6reg = new RegisterConnection(regfile, READ);

aluop1->addConnection(opread1reg);
aluop2->addConnection(opread2reg);
floatop->addConnection(opread3reg);
br1op->addConnection(opread4reg);
br2op->addConnection(opread5reg);
ldop->addConnection(opread6reg);

cntrl->addConnection(opread1reg);
cntrl->addConnection(opread2reg);
cntrl->addConnection(opread3reg);
cntrl->addConnection(opread4reg);
cntrl->addConnection(opread5reg);
cntrl->addConnection(opread6reg);

wb1reg = new RegisterConnection(regfile, WRITE);
wb2reg = new RegisterConnection(regfile, WRITE);
wb3reg = new RegisterConnection(regfile, WRITE);
wb4reg = new RegisterConnection(regfile, WRITE);
wb5reg = new RegisterConnection(regfile, WRITE);
wb6reg = new RegisterConnection(regfile, WRITE);

subwb1->addConnection(wb1reg);
subwb2->addConnection(wb2reg);
subwb3->addConnection(wb3reg);
subwb4->addConnection(wb6reg);
subwb5br1->addConnection(wb4reg);
subwb6br2->addConnection(wb5reg);

cntrl->addConnection(wb1reg);
cntrl->addConnection(wb2reg);
cntrl->addConnection(wb3reg);
cntrl->addConnection(wb4reg);
cntrl->addConnection(wb5reg);
cntrl->addConnection(wb6reg);

ldmem = new MemoryConnection(mem, RW);
ldex->addConnection(ldmem);

cntrl->addConnection(ldmem);
} 
   
#ifndef WIN32
//VC++ doesn't like it here and in Unit.h
//had to keep the Unit.h version
ControlUnit::ControlUnit() : _storage()
{
 CompoundUnit::CompoundUnit();
}
#endif
   
void ControlUnit::init()
{
 CompoundUnit::init();
}

void ControlUnit::addStorage(Storage *st)
{
  _storage.push_back(st);
}   

void ControlUnit::updateInstrPtrs()
{
//fetch instruction ptr
    fetchInstr = ((PcLatch *)LATCH_ELEM(0))->read();

//decode instruction ptr
    decodeInstr = ((FetchLatch *)LATCH_ELEM(1))->getLatch2().read();

//opread instruction ptr
    opreadInstr = ((DecodeLatch *)LATCH_ELEM(2))->getLatch2().read();

//execute instruction ptr
    executeInstr = ((OpReadLatch *)LATCH_ELEM(8))->getLatch2().read();

//writeback instruction ptr
    writebackInstr = ((WritebackLatch *)LATCH_ELEM(14))->getLatch2().read();

    if (pipePhase==CYCLES_0){
      decodeInstr = NULL;
      opreadInstr = NULL;
      executeInstr = NULL;
      writebackInstr = NULL;
    }
    else if (pipePhase==CYCLES_1) {
      opreadInstr = NULL;
      executeInstr = NULL;
      writebackInstr = NULL;
    }
    else if (pipePhase==CYCLES_2) {
      executeInstr = NULL;
      writebackInstr = NULL;
    }
    else if (pipePhase==CYCLES_3) {
      writebackInstr = NULL;
    }
}

void ArchUnit::updateInstrPtrs()
{
  ((ControlUnit *)UNIT_ELEM(0))->updateInstrPtrs();
}

void ArchUnit::initPC(BaseInstruction *bPtr)
{
   PC_SUB_LATCH(0)->write(bPtr);
   PC_SUB_LATCH(0)->doUpdate();
}

void ControlUnit::initPC(BaseInstruction *bPtr)
{
   PC_SUB_LATCH(0)->write(bPtr);
   PC_SUB_LATCH(0)->doUpdate();
}

int ArchUnit::endOfFetch()
{
   if (Fetch_PC_LATCH->read() != _NULL_)
      return _NO_;
   return _YES_;
}

int ControlUnit::endOfFetch()
{
   if (Fetch_PC_LATCH->read() != _NULL_)
      return _NO_;
   return _YES_;
}

void ArchUnit::getGUIUnitVector(ListUnitList & a)
{
   a = guiDisplayUnits;
}

   
