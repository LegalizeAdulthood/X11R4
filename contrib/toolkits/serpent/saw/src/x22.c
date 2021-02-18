	static char rcsid [] = "$Header: /u/04c/mnt/integ/saw/src/RCS/x22.c,v 1.9 89/07/20 18:28:14 rnk Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	x22.c
|
|  Description:
|	These are the "global" routines for Saw.
|
|  Component of:
|	Saw
|
|  Related files:
|	x22.h
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version:
|	1.0
|
|  Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|  Author:
|	Erik J. Hardy
|	erik@sei.cmu.edu
|
|  Change History:
$Log:	x22.c,v $
 * Revision 1.9  89/07/20  18:28:14  rnk
 * modified x22_build_default_parent to accept icon bitmaps and added debug
 * statements
 * 
 * Revision 1.8  89/07/20  12:11:35  rnk
 * added x22_select_point routine, which returns the location at which the
 * user invoked the routine (should be by a mouse click)
 * 
 * Revision 1.7  89/06/29  15:13:50  rnk
 * modified x22_child_move and x22_child_resize to check whether a widget
 * can be moved/resized and, if not, check up the tree until it either comes
 * to the top or finds a widget that can be moved/resized.
 * 
 * Revision 1.6  89/06/27  22:26:48  rnk
 * modified x22_get_widget_values and x22_add_widget_values to return a
 * pointer to the specified values_node
 * 
 * Revision 1.5  89/06/22  10:33:44  rnk
 * added the DrawBox routines and some defines for boundary box drawing,
 * and changed x22_child_move and x22_child_resize to draw boundary boxes
 * which follow the pointer around for moves and resizes.
 * 
 * Revision 1.4  89/06/13  13:13:56  rnk
 * fixed initialization in x22_get_widget_values
 * 
 * Revision 1.3  89/06/13  12:07:54  rnk
 * added routines x22_get_widget_values, x22_modify_widget_values and
 * x22_delete_widget_values to store the current values of widget parameters.
 * Currently, only "allow_user_move" and "allow_user_resize" are being 
 * stored.  This is to allow control over move and resize by widget.
 * 
 * Revision 1.2  89/06/07  17:50:16  rnk
 * added code to check values of allow_user_resize and allow_user_move to
 * determine whether to move/resize widgets in x22_child_move and
 * x22_child_resize
 * 
 * Revision 1.1  89/04/25  17:36:27  rnk
 * Initial revision
 * 
 * Revision 1.3  89/02/17  03:31:27  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  21:42:30  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  15:58:04  ejh
 * Initial revision
 * 
|	25 Oct 88	created		erik
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#define	x22_TOP
#define	memoryPack

#include	<X11/Intrinsic.h>
#include        <X11/StringDefs.h>
#include	<X11/Cardinals.h>
#include	<X11/Shell.h>
#include 	<X11/Xatom.h>

#include	"serpent.h"
#include	"x22.h"
#include	"iid.h"
#include	"listPack.h"
#include	"icon_bits.h"

/* Defs for boundary box manipulation */

#define MAX_BOX_VECTORS         4
#define UL                      0
#define UR                      1
#define LL                      2
#define LR                      3
#define DrawBox() XDrawSegments(display, parent_win, DrawGC, box, num_vectors)
#define EVENTMASK (ButtonPressMask | ButtonReleaseMask | PointerMotionMask)
extern  GC      DrawGC ;                   /* graphics context for drawing */
extern  Cursor  ArrowCrossCursor ;	   /* move/resize cursor 	   */

typedef	struct	{
	Widget	widget;
	Widget	parent;
	}	x22_parent_node;

#include	"xdb.h"


/* Store the vertices for the boundary box in a vertex list. 	  */
/* This code was taken from the uwm source and slightly modified. */

