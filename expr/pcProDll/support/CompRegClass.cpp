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
// $Id$
//
// File:  		CompRegClass.cpp
// Created:		Mon Nov 1, 99
// Last modified: 	Mon Nov 1, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"

#include "BaseArgument.h"
#include "RealRegArg.h"
#include "RegClassArgProperty.h"
#include "RoutParmProperty.h"
#include "Routine.h"
#include "IfOp.h"
#include "CallOp.h"
#include "DoubleArgProperty.h"
#include "RegArchModel.h"
#include "Program.h"
#include "BaseInstruction.h"
#include "NormalInstr.h"
#include "BaseOperation.h"
#include "DataOpSlot.h"
#include "ControlOpSlot.h"
#include "ArrayIterator.h"
#include "RAProperty.h"
#include "RegArchModel.h"


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern Routine *currRoutine;
extern RegisterFileList *globalRegFileList;
extern RegArchModel globalRegModel;

extern int isSPArg(BaseArgument *arg);
extern int isFPArg(BaseArgument *arg);
extern int isPCArg(BaseArgument *arg);
extern int isCCArg(BaseArgument *arg);
extern int isHILOArg(BaseArgument *arg);
extern int isHardwiredZero(BaseArgument *arg);
extern int doneRA;

int isCallAddressArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	return _NO_;
}

// Note: This function does not check if the symbol table value of arg matches any of the items in aList.
// Instead it checks if aList actally contains the arg.
//
inline int isPresentInList(BaseArgument *arg, ArgList & aList)
{
	// iterate over aList. return _YES_ if it contains arg.
	//
	for (ArgListIter ai = aList.begin(); ai != aList.end(); ai++)
	{
		if (((*ai) != NULL) && ((*ai) == arg)) return _YES_;
	}
	return _NO_;
}

int isCallParmArg(BaseArgument *arg, BaseOperation *op)
{

	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is a call parm arg if:
	// (1) it is a source argument of a call operation
	// (2) it is used as a formal parameter
	//
	// In the first case, we need to check if op is a call op and if it contains arg in its parameter list.
	// In the second case, we need to look at the RoutParmProperty of the routine and see if it contains arg
	//

	if ((op != NULL) && (op->isCallOp()))
	{
		ArgList & argList = ((CallRetOp *)op)->getParmList();

		if (isPresentInList(arg, argList) == _YES_) return _YES_;
	}

	// Now, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;
	
	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(currRoutine->getPtrToProperty(_ROUTPARM_))) != NULL)
	{
		// Note: To be absolutely correct, we should also check if arg is not in the destination list
		// of the operation op.
		//
		if (parmProp->hasCallParm(arg) == _NO_) return _NO_;

		ArgList & dL = op->destOperandList();
		ArgListIter di;
		for (di = dL.begin(); di != dL.end(); di++)
		{
			if (*di == NULL) continue;
			if (*di == arg) return _NO_;
		}
		return _YES_;
	}
	return _NO_;
}

int isRetValArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is a return value arg if:
	// (1) it is a destination argument of a call operation
	// (2) it is a source argument of a return operation
	// (3) it is returned as a formal parameter.
	//

	if (op != NULL)
	{
		if (op->isCallOp())
		{
			ArgList & aList = op->destOperandList();
			if (isPresentInList(arg, aList) == _YES_) return _YES_;
		}
		if (op->isRetOp())
		{
			ArgList & aList = op->sourceOperandList();
			if (isPresentInList(arg, aList) == _YES_) return _YES_;
		}
	}

	// Now, check to see if the routine has a return parm (_ROUTPARM_) property
	//
	RoutParmProperty *parmProp;
	
	// check to see if rptr already has the RoutParmProperty with the callParms initialized.
	//
	if ((parmProp = (RoutParmProperty *)(currRoutine->getPtrToProperty(_ROUTPARM_))) != NULL)
	{
		// Note: To be absolutely correct, we should also check if arg is not in the destination list
		// of the operation op.
		//
		if (parmProp->hasReturnParm(arg) == _YES_) return _YES_;
	}

	return _NO_;
}

int isSPArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// if the argument has "sp" as its register file, then return _YES_
	//
	return (isSPArg(arg));
}

int isFPArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// if the argument has "fp" as its register file, then return _YES_
	//
	return (isFPArg(arg));
}

int isCCArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// if the argument is used in an if op, return _YES_
	//
	if ((op != NULL) && (op->isIfOp()))
	{
		if (arg == (((IfOp *)op)->ptrToCCReg())) return _YES_;
	}

	// Added by SAH on 08/20/01
	//
	// As of now, if an argument was originally supposed to be "cc", it is still considered to be "cc" type.
	//
	return (isCCArg(arg));
}


/*
int isAddressArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is a double arg if it has the double arg property.
	//
	if (arg->getPtrToProperty(_ADDRESSARG_) != NULL) return _YES_;

	return _NO_;
}
*/

int isDoubleArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is a double arg if it has the double arg property.
	//
	if (arg->getPtrToProperty(_DOUBLEARG_) != NULL) return _YES_;

	return _NO_;
}

int isDouble1Arg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is a double arg if it has the double arg property.
	//
	BaseArgProperty *prop;
	if ((prop = arg->getPtrToProperty(_DOUBLEARG_)) == NULL) return _NO_;

	return (((DoubleArgProperty *)prop)->isDouble1Type());
}

