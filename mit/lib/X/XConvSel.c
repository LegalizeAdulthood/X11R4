#include "copyright.h"

/* $XConsortium: XConvSel.c,v 11.6 88/09/06 16:05:10 jim Exp $ */
/* Copyright    Massachusetts Institute of Technology    1986	*/

#include "Xlibint.h"

XConvertSelection(dpy, selection, target, property, requestor, time)
register Display *dpy;
Atom selection, target;
Atom property;
Window requestor;
Time time;
{
    register xConvertSelectionReq *req;

    LockDisplay(dpy);
    GetReq(ConvertSelection, req);
    req->selection = selection;
    req->target = target;
    req->property = property;
    req->requestor = requestor;
    req->time = time;
    UnlockDisplay(dpy);
    SyncHandle();
}
