#ifndef lint
static char Xrcsid[] = "$XConsortium: Geometry.c,v 1.29 88/09/06 16:27:56 jim Exp $";
/* $oHeader: Geometry.c,v 1.3 88/08/23 11:37:50 asente Exp $ */
#endif lint

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
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

#include "IntrinsicI.h"
#include "Shell.h"

/* Public routines */

XtGeometryResult XtMakeGeometryRequest (widget, request, reply)
    Widget         widget;
    XtWidgetGeometry *request, *reply;
{
    XtWidgetGeometry    junk;
    XtGeometryHandler manager;
    XtGeometryResult returnCode;
    Widget parent = widget->core.parent;
    XtGeometryMask	changeMask;
    Boolean managed, parentRealized,widgetRealized;
    XWindowChanges changes;

    if (parent == NULL) {
	XtErrorMsg("invalidParent","xtMakeGeometryRequest","XtToolkitError",
           "XtMakeGeometryRequest - NULL parent.  Use SetValues instead",
            (String *)NULL, (Cardinal *)NULL); 
    }

    if (XtIsComposite(parent)) {
	parentRealized = XtIsRealized(parent);
	manager = ((CompositeWidgetClass) (parent->core.widget_class))
    		->composite_class.geometry_manager;
    } else if (XtIsCompositeObject(parent)) {
	Widget t = parent->core.parent;
	while (! XtIsWindowObject(t)) t = t->core.parent;
	parentRealized = XtIsRealized(t);
	manager = ((CompositeObjectClass) (parent->core.widget_class))
    		->composite_class.geometry_manager;
	
    } else {
	/* Should never happen - XtManageChildren should have checked */
	XtErrorMsg("invalidParent","xtMakeGeometryRequest","XtToolkitError",
             "XtMakeGeometryRequest - parent not composite",
              (String *)NULL, (Cardinal *)NULL);
    }

    if (manager == (XtGeometryHandler) NULL) {
	XtErrorMsg("invalidGeometryManager","xtMakeGeometryRequest",
                 "XtToolkitError",
                 "XtMakeGeometryRequest - parent has no geometry manger",
                  (String *)NULL, (Cardinal *)NULL);
    }

    if (widget->core.being_destroyed) return XtGeometryNo;

    if (XtIsWindowObject(widget)) {
        widgetRealized = XtIsRealized(widget);
    } else {
        widgetRealized = FALSE;
    };
    managed = XtIsManaged(widget);

    /* see if requesting anything to change */
    changeMask = 0;
    if (request->request_mode & CWStackMode
	&& request->stack_mode != XtSMDontChange) {
	    changeMask |= CWStackMode;
	    if (request->request_mode & CWSibling) changeMask |= CWSibling;
    }
    if (request->request_mode & CWX
	&& widget->core.x != request->x) changeMask |= CWX;
    if (request->request_mode & CWY
	&& widget->core.y != request->y) changeMask |= CWY;
    if (request->request_mode & CWWidth
	&& widget->core.width != request->width) changeMask |= CWWidth;
    if (request->request_mode & CWHeight
	&& widget->core.height != request->height) changeMask |= CWHeight;
    if (request->request_mode & CWBorderWidth
	&& widget->core.border_width != request->border_width)
	changeMask |= CWBorderWidth;
    if (changeMask == NULL) return XtGeometryYes;
    changeMask |= (request->request_mode & XtCWQueryOnly);

    /* keep record of what the widget looks like before calling geometry mgr */
    changes.x = widget->core.x ;
    changes.y = widget->core.y ;
    changes.width = widget->core.width ;
    changes.height = widget->core.height ;
    changes.border_width = widget->core.border_width ;

    if (!managed || !parentRealized) {
	/* Don't get parent's manager involved--assume the answer is yes */
	if (changeMask & XtCWQueryOnly) {
	    /* He was just asking, don't change anything, just tell him yes */
	    return XtGeometryYes;
	} else {
	    /* copy values from request to widget */
	    if (request->request_mode & CWX)
		widget->core.x = request->x;
	    if (request->request_mode & CWY)
		widget->core.y = request->y;
	    if (request->request_mode & CWWidth)
		widget->core.width = request->width;
	    if (request->request_mode & CWHeight)
		widget->core.height = request->height;
	    if (request->request_mode & CWBorderWidth)
		widget->core.border_width = request->border_width;
	    if (!parentRealized) return XtGeometryYes;
	    else returnCode = XtGeometryYes;
	}
    } else {
	/* go ask the widget's geometry manager */
	if (reply == (XtWidgetGeometry *) NULL) {
	    returnCode = (*manager)(widget, request, &junk);
	} else {
	    returnCode = (*manager)(widget, request, reply);
	}
    }

    if (changeMask & XtCWQueryOnly) {
	/* Just asking about it, don't change any geometry fields */
	return returnCode;
    }

    if (returnCode == XtGeometryYes && widgetRealized) {
	/* reconfigure the window (if needed) */
	/* determine reconfigurable items by comparing with "old" widget */

	if (changes.x != widget->core.x) {
	    changeMask |= CWX;
	    changes.x = widget->core.x;
	}
	if (changes.y != widget->core.y) {
	    changeMask |= CWY;
	    changes.y = widget->core.y;
	}
	if (changes.width != widget->core.width) {
	    changeMask |= CWWidth;
	    changes.width = widget->core.width;
	}
	if (changes.height != widget->core.height) {
	    changeMask |= CWHeight;
	    changes.height = widget->core.height;
	}
	if (changes.border_width != widget->core.border_width) {
	    changeMask |= CWBorderWidth;
	    changes.border_width = widget->core.border_width;
	}

	if (changeMask & CWStackMode) {
	    changes.stack_mode = request->stack_mode;
	    if (changeMask & CWSibling) 
		changes.sibling = XtWindow(request->sibling);
	}

	XConfigureWindow(XtDisplay(widget), XtWindow(widget),
		changeMask, &changes);
    } else if (returnCode == XtGeometryDone) returnCode = XtGeometryYes;

    return returnCode;
} /* XtMakeGeometryRequest */

