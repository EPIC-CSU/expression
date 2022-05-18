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
// $Id: RealRegArg.cc,v 1.4 1998/03/27 00:02:54 wtang Exp $
//
// File:  		RealRegArg.cc
// Created:		Wed Nov 20, 96
// Last modified: 	Wed Nov 20, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: ARGUMENT

#include "stdafx.h"

#include "RealRegArg.h"
#include "RegisterFileList.h"
#include "SymbolTable.h"
#include "RAProperty.h"
#include "SSAArgProperty.h"
#include "BaseArgProperty.h"

#define _CC_ 0
#define _HILO_ 1
#define _SP_ 2
#define _FP_ 3
#define _TEMP_ -1
#define _RISA_ -1

extern RegisterFileList *globalRegFileList;
extern SymbolTable *globalSymbolTable;
extern int strcmpci(char *first, char *second);

extern int doneSSA;
extern int doneRA;

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#define MIPS_SPECIFIC


#ifdef COUNT_REALREGS
int RealRegArg::_numRealRegs = 0;	
#endif

RealRegArg::RealRegArg(char *regName, int regNum, int regFile) : _regFile(regFile)
{
	_symVal = regNum;

	// Purify correction : 08/07/01
	
	if(regName != NULL)
	{
		_regText = strdup(regName);
	}
	else
	{
		_regText = NULL;
	}
	
#ifdef COUNT_REALREGS
	_numRealRegs++;
#endif
}

RealRegArg::~RealRegArg()
{
	//if (_regText!=NULL)
			// delete _regText;
	// Note that we are not deleting _propertyPtr. To free space,
	// deleteProperty will have to be called explicitly.
#ifdef COUNT_REALREGS
	_numRealRegs--;
#endif
} 

BaseArgument *RealRegArg::copy()
{
	BaseArgument *retPtr;
	LinkedListIterator<BaseArgProperty> *propListIterator;
	
	retPtr = new RealRegArg(_regText, _symVal, _regFile);
	propListIterator = propIterForw();
	while (propListIterator->hasMoreElements()) {
		retPtr->addProperty(propListIterator->currentElement());
		propListIterator->nextElement();
	}
	delete propListIterator;
	
	return retPtr;
}

BaseArgument *RealRegArg::copyBody()
{
	BaseArgument *retPtr;
	
	retPtr = new RealRegArg(_regText, _symVal, _regFile);
	
	
	MyLinkedList<PropertyNames> propList;
	PropertyNames a(_SSA_);
	
	propList.append(a);
	
	a = _UDCHAIN_;
	propList.append(a);
	
	a = _DUCHAIN_;
	propList.append(a);
	
	(retPtr->getPropertyList()).makeACopyOf(_propertyPtr, propList);
	
	return retPtr;
}

int RealRegArg::isEq(BaseArgument *arg)
{
	if ((arg != NULL) && (arg->isRealRegArg()))
	{
		if (arg->getUniqID() == getUniqID())
			return _YES_;
	}
	return _NO_;
}

/*
int RealRegArg::isEq(BaseArgument *arg)
{
	if ((arg != NULL) && (arg->isRealRegArg()))
	{
		if ((strcmp(((RealRegArg *)arg)->_regText, _regText) == 0)
			&& (((RealRegArg *)arg)->_symVal == _symVal)
			&& (((RealRegArg *)arg)->_regFile == _regFile))
		{
			
			// As of now, if the two real regs do not have the same
			// register numbers, they are considered to be different hard
			// regs. (assuming they are in the same register file).
			// 
			// If sharing of hard regs is allowed (like in sliding
			// window), then this may require changes.
			//
			
			BaseArgProperty *prop1,*prop2;
			prop1=getPtrToProperty(_SSA_);
			prop2=arg->getPtrToProperty(_SSA_);
			if(prop1==NULL && prop2==NULL) return _YES_;
			if(prop1==NULL || prop2==NULL) return _NO_;
			if(((SSAArgProperty*)prop1)->getSSANum() == ((SSAArgProperty*)prop2)->getSSANum())
				return _YES_;
			else
				return _NO_;
		}
	}
	return _NO_;
}
*/

