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
#ifndef _EXPRESSADDRDATA_H_
#define _EXPRESSADDRDATA_H_

#include "AddrData.h"
#include "ArchClassIncludes.h"
#include "../../pcProDll/include/BaseInstruction.h"
#include "../../pcProDll/include/BaseOperation.h"

typedef class BaseInstruction *InstrPtr;
typedef class BaseOperation *OperPtr;

class IRPtr : public BaseType
{
protected:
	InstrPtr _instr;
	OperPtr _oper;

public:
	 IRPtr() : _instr(NULL), _oper(NULL){}
 ~IRPtr(){}
 IRPtr(const IRPtr& b) : _instr(b._instr), _oper(b._oper) {}
 IRPtr(const InstrPtr& b, const OperPtr & c): _instr(b), _oper(c) {}
 
  virtual BaseType * copy()
  {
   BaseType * retPtr;  
   retPtr = new IRPtr(*this);
   return retPtr;
  }
  virtual int is(BaseTypeEnum ctype)
    { return ((ctype == _IRPTR_) ? _YES_ : BaseType::is(ctype));}
  friend int operator ==(const IRPtr &a, const IRPtr &b)
  {
	  if ((a._instr == b._instr) && (a._oper == b._oper)) return _YES_;
	  return _NO_;
  }
  virtual void print(ostream &out)
    {
	  out << "Instr: " << _instr << " Oper: " << _oper;
    }
  virtual int isEq(BaseType *ptr)
  {
	  if (ptr == NULL) return _NO_;
	  if (ptr->is(_IRPTR_) == _NO_) return _NO_;
	  return ((*((IRPtr *) ptr)) == *this);
  }
  void operator=(const IRPtr &a){_instr = a._instr; _oper = a._oper;}

  virtual void reset(){ _instr = NULL; _oper = NULL;}
  virtual int isEmpty()
  {
	  if ((_instr == NULL) || (_oper == NULL)) return _YES_;
	  return _NO_;
  }
  InstrPtr& getInstr(){return _instr;}
  void setInstr(InstrPtr & b) { _instr = b; }

  OperPtr& getOper(){return _oper;}
  void setOper(OperPtr & b) { _oper = b; }

  int isValid(){ return (((_instr == NULL) || (_oper == NULL)) ? _NO_ : _YES_); }
};

#endif