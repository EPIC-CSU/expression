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
 *file  :		targetRegInfoGen.cpp						 *
 *authors :		Aviral Shrivastava                           *
 *last modified : 09/10/2001                                 *
 *                                                           *
 *contents:		Target Register Information Generation		 *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info : aviral@ics.uci.edu                         *
 *************************************************************/

#include "node.h"
#include <set>
#include "regInfoGen.h"
#include "targetRegInfo.h"
#include <math.h>

// temporary vector of var_group_names and associated data_types.
//
#ifdef  __UNIX__
typedef map<MyString, MyString> VarNameDataTypeMap;
#else
typedef std::map<MyString, MyString> VarNameDataTypeMap;
#endif

VarNameDataTypeMap tempVarNameDataTypeMap;


#ifdef  __UNIX__
typedef vector<MyString> MyStringVector;
#else
typedef std::vector<MyString> MyStringVector;
#endif

#ifdef  __UNIX__
typedef set<MyString> MyStringSet;
#else
typedef std::set<MyString> MyStringSet;
#endif

#ifdef  __UNIX__
typedef set<intList> intListSet;
#else
typedef std::set<intList> intListSet;
#endif

#ifdef  __UNIX__
typedef vector<MyStringVector> MyStringVectorVector;
#else
typedef std::vector<MyStringVector> MyStringVectorVector;
#endif

#ifdef  __UNIX__
typedef vector<int> intVector;
#else
typedef std::vector<int> intVector;
#endif

#ifdef  __UNIX__
typedef map<int, int> IntIntMap;
#else
typedef std::map<int, int> IntIntMap;
#endif

IntIntMap targetToRisaMap;

static int operationsCount = 0;

/*
The aim of this file is to read the EXPRESSION IR and generate the 3 files
1. targetRegClassToRegsMapping
2. regClassToRegClassMapping
3. rISARegisterMapping

Now for this this file first reads the IR, and generates an intermediate 
data structure and then prints out these three files from the data structure

This file reads mainly 2 parts of the IR
1. OPERATIONS_SECTION --> VAR_GROUPS
2. OPMAPPING_SECTION --> OPERAND_MAPPING

Also it reads 
1. instruction size information from the OPERATIONS_SECTION --> OP_GROUP.
2. instruction mapping info from OPMAPPING_SECTION --> OPCODE_MAPPING.

This file makes targetRegInfoMap, which is a table of

  <var_name> <ClasstypeDatatypePairList, RegisterInfo>

So each var_name corresponds to a list of <classType, dataType> pairs,
and can map to a set of registers mentioned in RegisterInfo.

*/


TargetRegInfoMap TargetRegInfoMapCopy(TargetRegInfoMap targetRegInfoMap)
{
	TargetRegInfoMap newTargetRegInfoMap;

	for (TargetRegInfoMap::iterator i = targetRegInfoMap.begin(); i != targetRegInfoMap.end(); i++)
	{
		newTargetRegInfoMap[(*i).first] = (*i).second.copy();
	}
	return newTargetRegInfoMap;
}


void removeNoMnemonicEntry(RegsList &regInfo)
{
	int done = 0;
	while (!done)
	{
		for (RegsList::iterator j = regInfo.begin(); j != regInfo.end(); j++)
		{
			MyString regFileName = (*j).RegFileName();
			MyString regFileMnemonic = (*j).RegFileMnemonic();
			if (regFileMnemonic.isEmpty())
			{
				// This regFile does not have a mnemonic
				// delete this entry
				regInfo.erase(j);
				break;
			}
		}
		
		int notdone = 0;
		for (RegsList::iterator k = regInfo.begin(); k != regInfo.end(); k++)
		{
			if ((*k).RegFileMnemonic().isEmpty())
				notdone = 1;
		}
		done = 1 - notdone;
	}	
}

// This function does two things
// 1. removes the entries which do not map to a register file with a mnemonic
// 2. In case the only register file is removed, then it has to delete the whole var_group entry.
//    So it returns a value in this case, else it returns 0
//
void removeNoMnemonicEntries(TargetRegInfoMap &targetRegInfoMap)
{
	for (TargetRegInfoMap::iterator i = targetRegInfoMap.begin(); i != targetRegInfoMap.end(); i++)
	{
		// clean up the regInfos first
		//
		RegsList & regInfo = (*i).second.RegInfo();
		removeNoMnemonicEntry(regInfo);
	}

	// now all the targetRegInfos have regFileMnemonics
	// we can remove all entries that do not have zero size regInfo
	//
	int done = 0;
	while(!done)
	{
		for (i = targetRegInfoMap.begin(); i != targetRegInfoMap.end(); i++)
		{
			RegsList regInfo = (*i).second.RegInfo();
			if (regInfo.size() == 0)
			{
				targetRegInfoMap.erase(i);
				break;
			}
		}

		int notdone = 0;
		for (i = targetRegInfoMap.begin(); i != targetRegInfoMap.end(); i++)
		{
			RegsList regInfo = (*i).second.RegInfo();
			if (regInfo.size() == 0)
				notdone = 1;
		}
		done = 1 - notdone;
	}
}

