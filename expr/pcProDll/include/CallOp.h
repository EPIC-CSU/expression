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
// $Id: CallRetOp.h,v 1.3 1998/01/22 23:58:16 pgrun Exp $
//
// File:  		CallRetOp.h
// Created:		Tue Oct 29, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __CALLRETOP_H__
#define __CALLRETOP_H__

#include "GlobalIncludes.h"
#include "ControlOp.h"
#include "BaseArgument.h"
//#include "Routine.h"
#include "PropertyIncludes.h"
#include "MyString.h"

class Routine;

#define CALL_DEBUG_PRINT_ 333
// Radu - May 21, 2000
//#define CALL_SQRT_ 334
//#define CALL_EXP_ 335

// As of now, CallRetOp inherits only the ControlOp class. Later on, it
// might make sense to inherit the FlowOp class also.

class CallRetOp : public ControlOp 
{
private:
	
#ifdef COUNT_CALLOPS
	static int _numCallRetOps;	// See comment 1 below.
#endif
	
protected:
	
/*
BaseArgument *_callAddress;
//
// For RETURNs, the _callAddress (_SOURCE_1_) contains the register that holds the return value.

	 BaseArgument *_destOperand;
	 BaseArgument *_srcOperand;
	 //
	 // Note: This is used only by special call instructions like DEBUG_PRINT
	 // This typically contains one of the input parameters to the function.
	 
		 //
		 // Note: For a call op, Source_1 is assumed to be the call address.
		 // The list of parameters is kept as a separate list;
		 
		    This was the orignal declaration
		   
			 
			Now we have _sourceOperandList and _destOperandList
			This is the new structure :
			   
			_sourceOperandList[_CALL_ADDR_]	: _callAddress
			_sourceOperandList[_SRC_2_]	: _srcOperand
				 
			_destOperandList[_DST_]		: _destOperand
				   
					 
	*/
	


	Routine *_ptrToRoutine;
	
	
	ArgList _parmList;
	
	// Note: due to SSA, the registers that need to be saved have to be determined
	// at the point of call.
	//
	ArgList _listOfSavedRegs;
	
	static Map<int, MyString> _specialCallMap;
	static Map<int, int> _specialOpcodeMap;
	
	inline char *_getSpecialCallStr(int type)
	{
		char *retPtr;
		retPtr = _specialCallMap[type].getStr(); 
		return retPtr;
	}
	
public:
	
	// 1. Constructor
	CallRetOp();
	
	
	
	
	
	// 2. copy comstructor
	CallRetOp(CallRetOp & a);
	
	
	
	
	
	
	// 3. destructor
	virtual ~CallRetOp();
	
	
	
	
	
	// 4. counter
#ifdef COUNT_CALLOPS
	static int numCallRetOps()
	{
		return _numCallRetOps;
	}
#endif
	
	
	
	
	
	
	// 5. copy
	BaseOperation *copy();
	
	
	
	
	
	
	
	// 6. copyBody
	BaseOperation *copyBody(bool copyProp=false);
	
	
	
	
	
	
	// 7. Initialize
	
	
	
	
	
	
	// 8. Identification
	int isCallRetOp();

	

	
	
	
	// 9. has Operand
	// virtual int hasOperand(int position);
	// Same as in NormalOp:: has Operand
	
	
	
	
	
	
	
	// 10. has Arg
	// virtual int hasArg(BaseArgument *arg);
	// Same as in NormalOp:: has Arg
	
	
	
	
	
	
	
	
	// 11. Add Operand	
	// virtual void addOperand(BaseArgument *operand, int position);
	// Same as in NormalOp:: Add Operand
	
	
	
	
	
	
	
	// 12. Ptr to operands
	// BaseArgument *ptrToOperand(int position);
	// Same as in NormalOp:: Ptr to Operand
	
	
	
	
	
	
	
	// 13. destOperand List
	// virtual void destOperandsList(ArgList & a);
	// Same as in NormalOp :: dest Operand List
	
	
	
	
	
	
	
