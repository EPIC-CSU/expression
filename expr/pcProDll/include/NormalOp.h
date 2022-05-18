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
// $Id: NormalOp.h,v 1.6 1998/01/22 23:59:17 pgrun Exp $
//
// File:  		NormalOp.h
// Created:		Mon Oct 28, 96
// Last modified: 	July 30th, 2000 (AVS) Predication 
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION
//
/*

  Changed by AVS on 5th August
  Instead of having differnt sources and operands for every type of operation
  We now have a sourceOperandList and destOperandList.
  So now we can generalize quite a lot of functions and in fact bring them down
  in the class definition hierarchy.
*/

#ifndef __NORMALOP_H__
#define __NORMALOP_H__

#include "BaseOperation.h"
#include "OpCode.h"
#include "BaseArgument.h"

class NormalOp : public BaseOperation 
{
private:
	
#ifdef COUNT_NORMALOPS
	static int _numNormalOps;	// See comment 1 below.
#endif
	
protected:
	OpCode _opCode;
	ArgList _sourceOperandList;
	ArgList _destOperandList;
	
	
	// hasSourceOperand
	int hasSourceOperand(int position);
	
	// hasDestOperand
	int hasDestOperand(int position);
	
	// addSourceOperand
	void addSourceOperand(BaseArgument *operand, int position);
	
	// addDestOperand
	void addDestOperand(BaseArgument *operand, int position);
	
	// ptrtoSourceOperand
	BaseArgument *ptrToSourceOperand(int position);
	
	// ptrtoDestOperand
	BaseArgument *ptrToDestOperand(int position);
	
	// clearSourceOperand
	void clearSourceOperand(int position); 
	
	// clearDestOperand
	void clearDestOperand(int position); 
	
	
public:

	
/*
NormalOp format

  We have made sources and dests as lists.
  _destOperandList
  _sourceOperandList

*/

	
	// 1. constructor
	NormalOp();

	



	// 2. copy constructor
	// We do not even declare this. 
	// So this becomes a pure virtual class.





	
	
	// 3. Destructor
	virtual ~NormalOp();
	
	


	
	// 4. counter
#ifdef COUNT_NORMALOPS
	static int numNormalOps()
	{
		return _numNormalOps;
	}
#endif
	
	
	


	// 5. copy
	
	
	


	
	// 6. copyBody
	//virtual BaseOperation *copyBody(bool copyProp=_NO_);
	
	
	
	
	
	
	// 7. Initialize
	void initializeID();

	




	
	// 8. Identify
	int isNormalOp();
	


	

	// 9. has Operand
	virtual int hasOperand(int position, int where = _SRC_LIST_);
	
	
	




	// 10. has Arg
	virtual int hasArg(BaseArgument *arg);
	


	


	
	// 11. Add Operand
	virtual void addOperand(BaseArgument *operand, int position, int where = _SRC_LIST_);
	
	
	





	// 12. ptr to operand
	virtual BaseArgument *ptrToOperand(int position, int where = _SRC_LIST_);
	
	
	
	virtual ArgList & destOperandList();

	virtual ArgList & sourceOperandList();

	virtual ArgList & operandList(int dstOrSrc);

	
	// 13. destOperand List
	virtual void destOperandsList(ArgList & a);
	
	




	
	// 14. sourceOperand List
	virtual void sourceOperandsList(ArgList & a);
	
	
	




	// 15. allOperand List
	virtual void allOperandsList(ArgList & a);
	





	
	// 16. clear Operand
	virtual void clearOperand(int position, int where = _SRC_LIST_); 
	



	


	// 17. reset
	virtual void reset();
	
	





	// 18. isEq
	



	
	// 19. operator ==
	friend int operator == (const NormalOp & a, const NormalOp & b)
	{
		return _YES_;
	}
	



	// 20. operator <<
	friend ostream & operator << (ostream & out, const NormalOp & s)
	{
		out << "OpCode: ";
		out << s._opCode;
		
		return out;
	}
	
	
	
	

	// 21. create List of Operands
	void NormalOp :: createListOfOperands(Symtab *operandSymTable);
	





	// 22. PrintIndent
	





	// 23. PrintIndent in a file

	
	
	
	
	
	// 24.  Print
	void print();






	// 25.  Print with indent
	void print(int indent);
	





	// 26. Print in outstream 
	void print(ostream& out);






	// 27. Print  C to String







	// 28. Print C




	
	

	// 29. IR to C
	


	


	// 30. Print to String
	void printToString(char *buf);

	
	
	


	// 31. Short Print
	void shortPrint(ostream& out);
	





	// 32. Print Properties
	void printProperties(ostream& out, PropNameList & pL);





	
	// 33. Some specific operations
	virtual void initOpCode(const OpCode & someOpCode);



	TableIndex getOpCodeIndex();
	



	virtual OpCode & getOpCode();
	



	virtual void getOpcodeName(MyString & s);
	


	virtual void changeOpCode(TableIndex newIndex);
	



//
// Rewrite the operators == and << later.
   
	virtual void changeOpCode(char *newStr);
	
	// for ASM gen
	virtual void printASMInstr(ostream &out);
	

	
	void ifConvert(BaseOpProperty* pred, OpList &IselList, BaseOperation* ifOp, OpList OperationsList);

	int NormalOp::isPresentOperand(int SSANum);

	int ChangeReg(int OldSSANum, int NewSSANum, BaseArgument *reg);

	
	virtual void recursiveRemoveProperty(PropertyNames name);

};

// Comment 0:
//
// This will also be an abstract class. (because we will not be
//  redefining the initializeID method)

#endif

// Comment 1:
//
// _numNormalOps is used to keep a count of the number of NormalOp objects
// in existence at any point. COUNT_NORMALOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
// Since this is an abstract class (see comment 0 above), no NormalOp
// object will be created explicitly. But, whenever an object of any
// class derived from it is created (destroyed), the NormalOp
// constructor (destructor) will be called (which will
// increment/decrement the count).
//
