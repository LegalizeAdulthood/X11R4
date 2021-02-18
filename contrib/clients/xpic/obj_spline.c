/* $Header: obj_spline.c,v 1.6 89/07/02 15:58:59 moraes Exp $ */
/* This code is UNBELIEVABLY convoluted and UGLY. Works - just barely! */
/*
 *  The spline routines - These require some really ugly hacks to get
 *  half-decent rubber-banding. And it still leaves some droppings on
 *  the screen. The basic idea is that the segments of the spline are
 *  invariant from midpoint of the defining segment to the midpoint of
 *  the next defining segment. So the non-invariant section is covered
 *  up by the defining segment during rubberbanding. When the spline is
 *  complete, the rubber banded segments are erased and the complete
 *  spline drawn. Yuck! There must be a better way to do this.
 */
#include <values.h>

#include "xpic.h"
#include "windows.h"
#include "spline.h"
#include "gels.h"
#include "draw.h"
#include "assert.h"

static int xmin, xmax, ymin, ymax;	/* Bounding box */
static XPoint *drawVerts;			/*
									 *  Points four points behind current
									 *  point, used by the spline drawing routine
									 */
static PointList *ptList;

spline_event(evtype, mx, my)
{
	switch(evtype) {
	case MOTION | START_MODE:
	case RIGHT  | START_MODE:
	case MIDDLE | START_MODE:
	case REDRAW | START_MODE:
		break;
	case MOTION | END_MODE:
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 2))
			Arrow(picDpy, picWin, verts[2].x, verts[2].y, 
			 verts[1].x, verts[1].y, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, verts[nVerts - 1].x, verts[nVerts - 1].y, 
			 verts[nVerts].x, verts[nVerts].y, gcInvert);
		drawlines(picDpy, picWin, gcInvert, drawVerts, 3, CoordModeOrigin);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat, 
		 CoordModePrevious);
		verts[nVerts].x = verts[nVerts+1].x = mx;
		verts[nVerts].y = verts[nVerts+1].y = my;
		CopyAndFlatten(drawVerts, 3, &flatVerts, &nFlat, &flatSize);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat, 
		 CoordModePrevious);
		drawlines(picDpy, picWin, gcInvert, drawVerts, 3, CoordModeOrigin);
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 2))
			Arrow(picDpy, picWin, verts[2].x, verts[2].y, 
			 verts[1].x, verts[1].y, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, verts[nVerts - 1].x, verts[nVerts - 1].y, 
			 verts[nVerts].x, verts[nVerts].y, gcInvert);
		break;
	case LEFT | START_MODE:
		drawingMode = END_MODE;
		xmin = xmax = verts[0].x = verts[1].x = verts[2].x = 
		 verts[3].x = mx;
		ymin = ymax = verts[0].y = verts[1].y = verts[2].y = 
		 verts[3].y = my;
		drawVerts = verts;
		nVerts = 2;
		CopyAndFlatten(drawVerts, 3, &flatVerts, &nFlat, &flatSize);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat, 
		 CoordModePrevious);
		drawlines(picDpy, picWin, gcInvert, drawVerts, 3, CoordModeOrigin);
		break;
	case LEFT | END_MODE:
		/*
		 *  must always have at least two extra vertices in the
		 *  verts array for the current mouse coordinates
		 */
		if (nVerts + 3 >= maxVerts) {
			maxVerts += INC_VERTS;
#ifdef DEBUG
			(void) fprintf(stderr, "Reallocing verts to %d\n", maxVerts);
#endif
			if ((verts = (XPoint *) realloc((char *) verts,
			 (unsigned) (maxVerts * sizeof(XPoint)))) == NULL) {
				message("No more memory for vertices");
				break;
			}
			drawVerts = &verts[nVerts - 2];
		}
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 2))
			Arrow(picDpy, picWin, verts[2].x, verts[2].y, 
			 verts[1].x, verts[1].y, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, verts[nVerts - 1].x, verts[nVerts - 1].y, 
			 verts[nVerts].x, verts[nVerts].y, gcInvert);
		drawlines(picDpy, picWin, gcInvert, drawVerts, 3, CoordModeOrigin);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat, 
		 CoordModePrevious);
		verts[nVerts].x = verts[nVerts+1].x = verts[nVerts+2].x = mx;
		verts[nVerts].y = verts[nVerts+1].y = verts[nVerts+2].y = my;
		/*
		 *  Really ugly - we'd like leave the last rubber banded section
		 *  around because the spline that we're showing at this point
		 *  consists of lots of rubber-banded segments. But we can't
		 *  leave the rubber banded section from the MOTION drawing,
		 *  because the button may have been pressed at a different
		 *  place, so we invert that, compute a fidged rubber segment
		 *  for this point, draw it, and never invert it back.
		 *  Eventually, the protruding lines from the end segments get
		 *  erased by the defining polyline, (we hope) and the spline
		 *  gets overwritten by the DRAW operation at the end.
		 *  Horrible!!
		 */
		CopyAndFlatten(drawVerts, 3, &flatVerts, &nFlat, &flatSize);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat, 
		 CoordModePrevious);
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 2))
			Arrow(picDpy, picWin, verts[2].x, verts[2].y, 
			 verts[1].x, verts[1].y, gcNormal);
		nVerts++;
		xmin = MIN(xmin, mx);
		xmax = MAX(xmax, mx);
		ymin = MIN(ymin, my);
		ymax = MAX(ymax, my);
		drawVerts++;
		CopyAndFlatten(drawVerts, 3, &flatVerts, &nFlat, &flatSize);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat,
		 CoordModePrevious);
		drawlines(picDpy, picWin, gcInvert, drawVerts, 3, CoordModeOrigin);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, verts[nVerts - 1].x, verts[nVerts - 1].y, 
			 verts[nVerts].x, verts[nVerts].y, gcInvert);
		break;
	case RIGHT | END_MODE:
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 2))
			Arrow(picDpy, picWin, verts[2].x, verts[2].y, verts[1].x, 
			 verts[1].y, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, verts[nVerts - 1].x, verts[nVerts - 1].y, 
			 verts[nVerts].x, verts[nVerts].y, gcInvert);
		drawlines(picDpy, picWin, gcInvert, drawVerts, 3, CoordModeOrigin);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat, 
		 CoordModePrevious);
		/* Use ERASE to clean out junk */
		setwidth(tmpGcErase, lineThickness);
		SETDASHES(tmpGcErase, line_type);
		drawlines(picDpy, picWin, tmpGcErase, verts, nVerts, CoordModeOrigin);
		verts[nVerts].x = verts[nVerts-1].x;
		verts[nVerts].y = verts[nVerts-1].y;
		FlattenSpline(verts, nVerts, &flatVerts, &nFlat, &flatSize);
		nVerts++;
		drawlines(picDpy, picWin, gcNormal, flatVerts, nFlat,
		 CoordModePrevious);
		if (((line_arrow & EN_ARROW) != 0) && (nVerts > 2))
			Arrow(picDpy, picWin, verts[nVerts - 3].x, verts[nVerts - 3].y, 
			 verts[nVerts - 2].x, verts[nVerts - 2].y, gcInvert);
		drawingMode = START_MODE;
		if (nVerts <= 3)
			break;
		if ((ptList = NewPtList(verts, nVerts)) == NULL)
			message("No more memory for line/spline");
		else {				
			AddLineGel(&(CurrentCell->gelList), SPLINE, ptList,
			 line_type | line_arrow, xmin, ymin, xmax, ymax, lineThickness);
			FreeGel(CurrentCell->undoList);
			CurrentCell->undoList = NULL;
			CurrentCell->undo = 1;
			CurrentCell->saved |= MODIFIED;
		}
		break;
	case MIDDLE | END_MODE:
		if ((line_arrow & ST_ARROW) != 0)
			Arrow(picDpy, picWin, verts[2].x, verts[2].y,
 			 verts[1].x, verts[1].y, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, verts[nVerts - 1].x, verts[nVerts - 1].y, 
			 verts[nVerts].x, verts[nVerts].y, gcInvert);
		drawlines(picDpy, picWin, gcInvert, drawVerts, 3, CoordModeOrigin);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat,
		 CoordModePrevious);
		/*
		 *  We use background and GXcopy instead of inverting
		 *  because some of the points on the path may have
		 *  already been turned off by the previous drawlines.
		 */
		setwidth(tmpGcErase, lineThickness);
		SETDASHES(tmpGcErase, line_type);
		drawlines(picDpy, picWin, tmpGcErase, verts, nVerts, CoordModeOrigin);
		verts[nVerts].x = verts[nVerts+1].x = mx;
		verts[nVerts].y = verts[nVerts+1].y = mx;
		nVerts += 2;
		FlattenSpline(verts, nVerts-1, &flatVerts, &nFlat, &flatSize);
		drawlines(picDpy, picWin, tmpGcErase, flatVerts, nFlat, 
		 CoordModePrevious);
		drawingMode = START_MODE;
		break;
	case REDRAW | END_MODE:
		if (((line_arrow & ST_ARROW) != 0) && (nVerts == 2))
			Arrow(picDpy, picWin, verts[2].x, verts[2].y, 
			 verts[1].x, verts[1].y, gcInvert);
		if ((line_arrow & EN_ARROW) != 0)
			Arrow(picDpy, picWin, verts[nVerts - 1].x, verts[nVerts - 1].y, 
			 verts[nVerts].x, verts[nVerts].y, gcInvert);
		/* !! Should call XDraw for the whole spline here  - this is wrong */
		drawlines(picDpy, picWin, gcInvert, drawVerts, 3, CoordModeOrigin);
		drawlines(picDpy, picWin, gcInvert, flatVerts, nFlat,
		 CoordModePrevious);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown SPLINE mode %d", drawingMode);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "spline_event");
}


