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
	/*************************************************************
*file  :		TargetRegInfo.h							     *
*authors :		Aviral Shrivastava                           *
*last modified : 09/10/2001                                  *
*                                                            *
*contents:		Register Information Generation Header		 *
*comments:-                                                  *
*compiled with : g++                                         *
*for more info : aviral@ics.uci.edu                          *
**************************************************************/

#ifndef _TARGET_REG_INFO_H_
#define _TARGET_REG_INFO_H_

#ifdef WIN32
#pragma warning(disable:4786)

#endif
#include <string.h>
#include <list>
#include <set>
#include <map>
#include "ClassTypeDataTypePair.h"

#ifdef  __UNIX__
typedef list<int> intList;
#else
typedef std::list<int> intList;
#endif

#ifdef  __UNIX__
typedef list<MyString> MyStringList;
#else
typedef std::list<MyString> MyStringList;
#endif

#ifdef  __UNIX__
typedef set<MyString> MyStringSet;
#else
typedef std::set<MyString> MyStringSet;
#endif

#ifdef  __UNIX__
typedef set<int> intSet;
#else
typedef std::set<int> intSet;
#endif

typedef struct record_type{
	MyString reg_mnemonic;
	intSet  reg_num;
} record;


#ifdef  __UNIX__
typedef list<record> RecordList;
#else
typedef std::list<record> RecordList;
#endif

typedef struct target_reg_mapping_type{
	MyString target_classtype;
	MyString target_datatype;
	RecordList R;
} target_reg_mapping;


#ifdef  __UNIX__
typedef list<target_reg_mapping> RegMappingList;
#else
typedef std::list<target_reg_mapping> RegMappingList;
#endif

class Regs
{
	MyString	_regFileName;
	MyString	_regFileMnemonic;
	intList		_regs;


public:
	
	Regs(void)
	{}

	~Regs(void)
	{}

	Regs(char *regFileName)
	{
		_regFileName = regFileName;
	}

	Regs(MyString regFileName)
	{
		_regFileName = regFileName;
	}

	MyString RegFileName(void)
	{
		return _regFileName;
	}

	void RegFileName(MyString regFileName)
	{
		_regFileName = regFileName;
	}

	MyString RegFileMnemonic(void)
	{
		return _regFileMnemonic;
	}

	void RegFileMnemonic(MyString regFileMnemonic)
	{
		_regFileMnemonic = regFileMnemonic;
	}

	intList RegList(void)
	{
		return _regs;
	}

	void AddReg(int reg_no)
	{
		for (intList::iterator i = _regs.begin(); i != _regs.end(); i++)
		{
			if ((*i) == reg_no)
				return;
		}
		_regs.push_back(reg_no);
	}

	Regs copy()
	{
		Regs newRegs;
		newRegs.RegFileName(_regFileName);
		newRegs.RegFileMnemonic(_regFileMnemonic);

		for (intList::iterator i = _regs.begin(); i != _regs.end(); i++)
		{
			newRegs.AddReg((*i));
		}

		return newRegs;
	}

	void Print()
	{
		printf("regFileName = %s\n", _regFileName.getStr());
		printf("regFileMnemonic = %s\n", _regFileMnemonic.getStr());
		
		printf("regs : ");
		for (intList::iterator j = _regs.begin(); j!= _regs.end(); j++)
		{
			printf(" %d", (*j));
		}
		printf("\n");
	}

	void printForTargetRegFileList(RecordList& reg_node_list)
	{
		record temp; int flag=1;
		temp.reg_mnemonic = _regFileMnemonic.getStr();
	
	
		for (intList::iterator k = _regs.begin(); k != _regs.end(); k++)
		{
			int num = (*k);
			temp.reg_num.insert(num);
		}
		

		for (RecordList::iterator i = reg_node_list.begin(); 
			 i!= reg_node_list.end(); i++)
			 
			   if ((*i).reg_mnemonic == temp.reg_mnemonic) 
			   {
				   for (intSet::iterator p = temp.reg_num.begin(); 
						p != temp.reg_num.end(); p ++) 
						 (*i).reg_num.insert(*p);
				   
			    flag=0;
				break;
			   }
			 
              	
		if (flag) reg_node_list.push_back(temp);
			
	}

	void PrintForTargetRegClassToRegsMapping(FILE *fptr,RegMappingList& RegMap,target_reg_mapping& temp)
	{
		record r;int foo=1;
		r.reg_mnemonic=_regFileMnemonic;
		for (RecordList::iterator j=temp.R.begin();j!=temp.R.end();j++)
		{
			MyString cmpr = (*j).reg_mnemonic;
			if ( _regFileMnemonic == cmpr)
			{
				foo=0;
				break;
			}
		}
		if (!foo) return;
		for (intList::iterator k = _regs.begin(); k != _regs.end(); k++)
		{
			r.reg_num.insert((*k));
		}
		
		temp.R.push_back(r);
	}

};



