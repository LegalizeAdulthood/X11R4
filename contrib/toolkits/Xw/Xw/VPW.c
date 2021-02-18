/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        VPW.c
 **
 **   Project:     X Widgets Library
 **
 **   Description: X Widgets Vertical Paned Window code.
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1987, 1988 by Digital Equipment Corporation, Maynard,
 **             Massachusetts, and the Massachusetts Institute of Technology,
 **             Cambridge, Massachusetts
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard, Digital or  M.I.T.  not be used in advertising or 
 **   publicity pertaining to distribution of the software without 
 **   written prior permission.
 **   
 **   DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 **   ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 **   DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 **   ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 **   WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 **   ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 **   SOFTWARE.
 **   
 *****************************************************************************
 *************************************<+>*************************************/

#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/cursorfont.h>
#include <X11/StringDefs.h>
#include <Xw/XwP.h>
#include <Xw/Xw.h>
#include <Xw/VPWP.h>
#include <Xw/VPW.h>
#include <Xw/Sash.h>

#ifndef XtCMin
#define XtCMin			"Min"
#define XtCMax			"Max"
#endif

/*****************************************************************************
 *
 * Full instance record declaration of VPane
 *
 ***************************************************************** ***********/


typedef enum {UpperPane='U', LowerPane='L', ThisBorderOnly='T'} Direction;


#define PaneInfo(w)	((Pane)(w)->core.constraints)
#define IsPane(w)	(PaneInfo(w)->isPane)
#define PaneIndex(w)	(PaneInfo(w)->position)
#define offset(field) XtOffset(XwVPanedWidget, v_paned.field)



/****************************************************************
 *
 * VPanedWindow Resources
 *
 ****************************************************************/

static XtResource resources[] = {
    {XtNsashIndent, XtCSashIndent, XtRPosition, sizeof(Position),
	 offset(sash_indent), XtRString, "-10"},
    {XtNborderFrame, XtCBorderWidth,XtRInt, sizeof(int),
         offset(border_frame),XtRString, "1"},
    {XtNpadding, XtCPadding, XtRInt, sizeof(int),
	 offset(padding), XtRString, "3"},
    {XtNframed, XtCBoolean, XtRBoolean, sizeof(Boolean),
         offset(framed), XtRString, "TRUE"},
    {XtNrefigureMode, XtCBoolean, XtRBoolean, sizeof(Boolean),
         offset(refiguremode), XtRString, "TRUE"},
};

#undef offset

static Boolean defFalse = False;
static int def0 = 0;
static int def1 = 1;
static int defM1 = 10000;


/****************************************************************
 *
 * VPanedWindow SubResources For Its Constraint Record
 *
 ****************************************************************/

#define offset(field) XtOffset(Pane, field)

static XtResource subresources[] = {
    {XtNallowResize, XtCBoolean, XtRBoolean, sizeof(Boolean),
	 offset(allow_resize), XtRBoolean, (caddr_t)&defFalse},
    {XtNmin, XtCMin, XtRDimension, sizeof(Dimension),
         offset(min), XtRInt, (caddr_t)&def1},
    {XtNmax, XtCMax, XtRDimension, sizeof(Dimension),
         offset(max), XtRInt, (caddr_t)&defM1},
    {XtNskipAdjust, XtCBoolean, XtRBoolean, sizeof(Boolean),
         offset(skip_adjust), XtRBoolean, (caddr_t)&defFalse},
};

#undef offset


static void GetFlipGC();
static void ConstraintInit();
static void Initialize();
static void Realize();
static void Destroy();
static void ConstraintDestroy();
static void Resize();
static Boolean SetValues();
static XtGeometryResult GeometryManager();
static void ChangeManaged();
static Cardinal InsertOrder();
static void InsertChild();
static void PaneInitialize();
static Boolean PaneSetValues();


/*************************************<->*************************************
 *
 *
 *   Description:  VPaned full class record
 *   -----------
 *************************************<->***********************************/

XwVPanedClassRec XwvPanedClassRec = {
   {
/* core class fields */
    /* superclass         */   (WidgetClass) &XwmanagerClassRec,
    /* class name         */   "VPanedWindow",
    /* size               */   sizeof(XwVPanedRec),
    /* class initialize   */   NULL,
    /* class_part_inite   */   NULL,
    /* class_inited       */   FALSE,
    /* initialize         */   Initialize,
    /* initialize_hook    */   NULL,
    /* realize            */   Realize,
    /* actions            */   NULL,
    /* num_actions        */   0,
    /* resourses          */   resources,
    /* resource_count     */   XtNumber(resources),
    /* xrm_class          */   NULLQUARK,
    /* compress_motion    */   TRUE,
    /* compress_exposure  */   TRUE,
    /* compress_enter/lv  */   TRUE,
    /* visible_interest   */   FALSE,
    /* destroy            */   Destroy,
    /* resize             */   Resize,
    /* expose             */   NULL,
    /* set_values         */   SetValues,
    /* set_values_hook    */   NULL,
    /* set_values_almost  */   XtInheritSetValuesAlmost,
    /* get_values_hook    */   NULL,
    /* accept_focus       */   NULL,
    /* version            */   XtVersion,
    /* callback_private   */   NULL,
    /* tm_table           */   NULL,
    /* query_geometry	  */   NULL,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
   }, {
/* composite class fields */
    /* geometry_manager   */   GeometryManager,
    /* change_managed     */   ChangeManaged,
    /* insert_child       */   InsertChild,
    /* delete_child       */   XtInheritDeleteChild,
			       NULL,
   }, {
/* constraint class fields */
    /* subresourses       */   subresources,
    /* subresource_count  */   XtNumber(subresources),
    /* constraint_size    */   sizeof(XwVPanedConstraintsRec),
    /* initialize         */   ConstraintInit,
    /* destroy            */   ConstraintDestroy,
    /* set_values         */   PaneSetValues,
			       NULL,
  },{
/* manager_class fields */
   /* traversal handler   */    (XwTraversalProc) XtInheritTraversalProc,
   /* translations        */	NULL,
   },
/* No XwVPaned class fields */
};

