/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/error.h,v 1.5 89/06/23 17:24:11 wjh Exp $ */
/* $ACIS:$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/error.h,v $ */

/* error.h  --  declarations for error handling routines
*/
#ifndef _ness_error_h_
#define _ness_error_h_
#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *error_h_rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/ness/objects/RCS/error.h,v 1.5 89/06/23 17:24:11 wjh Exp $";
#endif


#define freeze(msg) ((char *)strcpy(malloc(strlen(msg)+1), msg))

/* error message node */
struct errornode {
	struct mark *where;
	long execloc;	/* object code location of error */
	unsigned char *msg;	/* the error message */
	boolean ownmsg;	/* TRUE if this node is the only ptr to the msg */
	struct errornode *next;
};

struct errornode *errornode_New();
struct errornode *errornode_Create(/* long loc, long len, long execloc, unsigned char *msg,
	boolean ownmsg, struct errornode *next */);
void errornode_Destory(/* struct errornode *enode */);

void codelocStore(/* struct nesssym *fness */);		/* record code location */
void codelocForget(/* struct nesssym *fness */);	/* forget it */
struct nesssym *codelocFind(/* long loc */);		/* find nesssym for loc */

void   SaveError(/* unsigned char *msg, long loc, long len */);	/* add to error list */
void   ReportError(/* unsigned char *msg,  long index */); /* SaveError at the loc of 'index'th token */
void   ExprError(/* unsigned char *msg, struct exprnode *expr */); /* SaveError at the loc of 'expr' */
void   SetupErrorHandling(/*  */);		/* init compilation recovery */
void   yyerror(/* unsigned char *s */);	/* capture parser error */
long   errsynch(/*  */);			/* "... restart with token" */
boolean   isFuncStart(/* struct nesssym *tok */);	/* ? 0 indent for this or next two tokens */

struct errornode *LocateErrorFunc(/* loc, base, msg, ness */);  /* see which ness has the error */
void   MapRunErrors(/* struct ness *ness, struct errornode *errorlist */);

#endif _ness_error_h_

