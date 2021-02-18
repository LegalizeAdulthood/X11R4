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
#include  <St/StPictureP.h>

#define FAILURE -1
#define SUCCESS 0

static StFigureType _GetType();
static StDrawFunction _GetMenu();
static int      _GetSize();
static void     _CheckCoordLimit();
static void     _CheckRegionLimit();
static void     _AddDisp();
static void     _ToGlobalXY();
static void     _ToWidgetXY();
static void     _ScaleObject();
static Boolean  _NewObjNumber();
static Boolean  _DelObjNumber();
static StHashTbl *_MakeHashTbl();
static Boolean  _DeleteObjAddr();
static void     _SetObjAddr();
static StObject *_GetObjAddr();
static Boolean  _IsInBox();
static Boolean  _IsCrossBox();
static Boolean  _IsIncludeBox();
static void     _JoinBox();
static char    *_NewInqBuf();
static Boolean  _SetFigureList();
static Boolean  _SetPointList();
static void     _SetupTergetFigures();
static void     _FreeTergetFigures();
static Boolean  _IsMoveAction();
static Boolean  _IsResizeAction();
static void     _Memcpy();
static void     _SavePoint();
static void     _StuffPoint();
static StDrawControl *_GetDrawControl();
static void     _SetDrawStat();
static void     _SaveOldDrawStat();
static void     _RestorOldDrawStat();
static void     _MoveView();

StUtlFunctionDescRec utlfunctiondesc = {
					_GetType,
					_GetMenu,
					_GetSize,
					_CheckCoordLimit,
					_CheckRegionLimit,
					_AddDisp,
					_ToGlobalXY,
					_ToWidgetXY,
					_ScaleObject,
					_NewObjNumber,
					_DelObjNumber,
					_MakeHashTbl,
					_DeleteObjAddr,
					_SetObjAddr,
					_GetObjAddr,
					_IsInBox,
					_IsCrossBox,
					_IsIncludeBox,
					_JoinBox,
					_NewInqBuf,
					_SetFigureList,
					_SetPointList,
					_SetupTergetFigures,
					_FreeTergetFigures,
					_IsMoveAction,
					_IsResizeAction,
					_Memcpy,
					_SavePoint,
					_StuffPoint,
					_GetDrawControl,
					_SetDrawStat,
					_SaveOldDrawStat,
					_RestorOldDrawStat,
					_MoveView
};

typedef struct {
	StDrawFunction  menu;
	StFigureType    type;
	int             size;
}               FigureDescRec;

static FigureDescRec figuredesc[] = {
		    {StdrawRectangle, Strectangle, sizeof(StRectPrimitive)},
			   {StdrawCircle, Stcircle, sizeof(StArcPrimitive)},
			 {StdrawEllipse, Stellipse, sizeof(StArcPrimitive)},
			      {StdrawLine, Stline, sizeof(StLinePrimitive)},
				 {StdrawArc, Starc, sizeof(StArcPrimitive)},
		 {StdrawEllipticArc, StellipticArc, sizeof(StArcPrimitive)},
				 {StdrawPie, Stpie, sizeof(StArcPrimitive)},
		 {StdrawEllipticPie, StellipticPie, sizeof(StArcPrimitive)},
		      {StdrawPolyline, Stpolyline, sizeof(StLinePrimitive)},
			{StdrawPolygon, Stpolygon, sizeof(StLinePrimitive)},
		      {StdrawFreehand, Stfreehand, sizeof(StLinePrimitive)},
			      {StdrawText, Sttext, sizeof(StTextPrimitive)},
		{(StDrawFunction) - 1, Stbitmap, sizeof(StBitmapPrimitive)},
			      {(StDrawFunction) - 1, (StFigureType) - 1, 0},
};

static StFigureType
_GetType(menu)
	StDrawFunction  menu;
{
	FigureDescRec  *descp = figuredesc;

	while (descp->menu != (StDrawFunction) - 1) {
		if (descp->menu == menu)
			return descp->type;
		descp++;
	}
	return descp->type;
}

static StDrawFunction
_GetMenu(type)
	StFigureType    type;
{
	FigureDescRec  *descp = figuredesc;

	while (descp->type != (StFigureType) - 1) {
		if (descp->type == type)
			return descp->menu;
		descp++;
	}
	return descp->menu;
}

static int
_GetSize(type)
	StFigureType    type;
{
	FigureDescRec  *descp = figuredesc;

	while (descp->type != (StFigureType) - 1) {
		if (descp->type == type)
			return descp->size;
		descp++;
	}
	return descp->size;
}

