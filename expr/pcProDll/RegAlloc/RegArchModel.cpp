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
// File:  		RegArchModel.cpp
// Created:		Fri Jul 27, 01
// Last modified: 	Fri Jul 27, 01
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: SUPPORT

#include "stdafx.h"
#include "RegArchModel.h"

#include "BaseArgument.h"
#include "RegisterFileList.h"
#include "RegClassArgProperty.h"

#include <string.h>


//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern RegisterFileList *globalRegFileList;
RegArchModel globalRegModel;


// Generic Reg Class Types:
//
// These are guaranteed to be always supported in EXPRESS
//
// Integer, Float, Double
//
// Normal, Parm, RetVal
//
// Zero, CC, SP, FP, PC, RetAddr, HILO
//
// Note: These register classes are not mutually exclusive.

// Target/Optimization Reg Class Types:
//
// These may be specific to a target, or an optimization.
// These classes are defined and (created/set) by that particular optimization (or feature).
//
// Examples include: 
//
// RISA, TEMP, SIMD (??)


// Input: Argument, [register class type(s)]
// 
// Function: Set it to a particular register class type.
// If the second argument is specified, just add it, else derive the 
// register class type(s) based on the argument.
//
// Note: It might belong to a set of register classes. 
//
void setRegClassType(BaseArgument *arg, int genericOrTarget)
{
	Vector<RegClassEntryNode2> classList;

	globalRegModel.getAllClassTypesForReg(arg, classList, genericOrTarget);

	// TBD.
	assert(0);
}

// Input: Argument
//
// Function: Get its assigned register class type(s).
//
void getRegClassType(BaseArgument *arg, Vector<RegClassEntryNode2> & holder, int genericOrTarget)
{
	// TBD.
	// check if the arg is of type: 
	// cc, sp, fp, call_parm, ret_val, ret_addr, pc, risa, normal, etc.
	//

	globalRegModel.getAllClassTypesForReg(arg, holder, genericOrTarget);
}

// Input: Argument, [register class type(s)]
//
// Function: Change its assigned register class type(s)
// If the second argument is specified, just add it; else derive the new
// register class type(s) based on the argument.
//
void changeRegClassType()
{
}

int getTargetRegFile(int genericOrTarget1, int dType1, int cType1, int genericOrTarget2)
{
	return globalRegModel.getFirstRFToMapping(genericOrTarget1, dType1, cType1, genericOrTarget2);
}

int getTargetRegFile(int genericOrTarget1, RegClassEntryNode2 & ct, int genericOrTarget2)
{
	return globalRegModel.getFirstRFToMapping(genericOrTarget1, ct._dType, ct._cType, genericOrTarget2);
}

// Input: register class type(s)
//
// Function: Return the target registers that satisfy the register class type(s)
//
Vector<int> & getTargetRegList(int genericOrTarget1, int dType1, int cType1, int genericOrTarget2)
{
	return globalRegModel.getFirstRegsToMapping(genericOrTarget1, dType1, cType1, genericOrTarget2);
}

// Reads a file, and set the list of all register class types from that file.
//
// Use this function to read in both the generic supported register classes and the 
// target supported register class types.
//
void setRegisterClassTypes(MyString & fileName, int genericOrTarget)
{
	FilePointer fp;
	
	try
	{
		fp.open(fileName, "r");
	}
	catch (CouldNotOpenFileException e)
	{
		cout << e;
		assert(0);
	}

	char word[20];
	int numDataTypes = 0;

	// The first line in the file should contain the words "DATA_TYPES"
	//
	fscanf(fp, "%s %d\n", word, &numDataTypes);

	if (strcmp("DATA_TYPES", word) != 0) // error
	{
		// huh? The file is not in the proper format.
		//
		cout << "Panic: While setting RegisterClassTypes: ";
		fileName.print(cout);
		cout << " Not in proper format" << endl;
		assert(0);
	}
	
	globalRegModel.addNumDataTypes(numDataTypes, genericOrTarget);

	// The next numDataTypes lines contain the data types.
	//
	for (int i = 0; i < numDataTypes; i++)
	{
		fscanf(fp, "%s\n", word);
		globalRegModel.addDataType(word, genericOrTarget);
	}

	int numClassTypes = 0;

	// The line following the data types should contain the words "CLASS_TYPES"
	//
	fscanf(fp, "%s %d\n", word, &numClassTypes);

	if (strcmp("CLASS_TYPES", word) != 0) // error
	{
		// huh? The file is not in the proper format.
		//
		cout << "Panic: While setting RegisterClassTypes: ";
		fileName.print(cout);
		cout << " Not in proper format" << endl;
		assert(0);
	}
	
	globalRegModel.addNumClassTypes(numClassTypes, genericOrTarget);

	// The next numDataTypes lines contain the data types.
	//
	for (int j = 0; j < numClassTypes; j++)
	{
		fscanf(fp, "%s\n", word);
		globalRegModel.addClassType(word, genericOrTarget);
	}
	
	fp.close();
}

