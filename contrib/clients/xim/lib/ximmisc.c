/***********************************************************************
*  File:   xmisc.c
*  Author: Philip Thompson
*  $Date: 89/11/02 16:19:21 $
*  $Revision: 1.2 $
*  Purpose: General misc. libray of utililities
*  Copyright (c) 1988  Philip R. Thompson  (phils@athena.mit.edu)
*                Computer Resource Laboratory (CRL)
*                Dept. of Architecture and Planning
*                M.I.T., Rm 9-526
*                Cambridge, MA  02139
*   This  software and its documentation may be used, copied, modified,
*   and distributed for any purpose without fee, provided:
*       --  The above copyright notice appears in all copies.
*       --  This disclaimer appears in all source code copies.
*       --  The names of M.I.T. and the CRL are not used in advertising
*           or publicity pertaining to distribution of the software
*           without prior specific written permission from me or CRL.
*   I provide this software freely as a public service.  It is NOT a
*   commercial product, and therefore is not subject to an an implied
*   warranty of merchantability or fitness for a particular purpose.  I
*   provide it as is, without warranty.
*   This software is furnished  only on the basis that any party who
*   receives it indemnifies and holds harmless the parties who furnish
*   it against any claims, demands, or liabilities connected with using
*   it, furnishing it to others, or providing it to a third party.
*
*   Philip R. Thompson (phils@athena.mit.edu)
***********************************************************************/
#if (!defined(lint) && !defined(SABER))
static char xlib_rcs_id[] =
    "$Header: /mit/phils/X/Lib/RCS/ximmisc.c,v 1.2 89/11/02 16:19:21 phils Exp Locker: phils $";
#endif

#include <stdio.h>
#include "XimHeader.h"

#define Bcopy(a, b, c)  bcopy((char *)(a), (char *)(b), (int)(c))

char  *malloc(), *calloc(), *realloc();
extern int debug_flag;

swapshort (bp, n)
register char *bp;
register unsigned n;
{
    register char c;
    register char *ep = bp + n;

    while (bp < ep) {
        c = *bp;
        *bp = *(bp + 1);
        bp++;
        *bp++ = c;
    }
}


swaplong (bp, n)
register char *bp;
register unsigned n;
{
    register char *sp, c;
    register char *ep = bp + n;

    while (bp < ep) {
        sp = bp + 3;
        c = *sp;
        *sp = *bp;
        *bp++ = c;
        sp = bp + 1;
        c = *sp;
        *sp = *bp;
        *bp++ = c;
        bp += 2;
    }
}

Flip_bytes(xim)
XimImage *xim;
{
    int i, j;
    byte  flip[256], old, new;
    register byte *ep, *bp;

    for (i=0; i < 256; i++) {
        old = i;
        new = 0x00;
        for (j=0; j < 8; j++)
            if (old & (0x01 << j))
                new |= 0x01 << (7 - j);
        flip[i] = new;
    }
    ep = xim->data + xim->datasize;
    bp = xim->data;
    do {
        *bp = flip[*bp];
    } while (++bp < ep);

}


/* to get the size of an Xim object 
*/
int XimSize(xim)
XimImage *xim;
{
    int size, frame;

    size = sizeof(XimImage);
    if (xim->author)
        size += strlen(xim->author);
    if (xim->date)
        size += strlen(xim->date);
    if (xim->program)
        size += strlen(xim->program);
    if (xim->colors && xim->ncolors > 0)
        size += xim->ncolors * sizeof(Color);
    frame = xim->datasize * xim->nchannels;
    if (xim->alpha_flag)
        frame += xim->datasize;
    size += frame * xim->npics;
    return(size);
}


FreeXim(xim)
XimImage *xim;
{
    register i;

    if (xim->packed_flag) {
        fprintf(stderr, "FreeXim: attempting to free packed memory");
        free((char *)xim);
        xim = (XimImage *)0;
        return(1);
    }
    if (xim->author)
        free((char *)xim->author);
    if (xim->date)
        free((char *)xim->date);
    if (xim->program)
        free((char *)xim->program);
    if (xim->colors)
        free((char *)xim->colors);
    if (xim->data)
        free((char *)xim->data);
    if (xim->grn_data)
        free((char *)xim->grn_data);
    if (xim->blu_data)
        free((char *)xim->blu_data);
    if (xim->other)
        free((char *)xim->other);
    if (xim->ncomments && xim->comments) {
        for (i=0; i < xim->ncomments && xim->comments[i]; i++) {
            free(xim->comments[i]);
            xim->comments[i] = NULL;
        }
        free((char *)xim->comments);
    }
    bzero((char *)xim, sizeof(XimImage));
    free((char *)xim);
    xim = (XimImage *)0;
    return(1);
}


