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

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include <map>
#include <set>

#ifndef WIN32
#include <ctype.h>
#endif

//START: MFC DEBUG NEW: THIS MUST BE AFTER ALL THE #INCLUDES IN A FILE
#ifdef WIN32
#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#endif
//END MFC DEBUG NEW: 

#include "IburgToIR.h"
extern int debugISel;
char *GET_STR_ATTR(NODEPTR_TYPE p);

typedef map <MyString,BaseArgProperty *> TempMapType;
BaseArgProperty *searchFromTempMap(TempMapType, MyString);

extern TempMapType tempToOperationMap;


//////////////////// EssentialOps ///////////////////////////////

set<BaseOperation *> _EssentialOps;

void ClearEssentialOps(void){
	_EssentialOps.clear();
}

void AddEssentialOps(BaseOperation *op){
	_EssentialOps.insert(op);
}

/////////////////// OmittedOps ///////////////////////////////////

set<BaseOperation *> _OmittedOps;

void ClearOmittedOps(void){
	_OmittedOps.clear();
}

void AddOmittedOps(BaseOperation *op){
	_OmittedOps.insert(op);
}

int ContainsOmittedOps(BaseOperation *op){
	set<BaseOperation*>::iterator i;
	if((i=_OmittedOps.find(op))==_OmittedOps.end())
		return 0;
	else
		return 1;
}


/////////////////// Iburg To Op Map //////////////////////////////

typedef map<NODEPTR_TYPE, BaseOperation *> _IburgToOpMapType;

_IburgToOpMapType _IburgToOpMap;


void AddIburgToOpMap(NODEPTR_TYPE p,BaseOperation *o){
	_IburgToOpMap[p]=o;
}

BaseOperation *GetIburgToOpMap(NODEPTR_TYPE p){
	if(_IburgToOpMap.find(p)==_IburgToOpMap.end())
		return NULL;
	else
		return _IburgToOpMap[p];
}

NODEPTR_TYPE GetOpIburgToOpMap(BaseOperation *o){
	_IburgToOpMapType::iterator i;
	for(i=_IburgToOpMap.begin();i!=_IburgToOpMap.end();i++){
		if((*i).second==o) return (*i).first;
	}
	return NULL;
}

void DeleteIburgToOpMap(NODEPTR_TYPE p){
	_IburgToOpMap.erase(p);
}

void ClearIburgToOpMap(void){
	_IburgToOpMap.clear();
}


/////////////////// Iburg To Arg Map //////////////////////////////

typedef map<NODEPTR_TYPE, BaseArgument *> _IburgToArgMapType;

_IburgToArgMapType _IburgToArgMap;


void AddIburgToArgMap(NODEPTR_TYPE p,BaseArgument *o){
	_IburgToArgMap[p]=o;
}

BaseArgument *GetIburgToArgMap(NODEPTR_TYPE p){
	if(_IburgToArgMap.find(p)==_IburgToArgMap.end())
		return NULL;
	else
		return _IburgToArgMap[p];
}

void DeleteIburgToArgMap(NODEPTR_TYPE p){
	_IburgToArgMap.erase(p);
}

void ClearIburgToArgMap(void){
	_IburgToArgMap.clear();
}

/////////////////////////////////////////////////////////////////////
/////////////////// Connect/Disconnect operations ///////////////////
/////////////////////////////////////////////////////////////////////


void DisconnectOperation(BaseOperation *op, BaseInstruction *instr){
	instr->extract(op, 1);
}

void ConnectOperation(BaseOperation *op,BaseOpSlot *slot){
	if(dynamic_cast<DataOpSlot*>(slot)){
		((DataOpSlot*)slot)->setOperation(op);
	}
	if(dynamic_cast<ControlOpSlot*>(slot)){
		((ControlOpSlot*)slot)->setOperation(op);
	}
}

////////////////////////////////////////////////////////////////////////////////////
//Data structures used in GenerateOperation to find slot and instr for an operation
////////////////////////////////////////////////////////////////////////////////////