#ifdef  __UNIX__
typedef list<Regs> RegsList;
#else
typedef std::list<Regs> RegsList;
#endif


class TargetRegInfo
{
	MyString					_varName;
	ClassTypeDataTypePairList	_cdPairList;
	RegsList					_regInfo;
	
public:
	
	TargetRegInfo(void)
	{}

	// Note: The only constructor with a parameter has varName as input
	TargetRegInfo(char * varName)
	{
		_varName = varName;
	}

	~TargetRegInfo(void)
	{}

	MyString VarName(void) 
	{ 
		return _varName; 
	}

	void VarName(char *varName)
	{
		_varName = varName;
	}

	void VarName(MyString & varName)
	{
		_varName = varName;
	}

	ClassTypeDataTypePairList CDPairList()
	{
		return _cdPairList;
	}

	void TargetDataType(char * targetDataType)
	{
		ClassTypeDataTypePair cdPair(targetDataType);

		_cdPairList.push_back(cdPair);
	}

	void AddTargetClassAndDataType(MyString & targetClassType, MyString & targetDataType)
	{
		// Check if the pair already exists
		for ( ClassTypeDataTypePairList::iterator cdPairListIter = _cdPairList.begin();
			cdPairListIter != _cdPairList.end(); cdPairListIter++)
		{
			if ((targetClassType == (*cdPairListIter).TargetClassType())
				&& (targetDataType == (*cdPairListIter).TargetDataType()))
				return;
		}
		
		ClassTypeDataTypePair cdPair(targetClassType, targetDataType);
		_cdPairList.push_back(cdPair);
	}

	int ContainsCTDTPair(ClassTypeDataTypePair cdPair)
	{
		for (ClassTypeDataTypePairList::iterator i = _cdPairList.begin();
		i != _cdPairList.end(); i++)
		{
			ClassTypeDataTypePair thisCDPair = (*i);

			if ((!thisCDPair.TargetClassType().isEmpty())&&(!thisCDPair.TargetDataType().isEmpty()) &&
				(thisCDPair.TargetClassType() == cdPair.TargetClassType()) &&
				(thisCDPair.TargetDataType() == cdPair.TargetDataType()))
				return 1;
		}
		return 0;
	}
	
	void AddCDPair(ClassTypeDataTypePair cdPair)
	{
		_cdPairList.push_back(cdPair);
	}
	
	void AppendCDPairList(ClassTypeDataTypePairList cdPairList)
	{
		// if there is a classtype int, then it prevails.

		for (ClassTypeDataTypePairList::iterator i = cdPairList.begin();
		i != cdPairList.end(); i++)
		{
			ClassTypeDataTypePair cdPair = (*i);
			if (!this->ContainsCTDTPair(cdPair))
				_cdPairList.push_back(cdPair);
		}

/*
		// now check if the CTDTPairList contains a pair with classtype ANY
		// then remove all of the entries and leave only the one with ANY.

		for (i = cdPairList.begin(); i != cdPairList.end(); i++)
		{
			ClassTypeDataTypePair cdPair = (*i);
			if (cdPair.TargetClassType() == "ANY")
			{
				MyString dataType = cdPair.TargetDataType();
				_cdPairList.clear();
				ClassTypeDataTypePair newCDPair("ANY", dataType);
				_cdPairList.push_back(newCDPair);
				return;
			}
		}
*/
	}
	
	RegsList & RegInfo(void)
	{ 
		return _regInfo; 
	}

	void AddRegFileName(char *regFileName)
	{
		for (RegsList::iterator i = _regInfo.begin(); i != _regInfo.end(); i++)
		{
			if (strcmp(regFileName, (*i).RegFileName().getStr()) == 0)
				return;
		}
		Regs n = Regs(regFileName);
		_regInfo.push_back(n);
	}

	
	void AddRegFileName(MyString & regFileName)
	{
		for (RegsList::iterator i = _regInfo.begin(); i != _regInfo.end(); i++)
		{
			if (regFileName == (*i).RegFileName())
				return;
		}
		Regs n = Regs(regFileName);
		_regInfo.push_back(n);
	}

	
	void RemRegFile(MyString & regFileName)
	{
		for (RegsList::iterator i = _regInfo.begin(); i != _regInfo.end(); i++)
		{
			if (regFileName == (*i).RegFileName())
			{
				_regInfo.erase(i);
				return;
			}
		}
	}
	

	void AddReg(char * regFileName, int reg_no)
	{
		for (RegsList::iterator i = _regInfo.begin(); i != _regInfo.end(); i++)
		{
			if (strcmp(regFileName, (*i).RegFileName().getStr()) == 0)
			{
				// Add this register in this file
				(*i).AddReg(reg_no);
				return;
			}
		}
		// if the regFile does not exist
		// create a new regFile, and add the reg in it
		//
		Regs n = Regs(regFileName);
		n.AddReg(reg_no);
		_regInfo.push_back(n);
	}