	// 14. sourceOperand List
	// virtual void sourceOperandsList(ArgList & a);
	// Same as in NormalOp :: source Operand List
	
	
	
	
	
	
	
	
	// 15. allOperand List
	// virtual void allOperandsList(ArgList & a);
	// Same as in NormalOp :: all Operand List
	
	
	
	
	
	
	
	
	// 16. clear Operand
	// virtual void clearOperand(int position, int where = SRC); 
	// Same as in NormalOp :: clear Operand
	
	
	
	
	
	
	
	// 17. reset
	void reset();
	
	
	
	
	
	
	
	// 18. isEq
	virtual int isEq(BaseOperation *op);
	//
	// isEq returns _YES_ if the call address, the destination arg, and the
	// first source operand match.
	//
	// Note: It does not check the rest of the arguments. 
	
	
	
	
	
	// 19. operator ==
	friend int operator == (const CallRetOp & a, const CallRetOp & b)
	{
		return _YES_;
	}

	
	
	
	// 20. operator <<
	friend ostream & operator << (ostream & out, const CallRetOp & s)
	{
		return out;
	}

	
	
	
	
	// 21. Create List of Operands
	// void createListOfOperands(Symtab *operandSymTable);
	// Same as in NormalOp :: create list of operands
	
	
	
	
	
	// 22. PrintIndent
	
	
	
	
	
	
	// 23.  Print Indent in a file
	
	
	
	
	
	
	// 24. Print
	void print();
	
	
	
	
	
	
	// 25. Print with indent
	virtual void print(int indent);
	
	
	
	
	
	
	
	// 26. Print in outstream 
	void print(ostream& out);
	
	
	
	
	
	
	// 27. Print  C to String
	// virtual void printCToString(MyString & s);
	
	
	
	
	
	
	
	// 28. Print C
	virtual void printC(int indent);
	
	
	
	
	
	
	
	// 29. IR to C
	virtual void irToC(FilePointer & fp, int indent);
	
	
	
	
	
	
	
	
	// 30. Print to String
	virtual void printToString(char *buf);
	
	
	
	
	
	
	
	
	// 31. Short Print
	void shortPrint(ostream& out);
	
	
	
	
	
	
	
	
	// 32. Print Properties
	void printProperties(ostream & out, PropNameList & pL);
	
	
	
	
	
	
	
	// 33 Some specific operations
	
	
	// 34. get Target Instruction
	BaseInstruction *getTargetInstr();
	
	
	// 35. init Param List
	virtual void initParmList(ArgList & a);
	
	
	// 36. addParam
	virtual void addParm(BaseArgument *a);
	
	
	// 37. init Saved Register List
	virtual void initSavedRegList(ArgList & a);
	

	virtual ArgList & getParmList();
	
	
	// 38. add saved register
	virtual void addSavedReg(BaseArgument *a);
	
	
	//39. is Call Op
	int isCallOp();
	
	
	//40. is Return Op
	int isRetOp();
	
	
	// 41. add Special Call
	static void addSpecialCall(int type, MyString & str);
	
	
	// 42. is special call
	int isSpecialCall();
	
	
	// 43. is special call
	int isSpecialCall(int type);
	
	
	// 44. is dynamic address call
	int isDynamicAddrCall();
	
	
	
	// 45. add special opcode
	static void addSpecialOpcode(int type, int opcode);
	
	
	// 46. get special call type
	int getSpecialCallType();
	
	
	
	// 47. get special opcode
	int getSpecialOpcode(int type);
	
	
	
	// 48. get target label string
	char *getTargetLabelStr();
	
	
	
	virtual ArgList & getSavedRegList();
	
	Routine *ptrToRoutine();
	
	virtual void setRoutine(Routine *rPtr);
	
	

 void printASMInstr(ostream &out);

};

#endif

// Comment 1:
//
// _numCallRetOps is used to keep a count of the number of CallRetOp objects
// in existence at any point. COUNT_CallRetOpS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
