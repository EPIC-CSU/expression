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
#ifndef _CDT_H
#define _CDT_H		1

/*	Public interface for the dictionary library
**
**      Written by Kiem-Phong Vo (05/25/96)
*/

#define CDT_VERSION	19961031L

/*{KPV: standard definitions */

/* The symbol __STD_C indicates that the language is ANSI-C or C++ */
#ifndef __STD_C
#ifdef __STDC__
#define	__STD_C		1
#else
#if __cplusplus || c_plusplus
#define __STD_C		1
#else
#define __STD_C		0
#endif /*__cplusplus*/
#endif /*__STDC__*/
#endif /*__STD_C*/

/* For C++, extern symbols must be protected against name mangling */
#ifndef _BEGIN_EXTERNS_
#if __cplusplus || c_plusplus
#define _BEGIN_EXTERNS_	extern "C" {
#define _END_EXTERNS_	}
#else
#define _BEGIN_EXTERNS_
#define _END_EXTERNS_
#endif
#endif /*_BEGIN_EXTERNS_*/

/* _ARG_ simplifies function prototypes between K&R-C and more modern Cs */
#ifndef _ARG_
#if __STD_C
#define _ARG_(x)	x
#else
#define _ARG_(x)	()
#endif
#endif /*_ARG_*/

/* __INLINE__ if defined is the inline keyword */
#if !defined(__INLINE__)
#if defined(__cplusplus)
#define __INLINE__	inline
#else
#if defined(_WIN32)
#define __INLINE__	__inline
#endif/*_WIN32*/
#endif/*__cplusplus*/
#endif/*__INLINE__*/

/* The type Void_t is properly defined so that Void_t* can address any type */
#ifndef Void_t
#if __STD_C
#define Void_t		void
#else
#define Void_t		char
#endif
#endif /*Void_t*/

/* The NIL() macro simplifies defining nil pointers to a given type */
#ifndef NIL
#define NIL(type)	((type)0)
#endif /*NIL*/

/* dynamic linked library external scope handling */
#if _DLL && defined(_WIN32) && !defined(__EXPORT__)
#define __EXPORT__	__declspec(dllexport)
#endif
#if defined(_WIN32) && !defined(__IMPORT__)
#define __IMPORT__	__declspec(dllimport)
#endif
#ifndef _kpvimport
#if _DLL && defined(__IMPORT__)
#define _kpvimport	__IMPORT__
#else
#define _kpvimport	extern
#endif
#endif /*_kpvimport*/

/*KPV} */

#if __STD_C
#include	<stddef.h>
#else
#include	<sys/types.h>
#endif

typedef struct _dtlink_s	Dtlink_t;
typedef struct _dthold_s	Dthold_t;
typedef struct _dtdisc_s	Dtdisc_t;
typedef struct _dtmethod_s	Dtmethod_t;
typedef struct _dtdata_s	Dtdata_t;
typedef struct _dt_s		Dt_t;
typedef struct _dt_s		Dict_t;	/* for libdict compatibility */
typedef struct _dtstat_s	Dtstat_t;
typedef Void_t*			(*Dtsearch_f)_ARG_((Dt_t*,Void_t*,int));
typedef Void_t* 		(*Dtmake_f)_ARG_((Dt_t*,Void_t*,Dtdisc_t*));
typedef void 			(*Dtfree_f)_ARG_((Dt_t*,Void_t*,Dtdisc_t*));
typedef int			(*Dtcompar_f)_ARG_((Dt_t*,Void_t*,Void_t*,Dtdisc_t*));
typedef unsigned int		(*Dthash_f)_ARG_((Dt_t*,Void_t*,Dtdisc_t*));
typedef Void_t*			(*Dtmemory_f)_ARG_((Dt_t*,Void_t*,size_t,Dtdisc_t*));
typedef int			(*Dtevent_f)_ARG_((Dt_t*,int,Void_t*,Dtdisc_t*));

struct _dtlink_s
{	Dtlink_t*	right;	/* right child		*/
	union
	{ unsigned int	_hash;	/* hash value		*/
	  Dtlink_t*	_left;	/* left child		*/
	} hl;
};

