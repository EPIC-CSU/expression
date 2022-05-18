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
 *file  : DirectCache.h                                      *
 *created : Mar 16, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Direct Cache class         *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _DIRECT_CACHE_
#define _DIRECT_CACHE_

#include <map>
#include "AddrData.h"
#include "Cache.h"
#include "MemDefines.h"
#include "power.h"
 
extern Connectivity *connect;
/*
template
<long _No_Lines_, int _Line_Size_, int _Word_Size_, int _Latency_>
class DirectCache : MemoryModule
{
  private:
      Cache<_No_Lines_, _Line_Size_, _Word_Size_> _directCache;

	  void initPowerComponents() {

 		  // Use CACTI to extract the best cache parameters for the given cache
		  // specifications.
		  
		  time_result_type time_result;
		  time_parameter_type time_parameters;

		  time_parameters.cache_size = _Line_Size_ * _No_Lines_;  // C 
		  time_parameters.associativity = 1 ; // A 
		  time_parameters.block_size = _Line_Size_ ; // B 
		  time_parameters.number_of_sets = _No_Lines_ ; // C/(B*A)
		  
		  calculate_time(&time_result, &time_parameters);

		  if (debugSIMULATOR)
			output_data(&time_result, &time_parameters);
		  
		  int ndwl=time_result.best_Ndwl;
		  int ndbl=time_result.best_Ndbl;
		  int nspd=time_result.best_Nspd;
		  int ntwl=time_result.best_Ntwl;
		  int ntbl=time_result.best_Ntbl;
		  int ntspd=time_result.best_Ntspd;
		  
		  int c = time_parameters.cache_size;
		  int b = time_parameters.block_size;
		  int a = time_parameters.associativity;
		  
		  int rowsb = c/(b*a*ndbl*nspd);
		  int colsb = 8*b*a*nspd/ndwl;
		  
		  // Virtual Address, va_size = 32
		  int va_size = 32;
		  int tagsize = va_size - ((int)logtwo(_No_Lines_) + (int)logtwo(_Line_Size_));
		  int trowsb = c/(b*a*ntbl*ntspd);
		  int tcolsb = a * (tagsize + 1 + 6) * ntspd/ntwl;
		  
		  if(debugSIMULATOR) {
			  printf("%d KB %d-way cache (%d-byte block size):\n",c,a,b);
			  printf("ndwl == %d, ndbl == %d, nspd == %d\n",ndwl,ndbl,nspd);
			  printf("%d sets of %d rows x %d cols\n",ndwl*ndbl,rowsb,colsb);
			  printf("tagsize == %d\n",tagsize);
		  }
		  
		  double predeclength = rowsb * (RegCellHeight + WordlineSpacing);
		  double wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
		  double bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);
		  
		  if(debugSIMULATOR) {
			  // printf("Cache power stats\n");
			  // Print the power stats here...
		  }


		  _decoder_power = ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,1);
		  _wordline_power = ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,1);
		  _bitline_power = ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,1);
		  _senseamp_power = ndwl*ndbl*senseamp_power(colsb);
		  _tagarray_power = ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,1));

	  }

 
  public :
 
	  DirectCache() 
	  {
		  _latency = _Latency_;
		  initPowerComponents();
	  }
	  
	  ~DirectCache() {}
 
	  void print(ostream & out)
	  {
//		  _directCache.print(out);		   
	  }
	  
	  void init()
	  {
		  
		  _directCache.init();
		  
	  }

	  void read(long address, Val & value, int opcode, int & latency)
	  {
		  cache_accesses++;

		  int readCache = _directCache.read(address, value, opcode, latency);
		  
		  if (readCache == 1)
		  {
			  // Read is successful
			  read_hits++;
		  }
		  
		  latency = _latency;

		  if (readCache == -1)
		  {
			  // MISS, initiate REFILL
			  read_misses++;

			  MemoryModule *next_mod;
			  next_mod=connect->get_connected_module(out_connect);

			  if (next_mod == NULL)
			  {
				  // This Cache is already in the last level of memory hierarchy
				  cout << "Error: memory fault in DirectCache::read()" << endl;
				  assert(0);
			  }

			  // REPLACE the line 
			  int repLatency = 0;
			  _directCache.replace(next_mod, address, opcode, repLatency);
			  
			  // REFILL the line
			  int refLatency = 0;
			  _directCache.refill(next_mod, address, opcode, refLatency);
			  latency += refLatency;
			  
			  // Read the data now.
			  int junkLatency;
			  _directCache.read(address, value, opcode, junkLatency);
		  }
	  }

	  void write(long address, Val & value, int opcode, int & latency)
	  {
		  cache_accesses++;

		  int writeCache = _directCache.write(address, value, opcode, latency);
		  
		  if (writeCache == 1)
		  {
			  // Write is successful.
			  write_hits++;
		  }
		  
		  latency = _latency;

		  if (writeCache == -1)
		  {
			  write_misses++;

			  MemoryModule *next_mod;
			  next_mod=connect->get_connected_module(out_connect);

			  if (next_mod == NULL)
			  {
				  // This Cache is already in the last level of memory hierarchy
				  cout << "Error: memory fault in DirectCache::write()" << endl;
				  assert(0);
			  }

			  // REPLACE the line
			  int repLatency = 0;
			  _directCache.replace(next_mod, address, opcode, repLatency);
 
			  // Write the data now.
			  int junkLatency;
			  _directCache.write(address, value, opcode, junkLatency);
		  }
	  }

	  void print_statistics(FILE *statPtr)
	  {
		  float rhit_ratio, whit_ratio, hit_ratio;
		  		  
		  
		  if (cache_accesses == 0)
		  {
			  rhit_ratio=0.0;
			  whit_ratio=0.0;
			  hit_ratio=0.0;
		  }
		  else
		  {
			  hit_ratio=float(read_hits + write_hits)/float(cache_accesses);

			  if ((read_hits + read_misses) == 0)
				  rhit_ratio = 0.0;
			  else
				  rhit_ratio = float(read_hits)/float(read_hits + read_misses);

			  if ((write_hits + write_misses) == 0)
				  whit_ratio = 0.0;
			  else
				  whit_ratio = float(write_hits)/float(write_hits + write_misses);
		  }
		  
		  fprintf(statPtr, "\nDIRECT CACHE: ");
		  fprintf(statPtr, "Cache Accesses: %lu (%3.2f) \n \t read hits: %lu, read misses: %lu (%3.2f) \n\t write hits: %lu, write misses: %lu (%3.2f) \n",cache_accesses, hit_ratio, read_hits, read_misses, rhit_ratio, write_hits, write_misses, whit_ratio);
		  fprintf(statPtr, "\t Energy Dissipation: %5.3f uJ\n", (cache_accesses*Period) * (_decoder_power + _wordline_power + _bitline_power + _senseamp_power + _tagarray_power)*10e6);
	  }
};
*/
#endif