int regFileName_is_varName(MyString regFileName, TargetRegInfoMap targetRegInfoMap)
{
	for (TargetRegInfoMap::iterator i = targetRegInfoMap.begin(); i != targetRegInfoMap.end(); i++)
	{
		if (regFileName == (*i).first)
			return 1;
	}
	return 0;
}


// checks to see if any of the register file is also a varName
// 
int isReducedTargetRegInfoMap(TargetRegInfoMap targetRegInfoMap)
{
	for (TargetRegInfoMap::iterator i = targetRegInfoMap.begin(); i != targetRegInfoMap.end(); i++)
	{
		RegsList regInfo = (*i).second.RegInfo();

		for (RegsList::iterator j = regInfo.begin(); j != regInfo.end(); j++)
		{
			MyString regFileName = (*j).RegFileName();

			if (regFileName_is_varName(regFileName, targetRegInfoMap))
				return 0;
		}
	}
	return 1;
}

void reduceTargetRegInfo(TargetRegInfo &targetRegInfo, TargetRegInfoMap targetRegInfoMap)
{
	RegsList regList = targetRegInfo.RegInfo();

	for (RegsList::iterator i = regList.begin(); i != regList.end(); i++)
	{
		Regs regFileInfo = (*i);
		MyString varName = regFileInfo.RegFileName();

		if (regFileName_is_varName(varName, targetRegInfoMap))
		{
			// get the regInfo of this varName
			RegsList newRegInfo = targetRegInfoMap[varName].RegInfo();
			
			// remove regFileName
			// and add the new one

 			targetRegInfo.RemRegFile(varName);

			// now add the registers
			//
			for (RegsList::iterator j = newRegInfo.begin(); 
			j != newRegInfo.end(); j++)
			{
				MyString newRegFileName = (*j).RegFileName();
				MyString newRegFileMnemonic = (*j).RegFileMnemonic();
				targetRegInfo.AddRegFileName(newRegFileName);
				targetRegInfo.AddRegFileMnemonic(newRegFileName, newRegFileMnemonic);

				intList regs = (*j).RegList();
				for (intList::iterator k = regs.begin(); k != regs.end(); k++)
				{
					targetRegInfo.AddReg(newRegFileName, (*k));
				}
			}

			// Also we have to add the cdPair in cdPairList
			//
			ClassTypeDataTypePairList cdPairList = targetRegInfoMap[varName].CDPairList();
			targetRegInfo.AppendCDPairList(cdPairList);
		}
	}
}


void reduceTargetRegInfoMap(TargetRegInfoMap &targetRegInfoMap, NodePtr ir)
{
	// we have allowed for recursive definitions
	// thus definitions like
	// (double_retval (DATATYPE DOUBLE)(REGS double1_retval double2_retval))
	// are also possible
	
	// in such a case, the ir contains
    // nke_var double_retval -1
    //  nke_datatype DOUBLE -1
    //  nke_reg_list reg_list -1
    //   nke_regs double1_retval -1
    //   nke_regs double2_retval -1
	
	// This results in targetRegInfoMap entry like

	// double1_retval ANY DOUBLE FPRFile 0 2 4 6 8 10 ...
	// double2_retval ANY DOUBLE FPRFile 1 3 5 7 9 11 ...
	// double_retval  ANY DOUBLE double1_retval double2_retval

	// So we need to reduce the table formed, so that in the final table
	// there are only entries that have 

	if (isReducedTargetRegInfoMap(targetRegInfoMap))
		return;

	for (TargetRegInfoMap::iterator i = targetRegInfoMap.begin(); i != targetRegInfoMap.end(); i++)
	{
		RegsList regList = (*i).second.RegInfo();
		// iterate over this
		for (RegsList::iterator j = regList.begin(); j != regList.end(); j++)
		{
			if (regFileName_is_varName((*j).RegFileName(), targetRegInfoMap))
			{
				reduceTargetRegInfo((*i).second, targetRegInfoMap);
			}
		}
	}

	reduceTargetRegInfoMap(targetRegInfoMap, ir);

	printTargetRegInfo(targetRegInfoMap);

	// Step 2 is to remove the targetRegInfo's that have no registerFileMnemonics

	// removeNoMnemonicEntries(targetRegInfoMap);

	// Now we do not remove these.
	// Because we have to print out imm class type in rISARegsMappingFile
	// But we do not have to print these in targetRegClassToRegsMapping and regClassToRegClassMapping

}


