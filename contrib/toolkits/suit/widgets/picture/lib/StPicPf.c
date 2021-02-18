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
 * Picture Widget Public Functions. 
 */

#include  <stdio.h>
#include  <X11/Xlib.h>
#include  <X11/Intrinsic.h>
#include  <X11/StringDefs.h>
#include  <St/StPictureP.h>

#define	FIGURE		1
#define	GROUP_START	2
#define	GROUP_END	3

int
StPicGetMouseLocation(w, point_return, coordinate)
	Widget          w;
	StPoint        *point_return;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	Window          rw, cw;
	int             rx, ry, cx, cy;
	short           retx, rety;
	unsigned int    mask;

	point_return->x = point_return->y = 0;
	if (!XQueryPointer(dc->dpy, dc->window,
			   &rw, &cw, &rx, &ry, &cx, &cy, &mask))
		return Failure;
	retx = (short) cx;
	rety = (short) cy;
	if (coordinate == StCoordWidget)
		ToWidgetXY(w, retx, rety, &retx, &rety);
	point_return->x = retx;
	point_return->y = rety;
	return Success;
}

int
StPicSetFigureType(w, type)
	Widget          w;
	StFigureType    type;
{
	StPictureWidget spw = (StPictureWidget) w;

	if (spw->stPicture.mode_continue)
		return Failure;
	spw->stPicture.mode_continue = True;
	spw->stPicture.mode = StpictureDraw;
	spw->stPicture.mode_draw = GetMenu(type);
	SetDrawStat(w, PicStartDraw);
	return Success;
}

int
StPicBreakDraw(w)
	Widget          w;
{
	StPictureWidget spw = (StPictureWidget) w;

	if (spw->stPicture.mode_continue)
		return Failure;
	spw->stPicture.mode_continue = False;
	spw->stPicture.mode = StpictureNull;
	spw->stPicture.mode_draw = StdrawNull;
	SetDrawStat(w, PicStartSelect);
	return Success;
}

StFigure
StPicCreateRectangle(w, rectangle, coordinate)
	Widget          w;
	StRectangle    *rectangle;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	StFigure        fno;
	short           x = rectangle->x, y = rectangle->y;

	if (AllocObject(w, sizeof(StRectPrimitive), &obj_ptr))
		return StNullFigure;	/* error */
	obj_ptr->type = Strectangle;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_rect.type = Strectangle;
	obj_ptr->body.pd_rect.line_atr = dc->gl_line_atr;
	obj_ptr->body.pd_rect.paint_atr = dc->gl_paint_atr;
	obj_ptr->body.pd_rect.rectangle.x = 0;	/* set offset from group node */
	obj_ptr->body.pd_rect.rectangle.y = 0;	/* set offset from group node */
	obj_ptr->body.pd_rect.rectangle.width = rectangle->width;
	obj_ptr->body.pd_rect.rectangle.height = rectangle->height;

	if (coordinate == StCoordEvent)
		ToWidgetXY(w, x, y, &x, &y);
	if (NewFigure(w, x, y,
		      rectangle->width, rectangle->height, obj_ptr, &fno))
		return StNullFigure;	/* error */
	return fno;
}

StFigure
StPicCreateEllipse(w, ellipse, coordinate)
	Widget          w;
	StArc          *ellipse;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	StFigure        fno;
	short           x = ellipse->x, y = ellipse->y;

	if (AllocObject(w, sizeof(StArcPrimitive), &obj_ptr))
		return StNullFigure;	/* error */

	if (ellipse->width == ellipse->height)
		obj_ptr->type = Stcircle;
	else
		obj_ptr->type = Stellipse;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_arc.type = obj_ptr->type;
	obj_ptr->body.pd_arc.line_atr = dc->gl_line_atr;
	obj_ptr->body.pd_arc.paint_atr = dc->gl_paint_atr;
	obj_ptr->body.pd_arc.arc.x = 0;	/* set offset from group node */
	obj_ptr->body.pd_arc.arc.y = 0;	/* set offset from group node */
	obj_ptr->body.pd_arc.arc.width = ellipse->width;
	obj_ptr->body.pd_arc.arc.height = ellipse->height;

	if (coordinate == StCoordEvent)
		ToWidgetXY(w, x, y, &x, &y);
	if (NewFigure(w, x, y,
		      ellipse->width, ellipse->height, obj_ptr, &fno))
		return StNullFigure;	/* error */
	return fno;
}

StFigure
StPicCreateArc(w, arc, coordinate)
	Widget          w;
	StArc          *arc;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	StFigure        fno;
	short           x = arc->x, y = arc->y;

	if (AllocObject(w, sizeof(StArcPrimitive), &obj_ptr))
		return StNullFigure;	/* error */

	if (arc->width == arc->height)
		obj_ptr->type = Starc;
	else
		obj_ptr->type = StellipticArc;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_arc.type = obj_ptr->type;
	obj_ptr->body.pd_arc.line_atr = dc->gl_line_atr;
	obj_ptr->body.pd_arc.paint_atr = dc->gl_paint_atr;
	obj_ptr->body.pd_arc.arc.direction = arc->direction;
	obj_ptr->body.pd_arc.arc.x = 0;	/* set offset from group node */
	obj_ptr->body.pd_arc.arc.y = 0;	/* set offset from group node */
	obj_ptr->body.pd_arc.arc.width = arc->width / 2;
	obj_ptr->body.pd_arc.arc.height = arc->height / 2;
	obj_ptr->body.pd_arc.arc.angle1 = arc->angle1;
	obj_ptr->body.pd_arc.arc.angle2 = arc->angle2;

	switch (arc->angle1) {
	case 0 * 64:
		x += arc->width / 2;
		break;
	case 90 * 64:
		break;
	case 180 * 64:
		if (coordinate == StCoordEvent)
			y += arc->height / 2;
		else
			y -= arc->height / 2;
		break;
	case 270 * 64:
		x += arc->width / 2;
		if (coordinate == StCoordEvent)
			y += arc->height / 2;
		else
			y -= arc->height / 2;
		break;
	}
	if (coordinate == StCoordEvent)
		ToWidgetXY(w, x, y, &x, &y);
	if (NewFigure(w, x, y,
		      arc->width / 2, arc->height / 2, obj_ptr, &fno))
		return StNullFigure;	/* error */
	return fno;
}