spline_abort()
{
	spline_event((RIGHT | drawingMode), 0, 0);
}

#define MAXSPLINEBUF	7
static XPoint vbuf[MAXSPLINEBUF];

/*
 *  Copies N vertices from v to vbuf, duplicates the ones at the
 *  beginning and end, and calls the spline routine to flatten them. This
 *  wouldn't be needed if the spline routine properly doubled up the
 *  endpoints, but alas, it doesn't.
 */
/*
 *  NOTE: The new spline routine doubles the endpoints, but expects the
 *  0th and Nth locations in the verts array passed to it to be free
 */
CopyAndFlatten(v, n, flattened, nflat, flatSize)
register XPoint *v;
XPoint *flattened[];
int *nflat;
int *flatSize;
{
	register int i;
	register int j;

	ASSERT(n < MAXSPLINEBUF, "n too large");
	/* Copy the n points */
	for (i = 1, j = 0; j < n; i++, j++) {
		vbuf[i].x = v[j].x;
		vbuf[i].y = v[j].y;
	}
	FlattenSpline(vbuf, n+1, flattened, nflat, flatSize);
}


spline_adj(evtype, gel, mx, my)
int evtype;
Gel *gel;
int mx, my;
{
	static XPoint *v;
	static XPoint *adjusted;
	static int arrowstyle, start, end, npts;
	static Gel *splinegel, *oldsplinegel;
	/*
	 *  Will not need to process MOTION|START_MODE, RIGHT|START_MODE,
	 *  REDRAW|START_MODE - these are taken care of in
	 *  the adj_element routine.
	 */
	switch(evtype) {
	case MOTION | END_MODE:
		DrawSplineSection(v, npts, tmpGcInvert, start, end);
		adjusted->x = mx;
		adjusted->y = my;
		CopyAndFlatten(v, npts, &flatVerts, &nFlat, &flatSize);
		DrawSplineSection(v, npts, tmpGcInvert, start, end);
		break;
	case LEFT | START_MODE:
		splinegel = CopyGel(gel, 1);
		oldsplinegel = gel;
		gel = NULL;
		GetClosestSplinePoint(splinegel, mx, my, &v, &npts, &adjusted, &start, &end);
		/* Spline has been erased in element_adjust, so we redraw inverted */
		GelDraw(splinegel, INVERT);
		setwidth(tmpGcNormal, splinegel->linewidth);
		setwidth(tmpGcInvert, splinegel->linewidth);
		SETDASHES(tmpGcNormal, getlinestyle(splinegel->attributes))
		SETDASHES(tmpGcInvert, getlinestyle(splinegel->attributes))
		if (!start) {
			v--;
			npts++;
		}
		if (!end) {
			npts++;
		}
		CopyAndFlatten(v, npts, &flatVerts, &nFlat, &flatSize);
		/*
		 *  Now we redraw the section of the spline we're going to
		 *  adjust + the end segments. This erases it!
		 */
		drawlines(picDpy, picWin, tmpGcInvert, flatVerts, nFlat, 
		 CoordModePrevious);
		/* Now we redraw the spline section so it shows - yuk. */
		DrawSplineSection(v, npts, tmpGcInvert, start, end);
		drawingMode = END_MODE;
		arrowstyle = getlinearrow(splinegel->attributes);
		start = start && (arrowstyle & ST_ARROW);
		end = end && (arrowstyle & EN_ARROW);
		break;
	case LEFT | END_MODE:
		DrawSplineSection(v, npts, tmpGcInvert, start, end);
		adjusted->x = mx;
		adjusted->y = my;
		/*
		 *  We need to erase the end segments of the spline that
		 *  protrude. The logic (or illogic, is sililar to the junk code
		 *  in the LEFT | END_MODE case in the draw. Go read the comment
		 *  there. It won't help much either. 	
		 */
		CopyAndFlatten(v, npts, &flatVerts, &nFlat, &flatSize);
		drawlines(picDpy, picWin, tmpGcInvert, flatVerts, nFlat, 
		 CoordModePrevious);
		update_box(splinegel->b_box, mx, my);
		GelDraw(splinegel, DRAW);
		(void) PushGel(&(CurrentCell->gelList), splinegel);
		splinegel = NULL;
		FreeGel(CurrentCell->undoList);
		CurrentCell->undoList = oldsplinegel;
		CurrentCell->undo = 1;
		CurrentCell->saved |= MODIFIED;
		drawingMode = START_MODE;
		break;
	case RIGHT | END_MODE:
	case MIDDLE | END_MODE:
		DrawSplineSection(v, npts, tmpGcInvert, start, end);
		GelDraw(oldsplinegel, DRAW);
		(void) PushUnderUndo(&(CurrentCell->gelList), oldsplinegel, 
		 CurrentCell->undo);
		oldsplinegel = NULL;
		FreeGel(splinegel);
		splinegel = NULL;
		if (evtype == (MIDDLE | END_MODE))
			ClearGelFlags(CurrentCell->gelList);
		drawingMode = START_MODE;
		break;
	case MIDDLE | START_MODE:
		ClearGelFlags(CurrentCell->gelList);
		break;
	case REDRAW | END_MODE:
		DrawSplineSection(v, npts, tmpGcInvert, start, end);
		break;
	default:
#ifdef DEBUG
		(void) sprintf(errstring, "Hey! Unknown mode %d in spline_adj", 
		 evtype);
		message(errstring);
#endif
		break;
	}
	ASSERT(allock(), "spline_adj");
}