int isDouble2Arg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is a double arg if it has the double arg property.
	//
	BaseArgProperty *prop;
	if ((prop = arg->getPtrToProperty(_DOUBLEARG_)) == NULL) return _NO_;

	return (((DoubleArgProperty *)prop)->isDouble2Type());
}

int isFloatArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is a float arg if it:
	// (1) has "f" as its register file and 
	// (2) is not a double arg.
	//
	// We currently do not make the second check. (Assuming that that would have been done before).

	if( !doneRA )
	{
		if (strcmp(((RealRegArg *)arg)->regFilename(), "gf") == 0) return _YES_;
	}
	else
	   {
//		cout << "Warning: Target register mapping for f is not yet defined !" << endl;
		assert(0); // The code below is wrong.
		if (strcmp(((RealRegArg *)arg)->regFilename(), "f") == 0) return _YES_;
	}

/*  This segment of code looks at the register file to see if the argument could be a floating point arg.

	int idx = ((RealRegArg *)arg)->regFileIndex();
	if (((*globalRegFileList)[idx]).valType() != _FLOAT_) return _NO_;
*/

	return _NO_;
}

int isTargetRegisterFromRF(BaseArgument *arg, char *rfName)
{
	// Assume that RA has been done.
	//
	if (!doneRA) return _NO_;

	if ((arg == NULL) || (!arg->isRealRegArg())) return _NO_;
	RAProperty *raProp = (RAProperty *) arg->getPtrToProperty(_RA_);
	int regID = raProp->getRegID();
	RegisterFile& regFile = globalRegFileList->getRegFile(regID);
	
	if (strcmp(regFile.name(), rfName) == 0) return _YES_;

	return _NO_;
}

int isIntArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is an integer arg if it has "R" as its register file.
	//
	if( !doneRA )
	{
		if (strcmp(((RealRegArg *)arg)->regFilename(), "gR") == 0) return _YES_;
	}
	else
	{
//		cout << "Warning: Target Register set is not yet specified !" << endl;
		assert(0); // The code below is wrong.

		if (isTargetRegisterFromRF(arg, "R")) return _YES_;
	}

	// Added by SAH on 11/21/01
	//
	// Uncomment this portion to make:
	// cc, sp, fp into INT data-types.
	//
	
	if( !doneRA )
	{
		if (strcmp(((RealRegArg *)arg)->regFilename(), "gcc") == 0) return _YES_;
		if (strcmp(((RealRegArg *)arg)->regFilename(), "gsp") == 0) return _YES_;
		if (strcmp(((RealRegArg *)arg)->regFilename(), "gfp") == 0) return _YES_;
		if (strcmp(((RealRegArg *)arg)->regFilename(), "ghilo") == 0) return _YES_;
	}
	else
	{
		cout << "Warning: Target Register set is not yet specified !" << endl;
		if (isTargetRegisterFromRF(arg, "cc")) return _YES_;
		if (isTargetRegisterFromRF(arg, "sp")) return _YES_;
		if (isTargetRegisterFromRF(arg, "fp")) return _YES_;
		if (isTargetRegisterFromRF(arg, "hilo")) return _YES_;
	}
	

	return _NO_;
}

int isHILOArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// if the argument has "hilo" as its register file, return _YES_
	//
	return (isHILOArg(arg));
}

int isHWZeroArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// if the argument has "R0" as its register, return _YES_
	//
	return (isHardwiredZero(arg));
}

int isPCArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// if the argument has "pc" as its register file, return _YES_
	//
	return (isPCArg(arg));
}

int isRetAddrArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// Note: We currently do not bother about the return address argument.
	// This is assumed to be implicit (i.e. managed by the architecture itself).

	return _NO_;
}

int isNormalArg(BaseArgument *arg, BaseOperation *op)
{
	if ((arg == NULL) || (arg->isRealRegArg() != _YES_)) return _NO_;

	// an argument is a normal argument if
	// (1) is belongs to the "R", f, temp register file,
	// (2) is not a call parm, ret val, ret addr.
	//
	// Currently, we only make the first check. 
	// We do not make the second check. (Assuming that they have been done before).
	//

	if( !doneRA )
	{
		if (strcmp(((RealRegArg *)arg)->regFilename(), "gR") == 0) return _YES_;
		if (strcmp(((RealRegArg *)arg)->regFilename(), "gf") == 0) return _YES_;

		// Modified by SAH on 10/02/01.
		// 
		// Note: For generics, all other register names except for temp start with 'g'.
		//
		if (strcmp(((RealRegArg *)arg)->regFilename(), "temp") == 0) return _YES_;
	}
	else
	{
		cout << "Warning: Target Register set is not yet specified !" << endl;
		if (strcmp(((RealRegArg *)arg)->regFilename(), "R") == 0) return _YES_;
		if (strcmp(((RealRegArg *)arg)->regFilename(), "f") == 0) return _YES_;
		if (strcmp(((RealRegArg *)arg)->regFilename(), "temp") == 0) return _YES_;
	}
	
	
	return _NO_;
}


void addRegClassToArgument(BaseArgument *arg, int dType, int cType)
{
	if (arg == NULL) return;
	if (!arg->isRegisterArg()) return;

	BaseArgProperty *prop;

	// if prop == NULL, then add a new prop.

	RegClassEntryNode2 cls;
	cls._dType = dType; cls._cType = cType;

	if ((prop = arg->getPtrToProperty(_REGCLASS_)) == NULL)
	{
		RegClassArgProperty p;
		arg->addProperty(p);

		prop = arg->getPtrToProperty(_REGCLASS_);
	}

	((RegClassArgProperty *)prop)->addClass(cls);

}


