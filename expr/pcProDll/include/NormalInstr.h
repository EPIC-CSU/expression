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
// $Id: NormalInstr.h,v 1.44 1998/04/11 00:35:41 xji Exp $
//
// File:  		NormalInstr.h
// Created:		Fri Nov 08, 96
// Last modified: 	Fri Nov 08, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: INSTRUCTION

#ifndef __NORMALINSTR_H__
#define __NORMALINSTR_H__

#include "BaseInstruction.h"
#include "HeaderOp.h"
#include "Array.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "FlowOpSlot.h"
#include "MyLinkedList.h"
#include "BaseHTGNode.h"
#include "RegisterArg.h"
#ifndef WIN32
#include <assert.h>
#endif


// Defining some special variables (that are set at compile/run time).

extern int numDataOpsPerInstr;
//
// Defines the (max) number of data operations permitted in an
// instruction.

extern int numControlOpsPerInstr;
//
// Defines the (max) number of control operations permitted in an
// instruction.

extern int numFlowOpsPerInstr;
//
// Defines the (max) number of flow operations permitted in an
// instruction.

class UnifiableOp{
public:
 BaseOperation *oper;
 MyLinkedList<BaseOperation*> moves;
 UnifiableOp(BaseOperation *o,MyLinkedList<BaseOperation*> m):oper(o),moves(m){}
};

class NormalInstr : public BaseInstruction
{
public:
#ifdef WIN32_GUI//WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI
 //virtual void Draw(GRAPH_NODE_TYPE nodeType, CDC * pDC);//, int x_pos, int y_pos);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out);
 virtual void DOTText(GRAPH_NODE_TYPE nodeType, CString& out, BasicBlockNode* block);
 BasicBlockNode* hackGetTargetBlock(BaseInstruction* target, BasicBlockNode* block);
 virtual void DisplayProperties(BasePropPage* dialog);
 virtual void printToString(char *buf);
 void printOpcodesToString(char *buf, char* delim = NULL);
 virtual void DrawInfo(GRAPH_NODE_TYPE nodeType, CDC * pDC, CRect& rect);
#endif //WIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUIWIN32_GUI

private:
 
#ifdef COUNT_NORMALINSTRS
 static int _numNormalInstrs;	
 //
 // _numNormalInstrs is used to keep a count of the number of
 // NormalInstr objects in existence at any point. COUNT_NORMALINSTRS can
 // be either defined or undefined in the file "InstructionIncludes.h".
 // The count information will be used for debugging (and/or
 // statistical) purposes. It might be a good idea to allow the user to
 // specify whether he needs this info.
 //
#endif
 
protected:
 
 BaseArgument *_instrLabel;
 
 HeaderOp _opTreeHeader;
 
 Array<DataOpSlot> _dataOpList;
 Array<ControlOpSlot> _controlOpList;
 Array<FlowOpSlot> _flowOpList;
 
 ArrayIterator<FlowOpSlot> *_succListIter;
 //
 // This member data should be used only by the member functions.
 // This should not be accessible to the outside world.
 
 MyLinkedList<IDType> _listOfSuccessorIDs;
 
 MyLinkedList<UnifiableOp*> _unifiableOps; 
 
public:
 NormalInstr();
 
 NormalInstr(NormalInstr & copyInstr);

 virtual ~NormalInstr();
 
#ifdef COUNT_NORMALINSTRS
 static int numNormalInstrs()
 {
	 return _numNormalInstrs;
 }
#endif
 
 void initializeID()
 {
 }
 
 int isNormalInstr()
 {
	 return _YES_;
 }
 
 void addLabel(BaseArgument *label)
 {
	 _instrLabel = label->copy();
 }
 
 BaseArgument *getPtrToLabelArg()
 {
	 return _instrLabel;
 }
 
 void deleteLabel()
 {
	 if (_instrLabel != NULL)
		 delete _instrLabel;
	 _instrLabel = NULL;
 }

 int hasLabel()
 {
	 if (_instrLabel != NULL)
   return _YES_;
	 return _NO_;
 }
 
 void initAddDataOp(DataOp *someOpPtr, int pos = _INVALID_, bool noCopy = false);
 
 int addDataOpToList(DataOp *someOpPtr, int pos = _INVALID_, bool noCopy = false);
 
 void initAddControlOp(ControlOp *someOpPtr, int pos = _INVALID_, bool noCopy = false);
 
 int addControlOpToList(ControlOp *someOpPtr, int pos = _INVALID_, bool noCopy = false);
 
 void initAddFlowOp(FlowOp *someOpPtr, int pos = _INVALID_, bool noCopy = false);
 
 int addFlowOpToList(FlowOp *someOpPtr, int pos = _INVALID_, bool noCopy = false);
 
 void initScheduleAndAdd(BaseOperation *someOpPtr, bool noCopy = false);
 
 int addOperation(BaseOperation *someOpPtr, int pos = _INVALID_, bool noCopy = false);
 
 BaseOpSlot *getPtrToSlot(BaseOperation *op);

