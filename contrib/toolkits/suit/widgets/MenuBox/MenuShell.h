/***********************************************************************
 *
 * Menu Shell Widget (subclass of Override Shell)
 * Robert Blumen blumen@arisia.xerox.com blumen@cad.berkeley.edu
 *
 * MenuShell.h - Public definitions for MenuShell widget
 *
 * Converted to X11R3 by
 *	Win Treese
 *	Digital Equipment Corp.
 *	treese@crl.dec.com
 *
 ************************************************************************/

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

#ifndef _XtMenuShell_h
#define _XtMenuShell_h

#include <X11/Shell.h>


/* Resources:

 Name		Class		RepType		Default Value
 ----		-----		-------		-------------
other shell fields...

menuUnderCursor MenuUnderCursor	Boolean		True
putbackCursor	PutbackCursor	Boolean		False

*/

/*
 * MenuShell specific atoms
*/

#define XtNputbackCursor  "putbackCursor"
#define XtCPutbackCursor  "PutbackCursor"
#define XtNmenuUnderCursor "menuUnderCursor"
#define XtCMenuUnderCursor "MenuUnderCursor"
#define XtNmenuVSpace      "menuVSpace"
#define XtCMenuVSpace      "MenuVSpace"
/* Class record constants */

typedef struct _MenuShellClassRec *MenuShellClass;

#ifndef MENUSHELL
extern WidgetClass menuShellWidgetClass;
#endif

#endif _XtMenuShell_h
/* DON'T ADD STUFF AFTER THIS #endif */
