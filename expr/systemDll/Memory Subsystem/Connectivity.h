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
 *file  : Connectivity.h                                     *
 *created : Mar 03, 2000                                     *
 *authors : Gaby Grun, Prabhat Mishra                        *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Connectivity class         *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _CONNECTIVITY_H_
#define _CONNECTIVITY_H_

#include <map>
#include "MemoryModule.h"


class Connectivity
{
 public: 
  map<int,MemoryModule *> MemoryModules;
  int n_connections;
  int n_modules;
  map<int,MemoryModule *> connections_input_module;
  map<int,MemoryModule *> connections_output_module;

 public:
  Connectivity();
  ~Connectivity();
  void addConnection(int from, int to);
  void initialize();
  //returns the input module connected to the connection c
  MemoryModule *get_connected_module(int c);
  MemoryModule* get_module_for_address(long addr);
  void print_statistics(FILE *);
  void print_connectivity(void);
  // friend void initializeMemorySubsystem();
  void initializeMemorySubsystem();
};

#endif
