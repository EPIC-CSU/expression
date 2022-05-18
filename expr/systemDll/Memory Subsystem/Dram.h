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


#ifndef _DRAM_H_
#define _DRAM_H_

#include "MemoryModule.h"
#include "MyString.h"
#include "ArchSTLIncludes.h"
#include <stdio.h>
#include <string.h>
#include "power.h"

class Val;
extern map<MyString, long> GlobalLabels;
extern int __L2_LINE_SIZE__;

//template <int _Latency_>
class Dram : MemoryModule
{

protected :
	map<long, Val> _Dram; // <address, data>
	
public :
	Dram(int latency) 
	{

		_latency = latency;
	}
	
	~Dram() {
		map<long, Val>::iterator p;
		
		for (p = _Dram.begin(); p!=_Dram.end(); p++) {
			if (isStringType((*p).second)) 
				free(getStringValue((*p).second));
		}
		
	}

	void init()
	{
		_Dram.clear();
	}
	
	void print(ostream & out)
	{
		out << "Printing Dram" << endl;

		map<long, Val>::iterator i1;
		
		for (i1= _Dram.begin(); i1 != _Dram.end(); i1++)
		{
			out << "Addr: " << (*i1).first << " ";
			((*i1).second).print(out);
			out << endl;
		}
		out << "End printing Dram" << endl;
	}
	
	void print_statistics(FILE *statPtr)
	{
		fprintf(statPtr, "\nDRAM: ");
		fprintf(statPtr, "\tloads: %lu, \tstores: %lu", read_hits, write_hits);
		fprintf(statPtr, "\n \t Energy Dissipation: ");

		cout<<"\nDRAM: \tloads: "<<read_hits<<", \tstores: "<<write_hits<<"\n \tEnergy Dissipation: ";

		// Total Power Dissipation in DRAM is sum of 
		//	  operating current in burst mode for reads/writes 
		//		+ standby current when there are no accesses to DRAM
		//		+ Refresh currents drawn during the whole program execution. 
		//
		// The followin assumptions are made for power estimation purposes:
		//    - We assume that all the accesses to DRAM are burst mode accesses
		//    - DRAM would be in power down mode when there are no DRAM accesses.
		//    - Since the max currents are given in the DRAM data sheets, an
		//       linear model for current is assumed and the the avg. current is
		//		 assumed to be half (Imax/2) (** dont know the validity of this 
		//		 assumption)

		// Data sheet values: 
		//	   Operating current in burst mode (CAS = 3, 6ns cycle time) Imax = 140mA
		//     Autorefresh currents (to be issued every 15.62u sec) 
		//			in Power down mode, Imax= 1mA
		//			in Active mode, Imax = 45mA
		//	     total refresh commands = (total cycles * processor cycle time)/31.62u
		//	   Standby current(power down mode) = Imax = 2mA

		const double dramVdd = 3.3e-3; // volts
		const double IavgStandby = 1e-3, IavgBurst = 70e-3; // mA
		const double IavgRefreshPowerDownMode = 5e-4, IavgRefreshActiveMode = 22.5e-3; // mA
		const double dramCycleTime = 6e-9; // ns (assuming 166MHz)
		const double procCycleTime = Period;   // ns (600MHz)
		int dramCycles = clock_count * procCycleTime/dramCycleTime;
		

		// standby power
		float standbyPower = dramCycles * IavgStandby * dramVdd;
		
		// power for burst read and write accesses
		// accessPower = total_accesses * Iavg * (4 cycles for first read/write to come out)
		float accessPower = (read_hits+write_hits) * IavgBurst * dramVdd * 4;
		
		// Refresh Power
		// total time in active mode = total_accesses * (CAS latency + L2_line_size*2) dram cycles
		// total time in power down mode = prog execution time - total time in active mode
		int accessCycles = (read_hits+write_hits) * (3+ 2*__L2_LINE_SIZE__);
		// float refreshPower = (IavgRefreshActiveMode * accessCycles * dramVdd + IavgRefreshPowerDownMode * (dramCycles - accessCycles) * dramVdd);
		float refreshPower = IavgRefreshActiveMode * dramVdd * dramCycles;


		fprintf(statPtr, " %5.3f uJ\n", dramCycleTime * (standbyPower + accessPower + refreshPower)*10e6 );
		cout<<" "<<dramCycleTime * (standbyPower + accessPower + refreshPower)*10e6<<" uJ\n";
	}
	
