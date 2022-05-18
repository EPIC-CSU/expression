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
// $Id: MemoryOp.h,v 1.17 1998/03/21 01:19:07 ahalambi Exp $
//
// File:  		MemoryOp.h
// Created:		Tue Oct 29, 96
// Last modified: 	Thu Oct 31, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __MEMORYOP_H__
#define __MEMORYOP_H__

#include "DataOp.h"
#include "OpCodeTable.h"
#include "BaseArgument.h"
#include "disamb.h"
#include "IburgDefs.h"
#include "PropertyIncludes.h"

// One important Note:
//
// Load and Store Ops are both memory ops. However, they have one important distinction.
// A Load defines a register value (the _destOperand) while Store defines a memory value
// (the location pointed to by contentsof(_destOperand) + _offset).
//
// So, in terms of registers, Load uses a register, and defines another
// while Store uses two registers.
//
// This distinction is very important during the RSSA, unrolling (and any other register
// renaming phases).
//
//

class MemoryOp : public DataOp 
{
private:
	
#ifdef COUNT_MEMORYOPS
	static int _numMemoryOps;	// See comment 1 below.
#endif
	
protected:
	
public:
	
/*
MemoryOp format

	Load Operations are: <opcode> <dest> <base-address-reg> <offset>
	Store Operations are: <opcode> <base-address-reg> <store-source> <offset>

  Originally, for load : 
	_sourceOperand contained the base-address-reg, _offset contained the immediate offset, and _destOperand contained the dest register address.
  For Store:
	_sourceOperand contained the store-source, _offset contained the immediate offset, and _destOperand contained the base-address-reg.

  Now, after making sources and dests as lists, this has been changed to:

  Load Operations : _destOperandList[_DEST_] : <dest> (corresponding to _destOperand)
					_sourceOperandList[_SOURCE_1_] : <base-address-reg> (corresponding to _sourceOperand)
					_sourceOperandList[_OFFSET_] : <offset> (corresponding to _offset)

  Store Operations :
					_sourceOperandList[_SOURCE_1_] : <base-address-reg> (corresponding to _sourceOperand)
					_sourceOperandList[_OFFSET_] : <offset> (corresponding to _offset)
					_sourceOperandList[_SOURCE_3_] : <store-source> (corresponding to _destOperand).


  NOTE: _OFFSET_ = 1

*/
	
	// 1. Constructor
	MemoryOp();
	
	
	
	
	// 2. Copy constructor
	MemoryOp(MemoryOp & a);
	
	
	
	
	
	
	// 3. Destructor
	virtual ~MemoryOp();
	
	
	
	
	// 4. counter  	
#ifdef COUNT_MEMORYOPS
	static int numMemoryOps()
	{
		return _numMemoryOps;
	}
#endif
	
	
	
	
	// 5. copy function
	BaseOperation *copy();
	
	
	
	
	
	
	// 6. copyBody
	BaseOperation *copyBody(bool copyProp=false);
	
	
	
	
	
	
	// 7. Initialize
	//
	// This is the same as NormalOp::Initialize
	
	
	
	
	
	
	// 8. Identification
	int isMemoryOp();
	
	
	
	
	
	// 9. has Operand
	//
	// This is the same as NormalOp::has Operand

	
	
	
	
	
	
	// 10. has Arg
	//
	// This is the same as NormalOp:: has Arg
	
	
	
	
	
	
	// 11. Add operand
	//
	// This is the same as NormalOp::Add Operand

	void addSourceOperand(BaseArgument *operand);
	
	void addDestOperand(BaseArgument *operand);
	
    void setOffset(BaseArgument *offset); 
		
	
	
	
	
	
	// 12. Ptr to Operand
	//
	// This is the same as NormalOp:: Ptr to Operand
	
	BaseArgument *ptrToSourceOperand();
	
	BaseArgument *ptrToDestOperand();
	
	BaseArgument *getOffset();
		
	
	
	
	
	// 13. destOperand List
	//
	// This is the same as NormalOp:: Dest Operand List
	
	
	
	
	
	// 14. sourceOperand List
	//
	// This is the same as NormalOp:: Source Operand List
	
	
	
	
	
	// 15. allOperand List
	//
	// This is the same as NormalOp:: All Operand List
	
	
	
	
	
	// 16. clear Operand
	//
	// This is the same as NormalOp:: Clear Operand
	
	
	
	
	
	// 17. reset
	//
	// This is the same as NormalOp:: reset
	
	
	
	
	
	// 18. isEq
	int isEq(BaseOperation *op);
	
	
	
	
	
	// 19. operator ==
	friend int operator == (const MemoryOp & a, const MemoryOp & b)
	{
		return _YES_;
	}
	
	
	
	
	// 20. operator <<
	friend ostream & operator << (ostream & out, const MemoryOp & s)
	{
		return out;
	}
	
	
	
	
	
	// 21. Create List of Operands
	//
	// This is the same as NormalOp:: create list of operands
	
	
	
	
	
	
	// 22. PrintIndent
	// extern void printIndent(int indent);
	
	
	
	
	
	
	
	
	// 23. PrintIndent in a file
	// extern void printIndent(FilePointer & fp, int indent);
	
	
	
	
	

	
	// 24. Print
	void print();
	
	
	
	
	
	
	// 25. Print with indent
	virtual void print(int indent);
	
	
		
	
	
	
	
	
	// 26. Print in outstream 
	void print(ostream& out);
	
	
	
	
	
	
	
	// 27. Print  C to String
	virtual void printCToString(MyString & s);

	
	
	
	
	
	// 28. Print C
	virtual void printC(int indent);

	
	
	
	
	
	// 29. IR to C
	virtual void irToC(FilePointer & fp, int indent);
	
	
	
	
	
	
	// 30. Print to String
	virtual void printToString(char *buf);
	
	
	
	
	
	
	// 31. Short Print
	// 
	// Call the NormalOp::shortPrint instead.
	//
	// void shortPrint(ostream& out);
	
	
	
	
	
	
	// 32. Print Properties
	void printProperties(ostream & out, PropNameList & pL);
	
	
	
	


	
	// 33. Some specific operations
	
	int isLoad();
	
	int isStore();
	
	virtual expr BuildExpr();//disamb
	
	virtual NODEPTR_TYPE BuildIburgTree(int rootOfTree,int essential);
	
};

#endif

// Comment 1:
//
// _numMemoryOps is used to keep a count of the number of MemoryOp objects
// in existence at any point. COUNT_MEMORYOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
