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

#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/cursorfont.h>
#include <St/StPictureP.h>
#ifdef OMITARC
#include <math.h>
#endif

static void     _HandleDrag();
static void     _HandleMove();
static void     _HandleCopy();
static void     _HandleResize();
static void     _HandleSelect();
static void     _HandleSelectBox();
static void     _HandleScrool();
static void     _FixupObject();
static void     _FixupMoveObject();
static void     _FixupCopyObject();
static void     _FixupResizeObject();
static void     _FixupSelectBox();
static void     _FixupSelectObject();
static void     _FixupScrool();
static void     _SetResizeDir();
static void     _SetupArcParm();
static void     _StartScrool();

StUIFunctionDescRec uifunctiondesc = {
				      _HandleDrag,
				      _HandleMove,
				      _HandleCopy,
				      _HandleResize,
				      _HandleSelect,
				      _HandleSelectBox,
				      _HandleScrool,
				      _FixupObject,
				      _FixupMoveObject,
				      _FixupCopyObject,
				      _FixupResizeObject,
				      _FixupSelectBox,
				      _FixupSelectObject,
				      _FixupScrool,
				      _SetResizeDir,
				      _SetupArcParm,
				      _StartScrool
};

static Boolean  xb_ok, yb_ok, xe_ok, ye_ok;	/* resize direction flag */
#ifdef OMITARC
static XPoint   tmp_points[9];
static StDragImage tmp_image = {
		  Stpolyline, 9, tmp_points, 0, 0, (StArcDirection) 0, 0, 0,
};
#endif

static StDragImage *
NewImage(type)
	StFigureType    type;
{
	StDragImage    *image;

	image = (StDragImage *) calloc((unsigned) 1, sizeof(StDragImage));
	image->type = type;
	image->npoints = 0;
	image->points = (XPoint *) malloc(1);
	return image;
}

static void
FreeImage(image)
	StDragImage    *image;
{
	free(image->points);
	free(image);
}

#ifdef OMITARC
#define VART8	9

static void
calc_vart(x1, y1, x2, y2, tmp_image)
	int             x1, y1, x2, y2;
	StDragImage    *tmp_image;
{
	int             a, b, cx, cy, i;
	float           p, cos_p, sin_p, cos_tmp, sin_tmp, tmp;
	double          pi = acos((double) (-1));

	cx = (x1 + x2) / 2;
	cy = (y1 + y2) / 2;
	a = (x1 <= x2) ? (x2 - x1) / 2 : (x1 - x2) / 2;
	b = (y1 <= y2) ? (y2 - y1) / 2 : (y1 - y2) / 2;
	p = 2 * pi / (VART8 - 1);
	cos_p = cos(p);
	sin_p = sin(p);
	cos_tmp = 1;
	sin_tmp = 0;
	for (i = 0; i < VART8; i++) {
		tmp_image->points[i].x = cx + a * cos_tmp;
		tmp_image->points[i].y = cy + b * sin_tmp;
		tmp = cos_tmp * cos_p - sin_tmp * sin_p;
		sin_tmp = sin_tmp * cos_p + cos_tmp * sin_p;
		cos_tmp = tmp;
	}
}

static void
DrawTempEllipse(dc, x, y, width, height)
	StDrawControl  *dc;
	short           x, y, width, height;
{
	int             x1, x2, y1, y2;

	x1 = x;
	y1 = y;
	x2 = x + width;
	y2 = y + height;
	if (x1 == x2 || y1 == y2)
		XDrawLine(dc->dpy, dc->window, dc->handle_gc, x1, y1, x2, y2);

	else {
		calc_vart(x1, y1, x2, y2, &tmp_image);
		XDrawLines(dc->dpy, dc->window, dc->handle_gc,
			   tmp_image.points, tmp_image.npoints,
			   CoordModeOrigin);
	}
}
#endif

typedef struct {
	Boolean         is_normal_angle;
	short           old_x, old_y;	/* Xlib coordinate */
}               Arc_Packet;