DrawSplineSection(v, npts, gc, start, end)
XPoint *v;
GC gc;
int start, end, npts;
{
	drawlines(picDpy, picWin, gc, flatVerts, nFlat, CoordModePrevious);
	drawlines(picDpy, picWin, gc, v, npts, CoordModeOrigin);

	/* !! Fix! */
	if (start)
		Arrow(picDpy, picWin, v[1].x, v[1].y, v[0].x, v[0].y, gc);
	if (end)
		Arrow(picDpy, picWin, v[0].x, v[0].y, v[1].x, v[1].y, gc);
}


/*
 *  Finds the closest point in the spline gel 'g' to mx, my and
 *  puts the points in 'v'. Caller must allocate space for v. 'adjusted'
 *  will point to the closest point in the gel pointlist, and start and
 *  end will be set depending on whether the point is the start or end
 *  point. 'npts' is the number of points in v, usually 3, but 2 if one
 *  of the points is an endpoint.
 */
GetClosestSplinePoint(g, mx, my, v, npts, adjusted, start, end)
Gel *g;
XPoint **v;
XPoint **adjusted;
int *start, *end, *npts;
int mx, my;
{
	register int i;
	int mindist = MAXINT;
	int dist;
	int closest;
	int n = ((PointList *) g->data)->nVerts;
	XPoint *vertices = ((PointList *) g->data)->v;

	*adjusted = vertices;
	for (i = 1,vertices++; i < n-1; i++, vertices++) {
		dist = (vertices->x - mx)*(vertices->x - mx) + 
		 (vertices->y - my)*(vertices->y - my);
		if (dist < mindist) {
			closest = i;
			*adjusted = vertices;
			mindist = dist;
		}
	}
	*npts = 3;
	if (closest <= 1) {
		*start = TRUE;
		*v = *adjusted;
		(*npts)--;
	} else {
		*start = FALSE;
		*v = *adjusted - 1;
	}
	if (closest >= n - 2) {
		*end = TRUE;
		(*npts)--;
	} else {
		*end = FALSE;
	}
	ASSERT((*npts != 1), "One point spline");
}

/*
 * Finds distance of point from a spline. This is the distance of the closest
 * segment of the controlling line from the point. The distance of a segment
 * from the point is the perpendicular distance of the point from the segment,
 * if the perpendicular intersects the segment, else it is the distance of the
 * closest endpoint. We just call line_distance()..
 */
int
spline_distance(gel, xp, yp)
Gel *gel;
int xp, yp;
{
	extern int line_distance();
	
	return(line_distance(gel, xp, yp));
}
