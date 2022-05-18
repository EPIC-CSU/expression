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
 *created : March 15, 2000                                   *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Cache class                *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _CACHE_H_
#define _CACHE_H_

#include "AddrData.h"
#include "MemoryModule.h"

// Input is number of lines, number of words in a line and size of each word in bytes.
// template <long _No_Lines_, int _Line_Size_, int _Word_Size_>
class Cache
{
	//
	// |<---------------- Physical Address ------------------------->|
	// |<--- tagValue --->|<-- rowNo -->|<-- ColNo -->|<-- ByteNo -->|
	//                                          
  private:
	  //Val _cache[_No_Lines_][_Line_Size_];
	  //long _tag[_No_Lines_];
	  //int _valid[_No_Lines_];
	  Val **_cache;
	  long *_tag;
	  int **_valid;

	  int _No_Lines_;
	  int _Line_Size_;
	  int _Word_Size_;

	  long _rowMask;
	  long _colMask;

	  long _rowShift;
	  long _colShift;
	  long _tagShift;

  public:
	  Cache(long No_Lines, int Line_Size, int Word_Size)
	  {
		_No_Lines_ = No_Lines;
		_Line_Size_ = Line_Size;
		_Word_Size_ = Word_Size;

		_cache = (Val **)malloc(_No_Lines_*sizeof(Val *));
		// changed _valid to a 2 dimensional array
		// 1 valid bit for each word in line
		// 03/30/03 - Sudeep
		_valid = (int **)malloc(_No_Lines_*sizeof(int *));
        for (int i=0; i<_No_Lines_; i++) {
			_cache[i] = (Val *)malloc(_Line_Size_*sizeof(Val));
			_valid[i] = (int *)malloc(_Line_Size_*sizeof(int));
		}

		_tag = (long *)malloc(_No_Lines_*sizeof(long));



		  _rowMask = (_No_Lines_ * _Line_Size_ * _Word_Size_) - 1;
		  _colMask = (_Line_Size_ * _Word_Size_) - 1;
 	      _tagShift = log_2(_Line_Size_ * _No_Lines_ * _Word_Size_); // Shift needed for tagValue
          _rowShift = log_2(_Line_Size_ * _Word_Size_); // Shift needed for row address
		  _colShift = log_2(_Word_Size_); // Shift needed for col address

		  // Initialize _valid & _tag and _cache array
		  init();
	  }

	  ~Cache(){
		  for (int i=0; i<_No_Lines_; i++)
		  {
			  free(_cache[i]);
			  //added by Sudeep 03/30/03
			  free(_valid[i]);
		  }
		  free(_cache);
		  free(_tag);
		  free(_valid);
	  }
	  Cache& Cache::operator=(const Cache &a)
	  {
		return *this;
	  }

	  void init()
	  {
		  // Initialize _valid & _tag and _cache array
		  for (int i = 0; i < _No_Lines_; i++)
		  {

			  _tag[i] = -1;

			  for (int j = 0; j < _Line_Size_; j++)
			  {
				  Val value;
				  _cache[i][j] = value;
			  //added by Sudeep 03/30/03
				  _valid[i][j] = 0;
			  }
		  }
	  }

	  long log_2(long decimal)
	  {
		  // Implemented only for positive integers.
		  long result = 0;
		  
		  while(decimal > 1)
		  {
			  result++;
			  decimal = decimal/2;
		  }
		  

		  return result;
	  }

/*	  int delayedRead(long address, Val & value, int opcode, int & latency)
	  {
		  long rowNo = (address & _rowMask) >> _rowShift;
		  int colNo = (address & _colMask) >> _colShift;
		  long tagValue = address >> _tagShift;

		  // Check if delayed, 
		  // In this implementation we read the data immediately
		  // with latency value set to _valid[rowNo], unless that 
		  // latency is 1, all the elements in that line are not
		  // ready to be read. This technique is used otherwise
		  // first read with return with a latency n, whereas 
		  // remaining reads in the line with return with latency 1.

		  // The latency of the first read in that line gets decremented
		  // in memory Subsystem readQ and when latency is 1, that value 
		  // will be reflected in this line. Subsequent reads to that
		  // line will get data from next cycle onwards.
		  
		  if (opcode == 999)
		  {
			  // Delay waiting is over.
			  _valid[rowNo] = 1;
		  }

		  if (_valid[rowNo] > 1)
			  return 0; // DELAYED

		  // Check if hit
		  if ((_tag[rowNo] == tagValue) && (_valid[rowNo] == 1))
		  {
			  value = _cache[rowNo][colNo];
			  return 1;  // HIT
		  }
		  else
			  return -1; // MISS
	  } */

	  int read(long address, Val & value, int opcode, int & latency)
	  {
		  long rowNo = (address & _rowMask) >> _rowShift;
		  int colNo = (address & _colMask) >> _colShift;
		  long tagValue = address >> _tagShift;


		  // Check if hit
		  if ((_tag[rowNo] == tagValue) && (_valid[rowNo][colNo] >= 1))
		  {
			  value = _cache[rowNo][colNo];
			  return 1;  // HIT
		  }
		  else
			  return -1; // MISS
	  }

