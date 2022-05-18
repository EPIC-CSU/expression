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
 *file  : simulatorGeneratorSupportFuncs.cpp                 *
 *authors : Asheesh Khare                                    *
 *last modified :                                            *
 *                                                           *
 *modified by : Sudeep Pasricha	(10/24/02)				     *
 *contents:Support functions for simulator generation        *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#include <assert.h>
#include "SimulatorGeneratorSupportFuncs.h"
#include "symtabs.h"

#define NULL 0
#define MAXSLOTS 20

extern Symtab st;

// ComponentMap is used to find out the "parent nodes" for certain nodes - e.g nke_component
// in the architecture. This is needed because the ir does not contain backptrs to the parent nodes
map<NodePtr, NodePtr> ComponentMap;
map<NodePtr, NodePtr> DTMap;
map<char *, char *> OpMapper;
map<MyString,MyString> OpMapperGenericOpFile;
char * tree_opcode2[1000];
char * tree_opcode3[1000];
map<MyString,int> OpNumberMapper;
char * myportlist[500];
int myportcount = 0;
int count2_1;
// a limit of 50 units - which should be enough for any design ! or maybe not?
// '64K RAM should be good enough for everyone' - Bill Gates
vector<MyString> unit_inlatch[50];
vector<MyString> unit_outlatch[50];
char * comb_opcode[100];
char * comb_gen_opcode[100];
// Array from the parser which holds pairs of generic opcodes followed by
// target opcodes they are mapped to.
extern char * tree_opcode[];
extern char * tree_srclist[];
extern char * tree_dstlist[];
extern int combinations[];
extern char * tree_opcode_type[];
	
// ConnectivityMatrix is used to store temporarily, the netlist, i.e what units are 
// connected to what ports/connections etc.

ComponentGraph ConnectivityMatrix;
int numDataSlots;

// Vector of storage components at level 1 (for printing into mem.config)
vector<MyString> memlevel1;

void CreateOpMap ()
{
	int count=0;
	int count2=0;
	int found = 0;

	count2_1=0;
	while(!strcmp(tree_opcode_type[count],"GEN"))
		count++;
	
	while(tree_opcode[count])
	{
		
		int combcnt=0;
		int tmp=0;
		while(combinations[combcnt] != -1)
		{
			if (count == combinations[combcnt])
			{
				//a user defined combined instruction
				//skip at this point
				tmp=1;
				count++;
				while(!strcmp(tree_opcode_type[count],"GEN"))
					count++;
				break;
			}
			combcnt++;
		}
		
		if (tmp == 0)
		{
			
			while(tree_opcode2[count2])
			{
				if (!strcmp(tree_opcode[count],tree_opcode2[count2]))
				{
					found = 1;
					break;
				}
				count2++;
			}
			if (found == 0) 
			{
				tree_opcode2[count2_1++] = strdup(tree_opcode[count-1]);
				tree_opcode2[count2_1++] = strdup(tree_opcode[count]);
			}
			
			count++;
			
			while(tree_opcode_type[count] && (!strcmp(tree_opcode_type[count],"GEN")))
				count++;
		}
		
		tmp=0;
		count2=0;
		found=0;
	}

	map<MyString, MyString>::iterator i1, i2;

	count = 1;
	count2 = 1;
	found = 0;

			while (tree_opcode2[count]) 
			{

					i1 = OpMapperGenericOpFile.begin();
					i2 = OpMapperGenericOpFile.end();
					
					while (i1 != i2) 
					{
						if (i1->first == tree_opcode2[count-1])
						{
							found = 1;
							MyString t = (i1->second).getStr();
							char * x = strtok(t.getStr()," ");
							tree_opcode3[count] = strdup(x);
							tree_opcode3[count-1] = strdup(x);
							x = strtok(NULL," |\t");
							break;
						}
							i1++;
					}

				count+=2;
			}

/////////////////////////
//Now we generate file
// combined_ops.txt

			FILE * combfile = fopen("combined_ops.txt","w");

			if (combfile == NULL)
			{
				printf("Can't open file: combined_ops.txt\n");
				assert(0);
			}

			int combcnt=0;
			int tmpcnt=0;

			fprintf(combfile,"BEGIN\n");
			while (combinations[combcnt] != -1)
			{
				int target_marker = combinations[combcnt];
				int start_marker = combinations[combcnt]-1;
				
				fprintf(combfile,"(");
				fprintf(combfile,tree_opcode[target_marker]);
				fprintf(combfile,"\n");

				comb_opcode[tmpcnt] = strdup(tree_opcode[target_marker]);
				comb_gen_opcode[tmpcnt++] = strdup(tree_opcode[start_marker]);

				// set the start marker
				while((!strcmp(tree_opcode_type[start_marker],"GEN")) && (start_marker !=0) )
					start_marker--;

				if (start_marker !=0)
					start_marker++;

				//now start marker points to first GENERIC instr of this mapping
				while(!strcmp(tree_opcode_type[start_marker],"GEN"))
				{
					int count=0, found=0;
					while(tree_opcode2[count])
					{
						if (!strcmp(tree_opcode2[count],tree_opcode[start_marker]))
						{
							fprintf(combfile,"\t(%s\t",tree_opcode2[count+1]);
							found=1;
							break;
						}
						count++;
					}

					if (found == 0)
					{
						printf("No mapping available for %s or specify what function to map to for %s\n",
							tree_opcode[start_marker],tree_opcode[target_marker]);
							assert(0);
					}
					found=0;
					// now fill out the register values
					// first destination list, then source list
					
					if (tree_dstlist[start_marker])
					{
						char * x = strtok(tree_dstlist[start_marker]," ");
						char * y;
						
						do
						{
							
							if (y = strstr(tree_dstlist[target_marker],x))
							{
								int pos = 0;

								while (y != tree_dstlist[target_marker])
								{
									if (*y == ' ')
										pos++;
									y--;
								}

								fprintf(combfile,"%d\t",pos);
								
							} else
							{
								fprintf(combfile,"-1\t");
							}

						} while (x = strtok(NULL," "));
					}

					if (tree_srclist[start_marker])
					{
						char * x = strtok(tree_srclist[start_marker]," ");
						char * y;
						
						do
						{
							
							if (y = strstr(tree_srclist[target_marker],x))
							{
								int pos = 0;

								while (y != tree_srclist[target_marker])
								{
									if (*y == ' ')
										pos++;
									y--;
								}

								fprintf(combfile,"%d\t",pos);
								
							} else
							{
								fprintf(combfile,"-1\t");
							}

						} while (x = strtok(NULL," "));
					}


					start_marker++;
					fprintf(combfile,")\n");
				}



				fprintf(combfile,")\n");
				combcnt++;

			}

			fprintf(combfile,"END\n");
			fclose(combfile);

}

/*******************************************************************************************
 * MEMBER FUNCTIONS for class LowerTriangIntMatrix and class ComponentGraph defined here   *
 *******************************************************************************************/
void LowerTriangIntMatrix::Element(int val, int xpos, int ypos)
{
	// since it is a lower triangular matrix, y index should be >= xindex
	int xindex,yindex;
	if (xpos < ypos) 
	{
		yindex = ypos; 
		xindex = xpos;
	}
	else
	{
		yindex = xpos; 
		xindex = ypos;
	}

	// Add more rows to the matrix
	for(int y=_matrix.size(); y<=yindex; y++)
	{
		int temp0 = 0;
		vector<int> newRow;
		_matrix.push_back(newRow);
		// Fill zeroes in the empty spaces in the new row
		vector<int> & xdimArray = _matrix[y];
		for(int x = 0; x<=y; x++)
			xdimArray.push_back(temp0);
	}

	vector<int> & xdimArray = _matrix[yindex];
	xdimArray[xindex]=val;
}

int LowerTriangIntMatrix::Element(int xpos, int ypos)
{
	// since it is a lower triangular matrix, y index should be >= xindex
	int xindex,yindex;
	if (xpos < ypos) 
	{
		yindex = ypos; 
		xindex = xpos;
	}
	else
	{
		yindex = xpos; 
		xindex = ypos;
	}

	if (yindex > _matrix.size())
		return -1;

	vector<int> & xdimArray = _matrix[yindex];
	if (xindex > xdimArray.size())
		return -1;

	return xdimArray[xindex];
}

void LowerTriangIntMatrix:: printRow(int i)
{
	if (i > _matrix.size())
	{
		cout << "LowerTriangIntMatrix : Out of bounds exception " << endl;
		assert(0);
		return;
	}

	vector<int> & row = _matrix[i];
	for(int j=0;j<row.size();j++)
	{
		printf("%d ",row[j]);
	}
	printf("\n");
}

void LowerTriangIntMatrix:: print()
{
	for(int i=0; i<_matrix.size(); i++)
	{
		vector<int> & row = _matrix[i];
		for(int j=0;j<row.size();j++)
		{
			printf("%d ",row[j]);
		}
		printf("\n");
	}
}

// Asign unique index to a component
int ComponentGraph::getIndex(NodePtr p)
{
	map<MyString,int>:: iterator i;
	MyString tempStr(p->Name());
	i = _indexMap.find(tempStr);
	if (i!= _indexMap.end())
		return i->second;
	else
	{
		int temp = _lastIndex;
		_indexMap[tempStr] = _lastIndex++;
		_elementArray.push_back(p);
		return temp;
	}
}

void ComponentGraph::Connect(NodePtr i, NodePtr j)
{
	int u = getIndex(i);
	int v = getIndex(j);
	_adjMatrix.Element(1,u,v);
}

int ComponentGraph::Connected(NodePtr i, NodePtr j)
{
	int u = getIndex(i);
	int v = getIndex(j);
	if (_adjMatrix.Element(u,v) == 1)
		return 1;
	else
		return 0;
}

void ComponentGraph::printGraph(void)
{
	for(int j=0; j<_elementArray.size(); j++)
	{
		printf("%s ", (_elementArray[j])->Name());
		_adjMatrix.printRow(j);
	}
	printf("\n");
}

/******************************************************************************************
 * SUPPORT FUNCTIONS start here                                                           *
 ******************************************************************************************/

// This function when executed over each node of the ir, generates a ComponentMap 
// mapping component nodes to their parent nodes(the subtypes and basic types etc.)

int getArchRootNode(NodePtr p)
{
	if (p->Kind() == nke_architecture_section)
	{
		ArchRootNode = p;
		return 1; //stop traversal
	}
	return 0;
}

int CreateComponentList(NodePtr parent,NodePtr child)
{
	//static int start = 0;
	//MyString currStr(child->Name());
	
	switch(child->Kind())
	{
	case nke_subtypedecl : ComponentMap[child] = NULL;
		break;

	case nke_component : Componentlist.push_back(child);
	case nke_subtype : ComponentMap[child] = parent; break; 
	//case nke_latch : ComponentMap[currStr] = parent; break;
	default: ;
	}
	
	return 0;
}

int CreateStorageList(NodePtr parent,NodePtr child)
{
	//static int start = 0;
	//MyString currStr(child->Name());
	
	switch(child->Kind())
	{
	case nke_subtypedecl : ComponentMap[child] = NULL;
		break;

	case nke_component : Componentlist.push_back(child);
	case nke_subtype : ComponentMap[child] = parent; break; 
	//case nke_latch : ComponentMap[currStr] = parent; break;
	default: ;
	}
	
	return 0;
}

void getSubType(NodePtr node,MyString & retstr)
{
	NodePtr retnode;
	
	NodeListPtr deflist = (node->Symbol())->GetDefList();
	
	if(deflist!=NULL){
		if (deflist->size() == 1)
			retnode = (node->Symbol())->GetFirstDef();
		else
		{
			NodeList::iterator i;
			
			for(i=deflist->begin(); i!= deflist->end(); i++)
			{
				if (((*i)->Kind() == nke_component) || ((*i)->Kind() == nke_subtype) || ((*i)->Kind() == nke_subtypedecl))
				{
					retnode = *i;
					break;
				}
			}
		}
	}
	
	// Returns the Base class this component is derived from: 
	// Unit, Storage, Port, Connection or Latch
	
	ComponentMapIterator i;
	i = ComponentMap.find(retnode);

	if (i == ComponentMap.end())
	{
		cout << "Error : " << node->Name() << " not found in the Component Map" << endl;
		assert(0);
	}

	retstr = "";

	while ((ComponentMap[retnode]) != NULL)
	{
		NodePtr tempnode = ComponentMap[retnode];
			
		SymbolPtr tempsym = tempnode->Symbol();
		retstr = tempsym->Name();
		
		deflist = tempsym->GetDefList();
		
		if(deflist!=NULL){
			if (deflist->size() == 1)
				retnode = tempsym->GetFirstDef();
			else
			{
				NodeList::iterator i;
				
				for(i=deflist->begin(); i!= deflist->end(); i++)
				{
					if (((*i)->Kind() == nke_component) || ((*i)->Kind() == nke_subtype) || ((*i)->Kind() == nke_subtypedecl))
					{
						retnode = *i;
						break;
					}
				}
			}
		}


		i = ComponentMap.find(retnode);
		
		if (i == ComponentMap.end())
		{
			cout << "Error : " << retnode->Name() << " not found in the Component Map" << endl;
			assert(0);
		}
	}
}

void getComponentType(NodePtr node, MyString & retstr)
{
	// Returns the Derived class of ehich this component is an instance

	ComponentMapIterator i;
	i = ComponentMap.find(node);

	if (i == ComponentMap.end())
	{
		cout << "Error : " << node->Name() << " not found in the Component Map" << endl;
		assert(0);
	}
	retstr = (((ComponentMap[node]) == NULL)?0:(ComponentMap[node])->Name());
}

