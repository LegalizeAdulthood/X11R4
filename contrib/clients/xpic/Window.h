/*
* $Header: Window.h,v 1.2 89/04/21 03:30:16 xwindows Exp $
*/

#ifndef _XtWindow_h
#define _XtWindow_h

/***********************************************************************
 *
 * Window Widget
 *
 ***********************************************************************/

/* #ifndef XtNpixmap
/* #define XtNpixmap    "pixmap"
/* #endif /* XtNpixmap */
#ifndef XtCpixmap
#define XtCpixmap    "Pixmap"
#endif /* XtCpixmap */

/* Class record constants */

extern WidgetClass windowWidgetClass;

typedef struct _WindowClassRec *WindowWidgetClass;
typedef struct _WindowRec      *WindowWidget;

#endif _XtWindow_h
/* DON'T ADD STUFF AFTER THIS #endif */
