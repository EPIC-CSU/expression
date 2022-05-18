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
 *file  :		RegInfoGen.cpp								 *
 *authors :		Aviral Shrivastava                           *
 *last modified : 09/10/2001                                 *
 *                                                           *
 *contents:		Register Information Generation				 *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info : aviral@ics.uci.edu                         *
 *************************************************************/

/*
I am sorry, but the file names are kind of a misnomer.

From now on, associated with each variable is a pair of information -
* <generic_class_type, generic_data_type> information for generic variables.
* <target_class_type, target_data_type> information for target variables.

This file generates three files from the IR.
1. regClassToRegClassMapping.txt
2. targetRegClassToRegsMapping.txt
3. rISARegisterMapping.txt

Inputs:

  1. OPERATIONS_SECTION --> VAR_GROUP

	target_var_name
		-> target_datatype
		-> target_registers
	
  2. OPMAPPING_SECTION --> OPERAND_MAPPING
	target_var_name
		-> generic_classtype
		-> generic_datatype


Outputs:

1. regClassToRegClassMapping.txt
	generic_datatype, generic_classtype  --> target_datatype, target_classtype

2. targetRegClassToRegsMapping.txt
	target_datatype, target_classtype  --> registers

3. rISARegisterMapping.txt
	a. target_normal_opcode, {target_normal_datatype, target_normal_classtype}
	b. target_risa_opcode, {target_risa_datatype, target_risa_classtype}
	c. target_normal_opcode => target_risa_opcode


The central data structure is 

  structure target_reg_info_struct
	{ 
		target_var_name, 
		list of <target_datatype, target_classtype>, 
		target_registers,
		list_of_target_reg_info_structs
	}

	target_reg_info = list<target_reg_info_struct>


In this file, I just make this target_reg_info data structure.
*/

#include "regInfoGen.h"

extern Symtab st;

void regInfoGen(NodePtr ir)
{
	
	TargetRegInfoMap globalTargetRegInfoMap;
	// First I have to make the target_reg_info data_structure
	//
	genTargetRegInfo(globalTargetRegInfoMap, ir, st);

	printTargetRegInfo(globalTargetRegInfoMap);

	// Now I can generate regClassToRegClassMapping file
	//
	// printRegClassToRegClassMappingFile(globalTargetRegInfoMap, ir);

	// Now generate the targetRegClassToRegsMapping file
	//
   //ATRI 
	printTargetRegClassesFile(globalTargetRegInfoMap);
	
	//AVS 03/30/2003
	printTargetRegFileList(ir);
	
	printTargetRegClassToRegsMappingFile(globalTargetRegInfoMap);

	// Also generate the rISARegisterMapping file
	//
	// printrISARegisterMappingFile(globalTargetRegInfoMap, st, ir);
	
	// done
}