WidgetClass XwvPanedWidgetClass = (WidgetClass) &XwvPanedClassRec;

static int CursorNums[4] = {0,
			    XC_sb_up_arrow,
			    XC_sb_v_double_arrow,
			    XC_sb_down_arrow};




/*************************************<->*************************************
 *
 *  NeedsAdjusting (vpw)
 *
 *   Description:
 *   -----------
 *     Calculate the height needed to fully display this vpaned window.
 *   Take the current setting of "framed" into account.  If "framed"
 *   then the entire window border of each pane is visible, plus it is
 *   inset into the vpw by "padding's worth of  pixels
 *
 *************************************<->***********************************/

static Dimension NeedsAdjusting(vpw)
 register XwVPanedWidget vpw;
{
   int needed, i;

   needed = 0;
   for (i = 0; i < vpw->v_paned.pane_count; i++) 
    {
       needed += PaneInfo(vpw->manager.managed_children[i])->dheight
                      + vpw->v_paned.border_frame;
       if (vpw->v_paned.framed)
          needed += vpw->v_paned.border_frame  + vpw->v_paned.padding;

    }

   if (vpw->v_paned.pane_count > 0)
     {
       if (vpw->v_paned.framed)
         needed += vpw->v_paned.padding;
       else
         needed -= vpw->v_paned.border_frame;
     }

   return (needed != vpw->core.height) ? needed : 0 ;
}


/*************************************<->*************************************
 *
 *  AdjustVPanedHeight
 *
 *   Description:
 *   -----------
 *     Request a new size for the vertical paned window from its parent.
 *     If the requested new height is less than 1, then ask for size of 1.
 *     Since we don't 
 *
 *************************************<->***********************************/
static void AdjustVPanedHeight(vpw, newheight)
  XwVPanedWidget vpw;
  Dimension newheight;
{
    Dimension replyWidth, replyHeight;
    XtGeometryResult result;

    if (newheight < 1) newheight = 1;
    result = XtMakeResizeRequest( (Widget)vpw, vpw->core.width, newheight,
				  &replyWidth, &replyHeight );

  /* WITH XTINHERITSETVALUESALMOST WE SHOULDN'T NEED THIS ! */
    if (result == XtGeometryAlmost) {
	result = XtMakeResizeRequest( (Widget)vpw, replyWidth, replyHeight,
				      NULL, NULL );  
    }
}


/*************************************<->*************************************
 *
 *  ResetDHeights
 *
 *   Description:
 *   -----------
 *     Set the desired height field in the constraint record for each
 *     pane.
 *
 *************************************<->***********************************/
static void ResetDHeights(vpw)
  XwVPanedWidget vpw;
{
    register WidgetList children = vpw->composite.children;
    Widget *childP;
    int i;

    for (i=0, childP = vpw->manager.managed_children; 
          i < vpw->v_paned.pane_count;  childP++, i++)
             PaneInfo(*childP)->dheight = (*childP)->core.height;
}


/*************************************<->*************************************
 *
 *  RefigureLocations
 *
 *   Description:
 *   -----------
 *   This is the workhorse routine which actually computes where the children
 *   are going to be placed.  It honors any Min/Max constraints placed on a
 *   pane as well as honoring the direction to do the refiguring.
 *
 *************************************<->***********************************/
