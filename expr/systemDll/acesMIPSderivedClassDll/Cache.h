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
 *file  : Cache.h                                            *
 *authors : Prabhat Kumar Mishra		                     *
 *Written : 22nd Oct 1999	                                 *
 *last modified :                                            *
 *                                                           *
 *contents : contains the definitions of sizes of caches     *
 *                                                           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *caution : always grep for TODO to check for incompleteness *
 *************************************************************/
#ifndef _CACHE_H_
#define _CACHE_H_

/***************** Specifications for Primary Cache ******************/
// Data Cache is 2048 entries x 64 bit,
// Since it stores 8 bits at a location, so 64 bits are stored in 8 locations 
// Bits 0,1,2 is used for 8 locations. 
// Tag Cache is 512 entries x 26 bit
// Virtual address is 64 bits, Physical address is 40 bits
// 3 -13 is used for accessing data cache, 5-13 is used for tag cache
// 26 bit (from tag cache) concatenated with (3-13) of address and (0-2) of alignment
// to form 40 bit.
/***********
#define _DATA_CACHE_BLOCK_SIZE_      4
#define _DATA_CACHE_ADDR_SIZE_   9 + 3 //  3-11  0,1,2 is used for alignment
#define _TAG_CACHE_ADDR_SIZE_        7 //  5-11
#define _VIRTUAL_ADDRESS_SIZE_      64 //  0-63
#define _PHYSICAL_ADDRESS_SIZE_     40 //  0-39

 
#define _ADDR_SHIFT_FOR_DATA_TAG_   (_DATA_CACHE_ADDR_SIZE_ - _TAG_CACHE_ADDR_SIZE_)  
#define _TAG_VALUE_MASK_            0x000fffffff; // (40 - 12) = 28, 1's
#define _DATA_ADDR_MASK_            0x0000000fff; // 12, 1's
#define _TAG_ADDR_MASK_             0x000000007f; // 7, 1's
#define _DATA_TAG_SHIFT_MASK_       0xffffffffe0; // (12 - 7) = 5, 0's at LSB side


/***************** Specifications for Secondary Cache ******************/
// Data Cache is 512K entries x 64 bit,
// Since it stores 8 bits at a location, so 64 bits are stored in 8 locations 
// Bits 0,1,2 is used for 8 locations.
// Tag Cache is 2^15 entries x 18 bit
// Virtual address is 64 bits, Physical address is 40 bits
// 3 -21 is used for accessing data cache, 6-21 is used for tag cache
// 18 bits (from tag cache) concatenated with (3-21) of address and (0-2) of alignment
// to form 40 bit.

/*******
#define _SEC_DATA_CACHE_BLOCK_SIZE_     16
#define _SEC_DATA_CACHE_ADDR_SIZE_  10 + 3 //  3-12 0,1,2 is used for alignment
#define _SEC_TAG_CACHE_ADDR_SIZE_       7 //  6-12
#define _SEC_VIRTUAL_ADDRESS_SIZE_      64 //  0-63
#define _SEC_PHYSICAL_ADDRESS_SIZE_     40 //  0-39

 
#define _SEC_ADDR_SHIFT_FOR_DATA_TAG_   (_SEC_DATA_CACHE_ADDR_SIZE_ - _SEC_TAG_CACHE_ADDR_SIZE_)  
#define _SEC_TAG_VALUE_MASK_            0x0007ffffff; // (40 - 13) = 27, 1's
#define _SEC_DATA_ADDR_MASK_            0x000001ffff; // 13, 1's
#define _SEC_TAG_ADDR_MASK_             0x000000007f; // 7, 1's
#define _SEC_DATA_TAG_SHIFT_MASK_       0xffffffffc0; // (13 - 7) = 6, 0's at LSB side
/***************** Specifications for Primary Cache ******************/
// Data Cache is 2048 entries x 64 bit,
// Since it stores 8 bits at a location, so 64 bits are stored in 8 locations 
// Bits 0,1,2 is used for 8 locations. 
// Tag Cache is 512 entries x 26 bit
// Virtual address is 64 bits, Physical address is 40 bits
// 3 -13 is used for accessing data cache, 5-13 is used for tag cache
// 26 bit (from tag cache) concatenated with (3-13) of address and (0-2) of alignment
// to form 40 bit.

