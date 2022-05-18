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
// RegSetMapping.cpp
//
// File:  		RegSetMapping.cpp
// Created:		Fri Jul 27, 2001
// Author:		Partha Biswas
// Email:		partha@ics.uci.edu, ilp@ics.uci.edu
//
// Dividing the register files into various subsets 
// 

#include "stdafx.h"
#include "RegSetMapping.h"
#include "MyString.h"
#include "RegArchModel.h"
#include "BaseArgument.h"
#include "RegisterFileList.h"
#include "IConstArg.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif


#define _SRC_ 0
#define _DST_ 1


// Global Variables
GenericToTargetMapType GenericToTargetMap;
RegSetMappingVectorType RegSetMappingVector;
opcodeToSizeMapType opcodeToSizeMap;

extern RegArchModel globalRegModel;



void ReadRegSetMapFile(FILE *fpt)
{
	
	char tempStr[200];
	char mapNo[6];
	int index;
	char keyword1[4], keyword2[4];
	char length1[6], length2[6];
	char formatStr1[100], formatStr2[100];
	int startMapping = 0;
	
	// example of the format :
	// 0~ADD~4~DST~1~ANY:INT~SRC~2~ANY:INT,ANY:INT
	// means 
	// ADD @ index 0
	// length = 4; #dsts=1; #srcs=2
	// ANY register class and INT data type

	while(fgets(tempStr, 200, fpt) != NULL)
	{
		if( strstr(tempStr, "maps") != NULL )
		{
			startMapping = 1;
			continue;
		}
		if( startMapping == 0 )
		{
			RegSetMappingType RegSet;
			if( tempStr[0] == '\n' )
				continue;
			// map no.
			strcpy(mapNo, strtok(tempStr, "~"));
			if( atoi(mapNo) == 0 )
			{
				assert("Map nos should begin with 1\n");
			}
			index = atoi(mapNo);
			RegSet.mapNo = index;
			// opcode
			RegSet.opcode = (char *) malloc(10);
			strcpy(RegSet.opcode, strtok(NULL, "~"));
			// size
			RegSet.size = atoi(strtok(NULL, "~"));
			// insert into opcodeToSize map
			insertOpcodeToSizeMap((MyString)RegSet.opcode, RegSet.size);
			
			// keyword to identify srclist or dstlist
			strcpy(keyword1, strtok(NULL, "~"));
			// length of srclist or dstlist
			strcpy(length1, strtok(NULL, "~"));
			strcpy(formatStr1, strtok(NULL, "~"));
			// keyword to identify srclist or dstlist
			strcpy(keyword2, strtok(NULL, "~"));
			// length of srclist or dstlist
			strcpy(length2, strtok(NULL, "~"));
			strcpy(formatStr2, strtok(NULL, "~\n"));

			// SRC or DST
			if( strcmp( keyword1, "SRC" ) == 0 )
			{
				// Number of SRC operands
				RegSet.srcLen = atoi(length1);
				CreateSrcRegMap(formatStr1, index, RegSet.srcLen, &RegSet);
			}
			else
			{
				// Number of DST operands
				RegSet.dstLen = atoi(length1);
				CreateDstRegMap(formatStr1, index, RegSet.dstLen, &RegSet);
			}
			// DST or SRC
			if( strcmp( keyword2, "SRC" ) == 0 )
			{
				// Number of SRC operands
				RegSet.srcLen = atoi(length2);
				CreateSrcRegMap(formatStr2, index, RegSet.srcLen, &RegSet);
			}
			else
			{
				// Number of DST operands
				RegSet.dstLen = atoi(length2);
				CreateDstRegMap(formatStr2, index, RegSet.dstLen, &RegSet);
			}
			RegSetMappingVector.push_back((RegSetMappingType) RegSet);
		} // reading of the map entries done
		// now read the mappings between the above operations
		else
		{
			if( strstr(tempStr, "endMaps") != NULL )
			{
				break;
			}
			int map1 = atoi(strtok(tempStr, " =>"));
			int map2 = atoi(strtok(NULL, " =>"));
			GenericToTargetMap[map1] = map2;
			continue;
		}
		
	}
	fclose(fpt);
	
	// debugprint(1);
	// debugprint();
	return;
}
/*
void debugprint(int amap)
{
	RegSetMappingType *RegSetEntity;
	MyString temp;
	int i;
	getRegSetWithMap(amap, RegSetEntity);
	cout << "opcode = " << RegSetEntity->opcode << endl;
	cout << " dests ->" << endl;
	for( i = 0; i < RegSetEntity->dstLen; i++ )
	{
		cout << i+1 << endl;
		temp = unenumerate(RegSetEntity->dstMap[i]);
		cout << temp.getStr() << endl;
	}
	for( i = 0; i < RegSetEntity->srcLen; i++ )
	{
		cout << i+1 << endl;
		temp = unenumerate(RegSetEntity->srcMap[i]);
		cout << temp.getStr() << endl;
	}
}

*/

