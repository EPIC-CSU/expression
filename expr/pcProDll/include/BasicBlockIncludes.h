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
#ifndef __BASICBLOCKINCLUDES_H__
#define __BASICBLOCKINCLUDES_H__

#define _INVALID_ -1
#define _FIRST_BB_ 1
#define _LAST_BB_ 2
#define _SIMPLE_BB_ 3
#define _COMPLEX_BB_ 4

const int _FIRST_SUCC_ = 1;
const int _SECOND_SUCC_ = 2;
const int _ALL_SUCC_ = 3;

#define _TREE_ARC_ 1
#define _BACK_ARC_ 2
#define _FORWARD_ARC_ 3
#define _CROSS_ARC_ 4

#endif
