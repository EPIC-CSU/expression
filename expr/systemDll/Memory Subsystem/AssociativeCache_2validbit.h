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
 *file  : AssociativeCache.h                                 *
 *created : Mar 16, 2000                                     *
 *authors : Prabhat Mishra                                   *
 *last modified :                                            *
 *                                                           *
 *contents:contains definitios of Asscciative Cache class    *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#ifndef _ASSOC_CACHE_H_
#define _ASSOC_CACHE_H_

#include<math.h>
#include <map>
#include "AddrData.h"
#include "Connectivity.h"
#include "Cache.h"
#include "power.h"

extern Connectivity *connect;
extern bool debugSIMULATOR;

//template
//<long _No_Lines_, int _Line_Size_, int _N_Ways_, int _Word_Size_, int _Latency_>
class AssociativeCache : MemoryModule
{
private:
	
	int _No_Lines_;
	int _Line_Size_;
	int _N_Ways_;
	int _Word_Size_;
	int _Latency_ ;

	Cache **_assCache;
	
	void initPowerComponents() {
		
		// Use CACTI to extract the best cache parameters for the given cache
		// specifications.
		
		time_result_type time_result;
		time_parameter_type time_parameters;
		
		time_parameters.cache_size = _N_Ways_ * _Line_Size_ * _No_Lines_;  /* C */
		time_parameters.associativity = _N_Ways_ ; /* A */
		time_parameters.block_size = _Line_Size_ ; /* B */
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
		
		
		_decoder_power = ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,1);
		_wordline_power = ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,1);
		_bitline_power = ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,1);
		_senseamp_power = ndwl*ndbl*senseamp_power(colsb);
		_tagarray_power = ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,1));
		
		if(debugSIMULATOR) {
			// printf("Cache power stats\n");
			// add print functions for the power components
		}
		
	}
	
