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

void addName(Unit *comp, MyString & str)
{
	globalUnitNameList[comp] = str;
}

MyString & getName(Unit *comp)
{
	Map<Unit *, MyString>::iterator i;
	if ((i = globalUnitNameList.find(comp)) != globalUnitNameList.end())
		return globalUnitNameList[comp];

	// error condition.
	//
	cout << "Panic: Tried to find the name of a non-existant Unit" << endl;
	assert(0);
	
	static MyString a;
	return a;
}

void addName(Storage *comp, MyString & str)
{
	globalStorageNameList[comp] = str;
}

MyString & getName(Storage *comp)
{
	Map<Storage *, MyString>::iterator i;
	if ((i = globalStorageNameList.find(comp)) != globalStorageNameList.end())
		return globalStorageNameList[comp];

	// error condition.
	//
	cout << "Panic: Tried to find the name of a non-existant Storage" << endl;
	assert(0);
	
	static MyString a;
	return a;
}

void addName(Port *comp, MyString & str)
{
	globalPortNameList[comp] = str;
}

MyString & getName(Port *comp)
{
	Map<Port *, MyString>::iterator i;
	if ((i = globalPortNameList.find(comp)) != globalPortNameList.end())
		return globalPortNameList[comp];

	// error condition.
	//
	cout << "Panic: Tried to find the name of a non-existant Port" << endl;
	assert(0);

	static MyString a;
	return a;
}

void addName(Connection *comp, MyString & str)
{
	globalConnectionNameList[comp] = str;
}

MyString & getName(Connection *comp)
{
	Map<Connection *, MyString>::iterator i;
	if ((i = globalConnectionNameList.find(comp)) != globalConnectionNameList.end())
		return globalConnectionNameList[comp];

	// error condition.
	//
	cout << "Panic: Tried to find the name of a non-existant Connection" << endl;
	assert(0);
	
	static MyString a;
	return a;
}

void addName(Latch *comp, MyString & str)
{
	globalLatchNameList[comp] = str;
}

MyString & getName(Latch *comp)
{
	Map<Latch *, MyString>::iterator i;
	if ((i = globalLatchNameList.find(comp)) != globalLatchNameList.end())
		return globalLatchNameList[comp];

	// error condition.
	//
	cout << "Panic: Tried to find the name of a non-existant Latch" << endl;
	assert(0);
	
	static MyString a;
	return a;
}

void addName(BaseSimObject *comp, MyString & str)
{
	if (comp != NULL)
	{
		if (comp->is(_UNIT_)) addName((Unit *)comp, str);
		if (comp->is(_LATCH_)) addName((Latch *)comp, str);
		if (comp->is(_STORAGE_)) addName((Storage *)comp, str);
		if (comp->is(_PORT_)) addName((Port *)comp, str);
		if (comp->is(_CONNECTION_)) addName((Connection *)comp, str);
	}
}
//
// Note: the order of checking to see if comp is a latch or storage is important.
// This is because a latch is also a storage component.
// Hence, is(_STORAGE_) would be true for a latch.

MyString & getName(BaseSimObject *comp)
{
	if (comp != NULL)
	{
		if (comp->is(_UNIT_)) return getName((Unit *)comp);
		if (comp->is(_LATCH_)) return getName((Latch *)comp);
		if (comp->is(_STORAGE_)) return getName((Storage *)comp);
		if (comp->is(_PORT_)) return getName((Port *)comp);
		if (comp->is(_CONNECTION_)) return getName((Connection *)comp);
	}

	// error condition.
	//
	cout << "Panic: Tried to find the name of an invalid (or null) component" << endl;
	assert(0);
	
	static MyString a;
	return a;
}
//
// Note: the order of checking to see if comp is a latch or storage is important.
// This is because a latch is also a storage component.
// Hence, is(_STORAGE_) would be true for a latch.

__declspec( dllexport ) BaseSimObject *getComp(char* testStr)
{
 MyString str(testStr);
 return getComp(str);
}

