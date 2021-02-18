/******************************************************************
Copyright 1987 by Apollo Computer Inc., Chelmsford, Massachusetts.
Copyright 1989 by Hewlett-Packard Company.

                        All Rights Reserved

Permission to use, duplicate, change, and distribute this software and
its documentation for any purpose and without fee is granted, provided
that the above copyright notice appear in such copy and that this
copyright notice appear in all supporting documentation, and that the
names of Apollo Computer Inc., the Hewlett-Packard Company, or MIT not
be used in advertising or publicity pertaining to distribution of the
software without written prior permission.

HEWLETT-PACKARD MAKES NO WARRANTY OF ANY KIND WITH REGARD
TO THIS SOFTWARE, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  Hewlett-Packard shall not be liable for errors
contained herein or direct, indirect, special, incidental or
consequential damages in connection with the furnishing,
performance, or use of this material.

This software is not subject to any license of the American
Telephone and Telegraph Company or of the Regents of the
University of California.
******************************************************************/

#ifndef _APOLLO_TEXT_H_
#define _APOLLO_TEXT_H_

/* Declarations private to Apollo text support with the X server */

#ifdef NO_TEXT16    /* can't have NO_TEXT16 without NO_LOAD_FONT_PTR */
#ifndef NO_LOAD_FONT_PTR
#define NO_LOAD_FONT_PTR
#endif
#endif

typedef struct _gprFID *gprFIDPtr;

typedef struct _gprFID {
    gprFIDPtr   nfr, pfr;
    FontPtr     theXFont;
    int         use_count;
#ifndef NO_LOAD_FONT_PTR
    char       *fontLoc;
#endif
#ifdef NO_TEXT16
    short       nGprFonts;
    short       fontIds[1];
#else
    short       fontId;
#endif
} gprFIDRec;

#endif
