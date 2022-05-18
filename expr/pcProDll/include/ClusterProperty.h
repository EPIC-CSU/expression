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
#ifndef __CLUSTER_PROPERTY_H__
#define __CLUSTER_PROPERTY_H__

#include "PropertyIncludes.h"
#include "BaseOpProperty.h"
#include "MyLinkedList.h"
#include "BaseArgument.h"

class ClusterProp : public BaseOpProperty {
	MyLinkedList<BaseArgument*> cluster;
public:
	ClusterProp() {};
	virtual ~ClusterProp() {};
	ClusterProp(MyLinkedList<BaseArgument*>&args) {
	   	cluster.removeAll();
		MyLinkedListIterator<BaseArgument*>* iter=clusterIterForw();
		while ( iter->hasMoreElements() ) {
		   this->append(iter->currentElement());
		   iter->nextElement();
		}
		delete iter;
        }
	

	MyLinkedList<BaseArgument*>& getCluster() { return cluster;}
	void append(BaseArgument* arg) { cluster.append(arg); }

	BaseOpProperty* copy() const; 
 
	int is(PropertyNames name) {if(name==_CLUSTER_) return _YES_;return _NO_;}

	void print(ostream& out) {
		out << "the web: " << cluster << endl;
	}

	MyLinkedListIterator<BaseArgument*>* clusterIterForw() {
		return cluster.elementsForward();
	}


   	int isEq(BaseOpProperty* a) {
      	   if ( a->is(_CLUSTER_ ) ) {
         	if ( cluster.numItems() == ((ClusterProp*)a)->getCluster().numItems() ) {
            	MyLinkedListIterator<BaseArgument*>* iter = ((ClusterProp*)a)->clusterIterForw();
            	while ( iter->hasMoreElements() ) {
               	   if ( cluster.contains(iter->currentElement()) == _NO_ )
                  	return _NO_;
               	   iter->nextElement();
            	}
            	delete iter;
            	return _YES_;
         	}
      	   }
      	   return _NO_;
   	} 
};

#endif
