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
// File:  		RegArchModel.h
// Created:		Mon Jan 11, 99
// Last modified: 	Mon Jan 11, 99
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include

#ifndef __REGMODEL_H__
#define __REGMODEL_H__

#include "GlobalIncludes.h"
#include "STLIncludes.h"
#include "MyString.h"
#include "BaseArgument.h"
#include "SetList.h"
#include "MyLinkedListIterator.h"

// This RegArchModel class contains the register architecture model.
//
//
// The following are its components:
//
// 1) Generic Register Classes
// 2) Target Register Classes
// 3) Generic Classes to Registers mapping
// 4) Target Classes to Registers mapping
// 5) Register Classes to Register Classes mapping
//    a) Generic Register Classes to Target Register Classes
//    b) Target Register Classes to Target Register Classes (??)
//    c) Generic Register Classes to Generic Register Classes (??)
//
// The following are classes that define the individual components:
//
// 1) RegClassModel
// 2) RegClassToRegsMapping
// 3) RegClassToRegClassMapping
//
//
class RegClassModel
{
	// consists of two parts:
	// 1) Data types
	// 2) Class types

private:

	int _numDataTypes;
	int _numClassTypes;

	Vector<MyString> _dataTypes;
	Vector<MyString> _classTypes;

public:
	RegClassModel() : _numDataTypes(0), _numClassTypes(0) {}
	~RegClassModel() { }

	void addNumDataTypes(int num)
	{
		_numDataTypes += num;
	}

	void addNumClassTypes(int num)
	{
		_numClassTypes += num;
	}

	int getNumDataTypes() { return _numDataTypes; }
	int getNumClassTypes() { return _numClassTypes; }

	void addDataType(char *str);
	void addClassType(char *str);

	// Get the position of the DataType in the "_dataTypes" array.
	//
	int getDataTypeIndex(char *str);

	// Get the position of the ClassType in the "_classTypes" array.
	//
	int getClassTypeIndex(char *str);
};

// we use the following scheme to order two RegClassEntryNode2 objects.
//
// Assume that the number of Data types or Class types or RF Names is always less than 100.
// 
// Calculate value of RegClassEntryNode2 object "obj" as 10000*(obj._dType) + 100*(obj._cType) + (obj._rfName);
//
// This value can be used to consistently compare for less than.
//
class RegClassEntryNode1
{
public:
	int _dType;
	int _cType;
	int _rfName;

	RegClassEntryNode1() : _dType(_INVALID_), _cType(_INVALID_), _rfName(_INVALID_) {}
	RegClassEntryNode1(int dT, int cT, int rF) : _dType(dT), _cType(cT), _rfName(rF) {} 
	bool operator < (const RegClassEntryNode1 & b) const
	{
		int val1 = 10000*_dType + 100*_cType + _rfName;
		int val2 = 10000*(b._dType) + 100*(b._cType) + (b._rfName);

		if (val1 < val2) return true;

		return false;
	}
	bool operator == (const RegClassEntryNode1 & b) const
	{
		if ((_dType == b._dType) && (_cType == b._cType) && (_rfName == b._rfName))
			return true;
		return false;
	}
	void print(ostream & out) const
	{
		out << " D: " << _dType << "  C: " << _cType << "  R: " << _rfName << " ";
	}
};

typedef Vector<RegClassEntryNode1> RegClassEntryNode1List;
typedef Vector<RegClassEntryNode1>::iterator RegClassEntryNode1ListIter;

// we use the following scheme to order two RegClassEntryNode2 objects.
//
// Assume that the number of Data types or Class types is always less than 100.
// 
// Calculate value of RegClassEntryNode2 object "obj" as 100*(obj._dType) + (obj._cType);
//
// This value can be used to consistently compare for less than.
//
class RegClassEntryNode2
{
public:
	int _dType;
	int _cType;

	RegClassEntryNode2() : _dType(_INVALID_), _cType(_INVALID_){}
	RegClassEntryNode2(int dT, int cT) : _dType(dT), _cType(cT){}
	bool operator < (const RegClassEntryNode2 & b) const
	{
		int val1 = 100*_dType + _cType;
		int val2 = 100*(b._dType) + (b._cType);

		if (val1 < val2) return true;

		return false;
	}
	bool operator == (const RegClassEntryNode2 & b) const
	{
		if ((_dType == b._dType) && (_cType == b._cType))
			return true;
		return false;
	}
	void print(ostream & out) const
	{
		out << " D: " << _dType << "  C: " << _cType << " ";
	}
};

typedef Vector<RegClassEntryNode2> RegClassEntryNode2List;
typedef Vector<RegClassEntryNode2>::iterator RegClassEntryNode2ListIter;

typedef Vector<int> RegNums;

class RegNumList
{
public:
	int _rfName;
	RegNums _rList;
};

class RegClassToRegsMapping
{
private:
	int _numIndices;
	RegClassEntryNode1List _regClassIndices;
	Map<int, RegNums> _regRange;

	inline void _addNums(Vector<int> & vect, int startNum, int endNum);

