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
// ProjectFile.h: interface for the ProjectFile class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROJECTFILE_H__6B48AFAB_B47F_11D2_BBB6_00C04FB17504__INCLUDED_)
#define AFX_PROJECTFILE_H__6B48AFAB_B47F_11D2_BBB6_00C04FB17504__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include <afxtempl.h>

class BasePropPage;

class ProjectFile  
{
public:
//	void DisplayProperties();
 CString name;
 int selected;

 //command line switches
 int printIList, printCFG, printHTG, afterTbz;
 int simNoSSA, simSSA, simHTG, simISel, simTbz;
 int doISel, doTbz, doMut, doUnroll, printMC, pipelinedTbz;

	ProjectFile();
	virtual ~ProjectFile();
// void DisplayProperties(BasePropPage* page);
};

typedef CList<ProjectFile*,ProjectFile*> ProjectFileList;

#endif // !defined(AFX_PROJECTFILE_H__6B48AFAB_B47F_11D2_BBB6_00C04FB17504__INCLUDED_)
