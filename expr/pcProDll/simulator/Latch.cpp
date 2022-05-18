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
#include "stdafx.h"

#include "Latch.h"
#include "Unit.h"
#include "BaseInstruction.h"
#include "BaseArgument.h"
#include "RegisterArg.h"
#include "RealRegArg.h"
#include "IConstArg.h"
#include "FConstArg.h"
#include "SSAArgProperty.h"
#include "Connection.h"

#ifdef WIN32
#include <strstrea.h>
#else
#include <strstream.h>
#endif

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

char *NOPStr = "NOP";

DecodeOperand::DecodeOperand()
{
  reset();
}

void DecodeOperand::reset()
{
  _type = none;
  _data._intConstant = 0;
}

void DecodeOperand::setNone()
{
  reset();
}

void DecodeOperand::setIconst(long v)
{
  _type = iconst;
  _data._intConstant = v;
}

void DecodeOperand::setFconst(char *v)
{
  _type = fconst;
  _data._floatConstant = v;
}

void DecodeOperand::setReg(char *name)
{
  _type = reg;
  if (name!=NULL)
    strcpy(_data._regName,name);
}

void DecodeOperand::setMem(long m)
{
  _type = memory;
  _data._memAddress = m;
}

void DecodeOperand::setLabel()
{
//not used now, to be rewritten later
  _type = label;
  assert(0);
}

int DecodeOperand::isReg()
{
  return _type==reg;
}

int DecodeOperand::isNone()
{
  return _type==none;
}

DecodeOperand& DecodeOperand::operator=(const DecodeOperand& b)
{
  _type = b._type;
  _data = b._data;
  return *this;
}

void DecodeOperand::decodeArg(BaseArgument *argPtr)
{
   if (_NULL_ == argPtr)
     setNone();
   else if (argPtr->isRegisterArg()){
     strstream name;
     char *tStr;
     ((RegisterArg *)argPtr)->print(name);
     name<<ends;
     tStr = name.str();
     setReg(tStr);
     delete []tStr;
   }
   else if (argPtr->isIConstArg())
     setIconst(((IConstArg *)argPtr)->value());
   else if (argPtr->isFConstArg())
     setFconst(((FConstArg *)argPtr)->textValue());
   else
     assert(0);
}

val DecodeOperand::readData(Connection *con)
{
  val value;

  if (_type == reg)
    value = con->read(_data._regName);
  else if (_type == iconst) {
    value.val_type = _INT;
    value._as.integer = _data._intConstant;
  }
   else if (_type == none) {
    value.val_type = _INT;
    value._as.integer = 0;
   }
   else {/* Currently we do not support any other type. */ 
      assert(0);
   }
  return value;
}

void DecodeOperand::print(ostream& out)
{
  switch (_type) {
    case iconst:
       out<<_data._intConstant; break;
    case fconst:
       out<<_data._floatConstant; break;
    case reg:
       out<<_data._regName; break;
    case memory:
       out<<"(M)"<<_data._memAddress; break;
    case none:
       out<<"none"; break;
    default:
       assert(0);
  }
}

/*******************************************/

DecodeDataElem::DecodeDataElem()
{
  reset();
}

void DecodeDataElem:: reset()
{
  _opcode = NOPStr;
  _src1.reset();
  _src2.reset();
  _dest.reset();
}

void DecodeDataElem::print(ostream& out)
{
  out<<_opcode<<"  ";
  _src1.print(out);
  out<<",";
  _src2.print(out);
  out<<",";
  _dest.print(out);
}

DecodeDataElem& DecodeDataElem::operator= (const DecodeDataElem& b)
{
  _opcode = b._opcode;
  _src1 = b._src1;
  _src2 = b._src2;
  _dest = b._dest;
  return *this;
}

void DecodeDataElem::decodeDataOp(NormalOp *op)
{
  BaseArgument *tempArg;

  _opcode = globalOpCodeTable[op->getOpCodeIndex()].opName();
  tempArg = op->ptrToOperand(_DEST_);
  _dest.decodeArg(tempArg);
  tempArg = op->ptrToOperand(_SOURCE_1_);
  _src1.decodeArg(tempArg);
  tempArg = op->ptrToOperand(_SOURCE_2_);
  _src2.decodeArg(tempArg);
}

