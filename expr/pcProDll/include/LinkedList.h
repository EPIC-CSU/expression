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
// $Id: LinkedList.h,v 1.5 1998/01/22 23:58:58 pgrun Exp $

/*
** LinkedList
** Declarations of the LinkedList class
** Unfortunately it is a template, so implementation is visible
**
** I would eventually like to see this implemented using the STL...
*/

#ifndef _LINKED_LIST_H
#define _LINKED_LIST_H

// Includes
#ifndef WIN32
#include <stddef.h>
#include <iostream.h>
#endif

#include "LinkedListIterator.h"

// Defines

// Class Definitions

/*
** This is the node class which is used by the linked list
*/
template <class T> class ListNode
{
 private:
#ifdef LEAKY
  //NOS: used for memory leaks
  char tag[MAX_PATH];
#endif
  
  // The node's item
  T *item;
  
  // Link to next node
  ListNode *next;
  
  // Link to previous node
  ListNode *prev;
  
 public:
  
 /*
 ** Constructor
 ** @param new_item the item to add (copy is made)
 ** @param p the previous node to initialize to
 ** @param n the next node to initialize to
 ** If previous and next are non-NULL, then also set their next and/or
 ** previous pointers correctly
  */
  ListNode(T & new_item, ListNode *p, ListNode *n) :       next(n), prev(p)
  {
#ifdef LEAKY
   {
    //NOS
    char t[MAX_PATH];
    GetFileTitle(__FILE__,t,MAX_PATH); 
    sprintf(tag,"%d:%s",__LINE__,t);
   }
#endif
   
   item = new_item.copy();
   if (n)
    n->prev = this;
   if (p)
    p->next = this;
  }
  
  /*
  ** Destructor
  ** deletes our local copy of the item
  */   
  ~ListNode()
  {
   delete item;
  }
  
  /*
  ** Copy constructor
  ** Copy the items, and set next/previous pointer to null.
  ** it is up to the caller to set prev/next properly
  ** using placeBefore/placeAfter
  */
  ListNode(ListNode &node)
  {
   item = node.item->copy();
   prev = next = NULL;
  }
  
  /*
  ** getItem()
  ** @return the item in this node
  */
  T & getItem()
  {
   return *item;
  }
  
  /*
  ** nextItem()
  ** @return the pointer to next node
  */
  ListNode *nextItem()
  {
   return next;
  }
  
  /*
  ** prevItem()
  ** @return the pointer to previous node
  */
  ListNode *prevItem()
  {
   return prev;
  }
  
  // for the next two functions, you can place nodes before or
  // after NULL
  /*
  ** placeBefore(ListNode *)
  ** Places this node before the parameter
  ** @param next_node the node to place this one before
  */
  void placeBefore(ListNode * next_node)
  {
   if (next_node)
    next_node->prev = this;
   next = next_node;
  }
  
  /*
  ** placeAfter(ListNode *)
  ** Places this node after the parameter
  ** @param next_node the node to place this one after
  */
  void placeAfter(ListNode * prev_node)
  {
   if (prev_node)
    prev_node->next = this;
   prev = prev_node;
  }
  
  friend ostream & operator<< (ostream &out, ListNode &t)
  {
   out << " Node: " << *t.item;
   return out;
  }
};

