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

#include  <X11/Intrinsic.h>
#include  <X11/StringDefs.h>
#include <St/StPictureP.h>

#define	FAILURE	-1
#define SUCCESS	 0

static void     _CreateObject();
static void     _GroupObject();
static void     _UngroupObject();
static void     _PriorObject();
static void     _MirrorObject();
static void     _RotateObject();
static void     _DeleteObject();
static void     _CopyObject();
static void     _MoveObject();
static void     _ResizeObject();

StModFunctionDescRec modfunctiondesc = {
					_CreateObject,
					_GroupObject,
					_UngroupObject,
					_PriorObject,
					_MirrorObject,
					_RotateObject,
					_DeleteObject,
					_CopyObject,
					_MoveObject,
					_ResizeObject
};

typedef union {
	StRectangle     rect_packet;
	StArc           arc_packet;
	StPointList     point_packet;
}               Packet;


static Packet  *
SetPacket(w, packet, image, plist)
	Widget          w;
	Packet         *packet;
	StDragImage    *image;
	StPoint        *plist;
{
	short           x, y, x1, x2, y1, y2;	/* xlib coodinates */
	unsigned short  width, height;
	short           wx, wy;	/* widget coodinates */
	register        i;

	if (image->npoints == 2) {
		x1 = image->points[0].x;
		x2 = image->points[1].x;
		y1 = image->points[0].y;
		y2 = image->points[1].y;
		x = Min(x1, x2);
		y = Min(y1, y2);
		width = x1 + x2 - x - x;
		height = y1 + y2 - y - y;
		if (image->type == Stcircle) {
			width = Max(width, height);
			height = width;
		}
		ToWidgetXY(w, x, y, &wx, &wy);
	}
	switch ((int) image->type) {
	case Strectangle:
		packet->rect_packet.x = wx;
		packet->rect_packet.y = wy;
		packet->rect_packet.width = (unsigned short) width;
		packet->rect_packet.height = (unsigned short) height;
		break;
	case Starc:
	case StellipticArc:
	case Stpie:
	case StellipticPie:
		SetupArcPram(w, image, &wx, &wy, &width, &height);
	case Stcircle:
	case Stellipse:
		packet->arc_packet.x = wx;
		packet->arc_packet.y = wy;
		packet->arc_packet.width = width;
		packet->arc_packet.height = height;
		packet->arc_packet.direction = image->direction;
		packet->arc_packet.angle1 = image->angle1;
		packet->arc_packet.angle2 = image->angle2;
		break;
	case Stpolyline:
	case Stfreehand:
	case Stpolygon:
		for (i = 0; i < image->npoints; i++) {
			ToWidgetXY(w, image->points[i].x, image->points[i].y,
				   &(plist[i].x), &(plist[i].y));
		}
		packet->point_packet.points = plist;
		packet->point_packet.n_points = image->npoints;
	}
	return packet;
}

static void
JoinSelected(w, xb, yb, xe, ye)
	Widget          w;
	short          *xb, *yb, *xe, *ye;	/* Widget coordinates */
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *next;
	short           ixb, iyb, ixe, iye;	/* Widget coordinates */
	register        i;

	for (i = 0; i < dc->fig_buf->n_figures; i++) {
		next = GetObjAddr(w, dc->fig_buf->figures[i]);
		ixb = next->body.p_group.obj_pos.x;
		iyb = next->body.p_group.obj_pos.y;
		ixe = ixb + next->body.p_group.w;
		iye = iyb - next->body.p_group.h;
		JoinBox(xb, yb, xe, ye, ixb, iyb, ixe, iye);
	}
}