MyString getRegFileMnemonicFromRegsNode(NodePtr regsNode, Symtab st)
{
	MyString regFileMnemonic;
	
	// find the register node from the symbol table
	SymbolPtr sym = st.FindSymbol(regsNode->Name());
	NodeList *nList = sym->GetUseList();
	for (NodeList::iterator i = nList->begin(); i != nList->end(); i++)
	{
		if ((*i)->Kind() == nke_storage)
		{
			NodePtr useNode = (*i);
			if (useNode->ExistsChild(nke_mnemonic))
			{
				regFileMnemonic = useNode->Child(nke_mnemonic)->Name();
			}
		}
	}

	return regFileMnemonic;
}

void genRegInfoFromRegsNode(TargetRegInfo &targetRegInfo, NodePtr regsNode, Symtab st)
{
	if (regsNode->Kind() == nke_regs)
	{
		// regFileName is the name field
		MyString regFileName = regsNode->Name();
		
		targetRegInfo.AddRegFileName(regsNode->Name());
						
		// Add the registerMnemonic too
		MyString regFileMnemonic = getRegFileMnemonicFromRegsNode(regsNode, st);

		targetRegInfo.AddRegFileMnemonic(regFileName, regFileMnemonic);


		// the registers are listed in num field of children
		if (regsNode->hasChildren())
		{
			for (NodeList::iterator regNumIter = regsNode->Children()->begin();
			regNumIter != regsNode->Children()->end();
			regNumIter++)
			{
				targetRegInfo.AddReg(regsNode->Name(), atoi((*regNumIter)->Name()));
			}
		}
		else
			// if only register file is supplied
			// it means all the registers in that
		{
			// find the register node from the symbol table
			SymbolPtr sym = st.FindSymbol(regsNode->Name());
			NodeList *nList = sym->GetUseList();
			for (NodeList::iterator i = nList->begin(); i != nList->end(); i++)
			{
				if ((*i)->Kind() == nke_storage)
				{
					// This is the node
					// now find the capacity
					NodePtr defNode = (*i);
					if (defNode->ExistsChild(nke_storagesize))
					{
						// add these registers in targetRegInfo
						int cap = atoi(defNode->Child(nke_storagesize)->Name());
						
						// I should comment a lot on this thing
						// If capacity is large, it might be MEM, so reduce that to one.
						//
						if (cap > 1000)
							cap = 1;
						
						for (int i=0; i<cap; i++)
						{
							targetRegInfo.AddReg(regsNode->Name(), i);
						}
					}
					// else error.. .have to specify the capacity
					else
					{
						printf("Capacity of %s not specified\n", regsNode->Name());
						// assert(0);
					}
				}
			}
		}
	}
}

void genTargetRegInfoFromVar(TargetRegInfoMap &targetRegInfoMap, NodePtr varNode, Symtab st)
{
	// Choose only the vars that have datatype information
	// These are the ones that contain information about register mappings
				
	if (!(varNode->ExistsChild(nke_datatype)))
	{
		return;
	}

	// Now this varNode contains a datatype
	// 

	// Make the targetRegInfo
	TargetRegInfo targetRegInfo(varNode->Name());
	
	// The datatype is the name field of the datatype node
	// make the tempVarNameDataTypeMap
	//
	tempVarNameDataTypeMap[varNode->Name()] = varNode->Child(nke_datatype)->Name();
	
	// Get the register information in the reg_list node
	NodePtr regListNode = varNode->Child(nke_reg_list);
				
	for (NodeList::iterator i = regListNode->Children()->begin(); 
	i != regListNode->Children()->end(); i++)
	{
		genRegInfoFromRegsNode(targetRegInfo, (*i), st);
	}

	targetRegInfoMap[targetRegInfo.VarName()] = targetRegInfo;	
}


