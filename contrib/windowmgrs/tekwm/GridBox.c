#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: GridBox.c,v 5.7 89/11/22 15:10:45 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

/*
 * MODIFICATION HISTORY
 *
 * 000 -- M. Gancarz, DEC Ultrix Engineering Group
 * 001 -- Loretta Guarino Reid, DEC Ultrix Engineering Group,
 *  Western Software Lab. Converted to X11.
 * 002 -- Jordan Hubbard, Ardent Computer.
 *  Changes for titled windows.
 */

#ifndef lint
static char *sccsid = "@(#)StoreGridBox.c	3.8	1/24/86";
#endif

/*
 *	StoreBox - This subroutine is used by the X Window Manager (xwm)
 *	to store the vertices for the movement/resize grid and box in a
 *	vertex list.
 */

#include "awm.h"

/*
 * Store the vertices for the movement resize grid and box in a vertex list.
 */
int StoreBox(box, ulx, uly, lrx, lry)
register XSegment box[];
int ulx;			/* Upper left X coordinate. */
int uly;			/* Upper left Y coordinate. */
int lrx;			/* Lower right X coordinate. */
int lry;			/* Lower right Y coordinate. */
{
    int i;

    Entry("StoreBox")

    /*
     * Xor in.
     */
    /* draw a box at the inside edge of the window border */
    /* avoid redrawing corners so xor does not erase them */
    box[0].x1 = ulx + 1; box[0].y1 = uly;
    box[0].x2 = lrx - 1; box[0].y2 = uly;

    box[1].x1 = lrx; box[1].y1 = uly;
    box[1].x2 = lrx; box[1].y2 = lry;

    box[2].x1 = lrx - 1; box[2].y1 = lry;
    box[2].x2 = ulx + 1; box[2].y2 = lry;

    box[3].x1 = ulx; box[3].y1 = lry;
    box[3].x2 = ulx; box[3].y2 = uly;

    i = 4;  /* next line */

    if (Grid) {
	/* now we make an x in the box */
	box[i].x1 = lrx - 1; box[i].y1 = lry - 1;
	box[i].x2 = ulx + 1; box[i++].y2 = uly + 1;

	box[i].x1 = ulx + 1; box[i].y1 = lry - 1;
	box[i].x2 = lrx - 1; box[i++].y2 = uly + 1;
    }

    /*
     * Do not erase if we're freezing the screen.
     */
    if (Freeze)
        Leave(i)

    /*
     * From here on we're retracing the segments to clear the
     * box using GXxor.
     */
    box[i].x1 = ulx + 1; box[i].y1 = uly;
    box[i].x2 = lrx - 1; box[i++].y2 = uly;

    box[i].x1 = lrx; box[i].y1 = uly;
    box[i].x2 = lrx; box[i++].y2 = lry;

    box[i].x1 = lrx - 1; box[i].y1 = lry;
    box[i].x2 = ulx + 1; box[i++].y2 = lry;

    box[i].x1 = ulx; box[i].y1 = lry;
    box[i].x2 = ulx; box[i++].y2 = uly;

    if (Grid) {
	box[i].x1 = lrx - 1; box[i].y1 = lry - 1;
	box[i].x2 = ulx + 1; box[i++].y2 = uly + 1;

	box[i].x1 = ulx + 1; box[i].y1 = lry - 1;
	box[i].x2 = lrx - 1; box[i++].y2 = uly + 1;
    }

    /*
     * Total number of segments is i.
     */
    Leave(i)
}

/*
 * Store the vertices for the movement grid and box in a vertex list.
 * This is just like the routine above, but it adds a line for titled
 * windows, and if total border width > 1, a box for the outside of the
 * border.
 */