/****************************************************************************/
/**** RegArchModel **********************************************************/
/****************************************************************************/

int dataTypeIndexDoubleGeneric;
int dataTypeIndexFloatGeneric;
int dataTypeIndexIntGeneric;
int dataTypeIndexAnyGeneric;
int classTypeIndexCallParmGeneric;
int classTypeIndexRetValGeneric;
int classTypeIndexSPGeneric;
int classTypeIndexFPGeneric;
int classTypeIndexCCGeneric;
int classTypeIndexHILOGeneric;
int classTypeIndexPCGeneric;
int classTypeIndexHWZeroGeneric;
int classTypeIndexRetAddrGeneric;
int classTypeIndexNormalGeneric;
int classTypeIndexAnyGeneric;
int classTypeIndexAddressGeneric;
int classTypeIndexDouble1Generic;
int classTypeIndexDouble2Generic;
int classTypeIndexRISAGeneric;
int classTypeIndexTempRISAGeneric;
int classTypeIndexRISA2Generic;
int classTypeIndexRISA4Generic;
int	classTypeIndexRISA8Generic;
int	classTypeIndexRISA16Generic;

int classTypeIndexCallParm1Generic;
int classTypeIndexCallParm2Generic;
int classTypeIndexCallParm3Generic;
int classTypeIndexCallParm4Generic;
int classTypeIndexCallParm5Generic;
int classTypeIndexCallParm6Generic;
int classTypeIndexCallParm7Generic;
int classTypeIndexCallParm8Generic;
int classTypeIndexCallParm9Generic;

void RegArchModel::initGlobalIndexVariables(int genericOrTarget)
{
	// currently, we only initialize the generic variables.
	//
	if (genericOrTarget != _GENERIC_) return;

	dataTypeIndexDoubleGeneric = getDataTypeIndex("DOUBLE", _GENERIC_);
	dataTypeIndexFloatGeneric = getDataTypeIndex("FLOAT", _GENERIC_);
	dataTypeIndexIntGeneric = getDataTypeIndex("INT", _GENERIC_);
	dataTypeIndexAnyGeneric = getDataTypeIndex("ANY", _GENERIC_);
	classTypeIndexCallParmGeneric = getClassTypeIndex("CALL_PARM", _GENERIC_);
	classTypeIndexRetValGeneric = getClassTypeIndex("RET_VAL", _GENERIC_);
	classTypeIndexSPGeneric = getClassTypeIndex("SP", _GENERIC_);
	classTypeIndexFPGeneric = getClassTypeIndex("FP", _GENERIC_);
	classTypeIndexCCGeneric = getClassTypeIndex("CC", _GENERIC_);
	classTypeIndexHILOGeneric = getClassTypeIndex("HILO", _GENERIC_);
	classTypeIndexPCGeneric = getClassTypeIndex("PC", _GENERIC_);
	classTypeIndexHWZeroGeneric = getClassTypeIndex("ZERO", _GENERIC_);
	classTypeIndexRetAddrGeneric = getClassTypeIndex("RET_ADDR", _GENERIC_);
	classTypeIndexNormalGeneric = getClassTypeIndex("NORMAL", _GENERIC_);
	classTypeIndexAnyGeneric = getClassTypeIndex("ANY", _GENERIC_);
	classTypeIndexAddressGeneric = getClassTypeIndex("ADDRESS", _GENERIC_);
	classTypeIndexDouble1Generic = getClassTypeIndex("DOUBLE1", _GENERIC_);
	classTypeIndexDouble2Generic = getClassTypeIndex("DOUBLE2", _GENERIC_);
	classTypeIndexRISAGeneric = getClassTypeIndex("RISA", _GENERIC_);
	classTypeIndexRISA2Generic = getClassTypeIndex("RISA2", _GENERIC_);
	classTypeIndexRISA4Generic = getClassTypeIndex("RISA4", _GENERIC_);
	classTypeIndexRISA8Generic = getClassTypeIndex("RISA8", _GENERIC_);
	classTypeIndexRISA16Generic = getClassTypeIndex("RISA16", _GENERIC_);
	classTypeIndexTempRISAGeneric = getClassTypeIndex("TEMP_RISA", _GENERIC_);

	classTypeIndexCallParm1Generic = getClassTypeIndex("CALL_PARM1", _GENERIC_);
	classTypeIndexCallParm2Generic = getClassTypeIndex("CALL_PARM2", _GENERIC_);
	classTypeIndexCallParm3Generic = getClassTypeIndex("CALL_PARM3", _GENERIC_);
	classTypeIndexCallParm4Generic = getClassTypeIndex("CALL_PARM4", _GENERIC_);
	classTypeIndexCallParm5Generic = getClassTypeIndex("CALL_PARM5", _GENERIC_);
	classTypeIndexCallParm6Generic = getClassTypeIndex("CALL_PARM6", _GENERIC_);
	classTypeIndexCallParm7Generic = getClassTypeIndex("CALL_PARM7", _GENERIC_);
	classTypeIndexCallParm8Generic = getClassTypeIndex("CALL_PARM8", _GENERIC_);
	classTypeIndexCallParm9Generic = getClassTypeIndex("CALL_PARM9", _GENERIC_);
}