// returns 1 if successful, else 0
int genTargetRegInfo(TargetRegInfoMap &targetRegInfoMap, NodePtr ir, Symtab st)
{
	if (ir->Kind() != nke_expression)
	{
		printf("The root node is of the wrong type\n");
		return 0;
	}
	
	// First collect the information in the operation_section
	// also make the tempVarNameDataTypeMap
	NodePtr operationSectionNode = ir->Child(nke_operation_section);
	
	// We have the operation section node.
	// There can be many var_group nodes in this
	
	for (NodeList::iterator varGroupIter = operationSectionNode->Children()->begin();
	varGroupIter != operationSectionNode->Children()->end();
	varGroupIter++)
	{
		if ((*varGroupIter)->Kind() == nke_var_group)
		{
			// for each var in a var_group node
			for (NodeList::iterator varIter = (*varGroupIter)->Children()->begin();
			varIter != (*varGroupIter)->Children()->end();
			varIter++)
			{
				// for each var, collect the information in the targetRegInfo
				genTargetRegInfoFromVar(targetRegInfoMap, (*varIter), st);	
			}
		}
	}

	// Now collect the information from opmapping section
	// use the tempVarNameDataTypeMap
	NodePtr operandMappingNode = ir->Child(nke_opmapping_section)->Child(nke_operand_mapping);

	// for each of the operand map
	for (NodeList::iterator operandMapIter = operandMappingNode->Children()->begin();
	operandMapIter != operandMappingNode->Children()->end();
	operandMapIter++)
	{
		// Get the varName, and targetClassType
		MyString varName = (*operandMapIter)->Child(nke_target)->Name();
		MyString targetClassType = (*operandMapIter)->Child(nke_generic)->Child(nke_classtype)->Name();

		// Add the targetClassName
		targetRegInfoMap[varName].AddTargetClassAndDataType(targetClassType, tempVarNameDataTypeMap[varName]);
	}

		// we have allowed for recursive definitions
	// thus definitions like
	// (double_retval (DATATYPE DOUBLE)(REGS double1_retval double2_retval))
	// are also possible
	
	// in such a case, the ir contains
    // nke_var double_retval -1
    //  nke_datatype DOUBLE -1
    //  nke_reg_list reg_list -1
    //   nke_regs double1_retval -1
    //   nke_regs double2_retval -1

	printTargetRegInfo(targetRegInfoMap);

	// we call the reduceTargetRegInfoMap here
    reduceTargetRegInfoMap(targetRegInfoMap, ir);
	
	// now print and see
	printTargetRegInfo(targetRegInfoMap);


	return 1;
}

int contains(TargetRegInfoMap targetRegInfoMap, MyString varName)
{
	for (TargetRegInfoMap::iterator i = targetRegInfoMap.begin();
	i != targetRegInfoMap.end(); i++)
	{
		if (varName == (*i).first)
			return 1;
	}
	return 0;
}

void printTargetRegInfo(TargetRegInfoMap targetRegInfoMap)
{
	printf("\n################################################################\n");
	for (TargetRegInfoMap::iterator i = targetRegInfoMap.begin();
	i != targetRegInfoMap.end();
	i++)
	{
		(*i).second.Print();
		printf("\n");
	}
	printf("################################################################\n");
}

int printTargetRegClassToRegsMappingFile(TargetRegInfoMap targetRegInfoMap)
{
	FILE *fptr;
	RegMappingList RegMap;
	char fname[] = "targetRegClassToRegsMapping.txt";

	// open the file
	if ((fptr = fopen(fname, "w")) == NULL)
	{
		printf("Could not open file: targetRegClassToRegsMapping.txt\n");
		assert(0);
	}

	TargetRegInfoMap tempTargetRegInfoMap = TargetRegInfoMapCopy(targetRegInfoMap);
	removeNoMnemonicEntries(tempTargetRegInfoMap);

	// iterate over the targetRegInfoMap and print the information
	for (TargetRegInfoMap::iterator i = tempTargetRegInfoMap.begin();
	i != tempTargetRegInfoMap.end(); i++)
	{
		(*i).second.PrintForTargetRegClassToRegsMapping(fptr,RegMap);
	}
	
	for (RegMappingList::iterator j=RegMap.begin();j!=RegMap.end();j++)
	{
		fprintf(fptr," ");
		fprintf(fptr,"(DATA_TYPE %s) (CLASS_TYPE %s)",(*j).target_datatype.getStr(),(*j).target_classtype.getStr());
		for (RecordList::iterator k = (*j).R.begin();k!=(*j).R.end();k++)
		{
			fprintf(fptr," (REG_FILE %s)",(*k).reg_mnemonic);
 			fprintf(fptr," (REGS ");
			for (intSet::iterator l=(*k).reg_num.begin();l!=(*k).reg_num.end();l++)
				fprintf(fptr,"%d ",(*l));
				fprintf(fptr,")");	
		}
		fprintf(fptr,"\n");
	}

/*************** THIS PART HAS BEEN HARD CODED **********************************/
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM1) (REG_FILE R) (REGS 4)\n");
 
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM2) (REG_FILE R) (REGS 5)\n");
 
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM3) (REG_FILE R) (REGS 6)\n");
 
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM4) (REG_FILE R) (REGS 7)\n");
 
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM5) (REG_FILE R) (REGS 8)\n");
 
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM6) (REG_FILE R) (REGS 9)\n");
 
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM7) (REG_FILE R) (REGS 10)\n");
 
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM8) (REG_FILE R) (REGS 11)\n");
 
 fprintf(fptr," (DATA_TYPE INT) (CLASS_TYPE CALL_PARM9) (REG_FILE R) (REGS 12)\n");

 /*************** THIS PART HAS BEEN HARD CODED **********************************/
 fclose(fptr);
 return 1;
}

