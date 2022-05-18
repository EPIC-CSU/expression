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
//these defines control the appearance of the DOT-generated graphs

#define BOX_WIDTH_VAL 1.4
#define BOX_HEIGHT_STEP_VAL 0.15

#define FONT_SIZE_VAL -8
#define FONT_NAME_VAL "Small Fonts" //"Haettenschweiler" //"MS Sans Serif" 

//uncomment as needed
#define GRAPH_DEBUG

#ifdef GRAPH_DEBUG 

extern float BOX_WIDTH;
extern float BOX_HEIGHT_STEP;

extern int FONT_SIZE;
extern char FONT_NAME[];

#else

#define BOX_WIDTH BOX_WIDTH_VAL
#define BOX_HEIGHT_STEP BOX_HEIGHT_STEP_VAL

#define FONT_SIZE FONT_SIZE_VAL
#define FONT_NAME FONT_NAME_VAL

#endif