static Arc_Packet arc_packet;

static void
InitArcPacket()
{
	arc_packet.is_normal_angle = True;
	arc_packet.old_x = 0;
	arc_packet.old_y = 0;
}

static void
SetArcPacket(x, y)
	short           x, y;	/* Xlib coordinate */
{
	arc_packet.old_x = x;
	arc_packet.old_y = y;
}

static int
NormalAngle(xb, yb, xe, ye)
	short           xb, yb, xe, ye;
{
	return (xe < xb) ? ((ye < yb) ? 0 : 90 * 64) : ((ye < yb) ? 270 * 64 : 180 * 64);
}

static Boolean
CalcAngle(xb, yb, old_x, old_y, new_x, new_y, old_is_normal, angle)
	short           xb, yb, old_x, old_y, new_x, new_y;	/* Xlib coordinate */
	Boolean         old_is_normal;
	int            *angle;
{
	int             old_angle;

	old_angle = NormalAngle(xb, yb, old_x, old_y);
	*angle = NormalAngle(xb, yb, new_x, new_y);

	if (*angle != old_angle)
		old_is_normal = (*angle == (old_angle + 90 * 64) % (360 * 64));

	if (!old_is_normal)
		*angle = (*angle + 180 * 64) % (360 * 64);

	return old_is_normal;
}

static void
_SetupArcParm(w, image, x, y, width, height)
	Widget          w;
	StDragImage    *image;
	short          *x;	/* out Widget coordinate */
	short          *y;	/* out Widget coordinate */
	unsigned short *width;
	unsigned short *height;
{
	int             angle;

	arc_packet.is_normal_angle =
		CalcAngle(image->points[0].x, image->points[0].y,
			  arc_packet.old_x, arc_packet.old_y,
			  image->points[1].x, image->points[1].y,
			  arc_packet.is_normal_angle, &angle);

	*x = (angle == 0 || angle == 270 * 64) ?
		Min(image->points[0].x, image->points[1].x)
		: Max(image->points[0].x, image->points[1].x);
	*y = (angle == 180 * 64 || angle == 270 * 64) ?
		Min(image->points[0].y, image->points[1].y)
		: Max(image->points[0].y, image->points[1].y);

	*width = Abs((image->points[1].x - image->points[0].x));
	*height = Abs((image->points[1].y - image->points[0].y));

	*x -= *width;
	*y -= *height;
	ToWidgetXY(w, *x, *y, x, y);
	*width *= 2;
	*height *= 2;
	if (image->type == Starc || image->type == Stpie) {
		*width = Max(*width, *height);
		*height = *width;
	}
	image->direction = StantiClockwise;
	image->angle1 = angle;
	image->angle2 = 90 * 64;
}

static void
DrawTempArc(dpy, win, gc, x, y, w, h, angle1, angle2)
	Display        *dpy;
	Window          win;
	GC              gc;
	short           x, y;
	unsigned short  w, h;
	int             angle1, angle2;
{
	XPoint          points[3];

	switch (angle1) {
	case 0 * 64:
		points[0].x = x + w;
		points[0].y = y + h / 2;
		points[1].x = x + w;
		points[1].y = y;
		points[2].x = x + w / 2;
		points[2].y = y;
		break;
	case 90 * 64:
		points[0].x = x + w / 2;
		points[0].y = y;
		points[1].x = x;
		points[1].y = y;
		points[2].x = x;
		points[2].y = y + h / 2;
		break;
	case 180 * 64:
		points[0].x = x;
		points[0].y = y + h / 2;
		points[1].x = x;
		points[1].y = y + h;
		points[2].x = x + w / 2;
		points[2].y = y + h;
		break;
	case 270 * 64:
		points[0].x = x + w / 2;
		points[0].y = y + h;
		points[1].x = x + w;
		points[1].y = y + h;
		points[2].x = x + w;
		points[2].y = y + h / 2;
		break;
	}
	XDrawLines(dpy, win, gc, points, 3, CoordModeOrigin);
}

