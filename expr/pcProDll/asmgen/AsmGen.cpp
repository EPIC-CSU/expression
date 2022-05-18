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
 *file  : AsmGen.cpp										 *
 *created : 3 July, 2000.                                    *
 *authors : Partha Biswas									 *
 *last modified :                                            *
 *                                                           *
 *compiled with : VC++                                       *
 *for more info :                                            *
 *************************************************************/

#include <stdafx.h>
#include <iostream.h>
#include <ctype.h>
#include <fstream.h>
#include <string.h>
#include <stdio.h>
#ifndef __UNIX__
#include <crtdbg.h>
#else
#define _ASSERT assert
#endif

#include "AsmGen.h"

#include "STLIncludes.h"
#include "MyString.h"

#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector

#include <map>

#ifdef map
#undef map
#endif 
#define map std::map

//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Functions for creation of opcodeToOpnd Map table
/////////////////////////////////////////////////////////////////////////////////


typedef vector <formatStructure> formatStructVector;

typedef map <int, formatStructVector> opndIndexTableMap;
opndIndexTableMap opndIndexTable;

typedef map <MyString, int> opcodeIndexTableMap;
opcodeIndexTableMap opcodeIndexTable;

typedef map <MyString, formatStructVector> opcodeToOpndFormatTableMap;
opcodeToOpndFormatTableMap opcodeToOpndFormatTable;

// Function Prototypes
void printTheWholeTable();

void addEachFormatStructToVector(formatStructure, formatStructVector &);
void addToOpndIndexTableMap(int, formatStructVector);
void addToOpcodeIndexTableMap(MyString, int);
void fuseOpcodeAndOpndMapTable();
int countNoOfOperands(MyString);
void printByFormatStr(char *, char *, ostream &);
void printRegisterAllocBlockFooterForBranch(ostream &out);
void printRegisterAllocBlockHeaderForBranch(ostream &out);
void printByFormatStr(char *formatStr, ostream &out);
extern bool PrintingASM; // from NoGUIMainSupportFuncs
extern bool DumpingIR;

// will be populated from the globalDeclarations section
MyString regPref, delim = ",";

//GG: added to try to fix the memory allocation problem, 7/17/01
extern char *dummy_space;


