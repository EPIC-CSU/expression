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
#ifndef __ROUTRARMPROPERTY_H__
#define __ROUTRARMPROPERTY_H__

#include "GlobalIncludes.h"
#include "../../systemDll/NewbaseClassDll/AddrData.h"

#include "STLIncludes.h"
#include "PropertyIncludes.h"
#include "BaseRoutineProperty.h"

#include "BaseArgument.h"
#include "RegisterFileList.h"

const int _INITIALIZED_ = 987;
const int _RSSAIZED_ = 876;

extern RegisterFileList *globalRegFileList;
extern int doneRA;

class RoutParmProperty : public BaseRoutineProperty {

	Vector<VALUE_TYPE> _callParmsTypeList;
	//
	// This vector holds the type of each call parameter.

	ArgList _callParms;
	//
	// This vector holds each of the call input parameters.
	//
	// The order in the vector is the same as the order of the arguments in the parameter list.

	VALUE_TYPE _returnParmType;
	//
	// This object holds the type of the return parameter.

	ArgList _returnParms;
	//
	// This vector holds all the return parameters.
	//
	// As the routine may have more than one exit point, we need to maintain a vector of all the 
	// (RSSA) versions of the return argument.

	int _callParmStatus; // One of _INITIALIZED_, _RSSAIZED_, or _INVALID_
	int _returnParmStatus; // One of _INITIALIZED_, _RSSAIZED_, or _INVALID_

	// Before SSAization, this property (if present) indicates the MIPS physical registers that 
	// form the input and output paramters for the routine.

	// After SSAization, this property indicates the virtual registers that contain
	// the input and output parameters. 

	// This property is created (_INITAILZED_) right after IList creation,
	// then set (_RSSAIZED_) after RSSA, and finally used during Register Allocation (RA)

public:
   RoutParmProperty() : _callParmsTypeList(), _callParms(), _returnParmType(_ANY), 
	   _returnParms(), _callParmStatus(_INVALID_), _returnParmStatus(_INVALID_){ }
   RoutParmProperty(const RoutParmProperty& p) : _callParmsTypeList(p._callParmsTypeList), 
	   _callParms(p._callParms), _returnParmType(p._returnParmType), _returnParms(p._returnParms), 
	   _callParmStatus(p._callParmStatus), _returnParmStatus(p._returnParmStatus) { }
   RoutParmProperty& operator=(RoutParmProperty& p) { 
      if ( *this == p )
         return *this;

	  _callParmsTypeList = p._callParmsTypeList;
	  _callParms = p._callParms;
	  _returnParmType = p._returnParmType;
	  _returnParms = p._returnParms;
	  _callParmStatus = p._callParmStatus;
	  _returnParmStatus = p._returnParmStatus;

      return *this;
   }

   virtual ~RoutParmProperty() 
   {
	   ArgListIter i;

	   for (i = _callParms.begin(); i != _callParms.end(); i++)
	   {
		   if ((*i) != NULL)
			   delete (*i);
	   }
	   for (i = _returnParms.begin(); i != _returnParms.end(); i++)
	   {
		   if ((*i) != NULL)
			   delete (*i);
	   }
	   _callParms.clear(); 
	   _returnParms.clear(); 

	   _callParmsTypeList.clear();
   }
   
   BaseRoutineProperty* copy() const
   {
	   BaseRoutineProperty *retPtr;
	   retPtr = new RoutParmProperty(*this);
	   
	   // Added by SAH on Fri, Dec 08 2000
	   //
	   cout << "Warning: RoutParmProperty copy does not work if it is not an empty property" << endl;

	   return retPtr;
   }

   int is(PropertyNames name) { if ( name==_ROUTPARM_) return _YES_; return _NO_; }

   void addCallParmType(VALUE_TYPE a) { _callParmsTypeList.push_back(a); }

   void addCallParm(BaseArgument *arg) 
   {
	   // first check to see if the argument has already been added.
	   //
	   ArgListIter i;

	   int argID = arg->getUniqID();

	   for (i = _callParms.begin(); i != _callParms.end(); i++)
	   {
		   if ((*i)->getUniqID() == argID) return;
	   }
	   _callParms.push_back(arg->copy()); 
   }

