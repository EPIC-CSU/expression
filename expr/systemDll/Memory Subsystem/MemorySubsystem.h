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
 *file  : MemorySubsystem.h                                  *
 *created : Feb 18, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Memory subsystem class     *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _MEMORY_SUBSYSTEM_H_
#define _MEMORY_SUBSYSTEM_H_

#include <stdio.h>
#include "MemData.h"

 
class MemorySubsystem
{
  private:
	  int _stallBit; // Is marked "1" if memory is stalled, else "0"
	                 // Set by preprocess or IsReadDepSatisfied or IsWriteDepSatisfied

	  vector<MemData> ReadQueue;  // Queue which holds read requests
	  vector<MemData> WriteQueue; // Queue which holds write requests

	  map<int, int> BankReadAccess; // Holds info regarding which bank is read how many times in the same cycle.
	  map<int, int> BankWriteAccess;// Holds info regarding which bank is written how many times in the same cycle.

	  int IsReadDepSatisfied(int bankNum);   // Checks whether Read Dependency is satisfied or not
	  int IsWriteDepSatisfied(int bankNum);  // Checks whether Write Dependency is satisfied or not

  public:
	  MemorySubsystem();
	  ~MemorySubsystem();

	  void SendReadRequest(long addr, int opcode);               // Memory controller sends read request
	  void SendWriteRequest(long addr, Val & value, int opcode); // Memory controller sends write request

	  long ReceiveData(Val & value, int & opcode, long address=0); // Memory controller receives data, if available
	                                                 // "value" contains the data and return value is the address

	  int IsMemStalled();                            // Returns '1' if memory is stalled                            
	  
	  void doStep();                                 // Should be executed every cycle
	  void preprocess();                             // Should be executed before doStep invocation.
	  int getBank(long address);                     // Determines the bank number from the given address
	  void flushQueues();
	  bool hasDataWritten(long address); // whether data has already been written
};
 
#endif
