/***********************************************************

$Header: PropUtils.c,v 3.0 89/11/20 09:25:06 mikew Exp $

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
#include <X11/Xatom.h>
#include "X11/Xutil.h"
#include "ICC.h"
#include "xdefs.h"
#include "PropUtils.h"
#include "Atoms.h"

/*
 * Description: The inverse of XSetStandardProperties (pretty much).
 * 
 * Side Effects: None.
 */

/* ARGSUSED */
Status 
XGetStandardProperties(display, window, windowName, iconName,
		       iconPixmap, argc, argv, hints, clientMachine)
    Display        *display;	/* pointer to the X server */
    Window          window;	/* window from which to extract properties */
    unsigned char **windowName;	/* window name(ISO Latin 1) */
    unsigned char **iconName;	/* icon name (ISO Latin 1) */
    Pixmap         *iconPixmap;	/* icon pixmap */
    unsigned char ***argv;	/* command line argument list */
    int            *argc;	/* number of command line arguments */
    XSizeHints     *hints;	/* WM_NORMAL_HINTS */
    unsigned char **clientMachine;	/* WM_CLIENT_MACHINE */
{
    XTextProperty   textProperty;
    long            supplied;

    if (XGetCommand(display, window, argv, argc) != 1)
	return (0);
    if (XGetWMClientMachine(display, window, &textProperty) != 1)
	return (0);
    if (textProperty.encoding == XA_STRING)
	*clientMachine = textProperty.value;
    else {
	*clientMachine = NULL;
	if (textProperty.value)
	    XFree(textProperty.value);
    }
    XGetWMNormalHints(display, window, hints, &supplied);
    XGetWMName(display, window, &textProperty);
    if (textProperty.encoding == XA_STRING)
	*windowName = textProperty.value;
    else {
	*windowName = NULL;
	if (textProperty.value)
	    XFree(textProperty.value);
    }
    return (1);
}

#define UNSIGNEDINT unsigned long
#define SIGNEDINT long
#define RESOURCEID unsigned long
/* this structure may be extended, but do not change the order */
typedef struct {
    SIGNEDINT       state;
    RESOURCEID      icon;
}               xPropWMState;
#define NumPropWMStateElements 2/* number of elements in this structure */

/*
 * Description: Retrieve the contents of the WM_STATE property.
 * 
 */

Status 
XGetWMState(display, window, wmState)
    Display        *display;	/* pointer to the X server */
    Window          window;	/* window from which to get WM_STATE */
    WM_STATE       *wmState;	/* contents of WM_STATE */
{
    xPropWMState   *prop;
    Atom            actual_type;
    int             actual_format;
    unsigned long   nitems;
    unsigned long   leftover;

    if (XGetWindowProperty(display, window, wmStateAtom,
				 0L, (long) NumPropWMStateElements,
			   False, wmStateAtom, &actual_type, &actual_format,
			      &nitems, &leftover, (unsigned char **) &prop)
	!= Success)
	return (0);

    /* If the property is undefined on the window, return null pointer. */

    if ((actual_type != wmStateAtom) ||
	(nitems < NumPropWMStateElements) || (actual_format != 32)) {
	if (prop != NULL)
	    XFree((char *) prop);
	return (BadMatch);
    }
    wmState->state = prop->state;
    wmState->icon = prop->icon;
    XFree((char *) prop);
    return (1);
}
