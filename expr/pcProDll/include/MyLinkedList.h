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
// $Id: MyLinkedList.h,v 1.5 1998/01/22 23:59:08 pgrun Exp $

/*
** MyLinkedList
** Declarations of the MyLinkedList class
** Unfortunately it is a template, so implementation is visible
**
** I would eventually like to see this implemented using the STL...
*/

#ifndef _MYLINKED_LIST_H
#define _MYLINKED_LIST_H

// Includes
#ifndef WIN32
#include <stddef.h>
#include <iostream.h>
#endif

#include "MyLinkedListIterator.h"

// Defines

// Class Definitions

/*
** This is the node class which is used by the linked list
*/
template <class T> class MyListNode
{
private:
#ifdef LEAKY
	//NOS: used for memory leaks
	char tag[MAX_PATH];
#endif
	
	// The node's item
	T item;
	
	// Link to next node
	MyListNode *next;
	
	// Link to previous node
	MyListNode *prev;
	
public:
	
/*
** Constructor
** @param new_item the item to add (copy is made)
** @param p the previous node to initialize to
** @param n the next node to initialize to
** If previous and next are non-NULL, then also set their next and/or
** previous pointers correctly
	*/
	MyListNode(T & new_item, MyListNode *p, MyListNode *n) :	next(n), prev(p)
	{
#ifdef LEAKY
		{
			//NOS
			char t[MAX_PATH];
			GetFileTitle(__FILE__,t,MAX_PATH); 
			sprintf(tag,"%d:%s",__LINE__,t);
		}
#endif    
		
		item = new_item;
		
		if (n)
			n->prev = this;
		if (p)
			p->next = this;
	}
	
	/*
	** Destructor
	** deletes our local copy of the item
	*/   
	~MyListNode()
	{
	}
	
	/*
	** Copy constructor
	** Copy the items, and set next/previous pointer to null.
	** it is up to the caller to set prev/next properly
	** using placeBefore/placeAfter
	*/
	MyListNode(MyListNode &node)
	{
		{
			//NOS
			char t[MAX_PATH];
			GetFileTitle(__FILE__,t,MAX_PATH); 
			sprintf(tag,"%d:%s",__LINE__,t);
		}
		
		item = node.item;
		prev = next = NULL;
	}
	
	/*
	** getItem()
	** @return the item in this node
	*/
	T & getItem()
	{
		return item;
	}
	
	/*
	** nextItem()
	** @return the pointer to next node
	*/
	MyListNode *nextItem()
	{
		return next;
	}
	
	/*
	** prevItem()
	** @return the pointer to previous node
	*/
	MyListNode *prevItem()
	{
		return prev;
	}
	
	// for the next two functions, you can place nodes before or
	// after NULL
	/*
	** placeBefore(MyListNode *)
	** Places this node before the parameter
	** @param next_node the node to place this one before
	*/
	void placeBefore(MyListNode * next_node)
	{
		if (next_node)
			next_node->prev = this;
		next = next_node;
	}
	
	/*
	** placeAfter(MyListNode *)
	** Places this node after the parameter
	** @param next_node the node to place this one after
	*/
	void placeAfter(MyListNode * prev_node)
	{
		if (prev_node)
			prev_node->next = this;
		prev = prev_node;
	}
	
	friend ostream & operator<< (ostream &out, MyListNode &t)
	{
		out << " Node: " << t.item;
		return out;
	}
};


