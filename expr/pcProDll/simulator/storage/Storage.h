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
#ifndef _STORAGE_H_
#define _STORAGE_H_
 


class Storage {

public:
  Storage(void) {};
  virtual int isLatch() { return 0; }
  virtual int isRegFile() { return 0;}
  virtual int isMem() { return 0; }
  virtual void doUpdate() = 0;
};

class RegFile : public Storage {
  Array<
public:
  RegFile(  );
  ~RegFile();
  int isRegFile() { return 1; }
  void read(  );
  void write(  );
  void doUpdate();
};

class Memory : public Storage {
  int _buf_size,_buf_ptr;
  lhsMem *_bufs;
  sim_mem *_mem;
public:
  Memory(sim_mem *mem, int buff_size = 4);
  ~Memory();
  int isMem() { return 1; }
  void read(lhsMem&);
  void write(lhsMem&);
  void doUpdate();
};

#endif