ofstream afout;  // debug
void createOpcodeToOpndMapTable(FILE *fptMapTable)
{
	stringVector eachLine;
	stringVector globalDecl; // for storing list of global declarations
	int opndOrOpcodeSection=0;
	char firstString[6];
	MyString regMappingInfo;
	MyString formatStringInfo;
	
	int index=-1, indexIntoOpndMapTable, lastIndexIntoOpndMapTable;
	char tempStr[200];
	
	//delete [10000000] dummy_space;
	//free(dummy_space);
	
	{
		int i;
		char *s;
		for(i=0;i<10000;i++){
			//cout << i << endl;
			s=new char[100];
		}
	}

	afout.open("debugFile");

	while(fgets(tempStr, 200, fptMapTable) != NULL){
		if(strstr(tempStr, "textSectionHeader") != NULL){
			while(strstr(fgets(tempStr, 200, fptMapTable), "endTextSectionHeader") == NULL);
		}
		else if(strstr(tempStr, "textSectionFooter") != NULL){
            while(strstr(fgets(tempStr, 200, fptMapTable), "endTextSectionFooter") == NULL);
		}
		else if(strstr(tempStr, "regAllocBlockSectionHeader") != NULL){
            while(strstr(fgets(tempStr, 200, fptMapTable), "endRegAllocBlockSectionHeader") == NULL);
		}
		else if(strstr(tempStr, "regAllocBlockSectionFooter") != NULL){
            while(strstr(fgets(tempStr, 200, fptMapTable), "endRegAllocBlockSectionFooter") == NULL);
		}
		else if(strstr(tempStr, "globalDeclarations") != NULL){
            while(strstr(fgets(tempStr, 200, fptMapTable), "endGlobalDeclarations") == NULL)
			{
				globalDecl.push_back(MyString(tempStr));
			}
		}
		else{
			// cout << tempStr << endl;
			eachLine.push_back( MyString(tempStr) );
		}	
	}

	// reading the global declaration section
	for(stringVector :: iterator gi = globalDecl.begin(); gi != globalDecl.end(); gi++)
	{
		char *temp = (char *) malloc(strlen((*gi).getStr())+1);
		MyString globalVar;
		MyString globalVal;
		int i = 0;
		
		strcpy(temp, (*gi).getStr());
		globalVar = strtok(temp, "=");
		// right now, we have only registerPrefix and delimiter as 
		// known global variables in the assembly format file
		if( globalVar == "registerPrefix" )
		{
			regPref = strtok(NULL, "=");
			temp = (char *) malloc( strlen(regPref.getStr())+1 );
			strcpy(temp, regPref.getStr());
			char *p = strchr(temp, '\n');
			*p = '\0';
			regPref = temp;
		}
		else if( globalVar == "delimiter" )
		{
			delim = strtok(NULL, "=");
			temp = (char *) malloc( strlen(delim.getStr())+1 );
			strcpy(temp, delim.getStr());
			char *p = strchr(temp, '\n');
			*p = '\0';
			delim = temp;
		}
		else
		{
			cout << "Such a global variable is unknown to the assembler !" << endl;
			assert(0);
		}
	}

	/*
	for(stringVector :: iterator i = eachLine.begin(); i != eachLine.end(); i++){
		afout << (*i).getStr() << endl;
	}
	*/
	// Now eachLine contains the lines in the operandsMappingFile
	formatStructVector formatVector;
	for(stringVector :: iterator i = eachLine.begin(); i != eachLine.end(); i++){
		char *temp = (char *) malloc(strlen((*i).getStr())+1);
		
		strcpy(temp, (*i).getStr());
		if(strstr(temp, "endOpndMap")!= NULL){
			continue;
		}
		if(temp[0] == ' '){
			continue;
		}
		
		if(strstr(temp, "opndMap")!=NULL){
			opndOrOpcodeSection = 1;
			continue;
		}
		if(strstr(temp, "opcodeMap")!=NULL){
			opndOrOpcodeSection = 2;
			continue;
		}
		// Build the operand map table
		if(opndOrOpcodeSection == 1){
	
			strcpy(firstString, strtok(temp, "~"));
			if(!isdigit(firstString[0]) && firstString[0] != ';') continue;
			// above will be removed...
			if(temp[0] != ';'){
				index++;  // when first char is a number
				indexIntoOpndMapTable = atoi(firstString);
			}
			regMappingInfo = strtok(NULL, "~");
			//afout << "regMappingInfo->" << regMappingInfo.getStr() << endl;
			formatStringInfo = strtok(NULL, "~");
			//afout << "formatStringInfo->" << formatStringInfo.getStr() << endl;
			formatStructure eachFormatStruct;
			eachFormatStruct.setRegMapping(regMappingInfo);
			eachFormatStruct.setFormatString(formatStringInfo);
			eachFormatStruct.setOrder(formatStringInfo);
			
			
			if(index > 0 && temp[0] != ';'){  // Add to the maptable the format vector created last time
				addToOpndIndexTableMap(lastIndexIntoOpndMapTable, formatVector);
			}
			
			if(isdigit(firstString[0])){  // Create a new vector when a new block starts with a digit				
				formatVector.clear();
			}
			addEachFormatStructToVector(eachFormatStruct, formatVector);
			lastIndexIntoOpndMapTable = indexIntoOpndMapTable;
		}
		// Build the opcode map table
		else if(opndOrOpcodeSection == 2){
			// Create map from opcode to opndMap
			// afout << "opcode map->" << (*i).getStr() << endl;
			MyString opCode, indexIntoOpcodeMapTable;
			opCode = strtok(temp, " ");
			if(opCode == "endOpcodeMap") break;
			indexIntoOpcodeMapTable = strtok(NULL, " ");
			index = atoi(indexIntoOpcodeMapTable.getStr());
			addToOpcodeIndexTableMap(opCode, index);
		}
				
	}
	// Finishing touch to the opnd map table
	addToOpndIndexTableMap(indexIntoOpndMapTable, formatVector);
	// Generate final mapping table containing map from opcode to operand format
	fuseOpcodeAndOpndMapTable();
	// debug
	printTheWholeTable();
	afout.close();
	return;
}