void getConstructorString(NodePtr node, MyString & retstr)
{
	//Assumption is that the component has an entry in the symbol table 
	//pointing to the corresponding node in the ir;

	NodeListPtr tempList = node->Children();

	NodeList::iterator i;
	for (i=tempList->begin(); i!=tempList->end(); i++)
	{
		if ((*i)->Kind() == nke_initstring)
		{
			retstr = (*i)->Name();
			return;
		}
	}
	retstr = "";
}


void getOpCodeList(NodePtr node, NodeList & oplist)
{
	NodeListPtr childlist = node->Children();
	NodeList::iterator i,j,k;
	NodeList opgrp_list, *opcodelist;
	NodePtr grp_node;
	char * grp_name;

	oplist.clear();

	// get the nke_opcodes child for this nke_component
	for(i=childlist->begin(); i!=childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_opcodes)
		{
			// get the list of opcode group names associated with this unit
			NodeList *opgrp_list = (*i)->Children();
			for(j=opgrp_list->begin();j!=opgrp_list->end();j++)
			{
				grp_name = (*j)->Name();
				grp_node = (st.GetSymbol(grp_name))->GetFirstDef();
				// get the list of opcodes associated with this opcode group name
				opcodelist = grp_node->Children();
				for(k=opcodelist->begin();k!=opcodelist->end();k++)
					oplist.push_back(*k);
			}
		}
	}
}

void getLatchList(NodePtr node, NodeList & latchlist, NodeKindEnum kind)
{
	NodeListPtr childlist = node->Children();
	NodeList::iterator i,j,k;

	latchlist.clear();

	// get the nke_latches child for this nke_component
	for(i=childlist->begin(); i!=childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_latches)
		{
			// get the list of latch_group nodes associated with this unit
			NodeList *aggregate_latchlist = (*i)->Children();
			for(j=aggregate_latchlist->begin();j!=aggregate_latchlist->end();j++)
			{
				if ((*j)->Kind() == kind)
				{
					NodeList *latch_grp_kids = (*j)->Children();
					// get the list of latches associated with this latch group node
					for(k=latch_grp_kids->begin();k!=latch_grp_kids->end();k++)
						latchlist.push_back(*k);
				}
			}
		}
	}
}

// ElementList can be used to provide 
void getElementList(NodePtr node, NodeList & eltlist, NodeKindEnum kind)
{
	NodeListPtr childlist = node->Children();
	NodeList::iterator i,j;

	eltlist.clear();

	// get the nke_opcodes child for this nke_component
	for(i=childlist->begin(); i!=childlist->end(); i++)
	{

		if ((*i)->Kind() == kind)
		{
			// get the list of latch names associated with this unit
			NodeList *elt_list = (*i)->Children();
			for(j=elt_list->begin();j!=elt_list->end();j++)
				eltlist.push_back(*j);
		}
	}
}

void getElement(NodePtr node, NodeList & eltlist, NodeKindEnum kind)
{
	NodeListPtr childlist = node->Children();
	NodeList::iterator i,j;

	eltlist.clear();

	for(i=childlist->begin(); i!=childlist->end(); i++)
	{
		if ((*i)->Kind() == kind)
			eltlist.push_back(*i);
	}
}

void printComponentMap()
{
	ComponentMapIterator i1,i2;
	i1 = ComponentMap.begin();
	i2 = ComponentMap.end();	
	while (i1 != i2)
	{
		NodePtr temp = i1->first;
		NodePtr temp2 = i1->second;
		//printf("%s,%s\n", temp->Name(), (temp2==0)?0:temp2->Name());
		fprintf(fp,"componentlist: %s,%s\n", temp->Name(), (temp2==0)?0:temp2->Name());
		i1++;
	}
	printf("\n");
}

char* makeLowerCase(char* inpstr)
{
	char * newstr = strdup(inpstr);
	char * p = newstr;
	for(p;*p!='\0';p++)
		*p=tolower(*p); 
	return newstr;
}

int instantiateUnits(NodeList complist)
{
	NodeList::iterator i;

	fprintf(fp,"\n\t/********* Units *****************************/\n");
	
	fprintf(fp,"\ttoyArch = new ArchUnit();\n");
	fprintf(fp,"\ttoyArch->init();\n");
	fprintf(fp,"\tnameStr = \"ARCH\";\n");
	fprintf(fp,"\taddName(toyArch, nameStr);\n\n");

	fprintf(fp,"\tUnit *cntrl;\n");

	fprintf(fp,"\tcntrl = new ControlUnit();\n");
	fprintf(fp,"\tcntrl->init();\n");

    fprintf(fp,"\tnameStr = \"Control\";\n");
    fprintf(fp,"\taddName(cntrl, nameStr);\n\n");	// print instantiation of control unit

	//fprintf(fp,"\tglobalStatsObjects.push_back((BaseSimObject *) cntrl);\n\n");

	for(i=complist.begin(); i!=complist.end(); i++)
	{
		MyString className;
		MyString baseClass;
		MyString ctorStr;


		char * compName = (*i)->Name();

		getSubType(*i,baseClass);

		if (baseClass == "UNIT")
		{
			getComponentType(*i,className);
			getConstructorString(*i,ctorStr);

			fprintf(fp,"\tUnit * %s;\n\t%s=new %s(", compName, compName, 
				className.getStr());

			NodeList tlist;
			getElement(*i,tlist,nke_capacity);
			NodeList::iterator t2;
			for (t2=tlist.begin();t2!=tlist.end();t2++)
			{
				fprintf(fp,"%s",(*t2)->Name());
			}

			getElement(*i,tlist,nke_instrin);
			for (t2=tlist.begin();t2!=tlist.end();t2++)
			{
				fprintf(fp,",%s",(*t2)->Name());
			}

			getElement(*i,tlist,nke_instrout);
			for (t2=tlist.begin();t2!=tlist.end();t2++)
			{
				fprintf(fp,",%s",(*t2)->Name());
			}

			fprintf(fp,");\n");
			fprintf(fp,"\t%s->init();\n\n",compName);	
			

			fprintf(fp,"\tcntrl->addUnit(%s);\n\n",compName);
			fprintf(fp,"\tnameStr = \"%s\";\n",compName);
			fprintf(fp,"\taddName(%s, nameStr);\n\n",compName);



			Unitlist.push_back(*i);	
		}
	}
	fprintf(fp,"\ttoyArch->addUnit(cntrl);\n"); // ControlUnit is added to ArchUnit.
	
	return 0;
}

int instantiateLatches(NodeList complist)
{
	NodeList::iterator i;

	//printComponentMap();

	for(i=complist.begin(); i!=complist.end(); i++)
	{
		MyString className;
		MyString baseClass;
		MyString ctorStr;
		char * compName = (*i)->Name();
		
		getSubType(*i,baseClass);

		if (baseClass == "LATCH")
		{
			getComponentType(*i,className);
			getConstructorString(*i,ctorStr);
			
			fprintf(fp,"\tLatch * %s = new %s(%s);\n", (*i)->Name(), 
				className.getStr(), ctorStr.getStr());
			
			fprintf(fp,"\tnameStr = \"%s\";\n",compName);
			fprintf(fp,"\taddName(%s, nameStr);\n\n",compName);			
			
			Latchlist.push_back(*i);
		}
	}
	return 0;
}

char* getStorageType(NodePtr storage)
{
	NodeList::iterator i;
	NodeListPtr childlist = storage->Children();
	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_storagetype)
		{
			return (*i)->Name();
			break;
		}
	}

	return NULL;
}

int instantiateStorage(NodeList complist)
{
	NodeList::iterator i;
	for(i=complist.begin(); i!=complist.end(); i++)
	{
		MyString className;
		MyString baseClass;
		MyString ctorStr;
		char * compName = (*i)->Name();
		
		getSubType(*i,baseClass);

		if (baseClass == "STORAGE")
		{
			//char* type = getStorageType(*i);
			char* stsizeName = getParameter(*i, nke_storagesize);
			//fprintf(fp,"test = %s\n",stsizeName);
			getComponentType(*i,className);
			getConstructorString(*i,ctorStr);
			
			if (className != "Storage") {
			//fprintf(fp,"\tStorage * %s = new %s(%s);\n", (*i)->Name(),
			fprintf(fp,"\tLatch * %s = new %s(%s);\n", (*i)->Name(),
				className.getStr(), ctorStr.getStr());
	
			//fprintf(fp,"\t((ControlUnit *)cntrl)->addStorage(%s);\n",compName);
			//fprintf(fp,"\tcntrl->addLatch(%s);\n",compName);

			fprintf(fp,"\tnameStr = \"%s\";\n",compName);
			fprintf(fp,"\taddName(%s, nameStr);\n\n",compName);			
			} else {
					//fprintf(fp,"\t((ControlUnit *)cntrl)->addStorage(%s);\n",compName);
			}

			Storagelist.push_back(*i);
		}
	}
	return 0;
}

vector<char *> storageNames;