int StoreBox(box, ulx, uly, lrx, lry)
register XSegment box[];
int ulx;                        /* Upper left X coordinate. */
int uly;                        /* Upper left Y coordinate. */
int lrx;                        /* Lower right X coordinate. */
int lry;                        /* Lower right Y coordinate. */
{
 
    box[0].x1 = ulx; box[0].y1 = uly;
    box[0].x2 = lrx; box[0].y2 = uly;

    box[1].x1 = lrx; box[1].y1 = uly;
    box[1].x2 = lrx; box[1].y2 = lry;

    box[2].x1 = lrx; box[2].y1 = lry;
    box[2].x2 = ulx; box[2].y2 = lry;

    box[3].x1 = ulx; box[3].y1 = lry;
    box[3].x2 = ulx; box[3].y2 = uly;

    return(4);
}


void	x22_register_new_shell	(widget)
Widget	widget;

    {
    iid_id_type		w_id;
    int			value = 1;

    if (x22_debug >= 2)
	{
	fprintf (stderr, "x22_register_new_shell: Entering.\n");
	fprintf (stderr, "x22_register_new_shell: widget = %x.\n", widget);
	}

/*	temporarily commented out 'cause we don't know how to handle
	scalars yet

    if (!x22_transaction_started)
	{
	x22_trans = isd_start_transaction ();
	x22_transaction_started = TRUE;
	}

    w_id = isd_add_shared_data (x22_trans, "shell_widget", NULL, &value);

    xht_item_id_mapping (w_id, widget);

*/

    if (x22_debug >= 2)
	{
	fprintf (stderr, "x22_register_new_shell: w_id = %d.\n",
	    w_id);
	fprintf (stderr, "x22_register_new_shell: Leaving.\n");
	}
    }

static XtActionsRec actions[] =
{
     {"x22_child_move",         x22_child_move},
     {"x22_child_resize",       x22_child_resize},
     {"x22_select_point",       x22_select_point},
} ;


Widget	x22_build_default_parent()
{
    Display 	*display;
    int		n ;
    Arg		args[10] ;
    Widget	default_parent ;

    if (x22_debug > 1) fprintf(stderr, "x22_build_default_parent: enter.\n");

    n = 0 ;
    default_parent = XtCreateApplicationShell(NULL,
	topLevelShellWidgetClass, args, n) ;
    display = XtDisplay(default_parent) ;

    n = 0 ;
    XtSetArg(args[n], XtNiconPixmap, XCreateBitmapFromData(display,
            XtScreen(default_parent)->root, saw_bits, saw_width, saw_height));
    n++ ;
    XtSetArg (args[n], XtNallowShellResize, FALSE) ;
    n++ ;
    XtSetValues (default_parent, args, n);

    XtAddActions(actions, XtNumber(actions)) ;

    x22_register_new_shell(default_parent) ;

    if (x22_debug > 1)
	fprintf (stderr, "x22_build_default_parent: leaving.\n") ;

    return (default_parent);
}


void	x22_add_parent(widget, parent)
Widget	widget;
Widget	parent;

{
    x22_parent_node	*parent_node;

    if (x22_debug > 1)
	fprintf (stderr, "x22_add_parent: enter.\n");

    if (x22_debug > 2)
	fprintf (stderr, "x22_add_parent: widget = %d, parent = %d.\n",
	    widget, parent);

    parent_node = (x22_parent_node *) make_node (sizeof (x22_parent_node));
    parent_node -> widget = widget;
    parent_node -> parent = parent;
    (void) add_to_tail (x22_parent_list, parent_node);

    if (x22_debug > 1)
	fprintf (stderr, "x22_add_parent: leaving.\n");
}


void	x22_delete_parent(widget)
Widget	widget;

{
    NODE	current_node;
    boolean	exit_the_loop = FALSE;

    if (x22_debug > 1)
	fprintf (stderr, "x22_delete_parent: enter.\n");

    if (x22_debug > 2)
	fprintf (stderr, "x22_delete_parent: widget = %d\n", widget);

    current_node = get_list_head (x22_parent_list);
    while (current_node != NULL && !exit_the_loop)
    {
	if (((x22_parent_node *) current_node) -> widget == widget)
	{
	    XtDestroyWidget (((x22_parent_node *) current_node) -> parent);
	    (void) delete_list_node (x22_parent_list, current_node);
	    exit_the_loop = TRUE;
	}
	else
	    current_node = get_list_next (x22_parent_list, current_node);
    }

    if (x22_debug > 1)
	fprintf (stderr, "x22_delete_parent: leaving.\n");
}