	inline int _hasNum(Vector<int> & vect, int num);

public:
	RegClassToRegsMapping() : _numIndices(0) {}
	~RegClassToRegsMapping() {}

	int getNumIndices() { return _numIndices; }

	Vector<RegClassEntryNode1> & getRegClassIndices() {  return _regClassIndices; }

	int initRegClassToRegsMapping(int dType, int cType, int rfName);

	int getIndexForRegClassToRegsMapping(int dType, int cType, int rfName);

	// This function returns the first match (without considering rfName).
	//
	int getIndexForRegClassToRegsMapping(int dType, int cType);

	// This function fills the vector with all matches.
	void getAllIndicesForRegClassToRegsMapping(int dType, int cType, Vector<int> & indices);
	void getAllIndicesForRegClassToRegsMapping(RegClassEntryNode2 & ct, Vector<int> & indices);

	void addRegClassToRegsMapping(int index, char *regNums);

	int getRFForRegClass(int index) { return (_regClassIndices[index])._rfName; }

	Vector<int> & getRegsForRegClass(int index) { return _regRange[index]; }

	int hasReg(int index, int regNum);
	int hasReg(int dType, int cType, int rfName, int regNum);

	void setAllClassTypesForReg(int rfName, int regNum, Vector<RegClassEntryNode2> & holder);

	void setListOfUniqClasses(Vector<RegClassEntryNode2> & uniqClasses);
};

// This class contains a map of indices (from one regclass set) to indices (from another regclass set)
//
class RegClassToRegClassMapping
{
private:
	Map<int, int> _regClassToRegClass;

public:
	RegClassToRegClassMapping() {}
	~RegClassToRegClassMapping() {}

	void addMapping(int fromIndex, int toIndex) { _regClassToRegClass[fromIndex] = toIndex; }

	int getMapping(int index);
};

class RegClassRegClassType
{
public:
	int _genericOrTarget1;
	int _genericOrTarget2;

	bool operator < (const RegClassRegClassType & b) const
	{
		if ((_genericOrTarget1 == b._genericOrTarget1) && (_genericOrTarget2 == b._genericOrTarget2))
			return false;

		return true;
	}
};

class RegArchModel
{
private:
	RegClassModel _genericClasses;
	RegClassModel _targetClasses;

	RegClassToRegsMapping _genericRegClassToRegs;
	RegClassToRegsMapping _targetRegClassToRegs;

	Map<RegClassRegClassType, RegClassToRegClassMapping> _regClassToRegClass;


	Map<RegClassEntryNode2, RegClassEntryNode2List> _compatibilityArray;

public:
	RegArchModel(){}
	~RegArchModel(){}

	void addNumDataTypes(int num, int genericOrTarget);
	void addNumClassTypes(int num, int genericOrTarget);

	void addDataType(char *word, int genericOrTarget);
	void addClassType(char *word, int genericOrTarget);

	int getDataTypeIndex(char *word, int genericOrTarget);
	int getClassTypeIndex(char *word, int genericOrTarget);

	int initRegClassToRegsMapping(int dType, int cType, int rfType, int genericOrTarget);
	int initRegClassToRegsMapping(char *dataType, char *classType, char *rfName, int genericOrTarget);

	int getIndexForRegClassToRegsMapping(int dType, int cType, int rfType, int genericOrTarget);
	int getIndexForRegClassToRegsMapping
		(char *dataType, char *classType, char *rfName, int genericOrTarget);

	int getIndexForRegClassToRegsMapping(int dType, int cType, int genericOrTarget);
	int getIndexForRegClassToRegsMapping(char *dataType, char *classType, int genericOrTarget);

	void getAllIndicesForRegClassToRegsMapping
		(int dType, int cType, int genericOrTarget, Vector<int> & indices);

	void getAllIndicesForRegClassToRegsMapping
		(RegClassEntryNode2 & ct, int genericOrTarget, Vector<int> & indices);

	void addRegClassToRegsMapping(int index, char *regNums, int genericOrTarget);

	int getRFForRegClass(int index, int genericOrTarget);
	Vector<int> & getRegsForRegClass(int index, int genericOrTarget);

	void setListOfUniqClasses(RegClassEntryNode2List & uniqClasses, int genericOrTarget);

	void addRegClassToRegClassMappings
		(char *keyword1, char *dataType1, char *classType1, 
		char *keyword2, char *dataType2, char *classType2);

	int getToMapping(int genericOrTarget1, int index1, int genericOrTarget2);
	int getRFToMapping(int genericOrTarget1, int index1, int genericOrTarget2);
	int getFirstRFToMapping(int genericOrTarget1, int dType1, int cType1, int genericOrTarget2);
	int getFirstRFToMapping(int genericOrTarget1, RegClassEntryNode2 & ct, int genericOrTarget2);
	Vector<int> & getRegsToMapping(int genericOrTarget1, int index1, int genericOrTarget2);

	Vector<int> & getFirstRegsToMapping
		(int genericOrTarget1, int dType1, int cType1, int genericOrTarget2);