static void
_CreateObject(w, image)
	Widget          w;
	StDragImage    *image;
{
	Packet          cr_packet;
	StFigure        fno = StNullFigure;
	short           x1, y1, x2, y2;	/* widget coodinates for line */
	StPoint        *plist = (StPoint *) NULL;
	StCoordinate    coord = StCoordWidget;

	switch ((int) image->type) {
	case Strectangle:
		fno = StPicCreateRectangle(w,
					   (StRectangle *) SetPacket(w, &cr_packet, image, NULL), coord);
		break;
	case Stcircle:
	case Stellipse:
		fno = StPicCreateEllipse(w,
		    (StArc *) SetPacket(w, &cr_packet, image, NULL), coord);
		break;
	case Starc:
	case StellipticArc:
		fno = StPicCreateArc(w,
		    (StArc *) SetPacket(w, &cr_packet, image, NULL), coord);
		break;
	case Stpie:
	case StellipticPie:
		fno = StPicCreatePie(w,
		    (StArc *) SetPacket(w, &cr_packet, image, NULL), coord);
		break;
	case Stline:
		ToWidgetXY(w, image->points[0].x, image->points[0].y,
			   &x1, &y1);
		ToWidgetXY(w, image->points[1].x, image->points[1].y,
			   &x2, &y2);
		fno = StPicCreateLine(w, x1, y1, x2, y2, coord);
		break;
	case Stpolyline:
		plist = (StPoint *) malloc(sizeof(StPoint) * image->npoints);
		fno = StPicCreatePolyline(w,
					  (StPointList *) SetPacket(w, &cr_packet, image, plist), coord);
		break;
	case Stfreehand:
		plist = (StPoint *) malloc(sizeof(StPoint) * image->npoints);
		fno = StPicCreateFreehand(w,
					  (StPointList *) SetPacket(w, &cr_packet, image, plist), coord);
		break;
	case Stpolygon:
		plist = (StPoint *) malloc(sizeof(StPoint) * image->npoints);
		fno = StPicCreatePolygon(w,
					 (StPointList *) SetPacket(w, &cr_packet, image, plist), coord);
	}
	if (plist)
		free((char *) plist);
	SelectAll(w, StunsetSelect);
	if (fno == StNullFigure)
		return;
	StPicDrawFigure(w, fno);
	TgleHandle(w, fno);
	TgleSelect(w, fno);

	RealDeleteObjects(w);
	FreeUndoPacket(w);
	NewUndoPacket(UDraw);
	SetUndoDraw(fno);
}

static void
_GroupObject(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StFigure        fno;
	StRegion        region;

	if (dc->n_selected > 1) {	/* must be multi select */
		SetupTergetFigures(w, False);	/* get current selected figre
						 * and collect asending order */
		SelectAll(w, StunsetSelect);	/* erase old handle */
		fno = StPicGroupFigure(w, dc->fig_buf);
		FreeTergetFigures(w);
		StPicGetFigureRegion(w, fno, &region, StCoordWidget);
		AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
		region.width += 40;
		region.height += 40;
		CheckRegionLimit(w, &region);
		StPicRedrawPart(w, &region, StCoordWidget);
		TgleHandle(w, fno);
		TgleSelect(w, fno);

		RealDeleteObjects(w);
		FreeUndoPacket(w);
	}
}

static StFigureList *
NewTmpFigureList()
{
	StFigureList   *flist;

	/* alloc fixed part */
	if ((flist = (StFigureList *) calloc((unsigned int) 1, sizeof(StFigureList)))
	    == (StFigureList *) NULL)
		return (StFigureList *) NULL;	/* nomore memory error */

	/* alloc entry part (dummy) */
	if ((flist->figures = (StFigure *) malloc(1)) == (StFigure *) NULL) {
		free(flist);
		return (StFigureList *) NULL;
	}
	flist->n_figures = 0;

	return flist;
}

static int
StuffFigure(flist, fno)
	StFigureList   *flist;
	StFigure        fno;
{
	StFigure       *fpf;

	flist->n_figures++;
	fpf = (StFigure *) realloc(flist->figures, sizeof(StFigure) * flist->n_figures);
	if (fpf == (StFigure *) NULL) {
		flist->n_figures--;
		return FAILURE;
	}
	flist->figures = fpf;
	flist->figures[flist->n_figures - 1] = fno;
	return SUCCESS;
}

static void
FreeTmpFigureList(flist)
	StFigureList   *flist;
{
	free((char *) (flist->figures));
	free((char *) (flist));
}

