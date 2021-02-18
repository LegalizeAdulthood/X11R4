/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 * FORTRAN to C binding for XGKS
 * Utilities for Packing Data Records
 *
 * David Berkowitz
 * TCS Development
 * Cambridge MA
 *
 * September 12, 1988
 *
 * $Header: pdrutils.h,v 4.0 89/09/01 09:55:18 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/pdrutils.h,v $
 *
 * $Log:	pdrutils.h,v $
 * Revision 4.0  89/09/01  09:55:18  amy
 * Fixed copyright.
 * 
 * Revision 4.0  89/09/01  09:52:16  amy
 * Changed IBM copyright for MIT distribution.
 * 
 *
 */

/* This structure represents a packed data record */

typedef struct {
    /* This is the fixed portion of a packed data record */
    int magicnum;       /* magic number - set when record is packed */
    int nint;           /* number of int's                          */
    int *intptr;        /* pointer to int's                         */
    int nfloat;         /* number of float's                        */
    float *floatptr;    /* pointer to float's                       */
    int nstring;        /* number of string's                       */
    char **strptr;      /* pointer to string's                      */
    char *data;         /* variable portion of a packed data record */

    /* The data section is organized as follows:
      list of ints pointed to by intptr
      list of reals pointed to by realptr
      list of string pointers pointed to by strptr
      list of NULL terminated strings pointed to by the string pointers
      */
} Gdatarec;

#define pdrmagicnum(datarec) ( ((Gdatarec *) datarec)->magicnum )

#define pdrintnum(datarec) ( ((Gdatarec *) datarec)->nint )

#define pdrrealnum(datarec) ( ((Gdatarec *) datarec)->nfloat )

#define pdrstringnum(datarec) ( ((Gdatarec *) datarec)->nstring )

#define pdrintindex(datarec, i) ( (((Gdatarec *) datarec)->intptr)[i -1] )

#define pdrintaddr(datarec) ( (((Gdatarec *) datarec)->intptr))

#define pdrrealindex(datarec, i) ( (((Gdatarec *) datarec)->floatptr)[i -1] )

#define pdrstringtabptr(datarec) ( (((Gdatarec *) datarec)->strptr) )

#define pdrstringindex(datarec,i) ( (((Gdatarec *) datarec)->strptr)[i-1] )

#ifdef DEBUG
#define	dumppdr(datarec) \
{ \
Gdatarec *temp = (Gdatarec *) datarec; \
int i; \
debug ( ("\n******** DUMP PDR ********\n") ); \
debug ( ("magicnum %d \n",temp->magicnum ) ); \
debug ( ("nint %d \n", temp->nint ) ); \
debug ( ("intptr 0x%x \n", temp->intptr ) ); \
for (i=0; i<temp->nint; i++) \
debug ( ("%d ", (temp->intptr)[i]) ); \
debug ( ("\n") ); \
debug ( ("nfloat %d \n", temp->nfloat ) ); \
debug ( ("floatptr 0x%x \n", temp->floatptr ) ); \
for (i=0; i<temp->nfloat; i++) \
debug ( ("%7.2f ", (temp->floatptr)[i]) ); \
debug ( ("\n") ); \
debug ( ("nstring %d \n", temp->nstring ) ); \
debug ( ("strptr 0x%x \n", temp->strptr) ); \
debug ( ("*strptr 0x%x \n", *(temp->strptr)) ); \
for (i=0; i<temp->nstring; i++) \
{ \
debug ( ("%d <%s> \n", i, (temp->strptr)[i]) ); \
} \
debug ( ("******** DUMP PDR ********\n\n") ); \
}
#endif DEBUG
