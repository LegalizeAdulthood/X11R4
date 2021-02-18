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

/*
 * SUIT Picture Widget Drawing Functions. 
 */
#include  <stdio.h>
#include  <math.h>
#include  <X11/Intrinsic.h>
#include  <X11/StringDefs.h>
#include  <St/StPictureP.h>

#define   ERROR     -1
#define   SUCCESS    0

#define   BEGIN_CAP  0
#define   END_CAP    1

#define  RIGHT       0
#define  LEFT      180
#define  TOP        90
#define  BOTTOM    270



StDrawArcPrimitive(display, d, gc, type, arc)
	Display        *display;
	Drawable        d;
	GC              gc;
	StFigureType    type;
	StArcPrimitive *arc;


{
	StPoint         pie[3];
	int             rc;
	int             cp_x, cp_y, r1, r2;
	int             width, height;
	int             hw_begin, hw_end;	/* Half Line Width  */
	XArc           *xarc;

	xarc = (XArc *) & arc->arc.x;

	width = arc->arc.width;
	height = arc->arc.height;


	/* Set Arc Mode  */


	switch ((int) type) {

	case Starc:
	case StellipticArc:
		XSetArcMode(display, gc, ArcChord);
		break;

	case Stpie:
	case StellipticPie:
		XSetArcMode(display, gc, ArcPieSlice);
		break;

	case Stcircle:
	case Stellipse:
		arc->arc.angle1 = (short) 0;
		arc->arc.angle2 = (short) (360 * 64);
		break;

	default:

		fprintf(stderr,

		 "\n\n #ERROR_StDrawArcPrimitive : Unknown figure type \n\n"

			);

		return (ERROR);

	}			/* switch end   ...  Figure_Type  */


	/* Draw Fill Arc  */




	if ((type != Starc) && (type != StellipticArc) &&
	    (arc->paint_atr.fill_style != StfillHollow)) {


		ChangeFillAttributes(display, gc, &arc->paint_atr);


		XFillArcs(display, d, gc,
			  xarc,
			  1
			);

	}			/* if end  ...  Fill_Style  */
	/* Set Line Attribute  */


	ChangeLineAttributes(display, gc, &arc->line_atr);


	/* Draw Cap  */


	if (arc->type == Starc || arc->type == StellipticArc) {


		/* Begin Cap_Style */

		if (arc->line_atr.begin_cap == StcapRound ||
		    arc->line_atr.begin_cap == StcapArrow) {

			if ((rc = StCap_arc(display, d, gc,
					    arc->line_atr.begin_cap,
					    arc->line_atr.line_width,
					    arc->arc.x, arc->arc.y,
					    arc->arc.angle1, arc->arc.angle2,
					    arc->arc.width, arc->arc.height,
					    BEGIN_CAP)) == ERROR)
				return (ERROR);
		}
		/* End Cap_Style */

		if (arc->line_atr.end_cap == StcapRound ||
		    arc->line_atr.end_cap == StcapArrow) {

			if ((rc = StCap_arc(display, d, gc,
					    arc->line_atr.end_cap,
					    arc->line_atr.line_width,
					    arc->arc.x, arc->arc.y,
					    arc->arc.angle1, arc->arc.angle2,
					    arc->arc.width, arc->arc.height,
					    END_CAP)) == ERROR)
				return (ERROR);
		}
	}			/* if end   Arc_type check  */
	/* Draw Pie Line  */

	if (type == Stpie || type == StellipticPie) {


		if (arc->line_atr.begin_cap != StcapRound &&
		    arc->line_atr.line_width > 1) {
			hw_begin = arc->line_atr.line_width / 2;
		} else {
			hw_begin = 0;
		}


		if (arc->line_atr.end_cap != StcapRound &&
		    arc->line_atr.line_width > 1) {
			hw_end = arc->line_atr.line_width / 2;
		} else {
			hw_end = 0;
		}

		switch (arc->arc.angle1 / 64) {

		case RIGHT:
			pie[0].x = (int) arc->arc.x + width + hw_begin;
			pie[0].y = (int) arc->arc.y + (height / 2);
			pie[2].x = (int) arc->arc.x + (width / 2);
			pie[2].y = (int) arc->arc.y - hw_end;
			break;


		case TOP:
			pie[0].x = (int) arc->arc.x + (width / 2);
			pie[0].y = (int) arc->arc.y - hw_begin;
			pie[2].x = (int) arc->arc.x - hw_end;
			pie[2].y = (int) arc->arc.y + (height / 2);
			break;


		case LEFT:
			pie[0].x = (int) arc->arc.x - hw_begin;
			pie[0].y = (int) arc->arc.y + (height / 2);
			pie[2].x = (int) arc->arc.x + (width / 2);
			pie[2].y = (int) arc->arc.y + height + hw_end;
			break;


		case BOTTOM:
			pie[0].x = (int) arc->arc.x + (width / 2);
			pie[0].y = (int) arc->arc.y + height + hw_begin;
			pie[2].x = (int) arc->arc.x + width + hw_end;
			pie[2].y = (int) arc->arc.y + (height / 2);
			break;

		default:

			fprintf(stderr,

			"\n\n #ERROR_StDrawArcPrimitive : Bad Arc_Angle \n\n"

				);
			return (ERROR);

		}		/* switch end  */


		pie[1].x = (int) arc->arc.x + (width / 2);	/* Arc Center  */
		pie[1].y = (int) arc->arc.y + (height / 2);


		XDrawLines(display, d, gc, pie, 3, CoordModeOrigin);


		/* Draw Pie Round_Cap  */

		if (arc->line_atr.begin_cap == StcapRound) {
			XFillArc(display, d, gc,
				 pie[0].x - (arc->line_atr.line_width / 2),
				 pie[0].y - (arc->line_atr.line_width / 2),
				 arc->line_atr.line_width,
				 arc->line_atr.line_width,
				 0,
				 360 * 64);
		}
		if (arc->line_atr.end_cap == StcapRound) {
			XFillArc(display, d, gc,
				 pie[2].x - (arc->line_atr.line_width / 2),
				 pie[2].y - (arc->line_atr.line_width / 2),
				 arc->line_atr.line_width,
				 arc->line_atr.line_width,
				 0,
				 360 * 64);
		}
	}			/* if end   ( Check Pie )       */
	/* Draw Arc  */


	XDrawArcs(display, d, gc,
		  xarc,
		  1
		);

	XFlush(display);	/* Display Arc  */

	return (SUCCESS);

}				/* StDrawArcPrimitive End  */


