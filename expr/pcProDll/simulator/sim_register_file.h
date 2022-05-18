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
#ifndef _SIM_REGISTER_FILE_H
#define _SIM_REGISTER_FILE_H

#include <map>
#ifdef WIN32
#define map std::map
#endif

#include "ltstr.h"
#include "DataValue.h"


class sim_register_file  {
	map<char *, val , ltstr > _reg_file;
public:
  sim_register_file();
  ~sim_register_file();
  void init(int hasSSA);
  void clear();
  int containReg(char *);
  void write(lhsReg& v);
  void read(lhsReg& v);
  void write(char *regName,val& v);
  void read(char *regName,val& v);
  void zero();
  void dump(ostream&);
};

#endif
