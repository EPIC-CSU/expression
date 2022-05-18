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

#include "SpecialInclude.h"
#include "simulator.h"

#ifdef COMPILE_WITH_GUI
#include "simulatorWin.h"
#endif

#ifdef WIN32
#include <strstrea.h>
#endif

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

extern int yyparse();
extern void printError(char *);

extern Program *curProgram;
extern sim_register_file *globalSimRegFile;
extern sim_mem *globalSimMem;

simulator *simulator::current = NULL;

//used as globals, to pass parameters to instruction semantics
val src1Val, src2Val, destVal;

FILE *change_parse_file(char * newname)
{
  char *parse_file_name;

  extern int yylineno;

  yylineno = 1;

  parse_file_name = newname;

  FILE *pf = fopen(parse_file_name, "r");
  if (pf == NULL)
  {
     panic("change_parse_file:  file not found: %s\n",
               (const char *)parse_file_name);
  }

  extern void yyrestart( FILE *input_file );
  yyrestart( pf );

  return pf;
}

void clearRegFileList(vector <lhsReg *>& rList)
{
  vector<lhsReg *>::iterator iter;
//char * is private owned, so must be freed
  for (iter=rList.begin(); iter!=rList.end(); iter++)
    delete (*iter);
  rList.erase(rList.begin(),rList.end());
}

vector<lhsReg *>::iterator findRegPos(vector<lhsReg *>& regSet, char *regName )
{
  vector<lhsReg *>::iterator iter;
  for (iter=regSet.begin(); iter!=regSet.end(); iter++) 
    if (strcmp(regName,(*iter)->_regName)==0)
      return iter;
  return regSet.end();
}

void copyRegFileList(vector <lhsReg *>& src, vector <lhsReg *>& dest)
{
  clearRegFileList(dest);
  vector<lhsReg * >::iterator iter;
//char * is private owned, so must be freed
  for (iter=src.begin(); iter!=src.end(); iter++) 
    dest.push_back(new lhsReg(*(*iter)));
}

//if r!=NULL, we are simulating for a routine, otherwise we are simulating for
//the program, this will change the way to do initialization
simulator::simulator(Routine *r)
{
  _routine = r;

  _argc = 0;
  _argv_base = 0;
  _first_pass_cycle = 0;
  _second_pass_cycle = 0;
  _extra_files_loaded = 0;
  _spName = NULL;
  _pass_num = 1;
  _sameRegInit = 1;
  _sameRegComp = 1;

  setMemHierarchy(0);
}

simulator::~simulator()
{
  delete _cache;

  clearRegFileList(_fp_reg_comp_list);
  clearRegFileList(_sp_reg_comp_list);
  clearRegFileList(_fp_reg_init_list);
  clearRegFileList(_sp_reg_init_list);
}

void simulator::setMemHierarchy(int e_cache)
{
  _enable_cache = e_cache;

/*following lines is to set memory module, it should be separated from 
simulator later, to set up it in other place and pass the pointer to 
the simulator*/

  if (e_cache)
    _cache = new sim_cache();

}

void simulator::clear_program()
{
  _register_file->clear();

  if (_spName)
    delete _spName;
  _spName = NULL;
}

int simulator::init()
{
  _mem = globalSimMem;
  _register_file = globalSimRegFile;

  _enable_break = 0;
  _mem->zero();
  initMemory();
  initReg();

  if (!_extra_files_loaded) {
    load_extra_files();
    _extra_files_loaded = 1;
  }


#ifdef COMPILE_WITH_GUI
  Routine *workR;

  if (_routine!=NULL) 
    workR = _routine;
  else {
    workR = curProgram->getRoutine("_main");
    if (workR==NULL){ 
#ifndef WIN32
      simulatorWin::current->outMessage("Can't find routine _main, aborted!\n");
#endif
      return 0;
    }
  }

#endif

  _run_status = RUNNING;
/*disable for memory check
  CompilerRTStatus& status = workR->getStatusObj();
  if (status.numTimesCalled("BUILD_CFG"))
    _program_status = BEFORE_SSA;
  else 
    return 0; //abort simulation, since IR is not ready
  if (status.numTimesCalled("BUILD_SSA"))
    _program_status = BEFORE_SCHEDULING;
//conditions for AFTER_SCHEDULING has to be determined
*/

  int hasSSA = 0;
  _register_file->init(hasSSA);
  _register_file->zero();

  return 1;
}

void simulator::initMemory() 
{
  for (map<int, val, less< int > >::iterator iter=_mem_init_list.begin(); 
      iter!=_mem_init_list.end(); iter++) 
    _mem->write((*iter).first, (*iter).second);
}