void setDataTypeInRegClassEntryNode2ForArg(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op)
{
	int dType;

	if (isDoubleArg(arg, op))
	{
		dType = dataTypeIndexDoubleGeneric;
	}
	else if (isFloatArg(arg, op))
	{
		dType = dataTypeIndexFloatGeneric;
	}
	else if (isIntArg(arg, op))
	{
		dType = dataTypeIndexIntGeneric;
	}
	else
	{
		dType = dataTypeIndexAnyGeneric;
	}
	rcen._dType = dType;
}

void setClassTypeInRegClassEntryNode2ForArg(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op)
{
	int cType;

	/*if (isAddressArg(arg, op))
	{
		cType = classTypeIndexAddressGeneric;
	}
	else */
	if (isDouble1Arg(arg, op))
	{
		cType = classTypeIndexDouble1Generic;
	}
	else if (isDouble2Arg(arg, op))
	{
		cType = classTypeIndexDouble2Generic;
	}
	else if (isCallParmArg(arg, op))
	{
		cType = classTypeIndexCallParmGeneric;
	}
	else if (isRetValArg(arg, op))
	{
		cType = classTypeIndexRetValGeneric;
	}
	else if (isSPArg(arg, op))
	{
		cType = classTypeIndexSPGeneric;
	}
	else if (isFPArg(arg, op))
	{
		cType = classTypeIndexFPGeneric;
	}
	else if (isCCArg(arg, op))
	{
		cType = classTypeIndexCCGeneric;
	}
	else if (isHILOArg(arg, op))
	{
		cType = classTypeIndexHILOGeneric;
	}
	else if (isHWZeroArg(arg, op))
	{
		cType = classTypeIndexHWZeroGeneric;
	}
	else if (isPCArg(arg, op))
	{
		cType = classTypeIndexPCGeneric;
	}
	else if (isRetAddrArg(arg, op))
	{
		cType = classTypeIndexRetAddrGeneric;
	}
	else if (isNormalArg(arg, op))
	{
		cType = classTypeIndexNormalGeneric;
	}
	else // any argument
	{
		cType = classTypeIndexAnyGeneric;
	}
	rcen._cType = cType;
}

void setSPTypeInRegClassEntryNode2(RegClassEntryNode2 & rcen)
{
	rcen._cType = classTypeIndexSPGeneric;
	rcen._dType = dataTypeIndexAnyGeneric;
}

void setRISATypeInRegClassEntryNode2ForArg(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op)
{
	// set the data type correctly.
	//
	setDataTypeInRegClassEntryNode2ForArg(rcen, arg, op);

	// Next, set the Class type to RISA
	//
	rcen._cType = classTypeIndexRISAGeneric;
}

void setRISATypeInRegClassEntryNode2ForArg(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op, int classType)
{
	// set the data type correctly.
	//
	setDataTypeInRegClassEntryNode2ForArg(rcen, arg, op);
	
	// Next, set the Class type to RISA
	//
	if( classType == globalRegModel.getClassTypeIndex("RISA2", _GENERIC_) )
	{
		rcen._cType = classTypeIndexRISA2Generic;
	}
	else if( classType == globalRegModel.getClassTypeIndex("RISA4", _GENERIC_) )
	{
		rcen._cType = classTypeIndexRISA4Generic;
	}
	else if( classType == globalRegModel.getClassTypeIndex("RISA8", _GENERIC_) )
	{
		rcen._cType = classTypeIndexRISA8Generic;
	}
    else if( classType == globalRegModel.getClassTypeIndex("RISA16", _GENERIC_) )
	{
		rcen._cType = classTypeIndexRISA16Generic;
	}
	else
	{
		rcen._cType = classTypeIndexRISAGeneric;
	}
}

void setTempRISATypeInRegClassEntryNode2ForArg(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op)
{
	// set the data type correctly.
	//
	setDataTypeInRegClassEntryNode2ForArg(rcen, arg, op);

	// Next, set the Class type to RISA
	//
	rcen._cType = classTypeIndexTempRISAGeneric;
}


// setDataTypeInRegClassEntryNode2ForArg(...), setClassTypeInRegClassEntryNode2ForArg(...)
// 
// These two functions are defined in CompRegClass.cpp (in Analysis (support) Source)
//
// if the argument already contains a reg class property, this function does nothing.
//
void RegArchModel::setRegClassPropertyForArg(BaseArgument *arg, BaseOperation *op)
{
	if (arg == NULL) return;

	if (arg->getPtrToProperty(_REGCLASS_) != NULL) return;

	RegClassArgProperty tmp;
	RegClassEntryNode2 cls;

	// first set its data type.
	//
	setDataTypeInRegClassEntryNode2ForArg(cls, arg, op);

	// next set its class type.
	//
	setClassTypeInRegClassEntryNode2ForArg(cls, arg, op);

	tmp.addClass(cls);

	arg->addProperty(tmp);
}

