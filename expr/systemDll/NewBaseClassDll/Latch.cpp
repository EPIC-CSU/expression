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
 *file  : Latch.cpp                                          *
 *authors : S. Ashok Halambi,vgee                            *
 *last modified :                                            *
 *               1st May,1998.                               *
 *                                                           *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/
#include "stdafx.h"

#include "Latch.h"
#include "CompName.h"
 
//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

/*************************************************************
 *Latch class                                                *
 *************************************************************/
Latch::Latch(){}
Latch::Latch(const Latch &){}
Latch::~Latch(){}
 
Storage * Latch::copy()
{
 return this;
}
 
int Latch::is(StorageClassType ctype)
{
  return ((ctype == _LATCH_) ? _YES_ : Storage::is(ctype));
}
 
int operator ==(const Latch &a, const Latch &b)
{
  return _NO_;
}
 
void Latch::print(ostream &out)
{
}
 
void Latch::name(ostream &out)
{
	printName(out, this);
}

int Latch::isEq(Storage * ptr)
{
  return _NO_;
}
 
Latch& Latch::operator=(const Latch &a)
{
  return *this;
}
 
void Latch::read(BaseType & t)
{
    // Error: this function should never be called.
    cout << "Error: Base class method (Latch::read) should not be invoked" << endl;
    return;
}
 
void Latch::write(BaseType& t) {}
/*************************************************************
 *SimpleLatch class                                          *
 *************************************************************/
// functions for SimpleLatch are defined in the Latch.h file because
// microsoft Visual c++ requires template classes to be completely
// specified in the .h file 
 
/*************************************************************
 *CompoundLatch class                                        *
 *************************************************************/
CompoundLatch::CompoundLatch(){}
CompoundLatch::CompoundLatch(const CompoundLatch& a){}
CompoundLatch::~CompoundLatch(){}
 
 
Storage * CompoundLatch::copy()
{
  CompoundLatch * retPtr;
  retPtr = new CompoundLatch(*this);
  return retPtr;
}
 
int CompoundLatch::is(StorageClassType ctype)
{
  return((ctype == _COMPOUNDLATCH_)? _YES_ : Latch::is(ctype));
}
 
int operator ==(const CompoundLatch &a,const CompoundLatch &b)
{
  return _NO_;
}
 
void CompoundLatch::print(ostream &out){}
 
int CompoundLatch::isEq(Storage * ptr)
{
  return _NO_;
}
 
CompoundLatch& CompoundLatch::operator=(const CompoundLatch &a)
{
  _subLatches = a._subLatches;
  return *this;
}
 
void CompoundLatch::addLatch(Latch * newsublatch)
{
  _subLatches.push_back(newsublatch);
}
 
void CompoundLatch::deleteLatch(Latch *  newsublatch)
{
  LatchListIterator lIterBegin = 0;
  lIterBegin = _subLatches.begin();
  while(newsublatch != (*lIterBegin))
    {lIterBegin++;}
   (_subLatches).erase(lIterBegin);
}
 
void CompoundLatch::read(BaseType & value,BaseType & latchListIndex)
{
  if(!latchListIndex.is(_LATCHADDR_))
    {
     cout << "Panic. you tried to read CompoundLatch with wrong address." << endl;
     assert(0);
     }
  _subLatches[latchListIndex.convertAddrMechanism()]->read(value);
  return;
}
 
void CompoundLatch::write(BaseType& value,BaseType & latchListIndex)
{
   _subLatches[latchListIndex.convertAddrMechanism()]->write(value);
}
 
void CompoundLatch::doUpdate()
{
   LatchListIterator lIterBegin, lIterEnd;
   lIterBegin = _subLatches.begin();
   lIterEnd = _subLatches.end();
   while (lIterBegin < lIterEnd)
    {
       (*lIterBegin)->doUpdate();
       lIterBegin++;
    }
}

// Each component has a name. Need to store that in a map.

Map<Latch *, MyString> globalLatchNameList;