//  ATRI MANDAL 
int printTargetRegClassesFile(TargetRegInfoMap targetRegInfoMap)
{
	FILE *fptr;
	MyStringSet DataSet,ClassSet;
	char *str="\0";
	char fname[] = "targetRegClasses.txt";

	// open the file
	if ((fptr = fopen(fname, "w")) == NULL)
	{
		printf("Could not open file: targetRegClasses.txt\n");
		assert(0);
	}

	TargetRegInfoMap tempTargetRegInfoMap = TargetRegInfoMapCopy(targetRegInfoMap);
	TargetRegInfoMap dummy;
	removeNoMnemonicEntries(tempTargetRegInfoMap);

    
    // iterate over the targetRegInfoMap and print the information
	for (TargetRegInfoMap::iterator i = tempTargetRegInfoMap.begin();
	i != tempTargetRegInfoMap.end(); i++)
	{
		(*i).second.CollectDataForTargetRegClasses(DataSet);
	}

    for (i = tempTargetRegInfoMap.begin();
	i != tempTargetRegInfoMap.end(); i++)
	{
		(*i).second.CollectClassForTargetRegClasses(ClassSet);
	}

		
	int count_datatypes = DataSet.size();
	fprintf(fptr,"DATA_TYPES %d",count_datatypes); 
	
	for (MyStringSet::iterator z = DataSet.begin(); z != DataSet.end(); z++) 
	{
		fprintf(fptr,"\n%s",(*z).getStr());
	}

	int count_classtypes = ClassSet.size();
	fprintf(fptr,"\nCLASS_TYPES %d",count_classtypes +10); /* 10 has been added temporarily to make way for hardcoded data */
	
	for (z = ClassSet.begin(); z != ClassSet.end(); z++) 
	{
		fprintf(fptr,"\n%s",(*z).getStr());
	}

	/******************* THIS PART HAS BEEN HARDCODED *************************/
	fprintf(fptr,"\nPC\nCALL_PARM1\nCALL_PARM2\nCALL_PARM3\nCALL_PARM4\nCALL_PARM5\nCALL_PARM6\nCALL_PARM7\nCALL_PARM8\nCALL_PARM9");
/******************* THIS PART HAS BEEN HARDCODED *************************/
   	fclose(fptr);
    
	
	return 1;
}

//ATRI MANDAL
int printTargetRegFileList(NodePtr ir)
{
	FILE *fptr;
	RecordList reg_node_list;
	char fname[] = "targetRegFileList.txt";

	// open the file
	if ((fptr = fopen(fname, "w")) == NULL)
	{
		printf("Could not open file: targetRegFileList.txt\n");
		assert(0);
	}

	if (ir->Kind() != nke_expression)
	{
		printf("The root node is of the wrong type\n");
		return 0;
	}
	
	//Find the storage section Node
	NodePtr storageSectionNode = ir->Child(nke_storage_section);
	for (NodeList::iterator sIter = storageSectionNode->Children()->begin();
	sIter != storageSectionNode->Children()->end(); sIter++)
	{
		MyString storageType = (*sIter)->Child(nke_storagetype)->Name();
		MyString storageName = (*sIter)->Name();
		if (storageType == "VirtualRegFile")
		{
			NodePtr n = (*sIter);

			int storageWidth = atoi((*sIter)->Child(nke_storagewidth)->Name());

			MyString storageMnemonic = (*sIter)->Child(nke_mnemonic)->Name();

			int regFileCode = 0;
			if (storageName == "GPRFile")
				regFileCode = 1;
			else if (storageName == "FPRFile")
				regFileCode = 2;
			//else
			//	cout << "Error" << endl;
			
			fprintf(fptr, "%s %d %d\n", storageMnemonic.getStr(), storageWidth, regFileCode);
		}
	}

	fclose(fptr);
	return 1;
}

