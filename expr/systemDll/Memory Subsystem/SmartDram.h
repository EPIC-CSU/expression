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
 *file  : Dram.h                                             *
 *created : Mar 03, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Dram class                 *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/


#ifndef _SMARTDRAM_H_
#define _SMARTDRAM_H_

#include<stdio.h>

class Val;

template <int _NR_Latency_,int _PM_Latency_,int _Page_Size>
class SmartDram : MemoryModule
{
	long _last_read;
	int _pm_latency;
	int _page_size;
  protected :
	  map<long, Val> _Dram; // <address, data>
 
	  int same_page(long addr1,long addr2){
		  int r1,r2;
		  if(addr1==-1 || addr2==-1) return 0;
		  r1=addr1/_page_size;
		  r2=addr2/_page_size;
		  if(r1==r2) return 1;
		  return 0;
	  }
  public :
	  SmartDram() 
	  {
		  _latency = _NR_Latency_;//normal read latency
		  _pm_latency = _PM_Latency_; //page mode read latency
		  _page_size = _Page_Size; //the page size

		  //stores the last read to the memory module, for page mode computation
		  _last_read=-1;
	  }

	  ~SmartDram() {}
	 
	  void print(ostream & out)
	  {
		  out << "Printing Dram" << endl;
		  _Dram.print(out);
		  out << "End printing Dram" << endl;
		  
	  }

	  void print_statistics()
	  {
		  printf("SmartDRAM: ");
		  printf("\t%lu loads\n", cache_accesses);
	  }
	  
	  void read(long addr, Val & value, int opcode, int & latency)
	  {
		  cache_accesses++;
		  if(same_page(_last_read,addr)){
			  //page mode
			  latency = _pm_latency;
		  }
		  else{
			  //normal read (need row decode + precharge)
			  latency = _latency;
		  }
		  _last_read=addr;
		  value = _Dram[addr];	 
	  }
	  
	  void write(long addr, Val & value, int opcode, int & latency)
	  {
		  latency = _latency;
		  _Dram[addr] = value;
	  }
};

#endif
