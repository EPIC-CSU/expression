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
// File:  			profit_function.h
// Created:			July 30, 2001.
// Last modified: 	August 15, 2001.
// Author:			Aviral Shrivastava
// Email:			aviral@ics.uci.edu
//
////////////////////////////////////////////////////////

#ifndef _PROFIT_FUNCTION_H_
#define _PROFIT_FUNCTION_H_

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <vector>

#ifdef vector
#undef vector
#endif 
#define vector std::vector

#include "ThumbPass.h"

// #defs
//
#define SIZE_OF_NORMAL_INSTR_IN_BYTES 4
#define SIZE_OF_RISA_INSTR_IN_BYTES 2
#define NEED_MODE_CHANGE_INSTR 0.5
#define NUM_RISA_REGS 16
#define NUM_REGS 30
#define REG_FACTOR_FOR_SPILL 0.9
#define CONST1 1
#define CONST2 1
#define CONST3 1
#define CONST4 1
#define CONST5 0.5
#define CONST6 1.2

int CS1(rISABlock rb);
int CS2(rISABlock rb);
int CS3(rISABlock rb);

int getNoOfReloads(int noOfSpills, int aupd, rISABlock rb);
int getNoOfRisaSpills(int arp, double aupd, rISABlock rb);
int getNoOfRisaReloads(int arp, rISABlock rb);
int getNoOfNonRisaSpills(int noOfRisaSpills, int arp, int arp_nr, rISABlock rb);
int getNoOfSpillsIfNormal(int n_arp, int n_adul, rISABlock rb);
double getAvgUsesPerDef(rISABlock rb);
int getNoOfUses(BaseArgument *destArg);
int getAvgDefUseLen(rISABlock rb);
int getRegPress(BaseInstruction *instr, BasicBlockNode *bb, int index);
int getAvgRegPress(rISABlock rb, int risaOrNormal);
int profitableToConvert(rISABlock rb);
int getDefUseLen(BaseArgument *destArg, BaseInstruction *curInstr, rISABlock rb);
int getDefUseLen(BaseInstruction *def, BaseInstruction *use, rISABlock rb);

#endif