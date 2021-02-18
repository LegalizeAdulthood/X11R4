/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

/* Std system and C header files */

#ifndef _tex2h_
#define _tex2h_

#include <stdio.h>
#include <sys/types.h>
#include <sys/dir.h>
#include <sys/stat.h>

/* X include files */

#include <X11/Xlib.h>

#ifdef X11R4
#include <X11/Xatomtype.h>
#endif X11R4

/* X toolkit header files */

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

/* Widget header files. */

#ifdef	X11R4
#  include <X11/Xaw/AsciiText.h>
#  include <X11/Xaw/Box.h>
#  include <X11/Xaw/Command.h>
#  include <X11/Xaw/Viewport.h>
#  include <X11/Xaw/List.h>
#else
#  include <X11/AsciiText.h>
#  include <X11/Box.h>
#  include <X11/Command.h>
#  include <X11/Viewport.h>
#  include <X11/List.h>
#endif

#ifdef USE_TABLE
#  include "Table.h"
#endif

extern Widget TopLevelWidget;

extern char *ProgName;

typedef struct  {
    char *printProgram;

    int userMag;

    char *tfmPath;

    char *dpiHorizString;
    char *dpiVertString;

    Bool autoSize;
    int largeMag;
    int smallMag;

    int pageMaxWd;
    int pageMaxHt;

    char *paperWidthString;
    char *paperHeightString;

    char *leftOffsetString;
    char *topOffsetString;

    char *helpText;

    Bool useButtons;

    Bool debug;

} xtexResourceStruct;

extern xtexResourceStruct xtexResources;

#define Debug xtexResources.debug

#ifdef __STDC__
  extern void DialogMessage(char *, int);
#else
  extern void DialogMessage();
#endif

#endif
