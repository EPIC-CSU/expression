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
#ifndef _INSTRSELSAPRIM_H_
#define _INSTRSELSAPRIM_H_

#define IS_DUPLICATE_TREE(x) isel_duplicate_tree(x,old_kids,new_kids,eruleno)
#define IS_NEW_TREE(x,y,z) new_tree(isel_get_term_num(x),y,z)



int isel_index_of(NODEPTR_TYPE p,NODEPTR_TYPE kids[10],int eruleno);
NODEPTR_TYPE isel_coresp_kid(NODEPTR_TYPE p,NODEPTR_TYPE old_kids[10],NODEPTR_TYPE new_kids[10],int eruleno);
NODEPTR_TYPE isel_duplicate_tree(NODEPTR_TYPE p,NODEPTR_TYPE old_kids[10],NODEPTR_TYPE new_kids[10],int eruleno);

#endif
