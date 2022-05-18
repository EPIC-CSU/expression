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
// $Id: BaseInstruction.h,v 1.24 1998/04/09 18:47:57 pgrun Exp $
//
// File:  		BaseInstruction.h
// d:		Fri Nov 08, 96
// Last modified: 	Fri Nov 08, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION
//
// The BaseInstruction class is similar in may respects to the
// BaseOperation class. See the comments in that file if anything here
// requires an explanation. This is an abstract class. The data
// structure which will hold the operations is defined in the
// NormalInstr class and the phi operations are contained in the
// PhiInstr class.

#ifndef __BASEINSTRUCTION_H__
#define __BASEINSTRUCTION_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "BaseHTGNode.h"
#include "InstructionIncludes.h"
#include "BaseOpSlot.h"
#include "BaseArgument.h"
#include "LabelArg.h"
#include "BaseInstrProperty.h"
#include "PropertyList.h"
#include "BasicBlockNode.h"
#include "Flags.h"

const int _TRUE_BRANCH_ = 1;
const int _FALSE_BRANCH_ = 0;

const int _DEFAULT_NUM_ = -1;
const int _SECOND_NUM_ = -2;
const int _LAST_NUM_ = -3;


// added by Radu, 08/22/2001, to remove useless warnings
#ifdef WIN32
#pragma warning(disable:4172)
#endif

class BaseInstruction: public BaseHTGNode
{
public:
#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
 //virtual void Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC);//, int x_pos, int y_pos);
 virtual void DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out, BasicBlockNode* block);
 virtual GenericNode* FirstNotExpandedNode(GRAPH_NODE_TYPE nodeType);
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

private:
protected:
 IDType _uniqueID;
 int _isBreak;
 
 PropertyList<BaseInstrProperty> _propertyPtr;
 