// returns the number of additions that were made to the vector mergeInto.
//
// elements that were discarded becos they were already present in mergeInto do not count as additions.
//
int RegArchModel::mergeListsOfRegClassEntryNode2(Vector<RegClassEntryNode2> & mergeInto, Vector<RegClassEntryNode2> & mergeFrom, int genericOrTarget)
{
	// Note: this function follows the rules mentioned in the comments for the "createListOfClassTypesForArgs" function.
	//
	// iterate over mergeFrom. if an element in mergeFrom exists in mergeInto, discard the element.
	// else follow the rules mentioned below.

	int currDType, compareDType, currCType, compareCType;
	int tcurrCType, tcompareCType;
	int toAdd = 0;
	int numAdds = 0;

	Vector<RegClassEntryNode2>::iterator mFromIter, mIntoIter;

	for (mFromIter = mergeFrom.begin(); mFromIter != mergeFrom.end(); mFromIter++)
	{
		currDType = mFromIter->_dType;
		currCType = mFromIter->_cType;

		tcurrCType = currCType;
		
		if ((tcurrCType == classTypeIndexRISAGeneric) || (tcurrCType == classTypeIndexTempRISAGeneric) ||
			(tcurrCType == classTypeIndexRISA2Generic) || (tcurrCType == classTypeIndexRISA4Generic) ||
			(tcurrCType == classTypeIndexRISA8Generic) || (tcurrCType == classTypeIndexRISA16Generic)
			)
		{
			tcurrCType = classTypeIndexRISAGeneric;
		}

		numAdds++;
		toAdd = 1;

		for (mIntoIter = mergeInto.begin(); mIntoIter != mergeInto.end(); mIntoIter++)
		{
			compareDType = mIntoIter->_dType;
			compareCType = mIntoIter->_cType;
			tcompareCType = compareCType;

			if ((tcompareCType == classTypeIndexRISAGeneric) || (tcompareCType == classTypeIndexTempRISAGeneric) ||
				(tcompareCType == classTypeIndexRISA2Generic) || (tcompareCType == classTypeIndexRISA4Generic) ||
				(tcompareCType == classTypeIndexRISA8Generic) || (tcompareCType == classTypeIndexRISA16Generic)
				)
			{
				tcompareCType = classTypeIndexRISAGeneric;
			}


			if (currDType == compareDType)
			{
				if (currCType == compareCType) // discard the element.
				{
					toAdd = 0;
					numAdds--;
					break;
				}

				if (genericOrTarget == _TARGET_) // continue to the next iteration of the Into loop.
				{
					continue;
				}

				// is _GENERIC_, follow the rules.
				//
				if ((tcurrCType == classTypeIndexNormalGeneric) || (tcurrCType == classTypeIndexAnyGeneric)) // discard the element.
				{
					toAdd = 0;
					numAdds--;
					break;
				}
				if ((tcompareCType == classTypeIndexNormalGeneric) || (tcompareCType == classTypeIndexAnyGeneric)) // change the element in mIntoIter from NORMAL to the currCType case.
				{
					mIntoIter->_cType = currCType;
					toAdd = 0;
					break;
				}

				// Addition for ADDRESS (pointer registers)
				// By SAH and GG on 02/15/02
				//
				if (currCType == classTypeIndexAddressGeneric)
				{
					if ((compareCType == classTypeIndexSPGeneric) 
						|| (compareCType == classTypeIndexCCGeneric) 
						|| (compareCType == classTypeIndexFPGeneric)
						|| (compareCType == classTypeIndexHILOGeneric)
						|| (compareCType == classTypeIndexHWZeroGeneric))
					{
						toAdd = 0;
						break;
					}
					if ((compareCType == classTypeIndexNormalGeneric) || (compareCType == classTypeIndexAnyGeneric)) // discard the element.
					{
						toAdd = 0;
						mIntoIter->_cType = currCType;
						break;
					}
				}

				if (compareCType == classTypeIndexAddressGeneric)
				{
					if ((currCType == classTypeIndexSPGeneric) 
						|| (currCType == classTypeIndexCCGeneric) 
						|| (currCType == classTypeIndexFPGeneric)
						|| (currCType == classTypeIndexHILOGeneric)
						|| (currCType == classTypeIndexHWZeroGeneric))
					{
						mIntoIter->_cType = currCType;
						toAdd = 0;
						break;
					}
					if ((currCType == classTypeIndexNormalGeneric) || (currCType == classTypeIndexAnyGeneric)) // discard the element.
					{
						toAdd = 0;
						break;
					}
				}


				// Addition for rISA.
				// By SAH on 08/25/01
				//
				if (tcurrCType == classTypeIndexRISAGeneric)
				{
					if ((tcompareCType == classTypeIndexNormalGeneric) || (tcompareCType == classTypeIndexAnyGeneric))
					{
						// add the currCTYpe into the Into array.
						//
						mIntoIter->_cType = currCType;
						toAdd = 0;
						break;
					}
				}
				if (tcompareCType == classTypeIndexRISAGeneric)
				{
					if ((tcurrCType == classTypeIndexNormalGeneric) || (tcurrCType == classTypeIndexAnyGeneric))
					{
						// discard the element.
						//
						toAdd = 0;
						numAdds--;
						break;
					}
				}
				if ((tcompareCType == classTypeIndexRISAGeneric) && (currCType == classTypeIndexTempRISAGeneric)) // discard
				{
					toAdd = 0;
					numAdds--;
					break;
				}
				if ((tcurrCType == classTypeIndexRISAGeneric) && (compareCType == classTypeIndexTempRISAGeneric)) // discard
				{
					mIntoIter->_cType = currCType;
					toAdd = 0;
					break;
				}
				if (tcurrCType == classTypeIndexRISAGeneric)
				{
					if ((tcompareCType == classTypeIndexSPGeneric) 
						|| (tcompareCType == classTypeIndexCCGeneric) 
						|| (tcompareCType == classTypeIndexFPGeneric)
						|| (tcompareCType == classTypeIndexHILOGeneric)
						|| (tcompareCType == classTypeIndexHWZeroGeneric))
					{
						// discard
						toAdd = 0;
						numAdds--;
						break;
					}
				}
				if (tcompareCType == classTypeIndexRISAGeneric)
				{
					if ((tcurrCType == classTypeIndexSPGeneric) 
						|| (tcurrCType == classTypeIndexCCGeneric) 
						|| (tcurrCType == classTypeIndexFPGeneric)
						|| (tcurrCType == classTypeIndexHILOGeneric)
						|| (tcurrCType == classTypeIndexHWZeroGeneric))
					{
						mIntoIter->_cType = currCType;
						toAdd = 0;
						break;
					}
				}
				//
				// end Addition for rISA
			}
			else // the  two data types are not equal.
			{
				if ((currDType == dataTypeIndexFloatGeneric) && (compareDType == dataTypeIndexDoubleGeneric))
				{
					toAdd = 0;
					break;
				}
				else if ((compareDType == dataTypeIndexFloatGeneric) && (currDType == dataTypeIndexDoubleGeneric))
				{
					mIntoIter->_dType = currDType;
					mIntoIter->_cType = currCType;
					toAdd = 0;
					break;
				}
				
				// Addition for ADDRESS (pointer registers)
				// By SAH and GG on 02/15/02
				//
				if (currCType == classTypeIndexAddressGeneric)
				{
					if ((compareCType == classTypeIndexSPGeneric) 
						|| (compareCType == classTypeIndexCCGeneric) 
						|| (compareCType == classTypeIndexFPGeneric)
						|| (compareCType == classTypeIndexHILOGeneric)
						|| (compareCType == classTypeIndexHWZeroGeneric))
					{
						toAdd = 0;
						break;
					}
					if ((compareCType == classTypeIndexNormalGeneric) || (compareCType == classTypeIndexAnyGeneric)) // discard the element.
					{
						toAdd = 0;
						mIntoIter->_dType = currDType;
						mIntoIter->_cType = currCType;
						break;
					}
				}

				if (compareCType == classTypeIndexAddressGeneric)
				{
					if ((currCType == classTypeIndexSPGeneric) 
						|| (currCType == classTypeIndexCCGeneric) 
						|| (currCType == classTypeIndexFPGeneric)
						|| (currCType == classTypeIndexHILOGeneric)
						|| (currCType == classTypeIndexHWZeroGeneric))
					{
						mIntoIter->_dType = currDType;
						mIntoIter->_cType = currCType;
						toAdd = 0;
						break;
					}
					if ((currCType == classTypeIndexNormalGeneric) || (currCType == classTypeIndexAnyGeneric)) // discard the element.
					{
						toAdd = 0;
						break;
					}
				}

				// Addition for rISA.
				// By SAH on 08/25/01
				//
				if (tcurrCType == classTypeIndexRISAGeneric)
				{
					if ((tcompareCType == classTypeIndexSPGeneric) 
						|| (tcompareCType == classTypeIndexCCGeneric) 
						|| (tcompareCType == classTypeIndexFPGeneric)
						|| (tcompareCType == classTypeIndexHILOGeneric)
						|| (tcompareCType == classTypeIndexHWZeroGeneric))
					{
						// discard
						toAdd = 0;
						numAdds--;
						break;
					}
				}
				else if (tcompareCType == classTypeIndexRISAGeneric)
				{
					if ((tcurrCType == classTypeIndexSPGeneric) 
						|| (tcurrCType == classTypeIndexCCGeneric) 
						|| (tcurrCType == classTypeIndexFPGeneric)
						|| (tcurrCType == classTypeIndexHILOGeneric)
						|| (tcurrCType == classTypeIndexHWZeroGeneric))
					{
						mIntoIter->_dType = currDType;
						mIntoIter->_cType = currCType;
						toAdd = 0;
						break;
					}
				}
				//
				// end addition for rISA.
			}
		}

		if (toAdd)
		{
			RegClassEntryNode2 tmp;
			tmp._dType = currDType;
			tmp._cType = currCType;

			mergeInto.push_back(tmp);
		}
	}
	return numAdds;
}

