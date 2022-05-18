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
// $Id: GotoOp.h,v 1.9 1998/03/21 01:18:45 ahalambi Exp $
//
// File:  		GotoOp.h
// Created:		Tue Oct 29, 96
// Last modified: 	Tue Oct 29, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifndef __GOTOOP_H__
#define __GOTOOP_H__

#include "FlowOp.h"
#include "BaseArgument.h"
#include "LabelArg.h"
#include "BaseInstruction.h"
#include "PropertyIncludes.h"

// As of now, GotoOp class inherits only the FlowOp class. Later on,
// it might make sense for it to inherit the ControlOp class also.


class GotoOp : public FlowOp 
{
private:
 
#ifdef COUNT_GOTOOPS
 static int _numGotoOps;	// See comment 1 below.
#endif
 
protected:
	BaseInstruction *_targetInstr;


public:

	
/*
GotoOp format

	 <opcode> <label> 

  Originally, _destLabel contained the <label>

  Now, after making sources and dests as lists, this has been changed to:

  _destOperandList : empty
  _sourceOperandList[_DST_LBL_] : <label> 

*/

	// 1. Constructor
	GotoOp();
	

	
	

	// 2. copy constructor
	GotoOp(GotoOp & a);

	

	
	


	// 3. Destructor
	virtual ~GotoOp();
	
	


	
	
	// 4. counter 
#ifdef COUNT_GOTOOPS
	static int numGotoOps()
	{
		return _numGotoOps;
	}
#endif
	
	
	
	


	// 5. copy
	BaseOperation *copy();
	
	
	

	
	


	// 6. copyBody
	BaseOperation *copyBody(bool copyProp=false);
	
	
	




	
	// 7. Initialize
	//
	// This is the same as NormalOp::Initialize
	
	

	
	
	// 8. Identification
	int isGotoOp();
	




	// 9. has Operand
	// virtual int hasOperand(int position, int where = SRC);
	// This is the same as NormalOp::has Operand
	

	



	
	// 10. has Arg
	// virtual int hasArg(BaseArgument *arg);
	// This is the same as NormalOp:: has Arg
	
	

	

	
	
	// 11. Add Operand
	// virtual void addOperand(BaseArgument *arg, int position, int where = SRC);	
	// This is the same as NormalOp::Add Operand
	
	void addLabelArg(BaseArgument *label);
	
	
	

	

	// 12. Ptr to operands
	// virtual BaseArgument *ptrToOperand(int position, int where = SRC);
	// This is the same as NormalOp:: Ptr to Operand
	
	BaseArgument *ptrToLabel();
	
	char *getTargetLabelStr();
	
	




	// 13. destOperand List
	// virtual void destOperandsList(ArgList & a);
	// This is the same as NormalOp:: Dest Operand List
	
	



	
	// 14. sourceOperand List
	// virtual void sourceOperandsList(ArgList & a);
	// This is the same as NormalOp:: Source Operand List
	
	

	


	// 15. allOperand List
	// virtual void allOperandsList(ArgList & a);
	// This is the same as NormalOp:: All Operand List
	



	
	// 16. clear Operand
	// virtual void clearOperand(int position, int where = SRC); 
	// This is the same as NormalOp:: Clear Operand
	
	void delLabelArg();




	
	// 17. reset
	// virtual void reset();
	// This is the same as NormalOp:: reset
	
	





	// 18. isEq
	virtual int isEq(BaseOperation *op);
	



	
	
	// 19. operator ==
	friend int operator == (const GotoOp & a, const GotoOp & b)
	{
		return _YES_;
	}
	
	
	

	
	// 20. operator <<
	friend ostream & operator << (ostream & out, const GotoOp & s)
	{
		return out;
	}
	
	
	


	// 21. Create List of Operands
	// void createListOfOperands(Symtab *operandSymTable);
	// This is the same as NormalOp:: create list of operands

	




	// 22. PrintIndent
	// virtual void printIndent(int indent);

	
	
	
	
	
	// 23.  Print Indent in a file
	// void printIndent(FilePointer & fp, int indent);
	





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





	// 33. Some specific operations
	void setTargetInstr(BaseInstruction *someInstr);
	
	BaseInstruction *getTargetInstr();
		 
  
};

#endif

// Comment 1:
//
// _numGotoOps is used to keep a count of the number of gotoOp objects
// in existence at any point. COUNT_GOTOOPS can be either defined or
// undefined in the file "OperationIncludes.h". The count information
// will be used for debugging (and/or statistical) purposes. It might
// be a good idea to allow the user to specify whether he needs this
// info. 