static void
DrawDragArc(dc, image)
	StDrawControl  *dc;
	StDragImage    *image;
{
	short           x, y;	/* Xlib coordinate */
	unsigned short  width, height;
	int             angle;

	arc_packet.is_normal_angle =
		CalcAngle(image->points[0].x, image->points[0].y,
			  arc_packet.old_x, arc_packet.old_y,
			  image->points[1].x, image->points[1].y,
			  arc_packet.is_normal_angle, &angle);

	x = (angle == 0 || angle == 270 * 64) ?
		Min(image->points[0].x, image->points[1].x)
		: Max(image->points[0].x, image->points[1].x);
	y = (angle == 180 * 64 || angle == 270 * 64) ?
		Min(image->points[0].y, image->points[1].y)
		: Max(image->points[0].y, image->points[1].y);

	width = Abs((image->points[1].x - image->points[0].x));
	height = Abs((image->points[1].y - image->points[0].y));

	x -= width;
	y -= height;
	width *= 2;
	height *= 2;
	if (image->type == Starc || image->type == Stpie) {
		width = Max(width, height);
		height = width;
	}
#ifdef OMITARC
	DrawTempArc(dc->dpy, dc->window, dc->handle_gc, x, y,
		    width, height, angle, 90 * 64);
#else
	XDrawArc(dc->dpy, dc->window, dc->handle_gc, x, y,
		 width, height, angle, 90 * 64);
#endif
}

static void
DrawFixedArc(dc, image)
	StDrawControl  *dc;
	StDragImage    *image;
{
	short           x, y;	/* Xlib coordinate */
	unsigned short  width, height;

	x = image->points[0].x;
	y = image->points[0].y;
	width = Abs((image->points[1].x - image->points[0].x));
	height = Abs((image->points[1].y - image->points[0].y));
	switch (image->angle1) {
	case 0 * 64:
		x -= width;
		break;
	case 90 * 64:
		break;
	case 180 * 64:
		y -= height;
		break;
	case 270 * 64:
		x -= width;
		y -= height;
		break;
	}
	width *= 2;
	height *= 2;
#ifdef OMITARC
	DrawTempArc(dc->dpy, dc->window, dc->handle_gc, x, y,
		    width, height, image->angle1, image->angle2);
#else
	XDrawArc(dc->dpy, dc->window, dc->handle_gc, x, y,
		 width, height, image->angle1, image->angle2);
#endif
}

static void
DrawImage(dc, image)
	StDrawControl  *dc;
	StDragImage    *image;
{
	short           x1, x2, y1, y2, x, y, width, height;

	if (image->npoints == 2) {
		x1 = image->points[0].x;
		x2 = image->points[1].x;
		y1 = image->points[0].y;
		y2 = image->points[1].y;
		x = Min(x1, x2);
		y = Min(y1, y2);
		width = x1 + x2 - x - x;
		height = y1 + y2 - y - y;
	}
	switch ((int) image->type) {
	case Stline:
		XDrawLine(dc->dpy, dc->window, dc->handle_gc, x1, y1, x2, y2);
		break;
	case Strectangle:
	case Sttext:
	case Stbitmap:
		XDrawRectangle(dc->dpy, dc->window, dc->handle_gc,
			       x, y, width, height);
		break;
	case Stcircle:
		width = Max(width, height);
		height = width;
	case Stellipse:
#ifndef OMITARC
		XDrawArc(dc->dpy, dc->window, dc->handle_gc,
			 x, y, width, height,
			 0, 360 * 64);
#else
		DrawTempEllipse(dc, x, y, width, height);
#endif
		break;
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		DrawDragArc(dc, image);
		break;
	case Stpolygon:
	case Stpolyline:
	case Stfreehand:
		XDrawLines(dc->dpy, dc->window, dc->handle_gc,
			   image->points, image->npoints,
			   CoordModeOrigin);
		break;
	}
}