void DecodeDataElem::decodeControlOp(IfOp *op)
{
  BaseArgument *tempArg;

  _opcode=globalOpCodeTable[op->getOpCodeIndex()].opName();
  _dest.setReg(NULL);
//This is a hack, because of the ugly representation of ControlOp.
//The dest of ControlOp is: cc, which is only used to test condition, and it
//is not written, and src1 , src2 is not used. Yet we don't read the contents 
//of dest, we only operate on src1, src2, that we put the value of cc in src1 
//label is ignored here
  tempArg = op->ptrToCCReg();
  _src1.decodeArg(tempArg);
  _src2.setNone();
}

/****************************/

OpReadData::OpReadData()
{
  reset();
}

void OpReadData::reset()
{
  _opcode = NOPStr;
  _src1.zero();
  _src2.zero();
  _dest.reset();
}

OpReadData& OpReadData::operator=(const OpReadData& b)
{
  _opcode = b._opcode;
  _src1 = b._src1;
  _src2 = b._src2;
  _dest = b._dest;
  return *this;
}

void OpReadData::readData(DecodeDataElem& tempElem,Connection *con)
{

  _opcode = tempElem._opcode;
  _dest = tempElem._dest;
  _src1 = tempElem._src1.readData(con);
  _src2 = tempElem._src2.readData(con);
}

void OpReadData::print(ostream& out)
{
  out<<_opcode<<"  ";
  _src1.print(out);
  out<<",";
  _src2.print(out);
  out<<",";
  _dest.print(out);
}

/****************************/

WritebackDataElem::WritebackDataElem()
{
  reset();
}

void WritebackDataElem::reset()
{
  _result.zero();
  _dest.reset();
}

void WritebackDataElem::print(ostream& out)
{
  _dest.print(out);
  out<<" = ";
  _result.print(out);
}

WritebackDataElem& WritebackDataElem::operator=(const WritebackDataElem& b)
{
  _result = b._result;
  _dest = b._dest;
  return *this;
}

void WritebackDataElem::executeCompute(OpReadData& opR, OpCodeList& opList)
{
  _dest = opR._dest;
  
   if (opList.hasOpCode(opR._opcode))
     ((exeCompSem *)opList.getExecuteSem(opR._opcode))->execute(_result,opR._src1,opR._src2);
   else {
         cout << "Warning: Alu recieved invalid opcode" << endl;
         _dest.setNone(); 
   }
}
  
void WritebackDataElem::executeBranch(OpReadData& opR, OpCodeList& opList)
{
  _dest = opR._dest;
  if (opR._dest.isReg()) {//branch will be executed
    if (opList.hasOpCode(opR._opcode))
//only "if" is allow right now, a hack here, since its semantics has not been
//specified in the target_specification file
      _result = opR._src1;
    else
      cout << "Warning: Branch Unit recieved invalid opcode" << endl;
  }
}
  
void WritebackDataElem::executeMem(OpReadData& opR, OpCodeList& opList,
	Connection *con)
{
  _dest = opR._dest;
  
  if (opList.hasOpCode(opR._opcode))
    ((exeMemSem *)opList.getExecuteSem(opR._opcode))->execute(_result,opR._src1, opR._src2, con);
   else 
         cout << "Warning: Alu received invalid opcode" << endl;
}

void WritebackDataElem::writeback(Connection *con)
{
  if (_dest.isNone()) {
         // Corresponds to a NOP. Do nothing.
  }
  else if (_dest.isReg())
    con->write(_dest._data._regName, _result);
  else {
    cout << "Panic: The destination of a writeback must be a register. Encountered a different type" << endl;
    assert(0);
  }
}
   
int WritebackDataElem::branchExecuted()
{
//it is a hack to test if the branch is empty or has been executed
//dest is set to register, the branch will be executed
  return _dest.isReg();
}

/***************************/

PcLatch::PcLatch()
{
  _old = NULL; 
  _new = NULL;
}

void PcLatch::guiDisplay(ostream& out)
{
  _old->print(out);
}

FetchSubLatch::FetchSubLatch()
{
  _old = NULL; 
  _new = NULL;
}

void FetchSubLatch::guiDisplay(ostream& out)
{
  _old->print(out);
}

DecodeSubLatch::DecodeSubLatch()
{
  _old.reset();
  _new.reset();
}

void DecodeSubLatch::guiDisplay(ostream& out)
{
  _old.print(out);
}

WritebackSubLatch::WritebackSubLatch()
{
  _old.reset();
  _new.reset();
}

void WritebackSubLatch::guiDisplay(ostream& out)
{
  _old.print(out);
}

OpReadSubLatch::OpReadSubLatch()
{
  _old.reset();
  _new.reset();
}

void OpReadSubLatch::guiDisplay(ostream& out)
{
  _old.print(out);
}