static void
_UngroupObject(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StFigureList   *flist;
	StFigureList   *flp;
	StFigure        fno;
	StRegion        region;
	register        i, j;

	if (dc->n_selected > 0) {
		SetupTergetFigures(w, False);	/* get current selected figre
						 * and collect asending order */
		SelectAll(w, StunsetSelect);	/* erase old handle */
		flp = NewTmpFigureList();
		for (i = 0; i < dc->fig_buf->n_figures; i++) {
			fno = dc->fig_buf->figures[i];
			if (StPicGetFigureType(w, fno) == Stgroup) {
				StPicGetFigureRegion(w, fno, &region, StCoordWidget);
				StPicUngroupFigure(w, fno, &flist);
				AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
				region.width += 40;
				region.height += 40;
				CheckRegionLimit(w, &region);
				StPicRedrawPart(w, &region, StCoordWidget);
				for (j = 0; j < flist->n_figures; j++)
					StuffFigure(flp, flist->figures[j]);
			} else
				StuffFigure(flp, fno);
		}
		for (i = 0; i < flp->n_figures; i++) {
			TgleHandle(w, flp->figures[i]);
			TgleSelect(w, flp->figures[i]);
		}
		FreeTergetFigures(w);
		FreeTmpFigureList(flp);

		RealDeleteObjects(w);
		FreeUndoPacket(w);
	}
}

static void
_PriorObject(w, priority)
	Widget          w;
	StSetPriority   priority;
{
	StDrawControl  *dc = GetDrawControl(w);
	StRegion        region;
	StObject       *obj;
	short           xb, yb, xe, ye;
	register        i;

	if (dc->n_selected > 0) {
		if (priority == StbottomPriority)
			SetupTergetFigures(w, True);	/* get current selected
							 * figre and collect
							 * disending order */
		else
			SetupTergetFigures(w, False);	/* get current selected
							 * figre and collect
							 * asending order */
		obj = GetObjAddr(w, dc->fig_buf->figures[0]);
		xb = obj->body.p_group.obj_pos.x;
		yb = obj->body.p_group.obj_pos.y;
		xe = xb + obj->body.p_group.w;
		ye = yb - obj->body.p_group.h;
		JoinSelected(w, &xb, &yb, &xe, &ye);
		SelectAll(w, StunsetSelect);	/* erase old handle */
		RealDeleteObjects(w);
		FreeUndoPacket(w);
		NewUndoObjects(w);	/* for undo_succ_figs initialize */
		NewUndoPacket(UPrio);
		for (i = 0; i < dc->fig_buf->n_figures; i++) {
			SetUndoPrio(w, dc->fig_buf->figures[i], priority);
			StPicChangePriority(w, dc->fig_buf->figures[i], priority);
		}
		region.x = xb;
		region.y = yb;
		region.width = xe - xb + 1;
		region.height = yb - ye + 1;
		AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
		region.width += 40;
		region.height += 40;
		CheckRegionLimit(w, &region);
		StPicRedrawPart(w, &region, StCoordWidget);
		for (i = 0; i < dc->fig_buf->n_figures; i++) {
			TgleHandle(w, dc->fig_buf->figures[i]);
			TgleSelect(w, dc->fig_buf->figures[i]);
		}
		FreeTergetFigures(w);
	}
}

