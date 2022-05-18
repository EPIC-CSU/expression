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
 *file  : Sram.h                                             *
 *created : Mar 03, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Sram class                 *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/


#ifndef _SRAM_H_
#define _SRAM_H_

#include<stdio.h>
#include "power.h"

class Val;

// template <int _Latency_>
class Sram : MemoryModule
{
  protected :
 
	map<long, Val> _Sram; // <address, data>

	void initPowerComponents() {
		
		// SRAM is considered as a register file for power estimation purposes
		// and the corresponding power models from Wattch are used.
		
		// assuming the SRAM size is 512x32 (16k bits)
		// Change the parameters below (rows & cols) accordingly.
		  int rowsb = 128;
		  int colsb = 32;
		  
		  // Virtual Address, va_size = 32
		  int va_size = 32;
		  
		  double predeclength = rowsb * (RegCellHeight + WordlineSpacing);
		  double wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
		  double bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);

		  _decoder_power = array_decoder_power(rowsb,colsb,predeclength,1,1,0);
		  _wordline_power = array_wordline_power(rowsb,colsb,wordlinelength,1,1,0);
		  _bitline_power = array_bitline_power(rowsb,colsb,bitlinelength,1,1,0);
		  _senseamp_power = 0; _tagarray_power = 0;

		  if(debugSIMULATOR) {
			  // printf("Cache power stats\n");
			  // add print functions for the power components
		  }

	  }

  public :
	  Sram(int latency)
	  {

		  _latency = latency;
		  initPowerComponents();
	  }
 
	  ~Sram() {}
	 
	  void print(ostream & out)
	  {
		  out << "Printing Sram" << endl;
		  //_Sram.print(out);
		  out << "End printing Sram" << endl;
		  
	  }

	  void init()
	  {
		  _Sram.clear();
	  }

	  void print_statistics(FILE *statPtr)
	  {
		  fprintf(statPtr, "\nSRAM: \tloads: %lu, stores: %lu\n",read_hits, write_hits);
		  fprintf(statPtr, "\tEnergy Dissipation: %5.3f uJ\n", ((read_hits+write_hits) * Period) * (_decoder_power + _wordline_power + _bitline_power)*10e6);
		  cout << "\nSRAM: \tloads: "<<read_hits<<", stores: "<<write_hits<<endl;
		  cout <<" \tEnergy Dissipation: "<<((read_hits+write_hits) * Period) * (_decoder_power + _wordline_power + _bitline_power)*10e6<<" uJ"<<endl;
	  }
	  
	  void read(long addr, Val & value, int opcode, int & latency)
	  {
		  read_hits++;
		  latency = _latency;
		  value = _Sram[addr];
	  }
	  
	  void write(long addr, Val & value, int opcode, int & latency)
	  {
		  write_hits++;
		  latency = _latency;
		  _Sram[addr] = value;
	  }
};

#endif
 
