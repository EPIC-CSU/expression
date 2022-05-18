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
// $Id: NormalOp.cc,v 1.3 1998/01/22 23:46:25 pgrun Exp $
//
// File:  		NormalOp.h
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#include "stdafx.h"

#include "NormalOp.h"
#include "RegisterArg.h"
#include "symtabs.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_NORMALOPS
int NormalOp::_numNormalOps = 0;	
#endif

extern bool PrintingASM;
extern bool DumpingIR;
/*
NormalOp format

  destOperandList sourceOperandList
  
	this is just a base class, and is no specific operation.
	
*/


// 1. Constructor
NormalOp::NormalOp()
{
#ifdef COUNT_NORMALOPS
	   _numNormalOps++;	
#endif
}





// 2. copy constructor
/* We want NormalOp to be a pure virtual class,
   so we do not define a copy comstructor for this class
*/   





// 3. Destructor
NormalOp::~NormalOp()
{
	ArgListIter i;
	for (i = _sourceOperandList.begin(); i != _sourceOperandList.end(); i++)
	{
		if ((*i) != NULL)
			delete (*i);
	}

	for (i = _destOperandList.begin(); i != _destOperandList.end(); i++)
	{
		if ((*i) != NULL)
			delete (*i);
	}
#ifdef COUNT_NORMALOPS
	_numNormalOps--;
#endif
} 





// 4. counter





// 5. copy





// 6. copyBody





// 7. Initialize
void NormalOp::initializeID()
{
	_uniqueID = 0;
}





// 8. Identification
int NormalOp::isNormalOp()
{
	return _YES_;
}





// 9. has Operand
int NormalOp::hasSourceOperand(int position)
{
	if (position >= _sourceOperandList.size())
	{
		//cout << "Warning: Querry for an undefined operand" << endl;
		return _NO_;
	}
	else
	{
		if (_sourceOperandList[position] == NULL)
			return _NO_;
		return _YES_;
	}
}


int NormalOp::hasDestOperand(int position)
{
	if (position >= _destOperandList.size())
	{
		//cout << "Warning: Querry for an undefined operand" << endl;
		return _NO_;
	}
	else // position lies in the list
	{
		if (_destOperandList[position] == NULL)
			return _NO_;
		return _YES_;
	}
}

int NormalOp::hasOperand(int position, int where)
{
	if (where == _DEST_LIST_)
		return NormalOp::hasDestOperand(position);
	else
		return NormalOp::hasSourceOperand(position);
}



// 10. has Arg
int NormalOp::hasArg(BaseArgument *arg)
{
	ArgListIter tIter;
	
	if (arg == NULL) 
		return _NO_;
	
	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
		if ( ((*tIter) != NULL) && ((*tIter)->isEq(arg)) ) 
			return _YES_;
		
	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
		if ( ((*tIter) != NULL) && ((*tIter)->isEq(arg)) ) 
			return _YES_;
			
	return _NO_;		
}

// 11. Add operand
void NormalOp::addSourceOperand(BaseArgument *operand, int position)
{
/*
There could be a problem in this -
Suppose we want to add at say pos = 5.
And suppose the list had only 2 elements.
when we pushback a new element, it adds at 3rd position

  So let's decide to add at position = .begin() + pos
*/

	BaseArgument *targ = NULL;
	
	if (_sourceOperandList.begin() + position < _sourceOperandList.end())
	{
		// we'll have to erase the previously written operand, 
		// and then insert new operand in its place.
		//
		// Note: Since an operation possesses all its operands, we delete the previously existing operand.
		//
		if (_sourceOperandList[position] != NULL)
			delete _sourceOperandList[position];

		_sourceOperandList[position] = operand->copy();
	}
	else
	{
		// make the previous locations NULL
		while (_sourceOperandList.begin() + position > _sourceOperandList.end())
			_sourceOperandList.push_back(targ);
		
		_sourceOperandList.push_back(operand->copy());
	}
}

void NormalOp::addDestOperand(BaseArgument *operand, int position)
{
	BaseArgument *targ = NULL;
	
	if (_destOperandList.size() <= position)
	{
		while (_destOperandList.size() < position)
			// make the previous locations NULL
			_destOperandList.push_back(targ);

		_destOperandList.push_back(operand->copy());	
	}
	
	else
	{
		//cout << "Warning : the operand is being overwritten" << endl;
		if (_destOperandList[position] != NULL)
			delete _destOperandList[position];
		
		_destOperandList[position] = operand->copy();	
	}
	
}