static void
DrawMovingObj(w, orgx, orgy, dispx, dispy, grp_ptr)
	Widget          w;
	short           orgx, orgy;
	short           dispx, dispy;	/* Xlib coordinate */
	StObject       *grp_ptr;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           x1, y1, x2, y2, ox, oy;	/* Xlib coordinate */
	register        i;
	StObject       *prim_ptr = grp_ptr->body.p_group.first;
	StFigureType    ftype = prim_ptr->type;

	dc->image = NewImage(ftype);
	ToGlobalXY(w, orgx, orgy, &ox, &oy);
	switch ((int) ftype) {
	case Strectangle:
		x1 = prim_ptr->body.pd_rect.rectangle.x + ox + dispx;
		y1 = Abs(prim_ptr->body.pd_rect.rectangle.y) + oy + dispy;
		x2 = x1 + prim_ptr->body.pd_rect.rectangle.width;
		y2 = y1 + prim_ptr->body.pd_rect.rectangle.height;
		StuffPoint(dc->image, x1, y1);
		StuffPoint(dc->image, x2, y2);
		DrawImage(dc, dc->image);
		break;
	case Stcircle:
	case Stellipse:
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		x1 = prim_ptr->body.pd_arc.arc.x + ox + dispx;
		y1 = Abs(prim_ptr->body.pd_arc.arc.y) + oy + dispy;
		x2 = x1 + prim_ptr->body.pd_arc.arc.width;
		y2 = y1 + prim_ptr->body.pd_arc.arc.height;
		StuffPoint(dc->image, x1, y1);
		StuffPoint(dc->image, x2, y2);
		dc->image->direction = prim_ptr->body.pd_arc.arc.direction;
		dc->image->angle1 = prim_ptr->body.pd_arc.arc.angle1;
		dc->image->angle2 = prim_ptr->body.pd_arc.arc.angle2;
		if (ftype == Stcircle || ftype == Stellipse)
			DrawImage(dc, dc->image);
		else
			DrawFixedArc(dc, dc->image);
		break;
	case Stpolygon:
	case Stpolyline:
	case Stfreehand:
	case Stline:
		for (i = 0; i < prim_ptr->body.pd_line.point_list.n_points; i++) {
			x1 = prim_ptr->body.pd_line.point_list.points[i].x + ox + dispx;
			y1 = Abs(prim_ptr->body.pd_line.point_list.points[i].y)
				+ oy + dispy;
			StuffPoint(dc->image, x1, y1);
		}
		if (ftype == Stpolygon)
			StuffPoint(dc->image, dc->image->points[0].x,
				   dc->image->points[0].y);
		DrawImage(dc, dc->image);
		break;
	case Sttext:
		x1 = prim_ptr->body.pd_text.text.x + ox + dispx;
		y1 = Abs(prim_ptr->body.pd_text.text.y) + oy + dispy;
		x2 = x1 + grp_ptr->body.p_group.w;
		y2 = y1 + grp_ptr->body.p_group.h;
		StuffPoint(dc->image, x1, y1);
		StuffPoint(dc->image, x2, y2);
		DrawImage(dc, dc->image);
		break;
	case Stbitmap:
		x1 = prim_ptr->body.pd_bitmap.bitmap.x + ox + dispx;
		y1 = Abs(prim_ptr->body.pd_bitmap.bitmap.y) + oy + dispy;
		x2 = x1 + grp_ptr->body.p_group.w;
		y2 = y1 + grp_ptr->body.p_group.h;
		StuffPoint(dc->image, x1, y1);
		StuffPoint(dc->image, x2, y2);
		DrawImage(dc, dc->image);
		break;
	default:
		;
	}
	FreeImage(dc->image);
	dc->image = (StDragImage *) NULL;
}

