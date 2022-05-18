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
// $Id: MyLinkedListIterator.h,v 1.5 1998/03/27 00:15:10 ahalambi Exp $

/*
** MyLinkedListIterator
** Declarations of the MyLinkedListIterator class
** Unfortunately it is a template, so implementation is visible
**
** I would eventually like to see this implemented with the STL...
*/

#ifndef _MYLINKED_LIST_ITERATOR_H
#define _MYLINKED_LIST_ITERATOR_H

// Includes
#include "Exceptions.h"

// Defines

// Class Definitions

// Forward declaration of LinkedList
template <class T> class MyLinkedList;
template <class T> class MyListNode;

template <class T> class MyLinkedListIterator
{
private:
#ifdef LEAKY
 //NOS: used for memory leaks
 char tag[MAX_PATH];
#endif
 
 // The linked temp_list this iterator is associated with
 MyLinkedList<T> &l;
 
 // The current item this iterator is pointing to
 MyListNode<T> *cur_item;
 
 // How many items we have seen
 int count;
 
 // This is a flag to know if we are a forward or backward iterator
 bool isForward;
 
public:
 
/*
** Constructor
** Constructs a (forward or backward) iterator
** @param temp_list the temp_list we are associated with
** @param forw true if forward iterator, false if backward iterator
 */
 MyLinkedListIterator(MyLinkedList<T> &temp_list, bool forw) :	l(temp_list), count(1), isForward(forw) 
 {
#ifdef LEAKY
  {
   //NOS
   char t[MAX_PATH];
   GetFileTitle(__FILE__,t,MAX_PATH); 
   sprintf(tag,"%d:%s",__LINE__,t);
  }
#endif
  
  cur_item = isForward ? l.head : l.tail;
 }
 
 /*
 ** hasMoreElements()
 ** sees if there are more items left in the iterator
 ** @return true if more elements left, false if not
 */
 bool hasMoreElements()
 {
  //return count <= l.numItems();
  return cur_item!= NULL;
 }
 
 /*
 ** nextElement()
 ** Advances the iterator to the next element
 */
 void nextElement()
 {
  if (!cur_item)
   throw (Exception *) 
   new NoSuchElementException("MyLinkedListIterator::nextElement()");
  
  cur_item = isForward ? cur_item->nextItem() : cur_item->prevItem();
  count++;
 }
 
 /*
 ** currentElement()
 ** Returns the current element of the iterator
 ** @return the current item
 ** @exception NoSuchElementException if the current item is invalid
 */
 T & currentElement()
 {
  //if (count <= l.numItems())
  if ( cur_item ) 
   return cur_item->getItem();
  
  abort();
  throw (Exception *) 
   new NoSuchElementException("MyLinkedListIterator::currentElement()");
  
  //NOS
  
  // Comment to ASSERT_TRACE(0,"Forced EXIT!"); added by Ashok on Mar 26th
  // I use this exception to do  some stuff. So, exit should not be
  // called everytime this exception is thrown.
  
  //	ASSERT_TRACE(0,"Forced EXIT!");
  return cur_item->getItem();
 }
 
 /*
 ** reset()
 ** Resets the iterator (correct if forward or backward
 */
 void reset()
 {
  count = 1;
  cur_item = isForward ? l.head : l.tail;
 }
};

#endif