// Sets the dest/src to reg/imm mapping
void formatStructure::setRegMapping(MyString regMappingInfo)
{
	char **eachMapping;
	MyString first, second, temp1;
	int i, noOfOperands;
	char *temp = (char *) malloc(strlen(regMappingInfo.getStr())+1);
	strcpy(temp, regMappingInfo.getStr());
	
	noOfOperands = countNoOfOperands(regMappingInfo);
	eachMapping = (char **) malloc (noOfOperands*sizeof(char*));
	
	temp1 = strtok(temp, ",");
	eachMapping[0] = (char *) malloc (strlen(temp1.getStr())+1);
	strcpy(eachMapping[0], temp1.getStr());
	
	for(i = 1; i < noOfOperands; i++){
		temp1 = strtok(NULL, ",");
		eachMapping[i] = (char *) malloc (strlen(temp1.getStr())+1);
		strcpy(eachMapping[i], temp1.getStr());
	}

	for(i = 0; i < noOfOperands; i++){
		first = strtok(eachMapping[i], "=");
		second = strtok(NULL, "=");
		if(strstr(first.getStr(), "dst") != NULL)
			dstRegMapping[first] = second;
		else
			srcRegMapping[first] = second;
	}
/*  
	afout << "printing for " << regMappingInfo.getStr() << endl;

	for(stringToStringMap :: iterator j = regMapping.begin();
	j != regMapping.end(); j++){
		MyString printFirst, printSecond;
		printFirst = (*j).first;
		printSecond = (*j).second;
		afout << "first->" << printFirst.getStr() << endl;
		afout << "second->" << printSecond.getStr() << endl;
		afout << endl;
	}
*/
	return;
}

// sets the format string for printing assembly instructions
void formatStructure::setFormatString(MyString formatStringInfo)
{
	char *temp = (char *) malloc(strlen(formatStringInfo.getStr())+1);
	char *temp1 = (char *) malloc(strlen(formatStringInfo.getStr())+1);
	char temp2[100];
	MyString firstMapElement;  // in the regMappingInfo
	strcpy(temp,  formatStringInfo.getStr());
	int i = 0, k = 0;
	if(temp[i] == '\"')
		temp1[k++] = temp[i++];

	while(temp[i] != '\"'){
		if(temp[i] ==  '<'){
			i++;  // Move the pointer past the '<' character
			int j = 0;
			while(temp[i] != '>')
				temp2[j++] = temp[i++];
			temp2[j] = '\0';
			i++;  // Move the pointer past the '>' character
			firstMapElement = temp2;
			if(firstMapElement == "opcode"){
				temp1[k++] = '%';
				temp1[k++] = 's';
			}
			else if(firstMapElement == "regAllocBlockHeader"){
				temp1[k++] = '%';
				temp1[k++] = 's';
			}
			else if(firstMapElement == "regAllocBlockFooter"){
				temp1[k++] = '%';
				temp1[k++] = 's';
			}
			// dstRegMapping can never map to "imm"
			else if( strstr(firstMapElement.getStr(), "src") != NULL ){
				// supporting variable no. of arguments
				if( strchr(firstMapElement.getStr(), '*' ) != NULL )
				{
					temp1[k++] = '%';
					temp1[k++] = '*';
				}
				else if(srcRegMapping[firstMapElement] == "imm" ){
					temp1[k++] = '%';
					temp1[k++] = 'd';
				}
				else{
					temp1[k++] = '%';
					temp1[k++] = 's';  // label or register
				}
			}
			
			else if( dstRegMapping[firstMapElement] == "imm" )
				assert("A destination operand cannot be an immediate !\n");
			// we might have to give support for variable no. of arguments in destination
			else{
				temp1[k++] = '%';
				temp1[k++] = 's';  // dstmapping
			}
		}
		else
			temp1[k++] = temp[i++];
	}
	temp1[k] = temp[i];
	temp1[++k] = '\0';
	formatString = temp1;
	/*
	afout << "format string for " << formatStringInfo.getStr() << endl;
	afout << formatString.getStr() << endl;
	*/
	return;
}