	void AddReg(MyString & regFileName, int reg_no)
	{
		for (RegsList::iterator i = _regInfo.begin(); i != _regInfo.end(); i++)
		{
			if (regFileName == (*i).RegFileName())
			{
				// Add this register in this file
				(*i).AddReg(reg_no);
				return;
			}
		}
		// if the regFile does not exist
		// create a new regFile, and add the reg in it
		//
		Regs n = Regs(regFileName);
		n.AddReg(reg_no);
		_regInfo.push_back(n);
	}

	
	void AddRegFileMnemonic(MyString & regFileName, MyString & regFileMnemonic)
	{
		for (RegsList::iterator i = _regInfo.begin(); i != _regInfo.end(); i++)
		{
			if (regFileName == (*i).RegFileName())
			{
				// Add this register in this file
				(*i).RegFileMnemonic(regFileMnemonic);
				return;
			}
		}
		// if the regFile does not exist
		// print an error

		printf("The regFile %s does not exist\n", regFileName.getStr());
	}

	void AddRegInfo(Regs regs)
	{
		_regInfo.push_back(regs);
	}

	TargetRegInfo copy()
	{
		TargetRegInfo newTargetRegInfo;

		newTargetRegInfo.VarName(_varName);

		for (ClassTypeDataTypePairList::iterator i = _cdPairList.begin();
		i != _cdPairList.end();i++)
		{
			newTargetRegInfo.AddCDPair((*i).copy());
		}

		for (RegsList::iterator j = _regInfo.begin(); 
		j != _regInfo.end(); j++)
		{
			newTargetRegInfo.AddRegInfo((*j).copy());
		}

		return newTargetRegInfo;
	}
	

	void Print()
	{
		printf("varName = %s\n", _varName.getStr());
		for (ClassTypeDataTypePairList::iterator i = _cdPairList.begin();
		i != _cdPairList.end();i++)
		{
			(*i).Print();
		}

		for (RegsList::iterator j = _regInfo.begin(); 
		j != _regInfo.end(); j++)
		{
			(*j).Print();
		}

		printf("\n");
	}

	
	void PrintForTargetRegClassToRegsMapping(FILE *fptr,RegMappingList& RegMap)
	{
	
		MyString CT,DT;
		target_reg_mapping temp;
		ClassTypeDataTypePairList::iterator i = _cdPairList.begin();
	
		while (i != _cdPairList.end())
		{
			//(*i).PrintForTargetRegClassToRegsMapping(fptr,CT,DT);
			CT = (*i).TargetClassType();
			DT = (*i).TargetDataType();
			int flag=1;
			for (RegMappingList::iterator k= RegMap.begin();k!=RegMap.end();k++)
			{
				MyString D=(*k).target_datatype; MyString C=(*k).target_classtype;
				if (CT == C && DT == D)
				{
					flag=0;
					break;
				}
			}
			if (flag) 
			{
				temp.target_classtype=CT;
				temp.target_datatype=DT;
						

			for (RegsList::iterator j = _regInfo.begin();j != _regInfo.end();j++)
			(*j).PrintForTargetRegClassToRegsMapping(fptr,RegMap,temp);
			RegMap.push_back(temp);
			}	
			
			i++;
		}
		
	}

	//  ATRI MANDAL 
	void CollectDataForTargetRegClasses(MyStringSet& S)
	{
		MyString target_data;
		//fprintf(fptr, " ");

	    for (ClassTypeDataTypePairList::iterator i = _cdPairList.begin();
		i != _cdPairList.end();i++)
		{
			target_data = (*i).CollectDataForTargetRegClasses();
			S.insert(target_data);
		}
	}

	
	void CollectClassForTargetRegClasses(MyStringSet& T)
	{
		MyString target_class;
		//fprintf(fptr, " ");

	    for (ClassTypeDataTypePairList::iterator i = _cdPairList.begin();
		i != _cdPairList.end();i++)
		{
			target_class = (*i).CollectClassForTargetRegClasses();
			T.insert(target_class);
		}
	}
	
/*  Atri Mandal */
	void printForTargetRegFileList(RecordList& reg_node_list)
	{
	    
		for (RegsList::iterator j = _regInfo.begin(); j != _regInfo.end(); j++)
		{
			(*j).printForTargetRegFileList(reg_node_list);
		}
	}

		void PrintForRegClassToRegClassMapping(FILE *fptr, MyString genericDataType, MyString genericClassType)
	{
		for (ClassTypeDataTypePairList::iterator i = _cdPairList.begin();
		i != _cdPairList.end();i++)
		{
			(*i).PrintForRegClassToRegClassMapping(fptr, genericDataType, genericClassType);
		}
	}

};

#ifdef  __UNIX__
typedef map<MyString, TargetRegInfo> TargetRegInfoMap;
#else
typedef std::map<MyString, TargetRegInfo> TargetRegInfoMap;
#endif

#endif