void debugprint()
{
	char *temp, *tarOp=NULL;
	temp=strdup("SUBF");
	MyString tempStr1, tempStr2;
	rmapVectType srcVect1, srcVect2;
	rmapVectType dstVect;
	rmapVectTwoDim srcs, dsts;
	
	rmapType srcmap, dstmap;
	
	srcmap.rclass = 0;
	srcmap.rdata = 1;
	srcVect1.push_back(srcmap);
	srcmap.rclass = 1;
	srcmap.rdata = 1;
	srcVect2.push_back(srcmap);
	dstmap.rclass = 0;
	dstmap.rdata = 1;
	dstVect.push_back(dstmap);

	srcs.push_back(srcVect1);
	srcs.push_back(srcVect2);
	dsts.push_back(dstVect);

	tarOp = strdup(getTargetOpCode(temp));
	cout << "The target Opcode is -> " << tarOp << endl;
}

rmapVectType getRegClassesInSet(char *rclass, int no)
{
	char rclassStr[10], rdataStr[10];
	rmapVectType rmapVect;
	stringVector eachLine;
	
	int len;
	
	len = noOfClasses(rclass);
	
	feedEachLineVector(eachLine, len, rclass, " ");
	
	for( stringVector::iterator iter = eachLine.begin(); iter != eachLine.end(); iter++ )
	{
		rmapType rmap;
		// ANY:INT
		strcpy(rclassStr, strtok((*iter).getStr(), ":"));
		strcpy(rdataStr, strtok(NULL, ":"));
		
		// rmap.rclass = enumerate_rclass(rclassStr);
		rmap.rclass = globalRegModel.getClassTypeIndex(rclassStr, _GENERIC_);
		if( rmap.rclass == -1 )
		{
			rmap.rclass = globalRegModel.getClassTypeIndex(rclassStr, _TARGET_);
		}
		if( rmap.rclass == -1 )
		{
			ASSERT("No matching register class in either genericRegClasses.txt or targetRegClasses.txt\n");
		}
		// rmap.rdata = enumerate_rdata(rdataStr);
		rmap.rdata = globalRegModel.getDataTypeIndex(rdataStr, _GENERIC_);
		if( rmap.rdata == -1 )
		{
			rmap.rdata = globalRegModel.getDataTypeIndex(rdataStr, _TARGET_);
		}
		if( rmap.rdata == -1 )
		{
			ASSERT("No matching register data type in either genericRegClasses.txt or targetRegClasses.txt\n");
		}
		// create the vector of all register classes for this operand
		rmapVect.push_back((rmapType)rmap);
	}
	// clear eachLine vector
	deleteEachLineVector(eachLine);
	return rmapVect;
}

int noOfClasses(char *str)
{
	int n=1;
	
	strtok(str, " ");
	while(strtok(NULL, " ") != NULL) n++;
	return n;
}

void feedEachLineVector(stringVector &eachLine, int length, char *formatStr, char *sep)
{
	char *str = NULL;
	if( length == 1 )
	{
		str = strdup(formatStr);
		eachLine.push_back((MyString) str);
	}
	else
	{
		str=strdup(strtok( formatStr, sep));
		eachLine.push_back((MyString) str);
		
		for( int i = 1; i < length; i++ )
		{
			str=strdup(strtok( NULL, sep ));
			eachLine.push_back((MyString) str);
		}
	}
	return;
}

void deleteEachLineVector(stringVector &eachLine)
{
	eachLine.clear();
	return;
}

void CreateSrcRegMap(char *formatStr, int index, int srcLength, RegSetMappingType * RegSet)
{
	stringVector eachLine;
	rmapVectType regClasses;

	// if at least one src operand
	if( srcLength > 0 )
	{
		// populate eachLine vector
		feedEachLineVector(eachLine, srcLength, formatStr, ",");
	}
	else
	{
		return;
	}

	// take each of the register set assignment and populate the RegSet data structure
	for( stringVector::iterator iter = eachLine.begin(); iter != eachLine.end(); iter++ )
	{
		int no = noOfClasses((*iter).getStr());
		regClasses = getRegClassesInSet((*iter).getStr(), no);
		
		(RegSet->srcMap).push_back((rmapVectType)regClasses);
	
	}
	
	// delete the eachLine vector
	deleteEachLineVector(eachLine);
	return;
}

void CreateDstRegMap(char *formatStr, int index, int dstLength, RegSetMappingType * RegSet)
{
	stringVector eachLine;
	rmapVectType regClasses;

	// if at least one src operand
	if( dstLength > 0 )
	{
		// populate eachLine vector
		feedEachLineVector(eachLine, dstLength, formatStr, ",");
	}
	else
	{
		return;
	}

	// take each of the register set assignment and populate the RegSet data structure
	for( stringVector::iterator iter = eachLine.begin(); iter != eachLine.end(); iter++ )
	{
		int no = noOfClasses((*iter).getStr());
		regClasses = getRegClassesInSet((*iter).getStr(), no);
		
		(RegSet->dstMap).push_back((rmapVectType)regClasses);
	
	}
	// delete the eachLine vector
	deleteEachLineVector(eachLine);
	return;
}



