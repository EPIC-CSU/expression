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
 *file  :		GenTargetRegClassToRegsMapping.cpp			 *
 *authors :		Aviral Shrivastava                           *
 *last modified : 09/10/2001                                 *
 *                                                           *
 *contents:		Target Register Information Generation		 *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info : aviral@ics.uci.edu                         *
 *************************************************************/

#include "targetRegInfo.h"

int printTargetRegClassToRegsMappingFile(TargetRegInfoMap targetRegInfoMap)
{
	FILE *fptr;

	// open the file
	if ((fptr = fopen("targetRegClassToRegsMapping.txt", "w")) == NULL)
	{
		printf("Could not open file: targetRegClassToRegsMapping.txt\n");
		return 0;
	}

	// iterate over the targetRegInfoMap and print the information
	for (TargetRegInfoMap::iterator i = targetRegInfoMap.begin();
	i != targetRegInfoMap.end(); i++)
	{
		(*i).second.PrintForTargetRegClassToRegsMapping(fptr);
	}

	fclose(fptr);
	return 1;
}
