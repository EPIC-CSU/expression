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
// $Id: ArrayIterator.h,v 1.6 1998/04/11 00:35:33 xji Exp $

/*
** ArrayIterator class
** Iterator to go through Array's valid elements
*/

#ifndef _ARRAY_ITERATOR_H
#define _ARRAY_ITERATOR_H

// Includes
#include "Exceptions.h"

// Defines

// Class/(Module Function) declarations 

// Forward declaration of Array
template <class T> class Array;

template <class T> class ArrayIterator
{
private:
#ifdef LEAKY
 //NOS: used for memory leaks
 char tag[MAX_PATH];
#endif
 // Array to iterate across
 Array<T> & array;
 
 // The current element we are on
 int curItem;
 
 // The 1-based index of the current valid element
 int count;
 
 bool isForward;
 
public:
 
/*
** Constructor
** Constructs a new ArrayIterator
** Sets curItem to the first valid array index
** sets count to 1 if curItem exists
** @param v reference to the array to iterate across
 */
 ArrayIterator(Array<T> &v, bool forw = _YES_ ) : array(v), count(0), isForward(forw)
 {
#ifdef LEAKY
  {
   //NOS
   char t[MAX_PATH];
   GetFileTitle(__FILE__,t,MAX_PATH); 
   sprintf(tag,"%d:%s",__LINE__,t);
  }
#endif

  if ( forw == _YES_ ) {
   curItem = 0;
   if (!array.isEmpty())
    while (!array.isValid(curItem) && curItem < array.capacity())
     curItem++;
  } else {
   curItem = array.capacity() - 1;
   if (!array.isEmpty())
    while (!array.isValid(curItem) && curItem >= 0 )
     curItem--;
  }
  count++;
 }
 
 /*
 ** hasMoreElements()
 ** @return 0 if no more elements, non-zero otherwise
 */
 int hasMoreElements()
 {
  return count <= array.size();
 }
 
 /*
 ** nextElement()
 ** advances iterator to next valid element
 */
 void nextElement()
 {
  count++;
  if ( isForward ) {
   curItem++;
   while (curItem < array.capacity() && !array.isValid(curItem) )
    curItem++;
  } else {
   curItem--;
   while (curItem >= 0 && !array.isValid(curItem) )
    curItem--;
  }
 }
 
 /*
 ** currentElement()
 ** @return reference of the current element the iterator points to
 ** it's a const, so you can't change it.
 ** @exception NoSuchElementException if we have gone over bounds of 
 **        array
 */
 T & currentElement()
 {
  if (count <= array.size() && ( isForward && curItem < array.capacity()
   || ! isForward && curItem >=0 ) 
   && array.isValid(curItem))
   return array.elementAt(curItem);
  throw (Exception *)
   new NoSuchElementException("ArrayIterator::nextElement()");
  
  //NOS
  exit(-1);
  return array.elementAt(curItem);
 }
 
 /*
 ** currentIndex()
 ** @return index of the current element
 */
 int currentIndex()
 {
  return curItem;
 }
 
 /*
 ** reset()
 ** Resets the iterator
 */
 void reset()
 {
  count = 0;
  
  if ( isForward ) {
   curItem = 0;
   if (!array.isEmpty())
    while (!array.isValid(curItem) && curItem < array.capacity())
     curItem++;
  } else {
   curItem = array.capacity() - 1;
   if (!array.isEmpty())
    while (!array.isValid(curItem) && curItem >= 0 )
     curItem--;
  }
  count++;
 }
};

#endif