void getTargetRegSetMap(char *genericOp, rmapVectTwoDim srcmap, int srclen, rmapVectTwoDim dstmap, int dstlen, 
						rmapVectTwoDim &targetsrcmap, rmapVectTwoDim &targetdstmap)
{
	int map1, map2;
	RegSetMappingType *targetRegSet = NULL;
	GenericToTargetMapType::iterator mi;
	
	for( mi = GenericToTargetMap.begin(); mi != GenericToTargetMap.end(); mi++ )
	{
		map1 = (*mi).first;
		map2 = (*mi).second;
		// Generic to target maps are map1 to map2 respectively
		RegSetMappingVectorType::iterator vi;
		// Go thru' each of the register sets
		for( vi = RegSetMappingVector.begin(); vi != RegSetMappingVector.end(); vi++ )
		{
			RegSetMappingType RegSetEntity = (*vi);
			// RegSetEntity contains each of the instruction register set
			if( RegSetEntity.mapNo == map1 &&  strcmp(RegSetEntity.opcode, genericOp) == 0 )
			{
				// generic opcode map index is found
				// srcmap and dstmap are the input mapping vectors
				if( mapCmp(srcmap, srclen, &RegSetEntity, _SRC_) == 0 
					&& mapCmp(dstmap, dstlen, &RegSetEntity, _DST_) == 0 )
				{
					// map2 is finalized here because match has occured
					if( getRegSetWithMap(map2, targetRegSet) == 0 ) 
					{
						ASSERT(0);
					}
					if( targetRegSet != NULL )
					{
						targetsrcmap = targetRegSet->srcMap;
						targetdstmap = targetRegSet->dstMap;
						return;
					}
				}
			}
		}
	}
	ASSERT("Check the rISARegSetMapping.txt for including proper maps !\n");
	return;
}

void getMatchingTargets(char *genericOp, RegSetMappingVectorType &mapVector)
{
	RegSetMappingType *targetRegSet = NULL;

	RegSetMappingVectorType::iterator vi;
	
	for( vi = RegSetMappingVector.begin(); vi != RegSetMappingVector.end(); vi++ )
	{
		// RegSetEntity contains each of the instruction register set
		if(strcmp(vi->opcode, genericOp) == 0 )
		{
			
			if( getRegSetWithMap(GenericToTargetMap[vi->mapNo], targetRegSet) == 0 ) 
			{
				ASSERT(0);
			}
			if( targetRegSet != NULL )
			{
				mapVector.push_back((RegSetMappingType)(*targetRegSet));
			}
			
		}
	}
	return;
}

char * getTargetOpCode(char *genericOp)
{
	RegSetMappingType *targetRegSet = NULL;
	RegSetMappingVectorType::iterator vi;
	
	for( vi = RegSetMappingVector.begin(); vi != RegSetMappingVector.end(); vi++ )
	{
		// RegSetEntity contains each of the instruction register set
		if(strcmp(vi->opcode, genericOp) == 0 )
		{
			
			if( getRegSetWithMap(GenericToTargetMap[vi->mapNo], targetRegSet) == 0 ) 
			{
				ASSERT(0);
			}
			if( targetRegSet != NULL )
			{
				return targetRegSet->opcode;
			}
			
		}
	}
	ASSERT("Check the rISARegSetMapping.txt for including proper maps !\n");
	return NULL;
}

bool ifMapExists(char *genericOp, rmapVectTwoDim srcmap, int srclen, 
				 rmapVectTwoDim dstmap, int dstlen)
{
	int map1, map2;
	RegSetMappingType *targetRegSet = NULL;
	GenericToTargetMapType::iterator mi;
	
	for( mi = GenericToTargetMap.begin(); mi != GenericToTargetMap.end(); mi++ )
	{
		map1 = (*mi).first;
		map2 = (*mi).second;
		// Generic to target maps are map1 to map2 respectively
		RegSetMappingVectorType::iterator vi;
		// Go thru' each of the register sets
		for( vi = RegSetMappingVector.begin(); vi != RegSetMappingVector.end(); vi++ )
		{
			RegSetMappingType RegSetEntity = (*vi);
			// RegSetEntity contains each of the instruction register set
			if( RegSetEntity.mapNo == map1 &&  strcmp(RegSetEntity.opcode, genericOp) == 0 )
			{
				// generic opcode map index is found
				// srcmap and dstmap are the input mapping vectors
				if( mapCmp(srcmap, srclen, &RegSetEntity, _SRC_) == 0 
					&& mapCmp(dstmap, dstlen, &RegSetEntity, _DST_) == 0 )
				{
					// map2 is finalized here because match has occured
					if( getRegSetWithMap(map2, targetRegSet) == 0 ) 
					{
						ASSERT(0);
					}
					if( targetRegSet != NULL )
					{
						return true;
					}
				}
			}
		}
	}
	
	return false;
}

