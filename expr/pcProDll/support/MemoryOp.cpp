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
// $Id: MemoryOp.cc,v 1.9 1998/03/21 01:16:37 ahalambi Exp $
//
// File:  		MemoryOp.h
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#include "stdafx.h"

#include "MemoryOp.h"
#include "RegisterArg.h"
#include "symtabs.h"
#include "LabelArg.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif

#ifdef COUNT_MEMORYOPS
int MemoryOp::_numMemoryOps = 0;	
#endif

//END MFC DEBUG NEW: 

/*
MemoryOp format

	Load Operations are: <opcode> <dest> <base-address-reg> <offset>
	Store Operations are: <opcode> <base-address-reg> <store-source> <offset>

  Originally, _sourceOperand contained the base-address-reg, and _offset contained the immediate offset.
  Also, _destOperand contained the dest for load, and store-source for store.

  Now, after making sources and dests as lists, this has been changed to:

  Load Operations : _destOperandList[_DST_] : <dest> (corresponding to _destOperand)
					_sourceOperandList[_SRC_1_] : <base-address-reg> (corresponding to _sourceOperand)
					_sourceOperandList[_OFFSET_] : <offset> (corresponding to _offset)

  Store Operations :
					_sourceOperandList[_SRC_1_] : <base-address-reg> (corresponding to _sourceOperand)
					_sourceOperandList[_OFFSET_] : <offset> (corresponding to _offset)
					_sourceOperandList[_SRC_3_] : <store-source> (corresponding to _destOperand).


  NOTE: _OFFSET_ = 1
*/

// 1. Constructor
MemoryOp::MemoryOp()
{
#ifdef COUNT_MEMORYOPS
	_numMemoryOps++;	
#endif
}




// 2. Copy constructor
MemoryOp::MemoryOp(MemoryOp & a)
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
	   
	   
	   //!!!!!!!!!!!!ATTENTION, only these properties are copied!!!!!!!!!!!!!!!!!!!
	   //make copy of the following properties
	   MyLinkedList<PropertyNames> propList;
	   PropertyNames temp = _INSTR_TYPE_;
	   propList.append(temp);
	   
	   //_opProperties.makeACopyOf(a.propertyList(), propList);
	   _opProperties.makeACopyOf(a._opProperties, propList);
	   
#ifdef COUNT_MEMORYOPS
	   _numMemoryOps++;	
#endif
}




// 3. Destructor
MemoryOp::~MemoryOp()
{
#ifdef COUNT_MEMORYOPS
	_numMemoryOps--;
#endif
}





// 4. counter





// 5. copy
BaseOperation *MemoryOp::copy()
{
 BaseOperation *retPtr;
 
 retPtr = new MemoryOp(*this);
 
 return retPtr;
}


 

// 6. copyBody
BaseOperation *MemoryOp::copyBody(bool copyProp)
{
	BaseOperation *retPtr;
	retPtr = new MemoryOp();
	MemoryOp *memretPtr = (MemoryOp*) retPtr;
	
	ArgListIter tIter;
	
	// copy the opCode
	memretPtr->_opCode = _opCode;
	
	BaseArgument *nullPtr = NULL;

	// Now copy the destination list
	for(tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if (*tIter != NULL)
			memretPtr->_destOperandList.push_back((*tIter)->copy());
		else
			memretPtr->_destOperandList.push_back(nullPtr);
	}

	
	// Now copy the source list
	for(tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if (*tIter != NULL)
			memretPtr->_sourceOperandList.push_back((*tIter)->copy());
		else
			memretPtr->_sourceOperandList.push_back(nullPtr);
	}
	
	// We do not copy the properties
	
	return retPtr;
}



// 7. Initialize





// 8. Identify
int MemoryOp::isMemoryOp()
{
	return _YES_;
}

   
   
   
// 9. has Operand




// 10. has Arg





// 11. Add Operand

void MemoryOp::addSourceOperand(BaseArgument *operand)
{
	if (isLoad())
		addOperand(operand, _SRC_1_);
	else // if (isStore())
		addOperand(operand, _SRC_3_);
}


void MemoryOp::addDestOperand(BaseArgument *operand)
{
	if (isLoad())
		addOperand(operand, _DEST_, _DEST_LIST_);
	else // if (isStore())
		addOperand(operand, _SRC_1_);
}

