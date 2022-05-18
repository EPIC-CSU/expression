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
#include "stdafx.h"

#ifndef WIN32
#pragma implementation "bitset.h"
#endif

#ifndef WIN32
#include <stdio.h>
#include <malloc.h>
#include <errno.h>
#endif

#include "bitset.h"
#include "stdhdr.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#ifndef WIN32
#define min(a,b) ( (a) < (b) ? a : b )
#endif

void
bitset::Union(bitset &b)
{
  //int n;

  if (b.size == 0) return;

  if (size == 0)
  {
     *this = b;
     return;
  }

  if (size < b.size) 
  {
    // 5Jan97 word = (int *) realloc((char *)word,(b.size)*BYTES_PER_WORD);

    int *old = word;
    word = (int *) malloc((b.size)*BYTES_PER_WORD);
    bcopy((char*)old, (char *)word, size*BYTES_PER_WORD);

    free(old);

    bzero((char *)(word + size), (b.size-size)*BYTES_PER_WORD);

    size = b.size;
  } 

  int *lasta = word+b.size;
  int *firsta = word;
  int *firstb = b.word;

  for (; firsta < lasta; *(firsta++) |= *(firstb++))
	;
}

void
bitset::intersection(bitset& b)
{
  if (size == 0) return ;

  if (b.size == 0) {
     if (word) free(word);
     word = NULL;
     size = 0;
     return;
  }

  int i;
  for (i=0; i<min(size,b.size); i++)
      word[i] &= b.word[i];

  while (i<size) 
      word[i++] = 0;
}

void
bitset::difference(bitset& b)
{
  if (size == 0) return;
  if (b.size == 0) return;

  int i;
  for (i=0; i<min(size,b.size); i++)
    word[i] &= ~b.word[i];
}

int
bitset::empty(void) 
{
	if (size == 0) return 1;

	int i;
	for(i=0; i < size; i++)
	   if (word[i]) return 0;
	
	return 1;
}

int
bitset::intersects(bitset& b)
{
	if (size == 0) return 0;
	if (b.size == 0) return 0;

	int i;
	for (i=0; i<min(size,b.size); i++)
	   if (word[i] & b.word[i]) return 1;
	
	return 0;
}

void
bitset::insert(int i)
{
  if (i >= (size<<SET_LOGSIZE)) 
  {	/* resize is necessary */
    int blen;

    /* 17May94 SHN */
    blen = (i+1+(SET_WORDSIZE-1))>>SET_LOGSIZE;

    if (size == 0)
       word = (int *)malloc(blen*BYTES_PER_WORD);
    else
    {
       // 5Jan97 word = (int *)realloc(word, blen*BYTES_PER_WORD);
       int *old = word;
       word = (int *)malloc(blen*BYTES_PER_WORD);
       bcopy((char*)old, (char*)word, size*BYTES_PER_WORD);
       free(old);
    }

    bzero((char *)(word + size), (blen-size)*BYTES_PER_WORD);
    
    size = blen;
  }
  int word_pos = i >> SET_LOGSIZE;	/* find which word is affected */
  int bit_pos = i % SET_WORDSIZE;	/* find which bit to bitset */
  word[word_pos] |= (1 << bit_pos);
}

void
bitset::remove(int i)
{
  if (i < (size<<SET_LOGSIZE)) 
  {	/* the element fits? otherwise, nothing to remove */
    int word_pos = i >> SET_LOGSIZE;
    int bit_pos = i % SET_WORDSIZE;
    word[word_pos] &= (~(1 << bit_pos));
  }
}

int
bitset::contains(int x)
{
  if ( (size<< SET_LOGSIZE) <= x) 
    return 0;
  else
    return
          ((word[(x>>SET_LOGSIZE)] &
	   (1 << (x % SET_WORDSIZE)))	!= 0);
}

void
bitset::truncate(void)
{
	if (this) 
	   if (size>0) bzero((char *)word,size*BYTES_PER_WORD);
}

bitset::equals(bitset& b)
{
	if (empty())
	   return b.empty();
	else
	   if (b.empty())
	      return 0;

	int smaller, bigger;
	int *c;
	if (size<b.size) {
	   smaller = size;
	   bigger = b.size;
	   c = b.word;
	} else {
	   smaller = b.size;
	   bigger = size;
	   c = word;
	}

	int i;
	for(i=0; i<smaller; i++)
	   if (word[i] != b.word[i]) return 0;
	
	for(; i<bigger; i++)
	   if (c[i]) return 0;
	
	return 1;
}

void
bitset::operator=(bitset &b)
{
  if (word) free((char *)word);

  size = b.size;
  word = (int *)malloc((size)*BYTES_PER_WORD);
  bcopy((char *)b.word,(char *)word,(size)*BYTES_PER_WORD);
}


