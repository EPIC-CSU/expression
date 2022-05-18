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
#ifndef _SIM_MEM_H
#define _SIM_MEM_H

#include "stdhdr.h"
#include "bitset.h"
#include "DataValue.h"
#include "sim_cache.h"
#include "string.h"

class dyn_array {
private:
  int size;
  void grow(const int newsize);
protected:
  int *data;
public:
  dyn_array(void) { data = 0; size = 0; };
  dyn_array(const dyn_array& from) {
      size = from.size;
      data = (int *) malloc(size * sizeof(int));
      bcopy((char *)from.data, (char *)data, size*sizeof(int));
  };
  ~dyn_array(void) {
     if (data) free((char *) data);
  };

  void operator=(const dyn_array& from) {
      if (size < from.size)
      {
         if (data)  // 5Jan97 was using realloc
            free(data);
         data = (int *) malloc(from.size * sizeof(int));
      }
      size = from.size;
      bcopy((char *)from.data, (char *)data, size*sizeof(int));
  }

  int& operator[](int i) {
          if (i >= size)
             grow(i);
          return data[i];
  }

  void zero() {
     if (size)
        bzero((char *)data, size*sizeof(int));
  }
  int ub() const { return size; }
} ;

class byte_array : public dyn_array {
public:
  enum dir { up=1, down=-1 };
  dir increment;
  bitset already_initialized;

  // dyn_array is actually an array of int's, but
  // our memory is addressed as bytes so:
  int ub() {
     return dyn_array::ub() * sizeof(int);
  }
  void grow(int n_bytes) {
     (*this)[ (n_bytes + sizeof(int)-1) / sizeof(int) ];
  }
  void write(int addr, val& v) ;
  void read(int addr, VALUE_TYPE mode, val& v);

  void touch(int addr, VALUE_TYPE mode);
  int touched(int addr, VALUE_TYPE mode);
};

//determine if the bytes of a machine is organized from left to right or from
//right to left, (Unfinished: make it parameter later)
#define endianess_matches_host 1

const int one_meg = (0x1<<20);
const  int default_mem_read_latency = 5, default_mem_write_latency = 1;
const mem_size = 16*one_meg;

class sim_mem {
  int size;
  int seg_size;
  byte_array heap;
  byte_array stack;
  int watch_addr;
  int read_latency, write_latency;
public:
  sim_mem(int megabytes = 16, int r_l=default_mem_read_latency, 
    int w_l=default_mem_write_latency) {
     read_latency = r_l;
     write_latency = w_l;
     watch_addr = -1;
     set_size(megabytes);
     heap.increment = byte_array::up;
     stack.increment = byte_array::down;
  }

  void set_size(int mb) {
     size = mb * one_meg;
     seg_size = size/2;
  }

  int legal_addr(int addr) ;
  int write(lhsMem& v) ;
  int read(lhsMem& v);
  int write(long addr, val& value);
  int read(int, VALUE_TYPE, val&);
  int read(int, val&);

  void write_breakpoint();
  void touch(int addr, VALUE_TYPE mode);
  int touched(int addr, VALUE_TYPE mode);

  void zero() {
     heap.zero();
     stack.zero();
  }

  int getSize() {
    return size;
  }

  int get_read_latency() { return read_latency; }
  int get_write_latency() { return write_latency; }
};

#endif
