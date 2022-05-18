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
 *file  : MemData.h                                          *
 *created : Feb 18, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of MemData class              *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _MEM_DATA_H_
#define _MEM_DATA_H_

#include <stdio.h>
#include "AddrData.h"
#include "MemDefines.h"

class MemData
{
  private:
	  long _address;  // Address of read/write location
	  Val _value;     // Value read or to be written
	  int _latency;   // If latency is '0' then it's ready to be read/written
	  int _opcode;    // Opcode  e.g., IVLOAD/IVSTORE .....
	  MemAccessState _state; // State of the read/write
  public:
	  MemData();
	  MemData(long, Val &, int, int, MemAccessState);
	  MemData(const MemData & mData);
	  ~MemData();

	  void setAddress(long address);
	  void setValue(Val & value);
	  void setLatency(int latency);
	  void setOpcode(int opcode);
	  void setState(MemAccessState state);

	  long getAddress();
	  Val & getValue();
	  int getLatency();
	  int getOpcode();
	  MemAccessState getState();

	  MemData& operator=(const MemData& b) ;
};

#endif