StDrawLinePrimitive(display, d, gc, line)
	Display        *display;
	Drawable        d;
	GC              gc;
	StLinePrimitive *line;

{

	int             rc;
	unsigned int    n_points;

	/* Set Fill_Attribute  &  Draw Fill_Polygon  */


	if (line->type == Stpolygon
	    && line->paint_atr.fill_style != StfillHollow) {


		ChangeFillAttributes(display, gc, &line->paint_atr);


		/* Draw Fill_Polygon  */


		XFillPolygon(display, d, gc,
			     line->point_list.points,
			     line->point_list.n_points,
			     Nonconvex,
			     CoordModeOrigin
			);


	}			/* if end   ...   Check Fill Style   */
	/* Set Line_Attribute  */


	ChangeLineAttributes(display, gc, &line->line_atr);


	/* Draw Cap  */


	n_points = line->point_list.n_points;	/* Save  n_points  */


	/* Begin Cap_Style */

	if (line->line_atr.begin_cap == StcapRound ||
	    line->line_atr.begin_cap == StcapArrow) {

		if ((rc = StCap_line(display, d, gc,
				     line->line_atr.begin_cap,
				     line->line_atr.line_width,
				     line->point_list.points[1].x,
				     line->point_list.points[1].y,
				     line->point_list.points[0].x,
				     line->point_list.points[0].y)) == ERROR)
			return (ERROR);
	}
	/* End Cap_Style */

	if (line->line_atr.end_cap == StcapRound ||
	    line->line_atr.end_cap == StcapArrow) {


		if ((rc = StCap_line(display, d, gc,
				     line->line_atr.end_cap,
				     line->line_atr.line_width,
				     line->point_list.points[n_points - 2].x,
				     line->point_list.points[n_points - 2].y,
				     line->point_list.points[n_points - 1].x,
			 line->point_list.points[n_points - 1].y)) == ERROR)
			return (ERROR);
	}
	/* Draw Lines  */


	XDrawLines(display, d, gc,
		   line->point_list.points,
		   line->point_list.n_points,
		   CoordModeOrigin
		);


	XFlush(display);	/* Display Line    */

	return (SUCCESS);


}				/* StDrawLinePrimitive End  */

StDrawRectPrimitive(display, d, gc, rectangle)
	Display        *display;
	Drawable        d;
	GC              gc;
	StRectPrimitive *rectangle;
{

	int             rc;

	/* Set Fill_Attribute  &  Draw Fill_Rectangle  */


	if (rectangle->paint_atr.fill_style != StfillHollow) {


		ChangeFillAttributes(display, gc, &(rectangle->paint_atr));

		/* Draw Fill_Rectangle  */


		XFillRectangles(display, d, gc,
				&rectangle->rectangle,
				1
			);



	}			/* if end  ...  Fill_Style  */
	/* *  Set Line_Attribute  */


	ChangeLineAttributes(display, gc, &(rectangle->line_atr));


	/* Draw Rectangle  */

	XDrawRectangles(display, d, gc,
			&rectangle->rectangle,
			1
		);

	XFlush(display);	/* Display Rect  */

	return (SUCCESS);


}				/* StDrawRectPrimitive End  */