/*
int generateMemorySubsystem(NodePtr storageRootNode)
{
	long sramStartAddress = -1;
	long sramEndAddress = -1;
	int storageNumber = 0;
	int sramNumber;

	// Instantiate and connect the memory and  modules.
	fprintf(fp,"\n// Initialize the memory modules");
	fprintf(fp,"\nvoid Connectivity :: initialize()");
	fprintf(fp,"\n{");

	NodeList::iterator i;
	NodeListPtr childlist = storageRootNode->Children();
	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_storage)
		{
			char* type = getStorageType(*i);

			if (strcmp(type, "DCACHE") == 0)
			{
				char* stName = (*i)->Name();
				storageNames.push_back(stName);
				storageNumber++;

				char* wordsize = getParameter(*i, nke_wordsize);
				if (wordsize == NULL)
				{
					printf("Error: WORDSIZE is not specified for DCACHE %s\n", (*i)->Name());
					assert(0);
				}

				char* linesize = getParameter(*i, nke_linesize);
				if (linesize == NULL)
				{
					printf("Error: LINESIZE is not specified for DCACHE %s \n", (*i)->Name());
					assert(0);
				}

				char* numlines = getParameter(*i, nke_numlines);
				if (numlines == NULL)
				{
					printf("Error: NUM_LINES is not specified for DCACHE %s \n", (*i)->Name());
					assert(0);
				}

				char* associativity = getParameter(*i, nke_associativity);
				if (associativity == NULL)
				{
					printf("Comment: ASSOCITIVITY is not specified for DCACHE %s, Direct Cache assumed\n", (*i)->Name());
				}

				char* replacementPolicy = getParameter(*i, nke_replacementpolicy);
				if (replacementPolicy == NULL)
				{
					printf("Error: REPLACEMENT_POLICY is not specified for DCACHE %s, LRU assumed\n", (*i)->Name());
				}

				char* writePolicy = getParameter(*i, nke_writepolicy);
				if (writePolicy == NULL)
				{
					printf("Error: WRITE_POLICY is not specified for DCACHE %s, WRITE_BACK assumed\n", (*i)->Name());
				}

				char* readLatency = getParameter(*i, nke_read_latency);
				if (readLatency == NULL)
				{
					printf("Error: READ_LATENCY is not specified for DCACHE %s\n ", (*i)->Name());
					assert(0);
				}

				char* writeLatency = getParameter(*i, nke_write_latency);
				if (writeLatency == NULL)
				{
					printf("Error: WRITE_LATENCY is not specified for DCACHE %s\n ", (*i)->Name());
				}

				//char* level = getParameter(*i, nke_level);
				//if (level == NULL)
				//{
				//	printf("Error: LEVEL is not specified for DCACHE %s\n ", (*i)->Name());
				//	assert(0);
				//}

				//char* nextLevel = getParameter(*i, nke_next_level);
				//if (nextLevel == NULL)
				//{
				//	printf("Error: NEXT_LEVEL is not specified for DCACHE %s\n ", (*i)->Name());
				//	assert(0);
				//}
				
				if ((associativity != NULL) && (atoi(associativity) > 1))
				{
					fprintf(fp, "\n\n\t// %s: <No_Lines, words_in_a_line, no_ways, word_size(bytes), latency>", (*i)->Name());
					fprintf(fp, "\n\tMemoryModules[n_modules++]=(MemoryModule*)new AssociativeCache<%s, %s, %s, %s, %s>();", numlines, linesize, associativity, wordsize, readLatency);
				}
				else
				{
					fprintf(fp, "\n\n// %s: <No_Lines, words_in_a_line, word_size(bytes), latency>", (*i)->Name());
					fprintf(fp, "\nMemoryModules[n_modules++]=(MemoryModule*)new DirectCache<%s, %s, %s, %s>();", numlines, linesize, wordsize, readLatency);
				}

			}
			else if (strcmp(type, "DRAM") == 0)
			{
				char* stName = (*i)->Name();
				storageNames.push_back(stName);
				storageNumber++;

				char* readLatency = getParameter(*i, nke_read_latency);
				if (readLatency == NULL)
				{
					printf("Error: READ_LATENCY is not specified for DRAM %s\n ", (*i)->Name());
					assert(0);
				}

				char* writeLatency = getParameter(*i, nke_write_latency);
				if (writeLatency == NULL)
				{
					printf("Error: WRITE_LATENCY is not specified for DRAM %s\n ", (*i)->Name());
				}

				//char* level = getParameter(*i, nke_level);
				//if (level == NULL)
				//{
				//	printf("Error: LEVEL is not specified for DRAM %s\n ", (*i)->Name());
				//	assert(0);
				//}

				char* width = getParameter(*i, nke_storagewidth);
				if (width == NULL)
				{
					printf("Error: WIDTH is not specified for DRAM %s\n ", (*i)->Name());
					assert(0);
				}

				fprintf(fp, "\n\n\t// %s: <latency>", (*i)->Name());
				fprintf(fp, "\n\tMemoryModules[n_modules++]=(MemoryModule*)new Dram<%s>();", readLatency);
			}
			else if (strcmp(type, "SRAM") == 0)
			{
				char* stName = (*i)->Name();
				storageNames.push_back(stName);

				sramNumber = storageNumber++;

				char* readLatency = getParameter(*i, nke_read_latency);
				if (readLatency == NULL)
				{
					printf("Error: READ_LATENCY is not specified for SRAM %s\n ", (*i)->Name());
					assert(0);
				}

				char* writeLatency = getParameter(*i, nke_write_latency);
				if (writeLatency == NULL)
				{
					printf("Error: READ_LATENCY is not specified for SRAM %s\n ", (*i)->Name());
				}

				char* numBanks = getParameter(*i, nke_numbanks);
				if (numBanks == NULL)
				{
					printf("Error: NUM_BANKS is not specified for SRAM %s\n ", (*i)->Name());
				}

				char* accessMode = getParameter(*i, nke_access_mode);
				if (accessMode == NULL)
				{
					printf("Error: ACCESS_MODE is not specified for SRAM %s\n ", (*i)->Name());
				}

				char* numParRead = getParameter(*i, nke_num_par_read);
				if (numParRead == NULL)
				{
					printf("Error: NUM_PARALLEL_READ is not specified for SRAM %s\n ", (*i)->Name());
				}

				char* numParWrite = getParameter(*i, nke_num_par_write);
				if (numParWrite == NULL)
				{
					printf("Error: NUM_PARALLEL_WRITE is not specified for SRAM %s\n ", (*i)->Name());
				}

				char* rdWrConflict = getParameter(*i, nke_rdwr_conflict);
				if (rdWrConflict == NULL)
				{
					printf("Error: READ_WRITE_CONFLICT is not specified for SRAM %s\n ", (*i)->Name());
				}

				char* startAddress = getAddressRange(*i, 0);
				if (startAddress == NULL)
				{
					printf("Error: ADDRESS_RANGE is not specified properly for SRAM %s\n ", (*i)->Name());
					assert(0);
				}

				char* endAddress = getAddressRange(*i, 1);
				if (endAddress == NULL)
				{
					printf("Error: ADDRESS_RANGE is not specified properly for SRAM %s\n ", (*i)->Name());
					assert(0);
				}

				sramStartAddress = atoi(startAddress);
				sramEndAddress = atoi(endAddress);
 
				char* width = getParameter(*i, nke_storagewidth);
				if (width == NULL)
				{
					printf("Error: WIDTH is not specified for SRAM %s\n ", (*i)->Name());
					assert(0);
				}

				fprintf(fp, "\n\n\t// %s: <latency>", (*i)->Name());
				fprintf(fp, "\n\tMemoryModules[n_modules++]=(MemoryModule*)new Sram<%s>();", readLatency);
			}
			else if (strcmp(type, "CONNECTIVITY") == 0)
			{
				NodePtr connections = (*i)->Child(nke_stconnections);
				
				if (connections == NULL)
				{
					printf("Comment: No connections specified for the memory subsystem\n");
				}	 
				
				fprintf(fp, "\n");
				NodeListPtr connectionlist = connections->Children();
				NodeList::iterator j;
				for(j=connectionlist->begin(); j!= connectionlist->end(); j++)
				{
					if ((*j)->Kind() == nke_stconnection)
					{
						char* source = (*j)->Name();
						NodePtr destPtr = (*j)->Child(nke_identifier);
						char* dest = destPtr->Name();

						fprintf(fp, "\n\taddConnection(%d, %d);", getStorageNum(source), getStorageNum(dest));
					}
				}	
			}
			else if (strcmp(type, "STREAM_BUFFER") == 0)
			{
				char* stName = (*i)->Name();
				storageNames.push_back(stName);
				storageNumber++;

				char* wordsize = getParameter(*i, nke_wordsize);
				if (wordsize == NULL)
				{
					printf("Error: WORDSIZE is not specified for DCACHE %s\n", (*i)->Name());
					assert(0);
				}

				char* linesize = getParameter(*i, nke_linesize);
				if (linesize == NULL)
				{
					printf("Error: LINESIZE is not specified for DCACHE %s \n", (*i)->Name());
					assert(0);
				}

				char* numlines = getParameter(*i, nke_numlines);
				if (numlines == NULL)
				{
					printf("Error: NUM_LINES is not specified for DCACHE %s \n", (*i)->Name());
					assert(0);
				}
 
				char* readLatency = getParameter(*i, nke_read_latency);
				if (readLatency == NULL)
				{
					printf("Error: READ_LATENCY is not specified for DCACHE %s\n ", (*i)->Name());
					assert(0);
				}

				char* writeLatency = getParameter(*i, nke_write_latency);
				if (writeLatency == NULL)
				{
					printf("Error: WRITE_LATENCY is not specified for DCACHE %s\n ", (*i)->Name());
				}

				fprintf(fp, "\n\n\t// %s: <No_Lines, words_in_a_line, word_size(bytes), latency>", (*i)->Name());
				fprintf(fp, "\n\tMemoryModules[n_modules++]=(MemoryModule*)new StreamBuffer<%s, %s, %s, %s>();", numlines, linesize, wordsize, readLatency);
			}
		}
	}
	fprintf(fp, "\n}\n\n");

	// Create the initial entry point
	fprintf(fp, "\n\n\n// Implements the mapping between the address range and the memory modules");
	fprintf(fp, "\n// Returns the memory module corresponding to an address");
	fprintf(fp, "\nMemoryModule* Connectivity::get_module_for_address(long addr)");
	fprintf(fp, "\n{");
	fprintf(fp, "\n\tif(addr > %d)", sramEndAddress);
	fprintf(fp, "\n\t{");
	fprintf(fp, "\n\t\treturn MemoryModules[0];");
	fprintf(fp, "\n\t}");
	fprintf(fp, "\n\telse");
	fprintf(fp, "\n\t{");
	fprintf(fp, "\n\t\treturn MemoryModules[%d];", sramNumber);
	fprintf(fp, "\n\t}");
	fprintf(fp, "\n\treturn NULL;");
	fprintf(fp, "\n}\n\n");

	return 0;
}
*/

int generateMemorySubsystem(NodePtr storageRootNode)
{

	fprintf(fp,"\n// Initialize the memory and register (STORAGE) modules");
	//fprintf(fp,"\nvoid Connectivity :: initialize()");
	//fprintf(fp,"\n{");
	fprintf(fp,"\n\t/********* Storage *****************************/\n");

	NodeList::iterator i;
	NodeListPtr childlist = storageRootNode->Children();
	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_storage)
		{
				char* stName = (*i)->Name();

				char* stsizeName = getParameter(*i, nke_storagesize);

					//fprintf(fp,"\n\tStorage * %s;\n",stName);
					//fprintf(fp,"\t%s = new %s<MyMap%s,AddrOperand,Val>(%s);\n",stName,type,type,stsizeName);
					char* type = getStorageType(*i);

					if (!strcmp(type,"VirtualRegFile"))
					{
						fprintf(fp,"\t%s = new %s();\n",stName,type);
				
						fprintf(fp,"\tnameStr = \"%s\";\n",stName);
						fprintf(fp,"\taddName(%s, nameStr);\n",stName);
					}
		}
	}

	return 0;
}

char* getParameter(NodePtr storage, NodeKindEnum kind)
{
	char* param = NULL;

	NodeList::iterator i;
	NodeListPtr childlist = storage->Children();
	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == kind)
		{
			switch(kind)
			{
				case nke_replacementpolicy:
				{
					NodePtr policy = (*i)->Child(nke_lru);

					if (policy != NULL)
						return "LRU";
					else
						return "FIFO";
				}
				break;

				case nke_writepolicy:
				{
					NodePtr policy = (*i)->Child(nke_write_back);

					if (policy != NULL)
						return "WRITE_BACK";
					else
						return "WRITE_THROUGH";
				}
				break;

				default:
				{
					return (*i)->Name();
				}
				break;
			}
 
			break;
		}
	}

	return param;
}


char* getAddressRange(NodePtr storage, int start)
{
	// "start" 1 means start address else end address;
	 

	NodePtr ranges = storage->Child(nke_addrranges);

	if (ranges == NULL)
		return NULL;

	NodePtr range = ranges->Child(nke_addrrange);

	if (range == NULL)
	{
			printf("Error: Address Range field is NULL\n");
			assert(0);
	}

	NodeListPtr addrlist = range->Children();
	NodeList::iterator j;
	int check = 0;
	for(j=addrlist->begin(); j!= addrlist->end(); j++)
	{
		if (check == start)
			return (*j)->Name();
		check = 1;
	}		
 
	return NULL;
}

void printConnectivity(NodePtr storage, FILE * fpmem, map<MyString,int> memmap)
{
	NodePtr conns = storage->Child(nke_stconnections);
	
	NodeListPtr list1 = conns->Children();
	NodeList::iterator j;
	
	fprintf(fpmem,"BEGIN_CONNECTIVITY\n");
	for(j=list1->begin(); j!= list1->end(); j++)
	{
		
		map<MyString,int>::iterator it1;
		if ((it1 = memmap.find((*j)->Name()))!= memmap.end())
		{	
			char tmp[20];
			sprintf(tmp,"%d ",it1->second);
			fprintf(fpmem,tmp);
		}
		
		NodeListPtr list2 = (*j)->Children();
		NodeList::iterator k;
		
		for(k=list2->begin(); k!= list2->end(); k++)
		{
			map<MyString,int>::iterator it2;
			if ((it2 = memmap.find((*k)->Name()))!= memmap.end())
			{	
				char tmp[20];
				sprintf(tmp,"%d\n",it2->second);
				fprintf(fpmem,tmp);
			}
		}
	}		
	fprintf(fpmem,"END_CONNECTIVITY\n\n");
	
}

int getStorageNum(char* name)
{
	for (int i = 0; i < storageNames.size(); i++)
	{
		if (strcmp(storageNames[i], name) == 0)
			return i;
	}

	return -1;
}

int instantiatePorts(NodeList complist)
{
	NodeList::iterator i;
	for(i=complist.begin(); i!=complist.end(); i++)
	{
		MyString className;
		MyString baseClass;
		MyString ctorStr;
		char * compName = (*i)->Name();
		
		getSubType(*i,baseClass);

		if (baseClass == "PORT")
		{
			getComponentType(*i,className);
			getConstructorString(*i,ctorStr);
			
			fprintf(fp,"\tPort * %s = new %s(%s);\n", (*i)->Name(), 
				className.getStr(), ctorStr.getStr());
		
			fprintf(fp,"\tnameStr = \"%s\";\n",compName);
			fprintf(fp,"\taddName(%s, nameStr);\n\n",compName);			
			
			Portlist.push_back(*i);
		}
	}
	return 0;
}

int instantiateConnections(NodeList complist)
{
	NodeList::iterator i;
	for(i=complist.begin(); i!=complist.end(); i++)
	{
		MyString className;
		MyString baseClass;
		MyString ctorStr;
		char * compName = (*i)->Name();
		
		getSubType(*i,baseClass);

		if (baseClass == "CONNECTION")
		{
			getComponentType(*i,className);
			getConstructorString(*i,ctorStr);
			
			fprintf(fp,"\tConnection * %s = new %s(%s);\n", (*i)->Name(), 
				className.getStr(), ctorStr.getStr());

			fprintf(fp,"\tnameStr = \"%s\";\n",compName);
			fprintf(fp,"\taddName(%s, nameStr);\n\n",compName);			
			
			Connectionlist.push_back(*i);
		}
	}
	return 0;
}

int addLatchesToUnits(NodeList & unitlist)
{
	NodeList::iterator i,j;
	NodeList latchList;
	int unitnum = 0;

	for(i=unitlist.begin();i!=unitlist.end();i++)
	{
		char * compName = (*i)->Name();
		
		getLatchList(*i,latchList,nke_in_latches);
		
		unit_inlatch[unitnum].push_back(compName);

		for(j=latchList.begin();j!=latchList.end();j++)
		{
			fprintf(fp,"\t%s->addLatch(_IN_L_,%s);\n",compName,(*j)->Name());
			fprintf(fp,"\tcntrl->addLatch(_OTHER_L_,%s);\n",(*j)->Name());
			unit_inlatch[unitnum].push_back((*j)->Name());
			ConnectivityMatrix.Connect(*i,*j);
		}
		
		getLatchList(*i,latchList,nke_out_latches);

		unit_outlatch[unitnum].push_back(compName);

		for(j=latchList.begin();j!=latchList.end();j++)
		{
			fprintf(fp,"\t%s->addLatch(_OUT_L_,%s);\n",compName,(*j)->Name());
			fprintf(fp,"\tcntrl->addLatch(_OTHER_L_,%s);\n",(*j)->Name());
			unit_outlatch[unitnum].push_back((*j)->Name());
			ConnectivityMatrix.Connect(*i,*j);
		}
		
		getLatchList(*i,latchList,nke_other_latches);
		
		for(j=latchList.begin();j!=latchList.end();j++)
		{
			fprintf(fp,"\t%s->addLatch(_OTHER_L_,%s);\n",compName,(*j)->Name());
			fprintf(fp,"\tcntrl->addLatch(_OTHER_L_,%s);\n",(*j)->Name());
			ConnectivityMatrix.Connect(*i,*j);
		}

		unitnum++;

		if (latchList.size())
			fprintf(fp,"\n");	
		
		latchList.clear();
	}

	return 0;
}