template <class T> class MyLinkedList 
{
	// This is needed so that the iterator knows what the head and
	// tail of the list are...
	friend class MyLinkedListIterator<T>;
	
#ifdef LEAKY
	//NOS: used for memory leaks
	char tag[MAX_PATH];
#endif
	
private:
	// Number of items in the list
	int num_items;
	
	// Pointer to the head of the list
	MyListNode<T> *head;
	// Pointer to the tail of the list
	MyListNode<T> *tail;
	
public:
	
/*
** findElem(const T &)
** Finds the first occurrance of an item in the list
** @param elem the element to find
** @return pointer to the element (NULL if not in list
	*/
	MyListNode<T> * findElem(const T &elem)
	{
		MyListNode<T> *cur = head;
		
		while (cur)
		{
			if (cur->getItem() == elem)
				break;
			cur = cur->nextItem();
		}
		return cur;
	}
	
	/*
	** Constructor
	** Constructs basic linked list
	*/
	MyLinkedList() : head(NULL), tail(NULL), num_items(0)
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
	~MyLinkedList()
	{
		MyListNode<T> *cur = head;
		
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
	MyLinkedList(const MyLinkedList<T> &l)
	{
		MyListNode<T> *temp = l.head, *new_node = NULL, *cur = NULL;
		
		head = tail = NULL;
		num_items = 0;
		
		while (temp) 
		{
			new_node = new MyListNode<T>(temp->getItem(), cur, NULL);
			
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
		head = new MyListNode<T>(item, NULL, head);
		
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
		if((tail = new MyListNode<T>(item, tail, NULL))==NULL){
			//out of memory
			_ASSERT(0);
		}
		
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
		MyListNode<T> *temp = head;
		
		if (!head)
			throw (Exception *)
			new NoSuchElementException("MyLinkedList::pop()");
		
		head = head->nextItem();
		if ( head )
			head->placeAfter(NULL);
		
		if ( tail == temp ) // only one element in the list
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
		MyListNode<T> *temp = tail;
		
		if (!tail)
			throw (Exception *)
			new NoSuchElementException("MyLinkedList::pop()");
		
		tail = tail->prevItem();
		tail->placeBefore(NULL);
		
		if ( head == temp ) // only one element in the list
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
	bool remove(const T & item)
	{
		MyListNode<T> *cur = head;
		MyListNode<T> *prev, *next;
		
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
		if (!head){
			abort();
			throw (Exception *)
				new NoSuchElementException("MyLinkedList::getHead()");
		}
		
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
			new NoSuchElementException("MyLinkedList::getTail()");
		
		return tail->getItem();
	}
	
	/*
	** contains(const T &)
	** Sees if the item is in the list
	** @param elem the item to find
	** @return true if element in list, false otherwise
	*/
	bool contains(const T &elem)
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
	//NOS
	//void insertBefore(T & elem, const T &find)
	void insertBefore(T & elem, const T &to_find)
	{
		MyListNode<T> *cur = findElem(to_find);
		MyListNode<T> *new_node;
		
		if (!cur)
			throw (Exception *)
			new NoSuchElementException("MyLinkedList::insertBefore");
		
		new_node = new MyListNode<T>(elem, cur->prevItem(), cur);
		
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
	//NOS
	//void insertAfter(T & elem, const T &find)
	void insertAfter(T & elem, const T &to_find)
	{
		MyListNode<T> *cur = findElem(to_find);
		MyListNode<T> *new_node;
		
		if (!cur)
			throw (Exception *)
			new NoSuchElementException("MyLinkedList::insertAfter");
		
		new_node = new MyListNode<T>(elem, cur, cur->nextItem());
		
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
	MyLinkedListIterator<T> * elementsForward()
	{
		return new MyLinkedListIterator<T>(*this, true);
	}
	
	/*
	** elementsBackward()
	** Returns a new backward iterator for this list.
	** Responsibility falls on the caller to delete the returned object
	** Note that this violates our same creator same destroyer paradigm
	*/
	MyLinkedListIterator<T> * elementsBackward()
	{
		return new MyLinkedListIterator<T>(*this, false);
	}
	
	friend ostream & operator<< (ostream &out, MyLinkedList<T> &t)
	{
		out << "MyLinked List: num_items = " << t.num_items << endl;
		MyListNode<T> *cur = t.head;
		
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
	
	
	// removes all the nodes in the list
	void removeAll() {
		MyListNode<T> *cur = head;
		
		while (cur) 
		{
			head = cur->nextItem();
			delete cur;
			cur = head;
		}
		
		num_items = 0;
		head = tail = NULL;
	}
	
	
	// similar as append, except if an element is already in 
	// the list, need not append it again.
	void add(T &item)
	{
		if ( this->findElem(item) ) 
			return;
		tail = new MyListNode<T>(item, tail, NULL);
		
		if (!head)
			head = tail;
		
		num_items++;
	}
};

#endif