char *RealRegArg::Name(void)
{
	
	char *buffer;
	buffer=new char[strlen(regFilename())+20];
	sprintf(buffer,"%s%d",regFilename(),regNumber());
	
	return buffer;
}

int RealRegArg::getValType()
{
	return (((*globalRegFileList)[_regFile]).valType());
}
/*
SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
if (ssaP!=NULL)
sprintf(buffer,"%s%d_%d",regFilename(),regNumber(),ssaP->getSSANum());
else 
sprintf(buffer,"%s%d",regFilename(),regNumber());
return buffer;

  char *buffer;
  buffer = new char[strlen(regFilename())+20];
  SSAArgProperty *ssaP = (SSAArgProperty *)getPtrToProperty(_SSA_);
  if (ssaP!=NULL)
  sprintf(buffer,"%s%d_%d",regFilename(),regNumber(),ssaP->getSSANum());
  else 
		sprintf(buffer,"%s%d",regFilename(),regNumber());
		return buffer;
		}
*/

int RealRegArg::getIDWithoutSSA()
{
	if (doneRA)
	{
		RAProperty *raProp = (RAProperty *)getPtrToProperty(_RA_);
		if (raProp != NULL)
			return raProp->getRegID();
	}
	return ((globalRegFileList->getBaseID(_regFile)) + _symVal);
}

int RealRegArg::getUniqID()
{
	if ((doneSSA) && (!doneRA))
		return (globalSymbolTable->getID(this, getPtrToProperty(_SSA_)));
	else
		return getIDWithoutSSA();
}

//used by print(indent);
void RealRegArg::print()
{
	RegisterArg::print();
	//	 cout << " RF: " << _regFile << " ";
}

//used by printC(indent);
void RealRegArg::printC()
{
	RegisterArg::printC();
}

//used by buildSSA (don't modify). 
void RealRegArg::print(ostream& out)
{
	RegisterArg::print(out);
	//	 cout << " RF: " << _regFile << " ";
}

void RealRegArg::printC(ostream& out)
{
	RegisterArg::printC(out);
	//	 cout << " RF: " << _regFile << " ";
}

void RealRegArg::changeReg(char *rName, int rNum, int rFile)
{
	   _regText=strdup(rName);
	   _symVal = rNum;
	   _regFile = rFile;
	   return;
}

void RealRegArg::changeReg(BaseArgument *arg)
{
	if (arg->isRealRegArg() == _NO_) return;

	_regText=strdup(((RealRegArg *)arg)->_regText);
	_symVal = ((RealRegArg *)arg)->_symVal;
	_regFile = ((RealRegArg *)arg)->_regFile;
}

void addUniqArgToArgList(ArgList & aList, BaseArgument *arg)
{
	// iterate over aList.
	ArgListIter ai;

	for (ai = aList.begin(); ai != aList.end(); ai++)
	{
		if (arg->isEq(*ai))
			return;
	}
	aList.push_back(arg);
}

// ---------------------------------------------------------------------------------
//
// Use the function:
//
// int isTargetRegisterFromRF(BaseArgument *arg, char *rfName)
//
// defined in compRegClass.cpp
//
// in order to determine the type (or RF) of an argument after Register Allocation.
// (It looks at the RAProperty and then returns _YES_ or _NO_)
//
// ---------------------------------------------------------------------------------

int RealRegArg::isIntegerReg()
{
	if ((!doneRA) && (strcmp(_regText, "gR") == 0))
	{
		return _YES_;
	}
	else if (doneRA)
	{
//		cout << "Warning: Target register mapping for f is not yet defined !" << endl;
		
		RAProperty *raProp = (RAProperty *) this->getPtrToProperty(_RA_);
		int regID = raProp->getRegID();
		RegisterFile& regFile = globalRegFileList->getRegFile(regID);
		
		return (strcmp(regFile.name(), "R") == 0);
	}
	return _NO_;
}
   
