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

#include "my_ostream.h"

my_ostream::my_ostream(FP fp) : streambuf()
{
 m_buffersize=0;
 m_fp=fp;
}

int my_ostream::sync()
{ 
 m_buffer[m_buffersize]=0;
 (*m_fp)(&m_buffer[0],m_buffersize);
 m_buffersize=0;
 return streambuf::sync();
 //return 0;
}

ostream& my_ostream::endl(ostream& out)
{
 out.put('\r'); //CEdit needs both a CR and LF
 out.put('\n');
 out.flush();
 return out;
}

my_ostream::~my_ostream()
{
 streambuf::sync();
}

int my_ostream::overflow(int c)
{
 if (m_buffersize>BUFSIZE)
 {
  m_buffer[m_buffersize]=0;
  (*m_fp)(&m_buffer[0],m_buffersize);
  m_buffersize=0;
  m_buffer[m_buffersize++]=c;
 }
 else
 {
  //if (c==10) //expand LF to CR+LF
  //{
  // m_buffer[m_buffersize++]=13;
  // m_buffer[m_buffersize++]=10;
  //}
  //else
  //{
   m_buffer[m_buffersize++]=c;
  //}
 }
 return 0;
}

int my_ostream::underflow(void)
{
 return 0;
}