void NormalOp::addOperand(BaseArgument *operand, int position, int where)
{
	// Added by Partha because operand can be NULL for move operation for eg.
	// on 09/01/2000
	if(operand == NULL) return;  
	if (where == _DEST_LIST_)
		NormalOp::addDestOperand(operand, position);
	else
		NormalOp::addSourceOperand(operand, position);
}



// 12. ptr to operand
BaseArgument *NormalOp::ptrToSourceOperand(int position)
{
	if (_sourceOperandList.begin() + position >= _sourceOperandList.end())
	{
		//cout << "Warning: Ptr to an undefined operand requested" << endl;
		// assert(0);
		return NULL;
	}
	else
	{
		ArgListIter sIter = _sourceOperandList.begin() + position;
		return (*sIter);
	}
}


BaseArgument *NormalOp::ptrToDestOperand(int position)
{
	if (position >= _destOperandList.size())
	{
		//cout << "Warning: Ptr to an undefined operand requested" << endl;
		// assert(0);
		return NULL;
	}
	else
		return _destOperandList[position];
}

BaseArgument *NormalOp::ptrToOperand(int position, int where)
{
	if (where == _DEST_LIST_)
		return NormalOp::ptrToDestOperand(position);
	else
		return NormalOp::ptrToSourceOperand(position);
}


// 13. destOperand List
void NormalOp::destOperandsList(ArgList & a)
{
	ArgListIter tIter;

	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if (*tIter)
			a.push_back(*tIter);
	}
}
   
// 14. sourceOperand List
void NormalOp::sourceOperandsList(ArgList & a)
{
	ArgListIter tIter;

	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if (*tIter)
			a.push_back(*tIter);
	}
}

// 15. allOperand List
void NormalOp::allOperandsList(ArgList & a)
{
	ArgListIter tIter;
	
	// first push the destinations
	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if ((*tIter))
			a.push_back(*tIter);
	}
	
	// then push the sources
	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if ((*tIter))
			a.push_back(*tIter);
	}
}

ArgList & NormalOp::destOperandList()
{
	return _destOperandList;
}

ArgList & NormalOp::sourceOperandList()
{
	return _sourceOperandList;
}

ArgList & NormalOp::operandList(int dstOrSrc)
{
	return ((dstOrSrc == _DEST_LIST_) ? _destOperandList : _sourceOperandList);
}


// 16. clear Operand
void NormalOp::clearSourceOperand(int position)
{
	delete _sourceOperandList[position];
	_sourceOperandList[position] = NULL;	
}


void NormalOp::clearDestOperand(int position)
{
	delete _destOperandList[position];
	_destOperandList[position] = NULL;	
}

void NormalOp::clearOperand(int position, int where)
{
	if (where == _DEST_LIST_)
		NormalOp::clearDestOperand(position);
	else
		NormalOp::clearSourceOperand(position);
}



// 17. reset
void NormalOp::reset()
{
	ArgListIter i;

	for (i = _sourceOperandList.begin(); i != _sourceOperandList.end(); i++)
	{
		if ((*i) != NULL)
			delete (*i);

		(*i) = NULL;
	}

	for (i = _destOperandList.begin(); i != _destOperandList.end(); i++)
	{
		if ((*i) != NULL)
			delete (*i);

		(*i) = NULL;
	}
}






// 18. isEq 
// isEq does not make sense for a pure virtual function.






// 19. operator ==






// 20. operator <<






