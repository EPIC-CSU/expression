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

#include "sim_mem.h"
#include "simulator.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

int bad_memory_access;

sim_mem *globalSimMem;

void
dyn_array::grow(int i)
{
        if (size == 0)
        {
            size = 2*i+1;
            if (!(data = (int *) calloc(size, sizeof(int))))
               panic("dyn_array[]:  out of memory");
        }
        else
        {
           int new_size = 2*i+1;

           int *new_data = (int *)calloc(new_size, sizeof(int));
           if (! new_data)
              panic("dyn_array[]:  out of memory");
           bcopy((char *)data, (char *)new_data, size*sizeof(int));
           free(data);
           data = new_data;

           // 23Dec96 linux has trouble with realloc?
           // if (!(data = (int *) realloc((char *)data, new_size*sizeof(int))))
              // panic("dyn_array[]:  out of memory");
           // bzero((char *)(data+size), (new_size - size)*sizeof(int));

           size = new_size;
        }
}

void byte_array::write(int addr, val& v)
{
           if ( (addr + sizeof(double)) > ub() )
              grow( addr + sizeof(double) );

           char *to = ((char *)data)+addr;
           char *from;
           int size;

           switch(v.val_type)
           {
           case _INT :
              from = (char *)&(v._as.integer);
              size = sizeof(int);
              break;
           case _SHORT :
              from = (char *)&(v._as.short_int);
              size = sizeof(short);
              break;
           case _FLT :
              from = (char *)&(v._as.flt);
              size = sizeof(float);
              break;
           case _DBL :
              from = (char *)&(v._as.dbl);
              size = sizeof(double);
              break;
           case _BYTE :
              from = (char *)&(v._as.byte);
              size = sizeof(char);
              break;
           case _INTERNAL_LABEL :
              from = (char *)&(v._as.label);
              size = sizeof(char *);
              break;
           default:
              panic("byte_array::write::bad value type\n");
           }

           if (endianess_matches_host)
           {
              if (increment == up)
              {
                 for(; size>0; (*to++) = (*from++), size--);
                   ;
              }
              else
              {
                 for(; size>0; (*to--) = (*from++), size--);
                   ;
              }
           }
           else
           {
              from = from + size-1;
              if (increment == up)
              {
                 for(; size>0; (*to++) = (*from--), size--);
                   ;
              }
              else
              {
                 for(; size>0; (*to--) = (*from--), size--);
                   ;
              }
           }
}

void byte_array::read(int addr, VALUE_TYPE mode, val& v)
{
           if ( (addr + sizeof(double)) > ub() )
              grow( addr + sizeof(double) );

           v._as.dbl = 0.0; // assumes sizeof(double) is largest
           v.val_type = mode;

           char *from = ((char *)data)+addr;
           char *to;
           int size;

           switch(v.val_type)
           {
           case _INT :
              to = (char *)&(v._as.integer);
              size = sizeof(int);
              break;
           case _SHORT :
              to = (char *)&(v._as.short_int);
              size = sizeof(short);
              break;
           case _FLT :
              to = (char *)&(v._as.flt);
              size = sizeof(float);
              break;
           case _DBL :
              to = (char *)&(v._as.dbl);
              size = sizeof(double);
              break;
           case _ANY :
              to = (char *)&(v._as.dbl);
              size = sizeof(double);
              break;
           case _BYTE :
              to = (char *)&(v._as.byte);
              size = sizeof(char);
              break;
           case _INTERNAL_LABEL :
              to = (char *)&(v._as.label);
              size = sizeof(char*);
              break;
           default:
              panic("byte_array::write::bad value type\n");
           }

           if (endianess_matches_host)
           {
              if (increment == up)
              {
                 for(; size>0; (*to++) = (*from++), size--);
                   ;
              }
              else
              {
                 for(; size>0; (*to++) = (*from--), size--);
                   ;
              }
           }
           else
           {
              to = to + size-1;
              if (increment == up)
              {
                 for(; size>0; (*to--) = (*from++), size--);
                   ;
              }
              else
              {
                 for(; size>0; (*to--) = (*from--), size--);
                   ;
              }
           }
}

int byte_array::touched(int addr, VALUE_TYPE mode)
{
        int i = addr;
        switch(mode) {
        case _DBL:
           if (already_initialized.contains( i += increment )) return 1;
           if (already_initialized.contains( i += increment )) return 1;
           if (already_initialized.contains( i += increment )) return 1;
           if (already_initialized.contains( i += increment )) return 1;
        case _INT:
        case _INTERNAL_LABEL:
        case _FLT:
           if (already_initialized.contains( i += increment )) return 1;
           if (already_initialized.contains( i += increment )) return 1;
        case _SHORT:
           if (already_initialized.contains( i += increment )) return 1;
        case _BYTE:
           if (already_initialized.contains( i += increment )) return 1;
           return 0;
        default:
           panic("byte_array::touched: bad val_type\n");
        }

		//NOS
		ASSERT_TRACE(0,"Forced EXIT!");
		return 0;
}

void byte_array::touch(int addr, VALUE_TYPE mode)
{
        int i = addr;
        switch(mode) {
        case _DBL:
           already_initialized += i += increment;
           already_initialized += i += increment;
           already_initialized += i += increment;
           already_initialized += i += increment;
        case _INT:
        case _FLT:
        case _INTERNAL_LABEL:
           already_initialized += i += increment;
           already_initialized += i += increment;
        case _SHORT:
           already_initialized += i += increment;
        case _BYTE:
           already_initialized += i += increment;
           return;
        default:
           panic("byte_array::touch: bad val_type\n");
        }
}

void sim_mem::write_breakpoint() {
        cout << "reached write_breakpoint\n";
}

int sim_mem::legal_addr(int addr) {
           return (addr >= 0) && (addr < size);
}

int sim_mem::write(lhsMem& v)
{
  return write(v._addr,v._value);
}

int sim_mem::write(long addr, val& value)
{
  if ((addr <= watch_addr)
    &&  (watch_addr < addr + 4)) // should use modesize here
  {
     write_breakpoint();
  }
  if (! legal_addr(addr)) {
     panic("memory::write: illegal address (%d) write aborted.\n", addr);
     bad_memory_access = 1;
     return 0;
  }
  if (addr > seg_size)
     stack.write(size-addr, value);
  else
     heap.write(addr, value);
  return write_latency;
}

int sim_mem::read(lhsMem& v)
{
  return read(v._addr,v._value.val_type, v._value);
}

int sim_mem::read(int addr, val& v)
{
  read(addr, v.val_type, v);
  //NOS
  ASSERT_TRACE(0,"Forced EXIT!");
  return 0;
}

int sim_mem::read(int addr, VALUE_TYPE mode, val& v)
{
  if (! legal_addr(addr)) {
    panic("memory::read: illegal address (%d) read aborted.\n", addr);
    bad_memory_access = 1;
    return 0;
   }
   if (addr > seg_size)
     stack.read(size-addr, mode, v);
   else
     heap.read(addr, mode, v);
  return read_latency;
}

void sim_mem::touch(int addr, VALUE_TYPE mode)
{
           if (addr > seg_size)
              stack.touch(size-addr, mode);
           else
              heap.touch(addr, mode);
}

int sim_mem::touched(int addr, VALUE_TYPE mode)
{
           if (addr > seg_size)
              return stack.touched(size-addr, mode);
           else
              return heap.touched(addr, mode);
}

