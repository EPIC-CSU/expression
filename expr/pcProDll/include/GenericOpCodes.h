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
// See GenericOpCodes.cpp for details

#ifndef __GENERICOPCODES_H__
#define __GENERICOPCODES_H__

#define NEW_GENERIC_OPCODES

extern char *genericOpCodes[];		// defined GenericOpCodes.cpp

enum EnumGenericOpCode {
  _DEBUG_PRINT_,
  _NOP_,
  _ICONSTANT_,
  _DCONSTANT_,
  _FCONSTANT_,
  _IASSIGN_,
  _ASSIGN_,
  _DASSIGN_,
  _FASSIGN_,
  _IADD_,
  _DADD_,
  _FADD_,
  _ISUB_,
  _DSUB_,
  _FSUB_,
  _IMUL_,
  _IMULU_,
  _DMUL_,
  _FMUL_,
  _IDIV_,
  _IDIVU_,
  _DDIV_,
  _FDIV_,
  _IREM_,
  _IEQ_,
  _IEQU_,
  _DEQ_,
  _FEQ_,
  _INE_,
  _INEU_,
  _DNE_,
  _FNE_,
  _ILE_,
  _ILEU_,
  _DLE_,
  _FLE_,
  _IGE_,
  _IGEU_,
  _DGE_,
  _FGE_,
  _ILT_,
  _ILTU_,
  _DLT_,
  _FLT_,
  _IGT_,
  _IGTU_,
  _DGT_,
  _FGT_,
#ifndef NEW_GENERIC_OPCODES
  _ILSH_,
  _IASH_,
#else
  _ILLSH_,
  _IRLSH_,
  _ILASH_,
  _IRASH_,
#endif
  _ILAND_,
  _ILOR_,
  _IAND_,
  _IOR_,
  _INOR_,
  _IXOR_,
  _IVLOAD_,
  _DVLOAD_,
  _FVLOAD_,
  _HIVLOAD_,
  _HIVLOADU_,
  _QIVLOAD_,
  _QIVLOADU_,
  _IVSTORE_,
  _DVSTORE_,
  _FVSTORE_,
  _HIVSTORE_,
  _HIVSTOREU_,
  _QIVSTORE_,
  _QIVSTOREU_,
  _IF_,
  _IFFT_,
  _GOTO_,
  _IGOTO_,
  _CALL_,
  _RETURN_,
  _CVTDI_,
  _CVTSI_,
  _CVTSD_,
  _CVTDS_,
  _DMTC1_,
  _DMFC1_,
  _DNEG_,
  _MFC1_,
  _MTC1_,
  _TRUNCID_,
  _CVTID_,
  _TRUNCIS_,
#ifdef NEW_GENERIC_OPCODES
  _MFHI_,
  _MFLO_,
  _MTHI_,
  _MTLO_,
#endif
//  _PRINTF_,
//  _SQRT_,
//  _EXP_,
};
EnumGenericOpCode getGenericOpCodeEnumIndex(char *opCodeName);

#endif