	Vector<int> & getFirstRegsToMapping
		(int genericOrTarget1, RegClassEntryNode2 & ct, int genericOrTarget2);

	void getAllRegsToMapping
		(int genericOrTarget1, int dType1, int cType1, 
		int genericOrTarget2, Vector<RegNumList> & rList);

	void getAllRegsToMapping
		(int genericOrTarget1, RegClassEntryNode2 & ct, 
		int genericOrTarget2, Vector<RegNumList> & rList);

	int isEqualClassSets
		(Vector<RegClassEntryNode2> & cls1, Vector<RegClassEntryNode2> & cls2);
	
	void intersectRegisterSets 
		(Vector<RegNumList> & into, Vector<RegNumList> & from);

	void getRegsThatSatisfyAllMappings
		(int genericOrTarget1, RegClassEntryNode2 ct1, RegClassEntryNode2 ct2,
		int genericOrTarget2, Vector<RegNumList> & rList);

	void getRegsThatSatisfyAllMappings
		(int genericOrTarget1, RegClassEntryNode2 ct1, RegClassEntryNode2 ct2, 
		int genericOrTarget2, Vector<int> & regIndexList);

	void getRegsThatSatisfyAllMappings
		(int genericOrTarget1, Vector<RegClassEntryNode2> & ctList, 
		int genericOrTarget2, Vector<RegNumList> & rList);

	void getRegsThatSatisfyAllMappings
		(int genericOrTarget1, Vector<RegClassEntryNode2> & ctList, 
		int genericOrTarget2, Vector<int> & regIndexList);

	void getAllClassTypesForReg
		(BaseArgument *arg, Vector<RegClassEntryNode2> & holder, int genericOrTarget);

	// The next set of methods are defined in CompRegClass.cpp
	//
	void initGlobalIndexVariables(int genericOrTarget);
	void setRegClassPropertyForArg(BaseArgument *arg, BaseOperation *op);

	// returns the number of additions that were made to the vector mergeInto.
	//
	// elements that were discarded becos they were already present in mergeInto do not count as additions.
	//
	int mergeListsOfRegClassEntryNode2
		(Vector<RegClassEntryNode2> & mergeInto, Vector<RegClassEntryNode2> & mergeFrom, 
		int genericOrTarget);

	void createListOfClassTypesForArgs
		(ArgList & aList, Vector<RegClassEntryNode2> & vect, int genericOrTarget);

	void createListOfClassTypesForArgs
		(SetList<BaseArgument *> *args, Vector<RegClassEntryNode2> & vect, int genericOrTarget);


	// sets the compatibility of classes in genericOrTarget1, based on its mapping to classes (and registers)
	// of genericOrTarget2.
	// Note: A <classtype, datatype> tuple is compatible with another <classtype, datatype> tuple
	// if they have atleast one register (from genericOrTarget2) in common.
	//
	void setCompatibilityArray(int genericOrTarget1, int genericOrTarget2);

	// returns compatibility based on the compatibility array set by the method written above.
	//
	int isCompatible(RegClassEntryNode2 en1, RegClassEntryNode2 en2);

	// returns compatibility based on the compatibility array set by the method written above.
	//
	// returns yes if there is atleast 1 register that satisfies all the classes in both vectors.
	//
	int isCompatible
		(Vector<RegClassEntryNode2> & ven1, Vector<RegClassEntryNode2> & ven2);

	//
	// end methods defined in CompRegClass.cpp

	void printCompatibilityArray(ostream & out);
};

// These functions are defined in CompRegClass.cpp
//
extern int isDoubleArg(BaseArgument *arg, BaseOperation *op);
extern int isDouble1Arg(BaseArgument *arg, BaseOperation *op);
extern int isDouble2Arg(BaseArgument *arg, BaseOperation *op);
extern int isFloatArg(BaseArgument *arg, BaseOperation *op);
extern int isIntArg(BaseArgument *arg, BaseOperation *op);
extern int isCallParmArg(BaseArgument *arg, BaseOperation *op);
extern int isRetValArg(BaseArgument *arg, BaseOperation *op);
extern int isSPArg(BaseArgument *arg, BaseOperation *op);
extern int isFPArg(BaseArgument *arg, BaseOperation *op);
extern int isCCArg(BaseArgument *arg, BaseOperation *op);
extern int isPCArg(BaseArgument *arg, BaseOperation *op);
extern int isHWZeroArg(BaseArgument *arg, BaseOperation *op);
extern int isRetAddrArg(BaseArgument *arg, BaseOperation *op);
extern int isHILOArg(BaseArgument *arg, BaseOperation *op);
extern int isNormalArg(BaseArgument *arg, BaseOperation *op);

extern void setDataTypeInRegClassEntryNode2ForArg
(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op);

extern void setClassTypeInRegClassEntryNode2ForArg
(RegClassEntryNode2 & rcen, BaseArgument *arg, BaseOperation *op);
//
// end functions defined in CompRegClass.cpp

#endif
