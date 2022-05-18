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
// $Id: ComputeOp.cc,v 1.15 1998/04/11 00:36:38 xji Exp $
//
// File:  		ComputeOp.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: OPERATION

/* 
Last Change by AVS on 30th July, 2000, to include predication.
We intend to make new sources. i.e. Predicate sources. 
Some functions need to be modified. 
*/

#include "stdafx.h"

#include "ComputeOp.h"
#include "RegisterArg.h"

#include "UDProperty.h"
#include "DUProperty.h"
#include "PropertyIncludes.h"
#include "OpPredicate.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifdef COUNT_COMPUTEOPS
int ComputeOp::_numComputeOps = 0;	
#endif

/*
ControlOp format

  Control Operations are: <opcode> <dest> <source1> <source2>
  
	Originally,	_sourceOperand1 contained <source1>
				_sourceOperand2 contained <source2>.
				_destOperand contained <dest>
				
				  Now, after making sources and dests as lists, this has been changed to:
				  
					Control Operations :	_destOperandList[_DST_] : <dest> (corresponding to _destOperand)
					_sourceOperandList[_SRC_1_] : <source1> (corresponding to _sourceOperand1)
					_sourceOperandList[_SRC_2_] : <source2> (corresponding to _sourceOperand2)
					
*/


// 1. constructor
ComputeOp::ComputeOp()
{
	   // NormalOp constructors should be called automatically
#ifdef COUNT_COMPUTEOPS
	   _numComputeOps++;	
#endif
}

void printArgToStringAsC(BaseArgument *arg, MyString & s, int intOrFloatOrDouble, int srcOrDest, int num)
{
	if (arg->isRegisterArg())
	{
		s.concat("Do");
		
		switch (intOrFloatOrDouble)
		{
		case _C_INT_:
			s.concat("Integer");
			break;
		case _C_FLOAT_:
			s.concat("Float");
			break;
		case _C_DOUBLE_:
			s.concat("Double");
			break;
		case _C_2WORD_:
			s.concat("2Word");
			break;
		default:
			// error condition.
			cout << "Panic: Recieved un-recognizable type in printArgToStringAsC" << endl;
			assert(0);
		}

		if (srcOrDest == _C_SRC_)
			s.concat("Read");
		else
			s.concat("Write");

		s.concat("(");

		if (num == 1)
			arg->printCToString(s, "rtemp1");
		else if (num == 2)
			arg->printCToString(s, "rtemp2");
		else if (num == 3)
			arg->printCToString(s, "rtemp3");
		else
		{
			// error condition.
			//
			cout << "panic: Currently, we do not handle arg num greater than 3 in printArgToStringAsC" << endl;
			assert(0);
		}

		s.concat("); ");
	}
	else if (arg->isImmediateArg())
	{
		if (srcOrDest == _C_DST_)
		{
			// error condition.
			cout << "Panic: Dest cannot be immediate in printArgToStringAsC" << endl;
			assert(0);
		}

		if (num == 1)
			arg->printCToString(s, "rtemp1");
		else if (num == 2)
			arg->printCToString(s, "rtemp2");
		else if (num == 3)
			arg->printCToString(s, "rtemp3");
		else
		{
			// error condition.
			//
			cout << "panic: Currently, we do not handle arg num greater than 3 in printArgToStringAsC" << endl;
			assert(0);
		}

		s.concat("; ");
	}
	else if (arg->isLabelArg())
	{
		arg->printCToString(s);
	}
}
	   
// 2. copy constructor

ComputeOp::ComputeOp(ComputeOp & a)
{
	   ArgListIter tIter;
	   BaseArgument *arg;
	   
	   BaseArgument *tempArg = NULL;

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
			   _destOperandList.push_back(tempArg);
	   }
	   // Now copy the source list
	   for (tIter = a._sourceOperandList.begin(); tIter != a._sourceOperandList.end(); tIter++)
	   {
		   if ((arg = *tIter) != NULL)
			   _sourceOperandList.push_back(arg->copy());
		   else
			   _sourceOperandList.push_back(tempArg);
	   }
	   
#ifdef COUNT_COMPUTEOPS
	   _numComputeOps++;
#endif
}	




// 3. Destructor
ComputeOp::~ComputeOp()
{		
#ifdef COUNT_COMPUTEOPS
	   _numComputeOps--;
#endif
}



// 4. counter




// 5. copy
BaseOperation *ComputeOp::copy()
{
	   BaseOperation *retPtr;
	   
	   retPtr = new ComputeOp(*this);
	   
	   return retPtr;
}




