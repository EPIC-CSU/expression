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
// $Id: BaseOperation.h,v 1.24 1998/03/31 22:58:15 pgrun Exp $
//
// File:  		BaseOperation.h
// Created:		Mon Oct 28, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#ifndef __BASEOPERATION_H__
#define __BASEOPERATION_H__

#include "OperationIncludes.h"
#include "PropertyList.h"
#include "BaseOpProperty.h"
#include "../disamb/disamb.h"
#include "MyLinkedList.h"
#include "../mutation/IburgDefs.h"
#include "STLIncludes.h"
#include "OpCode.h"
#include "FilePointer.h"
#include "BaseArgument.h"

class Symtab;

class BasicBlockNode;
class NormalInstr;
class Routine;

//these numbers have to be the same as the ones in field_map file

//AVS : I have changed these. Now these are not the same as in field map fiule
// Previously they were in serial order. I have newly defined the last two.


/*
const int _DEST_ = 1;
const int _SOURCE_1_ = 2;
const int _SOURCE_2_ = 3;
const int _MEM_DEST_ = 4;
const int _MEM_SRC_  = 5;
const int _MEM_BYTE_ = 6;
const int _REG_SRC_  = 9;
const int _REG_DEST_ = 10;
*/

/*
const int _DST_ = 0;
const int _SRC_1_ = 0;
const int _SRC_2_ = 1;
const int _SRC_3_ = 2;
const int _COND_REG_ = 0;
const int _SRC_LABEL_ = 1;
const int _OFFSET_ = 1;
const int _CALL_ADDR_ = 0;
const int _DST_LBL_ = 0;
*/

const int _DEST_ = 1;
const int _SOURCE_1_ = 2;
const int _SOURCE_2_ = 3;
const int _SOURCE_3_ = 4;
const int _MEM_DEST_ = 5;
const int _MEM_SRC_ = 6;
const int _MEM_BYTE_ = 7;
const int _REG_SRC_ = 10;
const int _REG_DEST_ = 11;
const int _DEST_1_ = 12;
const int _DEST_2_ = 13;


const int _DST_ = 1;
const int _DOUBLE_DST_BEGIN_ = 3;
const int _DOUBLE_DST_END_ = 4;
const int _SRC_1_ = 2;
const int _SRC_2_ = 3;
const int _SRC_3_ = 4;
const int _DOUBLE_SRC_BEGIN_ = 5;
const int _DOUBLE_SRC_END_ = 6;
const int _PRED_SRC_BEGIN_ = 7;
const int _PRED_SRC_END_ = 9;
const int _COND_REG_ = 2;
const int _SRC_LABEL_ = 3;
const int _OFFSET_ = 3;
const int _CALL_ADDR_ = 2;
const int _DST_LBL_ = 2;

const int _SRC_LIST_ = 1111;
const int _DEST_LIST_ = 2222;

const int _IMPLIED_DEST_ = 2;

class BaseInstruction;
class BaseArgument;
class BaseOpSlot;

typedef Vector<BaseOperation *> OpList;
typedef OpList::iterator OpListIter;


// added by Radu, 08/22/2001, to remove useless warnings
#ifdef WIN32
#pragma warning(disable:4172)
#endif

class BaseOperation 
{
private:
protected:
	IDType _uniqueID;	// see OperationIncludes.h
	//
	// See Comment 1.
	
	PropertyList<BaseOpProperty> _opProperties;
	
	//
	// The list contains all the properties of an operation.
	// It is a list of pointers to the BaseOpProperty class.
	
public:
	BaseOperation()
	{
		// uniqueID will be initialized in a separate
		// method. 
	}
	
	virtual ~BaseOperation()
	{
	}
	
	virtual void initializeID() = 0;
	
	// initializeID makes no sense for BaseOperation, since we do not
	// expect any object of this class to ever be created separately.
	// So, initializeID becomes a pure virtual function, and by making
	// it equal to 0, we define BaseOperation to be an abstract class
	
