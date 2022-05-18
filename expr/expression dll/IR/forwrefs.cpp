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
#include "node.h"

//prototypes
void UpdateSymbolLinks(NodePtr node);
void UpdateSymbolsLinks(NodeListPtr list);

int forwref(NodePtr node){
	if(node->Kind()==nke_port ||
		node->Kind()==nke_latch ||
		node->Kind()==nke_connection ||
		node->Kind()==nke_subcomp){
		UpdateSymbolLinks(node);
	}
	if(node->Kind()==nke_pipeline ||
		node->Kind()==nke_parallel ||
		node->Kind()==nke_alternate ||
		node->Kind()==nke_var){
		UpdateSymbolsLinks(node->Children());
	}
	return 0; //when it returns 1, the Traverse function stops the traversal
}

//collect the parent informatio for all the IR
int collect_parents(NodePtr node){
	NodeList::iterator i;
	for(i=node->Children()->begin();i!=node->Children()->end();i++){
		(*i)->Parent(node);
	}

	return 0; //when it returns 1, the Traverse function stops the traversal
}


//Update forward references to symbols declraed later in the EXPRESSION description
void forwrefs(NodePtr node){
	//collect forward references in to table
	node->Traverse(&forwref); //the Traverse function calls the forwref function for each node 
	//collect parents information
	node->Traverse(&collect_parents); //the Traverse function calls the collect_parents function for each node 
}