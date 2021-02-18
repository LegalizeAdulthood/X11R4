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
 * Picture Widget Miscelanious Functions. 
 */

#include  <stdio.h>
#include  <X11/Xlib.h>
#include  <X11/Intrinsic.h>
#include  <X11/StringDefs.h>
#include  <St/StPictureP.h>

static Boolean  _IsWidget();
static Boolean  _IsFigureType();
static int      _SetErrorStatus();
static int      _GetErrorStatus();
static int      _SetObjAtr();
static int      _GetObjAtr();
static void     _DrawObject();
static Boolean  _IsRegion();
static void     _RedrawRegion();
static void     _ConvertMirrorObject();
static Boolean  _GetFiguresRegion();
static void     _BuildFigureGroup();
static Boolean  _DestoryFigureGroup();
static void     _ConvertRotateObject();
static void     _RotatePoint();

StMiscFunctionDescRec miscfunctiondesc = {
					  _IsWidget,
					  _IsFigureType,
					  _SetErrorStatus,
					  _GetErrorStatus,
					  _SetObjAtr,
					  _GetObjAtr,
					  _DrawObject,
					  _IsRegion,
					  _RedrawRegion,
					  _ConvertMirrorObject,
					  _GetFiguresRegion,
					  _BuildFigureGroup,
					  _DestoryFigureGroup,
					  _ConvertRotateObject,
					  _RotatePoint,
};

static void     _DrawPrimitive();
static int      _SetupPointsBuff();

static int      StErrorNo = Success;

#define Dgree0		0
#define Dgree90		5760	/* 90 * 64 */
#define Dgree180	11520	/* 180 * 64 */
#define Dgree270	17280	/* 270 * 64 */

/*
 * Set Error Status Code. 
 */
static int
_SetErrorStatus(errno)
	int             errno;	/* error code */
{
	register char  *mesg;

	switch (errno) {
	case StBadWidget:
		mesg = "parameter not a widget.";
		break;
	case StBadFigure:
		mesg = "parameter not a figure.";
		break;
	case StBadValue:
		mesg = "parameter out of range.";
		break;
	case StBadAlloc:
		mesg = "insufficient resources.";
		break;
	case StBadIO:
		mesg = "file I/O error.";
		break;
	default:
		mesg = "internal error detected!";
		break;
	}
	StErrorNo = errno;
	fprintf(stderr, "Picture Widget Error: %s\n", mesg);

	return (Failure);
}

/*
 * Get Error Status Code. 
 */
static int
_GetErrorStatus()
{
	return StErrorNo;
}

static Boolean
_IsWidget(w)
	Widget          w;
{
	return (w == (Widget) NULL ? False : True);
}

static Boolean
_IsFigureType(type)
	StFigureType    type;
{
	switch ((int) type) {
	case Strectangle:
	case Stcircle:
	case Stellipse:
	case Stline:
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
	case Stpolygon:
	case Stpolyline:
	case Stfreehand:
	case Sttext:
	case Stgroup:
		return True;
	default:
		return False;
	}
}

/*
 * set attribute to object. 
 */