int printRegClassToRegClassMappingFile(TargetRegInfoMap targetRegInfoMap, NodePtr node)
{
	FILE *fptr;

	// open the file
	if ((fptr = fopen("regClassToRegClassMapping.txt", "w")) == NULL)
	{
		printf("Could not open file: regClassToRegClassMapping.txt\n");
		assert(0);
	}

	// Make a reduced copy of targetRegInfoMap
	TargetRegInfoMap tempTargetRegInfoMap = TargetRegInfoMapCopy(targetRegInfoMap);
	removeNoMnemonicEntries(tempTargetRegInfoMap);

	// for each entry in the operand_mapping, 
	// we have to print the target classtype and datatype
	NodePtr operandMappingNode = node->Child(nke_opmapping_section)->Child(nke_operand_mapping);

	// for each of the operand map
	for (NodeList::iterator operandMapIter = operandMappingNode->Children()->begin();
	operandMapIter != operandMappingNode->Children()->end();
	operandMapIter++)
	{
		// Get the varName, and targetClassType
		MyString varName = (*operandMapIter)->Child(nke_target)->Name();
		MyString genericDataType = (*operandMapIter)->Child(nke_generic)->Child(nke_datatype)->Name();
		MyString genericClassType = (*operandMapIter)->Child(nke_generic)->Child(nke_classtype)->Name();


		// Make a reduced copy of targetRegInfoMap
		TargetRegInfoMap tempTargetRegInfoMap = TargetRegInfoMapCopy(targetRegInfoMap);
	removeNoMnemonicEntries(tempTargetRegInfoMap);

		// Print the targetClassType and targetDataType from the Map
		if (contains(tempTargetRegInfoMap, varName))
			tempTargetRegInfoMap[varName].PrintForRegClassToRegClassMapping(fptr, genericDataType, genericClassType);
	}
	
	fclose(fptr);
	return 1;
}


NodePtr getDefOfOperation(NodePtr node, Symtab st, NodePtr risaOperation)
{
	// the parse tree of EXPRESSION is wierd.
	// 1. An operation can come in any number of oproups
	// 2. and because of the grammar, it creates multiple definitions of it
	// 3. But strangely the definition is found in the uselist.
	
	// so let's search through all

	SymbolPtr sym = st.FindSymbol(risaOperation->Name());
	NodeList *defList = sym->GetDefList();

	for (NodeList::iterator i = defList->begin(); i != defList->end(); i++)
	{
		if ((*i)->Kind() == nke_opcode)
		{
			for (NodeList::iterator j = (*i)->Children()->begin(); 
			j != (*i)->Children()->end(); j++)
			{
				if ((*j)->Kind() == nke_operands)
				{
					return (*i);
				}
			}
		}
	}

	return NULL;
}	


char *getVarNameForMapVar(MyString mapVar, NodePtr risaDefOp)
{

	if (risaDefOp->Kind() == nke_opcode)
	{
		for (NodeList::iterator i = risaDefOp->Child(nke_operands)->Children()->begin();
		i != risaDefOp->Child(nke_operands)->Children()->end(); i++)
		{
			if (strcmp(mapVar.getStr(), (*i)->Name()) == 0)
				return (*i)->Child(nke_identifier)->Name();
		}
	}
	return NULL;
}


void makeOpFormatList(MyStringVectorVector& opFormatVecVec, 
					  ClassTypeDataTypePairListVector destVec, 
					  ClassTypeDataTypePairListVector sourceVec)
{
	for (int i=0;i<destVec.size();i++)
	{
		MyStringVector opFormatVec;
		ClassTypeDataTypePairList cdPairList = destVec[i];
		
		for (ClassTypeDataTypePairList::iterator i1 = cdPairList.begin(); i1 != cdPairList.end(); i1++)
		{
			ClassTypeDataTypePair cdPair = (*i1);
			
			MyString opFormat = cdPair.TargetClassType();
			opFormat.concat(":");
			opFormat.concat(cdPair.TargetDataType().getStr());
			
			opFormatVec.push_back(opFormat);
		}

		opFormatVecVec.push_back(opFormatVec);
	}

	for (int j=0;j<sourceVec.size();j++)
	{
		MyStringVector opFormatVec;
		ClassTypeDataTypePairList cdPairList = sourceVec[j];
		
		for (ClassTypeDataTypePairList::iterator i1 = cdPairList.begin(); i1 != cdPairList.end(); i1++)
		{
			ClassTypeDataTypePair cdPair = (*i1);
			
			MyString opFormat = cdPair.TargetClassType();
			opFormat.concat(":");
			opFormat.concat(cdPair.TargetDataType().getStr());
			
			opFormatVec.push_back(opFormat);
		}

		opFormatVecVec.push_back(opFormatVec);
	}
}


void SetCapacity(MyStringVectorVector opFormatVecVec, int capacity[])
{
	for (int i=0;i<opFormatVecVec.size();i++)
	{
		int count = 0;
		MyStringVector opFormatVec = opFormatVecVec[i];
		for (int j=0;j<opFormatVec.size();j++)
			count++;
		capacity[i] = count-1;
	}
}


void InitCurrentValue(int currentValue[], int count)
{
	for (int i=0;i<count;i++)
		currentValue[i] = 0;
}