static void
DrawResizeObj(w, orgx, orgy, dispx, dispy, grp_ptr)
	Widget          w;
	short           orgx, orgy;
	short           dispx, dispy;	/* Xlib coordinate */
	StObject       *grp_ptr;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           x1, y1, x2, y2, ox, oy;	/* Xlib coordinate */

	if (dc->image) {	/* at first free up IMAGE    */
		FreeImage(dc->image);	/* and at last used by Fixup */
		dc->image = (StDragImage *) NULL;	/* and free up also	     */
	}
	dc->image = NewImage(Strectangle);
	ToGlobalXY(w, orgx, orgy, &ox, &oy);
	x1 = ox;
	y1 = oy;
	x2 = x1 + grp_ptr->body.p_group.w;
	y2 = y1 + grp_ptr->body.p_group.h;
	if (xb_ok)
		x1 += dispx;
	if (yb_ok)
		y1 += dispy;
	if (xe_ok)
		x2 += dispx;
	if (ye_ok)
		y2 += dispy;
	StuffPoint(dc->image, x1, y1);
	StuffPoint(dc->image, x2, y2);
	DrawImage(dc, dc->image);

}

static void
MoveImage(w, orgx, orgy, dispx, dispy, grp_ptr, node_depth)
	Widget          w;
	short           orgx, orgy;	/* Widget coordinate */
	short           dispx, dispy;	/* Xlib coordinate   */
	int             node_depth;
	StObject       *grp_ptr;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           ox = orgx + grp_ptr->body.p_group.obj_pos.x;
	short           oy = orgy + grp_ptr->body.p_group.obj_pos.y;

	if (grp_ptr->n_obj > 1)	/* object is GROUP draw child */
		MoveImage(w, ox, oy, dispx, dispy, grp_ptr->body.p_group.first, 1);
	/* draw myself */
	if (!dc->first_move)	/* erase old image */
		DrawMovingObj(w, ox, oy, dc->old_disp_x, dc->old_disp_y, grp_ptr);
	DrawMovingObj(w, ox, oy, dispx, dispy, grp_ptr);

	if (node_depth && grp_ptr->body.p_group.succ)	/* draw brother in GROUP */
		MoveImage(w, orgx, orgy, dispx, dispy, grp_ptr->body.p_group.succ, 1);
}

static void
ResizeImage(w, orgx, orgy, dispx, dispy, grp_ptr, node_depth)
	Widget          w;
	short           orgx, orgy;	/* Widget coordinate */
	short           dispx, dispy;	/* Xlib coordinate   */
	int             node_depth;
	StObject       *grp_ptr;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           ox = orgx + grp_ptr->body.p_group.obj_pos.x;
	short           oy = orgy + grp_ptr->body.p_group.obj_pos.y;

	if (grp_ptr->n_obj > 1)	/* object is GROUP draw child */
		ResizeImage(w, ox, oy, dispx, dispy, grp_ptr->body.p_group.first, 1);
	/* draw myself */
	if (!dc->first_move)	/* erase old image */
		DrawResizeObj(w, ox, oy, dc->old_disp_x, dc->old_disp_y, grp_ptr);
	DrawResizeObj(w, ox, oy, dispx, dispy, grp_ptr);

	if (node_depth && grp_ptr->body.p_group.succ)	/* draw brother in GROUP */
		ResizeImage(w, orgx, orgy, dispx, dispy, grp_ptr->body.p_group.succ, 1);
}