   void addReturnParmType(VALUE_TYPE a) { _returnParmType = a; }

   void addReturnParm(BaseArgument *arg) 
   {
	   // first check to see if the argument has already been added.
	   //
	   ArgListIter i;

	   int argID = arg->getUniqID();

	   for (i = _returnParms.begin(); i != _returnParms.end(); i++)
	   {
		   if ((*i)->getUniqID() == argID) return;
	   }
	   _returnParms.push_back(arg->copy()); 
   }

   ArgList & getCallParms() { return _callParms; }
   ArgList & getReturnParms() { return _returnParms; }

   int hasCallParm(int symId)
   {
	   ArgListIter i;

	   for (i = _callParms.begin(); i != _callParms.end(); i++)
	   {
		   // Need to check for sp.
		   //
		   if( !doneRA )
		   {
			   if ((*i)->getIDWithoutSSA() == globalRegFileList->getID(0, "gsp"))
				   continue;
		   }
		   else
		   {
			   assert("Target Register set is not yet specified !\n");
			   if ((*i)->getIDWithoutSSA() == globalRegFileList->getID(0, "sp"))
				   continue;
		   }
		   
		   if ((*i)->getUniqID() == symId) return _YES_;
	   }
	   return _NO_;
   }
   
   
   int hasReturnParm(int symId)
   {
	   ArgListIter i;
	   
	   for (i = _returnParms.begin(); i != _returnParms.end(); i++)
	   {
		   if ((*i) == NULL) continue;

		   if ((*i)->getUniqID() == symId) return _YES_;
	   }
	   return _NO_;
    }

   int hasCallParm(BaseArgument *arg)
   {
	   if ((arg == NULL) || (!arg->isRegisterArg())) return _NO_;

	   return (hasCallParm(arg->getUniqID()));
   }

   int hasReturnParm(BaseArgument *arg)
   {
	   if ((arg == NULL) || (!arg->isRegisterArg())) return _NO_;

	   return (hasReturnParm(arg->getUniqID()));
   }

   Vector<VALUE_TYPE> & getCallParmsTypeList() { return _callParmsTypeList; }
   VALUE_TYPE getReturnParmType() { return _returnParmType; }

   void setCallParmStatus(int status) { _callParmStatus = status; }
   void setReturnParmStatus(int status) { _returnParmStatus = status; }

   int getCallParmStatus() { return _callParmStatus; }
   int getReturnParmStatus() { return _returnParmStatus; }

   void print() {

	   Vector<VALUE_TYPE>::iterator vi;

	   cout << "Printing the Call Return/Input Parms Type List: " << endl;
	   cout << " " << _returnParmType;
	   for (vi = _callParmsTypeList.begin(); vi != _callParmsTypeList.end(); vi++)
	   {
		   cout << " " << (*vi);
	   }
	   cout << endl;

	   ArgListIter i1;

	   cout << "Printing the Call Input Parms : " << endl;
	   for (i1 = _callParms.begin(); i1 != _callParms.end(); i1++)
	   {
		   cout << "	" << (*i1) << endl;
	   }
	   
	   cout << "Printing the Call Return Parms : " << endl;
	   for (i1 = _returnParms.begin(); i1 != _returnParms.end(); i1++)
	   {
		   cout << "	" << (*i1) << endl;
	   }
   }

   void print(ostream &out) {
	   Vector<VALUE_TYPE>::iterator vi;

	   out << "Printing the Call Return/Input Parms Type List: " << endl;
	   out << " " << _returnParmType;
	   for (vi = _callParmsTypeList.begin(); vi != _callParmsTypeList.end(); vi++)
	   {
		   out << " " << (*vi);
	   }
	   out << endl;

	   ArgListIter i1;

	   out << "Printing the Call Input Parms : " << endl;
	   for (i1 = _callParms.begin(); i1 != _callParms.end(); i1++)
	   {
		   out << "	" << (*i1) << endl;
	   }
	   
	   out << "Printing the Call Return Parms : " << endl;
	   for (i1 = _returnParms.begin(); i1 != _returnParms.end(); i1++)
	   {
		   out << "	" << (*i1) << endl;
	   }
   }
};

#endif