StCap_line(display, d, gc, cap_type, line_width, x1, y1, x2, y2)
	Display        *display;
	Drawable        d;
	GC              gc;
	StCapStyle      cap_type;
	int             line_width;
	int             x1, y1, x2, y2;	/* Start Point  &  End Point  */

{
	int             rc;
	StPoint         cap[3];

	float           dx, dy;
	float           ddx, ddy;
	float           px, py;
	float           pi = 3.141592654;
	float           angl;
	double          dist;


	switch ((int) cap_type) {

	case StcapButt:
		break;

	case StcapRound:

		if (line_width >= 3) {
			XFillArc(display, d, gc,
				 x2 - (line_width / 2),
				 y2 - (line_width / 2),
				 line_width,
				 line_width,
				 0,
				 360 * 64);
		}
		break;


	case StcapArrow:

		dx = x2 - x1;
		dy = y2 - y1;

		dist = sqrt(dx * dx + dy * dy);

		ddx = (20 * dx) / dist;
		ddy = (20 * dy) / dist;

		px = x2 - ddx;
		py = y2 - ddy;
		angl = pi / 180 * 30;

		cap[0].x = (px - x2) * cos(angl) - (py - y2) * sin(angl) + x2;
		cap[0].y = (px - x2) * sin(angl) + (py - y2) * cos(angl) + y2;
		cap[1].x = x2;
		cap[1].y = y2;
		angl = pi / 180 * (-30);
		cap[2].x = (px - x2) * cos(angl) - (py - y2) * sin(angl) + x2;
		cap[2].y = (px - x2) * sin(angl) + (py - y2) * cos(angl) + y2;


		XDrawLines(display, d, gc,
			   cap,
			   3,
			   CoordModeOrigin
			);

		break;

	default:
		return (ERROR);	/* Unknoun Cap_type  */

	}			/* Switch End ( Check Cap_type ) */

	return (SUCCESS);

}				/* end StCap_line */


StCap_arc(display, d, gc,
	  cap_type, line_width, x, y, ang1, ang2, width, height, cap_flg)
	Display        *display;
	Drawable        d;
	GC              gc;
	StCapStyle      cap_type;	/* StcapButt, StcapRound, StcapArrow  */
	int             line_width;	/* Cap Line Width (1-20)              */
	int             x, y;	/* Arc Center                         */
	int             ang1, ang2;	/* Arc Angle ( start, end )           */
	int             width, height;	/* Arc Width & Height                 */
	int             cap_flg;/* Cap Flag                           */
/* ( BEGIN_CAP or END_CAP )         */

{
	int             rc;
	int             xs, ys, xe, ye;	/* Interface to StCap_line            */

	switch (ang1 / 64) {

	case RIGHT:
		if (cap_flg == BEGIN_CAP) {
			xs = x + width;
			ys = y + (height / 2);
			xe = x + width;
			ye = y;
		} else {
			xs = x + (width / 2);
			ys = y;
			xe = x + width;
			ye = y;
		}
		break;

	case TOP:
		if (cap_flg == BEGIN_CAP) {
			xs = x + (width / 2);
			ys = y;
			xe = x;
			ye = y;
		} else {
			xs = x;
			ys = y + (height / 2);
			xe = x;
			ye = y;
		}
		break;

	case LEFT:
		if (cap_flg == BEGIN_CAP) {
			xs = x;
			ys = y + (height / 2);
			xe = x;
			ye = y + height;
		} else {
			xs = x + (width / 2);
			ys = y + height;
			xe = x;
			ye = y + height;
		}
		break;

	case BOTTOM:
		if (cap_flg == BEGIN_CAP) {
			xs = x + (width / 2);
			ys = y + height;
			xe = x + width;
			ye = y + height;
		} else {
			xs = x + width;
			ys = y + (height / 2);
			xe = x + width;
			ye = y + height;
		}
		break;
	default:

		fprintf(stderr,

			"\n\n #ERROR_StCap_Arc : Bad Arc_Angle \n\n"

			);
		return (ERROR);

	}			/* switch end  */


	if ((rc = StCap_line(display, d, gc,
			     cap_type, line_width, xe, ye, xs, ys)) == ERROR)
		return (ERROR);
	else
		return (SUCCESS);

}
/* StCap_arc end */
