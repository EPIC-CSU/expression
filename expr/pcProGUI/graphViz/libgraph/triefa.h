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
/* File - TrieFA.h
 *
 *    The data types for the generated trie-based finite automata.
 */

struct TrieState {				/* An entry in the FA state table			*/
	short			def;		/* 	If this state is an accepting state then*/
								/*	this is the definition, otherwise -1.	*/
	short			trans_base;	/* The base index into the transition table.*/
	long			mask;		/* The transition mask. 					*/
};

struct TrieTrans {				/* An entry in the FA transition table.		*/
	short			c;				/* The transition character (lowercase).*/
	short			next_state;		/* The next state.						*/
};

typedef struct TrieState TrieState;
typedef struct TrieTrans TrieTrans;

extern TrieState	TrieStateTbl[];
extern TrieTrans	TrieTransTbl[];
