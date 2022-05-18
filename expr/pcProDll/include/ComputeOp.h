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
// $Id: ComputeOp.h,v 1.22 1998/04/11 00:35:36 xji Exp $
//
// File:  		ComputeOp.h
// Created:		Tue Oct 29, 96
// Last modified: 	July 30th, 2000
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

/* 
Last Change by AVS on 30th July, 2000, to include predication.
We intend to make new sources. i.e. Predicate sources. 
So we generalize the sources to a list. This has to be done for all the operations.
So the addition of the source list has been moved to the NormalOp.h rather than each derived class.
*/

#ifndef __COMPUTEOP_H__
#define __COMPUTEOP_H__

#include "DataOp.h"
#include "BaseArgument.h"
#include "disamb.h"
#include "IburgDefs.h"
#include "PropertyIncludes.h"

class ComputeOp : public DataOp 
{
private:
	
#ifdef COUNT_COMPUTEOPS
	static int _numComputeOps;	// See comment 1 below.
#endif
	
protected:
	
public:

/*
computeOp format

	Compute Operations are: <opcode> <dest1> <source1> <source2>

    _sourceOperandList contains the source1, and source2.
    _destOperandList conatins dest1


    Compute Operations : _destOperandList[_DEST_]		: (corresponding to <dest1>)
						 _sourceOperandList[_SOURCE_1_] : (corresponding to <source1>)
						 _sourceOperandList[_SOURCE_2_] : (corresponding to <source2>)

*/

	
	// 1. Constructor
	ComputeOp();
	
	
	
	
	// 2. copy comstructor
	ComputeOp(ComputeOp & a);	
	
	
	
	// 3. destructor
	virtual ~ComputeOp();
	
	
	
	
	
	// 4. counter
#ifdef COUNT_COMPUTEOPS
	static int numComputeOps()
	{
		return _numComputeOps;
	}
#endif
	
	
	
	
	// 5. copy
	BaseOperation *copy();
	
	
	
	
	
	// 6. copyBody
	BaseOperation *copyBody(bool copyProp=false);
	
	
	
	
	
	// 7. Initialize
	
	
	
	
	
	
	// 8. Identification
	int isComputeOp();

	
	
	
	
	
	// 9. has Operand
	// virtual int hasOperand(int position, int where = SRC);
	// Same as in NormalOp:: has Operand
	
	
	
	
	// 10. has Arg
	// virtual int hasArg(BaseArgument *arg);
	// Same as in NormalOp:: has Arg
	
	
	
	
	
	// 11. Add Operand
	// virtual void addOperand(BaseArgument *operand, int position, int where = SRC);
	// Same as in NormalOp:: Add Operand

	
	
	
	
	// 12. Ptr to operands
	// virtual IfOp *ptrToOperand(int position, int where = _SRC_LIST_);
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
	// virtual void reset();
	// Same as in NormalOp :: reset 
	
	
	
	
	// 18. isEq
	virtual int isEq(BaseOperation *op);
	
	
	
	
	// 19. operator ==
	friend int operator == (const ComputeOp & a, const ComputeOp & b)
	{
		return _YES_;
	}
	
	
	
	
	// 20. operator <<
	friend ostream & operator << (ostream & out, const ComputeOp & s)
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
	virtual void printCToString(MyString & s);







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
	
	
	
	



	// 34. Some specific operations
	virtual int isMoveOp();
	
	virtual expr BuildExpr();//disamb
	
	virtual expr build_expr(char*& s);//disamb
	
	virtual expr build_subexpr(char*& s);//disamb
	
	void renameOperands(MyLinkedList<BaseOperation*> &moves);
	
	virtual NODEPTR_TYPE BuildIburgTree(int rootOfTree,int essential);
	



};

#endif

// Comment 1:
//
// _numComputeOps is used to keep a count of the number of ComputeOp objects
// in existence at any point. COUNT_COMPUTEOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
//