FreeXimData(xim)
XimImage *xim;
{
    register i;

    if (xim->packed_flag) {
        fprintf(stderr, "FreeXimData: can't to free packed memory");
        return(0);
    }
    if (xim->author) {
        free((char *)xim->author);
        xim->author = NULL;
    }
    if (xim->date) {
        free((char *)xim->date);
        xim->date = NULL;
    }
    if (xim->program) {
        free((char *)xim->program);
        xim->program = NULL;
    }
    if (xim->colors) {
        free((char *)xim->colors);
        xim->colors = (Color *)0;
        xim->ncolors = 0;
    }
    if (xim->data) {
        free((char *)xim->data);
        xim->data = (byte *)0;
    }
    if (xim->grn_data) {
        free((char *)xim->grn_data);
        xim->grn_data = (byte *)0;
    }
    if (xim->blu_data) {
        free((char *)xim->blu_data);
        xim->blu_data = (byte *)0;
    }
    if (xim->other) {
        free((char *)xim->other);
        xim->other = (byte *)0;
    }
    if (xim->ncomments && xim->comments) {
        for (i=0; i < xim->ncomments && xim->comments[i]; i++) {
            free(xim->comments[i]);
            xim->comments[i] = NULL;
        }
        free((char *)xim->comments);
        xim->comments = (char **)0;
    }
    return(1);
}


PackXim(xim, space, size)
XimImage *xim;
char *space;
int size;
{
    int i;
    char *ptr;
    XimImage *nxim;

    bzero(space, size);
    nxim = (XimImage *)space;
    Bcopy(xim, space, sizeof(XimImage));
    nxim->offset = ptr = space;

    /* Copy the variable length stuff after the fixed header */
    ptr += sizeof(XimImage);
    if (xim->author) {
        bcopy(xim->author, ptr, strlen(xim->author));
        nxim->author = ptr;
        ptr += strlen(xim->author);
    }
    if (xim->date) {
        bcopy(xim->date, ptr, strlen(xim->date));
        nxim->date = ptr;
        ptr += strlen(xim->date);
    }
    if (xim->program) {
        bcopy(xim->program, ptr, strlen(xim->program));
        nxim->program = ptr;
        ptr += strlen(xim->program);
    }
    if (xim->colors && xim->ncolors > 0) {
        Bcopy(xim->colors, ptr, (xim->ncolors * sizeof(Color)));
        nxim->colors = (Color *)ptr;
        ptr += xim->ncolors * sizeof(Color);
    }
    if (xim->nchannels == 1) {
        Bcopy(xim->data, ptr, (xim->datasize * xim->npics));
        nxim->data = (byte *)ptr;
        ptr += (xim->datasize * xim->npics);
    } else if (xim->nchannels == 3) {
        for (i=0; i < xim->npics; i++) {
            Bcopy(xim->data, ptr, xim->datasize);
            nxim->data = (byte *)ptr;
            ptr += xim->datasize;
            Bcopy(xim->grn_data, ptr, xim->datasize);
            nxim->grn_data = (byte *)ptr;
            ptr += xim->datasize;
            Bcopy(xim->blu_data, ptr, xim->datasize);
            nxim->blu_data = (byte *)ptr;
            ptr += xim->datasize;
        }
    }
    if (debug_flag) {
        fprintf(stderr, "space @ %lx   ptr @ %lx\n", space, ptr);
        fprintf(stderr, "Size given %d   used %d\n", size, ptr - space);
    }
    xim->packed_flag = 1;
    if (size < (ptr - space))
        XimWarn("PackXim(): space to small for image.");
    return(1);
}