	virtual BaseOperation *copy() = 0;
	// 
	// copy makes no sense for an abstract class. (Note that this is an
	// explicit copy, not a copy constructor)
	//
	// Later on, make this return a reference to the object rather than
	// returning a pointer.
	
	virtual BaseOperation *copyBody(bool copyProp=false)
	{
		return NULL;
	}
	
	IDType idValue() const
	{
		return _uniqueID;
	}
	void setID(int val){
		_uniqueID=val;
   }

   friend int operator == (const BaseOperation & a,
			   const BaseOperation & b) 
      {
	 return _YES_;
      }
 
   virtual int isEq(BaseOperation *op)
   {
	   return _NO_;
   }
   
   virtual void print()
      {
      }
   
   virtual void print(ostream& out)
      {
      }
   
   virtual void shortPrint(ostream& out)
      {
      }
   
   // for ASM
   virtual void printASMInstr(ostream &out)
   {
   }
   friend ostream& operator << (ostream & out, const BaseOperation & s)
      {
	 return out;
      }
   //
   // Rewrite the operators == and << later.

   void addProperty(BaseOpProperty & opProperty)
      {
	 _opProperties.addProperty(opProperty);
      }
   
   virtual BaseOpProperty & getProperty(PropertyNames aName)
      {
	 return _opProperties.getProperty(aName);
      }
   
   void printOpProperty(ostream & out)
      {
        _opProperties.print(out);
	}
	
	virtual void printProperties(ostream & out, PropNameList & pL);
	
	virtual void setTargetInstr(BaseInstruction *nextInstr)
	{
	}
	
	// which come from the following fcn
	virtual OpCode & getOpCode()
	{
		// error: Base class virtual method should never be called.
		//
		cout << "Panic: Tried to get opcode object from base operation" << endl;
		assert(0);
		
		static OpCode a;
		return a;
	}
	
	virtual void getOpcodeName(MyString & s) { }
	
	virtual void changeOpCode(char *newStr) { }
	
	virtual int hasOperand(int position, int where = _SRC_LIST_)
	{
		return _NO_;
	}
	
	virtual int hasArg(BaseArgument *arg)
	{
		return _NO_;
	}
	
	virtual BaseArgument *ptrToOperand(int position, int where = _SRC_LIST_)
	{
		return NULL;
	} 
	
	// The next set of methods expect to be given an empty vector which will be filled with
	// the appropriate operands (i.e. args) in the operation.
	//
	// Dest returns the list of dest args
	// Source returns the list of source args
	// All returns all the args in the operation.
	//
	// Note that all is not just the union of dest and source.
	//
	// For example, for a store operation, dest = NULL and source only returns 2 sources.
	// However, all returns two sources and the offset.
	//
	// Similar situations may hold for if ops, goto ops, call ops etc.
	//
	virtual void destOperandsList(vector<BaseArgument *> & a) {}
	virtual void sourceOperandsList(vector<BaseArgument *> & a) {}
	virtual void allOperandsList(vector<BaseArgument *> & a) {}
	//
	// Note: vector<BaseArgument *> is the same as ArgList.
	// We do not use ArgList becos BaseArgument.h includes BaseOperation.h
	// and BaseArgument is forward defined here.
	
	virtual vector<BaseArgument *> & destOperandList() {assert(0); vector<BaseArgument *> a; return a;}
	virtual vector<BaseArgument *> & sourceOperandList() {assert(0); vector<BaseArgument *> a; return a;}
	virtual vector<BaseArgument *> & operandList(int dstOrSrc) {assert(0); vector<BaseArgument *> a; return a;}
	
	virtual void addOperand(BaseArgument *operand, int position, int where = _SRC_LIST_)
	{
	}
	
	virtual BaseInstruction *getTargetInstr()
	{
		return NULL;
	}
	
	virtual char *getTargetLabelStr()
	{
		//NOS
		return NULL;
	}
	
	virtual int isHeaderOp()
	{
		return _NO_;
	}
	
	virtual int isNormalOp()
	{
		return _NO_;
	}
	