StFigure
StPicCreatePie(w, pie, coordinate)
	Widget          w;
	StArc          *pie;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	StFigure        fno;
	short           x = pie->x, y = pie->y;

	if (AllocObject(w, sizeof(StArcPrimitive), &obj_ptr))
		return StNullFigure;	/* error */

	if (pie->width == pie->height)
		obj_ptr->type = Stpie;
	else
		obj_ptr->type = StellipticPie;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_arc.type = obj_ptr->type;
	obj_ptr->body.pd_arc.line_atr = dc->gl_line_atr;
	obj_ptr->body.pd_arc.paint_atr = dc->gl_paint_atr;
	obj_ptr->body.pd_arc.arc.direction = pie->direction;
	obj_ptr->body.pd_arc.arc.x = 0;	/* set offset from group node */
	obj_ptr->body.pd_arc.arc.y = 0;	/* set offset from group node */
	obj_ptr->body.pd_arc.arc.width = pie->width / 2;
	obj_ptr->body.pd_arc.arc.height = pie->height / 2;
	obj_ptr->body.pd_arc.arc.angle1 = pie->angle1;
	obj_ptr->body.pd_arc.arc.angle2 = pie->angle2;

	switch (pie->angle1) {
	case 0 * 64:
		x += pie->width / 2;
		break;
	case 90 * 64:
		break;
	case 180 * 64:
		if (coordinate == StCoordEvent)
			y += pie->height / 2;
		else
			y -= pie->height / 2;
		break;
	case 270 * 64:
		x += pie->width / 2;
		if (coordinate == StCoordEvent)
			y += pie->height / 2;
		else
			y -= pie->height / 2;
		break;
	}
	if (coordinate == StCoordEvent)
		ToWidgetXY(w, x, y, &x, &y);
	if (NewFigure(w, x, y,
		      pie->width / 2, pie->height / 2, obj_ptr, &fno))
		return StNullFigure;	/* error */
	return fno;
}

StFigure
StPicCreateLine(w, x1, y1, x2, y2, coordinate)
	Widget          w;
	short           x1, y1;
	short           x2, y2;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	StPoint        *plist;
	short           x, y;
	unsigned short  width, height;
	StFigure        fno;

	if (AllocObject(w, sizeof(StLinePrimitive), &obj_ptr))
		return StNullFigure;	/* error */
	if ((plist = (StPoint *) malloc(sizeof(StPoint) * 2)) == (StPoint *) NULL) {
		free(obj_ptr);
		return StNullFigure;
	}
	obj_ptr->type = Stline;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_line.type = Stline;
	obj_ptr->body.pd_line.line_atr = dc->gl_line_atr;
	obj_ptr->body.pd_line.point_list.points = plist;
	obj_ptr->body.pd_line.point_list.n_points = 2;
	if (coordinate == StCoordEvent) {
		ToWidgetXY(w, x1, y1, &x1, &y1);
		ToWidgetXY(w, x2, y2, &x2, &y2);
	}
	x = Min(x1, x2);
	y = Max(y1, y2);
	plist[0].x = x1 - x;
	plist[0].y = y1 - y;	/* set offset from group node */
	plist[1].x = x2 - x;
	plist[1].y = y2 - y;	/* set offset from group node */

	width = x1 + x2 - x - x;
	height = -(y1 + y2 - y - y);
	if (NewFigure(w, x, y, width, height, obj_ptr, &fno))
		return StNullFigure;	/* error */
	return fno;
}

StFigure
StPicCreatePolyline(w, point_list, coordinate)
	Widget          w;
	StPointList    *point_list;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	short           minx, miny, maxx, maxy;
	unsigned short  width, height;
	StPoint        *pl;
	StFigure        fno;
	register        i;

	if (AllocObject(w, sizeof(StLinePrimitive), &obj_ptr))
		return StNullFigure;	/* error */
	obj_ptr->type = Stpolyline;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_line.type = Stpolyline;
	obj_ptr->body.pd_line.line_atr = dc->gl_line_atr;
	obj_ptr->body.pd_line.point_list.n_points = point_list->n_points;

	pl = (StPoint *) calloc((unsigned int) 1,
				sizeof(StPoint) * (point_list->n_points));
	for (i = 0; i < point_list->n_points; i++) {
		pl[i].x = point_list->points[i].x;
		pl[i].y = point_list->points[i].y;
	}
	if (coordinate == StCoordEvent)
		for (i = 0; i < point_list->n_points; i++)
			ToWidgetXY(w, pl[i].x, pl[i].y, &pl[i].x, &pl[i].y);

	minx = pl[0].x;
	maxy = pl[0].y;
	maxx = pl[0].x;
	miny = pl[0].y;
	for (i = 0; i < point_list->n_points; i++) {
		minx = Min(minx, pl[i].x);
		maxy = Max(maxy, pl[i].y);
		maxx = Max(maxx, pl[i].x);
		miny = Min(miny, pl[i].y);
	}
	for (i = 0; i < point_list->n_points; i++) {
		pl[i].x -= minx;/* set offset from group node */
		pl[i].y -= maxy;/* set offset from group node */
	}
	obj_ptr->body.pd_line.point_list.points = pl;

	width = minx + maxx - minx - minx;
	height = -(miny + maxy - maxy - maxy);
	if (NewFigure(w, minx, maxy, width, height, obj_ptr, &fno))
		return StNullFigure;	/* error */

	return fno;
}

StFigure
StPicCreatePolygon(w, vertices, coordinate)
	Widget          w;
	StPointList    *vertices;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	short           minx, miny, maxx, maxy;
	unsigned short  width, height;
	StPoint        *pl;
	StFigure        fno;
	register        i;

	if (AllocObject(w, sizeof(StLinePrimitive), &obj_ptr))
		return StNullFigure;	/* error */
	obj_ptr->type = Stpolygon;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_line.type = Stpolygon;
	obj_ptr->body.pd_line.line_atr = dc->gl_line_atr;
	obj_ptr->body.pd_line.paint_atr = dc->gl_paint_atr;
	obj_ptr->body.pd_line.point_list.n_points = vertices->n_points;

	pl = (StPoint *) calloc((unsigned int) 1,
				sizeof(StPoint) * (vertices->n_points));
	for (i = 0; i < vertices->n_points; i++) {
		pl[i].x = vertices->points[i].x;
		pl[i].y = vertices->points[i].y;
	}
	if (coordinate == StCoordEvent)
		for (i = 0; i < vertices->n_points; i++)
			ToWidgetXY(w, pl[i].x, pl[i].y, &pl[i].x, &pl[i].y);

	minx = pl[0].x;
	maxy = pl[0].y;
	maxx = pl[0].x;
	miny = pl[0].y;
	for (i = 0; i < vertices->n_points; i++) {
		minx = Min(minx, pl[i].x);
		maxy = Max(maxy, pl[i].y);
		maxx = Max(maxx, pl[i].x);
		miny = Min(miny, pl[i].y);
	}
	for (i = 0; i < vertices->n_points; i++) {
		pl[i].x -= minx;/* set offset from group node */
		pl[i].y -= maxy;/* set offset from group node */
	}
	obj_ptr->body.pd_line.point_list.points = pl;

	width = minx + maxx - minx - minx;
	height = -(miny + maxy - maxy - maxy);
	if (NewFigure(w, minx, maxy, width, height, obj_ptr, &fno))
		return StNullFigure;	/* error */

	return fno;
}