XtGeometryResult XtMakeResizeRequest
	(widget, width, height, replyWidth, replyHeight)
    Widget	widget;
    Dimension	width, height;
    Dimension	*replyWidth, *replyHeight;
{
    XtWidgetGeometry request, reply;
    XtGeometryResult r;

    request.request_mode = CWWidth | CWHeight;
    request.width = width;
    request.height = height;
    r = XtMakeGeometryRequest(widget, &request, &reply);
    if (replyWidth != NULL)
	if (r == XtGeometryAlmost && reply.request_mode & CWWidth)
	    *replyWidth = reply.width;
	else
	    *replyWidth = width;
    if (replyHeight != NULL)
	if (r == XtGeometryAlmost && reply.request_mode & CWHeight)
	    *replyHeight = reply.height;
	else
	    *replyHeight = height;
    return r;
} /* XtMakeResizeRequest */

void XtResizeWindow(w)
    Widget w;
{
    if (XtIsRealized(w)) {
	XWindowChanges changes;
	changes.width = w->core.width;
	changes.height = w->core.height;
	changes.border_width = w->core.border_width;
	XConfigureWindow(XtDisplay(w), XtWindow(w),
	    (unsigned) CWWidth | CWHeight | CWBorderWidth, &changes);
    }
} /* XtResizeWindow */


void XtResizeWidget(w, width, height, borderWidth)
    Widget w;
    Dimension height, width, borderWidth;
{
    XWindowChanges changes;
    Cardinal mask = 0;

    if (w->core.width != width) {
	changes.width = w->core.width = width;
	mask |= CWWidth;
    }

    if (w->core.height != height) {
	changes.height = w->core.height = height;
	mask |= CWHeight;
    }

    if (w->core.border_width != borderWidth) {
	changes.border_width = w->core.border_width = borderWidth;
	mask |= CWBorderWidth;
    }

    if (mask != 0) {
	if (XtIsRealized(w))
	    XConfigureWindow(XtDisplay(w), XtWindow(w), mask, &changes);
    if ((mask & (CWWidth | CWHeight)) &&
	XtClass(w)->core_class.resize != (XtWidgetProc) NULL)
	    (*(w->core.widget_class->core_class.resize))(w);
    }
} /* XtResizeWidget */