int getRegSetWithMap(int amap, RegSetMappingType *& RegSetEntity)
{
	RegSetMappingVectorType::iterator vi;
	for( vi = RegSetMappingVector.begin(); vi != RegSetMappingVector.end(); vi++ )
	{
		RegSetEntity = vi;
		if( RegSetEntity->mapNo == amap )
		{
			return 1;
		}
	}
	return 0;
}

// returns 0 if match found

int mapCmp(rmapVectTwoDim amap, int len, RegSetMappingType *RegSetEntity, int type)
{
	rmapVectTwoDim::iterator aiter, biter;
	rmapVectTwoDim bmap;
	rmapVectType::iterator citer;
	if( type == _SRC_ )
	{
		// bmap is gold
		bmap = RegSetEntity->srcMap; 
		if( len != RegSetEntity->srcLen ) return	1;
		// the operations with NULL argument are rISAizable
		if( len == 0 ) return	0;
		for(aiter = amap.begin(), biter = bmap.begin(); 
		    aiter != amap.end() && biter != bmap.end(); aiter++, biter++)
		{
			rmapVectType rclassToBeMatched = (*aiter);
			rmapVectType rclassGold = (*biter);
			// match each of the reg class
			// ANY:INT, NORMAL:INT and so on
		    for( citer = rclassToBeMatched.begin(); citer != rclassToBeMatched.end(); citer++ )
			{
				// if any of them doesn't match return false
				if( !isRegClassMatched((*citer), rclassGold) )
					return 1;
			}
		}
		
		return 0;
	}
	else // type == _DST_
	{
		// bmap is gold
		bmap = RegSetEntity->dstMap; 
		if( len != RegSetEntity->dstLen ) return	1;
		// the operations with NULL argument are rISAizable
		if( len == 0 ) return	0;
		for(aiter = amap.begin(), biter = bmap.begin(); 
		    aiter != amap.end() && biter != bmap.end(); aiter++, biter++)
		{
			rmapVectType rclassToBeMatched = (*aiter);
			rmapVectType rclassGold = (*biter);
		    for( citer = rclassToBeMatched.begin(); citer != rclassToBeMatched.end(); citer++ )
			{
				if( !isRegClassMatched((*citer), rclassGold) )
					return 1;
			}
		}
		
		return 0;
	}
}

bool isRegClassMatched(rmapType tobeMatched, rmapVectType gold)
{
	rmapVectType::iterator iter;
	for( iter = gold.begin(); iter != gold.end(); iter++ )
	{
		if((*iter).rclass == tobeMatched.rclass
			&& (*iter).rdata == tobeMatched.rdata)
		{
			return true;
		}
		if((*iter).rclass == globalRegModel.getClassTypeIndex("ANY", _GENERIC_)
			&& (*iter).rdata == tobeMatched.rdata)
		{
			return true;
		}
		if((*iter).rdata == globalRegModel.getDataTypeIndex("ANY", _GENERIC_)
			&& (*iter).rclass == tobeMatched.rclass)
		{
			return true;
		}
		if(tobeMatched.rclass == globalRegModel.getClassTypeIndex("ANY", _GENERIC_)
			&& tobeMatched.rdata == globalRegModel.getDataTypeIndex("INT", _GENERIC_))
		{
			return true;
		}
		// formats need to be comprehensive enuf to accomodate the following
		if(tobeMatched.rclass == globalRegModel.getClassTypeIndex("SP", _GENERIC_)
			&& tobeMatched.rdata == globalRegModel.getDataTypeIndex("ANY", _GENERIC_))
		{
			return true;
		}
		if(tobeMatched.rclass == globalRegModel.getClassTypeIndex("FP", _GENERIC_)
			&& tobeMatched.rdata == globalRegModel.getDataTypeIndex("ANY", _GENERIC_))
		{
			return true;
		}
		if(tobeMatched.rclass == globalRegModel.getClassTypeIndex("CC", _GENERIC_)
			&& tobeMatched.rdata == globalRegModel.getDataTypeIndex("ANY", _GENERIC_))
		{
			return true;
		}
		if(tobeMatched.rclass == globalRegModel.getClassTypeIndex("HILO", _GENERIC_)
			&& tobeMatched.rdata == globalRegModel.getDataTypeIndex("ANY", _GENERIC_))
		{
			return true;
		}
	}
	return false;
}


void mapCopy(rmapTypes & toMap, rmapType *fromMap, int len)
{
	int i;
	toMap = (rmapType *) malloc(len * sizeof(rmapType));
	for( i = 0; i < len; i++ )
	{
		toMap[i].rclass = fromMap[i].rclass;
		toMap[i].rdata = fromMap[i].rdata;
	}
	return;
}

int sizeOf(MyString opcode)
{
	opcodeToSizeMapType::iterator iter;
	if( (iter = opcodeToSizeMap.find(opcode)) != opcodeToSizeMap.end() )
	{
		return (*iter).second;
	}
	else
	{
		assert("Not a valid Opcode !\n");
		return 0;
	}
}