static void
_SetResizeDir(select)
	int             select;
{
	xb_ok = yb_ok = xe_ok = ye_ok = False;

	switch (select) {
	case SLUPPER_LEFT:
		xb_ok = yb_ok = True;
		xe_ok = ye_ok = False;
		break;
	case SLUPPER_MIDDLE:
		yb_ok = True;
		xb_ok = xe_ok = ye_ok = False;
		break;
	case SLUPPER_RIGHT:
		xe_ok = yb_ok = True;
		xb_ok = ye_ok = False;
		break;
	case SLLEFT:
		xb_ok = True;
		yb_ok = xe_ok = ye_ok = False;
		break;
	case SLRIGHT:
		xe_ok = True;
		xb_ok = yb_ok = ye_ok = False;
		break;
	case SLLOWER_LEFT:
		xb_ok = ye_ok = True;
		yb_ok = xe_ok = False;
		break;
	case SLLOWER_MIDDLE:
		ye_ok = True;
		xb_ok = yb_ok = xe_ok = False;
		break;
	case SLLOWER_RIGHT:
		xe_ok = ye_ok = True;
		xb_ok = yb_ok = False;
		break;
	}

}

static void
_HandleMove(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *next;
	register        i;
	short           disp_x = event->xbutton.x - dc->fixx;	/* Xlib coordinate   */
	short           disp_y = event->xbutton.y - dc->fixy;	/* Xlib coordinate   */

	for (i = 0; i < dc->fig_buf->n_figures; i++) {
		next = GetObjAddr(w, dc->fig_buf->figures[i]);
		MoveImage(w, 0, 0, disp_x, disp_y, next, 0);
	}

	if (dc->first_move)
		dc->first_move = False;
	dc->old_disp_x = disp_x;
	dc->old_disp_y = disp_y;
}

static void
_HandleCopy(w, event)
	Widget          w;
	XEvent         *event;
{
	/* This function is same as _HandleMove then not used now */
}

static void
_HandleResize(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *next;
	short           disp_x = event->xbutton.x - dc->fixx;	/* Xlib coordinate   */
	short           disp_y = event->xbutton.y - dc->fixy;	/* Xlib coordinate   */

	next = GetObjAddr(w, dc->fig_buf->figures[0]);
	ResizeImage(w, 0, 0, disp_x, disp_y, next, 0);

	if (dc->first_move)
		dc->first_move = False;
	dc->old_disp_x = disp_x;
	dc->old_disp_y = disp_y;
}

static void
_HandleScrool(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           disp_x = dc->fixx - event->xbutton.x;	/* Xlib coordinate   */
	short           disp_y = dc->fixy - event->xbutton.y;	/* Xlib coordinate   */

	MoveView(w, (Widget) NULL, disp_x, disp_y);
}

static void
_FixupSelectObject(w, select_type)
	Widget          w;
	int             select_type;
{
	StDrawControl  *dc = GetDrawControl(w);
	StPoint         pos;
	StFigureList   *figure_list;

	if (select_type == Single)
		SelectAll(w, StunsetSelect);
	pos.x = dc->fixx;
	pos.y = dc->fixy;
	StPicGetFigure(w, &pos, &figure_list, StCoordEvent);
	if (figure_list)
		StPicSelectFigure(w, figure_list->figures[0],	/* with drawing */
				  StsetSelect, StsetHandle);
}

static void
_HandleSelect(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StPoint         pos;
	StFigureList   *figure_list;
	StFigure        cur_figure;

	pos.x = dc->fixx;
	pos.y = dc->fixy;
	StPicGetFigure(w, &pos, &figure_list, StCoordEvent);
	if (figure_list) {
		cur_figure = figure_list->figures[0];
		if (!(IsSelectedFigure(w, cur_figure))) {
			SelectAll(w, StunsetSelect);
			StPicSelectFigure(w, cur_figure,	/* with drawing */
					  StsetSelect, StsetHandle);
		}
	}
}