	  int write(long address, Val & value, int opcode, int & latency)
	  {
		  long rowNo = (address & _rowMask) >> _rowShift;
		  int colNo = (address & _colMask) >> _colShift;
		  long tagValue = address >> _tagShift;

		  // Check if hit or free so can't write in this bank
		  if (_tag[rowNo] == tagValue)
		  {

			for (int i=0; i < _Line_Size_;i++)
			{
			  if (_valid[rowNo][i] != 0)
				  return -1; //the row has valid data
			}

			// the row has no valid data
			  _cache[rowNo][colNo] = value;
			  _tag[rowNo] = tagValue;
			  _valid[rowNo][colNo] = 1;
			  return 1;
		  }

			  return -1;		  

	  }

/*	  int delayedWrite(long address, Val & value, int opcode, int & latency)
	  {
		  long rowNo = (address & _rowMask) >> _rowShift;
		  int colNo = (address & _colMask) >> _colShift;
		  long tagValue = address >> _tagShift;

		  // Delayed write
		  // If writing for the first time make it a compulsory miss
		  // Perform refill and then write
		  if (_valid[rowNo] == 0)
			  return 0;

		  // Check if hit or free so can't write in this bank
		  if (_tag[rowNo] == tagValue)
		  {
			  _cache[rowNo][colNo] = value;
			  _tag[rowNo] = tagValue;
			  _valid[rowNo] = 1;
			  return 1;
		  }
		  else
			  return -1;
	  } */

	  int replace(MemoryModule* nextModule, int address, int opcode, int & latency)
	  {
		  long rowNo = (address & _rowMask) >> _rowShift;
		  int colNo = (address & _colMask) >> _colShift;
		  int i;		  

		  // Nothing to replace if not a valid line.
		  //if (_valid[rowNo] == 0) return 0;
			for (i=0; i < _Line_Size_;i++)
			{
			  if (_valid[rowNo][i] != 0)
				  break;
			}
			if (i == _Line_Size_)
				return 0;
			i = 0;

		  // Get the tagValue for the line
		  long tagValue = _tag[rowNo];

		  // Replace word by word
		  int rNo = rowNo;
		  Val value;
		  Val tempValue;
		  long tempAddress = (tagValue << _tagShift) + (rNo << _rowShift);


		  int tLatency;
		  for (i=0; i < _Line_Size_; i++)
		  {
			  value = _cache[rowNo][i];

			  // Make it NULL
			  _cache[rowNo][i] = tempValue;

			  // Form the target address for this word (value);
              long tAddress = tempAddress + (i << _colShift);

			if (_valid[rowNo][i])
			  nextModule->write(tAddress, value, opcode, tLatency);
			  if (i==0)
				  latency = tLatency;
		  }

		  // Mark this line as not valid
		  //_valid[rowNo] = 0;
			for (i=0; i < _Line_Size_;i++)
			{
			  _valid[rowNo][i] = 0;
			}

		  // Only one of all the writes should be counted.
		  nextModule->setWriteHits(nextModule->getWriteHits() - _Line_Size_ + 1);
		  nextModule->setCacheAccesses(nextModule->getCacheAccesses() - _Line_Size_ + 1);

		  return 1;
	  }
	  	  
	  
	  int refill(MemoryModule* nextModule, int address, int opcode, int & latency)
	  {
		  // Reads a line containing data for 'address' from higher order 
		  // cache/memory and refill in this cache.

		  long rowNo = (address & _rowMask) >> _rowShift;
		  int colNo = (address & _colMask) >> _colShift;
		  long tagValue = address >> _tagShift;

		  _tag[rowNo] = tagValue;

		  // Refill word by word
		  int rNo = rowNo;
		  Val value;
		  long tempAddress = (tagValue << _tagShift) + (rNo << _rowShift);

		  int i, temp;
		  int tLatency;
		  for (i=0; i < _Line_Size_; i++)
		  {
			  // Form the target address for this word (value);
              long tAddress = tempAddress + (i << _colShift);

			  temp = nextModule->read(tAddress, value, opcode, tLatency);

			  if (i==0)
			  {
				  // For all the reads latency should be same
				  latency = tLatency;
			  }

			  if (temp == 0)
				 _valid[rowNo][i] = 0;
			  else
				 _valid[rowNo][i] = latency;
			  
			  _cache[rowNo][i] = value;
		  }

		  // Mark this line as valid
		  //_valid[rowNo] = latency;
		  
		  // Only one of all the reads should be counted.
		  nextModule->setReadHits(nextModule->getReadHits() - _Line_Size_ + 1);
		  nextModule->setCacheAccesses(nextModule->getCacheAccesses() - _Line_Size_ + 1);
		  
		  return 1;
	  }
};
#endif