int StoreTitleBox(box, ulx, uly, lrx, lry, awi)
register XSegment box[];
int ulx;			/* Upper left X coordinate. */
int uly;			/* Upper left Y coordinate. */
int lrx;			/* Lower right X coordinate. */
int lry;			/* Lower right Y coordinate. */
AwmInfoPtr awi;
{
    int TitleHeight, BorderExcess;
    int clientulx, clientlrx, clientuly, clientlry, titlebottom;
    int i;

    Entry("StoreTitleBox")

    if (!awi)
	Leave(FALSE)

    TitleHeight = (awi->attrs & AT_TITLE) ? (titleHeight + WBorderWidth) : 0;
    BorderExcess = ((awi->attrs & AT_BORDER) ? BContext : 0) + 
		    WBorderWidth - 1;
    /* draw a box at the inside edge of the window/titlebar border */
    /* Incoming coordinates are the overall frame dimensions. */
    clientulx = ulx + BorderExcess;
    clientuly = uly + BorderExcess;
    titlebottom = clientuly + TitleHeight;
    /* draw line at bottom edge of title/client window border */
    clientlrx = lrx - BorderExcess;
    clientlry = lry - BorderExcess;

    i = 0;  /* first line */

    /*
     * Xor in.
     */
    /* Avoid redrawing corners so xor does not erase them. */
    box[i].x1 = ulx + 1; box[i].y1 = uly;
    box[i].x2 = lrx - 1; box[i++].y2 = uly;

    box[i].x1 = lrx; box[i].y1 = uly;
    box[i].x2 = lrx; box[i++].y2 = lry;

    box[i].x1 = lrx - 1; box[i].y1 = lry;
    box[i].x2 = ulx + 1; box[i++].y2 = lry;

    box[i].x1 = ulx; box[i].y1 = lry;
    box[i].x2 = ulx; box[i++].y2 = uly;

    if (BorderExcess > 0) {
	/* draw a box at the inside of the total border (outside title) */
	box[i].x1 = clientulx + 1; box[i].y1 = clientuly;
	box[i].x2 = clientlrx - 1; box[i++].y2 = clientuly;

	box[i].x1 = clientlrx;     box[i].y1 = clientuly;
	box[i].x2 = clientlrx;     box[i++].y2 = clientlry;

	box[i].x1 = clientlrx - 1; box[i].y1 = clientlry;
	box[i].x2 = clientulx + 1; box[i++].y2 = clientlry;

	box[i].x1 = clientulx;     box[i].y1 = clientlry;
	box[i].x2 = clientulx;     box[i++].y2 = clientuly;
    }
    if (TitleHeight > 0) {
	/* draw a line separating title from client */
	box[i].x1 = clientulx + 1; box[i].y1 = titlebottom;
	box[i].x2 = clientlrx - 1; box[i++].y2 = titlebottom;
    }

    if (Grid) {
	/* now we make an x in the inside box */
	box[i].x1 = clientlrx - 1; box[i].y1 = clientlry - 1;
	box[i].x2 = clientulx + 1; box[i++].y2 = titlebottom + 1;

	box[i].x1 = clientulx + 1; box[i].y1 = clientlry - 1;
	box[i].x2 = clientlrx - 1; box[i++].y2 = titlebottom + 1;
    }

    /*
     * Do not erase if we're freezing the screen.
     */
    if (Freeze)
        Leave(i)

    /*
     * From here on we're retracing the segments to clear the
     * grid using GXxor.
     */
    box[i].x1 = ulx + 1; box[i].y1 = uly;
    box[i].x2 = lrx - 1; box[i++].y2 = uly;

    box[i].x1 = lrx; box[i].y1 = uly;
    box[i].x2 = lrx; box[i++].y2 = lry;

    box[i].x1 = lrx - 1; box[i].y1 = lry;
    box[i].x2 = ulx + 1; box[i++].y2 = lry;

    box[i].x1 = ulx; box[i].y1 = lry;
    box[i].x2 = ulx; box[i++].y2 = uly;

    if (BorderExcess > 0) {
	/* draw a box at the inside of the total border (outside title) */
	box[i].x1 = clientulx + 1; box[i].y1 = clientuly;
	box[i].x2 = clientlrx - 1; box[i++].y2 = clientuly;

	box[i].x1 = clientlrx;     box[i].y1 = clientuly;
	box[i].x2 = clientlrx;     box[i++].y2 = clientlry;

	box[i].x1 = clientlrx - 1; box[i].y1 = clientlry;
	box[i].x2 = clientulx + 1; box[i++].y2 = clientlry;

	box[i].x1 = clientulx;     box[i].y1 = clientlry;
	box[i].x2 = clientulx;     box[i++].y2 = clientuly;
    }
    if (TitleHeight > 0) {
	/* draw a line separating title from client */
	box[i].x1 = clientulx + 1; box[i].y1 = titlebottom;
	box[i].x2 = clientlrx - 1; box[i++].y2 = titlebottom;
    }

    if (Grid) {
	/* now we make an x in the inside box */
	box[i].x1 = clientlrx - 1; box[i].y1 = clientlry - 1;
	box[i].x2 = clientulx + 1; box[i++].y2 = titlebottom + 1;

	box[i].x1 = clientulx + 1; box[i].y1 = clientlry - 1;
	box[i].x2 = clientlrx - 1; box[i++].y2 = titlebottom + 1;
    }

    /*
     * Total number of segments is i.
     */
    if (MAX_BOX_VECTORS < i)
	Error("GridBox: MAX_BOX_VECTORS must be redefined larger.");
    Leave(i)
}