void PrintFormat(FILE *fptr, MyString opName, int size, MyStringVectorVector opFormatVecVec, int currentValue[], int destNo, int sourceNo)
{
	fprintf(fptr, "%d~", operationsCount);
	fprintf(fptr, "%s~", opName.getStr());
	fprintf(fptr, "%d~", size);

	int no_of_dests = destNo;
	int no_of_sources = opFormatVecVec.size() - destNo;

	fprintf(fptr, "DST~");
	fprintf(fptr, "%d~",destNo); 
	if (no_of_dests == 0)
		fprintf(fptr, "NULL~");
	else
	{
		for (int i=0;i<no_of_dests;i++)
		{
			MyStringVector opFormatVec = opFormatVecVec[i];
			if (i == no_of_dests-1)
				fprintf(fptr, "%s~",opFormatVec[currentValue[i]].getStr());
			else
				fprintf(fptr, "%s,",opFormatVec[currentValue[i]].getStr());
		}
	}
	
	fprintf(fptr, "SRC~");
	fprintf(fptr, "%d~", sourceNo);
	if (no_of_sources == 0)
		fprintf(fptr, "NULL");
	else
	{
		for (int i=no_of_dests;i<no_of_dests+no_of_sources;i++)
		{
			MyStringVector opFormatVec = opFormatVecVec[i];
			if (i == no_of_dests+no_of_sources-1)
				fprintf(fptr, "%s",opFormatVec[currentValue[i]].getStr());
			else
				fprintf(fptr, "%s,",opFormatVec[currentValue[i]].getStr());
		}
	}

	fprintf(fptr, "\n");
	operationsCount++;
}


void increment(int currentValue[], int capacity[], int size)
{
	int i = size-1;
	int carry = 1;
	while((i>=0)&&(carry==1))
	{
		if (currentValue[i] < capacity[i])
		{
			currentValue[i]++;
			carry = 0;
		}
		else if (currentValue[i] == capacity[i])
		{
			currentValue[i] = 0;
			i--;
		}
		else
		{
			printf("WE HAVE AN ERROR\n");
				exit(0);
		}
	}
}

int are_equal(int currentValue[], int capacity[], int size)
{
	for (int i=0;i<size;i++)
	{
		if (currentValue[i] != capacity[i])
			return 0;
	}
	return 1;
}

void PrintAllFormats(FILE *fptr, MyString opName, int size, MyStringVectorVector OpFormatVecVec, int destNo, int sourceNo)
{
	int capacity[10];
	int currentValue[10];
	
	SetCapacity(OpFormatVecVec, capacity);
	InitCurrentValue(currentValue, OpFormatVecVec.size());

	PrintFormat(fptr, opName, size, OpFormatVecVec, currentValue, destNo, sourceNo);
	
	while(!are_equal(currentValue, capacity, OpFormatVecVec.size()))
	{
		increment(currentValue, capacity, OpFormatVecVec.size());
		PrintFormat(fptr, opName, size, OpFormatVecVec, currentValue, destNo, sourceNo);
	}
}


int FindNoOfOps(ClassTypeDataTypePairListVector destVec, 
					  ClassTypeDataTypePairListVector sourceVec)
{
	int count = 1;
	for (int i=0;i<destVec.size();i++)
	{
		count *= destVec[i].size();
	}

	for (int j=0;j<sourceVec.size();j++)
	{
		count *= sourceVec[j].size();
	}

	return count;
}

void PrintAllMappings(FILE *fptr,
					  MyString risaOpName,
					  MyString targetOpName,
					  ClassTypeDataTypePairListVector risaDestVec, 
					  ClassTypeDataTypePairListVector risaSourceVec, 
					  ClassTypeDataTypePairListVector targetDestVec, 
					  ClassTypeDataTypePairListVector targetSourceVec)
{
	MyStringVectorVector risaOperandFormatVecVec;
	MyStringVectorVector targetOperandFormatVecVec;

	makeOpFormatList(risaOperandFormatVecVec, risaDestVec, risaSourceVec);
	makeOpFormatList(targetOperandFormatVecVec, targetDestVec, targetSourceVec);
	
	int numRisaOps = FindNoOfOps(risaDestVec, risaSourceVec);
	int numTargetOps = FindNoOfOps(targetDestVec, targetSourceVec);

	int start = operationsCount;

	PrintAllFormats(fptr, risaOpName, 2, risaOperandFormatVecVec, risaDestVec.size(), risaSourceVec.size());
	PrintAllFormats(fptr, targetOpName, 4, targetOperandFormatVecVec, targetDestVec.size(), targetSourceVec.size());
	
	// now we make the mappings
	//
	for (int i=start;i<start+numTargetOps;i++)
	{
		targetToRisaMap[i] = numTargetOps + (i/numTargetOps)*numRisaOps + i%numTargetOps;
	}
}