XimImage *AllocAndCopyXim(xim)
XimImage *xim;
{
    int i;
    XimImage *nxim;

    if (!(nxim = (XimImage *)calloc(1, sizeof(XimImage)))) {
        XimWarn("AllocAndCopyXim: can't calloc new header");
        return((XimImage *)0);
    }
    Bcopy((char *)xim, (char *)nxim, sizeof(XimImage));

    /* Copy the variable length stuff after the fixed header */
    if (xim->author) {
        if (!(nxim->author = calloc(strlen(xim->author)+1, 1))) {
            XimWarn("AllocAndCopyXim: can't calloc author");
            return((XimImage *)0);
        }
        Bcopy(xim->author, nxim->author, strlen(xim->author));
    }
    if (xim->date) {
        if (!(nxim->date = calloc(strlen(xim->date)+1, 1))) {
            XimWarn("AllocAndCopyXim: can't calloc date");
            return((XimImage *)0);
        }
        Bcopy(xim->date, nxim->date, strlen(xim->date));
    }
    if (xim->program) {
        if (!(nxim->program = calloc(strlen(xim->program)+1, 1))) {
            XimWarn("AllocAndCopyXim: can't calloc program");
            return((XimImage *)0);
        }
        Bcopy(xim->program, nxim->program, strlen(xim->program));
    }
    if (xim->colors && xim->ncolors) {
        if (!(nxim->colors = (Color *)calloc((unsigned)xim->ncolors,
                sizeof(Color)))) {
            XimWarn("AllocAndCopyXim: can't calloc colors");
            return((XimImage *)0);
        }
        Bcopy(xim->colors, nxim->colors, xim->ncolors * sizeof(Color));
    }
    for (i=0; i < xim->npics; i++) {
        if (xim->nchannels >= 1 && xim->data) {
            if (!(nxim->data = (byte *)malloc(xim->datasize))) {
                XimWarn("AllocAndCopyXim: can't malloc data");
                return((XimImage *)0);
            }
            Bcopy(xim->data, nxim->data, xim->datasize);
        }
        if (xim->nchannels >= 3) {
            if (!(nxim->grn_data = (byte *)malloc(xim->datasize))) {
                XimWarn("AllocAndCopyXim: can't malloc grn_data");
                return((XimImage *)0);
            }
            Bcopy(xim->grn_data, nxim->grn_data, xim->datasize);
            if (!(nxim->blu_data = (byte *)malloc(xim->datasize))) {
                XimWarn("AllocAndCopyXim: can't malloc blu_data");
                return((XimImage *)0);
            }
            Bcopy(xim->blu_data, nxim->blu_data, xim->datasize);
        }
        if (xim->alpha_flag && xim->other) {
            if (!(nxim->other = (byte *)malloc(xim->datasize))) {
                XimWarn("AllocAndCopyXim: can't malloc alpha data");
                return((XimImage *)0);
            }
            Bcopy(xim->other, nxim->other, xim->datasize);
        }
    }
    return(nxim);
}


AllocAndCopyXimData(xim, dest)
XimImage *xim, *dest;
{
    int i;

    Bcopy(xim, dest, sizeof(XimImage));
    /* Copy the variable length stuff after the fixed header */
    if (xim->author) {
        if (!(dest->author = calloc(strlen(xim->author)+1, 1))) {
            XimWarn("AllocAndCopyXim: can't calloc author");
            return(0);
        }
        Bcopy(xim->author, dest->author, strlen(xim->author));
    }
    if (xim->date) {
        if (!(dest->date = calloc(strlen(xim->date)+1, 1))) {
            XimWarn("AllocAndCopyXim: can't calloc date");
            return(0);
        }
        Bcopy(xim->date, dest->date, strlen(xim->date));
    }
    if (xim->program) {
        if (!(dest->program = calloc(strlen(xim->program)+1, 1))) {
            XimWarn("AllocAndCopyXim: can't calloc program");
            return(0);
        }
        Bcopy(xim->program, dest->program, strlen(xim->program));
    }
    if (xim->colors && xim->ncolors) {
        if (!(dest->colors = (Color *)calloc((unsigned)xim->ncolors,
                sizeof(Color)))) {
            XimWarn("AllocAndCopyXim: can't calloc colors");
            return(0);
        }
        Bcopy(xim->colors, dest->colors, xim->ncolors * sizeof(Color));
    }
    for (i=0; i < xim->npics; i++) {
        if (xim->nchannels >= 1 && xim->data) {
            if (!(dest->data = (byte *)malloc(xim->datasize))) {
                XimWarn("AllocAndCopyXim: can't malloc data");
                return(0);
            }
            Bcopy(xim->data, dest->data, xim->datasize);
        }
        if (xim->nchannels >= 3) {
            if (!(dest->grn_data = (byte *)malloc(xim->datasize))) {
                XimWarn("AllocAndCopyXim: can't malloc grn_data");
                return(0);
            }
            Bcopy(xim->grn_data, dest->grn_data, xim->datasize);
            if (!(dest->blu_data = (byte *)malloc(xim->datasize))) {
                XimWarn("AllocAndCopyXim: can't malloc blu_data");
                return(0);
            }
            Bcopy(xim->blu_data, dest->blu_data, xim->datasize);
        }
        if (xim->alpha_flag && xim->other) {
            if (!(dest->other = (byte *)malloc(xim->datasize))) {
                XimWarn("AllocAndCopyXim: can't malloc alpha data");
                return(0);
            }
            Bcopy(xim->other, dest->other, xim->datasize);
        }
    }
    return(1);
}