/*
 * Store the vertices for the resize grid and box in a vertex list.
 * This is just like the routine above, but the grid delineates the resize
 * pick boundaries.
 */

int StoreResizeBox(box, ulx, uly, lrx, lry, awi)
register XSegment box[];
int ulx;			/* Upper left X coordinate. */
int uly;			/* Upper left Y coordinate. */
int lrx;			/* Lower right X coordinate. */
int lry;			/* Lower right Y coordinate. */
AwmInfoPtr awi;
{
    register int xfourth, yfourth;
    int x1fourth, y1fourth;
    int x2fourth, y2fourth;
    int TitleHeight, BorderExcess;
    int clientulx, clientlrx, clientuly, clientlry, titlebottom;
    int i;

    Entry("StoreResizeBox")

    if (!awi)
	Leave(FALSE)

    TitleHeight = (awi->attrs & AT_TITLE) ? (titleHeight + WBorderWidth) : 0;
    BorderExcess = ((awi->attrs & AT_BORDER) ? BContext : 0) + 
		    WBorderWidth - 1;
    /* draw a box at the inside edge of the window/titlebar border */
    /* Incoming coordinates are the overall frame dimensions. */
    clientulx = ulx + BorderExcess;
    clientuly = uly + BorderExcess;
    titlebottom = clientuly + TitleHeight;
    /* draw line at bottom edge of title/client window border */
    clientlrx = lrx - BorderExcess;
    clientlry = lry - BorderExcess;

    i = 0;  /* first line */

    /*
     * Xor in.
     */
    /* Avoid redrawing corners so xor does not erase them. */
    box[i].x1 = ulx + 1; box[i].y1 = uly;
    box[i].x2 = lrx - 1; box[i++].y2 = uly;

    box[i].x1 = lrx; box[i].y1 = uly;
    box[i].x2 = lrx; box[i++].y2 = lry;

    box[i].x1 = lrx - 1; box[i].y1 = lry;
    box[i].x2 = ulx + 1; box[i++].y2 = lry;

    box[i].x1 = ulx; box[i].y1 = lry;
    box[i].x2 = ulx; box[i++].y2 = uly;

    /*
     * These are the X and Y calculations for the parts of the grid that
     * are dependent on the division by 4 calculations.
     */

    if (BorderExcess > 0) {
	/* draw a box at the inside of the total border (outside title) */
	box[i].x1 = clientulx + 1; box[i].y1 = clientuly;
	box[i].x2 = clientlrx - 1; box[i++].y2 = clientuly;

	box[i].x1 = clientlrx;     box[i].y1 = clientuly;
	box[i].x2 = clientlrx;     box[i++].y2 = clientlry;

	box[i].x1 = clientlrx - 1; box[i].y1 = clientlry;
	box[i].x2 = clientulx + 1; box[i++].y2 = clientlry;

	box[i].x1 = clientulx;     box[i].y1 = clientlry;
	box[i].x2 = clientulx;     box[i++].y2 = clientuly;
    }
    if (TitleHeight > 0) {
	/* draw a line separating title from client */
	box[i].x1 = clientulx + 1; box[i].y1 = titlebottom;
	box[i].x2 = clientlrx - 1; box[i++].y2 = titlebottom;
    }

    if (Grid) {
	/*
	 * Y dimension fourth.
	 */
	yfourth = (clientlry - titlebottom) / RESIZE_BOUNDARY;
	y1fourth = titlebottom + yfourth;
	y2fourth = clientlry - yfourth;
   
	/*
	 * X dimension fourth.
	 */
	xfourth = (clientlrx - clientulx) / RESIZE_BOUNDARY;
	x1fourth = clientulx + xfourth;
	x2fourth = clientlrx - xfourth;

	/* down */
	box[i].x1 = x1fourth; box[i].y1 = clientlry - 1;
	box[i].x2 = x2fourth; box[i++].y2 = titlebottom + 1;

	box[i].x1 = x2fourth; box[i].y1 = clientlry - 1;
	box[i].x2 = x1fourth; box[i++].y2 = titlebottom + 1;
	/* across */
	box[i].x1 = clientulx + 1; box[i].y1 = y1fourth;
	box[i].x2 = clientlrx - 1; box[i++].y2 = y2fourth;

	box[i].x1 = clientulx + 1; box[i].y1 = y2fourth;
	box[i].x2 = clientlrx - 1; box[i++].y2 = y1fourth;
    }

    /*
     * Do not erase if we're freezing the screen.
     */
    if (Freeze)
        Leave(i)

    /*
     * From here on we're retracing the segments to clear the
     * grid using GXxor.
     */
    box[i].x1 = ulx + 1; box[i].y1 = uly;
    box[i].x2 = lrx - 1; box[i++].y2 = uly;

    box[i].x1 = lrx; box[i].y1 = uly;
    box[i].x2 = lrx; box[i++].y2 = lry;

    box[i].x1 = lrx - 1; box[i].y1 = lry;
    box[i].x2 = ulx + 1; box[i++].y2 = lry;

    box[i].x1 = ulx; box[i].y1 = lry;
    box[i].x2 = ulx; box[i++].y2 = uly;

    if (BorderExcess > 0) {
	/* draw a box at the inside of the total border (outside title) */
	box[i].x1 = clientulx + 1; box[i].y1 = clientuly;
	box[i].x2 = clientlrx - 1; box[i++].y2 = clientuly;

	box[i].x1 = clientlrx;     box[i].y1 = clientuly;
	box[i].x2 = clientlrx;     box[i++].y2 = clientlry;

	box[i].x1 = clientlrx - 1; box[i].y1 = clientlry;
	box[i].x2 = clientulx + 1; box[i++].y2 = clientlry;

	box[i].x1 = clientulx;     box[i].y1 = clientlry;
	box[i].x2 = clientulx;     box[i++].y2 = clientuly;
    }
    if (TitleHeight > 0) {
	/* draw a line separating title from client */
	box[i].x1 = clientulx + 1; box[i].y1 = titlebottom;
	box[i].x2 = clientlrx - 1; box[i++].y2 = titlebottom;
    }

    if (Grid) {
	/* down */
	box[i].x1 = x1fourth; box[i].y1 = clientlry - 1;
	box[i].x2 = x2fourth; box[i++].y2 = titlebottom + 1;

	box[i].x1 = x2fourth; box[i].y1 = clientlry - 1;
	box[i].x2 = x1fourth; box[i++].y2 = titlebottom + 1;
	/* across */
	box[i].x1 = clientulx + 1; box[i].y1 = y1fourth;
	box[i].x2 = clientlrx - 1; box[i++].y2 = y2fourth;

	box[i].x1 = clientulx + 1; box[i].y1 = y2fourth;
	box[i].x2 = clientlrx - 1; box[i++].y2 = y1fourth;
    }

    /*
     * Total number of segments is i.
     */
    if (MAX_BOX_VECTORS < i)
	Error("GridBox: MAX_BOX_VECTORS must be redefined larger.");
    Leave(i)
}
