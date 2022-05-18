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
 *file  : StreamBuffer.h                                 *
 *created : Mar 16, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Asscciative Cache class    *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _STREAM_BUFFER_
#define _STREAM_BUFFER_

#include<math.h>
#include <map>
#include "AddrData.h"
#include "Connectivity.h"
#include "Cache.h"

#define MISS_LIST_SIZE 16    // The number of entries in the miss list
#define MISS_COMPARE_SIZE 4   // The number of entries should have the specified distance to
                             // qualify for detected Sequence
#define MISS_DISTANCE 16     // The distance between miss addresses

extern int memJunk;
extern bool imemJunk;
extern Connectivity *connect;

//template
//<long _No_Lines_, int _Line_Size_, int _N_Ways_, int _Word_Size_, int _Latency_>
class StreamBuffer : MemoryModule
{
  private:
	  int _No_Lines_, _Line_Size_, _N_Ways_, _Word_Size_, _Latency_;
      //Cache<_No_Lines_, _Line_Size_, _Word_Size_> _streamBuffer[_N_Ways_];
	  Cache **_streamBuffer;
	  
	  	   void initPowerComponents() {
		  
		  // Use CACTI to extract the best cache parameters for the given cache
		  // specifications.
		  
		  time_result_type time_result;
		  time_parameter_type time_parameters;

		  time_parameters.cache_size = _N_Ways_ * _Line_Size_ * _No_Lines_;  // C 
		  time_parameters.associativity = _N_Ways_ ; // A 
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
		  
		  
		  double predeclength = rowsb * (RegCellHeight + WordlineSpacing);
		  double wordlinelength = colsb *  (RegCellWidth + BitlineSpacing);
		  double bitlinelength = rowsb * (RegCellHeight + WordlineSpacing);
		  
		  _decoder_power = ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,1);
		  _wordline_power = ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,1);
		  _bitline_power = ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,1);
		  _senseamp_power = ndwl*ndbl*senseamp_power(colsb);
		  // accounting the power due to missAddresslist as a tag array
		  _tagarray_power = ntwl*ntbl*(simple_array_power(MISS_LIST_SIZE,28,1,1,1));

		  if(debugSIMULATOR) {
			  // printf("Cache power stats\n");
			  // add print functions for the power components
		  }

	  }

  public:
	  long MissAddressList[MISS_LIST_SIZE];
	  int missIndex;
	  long addressSequence[4];
	  int busyBits[4];
	  
	  int SequenceDetected(long address)
	  {
		  int detected = 0;

		  // Check for first "MISS_COMPARE_SIZE" entries to see whether all of them
		  // have "MISS_DISTANCE" distance
		  if (missIndex > MISS_COMPARE_SIZE) 		  
		  {	
			  detected = 1;

			  long currAddress = address;

			  for (int i=missIndex - 1; missIndex - i <= MISS_COMPARE_SIZE ; i = i - 1)
			  {
				  int tempdiff = currAddress - MissAddressList[i - 1];
				  
				  if ((tempdiff == MISS_DISTANCE) || (tempdiff == -MISS_DISTANCE))
				  {
				  }
				  else
				  {
					  detected = 0;
					  break;
				  }	
				  currAddress = MissAddressList[i - 1];
			  }
		  }

		  if ((detected == 1)) 
		  {
			long addrSize = _Line_Size_ * _Word_Size_;

			addressSequence[0] = address + addrSize;
			addressSequence[1] = address + 2 * addrSize;
			addressSequence[2] = address + 3 * addrSize;
			addressSequence[3] = address + 4 * addrSize;

			if (memJunk || imemJunk)
				printf("Current Address: %d Next addresses: %d %d %d\n", address, addressSequence[0], addressSequence[1], addressSequence[2], addressSequence[3]);

			for (int i =0; i < MISS_LIST_SIZE; i++)
			  MissAddressList[i] = -1;

			missIndex = 0;
		  }

		  return detected;
	  }

	  StreamBuffer(long No_Lines, int Line_Size, int N_Ways, int Word_Size, int Latency) 
	  {
		  _No_Lines_ = No_Lines;
		  _Line_Size_ = Line_Size;
		  _N_Ways_ = N_Ways;
		  _Word_Size_ = Word_Size;
		  _Latency_ = Latency;

		  _streamBuffer = (Cache **)malloc(_N_Ways_ * sizeof(Cache *));
		  for (int i=0; i<_N_Ways_; i++) {			
		   	 Cache *cacheInst =	new Cache(_No_Lines_, _Line_Size_, _Word_Size_);
			 _streamBuffer[i] = cacheInst;
		  }

		  missIndex = 0;
		  _latency = _Latency_;

		  busyBits[0] = -1;
		  busyBits[1] = -1;
 

		  for (i =0; i < MISS_LIST_SIZE; i++)
			  MissAddressList[i] = -1;

		  initPowerComponents();
	  }

	  Vector<int> _accessPattern; // Maintains the list of access pattern based on LRU/FIFO
 
	  ~StreamBuffer() {}
 
	  void print(ostream & out)
	  {
		  int i;
		  for (i=0; i < _N_Ways_; i++)
		  {
			  out << "Printing Data Cahe " << i << endl;
			//  _streamBuffer[i].print(out);
		  }		  
	  }
	  

	  void read(long address, Val & value, int opcode, int & latency)
	  {		  
		  cache_accesses++;
		  int readCache = 0;
		  int ci;
		  
		  for (ci = 0; ci < _N_Ways_; ci++)
		  {
			  readCache = _streamBuffer[ci]->read(address, value, opcode, latency);
			  
			  if (readCache == 1)
			  {
				  // Read is successful.
				  read_hits++;
				  
				  // Perform processing for LRU/FIFO algorithm
				  if (REPLACEMENT_ALGORITHM == LRU)
				  {
					  // Reorder the access pattern.
					  vector<int>::iterator i1, i2;
					  i1 = _accessPattern.begin();
					  i2 = _accessPattern.end();
					  
					  while(i1 != i2)
					  {
						  if ((*i1) == ci)
						  {
							  _accessPattern.erase(i1);
							  
							  break;
						  }
						  i1++;
					  }
					  _accessPattern.push_back(ci);
					  
				  }
				  else
				  {
					  // FIFO does not need any ordering
				  }
				  
				  break;
			  }
		  }
		  
		  latency = _latency;
		  
		  if (readCache == -1)
		  {
			  MissAddressList[missIndex] = address;
			  missIndex = (missIndex + 1) % MISS_LIST_SIZE;

			  // MISS, initiate REFILL
			  read_misses++;
			  
			  MemoryModule *next_mod;
			  next_mod=connect->get_connected_module(out_connect);
			  
			  if (next_mod == NULL)
			  {
				  // This Cache is already in the last level of memory hierarchy
				  cout << "Error: memory fault in IAssociativeCache::read()" << endl;
				  assert(0);
			  }
			  
			  // Determine the LRU/FIFO cache.
			  int cacheWay = 0;
			  
			  int size = _accessPattern.size();
			  
			  if (size == _N_Ways_)
			  {
				  // All the sets are used find the LRU/FIFO one
				  cacheWay = _accessPattern.front();
				  _accessPattern.erase(_accessPattern.begin());
			  }
			  else
			  {
				  cacheWay = size;
			  }
			  
			  // REPLACE the line from the LRU/FIFO cache
			  int repLatency = 0;
			  _streamBuffer[cacheWay]->replace(next_mod, address, opcode, repLatency);
			  
			  
			  // REFILL the line
			  int refLatency = 0;
			  _streamBuffer[cacheWay]->refill(next_mod, address, opcode, refLatency);
			  latency += refLatency;
			  
			  // Update access pattern for LRU/FIFO
			  _accessPattern.push_back(cacheWay);
			  
			  // Read the data now.
			  int junkLatency;
			  _streamBuffer[cacheWay]->read(address, value, opcode, junkLatency);
			  
			  if (SequenceDetected(address))
			  {
				  _streamBuffer[cacheWay]->refill(next_mod, addressSequence[0], opcode, refLatency);
				  _streamBuffer[cacheWay]->refill(next_mod, addressSequence[1], opcode, refLatency);
				  _streamBuffer[cacheWay]->refill(next_mod, addressSequence[2], opcode, refLatency);
				  _streamBuffer[cacheWay]->refill(next_mod, addressSequence[3], opcode, refLatency);
			  }
		  }
		
	  }
	  
	  void write(long address, Val & value, int opcode, int & latency)
	  {
		  int writeCache = 0;
		  int ci;
		  cache_accesses++;
		  
		  for (ci = 0; ci < _N_Ways_; ci++)
		  {
			  writeCache = _streamBuffer[ci]->write(address, value, opcode, latency);
			  
			  
			  if (writeCache == 1)
			  {
				  // Write is successful.
				  write_hits++;
				  
				  // Perform processing for LRU/FIFO algorithm
				  if (REPLACEMENT_ALGORITHM == LRU)
				  {
					  // Reorder the access pattern.
					  vector<int>::iterator i1, i2;
					  i1 = _accessPattern.begin();
					  i2 = _accessPattern.end();
					  
					  while(i1 != i2)
					  {
						  if ((*i1) == ci)
						  {
							  _accessPattern.erase(i1);
							  
							  break;
						  }
						  i1++;
					  }
					  _accessPattern.push_back(ci);
					  
				  }
				  else
				  {
					  // FIFO does not need any ordering
				  }
				  
				  break;
			  }
		  }
		  
		  latency = _latency;
		  
		  if (writeCache == -1)
		  {
			  write_misses++;
			  // All the caches have valid data. REPLACE the Least Recently Used (LRU) 
			  // or FIFO block and then write the data there.  
			  
			  MemoryModule *next_mod;
			  next_mod=connect->get_connected_module(out_connect);
			  
			  if (next_mod == NULL)
			  {
				  // This Cache is already in the last level of memory hierarchy
				  cout << "Error: memory fault in IAssociativeCache::write()" << endl;
				  assert(0);
			  }
			  
			  // Determine the LRU/FIFO cache.
			  int cacheWay = 0;
			  
			  int size = _accessPattern.size();
			  
			  if (size == _N_Ways_)
			  {
				  // All the sets are used find the LRU/FIFO one
				  cacheWay = _accessPattern.front();
				  _accessPattern.erase(_accessPattern.begin());
			  }
			  else
			  {
				  cacheWay = size;
			  }
			  
			  // REPLACE the line from the LRU/FIFO cache
			  int repLatency = 0;
			  _streamBuffer[cacheWay]->replace(next_mod, address, opcode, repLatency);
			  			  
			  
			  // Update access pattern for LRU/FIFO
			  _accessPattern.push_back(cacheWay);
			  
			  // Write the data now.
			  int junkLatency;
			  _streamBuffer[cacheWay]->write(address, value, opcode, junkLatency);
		  }
	  }

	  void init()
	  {
		  int ci;
		  for (ci = 0; ci < _N_Ways_; ci++)
		  {
			  _streamBuffer[ci]->init();
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

		  printf("\tCache hit ratio: %.2f\n",hit_ratio);
		  fprintf(statPtr, "\n STREAM BUFFER: ");
		  fprintf(statPtr, "Cache Accesses: %lu (%3.2f) \n \t read hits: %lu, read misses: %lu (%3.2f) \n\t write hits: %lu, write misses: %lu (%3.2f) \n",cache_accesses, hit_ratio, read_hits, read_misses, rhit_ratio, write_hits, write_misses, whit_ratio);
		  fprintf(statPtr, "\t Energy Dissipation: %5.3f uJ\n", (cache_accesses * Period) * (_decoder_power + _wordline_power + _bitline_power + _senseamp_power + _tagarray_power)*10e6);

	  }
};

#endif