void simulator::initReg() 
{
  if (_pass_num==2&&_sameRegInit)
      copyRegFileList(simulator::current->_fp_reg_init_list,
        simulator::current->_sp_reg_init_list);

  if (_pass_num==1) { 
    for (vector<lhsReg *>::iterator iter=_fp_reg_init_list.begin(); 
      iter!=_fp_reg_init_list.end(); iter++) 
        _register_file->write(*(*iter));
  }
  else {
    for (vector<lhsReg *>::iterator iter=_sp_reg_init_list.begin(); 
      iter!=_sp_reg_init_list.end(); iter++) 
        _register_file->write(*(*iter));
  }
}

int simulator::exec()
{
  return 0;
/*
        decl_class *d = labels.lookup("_main");

        int pc;
        if (d)
           pc = d->value;
        else
        {
           cerr << "_main undefined!";
           _run_status = IDLE;
           return 0;
        }

        init_argv();

        extern char **environ;

        int env_addr = copy_strings_to_stack( environ );

        decls.begin_collection();
           initializer *init = new initializer();
           strstream s;
           s << env_addr << ends;
	   char *tStr = s.str();
           *init += new word_init(tStr);
	   delete []tStr;

           new decl_class(1, "_environ", 2, init);

           decls.init_sim_memory(*this);
        decls.end_collection();

        exec_count.zero();

        int cycles = 0;
        while( ( _run_status==RUNNING && (pc >= 0) && (pc < instrs.size()) )
        {
           if (pc == pc_break)
              breakpoint();

           exec_count[pc] ++;

           pc = instrs[pc]->exec(cycles);
        }
        return cycles;
*/
}

int simulator::load_file(const char *name)
{
  if (! change_parse_file((char *)name))
  {
     warning("simulator::load_file: file not found %s\n", name);
     return 0;
  }
  cout << "loading " << name << endl;
  yyparse();
  return 1;
}

float simulator::speedup() 
{ 
  return (float) _first_pass_cycle / _second_pass_cycle;
}

int simulator::write_to_memory(lhsMem& v) 
{ 
    if (_mem->legal_addr(v._addr)) {
      if ( auto_initializing)
        _mem->touch(v._addr, v._value.val_type);
    }
    if (_enable_cache) 
      return _cache->write(v);
    else
      return _mem->write(v);
}

int simulator::write_to_register_file(lhsReg& v) 
{ 
  _register_file->write(v);
//assume 1 for register access
  return 1;
}

int simulator::read_mem(int addr, VALUE_TYPE mode, val& v)
{ 
  if (  auto_initializing &&  _mem->legal_addr(addr)) {
    if ((mode == _DBL || mode == _FLT) &&  (! _mem->touched(addr, mode))) {
      val rand_val(( 100.0 * (float)(rand()&0x7fff) )/32767.0 - 50.0) ;
      lhsMem *init = new lhsMem( addr, rand_val);
      init->_value.val_type = mode;
      _mem->write(*init);
      delete init;
     }
     _mem->touch(addr, mode);
  }
  _mem->read(addr, mode, v);
  return _mem->get_read_latency();
}

int simulator::read_from_storage(int addr, VALUE_TYPE mode, val& v)
{
  int latency;
  if (_enable_cache)
    latency = read_cache(addr, mode, v);
  else
    latency = read_mem(addr,mode,v);
  return latency;
}

int simulator::read_cache(int addr, VALUE_TYPE mode, val& v)
{
  int latency = _cache->read(addr,mode, v);
  return latency;
}

val simulator::eval_arg(BaseArgument *arg)
{
  if (arg->isRealRegArg()) {
    val v;
    strstream regName;
    char *tStr;
    char *RFName = (char *)(((RealRegArg *)arg)->regFilename());
    int regNum = ((RealRegArg *)arg)->regNumber();
    BaseArgProperty& ssaPro = arg->getProperty(_SSA_);
    int ssaNum = ((SSAArgProperty *)(&ssaPro))->getSSANum();
    regName<<RFName<<regNum<<"."<<ssaNum<<ends;
    tStr = regName.str();
    _register_file->read(tStr, v);
    delete []tStr;
    return v;
  }
  else if (arg->isIConstArg())
    return val(((IConstArg *)arg)->value());
  else if (arg->isFConstArg()){
    return val(((FConstArg *)arg)->value());
  }
  else {
  //don't know how to simulator for other type of register yet
    assert(0);
  }
  //NOS
  ASSERT_TRACE(0,"Forced EXIT!");
  return NULL;
}

val simulator::eval_arg(char *regName )
{
  val v;
  _register_file->read(regName, v);
  return v;
}

void simulator::run()
{
  assert(0);
}

void simulator::addMemInit(lhsMem v)
{
  map<int, val, less< int > >::iterator iter;
  iter = _mem_init_list.find(v._addr);
  if (iter==_mem_init_list.end())
    _mem_init_list[v._addr] = v._value;
  else
    (*iter).second = v._value;
}

