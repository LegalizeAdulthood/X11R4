/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/envt.h,v 1.6 89/09/03 22:49:46 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/envt.h,v $ */

#ifndef _ness_envt_
#define _ness_envt_
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *envt_rcsid = "$Header:";
#endif

/* 
	envt.h

	The ness run-time environment.

*/

/*
 * $Log:	envt.h,v $
 * Revision 1.6  89/09/03  22:49:46  wjh
 * newness
 * 
 * Revision 1.5  89/06/01  16:02:36  wjh
 * campus release version
 * 
 * Revision 1.2  88/12/20  19:46:47  wjh
 * fixed various bugs
 * 
 * Revision 1.1  88/10/21  10:56:59  wjh
 * Initial revision
 * 
 * Creation 0.0  88/04/01 15:00  wjh
 * Initial creation by WJHansen
 * 
*/

/* The Ness run-time environment has these components:

	object code: a string containing the compilation of all functions
	execution stack: arguments, locals, and temporaries for execution
	system mark list: constant strings and goto targets
	some global pointers accessing the above

*/

#include "nessmark.ih"
#include "smpltext.ih"
#include "interp.h"		/* for TType */


/* each of the following are initialized to point to the value needed for class_IsType() */
extern struct classinfo *celClass;
extern struct classinfo *arbiterviewClass;
extern struct classinfo *textClass;
extern struct classinfo *textviewClass;
extern struct classinfo *dataobjectClass;
extern struct classinfo *lpairClass;
extern struct classinfo *scrollClass;
extern struct classinfo *matteClass;
extern struct classinfo *viewClass;
extern struct classinfo *celviewClass;
extern struct classinfo *valueviewClass;
extern struct classinfo *imClass;


extern struct simpletext *ObjectCode;	/* the compiled bytecode for the functions */

extern struct simpletext *EmptyText;	/* points to the special mark for error extent */


/* SysMark -  the system marks array

	retains marks for constants and goto's

Each constant is an entry in SysMark.

For each label and function in the interpretive code, there is an entry in the SysMark array.  The length field within the function mark gives the length of the code compiled for the function.  A branch to a label has a two byte quantity L which is an index into SysMark.  The position in the code is given by
	mark_GetPos(SysMarkLowEnd+L)

SysMarkFree indicates the first of the list of unallocated marks.
SysMarkLowEnd indicates which element is the bottom of the array
	{SysMark[0] is not used so no function has index 0. }
SysMarkHiEnd points one past the topmost marker and is thus 
	(SysMarkLowEnd+SysMarkSize)
*/
extern struct nessmark *SysMarkFree, *SysMarkLowEnd, *SysMarkHiEnd;
extern long SysMarkSize;
#define InitialSysMarkSize 1000



/* 
STACK

Each stack element begins with a pointer called a "objhdr".  Eventually it may point to an array of methods for dealing with this type of value on the stack.  For now it is a simple integer, except that the objhdr value for a mark is the actual pointer value.

*/

extern TType nessmarkHdr;
	/* the value found in the first word of all nessmarks */

/* elements that may appear on stack */
struct longstkelt {		/* LONG or INT or INTEGER or FIXED */
	TType hdr;
	long v;
};

struct boolstkelt {		/* BOOLEAN or LOGICAL */
	TType hdr;
	unsigned long v;
};

struct dblstkelt {		/* FLOAT or REAL */
	TType hdr;
	double v;
};

struct ptrstkelt {			/* object ptr */
	TType hdr;
	struct basicobject *v;
};


union stackelement {
	struct longstkelt  l;
	struct boolstkelt  b;
	struct dblstkelt  d;
	struct ptrstkelt  p;
	struct nessmark m;		/* STRING */
		/* ->m  *is*  a nessmark.  It is not a pointer to a nessmark. It has a objhdr.  */
	struct frameelt  f;
};

extern struct frameelt *FramePtr;	/* points to current stack frame */

/* extern long nArgsGiven;	/* number of arguments on stack when interpreter is called 
				(was used with stackArg) */

extern union stackelement *NSPstore, *NSPbase, *NSLowEnd, *NSHiEnd;
extern long NSSize;		/* the number of words in the stack. */
#define InitialNSSize 5000  /* initial size allocated to stack */

/* the execution stack 
	  NSPstore points to topmost stackelement in use; initially it equals NSHiEnd
	  NSPbase has the value of NSP after the initial frame is on it.
	  NSLowEnd points to the bottom most word in the stack.
	  NSHiEnd points one past the topmost word in the stack
*/

/* The stack grows downward.  The objhdr of each element is thus at the lower end of the stack.  Thus the word at the top of the stack  {*(long *)NSP}  is a objhdr.  (Got that straight?) */

/* A stack frame has this structure:

 	temporary variables	top end (lower address)
	struct frameelt
	local variables
	arguments		bottom end (higher address)

FramePtr->prevframe points to the hdr of the next earlier frame.
Arguments and locals are addressed with positive offsets from FramePtr.

When a function returns, the return value replaces the first argument.

The initial stack has only a dummy frame record.

*/

/* in interp.c */
union stackelement *popValue(/* union stackelement *NSP */);


#endif _ness_envt_

