/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

#include <IntrinsicI.h>
#include <X11/XawMisc.h>
#include <St/StwP.h>

Boolean StuContainsRectangle(rect, insideRect)
    XRectangle rect, insideRect;
{
    return (rect.x <= insideRect.x
               && rect.y <= insideRect.y
               && (rect.x + rect.width) >= (insideRect.x + insideRect.width)
               && (rect.y + rect.height) >= (insideRect.y + insideRect.height));
}

XRectangle StuMergeRectangles(rect1, rect2)
    XRectangle rect1, rect2;
{
    XRectangle rect;

    rect.x = min(rect1.x, rect2.x);
    rect.y = min(rect1.y, rect2.y);
    rect.width = max((rect1.x + rect1.width), (rect2.x + rect2.width))
                     - rect.x;
    rect.height = max((rect1.y + rect1.height), (rect2.y + rect2.height))
                      - rect.y;

    return rect;
}

Boolean StuEquivalentRectangles(rect1, rect2)
    XRectangle rect1, rect2;
{
    return (rect1.x == rect2.x
               && rect1.y == rect2.y
               && rect1.width == rect2.width
               && rect1.height == rect2.height);
}

Boolean StuContainsPoint(rect, point)
    XRectangle rect;
    StPoint point;
{
    return (rect.x <= point.x
               && rect.y <= point.y
               && (rect.x + rect.width) >= point.x
               && (rect.y + rect.height) >= point.y);
}

void StuFillRectangle(w, rect, gc)
    Widget w;
    XRectangle rect;
    GC gc;
{
    XFillRectangle(XtDisplay(w), XtWindow(w), gc, rect.x, rect.y,
        rect.width, rect.height);
}