NODE x22_get_widget_values(widget) 
Widget		widget ;

{
    x22_widget_values_node	*current_node;
    boolean	exit_the_loop = FALSE ;

    if (x22_debug > 1) fprintf(stderr, "x22_get_widget_values: enter.\n");

    if (x22_debug > 2) fprintf(stderr, 
	"x22_get_widget_values: widget=%x\n", widget);

    current_node = (x22_widget_values_node *) get_list_head(x22_values_list) ;

    while (current_node != NULL && !exit_the_loop)
    {
	if (current_node->widget == widget)
	    exit_the_loop = TRUE;
	else if (current_node != NULL) 
	    current_node = (x22_widget_values_node *) 
		get_list_next(x22_values_list, current_node);
    }

    if (x22_debug > 2) 
	fprintf(stderr, "x22_get_widget_values: current=%d\n", current_node);

    if (x22_debug > 1) fprintf(stderr, "x22_get_widget_values: leaving.\n");

    return((NODE) current_node) ;
}


void x22_delete_widget_values(widget)
Widget	widget;

{
    x22_widget_values_node	*current_node;
    boolean	exit_the_loop = FALSE ;

    if (x22_debug > 1)
	fprintf(stderr, "x22_delete_widget_values: enter.\n");

    if (x22_debug > 2)
	fprintf(stderr, "x22_delete_widget_values: widget=%x\n", widget);

    current_node = (x22_widget_values_node *) get_list_head(x22_values_list);
    while (current_node != NULL && !exit_the_loop)
    {
	if (current_node->widget == widget)
	{
	    (void) delete_list_node(x22_values_list, current_node);
	    exit_the_loop = TRUE;
	}
	else
	    current_node = (x22_widget_values_node *) 
		get_list_next(x22_values_list, current_node);
    }

    if (x22_debug > 1) fprintf(stderr,"x22_delete_widget_values: leaving.\n");
}


NODE x22_add_widget_values(widget)
Widget		widget ;

{
    x22_widget_values_node	*values_node;

    if (x22_debug > 1) fprintf(stderr, "x22_add_widget_values: enter.\n");

    if (x22_debug > 2) fprintf(stderr, 
	    "x22_add_widget_values: widget=%x.\n", widget) ;

    values_node = (x22_widget_values_node *) 
	make_node (sizeof (x22_widget_values_node));
    values_node->widget = widget;

    (void) add_to_tail (x22_values_list, values_node);

    if (x22_debug > 1)
	fprintf (stderr, "x22_add_widget_values: leaving.\n");
    return((NODE) values_node) ;
}


/*--------------------------------------------------------------------------*\
|  Name: x22_select_point
|
|  Description:
|       x22_select_point records the position (relative to the upper left 
|	corner of the widget) at which the user clicked the left button
|	of the mouse.
|
|  Parameters:
|       widget          the widget which generated the select event.
|       event           not used.
|       params          not used.
|       num_params      not used.
|
|  Returns: Nothing.
|
|  Author:
|       Rick Kazman
|       rnk@sei.cmu.edu
|
|       7/17/89  created  rnk
|
\*--------------------------------------------------------------------------*/