int simulator::removeMemInit(int addr)
{
  map<int, val, less< int > >::iterator iter;
  iter = _mem_init_list.find(addr);
  if (iter!=_mem_init_list.end()) {
    _mem_init_list.erase(iter);
    return 1;
  }
  else
    return 0;
}

void simulator::removeAllMemInit()
{
  _mem_init_list.erase(_mem_init_list.begin(),_mem_init_list.end());
}

void simulator::addRegInit(lhsReg v)
{
  vector<lhsReg *>::iterator iter;
  if (_pass_num==1) {
    iter = findRegPos(_fp_reg_init_list,v._regName);
    if (iter==_fp_reg_init_list.end())
      _fp_reg_init_list.push_back(new lhsReg(v));
    else
      (*iter)->_value = v._value;
  }
  else if (_pass_num==2) {
    iter = findRegPos(_sp_reg_init_list,v._regName);
    if (iter==_sp_reg_init_list.end())
      _sp_reg_init_list.push_back(new lhsReg(v));
    else
      (*iter)->_value = v._value;
  }
  else
    assert(0);
}

int simulator::removeRegInit(char *regName)
{
  vector<lhsReg *>::iterator iter;
  if (_pass_num==1) {
    iter = findRegPos(_fp_reg_init_list,regName);
    if (iter!=_fp_reg_init_list.end()) {
      delete (*iter);
      _fp_reg_init_list.erase(iter);
      return 1;
    }
    else
      return 0;
  }
  else if (_pass_num==2) {
    iter = findRegPos(_sp_reg_init_list,regName);
    if (iter!=_sp_reg_init_list.end()){
      delete (*iter);
      _sp_reg_init_list.erase(iter);
      return 1;
    }
    else
      return 0;
  }
  else
    assert(0);
  //NOS
  ASSERT_TRACE(0,"Forced EXIT!");
  return 0;
}

void simulator::removeAllRegInit()
{
  vector<lhsReg * >::iterator iter;
  if (_pass_num==1){ 
    for (iter=_fp_reg_init_list.begin();iter!=_fp_reg_init_list.end();iter++)
      delete *iter;
    _fp_reg_init_list.erase(_fp_reg_init_list.begin(),_fp_reg_init_list.end());
  }
  else if (_pass_num==2) {
    for (iter=_sp_reg_init_list.begin();iter!=_sp_reg_init_list.end();iter++)
      delete *iter;
    _sp_reg_init_list.erase(_sp_reg_init_list.begin(),_sp_reg_init_list.end());
  }
  else
    assert(0);
}

void simulator::addMemComp(int addr)
{
  val dummy;

  map<int, val, less< int > >::iterator iter;
  iter = _mem_compare_list.find(addr);
  if (iter==_mem_compare_list.end())
    _mem_compare_list[addr] = dummy;
}

int simulator::removeMemComp(int addr)
{
  map<int, val, less< int > >::iterator iter;
  iter = _mem_compare_list.find(addr);
  if (iter!=_mem_compare_list.end()) {
    _mem_compare_list.erase(iter);
    return 1;
  }
  else
    return 0;
}

void simulator::removeAllMemComp()
{
  _mem_compare_list.erase(_mem_compare_list.begin(),_mem_compare_list.end());
}

void simulator::addRegComp(char *regName)
{
  val dummy;

  vector<lhsReg *>::iterator iter;
  if (_pass_num==1) {
    iter = findRegPos(_fp_reg_comp_list,regName);
    if (iter==_fp_reg_comp_list.end())
      _fp_reg_comp_list.push_back(new lhsReg(regName,dummy));
  }
  else if (_pass_num==2) {
    iter = findRegPos(_sp_reg_comp_list,regName);
    if (iter==_sp_reg_comp_list.end())
      _sp_reg_comp_list.push_back(new lhsReg(regName,dummy));
  }
  else
    assert(0);
}

int simulator::removeRegComp(char *regName)
{
  vector<lhsReg *>::iterator iter;
  if (_pass_num==1) {
    iter = findRegPos(_fp_reg_comp_list,regName);
    if (iter!=_fp_reg_comp_list.end()){
      delete *iter;
      _fp_reg_comp_list.erase(iter);
      return 1;
    }
    else
      return 0;
  }
  else if (_pass_num==2) {
    iter = findRegPos(_sp_reg_comp_list,regName);
    if (iter!=_sp_reg_comp_list.end()){
      delete (*iter);
      _sp_reg_comp_list.erase(iter);
      return 1;
    }
    else
      return 0;
  }
  else
    assert(0);
  //NOS
  ASSERT_TRACE(0,"Forced EXIT!");
  return 0;
}