static void RefigureLocations(vpw, index, dir)
  register XwVPanedWidget vpw;
  int index;
  Direction dir;
{
    WidgetList children = vpw->manager.managed_children;
    int num_panes = vpw->v_paned.pane_count;
    int _dir = (dir == UpperPane) ? 1 : (dir == LowerPane) ? -1 : 0;
    int padding;
    Pane pane;
    register Widget *childP;
    Position y;
    Dimension heightused;
    int cdir, i, adjustment;
    Widget top;

    if (num_panes == 0 || !vpw->v_paned.refiguremode)
	return;

    if (vpw->v_paned.framed)
       padding = 2*vpw->v_paned.border_frame + vpw->v_paned.padding;
    else
       padding = vpw->v_paned.border_frame;

   /*
    * ENFORCE THE MIN/MAX CONSTRAINTS; ALSO KEEP TRACK OF THE 
    * TOTAL HEIGHT NEEDED TO DISPLAY VPW BASED ON DESIRED HEIGHTS OF
    * PANES.
    */
   heightused = 0;
   for (childP = children, i = 0; i < num_panes; childP++, i++) 
     {
       pane = PaneInfo(*childP);
       if (pane->dheight < pane->min) pane->dheight = pane->min;
       else
        if (pane->dheight > pane->max) pane->dheight = pane->max;
       heightused += pane->dheight + padding;
     }


   /*
    * MAKE SURE THAT "framed" IS TAKEN INTO ACCOUNT (IF FRAMED, PANE'S
    * BORDERS ARE SHOWN, OTHERWISE THEY OVERLAP.
    */

    if (vpw->v_paned.framed)
       heightused += vpw->v_paned.padding;
    else 
      heightused -= vpw->v_paned.border_frame;


   /*
    * NOW, IF THE DIRECTION IS "ThisBorderOnly" AND heightused IS
    * NOT EQUAL TO core.height THEN ADJUST THE TWO PANES INVOLVED.
    * NOTE: THERE ARE ALWAYS 2 PANES CHOSEN FOR "ThisBorderOnly"
    * (OF COURSE, THEY COULD BE THE SAME PANE)
    */
    if (dir == ThisBorderOnly && heightused != vpw->core.height) 
     {
       top=PaneInfo((vpw->v_paned.top_pane)->sash)->sash;

       adjustment = vpw->core.height - heightused;
       if (((heightused < vpw->core.height) && 
           (vpw->v_paned.top_pane->dheight < top->core.height)) ||
            ((heightused > vpw->core.height) &&
              (vpw->v_paned.top_pane->dheight > top->core.height)))
                vpw->v_paned.top_pane->dheight += adjustment;
       else
          vpw->v_paned.bottom_pane->dheight += adjustment;
       heightused += adjustment;
     }
    

    childP = children + index;
    if (dir == UpperPane && index != num_panes - 1) childP++;
    cdir = _dir;
    while (heightused != vpw->core.height) {
	pane = PaneInfo(*childP);
	if (!pane->skip_adjust || cdir != _dir) {
	    Dimension old = pane->dheight;
	    if (heightused < vpw->core.height)
	        pane->dheight+= vpw->core.height - heightused;
	    else
	        if (heightused - vpw->core.height < pane->dheight)
		    pane->dheight-= heightused - vpw->core.height;
	        else
		    pane->dheight = 1;
	    if (pane->dheight < pane->min) pane->dheight = pane->min;
	    if (pane->dheight > pane->max) pane->dheight = pane->max;
	    heightused += (pane->dheight - old);
	}
	childP+= cdir;
 /*
  * WE GET INTO THE NEXT WHILE LOOP WHEN WE HAVE EXHAUSTED OUR
  * LIST OF CHILDREN AND WE STILL NEED TO REDISTRIBUTE A CHANGE IN
  * HEIGHT, NOW WE WILL TRY TO CHANGE DIRECTION AND SEE IF THERE
  * IS A PANE BACK WHERE WE STARTED FROM THAT CAN ABSORB THE
  * SHORTAGE/OVERAGE
  */
	while (childP < children || childP - children >= num_panes) {
	    cdir = -cdir;
	    if (cdir == _dir) {
    		if (vpw->v_paned.framed) y = vpw->v_paned.padding;
    		else y = -(vpw->v_paned.border_frame);
    		for (childP = children, i = 0; i < num_panes; childP++, i++) {
       			PaneInfo(*childP)->dy = y;
       			y += PaneInfo(*childP)->dheight + padding;
    		}
                /* make sure the sum of the pane heights 
                   are not greater than the vpane height*/
                if (y > vpw->core.height) {
                     childP = children + index;
                     pane = PaneInfo(*childP);
                     pane->dheight = (pane->dheight - (y - vpw->core.height)); 
                } else
                    return;
             }
	     childP = children + index + cdir;

       /* HANDLE SPECIAL CASE */
           if ((index == 0) && (cdir < 0)) childP++;
	}
    }
    if (vpw->v_paned.framed)
       y = vpw->v_paned.padding;
    else
       y = -(vpw->v_paned.border_frame);

    for (childP = children, i = 0; i < num_panes; childP++, i++) {
       PaneInfo(*childP)->dy = y;
       y += PaneInfo(*childP)->dheight + padding;
    }
    if (y > vpw->core.height)
       printf("newheight = %d, it is too big", y);
}


/*************************************<->*************************************
 *
 *  CommitNewLocations
 *
 *   Description:
 *   -----------
 *    Use the core width/height, the border width specified in "border_frame"
 *    as the border width for all panes.  It also raises the sash for the
 *    pane, and prevents the sash from disappearing from the screen.
 *
 *************************************<->***********************************/

static void CommitNewLocations(vpw)
XwVPanedWidget vpw;
{
    WidgetList children = vpw->manager.managed_children;
    int num_panes = vpw->v_paned.pane_count;
    register Widget *childP;
    XWindowChanges changes;
    int i, xoffset;

    changes.stack_mode = Above;

    if (vpw->v_paned.framed)
      xoffset = vpw->v_paned.padding;
    else
      xoffset = -(vpw->v_paned.border_frame);

    for (childP = children, i = 0; i < num_panes; childP++, i++) {
	register Pane pane = PaneInfo(*childP);
	register Widget sash = pane->sash;

	XtMoveWidget(*childP, xoffset, pane->dy);
	XtResizeWidget(*childP, 
                       ((vpw->v_paned.framed) ? 
                          (vpw->core.width - 2*(vpw->v_paned.border_frame +
						vpw->v_paned.padding)) :
                           vpw->core.width), pane->dheight,
		             vpw->v_paned.border_frame);             

     if (sash)  /* IF THIS IS NOT NULL */
       {
	/* Move and Display the Sash */
        if (vpw->v_paned.sash_indent < 0)
  	     changes.x = vpw->core.width + vpw->v_paned.sash_indent
		        - sash->core.width - sash->core.border_width*2;
        else
             changes.x = vpw->v_paned.sash_indent;


         /* PREVENT SASH FROM DISAPPEARING FROM SCREEN */
        if ((changes.x > (vpw->core.width - sash->core.width)) || 
           (changes.x < 0))
             changes.x = 0;

	changes.y = (*childP)->core.y + (*childP)->core.height -
		    sash->core.height/2 - sash->core.border_width + 1;

	/* This should match XtMoveWidget, except that we're
	 * also insuring the sash is Raised in the same request */

	sash->core.x = changes.x;
	sash->core.y = changes.y;

	if (XtIsRealized(pane->sash))
	    XConfigureWindow( XtDisplay(pane->sash), XtWindow(pane->sash),
			      CWX | CWY | CWStackMode, &changes );
      }
    }
}


/*************************************<->*************************************
 *
 *  RefigureLocationsAndCommit 
 *
 *   Description:
 *   -----------
 *    A utility call that does the call to calculate the pane layout 
 *    and then move the panes to their new locations.
 *
 *************************************<->***********************************/

static void RefigureLocationsAndCommit(vpw, index, dir)
  XwVPanedWidget vpw;
  Direction dir;
{
    if (vpw->v_paned.refiguremode) {
	RefigureLocations(vpw, index, dir);
	CommitNewLocations(vpw);
    }
}


/*************************************<->*************************************
 *
 *  DrawTrackLines
 *
 *   Description:
 *   -----------
 *     Erase any old track lines (point are kept in the pane's constraint
 *     record--olddy field) and then draw new track lines across the top
 *     of all panes (except the first).  These lines will be interactively 
 *     moved (by other routines) to respond to the user's request to resize 
 *     panes within the VPane Manager.
 *
 *************************************<->***********************************/