static int
_SetAttribute(dc, type, obj, atr_type, atr_val)
	StDrawControl  *dc;
	StFigureType    type;
	StObject       *obj;
	int             atr_type;
	char           *atr_val;
{
	StObject       *parent;
	register        stat;

	stat = Success;

	switch ((int) type) {
	case Strectangle:
		if (atr_type == LINE_ATR)
			obj->body.pd_rect.line_atr
				= *((StLineAtr *) atr_val);
		else if (atr_type == PAINT_ATR)
			obj->body.pd_rect.paint_atr
				= *((StPaintAtr *) atr_val);
		else
			stat = Failure;
		break;
	case Stcircle:
	case Stellipse:
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		if (atr_type == LINE_ATR)
			obj->body.pd_arc.line_atr
				= *((StLineAtr *) atr_val);
		else if (atr_type == PAINT_ATR)
			obj->body.pd_arc.paint_atr
				= *((StPaintAtr *) atr_val);
		else
			stat = Failure;
		break;
	case Stline:
	case Stpolygon:
	case Stpolyline:
	case Stfreehand:
		if (atr_type == LINE_ATR)
			obj->body.pd_line.line_atr
				= *((StLineAtr *) atr_val);
		else if (atr_type == PAINT_ATR)
			obj->body.pd_line.paint_atr
				= *((StPaintAtr *) atr_val);
		else
			stat = Failure;
		break;
	case Sttext:
		if (atr_type == TEXT_ATR) {
			obj->body.pd_text.text_atr
				= *((StTextAtr *) atr_val);
			parent = obj->parent;
			GetTextRegion(dc,
				      parent->body.p_group.obj_pos.x,
				      parent->body.p_group.obj_pos.y,
				      &obj->body.pd_text.text_atr,
				      obj->body.pd_text.text.string,
				      obj->body.pd_text.text.n_bytes,
				      &parent->body.p_group.w,
				      &parent->body.p_group.h);
		} else
			stat = Failure;
		break;
	case Stbitmap:
		if (atr_type == PAINT_ATR)
			obj->body.pd_bitmap.paint_atr
				= *((StPaintAtr *) atr_val);
		else
			stat = Failure;
		break;
	default:
		stat = Failure;
	}

	return stat;
}

/*
 * set attribute to object. 
 */
static int
_SetObjAtr(dc, top_node, atr_type, atr_val)
	StDrawControl  *dc;
	StObject       *top_node;
	int             atr_type;
	char           *atr_val;
{
	StObject       *node;
	int             n_obj;
	register        stat;

	node = top_node->body.p_group.first;
	for (n_obj = top_node->n_obj; n_obj; --n_obj) {
		if (node->n_obj == 0) {
			stat = _SetAttribute(dc, node->type, node,
					     atr_type, atr_val);
		} else {
			stat = _SetObjAtr(dc, node, atr_type, atr_val);
		}
		node = node->body.p_group.succ;
	}
	return stat;
}

/*
 * get attribute from object. 
 */
static int
_GetAtrribute(type, obj_body, atr_type, atr_val)
	StFigureType    type;
	StObjBody      *obj_body;
	int             atr_type;
	char           *atr_val;
{
	register        stat;

	stat = Success;
	switch ((int) type) {
	case Strectangle:
		if (atr_type == LINE_ATR)
			*((StLineAtr *) atr_val)
				= obj_body->pd_rect.line_atr;
		else if (atr_type == PAINT_ATR)
			*((StPaintAtr *) atr_val)
				= obj_body->pd_rect.paint_atr;
		else
			stat = Failure;
		break;
	case Stcircle:
	case Stellipse:
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		if (atr_type == LINE_ATR)
			*((StLineAtr *) atr_val)
				= obj_body->pd_arc.line_atr;
		else if (atr_type == PAINT_ATR)
			*((StPaintAtr *) atr_val)
				= obj_body->pd_arc.paint_atr;
		else
			stat = Failure;
		break;
	case Stline:
	case Stpolygon:
	case Stpolyline:
	case Stfreehand:
		if (atr_type == LINE_ATR)
			*((StLineAtr *) atr_val)
				= obj_body->pd_line.line_atr;
		else if (atr_type == PAINT_ATR)
			*((StPaintAtr *) atr_val)
				= obj_body->pd_line.paint_atr;
		else
			stat = Failure;
		break;
	case Sttext:
		if (atr_type == TEXT_ATR)
			*((StTextAtr *) atr_val)
				= obj_body->pd_text.text_atr;
		else
			stat = Failure;
		break;
	case Stbitmap:
		if (atr_type == PAINT_ATR)
			*((StPaintAtr *) atr_val)
				= obj_body->pd_bitmap.paint_atr;
		else
			stat = Failure;
	default:
		stat = Failure;
	}
	return stat;
}