int RealRegArg::isFloatReg()
{
	if ((!doneRA) && (strcmp(_regText, "gf") == 0))
	{
		return _YES_;
	}
	else if (doneRA)
	{
//		cout << "Warning: Target register mapping for f is not yet defined !" << endl;
		
		RAProperty *raProp = (RAProperty *) this->getPtrToProperty(_RA_);
		int regID = raProp->getRegID();
		RegisterFile& regFile = globalRegFileList->getRegFile(regID);
		
		return (strcmp(regFile.name(), "f") == 0);
	}
	return _NO_;
}
   

int isReturnParameter(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;
	if (!arg->isRegisterArg()) return _NO_;
	
	// Currently, an argument is a return parameter if
	// it is derived from register R4 or f0
	
	if( !doneRA )
	{
		if (((RegisterArg *)arg)->getIDWithoutSSA() == globalRegFileList->getID(4, "gR")) return _YES_;
		if (((RegisterArg *)arg)->getIDWithoutSSA() == globalRegFileList->getID(0, "gf")) return _YES_;
	}
	else
	{
		// Note: The following piece of code is correct (becos it is hardcoded) only for MIPS architectures.
		//
		// For other architectures, need to get the mapping of R4 to target register set and then see if this arg belongs to that set.
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (((RegisterArg *)arg)->getIDWithoutSSA() == globalRegFileList->getID(4, "R")) return _YES_;
		if (((RegisterArg *)arg)->getIDWithoutSSA() == globalRegFileList->getID(0, "f")) return _YES_;
	}

	
	return _NO_;
}

int isGenericArchDefinedArg(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;
	if (!arg->isRegisterArg()) return _NO_;

	// Currently, an argument is a Register with a valid value if it is R0, sp, or R31, or a call parameter.
	//
	// Basically, as defined by the Generic Architecture Set, these registers always have a valid value
	// even if they are never defined anywhere in the program.
	//
	// Currently, an argument is a return parameter if
	// it is derived from register R4 or f0

	int regId = ((RegisterArg *)arg)->getIDWithoutSSA(); 
	
	if( !doneRA )
	{
		if (regId == globalRegFileList->getID(0, "gR")) return _YES_;
		if (regId == globalRegFileList->getID(0, "gsp")) return _YES_;
	}
	else
	   {
		// See note for "isReturnParameter"
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (regId == globalRegFileList->getID(0, "R")) return _YES_;
		if (regId == globalRegFileList->getID(0, "sp")) return _YES_;
	}


	int callParmStart = 4;
	int callParmEnd = 10;
	
	for (int i = callParmStart; i < callParmEnd; i++)
	{
		if( !doneRA )
		{
			if (regId == globalRegFileList->getID(i, "gR")) return _YES_;
		}
		else
		{
			// See note for "isReturnParameter"
			//
#ifndef MIPS_SPECIFIC
			cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
			if (regId == globalRegFileList->getID(i, "R")) return _YES_;
		}

	}

	// take care of f0 too.
	//

	if( !doneRA )
	{
		if (regId == globalRegFileList->getID(0, "gf")) return _YES_;
	}
	else
	{
		// See note for "isReturnParameter"
		//
		cout << "Warning: Target Register set is not yet specified !" << endl;
		if (regId == globalRegFileList->getID(0, "f")) return _YES_;
	}
	
	
	return _NO_;
}

int isSPArg(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;
	if (!arg->isRegisterArg()) return _NO_;

	int regId = ((RegisterArg *)arg)->getIDWithoutSSA();
	
	if( !doneRA )
	{
		if (regId == globalRegFileList->getID(0, "gsp")) return _YES_;
	}
	else
	{
		// See note for "isReturnParameter"
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (regId == globalRegFileList->getID(0, "sp")) return _YES_;
	}


	return _NO_;
}