// Read the register mappings from a file, and store it in a global data structure.
//
// For each register class, these mappings indicate the virtual or physical registers that
// satisfy it.
//
// The format of each line is:
//
// (DATA_TYPE <class-type>) (CLASS_TYPE <data-type>) (REG_FILE <reg-file-name>) (REGS <reg-list>)
//
// <class-type> and <data-type> are present in the regclasstypes DS.
// <reg-file-name> can either be virtual (generic) or real (target).
// GENERIC reg-file-name:
//		R, cc, sp, fp, f, temp
// TARGET reg-file-name depends on the architecture
//
// <reg-list> can be a space separated list of regs.
// regs can be a single register number, or reg-reg (e.g. 0-31)
//
void setRegClassToRegsMappings(MyString & fileName, int genericOrTarget)
{
	FilePointer fp;
	
	try
	{
		fp.open(fileName, "r");
	}
	catch (CouldNotOpenFileException e)
	{
		cout << e;
		assert(0);
	}

	char line[200];
	char *word, *datatype, *classtype, *regfilename;

	while (fgets(line, 200, fp) != NULL)
	{
		// Get the Keyword "DATA_TYPE"

		// Remove the first '('
		strtok(line, "(");
		word = strtok(NULL, " ");
		if (strcmp(word, "DATA_TYPE") != 0) // error condition.
		{
			// huh? The file is not in the proper format.
			//
			cout << "Panic: While setting RegClassToRegsMappings: ";
			fileName.print(cout);
			cout << " Not in proper format" << endl;
			assert(0);
		}

		// The next token will be the data type.
		//
		datatype = strtok(NULL, ")");

		// Get the Keyword "CLASS_TYPE"

		// Remove the first '('
		strtok(NULL, "(");
		word = strtok(NULL, " ");
		if (strcmp(word, "CLASS_TYPE") != 0) // error condition.
		{
			// huh? The file is not in the proper format.
			//
			cout << "Panic: While setting RegClassToRegsMappings: ";
			fileName.print(cout);
			cout << " Not in proper format" << endl;
			assert(0);
		}

		// The next token will be the class type.
		//
		classtype = strtok(NULL, ")");

		// Get the Keyword "REG_FILE"

		// Remove the first '('
		strtok(NULL, "(");
		word = strtok(NULL, " ");
		if (strcmp(word, "REG_FILE") != 0) // error condition.
		{
			// huh? The file is not in the proper format.
			//
			cout << "Panic: While setting RegClassToRegsMappings: ";
			fileName.print(cout);
			cout << " Not in proper format" << endl;
			assert(0);
		}

		// The next token will be the reg file.
		//
		regfilename = strtok(NULL, ")");

		int index = globalRegModel.initRegClassToRegsMapping(datatype, classtype, regfilename, genericOrTarget);

		// Then, read in the register numbers.
		//
		// Remove the first '('
		strtok(NULL, "(");
		word = strtok(NULL, " ");
		if (strcmp(word, "REGS") != 0) // error condition.
		{
			// huh? The file is not in the proper format.
			//
			cout << "Panic: While setting RegClassToRegsMappings: ";
			fileName.print(cout);
			cout << " Not in proper format" << endl;
			assert(0);
		}

		word = strtok(NULL, ")");
		globalRegModel.addRegClassToRegsMapping(index, word, genericOrTarget);
	}
	fp.close();
}

// This function sets the mappings between (generic and target) register classes.
//
// The file format is:
//
// (<keyword> (CLASS_TYPE <class-type>) (DATA_TYPE <data-type>)) (<keyword> (CLASS_TYPE <class-type> (DATA_TYPE <data-type>))
//
// <keyword> can be either GENERIC or TARGET
//
void setRegClassToRegClassMappings(MyString & fileName)
{
	FilePointer fp;
	
	try
	{
		fp.open(fileName, "r");
	}
	catch (CouldNotOpenFileException e)
	{
		cout << e;
		assert(0);
	}

	char line[200];
	char *word, *datatype1, *classtype1, *datatype2, *classtype2, *keyword1, *keyword2;

	while (fgets(line, 200, fp) != NULL)
	{
		// Get the first keyword 

		// Remove the first '('
		strtok(line, "(");
		keyword1 = strtok(NULL, " ");

		// Get the Keyword "DATA_TYPE"

		// Remove the first '('
		strtok(NULL, "(");
		word = strtok(NULL, " ");
		if (strcmp(word, "DATA_TYPE") != 0) // error condition.
		{
			// huh? The file is not in the proper format.
			//
			cout << "Panic: While setting RegClassToRegClassMappings: ";
			fileName.print(cout);
			cout << " Not in proper format" << endl;
			assert(0);
		}

		// The next token will be the data type.
		//
		datatype1 = strtok(NULL, ")");

		// Get the Keyword "CLASS_TYPE"

		// Remove the first '('
		strtok(NULL, "(");
		word = strtok(NULL, " ");
		if (strcmp(word, "CLASS_TYPE") != 0) // error condition.
		{
			// huh? The file is not in the proper format.
			//
			cout << "Panic: While setting RegClassToRegsMappings: ";
			fileName.print(cout);
			cout << " Not in proper format" << endl;
			assert(0);
		}

		// The next token will be the class type.
		//
		classtype1 = strtok(NULL, ")");

		// remove the last ')'

//		strtok(NULL, ")");


		// Get the second keyword 

		// Remove the first '('
		strtok(NULL, "(");
		keyword2 = strtok(NULL, " ");

		// Get the Keyword "DATA_TYPE"

		// Remove the first '('
		strtok(NULL, "(");
		word = strtok(NULL, " ");
		if (strcmp(word, "DATA_TYPE") != 0) // error condition.
		{
			// huh? The file is not in the proper format.
			//
			cout << "Panic: While setting RegClassToRegClassMappings: ";
			fileName.print(cout);
			cout << " Not in proper format" << endl;
			assert(0);
		}

		// The next token will be the data type.
		//
		datatype2 = strtok(NULL, ")");

		// Get the Keyword "CLASS_TYPE"

		// Remove the first '('
		strtok(NULL, "(");
		word = strtok(NULL, " ");
		if (strcmp(word, "CLASS_TYPE") != 0) // error condition.
		{
			// huh? The file is not in the proper format.
			//
			cout << "Panic: While setting RegClassToRegsMappings: ";
			fileName.print(cout);
			cout << " Not in proper format" << endl;
			assert(0);
		}

		// The next token will be the class type.
		//
		classtype2 = strtok(NULL, ")");

		// remove the last ')'

		strtok(NULL, ")");

		globalRegModel.addRegClassToRegClassMappings(keyword1, datatype1, classtype1, keyword2, datatype2, classtype2);
	}

	fp.close();
}

