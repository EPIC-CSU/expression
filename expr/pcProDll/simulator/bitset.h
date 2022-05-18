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
#ifndef _BITSET_H
#define _BITSET_H

#ifndef WIN32
#pragma interface
#endif

/*
 * SET_WORDSIZE is the number of bits of each bitset that fit in a word
 * SET_LOGSIZE is the log_2 of SET_WORDSIZE.  (Note the implicit power
 *      of 2 restriction)
 */
const int SET_WORDSIZE =  32;
const int SET_LOGSIZE  =   5;
const int BYTES_PER_WORD = 4;

class bitset {
private:
	int *word;
	int size;
public:
	bitset(void) { word = NULL; size = 0; }
	~bitset(void) { 
	   if (this && word) free(word);
	}

	void insert(int);
	void operator+=(int x) { insert(x); };
	void remove(int);
	void operator-=(int x) { remove(x); };

	void Union(bitset&);
	void operator+=(bitset& b) { Union(b); };

	void intersection(bitset&);
	void operator&=(bitset& b) { intersection(b); };

	void difference(bitset&);
	void operator-=(bitset& b) { difference(b); };

	void operator=(bitset& b);
	void truncate(void);

	int empty(void);
	int intersects(bitset &b);
	int operator&(bitset& b) { return intersects(b); };
	int equals(bitset &b);
	int operator==(bitset& b) { return equals(b); };
	int contains(int);

};

const int BITS_PER_LONG = 32;

#endif
