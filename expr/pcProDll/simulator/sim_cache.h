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
#ifndef _SIM_CACHE_H
#define _SIM_CACHE_H

#include "stdhdr.h"
#include "DataValue.h"
#ifdef WIN32
#include <map>
#define multimap std::multimap
#define less std::less
#else
#include <multimap.h>
#endif

const int one_k = (0x1<<10);
const int default_cache_read_latency = 1, default_cache_write_latency = 1;
const default_cache_line_size = 8*sizeof(int);
const default_cache_size = 16*one_k;

class cache_element
{
  int _cache_in_cycle;  //the time that this element is copied from memo ry
  int _last_access_cycle;  //the last time that this element is accessed
  int _is_dirty; //has been written
  int _mem_addr; //the memory address of first content
public:
  cache_element(int cache_line_size = default_cache_line_size);
  ~cache_element();
  void init();
  void read_from_mem();
  void write_to_mem();
  void read();
  void write();

  friend class sim_cache;
} ;

class sim_cache
{
  int _cache_size, _cache_line_size;
  int _read_latency, _write_latency;
  int _read_miss,_read_hit,_write_miss,_write_hit;
  cache_element *_elements;
//use map to establish the relationship between mem address and cache elements
//   <mem_address, index into cache elements, ...>
  multimap<int,int, less< int> > _mem_cache_map; 
public:
  sim_cache(int size=default_cache_size, int line_size = 
	default_cache_line_size, int r_l=default_cache_read_latency, 
 	int w_l=default_cache_write_latency);
  ~sim_cache();
  void create();
  void set_cache_size( int k) { _cache_size = k*one_k; }
  void set_cache_line_size(int s) { _cache_line_size = s;}
  void set_read_latency(int l) { _read_latency = l; }
  void set_write_latency(int l) { _write_latency = l; }
  int get_cache_line_size() { return _cache_line_size; }

//return value is the latency in read
  int read(int,VALUE_TYPE,val&);
  int write(lhsMem& );
//determine the index into the cache of a mem address 
//return -1 if not in cache yet
  int get_cache_index(int mem_addr);        
  int get_avail_cache_index();
//when cache is full, find a cache element for the new data using an algorithm
//that can be changed indepently
  int get_replacement_cache_index();    
//bring new memory contents to cache
  void update_cache(int index, int addr);
//write cache contents to memory when needed, and put this cache element ready
//for reuse
  void update_mem(int index);
//write back all the cache contents to memory
  void write_back_all();
};

#endif