void initRegArchModel()
{
	MyString fileName;

	fileName = "genericRegClasses.txt";
	setRegisterClassTypes(fileName, _GENERIC_);
	fileName = "targetRegClasses.txt";
	setRegisterClassTypes(fileName, _TARGET_);

	fileName = "genericRegClassToRegsMapping.txt";
	setRegClassToRegsMappings(fileName, _GENERIC_);
	fileName = "targetRegClassToRegsMapping.txt";
	setRegClassToRegsMappings(fileName, _TARGET_);

	fileName = "regClassToRegClassMapping.txt";
	setRegClassToRegClassMappings(fileName);

	globalRegModel.initGlobalIndexVariables(_GENERIC_);

    globalRegModel.setCompatibilityArray(_GENERIC_, _TARGET_);
//	globalRegModel.printCompatibilityArray(cout);
}

void RegClassModel::addDataType(char *str)
{
	if (str == NULL) return;
	MyString temp(str);

	_dataTypes.push_back(temp);
}

void RegClassModel::addClassType(char *str)
{
	if (str == NULL) return;
	MyString temp(str);

	_classTypes.push_back(temp);
}

int RegClassModel::getDataTypeIndex(char *str)
{
	if (str == NULL) return _INVALID_;
	MyString temp(str);

	Vector<MyString>::iterator i;
	int retVal = 0;
	for (i = _dataTypes.begin(); i != _dataTypes.end(); i++)
	{
		if (temp == *i)
			return retVal;
		retVal++;
	}
	return _INVALID_;
}

int RegClassModel::getClassTypeIndex(char *str)
{
	if (str == NULL) return _INVALID_;
	MyString temp(str);

	Vector<MyString>::iterator i;
	int retVal = 0;
	for (i = _classTypes.begin(); i != _classTypes.end(); i++)
	{
		if (temp == *i)
			return retVal;
		retVal++;
	}
	return _INVALID_;
}

inline void RegClassToRegsMapping::_addNums(Vector<int> & vect, int startNum, int endNum)
{
	// first, check if the number(s) is(are) to be added at the end.
	//
	if ((vect.empty()) || (startNum > vect.back()))
	{
		for (int k = startNum; k <= endNum; k++)
		{
			vect.push_back(k);
		}
		return;
	}
	else
	{
		// TBD.
		assert(0);
	}
}

inline int RegClassToRegsMapping::_hasNum(Vector<int> & vect, int num)
{
	Vector<int>::iterator i;
	for (i = vect.begin(); i != vect.end(); i++)
		if (num == *i) return _YES_;
	return _NO_;
}

int RegClassToRegsMapping::initRegClassToRegsMapping(int dType, int cType, int rfName)
{
	RegClassEntryNode1 tmp;
	tmp._dType = dType; tmp._cType = cType; tmp._rfName = rfName;
	_regClassIndices.push_back(tmp);

	_numIndices++;
	return (_numIndices - 1);
}

int RegClassToRegsMapping::getIndexForRegClassToRegsMapping(int dType, int cType, int rfName)
{
	int retVal = 0;

	Vector<RegClassEntryNode1>::iterator i;
	for (i = _regClassIndices.begin(); i != _regClassIndices.end(); i++)
	{
		if (((*i)._dType == dType) && ((*i)._cType == cType) && ((*i)._rfName == rfName))
			return retVal;
		retVal++;
	}
	return _INVALID_;
}