UpdateXimPointers(nxim)
XimImage *nxim;
{
    int  offset;

    if (nxim->packed_flag) {
        XimWarn("UpdateXimPointers: can't update unpacked space");
        return(0);
    }
    offset = (int)nxim - (int)nxim->offset;
    if (nxim->author)
        nxim->author += offset;
    if (nxim->date)
        nxim->date += offset;
    if (nxim->program)
        nxim->program += offset;
    if (nxim->colors && (nxim->ncolors > 0))
        nxim->colors += offset;
    nxim->data += offset;
    if (nxim->nchannels == 3) {
        nxim->grn_data += offset;
        nxim->blu_data += offset;
    }
    return(1);
}

char *AllocAndCopy(from, size)
char *from;
int size;
{
    char *to = NULL, *strcpy();

    if ((from == NULL) || (size <= 0))
        return(NULL);
    if (!(to = calloc((unsigned)size+1, sizeof(char))))
        XimWarn("MallocAndCopy: can't calloc().");
    (void)strcpy(to, from);
    return(to);
}


PrintXim(xim, fp)
XimImage *xim;
FILE *fp;
{
    fprintf(fp, "\nXim Image:\n");
    fprintf(fp, " width: %d  height: %d\n", xim->width, xim->height);
    fprintf(fp, " bytes_per_line: %u  datasize %u\n",
            xim->bytes_per_line, xim->datasize);
    fprintf(fp, " nchannels %u  bits_channel %u\n", xim->nchannels,
        xim->bits_channel);
    fprintf(fp, " ncolors %d  npics %d\n", xim->ncolors, xim->npics);
    fprintf(fp, " data %lx  grn_data %lx  blu_data %lx\n", xim->data,
            xim->grn_data, xim->blu_data);
    if (xim->program)
        fprintf(fp, " Program : %s\n", xim->program);
    if (xim->author)
        fprintf(fp, " Author : %s\n", xim->author);
    if (xim->date)
        fprintf(fp, " Date : %s\n", xim->date);
}


/* VARARGS1 */
XimWarn(s1, s2)           /*  A most tragic but non-fatal warning. */
char *s1, *s2;
{
    extern char *progName;
    extern int errno, sys_nerr;
    extern char *sys_errlist[];

    fprintf(stderr,"%c%s warning :\n  ", 7, progName);
    fprintf(stderr, s1, s2);
    if ((errno > 0) && (errno < sys_nerr))
        fprintf(stderr, "  (%s)", sys_errlist[errno]);
    fprintf(stderr, "\n");
}


/* VARARGS1 */
XimError(s1, s2)           /*  A most tragic and fatal error.  */
char *s1, *s2;
{
    extern char *progName;
    extern int errno, sys_nerr;
    extern char *sys_errlist[];

    fprintf(stderr,"%c%s error >>\n%c  ", 7, progName, 7);
    fprintf(stderr, s1, s2);
    if ((errno > 0) && (errno < sys_nerr))
        fprintf(stderr, "  (%s)", sys_errlist[errno]);
    fprintf(stderr, "\n");
    exit(1);
}

/*** end xmisc.c ***/