// Note: We assume that each argument in aList already contains the RegClassArg property
//
void RegArchModel::createListOfClassTypesForArgs(ArgList & aList, Vector<RegClassEntryNode2> & vect, int genericOrTarget)
{
	// if genericOrTarget == _GENERIC_
	//
	// iterate over the list of arguments.
	//
	// for each argument get its RegClassArg property.
	// then, use the following rules for combining these properties.
	//
	// Assume that vect contains a list of class nodes v1 ... vn
	// Let the property of the current argument be p
	//
	// if p.dType != {v1 ... vn}.dType, create a new node v(n+1) and add it to vect.
	// else let {vi} be the set of nodes that have the same data type.
	//
	// if (vi.cType == p.cType) do nothing.
	// else if (p.cType == NORMAL || ANY) do nothing.
	// else if (vi.cType == NORMAL || ANY) vi.cType = p.cType.
	// else if (vi.dType == FLOAT) && (p.dType == DOUBLE) vi.cType = DOUBLE.
	// else if (vi.dType == DOUBLE) && (p.dType == FLOAT) discard (do nothing).
	// else, create a new node v(n+1) (with cType and dType = p) and add it to vect.

	// if genericOrTarget == _TARGET_
	//
	// iterate over the list of arguments.
	//
	// for each argument get its RegClassArg property.
	// if that propery has not already been added to vect, add it.

	// Addition for rISA.
	// By SAH on 08/25/01
	//
	// if (vi.cType == NORMAL) && (p.cType == RISA) vi.cType = RISA
	// if (vi.cType == NORMAL) && (p.cType == TEMP_RISA) vi.cType = TEMP_RISA
	//
	// if (vi.cType == RISA) && (p.cType == NORMAL) discard
	// if (vi.cType == TEMP_RISA) && (p.cType == NORMAL) discard
	//
	// if (vi.cType == TEMP_RISA) && (p.cType == RISA) vi.cType = RISA
	// if (vi.cType == RISA) && (p.cType == TEMP_RISA) discard
	//
	// if (vi.cType == RISA) && (p.cType == SP) vi.cType = SP
	// if (vi.cType == TEMP_RISA) && (p.cType == SP) vi.cType = SP
	// if (vi.cType == SP) && (p.cType == RISA) discard
	// if (vi.cType == SP) && (p.cType == TEMP_RISA) discard
	//
	// if (vi.cType == RISA) && (p.cType == CC) vi.cType = CC
	// if (vi.cType == TEMP_RISA) && (p.cType == CC) vi.cType = CC
	// if (vi.cType == CC) && (p.cType == RISA) discard
	// if (vi.cType == CC) && (p.cType == TEMP_RISA) discard
	//
	// if (vi.cType == RISA) && (p.cType == FP) vi.cType = FP
	// if (vi.cType == TEMP_RISA) && (p.cType == SP) vi.cType = SP
	// if (vi.cType == FP) && (p.cType == RISA) discard
	// if (vi.cType == FP) && (p.cType == TEMP_RISA) discard
	

	BaseArgProperty *prop;

	ArgListIter ai;
	for (ai = aList.begin(); ai != aList.end(); ai++)
	{
		if ((prop = (*ai)->getPtrToProperty(_REGCLASS_)) != NULL)
		{
			mergeListsOfRegClassEntryNode2(vect, ((RegClassArgProperty *)prop)->getClasses(), genericOrTarget);
		}
	}
}