int RegClassToRegsMapping::getIndexForRegClassToRegsMapping(int dType, int cType)
{
	int retVal = 0;

	Vector<RegClassEntryNode1>::iterator i;
	for (i = _regClassIndices.begin(); i != _regClassIndices.end(); i++)
	{
		if (((*i)._dType == dType) && ((*i)._cType == cType))
			return retVal;
		retVal++;
	}
	return _INVALID_;
}

void RegClassToRegsMapping::getAllIndicesForRegClassToRegsMapping(int dType, int cType, Vector<int> & indices)
{
	int addVal = 0;

	Vector<RegClassEntryNode1>::iterator i;
	for (i = _regClassIndices.begin(); i != _regClassIndices.end(); i++)
	{
		if (((*i)._dType == dType) && ((*i)._cType == cType))
			indices.push_back(addVal);
		addVal++;
	}
}

void RegClassToRegsMapping::getAllIndicesForRegClassToRegsMapping(RegClassEntryNode2 & ct, Vector<int> & indices)
{
	int addVal = 0;

	Vector<RegClassEntryNode1>::iterator i;
	for (i = _regClassIndices.begin(); i != _regClassIndices.end(); i++)
	{
		if (((*i)._dType == ct._dType) && ((*i)._cType == ct._cType))
			indices.push_back(addVal);
		addVal++;
	}
}

void RegClassToRegsMapping::addRegClassToRegsMapping(int index, char *regNums)
{
	// regNums character string can be of two types:
	//
	// <int>     or      <int>-<int>
	//
	//
	int startNum = _INVALID_;
	int endNum = _INVALID_;

	char *word, *temp;

	word = strtok(regNums, " ");

	while (word != NULL)
	{
		if ((temp = strstr(word, "-")) != NULL)
		{ 
			// is of type <num1>-<num2>
			//
			*temp = '\0';
			temp++;

			startNum = atoi(word);
			endNum = atoi(temp);
		}
		else
		{
			// is of type <num>
			//
			startNum = atoi(word);
			endNum = startNum;
		}

		_addNums(_regRange[index], startNum, endNum);

		word = strtok(NULL, " ");
	}
}

int RegClassToRegsMapping::hasReg(int index, int regNum)
{
	return _hasNum(_regRange[index], regNum);
}

int RegClassToRegsMapping::hasReg(int dType, int cType, int rfName, int regNum)
{
	int index = getIndexForRegClassToRegsMapping(dType, cType, rfName);
	return hasReg(index, regNum);
}

void RegClassToRegsMapping::setAllClassTypesForReg(int rfName, int regNum, Vector<RegClassEntryNode2> & holder)
{
	// get all the indices that satisfy {rfName, regNum} and set the classes.
	//
	Vector<RegClassEntryNode1>::iterator i;
	int index = 0;
	for (i = _regClassIndices.begin(); i != _regClassIndices.end(); i++)
	{
		if ((*i)._rfName == rfName)
		{
			if (_hasNum(_regRange[index], regNum) == _YES_)
			{
				RegClassEntryNode2 tmp;
				tmp._dType = (*i)._dType;
				tmp._cType = (*i)._cType;

				holder.push_back(tmp);
			}
		}
		index++;
	}
}

// go thru RegClassEntryNode1List and determine the uniq classes (classtype+datatype)
//
void RegClassToRegsMapping::setListOfUniqClasses(RegClassEntryNode2List & uniqClasses)
{
	RegClassEntryNode1ListIter i;
	for (i = _regClassIndices.begin(); i != _regClassIndices.end(); i++)
	{
		RegClassEntryNode2 tmp(i->_dType, i->_cType);
		if (std::find(uniqClasses.begin(), uniqClasses.end(), tmp) == uniqClasses.end())
		{
			// found an unique class
			// this has not been added to the list of uniq classes

			uniqClasses.push_back(tmp);
		}
	}
}

int RegClassToRegClassMapping::getMapping(int index)
{
	int retVal = _INVALID_;

	Map<int, int>::iterator i;
	if ((i = _regClassToRegClass.find(index)) != _regClassToRegClass.end())
		retVal = (*i).second;

	return retVal;
}

void RegArchModel::addNumDataTypes(int num, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
		_genericClasses.addNumDataTypes(num);
	else // if (genericOrTarget == _TARGET_)
		_targetClasses.addNumDataTypes(num);
}

void RegArchModel::addNumClassTypes(int num, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
		_genericClasses.addNumClassTypes(num);
	else // if (genericOrTarget == _TARGET_)
		_targetClasses.addNumClassTypes(num);
}

void RegArchModel::addDataType(char *word, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
		_genericClasses.addDataType(word);
	else // if (genericOrTarget == _TARGET_)
		_targetClasses.addDataType(word);
}

void RegArchModel::addClassType(char *word, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
		_genericClasses.addClassType(word);
	else // if (genericOrTarget == _TARGET_)
		_targetClasses.addClassType(word);
}

int RegArchModel::getDataTypeIndex(char *word, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
		return _genericClasses.getDataTypeIndex(word);
	else // if (genericOrTarget == _TARGET_)
		return _targetClasses.getDataTypeIndex(word);
}