/*
 * get attribute to object. 
 */
static int
_GetObjAtr(top_node, atr_type, atr_val)
	StObject       *top_node;
	int             atr_type;
	char           *atr_val;
{
	StObject       *node;
	int             n_obj;
	register        stat;

	node = top_node->body.p_group.first;
	for (n_obj = top_node->n_obj; n_obj; --n_obj) {
		if (node->n_obj == 0) {
			stat = _GetAtrribute(node->type, &(node->body),
					     atr_type, atr_val);
		} else {
			stat = _GetObjAtr(node, atr_type, atr_val);
		}
		node = node->body.p_group.succ;
	}
	return stat;
}

/*
 * draw object 
 */
static void
_DrawObject(w, top_obj, top_x, top_y)
	Widget          w;	/* widget	(i) */
	StObject       *top_obj;/* object	(i) */
	short           top_x, top_y;	/* object position */
{
	StObject       *obj;
	int             n_obj;

	obj = top_obj->body.p_group.first;
	for (n_obj = top_obj->n_obj; n_obj; --n_obj) {
		if (obj->n_obj == 0) {
			_DrawPrimitive(w, obj->type, top_x, top_y,
				       (char *) &obj->body);
			return;
		} else
			DrawObject(w, obj,
				   top_x + obj->body.p_group.obj_pos.x,
				   top_y + obj->body.p_group.obj_pos.y);

		obj = obj->body.p_group.succ;
	}
	return;
}

/* point list for drawing line primitive */
static StPointList points_buf = {NULL, 0};

static int
_SetupPointsBuff(npoints)
{
	if (points_buf.points == (StPoint *) NULL)
		points_buf.points = (StPoint *) calloc(npoints, sizeof(StPoint));
	else if (npoints > points_buf.n_points)
		points_buf.points = (StPoint *) realloc(points_buf.points,
						 npoints * sizeof(StPoint));
	if (points_buf.points == NULL)
		return Failure;
	points_buf.n_points = npoints;
	return Success;
}

static void
AdjustArcPosition(arc)
	StArc          *arc;
{
	unsigned short  width = arc->width, height = arc->height;

	arc->width *= 2;
	arc->height *= 2;
	switch (arc->angle1) {
	case Dgree0:		/* 0 */
		arc->x -= width;
		return;
	case Dgree90:		/* 90 * 64 */
		return;
	case Dgree180:		/* 180 * 64 */
		arc->y -= height;
		return;
	case Dgree270:		/* 270 * 64 */
		arc->x -= width;
		arc->y -= height;
		return;
	}
}

/*
 * draw primitives 
 */