int isFPArg(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;
	if (!arg->isRegisterArg()) return _NO_;
	
	int regId = ((RegisterArg *)arg)->getIDWithoutSSA();
	if( !doneRA )
	{
		if (regId == globalRegFileList->getID(0, "gfp")) return _YES_;
	}
	else
	{
		// See note for "isReturnParameter"
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (regId == globalRegFileList->getID(0, "fp")) return _YES_;
	}
	
	return _NO_;
}

int isPCArg(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;
	if (!arg->isRegisterArg()) return _NO_;

	// Note: As of now (08/13/01), we do not have an explicit pc register

/*
	int regId = ((RegisterArg *)arg)->getIDWithoutSSA();
	if (regId == globalRegFileList->getID(0, "pc")) return _YES_;
*/

	return _NO_;
}

int isCCArg(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;
	if (!arg->isRegisterArg()) return _NO_;

	int regId = ((RegisterArg *)arg)->getIDWithoutSSA();
	if( !doneRA )
	{
		if (regId == globalRegFileList->getID(0, "gcc")) return _YES_;
	}
	else
	{
		// See note for "isReturnParameter"
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (regId == globalRegFileList->getID(0, "cc")) return _YES_;
	}


	return _NO_;
}

int isHILOArg(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;
	if (!arg->isRegisterArg()) return _NO_;
	
	int regId = ((RegisterArg *)arg)->getIDWithoutSSA();
	if( !doneRA )
	{
		if (regId == globalRegFileList->getID(0, "ghilo")) return _YES_;
	}
	else
	{
		// See note for "isReturnParameter"
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (regId == globalRegFileList->getID(0, "hilo")) return _YES_;
	}
//	if (regId == globalRegFileList->getID(1, "hilo")) return _YES_;

	return _NO_;
}

// For generic architectures, R0 is hardwired to zero.
//
int isHardwiredZero(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;
	if (!arg->isRegisterArg()) return _NO_;

	int regId = ((RegisterArg *)arg)->getIDWithoutSSA(); 
	if( !doneRA )
	{
		if (regId == globalRegFileList->getID(0, "gR")) return _YES_;
	}
	else
	{
		// See note for "isReturnParameter"
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (regId == globalRegFileList->getID(0, "R")) return _YES_;
	}

	return _NO_;
}

// Returns yes if arg is an immediate argument, or if it is an architecture defined argument.
//
int isImmediateLikeArg(BaseArgument *arg)
{
	if (arg == NULL) return _NO_;

	if (arg->isImmediateArg()) return _YES_;

	if (isGenericArchDefinedArg(arg)) return _YES_;

	return _NO_;
}

extern RealRegArg globalHI;
extern RealRegArg globalLO;

BaseArgument *getHI(BaseArgument *arg)
{
	if (!arg->isRegisterArg()) return NULL;
	
	if( !doneRA )
	{
		if (((RegisterArg *)arg)->getIDWithoutSSA() != globalRegFileList->getID(0, "ghilo"))
			return NULL;
	}
	else
	{
		// See note for "isReturnParameter"
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (((RegisterArg *)arg)->getIDWithoutSSA() != globalRegFileList->getID(0, "hilo"))
			return NULL;
	}


	return (BaseArgument *)(&globalHI);
}

BaseArgument *getLO(BaseArgument *arg)
{
	if (!arg->isRegisterArg()) return NULL;
	
	if( !doneRA )
	{
		if (((RegisterArg *)arg)->getIDWithoutSSA() != globalRegFileList->getID(0, "ghilo"))
			return NULL;
	}
	else
	{
		// See note for "isReturnParameter"
		//
#ifndef MIPS_SPECIFIC
		cout << "Warning: Target Register set is not yet specified !" << endl;
#endif
		if (((RegisterArg *)arg)->getIDWithoutSSA() != globalRegFileList->getID(0, "hilo"))
			return NULL;
	}

	return (BaseArgument *)(&globalLO);
}

