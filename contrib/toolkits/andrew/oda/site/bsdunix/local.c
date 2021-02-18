
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/bsdunix/RCS/local.c,v 1.5 89/11/01 10:49:58 jr Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/bsdunix/RCS/local.c,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	local.c,v $
 * Revision 1.5  89/11/01  10:49:58  jr
 * Buffering fixes.
 * 
 * Revision 1.4  89/11/01  09:29:56  jr
 * Have WriteBytes flush buffers.
 * 
 * Revision 1.3  89/10/31  12:39:43  jr
 * ANother casting problem in
 * LOCAL_malloc
 * 
 * Revision 1.1  89/09/21  06:08:51  mss
 * Initial revision
 * 
 * Revision 1.20  89/09/06  17:20:13  annm
 * fixing problems with names
 * 
 * Revision 1.19  89/08/31  17:57:41  mss
 * Chnaged to be made indpendent of tk
 * 
 * Revision 1.18  89/05/04  22:22:57  mss
 * Changed LOCAL_Free to return an error when freeing
 * a NULL pointer.
 * 
 * Revision 1.17  89/03/24  15:14:09  mss
 * Enlarged file descriptor index (MAXFILE) to 32
 * 
 * Revision 1.16  89/02/07  10:20:00  mss
 * Added "static" to forward dcl of GetFCB
 * 
 * Revision 1.15  89/02/03  12:14:29  jr
 * Forward decl of FlushOutput needs
 * to say static.
 * 
 * Revision 1.14  89/01/13  15:02:10  jr
 * Forward declare FlushOutput()
 * 
 * Revision 1.13  88/10/31  17:10:16  jr
 * Fix problem in GetFCB when
 * index is illegal.
 * 
 * Revision 1.12  88/10/27  09:18:50  jr
 * Fix bug when reusing i/o descriptor
 * after closing.
 * 
 * Revision 1.11  88/10/21  13:19:34  jr
 * Now doing buffered output for bytes
 * 
 * Revision 1.10  88/10/18  13:34:57  jr
 * Make LOCAL_ReadBytes do buffered
 * input.
 * 
 * Revision 1.9  88/10/07  12:20:39  mss
 * Changed eof semantics so that -1 singals end of
 * file instead of 0
 * 
 * Revision 1.8  88/10/02  14:15:41  mss
 * Changed "s to <>s
 * 
 * Revision 1.7  88/08/04  15:50:23  jr
 * Be careful about casting!!!
 * 
 * Revision 1.6  88/07/29  16:39:29  mz0k
 * Fixxed bugs in Read routines
 * 
 * Revision 1.5  88/07/29  16:13:39  mz0k
 * Made the case of retruning a '0' valid (less bytes are there than 
 * required) in the ReadBytes and ReadChars.
 * 
 * Revision 1.4  88/07/29  15:28:39  mz0k
 * Added LOCAL_ReadBytes and LOCAL_ReadChars
 * 
 * Revision 1.3  88/06/08  17:26:05  annm
 * dealt with line too long in copyright notice and
 * #end changed to #endif
 * 
 * Revision 1.2  88/06/08  15:54:40  annm
 * added copyright notice
 * 
*/

#ifndef LINT
static char RCSID[] = "$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/bsdunix/RCS/local.c,v $";
#endif

#define TK_IMPL_LOCAL
#define TK_GVARS_LOCAL

#include <local.h>

#ifndef USE_LONG_NAMES

#define FlushOutput	floutp

#endif

extern char *malloc();
extern char *realloc();

    /*
       MAX_FILES defines the maximum number of descriptors
       that may be opened at one time for reading (the number
       also applies independently to files open for writing).
    */

#define MAX_FILES   32

#define FILE_BUFFER 512

typedef struct {
    char    buffer[FILE_BUFFER];    /* Storage for the characters */
    int	    lastchar;		    /* Index of last buffer char available */
    int	    nextchar;		    /* Index of next char available */
} FCB_t;

static FCB_t *Input[MAX_FILES];
static FCB_t *Output[MAX_FILES];

#define NULL_FCB    ((FCB_t *) 0)

/* Forward declarations */
static FCB_t	    *GetFCB();
static INT_type    FlushOutput();

#ifdef USE_BCOPY

