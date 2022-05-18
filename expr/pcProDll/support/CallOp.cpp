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
// $Id: CallRetOp.cc,v 1.3 1998/01/22 23:45:49 pgrun Exp $
//
// File:  		CallRetOp.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

#include "stdafx.h"

#include "CallOp.h"
#include "Routine.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_CALLOPS
   int CallRetOp::_numCallRetOps = 0;	
#endif

   Map<int, MyString> CallRetOp::_specialCallMap;
   Map<int, int> CallRetOp::_specialOpcodeMap;


/*		 
			 
			Now we have _sourceOperandList and _destOperandList
			This is the new structure :
			   
			_sourceOperandList[_CALL_ADDR_]	: _callAddress
			_sourceOperandList[_SRC_2_]	: _srcOperand
				 
			_destOperandList[_DST_]		: _destOperand
				   
					 
*/


// 33. some specific operations

extern int convertReg(int & a, BaseArgument *argPtr);
extern int isMainRoutineForOutC;






   
// 1. Constructor
CallRetOp::CallRetOp()
	{
		_ptrToRoutine = NULL;
		
#ifdef COUNT_CALLOPS
		_numCallRetOps++;	
#endif
	}







// 2. copy comstructor
CallRetOp::CallRetOp(CallRetOp & a) : _ptrToRoutine(a._ptrToRoutine)
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


   for (tIter = (a._parmList).begin(); tIter != (a._parmList).end(); tIter++)
	   addParm(*tIter);

   for (tIter = (a._listOfSavedRegs).begin(); tIter != (a._listOfSavedRegs).end(); tIter++)
	   addSavedReg(*tIter);

   // Currently, we do not copy any properties of the operation.
/*
   MyLinkedList<PropertyNames> propList;
   propList.appendElement(<insert_property_name_here>
   
   _opProperties.makeACopyOf(a._opProperties, propList);
*/

#ifdef COUNT_CALLOPS
   _numCallRetOps++;	
#endif
}






// 3. destructor
CallRetOp::~CallRetOp()
	{
		
		ArgListIter i1;
		for (i1 = _parmList.begin(); i1 != _parmList.end(); i1++)
		{
			if ((*i1) != NULL) delete (*i1);
		}
		
		for (i1 = _listOfSavedRegs.begin(); i1 != _listOfSavedRegs.end(); i1++)
		{
			if ((*i1) != NULL) delete (*i1);
		}
		
		_ptrToRoutine = NULL;
		
#ifdef COUNT_CALLOPS
		_numCallRetOps--;
#endif
	}







// 4. counter






// 5. copy
BaseOperation *CallRetOp::copy()
   {
    BaseOperation *retPtr;
    
    retPtr = new CallRetOp(*this);
    
    return retPtr;
   }







// 6. copyBody
BaseOperation *CallRetOp::copyBody(bool copyProp)
{
   BaseOperation *retPtr;
   retPtr = new CallRetOp(*this);
   
   if (copyProp)
   {
    // Currently, we do not copy any properties of the operation.
    /*
    MyLinkedList<PropertyNames> propList;
    propList.appendElement(<insert_property_name_here>
    
      (retPtr->getPropertyList()).makeACopyOf(_propertyPtr, propList);
    */
   }

  return retPtr;
}





// 7. Initialize





// 8. Identify
int CallRetOp::isCallRetOp()
	{
		return _YES_;
	}

   
   
   
// 9. has Operand




// 10. has Arg





// 11. Add Operand

   
   
   
   
// 12. ptr to operand

   
   
   
// 13. destOperand List

   
   
   
// 14. sourceOperand List

   
   
   
   
// 15. allOperand List
   
   
 
   
// 16. clear Operand