/* private structure to hold an object */
struct _dthold_s
{	Dtlink_t	hdr;	/* header		*/
	Void_t*		obj;	/* user object		*/
};

/* method to manipulate dictionary structure */
struct _dtmethod_s 
{	Dtsearch_f	searchf; /* search function	*/
	int		type;	/* type of operation	*/
};

/* stuff that may be in shared memory */
struct _dtdata_s
{	int		type;	/* type of dictionary			*/
	Dtlink_t*	here;	/* finger to last search element	*/
	union
	{ Dtlink_t**	_htab;	/* hash table				*/
	  Dtlink_t*	_head;	/* linked list				*/
	} hh;
	int		ntab;	/* number of hash slots			*/
	int		size;	/* number of objects			*/
};

/* structure to hold methods that manipulate an object */
struct _dtdisc_s
{	int		key;	/* where the key begins in an object	*/
	int		size;	/* key size				*/
	int		link;	/* offset to Dtlink_t field		*/
	Dtmake_f	makef;	/* object constructor			*/
	Dtfree_f	freef;	/* object destructor			*/
	Dtcompar_f	comparf;/* to compare two objects		*/
	Dthash_f	hashf;	/* to compute hash values for objects	*/
	Dtmemory_f	memoryf;/* to allocate/free memory		*/
	Dtevent_f	eventf;	/* to process events			*/
};

/* the dictionary structure itself */
struct _dt_s
{	Dtsearch_f	searchf;/* search function			*/
	Dtdisc_t*	disc;	/* method to manipulate objs		*/
	Dtdata_t*	data;	/* sharable data			*/
	Dtmemory_f	memoryf;/* function to alloc/free memory	*/
	Dtmethod_t*	meth;	/* dictionary method			*/
	int		type;	/* type information			*/
	int		nview;	/* number of parent view dictionaries	*/
	Dt_t*		view;	/* next on viewpath			*/
	Dt_t*		walk;	/* dictionary being walked		*/
};

/* structure to get status of a dictionary */
struct _dtstat_s
{	int	dt_meth;	/* method type				*/
	int	dt_size;	/* number of elements			*/
	int	dt_n;		/* number of chains or levels		*/
	int	dt_max;		/* max size of a chain or a level	*/
	int*	dt_count;	/* counts of chains or levels by size	*/
};

/* supported storage methods */
#define DT_SET		0000001	/* set with unique elements		*/
#define DT_BAG		0000002	/* multiset				*/
#define DT_OSET		0000004	/* ordered set (self-adjusting tree)	*/
#define DT_OBAG		0000010	/* ordered multiset			*/
#define DT_LIST		0000020	/* linked list				*/
#define DT_STACK	0000040	/* stack				*/
#define DT_QUEUE	0000100	/* queue				*/
#define DT_METHODS	0000177	/* all currently supported methods	*/

/* asserts to dtdisc() */
#define DT_SAMECMP	0000001	/* compare methods equivalent		*/
#define DT_SAMEHASH	0000002	/* hash methods equivalent		*/

/* types of search */
#define DT_INSERT	0000001	/* insert object if not found		*/
#define DT_DELETE	0000002	/* delete object if found		*/
#define DT_SEARCH	0000004	/* look for an object			*/
#define DT_NEXT		0000010	/* look for next element		*/
#define DT_PREV		0000020	/* find previous element		*/
#define DT_RENEW	0000040	/* renewing an object			*/
#define DT_CLEAR	0000100	/* clearing all objects			*/
#define DT_FIRST	0000200	/* get first object			*/
#define DT_LAST		0000400	/* get last object			*/
#define DT_MATCH	0001000	/* find object matching key		*/

/* events */
#define DT_OPEN		0010000	/* a dictionary is being opened		*/
#define DT_CLOSE	0020000	/* a dictionary is being closed		*/
#define DT_DISC		0040000	/* discipline is about to be changed	*/
#define DT_METH		0100000	/* method is about to be changed	*/

