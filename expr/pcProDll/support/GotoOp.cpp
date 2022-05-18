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
// $Id: GotoOp.cc,v 1.1 1998/03/21 01:16:13 ahalambi Exp $
//
// File:  		GotoOp.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#include "stdafx.h"

#include "GotoOp.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_GOTOOPS
int GotoOp::_numGotoOps = 0;	
#endif


// 1. Constructor
GotoOp::GotoOp()
	{	 
#ifdef COUNT_GOTOOPS
		_numGotoOps++;	
#endif
	}




// 2. copy constructor
GotoOp::GotoOp(GotoOp & a)
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
	   
	   // NormalOp constructor must be called here
#ifdef COUNT_GOTOOPS
	   _numGotoOps++;	
#endif
}




// 3. Destructor
GotoOp::~GotoOp()
	{
#ifdef COUNT_GOTOOPS
		_numGotoOps--;
#endif
	}




// 4. counter




// 5. copy
BaseOperation *GotoOp::copy()
{
 BaseOperation *retPtr;
 
 retPtr = new GotoOp(*this);
 
 return retPtr;
}




// 6. copyBody
BaseOperation *GotoOp::copyBody(bool copyProp)
{
	
	BaseOperation *retPtr;
	retPtr = new GotoOp();
	GotoOp *gotoretPtr = (GotoOp*) retPtr;
	
	ArgListIter tIter;
	
	// copy the opCode
	gotoretPtr->_opCode = _opCode;
	
	BaseArgument *nullPtr = NULL;

	// Now copy the destination list
	for(tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if (*tIter != NULL)
			gotoretPtr->_destOperandList.push_back((*tIter)->copy());
		else
			gotoretPtr->_destOperandList.push_back(nullPtr);
	}
	
	// Now copy the source list
	for(tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if (*tIter != NULL)
		gotoretPtr->_sourceOperandList.push_back((*tIter)->copy());
		else
		gotoretPtr->_sourceOperandList.push_back(nullPtr);
	}
	
	// We do not copy the properties
	
	return retPtr;
}






// 7. Initialize


	

	
// 8. Identification
int GotoOp::isGotoOp()
	{
		return _YES_;
	}
	





// 9. has Operand
// virtual int hasOperand(int position, int where = SRC);
// This is the same as NormalOp::has Operand






// 10. has Arg
// virtual int hasArg(BaseArgument *arg);
// This is the same as NormalOp:: has Arg







// 11. Add Operand
// virtual void addOperand(BaseArgument *arg, int position, int where = SRC);	
// This is the same as NormalOp::Add Operand

void GotoOp::addLabelArg(BaseArgument *label)
{
 // Check to see if label is of type LabelArg
 
 if (label != NULL)
	 NormalOp::addSourceOperand(label, _DST_LBL_);
}






// 12. Ptr to operands
// virtual BaseArgument *ptrToOperand(int position, int where = SRC);
// This is the same as NormalOp:: Ptr to Operand

BaseArgument *GotoOp::ptrToLabel()
{
 return _sourceOperandList[_DST_LBL_];
}

char *GotoOp::getTargetLabelStr()
{
 return _sourceOperandList[_DST_LBL_]->lName();
}





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

void GotoOp::delLabelArg()
{
	// Check to see if label is of type LabelArg
	
	if (_sourceOperandList[_DST_LBL_] != NULL) delete _sourceOperandList[_DST_LBL_];
	_sourceOperandList[_DST_LBL_] = NULL;
}





// 17. reset
// virtual void reset();
// This is the same as NormalOp:: reset







// 18. isEq
int GotoOp::isEq(BaseOperation *op)
{
	ArgListIter tIter1, tIter2;

	if (op == NULL) 
		return _NO_;

	if (!op->isGotoOp())
		return _NO_;
	
	if (!NormalOp::isEq(op))
		return _NO_;

	if (_targetInstr != ((GotoOp *)op)->_targetInstr)
		return _NO_;

	if (_sourceOperandList.size() != (((GotoOp *)op)->_sourceOperandList).size())
		return _NO_;

	for (tIter1 = _sourceOperandList.begin(), tIter2 = (((GotoOp *)op)->_sourceOperandList).begin(); tIter1 != _sourceOperandList.end(); tIter1++, tIter2++)
	{
		if (!(*tIter1)->isEq(*tIter2))
			return _NO_;
	}

	return _YES_;
}


 



	// 19. operator ==
	
	
	

	
	// 20. operator <<








// 21. Create List of operands






// 22. PrintIndent
extern void printIndent(int indent);








// 23. PrintIndent in a file
extern void printIndent(FilePointer & fp, int indent);







// 24. Print
void GotoOp::print()
{
	NormalOp::print(); 
	cout << " Tgt: " << _targetInstr;
	cout << endl;
}	 






// 25. Print with indent
void GotoOp::print(int indent)
{ 
	printIndent(indent);
	NormalOp::print(indent);
}	 








// 26. Print in outstream
void GotoOp::print(ostream& out)
{
	NormalOp::print(out); 
	out << " Tgt: " << _targetInstr;
	
	out << endl;
}






// 27. Print  C to String






// 28. Print C
void GotoOp::printC(int indent)
{ 
  // cout << " ";
  // _destLabel->print();
 
 // cout << " " << _targetInstr;
}	 






// 29. IR to C
void GotoOp::irToC(FilePointer & fp, int indent)
{
	printIndent(indent);

	// first, print out the assembly instruction as a comment.
	//
	char buf[80];
	buf[0] = '\0';
	printToString(buf);
	fprintf(fp, "/* (%s) */\n", buf);

	printIndent(fp, indent);

	MyString s;
	s.concat(" goto ");
	printArgToStringAsC(_sourceOperandList[_DST_LBL_], s, _C_INT_, _C_SRC_, 1); // Note: The last three parameters are not relevant.
	s.concat(";");

	fprintf(fp, "%s", s.getStr());
}










// 30. Print to String
void GotoOp::printToString(char* buf)
{
 MyString str;
 getOpcodeName(str);
 strcat(buf,str.getStr());
}






// 31. Short Print
void GotoOp::shortPrint(ostream& out)
{
	NormalOp::shortPrint(out);
	out << _targetInstr;
}






// 32. Print Properties
void GotoOp::printProperties(ostream& out, PropNameList & pL)
{
	NormalOp::printProperties(out, pL);
	
	BaseOperation::printProperties(out, pL);
}






// 33. Some specific Operations
void GotoOp::setTargetInstr(BaseInstruction *someInstr)
{
	_targetInstr = someInstr;
}
	

BaseInstruction *GotoOp::getTargetInstr()
{
	return _targetInstr;
}