// 21. create List of Operands
void NormalOp :: createListOfOperands(Symtab *operandSymTable)
{
	ArgListIter tIter;
	char *newSymbolName = NULL;
	
	if (!_destOperandList.empty())
	{
		for(tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
		{
			if ((*tIter) != NULL)
			{
				if ((*tIter)->isRegisterArg())
				{
			 		newSymbolName = ((RegisterArg *)(*tIter))->createSymbol();
					operandSymTable->addSymbol(newSymbolName);
				}
			}
		}
	}
	
	if (!_sourceOperandList.empty())
	{
		for(tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
		{
			if ((*tIter) != NULL)
			{
				if ((*tIter)->isRegisterArg())
				{
					newSymbolName = ((RegisterArg *)(*tIter))->createSymbol();
					operandSymTable->addSymbol(newSymbolName);
				}
			}
		}
	}
	
}




// 22. PrintIndent





// 23. PrintIndent in a file





// 24. Print
void NormalOp::print()
{
	cout << "OpCode: " << _opCode << "  ID:" << _uniqueID /* << "=========================================="  << endl*/;
	
	ArgListIter tIter;

	cout << "Dest:";
	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			cout << " ";
			(*tIter)->print();
		}
	}

	cout << "Source:";
	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			cout << " ";
			(*tIter)->print();
		}
	}
	
// I do not print the properties here
}






// 25.  Print with indent
void NormalOp::print(int indent)
{
	cout << _uniqueID << ": " << _opCode;
	
	ArgListIter tIter;

	cout << " (";
	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			cout << ' ';
			(*tIter)->print(cout);
		}
	}

	cout << " )";
	cout << " (";
	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			cout << ' ';
			(*tIter)->print(cout);
		}
	}
	cout << " )";
}






// 26. Print in outstream 
void NormalOp::print(ostream& out)
{
	out << "OpCode: " << _opCode;
	
	ArgListIter tIter;

//	out << "Dest:";
	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			cout << " ";
			(*tIter)->print(out);
		}
	}

	out << "\t";
	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			cout << " ";
			(*tIter)->print(out);
		}
	}
}





// 27. Print  C to String







// 28. Print C







// 29. IR to C






// 30. Print to String
void NormalOp::printToString(char *buf)
{
	strcat(buf,globalOpCodeTable[_opCode.index()].opName());
	
	ArgListIter tIter;

	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		strcat(buf," ");
		if((*tIter)!=NULL)
			(*tIter)->printToString(buf);
	}

	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		strcat(buf," ");
		if((*tIter)!=NULL)
			(*tIter)->printToString(buf);
	}
}






// 31. Short Print
void NormalOp::shortPrint(ostream& out)
{
	cout << " (* " << this << " *)  ";
	out << _opCode;

	ArgListIter tIter;
	
	cout << " (";
	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			out <<"   ";
			if ((*tIter)->isRegisterArg()) 
				// i.e. print the register number (not string).
				((RegisterArg *)(*tIter))->print(0);
			else
				(*tIter)->print(out);			
		}
	}

	cout << " )";
	cout << " (";
	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			out <<" , ";
			if ((*tIter)->isRegisterArg()) 
				// i.e. print the register number (not string).
				((RegisterArg *)(*tIter))->print(0);
			else
				(*tIter)->print(out);			
		}
	}
	
	cout << " )";
// I do not print the properties here
}






// 32. Print Properties
void NormalOp::printProperties(ostream& out, PropNameList & pL)
{
	out << _opCode << " ";

	ArgListIter tIter;
	
	for (tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			if ((*tIter)->isRegisterArg()) 
				// i.e. print the register number (not string).
				((RegisterArg *)(*tIter))->printProperties(out, pL);
			else
				(*tIter)->printProperties(out, pL);	
			
			out << " ";
		}
	}

	out <<" , ";
	for (tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if ( (*tIter) != NULL )
		{
			if ((*tIter)->isRegisterArg()) 
				// i.e. print the register number (not string).
				((RegisterArg *)(*tIter))->printProperties(out, pL);
			else
				(*tIter)->printProperties(out, pL);			

			out << " ";
		}
	}
	
// I do not print the properties here
}






// 33. Some specific functions
	void NormalOp::initOpCode(const OpCode & someOpCode)
	{
		_opCode = someOpCode;
	}
	
	TableIndex NormalOp::getOpCodeIndex()
	{
		return _opCode.index();
	}
	
	OpCode & NormalOp::getOpCode()
	{
		return _opCode;
	}
	
	void NormalOp::getOpcodeName(MyString & s)
	{
		_opCode.mnemonic(s);
	}
	
	void NormalOp::changeOpCode(TableIndex newIndex)
	{
		_opCode.init(newIndex);
	}
	
	void NormalOp::changeOpCode(char *newStr)
	{
		_opCode.init(newStr);
	}



