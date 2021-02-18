/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        SWindowP.h
 **
 **   Project:     X Widgets
 **
 **   Description: Scrolled Window manager widget private include file
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1988 by the Massachusetts Institute of Technology
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard or  M.I.T.  not be used in advertising or publicity 
 **   pertaining to distribution of the software without specific, written 
 **   prior permission.
 **   
 *****************************************************************************
 *************************************<+>*************************************/

/***********************************************************************
 *
 * Scrolled Window Widget Private Data
 *
 ***********************************************************************/

/* New fields for the SWindow widget class record */
typedef struct {
     int mumble;   /* No new procedures */
} XwSWindowClassPart;

/* Full class record declaration */
typedef struct _XwSWindowClassRec {
    CoreClassPart	core_class;
    CompositeClassPart  composite_class;
    ConstraintClassPart constraint_class;
    XwManagerClassPart  manager_class;
    XwSWindowClassPart	swindow_class;
} XwSWindowClassRec;

extern XwSWindowClassRec XwswindowClassRec;

/* New fields for the SWindow widget record */
typedef struct {

/****************
 *
 *  This stuff is all exported from the ScrollBar widget.
 *
 ****************/
    
   Pixel   foreground;
   Pixel   highlight_color;
   int     highlight_tile;
   int     highlight_style;
   int     highlight_thickness;
   int     traversal_type;

   int vmin;		  /*  slider minimum coordiate position     */
   int vmax;		  /*  slider maximum coordiate position     */
   int vOrigin;		  /*  slider edge location                  */
   int vExtent;		  /*  slider size                           */

   int hmin;		  /*  Same as above for horizontal bar.     */
   int hmax;
   int hOrigin;
   int hExtent;

   int hsbX,hsbY,hsbWidth,hsbHeight;    /* Dimensions for the horiz bar */

   int vsbX,vsbY,vsbWidth,vsbHeight;    /* Dimensions for the vertical bar */

   Pixel selection_color;  /*  color used for drawing the slider     */

   int slide_area_tile;   /*  pattern to use for the slide area     */
   int slide_move_type;   /*  solid or transparent interactive move */

   int   arrow_area_tile;

   int initial_delay;
   int repeat_rate;
   int granularity;

   XtCallbackList vScrollEvent;
   XtCallbackList hScrollEvent;

/****************
 *
 *  Now for my stuff.
 *
 ****************/
   
   int		initialX, initialY;

   int 		initWidth,initHeight;

   int 		pad;

   Boolean	forceHSB;
   Boolean	forceVSB;
   Boolean	InInit;

   XwScrollBarWidget   hScrollBar;
   XwScrollBarWidget   vScrollBar;
   XwBulletinWidget    bboard;
   Widget              bbChild;
   
} XwSWindowPart;


/****************************************************************
 *
 * Full instance record declaration
 *
 ****************************************************************/

typedef struct _XwSWindowRec {
    CorePart	    core;
    CompositePart   composite;
    ConstraintPart constraint;
    XwManagerPart   manager;
    XwSWindowPart   swindow;
} XwSWindowRec;

#define DEFAULT_HEIGHT 300
#define DEFAULT_WIDTH 300