// Sets the order in which to print the operands
void formatStructure::setOrder(MyString formatStringInfo)
{
	char temp1[100];
	char *temp = (char *) malloc(strlen(formatStringInfo.getStr())+1);	
	MyString eachComp;   // Each component of an assembly instruction
	strcpy(temp, formatStringInfo.getStr());
	
	int i = 0;
	if(temp[i] == '\"') i++;
	while(temp[i] != '\"'){
		if(temp[i] == '<'){
			i++;
			int j = 0;
			while(temp[i] != '>')
				temp1[j++] = temp[i++];
			temp1[j] = '\0';
			eachComp = temp1;
			order.push_back(MyString(eachComp));
			
		}
		else i++;
	}
	/*
	afout << "Printing formatStrings :" << formatStringInfo.getStr() << ":" << endl;
	for(stringVector :: iterator j = order.begin(); 
	j != order.end(); j++)
	{
		afout << (*j).getStr() << endl;
	}
	*/
	return;
}

MyString formatStructure::getFormatString()
{
	return formatString;
}

stringVector formatStructure::getOperandOrder()
{
	return order;
}

stringToStringMap formatStructure::getSrcRegMapping()
{
	return srcRegMapping;
}

stringToStringMap formatStructure::getDstRegMapping()
{
	return dstRegMapping;
}

// augment the opnd map table
void addToOpndIndexTableMap(int index, formatStructVector formatVector)
{
	opndIndexTable[index] = formatVector;
	return;
}

// augment the opcode map table
void addToOpcodeIndexTableMap(MyString opCode, int index)
{
	opcodeIndexTable[opCode] = index;
	return;
}

// augment the format vector with the format structure
void addEachFormatStructToVector(formatStructure eachFormatStruct, formatStructVector &formatVector)
{
	formatVector.push_back(formatStructure(eachFormatStruct));
	return;
}

int countNoOfOperands(MyString inputregInfo)
{
	int n=1;
	char *temp = (char *)malloc(strlen(inputregInfo.getStr())+1);
	strcpy(temp, inputregInfo.getStr());
	strtok(temp, ",");
	while(strtok(NULL, ",") != NULL) n++;
	return n;
}

// Join the opcode and opnd map table
void fuseOpcodeAndOpndMapTable()
{
	MyString temp;
	opcodeIndexTableMap :: iterator temp_i;
	//for all the opcode to index mappings
	for(opcodeIndexTableMap :: iterator i = opcodeIndexTable.begin();i != opcodeIndexTable.end(); )
	{
		//for all the index to foramt mapping 
		for(opndIndexTableMap :: iterator j = opndIndexTable.begin();
		j != opndIndexTable.end(); j++)
		{
			//if the index from the two mappings match
			if((*i).second == (*j).first){
				//add the opcode to format mapping to the opcodeToOpndFormatTable
				temp = (*i).first;
				// cout << temp.getStr() << endl;
				// opcodeToOpndFormatTable[(*i).first] = (*j).second;
				opcodeToOpndFormatTable.insert(opcodeToOpndFormatTableMap::value_type(MyString((*i).first), (*j).second));
			}
		}
		//first save and increment, incrmeent the i
		temp_i=i;
		i++;
		//then delete the tmp (which is the old i)
		opcodeIndexTable.erase(temp_i);
	}
	// erase the unnecessary maps
    //opcodeIndexTable.erase(opcodeIndexTable.begin(), opcodeIndexTable.end());
    opndIndexTable.erase(opndIndexTable.begin(), opndIndexTable.end());
	return;
}