int RegArchModel::getClassTypeIndex(char *word, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
		return _genericClasses.getClassTypeIndex(word);
	else // if (genericOrTarget == _TARGET_)
		return _targetClasses.getClassTypeIndex(word);
}

int RegArchModel::initRegClassToRegsMapping(int dType, int cType, int rfType, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
	{
		return _genericRegClassToRegs.initRegClassToRegsMapping(dType, cType, rfType);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		return _targetRegClassToRegs.initRegClassToRegsMapping(dType, cType, rfType);
	}
}

int RegArchModel::initRegClassToRegsMapping(char *dataType, char *classType, char *rfName, int genericOrTarget)
{
	int dType, cType, rfType;
	if (genericOrTarget == _GENERIC_)
	{
		dType = _genericClasses.getDataTypeIndex(dataType);
		cType = _genericClasses.getClassTypeIndex(classType);

		rfType = globalRegFileList->getIndex(rfName);
		
		return _genericRegClassToRegs.initRegClassToRegsMapping(dType, cType, rfType);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		dType = _targetClasses.getDataTypeIndex(dataType);
		cType = _targetClasses.getClassTypeIndex(classType);

		rfType = globalRegFileList->getIndex(rfName);
		
		return _targetRegClassToRegs.initRegClassToRegsMapping(dType, cType, rfType);
	}
}

int RegArchModel::getIndexForRegClassToRegsMapping(int dType, int cType, int rfType, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
	{
		return _genericRegClassToRegs.getIndexForRegClassToRegsMapping(dType, cType, rfType);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		return _targetRegClassToRegs.getIndexForRegClassToRegsMapping(dType, cType, rfType);
	}
}

int RegArchModel::getIndexForRegClassToRegsMapping(char *dataType, char *classType, char *rfName, int genericOrTarget)
{
	int dType, cType, rfType;
	if (genericOrTarget == _GENERIC_)
	{
		dType = _genericClasses.getDataTypeIndex(dataType);
		cType = _genericClasses.getClassTypeIndex(dataType);

		rfType = globalRegFileList->getIndex(rfName);
		
		return _genericRegClassToRegs.getIndexForRegClassToRegsMapping(dType, cType, rfType);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		dType = _targetClasses.getDataTypeIndex(dataType);
		cType = _targetClasses.getClassTypeIndex(dataType);

		rfType = globalRegFileList->getIndex(rfName);
		
		return _targetRegClassToRegs.getIndexForRegClassToRegsMapping(dType, cType, rfType);
	}
}

int RegArchModel::getIndexForRegClassToRegsMapping(int dType, int cType, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
	{
		return _genericRegClassToRegs.getIndexForRegClassToRegsMapping(dType, cType);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		return _targetRegClassToRegs.getIndexForRegClassToRegsMapping(dType, cType);
	}
}

int RegArchModel::getIndexForRegClassToRegsMapping(char *dataType, char *classType, int genericOrTarget)
{
	int dType, cType;
	if (genericOrTarget == _GENERIC_)
	{
		dType = _genericClasses.getDataTypeIndex(dataType);
		cType = _genericClasses.getClassTypeIndex(classType);

		return _genericRegClassToRegs.getIndexForRegClassToRegsMapping(dType, cType);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		dType = _targetClasses.getDataTypeIndex(dataType);
		cType = _targetClasses.getClassTypeIndex(classType);

		return _targetRegClassToRegs.getIndexForRegClassToRegsMapping(dType, cType);
	}
}

void RegArchModel::getAllIndicesForRegClassToRegsMapping(int dType, int cType, int genericOrTarget, Vector<int> & indices)
{
	if (genericOrTarget == _GENERIC_)
	{
		_genericRegClassToRegs.getAllIndicesForRegClassToRegsMapping(dType, cType, indices);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		_targetRegClassToRegs.getAllIndicesForRegClassToRegsMapping(dType, cType, indices);
	}
}

void RegArchModel::getAllIndicesForRegClassToRegsMapping(RegClassEntryNode2 & ct, int genericOrTarget, Vector<int> & indices)
{
	if (genericOrTarget == _GENERIC_)
	{
		_genericRegClassToRegs.getAllIndicesForRegClassToRegsMapping(ct, indices);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		_targetRegClassToRegs.getAllIndicesForRegClassToRegsMapping(ct, indices);
	}
}

void RegArchModel::addRegClassToRegsMapping(int index, char *regNums, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
	{
		_genericRegClassToRegs.addRegClassToRegsMapping(index, regNums);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		_targetRegClassToRegs.addRegClassToRegsMapping(index, regNums);
	}
}

int RegArchModel::getRFForRegClass(int index, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
	{
		return _genericRegClassToRegs.getRFForRegClass(index);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		return _targetRegClassToRegs.getRFForRegClass(index);
	}
}

Vector<int> & RegArchModel::getRegsForRegClass(int index, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
	{
		return _genericRegClassToRegs.getRegsForRegClass(index);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		return _targetRegClassToRegs.getRegsForRegClass(index);
	}
}