static void
_DrawPrimitive(w, type, origin_x, origin_y, primitive)
	Widget          w;
	StFigureType    type;
	short           origin_x, origin_y;
	char           *primitive;
{
	StDrawControl  *dc;
	StObjBody       local_prm;
	StPoint        *srcp, *destp, local_pos;
	unsigned int    p_count;

	/* get DrawControl */
	dc = GetDrawControl(w);

	memcpy(&local_prm, primitive, sizeof(StObjBody));
	switch ((int) type) {
	case Strectangle:
		ToGlobalXY(w,
			   local_prm.pd_rect.rectangle.x + origin_x,
			   local_prm.pd_rect.rectangle.y + origin_y,
			   &(local_prm.pd_rect.rectangle.x),
			   &(local_prm.pd_rect.rectangle.y));
		StDrawRectPrimitive(dc->dpy, (Drawable) dc->window,
				    dc->figure_gc, &local_prm.pd_rect);
		return;
	case Stcircle:
	case Stellipse:
		ToGlobalXY(w,
			   local_prm.pd_arc.arc.x + origin_x,
			   local_prm.pd_arc.arc.y + origin_y,
			   &(local_prm.pd_arc.arc.x),
			   &(local_prm.pd_arc.arc.y));
		StDrawArcPrimitive(dc->dpy, (Drawable) dc->window,
				   dc->figure_gc, type, &(local_prm.pd_arc));
		return;
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		ToGlobalXY(w,
			   local_prm.pd_arc.arc.x + origin_x,
			   local_prm.pd_arc.arc.y + origin_y,
			   &(local_prm.pd_arc.arc.x),
			   &(local_prm.pd_arc.arc.y));
		AdjustArcPosition(&local_prm.pd_arc.arc);
		StDrawArcPrimitive(dc->dpy, (Drawable) dc->window,
				   dc->figure_gc, type, &(local_prm.pd_arc));
		return;
	case Stline:
	case Stpolyline:
	case Stfreehand:
		p_count = local_prm.pd_line.point_list.n_points;
		if (_SetupPointsBuff(p_count)) {
			SetErrorStatus(StBadAlloc);
			return;
		}
		srcp = local_prm.pd_line.point_list.points;
		destp = points_buf.points;
		while (p_count--) {
			ToGlobalXY(w,
				   srcp->x + origin_x,
				   srcp->y + origin_y,
				   &(destp->x), &(destp->y));
			++srcp;
			++destp;
		}
		local_prm.pd_line.point_list.points
			= points_buf.points;
		if (type == Stfreehand)
			local_prm.pd_line.point_list.n_points -= 1;
		StDrawLinePrimitive(dc->dpy, (Drawable) dc->window,
			     dc->figure_gc, (StArcPrimitive *) & local_prm);
		return;
	case Stpolygon:
		p_count = local_prm.pd_line.point_list.n_points;
		if (_SetupPointsBuff(p_count + 1)) {
			SetErrorStatus(StBadAlloc);
			return;
		}
		srcp = local_prm.pd_line.point_list.points;
		destp = points_buf.points;
		ToGlobalXY(w, srcp->x + origin_x, srcp->y + origin_y,
			   &((destp + p_count)->x), &((destp + p_count)->y));
		while (p_count--) {
			ToGlobalXY(w,
				   srcp->x + origin_x,
				   srcp->y + origin_y,
				   &(destp->x), &(destp->y));
			++srcp;
			++destp;
		}
		++local_prm.pd_line.point_list.n_points;
		local_prm.pd_line.point_list.points
			= points_buf.points;
		StDrawLinePrimitive(dc->dpy, (Drawable) dc->window,
			     dc->figure_gc, (StArcPrimitive *) & local_prm);
		return;
	case Sttext:
		ToGlobalXY(w,
			   local_prm.pd_text.text.x + origin_x,
			   local_prm.pd_text.text.y + origin_y,
			   &local_pos.x, &local_pos.y);
		ChangeTextAttributes(dc->dpy, dc->figure_gc,
				     &local_prm.pd_text.text_atr);
		DrawTextObject(dc->dpy, (Drawable) dc->window,
			       dc->figure_gc, local_pos.x, local_pos.y,
			       &local_prm.pd_text.text_atr,
			       local_prm.pd_text.text.string,
			       local_prm.pd_text.text.n_bytes, local_pos.x);
		return;
	case Stbitmap:
		ToGlobalXY(w,
			   local_prm.pd_bitmap.bitmap.x + origin_x,
			   local_prm.pd_bitmap.bitmap.y + origin_y,
			   &(local_prm.pd_bitmap.bitmap.x),
			   &(local_prm.pd_bitmap.bitmap.y));
		StDrawBitmapPrimitive(w, dc->dpy, (Drawable) dc->window,
				      dc->figure_gc, &local_prm.pd_bitmap);
		return;
	case Stgroup:
	default:
		return;
	}
}

