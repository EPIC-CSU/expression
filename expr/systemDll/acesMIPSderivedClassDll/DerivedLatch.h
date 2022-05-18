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
 *file  : DerivedLatch.h                                     *
 *authors : Prabhat, Srikanth                                *
 *created : 3 May, 2000.                                     *
 *last modified :                                            *
 *                                                           *
 *compiled with : g++                                        *
 *for more info : contact Ashok at ahalambi@ics.uci.edu      *
 *************************************************************/
#ifndef _DERIVEDLATCH_H_
#define _DERIVEDLATCH_H_

#include "DerivedClassIncludes.h"
#include "DerivedLatchData.h"
#include "Latch.h"

// PCLatchData contains instruction memory address pointed by PC
typedef SimpleLatch<PCLatchData> PCLatch;

// InstrStrLatchData is a list of strings where each string is an operation
typedef SimpleLatch<InstStrLatchData> InstStrLatch;

// InstructionLatchData supports list of operations (OperationLatchData)
typedef SimpleLatch<InstructionLatchData> InstructionLatch;


// OperationLatchData supports list of sources and list of detinations
typedef SimpleLatch<OperationLatchData> OperationLatch;



#endif