static void
_MirrorObject(w, mirror)
	Widget          w;
	StSetMirror     mirror;
{
	StDrawControl  *dc = GetDrawControl(w);
	StRegion        region;
	StObject       *obj;
	short           xb, yb, xe, ye;
	register        i;

	if (dc->n_selected > 0) {
		SetupTergetFigures(w, True);	/* get current selected figre
						 * and collect disending
						 * order */
		obj = GetObjAddr(w, dc->fig_buf->figures[0]);
		xb = obj->body.p_group.obj_pos.x;
		yb = obj->body.p_group.obj_pos.y;
		xe = xb + obj->body.p_group.w;
		ye = yb - obj->body.p_group.h;
		JoinSelected(w, &xb, &yb, &xe, &ye);
		SelectAll(w, StunsetSelect);	/* erase old handle */
		RealDeleteObjects(w);
		FreeUndoPacket(w);
		NewUndoPacket(UMirror);
		for (i = 0; i < dc->fig_buf->n_figures; i++) {
			SetUndoMirror(dc->fig_buf->figures[i], mirror);
			StPicMirrorFigure(w, dc->fig_buf->figures[i], mirror);
		}
		region.x = xb;
		region.y = yb;
		region.width = xe - xb + 1;
		region.height = yb - ye + 1;
		AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
		region.width += 40;
		region.height += 40;
		CheckRegionLimit(w, &region);
		StPicRedrawPart(w, &region, StCoordWidget);
		for (i = 0; i < dc->fig_buf->n_figures; i++) {
			TgleHandle(w, dc->fig_buf->figures[i]);
			TgleSelect(w, dc->fig_buf->figures[i]);
		}
		FreeTergetFigures(w);
	}
}

static void
_RotateObject(w, angle)
	Widget          w;
	int             angle;
{
	StDrawControl  *dc = GetDrawControl(w);
	StRegion        region;
	StObject       *obj;
	short           xb, yb, xe, ye;
	register        i;

	if (dc->n_selected > 0) {
		SetupTergetFigures(w, True);	/* get current selected figre
						 * and collect disending
						 * order */
		obj = GetObjAddr(w, dc->fig_buf->figures[0]);
		xb = obj->body.p_group.obj_pos.x;
		yb = obj->body.p_group.obj_pos.y;
		xe = xb + obj->body.p_group.w;
		ye = yb - obj->body.p_group.h;
		JoinSelected(w, &xb, &yb, &xe, &ye);
		SelectAll(w, StunsetSelect);	/* erase old handle */
		RealDeleteObjects(w);
		FreeUndoPacket(w);
		NewUndoPacket(URotate);
		for (i = 0; i < dc->fig_buf->n_figures; i++) {
			SetUndoRotate(dc->fig_buf->figures[i], 360 * 64 - angle);
			StPicRotateFigure(w, dc->fig_buf->figures[i], angle);
		}
		JoinSelected(w, &xb, &yb, &xe, &ye);
		region.x = xb;
		region.y = yb;
		region.width = xe - xb + 1;
		region.height = yb - ye + 1;
		AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
		region.width += 40;
		region.height += 40;
		CheckRegionLimit(w, &region);
		StPicRedrawPart(w, &region, StCoordWidget);
		for (i = 0; i < dc->fig_buf->n_figures; i++) {
			TgleHandle(w, dc->fig_buf->figures[i]);
			TgleSelect(w, dc->fig_buf->figures[i]);
		}
		FreeTergetFigures(w);
	}
}

static void
_DeleteObject(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	register        i;

	if (dc->n_selected > 0) {
		RealDeleteObjects(w);	/* crean up undo buffer */
		SetupTergetFigures(w, True);	/* get current selected figre
						 * and collect disending
						 * order */
		FreeUndoPacket(w);
		NewUndoPacket(UDelete);
		for (i = 0; i < dc->fig_buf->n_figures; i++)
			SetUndoDelete(dc->fig_buf->figures[i]);
		DoUndoDelete(w);
		FreeTergetFigures(w);
	}
}