BaseSimObject *getComp(MyString & str)
{
	Map<Unit *, MyString>::iterator ui;
	for (ui = globalUnitNameList.begin(); ui != globalUnitNameList.end(); ui++)
	{
		if (str == (*ui).second)
			return ((BaseSimObject *)((*ui).first));
	}

	Map<Latch *, MyString>::iterator li;
	for (li = globalLatchNameList.begin(); li != globalLatchNameList.end(); li++)
	{
		if (str == (*li).second)
			return ((BaseSimObject *)((*li).first));
	}

	Map<Storage *, MyString>::iterator si;
	for (si = globalStorageNameList.begin(); si != globalStorageNameList.end(); si++)
	{
		if (str == (*si).second)
			return ((BaseSimObject *)((*si).first));
	}

	Map<Port *, MyString>::iterator pi;
	for (pi = globalPortNameList.begin(); pi != globalPortNameList.end(); pi++)
	{
		if (str == (*pi).second)
			return ((BaseSimObject *)((*pi).first));
	}

	Map<Connection *, MyString>::iterator ci;
	for (ci = globalConnectionNameList.begin(); ci != globalConnectionNameList.end(); ci++)
	{
		if (str == (*ci).second)
			return ((BaseSimObject *)((*ci).first));
	}

	return NULL;
}
void printName(ostream & out, Unit *comp)
{
	Map<Unit *, MyString>::iterator i;
	if ((i = globalUnitNameList.find(comp)) != globalUnitNameList.end())
		globalUnitNameList[comp].print(out);
	else
	{
		// error condition.
		//
		cout << "Panic: Tried to find the name of a non-existant Unit" << endl;
		assert(0);
	}
}

void printName(ostream & out, Latch *comp)
{
	Map<Latch *, MyString>::iterator i;
	if ((i = globalLatchNameList.find(comp)) != globalLatchNameList.end())
		globalLatchNameList[comp].print(out);
	else
	{
		// error condition.
		//
		cout << "Panic: Tried to find the name of a non-existant Latch" << endl;
		assert(0);
	}
}

void printName(ostream & out, Storage *comp)
{
	Map<Storage *, MyString>::iterator i;
	if ((i = globalStorageNameList.find(comp)) != globalStorageNameList.end())
		globalStorageNameList[comp].print(out);
	else
	{
		// error condition.
		//
		cout << "Panic: Tried to find the name of a non-existant Storage" << endl;
		assert(0);
	}
}

void printName(ostream & out, Port *comp)
{
	Map<Port *, MyString>::iterator i;
	if ((i = globalPortNameList.find(comp)) != globalPortNameList.end())
		globalPortNameList[comp].print(out);
	else
	{
		// error condition.
		//
		cout << "Panic: Tried to find the name of a non-existant Port" << endl;
		assert(0);
	}
}

void printName(ostream & out, Connection *comp)
{
	Map<Connection *, MyString>::iterator i;
	if ((i = globalConnectionNameList.find(comp)) != globalConnectionNameList.end())
		globalConnectionNameList[comp].print(out);
	else
	{
		// error condition.
		//
		cout << "Panic: Tried to find the name of a non-existant Connection" << endl;
		assert(0);
	}
}

void printName(ostream & out, BaseSimObject *comp)
{
	if (comp != NULL)
	{
		if (comp->is(_UNIT_)) printName(out, (Unit *)comp);
		if (comp->is(_LATCH_)) printName(out, (Latch *)comp);
		if (comp->is(_STORAGE_)) printName(out, (Storage *)comp);
		if (comp->is(_PORT_)) printName(out, (Port *)comp);
		if (comp->is(_CONNECTION_)) printName(out, (Connection *)comp);
	}
	else
	{
		// error condition.
		//
		cout << "Panic: Tried to find the name of an invalid (or null) component" << endl;
		assert(0);
	}
}
//
// Note: the order of checking to see if comp is a latch or storage is important.
// This is because a latch is also a storage component.
// Hence, is(_STORAGE_) would be true for a latch.