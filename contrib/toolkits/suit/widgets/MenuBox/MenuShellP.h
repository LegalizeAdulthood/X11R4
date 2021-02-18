/***********************************************************************
 *
 * MenuShell Widget 
 * Robert Blumen blumen@arisia.xerox.com blumen@cad.berkeley.edu
 *
 * MenuShellP.h - Private definitions forMenuShell widget
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

#ifndef _XtMenuShellPrivate_h
#define _XtMenuShellPrivate_h

#include <X11/MenuShell.h>
#include <X11/Command.h>
#include <X11/ShellP.h>

/************************************
 *
 *  Class structure
 *
 ***********************************/

/* New fields for the MenuShell widget class record */

typedef struct {int foo;} MenuShellClassPart;

/* Full class record declaration */

typedef struct _MenuShellClassRec {
  	CoreClassPart      core_class;
	CompositeClassPart composite_class;
	ShellClassPart  shell_class;
	OverrideShellClassPart  override_shell_class;
	MenuShellClassPart menu_shell_class;
} MenuShellClassRec;

extern MenuShellClassRec menuShellClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

/* New fields for the menu shell widget */

typedef struct _MenuShell_Part { 
	Boolean putback_cursor; 
	XtJustify menu_under_cursor;
	int menu_vspace;
/* private */
	Position cursor_x, cursor_y;
} MenuShellPart;

/* Full widget declaration */

typedef  struct {
	CorePart 	core;
	CompositePart 	composite;
	ShellPart 	shell;
	OverrideShellPart override;
	MenuShellPart menu;
} MenuShellRec, *MenuShellWidget;


#endif  _XtMenuShellPrivate_h