// |<------------ Physical Address ---------------------------|
// |<--- tagValue ---->|<---- tagAddr ----->|<--- ...... ---->|
//                     |<-------- data address -------------->|

#define _DATA_CACHE_BLOCK_SIZE_      4
#define _DATA_CACHE_ADDR_SIZE_  11 + 3 //  3-13  0,1,2 is used for alignment
#define _TAG_CACHE_ADDR_SIZE_        9 //  5-13
#define _VIRTUAL_ADDRESS_SIZE_      64 //  0-63
#define _PHYSICAL_ADDRESS_SIZE_     40 //  0-39

 
#define _ADDR_SHIFT_FOR_DATA_TAG_   (_DATA_CACHE_ADDR_SIZE_ - _TAG_CACHE_ADDR_SIZE_)  
#define _TAG_VALUE_MASK_            0x0003ffffff; // (40 - 14) = 26, 1's
#define _DATA_ADDR_MASK_            0x0000003fff; // 14, 1's
#define _TAG_ADDR_MASK_             0x00000001ff; // 9, 1's
#define _DATA_TAG_SHIFT_MASK_       0xffffffffe0; // (14 - 9) = 5, 0's at LSB side


/***************** Specifications for Secondary Cache ******************/
// Data Cache is 512K entries x 64 bit,
// Since it stores 8 bits at a location, so 64 bits are stored in 8 locations 
// Bits 0,1,2 is used for 8 locations.
// Tag Cache is 2^15 entries x 18 bit
// Virtual address is 64 bits, Physical address is 40 bits
// 3 -21 is used for accessing data cache, 6-21 is used for tag cache
// 18 bits (from tag cache) concatenated with (3-21) of address and (0-2) of alignment
// to form 40 bit.


#define _SEC_DATA_CACHE_BLOCK_SIZE_     16
#define _SEC_DATA_CACHE_ADDR_SIZE_  19 + 3 //  3-21 0,1,2 is used for alignment
#define _SEC_TAG_CACHE_ADDR_SIZE_       15 //  6-21
#define _SEC_VIRTUAL_ADDRESS_SIZE_      64 //  0-63
#define _SEC_PHYSICAL_ADDRESS_SIZE_     40 //  0-39

 
#define _SEC_ADDR_SHIFT_FOR_DATA_TAG_   (_SEC_DATA_CACHE_ADDR_SIZE_ - _SEC_TAG_CACHE_ADDR_SIZE_)  
#define _SEC_TAG_VALUE_MASK_            0x000003ffff; // (40 - 22) = 18, 1's
#define _SEC_DATA_ADDR_MASK_            0x00003fffff; // 22, 1's
#define _SEC_TAG_ADDR_MASK_             0x0000007fff; // 15, 1's
#define _SEC_DATA_TAG_SHIFT_MASK_       0xffffffff80; // (22 - 15) = 7, 0's at LSB side


/************************ Used to test Main Memory involvement 
#define _SEC_DATA_CACHE_BLOCK_SIZE_     _DATA_CACHE_BLOCK_SIZE_
#define _SEC_DATA_CACHE_ADDR_SIZE_      _DATA_CACHE_ADDR_SIZE_
#define _SEC_TAG_CACHE_ADDR_SIZE_       _TAG_CACHE_ADDR_SIZE_
#define _SEC_VIRTUAL_ADDRESS_SIZE_      _VIRTUAL_ADDRESS_SIZE_
#define _SEC_PHYSICAL_ADDRESS_SIZE_     _PHYSICAL_ADDRESS_SIZE_

 
#define _SEC_ADDR_SHIFT_FOR_DATA_TAG_   (_SEC_DATA_CACHE_ADDR_SIZE_ - _SEC_TAG_CACHE_ADDR_SIZE_)  
#define _SEC_TAG_VALUE_MASK_            _TAG_VALUE_MASK_
#define _SEC_DATA_ADDR_MASK_            _DATA_ADDR_MASK_
#define _SEC_TAG_ADDR_MASK_             _TAG_ADDR_MASK_
#define _SEC_DATA_TAG_SHIFT_MASK_       _DATA_TAG_SHIFT_MASK_
******************************/

#endif // of #ifndef _CACHE_H_