_BEGIN_EXTERNS_	/* public data */
#if _BLD_cdt && defined(__EXPORT__)
#define extern	__EXPORT__
#endif
#if !_BLD_cdt && defined(__IMPORT__)
#define extern	__IMPORT__
#endif

#ifdef x_MYDEFS
extern Dtmethod_t* 	Dtset;
extern Dtmethod_t* 	Dtbag;
extern Dtmethod_t*	Dtstack;
extern Dtmethod_t*	Dtqueue;
extern Dtmethod_t*	Dtlist;
extern Dtmethod_t* 	Dtoset;
extern Dtmethod_t* 	Dtobag;
#endif

/* compatibility stuff; will go away */
#ifndef KPVDEL
#ifdef x_MYDEFS
extern Dtmethod_t*	Dttree;
extern Dtmethod_t	_Dttree;
extern Dtmethod_t*	Dtorder;
extern Dtmethod_t*	Dthash;
extern Dtmethod_t	_Dthash;
extern Dtmethod_t	_Dtlist;
extern Dtmethod_t	_Dtqueue;
extern Dtmethod_t	_Dtstack;
#endif
#endif

#undef extern
_END_EXTERNS_

_BEGIN_EXTERNS_	/* public functions */
#if _BLD_cdt && defined(__EXPORT__)
#define extern	__EXPORT__
#endif
#if !_BLD_cdt && defined(__IMPORT__) && defined(__EXPORT__)
#define extern	__IMPORT__
#endif

extern Dt_t*		dtopen _ARG_((Dtdisc_t*, Dtmethod_t*));
extern int		dtclose _ARG_((Dt_t*));
extern Dt_t*		dtview _ARG_((Dt_t*, Dt_t*));
extern Dtdisc_t*	dtdisc _ARG_((Dt_t* dt, Dtdisc_t*, int));
//extern Dtmethod_t*	dtmethod _ARG_((Dt_t*, Dtmethod_t*));

extern Dtlink_t*	dtflatten _ARG_((Dt_t*));
extern Dtlink_t*	dtextract _ARG_((Dt_t*));
extern int		dtrestore _ARG_((Dt_t*, Dtlink_t*));

extern int		dtwalk _ARG_((Dt_t*, int(*)(Dt_t*,Void_t*,Void_t*), Void_t*));

extern Void_t*		dtrenew _ARG_((Dt_t*, Void_t*));

extern int		dtsize _ARG_((Dt_t*));
extern int		dtstat _ARG_((Dt_t*, Dtstat_t*, int));
extern unsigned int	dtstrhash _ARG_((unsigned int, Void_t*, int));

#undef extern
_END_EXTERNS_

#define _DT_(d)		((Dt_t*)(d))

#define dtlink(d,e)	(((Dtlink_t*)(e))->right)
#define dtobj(d,e)	((_DT_(d)->disc->link < 0) ? (((Dthold_t*)(e))->obj) : \
				(Void_t*)((char*)(e) - _DT_(d)->disc->link) )
#define dtfinger(d)	(_DT_(d)->data->here ? dtobj((d),_DT_(d)->data->here) : \
				NIL(Void_t*) )

#define dtfirst(d)	(*(_DT_(d)->searchf))((d),NIL(Void_t*),DT_FIRST)
#define dtnext(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_NEXT)
#define dtlast(d)	(*(_DT_(d)->searchf))((d),NIL(Void_t*),DT_LAST)
#define dtprev(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_PREV)
#define dtsearch(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_SEARCH)
#define dtinsert(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_INSERT)
#define dtdelete(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_DELETE)
#define dtmatch(d,o)	(*(_DT_(d)->searchf))((d),(Void_t*)(o),DT_MATCH)
#define dtclear(d)	(*(_DT_(d)->searchf))((d),NIL(Void_t*),DT_CLEAR)

/* A linear congruential hash: h*17 + c + 97531 */
#define dtcharhash(h,c)	((((unsigned int)(h))<<4) + ((unsigned int)(h)) + \
			 ((unsigned char)(c)) + 97531 )

#endif /* _CDT_H */
