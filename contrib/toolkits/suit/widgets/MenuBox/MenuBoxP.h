/***********************************************************************
 *
 * MenuBox Widget Private Data
 * Robert Blumen blumen@arisia.xerox.com blumen@cad.berkeley.edu
 *
 * MenuBoxP.h - Private definitions for MenuBox widget
 *
 * Converted to X11R3 by
 *	Win Treese
 *	Digital Equipment Corp.
 *	treese@crl.dec.com
 ***********************************************************************/

/***********************************************************
Copyright 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef _MenuBoxP_h
#define _MenuBoxP_h


#include <X11/MenuBox.h>
#include <X11/BoxP.h>

/************************************
 *
 *  Class structure
 *
 ***********************************/

/* New fields for the MenuBox widget class record */
typedef struct {int empty;} MenuBoxClassPart;

/* Full class record declaration */
typedef struct _MenuBoxClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    BoxClassPart	box_class;
    MenuBoxClassPart    menu_box_class;
} MenuBoxClassRec;

extern MenuBoxClassRec menuboxClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

/* New fields for the MenuBox widget record */
typedef struct {
/* private */
	int does_nothing;
} MenuBoxPart;


/* full widget declaration */

typedef struct _MenuBoxRec {
    CorePart	    core;
    CompositePart   composite;
    BoxPart 	    box;
    MenuBoxPart     menu_box;
} MenuBoxRec;

#endif _MenuBoxP_h