static void DrawTrackLines(vpw)
  XwVPanedWidget vpw;
{
    Widget *childP;
    Pane pane;
    Widget *children = vpw->manager.managed_children;
    int num_panes = vpw->v_paned.pane_count;
    int thickness = vpw->v_paned.border_frame;
    int x1, x2;

    if (vpw->v_paned.framed)
      { x1= vpw->v_paned.padding;  x2= vpw->core.width - x1; }
    else
      { x1=0;  x2=vpw->core.width; }


    for (childP = children + 1; childP - children < num_panes; childP++) {
        pane = PaneInfo(*childP);
	if (pane->olddy != pane->dy) {
	    XDrawLine(XtDisplay(vpw), XtWindow(vpw), vpw->v_paned.flipgc,
		      x1, pane->olddy, x2, pane->olddy);
	    XDrawLine(XtDisplay(vpw), XtWindow(vpw), vpw->v_paned.flipgc,
		      x1, pane->dy, x2, pane->dy);
	    pane->olddy = pane->dy;
	}
    }
}

/*************************************<->*************************************
 *
 *  EraseTrackLines
 *
 *   Description:
 *   -----------
 *   After the user has stopped adjusting the pane sizes, erase the last
 *   set of track lines (remember that DrawTrackLines erases old track
 *   lines before drawing new ones).
 *
 *************************************<->***********************************/

static void EraseTrackLines(vpw)
  XwVPanedWidget vpw;
{
    Widget *childP;
    Widget *children = vpw->manager.managed_children;
    int num_panes = vpw->v_paned.pane_count;
    int thickness = vpw->v_paned.border_frame;
    int x1, x2;

    if (vpw->v_paned.framed)
      { x1= vpw->v_paned.padding;  x2= vpw->core.width - x1; }
    else
      { x1=0;  x2=vpw->core.width; }


    for (childP = children + 1; childP - children < num_panes; childP++)
	XDrawLine(XtDisplay(vpw), XtWindow(vpw), vpw->v_paned.flipgc,
		  x1, PaneInfo(*childP)->olddy, x2,
		       PaneInfo(*childP)->olddy);
}


/*************************************<->*************************************
 *
 *  HandleSash
 *
 *   Description:
 *   -----------
 *    Selection Events on the sashes invoke this routine through 
 *    callbacks.  An adjustment of the size of the panes always involves
 *    2 panes (one to shrink and one to grow) if the type of sash grab is 
 *    "This Window Only" then both a top and bottom pane are selected (these are the
 *    panes which will be adjusted); if the grab type is "Upper Pane"
 *    then only a "top" pane is chosen and the correct "bottom
 *    pane will be determined by the "RefigureLocations" routine.  If
 *    the grab type is "Lower" then only a "bottom" pane is chosen and the
 *    the correct "top" pane is found by the "RefigureLocations" routine.
 *
 *************************************<->***********************************/

static void HandleSash(w, closure, callData)
  Widget	w;
  caddr_t	closure;	/* (paned) Widget (unused) */
  caddr_t	callData;	/* SashCallData */
{
    SashCallData call_data = (SashCallData)callData;
    register XwVPanedWidget vpw = (XwVPanedWidget)w->core.parent;
    register WidgetList children = vpw->manager.managed_children;
    int num_panes = vpw->v_paned.pane_count;
    int     diff, y, index;
    Widget *childP;
    Pane pane;
    Time time;
    char ActionType;
    Direction direction;

    if (call_data->num_params == 0 ||
	(*call_data->params[0] != 'C' && call_data->num_params < 2))
      	XtError( "XwVPaned SashAction has too few parameters." );

    ActionType = *call_data->params[0];
    if (islower(ActionType)) ActionType = toupper(ActionType);

    switch (call_data->event->xany.type) {
        case ButtonPress:
	case ButtonRelease: time = call_data->event->xbutton.time;
			    y = call_data->event->xbutton.y_root;
			    break;

	case KeyPress:
	case KeyRelease:    time = call_data->event->xkey.time;
			    y = call_data->event->xkey.y_root;
			    break;

        case MotionNotify:  time = call_data->event->xmotion.time;
			    y = call_data->event->xmotion.y_root;
			    break;

	default:	    time = CurrentTime;
			    y = vpw->v_paned.starty;
    }

    if (ActionType != 'C') {
        direction = (Direction)*call_data->params[1];
	if (islower((char)direction))
	    direction = (Direction)toupper((char)direction);
    }

    switch (ActionType) {
	case 'S':		/* Start adjustment */
	    vpw->v_paned.top_pane = vpw->v_paned.bottom_pane = (Pane)NULL;
            if (direction == LowerPane || direction == ThisBorderOnly) {

              /* NOTE THAT w IS A SASH, TO GET POSITION WE HAVE
                 TO GET INDEX OF PANE ASSOCIATED WITH THIS SASH */
	        index = PaneIndex(PaneInfo(w)->sash);
                if (index < (num_panes-1)) index++;

		pane = PaneInfo(children[index]);
		while (pane->max == pane->min && index < num_panes - 1)
		    pane = PaneInfo(children[++index]);
		vpw->v_paned.bottom_pane = pane;
	    }
            if (direction == UpperPane || direction == ThisBorderOnly) {

              /* NOTE THAT w IS A SASH, TO GET POSITION WE HAVE
                 TO GET INDEX OF PANE ASSOCIATED WITH THIS SASH */
	        index = PaneIndex(PaneInfo(w)->sash);

		pane = PaneInfo(children[index]);
		while (pane->max == pane->min && index > 0)
		    pane = PaneInfo(children[--index]);
		vpw->v_paned.top_pane = pane;
	    }
	    (void) XGrabPointer(XtDisplay(vpw), XtWindow(w), False,
				XtBuildEventMask(w),
				GrabModeAsync, GrabModeAsync, None,
				XCreateFontCursor(XtDisplay(vpw),
				CursorNums[call_data->event->xbutton.button]),
				time);
	    vpw->v_paned.starty = y;

	    for (childP = children; childP - children < num_panes; childP++)
		PaneInfo(*childP)->olddy = -99;
	    break;

	case 'M': 
	    ResetDHeights( vpw );
	    diff = y - vpw->v_paned.starty;
	    if (vpw->v_paned.top_pane) {
		/* make sure heights don't go negative */
		if ((-diff) >= (int)vpw->v_paned.top_pane->dheight) {
		    /* can't add as much to other pane */
		    diff = -vpw->v_paned.top_pane->dheight + 1;
		    vpw->v_paned.top_pane->dheight = 1;
		}
		else
		    vpw->v_paned.top_pane->dheight += diff;
	    }
	    if (vpw->v_paned.bottom_pane) {
		if (diff >= (int)vpw->v_paned.bottom_pane->dheight) {
		    if (vpw->v_paned.top_pane)
			/* added too much to other pane */
			vpw->v_paned.top_pane->dheight -=
			    diff - vpw->v_paned.bottom_pane->dheight - 1;
		    vpw->v_paned.bottom_pane->dheight = 1;
		}
		else
		    vpw->v_paned.bottom_pane->dheight -= diff;
	    }
	    RefigureLocations(vpw, PaneIndex(PaneInfo(w)->sash), direction);
	    DrawTrackLines(vpw);
	    break;

	case 'C':
	    XUngrabPointer(XtDisplay(vpw), CurrentTime);
	    EraseTrackLines(vpw);
	    CommitNewLocations(vpw);
	    break;

	default:
	    XtError( "XwVPaned SashAction(); 1st parameter invalid" );
     }
}