static void
_CheckCoordLimit(w, xp, yp)
	Widget          w;
	long           *xp;
	long           *yp;
{
	StPictureWidget spw = (StPictureWidget) w;
	long            xmin = (long) (spw->stPicture.coor_xmin);
	long            xmax = (long) (spw->stPicture.coor_xmax);
	long            ymin = (long) (spw->stPicture.coor_ymin);
	long            ymax = (long) (spw->stPicture.coor_ymax);

	*xp = (*xp < xmin ? xmin : (*xp > xmax ? xmax : *xp));
	*yp = (*yp < ymin ? ymin : (*yp > ymax ? ymax : *yp));
}

static void
_CheckRegionLimit(w, region)
	Widget          w;
	StRegion       *region;
{
	StPictureWidget spw = (StPictureWidget) w;

	if (!IsRegion(w, region)) {
		if ((long) (region->x) + (long) (region->width)
		    > (long) (spw->stPicture.coor_xmax))
			region->x = spw->stPicture.coor_xmax - region->width - 1;
		if ((long) (region->y) - (long) (region->height)
		    < (long) (spw->stPicture.coor_ymin))
			region->y = spw->stPicture.coor_ymin + region->height + 1;
	}
}

static void
_AddDisp(w, source_x, source_y, disp_x, disp_y, rx, ry)
	Widget          w;
	short           source_x, source_y, disp_x, disp_y;
	short          *rx, *ry;
{
	long            long_x, long_y;

	/* range check */
	long_x = (long) source_x + (long) disp_x;
	long_y = (long) source_y - (long) disp_y;
	CheckCoordLimit(w, &long_x, &long_y);
	*rx = (short) long_x;
	*ry = (short) long_y;
}

static void
_ToGlobalXY(w, x, y, rx, ry)
	Widget          w;
	short           x, y;	/* Widget coordinate */
	short          *rx, *ry;/* Xlib coordinate */
{
	StPictureWidget spw = (StPictureWidget) w;
	long            orgx = spw->stPicture.orgin_x;
	long            orgy = spw->stPicture.orgin_y;
	long            viewx = spw->stPicture.view_x;
	long            viewy = spw->stPicture.view_y;
	long            gx, gy;

	gx = ((long) x - viewx) + orgx;
	gy = -((long) y - viewy) + (-orgy);
	*rx = (short) gx;
	*ry = (short) gy;
}

static void
_ToWidgetXY(w, x, y, rx, ry)
	Widget          w;
	short           x, y;	/* Xlib coordinate */
	short          *rx, *ry;/* Widget coordinate */
{
	StPictureWidget spw = (StPictureWidget) w;
	long            orgx = spw->stPicture.orgin_x;
	long            orgy = spw->stPicture.orgin_y;
	long            viewx = spw->stPicture.view_x;
	long            viewy = spw->stPicture.view_y;
	long            gx, gy;

	gx = ((long) x - orgx) + viewx;
	gy = -(((long) y - (-orgy)) - viewy);
	CheckCoordLimit(w, &gx, &gy);
	*rx = (short) gx;
	*ry = (short) gy;
}

static void
_ScaleObject(mul, div, x)
	unsigned short  mul, div;
	short          *x;
{
	long            xx;
	int             sign;

	xx = (long) (*x) * (long) mul;
	sign = (xx < 0) ? (-1) : 1;
	xx += sign * Abs(div) / 2;
	*x = (short) (xx / (long) div);
}

static Boolean
_NewObjNumber(w, figure)
	Widget          w;
	StFigure       *figure;
{
	StDrawControl  *dc = GetDrawControl(w);

	++(dc->n_group);
	*figure = ++(dc->figure_no);
	return SUCCESS;
}

static Boolean
_DelObjNumber(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);

	--(dc->n_group);
	return SUCCESS;
}

static StHashTbl *
_MakeHashTbl(hashsize)
	long            hashsize;
{
	return ((StHashTbl *) calloc((unsigned int) 1, sizeof(StHashTbl) * hashsize));
}

static StHashTbl *
AddHashEntry(next, figure, obj_ptr)
	StHashTbl      *next;
	StFigure        figure;
	StObject       *obj_ptr;
{
	StHashTbl      *new;

	if (next == (StHashTbl *) NULL) {
		new = (StHashTbl *) calloc((unsigned int) 1, sizeof(StHashTbl));
		new->figure = figure;
		new->addr = obj_ptr;
		return new;
	}
	next->succ = AddHashEntry(next->succ, figure, obj_ptr);
	return next;
}