StFigure
StPicCreateFreehand(w, point_list, coordinate)
	Widget          w;
	StPointList    *point_list;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	short           minx, miny, maxx, maxy;
	unsigned short  width, height;
	StPoint        *pl;
	StFigure        fno;
	register        i;

	if (AllocObject(w, sizeof(StLinePrimitive), &obj_ptr))
		return StNullFigure;	/* error */
	obj_ptr->type = Stfreehand;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_line.type = Stfreehand;
	obj_ptr->body.pd_line.line_atr = dc->gl_line_atr;
	obj_ptr->body.pd_line.point_list.n_points = point_list->n_points;

	pl = (StPoint *) calloc((unsigned int) 1,
				sizeof(StPoint) * (point_list->n_points));
	for (i = 0; i < point_list->n_points; i++) {
		pl[i].x = point_list->points[i].x;
		pl[i].y = point_list->points[i].y;
	}
	if (coordinate == StCoordEvent)
		for (i = 0; i < point_list->n_points; i++)
			ToWidgetXY(w, pl[i].x, pl[i].y, &pl[i].x, &pl[i].y);

	minx = pl[0].x;
	maxy = pl[0].y;
	maxx = pl[0].x;
	miny = pl[0].y;
	for (i = 0; i < point_list->n_points; i++) {
		minx = Min(minx, pl[i].x);
		maxy = Max(maxy, pl[i].y);
		maxx = Max(maxx, pl[i].x);
		miny = Min(miny, pl[i].y);
	}
	for (i = 0; i < point_list->n_points; i++) {
		pl[i].x -= minx;/* set offset from group node */
		pl[i].y -= maxy;/* set offset from group node */
	}
	obj_ptr->body.pd_line.point_list.points = pl;

	width = minx + maxx - minx - minx;
	height = -(miny + maxy - maxy - maxy);
	if (NewFigure(w, minx, maxy, width, height, obj_ptr, &fno))
		return StNullFigure;	/* error */

	return fno;
}

/*
 * create text. 
 */
StFigure
StPicCreateText(w, text, coordinate)
	Widget          w;
	StText         *text;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	StFigure        fno;
	short           x = text->x, y = text->y, ev_x, ev_y;
	unsigned short  width, height;

	/* check aguments. */
	if (IsWidget(w) == False)
		return ((StFigure) SetErrorStatus(StBadWidget));

	if (AllocObject(w, sizeof(StTextPrimitive), &obj_ptr))
		return ((StFigure) SetErrorStatus(StBadAlloc));

	obj_ptr->type = Sttext;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_text.type = Sttext;
	obj_ptr->body.pd_text.text_atr = dc->gl_text_atr;

	if (!(obj_ptr->body.pd_text.text.string = SaveString(text->string)))
		return ((StFigure) SetErrorStatus(StBadAlloc));

	obj_ptr->body.pd_text.text.n_bytes = text->n_bytes;

	if (coordinate == StCoordWidget)
		ToGlobalXY(w, x, y, &ev_x, &ev_y);
	else {
		ev_x = x;
		ev_y = y;
		ToWidgetXY(w, x, y, &x, &y);
	}

	GetTextRegion(dc, ev_x, ev_y, &dc->gl_text_atr,
		      text->string, text->n_bytes, &width, &height);

	if (NewFigure(w, x, y, width, height, obj_ptr, &fno))
		return ((StFigure) SetErrorStatus(StBadAlloc));

	return fno;
}

/*
 * create bitmap. 
 */
StFigure
StPicCreateBitmap(w, bitmap, coordinate)
	Widget          w;
	StBitmap       *bitmap;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj_ptr;
	StFigure        fno;
	short           x = bitmap->x, y = bitmap->y;
	unsigned short  width, height;

	if (AllocObject(w, sizeof(StBitmapPrimitive), &obj_ptr))
		return (SetErrorStatus(StBadAlloc));

	if (coordinate == StCoordEvent)
		ToWidgetXY(w, x, y, &x, &y);

	obj_ptr->type = Stbitmap;
	obj_ptr->obj_no = StNullFigure;
	obj_ptr->body.pd_bitmap.type = Stbitmap;
	obj_ptr->body.pd_bitmap.paint_atr = dc->gl_paint_atr;

	obj_ptr->body.pd_bitmap.bitmap.x = 0;
	obj_ptr->body.pd_bitmap.bitmap.y = 0;
	if (!(obj_ptr->body.pd_bitmap.bitmap.bitmap_file
	      = SaveString(bitmap->bitmap_file)))
		return (SetErrorStatus(StBadAlloc));

	obj_ptr->body.pd_bitmap.bitmap.n_bytes = strlen(bitmap->bitmap_file);

	if (SetupBitmap(w, bitmap, &width, &height) == Failure)
		return Failure;

	if (NewFigure(w, x, y, width, height, obj_ptr, &fno))
		return (SetErrorStatus(StBadAlloc));

	return fno;
}

int
StPicGetFigure(w, pos, figure_list_return, coordinate)
	Widget          w;
	StPoint        *pos;
	StFigureList   **figure_list_return;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           x = pos->x;
	short           y = pos->y;
	short           xb, yb, xe, ye;
	StObject       *next;

	*figure_list_return = (StFigureList *) NULL;

	if (coordinate == StCoordEvent)
		ToWidgetXY(w, x, y, &x, &y);

	next = dc->last_grp;
	while (next) {
		xb = next->body.p_group.obj_pos.x;
		yb = next->body.p_group.obj_pos.y;
		xe = xb + next->body.p_group.w;
		ye = yb - next->body.p_group.h;
		if (IsInBox(x, y, xb, yb, xe, ye)) {
			if (!*figure_list_return)
				if ((*figure_list_return = (StFigureList *) NewInqBuf(w))
				    == (StFigureList *) NULL)
					return Failure;	/* malloc error */
			if (SetFigureList(w, next->obj_no))
				return Failure;	/* realloc error */
		}
		next = next->body.p_group.pred;
	}
	return Success;
}

int
StPicGetSelectedFigure(w, figure_list_return)
	Widget          w;
	StFigureList   **figure_list_return;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *next;

	*figure_list_return = (StFigureList *) NULL;
	next = dc->last_grp;
	while (next) {
		if (next->body.p_group.is_selected) {
			if (!*figure_list_return)
				if ((*figure_list_return = (StFigureList *) NewInqBuf(w))
				    == (StFigureList *) NULL)
					return Failure;	/* malloc error */
			if (SetFigureList(w, next->obj_no))
				return Failure;	/* realloc error */
		}
		next = next->body.p_group.pred;
	}
	return Success;
}