 BaseOpSlot & getDataOpSlot(int index)
 {
	 return _dataOpList.elementAt(index);
 }
 
 BaseOpSlot & getControlOpSlot(int index)
 {
	 return _controlOpList.elementAt(index);
 }
 
 BaseOpSlot & getFlowOpSlot(int index)
 {
	 return _flowOpList.elementAt(index);
 }
 
 ArrayIterator<DataOpSlot> *getDataOpSlotIter()
 {
	 ArrayIterator<DataOpSlot> *retPtr;
	 retPtr = _dataOpList.elements();
  
	 return retPtr;
 }
 
 ArrayIterator<ControlOpSlot> *getControlOpSlotIter()
 {
	 ArrayIterator<ControlOpSlot> *retPtr;
	 retPtr = _controlOpList.elements();
	 
	 return retPtr;
 }
 
 ArrayIterator<FlowOpSlot> *getFlowOpSlotIter()
 {
	 ArrayIterator<FlowOpSlot> *retPtr;
	 retPtr = _flowOpList.elements();
  
	 return retPtr;
 }
 
 void setRootOp(int rootType, int rootIndex)
 {
	 _opTreeHeader.setType(rootType);
	 _opTreeHeader.setIndex(rootIndex);
 }

 void setRootOp(BaseOpSlot *slot);
 
 int getRootOpType()
 {
	 return _opTreeHeader.getType();
 }
 
 int getRootOpIndex()
 {
	 return _opTreeHeader.getIndex();
 }
 
 BaseOperation *getPtrToRootOperation();
 
 BaseOpSlot & getRootOpSlot();
 
 BaseInstruction *getFirstSuccOnly(void);
 
 BaseInstruction *getFirstSucc();
 
 BaseInstruction *getNextSucc();
 
 void deleteSuccIter()
 {
	 if (_succListIter == NULL) return;
	 delete _succListIter;
	 _succListIter = NULL;
 }
 
 BaseOperation *getPtrToOperation(IDType index);
 
 void initListOfSuccessorIDs();
 
 
 // Nov, 15, 96.
 // Do I need copy ?
 // What happens to the static _numCount stuff ?
 //
 // Nov, 18, 96
 // See BaseInstruction.h for an explanation
 
 virtual BaseInstruction *copy();
 
 virtual BaseInstruction *copyBody();
 
 virtual void setNextInstr(BaseInstruction *next, int flowOpNum = _DEFAULT_NUM_);
 
 friend int operator == (const NormalInstr & a, const NormalInstr & b)
 {
	 // this is not really needed. Also it is just too painful. 
	 return _YES_;
	 //
	 // No member data to compare for equivalency.
 }
 
 friend ostream & operator << (ostream & out, const NormalInstr & s)
 {
	 return out;
 }
 
 void removeSuccID(int value) {
  _listOfSuccessorIDs.remove(value);
 }
 
 void addSuccID(int value) {
  _listOfSuccessorIDs.append(value);
 }
 
 
 void print(ostream&);
 void shortPrint(ostream&);
 void printASMInstr(ostream&);
 
 void printOpsProperty(ostream & out); 
 
 void printArgsProperty(ostream & out); 
 
 void printProperties(ostream & out, PropNameList & pL);

 void print();
 
 virtual void testDisamb(BaseInstruction *instr);// this is for testing the disamb. Code in disamb part
 
 void setHeaderOp(int type, int index) {
  _opTreeHeader.setIndex(index);
  _opTreeHeader.setType(type);
 }
 
 /***********
 
   void delDataSlot(DataOpSlot* slot) {
   _dataOpList.removeElement(*slot);
   }
   
     void delFlowSlot(FlowOpSlot* slot) {
     _flowOpList.removeElement(*slot);
     }
     
       void delCntlSlot(ControlOpSlot* slot) {
       _controlOpList.removeElement(*slot);
       }
       
 ***********/
 
 Array<DataOpSlot>* getDataOpList() {
  return &_dataOpList;
 }
 
 Array<FlowOpSlot>* getFlowOpList() {
  return &_flowOpList;
 }
 
 Array<ControlOpSlot>* getControlOpList() {
  return &_controlOpList;
 }
 
 virtual int hasMultipleSuccs(void);

 BaseInstruction *getDefaultTargetInstr();

   Array<BaseOpSlot*> & getReachingFlowOp(BaseInstruction *instr);
   friend void moveOperation(NormalInstr *src, BaseOpSlot *o, NormalInstr *dst);
   friend void moveConditional(NormalInstr *src, BaseOpSlot *o, NormalInstr *dst);
   BaseOpSlot * getAvailSlot(BaseOperation *o);
   void setTargetMask(int mask);
   int isEmpty(void);
   int containsOp(BaseOperation *op);
   int containsOpNotMove(BaseOperation *op,MyLinkedList<BaseOperation*> &moves);
   int countDataOps(void);
   int countControlOps(void);
   void setTargetInstr(BaseInstruction *instr);
   void setTargetInstrForAllFlowOps(BaseInstruction *instr);
   
   BaseOpSlot *getCorespSlot(BaseOpSlot * slot,NormalInstr * instr);