void RegArchModel::setListOfUniqClasses(RegClassEntryNode2List & uniqClasses, int genericOrTarget)
{
	if (genericOrTarget == _GENERIC_)
	{
		_genericRegClassToRegs.setListOfUniqClasses(uniqClasses);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		_targetRegClassToRegs.setListOfUniqClasses(uniqClasses);
	}
}

inline int isGenericOrTargetStr(char *genericOrTarget)
{
	if (strcmp("GENERIC", genericOrTarget) == 0) return _GENERIC_;
	else // if (strcmp("TARGET", genericOrTarget) == 0)
		return _TARGET_;
}

void RegArchModel::addRegClassToRegClassMappings(char *keyword1, char *dataType1, char *classType1, char *keyword2, char *dataType2, char *classType2)
{
	if ((keyword1 == NULL) || (keyword2 == NULL)) return;

	int genericOrTarget1 = isGenericOrTargetStr(keyword1);
	
	int fromIndex = getIndexForRegClassToRegsMapping(dataType1, classType1, genericOrTarget1);

	int genericOrTarget2 = isGenericOrTargetStr(keyword2);

	int toIndex = getIndexForRegClassToRegsMapping(dataType2, classType2, genericOrTarget2);

	RegClassRegClassType tmp;
	tmp._genericOrTarget1 = genericOrTarget1; tmp._genericOrTarget2 = genericOrTarget2;

	(_regClassToRegClass[tmp]).addMapping(fromIndex, toIndex);
}

int RegArchModel::getToMapping(int genericOrTarget1, int index1, int genericOrTarget2)
{
	RegClassRegClassType tmp;
	tmp._genericOrTarget1 = genericOrTarget1; tmp._genericOrTarget2 = genericOrTarget2;

	return (_regClassToRegClass[tmp]).getMapping(index1);
}

int RegArchModel::getRFToMapping(int genericOrTarget1, int index1, int genericOrTarget2)
{
	int toIndex = getToMapping(genericOrTarget1, index1, genericOrTarget2);

	return getRFForRegClass(toIndex, genericOrTarget2);
}

int RegArchModel::getFirstRFToMapping(int genericOrTarget1, int dType1, int cType1, int genericOrTarget2)
{
	int index1 = getIndexForRegClassToRegsMapping(dType1, cType1, genericOrTarget1);
	int toIndex = getToMapping(genericOrTarget1, index1, genericOrTarget2);

	return getRFForRegClass(toIndex, genericOrTarget2);
}

int RegArchModel::getFirstRFToMapping(int genericOrTarget1, RegClassEntryNode2 & ct, int genericOrTarget2)
{
	int index1 = getIndexForRegClassToRegsMapping(ct._dType, ct._cType, genericOrTarget1);
	int toIndex = getToMapping(genericOrTarget1, index1, genericOrTarget2);

	return getRFForRegClass(toIndex, genericOrTarget2);
}

Vector<int> & RegArchModel::getRegsToMapping(int genericOrTarget1, int index1, int genericOrTarget2)
{
	int toIndex = getToMapping(genericOrTarget1, index1, genericOrTarget2);

	return getRegsForRegClass(toIndex, genericOrTarget2);
}

Vector<int> & RegArchModel::getFirstRegsToMapping(int genericOrTarget1, int dType1, int cType1, int genericOrTarget2)
{
	int index1 = getIndexForRegClassToRegsMapping(dType1, cType1, genericOrTarget1);
	int toIndex = getToMapping(genericOrTarget1, index1, genericOrTarget2);

	return getRegsForRegClass(toIndex, genericOrTarget2);
}

Vector<int> & RegArchModel::getFirstRegsToMapping(int genericOrTarget1, RegClassEntryNode2 & ct, int genericOrTarget2)
{
	int index1 = getIndexForRegClassToRegsMapping(ct._dType, ct._cType, genericOrTarget1);
	int toIndex = getToMapping(genericOrTarget1, index1, genericOrTarget2);

	return getRegsForRegClass(toIndex, genericOrTarget2);
}

void RegArchModel::getAllRegsToMapping(int genericOrTarget1, int dType1, int cType1, int genericOrTarget2, Vector<RegNumList> & rList)
{
	Vector<int> indices;

	getAllIndicesForRegClassToRegsMapping(dType1, cType1, genericOrTarget1, indices);

	Vector<int>::iterator i;
	for (i = indices.begin(); i != indices.end(); i++)
	{
		RegNumList tmp;
		tmp._rfName = getRFToMapping(genericOrTarget1, (*i), genericOrTarget2);
		tmp._rList = getRegsToMapping(genericOrTarget1, (*i), genericOrTarget2);

		rList.push_back(tmp);
	}
}

void RegArchModel::getAllRegsToMapping(int genericOrTarget1, RegClassEntryNode2 & ct, int genericOrTarget2, Vector<RegNumList> & rList)
{
	getAllRegsToMapping(genericOrTarget1, ct._dType, ct._cType, genericOrTarget2, rList);
}

void intersectRegisterNumbers(Vector<int> & into, Vector<int> & from1, Vector<int> & from2)
{
	// into = from1 /\ from2

	Vector<int>::iterator i1, i2;

	for (i1 = from1.begin(); i1 != from1.end(); i1++)
	{
		for (i2 = from2.begin(); i2 != from2.end(); i2++)
		{
			if (*i1 == *i2)
			{
				into.push_back(*i1);
				break;
			}
		}
	}
}