int consecutiveArgs(BaseArgument *arg1, BaseArgument *arg2)
{
	if ((arg1== NULL) || (arg2 == NULL)) return _INVALID_;

	// first, check to see if both of them have the same reg file name.
	//
	if ((!arg1->isRealRegArg()) || (!arg2->isRealRegArg())) return _INVALID_;

	if (((RealRegArg *)arg1)->regFileIndex() != ((RealRegArg *)arg2)->regFileIndex()) return _NO_;

	// get IDs of both without SSA.
	//
	int id1 = arg1->getIDWithoutSSA();
	int id2 = arg2->getIDWithoutSSA();

	if (id1 + 1 == id2) return _YES_;

	return _NO_;
}

// Look in NoGUIMain.cpp for the initialization of TEMP_CREATION_NUM
//
int TEMP_CREATION_NUM;
int RISA_CREATION_NUM = 33;

// This function creates a real reg arg with "temp" RF.
//
// The actual arg provides the number in temp<number>.
// If actArg == NULL, then the number is derived from 33 onwards.
//
BaseArgument *createTempArg(BaseArgument *actArg = NULL)
{
	int num = TEMP_CREATION_NUM;
	int rfNum;
	int regType = _TEMP_;
	BaseArgument *retVal;

	if ((actArg != NULL) && (actArg->isRegisterArg() == _YES_))
	{
		num = ((RegisterArg *)actArg)->getVal();

	}
	else
	{
		TEMP_CREATION_NUM++;
	}
	
	rfNum = globalRegFileList->getIndex("temp");
	retVal = new RealRegArg("temp", num, rfNum);
	
	SSAArgProperty tempProp(1);
	BaseArgProperty *ssaProp = & (tempProp);
	if ((actArg != NULL) && (doneSSA && (!doneRA)))
	{
		ssaProp = actArg->getPtrToProperty(_SSA_);
	}
	retVal->addProperty(*ssaProp);

	return retVal;
}

// This function creates a real reg arg with "risa" RF.
//
// The actual arg provides the number in risa<number>.
// If actArg == NULL, then the number is derived from 33 onwards.
//
BaseArgument *createRISAArg(BaseArgument *actArg = NULL)
{
	int num = RISA_CREATION_NUM;
	int rfNum;
	int regType = _RISA_;
	BaseArgument *retVal;

	if ((actArg != NULL) && (actArg->isRegisterArg() == _YES_))
	{
		num = ((RegisterArg *)actArg)->getVal();
	}
	else
	{
		RISA_CREATION_NUM++;
	}
	
	rfNum = globalRegFileList->getIndex("risa");
	retVal = new RealRegArg("risa", num, rfNum);
	
	return retVal;
}

/*
// This function creates a real reg arg with "temp" RF.
//
// The actual arg provides the number in temp<number>.
// If actArg == NULL, then the number is derived from 33 onwards.
//
BaseArgument *createTempArg(BaseArgument *actArg = NULL)
{
	int num = TEMP_CREATION_NUM;

	if ((actArg != NULL) && (actArg->isRegisterArg() == _YES_))
		num = ((RegisterArg *)actArg)->getVal();
	else
		TEMP_CREATION_NUM++;

	int rfNum = globalRegFileList->getIndex("temp");

	BaseArgument *retVal;
	retVal = new RealRegArg("temp", num, rfNum);

	return retVal;
}

// This function creates a real reg arg with "risa" RF.
//
// The actual arg provides the number in risa<number>.
// If actArg == NULL, then the number is derived from 33 onwards.
//
BaseArgument *createRISAArg(BaseArgument *actArg = NULL)
{
	int num = RISA_CREATION_NUM;

	if ((actArg != NULL) && (actArg->isRegisterArg() == _YES_))
		num = ((RegisterArg *)actArg)->getVal();
	else
		RISA_CREATION_NUM++;

	int rfNum = globalRegFileList->getIndex("risa");

	BaseArgument *retVal;
	retVal = new RealRegArg("risa", num, rfNum);

	return retVal;
}

*/