   int getDataSlotIndex(DataOpSlot *slot,Array<DataOpSlot> &slotList);

   int getControlSlotIndex(ControlOpSlot *slot,Array<ControlOpSlot> &slotList);

   int getFlowSlotIndex(FlowOpSlot *slot,Array<FlowOpSlot> &slotList);

   int getSlotIndex(BaseOpSlot *slot);

   virtual void print(int indent);
   virtual void printC(int indent);
   virtual void irToC(FilePointer & fp, int indent);

   virtual void createListOfOperations(OpList & oplist);

   void createListOfOperands(Symtab *operandSymTable);

   void schedule(void);

   void compute_unifiable_ops(void);

   virtual int contains_unifiable_op(BaseOperation *oper);

   int find_unifop_destination(BaseOperation *oper,MyLinkedList<BaseOperation *> moves);

   int addUnifiableOp(BaseOperation *oper,MyLinkedList<BaseOperation *> &moves);

   void updateSetsForDependents(BaseOperation *oper);

   void updateSetsForDependentsNew(BaseOperation *oper);

   int insert(BaseOperation *oper,vector<BaseHTGNode *> *path=NULL);

   int extract(BaseOperation *oper, bool keepUDDU = false);

   virtual int migrate(BaseOperation *oper,vector<BaseHTGNode *> *path);

   virtual int depends(BaseOperation *oper,MyLinkedList<BaseOperation *> &moves);
   virtual int cannotMoveBeyond(BaseOperation *oper,MyLinkedList<BaseOperation *> &moves);

   virtual void removeUnifiableOps(BaseOperation *oper);

   void appendInstr(BaseInstruction *prevInstr, int nextInstrDir,int change);

   virtual void renameAllDestValues();

   virtual void setLowestSSAValues(int type = _DEST_LIST_);
   
   virtual void renameAllSrcValues(ValuesSET & in, ValuesSET & out);

   virtual void getLocalDefs(ValuesSET & defSet);

   void removeAllOp();

/*   virtual void makeFullCopy(BaseInstruction* & instrPtr); */

   virtual void mutation(void);

   virtual void InstrSel(void);

   BasicBlockNode *getBasicBlock(void);

   void setOpID(int &opcrt);

   // If the instruction has a CALLRET op returns a pointer to it.
   // else it returns NULL
   //
   // Note: it returns a pointer to the first callret op in the controlFlowList.
   //
   BaseOperation *hasCallRetOp();

   // If the instruction has a If op returns a pointer to it.
   // else it returns NULL
   //
   // Note: it returns a pointer to the first callret op in the controlFlowList.
   //
   BaseOperation *hasIfOp();

   // If the instruction has a Goto Op, returns a pointer to it, else returns NULL.
   //
   BaseOperation *hasGotoOp();

   BaseOperation *getOpWithArg(BaseArgument *arg, int pos = _INVALID_);
   //
   // Returns operation which has arg as one of its operands.
   // In case of multiple ops with arg, returns the first op.
   // Note: The method does not check pointer equality.
   //
   // If pos = _INVALID_, checks all slot lists, else checks only the specified ones.
   // pos = One of : _INVALID_, DATA_OP, CONTROL_OP, FLOW_OP

   virtual int percUnify(OpList* opList, Routine* routine);
   virtual int percUnifyWrap(Routine* routine);
   virtual int percMigrate(Routine* routine, BaseOperation* op);

   virtual BaseOperation* NormalInstr::hasSameOp(BaseOperation* op);
   virtual void updateInstrUDDU(BaseInstruction* old, MyLinkedList<BaseOperation*>* moves);

   virtual void ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList);

   virtual void recursiveRemoveProperty(PropertyNames name);
};

// This function goes thru predInstr and changes all pointers that point to currInstr to point to newInstr
//
extern void changePointers(BaseInstruction *predInstr, BaseInstruction *currInstr, BaseInstruction *newInstr);

// Fills aList with all defines in instr
extern void listOfDefs(BaseInstruction *instr, ArgList & aList);

// Fills aList with all uses in instr
extern void listOfUses(BaseInstruction *instr, ArgList & aList);

#define ITERATE_OVER_DATA_OPS(instr, some_func) \\
	ArrayIterator<DataOpSlot> *diter; \\
	diter = instr->getDataOpSlotIter(); \\
	while (diter->hasMoreElements()) \\
	{ \\
		if ((diter->currentElement()).hasOper()) \\
			some_func((diter->currentElement()).getPtrToOperation()); \\
		diter->nextElement(); \\
	} \\
	delete diter;

#define ITERATE_OVER_CONTROL_OPS(instr, some_func) \\
	ArrayIterator<ControlOpSlot> *citer; \\
	citer = instr->getControlOpSlotIter(); \\
	while (citer->hasMoreElements()) \\
	{ \\
		if ((citer->currentElement()).hasOper()) \\
			some_func((citer->currentElement()).getPtrToOperation()); \\
		citer->nextElement(); \\
	} \\
	delete citer;


#endif
