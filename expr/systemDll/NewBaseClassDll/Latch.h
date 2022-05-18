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
 *file  : latch.h                                            *
 *authors : ashok,vgee		                                 *
 *last modified :                                            *
 *                                                           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#ifndef _LATCH_H_
#define _LATCH_H_

#include "ArchClassIncludes.h"
#include "AddrData.h"
#include "Storage.h"
#include "LatchData.h"

/*************************************************************
 *class : Latch                                              *
 *description : The latch class is defined to take care of   *
 *              latches between various units in the procesor*
 *              latches conceptually are the "transit" point *
 *              for any data when it get transfered from one *
 *              unit to another.                             *
 *                                                           *
 *Methods :      doUpdate() updates the old values with new  *
 *Data Members :                                             *
 *ToDo List : isEq() has to be redefined. confirm if portlist*
 *            is needed for storage.                         *              
 *************************************************************/
class Latch : public Storage{

public :
  Latch();
  Latch(const Latch &);
  ~Latch();

  virtual Storage * copy();
  virtual int is(StorageClassType ctype);
  friend int operator ==(const Latch &a, const Latch &b);
  virtual void print(ostream &out);
  virtual void name(ostream &out);
  virtual int isEq(Storage * ptr);
  Latch& operator=(const Latch &a);
  virtual void read(BaseType& t);
  virtual void write(BaseType& t);
  virtual void doUpdate() = 0;

  virtual void reset(){}
  virtual void resetOld(){}
  virtual void resetNew(){}
  virtual void addLatch(Latch *newLatch)
  {
	  // error condition.
	  cout << "Panic: Base Class method (Latch::addLatch) should not be called." << endl;
	  assert(0);
  }

  virtual BaseType *probeOld()
  {
	  // error condition.
	  //
	  cout << "Panic: Base Class method (Latch::probeOld) should not be called." << endl;
	  assert(0);

	  BaseType *ptr;
	  return ptr;
  }

  virtual BaseType *probeNew()
  {
	  // error condition.
	  //
	  cout << "Panic: Base Class method (Latch::probeNew) should not be called." << endl;
	  assert(0);

	  BaseType *ptr;
	  return ptr;
  }
};


typedef Vector<Latch *> LatchList;
typedef Vector<Latch *>::iterator LatchListIterator;


/*************************************************************
 *class : SimpleLatch                                        *
 *description : derived from latch contains a pair of data of*
 *              type T. each sublatch has just a single data *
 *              restrict datatype T to BaseType.             *
 *                                                           *
 *Methods :      doUpdate() updates the old values with new  *
 *Data Members :                                             *
 *ToDo List : isEq() has to be redefined. confirm if portlist*
 *            is needed for storage.                         *               
 *************************************************************/
template
<class T>
class SimpleLatch : public Latch
{
  protected:
  T _old;
  T _new;
  public:
  SimpleLatch() {}
  ~SimpleLatch() {}
  SimpleLatch(const SimpleLatch<T>& a)
  {
  _old = a._old;
  _new = a._new;
  }

  virtual Storage * copy()
  {
  Storage * retPtr;
  retPtr = new SimpleLatch<T>(*this);
  return retPtr;
  }
 
  virtual int is(StorageClassType ctype)
  {
	  if (ctype == _SIMPLELATCH_) return _YES_;
	  return (Latch::is(ctype));
  }

  friend int operator ==(const SimpleLatch<T> &a, const SimpleLatch<T> &b) { return _NO_;}

  virtual void print(ostream &out) {}

  virtual int isEq(Storage * ptr)
  {
   return _NO_;
  }//TODO
 
  SimpleLatch<T>& operator=(const SimpleLatch<T> &a)
  {
  _old = a._old;
  _new = a._new;
  return *this;
  }
 
  virtual void reset(){ _old.reset(); _new.reset();}
  virtual void resetOld(){ _old.reset();}
  virtual void resetNew(){ _new.reset();}
  virtual void read(BaseType& t) { (T&)t = _old; }
  virtual void write(BaseType& t){ _new = (T &)t ;}
  virtual void doUpdate() 
  {
	  _old = _new;
  }

  virtual BaseType *probeOld()
  {
	  return (BaseType *)(&_old);
  }

  virtual BaseType *probeNew()
  {
	  return (BaseType *)(&_new);
  }
};

/*************************************************************
 *class : CompoundLatch                                      *
 *description : CompoundLatch is  collection of simplelatches*
 *              latchListIndex is used to iterate thru' the  *
 *              latchlist                                    *
 *since it is too cumbersome to create new copies of all the *
 *sublatches in the latchlist, the copy constructor creates  *
 *an empty latch. thanks.                                    *
 *                                                           *
 *Methods :      doUpdate() updates the old values with new  *
 *Data Members :                                             *
 *ToDo List : isEq() has to be redefined. confirm if portlist*
 *            is needed for storage.                         *                
 *************************************************************/
//TODO. compound latch should be collection of simple latches.
//where the heck is old and new values stored.
class CompoundLatch : public Latch
{
  protected:
  LatchList _subLatches;
  public:
  CompoundLatch();
  ~CompoundLatch();
  CompoundLatch(const CompoundLatch& a);

  virtual Storage * copy();
  virtual int is(StorageClassType ctype);
  friend int operator ==(const CompoundLatch &a,const CompoundLatch &b);

  virtual void print(ostream &out);
  virtual int isEq(Storage * ptr);
  CompoundLatch& operator=(const CompoundLatch &a);
  virtual void addLatch(Latch * newsublatch);

  virtual void deleteLatch(Latch *  newsublatch);
  virtual void read(BaseType& value,BaseType & latchListIndex);
  virtual void write(BaseType& value,BaseType & latchListIndex);
  virtual void doUpdate();
};
#endif

















