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
// $Id: FilePointer.h,v 1.6 1998/03/27 00:01:10 wtang Exp $
//
// File:  		FilePointer.h
// Created:		Mon Nov 4, 96
// Last modified: 	Tue Nov 5, 96
// Author:		S. Ashok Halambi
// Email:		ahalambi@ics.uci.edu, ilp@ics.uci.edu
//
//   
// Module: include
#ifndef WIN32
#include <assert.h>
#endif

// This file contains the definition for the FilePointer class.
// Basically, this class encapsulates the procedure for opening (and
// closing) a file.
// Let us see why we need a class to open and close files.
// A file is a resource. Whenever a function acquires a new resource
// (opens a file), it is important that the resource is properly
// released (i.e. the file is closed).
// See "The C++ Programming Language" by Bjarne Stroustrup, Chapter 9,
// pg. 308 (in Exception Handling), 2nd edition for an explanation
// with examples.
// By using a class object, we can make sure that fclose is called
// whenever that object is destroyed. (See the constructors and
// destructors of this class for a better understanding).
//
// Another reason for making this a class is that we can handle
// erroneous conditions (failure in fopen) in a methodical manner.
// Since fopen is called only inside this class, we do not have to
// throw exceptions all over the place (which we would have to do if
// fopen was being called at different places in the code).
//
// This class has two data members, a file pointer and a string which
// contains the file name. The exception CouldNotOpenFile is thrown
// whenever fopen returns a failure.
// Also _countNumFilePtrs provides a way in which to keep track of
// the number of files opened. (Actually it keeps track of the number
// of FilePointer objects created).
//
// Shown below are examples of how to open files (and use FilePointer
// objects) :
//
/*
someFuntion() // This is just an example function.
{
      try
	 {
	    FilePointer f("inpfile", "r");
	    int i;
	    fscanf(f, "%d", &i);
	    cout << i;
	 }
      catch (CouldNotOpenFileException e)
	 {
	    // Insert your own code
	 }
            
      FilePointer f2;
      
#ifdef COUNT_NUMFILEPTRS
      cout << "Number of file pointers:";
      cout <<  FilePointer::numFilePtrs();
#endif
   
   try 
      {
	 f2.open("infile", "r");
      }
   catch (CouldNotOpenFileException e)
      {
	 cout << e;
	 assert(0);
      }
//
//  Use f2 in the same way as f
//
}
*/
//
// Notice that if we use the constructor/initializer function (as in
// the case for f) we have to include all the statements that make use
// of f inside the try block. (Because of scoping).
// I recommend that the second method of first declaring a FilePointer
// and then initializing it (as in the case of f2) be used because it
// makes for much easier understanding of the code.
//
// The drawback of doing all this is performance. Now, opening a
// file,instead of being just a function call, has become an elaborate
// procedure of constructing a class, etc. In this project, we will
// not be opening too many files for this to effect performance. But,
// I think that we will be opening enuff files to justify the effort
// put here. I do not expect anybody working on/with this piece of
// code to use fopen.
//
// Additional observations:
// I have not encapsulated fscanf, fprintf etc in the same manner.
// This is because doing it will result in a significant performance
// drop-off. Also, the only use of such a class object would be to
// catch exceptions elegantly. (i.e. they are not used for acquiring
// resources). 
// Other resources that can be "protected" in the same manner:
//  memory allocation, access control locking, etc.
//
// Here's the actual code (if you are interested anymore :-) ):

#ifndef __FILEPOINTER_H__
#define __FILEPOINTER_H__

#include "GlobalIncludes.h"
#include "CouldNotOpenFileException.h"
#include "MyString.h"

#define COUNT_NUMFILEPTRS
//
// This file is not the best place to define COUNT_NUMFILEPTRS.
// I expect that this will be defined during the development/coding
// phase of this project. In the final/release version, this will not
// be defined.
// I'd love to see it moved to a place where it is easy to find/not
// forgotten. 

class FilePointer
{
 private:
   FILE *_fptr;
   char *_filename;
   
#ifdef COUNT_NUMFILEPTRS
   static int _numFilePtrs;
#endif

 public:
   FilePointer()
      {
	 _filename = _NULL_;

#ifdef COUNT_NUMFILEPTRS
	 _numFilePtrs++;
#endif
      }
   
   FilePointer(const char *filename, const char *type)
      {
	 _filename = strdup(filename);
	 if ((_fptr = fopen(filename, type)) == _NULL_)
	    throw CouldNotOpenFileException(_filename); 
//
// This throw returns control back to the function that called it.
// The handler for this exception should be provided by that function.

#ifdef COUNT_NUMFILEPTRS
	 _numFilePtrs++;
#endif
      }

   FilePointer(MyString & s, const char *type)
      {
	 _filename = strdup(s.getStr());
	 if ((_fptr = fopen(_filename, type)) == _NULL_)
	    throw CouldNotOpenFileException(_filename); 
//
// This throw returns control back to the function that called it.
// The handler for this exception should be provided by that function.

#ifdef COUNT_NUMFILEPTRS
	 _numFilePtrs++;
#endif
      }

// Hopefully, the constructor written below will never be called. I
// have provided it in case we want to use a piece of already existing
// code. In that case, this constructor provides us with a way to
// convert an already existing FILE * to a FilePointer type (which can
// then replace the FILE * variable in the code.
//
   FilePointer(FILE *filePtr)
      {
	 _fptr = filePtr;
	 _filename = _NULL_;
	 
#ifdef COUNT_NUMFILEPTRS
	 _numFilePtrs++;
#endif
      } 

   ~FilePointer()
      {
	 fclose(_fptr);
	 if (_filename!=NULL)
	   free(_filename);
	 
#ifdef COUNT_NUMFILEPTRS
	 _numFilePtrs--;
#endif
      }

// The method open is intended to be used to open any file. It has the
// same structure as the fopen call e.g. open("infile", "r")
//
   void open(char *filename, char *type)
      {
	 if (_filename!=NULL)
	   delete _filename;
	 _filename = strdup(filename);
	 if ((_fptr = fopen(filename, type)) == _NULL_)
	    throw CouldNotOpenFileException(_filename); 
      }
   
// The method open is intended to be used to open any file. It has the
// same structure as the fopen call e.g. open("infile", "r")
//
   void open(MyString & s, char *type)
      {
	 if (_filename!=NULL)
	   delete _filename;
	 _filename = strdup(s.getStr());
	 if ((_fptr = fopen(_filename, type)) == _NULL_)
	    throw CouldNotOpenFileException(_filename); 
      }

   void close()
   {
	   if (_fptr == _NULL_) return;
	   fclose(_fptr);
	   if (_filename!=NULL)
		   free(_filename);
	   _filename = NULL;
   }

   operator FILE*()
      {
	 return _fptr;
      }

   FILE* file_ptr()
      {
	 return _fptr;
      }

// This method returns the number of FilePointers in existence.
//
#ifdef COUNT_NUMFILEPTRS
   static int numFilePtrs()
      {
	 return _numFilePtrs;
      }
#endif

// Method for checking equivalency between two file pointers.
// Two file pointer objects are defined to be equivalent if they both
// have the same file names.
// 
   friend int operator == (const FilePointer & a, const FilePointer & b)
      {
	 if (strcmp(a._filename, b._filename) == 0) // case equal
	    {
	       return _YES_;
	    }
	 return _NO_;
      }
   
// Operator << to display/print the file pointer.
// The file pointed to is displayed.
// e.g. File: infile
//
   friend ostream& operator << (ostream & out, const FilePointer & s)
      {
	 out << "File: ";
	 out << s._filename;
	 return out;
      }
   
};

#endif