static          Boolean
_IsRegion(w, region)
	Widget          w;
	StRegion       *region;	/* must be widget coordinate */
{
	StPictureWidget spw = (StPictureWidget) w;

	return (((long) (region->x + region->width) <= (long) (spw->stPicture.coor_xmax)
		 && (long) (region->y - region->height) >= (long) (spw->stPicture.coor_ymin))
		? True : False);
}

/*
 * this function check two coordinate of region diagonal (top and bottom),
 * and if these region is met, 
 *
 */
static          Boolean
_MeetRegion(x1, y1, w1, h1, x2, y2, w2, h2, reg_return)
	short           x1, y1, x2, y2;
	unsigned short  w1, h1, w2, h2;
	StRegion       *reg_return;
{
	long            bx, by, width, height;

	/* get the coordinates of left-top and right-bottom coner of Region. */
	reg_return->x = Max(x1, x2);
	reg_return->y = Min(y1, y2);

	bx = (long) Min((long) (x1 + w1), (long) (x2 + w2));
	by = (long) Max((long) (y1 - h1), (long) (y2 - h2));

	if ((width = bx - reg_return->x) <= 0
	    || (height = reg_return->y - by) <= 0)
		return False;

	reg_return->width = (unsigned short) width;
	reg_return->height = (unsigned short) height;

	return True;
}

/* This function redraws a region of the display.                  */
/* The function must be called without visible frames.             */
static void
_RedrawRegion(w, reg)
	Widget          w;
	StRegion       *reg;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc;
	StObject       *obj;
	StRegion        clip_reg, fig_reg;
	XRectangle      xclip;

	dc = GetDrawControl(w);

	/* check up the region is in view area. */
	if (_MeetRegion(reg->x, reg->y, reg->width, reg->height,
			spw->stPicture.view_x, spw->stPicture.view_y,
			spw->stPicture.view_w, spw->stPicture.view_h,
			&fig_reg) == False)
		return;

	/* clipping the region, and redraw background. */
	clip_reg = fig_reg;
	/* convert to Xlib Coordinate. */
	ToGlobalXY(w, clip_reg.x, clip_reg.y, &(xclip.x), &(xclip.y));
	xclip.width = clip_reg.width;
	xclip.height = clip_reg.height;

	XSetClipRectangles(dc->dpy, dc->figure_gc, 0, 0, &xclip, 1, Unsorted);

	XClearArea(dc->dpy, dc->window, (int) xclip.x, (int) xclip.y,
	    (unsigned int) xclip.width, (unsigned int) xclip.height, False);

	/* traversal figure objects. */
	obj = dc->first_grp;
	while (obj != NULL) {
		if (_MeetRegion(clip_reg.x, clip_reg.y,
				clip_reg.width, clip_reg.height,
				obj->body.p_group.obj_pos.x,
				obj->body.p_group.obj_pos.y,
		      	(obj->body.p_group.w ? obj->body.p_group.w : 1),
		      	(obj->body.p_group.h ? obj->body.p_group.h : 1),
				&fig_reg)) {
			DrawObject(w, obj,
				   obj->body.p_group.obj_pos.x,
				   obj->body.p_group.obj_pos.y);
		}
		obj = obj->body.p_group.succ;
	}
	ToGlobalXY(w, spw->stPicture.view_x, spw->stPicture.view_y,
		   &(xclip.x), &(xclip.y));
	xclip.width = spw->stPicture.view_w;
	xclip.height = spw->stPicture.view_h;

	XSetClipRectangles(dc->dpy, dc->figure_gc, 0, 0, &xclip, 1, Unsorted);

}