void MemoryOp::setOffset(BaseArgument *offset)
{
	addOperand(offset, _OFFSET_);
}


   
   
   
   
// 12. ptr to operand
BaseArgument *MemoryOp::ptrToSourceOperand()
{
	if (isLoad())
		return ptrToOperand(_SRC_1_);
	else // if (isStore())
		return ptrToOperand(_SRC_3_);
}
	
BaseArgument *MemoryOp::ptrToDestOperand()
{
	if (isLoad())
		return ptrToOperand(_DEST_, _DEST_LIST_);
	else // if (isStore())
		return ptrToOperand(_SRC_1_);
}
	
BaseArgument *MemoryOp::getOffset() 
{ 
	return ptrToOperand(_OFFSET_); 
}

   
   
   
// 13. destOperand List

   
   
   
// 14. sourceOperand List

   
   
   
   
// 15. allOperand List
   
   
 
   
// 16. clear Operand




// 17. reset




// 18. isEq
int MemoryOp::isEq(BaseOperation *op)
{
	ArgListIter tIter1, tIter2;

	if (op == NULL) 
		return _NO_;

	if (!op->isMemoryOp())
		return _NO_;
	
	if (!NormalOp::isEq(op))
		return _NO_;

	if (_sourceOperandList.size() != (((MemoryOp *)op)->_sourceOperandList).size())
		return _NO_;

	for (tIter1 = _sourceOperandList.begin(), tIter2 = (((MemoryOp *)op)->_sourceOperandList).begin(); tIter1 != _sourceOperandList.end(); tIter1++, tIter2++)
	{
		if (!(*tIter1)->isEq(*tIter2))
			return _NO_;
	}
			
	if (_destOperandList.size() != (((MemoryOp *)op)->_destOperandList).size())
		return _NO_;

	for (tIter1 = _destOperandList.begin(), tIter2 = (((MemoryOp *)op)->_destOperandList).begin(); tIter1 != _destOperandList.end(); tIter1++, tIter2++)
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
void MemoryOp::print()
{
	cout << _uniqueID << ": " << _opCode;
	
	cout << " Dest: ";
	if (isLoad())
		_destOperandList[_DST_]->print();
	else
		_sourceOperandList[_SRC_3_]->print();
	
	cout << " Source: ";
	_sourceOperandList[_SRC_1_]->print();
	
	cout << " Offset: ";
	_sourceOperandList[_OFFSET_]->print();
	
	printOpProperty(cout);
}




// 25. Print with indent
void MemoryOp::print(int indent)
{
	// Tue, july 07 98 : Ashok. I have commented out the next line
	// because it does not seem to have any definition.
	
	printIndent(indent);
	NormalOp::print(indent);
}







// 26. Print in outstream
void MemoryOp::print(ostream& out)
{
 out << _uniqueID << ": " << " OpCode: " << _opCode;
 
 out << " Dest: ";
	if (isLoad())
		_destOperandList[_DST_]->print(out);
	else
		_sourceOperandList[_SRC_3_]->print(out);
 

 out << " Source: ";
 _sourceOperandList[_SRC_1_]->print(out);
 
 out << " Offset: ";
	if (_sourceOperandList[_OFFSET_] != NULL)
		_sourceOperandList[_OFFSET_]->print(out);
 
 printOpProperty(out);
 
}


// #define IRTOC_PRINT_MEMORYTRACE


// 28. Print C
void MemoryOp::printC(int indent)
{
  printIndent(indent);



	MyString s;
	printCToString(s);
	s.print(cout);
}

void MemoryOp::printCToString(MyString & s)
{
	int numImmsInOp = 0;
  int opCodeGlobalIndex;	// index in the globalop code table
  EnumGenericOpCode genericOpCodeEnum;


  opCodeGlobalIndex = _opCode.index();
  genericOpCodeEnum = 
    globalOpCodeTable[opCodeGlobalIndex].getGenericOpCodeEnum();
  
  // First, calculate the address.
  //
  switch (genericOpCodeEnum) 
  {
  case _IVLOAD_:
  case _DVLOAD_:
  case _FVLOAD_:
  case _HIVLOAD_:
  case _HIVLOADU_:
  case _QIVLOAD_:
  case _QIVLOADU_:
	  printArgToStringAsC(_sourceOperandList[_SRC_1_], s, _C_INT_, _C_SRC_, 1);
	  printArgToStringAsC(_sourceOperandList[_OFFSET_], s, _C_INT_, _C_SRC_, 2);

	  s.concat(" temp1 = getIntegerValue(rtemp1 + rtemp2); ");

#ifdef IRTOC_PRINT_MEMORYTRACE
  s.concat("fprintf(memtraceout, \"(R) %d\\n \", temp1); ");
#endif
	  break;
  case _IVSTORE_:
  case _DVSTORE_:
  case _FVSTORE_:
  case _HIVSTORE_:
  case _HIVSTOREU_:
  case _QIVSTORE_:
  case _QIVSTOREU_:
	  printArgToStringAsC(_sourceOperandList[_SRC_1_], s, _C_INT_, _C_SRC_, 1);
	  printArgToStringAsC(_sourceOperandList[_OFFSET_], s, _C_INT_, _C_SRC_, 2);

	  s.concat(" temp1 = getIntegerValue(rtemp1 + rtemp2); ");

#ifdef IRTOC_PRINT_MEMORYTRACE
  s.concat("fprintf(memtraceout, \"(W) %d\\n \", temp1); ");
#endif
	  break;
  default:
	  break;
  } // end of  switch (genericOpCodeEnum)

  // Now, call the appropriate memory access function.
  //
  switch (genericOpCodeEnum)
  {
  case _IVLOAD_:
	  s.concat(" DoIntegerLoad(rtemp3, temp1); ");
	  printArgToStringAsC(_destOperandList[_DST_], s, _C_INT_, _C_DST_, 3);
	  break;
  case _DVLOAD_:
	  s.concat(" DoDoubleLoad(rtemp3, temp1); ");
	  printArgToStringAsC(_destOperandList[_DST_], s, _C_DOUBLE_, _C_DST_, 3);
	  break;
  case _FVLOAD_:
	  s.concat(" DoFloatLoad(rtemp3, temp1); ");
	  printArgToStringAsC(_destOperandList[_DST_], s, _C_FLOAT_, _C_DST_, 3);
	  break;
  case _QIVLOAD_:
	  s.concat(" DoCharLoad(rtemp3, temp1); ");
	  printArgToStringAsC(_destOperandList[_DST_], s, _C_INT_, _C_DST_, 3);
	  break;
  case _QIVLOADU_:
	  s.concat(" DoUCharLoad(rtemp3, temp1); ");
	  printArgToStringAsC(_destOperandList[_DST_], s, _C_INT_, _C_DST_, 3);
	  break;
  case _HIVLOAD_:
	  s.concat(" Do2CharAsShortLoad(rtemp3, temp1); ");
	  printArgToStringAsC(_destOperandList[_DST_], s, _C_INT_, _C_DST_, 3);
	  break;
  case _HIVLOADU_:
	  s.concat(" Do2UCharAsUShortLoad(rtemp3, temp1); ");
	  printArgToStringAsC(_destOperandList[_DST_], s, _C_INT_, _C_DST_, 3);
	  break;
	  
  case _IVSTORE_:
	  printArgToStringAsC(_sourceOperandList[_SRC_3_], s, _C_INT_, _C_SRC_, 3);
 	  s.concat(" DoIntegerStore(rtemp3, temp1); ");
	  break;
  case _DVSTORE_:
	  printArgToStringAsC(_sourceOperandList[_SRC_3_], s, _C_DOUBLE_, _C_SRC_, 3);
 	  s.concat(" DoDoubleStore(rtemp3, temp1); ");
	  break;
  case _FVSTORE_:
	  printArgToStringAsC(_sourceOperandList[_SRC_3_], s, _C_FLOAT_, _C_SRC_, 3);
 	  s.concat(" DoFloatStore(rtemp3, temp1); ");
	  break;
  case _QIVSTORE_:
	  printArgToStringAsC(_sourceOperandList[_SRC_3_], s, _C_INT_, _C_SRC_, 3);
 	  s.concat(" DoCharStore(rtemp3, temp1); ");
	  break;
  case _QIVSTOREU_:
	  printArgToStringAsC(_sourceOperandList[_SRC_3_], s, _C_INT_, _C_SRC_, 3);
 	  s.concat(" DoUCharStore(rtemp3, temp1); ");
	  break;
  case _HIVSTORE_:
	  printArgToStringAsC(_sourceOperandList[_SRC_3_], s, _C_INT_, _C_SRC_, 3);
 	  s.concat(" DoShortAs2CharStore(rtemp3, temp1); ");
	  break;
  case _HIVSTOREU_:
	  printArgToStringAsC(_sourceOperandList[_SRC_3_], s, _C_INT_, _C_SRC_, 3);
 	  s.concat(" DoUShortAs2UCharStore(rtemp3, temp1); ");
	  break;

  default:
	  // do nothing
	  break;
  }
}

void MemoryOp::irToC(FilePointer & fp, int indent)
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
	printCToString(s);

	fprintf(fp, "%s", s.getStr());
}





