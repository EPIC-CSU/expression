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
 *file  : DerivedUnit.cpp                                    *
 *created : 3 May, 2000                                      *
 *authors : Prabhat, Srikanth                                *
 *last modified :                                            *
 *                                                           *
 *contents:contains macros and other definitions for control *
 *         table.                                            *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef __DERIVED_CLASS_INCLUDES_H__
#define __DERIVED_CLASS_INCLUDES_H__

#ifdef WIN32
#pragma warning(disable:4786)
#endif

 

typedef class BaseInstruction *InstrPtr;

const SCALARITY = 5;
const numOfDataUnits = 4;
const numOfControlUnits = 1;

#define NUM_DATA_OPS_PER_INSTR 4
#define NUM_CONTROL_OPS_PER_INSTR 1
#define NUM_FLOW_OPS_PER_INSTR 2

const numPipes = 5;
 
const FETCH_STAGE_NUM = 0;
const DECODE_STAGE_NUM = 1;
const OPREAD_STAGE_NUM = 2;
const EXECUTE_STAGE_NUM = 3;
const WRITEBACK_STAGE_NUM = 4;
 

#define _BAD_REG_FILE -1
#define _INT_REG_FILE 0
#define _FLT_REG_FILE 1
#define _MISC_REG_FILE 2


#define MaxNumStages 8  

#define IN_LATCH_ELEM(q) (_inputLatches[q])
#define OUT_LATCH_ELEM(q) (_outputLatches[q])
#define OTHER_LATCH_ELEM(q) (_otherLatches[q])

#define MEM_RD_PORT _ports[0]
#define MEM_WR_PORT _ports[1]

const int _WORD_SIZE_ = 4;
const int _HALF_SIZE_ = 2;
const int _BYTE_SIZE_ = 1;

#endif