void insertOpcodeToSizeMap(MyString str, int si)
{
	// str is not already present in the map
	if( opcodeToSizeMap.find(str) == opcodeToSizeMap.end() )
	{
		opcodeToSizeMap[str] = si;
	}
	return;
}

void printOpcodeToSizeMap()
{
	cout << "############# Printing OpcodeToSizeMap ############" << endl;
	for (opcodeToSizeMapType::iterator i = opcodeToSizeMap.begin();
	i != opcodeToSizeMap.end();i++)
	{
		MyString temp = (*i).first;
		cout << temp.getStr() << "\t" << (*i).second << endl;
	}
}


void printGenericToTargetMap()
{
	cout << "############# Printing GenericToTargetMap ############" << endl;
	for (GenericToTargetMapType::iterator i = GenericToTargetMap.begin();
	i != GenericToTargetMap.end();i++)
	{
		cout << (*i).first << "\t" << (*i).second << endl;
	}
}


void printRegSetMappingVector()
{
	cout << "############# Printing RegSetMappingVector ############" << endl;
	for (int i=0; i<RegSetMappingVector.size(); i++)
	{
		cout << RegSetMappingVector[i].mapNo << "\t" 
			<< RegSetMappingVector[i].opcode << "\t" 
			<< RegSetMappingVector[i].size << endl;
	}
}