// Routine for checking whether the opCodeToOpndMapTable is created correctly
void printTheWholeTable()
{
	MyString opCode;
	stringToStringMap srmap;
	stringToStringMap drmap;
	MyString fs;
	stringVector oorder;
	formatStructVector opndFormatStructVector;
	for(opcodeToOpndFormatTableMap :: iterator i = opcodeToOpndFormatTable.begin();
	i != opcodeToOpndFormatTable.end(); i++)
	{
		opCode = (*i).first;
		afout << "Printing the details for... " << opCode.getStr() << endl;
		opndFormatStructVector = (*i).second;
		int k = 0;
		for( formatStructVector :: iterator j = opndFormatStructVector.begin();
		j != opndFormatStructVector.end(); j++)
		{
			afout << "Format " << k++ << ":" << endl;
			afout << "Register mapping->" << endl;
			srmap = (*j).getSrcRegMapping();
			drmap = (*j).getDstRegMapping();
			for(stringToStringMap :: iterator l = srmap.begin();
			l != srmap.end(); l++){
				MyString printFirst, printSecond;
				printFirst = (*l).first;
				printSecond = (*l).second;
				afout << "first->" << printFirst.getStr() << endl;
				afout << "second->" << printSecond.getStr() << endl;
				afout << endl;
			}
			for(stringToStringMap :: iterator m = drmap.begin();
			m != drmap.end(); m++){
				MyString printFirst, printSecond;
				printFirst = (*m).first;
				printSecond = (*m).second;
				afout << "first->" << printFirst.getStr() << endl;
				afout << "second->" << printSecond.getStr() << endl;
				afout << endl;
			}
			afout << "--------------------------------------------------" << endl;
			afout << "Format string->" << endl;
			fs = (*j).getFormatString();
			afout << fs.getStr() << endl;
			afout << "--------------------------------------------------" << endl;
			afout << "operand ordering->" << endl;
			oorder = (*j).getOperandOrder();
			for(stringVector :: iterator h = oorder.begin(); 
			h != oorder.end(); h++)
			{
				afout << (*h).getStr() << endl;
			}
			afout << "--------------------------------------------------" << endl;
		}
	    afout << "--------------------------------------------------" << endl;
	}
}

		
//\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\\
// Functions for final generation of assembly
/////////////////////////////////////////////////////////////////////////////////
/*
targetInstructionList asmInstructionList;  // will contain the list of target assembly instructions


targetInstruction::setLabel(MyString label)
{
	Label = label;
	return;
}
*/		

#include "BaseArgument.h"
#include "RegisterArg.h"
typedef map<MyString, BaseArgument *> regToArgMap;

int setPrintOrderAndFormatStr(MyString op, stringVector dstVector, stringVector srcVector, 
							   stringVector &printOrder, MyString &formatStr)
{
    MyString opCode;
	stringToStringMap srmap, drmap;
	formatStructVector opndFormatStructVector;
	for(opcodeToOpndFormatTableMap :: iterator i = opcodeToOpndFormatTable.begin();
	i != opcodeToOpndFormatTable.end(); i++)
	{
		opCode = (*i).first;
		if(opCode != op) continue;
		
		opndFormatStructVector = (*i).second;
		int k = 0, noOfVariableSources = 0;
		int hasVarArg=0;
		for( formatStructVector :: iterator j = opndFormatStructVector.begin();
		j != opndFormatStructVector.end(); j++)
		{
			srmap = (*j).getSrcRegMapping();
			drmap = (*j).getDstRegMapping();
			
			int matchFound = 1;
			if( srmap.size() != srcVector.size() ) continue;
			if( drmap.size() != dstVector.size() ) continue;
			
			
			
			stringToStringMap :: iterator l;
			stringVector :: iterator srcRegMapIterator;
			int srcNo;
			int noOfKnownSources = 0;
			
			for( srcRegMapIterator = srcVector.begin(), srcNo = 1; srcRegMapIterator != srcVector.end(); 
			srcRegMapIterator++, srcNo++)
			{
				
				char tempStr[6];
				sprintf(tempStr, "src%d", srcNo);
				for( l = srmap.begin(); l != srmap.end(); l++ )
				{
					MyString firstElem, secondElem;
					firstElem = (*l).first;
					if(strstr(firstElem.getStr(), "src") == NULL)
						assert("Comparing incompatible elements !\n");
					if( strcmp( firstElem.getStr(), tempStr ) == 0 )
					{
						
						secondElem = (*l).second;
						
						// PB: 05/01 : Accounting for function calls
						// in which case src* = reg and src1 = lab
						
						
						// right now only source list is assumed to have variable format spec
						// later an extension might be needed for this function to return
						// both the variable no. of sources and destinations whose format
						// specification varies from known ones.
						
						// so then we might have to return a structure having noOfVariableSources
						// and noOfVariableDests as elements
						
						if( secondElem != (*srcRegMapIterator) && !hasVarArg)
						{
							matchFound = 0;
							break;
						}
						
						noOfKnownSources++;
					}
					else if( firstElem == "src*" )
					{
						hasVarArg = 1;
					}
				}
				
			}
			hasVarArg = 0;
			if( matchFound == 1){
				formatStr = (*j).getFormatString();				
				printOrder = (*j).getOperandOrder();
				noOfVariableSources = srcVector.size() - noOfKnownSources;
				return noOfVariableSources;
			}		
		}	
		assert("No match found !\n");
	}
	return 0;
}

