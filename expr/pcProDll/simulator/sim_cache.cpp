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
#include "stdafx.h"

#include "sim_cache.h"
#include "simulator.h"
#include "DataValue.h"

#ifdef WIN32
#define pair std::pair
#define multimap std::multimap
#endif

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

//read policy: if in cache, read it directly.
//             if not in cache, first bring to cache, then read it
//write policy: if in cache, write it to cache, set dirty bit
//		if not in cache, write it memory directly (write through)
cache_element::cache_element(int cache_line_size)
{
  init();
}

cache_element::~cache_element()
{
}

void cache_element::init()
{
  _cache_in_cycle = 0;
  _last_access_cycle = 0;
  _is_dirty = 0;
  _mem_addr= -1;
}

void cache_element::read_from_mem()
{
  _cache_in_cycle = simulator::current->get_current_cycle();
}

void cache_element::write_to_mem()
{
  _is_dirty = 0;
}

void cache_element::read()
{
  _last_access_cycle = simulator::current->get_current_cycle();
}

void cache_element::write()
{
  _last_access_cycle = simulator::current->get_current_cycle();
  _is_dirty = 1;
}

sim_cache::sim_cache(int s, int ls, int r_l, int w_l)
  :_cache_size(s), _cache_line_size(ls), _read_latency(r_l), _write_latency(w_l)
{
  _write_miss = 0;
  _write_hit = 0;
  _read_miss = 0;
  _read_hit = 0;
  _elements = NULL;
}

sim_cache::~sim_cache()
{
  delete []_elements;
}

void sim_cache::create()
{
  int ele_count = _cache_size/_cache_line_size;
  _elements = new cache_element[ele_count];
//initialize the mapping between cache and memory
//initially, no mem in cache, so address is initialized to -1
  for (int index=0; index<ele_count; index++)
    _mem_cache_map.insert(pair<const int, int>(-1,index));
}

int sim_cache::read(int addr, VALUE_TYPE mode, val& v)
{
  //int size,size1,size2;
  int cache_index ;//,avail_index;
  simulator::current->_mem->read(addr,mode,v);
  int latency = 0;

  cache_index = get_cache_index(addr);
  if (cache_index==-1) {//not in cache yet
    cache_index = get_avail_cache_index();
    update_cache(cache_index,addr);
    latency = simulator::current->_mem->get_read_latency();
    _read_miss++;
  }
  else {
    _read_hit++;
    latency = _read_latency;
  }
  return latency;
}

int sim_cache::write(lhsMem& v)
{
  int cache_index;
  int latency = 0;

  cache_index = get_cache_index(v._addr);
  if (cache_index==-1) {
//not in cache yet,write through
    latency = simulator::current->_mem->get_write_latency();
    _write_miss++;
  }
  else {
    latency = _write_latency;
    _write_hit++;
  }
  simulator::current->_mem->write(v);
  return latency;
}
 
int sim_cache::get_cache_index(int mem_addr)
{
  int aligned_addr = mem_addr - mem_addr%_cache_line_size;
  multimap<int,int,less<int> >::iterator iter = _mem_cache_map.find(aligned_addr);
  if (iter==_mem_cache_map.end())//not in cache yet
    return -1;
  else
    return (*iter).second;
}

int sim_cache::get_avail_cache_index()
{
  multimap<int,int,less<int> >::iterator iter = _mem_cache_map.find(-1);
  if (iter!=_mem_cache_map.end()) //cache not full yet
    return (*iter).second;
  return get_replacement_cache_index();
}

void sim_cache::update_cache(int index, int new_addr)
{
  int old_addr = _elements[index]._mem_addr;
  int aligned_addr = new_addr - new_addr%_cache_line_size;
  multimap<int,int,less< int > >::iterator iter;
  iter = _mem_cache_map.find(old_addr);
  if (iter==_mem_cache_map.end())
    panic("Error in cache!!!");
  else {
    assert(index==(*iter).second);
    _mem_cache_map.erase(iter);
    _mem_cache_map.insert(pair<const int,int>(aligned_addr,index));
  }
  _elements[index].init();
  _elements[index].read_from_mem();
}

void sim_cache::update_mem(int index)
{
  if (_elements[index]._is_dirty)
    _elements[index].write_to_mem();
  int old_addr = _elements[index]._mem_addr;
  multimap<int,int,less< int > >::iterator iter;
  iter = _mem_cache_map.find(old_addr);
  if (iter==_mem_cache_map.end())
    panic("Error in cache!!!");
  else {
    assert(index==(*iter).second);
    _mem_cache_map.erase(iter);
    //make it ready for reuse
    _mem_cache_map.insert(pair<const int, int> (-1,index));
    _elements[index].init();
  }
}

void sim_cache::write_back_all()
{
  int ele_count = _cache_size/_cache_line_size;
  for (int index=0; index<ele_count; index++)
    if (_elements[index]._is_dirty)
      _elements[index].write_to_mem();
}

int sim_cache::get_replacement_cache_index()
{
  assert(0);
  //NOS
  return 0;
}