int
StPicSelectFigure(w, figure, select, handle)
	Widget          w;
	StFigure        figure;
	StSetSelect     select;
	StSetHandle     handle;
{
	StObject       *grp_ptr = GetObjAddr(w, figure);

	grp_ptr->body.p_group.is_handled = handle;
	if (grp_ptr->body.p_group.is_selected != select)
		TgleGroup(w, grp_ptr);
	return Success;
}

int
StPicMoveFigure(w, figure, dest_pos, coordinate)
	Widget          w;
	StFigure        figure;
	StPoint        *dest_pos;
	StCoordinate    coordinate;
{
	StObject       *grp_ptr = GetObjAddr(w, figure);
	short           newx = dest_pos->x, newy = dest_pos->y;
	StRegion        reg;

	if (coordinate == StCoordEvent)
		ToWidgetXY(w, newx, newy, &newx, &newy);
	reg.x = newx;
	reg.y = newy;
	reg.width = grp_ptr->body.p_group.w;
	reg.height = grp_ptr->body.p_group.h;
	CheckRegionLimit(w, &reg);
	newx = reg.x;
	newy = reg.y;
	grp_ptr->body.p_group.obj_pos.x = newx;
	grp_ptr->body.p_group.obj_pos.y = newy;

	return Success;
}

StFigure
StPicCopyFigure(w, figure, dest_pos, coordinate)
	Widget          w;
	StFigure        figure;
	StPoint        *dest_pos;
	StCoordinate    coordinate;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *grp_ptr = GetObjAddr(w, figure);
	StObject       *new_obj = (StObject *) NULL;
	short           newx = dest_pos->x, newy = dest_pos->y;
	StRegion        reg;

	if (coordinate == StCoordEvent)
		ToWidgetXY(w, newx, newy, &newx, &newy);

	if (CopyObjectTree(w, grp_ptr, &new_obj, 0))
		return StNullFigure;
	reg.x = newx;
	reg.y = newy;
	reg.width = grp_ptr->body.p_group.w;
	reg.height = grp_ptr->body.p_group.h;
	CheckRegionLimit(w, &reg);
	newx = reg.x;
	newy = reg.y;
	new_obj->body.p_group.obj_pos.x = newx;
	new_obj->body.p_group.obj_pos.y = newy;
	LinkObject(&dc->first_grp, &dc->last_grp, (StObject *) NULL, new_obj);
	return new_obj->obj_no;
}

/*
 * set line attributes 
 */
int
StPicSetLineAtr(w, figure, line_atr)
	Widget          w;	/* widget identifire    (i) */
	StFigure        figure;	/* figure identifire    (i) */
	StLineAtr      *line_atr;	/* line attribute       (i) */
{
	StDrawControl  *dc;
	StObject       *obj;

	dc = GetDrawControl(w);

	/* check arguments */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));
	if (figure == StNullFigure) {
		/* set global line attributes */
		dc->gl_line_atr = *line_atr;
	} else {
		/* search figure in object tree */
		if ((obj = GetObjAddr(w, figure)) == NULL)
			return (SetErrorStatus(StBadFigure));
		/* set specified figure's line attribute */
		if (SetObjAtr(dc, obj, LINE_ATR, (char *) line_atr))
			return (SetErrorStatus(StBadFigure));
	}
	return Success;
}

/*
 * set paint attributes 
 */
int
StPicSetPaintAtr(w, figure, paint_atr)
	Widget          w;	/* widget identifire    (i) */
	StFigure        figure;	/* figure identifire    (i) */
	StPaintAtr     *paint_atr;	/* paint attribute      (i) */
{
	StDrawControl  *dc;
	StObject       *obj;

	dc = GetDrawControl(w);

	/* check arguments */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));
	if (figure == StNullFigure) {
		/* set global paint attributes */
		dc->gl_paint_atr = *paint_atr;
	} else {
		/* search figure in object tree */
		if ((obj = GetObjAddr(w, figure)) == NULL)
			return (SetErrorStatus(StBadFigure));
		/* set specified figure's paint attribute */
		if (SetObjAtr(dc, obj, PAINT_ATR, (char *) paint_atr))
			return (SetErrorStatus(StBadFigure));
	}
	return Success;
}

/*
 * set text attributes 
 */
int
StPicSetTextAtr(w, figure, text_atr)
	Widget          w;	/* widget identifire    (i) */
	StFigure        figure;	/* figure identifire    (i) */
	StTextAtr      *text_atr;	/* text attribute       (i) */
{
	StDrawControl  *dc;
	StObject       *obj;

	dc = GetDrawControl(w);

	/* check arguments */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));
	if (figure == StNullFigure) {
		/* set global text attributes */
		dc->gl_text_atr = *text_atr;
	} else {
		/* search figure in object tree */
		if ((obj = GetObjAddr(w, figure)) == NULL)
			return (SetErrorStatus(StBadFigure));
		/* set specified figure's text attribute */
		if (SetObjAtr(dc, obj, TEXT_ATR, (char *) text_atr))
			return (SetErrorStatus(StBadFigure));
	}
	return Success;
}

/*
 * get line attributes 
 */
int
StPicGetLineAtr(w, figure, line_atr_return)
	Widget          w;	/* widget identifire    (i) */
	StFigure        figure;	/* figure identifire    (i) */
	StLineAtr      *line_atr_return;	/* line attribute      (o) */
{
	StDrawControl  *dc;
	StObject       *obj;

	dc = GetDrawControl(w);

	/* check arguments */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));
	if (figure == StNullFigure) {
		/* get global line attributes */
		*line_atr_return = dc->gl_line_atr;
	} else {
		/* search figure in object tree */
		if ((obj = GetObjAddr(w, figure)) == NULL)
			return (SetErrorStatus(StBadFigure));
		/* get specified figure's line attribute */
		if (GetObjAtr(obj, LINE_ATR, (char *) line_atr_return))
			return (SetErrorStatus(StBadFigure));
	}

	return Success;
}

/*
 * get paint attributes 
 */
int
StPicGetPaintAtr(w, figure, paint_atr_return)
	Widget          w;	/* widget identifire    (i) */
	StFigure        figure;	/* figure identifire    (i) */
	StPaintAtr     *paint_atr_return;	/* paint attribute      (o) */
{
	StDrawControl  *dc;
	StObject       *obj;

	dc = GetDrawControl(w);

	/* check arguments */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));
	if (figure == StNullFigure) {
		/* get global paint attributes */
		*paint_atr_return = dc->gl_paint_atr;
	} else {
		/* search figure in object tree */
		if ((obj = GetObjAddr(w, figure)) == NULL)
			return (SetErrorStatus(StBadFigure));
		/* get specified figure's paint attribute */
		if (GetObjAtr(obj, PAINT_ATR, (char *) paint_atr_return))
			return (SetErrorStatus(StBadFigure));
	}

	return Success;
}