/*************************************<->*************************************
 *
 * GeometryManager
 *
 *   Description:
 *   -----------
 *  The Geometry Manager only allows changes after Realize if
 *  allow_resize is True in the constraints record.  It
 *  only allows height changes, but offers the requested height
 *  as a compromise if both width and height changes were requested.
 *  As all good Geometry Managers should, we will return No if the
 *  request will have no effect; i.e. when the requestor is already
 *  of the desired geometry.
 *
 *************************************<->***********************************/

static XtGeometryResult GeometryManager(w, request, reply)
Widget w;
XtWidgetGeometry *request, *reply;
{
    XwVPanedWidget vpw = (XwVPanedWidget) w->core.parent;
    XtGeometryMask mask = request->request_mode;
    Dimension old_dheight, new_height;
    XtWidgetGeometry allowed;
    Pane pane = PaneInfo(w);
    Boolean request_altered = FALSE;

/* DISALLOW RESIZES IF FLAG FOR THIS PANE SAYS NO */    
    if (XtIsRealized((Widget)vpw) && !pane->allow_resize)
      return XtGeometryNo;

/* REJECT ATTEMPTS THAT DO NOT WANT TO ADJUST HEIGHT, OR REQUESTS WHERE
   WIDGET IS ALREADY EQUAL TO SIZE THEY ARE REQUESTING  */
    if (!(mask & CWHeight) || mask & ~(CWWidth | CWHeight))
        return XtGeometryNo;

/* MAKE SURE THAT MIN/MAX CONSTRAINTS ARE HONORED. AND IF WE NEED TO
   ENFORCE IT, RETURN XtGeometryAlmost */
   if (request->height < pane->min)
    {  request->height = pane->min; request_altered = TRUE; }
   if (request->height > pane->max)
    {  request->height = pane->max;  request_altered = TRUE; }

    allowed.request_mode = mask;
    allowed.width = vpw->core.width;
    if (vpw->v_paned.framed)
     allowed.width -= 2*(vpw->v_paned.border_frame + vpw->v_paned.padding);

    /* try out the new size */
    ResetDHeights( vpw );
    old_dheight = pane->dheight;
    pane->dheight = request->height;

    if (new_height = NeedsAdjusting(vpw)) {
        Dimension old_height = vpw->core.height;

/* WHAT IF SOME EVIL WIDGET GETS AN XtGeometryAlmost ANSWER AND CHOOSES
   NOT TO CHANGE?  WE'VE ALREADY ALTERED THE SIZE OF VPANE. */
        AdjustVPanedHeight( vpw, new_height );
        if (request->height < pane->min) allowed.height = pane->min;
        else if (request->height > pane->max) allowed.height = pane->max;
	else allowed.height = request->height;
    }
    else
        allowed.height = request->height;

    if (((mask & CWWidth) && request->width != allowed.width)
	|| (request->height != allowed.height) || request_altered) {
      pane->dheight = old_dheight;
      *reply = allowed;
      return XtGeometryAlmost;
    }

    w->core.height = request->height;
    XtResizeWidget(w, request->width, request->height,
                    vpw->v_paned.border_frame);
    RefigureLocations(vpw, PaneIndex(w), UpperPane);

    if (XtIsRealized((Widget)vpw))
        CommitNewLocations( vpw );

    return XtGeometryYes;
}


/************************************************************************
 *
 *  Initialize
 *     The main widget instance initialization routine.
 *
 ************************************************************************/

static void Initialize(request, new)
  Widget request, new;
{
  XwVPanedWidget vpw = (XwVPanedWidget) new;

 /* Protect against empty widgets */
  vpw->v_paned.pane_count = 0;

/*************************************************************
 * NOTE: that sash_indent is made to conform to a correct size
 * during changed managed time/layout time.  Since the size of
 * the window may change we won't require that 
 * abs(sash_indent) <= width of a pane.
 ************************************************************/

  vpw -> composite.insert_position = (XtOrderProc) InsertOrder;
  vpw -> v_paned.recursively_called = FALSE;

  if (vpw->v_paned.padding < 0)
    {
      XtWarning ("XwVPane Initialize: padding < 0");
      vpw->v_paned.padding = 0;
    }

  if (vpw->v_paned.border_frame < 0)
    {
      XtWarning ("XwVPane Initialize: borderFrame < 0");
      vpw->v_paned.border_frame = 0;
    }

/* DON'T ALLOW HEIGHT/WIDTH TO BE 0, OR X DOESN'T HANDLE THIS WELL */
  
  if (vpw->core.width == 0) vpw->core.width = 10;
  if (vpw->core.height == 0) vpw->core.height = 10;

}


