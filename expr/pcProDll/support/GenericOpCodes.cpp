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
// $Id: GenericOpCodes.h,v 1.0 1999/11/24 sumitg $
//
// File:  		GenericOpCodes.h
// Created:		Wed Nov 24 , 99
// Last modified: 	Web Nov 24 , 99
// Author:		Sumit Gupta
// Email:		sumitg@ics.uci.edu
//
//   
// This file contains the generic opcodes that are recognized by the
// system when generating code in C or VHDL
// Note that the file mainly has 2 arrays, one of strings and one of
// enums.  If a generic Opcode is added or deleted, both these arrays
// have to edited.  Tedious, but it saves a lot of grief at other places
// 
// So, say you are looking for the enum value of IADD, search through
// the genericOpCodes array of strings, and find the index/position of
// this string.  This is the enum value of the OpCode
//

#include "stdafx.h"
#include "GenericOpCodes.h"

char *genericOpCodes[] = {
  "DEBUG_PRINT",
  "NOP",
  "ICONSTANT",
  "DCONSTANT",
  "FCONSTANT",
  "IASSIGN",
  "ASSIGN",
  "DASSIGN",
  "FASSIGN",
  "IADD",
  "DADD",
  "FADD",
  "ISUB",
  "DSUB",
  "FSUB",
  "IMUL",
  "IMULU",
  "DMUL",
  "FMUL",
  "IDIV",
  "IDIVU",
  "DDIV",
  "FDIV",
  "IREM",
  "IEQ",
  "IEQU",
  "DEQ",
  "FEQ",
  "INE",
  "INEU",
  "DNE",
  "FNE",
  "ILE",
  "ILEU",
  "DLE",
  "FLE",
  "IGE",
  "IGEU",
  "DGE",
  "FGE",
  "ILT",
  "ILTU",
  "DLT",
  "FLT",
  "IGT",
  "IGTU",
  "DGT",
  "FGT",
#ifndef NEW_GENERIC_OPCODES
  "ILSH",
  "IASH",
#else
  "ILLSH",
  "IRLSH",
  "ILASH",
  "IRASH",
#endif
  "ILAND",
  "ILOR",
  "IAND",
  "IOR",
  "INOR",
  "IXOR",
  "IVLOAD",
  "DVLOAD",
  "FVLOAD",
  "HIVLOAD",
  "HIVLOADU",
  "QIVLOAD",
  "QIVLOADU",
  "IVSTORE",
  "DVSTORE",
  "FVSTORE",
  "HIVSTORE",
  "HIVSTOREU",
  "QIVSTORE",
  "QIVSTOREU",
  "IF",
  "IFFT",
  "GOTO",
  "IGOTO",
  "CALL",
  "RETURN",
  "CVTDI",
  "CVTSI",
  "CVTSD",
  "CVTDS",
  "DMTC1",
  "DMFC1",
  "DNEG",
  "MFC1",
  "MTC1",
  "TRUNCID",
  "CVTID",
  "TRUNCIS",
#ifdef NEW_GENERIC_OPCODES
  "MFHI",
  "MFLO",
  "MTHI",
  "MTLO",
#endif
// Radu - May 21, 2000
//  "SQRT",
//  "EXP",
  "EOF"				/* just a end of array symbol */
};

EnumGenericOpCode getGenericOpCodeEnumIndex(char *opCodeName) {
  int i=0;
  while (strcmp(genericOpCodes[i], "EOF") != 0)
    { 
      if (strcmp(genericOpCodes[i], opCodeName) == 0)
	break;
      i++;
    }
  return EnumGenericOpCode(i);
} /* end of fcn */


