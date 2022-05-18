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

#include "Storage.h"
#include "simulatorWin.h"
#include "SpecialInclude.h"

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

RegFile::RegFile(sim_register_file *reg_file, int buff_size)
{
  _reg_file = reg_file;
  _buf_size = buff_size;
  _bufs = new lhsReg[_buf_size];
  _buf_ptr = 0;
}

RegFile::~RegFile()
{
  delete []_bufs;
}

void RegFile::read(lhsReg& v)
{
  _reg_file->read(v);
}

void RegFile::write(lhsReg& v)
{
  if (_buf_ptr<_buf_size)
    _bufs[_buf_ptr++] = v;
  else 

#ifdef COMPILE_WITH_GUI
#ifndef WIN32
    simulatorWin::current->outMessage("Error: exceeding the buffer for register	file!\n")
#endif
#endif
	;
}

void RegFile::doUpdate()
{
  for (int i=0; i<_buf_ptr; i++) 
    _reg_file->write(_bufs[i]);
  _buf_ptr = 0;
}

Memory::Memory(sim_mem *mem, int buff_size)
{
  _mem = mem;
  _buf_size = buff_size;
  _bufs = new lhsMem[_buf_size];
  _buf_ptr = 0;
}

Memory::~Memory()
{
  delete []_bufs;
}

void Memory::read(lhsMem& v)
{
  _mem->read(v);
}

void Memory::write(lhsMem& v)
{
  if (_buf_ptr<_buf_size)
    _bufs[_buf_ptr++] = v;
  else 

#ifdef COMPILE_WITH_GUI
#ifndef WIN32
    simulatorWin::current->outMessage("Error: exceeding the buffer for register	file!\n")
#endif
#endif
	;
}

void Memory::doUpdate()
{
  for (int i=0; i<_buf_ptr; i++) 
    _mem->write(_bufs[i]);
  _buf_ptr = 0;
}

