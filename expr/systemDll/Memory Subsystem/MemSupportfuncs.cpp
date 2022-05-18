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




// This function loads the memory with the program 
// provided through "filename". The loadtype indicates whether
// the data/instruction/both sections have to be loaded 
// into this memory module. strtaddr indicates the start address 
// when the pgm instructions have to start in the memory. (note that
// the instructions are put sequentially. 
// For data section the address where the data is stored is provided in the 
// input file itself.

void Dram::initmem(long strtaddr, char *loadtype, char *fname) {
	FILE *fp;
	char *line, typ[10], data[80];
	long addr;
	float data;
	
	line = (char *)malloc(50*sizeof(char));
	
	if ((fp = fopen(fname, "r")) == NULL) {
		printf("File: %s cannot be opened\n",fname);
	} else {
		
		// The data section is always at the beginning of the input-file.
		if ((!strcmp(loadtype,"data")) || (!strcmp(loadtype,"both")))  {
			
			if (!strcmp(fgets(line,50,fp),"Data Section:")) {
				assert("Data Section MISSING at the beginning of the file: %s..\n", fname);
			}
			
			while((!feof(fp)) && ((!strcmp(fgets(line,80,fname),"End Data Section")) {
				// Data section is assumed to have 3 parts.	
				sscanf(line,"%lu%s%s",&addr,typ,data);
				if (!strcmp(typ,"Short")) {
					_Dram[addr] = (short) (atoi(data));
				}
				if (!strcmp(typ,"Word")) {
					_Dram[addr] = atoi(data);
				}
				if (!strcmp(typ,"Char")) {
					_Dram[addr] = data[0];
				}
				if (!strcmp(typ,"String")) {
					_Dram[addr] = data;
				}
			}
		} 
		
		addr = strtaddr; // pgm instructions start address.
		if ((!strcmp(loadtype,"instr")) || (!strcmp(loadtype,"both"))) {
			
			map<string, long> lables;
			long i, *tmp;
			char opcode[10], op1[10], op2[10];
			stack blist;
			
			while(!feof(fp)) {
				// only labels have character ':'
				if(!strchr(fgets(line,80,fname),':')) {
					
					// not a label
					_Dram[addr] = line;
					
					// check if the opcode is branch, at which push it into an array.
					if (!strcmp(line,"IF")) {
						tmp = (long *)malloc(sizeof(long));
						*tmp = addr;
						blist.push(tmp);
					}
					
					addr += 4; // all instructions are 32-bit wide
					
				} else {
					// the line read is a label
					lables[*line] = addr;
				}
			}
			
			// this loop resolves the lables used in the program for 
			// branching...
			while(!blist.empty) {
				tmp = blist.pop();
				line = _Dram[*tmp];
				sscanf(line,"%s%s%s",opcode,op1,op2);
				op2 = strcat(op2,":");
				sprintf(line,"%s\t%s\t%ul",opcode,op1,lables[op2]);
				_Dram[*tmp] = line;
			}
		}
		
		fclose(fname);
	}			
};


#endif