/*
 * get text attributes 
 */
int
StPicGetTextAtr(w, figure, text_atr_return)
	Widget          w;	/* widget identifire    (i) */
	StFigure        figure;	/* figure identifire    (i) */
	StTextAtr      *text_atr_return;	/* text attribute       (o) */
{
	StDrawControl  *dc;
	StObject       *obj;

	dc = GetDrawControl(w);

	/* check arguments */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));
	if (figure == StNullFigure) {
		/* get global text attributes */
		*text_atr_return = dc->gl_text_atr;
	} else {
		/* search figure in object tree */
		if ((obj = GetObjAddr(w, figure)) == NULL)
			return (SetErrorStatus(StBadFigure));
		/* get specified figure's text attribute */
		if (GetObjAtr(obj, TEXT_ATR, (char *) text_atr_return))
			return (SetErrorStatus(StBadFigure));
	}

	return Success;
}

/*
 * draw figure 
 */
int				/* status  code                 (ret) */
StPicDrawFigure(w, figure)
	Widget          w;	/* widget identifire            (i) */
	StFigure        figure;	/* figure idenitifire           (i) */
{
	StObject       *obj;

	/* checkup argument */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	/* get object, specified figure number */
	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	/* set clipping area from DrawCntl */

	/* darwing primitive acording to object's type */
	DrawObject(w, obj,
		   obj->body.p_group.obj_pos.x, obj->body.p_group.obj_pos.y);

	return Success;
}

/*
 * redraw part 
 */
int				/* status  code                 (ret) */
StPicRedrawPart(w, region, coordinate)
	Widget          w;	/* widget identifire            (i) */
	StRegion       *region;	/* redraw region                (i) */
	StCoordinate    coordinate;
{
	StFigureList    f_list;
	StObject       *obj;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if (coordinate == StCoordEvent)
		ToWidgetXY(w, region->x, region->y, &region->x, &region->y);

	if (IsRegion(w, region) == False)
		return (SetErrorStatus(StBadValue));

	/* traversal figures in the region. */
	RedrawRegion(w, region);

	return Success;
}

/*
 * redraw all figures 
 */
int				/* status  code                 (ret) */
StPicRedrawAll(w)
	Widget          w;	/* widget identifire            (i) */
{
	StDrawControl  *dc;
	StPictureWidget spw = (StPictureWidget) w;
	StRegion        view_reg;
	StFigureList   *flst = (StFigureList *) NULL;
	register        i;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	/* get view area region. */
	dc = GetDrawControl(w);

	view_reg.x = spw->stPicture.view_x;
	view_reg.y = spw->stPicture.view_y;
	view_reg.width = spw->stPicture.view_w;
	view_reg.height = spw->stPicture.view_h;

	StPicGetSelectedFigure(w, &flst);
	SelectAll(w, StunsetSelect);
	/* traversal figures in the region. */
	RedrawRegion(w, &view_reg);
	if (flst) {
		for (i = 0; i < flst->n_figures; i++)
			StPicSelectFigure(w, flst->figures[i],
					  StsetSelect, StsetHandle);
	}
	return Success;
}

/****************************************************************
 *		Inquire functions				*
*****************************************************************/

/*
 * get all figure identifire 
 */
int				/* status  code 		(ret) */
StPicGetAllFigure(w, figure_list_return)
	Widget          w;	/* widget identifire		(i) */
	StFigureList  **figure_list_return;	/* figure idenitifire list
						 * (o) */
{
	StDrawControl  *dc;
	StObject       *obj;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	*figure_list_return = (StFigureList *) NULL;

	dc = GetDrawControl(w);
	obj = dc->first_grp;

	while (obj != NULL) {
		if (!*figure_list_return) {
			if (!(*figure_list_return
			      = (StFigureList *) NewInqBuf(w)))
				return (SetErrorStatus(StBadAlloc));
		}
		if (SetFigureList(w, obj->obj_no))
			return (SetErrorStatus(StBadAlloc));
		obj = obj->body.p_group.succ;
	}
	return Success;
}

/*
 * get figure type 
 */
StFigureType			/* figure  type 		(ret) */
StPicGetFigureType(w, figure)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
{
	StObject       *obj;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return ((StFigureType) SetErrorStatus(StBadWidget));

	if (!(obj = GetObjAddr(w, figure)))
		return ((StFigureType) SetErrorStatus(StBadFigure));

	if (obj->n_obj > 1)
		return Stgroup;
	return (obj->body.p_group.first->type);
}

/*
 * get line infomation 
 */
int				/* status  code 	(ret) */
StPicGetlineInfo(w, figure, point_list_return, coordinate)
	Widget          w;	/* widget identifire	(i) */
	StFigure        figure;	/* figure idenitifire	(i) */
	StPointList   **point_list_return;	/* point list of line	(o) */
	StCoordinate    coordinate;	/* coordinate type	(i) */
{
	StObject       *obj, *first;
	unsigned int    npoints;
	StPoint        *points;
	register        i;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if (!(obj = GetObjAddr(w, figure)))
		return (SetErrorStatus(StBadFigure));

	first = obj->body.p_group.first;
	if (first->type != Stline)
		return (SetErrorStatus(StBadFigure));

	npoints = first->body.pd_line.point_list.n_points;
	points = first->body.pd_line.point_list.points;

	for (i = 0; i < npoints; ++i) {
		if (!*point_list_return) {
			if (!(*point_list_return
			      = (StPointList *) NewInqBuf(w)))
				return (SetErrorStatus(StBadAlloc));
		}
		if (SetPointList(w, points[i]))
			return (SetErrorStatus(StBadAlloc));
		points[i].x += obj->body.p_group.obj_pos.x;
		points[i].y += obj->body.p_group.obj_pos.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, points[i].x, points[i].y,
				   &points[i].x, &points[i].y);
	}
	return Success;
}

/*
 * get rectangle infomation 
 */
int				/* status code		(ret) */
StPicGetRectInfo(w, figure, rect_info_return, coordinate)
	Widget          w;	/* widget identifire	(i) */
	StFigure        figure;	/* figure idenitifire	(i) */
	StRectangle    *rect_info_return;	/* rectangle infomation	(o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StDrawControl  *dc;
	StObject       *obj, *first;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	first = obj->body.p_group.first;
	if (first->type == Strectangle) {
		*rect_info_return = first->body.pd_rect.rectangle;
		rect_info_return->x += obj->body.p_group.obj_pos.x;
		rect_info_return->y += obj->body.p_group.obj_pos.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, rect_info_return->x, rect_info_return->y,
				&rect_info_return->x, &rect_info_return->y);
		return Success;
	} else
		return (SetErrorStatus(StBadFigure));
}

/*
 * get polyline infomation 
 */
