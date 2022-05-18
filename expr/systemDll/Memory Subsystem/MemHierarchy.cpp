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
 *file  : MemHierarchy.h                                     *
 *created : Mar 03, 2000                                     *
 *authors : Gaby Grun, Prabhat Mishra                        *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of memory hierarchy           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#include <stdio.h>

#include "MemoryModule.h"
#include "DirectCache.h"
#include "AssociativeCache.h"
#include "Dram.h"
#include "Sram.h"
#include "Connectivity.h"

class Val;

/*prototypes*/
extern Connectivity *connect;
void open_file(void);
void close_file(void);
void print_mem_statistics(FILE *ptr);



void set_sim_cache(int sc);
void init_mem_hier();
void close_mem_hier(void);
void set_collect_trace(int ct);

static int read_latency=0, write_latency=0;
static FILE *f=NULL;
static int sim_cache=1;
static int collect_trace=0;
static double nr_lds=0,nr_sts=0;


void init_mem_hier(){
  /*initializations*/
  if(collect_trace)
    open_file();

  //initialize the Connectivity
  connect=new Connectivity();
}

void close_mem_hier(void){
  /*close files*/
  if(collect_trace)
    close_file();
  printf("\nMEMORY HIERARCHY: \n");

  /*print out the collected information*/
  printf("\t%.0f loads, %.0f stores\n", nr_lds,nr_sts);

  FILE *ptr;
  print_mem_statistics(ptr);
}

void set_sim_cache(int sc){
  sim_cache=sc;
}

void set_collect_trace(int ct){
  collect_trace=ct;
}

void print_mem_statistics(FILE* ptr){
  //printf("\tRead latency: %d, write latnecy: %d\n",read_latency,write_latency);
  connect->print_statistics(ptr);

  cout << endl << "+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+" << endl;
}

void open_file(void){
  f=fopen("loads.dat","w");
  if(f==NULL){
    printf("Could not open file loads.dat\n");
    exit(-1);
  }
}

void close_file(void){
  fclose(f);
}

void read_access(long addr, Val & value, int opcode, int & latency)
{
  static int cnt=0;
  
  nr_lds++;

  if(sim_cache)
    connect->get_module_for_address(addr)->read(addr, value, opcode,latency);

  read_latency += latency;
  if(collect_trace)
    fprintf(f,"%d %u\n",cnt++,addr);
}

void write_access(long addr, Val & value, int opcode, int & latency)
{
  nr_sts++;

  if(sim_cache)
    connect->get_module_for_address(addr)->write(addr, value, opcode,latency);

  write_latency += latency;
}
