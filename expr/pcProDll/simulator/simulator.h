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
#ifndef _SIMULATOR_H
#define _SIMULATOR_H

#include "stdhdr.h"
//#include "String.h"
#include "sim_register_file.h"
#include "sim_mem.h"
#include "sim_cache.h"

#include "BaseArgument.h"
#include "FConstArg.h"
#include "IConstArg.h"
#include "ImmediateArg.h"
#include "LabelArg.h"
#include "LatchRegArg.h"
#include "RegisterArg.h"
#include "RealRegArg.h"
#include "Program.h"
#include "Routine.h"
#include "ComputeOp.h"
#include "MemoryOp.h"

#include <map>
#ifdef WIN32
#include <vector>
#undef map
#undef less
#undef vector
#define map std::map
#define less std::less
#define vector std::vector
#endif

//make it a parameter later
#define auto_initializing 0

class simulator 
{
  Routine *_routine;
  int _enable_cache;	//let us bypass cache in simulatation

//parameters
  int _argc,_argv_base;  

  char *_spName;
  int _extra_files_loaded;

  enum {IDLE, RUNNING, FINISHED}  _run_status;
  enum {BEFORE_SSA, BEFORE_SCHEDULING, AFTER_SCHEDULING} _program_status;
/*
  RESOURCES _resources;
  sim_call_instr_stack call_stack;
  dyn_stack save_stack;
*/

public:
  // stats
  int _first_pass_cycle, _second_pass_cycle;
  int _pass_num;
  int _enable_break;
  int _sameRegInit;
  int _sameRegComp;

  map<int, val, less< int > > _mem_compare_list;
  vector< lhsReg *> _fp_reg_comp_list;
  vector< lhsReg *> _sp_reg_comp_list;
  map<int, val, less< int > > _mem_init_list;
  vector< lhsReg *> _fp_reg_init_list;
  vector< lhsReg *> _sp_reg_init_list;

  sim_mem *_mem;
  sim_cache *_cache;
  sim_register_file *_register_file;

  static simulator *current;

  simulator(Routine *r=NULL);
  ~simulator();
  void setIdle() { _run_status = IDLE; }
  void setFinished() { _run_status = FINISHED; }
  void setRunning() { _run_status = RUNNING; }
  int isRunning() { return _run_status == RUNNING; }
  int beforeSSA() { return _program_status==BEFORE_SSA; }
  int beforeScheduling() { return _program_status==BEFORE_SCHEDULING; }
  int afterScheduling() { return _program_status==AFTER_SCHEDULING; }
  int init();
  void run();
  int exec();
  void setMemHierarchy(int enable_cache);

  void clear_program();
  void load_program(Program *P);
  void load_extra_files();
  int load_file(const char *name);
  void print_program(ostream& out = cout) ;
  void initMemory();
  void initReg();

  void init_builtins();
  void init_argv();

  void print_stats(ostream& out = cout);
  void print_profile(ostream& out = cout);
  void breakpoint();

  float speedup() ;
  val eval_arg(BaseArgument *);
  val eval_arg(char *);

  int write_to_memory(lhsMem& v) ;
  int write_to_register_file(lhsReg& v);
/*
  void read_string(int addr, String& S);
  void write_string(int addr, String& S);
  void show_mem_string(int addr);
  int copy_strings_to_stack(char **strings);
*/
  int read_from_storage(int addr, VALUE_TYPE mode, val&v);
  int read_mem(int addr, VALUE_TYPE mode, val& v);
  int read_cache(int addr, VALUE_TYPE mode, val& v);

  int isFirstPass() { return _pass_num==1; }
  int isSecondPass() { return _pass_num==2; }
  void addMemInit(lhsMem);
  int removeMemInit(int addr);
  void removeAllMemInit();
  void addRegInit(lhsReg);
  int removeRegInit(char *);
  void removeAllRegInit();
  void addMemComp(int addr);
  int removeMemComp(int addr);
  void removeAllMemComp();
  void addRegComp(char * regName);
  int removeRegComp(char *regName);
  void removeAllRegComp();
  void collectCompMem();
  void collectCompReg();
  void zeroCompMem();
  void zeroCompReg();

  void dumpMemCompSet(ostream&);
  void dumpMemInitSet(ostream&);
  void dumpFpRegCompSet(ostream&);
  void dumpSpRegCompSet(ostream&);
  void dumpFpRegInitSet(ostream&);
  void dumpSpRegInitSet(ostream&);
 
  int cache_enabled() { return _enable_cache; }

  Routine *getRoutine() { return _routine; }
  int get_current_cycle() 
  {
	  assert(0); 
	  //NOS
	  return 0;
  }
};

#endif