static Boolean
_GetFiguresRegion(w, f_list, grp_x, grp_y, grp_w, grp_h, first, last)
	Widget          w;
	StFigureList   *f_list;
	short          *grp_x, *grp_y;
	unsigned short *grp_w, *grp_h;
	StObject      **first, **last;
{
	StDrawControl  *dc = GetDrawControl(w);
	StFigure       *figure = f_list->figures;
	unsigned int    n_fig = f_list->n_figures;
	StObject       *obj;
	short           top_x, top_y, bottom_x, bottom_y;
	Boolean         s_flag = False;

	if (dc->n_group <= 1)
		return True;
	*first = *last = (StObject *) NULL;
	while (n_fig--) {
		if ((obj = GetObjAddr(w, *figure)) == NULL)
			return False;
		if (obj->parent != NULL)
			return False;
		if (*first == NULL)
			*first = obj;
		if (s_flag == True) {
			JoinBox(&top_x, &top_y, &bottom_x, &bottom_y,
				obj->body.p_group.obj_pos.x,
				obj->body.p_group.obj_pos.y,
			  obj->body.p_group.obj_pos.x + obj->body.p_group.w,
			 obj->body.p_group.obj_pos.y - obj->body.p_group.h);
		} else {
			s_flag = True;
			top_x = obj->body.p_group.obj_pos.x;
			top_y = obj->body.p_group.obj_pos.y;
			bottom_x = obj->body.p_group.obj_pos.x
				+ obj->body.p_group.w;
			bottom_y = obj->body.p_group.obj_pos.y
				- obj->body.p_group.h;
		}
		++figure;
	}
	*last = obj;
	*grp_x = top_x;
	*grp_y = top_y;
	*grp_w = bottom_x - top_x;
	*grp_h = top_y - bottom_y;

	return True;
}

static void
_BuildFigureGroup(w, f_list, parent)
	Widget          w;
	StFigureList   *f_list;
	StObject       *parent;	/* group node */
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj, *pred, *succ;
	StFigure       *figure = f_list->figures;
	unsigned int    n_fig = f_list->n_figures;

	LinkObject(&dc->first_grp, &dc->last_grp, (StObject *) NULL, parent);
	parent->n_obj = n_fig;

	pred = (StObject *) NULL;
	while (n_fig--) {
		obj = GetObjAddr(w, *figure);
		/* delete object number and delete entry from Hash Table */
		BreakObject(&dc->first_grp, &dc->last_grp,
			    obj->body.p_group.pred,
			    obj->body.p_group.succ);
		DeleteObjAddr(w, obj->obj_no);
		DelObjNumber(w);
		obj->obj_no = NULL;

		/* link node in group object */
		obj->parent = parent;
		if (pred == NULL) {
			pred = obj;
			obj->body.p_group.pred = (StObject *) NULL;
		} else {
			obj->body.p_group.pred = pred;
			pred->body.p_group.succ = obj;
		}
		obj->body.p_group.obj_pos.x
			-= parent->body.p_group.obj_pos.x;
		obj->body.p_group.obj_pos.y
			-= parent->body.p_group.obj_pos.y;

		pred = obj;
		++figure;
	}
	obj->body.p_group.succ = (StObject *) NULL;
}

static Boolean
_DestoryFigureGroup(w, grp_node, figure_list_return)
	Widget          w;
	StObject       *grp_node;
	StFigureList  **figure_list_return;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj, *succ;

	*figure_list_return = (StFigureList *) NULL;

	obj = grp_node->body.p_group.first;

	BreakObject(&dc->first_grp, &dc->last_grp,
		  grp_node->body.p_group.pred, grp_node->body.p_group.succ);
	DeleteObjAddr(w, grp_node->obj_no);
	DelObjNumber(w);

	while (obj) {
		succ = obj->body.p_group.succ;

		(void) NewObjNumber(w, &obj->obj_no);
		SetObjAddr(w, obj->obj_no, obj);

		obj->body.p_group.obj_pos.x
			+= grp_node->body.p_group.obj_pos.x;
		obj->body.p_group.obj_pos.y
			+= grp_node->body.p_group.obj_pos.y;

		obj->parent = (StObject *) NULL;

		LinkObject(&dc->first_grp, &dc->last_grp,
			   (StObject *) NULL, obj);

		if (!*figure_list_return) {
			if ((*figure_list_return = (StFigureList *) NewInqBuf(w)) == (StFigureList *) NULL)
				return False;
		}
		if (SetFigureList(w, obj->obj_no))
			return False;

		obj = succ;
	}
	FreeObject(grp_node);

	return True;
}