void addToRegArgMap(MyString reg, BaseArgument *arg, regToArgMap &regToArg)
{
	regToArg[reg] = arg;
	return;
}

void printInOrderUsingFormatStr(ostream &out, MyString opCode, regToArgMap regToArg, stringVector printOrder, MyString formatStr, int noOfVarSrcs)
{
	if( *(formatStr.getStr()) == '\0' ) assert(0);
	char *temp = (char *) malloc(strlen(formatStr.getStr())+1);
	char *temp1;
	int i = 0;
	MyString token, srcToken = "\0";
	BaseArgument *argument;
	stringVector :: iterator j = printOrder.begin();
	int noOfSources = 0;
	strcpy(temp, formatStr.getStr());
	if(temp[i] == '\"') i++;
	
	while(temp[i] != '\"'){
		if(temp[i] == '%'){
			i++;  // read %s or %*
			
			token = (*j);
			if(token == "opcode"){
				temp1 = (char *) malloc(strlen(opCode.getStr())+1);
				strcpy(temp1, opCode.getStr());
				while(*temp1 != '\0')
				{
					if(*temp1 == '_')
					{
						if(*(temp1+1) == '_') out << "_";  // replace "__" by "_"
						else out << "_";    
					}// replace "_" by "."
					else if(*temp1 == '2'){  // convert la2 to la
						++temp1;
						break;                                            
					}
					else
						out << *temp1;
					++temp1;
				}
				temp1[0] = '\0';
				//out << opCode.getStr();
			}
			else if(token == "regAllocBlockHeader")
			{
				printRegisterAllocBlockHeaderForBranch(out);
			}
			else if(token == "regAllocBlockFooter")
			{
				printRegisterAllocBlockFooterForBranch(out);
			}
			// Only sources are assumed to contain variable arguments
			// for example DEBUG_PRINTs
			else if( token == "src*" )
			{
				regToArgMap::iterator rIter;
				
				if( srcToken != "\0" )
				{
					rIter = regToArg.find(srcToken);
					rIter++;  // the token next to srcToken is the starting point for variable sources
				}
				else
				{
					rIter = regToArg.begin();
					while( rIter != regToArg.end() )
					{
						MyString tok;
						tok = (*rIter).first;
						if( strstr(tok.getStr(), "src") != NULL )
							break;
						rIter++; // get an hold to the first src operand which in this case is a variable src opnd
					}
				}
				while (rIter != regToArg.end())
				{
					// previous token found in the regToArg map
					// now print all the other entries in the map
					
					argument = (*rIter).second;
					if( argument != NULL )  // sanity check ?
					{
						
						if(argument->isRegisterArg())
						{
							if( regPref.getStr() != NULL )
								out << regPref.getStr();  // from the global decl section
							((RegisterArg *)argument)->printAsm(out);
						}
						else
						{
							argument->print(out);
						}
					}
					noOfSources++;
					if( noOfSources == noOfVarSrcs )
					{
						break;
					}
					
					rIter++;
					out << delim.getStr() << " ";  // from the global decl section
				}
			}
			else{
				argument = regToArg[token];
				if( argument != NULL ) // takes care of operations having no operands
				{
					if( strstr( token.getStr(), "src" ) != NULL )
					{
						// sets srcToken to the last obtained fixed source
						srcToken = token;
					}
					if(argument->isRegisterArg())
					{
						((RegisterArg *)argument)->printAsm(out);
					}
					else
					{
						argument->print(out);
					}
				}
			}
			
			j++;
			
		}
		else if(temp[i] == '\\'){
			i++;
			if(temp[i] == 'n') out << "\n";  // newline
			else if(temp[i] == 't') out << "\t"; // tab
		}
		else{
			out << temp[i];
		}
		i++;
	}
	return;
}