static void
_FixupObject(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           x, y, x1, x2, y1, y2;

	DrawImage(dc, dc->image);	/* erase labber figure */
	switch ((int) (dc->image->type)) {
	case Strectangle:
	case Stline:
	case Stcircle:
	case Stellipse:
		dc->image->points[1].x = dc->fixx;
		dc->image->points[1].y = dc->fixy;
		break;
	case Stpolyline:
	case Stpolygon:
		dc->image->points[dc->image->npoints - 1].x = dc->fixx;
		dc->image->points[dc->image->npoints - 1].y = dc->fixy;
		break;
	case Stfreehand:
		StuffPoint(dc->image, dc->fixx, dc->fixy);
		break;
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		SetArcPacket(dc->image->points[1].x, dc->image->points[1].y);
		dc->image->points[1].x = dc->fixx;
		dc->image->points[1].y = dc->fixy;
		break;
	default:
		goto FREE;
	}
	CreateObject(w, dc->image);
FREE:
	FreeImage(dc->image);
	dc->image = NULL;
}

static void
_FixupMoveObject(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	register        i;
	StObject       *next;
	short           disp_x = event->xbutton.x - dc->fixx;
	short           disp_y = event->xbutton.y - dc->fixy;

	dc->first_move = True;
	for (i = 0; i < dc->fig_buf->n_figures; i++) {	/* erase old image */
		next = GetObjAddr(w, dc->fig_buf->figures[i]);
		MoveImage(w, 0, 0, dc->old_disp_x, dc->old_disp_y, next, 0);
	}
	MoveObject(w, disp_x, disp_y);
	dc->old_disp_x = 0;
	dc->old_disp_y = 0;
	FreeTergetFigures(w);
}

static void
_FixupCopyObject(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	register        i;
	StObject       *next;
	short           disp_x = event->xbutton.x - dc->fixx;	/* Xlib coordinate */
	short           disp_y = event->xbutton.y - dc->fixy;	/* Xlib coordinate */

	dc->first_move = True;
	for (i = 0; i < dc->fig_buf->n_figures; i++) {	/* erase old image */
		next = GetObjAddr(w, dc->fig_buf->figures[i]);
		MoveImage(w, 0, 0, dc->old_disp_x, dc->old_disp_y, next, 0);
	}
	CopyObject(w, disp_x, disp_y);
	dc->old_disp_x = 0;
	dc->old_disp_y = 0;
	FreeTergetFigures(w);
}

static void
_FixupResizeObject(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *next;

	dc->first_move = True;
	next = GetObjAddr(w, dc->fig_buf->figures[0]);	/* erase old image */
	ResizeImage(w, 0, 0, dc->old_disp_x, dc->old_disp_y, next, 0);

	ResizeObject(w);
	dc->old_disp_x = 0;
	dc->old_disp_y = 0;
	if (dc->image) {	/* free up IMAGE here   */
		FreeImage(dc->image);
		dc->image = (StDragImage *) NULL;
	}
	FreeTergetFigures(w);
}

static void
_HandleSelectBox(w, event)
	Widget          w;
	XEvent         *event;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);
	short           newx = event->xbutton.x;
	short           newy = event->xbutton.y;

	if (!dc->image) {
		dc->image = NewImage(Strectangle);
		StuffPoint(dc->image, dc->fixx, dc->fixy);
		StuffPoint(dc->image, newx, newy);
	} else {
		DrawImage(dc, dc->image);	/* erase old image */
		dc->image->points[1].x = newx;
		dc->image->points[1].y = newy;
	}
	DrawImage(dc, dc->image);
}

static void
_FixupSelectBox(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           ix1, iy1, ix2, iy2, ox1, ox2, oy1, oy2;
	StObject       *grp_ptr;

	DrawImage(dc, dc->image);	/* erase labber figure */
	SelectAll(w, StunsetSelect);
	ix1 = Min(dc->image->points[0].x, dc->image->points[1].x);
	iy1 = Min(dc->image->points[0].y, dc->image->points[1].y);
	ix2 = Max(dc->image->points[0].x, dc->image->points[1].x);
	iy2 = Max(dc->image->points[0].y, dc->image->points[1].y);
	ToWidgetXY(w, ix1, iy1, &ix1, &iy1);
	ToWidgetXY(w, ix2, iy2, &ix2, &iy2);

	grp_ptr = dc->first_grp;
	while (grp_ptr) {
		ox1 = grp_ptr->body.p_group.obj_pos.x;
		oy1 = grp_ptr->body.p_group.obj_pos.y;
		ox2 = ox1 + grp_ptr->body.p_group.w;
		oy2 = oy1 - grp_ptr->body.p_group.h;
		if (IsIncludeBox(ix1, iy1, ix2, iy2, ox1, oy1, ox2, oy2))
			StPicSelectFigure(w, grp_ptr->obj_no,
					  StsetSelect, StsetHandle);
		grp_ptr = grp_ptr->body.p_group.succ;
	}
	if (dc->image) {	/* free up IMAGE here   */
		FreeImage(dc->image);
		dc->image = (StDragImage *) NULL;
	}
}