void x22_select_point(w, event, params, num_params)
    Widget w;
    XEvent *event ;
    String *params ;
    Cardinal *num_params ;
{
    Display *display;
    Window window, sub_window ;		/* Current windows */
    Window root;                        /* Root query window. */
    int ptrmask;                        /* pointer status word */
    int root_x, root_y;                 /* root window coordinates */
    int cur_x, cur_y;                   /* current pointer position */
    transaction_type trans;		/* Transaction to send to dm */
    iid_id_type	w_id;			/* id of the current widget */
    x22_widget_values_node *values_node;/* widget's values node */

    if (x22_debug > 1)
    {
        fprintf (stderr, "x22_select_point: Entering.\n");
        fprintf (stderr, "x22_select_point: widget=%x\n", w) ;
    }

    values_node = (x22_widget_values_node *) x22_get_widget_values(w) ;
    if (values_node == NULL) 
    {
	fprintf (stderr, "x22_select_point: Can't find widget.\n");
	return ;
    }

    window = XtWindow(w) ;
    display = XtDisplay(w) ;

    /* Find the position of the pointer */
    XQueryPointer(display, window, &root, &sub_window,
                &root_x, &root_y, &cur_x, &cur_y, &ptrmask);

    /* Send the position of the widget to the dm */
    w_id = xht_get_dm_id (w) ;
    trans = isd_start_transaction();

    isd_put_shared_data(trans, w_id, 
	values_node->widget_name, "method", "select");
    isd_put_shared_data(trans, w_id, 
	values_node->widget_name, "selected_x", &cur_x);
    isd_put_shared_data(trans, w_id, 
	values_node->widget_name, "selected_y", &cur_y);

    isd_commit_transaction(trans);

    if (x22_debug > 2) fprintf(stderr, "x22_select_point: Sent to dm.\n");

    if (x22_debug > 1) fprintf(stderr, "x22_select_point: Leaving.\n");
    return;
}

/*--------------------------------------------------------------------------*\
|  Name: x22_child_resize
|
|  Description:
|       x22_child_resize resizes child widgets within their parents, 
|       according to the position selected by the mouse.
|
|  Parameters:
|       widget          the widget which generated the resize event.
|       event           not used.
|       params          not used.
|       num_params      not used.
|
|  Returns: Nothing.
|
|  Author:
|       Rick Kazman
|       rnk@sei.cmu.edu
|
|       3/15/89  created  rnk
|
\*--------------------------------------------------------------------------*/

