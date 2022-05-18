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
/*****************************************************************
 *file  :			irDumpGen.cpp								 *
 *authors :			Atri Mandal									 *
 *last modified :	11/25/2002									 *
 *																 *
 *contents:			Dump Format Generation						 *
 *comments:-													 *
 *compiled with :	g++											 *
 *for more info :	mandala@ics.uci.edu							 *
 *****************************************************************/

#ifndef _IRDUMPGEN_CPP_
#define _IRDUMPGEN_CPP_


#include <iostream.h>
#include "node.h"
#include "MyString.h"
#include "symtabs.h"
#include <set>
#include<list>

#ifdef  __UNIX__
typedef list<MyString> CharList;
#else
typedef std::list<MyString> CharList;
#endif


int print_IRFormat(FILE *fp,NodePtr p)
{
	NodePtr node=NULL;
	static int tick=0;		//static int required to preserve value between calls
    tick++;

	//select the node which contains the dump format
 for(NodeList::iterator i = p->Children()->begin(); i != p->Children()->end(); i++)
	{
	 	if ((*i)->Kind() == nke_dumpFormat)
		{
			node = *i;
			break;
		}
	}

 // All Set...Go and print the format
 if (node !=NULL)
 {
 fprintf(fp,"%d",tick);
 NodeList::iterator j= node->Children()->begin();	
 fprintf(fp,"%s\n",(*j)->Name());
 j++;
 
 while (j != node->Children()->end())
	{
	    fprintf(fp,";");
		fprintf(fp,"%s\n",(*j)->Name());
		j++;
	}

 }
 return 1;
}

extern int numDataSlots;

int printdumpIRFile(Symtab st, NodePtr node)
{
	FILE *fp; NodePtr n=NULL;int count;
	int icount;

    CharList MyList;
	
	if ((fp = fopen("dumpIRFormat.txt", "w")) == NULL)
	{
		printf("Could not open file: dumpIRFormat.txt\n");
		assert(0);
	}

/*********************This part has been Hardcoded****************************/
/*********************There must be some provision in the ADL to *************/
/*********************give these specifications******************************/
fprintf(fp,"globalDeclarations\n");
fprintf(fp,"registerPrefix=$\n");
fprintf(fp,"delimiter=,\n");
fprintf(fp,"endGlobalDeclarations\n");
fprintf(fp,"\n");
fprintf(fp,"textSectionHeader\n");
fprintf(fp,"\"\\t4\\tli\\t($258)\\t(8888888)\\n\"\n");
// Data op slots
for( icount = 0; icount < numDataSlots-1; icount++ ) 
	fprintf(fp,"\"\\t4\\tnop\\t()\\t()\\n\"\n");
// Control op slot
	fprintf(fp,"\"\\t4\\tnop\\t()\\t()\\n\"\n");
fprintf(fp,"endTextSectionHeader\n\n");
fprintf(fp,"textSectionFooter\n");
fprintf(fp,"endTextSectionFooter\n\n");
fprintf(fp,"regAllocBlockSectionHeader\n");
fprintf(fp,"endRegAllocBlockSectionHeader\n\n");
fprintf(fp,"regAllocBlockSectionFooter\n");
fprintf(fp,"endRegAllocBlockSectionFooter\n");
/*********************Hardcoding ends****************************************/

	// Point towards the operation section node in the parse tree 
    NodePtr MappingNode = node->Child(nke_operation_section);	

	// Select the _all_ops node under OPGROUPS
	NodePtr p=NULL;
	for(NodeList::iterator i = MappingNode->Children()->begin(); i != MappingNode->Children()->end(); i++)
	{
		if (((*i)->Kind() == nke_op_group) && strcmp((*i)->Name(),"_all_ops_") == 0)
		{
			p=*i;
			break;
		}
	}

	//Now select each opcode and dump the format in the dumpIR file
	fprintf(fp,"\nopndMap\n");
	for (NodeList::iterator j = p->Children()->begin(); j != p->Children()->end(); j++)
	{
	  MyList.push_back(MyString((*j)->Name()));
 	  print_IRFormat(fp,*j);
	}
	fprintf(fp,"endOpndMap\n");

	// Print the opcode mapping (one-to-one)
	count=0;
	fprintf(fp,"\nopcodeMap\n");
	for (CharList::iterator k= MyList.begin(); k != MyList.end();
		 k++)
	{
		count++;
		fprintf(fp,"%s %d\n",(*k).getStr(),count);
	}
    fprintf(fp,"endOpcodeMap");

	fclose(fp);
	return 1;
}	

#endif