static StObject *
SurchHashEntry(next, figure)
	StHashTbl      *next;
	StFigure        figure;
{
	StHashTbl      *new;

	if (next->figure == figure)
		return next->addr;
	return SurchHashEntry(next->succ, figure);
}

static StHashTbl *before;

static Boolean
DeleteHashEntry(next, figure)
	StHashTbl      *next;
	StFigure        figure;
{
	if (next->figure == figure) {
		if (next->succ == NULL) {
			if (before == NULL) {
				next->figure = StNullFigure;
				next->addr = NULL;
			} else {
				before->succ = NULL;
				free(next);
			}
		} else {
			for (;;) {
				if (next->succ->succ == NULL) {
					next->figure = next->succ->figure;
					next->addr = next->succ->addr;
					free(next->succ);
					next->succ = NULL;
					break;
				} else {
					next->figure = next->succ->figure;
					next->addr = next->succ->addr;
					next = next->succ;
				}
			}
		}
		before = NULL;
		return SUCCESS;
	} else {
		if (next->succ == NULL)
			return FAILURE;
		before = next;
		DeleteHashEntry(next->succ, figure);
	}
}

static Boolean
_DeleteObjAddr(w, figure)
	Widget          w;
	StFigure        figure;
{
	StDrawControl  *dc = GetDrawControl(w);
	int             index = figure % dc->hash_size;

	if (dc->hash_tbl[index].figure == StNullFigure)
		return FAILURE;

	return DeleteHashEntry(&(dc->hash_tbl[index]), figure);
}

static void
_SetObjAddr(w, figure, obj_ptr)
	Widget          w;
	StFigure        figure;
	StObject       *obj_ptr;
{
	StDrawControl  *dc = GetDrawControl(w);
	int             index = figure % dc->hash_size;

	if (dc->hash_tbl[index].addr != (StObject *) NULL)
		dc->hash_tbl[index].succ = AddHashEntry(dc->hash_tbl[index].succ,
							figure, obj_ptr);
	else {
		dc->hash_tbl[index].figure = figure;
		dc->hash_tbl[index].addr = obj_ptr;
	}
}

static StObject *
_GetObjAddr(w, figure)
	Widget          w;
	StFigure        figure;
{
	StDrawControl  *dc = GetDrawControl(w);
	int             index = figure % dc->hash_size;

	if (dc->hash_tbl[index].figure == figure) {
		return dc->hash_tbl[index].addr;
	}
	return (SurchHashEntry(dc->hash_tbl[index].succ, figure));
}

static Boolean
_IsInBox(x, y, xb, yb, xe, ye)
	short           x, y;	/* widget coordinates */
	short           xb, xe, yb, ye;	/* widget coordinates */
{
	return (xb <= x && x <= xe && ye <= y && y <= yb);
}

static Boolean
_IsCrossBox(dxb, dyb, dxe, dye, sxb, syb, sxe, sye)
	short           dxb, dyb, dxe, dye;	/* widget coordinates */
	short           sxb, syb, sxe, sye;	/* widget coordinates */
{
	return (dxb <= sxe && dxe >= sxb && dyb >= sye && dye <= syb);
}

static Boolean
_IsIncludeBox(dxb, dyb, dxe, dye, sxb, syb, sxe, sye)
	short           dxb, dyb, dxe, dye;	/* widget coordinates */
	short           sxb, syb, sxe, sye;	/* widget coordinates */
{
	return (IsInBox(sxb, syb, dxb, dyb, dxe, dye)
		&& IsInBox(sxe, sye, dxb, dyb, dxe, dye));
}

static void
_JoinBox(dxb, dyb, dxe, dye, sxb, syb, sxe, sye)
	short          *dxb, *dyb, *dxe, *dye;	/* widget coordinates */
	short           sxb, syb, sxe, sye;	/* widget coordinates */
{
	*dxb = Min(*dxb, sxb);
	*dxe = Max(*dxe, sxe);
	*dyb = Max(*dyb, syb);
	*dye = Min(*dye, sye);
}

static char    *
_NewInqBuf(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);

	if (dc->inq_buf != (char *) NULL) {
		free((char *) (((StPointList *) (dc->inq_buf))->points));
		free(dc->inq_buf);
		dc->inq_buf = (char *) NULL;
	}
	/* alloc fixed part */
	if ((dc->inq_buf = (char *) calloc((unsigned int) 1, sizeof(StPointList))) == (char *) NULL)
		return (char *) NULL;	/* nomore memory error */
	/* alloc entry part (dummy) */
	if ((((StPointList *) (dc->inq_buf))->points = (StPoint *) malloc(1)) == (StPoint *) NULL) {
		free(dc->inq_buf);
		dc->inq_buf = (char *) NULL;
		return (char *) NULL;
	}
	((StPointList *) (dc->inq_buf))->n_points = 0;

	return dc->inq_buf;
}

