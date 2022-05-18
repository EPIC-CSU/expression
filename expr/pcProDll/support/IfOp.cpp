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
// $Id: IfOp.cc,v 1.9 1998/03/21 01:16:28 ahalambi Exp $
//
// File:  		IfOp.h
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "stdafx.h"

#include "IfOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_IFOPS
int IfOp::_numIfOps = 0;	
#endif

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
IfOp::IfOp()
	{		
		// this should call the NormalOp constructor.
#ifdef COUNT_IFOPS
		_numIfOps++;	
#endif
	}



// 2. copy constructor
IfOp::IfOp(IfOp & a)
{
	ArgListIter tIter;
	BaseArgument *arg;
	   
	   // First copy the unique ID
	   _uniqueID = a._uniqueID;
	   
	   // copy the opCode
	   _opCode = a._opCode;
	   
	   // Now copy the destination list
	   for (tIter = a._destOperandList.begin(); tIter != a._destOperandList.end(); tIter++)
	   {
		   if ((arg = *tIter) != NULL)
			   _destOperandList.push_back(arg->copy());
		   else
			   _destOperandList.push_back(NULL);
	   }
	   // Now copy the source list
	   for (tIter = a._sourceOperandList.begin(); tIter != a._sourceOperandList.end(); tIter++)
	   {
		   if ((arg = *tIter) != NULL)
			   _sourceOperandList.push_back(arg->copy());
		   else
			   _sourceOperandList.push_back(NULL);
	   }

#ifdef COUNT_IFOPS
		_numIfOps++;	
#endif
	}



// 3. Destructor
IfOp::~IfOp()
{
#ifdef COUNT_IFOPS
		_numIfOps--;
#endif
}



// 4. counter



// 5. copy
BaseOperation *IfOp::copy()
{
	BaseOperation *retPtr;
	
	retPtr = new IfOp(*this);
	
	return retPtr;
}


// 6. copyBody
BaseOperation *IfOp::copyBody(bool copyProp)
{
	BaseOperation *retPtr;
	retPtr = new IfOp();
	IfOp *ifretPtr = (IfOp*) retPtr;
	
	ArgListIter tIter;
	
	// copy the opCode
	ifretPtr->_opCode = _opCode;

	BaseArgument *nullPtr = NULL;

	// Now copy the destination list
	for(tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if (*tIter != NULL)
			ifretPtr->_destOperandList.push_back((*tIter)->copy());
		else
			ifretPtr->_destOperandList.push_back(nullPtr);
	}
	
	// Now copy the source list
	for(tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if (*tIter != NULL)
			ifretPtr->_sourceOperandList.push_back((*tIter)->copy());
		else
			ifretPtr->_sourceOperandList.push_back(nullPtr);
	}
	
	// We do not copy the properties.
	
	return retPtr;
}




// 7. Initialize




// 8. Identification
int IfOp::isIfOp()
{
	return _YES_;
}




// 9. has Operand





// 10. has Arg





// 11. Add Operand

void IfOp::addCCReg(BaseArgument *operand)
{
	// We define this just as a call to addOperand
	addOperand(operand, _COND_REG_);
}

void IfOp::addLabelArg(BaseArgument *label)
{
	// We define this just as a call to addOperand
	addOperand(label, _SRC_LABEL_);
}




// 12. Ptr to Operands

BaseArgument *IfOp::ptrToCCReg()
{
	return ptrToOperand(_COND_REG_);
}


BaseArgument *IfOp::ptrToLabel()
{
	return ptrToOperand(_SRC_LABEL_);
}

char* IfOp::getTargetLabelStr()
{
	BaseArgument *tempPtr;
	
	if ((tempPtr = ptrToOperand(_SRC_LABEL_)) != NULL)
		return tempPtr->lName();
	
	cout << "Panic: No source is defined" << endl;
	assert(0);
	return NULL;
}

BaseArgument* IfOp::getCC()
{ 
	return ptrToOperand(_COND_REG_); 
}


// 13. destOperand List




// 14. sourceOperand List




// 15. allOperand List




// 16. clear Operand




// 17. reset





// 18. isEq 
int IfOp::isEq(BaseOperation *op)
{
	ArgListIter tIter1, tIter2;
	
	
	if (op == NULL) 
		return _NO_;
	
	if (!op->isIfOp())
		return _NO_;
	
	if (!NormalOp::isEq(op))
		return _NO_;
	
	if (_sourceOperandList.size() != (((IfOp *)op)->_sourceOperandList).size())
		return _NO_;
	
	for (tIter1 = _sourceOperandList.begin(), tIter2 = (((IfOp *)op)->_sourceOperandList).begin(); tIter1 != _sourceOperandList.end(); tIter1++, tIter2++)
	{
		if (!(*tIter1)->isEq(*tIter2))
			return _NO_;
	}
	
	if (_destOperandList.size() != (((IfOp *)op)->_destOperandList).size())
		return _NO_;
	
	for (tIter1 = _destOperandList.begin(), tIter2 = (((IfOp *)op)->_destOperandList).begin(); tIter1 != _destOperandList.end(); tIter1++, tIter2++)
	{
		if (!(*tIter1)->isEq(*tIter2))
			return _NO_;
	}
	
	return _YES_;
}




// 19. operator ==
	



// 20. operator <<







// 21. create list of operands







// 22. PrintIndent
extern void printIndent(int indent);







// 23.  PrintIndent in a file
extern void printIndent(FilePointer & fp, int indent);







// 24.  Print 
void IfOp::print()
{
	NormalOp::print();	
	printOpProperty(cout);
	cout << endl;
}







// 25. Print with indent
void IfOp::print(int indent)
{
	printIndent(indent);
	NormalOp::print(indent);
}








// 26. Print in outstream 
void IfOp::print(ostream& out)
{
	NormalOp::print(out); 
	
	out << endl;
}






// 27. Print  C to String






// 28. Print C
void IfOp::printC(int indent)
{
	printIndent(indent);
	_opCode.printC();
	
	cout << "\t(";
	ptrToOperand(_COND_REG_)->printC(cout);
	
	cout << ")  goto ";
	ptrToOperand(_SRC_LABEL_)->printC(cout);
}




// 29. IR to C
void IfOp::irToC(FilePointer & fp, int indent)
{
	printIndent(fp, indent);
	
	// first, print out the assembly instruction as a comment.
	//
	char buf[80];
	buf[0] = '\0';
	printToString(buf);
	fprintf(fp, "/* (%s) */\n", buf);
	
	printIndent(fp, indent);


	MyString s;
	printArgToStringAsC(_sourceOperandList[_COND_REG_], s, _C_INT_, _C_SRC_, 1);
	s.concat(" if (getIntegerValue(rtemp1)) goto ");
	printArgToStringAsC(_sourceOperandList[_SRC_LABEL_], s, _C_INT_, _C_SRC_, 1); // Note: if _destLabel is a label arg, then the last 3 parameters are not necessary.
	s.concat("; ");

	fprintf(fp, "%s", s.getStr());
}






// 30. Print to String
void IfOp::printToString(char *buf)
{
	NormalOp::printToString(buf);
}






// 31. Short Print
void IfOp::shortPrint(ostream& out)
{
	NormalOp::shortPrint(out);
}




// 32. printProperties
void IfOp::printProperties(ostream& out, PropNameList & pL)
{
	NormalOp::printProperties(out, pL);
	BaseOperation::printProperties(out, pL);
}






// 33. Some specific operations
