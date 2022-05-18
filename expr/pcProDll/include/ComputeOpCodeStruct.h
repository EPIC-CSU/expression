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
#ifndef __COMPUTEOPCODESTRUCT_H__
#define __COMPUTEOPCODESTRUCT_H__

// pls. note:
// the opcode's enum order is the same as the order
// that this op is in ComputeOpCodeTable
enum ComputeOpcode {
	IASSIGN,
	IADD,
	IGE,
	IASH,
	ISUB,
	ILSH,
	DMUL,
	DSUB,
	ILT 
};


enum constType {
   UNKNOWNTYPE,
   ICONSTTYPE,
   FCONSTTYPE,
   DCONSTTYPE
};


struct ComputeOpCodeStruct {
	char* name;
        enum constType retType;
        // return value indicate if successful
        // first argument store the operation need to be evalated
	int (*f)(BaseOperation*, ...); 
        unsigned int commutative:1;
        unsigned int canSource_2BeImm:1;
};

#endif