void XtConfigureWidget(w, x, y, width, height, borderWidth)
    Widget w;
    Position x, y;
    Dimension height, width, borderWidth;
{
    XWindowChanges changes;
    Cardinal mask = 0;

    if (w->core.x != x) {
	changes.x = w->core.x = x;
	mask |= CWX;
    }

    if (w->core.y != y) {
	changes.y = w->core.y = y;
	mask |= CWY;
    }

    if (w->core.width != width) {
	changes.width = w->core.width = width;
	mask |= CWWidth;
    }

    if (w->core.height != height) {
	changes.height = w->core.height = height;
	mask |= CWHeight;
    }

    if (w->core.border_width != borderWidth) {
	changes.border_width = w->core.border_width = borderWidth;
	mask |= CWBorderWidth;
    }

    if (mask != 0) {
	if (XtIsRealized(w))
	    XConfigureWindow(XtDisplay(w), XtWindow(w), mask, &changes);
    if ((mask & (CWWidth | CWHeight)) &&
	XtClass(w)->core_class.resize != (XtWidgetProc) NULL)
	    (*(w->core.widget_class->core_class.resize))(w);
    }
} /* XtConfigureWidget */

void XtMoveWidget(w, x, y)
    Widget w;
    Position x, y;
{
    if ((w->core.x != x) || (w->core.y != y)) {
	w->core.x = x;
	w->core.y = y;
	if (XtIsRealized(w)) {
	    XMoveWindow(XtDisplay(w), XtWindow(w), w->core.x, w->core.y);
        }
    }
} /* XtMoveWidget */

void XtTranslateCoords(w, x, y, rootx, rooty)
    register Widget w;
    Position x, y;
    register Position *rootx, *rooty;	/* return */
{
    Position garbagex, garbagey;

    if (rootx == NULL) rootx = &garbagex;
    if (rooty == NULL) rooty = &garbagey;

    *rootx = x;
    *rooty = y;

    for (; w != NULL && ! XtIsShell(w); w = w->core.parent) {
	*rootx += w->core.x + w->core.border_width;
	*rooty += w->core.y + w->core.border_width;
    }

    if (w == NULL)
        XtWarningMsg("invalidShell","xtTranslateCoords","XtToolkitError",
                "Widget has no shell ancestor",
		(String *)NULL, (Cardinal *)NULL);
    else {
	*rootx += w->core.x + w->core.border_width;
	*rooty += w->core.y + w->core.border_width;
    }
}

XtGeometryResult XtQueryGeometry(widget, intended, reply)
    Widget widget;
    register XtWidgetGeometry *intended; /* parent's changes; may be NULL */
    XtWidgetGeometry *reply;	/* child's preferred geometry; never NULL */
{
    XtWidgetGeometry null_intended;
    XtGeometryHandler query = XtClass(widget)->core_class.query_geometry;
    XtGeometryResult result;

    reply->request_mode = 0;
    if (query != NULL) {
	if (intended == NULL) {
	    null_intended.request_mode = 0;
	    intended = &null_intended;
	}
	result = (*query) (widget, intended, reply);
    }
    else {
	result = XtGeometryYes;
    }

#define FillIn(mask, field) \
	if (!(reply->request_mode & mask)) reply->field = widget->core.field;

    FillIn(CWX, x);
    FillIn(CWY, y);
    FillIn(CWWidth, width);
    FillIn(CWHeight, height);
    FillIn(CWBorderWidth, border_width);
#undef FillIn

    if (!reply->request_mode & CWStackMode) reply->stack_mode =
XtSMDontChange;

    return result;
  }
