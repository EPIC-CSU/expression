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
 *file  :		PredicationInfoGen.h		                 *
 *authors :		Aviral Shrivastava                           *
 *last modified : 07/25/2000                                 *
 *                                                           *
 *contents:		Predication Information Generation Header    *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info : aviral@ics.uci.edu                         *
 *************************************************************/

#include "node.h"

void predicationInfoGen(NodePtr ir);
void printCorresTarAssign(NodePtr ir, FILE *predInfoFilePtr);
void print_isels(NodePtr opMappingNode, FILE *predInfoFilePtr);
void printTargetOpCodeFile(NodePtr ir);
void printPredInfoFile(NodePtr ir);
void printOpInFile(NodePtr operationNode, FILE *targetOpCodeFilePtr);
