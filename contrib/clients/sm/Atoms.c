/***********************************************************

$Header: Atoms.c,v 3.0 89/11/20 09:24:35 mikew Exp $

Copyright 1989 by Mike Wexler, Santa Clara, Ca.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of Mike Wexler or not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

MIKE WEXLER DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#include "SM.h"
#include <X11/Xlib.h>
#include "Atoms.h"

Atom            wmStateAtom;		/* WM_STATE atom */
Atom            wmProtocolsAtom;	/* WM_PROTOCOLS atom */
Atom            wmSaveYourselfAtom;	/* WM_SAVE_YOURSELF atom */

/*
 * Description: Get the bindings for the atoms that SM uses.
 * 
 * Side Effects: Modifies the 3 global variables.
 */

/* This function gets the bindings for the atoms that SM uses. */
Status
InternAtoms(display)
    Display        *display;	/* pointer to X server */
{
    wmStateAtom = XInternAtom(display, "WM_STATE", False);
    wmProtocolsAtom = XInternAtom(display, "WM_PROTOCOLS", False);
    wmSaveYourselfAtom = XInternAtom(display, "WM_SAVE_YOURSELF", False);
    return (SM_SUCCESS);
}