// 6. copyBody
BaseOperation *ComputeOp::copyBody(bool copyProp)
{
	BaseOperation *retPtr;
	retPtr = new ComputeOp();
	ComputeOp *compretPtr = (ComputeOp*) retPtr;
	
	ArgListIter tIter;
	
	// copy the opCode
	compretPtr->_opCode = _opCode;
	
	BaseArgument *nullPtr = NULL;

	// Now copy the destination list
	for(tIter = _destOperandList.begin(); tIter != _destOperandList.end(); tIter++)
	{
		if (*tIter != NULL)
			compretPtr->_destOperandList.push_back((*tIter)->copy());
		else
			compretPtr->_destOperandList.push_back(nullPtr);
	}
	
	// Now copy the source list
	for(tIter = _sourceOperandList.begin(); tIter != _sourceOperandList.end(); tIter++)
	{
		if (*tIter != NULL)
			compretPtr->_sourceOperandList.push_back((*tIter)->copy());
		else
			compretPtr->_sourceOperandList.push_back(nullPtr);
	}
	
	// Copy the properties if required
	if (copyProp)
	{
		if (!retPtr->propertyList().isEmpty())
		{
			MyLinkedList<PropertyNames> propList;
			PropertyNames a(_DUCHAIN_);
			propList.append(a);
			a=_UDCHAIN_;
			propList.append(a);
			(retPtr->propertyList()).makeACopyOf(_opProperties, propList);
		}
	}
	
	return retPtr;
}





// 7. Initialize





// 8. Identify
int ComputeOp::isComputeOp()
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




