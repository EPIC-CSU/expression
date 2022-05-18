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
// $Id: Array.h,v 1.8 1998/04/11 00:35:31 xji Exp $

/*
** Array class
** Growable array
** This is alot like C's array, but with both bounds checking, and
** the ability to grow.  
**
** NOTE!!!
** If you make an array of pointers to objects, you can NOT delete
** your copy of the object, then access the array.  (Because the array now
** has a dangling pointer.
** e.g., from foo.cc:
** Array<int *> bar;
** int *baz = new int(5);
** bar.setElementAt(baz, 0);
** delete baz;
** bar.elementAt(0);        // ERROR HERE - pointer ref is gone!
**
** Also note, that if the array is an array of pointers, to remember to
** delete each object as well
**
** I would eventually like to see this done using a STL implementation,
** but STL really has no sparse vector type (it is all compacted).
*/

#ifndef _ARRAY_H
#define _ARRAY_H

// Includes
#include "GlobalIncludes.h"
#include "Exceptions.h"
#include "ArrayIterator.h"
#ifndef WIN32
#include <assert.h>
#endif

// Defines
#define ARRAY_INITIAL_CAP 10

// Class/(Module Function) declarations 

template <class T> class Array
{
private:
#ifdef LEAKY
 //NOS: used for memory leaks
 char tag[MAX_PATH];
#endif
 
 
 // Array of data
 T **elementData;
 
 // Number of valid elements in the array
 int elementCount;
 
 // How many elements to increment capacity
 int capacityIncrement;
 
 // How many elements the array can store
 int arrayCapacity;
 
public:
 
/*
** Constructor
** @param initial_capacity the initial size of the array
**        defaults to ARRAY_INITIAL_CAP
** @param capacity_increment the size to increase the array when needed
**        defaults to 0 (cannot change size)
 */
 Array(int initial_capacity = ARRAY_INITIAL_CAP, int capacity_increment = 0) : 
      elementCount(0), capacityIncrement(capacity_increment), arrayCapacity(initial_capacity)
 {
#ifdef LEAKY
  {
   //NOS
   char t[MAX_PATH];
   GetFileTitle(__FILE__,t,MAX_PATH); 
   sprintf(tag,"%d:%s",__LINE__,t);
  }
#endif
  
  if (arrayCapacity <=0)
  {
   elementData = NULL;
   return;
  }
  
  elementData = new T*[arrayCapacity];
  for (int i = 0; i < arrayCapacity; i++)
   elementData[i] = NULL;
 }
 
 /* 
 ** Destructor
 */
 ~Array()
 {
  removeAllElements();
  delete[] elementData;
 }
 
 /*
 ** Copy constructor
 ** Copy a array (and all it's objects).
 ** Calls the copy constructor on the objects...
 */
 Array(const Array<T> & v) :
 elementCount(v.elementCount), capacityIncrement(v.capacityIncrement), 
  arrayCapacity(v.arrayCapacity)
 {
  if (arrayCapacity <= 0)
  {
   elementData = NULL;
   return;
  }
  
  elementData = new T*[v.arrayCapacity];
  
  for (int i = 0; i < arrayCapacity; i++)
   if (v.elementData[i])
    elementData[i] = new T(*v.elementData[i]);
   else
    elementData[i] = NULL;
 }
 
 /*
 ** setSize(const int)
 ** Sets the size of the array. If the size shrinks, the extra elements
 ** (at the end of the array) are lost; if the size increases, the
 ** memory is allocated to handle them - memory is increased by
 ** capacityIncrement at least...
 **
 ** Ashok: May, 15 97
 ** Also, if the size is increased, the number of valid elements remains
 ** the same. If the size is decreased, the number of valid elements is
 ** (as of now) set to zero. This seems to be the only easy solution.
 ** Note that this is a very important point. As of now, it seems that
 ** decreasing the size of the array must also be accompanied by a call
 ** to the method that can set the number of valid elements in the array.
 ** end: May, 15 97
 **
 ** @param newSize the new size of the array
 ** @exception Exception - if capacityIncrement is 0
 */
 void setSize(const int newSize) 
 {
  if (newSize == arrayCapacity)
   return;
  
  if (!capacityIncrement)
   throw (Exception *)
   new Exception("Try to set capacity of fixed array");
  
  T **oldData = elementData;
  int oldCapacity = arrayCapacity;
  
  if (newSize > arrayCapacity) 
   while (arrayCapacity < newSize)
    arrayCapacity += capacityIncrement;
   else
   {
    arrayCapacity = newSize;
    elementCount = 0;
   }
   
   if (arrayCapacity <= 0) return;
   
   elementData = new T*[arrayCapacity];
   
   for (int i = 0; i < oldCapacity && i < arrayCapacity; i++)
    elementData[i] = oldData[i];
   
   if (oldCapacity < arrayCapacity)
    for (int i = oldCapacity; i < arrayCapacity; i++)
     elementData[i] = NULL;
    
    delete[] oldData;
 }
 
 /*
 ** capacity()
 ** @return the current capacity of the array - the number of elements
 **         able to be stored in the array
 */
 int capacity() 
 {
  return arrayCapacity;
 }
 
 /*
 ** size()
 ** @returns the number of valid elements in the array.
 ** Note that this is not the same as the array's capacity.
 */
 int size() 
 {
  return elementCount;
 }
 
 /*
 ** isEmpty()
 ** @return true if the collection contains no values.
 */
 bool isEmpty() 
 {
  return elementCount == 0;
 }
 
 /*
 ** elements()
 ** @return a pointer to a new iterator over this array.
 ** It is up to the caller to free the iterator
 ** This violates our same creator/same deletor paradigm, really.
 */
 ArrayIterator<T> * elements()
 {
  return new ArrayIterator<T>(*this);
 }
 
 /* 
 ** elementsBackward
 ** similar as elements, but backward 
 */ 
 ArrayIterator<T> * elementsBackward()
 {
  return new ArrayIterator<T>(*this, false);
 }
 
 /*
 ** contains(const T &)
 ** @param elem reference to the desired element
 ** @return true if the specified object is a value of the array.
 */
 bool contains(const T &elem)
 {
  return indexOf(elem, 0) >= 0;
 }
 
 /*
 ** isValid(const int)
 ** @param index reference to the desired element
 ** @return true if the specified index  is a valid value of the array.
 */
 bool isValid(const int index)
 {
  return elementData[index] != NULL;
 }
 
 /*
 ** indexOf(const T &)
 ** Searches for the specified object, starting from the first position
 ** and returns an index to it.
 ** @param elem reference to the desired element
 ** @return the index of the element, or -1 if it was not found.
 */
 int indexOf(const T & elem)
 {
  return indexOf(elem, 0);
 }
 
 /*
 ** indexOf(const T &, int)
 ** Searches for the specified object, starting at the specified 
 ** position and returns an index to it.
 ** @param elem the desired element
 ** @param index the index where to start searching
 ** @return the index of the element, or -1 if it was not found.
 */
 int indexOf(const T &elem, int index) 
 {
  for (int i = index ; i < arrayCapacity; i++) 
   if (elementData[i] && elem == *elementData[i]) 
    return i;
   return -1;
 }
 
 /*
 ** lastIndexOf(const T &) 
 ** Searches backwards for the specified object, starting from the last
 ** position and returns an index to it. 
 ** @param elem reference to the desired element
 ** @return the index of the element, or -1 if it was not found.
 */
 int lastIndexOf(const T &elem)
 {
  return lastIndexOf(elem, arrayCapacity);
 }
 
 /*
 ** lastIndexOf(const T &, int)
 ** Searches backwards for the specified object, starting from the specified
 ** position and returns an index to it. 
 ** @param elem reference to the desired element
 ** @param index the index where to start searching
 ** @return the index of the element, or -1 if it was not found.
 */
 int lastIndexOf(const T &elem, int index) 
 {
  for (int i = index ; --i >= 0 ; )
   if (elementData[i] && elem == *elementData[i])
    return i;
   return -1;
 }
 
 /*
 ** elementAt(int)
 ** Returns reference to the element at the specified index.
 ** @param index the index of the desired element
 ** @exception ArrayIndexOutOfBoundsException If an invalid index given.
 ** @exception NoSuchElementException If an invalid element is at index.
 */
 T & elementAt(int index)
 {
  if (index >= arrayCapacity || index < 0) 
  {
   cout << " ArrayOutOfBoundsException: Array::elementAt()" << index; 
   throw (Exception *) 
    new ArrayOutOfBoundsException("Array::elementAt()", index);
  }
  
  if (!elementData[index])
   throw (Exception *)
   new NoSuchElementException("Array::elementAt()");
  
  return *elementData[index];
 }
 
 /*
 ** setElementAt(const T, int)
 ** Sets the element at the specified index to be the specified object.
 ** The previous element at that position is discarded.
 ** Note that setElementAt uses a COPY of the object.
 ** Call this method when you want to add an object to the array.
 ** @param obj what the element is to be set to
 ** @param index the specified index
 ** @exception ArrayIndexOutOfBoundsException If the index was 
 ** invalid.
 */
 void setElementAt(const T & obj, int index)
 {
  if (index >= arrayCapacity || index < 0)
   throw (Exception *)
   new ArrayOutOfBoundsException("Array::setElementAt()",
   index);
  
  // We go ahead and removeElementAt index here
  removeElementAt(index);
  
  elementData[index] = new T(obj);
  elementCount++;
 }
 
 /*
 ** removeElementAt(int)
 ** Deletes the element at the specified index.
 ** The object at index is explicitly destroyed.
 ** @param index the element to remove
 ** @exception ArrayIndexOutOfBoundsException If the index was invalid.
 **/
 void removeElementAt(int index) 
 {
  if (index >= arrayCapacity || index < 0) 
   throw (Exception *)
   new ArrayOutOfBoundsException("Array::removeElementAt()",
   index);
  
  if (elementData[index])
  {
   delete elementData[index];
   elementData[index] = NULL;
   elementCount--;
  }
 }
 
 /*
 ** removeElement(const T &)
 ** Removes the element from the array. If the object occurs more
 ** than once, only the first is removed. If the object is not an
 ** element, returns false.
 ** @param obj the element to be removed
 ** @return true if the element was actually removed; false otherwise.
 */
 int removeElement(const T & obj)
 {
  int i = indexOf(obj);
  if (i >= 0) 
  {
   removeElementAt(i);
   return true;
  }
  return false;
 }
 
 /*
 ** removeAllElements()
 ** Removes all elements of the array. The array becomes empty.
 ** Explicitly deletes each element as well.
 */
 void removeAllElements() 
 {
  for (int i = 0; i < arrayCapacity; i++)
  {
   if (elementData[i])
    delete elementData[i];
   elementData[i] = NULL;
  }
  
  elementCount = 0;
 }
 
 /*
 ** appendElement(const T & item)
 ** appends item to the array.  ALWAYS puts it on the end,
 ** array is sized to arrayCapacity+1
 ** @param item the element to be added
 ** @exception (propogated from setSize) if the array cannot be resized
 */
 void appendElement(const T &item)
 {
  int oldCapacity = arrayCapacity;
  
  setSize(arrayCapacity + 1);
  
  if (arrayCapacity <= 0) 
  {
   cout << "Panic: Could not allocate space for array" << endl;
   assert(0);
  }
  
  elementData[oldCapacity] = new T(item);
  elementCount++;
 }
 
 /*
 ** insertElement(const T &item)
 ** inserts an element into the first available slot.  if the array
 ** is full, call appendElement
 ** @param item the item to add
 ** @return index of the slot it was added into
 ** @exception (propogated from appendElement) if the array cannot be
 **            resized and it is full
 */
 int insertElement(const T &item)
 {
  int index;
  
  for (index = 0; index < arrayCapacity; index++)
   if (!elementData[index])
    break;
   
   if (index == arrayCapacity)
    appendElement(item);
   else {
    elementData[index] = new T(item);
    elementCount++;
   }
   return index;
 }
 
 friend ostream & operator<< (ostream &out, Array<T> &t)
 {
  out << "Array: elementCount = " << t.elementCount << endl
   << "       arrayCapacity = " << t.arrayCapacity << endl;
  
  for (int i = 0; i < t.arrayCapacity; i++) 
   if (t.elementData[i])
    out << "index: " << i << " elem: " 
    <<  *t.elementData[i] << endl;
   else
    out << "index: " << i << " elem: EMPTY" << endl;
   
   return out;
 }
};

#endif

