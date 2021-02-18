/*
* $Header: WindowP.h,v 1.1 88/08/18 23:55:45 moraes Exp $
*/

/*
 * Window.P.h - Private Window widget definitions.
 *
 * Author:      Chris Peterson
 *              M.I.T.  
 *              Bldg. E40 - Project Athena
 *              77 Massachussets Ave.
 *              Cambridge, MA 02139
 * 
 * Date:        Tue Oct 27 1987
 *
 * Based upon the label widget by Charles Haynes.
 */

#ifndef _XtWindowPrivate_h
#define _XtWindowPrivate_h

/***********************************************************************
 *
 * Label Widget Private Data
 *
 ***********************************************************************/

/* New fields for the Label widget class record */

typedef struct {int foo;} WindowClassPart;

/* Full class record declaration */
typedef struct _WindowClassRec {
    CoreClassPart	core_class;
    WindowClassPart	window_class;
} WindowClassRec;

extern WindowClassRec windowClassRec;

/* New fields for the Label widget record */
typedef struct {
  Pixmap         pixmap;	/* The pixmap for this window. */
  Pixel          foreground;	/* The foreground color for graphics in this
				   window.*/
} WindowPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _WindowRec {
    CorePart	core;
    WindowPart	window;
} WindowRec;

#endif _XtWindowPrivate_h
/* DON'T ADD STUFF AFTER THIS #endif */