static void
_StartScrool(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);

	if (dc->n_selected > 0) {
		SetupTergetFigures(w, True);
		SelectAll(w, StunsetSelect);
	}
	XDefineCursor(dc->dpy, dc->window,
		      XCreateFontCursor(dc->dpy, XC_fleur));
}

static void
_FixupScrool(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);
	register        i;

	if (dc->fig_buf) {
		for (i = 0; i < dc->fig_buf->n_figures; i++)
			StPicSelectFigure(w, dc->fig_buf->figures[i],
					  StsetSelect, StsetHandle);

		FreeTergetFigures(w);
	}
	XUndefineCursor(dc->dpy, dc->window);
}

static void
_HandleDrag(w, event)
	Widget          w;
	XEvent         *event;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);
	short           newx = event->xbutton.x;
	short           newy = event->xbutton.y;

	switch ((int) (spw->stPicture.mode_draw)) {
	case StdrawRectangle:
	case StdrawLine:
	case StdrawCircle:
	case StdrawEllipse:
	case StdrawArc:
	case StdrawPie:
	case StdrawEllipticArc:
	case StdrawEllipticPie:
		if (!dc->image) {
			dc->image = NewImage(GetType(spw->stPicture.mode_draw));
			StuffPoint(dc->image, dc->fixx, dc->fixy);
			StuffPoint(dc->image, newx, newy);
			InitArcPacket();
		} else {
			DrawImage(dc, dc->image);	/* erase old image */
			SetArcPacket(dc->image->points[1].x, dc->image->points[1].y);
			dc->image->points[1].x = newx;
			dc->image->points[1].y = newy;
		}
		DrawImage(dc, dc->image);
		break;
	case StdrawPolyline:
	case StdrawPolygon:
		if (dc->image == NULL) {
			dc->image = NewImage(GetType(spw->stPicture.mode_draw));
			StuffPoint(dc->image, dc->fixx, dc->fixy);
			StuffPoint(dc->image, newx, newy);
		} else {
			short           oldx = dc->image->points[dc->image->npoints - 1].x;
			short           oldy = dc->image->points[dc->image->npoints - 1].y;

			if (dc->fixx != oldx || dc->fixy != oldy)
				XDrawLine(dc->dpy, dc->window, dc->handle_gc,
					  dc->fixx, dc->fixy, oldx, oldy);
			dc->image->points[dc->image->npoints - 1].x = newx;
			dc->image->points[dc->image->npoints - 1].y = newy;
		}
		XDrawLine(dc->dpy, dc->window, dc->handle_gc,
			  dc->fixx, dc->fixy, newx, newy);
		break;
	case StdrawFreehand:
		if (dc->image == NULL) {
			dc->image = NewImage(GetType(spw->stPicture.mode_draw));
			StuffPoint(dc->image, dc->fixx, dc->fixy);
		}
		StuffPoint(dc->image, newx, newy);
		/*
		 * build line piece by piece, don't rerender the whole
		 * polyline 
		 */
		XDrawLine(dc->dpy, dc->window, dc->handle_gc,
			  dc->fixx, dc->fixy, newx, newy);
		dc->fixx = newx;
		dc->fixy = newy;
		break;
	}
}