int				/* status  code 		(ret) */
StPicGetPolylineInfo(w, figure, point_list_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StPointList   **point_list_return;	/* point list of polyline
						 * (o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StObject       *obj, *first;
	unsigned int    npoints;
	StPoint        *points;
	register        i;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if (!(obj = GetObjAddr(w, figure)))
		return (SetErrorStatus(StBadFigure));

	first = obj->body.p_group.first;
	if (first->type != Stpolyline)
		return (SetErrorStatus(StBadFigure));

	npoints = first->body.pd_line.point_list.n_points;
	points = first->body.pd_line.point_list.points;

	for (i = 0; i < npoints; ++i) {
		if (!*point_list_return) {
			if (!(*point_list_return
			      = (StPointList *) NewInqBuf(w)))
				return (SetErrorStatus(StBadAlloc));
		}
		if (SetPointList(w, points[i]))
			return (SetErrorStatus(StBadAlloc));
		points[i].x += obj->body.p_group.obj_pos.x;
		points[i].y += obj->body.p_group.obj_pos.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, points[i].x, points[i].y,
				   &points[i].x, &points[i].y);

	}
	return Success;
}

/*
 * get polygon infomation 
 */
int				/* status  code 		(ret) */
StPicGetPolygonInfo(w, figure, vertices_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StPointList   **vertices_return;	/* vertex list of polygon
						 * (o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StObject       *obj, *first;
	unsigned int    npoints;
	StPoint        *points;
	register        i;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if (!(obj = GetObjAddr(w, figure)))
		return (SetErrorStatus(StBadFigure));

	first = obj->body.p_group.first;
	if (first->type != Stpolygon)
		return (SetErrorStatus(StBadFigure));

	npoints = first->body.pd_line.point_list.n_points;
	points = first->body.pd_line.point_list.points;

	for (i = 0; i < npoints; ++i) {
		if (!*vertices_return) {
			if (!(*vertices_return
			      = (StPointList *) NewInqBuf(w)))
				return (SetErrorStatus(StBadAlloc));
		}
		if (SetPointList(w, points[i]))
			return (SetErrorStatus(StBadAlloc));
		points[i].x += obj->body.p_group.obj_pos.x;
		points[i].y += obj->body.p_group.obj_pos.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, points[i].x, points[i].y,
				   &points[i].x, &points[i].y);
	}
	return Success;
}

/*
 * get freehanded infomation 
 */
int				/* status  code 		(ret) */
StPicGetFreehandInfo(w, figure, point_list_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StPointList   **point_list_return;	/* position list of line
						 * (o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StObject       *obj, *first;
	unsigned int    npoints;
	StPoint        *points;
	register        i;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if (!(obj = GetObjAddr(w, figure)))
		return (SetErrorStatus(StBadFigure));

	first = obj->body.p_group.first;
	if (first->type != Stfreehand)
		return (SetErrorStatus(StBadFigure));

	npoints = first->body.pd_line.point_list.n_points;
	points = first->body.pd_line.point_list.points;

	for (i = 0; i < npoints; ++i) {
		if (!*point_list_return) {
			if (!(*point_list_return
			      = (StPointList *) NewInqBuf(w)))
				return (SetErrorStatus(StBadAlloc));
		}
		if (SetPointList(w, points[i]))
			return (SetErrorStatus(StBadAlloc));
		points[i].x += obj->body.p_group.obj_pos.x;
		points[i].y += obj->body.p_group.obj_pos.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, points[i].x, points[i].y,
				   &points[i].x, &points[i].y);
	}
	return Success;
}

/*
 * get ellipse infomation 
 */
int				/* status  code 		(ret) */
StPicGetEllipseInfo(w, figure, ellp_info_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StArc          *ellp_info_return;	/* ellipse infomation
						 * o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StDrawControl  *dc;
	StObject       *obj, *first;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));
	first = obj->body.p_group.first;

	if (first->type == Stellipse || first->type == Stcircle) {
		*ellp_info_return = first->body.pd_arc.arc;
		ellp_info_return->x = obj->body.p_group.obj_pos.x
			+ first->body.pd_arc.arc.x;
		ellp_info_return->y = obj->body.p_group.obj_pos.y
			+ first->body.pd_arc.arc.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, ellp_info_return->x, ellp_info_return->y,
				&ellp_info_return->x, &ellp_info_return->y);
		return Success;
	} else
		return (SetErrorStatus(StBadFigure));
}

/*
 * get arc infomation 
 */
int				/* status  code 		(ret) */
StPicGetArcInfo(w, figure, arc_info_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StArc          *arc_info_return;	/* arc infomation
						 * o) */
	StCoordinate    coordinate;
{
	StDrawControl  *dc;
	StObject       *obj, *first;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));
	first = obj->body.p_group.first;

	if (first->type == StellipticArc || first->type == Starc) {
		*arc_info_return = first->body.pd_arc.arc;
		arc_info_return->x = obj->body.p_group.obj_pos.x
			+ first->body.pd_arc.arc.x;
		arc_info_return->y = obj->body.p_group.obj_pos.y
			+ first->body.pd_arc.arc.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, arc_info_return->x, arc_info_return->y,
				   &arc_info_return->x, &arc_info_return->y);
		return Success;
	} else
		return (SetErrorStatus(StBadFigure));
}

/*
 * get pie infomation 
 */
int				/* status  code 		(ret) */
StPicGetPieInfo(w, figure, pie_info_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StArc          *pie_info_return;	/* pie infomation
						 * o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StDrawControl  *dc;
	StObject       *obj, *first;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	first = obj->body.p_group.first;
	if (first->type == StellipticPie || first->type == Stpie) {
		*pie_info_return = first->body.pd_arc.arc;
		pie_info_return->x = obj->body.p_group.obj_pos.x
			+ first->body.pd_arc.arc.x;
		pie_info_return->y = obj->body.p_group.obj_pos.y
			+ first->body.pd_arc.arc.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, pie_info_return->x, pie_info_return->y,
				   &pie_info_return->x, &pie_info_return->y);
		return Success;
	} else
		return (SetErrorStatus(StBadFigure));
}

/*
 * get text infomation 
 */
int				/* status  code 		(ret) */
StPicGetTextInfo(w, figure, text_info_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StText         *text_info_return;	/* text infomation
						 * o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StDrawControl  *dc;
	StObject       *obj, *first;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	first = obj->body.p_group.first;
	if (first->type == Sttext) {
		text_info_return->x = obj->body.p_group.obj_pos.x
			+ first->body.pd_text.text.x;
		text_info_return->y = obj->body.p_group.obj_pos.y
			+ first->body.pd_text.text.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w, text_info_return->x, text_info_return->y,
				&text_info_return->x, &text_info_return->y);
		text_info_return->string = first->body.pd_text.text.string;
		text_info_return->n_bytes = first->body.pd_text.text.n_bytes;
		return Success;
	} else
		return (SetErrorStatus(StBadFigure));
}

/*
 * get bitmap infomation 
 */
int				/* status  code 		(ret) */
StPicGetBitmapInfo(w, figure, bitmap_info_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StBitmap       *bitmap_info_return;	/* bitmap infomation
						 * o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StDrawControl  *dc;
	StObject       *obj, *first;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	first = obj->body.p_group.first;
	if (first->type == Stbitmap) {
		bitmap_info_return->x = obj->body.p_group.obj_pos.x
			+ first->body.pd_bitmap.bitmap.x;
		bitmap_info_return->y = obj->body.p_group.obj_pos.y
			+ first->body.pd_bitmap.bitmap.y;
		if (coordinate == StCoordEvent)
			ToGlobalXY(w,
			       bitmap_info_return->x, bitmap_info_return->y,
			    &bitmap_info_return->x, &bitmap_info_return->y);
		bitmap_info_return->bitmap_file
			= first->body.pd_bitmap.bitmap.bitmap_file;
		bitmap_info_return->n_bytes
			= first->body.pd_bitmap.bitmap.n_bytes;
		return Success;
	} else
		return (SetErrorStatus(StBadFigure));
}

/*
 * get figure region 
 */
int				/* status  code 		(ret) */
StPicGetFigureRegion(w, figure, region_return, coordinate)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StRegion       *region_return;	/* figure region 		(o) */
	StCoordinate    coordinate;	/* coordinate type		(i) */
{
	StDrawControl  *dc;
	StObject       *obj;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	region_return->x = obj->body.p_group.obj_pos.x;
	region_return->y = obj->body.p_group.obj_pos.y;
	region_return->width = obj->body.p_group.w;
	region_return->height = obj->body.p_group.h;

	if (coordinate == StCoordEvent)
		ToGlobalXY(w, region_return->x, region_return->y,
			   &(region_return->x), &(region_return->y));

	return Success;
}

/*
 * grouping figure 
 */
StFigure			/* group figure identifire	(ret) */
StPicGroupFigure(w, figure_list)
	Widget          w;	/* widget identifire		(i) */
	StFigureList   *figure_list;	/* figure list			(i) */
{
	StObject       *obj_ptr, *first, *last;
	StFigure        fno;
	short           grp_x, grp_y;
	unsigned short  grp_w, grp_h;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));
	if (figure_list->n_figures <= 1)
		return (SetErrorStatus(StBadFigure));

	/* get group region */
	if (GetFiguresRegion(w, figure_list, &grp_x, &grp_y,
			     &grp_w, &grp_h, &first, &last) == False)
		return (SetErrorStatus(StBadFigure));

	if (NewGroup(w, grp_x, grp_y, grp_w, grp_h, first, last,
		     &obj_ptr, &fno, True))
		return (SetErrorStatus(StBadAlloc));

	/* set figure offset position from group node */
	BuildFigureGroup(w, figure_list, obj_ptr);

	return fno;
}

/*
 * ungrouping figure 
 */
int
StPicUngroupFigure(w, figure, figure_list_return)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire	(i) */
	StFigureList  **figure_list_return;	/* figure list		(ret) */
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *grp_node;

	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if ((grp_node = GetObjAddr(w, figure)) == NULL
	    || grp_node->type != Stgroup || grp_node->n_obj <= 1)
		return (SetErrorStatus(StBadFigure));

	if (DestoryFigureGroup(w, grp_node, figure_list_return) == False)
		return (SetErrorStatus(StBadAlloc));

	return Success;
}

/*
 * set priority 
 */
int				/* status  code 		(ret) */
StPicChangePriority(w, figure, priority)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StSetPriority   priority;	/* figure priority		(i) */
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj, *first;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if (priority != StbottomPriority && priority != SttopPriority)
		return (SetErrorStatus(StBadValue));

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	if (dc->n_group <= 1)
		return Success;

	/* break object link. */
	BreakObject(&dc->first_grp, &dc->last_grp,
		    obj->body.p_group.pred, obj->body.p_group.succ);

	if (priority == StbottomPriority) {
		if (first = dc->first_grp) {
			first->body.p_group.pred = obj;
			obj->body.p_group.succ = first;
		}
		dc->first_grp = obj;
		obj->body.p_group.pred = (StObject *) NULL;
	} else if (priority == SttopPriority)
		LinkObject(&dc->first_grp, &dc->last_grp,
			   (StObject *) NULL, obj);

	return Success;
}

/*
 * mirror figure 
 */
int				/* status  code 		(ret) */
StPicMirrorFigure(w, figure, mirror)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	StSetMirror     mirror;	/* mirror flag			(i) */
{
	StObject       *obj;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if (mirror != SthorizontalMirror && mirror != StverticalMirror)
		return (SetErrorStatus(StBadValue));

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	ConvertMirrorObject(w, mirror, obj,
			    obj->body.p_group.w, obj->body.p_group.h);

	return Success;
}

/*
 * rotate figure 
 */
int				/* status  code 		(ret) */
StPicRotateFigure(w, figure, angle)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
	int             angle;	/* rotation angle 		(i) */
{
	StObject       *obj;
	unsigned short  t_size;
	Boolean         stat = True;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	if ((obj = GetObjAddr(w, figure)) == NULL)
		return (SetErrorStatus(StBadFigure));

	/* trancate angle by 90 degree */
	if (angle > 0) {
		angle /= (64 * 90);
		angle *= 90;
	}
	if (angle == 0)
		return Success;

	ConvertRotateObject(w, angle, obj,
			    obj->body.p_group.w, obj->body.p_group.h, &stat);
	if (stat == False)
		return Success;

	RotatePoint(&obj->body.p_group.obj_pos.x,
		    &obj->body.p_group.obj_pos.y,
		    obj->body.p_group.w, obj->body.p_group.h, angle);

	if (angle == 90 || angle == 270) {
		t_size = obj->body.p_group.w;
		obj->body.p_group.w = obj->body.p_group.h;
		obj->body.p_group.h = t_size;
	}
	return Success;
}

/*
 * delete figure 
 */
int				/* status  code 		(ret) */
StPicDeleteFigure(w, figure)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* figure idenitifire		(i) */
{
	StObject       *obj_ptr;

	if ((obj_ptr = GetObjAddr(w, figure)) == NULL)
		return Failure;
	UnlinkObject(w, obj_ptr);
	DeleteObjAddr(w, figure);
	DelObjNumber(w);
	return Success;
}

/*
 * resize figure 
 */
int				/* status code			(ret) */
StPicResizeFigure(w, figure, width, height)
	Widget          w;	/* widget identifire		(i) */
	StFigure        figure;	/* source figure identifire	(i) */
	unsigned short  width, height;	/* destination region size	(i) */
{
	StObject       *grp_ptr = GetObjAddr(w, figure);

	TransformObject(w, grp_ptr, width, grp_ptr->body.p_group.w,
			height, grp_ptr->body.p_group.h, 0);
}

/*
 * reinitialize figure 
 */
int				/* status code			(ret) */
StPicReinitialize(w)
	Widget          w;	/* widget identifire		(i) */
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc;
	StObject       *obj;
	StObject       *side_ptr;

	if (IsWidget(spw) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(spw);
	obj = dc->first_grp;
	while (obj) {
		side_ptr = obj->body.p_group.succ;
		StPicDeleteFigure(spw, obj->obj_no);
		obj = side_ptr;
	}
	RealDeleteObjects(spw);
	FreeUndoPacket(spw);
	if (dc->undo_succ_figs.figures) {
		XtFree((char *) dc->undo_succ_figs.figures);
		dc->undo_succ_figs.figures = (StFigure *) NULL;
	}
	if (dc->hash_tbl) {
		XtFree((char *) (dc->hash_tbl));
		dc->hash_tbl = (StHashTbl *) NULL;
	}
	if (dc->image) {
		XtFree((char *) (dc->image->points));
		XtFree((char *) (dc->image));
		dc->image = (StDragImage *) NULL;
	}
	if (dc->inq_buf) {
		XtFree((char *) (((StPointList *) (dc->inq_buf))->points));
		XtFree(dc->inq_buf);
		dc->inq_buf = (char *) NULL;
	}
	if (dc->fig_buf) {
		XtFree((char *) (dc->fig_buf->figures));
		XtFree((char *) (dc->fig_buf));
		dc->fig_buf = (StFigureList *) NULL;
	}
	if (dc->text_buf) {
		XtFree(dc->text_buf);
		dc->text_buf = (char *) NULL;
	}
	dc->text_buf = (char *)malloc(1024);
	dc->text_index = -1;
	dc->undo_succ_figs.n_figures = 0;
	dc->n_group = 0;
	dc->figure_no = 0;
	dc->n_selected = 0;
	dc->hash_tbl = MakeHashTbl(dc->hash_size);
	dc->drawstat = PicStartSelect;
}

/*
 * set view area 
 */
int				/* status code		(ret) */
StPicSetViewArea(w, move_x, move_y)
	Widget          w;	/* widget identifire	(i) */
	int             move_x, move_y;	/* move position x, y	(i) */
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc;
	StObject       *obj;
	long            n_x, n_y;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	/* check value is in short integer range. */
	if ((n_x = spw->stPicture.view_x + move_x) > 32767L
	    || (n_y = spw->stPicture.view_y + move_y) > 32767L
	    || (n_x = spw->stPicture.view_x + move_x) < -32768L
	    || (n_y = spw->stPicture.view_y + move_y) < -32768L)
		return (SetErrorStatus(StBadValue));

	spw->stPicture.view_x = (Position) n_x;
	spw->stPicture.view_y = (Position) n_y;

	return Success;
}