typedef map<MyString, BaseArgument *> regToArgMap;
typedef vector<MyString> stringVector;
void addToRegArgMap(MyString, BaseArgument *, regToArgMap &);
void printInOrderUsingFormatStr(ostream &, MyString, regToArgMap, stringVector, MyString, int);
int setPrintOrderAndFormatStr(MyString, stringVector, stringVector, stringVector &, MyString &);

// for ASM gen
void NormalOp::printASMInstr(ostream &out)
{
	MyString opCode;
	char operandType[6];
	int count = 1;
	MyString dst, src, formatStr;
	regToArgMap regToArg;
	stringVector dstVector, srcVector, printOrder;
	opCode = getOpcode(_opCode);

	if (!_destOperandList.empty()){
		ArgListIter tIter;

		// Also, do not print out double (pair) arguments. That is, just print the first of the pair.
		//
		int dstCount = 0;
		for(tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++, dstCount++){
			if ((*tIter) == NULL) continue;

			if ((dstCount >= _DOUBLE_DST_BEGIN_) && (dstCount <= _DOUBLE_DST_END_)) continue;

			if ((*tIter)->isRegisterArg()){
				dst = "reg";    
				//((RegisterArg *)_destOperand)->print(out); // i.e. print the register number (not string).
			}
			else if((*tIter)->isLabelArg()){
				dst = "lab";
			}
			else if((*tIter)->isImmediateArg()){
				dst = "imm";  // not possible
				//_destOperand->print(out);
			}
			
			dstVector.push_back(MyString(dst));
			sprintf(operandType, "dst%d", count++);

			addToRegArgMap(operandType, (*tIter), regToArg);
		}
	}
	count=1;

	if (!_sourceOperandList.empty()){
		ArgListIter tIter;

		// Also, do not print out double (pair) arguments. That is, just print the first of the pair.
		//
		int srcCount = 0;
		for(tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++,srcCount++){
			if ((*tIter) == NULL) continue;

			if ((srcCount >= _DOUBLE_SRC_BEGIN_) && (srcCount <=_DOUBLE_SRC_END_)) continue;

			if ((*tIter)->isRegisterArg()){
				src = "reg";    
				//((RegisterArg *)_destOperand)->print(out); // i.e. print the register number (not string).
			}
			else if((*tIter)->isLabelArg()){
				src = "lab";
			}
			else if((*tIter)->isImmediateArg()){
				src = "imm";  // not possible
				//_destOperand->print(out);
			}
			sprintf(operandType, "src%d", count++);
			
			srcVector.push_back(MyString(src));
			addToRegArgMap(operandType, (*tIter), regToArg);
		}
	}
	else  // operations like mx, mx_r and nop_r will not contain any source operand
	{
		src = "nil"; // sourceList is empty
		sprintf(operandType, "src%d", count++);
			
		srcVector.push_back(MyString(src));
		addToRegArgMap(operandType, NULL, regToArg);
		int noOfVarSrcs = setPrintOrderAndFormatStr(opCode, dstVector, srcVector, printOrder, formatStr);
		printInOrderUsingFormatStr(out, opCode, regToArg, printOrder, formatStr, noOfVarSrcs);
		return;
		
	}
	int noOfVarSrcs = setPrintOrderAndFormatStr(opCode, dstVector, srcVector, printOrder, formatStr);
	if(formatStr.getStr()==NULL){
		printf("Could not find ASM format for operation: ");
		this->print(cout);
		_ASSERT(0);
	}
	printInOrderUsingFormatStr(out, opCode, regToArg, printOrder, formatStr, noOfVarSrcs);
	return;

}


void NormalOp::recursiveRemoveProperty(PropertyNames name)
{
	BaseOperation::recursiveRemoveProperty(name);

	if (!_destOperandList.empty()){
		ArgListIter tIter;
		for(tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++){
			if ((*tIter) == NULL) continue;

			(*tIter)->recursiveRemoveProperty(name);
		}
	}
	if (!_sourceOperandList.empty()){
		ArgListIter tIter;
		for(tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++){
			if ((*tIter) == NULL) continue;

			(*tIter)->recursiveRemoveProperty(name);
		}
	}
}