static Boolean
_SetFigureList(w, figure)
	Widget          w;
	StFigure        figure;
{
	StDrawControl  *dc = GetDrawControl(w);
	StFigureList   *fp = (StFigureList *) dc->inq_buf;
	StFigure       *fpf;

	fp->n_figures++;
	fpf = (StFigure *) realloc(fp->figures, sizeof(StFigure) * fp->n_figures);
	if (fpf == (StFigure *) NULL) {
		fp->n_figures--;
		return FAILURE;
	}
	fp->figures = fpf;
	fp->figures[fp->n_figures - 1] = figure;
	return SUCCESS;
}

static Boolean
_SetPointList(w, point)
	Widget          w;
	StPoint         point;
{
	StDrawControl  *dc = GetDrawControl(w);
	StPointList    *pp = (StPointList *) dc->inq_buf;
	StPoint        *ppp;

	pp->n_points++;
	ppp = (StPoint *) realloc(pp->points, sizeof(StPoint) * pp->n_points);
	if (ppp == (StPoint *) NULL) {
		pp->n_points--;
		return FAILURE;
	}
	pp->points = ppp;
	pp->points[pp->n_points - 1].x = point.x;
	pp->points[pp->n_points - 1].y = point.y;
	return SUCCESS;
}

static void
_FreeTergetFigures(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);

	free((char *) (dc->fig_buf->figures));
	free((char *) (dc->fig_buf));
	dc->fig_buf = (StFigureList *) NULL;
}

static void
_SetupTergetFigures(w, disend)
	Widget          w;
	Boolean         disend;
{
	/* CAUTION no error recover */
	StDrawControl  *dc = GetDrawControl(w);
	StFigureList   *flist;
	register        i, j;

	StPicGetSelectedFigure(w, &flist);
	dc->fig_buf = (StFigureList *) calloc((unsigned int) 1, sizeof(StFigureList));
	dc->fig_buf->n_figures = flist->n_figures;
	dc->fig_buf->figures = (StFigure *) malloc(sizeof(StFigure) * flist->n_figures);
	if (disend)
		for (i = 0; i < flist->n_figures; i++)
			dc->fig_buf->figures[i] = flist->figures[i];
	else
		for (i = 0, j = flist->n_figures - 1; i < flist->n_figures; i++, j--)
			dc->fig_buf->figures[i] = flist->figures[j];
}

static Boolean
_IsMoveAction(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StPoint         pos;
	StFigureList   *figure_list;
	register        i;

	dc->first_move = True;
	dc->old_disp_x = 0;
	dc->old_disp_y = 0;
	if (dc->n_selected > 0) {
		SetupTergetFigures(w, True);
		return True;
	}
	return False;
}

static Boolean
_IsResizeAction(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StPoint         pos;
	StFigureList   *figure_list;
	StFigureType    type;
	StFigure        fno;
	register        i;

	dc->first_move = True;
	dc->old_disp_x = 0;
	dc->old_disp_y = 0;
	if (dc->n_selected > 0) {
		ToWidgetXY(w, dc->fixx, dc->fixy, &(pos.x), &(pos.y));
		StPicGetFigure(w, &pos, &figure_list, StCoordWidget);
		if (figure_list) {
			for (i = 0; i < figure_list->n_figures; i++) {
				if (IsSelectedFigure(w, figure_list->figures[i])) {
					fno = figure_list->figures[i];
					SetupTergetFigures(w, True);	/* get fno list buffer
									 * for  */
					/* dummy and set terget fno */
					/* into [0]		     */
					dc->fig_buf->figures[0] = fno;
					if ((type = StPicGetFigureType(w, dc->fig_buf->figures[0]))
					    == Sttext || type == Stbitmap)
						break;
					else if ((dc->select = FindHandle(w, pos.x, pos.y,
						   dc->fig_buf->figures[0]))
						 == SLNONE)
						break;
					else {
						SetResizeDir(dc->select);
						TgleSelect(w, dc->fig_buf->figures[0]);
						return True;
					}
				}
			}
		}
	}
	return False;
}

static void
_Memcpy(dest, src, len)
	char           *dest;
	char           *src;
	int             len;
{
	while (len--)
		*dest++ = *src++;
}