	virtual int isPhiOp()
	{
		return _NO_;
	}
	
	virtual int isControlOp()
	{
		return _NO_;
	}
	
	virtual int isFlowOp()
	{
		return _NO_;
	}
	
	virtual int isDataOp()
	{
		return _NO_;
	}
	
	virtual int isIfOp()
	{
		return _NO_;
	}
	
	virtual int isJumpOp()
	{
		return _NO_;
	}
	
	virtual int isGotoOp()
	{
		return _NO_;
	}
	
	virtual int isCallOp()
	{
		return _NO_;
	}
	
	virtual int isRetOp()
	{
		return _NO_;
	}
	
	virtual int isCallRetOp()
	{
		return _NO_;
	}
	
	virtual int isMemoryOp()
	{
		return _NO_;
	}   
	
	virtual int isComputeOp()
	{
		return _NO_;
	}
	
	virtual int isMoveOp()
	{
		return _NO_;
	}
	
	
	virtual expr BuildExpr()
	{
		//NOS
		return NULL;
	}
	
	LinkedListIterator<BaseOpProperty>* propIterForw() {
		return _opProperties.elementsForward();
	}
	
	
	virtual BaseOpProperty* getPtrToProperty(PropertyNames name) {
		return _opProperties.getPtrToProperty(name);
	}

	virtual void recursiveRemoveProperty(PropertyNames name)
	{
		_opProperties.removeProperty(name);
	}

	virtual void print(int indent){
	}
	
	virtual void printCToString(MyString & s){
	}
	
	virtual void printC(int indent)
	{}
	
	virtual void irToC(FilePointer & fp, int indent)
	{}
	
	virtual void createListOfOperands(Symtab *operandSymTable)
	{}
	
	virtual void printToString(char *buf)
	{}
	
	NormalInstr *getInstr(void);
	
	BaseOpSlot *getSlot(BaseInstruction* parentInstr = NULL);
	
	void renameOperands(MyLinkedList<BaseOperation*> &moves);
	
	PropertyList<BaseOpProperty>& propertyList() 
	{
		return _opProperties;
	}
	
	void compute_unifiable_ops(void);
	
	void updateUDDU(BaseOperation *old,MyLinkedList<BaseOperation*> *moves);
	
	int mutation(BaseOperation *&new_op);
	int InstrSel(BaseOperation *&new_op);
	
	virtual NODEPTR_TYPE BuildIburgTree(int rootOfTree,int essential)
	{
		cout << "This function shouldn't be called";
		abort();
		return 0;
	}
	
	virtual int percMoveOP(Routine* routine);
	virtual int percMoveCJ(Routine* routine);
	virtual int percMigrate(Routine* routine);
	
// 	virtual void ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList);
};

// added by Radu, 08/22/2001, to remove useless warnings
#ifdef WIN32
#pragma warning(default:4172)
#endif


int isPresentOperation(BaseOperation *op, OpList OperationsList);

// Forward define.
//
class BaseArgument;

// adds only the register defs to aList. (ArgList)
//
extern void addDefsToList(BaseOperation *oper, Vector<BaseArgument *> & aList);

// adds only the register uses to aList. (ArgList)
//
// if oper is a call op, also adds all its register call parameters to aList.
//
extern void addUsesToList(BaseOperation *oper, Vector<BaseArgument *> & aList);

#endif

// Comment 1:
//
// Note that _uniqueID is not initialized as soon as an operation
// has been created. This is because only certain operations need
// to have IDs. (i.e. operations which are on a list/ actually
// represent real operations etc). Other operations (which are used
// as temporaries) should not have IDs. 
// Here's why this decision was reached:

// When we store operations in a list we make a copy of the
// original operation and store the copy. If every copy got a new
// ID, then we could have a case wherein, another object which
// holds the ID number of the original object would need to be
// changed evertime this operation is added/copied. I thought that
// it would be better to let the user decide what operations should
// have IDs (and *when*). Be warned, this can be very confusing.
// (Especially if the user decides to initialize the IDs at a time
// other than the creation of the operation.)