static void
_CopyObject(w, dispx, dispy)
	Widget          w;
	short           dispx, dispy;	/* Xlib coordinate */
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *next;
	short           sxb, syb, sxe, sye, dxb, dyb, dxe, dye;	/* widget coord */
	register        i, j;
	StPoint         pos;
	StRegion        rgn;
	StFigure        fno = dc->fig_buf->figures[0];
	StFigure       *new_figs;

	next = GetObjAddr(w, fno);
	sxb = next->body.p_group.obj_pos.x;
	syb = next->body.p_group.obj_pos.y;
	sxe = sxb + next->body.p_group.w;
	sye = syb - next->body.p_group.h;
	JoinSelected(w, &sxb, &syb, &sxe, &sye);

	/* check limit and adjust orgin points */
	AddDisp(w, sxb, syb, dispx, dispy, &dxb, &dyb);
	rgn.x = dxb;
	rgn.y = dyb;
	rgn.width = sxe - sxb + 1;
	rgn.height = Abs((sye - syb)) + 1;
	CheckRegionLimit(w, &rgn);
	dxb = rgn.x;
	dyb = rgn.y;
	dxe = dxb + rgn.width;
	dye = dyb - rgn.height;
	dispx = dxb - sxb;
	dispy = (dyb - syb) * -1;

	SelectAll(w, StunsetSelect);	/* erase old handle */

	new_figs = (StFigure *) malloc(sizeof(StFigure) * dc->fig_buf->n_figures);

	for (i = dc->fig_buf->n_figures, j = 0; i > 0; i--, j++) {
		next = GetObjAddr(w, dc->fig_buf->figures[i - 1]);
		AddDisp(w, next->body.p_group.obj_pos.x,
			next->body.p_group.obj_pos.y,
			dispx, dispy, &pos.x, &pos.y);
		new_figs[j] = StPicCopyFigure(w, dc->fig_buf->figures[i - 1],
					      &pos, StCoordWidget);
	}
	rgn.x = dxb;
	rgn.y = dyb;
	rgn.width = sxe - sxb + 1;
	rgn.height = Abs((sye - syb)) + 1;
	AddDisp(w, rgn.x, rgn.y, -20, -20, &rgn.x, &rgn.y);
	rgn.width += 40;
	rgn.height += 40;
	CheckRegionLimit(w, &rgn);
	StPicRedrawPart(w, &rgn, StCoordWidget);

	RealDeleteObjects(w);
	FreeUndoPacket(w);
	NewUndoPacket(UCopy);
	for (i = 0; i < dc->fig_buf->n_figures; i++) {
		TgleSelect(w, new_figs[i]);
		SetUndoCopy(new_figs[i]);
	}
}

static void
_MoveObject(w, dispx, dispy)
	Widget          w;
	short           dispx, dispy;	/* Xlib coordinate */
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *next;
	short           sxb, syb, sxe, sye, dxb, dyb, dxe, dye;	/* widget coord */
	long            longx, longy;
	register        i;
	StPoint         pos;
	StRegion        rgn;
	StFigure        fno = dc->fig_buf->figures[0];

	next = GetObjAddr(w, fno);
	sxb = next->body.p_group.obj_pos.x;
	syb = next->body.p_group.obj_pos.y;
	sxe = sxb + next->body.p_group.w;
	sye = syb - next->body.p_group.h;
	JoinSelected(w, &sxb, &syb, &sxe, &sye);

	/* check limit and adjust orgin points */
	AddDisp(w, sxb, syb, dispx, dispy, &dxb, &dyb);
	rgn.x = dxb;
	rgn.y = dyb;
	rgn.width = sxe - sxb + 1;
	rgn.height = Abs((sye - syb)) + 1;
	CheckRegionLimit(w, &rgn);
	dxb = rgn.x;
	dyb = rgn.y;
	dxe = dxb + rgn.width;
	dye = dyb - rgn.height;
	dispx = dxb - sxb;
	dispy = (dyb - syb) * -1;

	SelectAll(w, StunsetSelect);	/* erase old handle */
	RealDeleteObjects(w);
	FreeUndoPacket(w);
	NewUndoPacket(UMove);
	for (i = 0; i < dc->fig_buf->n_figures; i++) {
		next = GetObjAddr(w, dc->fig_buf->figures[i]);
		SetUndoMove(dc->fig_buf->figures[i],
			    next->body.p_group.obj_pos.x,
			    next->body.p_group.obj_pos.y);
		AddDisp(w, next->body.p_group.obj_pos.x,
			next->body.p_group.obj_pos.y,
			dispx, dispy, &pos.x, &pos.y);
		StPicMoveFigure(w, dc->fig_buf->figures[i], &pos, StCoordWidget);
	}
	rgn.x = sxb;
	rgn.y = syb;
	rgn.width = sxe - sxb + 1;
	rgn.height = Abs((sye - syb)) + 1;
	AddDisp(w, rgn.x, rgn.y, -20, -20, &rgn.x, &rgn.y);
	rgn.width += 40;
	rgn.height += 40;
	CheckRegionLimit(w, &rgn);
	if (IsCrossBox(sxb, syb, sxe, sye, dxb, dyb, dxe, dye)) {
		JoinBox(&sxb, &syb, &sxe, &sye, dxb, dyb, dxe, dye);
		rgn.x = sxb;
		rgn.y = syb;
		rgn.width = sxe - sxb + 1;
		rgn.height = Abs((sye - syb)) + 1;
		AddDisp(w, rgn.x, rgn.y, -20, -20, &rgn.x, &rgn.y);
		rgn.width += 40;
		rgn.height += 40;
		CheckRegionLimit(w, &rgn);
		StPicRedrawPart(w, &rgn, StCoordWidget);
	} else {
		StPicRedrawPart(w, &rgn, StCoordWidget);
		rgn.x = dxb;
		rgn.y = dyb;
		AddDisp(w, rgn.x, rgn.y, -20, -20, &rgn.x, &rgn.y);
		CheckRegionLimit(w, &rgn);	/* width and height have
						 * allready calcrated */
		StPicRedrawPart(w, &rgn, StCoordWidget);
	}
	for (i = 0; i < dc->fig_buf->n_figures; i++) {
		TgleSelect(w, dc->fig_buf->figures[i]);
	}
}