void x22_child_resize(w, event, params, num_params)
    Widget w;
    XEvent *event ;
    String *params ;
    Cardinal *num_params ;
{
    Window window, parent_win ;
    Widget parent ;
    Display *display;
    XWindowAttributes window_info;      /* Event window info. */
    register int prev_x;                /* Previous event window X location.*/
    register int prev_y;                /* Previous event window Y location.*/
    int mid_x, mid_y;                   /* mid-point of the widget */
    int cur_x, cur_y;                   /* current pointer position */
    Position new_x, new_y ;             /* final widget location */
    Dimension height, width ;           /* widget dimensions */
    int i_height, i_width ;		/* widget dimensions (int version) */
    int ulx, uly;                       /* Event window upper left X and Y. */
    int lrx, lry;                       /* Event window lower right X and Y.*/
    int root_x, root_y;                 /* root window coordinates */
    int corner ;                        /* corner which controls resize */
    int ptrmask;                        /* pointer status word */
    int num_vectors;                    /* Number of vectors to XDraw. */
    Window sub_window;                  /* Mouse query sub window. */
    Window root;                        /* Root query window. */
    XEvent button_event;                /* Button event packet. */
    XSegment box[MAX_BOX_VECTORS];      /* Box drawing vertex buffer. */
    int send_h, send_w ;		/* Height/width for dm */
    transaction_type trans;		/* Transaction to send to dm */
    iid_id_type	w_id;			/* id of the current widget */
    boolean move, resize ;		/* geometry management switches */
    x22_widget_values_node *values_node;/* widget's values node */
    boolean found ;			/* flag */

    if (x22_debug > 1)
    {
        fprintf (stderr, "x22_child_resize: Entering.\n");
        fprintf (stderr, "x22_child_resize: widget=%x\n", w) ;
    }

    /* Loop upwards from child to parent, searching for the first widget */
    /* in the hierarchy which allows resizing.				 */
    found = FALSE ;
    while (!found)
    {
	values_node = (x22_widget_values_node *) x22_get_widget_values(w) ;
        if (values_node == NULL) return ;
	if (!values_node->allow_user_resize) w = XtParent(w) ;
	else found = TRUE ;
	if (w == NULL) return ;
    }

    /* Find the display and the window id of the widget and its parent */
    window = XtWindow(w) ;
    display = XtDisplay(w) ;
    parent = XtParent(w) ;
    parent_win = XtWindow(parent) ;

    /* Change the cursor. */
    XChangeActivePointerGrab(display,EVENTMASK,ArrowCrossCursor,CurrentTime);

    /* Clear the vector buffers. */
    bzero(box, sizeof(box));

    /* Gather info on the event window. */
    if (!XGetWindowAttributes(display, window, &window_info))
        fprintf(stderr, "x22_child_resize:  Can't get window attributes\n");

    /* Initialize movement variables. */
    ulx = window_info.x;
    uly = window_info.y;
    lrx = window_info.x + window_info.width +
                     (window_info.border_width << 1) - 1;
    lry = window_info.y + window_info.height +
                     (window_info.border_width << 1) - 1;

    num_vectors = StoreBox(box, ulx, uly, lrx, lry) ;

    /* Find and save the initial position of the pointer */
    XQueryPointer(display, window, &root, &sub_window,
                &root_x, &root_y, &cur_x, &cur_y, &ptrmask);

    prev_x = root_x ;
    prev_y = root_y ;

    /* Locate the middle of the widget */
    mid_x = window_info.width/2 ;
    mid_y = window_info.height/2 ;

    if (cur_x < mid_x)
    {
        if (cur_y < mid_y)
        {   /* Pointer is in the upper left quadrant of the widget */
            corner = UL ;
	} else
        {   /* Pointer is in the lower left quadrant of the widget */
            corner = LL ;
	}
    } else {
        if (cur_y < mid_y)
        {   /* Pointer is in the upper right quadrant of the widget */
            corner = UR ;
	} else
        {   /* Pointer is in the lower right quadrant of the widget */
            corner = LR ;
	}
    }

    /* Process any pending exposure events before drawing the box. */
    while (QLength(display) > 0)
    {
        XPeekEvent(display, &button_event);
        if (((XAnyEvent *)&button_event)->window == window
           && (button_event.type == ButtonPress ||
                button_event.type == ButtonRelease))
            break;
        XNextEvent(display, &button_event);
    }

    /* Freeze the server so we get a solid box instead of a flickering one */
    XGrabServer(display);

    /* Now draw the box.  */
    DrawBox();

    /* Main loop. */
    while (TRUE)
    {
        /* Check to see if we have a change in mouse button status.
         * This is how we get out of this "while" loop.         */
        XNextEvent(display, &button_event);

        if ((button_event.type != ButtonPress) &&
            (button_event.type != ButtonRelease) &&
            (button_event.type != MotionNotify))
        {
            continue; /* spurious menu event... */
	}

        DrawBox();
        XUngrabServer(display);

        if (button_event.type == ButtonRelease)
        {
            /* The button was released, so move and resize the window. */

            new_x = ulx ;
            new_y = uly ;
            i_width = lrx - ulx ;
            i_height = lry - uly ;
            if (i_width < 2) width = 2 ;
	    else width = i_width ;
            if (i_height < 2) height = 2 ;
	    else height = i_height ;

	    XtConfigureWidget(w, new_x, new_y, width, height, 
		window_info.border_width) ;

            /* Redraw the affected widget */
            XClearArea (display, window, 0, 0, 0, 0, TRUE) ;

	    /* Send the position of the widget to the dm */
	    w_id = xht_get_dm_id (w) ;

	    trans = isd_start_transaction();

	    send_h = height ; /* Assigning to an int, so dm doesn't choke */  
	    send_w = width ;  

	    isd_put_shared_data(trans, w_id, 
		values_node->widget_name, "method", "resize");
	    isd_put_shared_data(trans, w_id, 
		values_node->widget_name, "x", &ulx);
	    isd_put_shared_data(trans, w_id, 
		values_node->widget_name, "y", &uly);
	    isd_put_shared_data(trans, w_id, 
		values_node->widget_name, "height", &send_h);
	    isd_put_shared_data(trans, w_id, 
		values_node->widget_name, "width", &send_w);

	    isd_commit_transaction(trans);

	    if (x22_debug > 2)
        	fprintf (stderr, "x22_child_resize: Sent to dm.\n");

	    if (x22_debug > 1)
        	fprintf (stderr, "x22_child_resize: Leaving.\n");

            return;
	}

        /* Take care of all the little things that have changed. */
        XQueryPointer(display, window, &root,  &sub_window,
                      &root_x, &root_y, &cur_x, &cur_y, &ptrmask);

        if ((root_x != prev_x) || (root_y != prev_y))
        {
            /* Box position has changed. */
            switch(corner)
            {
                case UL:

                    ulx += root_x - prev_x;
                    uly += root_y - prev_y;
                    break ;

                case UR:

                    uly += root_y - prev_y;
                    lrx += root_x - prev_x;
                    break ;

                case LL:

                    ulx += root_x - prev_x;
                    lry += root_y - prev_y;
                    break ;

                case LR:

                    lrx += root_x - prev_x;
                    lry += root_y - prev_y;
                    break ;
	    }

            /* Box needs to be restored. */
            num_vectors = StoreBox(box, ulx, uly, lrx, lry);
	}

        /* Save old box position. */
        prev_x = root_x;
        prev_y = root_y;

        DrawBox();
    }
}