// Note: We assume that each argument in aList already contains the RegClassArg property
//
void RegArchModel::createListOfClassTypesForArgs(SetList<BaseArgument *> *args, Vector<RegClassEntryNode2> & vect, int genericOrTarget)
{
	// See comments for the method above. 
	//
	// createListOfClassTypesForArgs(ArgList & aList, Vector<RegClassEntryNode2> & vect, int genericOrTarget)


	BaseArgProperty *prop;

	MyLinkedListIterator<BaseArgument *> *iter = args->elementsForward();
	BaseArgument *arg;

	while (iter->hasMoreElements())
	{
		arg = iter->currentElement();
		if ((prop = arg->getPtrToProperty(_REGCLASS_)) != NULL)
		{
			mergeListsOfRegClassEntryNode2(vect, ((RegClassArgProperty *)prop)->getClasses(), genericOrTarget);
		}
		iter->nextElement();
	}
	delete iter;
}

// sets the compatibility of classes in genericOrTarget1, based on its mapping to classes (and registers)
// of genericOrTarget2.
// Note: A <classtype, datatype> tuple is compatible with another <classtype, datatype> tuple
// if they have atleast one register (from genericOrTarget2) in common.
//
void RegArchModel::setCompatibilityArray(int genericOrTarget1, int genericOrTarget2)
{
	// currently only handle case when genericOrTarget1 == _GENERIC_ and genericOrTarget2 == _TARGET_
	//
	if (!((genericOrTarget1 == _GENERIC_) && (genericOrTarget2 == _TARGET_)))
	{
		// TBD
		assert(0);
	}

	// for each class1 & class2 (classtype+datatype) in genericOrTarget1
	// compute their mapping into geenricOrTarget2
	//
	RegClassEntryNode2ListIter i, j; 

	RegClassEntryNode2List uniqClasses;
	setListOfUniqClasses(uniqClasses, genericOrTarget1);

	for (i = uniqClasses.begin(); i != uniqClasses.end(); i++)
	{
//		cout << "Compatibility for: "; i->print(cout);

		for (j = i+1; j != uniqClasses.end(); j++)
		{
//			cout << " with: "; j->print(cout); cout << " ";

			Vector<int> rIList;
			getRegsThatSatisfyAllMappings(genericOrTarget1, *i, *j, genericOrTarget2, rIList);

			if (!rIList.empty()) // this means that *i, *j are compatible.
			{
				(_compatibilityArray[*i]).push_back(*j);
				(_compatibilityArray[*j]).push_back(*i);
			}
		}
//		cout << endl;
	}
}

// returns compatibility based on the compatibility array set by the method written above.
//
int RegArchModel::isCompatible(RegClassEntryNode2 en1, RegClassEntryNode2 en2)
{
	// Default, return _YES_
	//
	if (en1 == en2) return _YES_;

	if (std::find((_compatibilityArray[en1]).begin(), (_compatibilityArray[en1]).end(), en2) == (_compatibilityArray[en1]).end())
		return _NO_;

	return _YES_;
}

// returns compatibility based on the compatibility array set by the method written above.
//
// returns yes if there is atleast 1 register that satisfies all the classes in both vectors.
//
int RegArchModel::isCompatible(Vector<RegClassEntryNode2> & ven1, Vector<RegClassEntryNode2> & ven2)
{
	// TBD.
	assert(0);
	return _YES_;
}

extern RegArchModel globalRegModel;