void simulator::removeAllRegComp()
{
  vector<lhsReg * >::iterator iter;
  if (_pass_num==1) { 
    for (iter=_fp_reg_comp_list.begin(); iter!=_fp_reg_comp_list.end(); iter++)
      delete (*iter);
    _fp_reg_comp_list.erase(_fp_reg_comp_list.begin(),_fp_reg_comp_list.end());
  }
  else if (_pass_num==2) {
    for (iter=_sp_reg_comp_list.begin(); iter!=_sp_reg_comp_list.end(); iter++)
      delete (*iter);
    _sp_reg_comp_list.erase(_sp_reg_comp_list.begin(),_sp_reg_comp_list.end());
  }
  else
    assert(0);
}

void simulator::zeroCompMem()
{
  map<int, val, less< int > >::iterator iter;
  for (iter=_mem_compare_list.begin(); iter!=_mem_compare_list.end(); iter++)
    (*iter).second.zero();
}

void simulator::zeroCompReg()
{
  vector<lhsReg * >::iterator iter;
  if (_pass_num==1) {
    for (iter=_fp_reg_comp_list.begin(); iter!=_fp_reg_comp_list.end(); iter++)
      (*iter)->_value.zero();
  }
  else if(_pass_num==2) {
    for (iter=_sp_reg_comp_list.begin(); iter!=_sp_reg_comp_list.end(); iter++)
      (*iter)->_value.zero();
  }
  else
    assert(0);
}

void simulator::collectCompMem()
{
  map<int, val, less< int > >::iterator iter;
  for (iter=_mem_compare_list.begin();iter!=_mem_compare_list.end();iter++)
    _mem->read((*iter).first,(*iter).second);
}

void simulator::collectCompReg()
{
  vector<lhsReg *>::iterator iter;
  if (_pass_num==1) {
    for (iter=_fp_reg_comp_list.begin();iter!=_fp_reg_comp_list.end();iter++)
      _register_file->read(*(*iter));
  }
  else if (_pass_num==2) {
    for (iter=_sp_reg_comp_list.begin();iter!=_sp_reg_comp_list.end();iter++)
      _register_file->read(*(*iter));
  }
  else
    assert(0);
}

void simulator::dumpMemCompSet(ostream& out)
{
  int i = 0;

  map<int, val, less< int > >::iterator iter;
  for (iter = _mem_init_list.begin(); iter!=_mem_init_list.end(); iter++) {
    out.width(8);
    out<<(*iter).first;
    i++;
    if (i%3==0)
      out<<endl;
  }
}

void simulator::dumpMemInitSet(ostream& out)
{
  int i = 0;

  map<int, val, less< int > >::iterator iter;
  for (iter = _mem_init_list.begin(); iter!=_mem_init_list.end(); iter++) {
    out.width(8);
    out<<(*iter).first;
    out.width(12);
    (*iter).second.printTyped(out);
    i++;
    if (i%3==0)
      out<<endl;
  }
}

void simulator::dumpFpRegInitSet(ostream& out)
{
  int i = 0;

  vector<lhsReg *>::iterator iter;
  for (iter = _fp_reg_init_list.begin(); iter!=_fp_reg_init_list.end();iter++) {
    out.width(8);
    out<<(*iter)->_regName;
    out<<"=";
    out.width(12);
    (*iter)->_value.printTyped(out);
    i++;
    if (i%3==0)
      out<<endl;
  }
}

void simulator::dumpSpRegInitSet(ostream& out)
{
  int i = 0;

  vector<lhsReg *>::iterator iter;
  for (iter = _sp_reg_init_list.begin(); iter!=_sp_reg_init_list.end();iter++) {
    out.width(8);
    out<<(*iter)->_regName;
    (*iter)->_value.printTyped(out);
    out<<"=";
    out.width(12);
    i++;
    if (i%3==0)
      out<<endl;
  }
}

void simulator::dumpFpRegCompSet(ostream& out)
{
  int i = 0;

  vector<lhsReg *>::iterator iter;
  for (iter = _fp_reg_comp_list.begin(); iter!=_fp_reg_comp_list.end();iter++) {
    out.width(8);
    out<<(*iter)->_regName;
    out<<"=";
    out.width(12);
    (*iter)->_value.printTyped(out);
    i++;
    if (i%3==0)
      out<<endl;
  }
}

void simulator::dumpSpRegCompSet(ostream& out)
{
  int i = 0;

  vector<lhsReg *>::iterator iter;
  for (iter = _sp_reg_comp_list.begin(); iter!=_sp_reg_comp_list.end();iter++) {
    out.width(8);
    out<<(*iter)->_regName<<"=";
    out.width(12);
    (*iter)->_value.printTyped(out);
    i++;
    if (i%3==0)
      out<<endl;
  }
}

