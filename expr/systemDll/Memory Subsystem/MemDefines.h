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
 *file  : MemDefines.h                                       *
 *created : Feb 28, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios for MemorySubsystem           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _MEM_DEFINES_H_
#define _MEM_DEFINES_H_

#include<stdio.h>

enum MemAccessState
{
	_WAITING_,  // Just got into the read/write queue
	_READING_,  // Initiated the read, not yet complete
	_WRITING_,  // Initiated the write, not yet complete
	_STALLED_,  // Stalled read/write due to access conflict
	_DONE_      // Read/write completed.
};


enum MemType
{
	ASSOC_CACHE,
	DIRECT_CACHE,
	_DRAM_,
	_SRAM
};

//variable for LRU testing
#define LRU 1
//number of parallel reads per cycle allowed
#define NO_READS_PER_CYCLE 1
//size of the read queue
#define MAX_READQ_SIZE 3
//number of writes per cycle allowed
#define NO_WRITES_PER_CYCLE 1
//size of write queue
#define MAX_WRITEQ_SIZE 3
//number of parallel read accesses to the same bank
#define NO_PARALLEL_READ_ACCESS 1
//number of parallel write accesses to the same bank
#define NO_PARALLEL_WRITE_ACCESS 1
//READ_WRITE_CONFLICT=1 -> same bank can NOT have parallel read and write accesses
//READ_WRITE_CONFLICT=0 -> same bank can have parallel read and write accesses
#define READ_WRITE_CONFLICT 1
//address space
#define HIGH_ADDRESS 1000000000
#define LOW_ADDRESS 0
//number of banks
#define NO_OF_BANKS 2 
//if LRU (1) least recently used policy replacement
//if not LRU (0) FIFO replacement policty
#define REPLACEMENT_ALGORITHM LRU
 
#include "Cache.h"
#include "AssociativeCache.h"
#include "DirectCache.h"
#include "MemoryModule.h"
#include "Sram.h"
#include "Dram.h"
#include "StreamBuffer.h"



#endif