void addNewRegClassArgPropertyToArg(BaseArgument *arg, int cType, int dType)
{
	if (arg == NULL) return;

	if (arg->getPtrToProperty(_REGCLASS_) != NULL) return;

	RegClassArgProperty tmp;
	RegClassEntryNode2 cls;
	cls._cType = cType;
	cls._dType = dType;

	tmp.addClass(cls);

	arg->addProperty(tmp);
}

void addCallParmRegClassArgProperty(BaseArgument *arg, int cnt)
{
	int dT = dataTypeIndexIntGeneric;
	int cT = classTypeIndexCallParmGeneric;

	switch (cnt)
	{
	case 1:	// first parameter
		cT = classTypeIndexCallParm1Generic;
		break;
	case 2: // second parameter
		cT = classTypeIndexCallParm2Generic;
		break;
	case 3: // second parameter
		cT = classTypeIndexCallParm3Generic;
		break;
	case 4: // second parameter
		cT = classTypeIndexCallParm4Generic;
		break;
	case 5: // second parameter
		cT = classTypeIndexCallParm5Generic;
		break;
	case 6: // second parameter
		cT = classTypeIndexCallParm6Generic;
		break;
	case 7: // second parameter
		cT = classTypeIndexCallParm7Generic;
		break;
	case 8: // second parameter
		cT = classTypeIndexCallParm8Generic;
		break;
	case 9: // second parameter
		cT = classTypeIndexCallParm9Generic;
		break;
	default: // do nothing.
		;
	}

	addNewRegClassArgPropertyToArg(arg, cT, dT);
}

void addCallParmRegClassArgProperty(BaseOperation *op)
{
	if ((op == NULL) || (!op->isCallOp())) return;

	// iterate thru the parm list.
	//
	ArgList & pL = ((CallRetOp *)op)->getParmList();
	ArgListIter pLI;

	int cnt = 1;
	for (pLI = pL.begin(); pLI != pL.end(); pLI++)
	{
		if (*pLI == NULL) continue;

		addCallParmRegClassArgProperty(*pLI, cnt);
		cnt++;
	}
}

void compRegClass(BaseOperation *oper)
{
	// First, go thru the list of dest operands.
	// For each "register" operand, determine its register class.
	//
	ArgList & dL = oper->destOperandList();
	ArgListIter diter;
	for (diter = dL.begin(); diter != dL.end(); diter++)
	{
		if (*diter == NULL) continue;
		if (!(*diter)->isRegisterArg()) continue;

		globalRegModel.setRegClassPropertyForArg(*diter, oper);
	}

	ArgList & sL = oper->sourceOperandList();
	ArgListIter siter;
	for (siter = sL.begin(); siter != sL.end(); siter++)
	{
		if (*siter == NULL) continue;
		if (!(*siter)->isRegisterArg()) continue;

		globalRegModel.setRegClassPropertyForArg(*siter, oper);
	}

	if (oper->isCallOp())
		addCallParmRegClassArgProperty(oper);
}

void compRegClass(BaseInstruction *instr)
{
	// set the register classes for each operation.
	//
	// Go thru the array of data ops and control ops.
	//
	ArrayIterator<DataOpSlot> *dIter = ((NormalInstr *)instr)->getDataOpSlotIter();
	while (dIter->hasMoreElements())
	{
		if ((dIter->currentElement()).hasOper())
			compRegClass((dIter->currentElement()).getPtrToOperation());

		dIter->nextElement();
	}
	delete dIter;

	ArrayIterator<ControlOpSlot> *cIter = ((NormalInstr *)instr)->getControlOpSlotIter();
	while (cIter->hasMoreElements())
	{
		if ((cIter->currentElement()).hasOper())
			compRegClass((cIter->currentElement()).getPtrToOperation());

		cIter->nextElement();
	}
	delete cIter;
}


void compRegClassForSpecialNOP(BaseInstruction *instr)
{
	if (instr == NULL) return;

	// Assume that the instruction only contains the NOP operation.
	//
	BaseOperation *noper = instr->getPtrToRootOperation();

	if (noper == NULL) return;

	// iterate over the destination list.
	//
	ArgList & dL = noper->destOperandList();
	ArgListIter i;

	int cnt = 0;

	for (i = dL.begin(); i != dL.end(); i++)
	{
		if (*i == NULL) continue;

		if (cnt == 0) // the first argument is the sp arg.
			addNewRegClassArgPropertyToArg(*i, classTypeIndexSPGeneric, dataTypeIndexIntGeneric);
		else
			addCallParmRegClassArgProperty(*i, cnt);
		cnt++;
	}
}

void compRegClass(Routine *rout)
{
	// set the register classes for each instruction.
	//
	MyLinkedListIterator<BaseInstruction *> *iter = rout->instrListIteratorForw();

	// The first instruction is the NOP instruction that contains a definition of:
	// sp, and the parameters used in the routine.
	// Treat it separately.
	//
	compRegClassForSpecialNOP(iter->currentElement());
	iter->nextElement();
	while (iter->hasMoreElements())
	{
		compRegClass(iter->currentElement());
		iter->nextElement();
	}
	delete iter;
}

void compRegClass(Program *prog)
{
	// set the register classes for each routine.
	//
	MyLinkedListIterator<Routine *> *pIter = prog->routineListIteratorForw();

	while (pIter->hasMoreElements())
	{
		compRegClass(pIter->currentElement());
		pIter->nextElement();
	}
	delete pIter;
}