/*
 * clear view area 
 */
int				/* status code		(ret) */
StPicClearView(w)
	Widget          w;	/* widget identifire	(i) */
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc;
	StObject       *obj;
	int             view_x, view_y;

	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	dc = GetDrawControl(w);

	ToGlobalXY(w, spw->stPicture.view_x, spw->stPicture.view_y,
		   &view_x, &view_y);

	XClearArea(dc->dpy, dc->window, view_x, view_y,
		   (unsigned int) spw->stPicture.view_w,
		   (unsigned int) spw->stPicture.view_h, False);

	return Success;
}

/*
 * read file 
 */
int				/* status code		(ret) */
StPicReadFile(w, file_name)
	Widget          w;	/* widget identifire	(i) */
	char           *file_name;	/* file name		(i) */
{
	int             flag;
	FILE           *fp;
	char           *prim_ptr;

	if (IsWidget(w) == False)
		return SetErrorStatus(StBadWidget);
	if ((fp = fopen(file_name, "r")) == NULL)
		return SetErrorStatus(StBadIO);

	if (ReadHeader(fp) == Failure) {
		fclose(fp);
		return SetErrorStatus(StBadIO);
	}
	while (1) {
		flag = ReadObject(fp, &prim_ptr);

		if (flag == FIGURE) {
			if (ReadFigure(w, prim_ptr) == StNullFigure) {
				fclose(fp);
				return SetErrorStatus(StBadIO);
			}
		}
		if (flag == GROUP_START) {
			if (ReadGroup(w, fp) == StNullFigure) {
				fclose(fp);
				return SetErrorStatus(StBadIO);
			}
		}
		if ((flag == GROUP_END) || (flag == Failure)) {
			fclose(fp);
			return SetErrorStatus(StBadIO);
		}
		if (flag == Success) {
			fclose(fp);
			return Success;
		}
	}
}