/************************************************************************
 *
 *  Constraint Initialize
 *
 *  This routine is called to initialize the constraint record for
 *  every child inserted into the v_paned window.
 *
 ************************************************************************/

static void ConstraintInit(request, new)
  Widget request, new;
{
  Pane pane = PaneInfo(new);
  Dimension height;

  height = new->core.height;
  pane->position = 0;

  if (pane->min < 1)
    {
     XtWarning ("XwVPane Initialize: min < 1");
     pane->min = 1;
   }

  if (pane->max < 1)
    {
     XtWarning ("XwVPane Initialize: max < 1");
     pane->max = pane->min + 1;
     }

  if (pane->min > pane->max)
    {
     XtWarning("XwVPane Initialize: min > max");
    pane->max = pane->min + 1;
   }

  /* ENFORCE MIN/MAX */
  if (height < pane->min) height = pane->min;
  if (height > pane->max) height = pane->max;

  if (height != new->core.height)
    XtResizeWidget(new, new->core.width, height, new->core.border_width);


}


/*************************************<->*************************************
 *
 *  Realize
 *
 *   Description:
 *   -----------
 *    Create our window, set NW gravity (_XwRealize), set up
 *
 *
 *************************************<->***********************************/
static void Realize(w, valueMask, attributes)
Widget w;
Mask *valueMask;
XSetWindowAttributes *attributes;
{
    register XwVPanedWidget vpw = (XwVPanedWidget)w;
    WidgetList children = vpw->manager.managed_children;
    int num_children = vpw->manager.num_managed_children;
    unsigned long valuemask;
    XGCValues values;
    Widget *childP;


    _XwRealize(w, valueMask, attributes);
    GetFlipGC(vpw);   

    /* one last time, in case we grew to try to return an
     * XtGeometryAlmost for a child, but the child decided not to grow 
     * or in case some child grew itself and we didn't hear about it.
     */
    Resize( vpw );

    /* now we have to make sure all the sashs are on above their
     * panes, which means that we have to realize all our children
     * here and now.  If we realize from the beginning of the list,
     * then the sashs (which are at the end) will be Above by default. */

    for (childP = children; childP - children < num_children; childP++)
	    XtRealizeWidget( *childP );

} /* Realize */


/*************************************<->*************************************
 *
 *  Destroy
 *
 *************************************<->***********************************/

static void Destroy(w)
Widget w;
{
    XwVPanedWidget vpw = (XwVPanedWidget)w;

    XtDestroyGC( vpw->v_paned.flipgc );

} /* Destroy */


/***************************************************************************
 *
 * static Cardinal
 * InsertOrder (cw, args, p_num_args)
 *
 *   This function searches through the composite widget's list of children
 *   to find the correct insertion position for the new child.  If the
 *   new child is an ordinary child (not a subclass of XwSashWidget)
 *   the position returned will cause it to be inserted after the other
 *   ordinary children but before any Sashs; if the new child is a 
 *   sash the position returned will cause it to be inserted at the
 *   end of the list.  This procedure does not examine the arglist.
 *
 ************************************************************************/

static Cardinal InsertOrder (cw, args, p_num_args)

   CompositeWidget cw;         /* Composite widget being inserted into */
   ArgList         args;       /* Arglist, we don't care */
   Cardinal      * p_num_args; /* Number of argument pairs, we don't care */

{
   Cardinal i=0;

   while ((i < cw->composite.num_children) &&
            (! XtIsSubclass(cw->composite.children[i], XwsashWidgetClass)))
                i++;
   return (i);
}



/*************************************<->*************************************
 *
 *  InsertChild()
 *
 *************************************<->***********************************/

static void InsertChild(w)
    register Widget w;
{
    int _XW_NARGS = 0;
    ArgList args = NULL;
    Cardinal *argcount = (Cardinal *) &_XW_NARGS;

   XwVPanedWidget vpw = (XwVPanedWidget)w->core.parent;
   Pane pane = PaneInfo(w);

   static XtCallbackRec SashCallback[] = { {HandleSash, NULL}, {NULL, NULL} };

   static Arg sashargs[] = {
      {XtNcallback, (XtArgVal)SashCallback},
   };


   /* 
    * Insert the child widget in the composite children list with an
    * insertion procedure exported from Manager.c, it, in turn, will
    * make use of the special insert procedure, InsertOrder, defined
    * above.  Essentially, ordinary panes are grouped together at the
    * beginning of the list of composite children, sashs are always 
    * put at the end of the list.
    */

   _XwManagerInsertChild (w, args, argcount);


  /*
   * If we are creating a sash for an ordinary pane, then just
   * return here. However, before we do, set its "isPane" flag
   * to false, meaning that this is NOT a pane; if it is a 
   * pane then set its "isPane" flag to TRUE.
   */
   if (w->core.widget_class == XwsashWidgetClass) 
     {
       pane->isPane = FALSE;
       return;
     }
    pane->isPane = TRUE;


   /* If we create a sash then have the pane's constraint rec point
    * to the sash, ignore the constraint rec of the sash (Yes, it 
    * gets one).  If we don't create a sash for the pane, just set
    * that field to NULL.   
    */
      SashCallback[0].closure = (caddr_t)w;
      pane->sash = XtCreateWidget("sash", XwsashWidgetClass,
					 (Widget)vpw, sashargs, 
					     XtNumber(sashargs));
      PaneInfo(pane->sash)->sash = w;

} /* InsertChild */




/*************************************<->*************************************
 *
 * ChangeManaged
 *
 *************************************<->***********************************/