int addSubCompsToLatches(NodeList & latchlist)
{
	NodeList::iterator i,j;
	NodeList subcompList;
	for(i=latchlist.begin();i!=latchlist.end();i++)
	{
		char * compName = (*i)->Name();

		getElementList(*i,subcompList,nke_subcomps);

		/*if (subcompList.size())
			fprintf(fp,"\ttoyArch->addlatch(%s);\n",compName);*/

		for(j=subcompList.begin();j!=subcompList.end();j++)
			fprintf(fp,"\t%s->addLatch(%s);\n",compName,(*j)->Name());
		
		if (subcompList.size())
			fprintf(fp,"\n");	
		
		subcompList.clear();
	}
	return 0;
}

int addSubCompsToUnits(NodeList & unitlist)
{
	NodeList::iterator i,j;
	NodeList subcompList;
	for(i=unitlist.begin();i!=unitlist.end();i++)
	{
		char * compName = (*i)->Name();


		getElementList(*i,subcompList,nke_subcomps);

		/*if (subcompList.size())
			fprintf(fp,"\ttoyArch->addUnit(%s);\n",compName);*/

		for(j=subcompList.begin();j!=subcompList.end();j++)
			fprintf(fp,"\t%s->addUnit(%s);\n",compName,(*j)->Name());
		
		if (subcompList.size())
			fprintf(fp,"\n");	
		
		subcompList.clear();
	}
	return 0;
}

int addPortsToUnits(NodeList & unitlist)
{
	NodeList::iterator i,j;
	NodeList portList;
	for(i=unitlist.begin();i!=unitlist.end();i++)
	{
		char * compName = (*i)->Name();
		
		getElementList(*i,portList,nke_ports);

		//getElementList(*i,portList,nke_connections);
		//if (!portList.size())
		for(j=portList.begin();j!=portList.end();j++)
		{
			fprintf(fp,"\t%s->addPort(%s);\n",compName,(*j)->Name());
			//fprintf(fp,"\t%s->addConnection(%s);\n",compName,(*j)->Name());
			// put a '1' in the connectivity matrix to show adjacency

			ConnectivityMatrix.Connect(*i,*j);
		}
		
		if (portList.size())
			fprintf(fp,"\n");	
		
		portList.clear();
	}
	return 0;
}

int addConnectionsToUnits(NodeList & unitlist)
{
	NodeList::iterator i,j;
	NodeList connectionList;
	for(i=unitlist.begin();i!=unitlist.end();i++)
	{
		char * compName = (*i)->Name();
		
		getElementList(*i,connectionList,nke_connections);
		//		if (!connectionList.size())
		for(j=connectionList.begin();j!=connectionList.end();j++)
		{
			fprintf(fp,"\t%s->addConnection(%s);\n",compName,(*j)->Name());
			ConnectivityMatrix.Connect(*i,*j);
		}
		
		if (connectionList.size())
			fprintf(fp,"\n");	
		
		connectionList.clear();
	}
	return 0;
}

void addDataTransStatements(NodeList & unitlist) 
{
	
	NodeList::iterator i,j;
	NodeList subcompList;
	MyString className;
	char * t;
	for(i=unitlist.begin();i!=unitlist.end();i++)
	{
		char * compName = (*i)->Name();
		
		getElementList(*i,subcompList,nke_subcomps);
		
		if (!subcompList.size())
		{
			getComponentType(*i,className);
			t = className.getStr();
			
			if (className == "LoadStoreExecuteUnit")
			{
				
				fprintf(fp,"\n\n\t// Add Memory Data Transfer Semantics to the load store writeback unit\n\t//\n");
				fprintf(fp,"\n\t%s->addDataTrans(\"IVLOAD\", integerLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"DVLOAD\", doubleLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"FVLOAD\", floatLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"QIVLOAD\", charLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"QIVLOADU\", ucharLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"HIVLOAD\", twoCharAsShortLoadFunc);",compName);
				fprintf(fp,"\n\n\t//GG (5/15/00): changed to use only signed loads, since gcc generates HIVLOADU even for signed data");
				fprintf(fp,"\n\t//was: %s->addDataTrans(\"HIVLOADU\", twoUCharAsUShortLoadFunc);\n",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"HIVLOADU\", twoCharAsShortLoadFunc);",compName);
				fprintf(fp,"\n\t");
				fprintf(fp,"\n\t%s->addDataTrans(\"IVSTORE\", integerStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"DVSTORE\", doubleStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"FVSTORE\", floatStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"QIVSTORE\", charStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"HIVSTORE\", shortAs2CharStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"QIVSTOREU\", ucharStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"HIVSTOREU\", ushortAs2UCharStoreFunc);",compName);
				fprintf(fp,"\n\t");
				//fprintf(fp,"\n\t%s->addDataTrans(\"DEFAULT\", regWriteFunc);",compName);
				fprintf(fp,"\n\t");
				fprintf(fp,"\n\n\t/// Added by PB and AVS on 05/25/01");
				fprintf(fp,"\n\t// target opcodes");
				fprintf(fp,"\n\t// Add Memory Data Transfer Semantics to the load store writeback unit");
				fprintf(fp,"\n\t//\n");
				fprintf(fp,"\n\t%s->addDataTrans(\"lw\", integerLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"l_d\", doubleLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"l_s\", floatLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"lb\", charLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"lbu\", ucharLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"lh\", twoCharAsShortLoadFunc);",compName);
				fprintf(fp,"\n\n\t//GG (5/15/00): changed to use only signed loads, since gcc generates HIVLOADU even for signed data");
				fprintf(fp,"\n\t//was: %s->addDataTrans(\"HIVLOADU\", twoUCharAsUShortLoadFunc);\n",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"lhu\", twoCharAsShortLoadFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"sw\", integerStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"s_d\", doubleStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"s_s\", floatStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"sb\", charStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"sh\", shortAs2CharStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"sbu\", ucharStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"shu\", ushortAs2UCharStoreFunc);",compName);
				fprintf(fp,"\n\t%s->addDataTrans(\"DEFAULT\", regReadFunc);",compName);
			} else if (strstr(t,"WriteBack"))
			{
				fprintf(fp,"\n\t%s->addDataTrans(\"DEFAULT\", regWriteFunc);",compName);
			} else
			{
				fprintf(fp,"\n\t%s->addDataTrans(\"DEFAULT\", regReadFunc);",compName);
			}
		}
		subcompList.clear();		
		
	}

/*
	fprintf(fp,"\n\taluop1->addDataTrans(\"DEFAULT\", regReadFunc);");
	fprintf(fp,"\n\taluop2->addDataTrans(\"DEFAULT\", regReadFunc);");
	fprintf(fp,"\n\tfloatop->addDataTrans(\"DEFAULT\", regReadFunc);");
	fprintf(fp,"\n\tldop->addDataTrans(\"DEFAULT\", regReadFunc);");
	fprintf(fp,"\n\tbr1op->addDataTrans(\"DEFAULT\", regReadFunc);");
	fprintf(fp,"\n\tsubwb1->addDataTrans(\"DEFAULT\", regWriteFunc);");
	fprintf(fp,"\n\tsubwb2->addDataTrans(\"DEFAULT\", regWriteFunc);");
	fprintf(fp,"\n\tsubwb3->addDataTrans(\"DEFAULT\", regWriteFunc);");
	fprintf(fp,"\n\tsubwb5br1->addDataTrans(\"DEFAULT\", regWriteFunc);");
	fprintf(fp,"\n\n\t// Add Memory Data Transfer Semantics to the load store writeback unit\n\t//\n");
	fprintf(fp,"\n\tsubwb4->addDataTrans(\"IVLOAD\", integerLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"DVLOAD\", doubleLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"FVLOAD\", floatLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"QIVLOAD\", charLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"QIVLOADU\", ucharLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"HIVLOAD\", twoCharAsShortLoadFunc);");
 	fprintf(fp,"\n\n\t//GG (5/15/00): changed to use only signed loads, since gcc generates HIVLOADU even for signed data");
 	fprintf(fp,"\n\t//was: subwb4->addDataTrans(\"HIVLOADU\", twoUCharAsUShortLoadFunc);\n");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"HIVLOADU\", twoCharAsShortLoadFunc);");
	fprintf(fp,"\n\t");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"IVSTORE\", integerStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"DVSTORE\", doubleStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"FVSTORE\", floatStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"QIVSTORE\", charStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"HIVSTORE\", shortAs2CharStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"QIVSTOREU\", ucharStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"HIVSTOREU\", ushortAs2UCharStoreFunc);");
	fprintf(fp,"\n\t");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"DEFAULT\", regWriteFunc);");
	fprintf(fp,"\n\t");
 	fprintf(fp,"\n\n\t/// Added by PB and AVS on 05/25/01");
 	fprintf(fp,"\n\t// target opcodes");
 	fprintf(fp,"\n\t// Add Memory Data Transfer Semantics to the load store writeback unit");
 	fprintf(fp,"\n\t//\n");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"lw\", integerLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"l_d\", doubleLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"l_s\", floatLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"lb\", charLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"lbu\", ucharLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"lh\", twoCharAsShortLoadFunc);");
 	fprintf(fp,"\n\n\t//GG (5/15/00): changed to use only signed loads, since gcc generates HIVLOADU even for signed data");
 	fprintf(fp,"\n\t//was: subwb4->addDataTrans(\"HIVLOADU\", twoUCharAsUShortLoadFunc);\n");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"lhu\", twoCharAsShortLoadFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"sw\", integerStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"s_d\", doubleStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"s_s\", floatStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"sb\", charStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"sh\", shortAs2CharStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"sbu\", ucharStoreFunc);");
 	fprintf(fp,"\n\tsubwb4->addDataTrans(\"shu\", ushortAs2UCharStoreFunc);");
	fprintf(fp,"\n\t");
	fprintf(fp,"\n\tbr1ex->addDataTrans(\"DEFAULT\", regWriteFunc);");
*/
 	fprintf(fp,"\n\t// end addition.");

	fprintf(fp,"\n\t");

}

void addBranchPredictionStatements() 
{

	fprintf(fp,"\n\n\t((BranchWriteBackSubUnit *)subwb5br1)->setPriority(_FIRST_PRIORITY_);");
	fprintf(fp,"\n\t((BranchWriteBackSubUnit *)subwb5br1)->setPos(_FIRST_POS_);");
	fprintf(fp,"\n\n");
	fprintf(fp,"\n\t((BranchWriteBackUnit *)brwb)->setLastPriority(_LAST_PRIORITY_);");
	fprintf(fp,"\n\t((BranchWriteBackUnit *)brwb)->setLastPos(_LAST_POS_);");
	fprintf(fp,"\n");

}

int addOpCodesToUnits(NodeList & unitlist)
{
	NodeList::iterator i,j;
	NodeList opcodeList;
	FILE *f;
	f = fopen("../../acesMIPSdll/bin/MIPS_OpCodeTiming.txt","w");

	if(f==NULL)
	{
		printf( "Can't open file MIPS_OpCodeTiming.txt\n");
		assert(0);
	}

	FILE *f2;
	f2 = fopen("../../acesMIPSdll/bin/SpillReloadTarget.txt","w");

	if(f2==NULL)
	{
		printf( "Can't open file SpillReloadTarget.txt\n");
		assert(0);
	}

	fprintf(f2,"spillReloadInstr\n");

	for(i=unitlist.begin();i!=unitlist.end();i++)
	{
		char * compName = (*i)->Name();
	


		getOpCodeList(*i,opcodeList);
		
		// Add Opcodes to the units
		
		char * funcName;
		char * suffix = strdup("Func");
		char opcodeStr[20];

		if (opcodeList.size())
			fprintf(f,"( UNIT %s )\n",compName);

		for(j=opcodeList.begin();j!=opcodeList.end(); j++)
		{
			
			char * tmp = (*j)->Name();
			int count=0;
			int found=0;

			while (tree_opcode2[count]) 
			{
				if (!strcmp(tree_opcode2[count],tmp))
				{
					found = 0;
					////////////////////////////////
					// multiple cycle opcode latency handling
					///////////////////////////////////////
					int found1=0;
					NodeList::iterator t;
					NodeListPtr childlist = (*i)->Children();
					for(t=childlist->begin(); t!= childlist->end(); t++)
					{

						if ((*t)->Kind() == nke_timing)
						{
							NodeList::iterator t1;
							NodeListPtr childlist1 = (*t)->Children();
							for(t1=childlist1->begin(); t1!= childlist1->end(); t1++)
							{
								char * x = (*t1)->Name();
								if (!strcmp(x,tmp))
								{
									t1++;
									if (t1!=childlist1->end())
									{
										found1=1;
										fprintf(f,"( %s %s )\n",tmp,(*t1)->Name());
										fprintf(fp,"\t%s->addOpCode(\"%s\",%sFunc,%s);\n",compName,tree_opcode2[count],tree_opcode3[count],(*t1)->Name());
										//fprintf(fp,"\t%s->addOpCode(\"%s\",%sFunc);\n",compName,tree_opcode2[count],tree_opcode3[count]);
									}
								}
							}
						}

					}
					

					///////////////////


					if (found1==0)
					{
						fprintf(f,"( %s 1 )\n",tmp);
						fprintf(fp,"\t%s->addOpCode(\"%s\",%sFunc,1);\n",compName,tree_opcode2[count],tree_opcode3[count]);
					}

					//generic opcode timing (default = 1)
					fprintf(f,"( %s 1 )\n",tree_opcode2[count-1]);
					fprintf(fp,"\t%s->addOpCode(\"%s\",%sFunc,1);\n",compName,tree_opcode2[count-1],tree_opcode3[count-1]);	
					//fprintf(fp,"\t%s->addOpCode(\"%s\",%sFunc);\n",compName,tree_opcode2[count-1],tree_opcode3[count-1]);
					///////

					if (strstr(tree_opcode2[count-1],"STORE") || strstr(tree_opcode2[count-1],"LOAD"))
						fprintf(f2,"%s => %s\n",tree_opcode2[count-1],tree_opcode2[count]);

					found = 1;
					break;
				}
				count++;
			}

			//////////////////////////////////
			// handle user defined combinedopcodes
			if (found == 0)
			{
				int cnt=0;
				while(comb_opcode[cnt])
				{
					if (!strcmp(comb_opcode[cnt],tmp))
					{
						fprintf(f,"( %s 1 )\n",tmp);
						fprintf(fp,"\t%s->addOpCode(\"%s\", nopFunc);\n",compName,tmp);
						found=1;
						break;
					}
					cnt++;
				}
			}

			//////////////////////////////////

			if (found == 0)
			{
			opcodeStr[0]='\0';
			funcName = makeLowerCase((*j)->Name());
			strcat(opcodeStr,funcName);
			strcat(opcodeStr,suffix);
			if (!strcmp(tmp,"DEBUG_PRINT"))
				fprintf(f,"( %s 1 )\n",tmp);
			else {
				fprintf(f,";( %s 1 )\n",tmp);
				printf("Target opcode %s not mapped in Tree mapping section\n",tmp);
				//assert(0);
			}

			fprintf(fp,"\t// %s->addOpCode(\"%s\",%s);\n",compName,(*j)->Name(),opcodeStr);
			delete funcName;
			}
			found = 0;
		}

		if (opcodeList.size())
			fprintf(f,";\n");			
		
		fprintf(fp,"\n");

		opcodeList.clear();
	}
	fprintf(f,";");
	fclose(f);

	fprintf(f2,"endSpillReloadInstr");
	fclose(f2);
	return 0;
}