void getBestTargetOpForGeneric_implicit(RegSetMappingVectorType RegSetMapVector, BaseOperation *oper, RegSetMappingType &targetOpMap)
{
	MyString opc;
	oper->getOpcodeName(opc);
	BaseArgument *dstArg, *srcArg;
	ArgList srcArgList;
	ArgListIter aIter;
	char *srcRegText, *dstRegText;
	int srcVal, dstVal;
	int isDstEqualCC = 0;
	int isDstEqualHILO = 0;
	int isDstEqualSP = 0;
	int isDstEqualFP = 0;
	// get the destination argument
	dstArg = oper->ptrToOperand(_DEST_, _DEST_LIST_);
	if( dstArg != NULL )
	{
		dstRegText = strdup(((RegisterArg*)dstArg)->getText());
		dstVal = ((RegisterArg*)dstArg)->getVal();
		if( strcmp(((RegisterArg *)dstArg)->getText(), "gcc") == 0 )
			isDstEqualCC = 1;
		else if( strcmp(((RegisterArg *)dstArg)->getText(), "ghilo") == 0 )
			isDstEqualHILO = 1;
		else if( strcmp(((RegisterArg *)dstArg)->getText(), "gsp") == 0 )
			isDstEqualSP = 1;
		else if( strcmp(((RegisterArg *)dstArg)->getText(), "gfp") == 0 )
			isDstEqualFP = 1;
		
	}
	// get the srcArgList
	oper->sourceOperandsList(srcArgList);
	
	int isDstEqualSrc1 = 0;
	int isDstEqualSrc2 = 0;
	int isSrc1EqualFP = 0;
	int isSrc2EqualSP = 0;
	int isSrcEqualSP = 0;
	int isSrc2EqualImm = 0;
	int iterCount = 0;
	int isImmEqual0 = 0;
	int isImmEqual2 = 0;	
	int isSrcEqual0 = 0;
	int isImmEqual1 = 0;
	int isImmEqualNeg1 = 0;
	int isSrc3EqualZero = 0;
	int isSrc3EqualFP = 0;

	
	// traverse all the source arguments and set the appropriate variables..
	for (aIter = srcArgList.begin(); aIter != srcArgList.end(); aIter++)
	{
		srcArg = (*aIter);
		if( srcArg->isRegisterArg() )
		{
			srcRegText = strdup(((RegisterArg*)srcArg)->getText());
			srcVal = ((RegisterArg*)srcArg)->getVal();
			if( dstArg != NULL && strcmp(dstRegText, srcRegText) == 0 && dstVal == srcVal )
			{
				if( iterCount == 0 )
					isDstEqualSrc1 = 1;
				else
					isDstEqualSrc2 = 1;
			}
			if( iterCount == 1 && strcmp(srcRegText, "gsp") == 0 )
			{
				isSrc2EqualSP = 1;
			}
			if( iterCount == 0 && strcmp(srcRegText, "gfp") == 0 )
			{
				isSrc1EqualFP = 1;
			}
			if( iterCount == 0 && strcmp(srcRegText, "gsp") == 0 )
			{
				isSrcEqualSP= 1;
			}
			if( iterCount == 2 && strcmp(srcRegText, "gfp") == 0 )
			{
				isSrc3EqualFP= 1;
			}
			if( iterCount == 2 && strcmp(srcRegText, "gR") == 0 && srcVal == 0 )
			{
				isSrc3EqualZero = 1;
			}
			if( iterCount == 1 && strcmp(srcRegText, "gR") == 0 && srcVal == 0 )
			{
				isSrcEqual0 = 1;
			}
		}
		else if( srcArg->isImmediateArg() )
		{
			// special cases are always with src2 = imm
			if( iterCount == 1 )
			{
				isSrc2EqualImm = 1;
				// immediate is always integer constant
				if(((IConstArg *)srcArg)->value() == 2 )
					isImmEqual2 = 1;
				else if(((IConstArg *)srcArg)->value() == 0)
					isImmEqual0 = 1;
				else if(((IConstArg *)srcArg)->value() == 1)
					isImmEqual1 = 1;
				else if(((IConstArg *)srcArg)->value() == -1)
					isImmEqualNeg1 = 1;
			}
		}

		iterCount++;
		
	}
	RegSetMappingVectorType::iterator rIter;
	for( rIter = RegSetMapVector.begin(); rIter != RegSetMapVector.end(); rIter++ )
	{
		// addu
		if( opc == "addu" )
		{
			if( isSrc2EqualImm )
			{
				// src2 is immediate; can be addu2_r or addu3_r or addu5_r
				if( isDstEqualSrc1 )
				{
					if( strcmp((*rIter).opcode, "addu2_r") == 0)
					targetOpMap = *rIter;
				}
				else if( isSrc1EqualFP )
				{
					if( strcmp((*rIter).opcode, "addu3_r") == 0 )
					targetOpMap = *rIter;
				}
				else if( isImmEqual1 )
				{
					if( strcmp((*rIter).opcode, "addu6_r") == 0 )
					targetOpMap = *rIter;
				}
				else if( isImmEqualNeg1 )
				{
					if( strcmp((*rIter).opcode, "addu7_r") == 0 )
					targetOpMap = *rIter;
				}
				else if( strcmp((*rIter).opcode, "addu5_r") == 0 )
					targetOpMap = *rIter;
			}
			else
			{
				// src2 is a register argument
				// can match addu0_r, addu1_r or addu4_r
				if( isDstEqualSrc1 )
				{
					if( strcmp((*rIter).opcode, "addu1_r") == 0 )
					targetOpMap = *rIter;
				}
				else if( isDstEqualSrc2 )
				{
					if( strcmp((*rIter).opcode, "addu4_r") == 0 )
					targetOpMap = *rIter;
				}
				else if( strcmp((*rIter).opcode, "addu0_r") == 0 )
					targetOpMap = *rIter;
			}
		}
		// sll
		else if( opc == "sll" )
		{
			if( isImmEqual2 )
			{
				// cases are sll1_r or sll2_r
				if( isDstEqualSrc1 )
				{
					if( strcmp((*rIter).opcode, "sll2_r") == 0 )
					targetOpMap = *rIter;
				}
				else  if( strcmp((*rIter).opcode, "sll1_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "sll0_r") == 0 )
				targetOpMap = *rIter;
		}
		// seq
		else if( opc == "seq" )
		{
			if( isSrcEqual0)
			{
				if( isSrcEqual0 && strcmp((*rIter).opcode, "seq1_r") == 0 )
			    targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "seq0_r") == 0 )
				targetOpMap = *rIter;
		}
		// sne
		else if( opc == "sne" )
		{
			if( isSrcEqual0 )
			{
				if( isSrcEqual0 && strcmp((*rIter).opcode, "sne1_r") == 0 )
			    targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "sne0_r") == 0 )
				targetOpMap = *rIter;
		}
		// sgt
		else if( opc == "sgt" )
		{
			if( isSrcEqual0)
			{
				if( isSrcEqual0 && strcmp((*rIter).opcode, "sgt1_r") == 0 )
			    targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "sgt0_r") == 0 )
				targetOpMap = *rIter;
		}
		// sw
		else if( opc == "sw" )
		{
			if( isImmEqual0 )
			{
				if( strcmp((*rIter).opcode, "sw1_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( isSrc1EqualFP )
			{
				if( strcmp((*rIter).opcode, "sw2_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( isSrcEqualSP && isSrc3EqualFP)
			{
				if( strcmp((*rIter).opcode, "sw3_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( isSrcEqualSP && isSrc3EqualZero)
			{
				if( strcmp((*rIter).opcode, "sw4_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "sw0_r") == 0 )
				targetOpMap = *rIter;
		}
		// lw
		else if( opc == "lw" )
		{
			if( isImmEqual0 )
			{
				// can be lw1_r or lw2_r
				if( isDstEqualSrc1 )
				{
					if( strcmp((*rIter).opcode, "lw1_r") == 0 )
						targetOpMap = *rIter;
				}
				else if( strcmp((*rIter).opcode, "lw2_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( isSrc1EqualFP )
			{
				if( strcmp((*rIter).opcode, "lw3_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "lw0_r") == 0 )
				targetOpMap = *rIter;
		}			
		// li
		else if( opc == "li" )
		{
			if( strcmp((*rIter).opcode, "li0_r") == 0 )
				targetOpMap = *rIter;
		}
		// mult
		else if( opc == "mult" )
		{
			if( strcmp((*rIter).opcode, "mult0_r") == 0 )
				targetOpMap = *rIter;
		}
		// div
		else if( opc == "div" )
		{
			if( strcmp((*rIter).opcode, "div0_r") == 0 )
				targetOpMap = *rIter;
		}
		// mflo
		else if( opc == "mflo" )
		{
			if( strcmp((*rIter).opcode, "mflo0_r") == 0 )
				targetOpMap = *rIter;
		}
		// mfhi
		else if( opc == "mfhi" )
		{
			if( strcmp((*rIter).opcode, "mfhi0_r") == 0 )
				targetOpMap = *rIter;
		}
		// slt
		else if( opc == "slt" )
		{
			if( isSrc2EqualImm )
			{
				if( strcmp((*rIter).opcode, "slt1_r") == 0 )
					targetOpMap = *rIter;
			}
			else
			{
				if( strcmp((*rIter).opcode, "slt0_r") == 0 )
					targetOpMap = *rIter;
			}
			
		}
		// move
		else if( opc == "move" )
		{
			if( isSrcEqual0)
			{
				if( strcmp((*rIter).opcode, "move1_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( isDstEqualFP && isSrcEqualSP )
			{
				if( strcmp((*rIter).opcode, "move2_r") == 0 )
				targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "move0_r") == 0 )
				targetOpMap = *rIter;
		}
		// subu
		else if( opc == "subu" )
		{
			
			if( isSrc2EqualImm )
			{
				if( isSrc2EqualSP && isDstEqualSP )
				{
					if( strcmp((*rIter).opcode, "subu2_r") == 0 )
						targetOpMap = *rIter;
				}
				if( strcmp((*rIter).opcode, "subu1_r") == 0 )
					targetOpMap = *rIter;
			}
			else
			{
				if( isDstEqualSrc1 )
				{
					if( strcmp((*rIter).opcode, "subu3_r") == 0)
						targetOpMap = *rIter;
				}
				else if( isDstEqualSrc2 )
				{
					if( strcmp((*rIter).opcode, "subu4_r") == 0)
						targetOpMap = *rIter;
				}
				else if( strcmp((*rIter).opcode, "subu0_r") == 0 )
					targetOpMap = *rIter;
			}
		}
		// sra
		else if( opc == "sra" )
		{
			if( strcmp((*rIter).opcode, "sra0_r") == 0 )
				targetOpMap = *rIter;
		}
		// srl
		else if( opc == "srl" )
		{
			if( strcmp((*rIter).opcode, "srl0_r") == 0 )
				targetOpMap = *rIter;
		}
		// bnez
		else if( opc == "bnez" )
		{
			if( strcmp((*rIter).opcode, "bnez0_r") == 0 )
				targetOpMap = *rIter;
		}
	}
	return;
}

void getBestTargetOpForGeneric_733(RegSetMappingVectorType RegSetMapVector, BaseOperation *oper, RegSetMappingType &targetOpMap)
{
	MyString opc;
	oper->getOpcodeName(opc);
	BaseArgument *dstArg, *srcArg;
	ArgList srcArgList;
	ArgListIter aIter;
	char *srcRegText, *dstRegText;
	int srcVal, dstVal;
	int isDstEqualCC = 0;
	int isDstEqualHILO = 0;
	// get the destination argument
	dstArg = oper->ptrToOperand(_DEST_, _DEST_LIST_);
	if( dstArg != NULL )
	{
		dstRegText = strdup(((RegisterArg*)dstArg)->getText());
		dstVal = ((RegisterArg*)dstArg)->getVal();
		if( strcmp(((RegisterArg *)dstArg)->getText(), "gcc") == 0 )
			isDstEqualCC = 1;
		else if( strcmp(((RegisterArg *)dstArg)->getText(), "ghilo") == 0 )
			isDstEqualHILO = 1;
		
	}
	// get the srcArgList
	oper->sourceOperandsList(srcArgList);
	
	int isDstEqualSrc1 = 0;
	int isDstEqualSrc2 = 0;
	int isSrc1EqualFP = 0;
	int isSrc2EqualImm = 0;
	int iterCount = 0;
	int isImmEqual0 = 0;
	int isImmEqual2 = 0;	
	int isSrcEqual0 = 0;
	
	
	// traverse all the source arguments and set the appropriate variables..
	for (aIter = srcArgList.begin(); aIter != srcArgList.end(); aIter++)
	{
		srcArg = (*aIter);
		if( srcArg->isRegisterArg() )
		{
			srcRegText = strdup(((RegisterArg*)srcArg)->getText());
			srcVal = ((RegisterArg*)srcArg)->getVal();
			if( dstArg != NULL && strcmp(dstRegText, srcRegText) == 0 && dstVal == srcVal )
			{
				if( iterCount == 0 )
					isDstEqualSrc1 = 1;
				else
					isDstEqualSrc2 = 1;
			}
			if( iterCount == 0 && strcmp(srcRegText, "gfp") == 0 )
			{
				isSrc1EqualFP = 1;
			}
		}
		else if( srcArg->isImmediateArg() )
		{
			// special cases are always with src2 = imm
			if( iterCount == 1 )
			{
				isSrc2EqualImm = 1;
				// immediate is always integer constant
				if(((IConstArg *)srcArg)->value() == 2 )
					isImmEqual2 = 1;
				else if(((IConstArg *)srcArg)->value() == 0)
					isImmEqual0 = 1;
			}
		}
		if( strcmp(srcRegText, "gR") == 0 && srcVal == 0 )
			isSrcEqual0 = 1;
		iterCount++;
		
	}
	RegSetMappingVectorType::iterator rIter;
	for( rIter = RegSetMapVector.begin(); rIter != RegSetMapVector.end(); rIter++ )
	{
		// addu
		if( opc == "addu" )
		{
			if( isSrc2EqualImm )
			{
				if( strcmp((*rIter).opcode, "addu1_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "addu0_r") == 0 )
				targetOpMap = *rIter;
		}
		// subu
		else if( opc == "subu" )
		{
			if( isSrc2EqualImm )
			{
				if( strcmp((*rIter).opcode, "subu1_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "subu0_r") == 0 )
				targetOpMap = *rIter;
		}
		// slt
		else if( opc == "slt" )
		{
			if( isSrc2EqualImm )
			{
				if( strcmp((*rIter).opcode, "slt1_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "slt0_r") == 0 )
				targetOpMap = *rIter;
		}
		else
			targetOpMap = *rIter;
	}
	return;
}

void getBestTargetOpForGeneric_444(RegSetMappingVectorType RegSetMapVector, BaseOperation *oper, RegSetMappingType &targetOpMap)
{
	MyString opc;
	oper->getOpcodeName(opc);
	BaseArgument *dstArg, *srcArg;
	ArgList srcArgList;
	ArgListIter aIter;
	char *srcRegText, *dstRegText;
	int srcVal, dstVal;
	int isDstEqualCC = 0;
	int isDstEqualHILO = 0;
	// get the destination argument
	dstArg = oper->ptrToOperand(_DEST_, _DEST_LIST_);
	if( dstArg != NULL )
	{
		dstRegText = strdup(((RegisterArg*)dstArg)->getText());
		dstVal = ((RegisterArg*)dstArg)->getVal();
		if( strcmp(((RegisterArg *)dstArg)->getText(), "gcc") == 0 )
			isDstEqualCC = 1;
		else if( strcmp(((RegisterArg *)dstArg)->getText(), "ghilo") == 0 )
			isDstEqualHILO = 1;
		
	}
	// get the srcArgList
	oper->sourceOperandsList(srcArgList);
	
	int isDstEqualSrc1 = 0;
	int isDstEqualSrc2 = 0;
	int isSrc1EqualFP = 0;
	int isSrc2EqualImm = 0;
	int iterCount = 0;
	int isImmEqual0 = 0;
	int isImmEqual2 = 0;	
	int isSrcEqual0 = 0;
	
	
	// traverse all the source arguments and set the appropriate variables..
	for (aIter = srcArgList.begin(); aIter != srcArgList.end(); aIter++)
	{
		srcArg = (*aIter);
		if( srcArg->isRegisterArg() )
		{
			srcRegText = strdup(((RegisterArg*)srcArg)->getText());
			srcVal = ((RegisterArg*)srcArg)->getVal();
			if( dstArg != NULL && strcmp(dstRegText, srcRegText) == 0 && dstVal == srcVal )
			{
				if( iterCount == 0 )
					isDstEqualSrc1 = 1;
				else
					isDstEqualSrc2 = 1;
			}
			if( iterCount == 0 && strcmp(srcRegText, "gfp") == 0 )
			{
				isSrc1EqualFP = 1;
			}
		}
		else if( srcArg->isImmediateArg() )
		{
			// special cases are always with src2 = imm
			if( iterCount == 1 )
			{
				isSrc2EqualImm = 1;
				// immediate is always integer constant
				if(((IConstArg *)srcArg)->value() == 2 )
					isImmEqual2 = 1;
				else if(((IConstArg *)srcArg)->value() == 0)
					isImmEqual0 = 1;
			}
		}
		if( strcmp(srcRegText, "gR") == 0 && srcVal == 0 )
			isSrcEqual0 = 1;
		iterCount++;
		
	}
	RegSetMappingVectorType::iterator rIter;
	for( rIter = RegSetMapVector.begin(); rIter != RegSetMapVector.end(); rIter++ )
	{
		// addu
		if( opc == "addu" )
		{
			if( isSrc2EqualImm )
			{
				if( strcmp((*rIter).opcode, "addu1_r") == 0 )
					targetOpMap = *rIter;
			}
			else if( strcmp((*rIter).opcode, "addu0_r") == 0 )
				targetOpMap = *rIter;
		}
		else
			targetOpMap = *rIter;
	}
	return;
}