static void
_ResizeObject(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *next;
	short           sxb, syb, sxe, sye, dxb, dyb, dxe, dye;	/* widget coord */
	short           x1, y1, x2, y2;	/* Xlib coord */
	unsigned short  width, height;
	StPoint         pos;
	StRegion        rgn;
	StFigureList   *flist;
	StFigure        fno = dc->fig_buf->figures[0];	/* terget fig set in
							 * IsResizeAction() */
	register        i;

	next = GetObjAddr(w, fno);
	sxb = next->body.p_group.obj_pos.x;
	syb = next->body.p_group.obj_pos.y;
	sxe = sxb + next->body.p_group.w;
	sye = syb - next->body.p_group.h;

	RealDeleteObjects(w);
	FreeUndoPacket(w);
	NewUndoPacket(UResize);
	SetUndoResize(fno, sxb, syb, next->body.p_group.w, next->body.p_group.h);

	x1 = dc->image->points[0].x;
	x2 = dc->image->points[1].x;
	y1 = dc->image->points[0].y;
	y2 = dc->image->points[1].y;
	dxb = Min(x1, x2);
	dyb = Min(y1, y2);
	width = x1 + x2 - dxb - dxb;
	height = y1 + y2 - dyb - dyb;
	ToWidgetXY(w, dxb, dyb, &dxb, &dyb);
	dxe = dxb + width;
	dye = dyb - height;

	StPicGetSelectedFigure(w, &flist);	/* get fno list of current
						 * selected fig */
	SelectAll(w, StunsetSelect);	/* erase old handle */
	StPicResizeFigure(w, fno, width, height);
	if (sxb != dxb || syb != dyb) {
		pos.x = dxb;
		pos.y = dyb;
		StPicMoveFigure(w, fno, &pos, StCoordWidget);
	}
	JoinBox(&sxb, &syb, &sxe, &sye, dxb, dyb, dxe, dye);
	rgn.x = sxb;
	rgn.y = syb;
	rgn.width = sxe - sxb + 1;
	rgn.height = Abs((sye - syb)) + 1;
	AddDisp(w, rgn.x, rgn.y, -20, -20, &rgn.x, &rgn.y);
	rgn.width += 40;
	rgn.height += 40;
	CheckRegionLimit(w, &rgn);
	StPicRedrawPart(w, &rgn, StCoordWidget);

	if (flist)		/* exist another selected figure */
		for (i = 0; i < flist->n_figures; i++)
			TgleSelect(w, flist->figures[i]);
	TgleSelect(w, fno);
}
