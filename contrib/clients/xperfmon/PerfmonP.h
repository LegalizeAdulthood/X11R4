/*
 * $XConsortium: PerfmonP.h,v 1.2 89/09/19 14:44:16 jim Exp $
 * 
 * Perfmon Performance Monitor Widget
 *
 * Copyright 1989, PCS Computer Systeme GmbH, West Germany
 * Copyright 1989 PCS Computer Systeme GmbH, West Germany
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of PCS not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  PCS makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL PCS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Emanuel Jay Berkenbilt, MIT Project Athena
 * Author:           Thomas A. Baghli, PCS Computer Systeme GmbH, West Germany
 *                   tom@meepmeep.pcs.com
 */

#ifndef _LoadP_h
#define _LoadP_h

#include "Perfmon.h"
#include "system.h"
#include <X11/CoreP.h>

/* New fields for the perfmon widget instance record */
typedef struct {
    GC fgGC;			/* graphics context for fgpixel */
    GC hiGC;			/* graphics context for hipixel */
    Pixel fgpixel;		/* color index for graph */
    Pixel hipixel;		/* color index for text */
    int step_size;              /* pixel (value) draw size */
    int update;			/* update frequency */
    XtIntervalId interval_id;   /* timeout interval id */
    XFontStruct *font;		/* font for text */
    unsigned int font_height;	/* height of font */
    Boolean reverse_video;	/* display in reverse video */
    Boolean sys_params_used[NUM_POSSIBLE_STATS];
				/* display specified system stats */
} PerfmonPart;

/* Full instance record declaration */
typedef struct _PerfmonRec {
    CorePart core;
    PerfmonPart perfmon;
} PerfmonRec;

/* New fields for the Perfmon widget class record */
typedef struct {
    int dummy;
} PerfmonClassPart;

/* Full class record declaration */
typedef struct _PerfmonClassRec {
    CoreClassPart core_class;
    PerfmonClassPart perfmon_class;
} PerfmonClassRec;

/* Class pointer */
extern PerfmonClassRec perfmonClassRec;

#endif /* _LoadP_h */