void printTextSectionHeader(char *routineName, ostream &out)
{
	FILE *fpt;	
	char formatStr[200];
	//if((fpt = fopen("operandsMappingFile.txt", "r")) ==NULL){
	if( PrintingASM == true )
	{
		if((fpt = fopen("operandsMappingFile.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
	}
	else // if( DumpingIR == true )
	{
		if((fpt = fopen("dumpIRFormat.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
	}
	while(fgets(formatStr, 200, fpt) != NULL){
		if(strstr(formatStr, "textSectionHeader") != NULL){
			while(strstr(fgets(formatStr, 200, fpt), "endTextSectionHeader") == NULL)
				printByFormatStr(formatStr, routineName, out);
			fclose(fpt);
			return;
		}
	}
}

void printByFormatStr(char *formatStr, char *actualArg, ostream &out)
{
	int index = 0;
	char tempStr[200];
	if(formatStr[0] == '\"')
		index++;
	while(formatStr[index] != '\"'){
		if(formatStr[index] == '<'){
			int j = 0;
			index++;
			while(formatStr[index] != '>'){				
				tempStr[j++] = formatStr[index++];
			}
			tempStr[j] = '\0';
			if(strcmp(tempStr, "routine") == 0){
				if(actualArg[0] == '_')
					actualArg = actualArg + 1; // Discard the leading underscore
				out << actualArg;
			}
		}
		else if(formatStr[index] == '\\'){
			index++;
			if(formatStr[index] == 'n') out << "\n";  // newline
			else if(formatStr[index] == 't') out << "\t"; // tab
		}
		else{
			out << formatStr[index];
		}
		index++;
	}	
}

void printTextSectionFooter(char *routineName, ostream &out)
{
	FILE *fpt;	
	char formatStr[200];
	//if((fpt = fopen("operandsMappingFile.txt", "r")) ==NULL){
	if( PrintingASM == true )
	{
		if((fpt = fopen("operandsMappingFile.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
	}
	else // if( DumpingIR == true )
	{
		if((fpt = fopen("dumpIRFormat.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
	}
	while(fgets(formatStr, 200, fpt) != NULL){
		if(strstr(formatStr, "textSectionFooter") != NULL){
			while(strstr(fgets(formatStr, 200, fpt), "endTextSectionFooter") == NULL)
				printByFormatStr(formatStr, routineName, out);
			fclose(fpt);
			return;
		}
	}
}


void printRegisterAllocBlockHeaderForBranch(ostream &out)
{
	FILE *fpt;	
	char formatStr[200];
	//if((fpt = fopen("operandsMappingFile.txt", "r")) ==NULL){
	if( PrintingASM == true )
	{
		if((fpt = fopen("operandsMappingFile.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
	}
	else // if( DumpingIR == true )
	{
		if((fpt = fopen("dumpIRFormat.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
	}

	while(fgets(formatStr, 200, fpt) != NULL){
		if(strstr(formatStr, "regAllocBlockSectionHeader") != NULL){
			while(strstr(fgets(formatStr, 200, fpt), "endRegAllocBlockSectionHeader") == NULL)
				printByFormatStr(formatStr, out);
			fclose(fpt);
			return;
		}
	}
}

void printRegisterAllocBlockFooterForBranch(ostream &out)
{
	FILE *fpt;	
	char formatStr[200];
	//if((fpt = fopen("operandsMappingFile.txt", "r")) ==NULL){
	if( PrintingASM == true )
	{
		if((fpt = fopen("operandsMappingFile.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
	}
	else // if( DumpingIR == true )
	{
		if((fpt = fopen("dumpIRFormat.txt", "r")) ==NULL){
			assert("Unable to open file \n");
		}
	}
	while(fgets(formatStr, 200, fpt) != NULL){
		if(strstr(formatStr, "regAllocBlockSectionFooter") != NULL){
			while(strstr(fgets(formatStr, 200, fpt), "endRegAllocBlockSectionFooter") == NULL)
				printByFormatStr(formatStr, out);
			fclose(fpt);
			return;
		}
	}
}


void printByFormatStr(char *formatStr, ostream &out)
{
	int index = 0;
	if(formatStr[0] == '\"')
		index++;
	while(formatStr[index] != '\"'){
		if(formatStr[index] == '\\'){
			index++;
			if(formatStr[index] == 'n') out << "\n";  // newline
			else if(formatStr[index] == 't') out << "\t"; // tab
		}
		else{
			out << formatStr[index];
		}
		index++;
	}	
}
