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
#include "stdafx.h"

#include "simulator.h"
// extern void outMessage(char *);

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

void simulator::load_program(Program *P)
{
  init();
}

void simulator::load_extra_files()
{
/*
  extern char **extra_sim_files;
  int i;
  for(i=0; extra_sim_files[i]; i++)
  {
     extern Regex RXsuffix;
     String name(extra_sim_files[i]);
     name = name.before(RXsuffix) + ".before";
     load_file(name.chars());
  }

  cout << "running baseline simulation of '.before' files\n";
*/
}

void simulator::print_program(ostream& out)
{
  assert(0);
}

void simulator::init_builtins()
{
  assert(0);
}

void simulator::init_argv()
{
/*
  extern char **sim_args;
  extern int n_sim_args;

  _argc = n_sim_args;
  _argv_base = copy_strings_to_stack( sim_args );
  lhsReg v1("R4.0", val(_argc));
  write_to_register_file(v1);
  lhsReg v2("R5.0", val(_argv_base));
  write_to_register_file(v2);
*/
}

void simulator::print_stats(ostream& out)
{
  assert(0);
}

void simulator::print_profile(ostream& out)
{
  assert(0);
}

void simulator::breakpoint()
{
  assert(0);
}

/*
void simulator::read_string(int addr, String& S)
{
  val v;
  while(1) {
     read_from_storage(addr++, BYTE, v);
     if (! v.as.byte) return;
     S += v.as.byte;
  }
}

void simulator::write_string(int addr, String& S)
{
  const char *s = S;
  while(*s) {
   lhsMem v( addr++, val(*s++));
   write_to_memory(v);
  }
  lhsMem v(addr++, val((char)0));
  write_to_memory(v);
}


void simulator::show_mem_string(int addr)
{ 
  String S;

  read_string(addr, S);
  printf("String @ %d = %s\n", addr, (const char *)S);
}

int simulator::copy_strings_to_stack(char **strings)
{
  vector<int> sptrs;

  int sp = eval_arg("sp").as.integer;

  if (strings) {
    for(; *strings; strings ++) {
      String S(*strings);
      sp -= 1 + S.length();
      sptrs.push_back(sp);
      write_string(sp, S);
   }
  }
  sptrs.push_back(0); // null terminate
  sp -= sptrs.size() * sizeof(int);
  int p = sp;
  for (vector<int>::iterator iter=sptrs.begin(); iter!=sptrs.end(); iter++) {
    lhsMem v( p, val(*iter));
    write_to_memory(v);
    p += sizeof(int);
  }
  lhsReg v("sp", val(sp));
  write_to_register_file(v);
  return sp;
}
*/

