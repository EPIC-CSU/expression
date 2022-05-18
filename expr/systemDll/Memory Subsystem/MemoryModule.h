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
 *file  : MemoryModule.h                                     *
 *created : Mar 16, 2000                                     *
 *authors : Gaby Grun, Prabhat Mishra                        *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Dram class                 *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _MEMORY_MODULE_H_
#define _MEMORY_MODULE_H_

#include<string.h>
#include<iostream.h>
#include<assert.h>

class Val;


//this is the base class for all the memory module classes
class MemoryModule
{
  private:
	  char *_memType;

  protected:
	  //the number of cache hits, misses, total accesses
      unsigned long read_hits,read_misses,write_hits, write_misses, cache_accesses;

	  // Memory Module Power dissipation components.
	  // This could be CACHEs/DRAMs or any other memory modules...
	  double _decoder_power, _wordline_power, _bitline_power, _senseamp_power, _tagarray_power;

	  int _latency;
	  
	  //the input and output connections
	  int in_connect,out_connect;
	  
  public:
	  MemoryModule(void)
	  {
		  _memType=NULL;
		  in_connect=-1;
		  out_connect=-1;
		  _latency = 0;
		  read_hits = 0;
		  read_misses = 0;
		  write_hits = 0;
		  write_misses = 0;
		  cache_accesses = 0;

		  
		  //initialize the power component values.
		  _decoder_power = 0; 
		  _wordline_power = 0;
		  _bitline_power = 0;
		  _senseamp_power = 0;
		  _tagarray_power = 0;

	  }

	  virtual ~MemoryModule()
	  {
		  if (_memType != NULL)
			  delete _memType;
		  _memType = NULL;
	  }
	  
	  //try to read from the direct-mapped cache
	  virtual void read(long addr, Val & value, int opcode,int &latency)=0;
	  //try to write to the direct-assoc cache
	  virtual void write(long addr, Val & value, int opcode,int &latency)=0;

	  // Initializes the memory... 
	  virtual void init()
	  {
		  // note: this function should not be called.
		  //
		  cout << "Panic: Tried to call base class method (MemoryModule::init)" << endl;
		  assert(0);
	  }

	  // memory initialization routine.
	  //
	  virtual void initmem( long strtaddr, char *loadtype, char *fname)
	  {
		  // note: this function should not be called.
		  //
		  cout << "Panic: Tried to call base class method (MemoryModule::initmem)" << endl;
		  assert(0);
	  }

	  virtual void print(ostream & out)
	  {
		  // note: this function should not be called.
		  //
		  cout << "Panic: Tried to call base class method (MemoryModule::print)" << endl;
		  assert(0);
	  }

	  //adds an input connection
	  void add_in_connection(int c)
	  {
		  in_connect=c;
	  }
	  
	  //adds an output connection
	  void add_out_connection(int c)
	  {
		  out_connect=c;
	  }
	  
	  //prints statistics
	  virtual void print_statistics(FILE *)=0;

	  //returns the _memType of the module
	  void set_mem_type(char *t)
	  {
		  if (t != NULL)
			  _memType=strdup(t);
	  }
	  
	  char *get_mem_type(void)
	  {
		  return _memType;
	  }

	  unsigned long getReadHits()
	  {
		  return read_hits;
	  }

	  unsigned long getWriteHits()
	  {
		  return write_hits;
	  }

	  unsigned long getCacheAccesses()
	  {
		  return cache_accesses;
	  }

	  unsigned long getReadMisses()
	  {
		  return read_misses;
	  }

	  unsigned long getWriteMisses()
	  {
		  return write_misses;
	  }

	  void setReadHits(unsigned long cHits)
	  {
		  read_hits = cHits;
	  }

	  void setWriteHits(unsigned long cHits)
	  {
		  write_hits = cHits;
	  }

	  void setCacheAccesses(unsigned long cAccesses)
	  {
		  cache_accesses = cAccesses;
	  }

	  void setReadMisses(unsigned long cMisses)
	  {
		  read_misses = cMisses;
	  }

	  void setWriteMisses(unsigned long cMisses)
	  {
		  write_misses = cMisses;
	  }
};

#endif