// 30. Print to String
void MemoryOp::printToString(char *buf)
{
	strcat(buf,globalOpCodeTable[_opCode.index()].opName());
	
	strcat(buf," ");
	if (isLoad())
		_destOperandList[_DST_]->printToString(buf);
	else
		_sourceOperandList[_SRC_3_]->printToString(buf);
	
	strcat(buf," ");
	_sourceOperandList[_SRC_1_]->printToString(buf);
	
	strcat(buf," ");
	if (_sourceOperandList[_OFFSET_] != NULL)
		_sourceOperandList[_OFFSET_]->printToString(buf);
}



/*
// 31. Short Print
void MemoryOp::shortPrint(ostream& out)
{
	out << _opCode<<" ";
	if (isLoad())
	{
		if (_destOperandList[_DST_] != NULL)
		{
			if (_destOperandList[_DST_]->isRegisterArg())
				((RegisterArg *)_destOperandList[_DST_])->print(0); // i.e. print the register number (not string).
			else
				_destOperandList[_DST_]->print(out);
		}
	}
	else // isStore()
	{
		if (_sourceOperandList[_SRC_3_] != NULL)
		{
			if (_sourceOperandList[_SRC_3_]->isRegisterArg())
				((RegisterArg *)_sourceOperandList[_SRC_3_])->print(0); // i.e. print the register number (not string).
			else
				_sourceOperandList[_SRC_3_]->print(out);
		}
	}
	
	out<<" , ";	
	if (_sourceOperandList[_SRC_1_] != NULL)
	{
		if (_sourceOperandList[_SRC_1_]->isRegisterArg())
			((RegisterArg *)_sourceOperandList[_SRC_1_])->print(0); // i.e. print the register number (not string).
		else
			_sourceOperandList[_SRC_1_]->print(out);
	}
	
	out<<" , ";
	if (_sourceOperandList[_OFFSET_] != NULL)
		_sourceOperandList[_OFFSET_]->print(out);
}
*/




