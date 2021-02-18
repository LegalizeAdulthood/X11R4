/***********************************************************************
 *
 * MenuBox Widget (subclass of Box class )
 * Robert Blumen blumen@arisia.xerox.com blumen@cad.berkeley.edu
 *
 * MenuBox.h - Public definitios for MenuBox widget
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


/*----------------------------------------------------------------------

General Description:

A MenuBox can be used to contain Menu Buttons.  It should be the child
widget of a menuShell, and its children should be menu buttons,
although this is not enforced.  It is the same as a box, except that
it makes all of its managed children the same width.

- ----------------------------------------------------------------------*/

/* Parameters:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		pixel		White
 border		     BorderColor	pixel		Black
 borderWidth	     BorderWidth	int		1
 destroyCallback     Callback		Pointer		NULL
 hSpace 	     HSpace		int		4
 height		     Height		int		0
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 vSpace 	     VSpace		int		4
 width		     Width		int		0
 x		     Position		int		0
 y		     Position		int		0

*/

#ifndef _XtMenuBox_h
#define _XtMenuBox_h


/* Class record constants */

extern WidgetClass menuboxWidgetClass;

typedef struct _MenuBoxClassRec *MenuBoxWidgetClass;
typedef struct _MenuBoxRec      *MenuBoxWidget;

#endif _XtMenuBox_h