static void ChangeManaged(w)
   Widget w;
{
   register XwVPanedWidget vpw = (XwVPanedWidget)w;
   int num_children = vpw->composite.num_children;
   register Widget *children;
   Widget sash;
   int i, index, width, height, childWidth;
   int num_panes =0;
   Dimension needed;
   register Widget *childP;

  /* 
   * THIS PREVENTS US FROM RE-ENTERING THIS CODE AS WE MANAGE/UNMANAGE
   * THE SASHES
   */
   if (vpw->v_paned.recursively_called++)  return;

  /* 
   * NOW RECOMPUTE THE LIST OF MANAGED CHILDREN.
   */
   _XwReManageChildren((XwManagerWidget)vpw);
   children = vpw->manager.managed_children;

  /*
   * COUNT THE NUMBER OF PANES THAT ARE MANAGED.
   */
   childP = children;
   while ( (num_panes < vpw->manager.num_managed_children) &&
             (*childP++)->core.widget_class != XwsashWidgetClass)
                  num_panes++;

   vpw->v_paned.pane_count = num_panes;


  /* 
   * SET WIDTH OF PANED WINDOW EQUAL TO THAT OF WIDEST CHILD
   */

       childWidth = 0;
       height = 0;
       for (childP = children, i = 0; i < num_panes; childP++, i++)
	 {
	   if ((*childP)->core.width > childWidth) 
	              childWidth = (*childP)->core.width;
           height += (*childP)->core.height;
	 }
       if (childWidth < 1) childWidth = 1;
       width = childWidth;
       if (height < 1) height = 1;

       if (vpw->v_paned.framed)
	{
         width = childWidth + 
	          2*(vpw->v_paned.border_frame + vpw->v_paned.padding);
         height += vpw->v_paned.padding*(num_panes+1) +
	           2*vpw->v_paned.border_frame*num_panes;
         }
       else
         height += vpw->v_paned.border_frame*(num_panes-1);

       if (XtMakeResizeRequest(w, width, height, &width, &height)
	   == XtGeometryAlmost)
	   XtMakeResizeRequest(w, width, height, &width, &height);

       vpw->core.width = width;
       vpw->core.height = height;


  /*
   * NOW SCAN THE COMPOSITE LIST OF CHILDREN, AND MAKE SURE
   * THAT THEIR MANAGEMENT SETTING REFLECTS THAT OF THEIR PANE.
   */

   for (childP = vpw->composite.children, i = 0; 
                     i < num_children; childP++, i++) 
    {
      if (! IsPane(*childP)) break;  /* jump out of loop */
      sash = PaneInfo(*childP)->sash;

      /* Realize child now so it won't get realized and put on the 
         top of the stack, above the sash, when it is realized later */
      if (XtIsRealized(vpw) && XtIsManaged(*childP))
          XtRealizeWidget(*childP);

      /* KEEP SOME RECORD OF DESIRED HEIGHT */
      PaneInfo(*childP)->dheight = (*childP)->core.height;

      /* ENFORCE OUR BORDERWIDTH */
      XtResizeWidget(*childP, childWidth, (*childP)->core.height,
		      vpw->v_paned.border_frame);
       if ((XtIsManaged(*childP)) && (*childP != children[num_panes-1])
          && (PaneInfo(*childP)->min != PaneInfo(*childP)->max))
         {
              if (sash)
		{
                  if (!XtIsManaged(sash)) XtManageChild(sash);
  	          if (XtIsRealized(sash))
		    XRaiseWindow( XtDisplay(sash), XtWindow(sash) );
		}
	    }
	  else
            if (sash)
               if (XtIsManaged(sash)) XtUnmanageChild(sash);
     }

  /* NOW CHANGEMANAGED CAN BE ENTERED NORMALLY */
   vpw->v_paned.recursively_called = False;

   /*
    * TRAVERSE MANAGED PANES AND SET THE POSITION FIELD IN THE CONSTRAINT
    * RECORD TO 0, 1, 2, 3, 4 ETC.
    */
   
   childP = vpw->manager.managed_children;
   for (index = 0; index < vpw->v_paned.pane_count; childP++)
      (PaneInfo(*childP))->position = index++;
        

   /* see if the height of the VPaned needs to be adjusted to fit all the panes */
   if (needed = NeedsAdjusting(vpw))
      AdjustVPanedHeight(vpw, needed);

   ResetDHeights( vpw );
   RefigureLocations( vpw, 0, UpperPane );

   if (XtIsRealized((Widget)vpw))
       CommitNewLocations( vpw );

} /* ChangeManaged */


/*************************************<->*************************************
 *
 *  Resize
 *
 *************************************<->***********************************/
static void Resize(vpw)
    XwVPanedWidget vpw;
{
    RefigureLocationsAndCommit( vpw, vpw->v_paned.pane_count - 1, LowerPane );

} /* Resize */



/*************************************<->*************************************
 *
 *  SetValues
 *   -----------------
 *
 *************************************<->***********************************/