int getPipelineRootNode(NodePtr p)
{
	if (p->Kind() == nke_pipeline_section)
	{
		PipelineRootNode = p;
		return 1; //stop traversal
	}
	return 0;
}

int CollectDTPaths(NodePtr parent, NodePtr child)
{
	switch(child->Kind())
	{
	case nke_dtpath : Dtpathlist.push_back(child);
						  DTMap[child] = parent;
						  break;
	default: ;
	}
	
	return 0;
}


void getDTType(NodePtr p, MyString & retstr)
{
	// DTMap stores the mapping betwen nke_dtpath node and nke_dtpaths node
	// The latter stores the types of datapaths
	retstr = DTMap[p]->Name();
}

NodeList getDTComps(NodePtr dtpath)
{
	NodeList retlist;
	NodeList::iterator i;
	NodeListPtr childlist = dtpath->Children();
	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_dtcomponent)
			retlist.push_back(*i);
	}
	return retlist;
}

NodePtr getTo(NodePtr dtpath)
{
	NodeList::iterator i;
	NodeListPtr childlist = dtpath->Children();
	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_to)
			return (*i);
	}
	cout << "getTo : The TO node is not present in this DT path" << endl;
	assert(0);
	return 0;
}

NodePtr getFrom(NodePtr dtpath)
{
	NodeList::iterator i;
	NodeListPtr childlist = dtpath->Children();
	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_from)
			return (*i);
	}
	cout << "getTo : The FROM node is not present in this DT path" << endl;
	assert(0);
	return 0;
}

void addDTPaths(NodeList & dtpathlist, NodeList & complist)
{
	NodeList::iterator i;
	NodePtr from, to;

	if (dtpathlist.size() == 0)
	{
		cout << "Warning: addDTPaths() - DT Paths not specified; simulator may be incorrect" << endl;
		return;
	}

	for(i=dtpathlist.begin();i!=dtpathlist.end(); i++)
	{
		from = getFrom(*i);
		to = getTo(*i);

		MyString fromBaseClass,toBaseClass;
		getSubType(from,fromBaseClass);
		getSubType(to,toBaseClass);

		NodeList DTComps = getDTComps(*i);

		if (fromBaseClass == "UNIT") {
			MyString t = to->Name();
			memlevel1.push_back(t);
			ProcessDTPath(DTComps,from,to, complist);
		}
		else if (toBaseClass == "UNIT")
		{
			reverseList(DTComps);
			MyString t = from->Name();
			memlevel1.push_back(t);
			ProcessDTPath(DTComps,to,from, complist);
		}
		else if ((fromBaseClass == "STORAGE") || (toBaseClass == "STORAGE"))
			ProcessDTPath(DTComps,from,to, complist);
		else
		{
			if((fromBaseClass == "PORT") || (toBaseClass == "CONNECTION"))
				if(IsAdjacentToUnit((st.GetSymbol(fromBaseClass.getStr()))->GetFirstDef()))
				{
					ProcessDTPath(DTComps,from,to, complist);
					continue;
				}
			if((toBaseClass == "PORT") || (fromBaseClass == "CONNECTION"))
				if(IsAdjacentToUnit((st.GetSymbol(fromBaseClass.getStr()))->GetFirstDef()))
				{
					reverseList(DTComps);
					ProcessDTPath(DTComps,to,from, complist);
					continue;
				}
			ProcessDTPath(DTComps,from,to, complist);
		}
	}
}

char * getAddString(char *inpstr)
{
	char addString[20] = "\0";
	char * prefix = strdup("add");
	strcat(addString,prefix);

	char * p = strdup(inpstr);
	*p = toupper(*p);

	if (!strcmp(p,"STORAGE"))
	{
		char ptrTo[] = "PtrTo\0";
		strcat(addString,ptrTo);
	}
	
	char * q = p+1;
	q = makeLowerCase(q);

	*(p+1) = '\0';

	strcat(addString,p);
	strcat(addString,q);

	char * retstr = strdup(addString);

	delete p,q;

	return retstr;
}

void ProcessDTPath(NodeList & compList, NodePtr start, NodePtr end, NodeList & actualcomplist)
{

	NodePtr headComp = start;
	NodePtr tailComp;
	MyString tempStr;
	MyString subTypeStr;
	char * addString;
	while(compList.size())
	{
		tailComp = getHead(compList);
		getSubType(tailComp,subTypeStr);
		addString = getAddString(subTypeStr.getStr());

		if (!(ConnectivityMatrix.Connected(headComp,tailComp)))
		{
			fprintf(fp,"\t%s->%s(%s);\n",headComp->Name(),addString,tailComp->Name());
			ConnectivityMatrix.Connect(headComp,tailComp);
		}
		headComp = tailComp;
		deleteHead(compList);
		delete addString;
	}
	tailComp = end;
	getSubType(tailComp,subTypeStr);
	addString = getAddString(subTypeStr.getStr());

	fprintf(fp,"\t%s->%s(%s);\n",headComp->Name(),addString,tailComp->Name());
	ConnectivityMatrix.Connect(headComp,tailComp);
	headComp = tailComp;
	delete addString;
	fprintf(fp,"\n");
}

NodePtr getHead(NodeList & compList)
{
	return *(compList.begin());
}

void deleteHead(NodeList & compList)
{
	compList.pop_front();
}

int getSymbolList(SymbolPtr sym)
{
	Symbollist.push_back(sym);
	return 0;
}

int IsAdjacentToUnit(NodePtr p)
{
	SymbolList::iterator i;
	for(i=Symbollist.begin();i!=Symbollist.end();i++)
	{
		MyString subType;
		if (ConnectivityMatrix.Connected((*i)->GetFirstDef(),p))
		{
			getSubType((*i)->GetFirstDef(),subType);
			if (subType.getStr() == "UNIT")
				return 1;
		}
	}
	return 0;
}

void reverseList(NodeList & nodelist)
{
	NodeList::iterator i;
	NodeList tempList;
	for(i=nodelist.begin();i!= nodelist.end(); i++)
	{
		tempList.push_front(*i);
	}
	nodelist.clear();
	for(i=tempList.begin();i!= tempList.end(); i++)
	{
		nodelist.push_back(*i);
	}

	// TODO try nodelist.reverse();
}

int getOpMapRootNode(NodePtr p)
{
	if (p->Kind() == nke_opmapping_section)
	{
		OpMapRootNode = p;
		return 1; //stop traversal
	}
	return 0;
}

int getInstrSectionRootNode(NodePtr p)
{
	if (p->Kind() == nke_instruction_section)
	{
		InstrSectionRootNode = p;
		return 1; //stop traversal
	}
	return 0;
}


int getStorageRootNode(NodePtr p)
{
	if (p->Kind() == nke_storage_section)
	{
		StorageRootNode = p;
		return 1; //stop traversal
	}
	return 0;
}

void createPipelineInfo(void)
{
	NodeListPtr locallist, locallist2, locallist3, locallist4, locallist5;
	NodeList::iterator i, j, k, k1, k2, k3, k4, k5;
	MyString unitName, unitName1, unitName2, unitName3, unitName4, unitName5;
	int found = 0, found1 = 0, counting = 0, count1 = 0, count1max = 0;
	vector<MyString> piperow[20];

	//fprintf(fp," // Adding the units corresponding to each stage to the _unitsCorrespToIthStage data member;\n\n");
	NodeListPtr childlist = PipelineRootNode->Children();
	
	if (childlist->size() == 0)
	{
		cout << "Warning: createPipelineInfo() - Pipeline section not specified; simulator may be incorrect" << endl;
		return;
	}
	
	for(i=childlist->begin();i!=childlist->end();i++)
	{
		if ((*i)->Kind() == nke_pipeline)
		{
			locallist = (*i)->Children();
			//assume there's only one 'super' PIPELINE construct
			break;
		}
	}
	
	for(i=locallist->begin();i!=locallist->end();i++)
	{
		
		//unitname will have 'fetch, decode, read_execute and wb resp at each iteration'
		unitName = (*i)->Name();
		
		for(j=childlist->begin();j!=childlist->end();j++)
		{
			if ((*j)->Kind() == nke_pipelineinfo)
			{
				unitName2 = (*j)->Name();
				if (strcmp(unitName.getStr(),unitName2.getStr())) {
				} else {
					found = 1;
					//alternate list found for unitName
					locallist2 = (*j)->Children();
					for(k=locallist2->begin();k!=locallist2->end();k++) {
						if ((*k)->Kind() == nke_alternate) {
							//fprintf(fp,"alternate list found !\n");
							locallist3 = (*k)->Children();
							break;
						}
					}
					if (locallist3->size() != 0) {
						//we have the alternate list in sortlist3
						for(k1=locallist3->begin();k1!=locallist3->end();k1++) {	
							unitName3 = (*k1)->Name();
							for(k2=childlist->begin();k2!=childlist->end();k2++) {	
								if ((*k2)->Kind() == nke_pipelineinfo)
								{
									unitName4 = (*k2)->Name();
									if (strcmp(unitName3.getStr(),unitName4.getStr())) {								
									} else {
										found1 = 1;
										//pipeline list found for entry in ALTERNATE list
										//fprintf(fp,"pipeline list found!\n");
										locallist4 = (*k2)->Children();
										count1 = counting;
										for(k3=locallist4->begin();k3!=locallist4->end();k3++) {
											if ((*k3)->Kind() == nke_pipeline) {
												locallist5 = (*k3)->Children();
												break;
											}
										}
										for(k4=locallist5->begin();k4!=locallist5->end();k4++) {
											//assign count values here
											//fprintf(fp,"assigning values\n");
											unitName5 = (*k4)->Name();
											piperow[count1].push_back(unitName5);
											count1++;
											//fprintf(fp,"((ControlUnit *)cntrl)->addUnitToIthStage(%s,%d);\n",unitName5.getStr(),count1++);
										}
									} //if (strcmp
								}
							}
						}
						if (found1 == 0) {
							//error
						} else
							found1 = 0;
						// count1max holds max depth of alternate stage
						if (count1max<count1)
							count1max=count1;
					} //if locallist3 ...
				} //if (strcmp ...
			} // if ((*j)->Kind() == nke_pipelineinfo)
		}
		if (found == 0) {
			piperow[counting].push_back(unitName);
			counting++;
			//fprintf(fp,"((ControlUnit *)cntrl)->addUnitToIthStage(%s,%d);\n",unitName.getStr(),counting++);
		} else {
			found = 0;
			counting = count1max;
			count1max=0;
		}
		
	} //for(i=locallist->begin()

	// populate global control table
	int col;
	int iloop;
	for (iloop=0;iloop<counting;iloop++)
	{
		col=0;
		for (vector<MyString>::iterator it=piperow[iloop].begin();it!=piperow[iloop].end();it++)
		{
			//fprintf(fp,"iloop = %d, col = %d\n",iloop,col);
			fprintf(fp,"\tglobalControlTable.addUnitToTable(%s,%d,%d);\n",(*it).getStr(),iloop,col++);
		}
		fprintf(fp,"\n");
	}


	int x = 0;

	while (unit_inlatch[x].size())
	{
		fprintf(fp,"\tglobalNetList.addUnit(%s, 0, 0);\n",(*(unit_inlatch[x].begin())).getStr());
		x++;
	}

	x = 0;
	fprintf(fp,"\n");

	while (unit_inlatch[x].size())
	{

		vector<MyString>::iterator ti = unit_inlatch[x].begin();
		vector<MyString>::iterator ti2;

		//fprintf(fp,"\tglobalNetList.addUnit(%s, 0, 0);\n",(*ti).getStr());

		for (ti=unit_inlatch[x].begin()+1;ti!=unit_inlatch[x].end();ti++)
		{
			int y = 0;
			while (unit_outlatch[y].size())
			{
				for (ti2=unit_outlatch[y].begin();ti2!=unit_outlatch[y].end();ti2++)
				{
					if (!strcmp((*ti2).getStr(),(*ti).getStr()))
					{
						fprintf(fp,"\tglobalNetList.setPredecessor(%s,%s);\n",(*(unit_inlatch[x].begin())).getStr(),(*(unit_outlatch[y].begin())).getStr());
						break;
					}
				}
				y++;
			}
			
		}
		
	x++;
	}

	x = 0;
	fprintf(fp,"\n");

	while (unit_outlatch[x].size())
	{

		vector<MyString>::iterator ti = unit_outlatch[x].begin();
		vector<MyString>::iterator ti2;

		for (ti=unit_outlatch[x].begin()+1;ti!=unit_outlatch[x].end();ti++)
		{
			int y = 0;
			while (unit_inlatch[y].size())
			{
				for (ti2=unit_inlatch[y].begin();ti2!=unit_inlatch[y].end();ti2++)
				{
					if (!strcmp((*ti2).getStr(),(*ti).getStr()))
					{
						fprintf(fp,"\tglobalNetList.setSuccessor(%s,%s);\n",(*(unit_outlatch[x].begin())).getStr(),(*(unit_inlatch[y].begin())).getStr());
						break;
					}
				}
				y++;
			}
			
		}
		
	x++;
	}
	
}

