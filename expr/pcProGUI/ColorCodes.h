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

#define WHITE         RGB(255,255,255)
#define BLACK         RGB(  0,  0,  0)
#define BLUE          RGB(  0,  0,255)
#define RED           RGB(255,  0,  0)
#define GREEN         RGB(  0,255,  0)
#define YELLOW        RGB(255,255,  0)
#define MAGENTA       RGB(255,  0,255)
#define CYAN          RGB(  0,255,255)
#define ORANGE        RGB(255,128,  0)
#define HOTPINK       RGB(255,  0,128)
#define CREAM         RGB(255,255,128)
#define BROWN         RGB(128, 64,  0)
#define LITEGREEN     RGB(128,255,128)
#define MAUVE         RGB(128,128,255)
#define DIRTYYELLOW   RGB(128,128,  0)
#define BURGUNDY      RGB(128,  0, 64)
#define DARKCYAN      RGB(  0,128,128)
#define PINK          RGB(255,  0,128)
#define BLUEGRAY      RGB(128,128,192)
#define BLUEGREEN     RGB(  0,128, 64)
#define CAFFEAULAIT   RGB(188,148,131)
//#define BLACK         RGB(  0,  0,  0)
//#define BLACK         RGB(  0,  0,  0)
//#define BLACK         RGB(  0,  0,  0)
//#define BLACK         RGB(  0,  0,  0)
//#define BLACK         RGB(  0,  0,  0)

#define FUNNYGREEN    RGB( 35,114, 55)
#define FUNNYPINK     RGB(202,  2,127)
#define FUNNYMAUVE    RGB(123,  5,255)
#define FUNNYORANGE   RGB(252,118, 54)
#define FUNNYBROWN    RGB(165,116,  5)

#define FUNNYGREY     RGB(100,100,100)
#define FUNNYBLUE     RGB( 77, 77,242)
#define FUNNYBLUE2    RGB( 29,162,255)

//ILG, CFG, HTG #defines

#define BORDER_COLOR_INSTR      FUNNYGREEN
#define BORDER_COLOR_BASICBLOCK FUNNYPINK
#define BORDER_COLOR_SIMPLEHTG  FUNNYMAUVE
#define BORDER_COLOR_LOOPHTG    FUNNYORANGE
#define BORDER_COLOR_IFHTG      FUNNYBROWN

#define EDGE_COLOR_FLOW         FUNNYGREY
#define EDGE_COLOR_DU           FUNNYBLUE
#define EDGE_COLOR_UD           FUNNYBLUE2

#define HIGHLIGHT_COLOR_BACK    BLUE
#define HIGHLIGHT_COLOR_FORE    YELLOW

#define TEXT_COLOR_FORE         WHITE

//SIM #defines (fill in once decided)

//#define BORDER_COLOR_SIM_CONNECTION      DIRTYYELLOW
//...