static Boolean SetValues(oldvpw, requestvpw, newvpw, last)
    XwVPanedWidget oldvpw, requestvpw, newvpw;
    Boolean last;
{
   Boolean returnFlag = False;
   WidgetList children = newvpw->manager.managed_children;
   int num_panes = newvpw->v_paned.pane_count;
   register Widget *childP;
   int i, x, y, width, height, childWidth;

   if ((oldvpw->core.background_pixel != newvpw->core.background_pixel) ||
      (oldvpw->v_paned.border_frame != newvpw->v_paned.border_frame))
         {
          XtDestroyGC(newvpw->v_paned.flipgc);
          GetFlipGC(newvpw);
          returnFlag = True;
	 }        
   if (oldvpw->v_paned.sash_indent != newvpw->v_paned.sash_indent ||
       oldvpw->v_paned.border_frame != newvpw->v_paned.border_frame ||
       oldvpw->v_paned.framed != newvpw->v_paned.framed ||
       oldvpw->v_paned.padding != newvpw->v_paned.padding) {
       childWidth = 0;
       height = 0;
       for (childP = children, i = 0; i < num_panes; childP++, i++) {
         register Pane pane = PaneInfo(*childP);
	 register Widget sash = pane->sash;

         if (oldvpw->v_paned.sash_indent != newvpw->v_paned.sash_indent) {
            if (sash)  /* IF THIS IS NOT NULL */
            {
	        /* Move and Display the Sash */
                if (newvpw->v_paned.sash_indent < 0)
  	             x = newvpw->core.width + newvpw->v_paned.sash_indent
		         - sash->core.width - sash->core.border_width*2;
                else
                     x = newvpw->v_paned.sash_indent;


                /* PREVENT SASH FROM DISAPPEARING FROM SCREEN */
                if ((x > (newvpw->core.width - sash->core.width)) || (x < 0))
                     x = 0;

	        y = (*childP)->core.y + (*childP)->core.height -
 	            sash->core.height/2 - sash->core.border_width + 1;

                XtMoveWidget(sash, x, y);

             }
           }
           if (oldvpw->v_paned.border_frame != newvpw->v_paned.border_frame ||
               oldvpw->v_paned.framed != newvpw->v_paned.framed ||
               oldvpw->v_paned.padding != newvpw->v_paned.padding) {
	         if ((*childP)->core.width > childWidth) 
	              childWidth = (*childP)->core.width;
                 height += (*childP)->core.height;
           }
       }
       if (oldvpw->v_paned.border_frame != newvpw->v_paned.border_frame ||
           oldvpw->v_paned.framed != newvpw->v_paned.framed ||
           oldvpw->v_paned.padding != newvpw->v_paned.padding) {
              if (childWidth < 1) childWidth = 1;
              width = childWidth;
              if (height < 1) height = 1;
          
              if (newvpw->v_paned.framed)
              {
                  width = childWidth + 2*(newvpw->v_paned.border_frame + 
                          newvpw->v_paned.padding);
                  height += newvpw->v_paned.padding*(num_panes+1) +
	                    2*newvpw->v_paned.border_frame*num_panes;
               }
               else
                  height += newvpw->v_paned.border_frame*(num_panes-1);
             
               newvpw->core.width = width;
               newvpw->core.height = height;
               returnFlag = True;
           }
      
   }

   return(returnFlag);
} /* SetValues */



/*************************************<->*************************************
 *
 *  PaneSetValues
 *
 *
 *************************************<->***********************************/
static Boolean PaneSetValues(old, request, new)
    Widget old, request, new;
{
   static XtCallbackRec SashCallback[] = { {HandleSash, NULL}, {NULL, NULL} };

   static Arg sashargs[] = {
      {XtNcallback, (XtArgVal)SashCallback},
   };

    int count;
    Pane old_pane = PaneInfo(old);
    Pane new_pane = PaneInfo(new);

    if (old_pane->min != new_pane->min || old_pane->max != new_pane->max)
    {
        
	if (new_pane->min < 1)
	{
	    XtWarning ("XwVPane SetValues: min < 1");
	    new_pane->min = 1;
	}

	if (new_pane->max < 1)
	{
	    XtWarning ("XwVPane SetValues: max < 1");
	    new_pane->max = new_pane->min + 1;
	}

	if (new_pane->min > new_pane->max)
	{
	      XtWarning("XwVPane SetValues: min > max");
	      new_pane->max = new_pane->min;
	}

      	if ((new_pane->min == new_pane->max) && 
            (new_pane->sash != NULL))
      	{
             XtUnmanageChild(new_pane->sash);
      	} 
      	else
      	{
              count =((XwVPanedWidget)new->core.parent)->v_paned.pane_count -1;
              if ((new_pane->min != new_pane->max) &&  (new_pane->position != 
						        count))
              { 
                  if (new_pane->sash == NULL)
	          {
	               SashCallback[0].closure = (caddr_t)new;
	               new_pane->sash = XtCreateManagedWidget("sash", 
                                    XwsashWidgetClass, (Widget)new->core.parent,
                                    sashargs, XtNumber(sashargs));

                       if (XtIsRealized(new))  XtRealizeWidget(new_pane->sash);
            
	          }
                  else
                       XtManageChild(new_pane->sash);
	       }
	}

	if (new_pane->min > new->core.height) new->core.height = new_pane->min;
	if (new_pane->max < new->core.height) new->core.height = new_pane->max;
	old_pane->min = new_pane->min;
	old_pane->max = new_pane->max;

        return(True);
   }

   return False;
} 



/*************************************<->*************************************
 *
 *  ConstraintDestroy
 *
 *   Description:
 *   -----------
 *    Destroy the sash of any pane which is being destroyed.
 *
 *
 *************************************<->***********************************/
static void ConstraintDestroy(w)
Widget w;
{
   /* 
    * If this is an ordinary pane,  delete its sash (if it has one)
    * and then invoke the standard inherited delete child routines.
    */
   if ((w->core.widget_class != XwsashWidgetClass) &&
        (PaneInfo(w)->sash != NULL))
          XtDestroyWidget(PaneInfo(w)->sash);

}


/*************************************<->*************************************
 *
 *  void GetFlipGC(vpw)
 *          XwVPanedWidget  vpw;
 *
 *   Description:
 *   -----------
 *   Create a GC which can be used to draw/erase track lines when the
 *   the size of the panes is being changed.
 *
 *************************************<->***********************************/

static void GetFlipGC(vpw)
    XwVPanedWidget vpw;
{
    XGCValues	values;
    unsigned long valuemask;

    values.foreground	= vpw->core.background_pixel;
    values.subwindow_mode = IncludeInferiors;
    valuemask = GCForeground | GCSubwindowMode;
    values.function = GXinvert;

    if (vpw->v_paned.border_frame == 1)
      values.line_width = 0;	/* Take advantage of fast server lines. */
    else
      values.line_width = vpw->v_paned.border_frame;

    values.line_style = LineSolid;
    valuemask |= GCLineWidth | GCLineStyle | GCFunction;
    vpw->v_paned.flipgc = XtGetGC(vpw, valuemask, &values);

}