/*
 * write file 
 */
int				/* status code		(ret) */
StPicWriteFile(w, file_name)
	Widget          w;	/* widget identifire	(i) */
	char           *file_name;	/* file name		(i) */
{
	int             i;
	FILE           *fp;

	StDrawControl  *dc;
	StObject       *grp_ptr = dc->first_grp;

	if (IsWidget(w) == False)
		return SetErrorStatus(StBadWidget);
	if ( (dc = GetDrawControl(w)) == NULL )
		return SetErrorStatus(StBadWidget);
	if ( (fp = fopen(file_name, "w")) == NULL)
		return SetErrorStatus(StBadIO);
	if ( (grp_ptr = dc->first_grp) == NULL ) {
		fclose(fp);
		return Success;
	}

	WriteHeader(fp);

	for (i = 0; i < dc->n_group; i++) {
		if (WriteGroup(fp, grp_ptr) == Failure) {
			fclose(fp);
			return SetErrorStatus(StBadIO);
		}
		if ((grp_ptr = grp_ptr->body.p_group.succ) == NULL)
			break;
	}

	fclose(fp);
	return Success;
}

/*
 * get Widget's status 
 */
int				/* status code		(ret) */
StPicGetStatus(w)
	Widget          w;	/* widget identifire	(i) */
{
	/* checkup argument. */
	if (IsWidget(w) == False)
		return (SetErrorStatus(StBadWidget));

	return (GetErrorStatus());
}