set<BaseOperation *> _OldOps; //operations in the ORIGINAL tree (all of them). 
set<BaseOperation *> _NewOps;		//all the NEWLY created operations
set<BaseOperation *> _ReinsertedOps;	//all ops from original tree REINSERTED in IR because they are still needed. 

map<BaseOperation *,NormalInstr*> _OpToInstrMap;	//Holds the instructions where the original ops were located
map<BaseOperation *,BaseOpSlot*> _OpToSlotMap;		//Holds the slots where the original ops were located

int ContainsOperation(set<BaseOperation *> &OpsSet,BaseOperation*op){
	set<BaseOperation*>::iterator i;
	if((i=OpsSet.find(op))==OpsSet.end())
		return 0;
	else
		return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////GenerateArgument///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int isRegister(char *arg){
	if(strcmp(arg,"INT")==0) return 0;
	if(strcmp(arg,"FLOAT")==0) return 0;
	if(strcmp(arg,"ZERO")==0) return 0;
	if(strcmp(arg,"ONE")==0) return 0;
	if(strcmp(arg,"TWO")==0) return 0;
	if(strcmp(arg,"THREE")==0) return 0;
	if(strcmp(arg,"FOUR")==0) return 0;
	return 1;
}

char *prefix(char *s){
	char *p,*r=new char[strlen(s)+1];
	strcpy(r,s);
	for(p=r;*p!='\0' && !isdigit(*p);p++);
	*p='\0';
	return r;
}

char *sufix(char *s){
	char *p;
	for(p=s;*p!='\0' && !isdigit(*p);p++);
	return p;
}

void CopyProperties(BaseArgument *arg1,BaseArgument *arg2){
	LinkedListIterator<BaseArgProperty>* i;
	i=arg1->propIterForw();
	while(i->hasMoreElements()){
		arg2->addProperty(i->currentElement());
		i->nextElement();
	}
	delete i;
}


void CopyProperties(BaseOperation *op1,BaseOperation *op2){
	LinkedListIterator<BaseOpProperty>* i;
	i=op1->propIterForw();
	while(i->hasMoreElements()){
		op2->addProperty(i->currentElement());
		i->nextElement();
	}
	delete i;
}