	void read(long addr, Val & value, int opcode, int & latency)
	{
		// Calculate the reduction in transition activities using different
		// encoding techniques. We are using the address only once for the
		// the complete LINE.
/*
		static int BusInvertEncAddr, prevBusInvertEncAddr=0, togglesBI=0;
		static int invBitLines, prevInvBitLines=0;
		static int GrayEncAddr, prevGrayEncAddr=0, togglesGray=0;
		static int MTFencAddr, prevMTFencAddr=0, togglesMTF=0;
		static int MTFincXorEncAddr, prevMTFincXorEncAddr=0, togglesMTFincXor=0;
		static int TRencAddr, prevTRencAddr=0, togglesTR=0;
		static int TRIncXorEncAddr, prevTRIncXorEncAddr=0, togglesTRincXor=0;
		static int IncXorEncAddr, prevIncXorEncAddr=0, togglesIncXor=0;

		if ((read_hits%__L2_LINE_SIZE__) == 0)
		{
			// Compute for BusInvert encoding
			BusInvertEncAddr = busInvert_encode(addr, &invBitLines);
			togglesBI += no_of_ones(BusInvertEncAddr ^ prevBusInvertEncAddr) + 
				                   no_of_ones(invBitLines ^ prevInvBitLines);
			prevBusInvertEncAddr = BusInvertEncAddr;
			prevInvBitLines = invBitLines;

			// Compute Gray encoding
			GrayEncAddr = gray_encode(addr);
			togglesGray += no_of_ones(GrayEncAddr ^ prevGrayEncAddr);
			prevGrayEncAddr = GrayEncAddr;
			
			// Compute MTF encoding
			MTFencAddr = mtf_encode(addr);
			togglesMTF += no_of_ones(MTFencAddr ^ prevMTFencAddr);
			prevMTFencAddr = MTFencAddr;

			// Compute TR encoding
			TRencAddr = tr_encode(addr);
			togglesTR += no_of_ones(TRencAddr ^ prevTRencAddr);
			prevTRencAddr = TRencAddr;

			// Compute TRincXor encoding
			TRincXorEncAddr = trIncxor_encode(addr);
			togglesTRincXor += no_of_ones(TRincXorEncAddr ^ prevTRincXorEncAddr);
			prevTRincXorEncAddr = TRincXorEncAddr;

			// Compute MTFincXor encoding
			MTFincXorEncAddr = mtfIncxor_encode(addr);
			togglesMTFincXor += no_of_ones(MTFincXorEncAddr ^ prevMTFincXorEncAddr);
			prevMTFincXorEncAddr = MTFincXorEncAddr;

			// Compute IncXor encoding
			IncXorEncAddr = incxor_encode(addr);
			togglesIncXor += no_of_ones(IncXorEncAddr ^ prevIncXorEncAddr);
			prevIncXorEncAddr = IncXorEncAddr;
		}
*/
		read_hits++;
		latency = _latency;
		value = _Dram[addr];
	}
	
	void read(long addr, Val & value)
	{
		read_hits++;
		value = _Dram[addr];
	}

	void write(long addr, Val & value, int opcode, int & latency)
	{
		write_hits++;
		latency = _latency;
		_Dram[addr] = value;
	}
	
	
	void initmem( long strtaddr, char *loadtype, char *fname) 
	{
		FILE *fp;
		char typ[10];
		long addr;
		
		Val a;
		
		if ((fp = fopen(fname, "r")) == NULL) {
			printf("File: %s cannot be opened\n",fname);
		}
		else
		{
			
			// The data section is always at the beginning of the input-file.
			if ((!strcmp(loadtype,"data")) || (!strcmp(loadtype,"both"))) 
			{
				char line[80];
				if (!strcmp(fgets(line,80,fp),"Data Section:")) 
				{
					assert("Data Section MISSING at the beginning of the file\n");
				}
				
				while( (!feof(fp)) && (!strcmp(fgets(line,80,fp),"End Data Section")))
				{
					// Data section is assumed to have 3 parts.	
					char *newdata = (char *)malloc(80);
					sscanf(line,"%lu%s%s",&addr,typ,newdata);
					if (!strcmp(typ,"Short")) {
						a.changeValue((short) (atoi(newdata)));
						_Dram[addr] = a;
						free(newdata);
					} else
					if (!strcmp(typ,"Word")) {
						a.changeValue((int) (atoi(newdata)));
						_Dram[addr] = a;
						free(newdata);
					} else
					if (!strcmp(typ,"Char")) {
						a.changeValue((char) (atoi(newdata)));
						_Dram[addr] = a;
						free(newdata);
					} else
					if (!strcmp(typ,"String")) {
						a.changeValue(newdata);
						_Dram[addr] = a;
					}
				}
			} 
			
			addr = strtaddr; // pgm instructions start address.
			if ((!strcmp(loadtype,"instr")) || (!strcmp(loadtype,"both")))
			{
				char last_line[80];
				int is_last_line = 0;

				char *line = (char *)malloc(80);
				while(fgets(line, 80, fp) != NULL) 
				{
					if (!strcmp(line,"\n")) continue;

					// only labels have character ':'
					if((line != NULL) && (!strstr(line,":"))) 
					{
						// not a label

						int size = extract_size(line);
						if (size == 4)
						{
							a.changeValue(line);
							_Dram[addr] = a;					
							addr += 4; // for 32 bit instructions
						}
						else if (size == 2)
						{
							if (is_last_line)
							{
								char *temp = (char *)malloc(sizeof(line)+1); 
								strcpy(temp, line);
								strcpy(line, last_line);
								strcat(line, "\t#");
								strcat(line, temp);
								strcat(line, "\n");
								is_last_line = 0;
								//free(temp);
								//strcat(last_line, "");

								a.changeValue(line);
								_Dram[addr] = a;					
								addr += 4; // still respecting the word boundary
							}
							else
							{
								if (!is_last_line)
								{
									strcpy(last_line, line);
									is_last_line = 1;
								}
							}
						}
						else
						{
							assert("Unknown size!! Sorry bailing out\n");
							exit(0);
						}
					} 
					else 
					{
						// the line read is a label
						
						//get rid of the ':' at the end of the label...
						for(int i=0;line[i]!=':';i++);
						line[i] = '\0';

						MyString s(line);
						GlobalLabels[s] = addr;
						//delete line;
					}
					line = (char *)malloc(80);
				}
				fclose(fp);
				free(line);
			}
		}			
		
	}	

private :
	int extract_size(char *line)
	{
		int size = 0;
		const char *rest_line1 = (char *)malloc(strlen(line)+1);
		const char *rest_line2 = (char *)malloc(strlen(line)+1);
		const char *rest_line3 = (char *)malloc(strlen(line)+1);

		sscanf(line, "%d%s%s%s", &size, rest_line1, rest_line2, rest_line3);
		strcpy(line, "\t");
		strcat(line, rest_line1);
		strcat(line, "\t");
		strcat(line, rest_line2);
		strcat(line, "\t");
		strcat(line, rest_line3);
		return size;
	}
	
};
#endif