INT_type LOCAL_Bcopy(from, to, length)
    IN BYTE_type    *from;
    OUT BYTE_type   *to;
    IN INT_type	    length;
{

    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
#endif

INT_type LOCAL_Free(block) 
    IN POINTER_type block;    
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    if (block == (char *) 0) {
        rval = LCL_ERROR_INT;
	LCLError = LCL_LOCAL_err;
    }
    else {
        free((char *) block);
        rval = LCL_SUCCESS_INT;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

POINTER_type LOCAL_Malloc(nbytes)
    IN INT_type nbytes;    
{
    POINTER_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = (POINTER_type) malloc((unsigned int) nbytes);
    if (rval == (POINTER_type) 0) {
	rval = LCL_ERROR_POINTER;
	LCLError = LCL_NOMEM_err;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type LOCAL_ReadBytes(id, buffer, length)
    IN  INT_type    id;
    OUT BYTE_type   buffer[];
    IN  INT_type    length;
{
	     INT_type	rval;   /* The returned value */
    REGISTER int	left, nextbuf, charsread;
    REGISTER FCB_t	*fcb;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    if (length == -1) {
	close((int) id);
	if (id>=0 && id<MAX_FILES && Input[id]!=NULL_FCB) {
	    rval = LOCAL_Free((POINTER_type) Input[id]);
	    Input[id] = NULL_FCB;
	}
	goto LEAVE;
    }

    if (length == 0) {
	rval = 0;
	goto LEAVE;
    }

    /* Point to appropriate FCB */
    fcb = GetFCB(Input, id);
    if (fcb == NULL_FCB) {
	rval = LCL_ERROR_INT;
	goto LEAVE;
    }

    nextbuf = 0;    /* Where to start in user's buffer */
    left = length;  /* How many bytes still wanted */
    charsread = 0;  /* How many bytes have been read */
    for (;;) {

	/* Use up bytes in local buffer first */
	for (; fcb->nextchar<=fcb->lastchar && left>0; left--, charsread++) {
	    buffer[nextbuf++] = (BYTE_type) fcb->buffer[fcb->nextchar++];
	}

	/* Need more ? */
	if (left == 0) {
	    rval = charsread;
	    goto LEAVE;
	}

	/* Get more bytes into buffer */
	rval = read((int) id, fcb->buffer, FILE_BUFFER);
	if (rval < 0) {
	    rval = LCL_ERROR_INT;
	    goto LEAVE;
	}
	if (rval == 0) {
	    /* Got EOF */
	    rval = charsread;
	    goto LEAVE;
	}
	fcb->nextchar = 0;
	fcb->lastchar = rval - 1;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type LOCAL_ReadChars(id, buffer, length)
    IN  INT_type    id;
    OUT CHAR_type   buffer[];
    IN  INT_type    length;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    if (length == -1) {
	close((int) id);
	rval = LCL_SUCCESS_INT;
	goto LEAVE;
    }

    if (length == 0) {
	rval = 0;
	goto LEAVE;
    }

    rval = read((int) id, buffer, (int) length);
    if (rval < 0) {
	rval = LCL_ERROR_INT;
    }


LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

POINTER_type LOCAL_Realloc(old, newsize)
    IN POINTER_type old;
    IN INT_type	    newsize;
{
    POINTER_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = (POINTER_type) realloc((char *) old, (unsigned int) newsize);
    if (rval == (POINTER_type) 0) {
	rval = LCL_ERROR_POINTER;
	LCLError = LCL_NOMEM_err;
    }

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type LOCAL_WriteBytes(od, buffer, length)
    IN INT_type	    od;
    IN BYTE_type    buffer[];
    IN INT_type	    length;
{
	     INT_type	rval;	    /* The returned value */
    REGISTER FCB_t	*fcb;
    REGISTER int	nextbuf, charswritten;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check for legal parameters */
    if (od <0 || od>=MAX_FILES || length < -1) {
	rval = LCL_ERROR_INT;
	goto LEAVE;
    }

    /* See if we should close the file */
    if (length == -1) {
	/* See if fcb exists for this descriptor */
	if (Output[od] != NULL_FCB) {
	    /* Flush buffer */
	    rval = FlushOutput(od, Output[od]);
	    if (rval <= 0) {
		rval = LCL_ERROR_INT;
		goto LEAVE;
	    }
	    rval = LOCAL_Free((POINTER_type) Output[od]);
	    if (rval == LCL_ERROR_INT) {
		goto LEAVE;
	    }
	    Output[od] = NULL_FCB;
	}
	rval = close((int) od);
	if (rval < 0) {
	    rval = LCL_ERROR_INT;
	    goto LEAVE;
	}
	rval = LCL_SUCCESS_INT;
	goto LEAVE;
    }

    /* Special case 0 length */
    if (length == 0) {
	rval = 0;
	goto LEAVE;
    }

    /* Point to FCB for file */
    fcb = GetFCB(Output, od);
    if (fcb == NULL_FCB) {
	rval = LCL_ERROR_INT;
	goto LEAVE;
    }

    /* Got an FCB, add as many bytes as possible to its buffer */
    nextbuf = 0;
    for (charswritten=0;
	 fcb->lastchar<FILE_BUFFER && charswritten<length;
	 charswritten++) {

	    fcb->buffer[fcb->lastchar++] = (char) buffer[nextbuf++];
    }

    /* Now see if buffer is full */
    if (fcb->lastchar >= FILE_BUFFER) {
	rval = FlushOutput(od, fcb);
	if (rval <= 0) {
	    rval = LCL_ERROR_INT;
	    goto LEAVE;
	}
    }

    /* Indicate # chars written */
    rval = charswritten;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

static INT_type FlushOutput(od, fcb)
    IN INT_type	od;
    IN FCB_t	*fcb;
{
	     INT_type	rval;
    REGISTER INT_type	numchars;
    REGISTER INT_type	amt;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    numchars = 0;
    while (fcb->lastchar >= fcb->nextchar) {
	amt = fcb->lastchar - fcb->nextchar + 1;
	rval = write((int) od, &fcb->buffer[fcb->nextchar-1], (int) amt);
	if (rval <= 0) {
	    goto LEAVE;
	}
	fcb->nextchar += rval;
	numchars += rval;
    }
    fcb -> lastchar = 0;
    fcb -> nextchar = 1;
    rval = numchars;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type LOCAL_WriteChars(od, buffer, length)
    IN INT_type	    od;
    IN CHAR_type    buffer[];
    IN INT_type	    length;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check for legal parameters */
    if (od <0 || od>=MAX_FILES || length < -1) {
	rval = LCL_ERROR_INT;
	goto LEAVE;
    }

    /* length of 0 is a noop */
    if (length == 0) {
	rval = 0;
	goto LEAVE;
    }

    /* This is the time to flush bytes from FCB if possible */
    /* See if fcb exists for this descriptor */
    if (Output[od]!=NULL_FCB) {
	/* Flush buffer */
	rval = FlushOutput(od, Output[od]);
	if (rval <= 0) {
	    rval = LCL_ERROR_INT;
	    goto LEAVE;
	}
	if (length == -1) {
	    rval = LOCAL_Free((POINTER_type) Output[od]);
	    if (rval == LCL_ERROR_INT) {
		goto LEAVE;
	    }
	    Output[od] = NULL_FCB;
	}
    }

    if (length == -1) {
	close((int) od);
	rval = LCL_SUCCESS_INT;
	goto LEAVE;
    }

    rval = write((int) od, buffer, (int) length);
    if (rval <= 0) {
	rval = LCL_ERROR_INT;
    }

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

#ifdef SWAPPING

SWAP_INFO_type *LOCAL_SwapOut(address, length)
    IN POINTER_type address;
    IN INT_type	    length;
{
    SWAP_INFO_type  *rval;    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = LCL_ERROR_SWAP_INFO;
    LCLError = LCL_UNIMPL_err;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type LOCAL_SwapIn(swapinfo, address, available)
    IN SWAP_INFO_type	*swapinfo;
    IN POINTER_type	address;
    IN INT_type		available;
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = LCL_ERROR_INT;
    LCLError = LCL_UNIMPL_err;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

#endif

INT_type LOCAL_Final()
{
    INT_type	rval;	    /* The returned value */

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    rval = LCL_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}

INT_type LOCAL_Init()
{
	     INT_type	rval;	    /* The returned value */
    REGISTER INT_type	i;

#ifdef DEBUG
    if (TraceRoutine()) {
	Entering();
    }
#endif

    for (i=0; i<MAX_FILES; i++) {
	Input[i] = NULL_FCB;
	Output[i] = NULL_FCB;
    }

    rval = LCL_SUCCESS_INT;

LEAVE:
#ifdef DEBUG
    if (TraceRoutine()) {
	Leaving();
    }
#endif

    return rval;
}

static FCB_t *GetFCB(array, index)
    IN OUT FCB_t    *array[];
    IN INT_type	    index;
{
    REGISTER FCB_t	    *rval;  /* The returned value */
    REGISTER POINTER_type   ptr;

#ifdef DEBUG
    if( TraceRoutine() ){
	Entering();
    }
#endif

    /* Check range of index */
    if (index<0 || index>=MAX_FILES) {
	rval = NULL_FCB;
	LCLError = LCL_INTERNAL_err;
	goto LEAVE;
    }

    /* If fcb already exists, just return address */
    if (array[index] != NULL_FCB) {
	rval = array[index];
	goto LEAVE;
    }

    /* Doesn't exist, create one */
    ptr = LOCAL_Malloc((INT_type) sizeof(FCB_t));
    if (ptr == LCL_ERROR_POINTER) {
	rval = NULL_FCB;
	goto LEAVE;
    }
    array[index] = (FCB_t *) ptr;
    rval = array[index];
    /* Make it look empty */
    rval -> lastchar = 0;
    rval -> nextchar = 1;

LEAVE:
#ifdef DEBUG
    if( TraceRoutine() ){
	Leaving();
    }
#endif

    return rval;
}
