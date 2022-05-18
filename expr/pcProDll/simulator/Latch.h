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
*file  : latch.h                                            *
*created : 11th nov,97.                                     *
*authors : Wei Yu Tang,vgee                                 *
*last modified :                                            *
*                                                           *
*compiled with : g++                                        *
*for more info :                                            *
*************************************************************/
#ifndef _LATCH_H_
#define _LATCH_H_
#include "ArchClassIncludes.h"
#include "DataValue.h"
#include "Storage.h"
#include "NormalOp.h"
#include "IfOp.h"

class DecodeOperand {
 enum ArgType { none, iconst,fconst, reg, memory, label };
 
 ArgType _type;
 union {
  long _intConstant;
  char *_floatConstant;
  char _regName[MAX_LEN];
  long _memAddress;
 } _data; 
 
public:
 //should I make a copy of _floatConstant? maybe not necessay, since I don't
 //delete instructions in simulation, so all operands in instructions are
 //still valid
 DecodeOperand();
 void reset();
 void setNone();
 void setIconst(long );
 void setFconst(char *);
 void setReg(char *);
 void setMem(long);
 void setLabel();
 int isReg();
 int isNone();
 void decodeArg(BaseArgument *);
 val readData(class Connection *);
 DecodeOperand& operator = (const DecodeOperand & b);
 void print(ostream& out);
 friend class WritebackDataElem;
};

class DecodeDataElem {
 char *_opcode;
 DecodeOperand _src1, _src2,  _dest;
public:
 
 DecodeDataElem();
 void reset();
 void decodeDataOp(NormalOp *);
 void decodeControlOp(IfOp *);
 DecodeDataElem& operator = (const DecodeDataElem & b);
 void print(ostream& out);
 friend class OpReadData;
};

class OpReadData {
 char * _opcode;
 val _src1,_src2;
 DecodeOperand _dest;
public:
 OpReadData();
 void reset();
 OpReadData& operator = (const OpReadData & );
 void readData(DecodeDataElem&,class Connection *);
 void print(ostream& out);
 friend class WritebackDataElem;
};

class WritebackDataElem {
 val _result;
 DecodeOperand  _dest;
public:
 WritebackDataElem();
 void reset();
 void print(ostream& out);
 WritebackDataElem& operator = (const WritebackDataElem & b);
 void executeCompute(OpReadData&,class OpCodeList&);
 void executeBranch(OpReadData&,class OpCodeList&);
 void executeMem(OpReadData&,class OpCodeList&,class Connection *);
 void writeback(class Connection *);
 val getResult() {return _result; }
 int branchExecuted();
};

/******************************/

class Latch : public Storage{
public :
 Latch() {};
 int isLatch() { return 1; }
 virtual void guiDisplay(ostream& out) {"NOT HERE";}
 virtual void doUpdate() {};
};

class SubLatch : public Latch
{
public:
 SubLatch() {};
 virtual void reset() = 0;
};

template <class T1, class T2> class CompoundLatch : public Latch
{
#ifdef LEAKY
 //NOS: used for memory leaks
 char tag[MAX_PATH];
#endif
 
 
 T1 _latch1;
 T2 _latch2;
public:
 CompoundLatch() 
 { 
#ifdef LEAKY
  {
   //NOS
   char t[MAX_PATH];
   GetFileTitle(__FILE__,t,MAX_PATH); 
   sprintf(tag,"%d:%s",__LINE__,t);
  }
#endif
  
 };
 
 T1 & getLatch1(){ return _latch1; }
 T2 & getLatch2(){ return _latch2; }
 
 virtual void doUpdate() 
 { _latch1.doUpdate(); 
 _latch2.doUpdate();
 }
 
 virtual void reset()
 {
  _latch1.reset();
  _latch2.reset();
 }
};

class PcLatch : public SubLatch
{
 PcType _old;
 PcType _new;
public:
 
 PcLatch() ;
 virtual PcType & read() { return _old; };
 virtual void write(PcType & t) { _new = t ; }
 void guiDisplay(ostream& out);
 virtual void doUpdate() 
 {
  _old = _new;
 }
 virtual void reset()
 {
  _old = NULL;
  _new = NULL;
 }
};

class FetchSubLatch : public SubLatch
{
 PcType _old;
 PcType _new;
public:
 FetchSubLatch() ;
 virtual PcType & read() { return _old; };
 virtual void write(PcType & t) { _new = t ; }
 void guiDisplay(ostream& out);
 virtual void doUpdate() 
 {
  _old = _new;
 }
 virtual void reset()
 {
  _old = NULL;
  _new = NULL;
 }
};

class DecodeSubLatch : public SubLatch
{
 DecodeDataElem _old;
 DecodeDataElem _new;
public:
 DecodeSubLatch() ;
 virtual DecodeDataElem & read() { return _old; };
 virtual void write(DecodeDataElem & t) { _new = t ; }
 void guiDisplay(ostream& out);
 virtual void doUpdate() 
 {
  _old = _new;
 }
 virtual void reset()
 {
  _old.reset();
  _new.reset();
 }
};

class WritebackSubLatch : public SubLatch
{
 WritebackDataElem _old;
 WritebackDataElem _new;
public:
 WritebackSubLatch() ;
 virtual WritebackDataElem & read() { return _old; };
 virtual void write(WritebackDataElem & t) { _new = t ; }
 void guiDisplay(ostream& out);
 virtual void doUpdate() 
 {
  _old = _new;
 }
 virtual void reset()
 {
  _old.reset();
  _new.reset();
 }
};

class OpReadSubLatch : public SubLatch
{
 OpReadData _old;
 OpReadData _new;
public:
 OpReadSubLatch() ;
 virtual OpReadData & read() { return _old; };
 virtual void write(OpReadData & t) { _new = t ; }
 void guiDisplay(ostream& out);
 virtual void doUpdate() 
 {
  _old = _new;
 }
 virtual void reset()
 {
  _old.reset();
  _new.reset();
 }
};

#define PC_SUB_LATCH(p) ((PcLatch *)(LATCH_ELEM(p)))

typedef CompoundLatch<FetchSubLatch, PcLatch> FetchLatch;

#define FETCH_SUB_LATCH(p, q) (COMP_LATCH_SUB_ELEM(p, FetchSubLatch, PcLatch, q))

typedef CompoundLatch<DecodeSubLatch, PcLatch> DecodeLatch;

#define DECODE_SUB_LATCH(p, q) (COMP_LATCH_SUB_ELEM(p, DecodeSubLatch, PcLatch, q))

typedef CompoundLatch<WritebackSubLatch, PcLatch> WritebackLatch;

#define WRITEBACK_SUB_LATCH(p, q) (COMP_LATCH_SUB_ELEM(p, WritebackSubLatch, PcLatch, q))

typedef CompoundLatch<OpReadSubLatch, PcLatch > OpReadLatch;

#define OPREAD_SUB_LATCH(p, q) (COMP_LATCH_SUB_ELEM(p, OpReadSubLatch, PcLatch, q))

#endif
