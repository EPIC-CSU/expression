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
 *file  : Connectivity.cpp                                   *
 *created : Mar 03, 2000                                     *
 *authors : Gaby Grun, Prabhat Mishra                        *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitions of Connectivity class        *
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


extern Connectivity *connect;

Connectivity::Connectivity()
{
  n_connections=0;
  n_modules=0;

  initialize();
}


Connectivity::~Connectivity()
{
}


// Perform the connections
void Connectivity::addConnection(int from, int to)
{
  int c = n_connections++;
  MemoryModules[from]->add_out_connection(c);
  MemoryModules[to]->add_in_connection(c);
  connections_input_module[c]=MemoryModules[to];
  connections_output_module[c]=MemoryModules[from];
}

//returns the input module connected to the connection c
MemoryModule *Connectivity::get_connected_module(int c){
  return connections_input_module[c];
}

void Connectivity::print_connectivity(void){
  int i;
  printf("Connectivity:\n");
  for(i=0;i<n_connections;i++){
    printf("Connection %d: from %s, to %s\n",i,connections_output_module[i]->get_mem_type(),
	   connections_input_module[i]->get_mem_type());
  }
}


void Connectivity::print_statistics(FILE* ptr){
  int i;
  for(i=0;i<n_modules;i++){
    MemoryModules[i]->print_statistics(ptr);
  }
  //print_connectivity();
}