BaseArgument *GenerateArgument(NODEPTR_TYPE p){
	RealRegArg *reg;
	BaseArgument *old_arg;
	LabelArg *label;
	IConstArg *con;
	if(p==NULL) return NULL;
	char *str=get_term_str(OP_LABEL(p));

	if(strcmp(str,"DEF")==0){
		return GenerateArgument(LEFT_CHILD(p));
	}
	else if(strcmp(str,"isel_label")==0){
		char *name=GET_STR_ATTR(p);
		_ASSERT(name!=NULL);
		label=new LabelArg(name);
		old_arg=GetIburgToArgMap(p);
		if(old_arg!=NULL){
			CopyProperties(old_arg,label);
			return label;
		}
		else{
			return label;
		}
	}
	else if(isRegister(str)){
		char *pref=prefix(str);
		
		reg=new RealRegArg(pref,atoi(sufix(str)),globalRegFileList->getIndex(pref));
		if(strcmp(pref,"temp")==0){
			globalSymbolTable->addNewRegs(reg,1);
			//add SSA property with value 1 to reg (temp register)
			SSAArgProperty *ssaP;
			ssaP=new SSAArgProperty();
			ssaP->setSSANum(1);
			reg->addProperty(*ssaP);

			/*for thumb

			// if _DUCHAIN_ property exists for the temp
			argPropertyForTemp = searchFromTempMap(tempToOperationMap, str);
			if( argPropertyForTemp != NULL )
			{
				reg->addProperty(*argPropertyForTemp);
				
 			}
            */
		}
		delete pref;
		old_arg=GetIburgToArgMap(p);
		if(old_arg!=NULL){
			CopyProperties(old_arg,reg);
			return reg;
		}
		else{
			return reg;
		}
	}
	else if(strcmp(str,"DUMMY_DEF")==0){
		//dummy definition for control operations
		//need not generate anything for this guy
		return NULL;
	}
	else{
		//ints or floats
		int *ptr=(int*)get_attr(p);
		if(strcmp(str,"FLOAT")==0 /*ptr==NULL*/){
			old_arg=GetIburgToArgMap(p);
			FConstArg *old_const,*new_const;
			if(old_arg!=NULL && (old_const=dynamic_cast<FConstArg*>(old_arg))!=NULL){
				new_const=new FConstArg(old_const->textValue());
				return new_const;
			}
			abort();
		}
		else if(strcmp(str,"INT")==0 && ptr!=NULL){
			con=new IConstArg(*ptr);
		}
		else 
			abort();
		return con;
	}
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////ReplaceArgument uses //////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

int equalRegs(BaseArgument *reg1, BaseArgument *reg2){
	if(reg1==NULL || reg2==NULL) return 0;

	if (reg1->isRegisterArg() && reg2->isRegisterArg())
	{
		if (((RegisterArg *)reg1)->getUniqID() == ((RegisterArg *)reg2)->getUniqID())
			return 1;

		// was originally :
		// if(reg1->isRealRegArg() && reg2->isRealRegArg() &&
		// *(RealRegArg*)(reg1) == *(RealRegArg*)reg2)
		// return 1;
	}
	return 0;
}

int ReplaceArgUses(BaseArgument *new_dst,BaseOperation *new_op,BaseArgument *old_dst,BaseOperation *old_op){
	MyLinkedList<BaseOperation*> *useList;
	MyLinkedListIterator<BaseOperation*> *iter;
	BaseArgument * src1,*src2;
	BaseOperation *use_op;

	//Changed by GG, 7/7/01
	if(new_dst==NULL) return 0;
	//end change

	useList=GetUseList(old_dst);
	//check if any of the uses of old_dst has more than one defs. 
	iter=useList->elementsForward();
	while(iter->hasMoreElements()){
		NormalInstr *instr=iter->currentElement()->getInstr();
		src1=iter->currentElement()->ptrToOperand(_SOURCE_1_);
		src2=iter->currentElement()->ptrToOperand(_SOURCE_2_);
		if((equalRegs(src1,old_dst) && GetDefList(src1)->numItems()>1) ||
			(src2!=NULL && equalRegs(src2,old_dst) && GetDefList(src2)->numItems()>1)){
			//use of old_dst has more than 1 def: insert an assignment instead of replacing the uses
			delete iter;
			return 0;
		}
		else if(equalRegs(src1,old_dst) && GetDefList(src1)->getHead()->ptrToOperand(_DEST_, _DEST_LIST_)!=old_dst){
			abort();
		}
		if(equalRegs(src2,old_dst) && GetDefList(src2)->getHead()->ptrToOperand(_DEST_, _DEST_LIST_)!=old_dst){
			abort();
		}
		iter->nextElement();
	}
	delete iter;

	//no use of old_dst has more than one def (the old_dst), therefore, we can replace the uses with old_dst
	iter=useList->elementsForward();
	while(iter->hasMoreElements()){
		use_op=iter->currentElement();
		src1=use_op->ptrToOperand(_SOURCE_1_);
		src2=use_op->ptrToOperand(_SOURCE_2_);
		iter->nextElement();
		if(equalRegs(src1,old_dst)){
			RemoveUseDef(old_dst,old_op,src1,use_op);
			use_op->addOperand(new_dst,_SOURCE_1_);
			if(new_op!=NULL)
				LinkUseDef(new_dst,new_op,src1,use_op);
		}
		if(src2!=NULL && equalRegs(src2,old_dst)){
			RemoveUseDef(old_dst,old_op,src2,use_op);
			use_op->addOperand(new_dst, _SOURCE_2_);
			if(new_op!=NULL)
				LinkUseDef(new_dst,new_op,src2,use_op);
		}
	}
	delete iter;
	return 1;
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////GenerateOper///////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

	

//inserts an operation into an instruction
//op: input, the operation to insert
//instr: input, the instruction to insert op into
int InsertOp(NormalOp *op,NormalInstr *instr){ //ComputeOp
	DataOpSlot *availSlot1;
	ControlOpSlot *availSlot2;
	
	if(op->isComputeOp() || op->isMemoryOp()){
		availSlot1=(DataOpSlot*)instr->getAvailSlot(op);
		//if instr has an available slot
		if(availSlot1!=NULL){
			//add op as the operation of this slot
			availSlot1->setOperation(op);
			
			BaseOpSlot &root=instr->getRootOpSlot();
			
			availSlot1->setNext(&root);
			
			instr->setRootOp(availSlot1);
			
			return FIT;
		}
		else return DIDNTFIT;
	}
	else if(op->isControlOp()){
		availSlot2=(ControlOpSlot*)instr->getAvailSlot(op);
		//if instr has an available slot
		if(availSlot2!=NULL){
			//add op as the operation of this slot
			availSlot2->setOperation(op);
			
			/*BaseOpSlot &root=instr->getRootOpSlot();
			
			availSlot2->setNext(&root);
			
			instr->setRootOp(availSlot2);*/
			
			return FIT;
		}
		else return DIDNTFIT;
	}
	else _ASSERT(0);
	
	return DIDNTFIT;
}


/*
//inserts an operation into an instruction
//op: input, the operation to insert
//instr: input, the instruction to insert op into
int InsertOp(NormalOp *op,NormalInstr *instr){ //ComputeOp
	DataOpSlot *availSlot;
	ArrayIterator<FlowOpSlot> *iter;

	availSlot=(DataOpSlot*)instr->getAvailSlot(op);
	//if instr has an available slot
	if(availSlot!=NULL){
		//add op as the operation of this slot
		availSlot->setOperation(op);

		//for each flow slot in instr
		iter=instr->getFlowOpSlotIter();
		while(iter->hasMoreElements()){
			//if this flow slot has an operation
			if(iter->currentElement().hasOper()){
				//
				availSlot->setNext((iter->currentElement()).getPtrToNextSlot());
				(iter->currentElement()).setNext(availSlot);
			}
			iter->nextElement();
		}
		delete iter;
		return FIT;
	}
	else return DIDNTFIT;
}

*/

NormalInstr *globalLastInstrInserted;

int globalRecomputeUDDU=0;


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////InsertAssignment///////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseOperation *InsertAssign(BaseArgument *new_dst,BaseArgument *old_dst,NormalInstr *old_root_instr,int id){
	ComputeOp *new_op;
	int opcodeind;

	//Changed by GG, 7/7/01
	if(new_dst==NULL) return NULL;
	//end change

	if(debugISel || debugMut) cout << "Inserting back an assignment" << endl;
	opcodeind=globalOpCodeTable.getIndex("ASSIGN");
	new_op=new ComputeOp();
	new_op->initOpCode(*(new OpCode(opcodeind)));
	new_op->addOperand(new_dst,_SOURCE_1_);
	new_op->addOperand(old_dst,_DEST_, _DEST_LIST_);
	new_op->setID(id);
	_NewOps.insert(new_op);
	if(InsertOp(new_op,old_root_instr))
		return new_op;
	else 
		return NULL;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////UndoLastMutation//////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UndoLastMutation(void){
	set<BaseOperation *>::iterator j;
	//_OldOps contains the old operations
	//_NewOps contains the new mutated operations

	//disconnect the new mutated operations
	for(j=_NewOps.begin();j!=_NewOps.end();j++){
		DisconnectOperation(*j, (*j)->getInstr());
	}

	//connect back the original mutations
	for(j=_OldOps.begin();j!=_OldOps.end();j++){
		ConnectOperation(*j,_OpToSlotMap[*j]);
	}
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////
//////////////IburgToIR translator///////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern int GenerateOperation(NODEPTR_TYPE, NormalOp *&);

//this function is called when we did a mutation, and we want to replace the original set of operations
//with a mutated one. Here we create new operations for each Iburg node in the mutated tree, and place them in the IR
BaseOperation* IburgToIR(NODEPTR_TYPE p,BaseArgument *old_dest,BaseOperation *old_op,NormalInstr *old_root_instr){
	_IburgToOpMapType::iterator i;
	set<BaseOperation *>::iterator j;
	NormalOp *new_op; //ComputeOp
	_NewOps.clear();

	//disconnect all the operations from _OldOps from the IR
	for(j=_OldOps.begin();j!=_OldOps.end();j++){
		if((_OpToInstrMap[*j]=(*j)->getInstr())!=NULL){
			_OpToSlotMap[*j]=(*j)->getSlot(); //remember the slot where this operation was
			DisconnectOperation(*j, _OpToInstrMap[*j]);
		}
		else 
			abort();
	}

	//generate operations from the new mutated Iburg tree. 
	int res=GenerateOperation(p,new_op);
	
	//print out a message
	if(debugISel || debugMut){
		switch(res){
		case FIT:
			cout << "\nReplaced with mutated tree///////////////////////////////////\n\n";
			break;
		case DIDNTFIT:
			cout << "\nNot replaced (didn't fit in resources)///////////////////////\n\n";
			break;
		case NOTINSTR:
			cout << "\nReplaced uses of def (nothing left of tree)//////////////////\n\n";
			break;
		default:
			abort();
		}
	}

	//insert back the essential operations which were omitted
	if(res==FIT || res==NOTINSTR){
		set<BaseOperation*>::iterator k;
		for(k=_EssentialOps.begin();k!=_EssentialOps.end();k++){
			if(ContainsOmittedOps(*k)){
				//insert it back
				if((*k)->isComputeOp()){
					if(debugISel || debugMut) cout << "Inserting back old operation" << endl;
					InsertOp((ComputeOp*)(*k),_OpToInstrMap[*k]);
					_ReinsertedOps.insert(*k);
				}
			}
		}
	}

	//if the mutated tree had enough resources, fine. 
	if(res==FIT){
		//if the root operation has many uses, fix it's uess, or if they have many defs, insert an assign instead. 
		if(new_op->hasOperand(_DEST_, _DEST_LIST_) && old_dest!=NULL && !equalRegs(old_dest,new_op->ptrToOperand(_DEST_, _DEST_LIST_))){
			if(ReplaceArgUses(new_op->ptrToOperand(_DEST_, _DEST_LIST_),new_op,old_dest,old_op))
				return new_op;
			else{//insert an assignment
				return InsertAssign(new_op->ptrToOperand(_DEST_, _DEST_LIST_),old_dest,old_root_instr,old_op->idValue());
			}
		}
		else return new_op;
	}
	//if nothing left of tree, fix it's uses, or if it has uses with many defs, insert an assign
	else if(res==NOTINSTR){
		if(old_dest!=NULL){
			BaseArgument *new_dst=GenerateArgument(p);
			if(ReplaceArgUses(new_dst,NULL,old_dest,old_op)){
				return NULL;
			}
			else{//insert an assignment
				return InsertAssign(new_dst,old_dest,old_root_instr,old_op->idValue());
			}
		}
	}
	else if(res==DIDNTFIT){
		//all other cases (DIDNTFIT), undo the mutation. 
		for(j=_OldOps.begin();j!=_OldOps.end();j++){
			ConnectOperation(*j,_OpToSlotMap[*j]);
		}
		return _IburgToOpMap[p];
	}
	return NULL;
}


BaseArgProperty *searchFromTempMap(TempMapType tempMap, MyString tempName)
{
	for( TempMapType :: iterator i = tempMap.begin(); 
	          i != tempMap.end(); i++ )
			  {
				  if( (*i).first == tempName )
					  return (*i).second;
			  }
	return NULL;
}