void PrintOperationsForrISARegisterMappingFile(FILE *fptr, NodePtr risaOperation, 
											   NodePtr targetOperation, NodePtr ir, 
											   Symtab st, TargetRegInfoMap targetRegInfoMap)
{
	MyString risaOpName = risaOperation->Name();
	MyString targetOpName = targetOperation->Name();

	// get the definition of risaOperation and targetOperation
	NodePtr risaDefOp = getDefOfOperation(ir, st, risaOperation);
	NodePtr targetDefOp = getDefOfOperation(ir, st, targetOperation);
				
	// all the children of these are operands
	NodePtr risaOperands = risaDefOp->Child(nke_operands);
	NodePtr targetOperands = targetDefOp->Child(nke_operands);
	
	// strategy :
	// we will get the mapvars from risaOperation
	// lookup the mapvar in risaDefOp to get the varName


	// make the risaVarNameList
	ClassTypeDataTypePairListVector risaDestCTDTPairListVec;
	ClassTypeDataTypePairListVector risaSourceCTDTPairListVec;
	
	for (NodeList::iterator i = risaOperation->Children()->begin();
		i != risaOperation->Children()->end(); i++)
	{
		// get the varName for mapVar
		MyString mapVar = (*i)->Name();
		MyString varName = getVarNameForMapVar(mapVar, risaDefOp);
		ClassTypeDataTypePairList cdPairList = targetRegInfoMap[varName].CDPairList();		

		if ((mapVar == "_SOURCE_1_")||(mapVar == "_SOURCE_2_")||(mapVar == "_SOURCE_3_"))
			risaSourceCTDTPairListVec.push_back(cdPairList);
		else if ((mapVar == "_DEST_"))
			risaDestCTDTPairListVec.push_back(cdPairList);
		else
			printf("operand is niether a source nor a destination\n");
	}

	// make the targetVarNameList
	ClassTypeDataTypePairListVector targetDestCTDTPairListVec;
	ClassTypeDataTypePairListVector targetSourceCTDTPairListVec;
	
	for (NodeList::iterator j = targetOperation->Children()->begin();
		j != targetOperation->Children()->end(); j++)
	{
		// get the varName for mapVar
		MyString mapVar = (*j)->Name();
		MyString varName = getVarNameForMapVar(mapVar, targetDefOp);
		ClassTypeDataTypePairList cdPairList = targetRegInfoMap[varName].CDPairList();		

		if ((mapVar == "_SOURCE_1_")||(mapVar == "_SOURCE_2_")||(mapVar == "_SOURCE_3_"))
			targetSourceCTDTPairListVec.push_back(cdPairList);
		else if ((mapVar == "_DEST_"))
			targetDestCTDTPairListVec.push_back(cdPairList);
		else
			printf("operand is niether a source nor a destination\n");
	}

	PrintAllMappings(fptr, risaOpName, targetOpName, 
		risaDestCTDTPairListVec, risaSourceCTDTPairListVec, 
		targetDestCTDTPairListVec, targetSourceCTDTPairListVec);
}
				
int printrISARegisterMappingFile(TargetRegInfoMap globalTargetRegInfoMap, Symtab st, NodePtr node)
{
	FILE *fptr;

	// open the file
	if ((fptr = fopen("rISARegisterMapping.txt", "w")) == NULL)
	{
		printf("Could not open file: rISARegisterMapping.txt\n");
		assert(0);
	}
	
	// get to the opcode mapping section
	NodePtr opcodeMappingNode = node->Child(nke_opmapping_section)->Child(nke_opcode_mapping);

	// pick out the RISA -> TARGET mappings
	for (NodeList::iterator opcodeMapIter = opcodeMappingNode->Children()->begin();
	opcodeMapIter != opcodeMappingNode->Children()->end(); 
	opcodeMapIter++)
	{
		// we have the nke_opcode_map nodes
		// we have to find the nodes, whose children have at least one nke_target_risa nodes.
		
		for (NodeList::iterator operationIter = (*opcodeMapIter)->Children()->begin();
		operationIter != (*opcodeMapIter)->Children()->end(); 
		operationIter++)
		{

			if ((*operationIter)->Kind() == nke_target_risa)
			{
				// this is the risa to target mapping
				//
				NodePtr risaOperation = (*opcodeMapIter )->Child(nke_target_risa)->Child(nke_operation);
				NodePtr targetOperation = (*opcodeMapIter )->Child(nke_target)->Child(nke_operation);

				// print this
				PrintOperationsForrISARegisterMappingFile(fptr, risaOperation, targetOperation, node, st, globalTargetRegInfoMap);	
			}
		}
	}

	// Now print the targetToRisaMap
	//
	fprintf(fptr, "maps\n");
	for (IntIntMap::iterator i=targetToRisaMap.begin(); i != targetToRisaMap.end();i++)
	{
		fprintf(fptr, "%d => %d\n", (*i).second, (*i).first);
	}
	fprintf(fptr, "endMaps\n");

	fclose(fptr);
	return 1;
}	