static int      rotation[2][4] = {
				  {Dgree270, Dgree180, Dgree90, Dgree0},
				  {Dgree90, Dgree0, Dgree270, Dgree180},
};
/*
 * set figure virtical mirror 
 */
static void
_MirrorPrimitive(type, mirror, width, height, primitive)
	StFigureType    type;
	StSetMirror     mirror;
	unsigned short  width, height;
	StObjBody      *primitive;
{
	StPoint        *points;
	unsigned int    p_count;
	int             rot;

	switch ((int) type) {
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		rot = primitive->pd_arc.arc.angle1 / Dgree90;
		primitive->pd_arc.arc.angle1
			= rotation[(int) mirror][rot];
		return;
	case Stline:
	case Stpolyline:
	case Stfreehand:
	case Stpolygon:
		p_count = primitive->pd_line.point_list.n_points;
		points = primitive->pd_line.point_list.points;
		while (p_count--) {
			if (mirror == StverticalMirror)
				points->x = width - points->x;
			else
				points->y = -(height + points->y);
			++points;
		}
		return;
	case Strectangle:
	case Stcircle:
	case Stellipse:
	case Sttext:
	case Stbitmap:
	case Stgroup:
	default:
		return;
	}
}

static void
_ConvertMirrorObject(w, mirror, top_node, width, height)
	Widget          w;	/* widget identifire	(i) */
	StSetMirror     mirror;	/* mirror flag		(i) */
	StObject       *top_node;	/* objcet top node	(i) */
	unsigned short  width, height;	/* top node object size	(i) */
{
	StObject       *obj;
	int             n_obj;

	obj = top_node->body.p_group.first;
	for (n_obj = top_node->n_obj; n_obj; --n_obj) {
		if (obj->n_obj == 0) {
			_MirrorPrimitive(obj->type, mirror,
					 width, height, (char *) &obj->body);
		} else {
			/* do mirror to group. */
			if (mirror == StverticalMirror)
				obj->body.p_group.obj_pos.x = width -
					(obj->body.p_group.obj_pos.x
					 + obj->body.p_group.w);
			else
				obj->body.p_group.obj_pos.y = -(height +
						(obj->body.p_group.obj_pos.y
						 - obj->body.p_group.h));
			_ConvertMirrorObject(w, mirror, obj,
				  obj->body.p_group.w, obj->body.p_group.h);
		}
		obj = obj->body.p_group.succ;
	}

	return;
}

static void
_RotatePoint(x, y, w, h, angle)
	short          *x, *y;
	short           w, h;
	int             angle;
{
	short           t_x, t_y, cnt_x, cnt_y;

	cnt_x = *x + (w / 2);
	cnt_y = *y - (h / 2);

	switch (angle) {
	case 90:
		t_x = *x + w - cnt_x;
		t_y = *y - cnt_y;
		*x = -(t_y);
		*y = t_x;
		break;
	case 180:
		t_x = *x + w - cnt_x;
		t_y = *y - h - cnt_y;
		*x = -(t_x);
		*y = -(t_y);
		break;
	case 270:
		t_x = *x - cnt_x;
		t_y = *y - h - cnt_y;
		*x = -(-(t_y));
		*y = -(t_x);
		break;
	}
	*x += cnt_x;
	*y += cnt_y;
}