int RegArchModel::isEqualClassSets(Vector<RegClassEntryNode2> & cls1, Vector<RegClassEntryNode2> & cls2)
{
	// compares two class sets and returns true if each vector has the same number and type of classes
	
	if (cls1.size() != cls2.size()) return _NO_;

	Vector<RegClassEntryNode2>::iterator i1, i2;

	for (i1 = cls1.begin(); i1 != cls1.end(); i1++)
	{
		for (i2 = cls2.begin(); i2 != cls2.end(); i2++)
		{
			if (((*i1)._cType == (*i2)._cType) && ((*i1)._dType == (*i2)._dType))
				break;
		}

		if (i2 == cls2.end()) // i.e. did not find a match for the i1 element
			return _NO_;
	}

	return _YES_;
}

void RegArchModel::intersectRegisterSets(Vector<RegNumList> & into, Vector<RegNumList> & from)
{
	// the into register set contains:
	// { (INTO1 = {rf-name11 (reg-nums1[j-k]1)}), ..., (INTOi = {rf-name1i (reg-nums1[j-k]i)}) }
	//
	// the from register set contains:
	// { (FROM1 = {rf-name21 (reg-nums2[j-k]1)}), ..., (FROMl = {rf-name2l (reg-nums2[j-k]l)}) }
	//
	//
	// for each INTOi, if INTOi.rf-name != {FROM1, ..., FROMl}.rf-name, delete that entry from INTO
	//
	// else, intersect the registers from INTOi with all matches in FROM

	Vector<RegNumList> tempList;
	Vector<RegNumList>::iterator i1, i2;

	for (i1 = into.begin(); i1 != into.end(); i1++)
	{
		int rfName1 = i1->_rfName;
		for (i2 = from.begin(); i2 != from.end(); i2++)
		{
			if (rfName1 == i2->_rfName)
			{
				RegNumList tmp;
				tmp._rfName = rfName1;

				intersectRegisterNumbers(tmp._rList, i1->_rList, i2->_rList);

				if (!(tmp._rList).empty())
					tempList.push_back(tmp);
			}
		}
	}

	into.erase(into.begin(), into.end());
	into = tempList;
}

void RegArchModel::getRegsThatSatisfyAllMappings
		(int genericOrTarget1, RegClassEntryNode2 ct1, RegClassEntryNode2 ct2,
		int genericOrTarget2, Vector<RegNumList> & rList)
{
	RegClassEntryNode2List cTList;
	cTList.push_back(ct1);
	cTList.push_back(ct2);

	getRegsThatSatisfyAllMappings(genericOrTarget1, cTList, genericOrTarget2, rList);
}


void RegArchModel::getRegsThatSatisfyAllMappings
		(int genericOrTarget1, RegClassEntryNode2 ct1, RegClassEntryNode2 ct2, 
		int genericOrTarget2, Vector<int> & regIndexList)
{
	RegClassEntryNode2List cTList;
	cTList.push_back(ct1);
	cTList.push_back(ct2);

	getRegsThatSatisfyAllMappings(genericOrTarget1, cTList, genericOrTarget2, regIndexList);
}

void RegArchModel::getRegsThatSatisfyAllMappings(int genericOrTarget1, Vector<RegClassEntryNode2> & ctList, int genericOrTarget2, Vector<RegNumList> & rList)
{
	if (ctList.size() == 0)
	{
		cout << "NULL Class list recieved" << endl;
		assert(0);
	}

	if (ctList.size() == 1)
		getAllRegsToMapping(genericOrTarget1, ctList.front(), genericOrTarget2, rList);

	else // if (ctList.size() > 1)
	{
		Vector<RegClassEntryNode2>::iterator ctI;
		Vector<RegNumList> tempList;

		// Add the first set of registers to rList.
		//
		ctI = ctList.begin();
		getAllRegsToMapping(genericOrTarget1, *ctI, genericOrTarget2, rList);

		ctI++;

		for (ctI; ctI != ctList.end(); ctI++)
		{
			tempList.clear();
			getAllRegsToMapping(genericOrTarget1, *ctI, genericOrTarget2, tempList);
			
			intersectRegisterSets(rList, tempList);
		}
	}
}

// Converts from the RegArchModel internal format to the RegFileList format.
//
int convertRegNumToRegIndex(int rfName, int regNum)
{
	return (globalRegFileList->getBaseID(rfName) + regNum);
}