// 32. Print Properties
void MemoryOp::printProperties(ostream& out, PropNameList & pL)
{
	out << _opCode<<" ";
	if (isLoad())
	{
		if (_destOperandList[_DST_] != NULL)
		{
			if (_destOperandList[_DST_]->isRegisterArg())
				((RegisterArg *)_destOperandList[_DST_])->printProperties(out, pL); // i.e. print the register number (not string).
			else
				_destOperandList[_DST_]->printProperties(out, pL);
			out<<" , ";
		}
	}
	else // isStore()
	{
		if (_sourceOperandList[_SRC_3_] != NULL)
		{
			if (_sourceOperandList[_SRC_3_]->isRegisterArg())
				((RegisterArg *)_sourceOperandList[_SRC_3_])->printProperties(out, pL); // i.e. print the register number (not string).
			else
				_sourceOperandList[_SRC_3_]->printProperties(out, pL);
			out<<" , ";
		}
	}
	
	if (_sourceOperandList[_SRC_1_] != NULL)
	{
		if (_sourceOperandList[_SRC_1_]->isRegisterArg())
			((RegisterArg *)_sourceOperandList[_SRC_1_])->printProperties(out, pL); // i.e. print the register number (not string).
		else
			_sourceOperandList[_SRC_1_]->printProperties(out, pL);
		out<<" , ";
	}

	if (_sourceOperandList[_OFFSET_] != NULL)
		_sourceOperandList[_OFFSET_]->printProperties(out, pL);
		
	BaseOperation::printProperties(out, pL);
}


// 33. Some specific functions

int MemoryOp::isLoad()
{
 if (_opCode.typeOf() == LOAD_OP)
  return _YES_;
 return _NO_;
}

int MemoryOp::isStore()
{
	if (_opCode.typeOf() == STORE_OP)
  return _YES_;
 return _NO_;
}