// 17. reset
void CallRetOp::reset()
{
	/*
	if (_callAddress != NULL)
		delete _callAddress;
	if (_destOperand != NULL)
		delete _destOperand;
	if (_srcOperand != NULL)
		delete _srcOperand;
	*/
	// I suppose instead of the above statements I can just call NormalOp::reset()

	NormalOp::reset();

	if (!_parmList.empty())
	{
		ArgListIter tIter;
		for (tIter = _parmList.begin(); tIter != _parmList.end(); tIter++)
		{
			if ((*tIter) != NULL) delete (*tIter);
		}
		_parmList.clear();
	}   
	//_destOperand = NULL;
	//_callAddress = NULL;
	//_srcOperand = NULL;   
	_ptrToRoutine = NULL;
}





// 18. isEq
int CallRetOp::isEq(BaseOperation *op)
{
	ArgListIter tIter1, tIter2;

	if (op == NULL) 
		return _NO_;
	
	if (!op->isCallRetOp())
		return _NO_;
	
	if (!NormalOp::isEq(op))
		return _NO_;
	
	if (_sourceOperandList.size() != (((CallRetOp *)op)->_sourceOperandList).size())
		return _NO_;

	for (tIter1 = _sourceOperandList.begin(), tIter2 = (((CallRetOp *)op)->_sourceOperandList).begin(); tIter1 != _sourceOperandList.end(); tIter1++, tIter2++)
	{
		if (!(*tIter1)->isEq(*tIter2))
			return _NO_;
	}
			
	if (_destOperandList.size() != (((CallRetOp *)op)->_destOperandList).size())
		return _NO_;

	for (tIter1 = _destOperandList.begin(), tIter2 = (((CallRetOp *)op)->_destOperandList).begin(); tIter1 != _destOperandList.end(); tIter1++, tIter2++)
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
void CallRetOp::print()
{
	NormalOp::print();
	
	if (!_parmList.empty())
	{
		cout << " Parms: ";
		ArgListIter tIter;
		
		for(tIter = _parmList.begin(); tIter != _parmList.end(); tIter++)
		{
			(*tIter)->print();
			cout << " ";
		}
	}
	
	if (!_listOfSavedRegs.empty())
	{
		cout << " Saved regs: ";
		ArgListIter tIter;
		
		for(tIter = _listOfSavedRegs.begin(); tIter != _listOfSavedRegs.end(); tIter++)
		{
			(*tIter)->print();
			cout << " ";
		}
	}
	
	cout << endl;
	
	printOpProperty(cout);
}






// 25. Print with indent
void CallRetOp::print(int indent)
{
	printIndent(indent);
	NormalOp::print(indent);
	
	if (!_parmList.empty())
	{
		cout << " ";
		ArgListIter tIter;
		
		for(tIter = _parmList.begin(); tIter != _parmList.end(); tIter++)
		{
			(*tIter)->print(cout);
			cout << " ";
		}
	}
	if (!_listOfSavedRegs.empty())
	{
		cout << " [ ";
		ArgListIter tIter;
		
		for(tIter = _listOfSavedRegs.begin(); tIter != _listOfSavedRegs.end(); tIter++)
		{
			(*tIter)->print(cout);
			cout << " ";
		}
		cout << "] ";
	}
}






// 26. Print in outstream 
void CallRetOp::print(ostream& out)
{
	NormalOp::print(out); 
	
	if (!_parmList.empty())
	{
		out << " Parms: ";
		ArgListIter tIter;
		
		for(tIter = _parmList.begin(); tIter != _parmList.end(); tIter++)
		{
			(*tIter)->print(out);
			out << " ";
		}
	}
	
	if (!_listOfSavedRegs.empty())
	{
		out << " Saved regs: ";
		ArgListIter tIter;
		
		for(tIter = _listOfSavedRegs.begin(); tIter != _listOfSavedRegs.end(); tIter++)
		{
			(*tIter)->print(out);
			out << " ";
		}
	}
	
	cout << endl;
	
}








// 27. Print  C to String








// 28. Print C
void CallRetOp::printC(int indent)
{

	if (isCallOp())
	{
		printIndent(indent);
		//  _opCode.printC();
		
		if(ptrToOperand(_DST_, _DEST_LIST_) !=NULL)
		{
			ptrToOperand(_DST_, _DEST_LIST_)->printC(cout);
			cout << " = ";
		}
		
		cout << ' ';
		if (ptrToOperand(_CALL_ADDR_) != NULL)
			ptrToOperand(_CALL_ADDR_)->printC(cout);
		cout << '(';
		
		if (!_parmList.empty())
		{
			ArgListIter tIter;
			
			tIter = _parmList.begin();
			(*tIter)->printC(cout);
			tIter++;
			for(tIter; tIter != _parmList.end(); tIter++)
			{
				cout << ',';
				(*tIter)->printC(cout);
			}
		}
		cout << ')';
	}
	else // isRetOp()
	{
		printIndent(indent);
		_opCode.printC();

		// TBD : also output the return value
	}
}



// 29. IR to C
void CallRetOp::irToC(FilePointer & fp, int indent)
{
	int uniqId;

	printIndent(fp, indent);

	// first, print out the assembly instruction as a comment.
	//
	char buf[1000];
	buf[0] = '\0';
	printToString(buf);
	fprintf(fp, "/* (%s) */\n", buf);

	printIndent(fp, indent);

	if (isCallOp())
	{
		// First, output the call args into the callArgsIndex table.
		// This table contains the register file index of each call argument.
		//
		fprintf(fp, "callArgsIndex.clear();\n");
		printIndent(fp, indent);

		MyString s;
		if (!_parmList.empty())
		{
			ArgListIter tIter;
			
			tIter = _parmList.begin();

			for (tIter; tIter != _parmList.end(); tIter++)
			{
				if ((*tIter)->isRegisterArg())
				{
					convertReg(uniqId, (*tIter));
					fprintf(fp, "callArgsIndex.push_back(%d); ", uniqId);
				}
				else
				{
					// currently, we do not handle the case when a call argument is not in a register.
					//
					cout << "Panic: Recieved a non register call argument" << endl;
					assert(0);
				}
			}
			fprintf(fp, "\n");
			printIndent(fp, indent);
		}

		// Changed by SAH on 07/29/2000
		//
		// Since we are currently, only running irtoc either before SSA or after RA,
		// we don't need to assign the call destination.
		// The appropriate register will already be set in the body of the call.
		//
		// However, if it was a system call, then the return register has to be explicitly set
		// after the call.
		//


		BaseArgument *dstOprnd;
		if ((dstOprnd = ptrToOperand(_DST_, _DEST_LIST_)) != NULL)
		{
			fprintf(fp, "callDestIndex.clear();\n");
			printIndent(fp, indent);

			if (dstOprnd->isRegisterArg())
				convertReg(uniqId, dstOprnd);
			else
			{
				// currently, we do not handle the case when a call argument is not in a register.
				//
				cout << "Panic: Recieved a non register call dest argument" << endl;
				assert(0);
			}
			fprintf(fp, "callDestIndex.push_back(%d); ", uniqId);
			fprintf(fp, "\n");
			printIndent(fp, indent);
		}
		/*
		if(_destOperand!=NULL)
		{
			MyString s1;
			_destOperand->printCToString(s1);
			fprintf(fp, "%s = ", s1.getStr());
		}
		*/
		if (ptrToOperand(_CALL_ADDR_) != NULL)
		{
			fprintf(fp, "Do_CALL(");
			MyString s2;
			if (ptrToOperand(_CALL_ADDR_)->isLabelArg())
			{
				char buf[80];
				buf[0] = '\0';
				ptrToOperand(_CALL_ADDR_)->printToString(buf);
				fprintf(fp, "\"%s\"", buf+1);
			}
			else
			{
				ptrToOperand(_CALL_ADDR_)->printCToString(s2);
				fprintf(fp, "%s", s2.getStr());
			}
/* 
** Commented by SAH on Wed, Aug 30th, 2000
** Previously, the destination register was explicitly set by the call.
** E.g. Do_CALL(_fopen, R4)
**
** Now, we handle it the same way as we handle input parameters.
** E.g. callDestIndex.push_back(4); Do_CALL(_fopen);
**

			if(ptrToOperand(_DST_, _DEST_LIST_)!=NULL)
			{
				MyString s1;
				ptrToOperand(_DST_, _DEST_LIST_)->printCToString(s1);
				fprintf(fp, ", %s)", s1.getStr());
			}
			else
			{
				fprintf(fp, ")");
			}
*/

			fprintf(fp, ")");
		}
	}
	else // isRetOp()
	{
		if (isMainRoutineForOutC)
		{
			fprintf(fp, "printf(\"\\n Cycle count = %%d\\n\", cycle_count);\n");
			fprintf(fp, "exit(0);\n");
		}
		else
			fprintf(fp, "return ");

		/* Commented by SAH on Thu, Aug 31, 2000
		** Return does not need to return a value, cos the appropriate register is already set.
		**
		if (ptrToOperand(_DST_, _DEST_LIST_) != NULL)
		{
			MyString s3;
			ptrToOperand(_DST_, _DEST_LIST_)->printCToString(s3);
			fprintf(fp, "(%s)", s3.getStr());
		}
		else if (!isMainRoutineForOutC)
		{
			fprintf(fp, "(REG[retRegNum])"); 
		}
		*/
	}
	fprintf(fp, ";");
}






// 30. Print to String
void CallRetOp::printToString(char *buf)
{
	NormalOp::printToString(buf);
	
	if (!_parmList.empty())
	{
		strcat(buf, " ");
		ArgListIter tIter;
		
		for(tIter = _parmList.begin(); tIter != _parmList.end(); tIter++)
		{
			(*tIter)->printToString(buf);
			strcat(buf, " ");
		}
	}
	if (!_listOfSavedRegs.empty())
	{
		strcat(buf, " [ ");
		ArgListIter tIter;
		
		for(tIter = _listOfSavedRegs.begin(); tIter != _listOfSavedRegs.end(); tIter++)
		{
			(*tIter)->printToString(buf);
			strcat(buf, " ");
		}
		strcat(buf, "] ");
	}
}





// 31. Short Print
void CallRetOp::shortPrint(ostream& out)
{
	NormalOp::shortPrint(out);

   if (!_parmList.empty())
   {
	   out << " ";
	   ArgListIter tIter;

		int tmpI = 0;
	   for(tIter = _parmList.begin(); tIter != _parmList.end(); tIter++)
	   {
		   out << " ";
		   (*tIter)->print(out);

// Note: There are some problems with cout. The buffer
// gets filled very soon. So we insert an endl after some iterations.
//
		   tmpI++;
		   if ((tmpI % 5) == 0)
			   out << endl;
	   }
   }

   if (!_listOfSavedRegs.empty())
   {
	   out << " ";
	   ArgListIter tIter;

		int tmpI = 0;
	   for(tIter = _listOfSavedRegs.begin(); tIter != _listOfSavedRegs.end(); tIter++)
	   {
		   out << " ";
		   (*tIter)->print(out);

// Note: There are some problems with cout. The buffer
// gets filled very soon.
//
		   tmpI++;
		   if ((tmpI % 5) == 0)
			   out << endl;
	   }
   }
}






// 32. Print Properties
void CallRetOp::printProperties(ostream& out, PropNameList & pL)
{
	NormalOp::printProperties(out, pL);

   if (!_parmList.empty())
   {
	   out << " ";
	   ArgListIter tIter;

		int tmpI = 0;
	   for(tIter = _parmList.begin(); tIter != _parmList.end(); tIter++)
	   {
		   out << " ";
		   (*tIter)->printProperties(out, pL);

// Note: There are some problems with cout. The buffer
// gets filled very soon. So we insert an endl after some iterations.
//
		   tmpI++;
		   if ((tmpI % 5) == 0)
			   out << endl;
	   }
   }

   if (!_listOfSavedRegs.empty())
   {
	   out << " ";
	   ArgListIter tIter;

		int tmpI = 0;
	   for(tIter = _listOfSavedRegs.begin(); tIter != _listOfSavedRegs.end(); tIter++)
	   {
		   out << " ";
		   (*tIter)->printProperties(out, pL);

// Note: There are some problems with cout. The buffer
// gets filled very soon.
//
		   tmpI++;
		   if ((tmpI % 5) == 0)
			   out << endl;
	   }
   }

   BaseOperation::printProperties(out, pL);
}
   





// 34. get Target Instruction
BaseInstruction* CallRetOp::getTargetInstr()
{
 if (_ptrToRoutine != NULL)
  return _ptrToRoutine->getFirstInstr();
 
 return NULL;
}





// 35. init Param List
void CallRetOp::initParmList(ArgList & a)
{
	ArgListIter tIter;
	BaseArgument *tempArg;

	for (tIter = a.begin(); tIter != a.end(); tIter++)
	{
	   if ((*tIter) != NULL)
	   {
		   tempArg = (*tIter)->copy();
		   _parmList.push_back(tempArg);
	   }
	}
} 



// 36. addParam
void CallRetOp::addParm(BaseArgument *a)
{
	if (a == NULL) return;
	_parmList.push_back(a->copy());
}



// 37. init Saved Register List
void CallRetOp::initSavedRegList(ArgList & a)
{
	ArgListIter tIter;
	BaseArgument *tempArg;

	for (tIter = a.begin(); tIter != a.end(); tIter++)
	{
	   if ((*tIter) != NULL)
	   {
		   tempArg = (*tIter)->copy();
		   _listOfSavedRegs.push_back(tempArg);
	   }
	}
} 



ArgList & CallRetOp::getParmList()
{ 
	return _parmList; 
}





// 38. add saved register
void CallRetOp::addSavedReg(BaseArgument *a)
{
	if (a == NULL) return;
	_listOfSavedRegs.push_back(a->copy());
}




//39. is Call Op
int CallRetOp::isCallOp()
{
 if (_opCode.typeOf() == CALL_OP)
  return _YES_;
 return _NO_;
}



//40. is Return Op 
int CallRetOp::isRetOp()
{
 if (_opCode.typeOf() == RET_OP)
  return _YES_;
 return _NO_;
}


// 41. add Special Call
void CallRetOp::addSpecialCall(int type, MyString & str)
{
	if (_specialCallMap.find(type) == _specialCallMap.end())
		_specialCallMap[type] = str;
	else
	{
		// error condition.
		//
		cout << "Warning: Tried to add an already existing Special Call: ";
		str.print(cout);
		cout << endl;
	}
}



// 42. is special call
int CallRetOp::isSpecialCall(int type)
{
	if (_sourceOperandList[_CALL_ADDR_] == NULL) return _NO_;

	if (_sourceOperandList[_CALL_ADDR_]->isLabelArg())
	{
		if (strcmp(_sourceOperandList[_CALL_ADDR_]->lName(), _getSpecialCallStr(type)) == 0)
			return _YES_;
	}
	return _NO_;
}


// 43. is special call
int CallRetOp::isSpecialCall(void)
{
	Map<int, MyString>::iterator i;
	for (i = _specialCallMap.begin(); i != _specialCallMap.end(); i++)
	{
		if (isSpecialCall((*i).first)) return _YES_;
	}
	return _NO_;
}



// 44. is dynamic address call
int CallRetOp::isDynamicAddrCall(void)
{
	if (_sourceOperandList[_CALL_ADDR_] == NULL) return _NO_;
	if (_sourceOperandList[_CALL_ADDR_]->isLabelArg()) return _NO_;

	return _YES_;
}



// 45. add special opcode
void CallRetOp::addSpecialOpcode(int type, int opcode)
{
	if (_specialOpcodeMap.find(type) == _specialOpcodeMap.end())
		_specialOpcodeMap[type] = opcode;
	else
	{
		// error condition.
		//
		cout << "Warning: Tried to add an already existing Special Opcode: " << opcode << endl;
	}
}



// 46. get special call type
int CallRetOp::getSpecialCallType()
{
	Map<int, MyString>::iterator i;
	for (i = _specialCallMap.begin(); i != _specialCallMap.end(); i++)
	{
		if (isSpecialCall((*i).first)) return ((*i).first);
	}
	return _INVALID_;
}


// 47. get special opcode
int CallRetOp::getSpecialOpcode(int type)
{
	if (_specialOpcodeMap.find(type) != _specialOpcodeMap.end())
		return _specialOpcodeMap[type];
	return _INVALID_;
}


// 48. get target label string
char* CallRetOp::getTargetLabelStr()
{
	if (_sourceOperandList[_CALL_ADDR_]->isLabelArg())
		return _sourceOperandList[_CALL_ADDR_]->lName();
	return NULL;
}


ArgList & CallRetOp::getSavedRegList() 
{ 
	return _listOfSavedRegs; 
}
	
Routine *CallRetOp::ptrToRoutine() 
{ 
	return _ptrToRoutine; 
}
	
void CallRetOp::setRoutine(Routine *rPtr) 
{ 
	_ptrToRoutine = rPtr; 
}
	

typedef map<MyString, BaseArgument *> regToArgMap;
typedef vector<MyString> stringVector;
void addToRegArgMap(MyString, BaseArgument *, regToArgMap &);
int setPrintOrderAndFormatStr(MyString, stringVector, stringVector, stringVector &, MyString &);
void printInOrderUsingFormatStr(ostream &, MyString, regToArgMap, stringVector, MyString, int);

void CallRetOp::printASMInstr(ostream& out)
{
	MyString opCode;
	MyString src, formatStr;
	int count = 1, libraryFuncCalled = 0;
	char operandType[6];
	regToArgMap regToArg;
	stringVector dstVector, srcVector, printOrder;
	opCode = getOpcode(_opCode);


	if (!_sourceOperandList.empty()){
		ArgListIter tIter;
		for(tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++){
			if ((*tIter) == NULL) continue;
			if ((*tIter)->isRegisterArg()){
				src = "reg";    
				//((RegisterArg *)_destOperand)->print(out); // i.e. print the register number (not string).
			}
			else if((*tIter)->isLabelArg()){
				src = "lab";
				if((*tIter)->testIfLibraryFunction())
					libraryFuncCalled = 1;
			}
			else if((*tIter)->isImmediateArg()){
				src = "imm";  // not possible
				//_destOperand->print(out);
			}
			if (libraryFuncCalled)
				src = "sav";  // special category to distinguish the library call
			sprintf(operandType, "src%d", count++);
			
			srcVector.push_back(MyString(src));
			addToRegArgMap(operandType, (*tIter), regToArg);
			libraryFuncCalled = 0;
		}
		int noOfVarSrcs = setPrintOrderAndFormatStr(opCode, dstVector, srcVector, printOrder, formatStr);
		if(formatStr.getStr()==NULL){
			printf("Could not find ASM format for operation: ");
			this->print(cout);
			_ASSERT(0);
		}
		printInOrderUsingFormatStr(out, opCode, regToArg, printOrder, formatStr, noOfVarSrcs);
		
	}
	else  // printing for RETURN  : More later for ASM : Partha
	{
		//out << "inside CallOp " << endl;
		//out << "printing for : " << opCode.getStr() << endl;
		//out << "\tjr\t$31\n";
		// out << "\tBLR\n";

		// a special category where there is no source for the operation
		// here we only consider source list empty because when source list is empty
		// destination list will surely be empty
		src = "nil"; // sourceList is empty
		sprintf(operandType, "src%d", count++);
			
		srcVector.push_back(MyString(src));
		addToRegArgMap(operandType, NULL, regToArg);
		int noOfVarSrcs = setPrintOrderAndFormatStr(opCode, dstVector, srcVector, printOrder, formatStr);
		printInOrderUsingFormatStr(out, opCode, regToArg, printOrder, formatStr, noOfVarSrcs);
		

		//		out << "\t#################################################################\n";
		//		out << "\t# Callee pops the stack frame and returns to the address in $ra #\n";
		//		out << "\t#################################################################\n";
	}
	
	//out << "----------------------------------------------------" << endl;	
}