public:
 BaseInstruction();
 
 virtual ~BaseInstruction()
 {
 }
 
 virtual BaseInstruction *copyBody()
 {
	 cout << "Error: Base class virtual method should never be called" << endl;
	 exit(1);
	 return NULL;
 }
 
 virtual void setNextInstr(BaseInstruction *next, int flowOpNum = _DEFAULT_NUM_)
 {
	 cout << "Error: Base class virtual method should never be called" << endl;
	 exit(1);
      }

   virtual void renameAllDestValues()
   {
	 cout << "Error: Base class virtual method should never be called" << endl;
	 exit(1);
   }

   virtual void setLowestSSAValues(int type = _DEST_LIST_)
   {
	 cout << "Error: Base class virtual method should never be called" << endl;
	 exit(1);
   }

   virtual void getLocalDefs(ValuesSET & defSet)
   {
	 cout << "Error: Base class virtual method should never be called" << endl;
	 exit(1);
   }

   virtual void renameAllSrcValues(ValuesSET & in, ValuesSET & out)
   {
	 cout << "Error: Base class virtual method should never be called" << endl;
	 exit(1);
   }

   virtual PropertyList<BaseInstrProperty> & getPropertyList()
      {
	 return _propertyPtr;
 }
 
 int isBreakpoint()
 {
  return _isBreak;
 }
 
 void setBreakpoint()
 {
  _isBreak = 1;
 }
 
 void clearBreakpoint()
 {
  _isBreak = 0;
 }
 
 virtual void initializeID() = 0;
 
 IDType idValue() const
 {
	 return _uniqueID;
 }
 
 void setID(IDType val)
 {
	 _uniqueID=val;
 }
 
 virtual int isNormalInstr()
 {
	 return _NO_;
 }
 
 virtual int isPhiInstr()
 {
	 return _NO_;
 }
 
 virtual int hasLabel()
 {
	 return _NO_;
 }
 //
 // This function is needed in BuildBB.cc
 
 // Nov 15, 96
 // Do I need copy ?
 // 
 // Nov 16, 96
 // Yes, you do.
 // num...Instrs variable keeps track of the number of Instruction
 // objects created. Now, if you create a copy, then it can be one of
 // two things :
 // It is going to be used exactly like an instruction, in which case
 // it should be counted, or,
 // It is created because of the rule that the creating and destroying
 // the object should be sort of symmetrical (i.e. the scope in which
 // the object is created should be the same as the scope in which it
 // is destroyed.) Now, this reason indicates to us that this object
 // should be viewed as the "real" instruction, and the object from
 // which it was copied is like a "temp" variable.
 // 
 // I don't think that the above explanation is very clear. :-)
 //
 virtual BaseInstruction *copy() = 0;
 
 virtual int getRootOpType()
 {
  //NOS
  return 0;
 }
 
 virtual BaseOpSlot & getRootOpSlot()
 {
  //NOS
  ASSERT_TRACE(0,"Forced EXIT!");
  BaseOpSlot* t;
  return *t;
 }
 
 virtual BaseOperation *getPtrToRootOperation()
 {
  //NOS
  return NULL;
 }
 
 virtual BaseArgument *getPtrToLabelArg()
 {
  //NOS
  return NULL;
 }
 
 virtual BaseInstruction *getFirstSuccOnly()
 {
  //NOS
  return NULL;
 }
 
 virtual BaseInstruction *getFirstSucc()
 {
  //NOS
  return NULL;
 }
 
 virtual BaseInstruction *getNextSucc()
 {
  //NOS
  return NULL;
 }

 virtual int hasMultipleSuccs()
 {
  //NOS
  return -1;
 }
 
 virtual void deleteSuccIter()
 {
 }
 
 virtual BaseOperation *getOpWithArg(BaseArgument *arg, int pos = _INVALID_)
 {
	 return NULL;
 }

 friend int operator == (const BaseInstruction & a,
  const BaseInstruction & b) 
 {
	 return _YES_;
 }
 
 friend ostream& operator << (ostream & out, const BaseInstruction & s)
 {
	 return out;
 }
 //
 // Rewrite the operators == and << later.
 
 virtual void print()
 {
  _propertyPtr.print();
 }

 virtual void printToString(char *buf){
 }

 virtual void printOpcodesToString(char *buf, char* delim = NULL){
 }
 
 virtual void print(ostream& out)
 {
  _propertyPtr.print( out);
 }
 
 virtual void shortPrint(ostream& out)
 {
 }

 // for ASM
 virtual void printASMInstr(ostream& out)
 {
 }
 
 void printInstrProperty(ostream & out)
 {
  _propertyPtr.print(out);
 }
 
 virtual void printOpsProperty(ostream & out)
 {
 }
 
 virtual void printProperties(ostream & out, PropNameList & pL);

 virtual void addProperty(BaseInstrProperty & someProp)
	{
		_propertyPtr.addProperty(someProp);
	}
 
 virtual BaseInstrProperty&  getProperty(PropertyNames aName)
	{
		return _propertyPtr.getProperty(aName);
	}

	virtual BaseInstrProperty* getPtrToProperty(PropertyNames name) {
		return _propertyPtr.getPtrToProperty(name);
	}

   virtual void testDisamb(BaseInstruction *instr){
        }

   LinkedListIterator<BaseInstrProperty>* propIterForw() {
      return _propertyPtr.elementsForward();
   }

   virtual void recursiveRemoveProperty(PropertyNames name);

   virtual void setTargetInstr(BaseInstruction *instr)=0;

   virtual void print(int indent){
   }

   virtual void printC(int indent){
   }

   virtual void irToC(FilePointer & fp, int indent){
   }

   virtual void createListOfOperations(OpList & oplist) 
   {}

   virtual void createListOfOperands(Symtab *operandSymTable) {
   }

   virtual int migrate(BaseOperation *oper,vector<BaseHTGNode *> *path){
	   // SAH, Nov 03 99 : Changed from BaseHTGNode *node; 
	   // Cos, VC6 gives warning about un-initalized use.
	   //
     BaseHTGNode *node=NULL;
	 path->push_back(this);
     node=prevTB(node,oper,path);
     if(node!=NULL){
		 return node->migrate(oper,path);
	 }
     else return 0;
   }

   virtual int extract(BaseOperation *oper, bool keepUDDU = false){
     cout << "Should not be called for other than normal instr." << endl;
	 return -1;
 }
 
 virtual int find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *>moves){
  BaseHTGNode *node;
  node=prevTB(this,oper);
  if(node!=NULL) return node->find_unifop_destination(oper,moves);
  else return 0;
 }

 Flags flags;

 virtual int percUnify(OpList* opList, Routine* routine);
 virtual int percUnifyWrap(Routine* routine);
 virtual int percMigrate(Routine* routine, BaseOperation* op);

 virtual void updateInstrUDDU(BaseInstruction* old, MyLinkedList<BaseOperation*>* moves);

 virtual void ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList);

 virtual int getLoc();
};

// added by Radu, 08/22/2001, to remove useless warnings
#ifdef WIN32
#pragma warning(default:4172)
#endif


typedef Vector<BaseInstruction *> InstrList;
typedef InstrList::iterator InstrListIter;

#endif

