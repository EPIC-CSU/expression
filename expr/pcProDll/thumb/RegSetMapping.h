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
// RegSetMapping.h
//
// File:  		RegSetMapping.h
// Created:		Fri Jul 27, 2001
// Author:		Partha Biswas
// Email:		partha@ics.uci.edu, ilp@ics.uci.edu
//
// Dividing the register files into various subsets 
// 


#ifndef _REG_SET_MAPPING_H_
#define _REG_SET_MAPPING_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif


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

#include "MyString.h"
#include "..\AsmGen\AsmGen.h"
#include "BaseOperation.h"


// maptype which reduces to different register classes

typedef int rclassType;
typedef int rdataType;

typedef struct{
	rclassType rclass;
	rdataType rdata;
} rmapType;

typedef vector<rmapType> rmapVectType;
typedef vector<rmapVectType> rmapVectTwoDim;
typedef rmapType * rmapTypes;

typedef struct RegSetMapping{
	int mapNo;
	char *opcode;  
	int size;
	rmapVectTwoDim srcMap;  // contains a set of register classes for each src argument
	rmapVectTwoDim dstMap; // contains a set of register classes for each dst argument
    int srcLen;
	int dstLen;
} RegSetMappingType;

typedef vector <RegSetMappingType> RegSetMappingVectorType;
extern RegSetMappingVectorType RegSetMappingVector;

typedef map < int , int > GenericToTargetMapType;
extern GenericToTargetMapType GenericToTargetMap;

typedef map <MyString, int> opcodeToSizeMapType;
extern opcodeToSizeMapType opcodeToSizeMap;


// Global function declarations
int getRegSetWithMap(int amap, RegSetMappingType *& RegSetEntity);
void ReadRegSetMapFile(FILE *fpt);

void CreateSrcRegMap(char *formatStr, int index, int srcLength, RegSetMappingType * RegSet);
void CreateDstRegMap(char *formatStr, int index, int dstLength, RegSetMappingType * RegSet);

void mapCopy(rmapTypes & toMap, rmapType *fromMap, int len);
char * getTargetOpCode(char *genericOp);
void getTargetRegSetMap(char *genericOp, rmapVectTwoDim srcmap, int srclen, rmapVectTwoDim dstmap, int dstlen, 
						rmapVectTwoDim &targetsrcmap, rmapVectTwoDim &targetdstmap);
bool ifMapExists(char *genericOp, rmapVectTwoDim srcmap, int srclen, 
					   rmapVectTwoDim dstmap, int dstlen);
int mapCmp(rmapVectTwoDim amap, int len, RegSetMappingType *RegSetEntity, int type);
bool isRegClassMatched(rmapType tobeMatched, rmapVectType gold);
void feedEachLineVector(stringVector &eachLine, int length, char *formatStr, char *sep);
void deleteEachLineVector(stringVector &eachLine);
rmapVectType getRegClassesInSet(char *rclass, int no);
int noOfClasses(char *str);
void debugprint();

rclassType enumerate_rclass( char *str );
MyString unenumerate_rclass( rclassType key );
rdataType enumerate_rdata( char *str );
MyString unenumerate_rdata( rdataType key );

int sizeOf(MyString);
void insertOpcodeToSizeMap(MyString, int);

void getMatchingTargets(char *, RegSetMappingVectorType &);
void getBestTargetOpForGeneric_implicit(RegSetMappingVectorType, BaseOperation *, RegSetMappingType &);
void getBestTargetOpForGeneric_733(RegSetMappingVectorType, BaseOperation *, RegSetMappingType &);
void getBestTargetOpForGeneric_444(RegSetMappingVectorType, BaseOperation *, RegSetMappingType &);

// Print functions
void printOpcodeToSizeMap();
void printGenericToTargetMap();
void printRegSetMappingVector();

#endif

