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
// $Id: IfOp.h,v 1.13 1998/03/21 01:18:54 ahalambi Exp $
//
// File:  		IfOp.h
// Created:		Tue Oct 29, 96
// Last modified: 	Tue Oct 29, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __IFOP_H__
#define __IFOP_H__

#include "ControlOp.h"
#include "BaseArgument.h"
#include "RegisterArg.h"
#include "LabelArg.h"
#include "PropertyIncludes.h"

// As of now, IfOp class inherits only the ControlOp class. Later on,
// it might make sense for it to inherit the FlowOp class also.

class IfOp : public ControlOp 
{
private:
	
#ifdef COUNT_IFOPS
	static int _numIfOps;	// See comment 1 below.
#endif
	
protected:
	
public:


/* 
IfOp format

	If Operations are: <opcode> <cond-reg> <label>

    _sourceOperandList containes the cond-reg, and _target_label.
    There is no _destOperandList

    If Operations : _destOperandList : empty
					_sourceOperandList[_COND_REG_] : (corresponding to _cond_reg)
					_sourceOperandList[_SRC_LABEL_] :   (corresponding to _target_label)

*/	
	// 1. Constructor
	IfOp();
	



	// 2. copy constructor
	IfOp(IfOp & a);

		




	// 3. Destructor
	virtual ~IfOp();
	



	// 4. counter
#ifdef COUNT_IFOPS
	static int numIfOps()
	{
		return _numIfOps;
	}
#endif
	



	// 5. copy
	BaseOperation *copy();
	




	// 6. copyBody
	BaseOperation *copyBody(bool copyProp=false);
	

	
	
	
	// 7. Initialize


	
	
	// 8. Identification
	int isIfOp();

	


	// 9. has Operand
	// virtual int hasOperand(int position, int where = SRC);
	// Same as in NormalOp :: has Operand
	
	
	


	// 10. has Arg
	// virtual int hasArg(BaseArgument *arg);
	// Same as in NormalOp :: has Arg



	
	
	// 11. Add Operand
	// virtual void addOperand(BaseArgument *operand, int position, int where = SRC);
	// Same as in NormalOp :: add Operand

	void addCCReg(BaseArgument *operand);
	
	void addLabelArg(BaseArgument *label);
	
	
	
	
	// 12. Ptr to operands
	// virtual IfOp *ptrToOperand(int position, int where = SRC);
	// Same as in NormalOp :: Ptr to Operand

	
	BaseArgument *ptrToCCReg();
	
	BaseArgument *ptrToLabel();
	



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
	friend int operator == (const IfOp & a, const IfOp & b)
{
	return _YES_;
}
	



	// 20. operator <<
	friend ostream & operator << (ostream & out, const IfOp & s)
{
	out << "OpCode: ";
	out << s._opCode;
		
	return out;
}
	
	


	// 21. create list of operands
	// void createListOfOperands(Symtab *operandSymTable);
	// same as in NormalOp :: create lis of Operands






	// 22. PrintIndent





	// 23. PrintIndent in a file




	

	// 24. Print
	void print();
	






	// 25. Print with indent
	void print(int indent);







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



	



	// 33. Some specific operations
	char *getTargetLabelStr();

	BaseArgument* getCC();

};

#endif

// Comment 1:
//
// _numIfOps is used to keep a count of the number of IfOp objects
// in existence at any point. COUNT_IFOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
