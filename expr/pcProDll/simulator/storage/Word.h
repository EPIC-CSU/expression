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
#ifndef _WORD_H_
#define _WORD_H_

class Word {
 public:
  Word(void);
  virtual int isBit(void){return 0;}
  virtual int isByte(void){return 0;}
  virtual int isDoubleByte(void){return 0;}
  virtual int isFourByte(void){return 0;}
};


class Bit: public Word{
  short _data;
 public:
  Bit(void)_data(0),Word(){}
  Bit(short d):_data(d),Word(){}
  short GetData(void){return _data;}
  void SetData(short d){_data=d;}
  virtual int isBit(void){return 1;}
};

class Byte: public Word{
  short _data;
 public:
  Byte(void)_data(0),Word(){}
  Byte(short d):_data(d),Word(){}
  short GetData(void){return _data;}
  void SetData(short d){_data=d;}
  int isByte(void){return 1;}
};

class DoubleByte: public Word{
  int _data;
 public:
  DoubleByte(void)_data(0),Word(){}
  DoubleByte(int d):_data(d),Word(){}
  int GetData(void){return _data;}
  void SetData(int d){_data=d;}
  int isDoubleByte(void){return 1;}
};

class FourByte: public Word{
  long _data;
 public:
  FourByte(void)_data(0),Word(){}
  FourByte(long d):_data(d),Word(){}
  long GetData(void){return _data;}
  void SetData(long d){_data=d;}
  int isFourByte(void){return 1;}
};

#endif
