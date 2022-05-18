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
// ISInfo.cpp: implementation of the ISInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "..\pcprogui.h"
#include "ISInfo.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

IMPLEMENT_SERIAL( ISInfo, CObject, 1 )

ISInfo::ISInfo()
{

}

ISInfo::~ISInfo()
{

}

void ISInfo::Serialize(CArchive& ar)
{
 instrDescr.Serialize(ar);
 varGroups.Serialize(ar);
 opMappings.Serialize(ar);
 opGroups.Serialize(ar);
}

void ISInfo::xGen(CFile* file, int level)
{
 CString spaces(_T(' '), level);
 CString out;

 //section 1
 out="\n;///////////////////////////////Section 1: Specific operations //////////////////////////////////////////\n\n";
 
 out+=spaces+"(OPERATIONS_SECTION\n";
 file->Write(out, out.GetLength());
 
 varGroups.xGen(file,level+1);
 opGroups.xGen(file,level+1);

 out=spaces+")\n\n\n";
 file->Write(out, out.GetLength());

 //section 2
 out="\n;////////////////////////////////////Section 2: Instruction template //////////////////////////////////\n\n";
 out+=spaces+"(INSTRUCTION_SECTION\n";
 file->Write(out, out.GetLength());
 
 instrDescr.xGen(file,level+1);
 
 out=spaces+")\n\n\n";
 file->Write(out, out.GetLength());
 
 //section 3
 out="\n;////////////////////////////////////Section 3: Operation mappings //////////////////////////////////////\n\n";
 out+=spaces+"(OPMAPPING_SECTION\n";
 file->Write(out, out.GetLength());
 
 opMappings.xGen(file,level+1);
 
 out=spaces+")\n\n\n";
 file->Write(out, out.GetLength());
}