void createOpcodeMapping(void) 
{

	char * name1 = new char[100];
	map<char *,char *>::iterator i1,i2;
	int i;

	OpMapper.clear();

	while (tree_opcode[i]) {
		OpMapper[tree_opcode[i+1]] = tree_opcode[i];
		//fprintf(fp,"%s %d %d\n",tree_opcode[i+1],strlen(tree_opcode[i+1]),
		i+=2;
	}

		i1 = OpMapper.begin();
		i2 = OpMapper.end();
		while (i1 != i2)
		{
			i1->second = makeLowerCase(i1->second);
			strcat(i1->second,"Func");
			i1++;
		}
}

void createInstructionTemplate(void)
{
	// The part that follows from here generates this function in the file BuildSystem
	NodeListPtr slotlist;

	NodeList::iterator i,j,k;

	// childlist of Instruction section node contains nke_wordlen and nke_slotlist
	// slotlist here is the child of nke_slotlist node, and contains the list of all slots

	NodeListPtr childlist = InstrSectionRootNode->Children();
	
	if (childlist->size() == 0)
	{
		cout << "Warning: createInstructionTemplate() - Instr. Template not specified; simulator may be incorrect" << endl;
		return;
	}

	for(i=childlist->begin();i!=childlist->end();i++)
	{
		if ((*i)->Kind() == nke_slotlist)
		{
			slotlist = (*i)->Children();
			break;
		}
	}

	MyString slotTypeStr = "";
	
	int dNum=1, cNum=1, fNum=1;
	int maxslots=1;

	// specify one slot at a time - get the name of the unit & the type & slot position

	struct slotinfo
	{
	public:
		MyString slotType;
		MyString unitName;
		int slotNum;
		slotinfo(MyString & a, MyString &b, int num)
		{
			slotType = a; unitName = b; slotNum = num;
		}
		~slotinfo(){}
	};
	
	struct slotinfo * slotInfoList[MAXSLOTS];

	struct slotinfo * slotPtr;
	
	for(i=slotlist->begin();i!=slotlist->end();i++)
	{
		NodeListPtr slotattrlist = (*i)->Children();
		MyString unitName;

		int data_slot=0,control_slot=0,flow_slot=0;

		// get pointer to the unit
		for(j=slotattrlist->begin();j!=slotattrlist->end();j++)
		{
			if ((*j)->Kind() == nke_slotunit)
			{
				unitName = (*j)->Name();
				break;
			}
		}

		// get the type of the operation slot in the instruction
		
		for(j=slotattrlist->begin();j!=slotattrlist->end();j++)
		{
			if ((*j)->Kind() == nke_slottype) // begin processing the type of the slot
			{
				NodeListPtr typelist = (*j)->Children();

				for(k=typelist->begin();k!=typelist->end();k++)
				{
					if ((*k)->Kind() == nke_dataslot)
						data_slot = 1;
					if ((*k)->Kind() == nke_controlslot)
						control_slot = 1;
					if ((*k)->Kind() == nke_flowslot)
						flow_slot = 1;
				}
				
				if (data_slot)
				{
					slotTypeStr = "DATA_OP_SLOT";
					slotPtr = new slotinfo(slotTypeStr,unitName,dNum++);
					slotInfoList[maxslots++] = slotPtr;
				}
				
				if (control_slot)
				{
					slotTypeStr = "CONTROL_OP_SLOT";
					slotPtr = new slotinfo(slotTypeStr,unitName,cNum++);
					slotInfoList[maxslots++] = slotPtr;
				}
				
				if (flow_slot)
				{
					slotTypeStr = "FLOW_OP_SLOT";
					slotPtr = new slotinfo(slotTypeStr,unitName,fNum++);
					slotInfoList[maxslots++] = slotPtr;
				}
			}// endif processing the types of the slot	
		}
	}

	int l;

	// print #defines for the SLOT numbers
	int max = dNum;
	if (cNum > max)
		max = cNum;
	
	if (fNum > max)
		max = fNum;

	for(l=1;l<max;l++)
		fprintf(fp,"#define SLOT_%d %d\n",l,l-1);

	fprintf(fp,"\n");

	/*fprintf(fp,"\t// See normalInstr.h for an explanation of the foll. variables.\n\n");*/
	
	fprintf(fp,"\tsimpleVLIWInstrTemplate = new Simple_VLIW_Instr();\n");

	struct slotinfo * slotptr;
	int data_slot_count=0;
	for(l=1;l<maxslots;l++)
	{
		slotptr = slotInfoList[l];
		fprintf(fp,"\tsimpleVLIWInstrTemplate->addSlot(%s,%s,SLOT_%d);\n",
			slotptr->slotType.getStr(),slotptr->unitName.getStr(),slotptr->slotNum);
		if (!strcmp(slotptr->slotType.getStr(),"DATA_OP_SLOT"))
			data_slot_count++;

	}

	numDataSlots = data_slot_count;
	//fprintf(fp,"\n\tnumDataOpsPerInstr = NUM_DATA_OPS_PER_INSTR;\n");
	fprintf(fp,"\n\tnumDataOpsPerInstr = %d;\n",data_slot_count);
	fprintf(fp,"\tnumControlOpsPerInstr = NUM_CONTROL_OPS_PER_INSTR;\n");
	fprintf(fp,"\tnumFlowOpsPerInstr = NUM_FLOW_OPS_PER_INSTR;\n\n");

	for(l=1;l<maxslots;l++)
	{
		delete slotInfoList[l];
	}
}

void printInitialDecls(NodePtr storageRootNode)
{
	
	// Changed by Sudeep (10/2)
	// print platform dependant declarations
	//fprintf(fp,"\n#ifdef WIN32\n#pragma warning(disable:4786)\n#endif\n");
	//fprintf(fp,"\n#ifndef __UNIX__\n#include \"StdAfx.h\"\n#endif\n");
	//fprintf(fp,"\n#ifdef WIN32\n#pragma warning(disable:4786)\n#endif\n");

	// print global declarations

	fprintf(fp,"\n#include \"DerivedUnit.h\"");
	fprintf(fp,"\n#include \"DerivedLatch.h\"");
	fprintf(fp,"\n#include \"DerivedLatchData.h\"");
	fprintf(fp,"\n#include \"DerivedPort.h\"");
	fprintf(fp,"\n#include \"DerivedStorage.h\"\n");
	fprintf(fp,"\n#include \"Instruction.h\"");	
	fprintf(fp,"\n#include \"DerivedOpExecSemantics.h\"");
	fprintf(fp,"\n#include \"CompName.h\"\n");
	fprintf(fp,"\n#include \"ControlTable.h\"\n");
	fprintf(fp,"\n#include \"Connectivity.h\"\n");
	fprintf(fp,"\n#include \"AssociativeCache.h\"\n");
	fprintf(fp,"\n#include \"Dram.h\"\n\n");

	fprintf(fp,"\n#include \"ArchStats.h\"");
	fprintf(fp,"\n#include \"StatList.h\"\n");

	fprintf(fp,"\nUnit *toyArch;\n");
	fprintf(fp,"MemoryModule *mainMemory;\n");
	fprintf(fp,"Connectivity * connect;\n");
	fprintf(fp,"int __L2_LINE_SIZE__;\n\n");
	fprintf(fp,"\nMyString nameStr;\n");


	NodeList::iterator i;
	NodeListPtr childlist = storageRootNode->Children();
	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_storage)
		{
			char* type = getStorageType(*i);

//			if (!strcmp(type,"VirtualRegFile"))
//			{

				char* stName = (*i)->Name();

				//char * type has the storage type

					fprintf(fp,"Storage * %s;\n",stName);
//			}
		}
	}


	fprintf(fp,"\nextern ArchStatsDB globalArchStatsDB;\n");
	fprintf(fp,"\nextern MyString memConfigFile;\n\n");
	fprintf(fp,"__declspec (dllexport) MemoryModule *icachePtr;\n");

	fprintf(fp,"\nSimple_VLIW_Instr *simpleVLIWInstrTemplate;\n");
	fprintf(fp,"\n// Need to store a list of Objects that contain simulator wide statistics");
	fprintf(fp,"\n// For e.g. control unit with the hazard stall statistics.\n\n");

	fprintf(fp,"__declspec (dllexport) vector<BaseSimObject *> globalStatsObjects;\n");

	fprintf(fp,"typedef struct address_map_node {\n");
	fprintf(fp,"int start;\n");
	fprintf(fp,"int end;\n");
	fprintf(fp,"int module_number;\n");
	fprintf(fp,"address_map_node *next;\n");
	fprintf(fp,"} addr_map_node;\n\n");

	fprintf(fp,"addr_map_node *address_map;\n");

	// function buildSystem() starts 
	fprintf(fp,"\nvoid buildSystem(void)\n");
	fprintf(fp,"{\n");
	//fprintf(fp,"\tsimpleVLIWInstrTemplate = new Simple_VLIW_Instr();\n\n\tMyString nameStr;\n");
	//fprintf(fp,"\n\tUnitList tempGUIFetchList, tempGUIDecodeList;\n\tUnitList tempGUIOpReadList, tempGUIExecList;\n\n\tsetOpCodeToExecFuncMap();\n");
	
	//fprintf(fp,"//NOSTEMP hack\n//char type;\n//FILE* unitsFile=fopen(\"./units\",\"r\");\n");
	//fprintf(fp,"//if (unitsFile!=NULL)\n//{\n//fscanf(unitsFile,\"%%c\\n\",&type);\n//useUnitsHack=(type!='*');\n//} \n\n");

	//fprintf(fp," //\tFirst Create all the units, then all the latches, and finally the\n\t// storage, connection elements.\n");

	//fprintf(fp,"\n\t/********* Units *****************************/\n");
	//fprintf(fp,"\ttoyArch = new ArchUnit();\n");
	//fprintf(fp,"\ttoyArch->init();\n");
    //fprintf(fp,"\tMyString nameStr;\n");

    //fprintf(fp,"\tnameStr = \"MipsArch\";\n");
    //fprintf(fp,"\taddName(toyArch, nameStr);\n\n");	// print instantiation of control unit

	//fprintf(fp,"\tUnit *cntrl;\n");

	//fprintf(fp,"\tcntrl = new ControlUnit();\n");
	//fprintf(fp,"\tcntrl->init();\n");
	//fprintf(fp,"\ttoyArch->addUnit(cntrl);\n"); // ControlUnit is added to ArchUnit.

    //fprintf(fp,"\tnameStr = \"Control\";\n");
    //fprintf(fp,"\taddName(cntrl, nameStr);\n\n");	// print instantiation of control unit

	//fprintf(fp,"\ttoyArch->addUnit(cntrl);\n");

	//fprintf(fp,"\n\tglobalStatsObjects.push_back((BaseSimObject *) cntrl);\n");
}

void addStorageToCntrl(NodePtr storageRootNode)
{

	NodeList::iterator i, iconn;
	NodeListPtr childlist = storageRootNode->Children();

	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_storage)
		{
			char* type = getStorageType(*i);
			char* stName = (*i)->Name();

			if (!strcmp(type,"VirtualRegFile"))
			{
				fprintf(fp,"\t((ControlUnit *)cntrl)->addStorage(%s);\n",stName);
			} 
		}
	}
}

