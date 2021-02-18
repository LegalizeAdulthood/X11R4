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

#ifndef _APOLLO_H_
#define _APOLLO_H_


#ifdef PRE_SR10_2

/* gpr_$set_quit_event doesn't exist until SR10.2 */
#define NO_GPR_QUIT

/* gpr_$physical_keys cannot be enabled in gpr_$enable_input until SR10.2 */
#define NO_PHYS_KEYS

/* gpr_$load_font_ptr doesn't exist until SR10.2 */
#define NO_LOAD_FONT_PTR

/* gpr_$text16 doesn't exist until SR10.2 */
#define NO_TEXT16

#endif /* PRE_SR10_2 */


#include <apollo/base.h>
#include <apollo/gpr.h>

#include <errno.h>
#include <stdio.h>
#include <sys/file.h>
#include <sys/time.h>
#include <sys/types.h>

#define NEED_EVENTS
#include "X.h"
#include "Xproto.h"
#include "servermd.h"
#include "inputstr.h"
#include "opaque.h"
#include "scrnintstr.h"
#include "regionstr.h"
#include "windowstr.h"

#define MAX_DEPTH_SUPPORTED 8

#define MFB_DRIVER 0
#define GPR_DRIVER 1

/*
 * Screen privates of all Apollo screens, regardless of driver
 */
extern int              apDisplayScreenIndex;
typedef struct
{
    int                 display_unit;   /* OS unit number */
    gpr_$disp_char_t    display_char;   /* display characteristics record */
    gpr_$bitmap_desc_t  display_bitmap; /* GPR bitmap ID */
    pointer             bitmap_ptr;     /* address of bitmap */
    int                 words_per_line; /* number of 16-bit words of address space from one scanline to the next */
    int                 depth;          /* actual depth being used (may be less than available depth) */
    int                 driver_type;    /* MFB_DRIVER or GPR_DRIVER */
} apDisplayDataRec, *apDisplayDataPtr;

/*
 * Screen privates for Apollo monochrome driver(s).
 */
extern int              apMonoScreenIndex;
typedef struct
{
    Bool                (*CloseScreen)();   /* wrapped CloseScreen */
} apMonoPrivScreenRec, *apMonoPrivScreenPtr;

/*
 * Screen privates for Apollo color driver(s).
 */
#define MAXVISUALS 1
#define MAXDEPTHS 2
extern int              apClrScreenIndex;
typedef struct
{
    Bool                (*CloseScreen)();       /* wrapped CloseScreen */
    VisualRec           visuals[MAXVISUALS];    /* visual list for screen */
    VisualID            vids[MAXVISUALS];       /* visual ID list for screen */
    DepthRec            depths[MAXDEPTHS];      /* depth list for screen */
    ColormapPtr         installedCmap;          /* currently installed colormap on screen */
} apClrPrivScreenRec, *apClrPrivScreenPtr;

/*
 * Pointer to any screen wanting mi cursor support.
 */
extern ScreenPtr        apMICursorScreen;



/*
 * private data for Apollo keyboards
 */
typedef struct
{
    time_$clock_t   beepTime;                   /* duration of bell */
    Bool            (* LegalModifierPtr) ();    /* LegalModifier function implementation for this keyboard */
    void            (* HandleKeyPtr) ();        /* Key transition event handler for this keyboard */
    void            (* SetRepeatPtr) ();        /* Code setting autorepeats for this keyboard */
} apPrivKbdRec, *apPrivKbdPtr;

/*
 * private data for Apollo pointer devices
 */
typedef struct
{
    int         numCurScreen;   /* number of screen pointer is currently on */
    int         x;              /* x coord of pointer position */
    int         y;              /* y coord of pointer position */
} apPrivPointrRec, *apPrivPointrPtr;

/*
 * Variables from the input part of the driver:
 * pointers to the "main" (i.e. only) keyboard and pointer device records.
 */
extern DevicePtr        apKeyboard;
extern DevicePtr        apPointer;



/*
 * externs for input driver routines:
 * our implementations of DDX input functions
 */
extern Bool             apSaveScreen();
extern int              apGetMotionEvents();
extern void             apChangePointerControl();
extern void             apChangeKeyboardControl();
extern void             apBell();
extern int              apMouseProc();
extern int              apKeybdProc();

/*
 * externs for common utility functions
 */
extern gpr_$bitmap_desc_t   apSetBitmap();
extern void                 apDeallocateBitmap();
extern gpr_$bitmap_desc_t   apMakeBitmapFromArray();

#endif
