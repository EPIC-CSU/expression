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
 *file  : DataConversion.cpp                                       *
 *authors : gabi                                             *
 *last modified :                                            *
 *                                                           *
 *contents:Data conversion for memory read/writes            *
 *comments:-                                                 *
 *compiled with : g++                                        *
 *for more info :                                            *
 *************************************************************/

#include "stdafx.h"

#ifdef WIN32
#pragma warning(disable:4786)
#endif


#include "DataConversion.h"
#include "AddrData.h"



//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 


//if want PC endianness, comment out this guy
#define MIPS_ENDIANNESS



int get_final_size(VALUE_TYPE t){
	switch(t){
	case _UCHAR:
	case _CHAR:
		return 1;
	case _USHORT:
	case _SHORT:
		return 2;
	case _UINT:
	case _INT:
		return 4;
	case _ULONG:
	case _LONG:
		return 4;
	case _FLT:
		return 4;
	case _DBL:
		return 8;
	default:
		_ASSERT(0);
	}
	return -1;
}

VALUE_TYPE get_intermediate_type(int size,VALUE_TYPE final_t){
	switch(final_t){
	case _UCHAR:
	case _USHORT:
	case _UINT:
	case _ULONG:
		switch(size){
		case 1: return _UCHAR;
		case 2: return _USHORT;
		case 4: return _UINT;
		default: _ASSERT(0);
		}
	case _CHAR:
	case _SHORT:
	case _INT:
	case _LONG:
		switch(size){
		case 1: return _CHAR;
		case 2: return _SHORT;
		case 4: return _INT;
		default: _ASSERT(0);
		}
	case _FLT:
		switch(size){
		case 4: return _FLT;
		case 8: return _DBL;
		}
		break;
	case _DBL:
		switch(size){
		case 4: return _FLT;
		case 8: return _DBL;
		}
		break;
	default:
		_ASSERT(0);
	}
	// this should never be taken.
	//
	_ASSERT(0);
	return _INT;
}


//converts an array of bytes into a Val
//byte_array: input, the array of bytes
//size: input, the number of valid bytes in the array of bytes (may be different than size of t)
//type_enum: input, the type to convert the array of bytes to
//val: output, the val created from the array of bytes
void ArrayToData(unsigned char byte_array[MAX_ARRAY_SIZE],int initial_size,VALUE_TYPE final_t,Val &val){
	char r_char;
	unsigned char r_uchar;
	short r_short;
	unsigned short r_ushort;
	int r_int;
	unsigned int r_uint;
	long r_long;
	unsigned long r_ulong;
	float r_float;
	double r_double;
	unsigned char temp_byte_array[MAX_ARRAY_SIZE];
	int i;
	int final_size;
	VALUE_TYPE intermidate_t;

	//initialize temp_byte_array
	for(i=0;i<MAX_ARRAY_SIZE;i++){
		temp_byte_array[i]=0;
	}


#ifdef MIPS_ENDIANNESS
	//switch the endianness
	for(i=0;i<initial_size;i++){
		temp_byte_array[i]=byte_array[initial_size-1-i];
	}
#else
	for (i=0; i<initial_size;i++)
	{
		temp_byte_array[i] = byte_array[i];
	}
#endif
 

	//get the final size (the initial_size of the final type)
	final_size=get_final_size(final_t);

	//get the intermidiate type
	intermidate_t=get_intermediate_type(initial_size,final_t);

	switch(intermidate_t){
	case _CHAR:
		_ASSERT(initial_size==1);
		r_char=(char)temp_byte_array[0];
		val.changeValue(r_char);
		break;
	case _UCHAR:
		_ASSERT(initial_size==1);
		r_uchar=(unsigned char)temp_byte_array[0];
		val.changeValue(r_uchar);
		break;
	case _SHORT:
		_ASSERT(initial_size==2);
		r_short=*((short*)temp_byte_array);
		val.changeValue(r_short);
		break;
	case _USHORT:
		_ASSERT(initial_size==2);
		r_ushort=*((unsigned short*)temp_byte_array);
		val.changeValue(r_ushort);
		break;
	case _INT:
		_ASSERT(initial_size==4);
		r_int=*((int*)temp_byte_array);
		val.changeValue(r_int);
		break;
	case _UINT:
		_ASSERT(initial_size==4);
		r_uint=*((unsigned int*)temp_byte_array);
		val.changeValue(r_uint);
		break;
	case _FILE_PTR:
		_ASSERT(initial_size==4);
		r_uint=*((unsigned int*)temp_byte_array);
		val.changeValue(r_uint);
		val._valType = _FILE_PTR;
		break;
	case _LONG:
		_ASSERT(initial_size==4);
		r_long=*((long int*)temp_byte_array);
		val.changeValue(r_long);
		break;
	case _ULONG:
		_ASSERT(initial_size==4);
		r_ulong=*((unsigned long int*)temp_byte_array);
		val.changeValue(r_ulong);
		break;
	case _FLT:
		_ASSERT(initial_size==4);
		r_float=*((float*)temp_byte_array);
		val.changeValue(r_float);
		break;
	case _DBL:
		_ASSERT(initial_size==8);
		r_double=*((double*)temp_byte_array);
		val.changeValue(r_double);
		break;
	default:
		_ASSERT(0);
		break;
	}
	if(intermidate_t!=final_t)
		val.changeType(final_t);
}


//converts a Val into an array of bytes
//val: input, the val to be converted
//byte_array: output, the array of bytes generated
//size: output, the number of valid bytes in the array of bytes generated
void DataToArray(Val &val,unsigned char byte_array[MAX_ARRAY_SIZE],int &size){
	unsigned char temp_byte_array[MAX_ARRAY_SIZE];
	int i;

	//first clear these arrays
	for (i = 0; i < MAX_ARRAY_SIZE; i++)
	{
		temp_byte_array[i] = (unsigned char)0;
		byte_array[i] = (unsigned char)0;
	}

	switch(val.getType()){
	case _CHAR:
		temp_byte_array[0]=(char)val._as.byte;
		size=1;
		break;
	case _UCHAR:
		temp_byte_array[0]=(unsigned char)val._as.byte;
		size=1;
		break;
	case _SHORT:
		*((short*)temp_byte_array)=(short)val._as.short_int;
		size=2;
		break;
	case _USHORT:
		*((unsigned short*)temp_byte_array)=(unsigned short)val._as.short_int;
		size=2;
		break;
	case _INT:
		*((int*)temp_byte_array)=(int)val._as.integer;
		size=4;
		break;
	case _UINT:
	case _FILE_PTR:
		*((unsigned int*)temp_byte_array)=(unsigned int)val._as.uinteger;
		size=4;
		break;
	case _LONG:
		*((long*)temp_byte_array)=(long)val._as.integer;
		size=4;
		break;
	case _ULONG:
		*((unsigned long*)temp_byte_array)=(unsigned long)val._as.uinteger;
		size=4;
		break;
	case _FLT:
		*((float*)temp_byte_array)=(float)val._as.flt;
		size=4;
		break;
	case _DBL:
		*((double*)temp_byte_array)=(double)val._as.dbl;
		size=8;
		break;
	default:
		_ASSERT(0);
		break;
	}
	
#ifdef MIPS_ENDIANNESS
	//switch the endianness
	for(i=0;i<size;i++){
		byte_array[i]=temp_byte_array[size-1-i];
	}
#else
	for (i=0; i<size;i++)
	{
		byte_array[i] = temp_byte_array[i];
	}
#endif
}