void creatememconfig(NodePtr storageRootNode)
{

	// we create mem.config here

	NodeList::iterator i, iconn;
	NodeListPtr childlist = storageRootNode->Children();
	char fname[] = "../../acesMIPSDll/bin/mem.config";
	vector<MyString> table, ranges;
	map<MyString,int> memmap;
	int memcount = 0;
	FILE * fpmem = fopen(fname,"w");


	if(fpmem==NULL)
	{
		printf( "Can't open file '%s'\n",fname);
		assert(0);
	}

	for(i=childlist->begin(); i!= childlist->end(); i++)
	{
		if ((*i)->Kind() == nke_storage)
		{
			char* type = getStorageType(*i);
			char* stName = (*i)->Name();

			if (!strcmp(type,"VirtualRegFile"))
			{
			} else if ((!strcmp(type,"ICACHE"))||(!strcmp(type,"DCACHE")))
			{

				char* numlines = getParameter(*i, nke_numlines);
				char* linesize = getParameter(*i, nke_linesize);
				char* associativity = getParameter(*i, nke_associativity);
				char* wordsize = getParameter(*i, nke_wordsize);
				char* accesstime = getParameter(*i, nke_accesstimes);
				char* startAddress = getAddressRange(*i, 0);
				char* endAddress = getAddressRange(*i, 1);

				char inpstr[200];
				char rangestr[200];

				memmap[stName] = memcount;
				sprintf(rangestr,"%d %s %s",memcount,startAddress, endAddress);
				sprintf(inpstr,"%d %s %s:%s:%s:%s:%s",memcount++,type,numlines,linesize,associativity,wordsize,accesstime);

				table.push_back(inpstr);
				for (vector<MyString>::iterator t = memlevel1.begin(); t != memlevel1.end(); t++)
				{
					if (!strcmp((*t).getStr(),stName))
						ranges.push_back(rangestr);
				}

			} else if ((!strcmp(type,"SRAM"))||(!strcmp(type,"DRAM")))
			{

				char* startAddress = getAddressRange(*i, 0);
				char* endAddress = getAddressRange(*i, 1);
				char* accesstime = getParameter(*i, nke_accesstimes);
				char inpstr[200];
				char rangestr[200];
				int found_t=0;

				memmap[stName] = memcount;
				sprintf(rangestr,"%d %s %s",memcount,startAddress, endAddress);
				sprintf(inpstr,"%d %s %s:%s",memcount++,type,endAddress,accesstime);

				table.push_back(inpstr);
				for (vector<MyString>::iterator t = memlevel1.begin(); t != memlevel1.end(); t++)
				{
					if (!strcmp((*t).getStr(),stName))
					{
						ranges.push_back(rangestr);
						found_t = 1;
					}
				}
				// add sram even if it is not part of connectivity
				if ((found_t == 0)&&(!strcmp(type,"SRAM")))
					ranges.push_back(rangestr);

			} else if ((!strcmp(type,"CONNECTIVITY")))
			{
				iconn = i;
			}
		}
	}

	fprintf(fpmem,"BEGIN_MEM_MODULES\n");
	for (vector<MyString>::iterator t = table.begin(); t != table.end(); t++)
	{
		fprintf(fpmem,"%s\n",(*t).getStr());

	}
	fprintf(fpmem,"END_MEM_MODULES\n\n");

	printConnectivity(*iconn,fpmem,memmap);

	fprintf(fpmem,"BEGIN_MEMORY_MAP\n");
	for (vector<MyString>::iterator t1 = ranges.begin(); t1 != ranges.end(); t1++)
	{
		fprintf(fpmem,"%s\n",(*t1).getStr());

	}
	fprintf(fpmem,"END_MEMORY_MAP\n");

}

void printUnitStatistics(NodeList & unitlist)
{
	NodeList::iterator i,j;
	NodeList subcompList;

	for(i=unitlist.begin();i!=unitlist.end();i++)
	{
		char * compName = (*i)->Name();

		getElementList(*i,subcompList,nke_subcomps);
		
		if (!subcompList.size()) // only if it is not a 'super component'
		{
			fprintf(fp,"\t%s->addStat(_NUMTIMES_);\n",compName);
			fprintf(fp,"\t%s->addStat(_TRACKNUMTIMES_);\n",compName);
			fprintf(fp,"\t%s->addStat(_PERCENTUSAGE_);\n",compName);
			fprintf(fp,"\t(globalArchStatsDB.unitStatsDB).addStat(%s, statPtr1);\n",compName);
			fprintf(fp,"\tstatToTrack = (globalArchStatsDB.unitStatsDB).getStat(%s, _NUMTIMES_);\n",compName);
			fprintf(fp,"\ttrackTimes.setStatToTrack(statToTrack);\n");
			fprintf(fp,"\t(globalArchStatsDB.trackUnitStatsDB).addStat(%s, statPtr2);\n",compName);
			fprintf(fp,"\tpercentUsage.setStatToTrack(statToTrack);\n");
			fprintf(fp,"\t(globalArchStatsDB.trackUnitStatsDB).addStat(%s, statPtr3);\n\n",compName);
		}

	subcompList.clear();		

	}
}

