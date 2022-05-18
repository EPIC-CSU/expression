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
 *file  : Storage.cpp                                        *
 *authors : vgee,ashok				                         *
 *last modified :                                            *
 *                                                           *
 *contents : contains the storage class definitions  and     *
 *           associated classes.                             *
 *compiled with : g++                                        *
 *for more info :                                            *
 *caution : always grep for TODO to check for incompleteness *
 *************************************************************/
#include "stdafx.h"

#include "Storage.h"
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
 *Storage class.                                             *
 *************************************************************/
Storage::Storage(){}
Storage::Storage(const Storage &){}
Storage::~Storage(){}

Storage * Storage::copy()
{
  return this;

}  

void Storage::name(ostream & out) { printName(out, this); }

int Storage::is(StorageClassType ctype)
{ 
  return ((ctype == _STORAGE_) ? _YES_ : _NO_);
}

int operator ==(const Storage &a, const Storage & b){return _NO_;}
int Storage::isEq(Storage * ptr){return _NO_;}
Storage& Storage::operator=(const Storage &a){return *this;}

void Storage::addPort(Port * port)
{
  _ports.push_back(port);
}
void Storage::read() {}
void Storage::read(BaseType &) {}
void Storage::read(BaseType &, BaseType &) {}
void Storage::read(BaseType &, BaseType &, SignalClass & latency) {}
void Storage::read(BaseType &, BaseType &, SignalClass & latency, int memType) {}

void Storage::write(BaseType &){}
void Storage::write(BaseType &, BaseType &){} 
void Storage::write(BaseType &, BaseType &, SignalClass & latency){}  

void Storage::initialize(BaseType &){}
void Storage::initialize(BaseType &, BaseType &){} 

// Each component has a name. Need to store that in a map.

Map<Storage *, MyString> globalStorageNameList;