/*--------------------------------------------------------------------------*\
|  Name: x22_child_move
|
|  Description:
|       x22_child_move moves child widgets within their parents, 
|       according to the position selected by the mouse.
|
|  Parameters:
|       widget          the widget which generated the move event.
|       event           not used.
|       params          not used.
|       num_params      not used.
|
|  Returns: Nothing.
|
|  Author:
|       Rick Kazman
|       rnk@sei.cmu.edu
|
|       3/15/89  created  rnk
|
\*--------------------------------------------------------------------------*/

void x22_child_move(w, event, params, num_params)
    Widget w;
    XEvent *event ;
    String *params ;
    Cardinal *num_params ;
{
    Widget parent ;
    Window window, parent_win ;
    Display *display ;
    Arg local_args[50] ;
    Position x, y ;
    int status, i ;
    register int prev_x;                /* Previous event window X location.*/
    register int prev_y;                /* Previous event window Y location.*/
    XWindowAttributes window_info;      /* Event window information.    */
    int cur_x;                          /* Current event window X location. */
    int cur_y;                          /* Current event window Y location. */
    int root_x;                         /* Root window X location. */
    int root_y;                         /* Root window Y location. */
    int ulx, uly;                       /* Event window upper left X and Y. */
    int lrx, lry;                       /* Event window lower right X and Y.*/
    int num_vectors;                    /* Number of vectors in box. */
    int ptrmask;                        /* state of ptr when queried */
    Window sub_window;                  /* Query mouse event sub-window. */
    Window root;                        /* Query mouse event root. */
    XEvent button_event;                /* Button event packet. */
    XSegment box[MAX_BOX_VECTORS];      /* Box vertex buffer. */
    transaction_type trans;		/* Transaction to send to dm */
    iid_id_type	w_id;			/* id of the current widget */
    boolean move, resize ;		/* geometry management switches */
    x22_widget_values_node *values_node;/* widget's values node */
    boolean found ;			/* flag */

    if (x22_debug > 1)
    {
        fprintf (stderr, "x22_child_move: Entering.\n");
        fprintf (stderr, "x22_child_move: widget=%x\n", w) ;
    }

    /* Loop upwards from child to parent, searching for the first widget */
    /* in the hierarchy which allows moving.				 */
    found = FALSE ;
    while (!found)
    {
	values_node = (x22_widget_values_node *) x22_get_widget_values(w) ;
        if (values_node == NULL) return ;
	if (!values_node->allow_user_move) w = XtParent(w) ;
	else found = TRUE ;
	if (w == NULL) return ;
    }

    /* Find the display and the window id of the widget and its parent */
    display = XtDisplay(w) ;
    window = XtWindow(w) ;
    parent = XtParent(w) ;
    parent_win = XtWindow(parent) ;

    /* Change the cursor. */
    XChangeActivePointerGrab(display,EVENTMASK,ArrowCrossCursor,CurrentTime);

    /* Clear the vector buffers. */
    bzero(box, sizeof(box));

    /* Gather info on the event window. */
    if (!XGetWindowAttributes(display, window, &window_info))
        fprintf(stderr, "x22_child_move:  Can't get window attributes\n") ;

    /* Initialize movement variables. */
    ulx = window_info.x;
    uly = window_info.y;
    lrx = window_info.x + window_info.width +
                     (window_info.border_width << 1) - 1;
    lry = window_info.y + window_info.height +
                     (window_info.border_width << 1) - 1;

    /* Store the box. */
    num_vectors = StoreBox(box, ulx, uly, lrx, lry);

    XQueryPointer(display, window, &root,  &sub_window,
                      &root_x, &root_y, &cur_x, &cur_y, &ptrmask) ;
    prev_x = root_x ;
    prev_y = root_y ;

    /* Process any pending exposure events before drawing the box. */
    while (QLength(display) > 0) {
        XPeekEvent(display, &button_event);
        if (((XAnyEvent *)&button_event)->window == window &&
                (button_event.type == ButtonPress ||
                        button_event.type == ButtonRelease))
            break;
        XNextEvent(display, &button_event);
    }

    /* Freeze the server so we get a solid box instead of a flickering one */
    XGrabServer(display);

    /* Now draw the box.  */
    DrawBox();

    /* Main loop. */
    while (TRUE)
    {
        /* Check to see if we have a change in mouse button status.
         * This is how we get out of this "while" loop.         */
        XNextEvent(display, &button_event);

        /* Process the pending events, this sequence is the only
         * way out of the loop and the routine.    */

        if ((button_event.type != ButtonPress) &&
            (button_event.type != ButtonRelease) &&
            (button_event.type != MotionNotify))
        {
            continue; /* spurious menu event... */
	}

        DrawBox();
        XUngrabServer(display);

        if (button_event.type == ButtonRelease)
        {
            /* The button was released, so move the window.      */
            XtMoveWidget(w, ulx, uly); 

	    /* Send the position of the widget to the dm */
	    w_id = xht_get_dm_id (w) ;

	    trans = isd_start_transaction();

	    isd_put_shared_data(trans, w_id, 
		values_node->widget_name, "method", "move");
	    isd_put_shared_data(trans, w_id, 
		values_node->widget_name, "x", &ulx);
	    isd_put_shared_data(trans, w_id, 
		values_node->widget_name, "y", &uly);

	    isd_commit_transaction (trans);

	    if (x22_debug > 2)
        	fprintf (stderr, "x22_child_move: Sent to dm.\n");

	    if (x22_debug > 1)
        	fprintf (stderr, "x22_child_move: Leaving.\n");

            return;
	}

        /* Take care of all the little things that have changed. */
        XQueryPointer(display, window, &root,  &sub_window,
                      &root_x, &root_y, &cur_x, &cur_y, &ptrmask);
        if ((root_x != prev_x) || (root_y != prev_y))
        {

            /* Box position has changed. */
            ulx += root_x - prev_x;
            uly += root_y - prev_y;
            lrx += root_x - prev_x;
            lry += root_y - prev_y;

            /* Box needs to be restored. */
            num_vectors = StoreBox(box, ulx, uly, lrx, lry);
	}

        /* Save old box position. */
        prev_x = root_x;
        prev_y = root_y;
        DrawBox();
    }
}