void RegArchModel::getRegsThatSatisfyAllMappings(int genericOrTarget1, Vector<RegClassEntryNode2> & ctList, int genericOrTarget2, Vector<int> & regIndexList)
{
	if (ctList.size() == 0)
	{
		cout << "NULL Class list recieved" << endl;
		assert(0);
	}

	Vector<RegNumList> rList;

	if (ctList.size() == 1)
		getAllRegsToMapping(genericOrTarget1, ctList.front(), genericOrTarget2, rList);

	else // if (ctList.size() > 1)
	{
		Vector<RegClassEntryNode2>::iterator ctI;
		Vector<RegNumList> tempList;

		// Add the first set of registers to rList.
		//
		ctI = ctList.begin();
		getAllRegsToMapping(genericOrTarget1, *ctI, genericOrTarget2, rList);

		ctI++;

		for (ctI; ctI != ctList.end(); ctI++)
		{
			tempList.clear();
			getAllRegsToMapping(genericOrTarget1, *ctI, genericOrTarget2, tempList);
			
			intersectRegisterSets(rList, tempList);
		}
	}

	Vector<RegNumList>::iterator rlI;
	for (rlI = rList.begin(); rlI != rList.end(); rlI++) 
	{
		int rfName = rlI->_rfName;
		int baseID = globalRegFileList->getBaseID(rfName);

		Vector<int>::iterator rnI;
		for (rnI = (rlI->_rList).begin(); rnI != (rlI->_rList).end(); rnI++) 
		{
			int regNum = (*rnI);

			int regIndex = baseID + regNum;
	
			regIndexList.push_back(regIndex);
		}
	}
}

void RegArchModel::getAllClassTypesForReg(BaseArgument *arg, Vector<RegClassEntryNode2> & holder, int genericOrTarget)
{
	if ((arg != NULL) && (!arg->isRealRegArg())) return;

	RegClassArgProperty tmp;
	int rfName = ((RealRegArg *)arg)->regFileIndex();
	int regNum = ((RealRegArg *)arg)->getVal();

	if (genericOrTarget == _GENERIC_)
	{
		_genericRegClassToRegs.setAllClassTypesForReg(rfName, regNum, holder);
	}
	else // if (genericOrTarget == _TARGET_)
	{
		_targetRegClassToRegs.setAllClassTypesForReg(rfName, regNum, holder);
	}
}

void RegArchModel::printCompatibilityArray(ostream & out)
{
	out << endl << "Printing Compatibility Array" << endl;

	Map<RegClassEntryNode2, RegClassEntryNode2List>::iterator i;
	RegClassEntryNode2List::iterator j;

	for (i = _compatibilityArray.begin(); i != _compatibilityArray.end(); i++)
	{
		cout << "\t Compatibility for : ";
		(i->first).print(out);
		cout << endl;
		cout << "\t\t";

		for (j = (i->second).begin(); j != (i->second).end(); j++)
		{
			j->print(out);
		}
		cout << endl;
	}
}

// PB:
// This module finds the register in a register file that maps to class SP (stack pointer)
void getSPSymVal(MyString &regText, int &symVal)
{
	MyString fileName = "targetRegClassToRegsMapping.txt";
	FilePointer fp;
	symVal = -1;
	
	try
	{
		fp.open(fileName, "r");
	}
	catch (CouldNotOpenFileException e)
	{
		cout << e;
		assert(0);
	}
	
	char line[200];
	char *word;
	
	while (fgets(line, 200, fp) != NULL)
	{
		// Find the line that has a mapping from SP to a target register
		
		if( strstr(line, "CLASS_TYPE SP") != NULL )
		{
			// line contains the mapping of SP to a target register
			strtok(line, "(");
			word = strtok(NULL, " ");
			if (strcmp(word, "DATA_TYPE") != 0) // error condition.
			{
				// huh? The file is not in the proper format.
				//
				cout << "Panic: While setting RegClassToRegsMappings: ";
				fileName.print(cout);
				cout << " Not in proper format" << endl;
				assert(0);
			}
			
			// The next token will be the data type.
			//
			word = strtok(NULL, ")");
			
			// Get the Keyword "CLASS_TYPE"
			
			// Remove the first '('
			strtok(NULL, "(");
			word = strtok(NULL, " ");
			if (strcmp(word, "CLASS_TYPE") != 0) // error condition.
			{
				// huh? The file is not in the proper format.
				//
				cout << "Panic: While setting RegClassToRegsMappings: ";
				fileName.print(cout);
				cout << " Not in proper format" << endl;
				assert(0);
			}
			
			
			// Get the Keyword "REG_FILE"
			
			// Remove the first '('
			strtok(NULL, "(");
			word = strtok(NULL, " ");
			if (strcmp(word, "REG_FILE") != 0) // error condition.
			{
				// huh? The file is not in the proper format.
				//
				cout << "Panic: While setting RegClassToRegsMappings: ";
				fileName.print(cout);
				cout << " Not in proper format" << endl;
				assert(0);
			}
			
			// The next token will be the reg file.
			//
			regText = strtok(NULL, ")");
						
			// Then, read in the register numbers.
			//
			// Remove the first '('
			strtok(NULL, "(");
			word = strtok(NULL, " ");
			if (strcmp(word, "REGS") != 0) // error condition.
			{
				// huh? The file is not in the proper format.
				//
				cout << "Panic: While setting RegClassToRegsMappings: ";
				fileName.print(cout);
				cout << " Not in proper format" << endl;
				assert(0);
			}
			
			word = strtok(NULL, ")");
			symVal = atoi(word);
			return;
		}
	}
	fp.close();
	return;
}