void printEndOfFunction(NodeList & UnitList)
{




	//fprintf(fp,"\t/***********************************************************/\n");
	//fprintf(fp,"\t/*********** Statistics Collection *************************/\n");
/*
	fprintf(fp,"\tBaseStat *statPtr1, *statPtr2, *statPtr3, *statToTrack;\n");
	fprintf(fp,"\tNumTimesStat numTimes;\n");
	fprintf(fp,"\tTrackNumTimesStat trackTimes;\n");
	fprintf(fp,"\tPercentUsageStat percentUsage;\n");

	fprintf(fp,"\tstatPtr1 = &(numTimes);\n");
	fprintf(fp,"\tstatPtr2 = &(trackTimes);\n");
	fprintf(fp,"\tstatPtr3 = &(percentUsage);\n");

	printUnitStatistics(UnitList);


	fprintf(fp,"\tcntrl->addStat(_NUMTIMES_);\n");
	fprintf(fp,"\t(globalArchStatsDB.unitStatsDB).addStat(cntrl, statPtr1);\n");

	fprintf(fp,"\tHazardStallStat hazardStall;\n");

	fprintf(fp,"\tstatPtr1 = &(hazardStall);\n");

	fprintf(fp,"\tcntrl->addStat(_HAZARDSTALL_);\n");
	fprintf(fp,"\t(globalArchStatsDB.unitStatsDB).addStat(cntrl, statPtr1);\n");
*/
	fprintf(fp,"\ncreateForwardingTable();\n");

	fprintf(fp,"\nglobalControlTable.initGlobalFUList();\n");
	fprintf(fp,"globalControlTable.createFUandRFConnTable();\n\n");
	fprintf(fp,"\n}\n");

	/*
	fprintf(fp,"extern void initializeMemory(Storage *);\n");
	fprintf(fp,"extern void initializeStackPointer(Storage *);\n\n");
	fprintf(fp,"void initializeMemory()\n{\n\tinitializeMemory(D_MEMORY);");
	fprintf(fp,"\n\tinitializeStackPointer(GPRFile);\n}\n\n");
	fprintf(fp,"extern void initializeArgc(Storage *regFile);\n");
	fprintf(fp,"extern void initializeArgv(Storage *regFile, Storage *D_MEMORY);\n\n");
	fprintf(fp,"void initStorageWithCmdLineArgs()\n");
	fprintf(fp,"{\n\tinitializeArgc(GPRFile);\n\tinitializeArgv(GPRFile, D_MEMORY);\n}\n\n");
	*/
////////////////

fprintf(fp,"\n\nvoid Connectivity::initialize()\n");
fprintf(fp,"{\n\n");

fprintf(fp,"  // open the memory config file to read the following:\n");
fprintf(fp,"  // 1. memory modules and their parameters. (between $begin_mem_modules and $end_mem_modules)\n");
fprintf(fp,"  //    syntax: module_number(int) module_type(string) module_parameters(par1:par2:par3)\n");
fprintf(fp,"  // 2. connectivity between modules (between $begin_connectivity and $end_connectivity)\n");
fprintf(fp,"  //    syntax: module_from(int) module_to(int)\n");
fprintf(fp,"  // 3. Address mapping (between $begin_address_map and $end_address_map )\n");
fprintf(fp,"  //    syntax: module_number start_address(int) end_address(int) \n");
fprintf(fp,"\n\n");

fprintf(fp,"	FILE *fp;\n");
fprintf(fp,"	char line[80];\n");
fprintf(fp,"\n");
fprintf(fp,"	fp = fopen(memConfigFile.getStr(),\"r\");\n");
fprintf(fp,"\n");
fprintf(fp,"	while (fgets(line,80,fp) != NULL) {\n");
fprintf(fp,"		if (!strcmp(line,\"BEGIN_MEM_MODULES\\n\")) break;\n");
fprintf(fp,"	}\n");
fprintf(fp,"	\n");
fprintf(fp,"	// instatiate the memory modules described in the config file\n");
fprintf(fp,"	while (fgets(line,80,fp) != NULL) {\n");
fprintf(fp,"\n");
fprintf(fp,"		if (!strcmp(line,\"\\n\")) continue;\n");
fprintf(fp,"\n");
fprintf(fp,"		if (!strcmp(line,\"END_MEM_MODULES\\n\")) break;\n");
fprintf(fp,"\n");
fprintf(fp,"		int module_number;\n");
fprintf(fp,"		char module_type[20], module_parameters[40];\n");
fprintf(fp,"		sscanf(line,\"%%d%%s%%s\",&module_number,module_type,module_parameters);\n");
fprintf(fp,"		n_modules++;\n");
fprintf(fp,"		if (!strcmp(module_type,\"DCACHE\") || !strcmp(module_type,\"dcache\")) {\n");
fprintf(fp,"			// syntax - lines:words_per_line:no_of_ways:word_size:latency\n");
fprintf(fp,"			int lines = atoi(strtok(module_parameters,\":\"));\n");
fprintf(fp,"			int words = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"			int ways = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"			int wordsize = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"			int latency = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"		\n");
fprintf(fp,"			MemoryModules[module_number]=(MemoryModule*)new AssociativeCache(lines, words, ways, wordsize, latency);\n");
fprintf(fp,"		\n");
fprintf(fp,"		} else\n");
fprintf(fp,"			if (!strcmp(module_type,\"ICACHE\") || !strcmp(module_type,\"icache\")) {\n");
fprintf(fp,"			// syntax - lines:words_per_line:no_of_ways:word_size:latency\n");
fprintf(fp,"			int lines = atoi(strtok(module_parameters,\":\"));\n");
fprintf(fp,"			int words = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"			int ways = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"			int wordsize = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"			int latency = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"		\n");
fprintf(fp,"			MemoryModules[module_number]=(MemoryModule*)new AssociativeCache(lines, words, ways, wordsize, latency);\n");
fprintf(fp,"			icachePtr = MemoryModules[module_number];\n");
fprintf(fp,"		\n");
fprintf(fp,"		} else\n");
fprintf(fp,"			if (!strcmp(module_type,\"SRAM\") || !strcmp(module_type,\"sram\")) {\n");
fprintf(fp,"				// syntax - size:latency\n");
fprintf(fp,"				int size = atoi(strtok(module_parameters,\":\"));\n");
fprintf(fp,"				int latency = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"\n");
fprintf(fp,"				MemoryModules[module_number]=(MemoryModule*)new Sram(latency);\n");
fprintf(fp,"			\n");
fprintf(fp,"			} else \n");
fprintf(fp,"				if (!strcmp(module_type,\"STREAM_BUFFER\") || !strcmp(module_type,\"stream_buffer\")) {\n");
fprintf(fp,"					// syntax - lines:words_per_line:no_of_ways:word_size:latency\n");
fprintf(fp,"					int lines = atoi(strtok(module_parameters,\":\"));\n");
fprintf(fp,"					int words = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"					int ways = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"					int wordsize = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"					int latency = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"\n");
fprintf(fp,"					MemoryModules[module_number]=(MemoryModule*)new StreamBuffer(lines, words, ways, wordsize, latency);\n");
fprintf(fp,"				} else\n");
fprintf(fp,"					if (!strcmp(module_type,\"DRAM\") || !strcmp(module_type,\"dram\")) {\n");
fprintf(fp,"					// syntax - size:latency\n");
fprintf(fp,"					int size = atoi(strtok(module_parameters,\":\"));\n");
fprintf(fp,"					int latency = atoi(strtok(NULL,\":\"));\n");
fprintf(fp,"\n");
fprintf(fp,"					MemoryModules[module_number]=(MemoryModule*)new Dram(latency);\n");
fprintf(fp,"					mainMemory = MemoryModules[module_number];\n");
fprintf(fp,"\n");
fprintf(fp,"			\n");
fprintf(fp,"				} else {\n");
fprintf(fp,"					cout << \"Panic: Invalid memory module \" << module_type  << \" specified \" << endl;\n");
fprintf(fp,"					assert(0);\n");
fprintf(fp,"				}\n");
fprintf(fp,"	}\n");
fprintf(fp,"\n");
fprintf(fp,"	while (fgets(line,80,fp) != NULL) {\n");
fprintf(fp,"		if (!strcmp(line,\"BEGIN_CONNECTIVITY\\n\")) break;\n");
fprintf(fp,"	}\n");
fprintf(fp,"	\n");
fprintf(fp,"	// instatiate the memory modules described in the config file\n");
fprintf(fp,"	while (fgets(line,80,fp) != NULL) {\n");
fprintf(fp,"\n");
fprintf(fp,"		if (!strcmp(line,\"\\n\")) continue;\n");
fprintf(fp,"\n");
fprintf(fp,"		if (!strcmp(line,\"END_CONNECTIVITY\\n\")) break;\n");
fprintf(fp,"		\n");
fprintf(fp,"		int from, to;\n");
fprintf(fp,"		sscanf(line,\"%%d%%d\",&from,&to);\n");
fprintf(fp,"		addConnection(from,to);\n");
fprintf(fp,"	}\n");
fprintf(fp,"\n");
fprintf(fp,"	while (fgets(line,80,fp) != NULL) {\n");
fprintf(fp,"		if (!strcmp(line,\"BEGIN_MEMORY_MAP\\n\")) break;\n");
fprintf(fp,"	}\n");
fprintf(fp,"\n");
fprintf(fp,"\n");
fprintf(fp,"	address_map = NULL;\n");
fprintf(fp,"	addr_map_node *tmp;\n");
fprintf(fp,"	\n");
fprintf(fp,"	while (fgets(line,80,fp) != NULL) {\n");
fprintf(fp,"\n");
fprintf(fp,"		if (!strcmp(line,\"\\n\")) continue;\n");
fprintf(fp,"\n");
fprintf(fp,"		if (!strcmp(line,\"END_MEMORY_MAP\\n\")) break;\n");
fprintf(fp,"		\n");
fprintf(fp,"		// Syntax: module_number(int), start_address(int), end_address (int)\n");
fprintf(fp,"		int module_number, address_start, address_end;\n");
fprintf(fp,"\n");
fprintf(fp,"		sscanf(line,\"%%d%%d%%d\",&module_number,&address_start,&address_end);\n");
fprintf(fp,"		tmp = (addr_map_node *)malloc(sizeof(addr_map_node));\n");
fprintf(fp,"		tmp->start = address_start;\n");
fprintf(fp,"		tmp->end = address_end;\n");
fprintf(fp,"		tmp->module_number = module_number;\n");
fprintf(fp,"		tmp->next = address_map;\n");
fprintf(fp,"		address_map = tmp;\n");
fprintf(fp,"	}\n");
fprintf(fp,"\n");
fprintf(fp,"	fclose(fp);\n");
fprintf(fp,"	// Temporarily hardcoding this variable...\n");
fprintf(fp,"	__L2_LINE_SIZE__ = 16; // Used in DRAM.h for power calculations.\n");
fprintf(fp,"\n");
fprintf(fp,"}\n");
fprintf(fp,"\n");
fprintf(fp,"\n");
fprintf(fp,"//implements the mapping between the address range and the memory modules\n");
fprintf(fp,"//returns the memory module corresponding to an address\n");
fprintf(fp,"MemoryModule* Connectivity::get_module_for_address(long addr)\n");
fprintf(fp,"{\n");
fprintf(fp,"\n");
fprintf(fp,"  int memory_module = -1;\n");
fprintf(fp,"  addr_map_node *tmp = address_map;\n");
fprintf(fp,"  while (tmp != NULL) {\n");
fprintf(fp,"  	if ((addr >= tmp->start) && (addr <= tmp->end)) {\n");
fprintf(fp,"		memory_module = tmp->module_number;\n");
fprintf(fp,"		break;\n");
fprintf(fp,"	}\n");
fprintf(fp,"	tmp = tmp->next;\n");
fprintf(fp,"  }\n");
fprintf(fp,"\n");
fprintf(fp,"  if (memory_module < 0) {\n");
fprintf(fp,"	  cout << \" Address location \"<<addr<<\" out of range (not found in address map) \" << endl;\n");
fprintf(fp,"  }	  \n");
fprintf(fp,"  return MemoryModules[memory_module];\n");
fprintf(fp,"\n");
fprintf(fp,"}\n");
fprintf(fp,"\n");
fprintf(fp,"\n");
fprintf(fp,"extern void initializeMemory(Storage *);\n");
fprintf(fp,"extern void initializeStackPointer(Storage *);\n");
fprintf(fp,"\n");
fprintf(fp,"void Connectivity::initializeMemorySubsystem()\n");
fprintf(fp,"{\n");
fprintf(fp,"	//initializeMemory(toyMem);\n");
fprintf(fp,"	for (int i = 0; i < n_modules; i++)\n");
fprintf(fp,"	{\n");
fprintf(fp,"		MemoryModules[i]->init();\n");
fprintf(fp,"	}\n");
fprintf(fp,"}\n");
fprintf(fp,"\n");
fprintf(fp,"void initializeMemory(long strtAddr, char *ldType, char *ldFileName)\n");
fprintf(fp,"{\n");
fprintf(fp,"	//initializeMemory(toyMem);\n");
fprintf(fp,"	mainMemory->initmem(strtAddr, ldType, ldFileName);\n");
fprintf(fp,"	//initializeStackPointer(IntRegFile);\n");
fprintf(fp,"}\n");
fprintf(fp,"\n");
fprintf(fp,"extern void initializeArgc(Storage *regFile);\n");
fprintf(fp,"extern void initializeArgv(Storage *regFile, Storage *toyMem);\n");
fprintf(fp,"\n");
fprintf(fp,"\n");
fprintf(fp,"\n");
fprintf(fp,"\n");
fprintf(fp,"struct slot_type{\n");
fprintf(fp,"	int nr;\n");
fprintf(fp,"	SlotType type;\n");
fprintf(fp,"};\n");
fprintf(fp,"\n");
fprintf(fp,"//the physical to virtual slot mappings\n");
fprintf(fp,"map<int,set<slot_type*>*> slot_mappings;\n");
fprintf(fp,"\n");
fprintf(fp,"//reads the slot mapping file\n");
fprintf(fp,"void read_slot_mappings(void){\n");
fprintf(fp,"  char line[1000],word[1000];\n");
fprintf(fp,"  int n;\n");
fprintf(fp,"  FILE *f;\n");
fprintf(fp,"  int nr,current_phys_slot;\n");
fprintf(fp,"  slot_type *s;\n");
fprintf(fp,"\n");
fprintf(fp,"  f=fopen(\"SlotMappings.txt\",\"r\");\n");
fprintf(fp,"\n");
fprintf(fp,"  if(f==NULL){\n");
fprintf(fp,"    printf(\"Could not open SlotMappings.txt\\n\");\n");
fprintf(fp,"    exit(1);\n");
fprintf(fp,"  }\n");
fprintf(fp,"\n");
fprintf(fp,"  while(!feof(f)){\n");
fprintf(fp,"    fgets(line,1000,f);\n");
fprintf(fp,"    if(line[0]!='#'){\n");
fprintf(fp,"      n=sscanf(line,\"%%s %%d\\n\",word,&nr);\n");
fprintf(fp,"      if(n==2){\n");
fprintf(fp,"		  if(strcmp(word,\"physical_slot\")==0){\n");
fprintf(fp,"			  current_phys_slot=nr;\n");
fprintf(fp,"			  slot_mappings[current_phys_slot]=new set<slot_type*>();\n");
fprintf(fp,"		  }\n");
fprintf(fp,"		  else if(strcmp(word,\"data\")==0){\n");
fprintf(fp,"			  //this is a data virtual slot\n");
fprintf(fp,"			  s=new slot_type();\n");
fprintf(fp,"			  s->nr=nr;\n");
fprintf(fp,"			  s->type=DATA_OP_SLOT;\n");
fprintf(fp,"			  slot_mappings[current_phys_slot]->insert(s);\n");
fprintf(fp,"		  }\n");
fprintf(fp,"		  else if(strcmp(word,\"control\")==0){\n");
fprintf(fp,"			  //this is a control slot\n");
fprintf(fp,"			  s=new slot_type();\n");
fprintf(fp,"			  s->nr=nr;\n");
fprintf(fp,"			  s->type=CONTROL_OP_SLOT;\n");
fprintf(fp,"			  slot_mappings[current_phys_slot]->insert(s);\n");
fprintf(fp,"		  }\n");
fprintf(fp,"		  else _ASSERT(0);\n");
fprintf(fp,"      }\n");
fprintf(fp,"    }\n");
fprintf(fp,"  }\n");
fprintf(fp,"  fclose(f);\n");
fprintf(fp,"}\n");
fprintf(fp,"\n");
fprintf(fp,"int isDelayedRead(BaseArgument *arg, BaseOperation *op)\n");
fprintf(fp,"{\n");
fprintf(fp,"	return false;\n");
fprintf(fp,"}\n");
/////////////////
}

void addGenericOpcodesFromFile()
{


	FILE * f;
	int res;
	char line[200];
	char fname[] = "../../acesMIPSDll/bin/genericOpCodeFile";
	char *buffer = new char[100];
	char semant[200];
	int opType;
	char classtype[200];

		char * suffix = strdup("Func");

	if((f=fopen(fname,"r"))==NULL)
	{
		printf( "Can't open file '%s'\n", fname);
		assert(0);
	}

	//fprintf(fp,"\nvoid setGenericOpCodeToExecFuncMap()\n{\n");

	int cnt=0;

	while (fgets(line,200,f) != NULL)
	{
	  res=sscanf(line, "%s %d %s %s", buffer, &opType, semant, classtype);
	  
	  if (res==3) {
		//fprintf(fp, "\tOpCodeList::setExecFunc(\"%s\", &%sFunc);\n",buffer,semant);
		char * ts = new char[40];
		sprintf(ts," %d",opType);
	  	OpMapperGenericOpFile[buffer] = strcat(semant,ts);
		OpNumberMapper[buffer] = opType;
		delete ts;
	  }
	  else if(res==4) {
		char * ts = new char[40];
		sprintf(ts," %d",opType);
		//fprintf(fp, "\tOpCodeList::setExecFunc(\"%s\", &%sFunc);\n",buffer,classtype);
		OpMapperGenericOpFile[buffer] = strcat(classtype,ts);
		OpNumberMapper[buffer] = opType;
		delete ts;
	  }

	}

}

void addTargetOpcodesFromTreeMapping(NodeList & unitlist)
{

	NodeList opcodeList;
	map<MyString, MyString>::iterator i1, i2;
	map<MyString, int>::iterator i3, i4;

	char fname2[] = "../../acesMIPSDll/bin/targetOpCodeFile";

	FILE *f;

	if((f=fopen(fname2,"w"))==NULL)
	{
		printf( "Can't open file '%s'\n", fname2);
		assert(0);
	}

	int count = 1;
	int count2 = 1;
	int found = 0;

			while (tree_opcode2[count]) 
			{

					i1 = OpMapperGenericOpFile.begin();
					i2 = OpMapperGenericOpFile.end();
					
					while (i1 != i2) 
					{
						if (i1->first == tree_opcode2[count-1])
						//if (!strcmp(i1->first,tree_opcode2[count-1]))
						//i1=OpMapperGenericOpFile.find(tree_opcode2[count-1]);
						//if (i1!=OpMapperGenericOpFile.end())
						{
							found = 1;
							MyString t = (i1->second).getStr();
							char * x = strtok(t.getStr()," ");
							//fprintf(fp,"\tOpCodeList::setExecFunc(\"%s\",&%sFunc);\n",tree_opcode2[count],x);
							x = strtok(NULL," |\t");
							fprintf(f,"%s %s\n",tree_opcode2[count],x);
							break;
						}
							i1++;
					}

				count+=2;
			}


			count = 0;
			// handle user defined combined opcode
			while (comb_opcode[count])
			{

					i1 = OpMapperGenericOpFile.begin();
					i2 = OpMapperGenericOpFile.end();
					
					while (i1 != i2) 
					{
						if (i1->first == comb_gen_opcode[count])
						{
							MyString t = (i1->second).getStr();
							char * x = strtok(t.getStr()," ");
							x = strtok(NULL," |\t");
							fprintf(f,"%s %s\n",comb_opcode[count],x);
							break;
						}
							i1++;
					}
				count++;
			}

	fclose(f);

}

void printEndOfFunction2()
{

	fprintf(fp,"\nvoid setOpCodeToExecFuncMap()\n{\n\tsetGenericOpCodeToExecFuncMap();\n\tsetTargetOpCodeToExecFuncMap();\n}");
	fprintf(fp,"\n\nvoid readOpCodeAndTimingFile(MyString & s)\n{\n\tFilePointer fp;\n\ttry");
	fprintf(fp,"\n\t{\n\t\tfp.open(s, \"r\");\n\t}\n\tcatch (CouldNotOpenFileException e)\n\t");
	fprintf(fp,"{\n\t\tcout << e;\n\t\tassert(0);\n\t}\n\n\n\tchar line[200];\n\tchar *word;\n");
	fprintf(fp,"\n\tUnit *currUnit;\n\tint opCodeIndex, timing;\n\n\twhile (fgets(line, 200, fp) != NULL)\n");
	fprintf(fp,"\n\t{\n\t\tif (line[0] == ';') // comment ignore the rest of the line\n");
	fprintf(fp,"\t\t\tcontinue;\n\n\t\t// first character is \"(\". ignore it.\n\t\t//");
	fprintf(fp,"\n\t\tstrtok(line, \" \");\n\n\t\t// the next string can be UNIT or opcode");
	fprintf(fp,"\n\t\t//\n\t\tword = strtok(NULL, \" \\n\");\n\n\t\tif (strcmp(word, \"UNIT\") == 0) // error condition.");
	fprintf(fp,"\n\t\t{\n\t\t\tword = strtok(NULL, \" \");\n\n\t\t\t// Now, word contains the name of the unit.");
	fprintf(fp,"\n\t\t\t//\n\t\t\tcurrUnit = (Unit *)getComp(word);\n\t\t}\n\t\telse // must be the opcodes for the units");
	fprintf(fp,"\n\t\t{\n\t\t\topCodeIndex = globalOpCodeTable.getIndex(word);\n\t\t\ttiming = _SINGLE_CYCLE_TIMING_;");
	fprintf(fp,"\n\n\t\t\tword = strtok(NULL, \" \");\n\n\t\t\tif (word != NULL) // then, we have the timing.");
	fprintf(fp,"\n\t\t\t{\n\t\t\t\ttiming = atoi(word);\n\t\t\t}\n\t\t\tcurrUnit->addOpCode(opCodeIndex, timing);");
	fprintf(fp,"\n\t\t}\n\t}\n}\n\n\nstruct slot_type{\n\tint nr;\n\tSlotType type;\n};\n\n");
	fprintf(fp,"//the physical to virtual slot mappings\n\nmap<int,set<slot_type*>*> slot_mappings;");
	fprintf(fp,"\n\nint isDelayedRead(BaseArgument *arg, BaseOperation *op)\n{\n\treturn false;\n}\n");

}