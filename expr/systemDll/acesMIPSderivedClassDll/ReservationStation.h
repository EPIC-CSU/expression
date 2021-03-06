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
{\rtf1\ansi\ansicpg1252\deff0\deflang1033{\fonttbl{\f0\fswiss\fcharset0 Arial;}}
\viewkind4\uc1\pard\f0\fs20 template <class T>\par
class ReservationStation : public Storage\{\par
\par
public :\par
  ReservationStation();\par
  ReservationStation(const ReservationStation &);\par
  ~ReservationStation();\par
\par
  virtual Storage * copy()\par
\{\par
\tab\par
\}\par
  virtual int is(StorageClassType ctype);\par
  friend int operator ==(const Latch &a, const Latch &b);\par
  virtual void print(ostream &out);\par
  virtual void name(ostream &out);\par
  virtual int isEq(Storage * ptr);\par
  Latch& operator=(const Latch &a);\par
  virtual void read(BaseType& t);\par
  virtual void write(BaseType& t);\par
  virtual void doUpdate() = 0;\par
\par
  virtual void reset()\{\}\par
  virtual void resetOld()\{\}\par
  virtual void resetNew()\{\}\par
  virtual void addLatch(Latch *newLatch)\par
  \{\par
\tab   // error condition.\par
\tab   cout << "Panic: Base Class method (Latch::addLatch) should not be called." << endl;\par
\tab   assert(0);\par
  \}\par
\par
  virtual BaseType *probeOld()\par
  \{\par
\tab   // error condition.\par
\tab   //\par
\tab   cout << "Panic: Base Class method (Latch::probeOld) should not be called." << endl;\par
\tab   assert(0);\par
\par
\tab   BaseType *ptr;\par
\tab   return ptr;\par
  \}\par
\par
  virtual BaseType *probeNew()\par
  \{\par
\tab   // error condition.\par
\tab   //\par
\tab   cout << "Panic: Base Class method (Latch::probeNew) should not be called." << endl;\par
\tab   assert(0);\par
\par
\tab   BaseType *ptr;\par
\tab   return ptr;\par
  \}\par
\};\par
}
 