// 18. isEq
int ComputeOp::isEq(BaseOperation *op)
{
	ArgListIter tIter1, tIter2;
	
	if (op == NULL) 
		return _NO_;
	
	if (!op->isComputeOp())
		return _NO_;
	
	if (!NormalOp::isEq(op))
		return _NO_;
	
	if (_sourceOperandList.size() != (((ComputeOp *)op)->_sourceOperandList).size())
		return _NO_;
	
	for (tIter1 = _sourceOperandList.begin(), tIter2 = (((ComputeOp *)op)->_sourceOperandList).begin(); tIter1 != _sourceOperandList.end(); tIter1++, tIter2++)
	{
		if (!(*tIter1)->isEq(*tIter2))
			return _NO_;
	}
	
	if (_destOperandList.size() != (((ComputeOp *)op)->_destOperandList).size())
		return _NO_;
	
	for (tIter1 = _destOperandList.begin(), tIter2 = (((ComputeOp *)op)->_destOperandList).begin(); tIter1 != _destOperandList.end(); tIter1++, tIter2++)
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
void ComputeOp::print()
{
	NormalOp::print();
	printOpProperty(cout);
	cout << endl;
}




// 24. Print with indent
void ComputeOp::print(int indent)
{
	printIndent(indent);
	NormalOp::print(indent);
}



extern BaseArgument *getHI(BaseArgument *arg);
extern BaseArgument *getLO(BaseArgument *arg);


//#define IRTOC_PRINT_REGISTERTRACE

void ComputeOp::printCToString(MyString & s)
{
	int numImmsInOp = 0;

  int opCodeGlobalIndex;	// index in the globalop code table
  EnumGenericOpCode genericOpCodeEnum;

  opCodeGlobalIndex = _opCode.index();
  genericOpCodeEnum = 
    globalOpCodeTable[opCodeGlobalIndex].getGenericOpCodeEnum();

  BaseArgument *src1Oprnd, *src2Oprnd, *dstOprnd;
/*
  if(_destOperand!=NULL)
    _destOperand->printCToString(s);
  
  s.concat(" = ");
*/

  // First, print out the code for generating the sources
  //
  switch (genericOpCodeEnum) {
  case _DEBUG_PRINT_:
  case _NOP_:
    break;
  case _ICONSTANT_:
  case _IASSIGN_:
  case _ASSIGN_: // Note: ASSIGN is being treated the same as ISSAIGN. Could be a problem. 
	  // should it be handled as type-less ?

  case _IEQ_:
  case _INE_:
  case _ILE_:
  case _IGE_:
  case _ILT_:
  case _IGT_:
  case _IEQU_:
  case _INEU_:
  case _ILEU_:
  case _IGEU_:
  case _ILTU_:
  case _IGTU_:

#ifndef NEW_GENERIC_OPCODES
  case _ILSH_:
  case _IASH_:
#else
  case _ILLSH_:
  case _IRLSH_:
  case _ILASH_:
  case _IRASH_:
#endif

  case _ILAND_:
  case _IAND_:
  case _ILOR_:
  case _IOR_:
  case _INOR_:
  case _IXOR_:

  case _CVTDI_:
  case _CVTSI_:

  case _MFC1_: // should it be handled as type-less ?
  case _MTC1_: // should it be handled as type-less ?

  case _IADD_:
  case _ISUB_:
  case _IMUL_:
  case _IMULU_:
  case _IDIV_:
  case _IDIVU_:
  case _IREM_:
    if ((src1Oprnd = ptrToOperand(_SRC_1_)) !=NULL)
		printArgToStringAsC(src1Oprnd, s, _C_INT_, _C_SRC_, 1); 
    if((src2Oprnd = ptrToOperand(_SRC_2_)) !=NULL)
		printArgToStringAsC(src2Oprnd, s, _C_INT_, _C_SRC_, 2); 
	break;
  case _FCONSTANT_:
  case _FASSIGN_:

  case _FEQ_:
  case _FNE_:
  case _FLE_:
  case _FGE_:
  case _FLT_:
  case _FGT_:

  case _CVTDS_:

  case _TRUNCIS_:

  case _FADD_:
  case _FSUB_:
  case _FMUL_:
  case _FDIV_:
    if ((src1Oprnd = ptrToOperand(_SRC_1_)) !=NULL)
		printArgToStringAsC(src1Oprnd, s, _C_FLOAT_, _C_SRC_, 1); 
    if((src2Oprnd = ptrToOperand(_SRC_2_)) !=NULL)
		printArgToStringAsC(src2Oprnd, s, _C_FLOAT_, _C_SRC_, 2); 
	break;
  case _DCONSTANT_:
  case _DASSIGN_:

  case _DEQ_:
  case _DNE_:
  case _DLE_:
  case _DGE_:
  case _DLT_:
  case _DGT_:

  case _CVTSD_:

  case _DMTC1_: // should it be handled as type-less ?
  case _DMFC1_: // should it be handled as type-less ?

  case _TRUNCID_:
  case _CVTID_:

  case _DADD_:
  case _DSUB_:
  case _DMUL_:
  case _DDIV_:
    if ((src1Oprnd = ptrToOperand(_SRC_1_)) !=NULL)
		printArgToStringAsC(src1Oprnd, s, _C_DOUBLE_, _C_SRC_, 1); 
    if((src2Oprnd = ptrToOperand(_SRC_2_)) !=NULL)
		printArgToStringAsC(src2Oprnd, s, _C_DOUBLE_, _C_SRC_, 2); 
	break;

#ifdef NEW_GENERIC_OPCODES
  case _MFHI_:
    if ((src1Oprnd = ptrToOperand(_SRC_1_)) !=NULL)
		printArgToStringAsC(getHI(src1Oprnd), s, _C_INT_, _C_SRC_, 1); 
	break;
  case _MFLO_:
    if ((src1Oprnd = ptrToOperand(_SRC_1_)) !=NULL)
		printArgToStringAsC(getLO(src1Oprnd), s, _C_INT_, _C_SRC_, 1); 
	break;
  case _MTHI_:
  case _MTLO_:
    if ((src1Oprnd = ptrToOperand(_SRC_1_)) !=NULL)
		printArgToStringAsC(src1Oprnd, s, _C_INT_, _C_SRC_, 1); 
	  break;
#endif

  default:
	  char buf[200];
	  sprintf(buf," /* Recieved an invalid opcode in ComputeOp. %s */",globalOpCodeTable[_opCode.index()].opName());
	  s.concat(buf);
	  break;
  }


  switch (genericOpCodeEnum) {
  case _ICONSTANT_:
  case _IASSIGN_:
    if((src2Oprnd = ptrToOperand(_SRC_2_)) ==NULL)
	  s.concat(" rtemp3 = rtemp1.cast_to(_INT); ");
	else
		s.concat(" rtemp3 = (rtemp1 + rtemp2).cast_to(_INT); ");
	  break;
  case _DCONSTANT_:
  case _DASSIGN_:
    if((src2Oprnd = ptrToOperand(_SRC_2_)) ==NULL)
	  s.concat(" rtemp3 = rtemp1.cast_to(_DBL); ");
	else
		s.concat(" rtemp3 = (rtemp1 + rtemp2).cast_to(_DBL); ");
	  break;
  case _FCONSTANT_:
  case _FASSIGN_:
    if((src2Oprnd = ptrToOperand(_SRC_2_)) ==NULL)
	  s.concat(" rtemp3 = rtemp1.cast_to(_FLT); ");
	else
		s.concat(" rtemp3 = (rtemp1 + rtemp2).cast_to(_FLT); ");
	  break;
  case _ASSIGN_:
    if((src2Oprnd = ptrToOperand(_SRC_2_)) ==NULL)
	  s.concat(" rtemp3 = rtemp1; ");
	else
		s.concat(" rtemp3 = rtemp1 + rtemp2; ");
    break;
  case _IADD_:
  case _DADD_:
  case _FADD_:
	  s.concat(" rtemp3 = rtemp1 + rtemp2; ");
	  break;
  case _ISUB_:
  case _DSUB_:
  case _FSUB_:
	  s.concat(" rtemp3 = rtemp1 - rtemp2; ");
    break;
  case _IMUL_:
	  s.concat(" rtemp3 = DoIMUL(rtemp1, rtemp2); ");
	  break;
  case _IMULU_:
	  s.concat(" rtemp3 = DoIMULU(rtemp1, rtemp2); ");
	  break;
  case _DMUL_:
  case _FMUL_:
	  s.concat(" rtemp3 = rtemp1 * rtemp2; ");
    break;
  case _IDIV_:
	  s.concat(" rtemp3 = DoIDIV(rtemp1, rtemp2); ");
	  break;
  case _IDIVU_:
	  s.concat(" rtemp3 = DoIDIVU(rtemp1, rtemp2); ");
	  break;
  case _DDIV_:
  case _FDIV_:
	  s.concat(" rtemp3 = rtemp1 / rtemp2; ");
    break;
  case _IREM_:
	  s.concat(" rtemp3 = rtemp1 % rtemp2; ");
    break;
  case _IEQ_:
  case _IEQU_:
  case _DEQ_:
  case _FEQ_:
	  s.concat(" rtemp3 = (rtemp1 == rtemp2); ");
    break;
  case _INE_:
  case _INEU_:
  case _DNE_:
  case _FNE_:
	  s.concat(" rtemp3 = (rtemp1 != rtemp2); ");
    break;
  case _ILE_:
  case _ILEU_:
  case _DLE_:
  case _FLE_:
	  s.concat(" rtemp3 = (rtemp1 <= rtemp2); ");
    break;
  case _IGE_:
  case _IGEU_:
  case _DGE_:
  case _FGE_:
	  s.concat(" rtemp3 = (rtemp1 >= rtemp2); ");
    break;
  case _ILT_:
  case _ILTU_:
  case _DLT_:
  case _FLT_:
	  s.concat(" rtemp3 = (rtemp1 < rtemp2); ");
    break;
  case _IGT_:
  case _IGTU_:
  case _DGT_:
  case _FGT_:
	  s.concat(" rtemp3 = (rtemp1 > rtemp2); ");
    break;

#ifndef NEW_GENERIC_OPCODES
  case _ILSH_:
    s.concat(" rtemp3 = ((rtemp2 >= 0) ? (rtemp1 << rtemp2) : (rtemp1 >> (-rtemp2))) ");
	break;
  case _IASH_:
    s.concat(" rtemp3 = ((rtemp2 >= 0) ? (rtemp1 << rtemp2) : (rtemp1 >> (-rtemp2))) ");
	break;
#else
  case _ILLSH_:
	  s.concat(" rtemp3 = rtemp1 << rtemp2; ");
    break;
  case _IRLSH_:
	  s.concat(" rtemp3 = rtemp1 >> rtemp2; ");
    break;
  case _ILASH_:
	  s.concat(" rtemp3 = rtemp1 << rtemp2; ");
    break;
  case _IRASH_:
	  s.concat(" rtemp3 = rtemp1 >> rtemp2; ");
    break;
#endif

  case _ILAND_:
  case _IAND_:
	  s.concat(" rtemp3 = rtemp1 & rtemp2; ");
    break;
  case _ILOR_:
  case _IOR_:
	  s.concat(" rtemp3 = rtemp1 | rtemp2; ");
    break;
  case _INOR_:
	  s.concat(" rtemp3 = ~(rtemp1 | rtemp2); ");
    break;
  case _IXOR_:
	  s.concat(" rtemp3 = rtemp1 ^ rtemp2; ");
    break;
  case _DNEG_:
	  cout << "Panic: DNEG not tackled in printIrToC()" << endl;
	  assert(0);

  case _CVTDI_:
	  s.concat(" rtemp3 = DoCVTDI(rtemp1); ");
	  break;
  case _CVTSI_:
	  s.concat(" rtemp3 = DoCVTSI(rtemp1); ");
	  break;
  case _CVTSD_:
	  s.concat(" rtemp3 = DoCVTSD(rtemp1); ");
	  break;
  case _CVTDS_:
	  s.concat(" rtemp3 = DoCVTDS(rtemp1); ");
	  break;
  case _DMTC1_:
	  s.concat(" rtemp3 = DoDMTC1(rtemp1); ");
	  break;
  case _DMFC1_:
	  s.concat(" rtemp3 = DoDMFC1(rtemp1); ");
	  break;  
  case _MFC1_:
	  s.concat(" rtemp3 = DoMFC1(rtemp1); ");
	  break;  
  case _MTC1_:
	  s.concat(" rtemp3 = DoMTC1(rtemp1); ");
	  break;  
  case _TRUNCID_:
  case _CVTID_:
	  s.concat(" rtemp3 = DoTRUNCID(rtemp1); ");
	  break;  
  case _TRUNCIS_:
	  s.concat(" rtemp3 = DoTRUNCIS(rtemp1); ");
	  break;  
	/* Note: The next set of opcodes are not ComputeOps */
  case _IVLOAD_:
  case _DVLOAD_:
  case _FVLOAD_:
  case _HIVLOAD_:
  case _HIVLOADU_:
  case _QIVLOAD_:
  case _QIVLOADU_:
  case _IVSTORE_:
  case _DVSTORE_:
  case _FVSTORE_:
  case _HIVSTORE_:
  case _HIVSTOREU_:
  case _QIVSTORE_:
  case _QIVSTOREU_:
  case _IF_:
  case _GOTO_:
  case _IGOTO_:
  case _CALL_:
  case _RETURN_:
// Radu - May 21, 2000
//  case _PRINTF_:
//  case _SQRT_:
//  case _EXP_:

#ifdef NEW_GENERIC_OPCODES
  case _MFHI_:
  case _MFLO_:
  case _MTHI_:
  case _MTLO_:
	  s.concat(" rtemp3 = rtemp1; ");
	  break;
#endif

  default:
    break;
 } // end of  switch (genericOpCodeEnum)

  switch (genericOpCodeEnum) {
  case _DEBUG_PRINT_:
  case _NOP_:
    break;
  case _ICONSTANT_:
  case _IASSIGN_:
  case _ASSIGN_: // Note: ASSIGN is being treated the same as ISSAIGN. Could be a problem. 
	  // should it be handled as type-less ?

  case _IEQ_:
  case _INE_:
  case _ILE_:
  case _IGE_:
  case _ILT_:
  case _IGT_:
  case _IEQU_:
  case _INEU_:
  case _ILEU_:
  case _IGEU_:
  case _ILTU_:
  case _IGTU_:

#ifndef NEW_GENERIC_OPCODES
  case _ILSH_:
  case _IASH_:
#else
  case _ILLSH_:
  case _IRLSH_:
  case _ILASH_:
  case _IRASH_:
#endif

  case _ILAND_:
  case _IAND_:
  case _ILOR_:
  case _IOR_:
  case _INOR_:
  case _IXOR_:

  case _MFC1_: // should it be handled as type-less ?
  case _MTC1_: // should it be handled as type-less ?

  case _TRUNCIS_:
  case _TRUNCID_:
  case _CVTID_:

  case _IADD_:
  case _ISUB_:
  case _IREM_:
    if((dstOprnd = ptrToOperand(_DST_, _DEST_LIST_)) !=NULL)
		printArgToStringAsC(dstOprnd, s, _C_INT_, _C_DST_, 3); 
	break;
  case _CVTSI_:
  case _CVTSD_:

  case _FCONSTANT_:
  case _FASSIGN_:

  case _FEQ_:
  case _FNE_:
  case _FLE_:
  case _FGE_:
  case _FLT_:
  case _FGT_:

  case _FADD_:
  case _FSUB_:
  case _FMUL_:
  case _FDIV_:
    if((dstOprnd = ptrToOperand(_DST_, _DEST_LIST_)) !=NULL)
		printArgToStringAsC(dstOprnd, s, _C_FLOAT_, _C_DST_, 3); 
	break;
  case _CVTDI_:
  case _CVTDS_:

  case _DCONSTANT_:
  case _DASSIGN_:

  case _DEQ_:
  case _DNE_:
  case _DLE_:
  case _DGE_:
  case _DLT_:
  case _DGT_:

  case _DMTC1_: // should it be handled as type-less ?
  case _DMFC1_: // should it be handled as type-less ?

  case _DADD_:
  case _DSUB_:
  case _DMUL_:
  case _DDIV_:
    if((dstOprnd = ptrToOperand(_DST_, _DEST_LIST_)) !=NULL)
		printArgToStringAsC(dstOprnd, s, _C_DOUBLE_, _C_DST_, 3); 
	break;
  case _IMUL_:
  case _IMULU_:
  case _IDIV_:
  case _IDIVU_:
    if((dstOprnd = ptrToOperand(_DST_, _DEST_LIST_)) !=NULL)
		printArgToStringAsC(dstOprnd, s, _C_2WORD_, _C_DST_, 3); 
	break;

#ifdef NEW_GENERIC_OPCODES
  case _MFHI_:
  case _MFLO_:
    if((dstOprnd = ptrToOperand(_DST_, _DEST_LIST_)) !=NULL)
		printArgToStringAsC(dstOprnd, s, _C_INT_, _C_DST_, 3); 
	break;
  case _MTHI_:
    if((dstOprnd = ptrToOperand(_DST_, _DEST_LIST_)) !=NULL)
		printArgToStringAsC(getHI(dstOprnd), s, _C_INT_, _C_DST_, 3); 
	break;
  case _MTLO_:
    if((dstOprnd = ptrToOperand(_DST_, _DEST_LIST_)) !=NULL)
		printArgToStringAsC(getLO(dstOprnd), s, _C_INT_, _C_DST_, 3); 
	break;
#endif

  default:
	  s.concat(" /* Recieved an invalid opcode in ComputeOp. */");
	  s.concat(" assert(0); ");
	  break;
  }
#ifdef IRTOC_PRINT_REGISTERTRACE
  s.concat(" rtemp3.printTyped(cout); ");
#endif
}


// 28. Print C
void ComputeOp::printC(int indent)
{
	printIndent(indent);
	
	MyString s;
	
	printCToString(s);
	
	s.print(cout);
} // end of fcn ComputeOp::printC





// 29. IR to C
void ComputeOp::irToC(FilePointer & fp, int indent)
{
	printIndent(fp, indent);
	
	// first, print out the assembly instruction as a comment.
	//
	char buf[500];
	buf[0] = '\0';
	printToString(buf);
	fprintf(fp, "/* (%s) */\n", buf);
	
	printIndent(fp, indent);
	
	MyString s;
	printCToString(s);
	if (s.isEmpty())
		fprintf(fp, "\n");
	else
		fprintf(fp, "%s", s.getStr());
}





// 30. Print to String
void ComputeOp::printToString(char *buf)
{
	NormalOp::printToString(buf);
}






// 31. Short Print
void ComputeOp::shortPrint(ostream& out)
{

	NormalOp::shortPrint(out);

	//NOSTEMP
	OpPredicate* prop1=(OpPredicate*)getPtrToProperty(_PREDICATE_);
	if (prop1!=NULL) 
	{
		prop1->print(out);
	}

}

// Note: I have separated the RegisterArg prints from the normal prints, because
// we may need to print register symboltable numbers instead of their names.
//




// 32. Print Properties
void ComputeOp::printProperties(ostream& out, PropNameList & pL)
{
	NormalOp::printProperties(out, pL);
	BaseOperation::printProperties(out, pL);
}






// 33. Some specific operations

int ComputeOp::isMoveOp(void)
{
	return _NO_; 
	
	if(getOpCodeIndex()==6 || getOpCodeIndex()==7)
		return _YES_;
	else
		return _NO_;
}


void ComputeOp::print(ostream& out)
{
	out << "OpCode: " << _opCode;
	
	if (ptrToOperand(_DST_, _DEST_LIST_) != NULL){
		out << " Dest: ";
		(ptrToOperand(_DST_, _DEST_LIST_))->print(out);
	}
	
	if (ptrToOperand(_SRC_1_) != NULL){
		out << " Op1: ";
		(ptrToOperand(_SRC_1_))->print(out);
	}
	
	if (ptrToOperand(_SRC_2_) != NULL) {
		out << " Op2: ";
		(ptrToOperand(_SRC_2_))->print(out);
  	 }
	
	out << endl;
}