static void
_SavePoint(w, event)
	Widget          w;
	XEvent         *event;
{
	StDrawControl  *dc = GetDrawControl(w);

	dc->fixx = event->xbutton.x;
	dc->fixy = event->xbutton.y;
}

static void
_StuffPoint(image, x, y)
	StDragImage    *image;
	short           x, y;	/* Xlib coordinate */
{
	image->npoints++;
	image->points = (XPoint *)
		realloc(image->points, sizeof(XPoint) * image->npoints);
	image->points[image->npoints - 1].x = x;
	image->points[image->npoints - 1].y = y;
}

static StDrawControl *
_GetDrawControl(w)
	Widget          w;
{
	StPictureWidget spw = (StPictureWidget) w;

	return spw->stPicture.dt;
}

static void
_SetDrawStat(w, stat)
	Widget          w;
	StPicDrawStat   stat;
{
	StDrawControl  *dc = GetDrawControl(w);

	dc->drawstat = stat;
}

static void
_SaveOldDrawStat(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);

	dc->oldstat = dc->drawstat;
}

static void
_RestorOldDrawStat(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);

	dc->drawstat = dc->oldstat;
}

static void
SetBar(w, top, length, total)
	Widget          w;
	Dimension       top;
	Dimension       length, total;
{
	XtScrollBarSetThumb(w, (float) top / total, (float) length / total);
}

static void
RedrawThumbs(w, widget)
	StPictureWidget w;
	Widget          widget;
{
	if (widget == w->stPicture.sbhor)
		SetBar(widget, w->stPicture.view_x - w->stPicture.coor_xmin,
		       w->stPicture.view_w, w->stPicture.coor_xmax - w->stPicture.coor_xmin);

	if (widget == w->stPicture.sbver)
		SetBar(widget, w->stPicture.view_y - w->stPicture.coor_ymin,
		       w->stPicture.view_h, w->stPicture.coor_ymax - w->stPicture.coor_ymin);
}

static void
_MoveView(w, widget, move_x, move_y)
	StPictureWidget w;
	Widget          widget;
	int             move_x, move_y;
{
	int             src_x, src_y;
	int             dest_x, dest_y;
	unsigned int    width, height;
	StRegion        region, subreg;
	StDrawControl  *dc;

	if (move_x == 0 && move_y == 0)
		return;

	StPicSetViewArea(w, move_x, -move_y);

	if (widget)
		RedrawThumbs(w, widget);

	if (move_x > -(int) w->stPicture.view_w
	    && move_x < (int) w->stPicture.view_w
	    && move_y > -(int) w->stPicture.view_h
	    && move_y < (int) w->stPicture.view_h) {

		/* partially reusable ( need not to be redrawn ) */

		if (move_x < 0) {
			src_x = (!w->stPicture.useright) * (w->core.width - w->stPicture.view_w);
			dest_x = src_x - move_x;
			width = w->stPicture.view_w + move_x;
			region.x = src_x;
			subreg.x = region.x + region.width;
		} else {	/* move_x >= 0 */
			dest_x = (!w->stPicture.useright) * (w->core.width - w->stPicture.view_w);
			src_x = dest_x + move_x;
			width = w->stPicture.view_w - move_x;
			region.x = dest_x + width;
			subreg.x = dest_x;
		}
		if (move_y < 0) {
			src_y = (!w->stPicture.usebottom) * (w->core.height - w->stPicture.view_h);
			dest_y = src_y - move_y;
			height = w->stPicture.view_h + move_y;
			subreg.y = src_y;
		} else {	/* move_y >= 0 */
			dest_y = (!w->stPicture.usebottom) * (w->core.height - w->stPicture.view_h);
			src_y = dest_y + move_y;
			height = w->stPicture.view_h - move_y;
			subreg.y = dest_y + height;
		}
		region.y = (!w->stPicture.usebottom) * (w->core.height - w->stPicture.view_h);
		region.width = w->stPicture.view_w - width;
		region.height = w->stPicture.view_h;
		subreg.width = width;
		subreg.height = w->stPicture.view_h - height;

		dc = GetDrawControl(w);

		XCopyArea(dc->dpy, dc->window, dc->window, DefaultGC(dc->dpy, DefaultScreen(dc->dpy)), src_x, src_y, width, height, dest_x, dest_y);

		if (region.width && region.height) {
			StPicRedrawPart(w, &region, StCoordEvent);
		}
		if (subreg.width && subreg.height) {
			StPicRedrawPart(w, &subreg, StCoordEvent);
		}
	} else {
		/* the entire window has to be redrawn */
		StPicRedrawAll(w);
	}
}