template <class T> class LinkedList 
{
 // This is needed so that the iterator knows what the head and
 // tail of the list are...
 friend class LinkedListIterator<T>;
 
 private:
#ifdef LEAKY
  //NOS: used for memory leaks
  char tag[MAX_PATH];
#endif
  
  // Number of items in the list
  int num_items;
  
  // Pointer to the head of the list
  ListNode<T> *head;
  // Pointer to the tail of the list
  ListNode<T> *tail;
  
  /*
  ** findElem(const T &)
  ** Finds the first occurrance of an item in the list
  ** @param elem the element to find
  ** @return pointer to the element (NULL if not in list
  */
  ListNode<T> * findElem(T &elem)
  {
   ListNode<T> *cur = head;
   
   while (cur)
   {
    if (cur->getItem() == elem)
     break;
    cur = cur->nextItem();
   }
   return cur;
  }
  
 public:
  
 /*
 ** Constructor
 ** Constructs basic linked list
  */
  LinkedList() : head(NULL), tail(NULL), num_items(0)
  {
#ifdef LEAKY
   {
    //NOS
    char t[MAX_PATH];
    GetFileTitle(__FILE__,t,MAX_PATH); 
    sprintf(tag,"%d:%s",__LINE__,t);
   }
#endif
  
  }
  
  /*
  ** Destructor
  ** Deletes all nodes in the list
  */
  ~LinkedList()
  {
   ListNode<T> *cur = head;
   
   while (cur) 
   {
    head = cur->nextItem();
    delete cur;
    cur = head;
   }
  }
  
  /*
  ** Copy constructor
  ** Copies linked list and all it's objects
  */
  LinkedList(LinkedList<T> &l)
  {
   ListNode<T> *temp = l.head, *new_node = NULL, *cur = NULL;
   
   head = tail = NULL;
   num_items = 0;
   
   while (temp) 
   {
    new_node = new ListNode<T>(temp->getItem(), cur, NULL);
    
    if (!cur)
     head = new_node;
    
    cur = tail = new_node;
    num_items++;
    temp = temp->nextItem();
   }
   tail = cur;
  }
  
  /*
  ** prepend(const T &)
  ** Prepends (adds to front) an item to the list
  ** @param item the item to add
  */
  void prepend(T &item)
  {
   head = new ListNode<T>(item, NULL, head);
   
   if (!tail)
    tail = head;
   
   num_items++;
  }
  
  /*
  ** append(const T &)
  ** Appends (adds to end) an item to the list
  ** @param item the item to add
  */
  void append(T &item)
  {
   tail = new ListNode<T>(item, tail, NULL);
   
   if (!head)
    head = tail;
   
   num_items++;
  }
  
  /*
  ** removeFirst()
  ** Removes the first item in the list
  */
  void removeFirst()
  {
   ListNode<T> *temp = head;
   
   if (!head)
    throw (Exception *)
    new NoSuchElementException("LinkedList::pop()");
   
   head = head->nextItem();
   if ( head ) 
    head->placeAfter(NULL);
   
   if ( tail == temp )  // only one element
    tail = NULL;
   
   // just in case
   temp->placeAfter(NULL);
   temp->placeBefore(NULL);
   delete temp;
   num_items--;
  }
  
  /*
  ** removeLast()
  ** Removes the last item in the list
  */
  void removeLast()
  {
   ListNode<T> *temp = tail;
   
   if (!tail)
    throw (Exception *)
    new NoSuchElementException("LinkedList::pop()");
   
   tail = tail->prevItem();
   tail->placeBefore(NULL);
   
   if ( head == temp )  // only one element
    head = NULL;
   
   // just in case
   temp->placeAfter(NULL);
   temp->placeBefore(NULL);
   delete temp;
   num_items--;
  }
  
  /*
  ** remove(const T &)
  ** Removes the specified item from the list.  It removes the first
  ** occurance of the item (from the front)
  ** @param item the item to remove
  ** @return true if the item was found and removed false if otherwise
  **         (the item was not found in the list)
  */
  bool remove(T & item)
  {
   ListNode<T> *cur = head;
   ListNode<T> *prev, *next;
   
   cur = findElem(item);
   
   if (!cur)
    return false;
   
   prev = cur->prevItem();
   next = cur->nextItem();
   
   cur->placeBefore(NULL);
   cur->placeAfter(NULL);
   delete cur;
   num_items--;
   
   if (prev)
    prev->placeBefore(next);
   else
   {
    head = next;
    if (head)
     head->placeAfter(NULL);
   }
   
   if (!next)
    tail = prev;
   
   return true;
  }
  
  /*
  ** numItems()
  ** Find the number of items in the list
  ** @return the number of items in the list
  */
  int numItems()
  {
   return num_items;
  }
  
  /*
  ** isEmpty()
  ** See if list is empty
  ** @return true if list is empty, false otherwise
  */
  bool isEmpty()
  {
   return num_items == 0;
  }
  
  /*
  ** getHead()
  ** Returns the first item in the list
  ** @return Reference to the first item in list
  ** @exception NoSuchElementException if the list is empty
  */
  T & getHead()
  {
   if (!head)
    throw (Exception *)
    new NoSuchElementException("LinkedList::getHead()");
   
   return head->getItem();
  }
  
  /*
  ** getTail()
  ** Returns the last item in the list
  ** @return Reference to the last item in list
  ** @exception NoSuchElementException if the list is empty
  */
  T & getTail()
  {
   if (!tail)
    throw (Exception *)
    new NoSuchElementException("LinkedList::getTail()");
   
   return tail->getItem();
  }
  
  /*
  ** contains(const T &)
  ** Sees if the item is in the list
  ** @param elem the item to find
  ** @return true if element in list, false otherwise
  */
  bool contains(T &elem)
  {
   return findElem(elem) != NULL;
  }
  
  /*
  ** insertBefore(const T &, const T &)
  ** Inserts an element in the list before the first occurrance of the
  ** specified item.  Throws an exception if the specified item is not
  ** in the list.
  ** @param elem the item to add
  ** @param find the item to add before
  ** @exception NoSuchElementException if the item 'find' is not in list
  */
  void insertBefore(T & elem, T &find)
  {
   ListNode<T> *cur = findElem(find);
   ListNode<T> *new_node;
   
   if (!cur)
    throw (Exception *)
    new NoSuchElementException("LinkedList::insertBefore");
   
   new_node = new ListNode<T>(elem, cur->prevItem(), cur);
   
   if (cur == head)
    head = new_node;
   
   num_items++;
  }
  
  /*
  ** insertAfter(const T &, const T &)
  ** Inserts an element in the list after the first occurrance of the
  ** specified item.  Throws an exception if the specified item is not
  ** in the list.
  ** @param elem the item to add
  ** @param find the item to add after
  ** @exception NoSuchElementException if the item 'find' is not in list
  */
  void insertAfter(T & elem, T &find)
  {
   ListNode<T> *cur = findElem(find);
   ListNode<T> *new_node;
   
   if (!cur)
    throw (Exception *)
    new NoSuchElementException("LinkedList::insertAfter");
   
   new_node = new ListNode<T>(elem, cur, cur->nextItem());
   
   if (cur == tail)
    tail = new_node;
   
   num_items++;
  }
  
  /*
  ** elementsForward()
  ** Returns a new forward iterator for this list.
  ** Responsibility falls on the caller to delete the returned object
  ** Note that this violates our same creator same destroyer paradigm
  */
  LinkedListIterator<T> * elementsForward()
  {
   return new LinkedListIterator<T>(*this, true);
  }
  
  /*
  ** elementsBackward()
  ** Returns a new backward iterator for this list.
  ** Responsibility falls on the caller to delete the returned object
  ** Note that this violates our same creator same destroyer paradigm
  */
  LinkedListIterator<T> * elementsBackward()
  {
   return new LinkedListIterator<T>(*this, false);
  }
  
  friend ostream & operator<< (ostream &out, LinkedList<T> &t)
  {
   out << "Linked List: num_items = " << t.num_items << endl;
   ListNode<T> *cur = t.head;
   
   if (!cur)
    out << " EMPTY" << endl;
   else
    while (cur)
    {
#ifndef WIN32
     out << *cur;
#endif
     cur = cur->nextItem();
    }
    
    return out;
  }
  
  
  
  ListNode<T> * findE(T &elem)
  {
   ListNode<T> *cur = head;
   
   while (cur)
   {
    if ( elem.isEq(&cur->getItem()) )
     break;
    cur = cur->nextItem();
   }
   return cur;
  }
  
  bool removeHier(T & item)
  {
   ListNode<T> *cur = head;
   ListNode<T> *prev, *next;
   
   cur = findE(item);
   
   if (!cur)
    return false;
   
   prev = cur->prevItem();
   next = cur->nextItem();
   
   cur->placeBefore(NULL);
   cur->placeAfter(NULL);
   delete cur;
   num_items--;
   
   if (prev)
    prev->placeBefore(next);
   else
   {
    head = next;
    if (head)
     head->placeAfter(NULL);
   }
   
   if (!next)
    tail = prev;
   
   return true;
  }
  
  
};

#endif

