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

#include "sim_register_file.h"
#include "SymbolTable.h"

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

extern RegisterFileList *globalRegFileList;
extern SymbolTable *globalSymbolTable;

sim_register_file *globalSimRegFile;

sim_register_file::sim_register_file() 
{
}

sim_register_file::~sim_register_file()
{
  clear();
}

void sim_register_file::init(int hasSSA)
{
  ArrayIterator<RegisterFile> *iter = globalRegFileList->getRegFileListIter();

  while (iter->hasMoreElements()) {
    RegisterFile & regFile = iter->currentElement();
    char *regFileName = (char *)regFile.name();
    int size = regFile.size();
    
    val temp;

    if (regFileName[0]=='R')
      temp.val_type = _INT;
    else if (regFileName[0]=='C')
      temp.val_type = _INT;
    else if (regFileName[0]=='F')
      temp.val_type = _DBL;
    else 
      temp.val_type = _ANY;

    for (int i=0; i<size; i++) {
      if (hasSSA) {
        int numOfSSA = globalSymbolTable->getSSANum(i,regFileName);
        for (int j=1; j<=numOfSSA; j++) {
          strstream ssaRegName;
	  char *tStr;
          ssaRegName<<regFileName<<i<<"."<<j<<ends;
	  tStr = ssaRegName.str();
          _reg_file[strdup(tStr)] = temp;
	  delete []tStr;
        }
      }
      else {
	strstream regName;
	char *tStr;
	regName<<regFileName<<i<<ends;
	tStr = regName.str();
	_reg_file[strdup(tStr)] = temp;
	delete []tStr;
      }
    }
    
    iter->nextElement();
  }

  delete iter;
}

void sim_register_file::clear()
{
  map<char *, val , ltstr >::iterator iter;

  for ( iter = _reg_file.begin(); iter!=_reg_file.end(); iter++)
    free((*iter).first);
  _reg_file.erase(_reg_file.begin(),_reg_file.end());
}

void sim_register_file::write(lhsReg& v)
{
  write(v._regName, v._value);
}

void sim_register_file::write(char *name, val& v)
{
  map<char *,val , ltstr>::iterator iter;
  iter = _reg_file.find(name); 
  if (iter==_reg_file.end())  //not in the register file yet, report error
    assert(0);
  else  //this register is already in the register file
    (*iter).second = v;
}

void sim_register_file::read(lhsReg& v)
{
  read(v._regName, v._value);
}

void sim_register_file::read(char *name, val& v)
{
  map<char *,val , ltstr>::iterator iter;
  iter = _reg_file.find(name); 
  if (iter==_reg_file.end())  //report error
    assert(0);
  else  //register exists in the register file
    v = ((*iter).second);
}

int sim_register_file::containReg(char *name)
{
  map<char *,val , ltstr>::iterator iter;
  iter = _reg_file.find(name); 
  if (iter==_reg_file.end())  
    return 0;
  else 
    return 1;
}

void sim_register_file::zero()
{
  for (map<char *, val, ltstr>::iterator iter = _reg_file.begin();
	iter!=_reg_file.end(); iter++) 
     (*iter).second = val((int)0);	
}

void sim_register_file::dump(ostream& dumps)
{
  int i=0;
  for (map<char *,val,ltstr>::iterator iter = _reg_file.begin();
	iter!=_reg_file.end(); iter++) {
    dumps.width(6);
    dumps<<(*iter).first;
    dumps<<" = ";
    dumps.width(10);
    ((*iter).second).print(dumps);
    dumps<<"   ";
    if (++i%4==0)
      dumps<<endl;
  }
}