public:
	Vector<int> _accessPattern; // Maintains the list of access pattern based on LRU/FIFO
	
	AssociativeCache(int No_Lines, int Line_Size, int N_Ways, int Word_Size, int Latency) 
	{
		_No_Lines_ = No_Lines;
		_Line_Size_ = Line_Size;
		_N_Ways_ = N_Ways;
		_Word_Size_ = Word_Size;
		_Latency_ = Latency;
			
		_assCache = (Cache **)malloc(_N_Ways_ * sizeof(Cache *));
		for (int i=0; i<_N_Ways_; i++) {			
			Cache *cacheInst =	new Cache(_No_Lines_, _Line_Size_, _Word_Size_);
			_assCache[i] = cacheInst;
		}

		_latency = _Latency_;
		initPowerComponents();
		
		// Initialize the power components. These variables get updated for every 
		// cache access depending on read/write.
		
		/* decoder_power = 0;
		wordline_power = 0;
		bitline_power = 0;
		sensamp_power = 0;
		tagarray_power = 0; */
		
	}
	
	~AssociativeCache() {

		for (int i=0; i<_N_Ways_; i++) 
			delete _assCache[i];
		free(_assCache);
	}

	AssociativeCache& AssociativeCache::operator=(const AssociativeCache &a)
	{
		return *this;
	}
	
	void print(ostream & out)
	{
		int i;
		for (i=0; i < _N_Ways_; i++)
		{
			out << "Printing Data Cahe " << i << endl;
			//  _assCache[i].print(out);
		}		  
	}
	
	void init()
	{
		int ci;
		for (ci = 0; ci < _N_Ways_; ci++)
		{
			_assCache[ci]->init();
		}
	}
		  
		  
		  
	int read(long address, Val & value, int opcode, int & latency)
	{	  
		int readCache = 0;
		int ci;
		cache_accesses++;
		
		for (ci = 0; ci < _N_Ways_; ci++)
		{
			readCache = _assCache[ci]->read(address, value, opcode, latency);
			
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
				
				latency = _latency;
				
				break;
			}
		}
		
		
		if (readCache == -1)
		{
			// MISS, initiate REFILL
			read_misses++;
			
			MemoryModule *next_mod;
			next_mod=connect->get_connected_module(out_connect);
			
			if (next_mod == NULL)
			{
				// This Cache is already in the last level of memory hierarchy
				cout << "Error: memory fault in AssociativeCache::read()" << endl;
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
			
			latency = _latency;
			
			// REPLACE the line from the LRU/FIFO cache
			int repLatency = 0;
			_assCache[cacheWay]->replace(next_mod, address, opcode, repLatency);
			
			// REFILL the line
			int refLatency = 0;
			_assCache[cacheWay]->refill(next_mod, address, opcode, refLatency);
			latency += refLatency;
			
			// Update access pattern for LRU/FIFO
			_accessPattern.push_back(cacheWay);
			
			// Read the data now.
			int junkLatency;
			_assCache[cacheWay]->read(address, value, opcode, junkLatency);
		}

		return 1;
	}
	
	int write(long address, Val & value, int opcode, int & latency)
	{
		int writeCache = 0;
		int ci;
		cache_accesses++;
		
		for (ci = 0; ci < _N_Ways_; ci++)
		{
			writeCache = _assCache[ci]->write(address, value, opcode, latency);
			
			if (writeCache == 1)
			{
				// Write is successful.
				latency = _latency;
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
			//added by Sudeep 03/30/03
			/*else if (writeCache == -2)
			{
				// Write is successful - writing to an empty slot in cache
				latency = _latency;
				write_hits++;

				MemoryModule *next_mod;
				next_mod=connect->get_connected_module(out_connect);
			
				if (next_mod == NULL)
				{
					// This Cache is already in the last level of memory hierarchy
					cout << "Error: memory fault in AssociativeCache::write()" << endl;
					assert(0);
				}

				// get latest cache line from next memory module
				int refLatency = 0;
				_assCache[ci]->refill(next_mod, address, opcode, refLatency);

				// Write the data now.
				int junkLatency;
				_assCache[ci]->write(address, value, opcode, junkLatency);

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
			}*/

		}
		
		
		if (writeCache == -1)
		{
			// All the caches have valid data. REPLACE the Least Recently Used (LRU) 
			// or FIFO block and then write the data there.  
			
			write_misses++;
			
			MemoryModule *next_mod;
			next_mod=connect->get_connected_module(out_connect);
			
			if (next_mod == NULL)
			{
				// This Cache is already in the last level of memory hierarchy
				cout << "Error: memory fault in AssociativeCache::write()" << endl;
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
			_assCache[cacheWay]->replace(next_mod, address, opcode, repLatency);
			//added by Sudeep 03/30/03
			//_assCache[cacheWay]->refill(next_mod, address, opcode, repLatency);			
			latency = _latency;


			// Update access pattern for LRU/FIFO
			_accessPattern.push_back(cacheWay);
			
			// Write the data now.
			int junkLatency;
			_assCache[cacheWay]->write(address, value, opcode, junkLatency);
		}
					return 1;
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
		
		fprintf(statPtr, "\nASSOCIATIVE DCACHE: ");
		fprintf(statPtr, "Cache Accesses: %lu (%3.2f) \n \t read hits: %lu, read misses: %lu (%3.2f) \n\t write hits: %lu, write misses: %lu (%3.2f) \n",cache_accesses, hit_ratio, read_hits, read_misses, rhit_ratio, write_hits, write_misses, whit_ratio);
		fprintf(statPtr, "\t Energy Dissipation: %5.3f uJ\n", (cache_accesses*Period) * (_decoder_power + _wordline_power + _bitline_power + _senseamp_power + _tagarray_power)*10e6);
		
		cout<<"\nASSOCIATIVE DCACHE: Cache Accesses: "<<cache_accesses<<" ("<<hit_ratio<<") \n \t read hits: "<<read_hits<<", read misses: "<<read_misses<<" ("<<rhit_ratio<<") \n\t write hits: "<<write_hits<<", write misses: "<<write_misses<<" ("<<whit_ratio<<") \n"; 
		cout<<"\t Energy Dissipation: "<<(cache_accesses*Period) * (_decoder_power + _wordline_power + _bitline_power + _senseamp_power + _tagarray_power)*10e6<<" uJ\n";
	}
};

/*
template
<long _No_Lines_, int _Line_Size_, int _N_Ways_, int _Word_Size_, int _Latency_>
class IAssociativeCache : MemoryModule
{
  private:
      Cache<_No_Lines_, _Line_Size_, _Word_Size_> _assCache[_N_Ways_];
	  
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
		  
		  _decoder_power = ndwl*ndbl*array_decoder_power(rowsb,colsb,predeclength,1,1,1);
		  _wordline_power = ndwl*ndbl*array_wordline_power(rowsb,colsb,wordlinelength,1,1,1);
		  _bitline_power = ndwl*ndbl*array_bitline_power(rowsb,colsb,bitlinelength,1,1,1);
		  _senseamp_power = ndwl*ndbl*senseamp_power(colsb);
		  _tagarray_power = ntwl*ntbl*(simple_array_power(trowsb,tcolsb,1,1,1));
	  
		  if(debugSIMULATOR) {
			  // printf("Cache power stats\n");
			  // add print functions for the power components
		  }
	

	  }


  public:
	  Vector<int> _accessPattern; // Maintains the list of access pattern based on LRU/FIFO
	  
	  IAssociativeCache() 
	  {
		  _latency = _Latency_;
		  initPowerComponents();
	  }
	  
	  ~IAssociativeCache() {}
	  
	  void print(ostream & out)
	  {
		  int i;
		  for (i=0; i < _N_Ways_; i++)
		  {
			  out << "Printing Data Cahe " << i << endl;
			  // _assCache[i].print(out);
		  }		  
	  }
	  
	  void init()
	  {
		  int ci;
		  for (ci = 0; ci < _N_Ways_; ci++)
		  {
			  _assCache[ci].init();
		  }
	  }

	  void read(long address, Val & value, int opcode, int & latency)
	  {		  
		  cache_accesses++;
		  int readCache = 0;
		  int ci;
		  
		  for (ci = 0; ci < _N_Ways_; ci++)
		  {
			  readCache = _assCache[ci].read(address, value, opcode, latency);
			  
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
			  _assCache[cacheWay].replace(next_mod, address, opcode, repLatency);
			  
			  
			  // REFILL the line
			  int refLatency = 0;
			  _assCache[cacheWay].refill(next_mod, address, opcode, refLatency);
			  latency += refLatency;
			  
			  // Update access pattern for LRU/FIFO
			  _accessPattern.push_back(cacheWay);
			  
			  // Read the data now.
			  int junkLatency;
			  _assCache[cacheWay].read(address, value, opcode, junkLatency);
		  }
	  }
	  
	  void write(long address, Val & value, int opcode, int & latency)
	  {
		  int writeCache = 0;
		  int ci;
		  cache_accesses++;
		  
		  for (ci = 0; ci < _N_Ways_; ci++)
		  {
			  writeCache = _assCache[ci].write(address, value, opcode, latency);
			  
			  
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
			  _assCache[cacheWay].replace(next_mod, address, opcode, repLatency);
			  			  
			  
			  // Update access pattern for LRU/FIFO
			  _accessPattern.push_back(cacheWay);
			  
			  // Write the data now.
			  int junkLatency;
			  _assCache[cacheWay].write(address, value, opcode, junkLatency);
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
		  
		  fprintf(statPtr, "\nASSOCIATIVE ICACHE: ");
		  fprintf(statPtr, "Cache Accesses: %lu (%3.2f) \n \t read hits: %lu, read misses: %lu (%3.2f) \n",cache_accesses, hit_ratio, read_hits, read_misses, rhit_ratio);
		  fprintf(statPtr, "\t Energy Dissipation: %5.3f uJ\n", (cache_accesses * Period) * (_decoder_power + _wordline_power + _bitline_power + _senseamp_power + _tagarray_power)*10e6);
	  }
};
*/
#endif