static void
_RotateGroup(x, y, w, h, grp_w, grp_h, angle)
	short          *x, *y;
	unsigned short  w, h, grp_w, grp_h;
	int             angle;
{
	short           t_x, t_y;

	t_x = *x;
	t_y = *y;

	switch (angle) {
	case 90:
		*x = -(t_y);
		*y = t_x + w - grp_w;
		return;
	case 180:
		*x = grp_w - (t_x + w);
		*y = -(t_y - h + grp_h);
		return;
	case 270:
		*x = t_y - h + grp_h;
		*y = -(t_x);
		return;
	}
}

/*
 * set rotate to figure's coordinates. 
 */
static Boolean
_RotatePrimitive(type, angle, width, height, primitive)
	StFigureType    type;
	int             angle;
	unsigned short  width, height;
	StObjBody      *primitive;
{
	StPoint        *points, t_point;
	unsigned int    p_count;
	unsigned short  t_size;

	switch ((int) type) {
	case Strectangle:
		if (angle == 90 || angle == 270) {
			t_size = primitive->pd_rect.rectangle.width;
			primitive->pd_rect.rectangle.width
				= primitive->pd_rect.rectangle.height;
			primitive->pd_rect.rectangle.height = t_size;
		}
		return True;
	case Stellipse:
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		if (angle == 90 || angle == 270) {
			t_size = primitive->pd_arc.arc.width;
			primitive->pd_arc.arc.width
				= primitive->pd_arc.arc.height;
			primitive->pd_arc.arc.height = t_size;
		}
		if (type == Stcircle || type == Stellipse)
			return;
		primitive->pd_arc.arc.angle1
			= (primitive->pd_arc.arc.angle1 / 64 + angle)
			% 360 * 64;
		return True;
	case Stline:
	case Stpolyline:
	case Stfreehand:
	case Stpolygon:
		p_count = primitive->pd_line.point_list.n_points;
		points = primitive->pd_line.point_list.points;
		while (p_count--) {
			t_point = *points;
			if (angle == 90) {
				points->x = -(t_point.y);
				points->y = t_point.x - width;
			} else if (angle == 180) {
				points->x = width - t_point.x;
				points->y = -(t_point.y + height);
			} else {
				points->x = t_point.y + height;
				points->y = -(t_point.x);
			}
			++points;
		}
		return True;
	case Stcircle:
	case Stgroup:
		return True;
	case Sttext:
	case Stbitmap:
	default:
		return False;
	}
}

/*
 * convert rotate object 
 */
static void
_ConvertRotateObject(w, angle, top_node, width, height, stat)
	Widget          w;	/* widget identifire	(i) */
	int             angle;	/* mirror flag(0, 90, 180, 270)	(i) */
	StObject       *top_node;	/* objcet top node	(i) */
	unsigned short  width, height;	/* top node object size	(i) */
	Boolean        *stat;	/* rotate permission status (o) */
{
	StObject       *obj;
	int             n_obj;
	short           t_size;

	obj = top_node->body.p_group.first;
	for (n_obj = top_node->n_obj; n_obj; --n_obj) {
		if (obj->n_obj == 0) {
			if (_RotatePrimitive(obj->type, angle,
			       width, height, (char *) &obj->body) == False)
				*stat = False;
		} else {
			/* do rotate to group. */
			_ConvertRotateObject(w, angle, obj,
				   obj->body.p_group.w, obj->body.p_group.h,
					     stat);
			if (obj->body.p_group.first->type != Sttext
			    || obj->body.p_group.first->type != Stbitmap) {
				_RotateGroup(&obj->body.p_group.obj_pos.x,
					     &obj->body.p_group.obj_pos.y,
					     obj->body.p_group.w,
					     obj->body.p_group.h,
					     top_node->body.p_group.w,
					   top_node->body.p_group.h, angle);
				if (angle == 90 || angle == 270) {
					t_size = obj->body.p_group.w;
					obj->body.p_group.w
						= obj->body.p_group.h;
					obj->body.p_group.h = t_size;
				}
			}
		}
		obj = obj->body.p_group.succ;
	}
	return;
}
