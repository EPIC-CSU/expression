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
////////////////////////////////////////////////////////
// File:  			ThumbPass.h
// Created:			March 15, 2001.
// Last modified: 	July 30, 2001.
// Author:			Aviral Shrivastava
// Email:			aviral@ics.uci.edu
//
////////////////////////////////////////////////////////

#ifndef _THUMB_PASS_H_
#define _THUMB_PASS_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector

#include "MyString.h"
#include "OpCode.h"

// Data Structure Definitions
//
// We assume the mapping from thumb to normal instruction is a one-to-one mapping
typedef map<MyString, MyString> NormalToThumbInstrMap;
typedef map<MyString, MyString>::iterator NtoTIter;

typedef enum { _NORMAL_, _THUMB_ } Mode;
typedef struct block_Thumb_Info
{
	Mode oldEntryMode, oldExitMode, newEntryMode, newExitMode;
}blockThumbInfo;

typedef Map <BasicBlockNode *, blockThumbInfo> thumbInfo;
typedef thumbInfo::iterator thumbInfoIter;

typedef struct rISABlock
{
	BaseInstruction *startInstr; // start of the rISA block. (Note: This may/may not contain the MX oper)
	BaseInstruction *endInstr;   // last Instr of the rISA block. (Note: This may/may not contain the MX_r oper)

	BasicBlockNode *bb; // a rISA Block is a sub basic-block
	int numInstrs;
	
	int totalNormalNumLive; // total number of variables live.

	int priority; // an assigned priority indicates the order in which these blocks will be processed during phase 2
	int profitability; 
} rISABlock;

typedef Vector<rISABlock> rISABlockList;
typedef Vector<rISABlock>::iterator rISABlockListIter;

extern rISABlockList rISABlocks;

// Functions Declarations
void rISAphase1(Routine *rout);
void rISAphase2(Routine *rout);
void rISAphase3(Routine *rout);
void rISAphase4(Routine *rout);
void createRISABlockList(BasicBlockNode *currBB);
void convertVarsToTempRISA(rISABlock & rb);
void convertVarsToTempRISA(BaseInstruction *instr);
void setPriority(rISABlock & rb);
void orderRISABlocksByPriority();
void convertToRISA(rISABlock & rb);
void convertToRISA(BaseInstruction *instr, int phase1or2, int changeOpcode);
void convertToRISA(BaseOperation *oper, int phase1or2, int changeOpcode);
void addRISAPrologue(rISABlock & currBB);
void addRISAEpilogue(rISABlock & currBB);
void SetEntryModeOfBB(BasicBlockNode *currBB);
void SetExitModeOfBB(BasicBlockNode *currBB);
void ChangeModeOfBB(BasicBlockNode *currBB);
void recursiveConvertToTemp(BaseOperation *oper, BaseArgument *origArg, BaseArgument *tempArg, int destOrSrc);
void convertToTemp(BaseArgument *arg, BaseArgument *tempArg);
NormalInstr *createThumbInstr(MyString opcode);
int strcmpci(char *first, char *second);
int CanbeThumbed(BaseInstruction *instr);
int OperationCount(BaseInstruction *instr);
int CanbeThumbed(BaseOperation *oper);
int CanbeThumbed(BaseInstruction *instr);
void ConvertInstrToNormal(NormalInstr *instr);
void ConvertOperationToNormal(NormalOp *oper);
int profitableToConvert(rISABlock rb);
void printRisaInfo(rISABlockList rbList);
void printThumbInfo(Routine *rout);
void printNtoTMap(NormalToThumbInstrMap N2TMap);
int isFeasibleToConvert(BaseOperation *op);

int addRISARegClassProperty(BaseArgument *arg, BaseOperation *op, int tempRISA, int destOrSrc);
int addNewRISARegClassProperty(BaseArgument *arg, BaseOperation *op, int destOrSrc, int classType);

int addRISAProp(BaseArgument *arg, BaseOperation *op, int tempRISA, int destOrSrc);
void addRISAProp(BaseInstruction *instr, int tempRISA);
void addRISAProp(rISABlock & rb, int tempRISA);
void recursiveAddRISAProp(BaseOperation *oper, int tempRISA, int destOrSrc, BaseArgument *origArg = NULL);

int ThumbInit();

#endif