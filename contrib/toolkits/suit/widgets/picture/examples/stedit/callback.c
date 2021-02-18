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

#include "stedit.h"
#define BOXWIDTH 57
#define BOXHEIGHT 30
#define ANGLE 5760

static StFigureList fig_stract;

int Isnotselectedfigure(figure)
StFigure figure;
{
	int i;

	for (i = 0; i < fig_stract.n_figures; i++) {
		if (figure == fig_stract.figures[i]) {
			return(0);
		}
	}
	return(1);
}

void Setgroupready()
{
	StPicGroupFigure(pic1, &fig_stract);
	free(fig_stract.figures);
	fig_stract.n_figures = 0;
}

void Unsetfigure()
{
	StFigureList *fig_list;
	int i;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list) {
		for (i = 0;i < fig_list->n_figures; i++) {
			StPicSelectFigure(pic1, fig_list->figures[i],
					StunsetSelect, StunsetHandle);
		}
	}
}

void PSetgroup(w, non1, non2)
Widget w;
Widget non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	fig_stract.figures = (StFigure *) calloc((unsigned)1,
			sizeof(StFigure));
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = setgroup;
}

void PUnsetgroup(w, non1, non2)
Widget w;
Widget non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = unsetgroup;
}

void PTop(w, non1, non2)
Widget w;
Widget non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = top;
}

void PBottom(w, non1, non2)
Widget w;
Widget non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = bottom;
}

void PResize(w, non1, non2)
Widget w;
caddr_t non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = resize;
}

void PMove(w, non1, non2)
Widget w;
caddr_t non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = move;
}

void PCopy(w, non1, non2)
Widget w;
caddr_t non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = copy;
}

void PToptobottom(w, non1, non2)
Widget w;
caddr_t non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = toptobottom;
}

void PLefttoright(w, non1, non2)
Widget w;
caddr_t non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = lefttoright;
}

void PRotate(w, non1, non2)
Widget w;
caddr_t non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = rotate;
}

void PAlldelete(w, non1, non2)
Widget w;
Widget non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	StPicReinitialize(pic1);
	StPicRedrawAll(pic1);
	mode = icon;
}

void POnedelete(w, non1, non2)
Widget w;
Widget non1;
caddr_t non2;
{
	COUNT = 0;
	if (mode == setgroup)
		Setgroupready();
	StPicBreakDraw(pic1);
	Unsetfigure();
	mode = onedelete;
}

void Sizecalc(fig_region)
StRegion *fig_region;
{
		fig_region->x -= 22;
		fig_region->y += 22;
		fig_region->width += 45;
		fig_region->height += 45;
}

void Setgroup()
{
	StFigureList *fig_list;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list && Isnotselectedfigure(fig_list->figures[0])) {
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
		fig_stract.n_figures += 1;
		fig_stract.figures = (StFigure *)
				realloc(fig_stract.figures,
				sizeof(StFigure) * fig_stract.n_figures);
		fig_stract.figures[fig_stract.n_figures - 1]
				= fig_list->figures[0];
	}
	fig_list = NULL;
}

void Unsetgroup()
{
	StFigureList *fig_list;
	StFigureList *ret_list;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list) {
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
		StPicUngroupFigure(pic1, fig_list->figures[0], &ret_list);
	}
	mode = icon;
}

void Top()
{
	StFigureList *fig_list;
	StRegion fig_region;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list) {
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
		StPicChangePriority(pic1, fig_list->figures[0], SttopPriority);
		StPicGetFigureRegion(pic1, fig_list->figures[0],
				&fig_region, StCoordWidget);
		Sizecalc(&fig_region);
		StPicRedrawPart(pic1, &fig_region, StCoordWidget);
	}
	mode = icon;
}

void Bottom()
{
	StFigureList *fig_list;
	StRegion fig_region;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list) {
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
		StPicChangePriority(pic1,
				fig_list->figures[0], StbottomPriority);
		StPicGetFigureRegion(pic1, fig_list->figures[0],
				&fig_region, StCoordWidget);
		Sizecalc(&fig_region);
		StPicRedrawPart(pic1, &fig_region, StCoordWidget);
	}
	mode = icon;
}

void Resize()
{
	static StFigureList *fig_list;
	static StFigure fig_old;
	StRegion fig_region;
	StPoint point_ret;
	StPoint fig_point;
	short width, height;

	if (COUNT == 0) {
		StPicGetSelectedFigure(pic1, &fig_list);
		if(fig_list) {
			fig_old = fig_list->figures[0];
			fig_list = NULL;
			COUNT = 1;
		}
	} else {
		StPicGetSelectedFigure(pic1, &fig_list);
		if (fig_list)
			StPicSelectFigure(pic1, fig_list->figures[0],
					StunsetSelect, StunsetHandle);
		StPicGetMouseLocation(pic1, &point_ret, StCoordWidget);
		StPicGetFigureRegion(pic1, fig_old,
				&fig_region, StCoordWidget);
		fig_point.x = fig_region.x;
		fig_point.y = fig_region.y;
		width = point_ret.x - fig_region.x;
		if (width < 0) {
			width = width * -1;
			fig_point.x = fig_region.x - width;
		}
		height = fig_region.y - point_ret.y;
		if (height < 0) {
			height = height * -1;
			fig_point.y = fig_region.y + height;
		}
		if (fig_region.width < point_ret.x - fig_region.x)
			fig_region.width = point_ret.x - fig_region.x;
		if (fig_region.height < fig_region.y - point_ret.y)
			fig_region.height = fig_region.y - point_ret.y;
		StPicResizeFigure(pic1, fig_old, width, height);
		StPicMoveFigure(pic1, fig_old, &fig_point, StCoordWidget);
		Sizecalc(&fig_region);
		StPicRedrawPart(pic1, &fig_region, StCoordWidget);
		StPicGetFigureRegion(pic1, fig_old,
				&fig_region, StCoordWidget);
		Sizecalc(&fig_region);
		StPicRedrawPart(pic1, &fig_region, StCoordWidget);
		StPicSelectFigure(pic1, fig_old,
				StunsetSelect, StunsetHandle);
		fig_list = NULL;
		COUNT = 0;
		mode = icon;
	}
}

void Move()
{
	static StFigureList fig_stract;
	static StFigureList *fig_list;
	static StFigure fig_old;
	static StPoint point_ret[2];
	static StRegion fig_region;
	static StRegion redrawregion;
	static StPoint move_value;
	static StPoint new_origin;
	int i;

	StPicGetMouseLocation(pic1, &point_ret[COUNT], StCoordWidget);
	if (COUNT == 0) {
		StPicGetSelectedFigure(pic1, &fig_list);
		if (fig_list) {
			fig_old = fig_list->figures[0];
			COUNT = 1;
			fig_list = NULL;
		}
	} else {
		StPicGetSelectedFigure(pic1, &fig_list);
		if (fig_list)
			StPicSelectFigure(pic1, fig_list->figures[0],
					StunsetSelect, StunsetHandle);
		StPicGetFigureRegion(pic1, fig_old,
				&fig_region, StCoordWidget);
		new_origin.x = fig_region.x - (point_ret[0].x - point_ret[1].x);
		new_origin.y = fig_region.y - (point_ret[0].y - point_ret[1].y);
		StPicMoveFigure(pic1, fig_old,
				&new_origin, StCoordWidget);
		Sizecalc(&fig_region);
		StPicRedrawPart(pic1, &fig_region, StCoordWidget);
		StPicGetFigureRegion(pic1, fig_old,
				&fig_region, StCoordWidget);
		Sizecalc(&fig_region);
		StPicRedrawPart(pic1, &fig_region, StCoordWidget);
		StPicSelectFigure(pic1, fig_old,
				StsetSelect, StsetHandle);
		fig_list = NULL;
		COUNT = 0;
		mode = icon;
	}
}

void Copy()
{
	static StFigureList *fig_list;
	static StFigure fig_old;
	static StFigure fig_new;
	static StRegion old_origin;
	static StPoint dest_p[2];
	static StPoint move_value;
	static StPoint new_origin;
	int i;

	StPicGetMouseLocation(pic1, &dest_p[COUNT], StCoordWidget);
	if (COUNT == 0) {
		StPicGetSelectedFigure(pic1, &fig_list);
		if (fig_list) {
			fig_old = fig_list->figures[0];
			COUNT = 1;
		}
	} else {
		StPicGetSelectedFigure(pic1, &fig_list);
		if (fig_list) {
			StPicSelectFigure(pic1, fig_list->figures[0],
					StunsetSelect, StunsetHandle);
			fig_list = NULL;
		}
		StPicGetFigureRegion(pic1, fig_old,
				&old_origin, StCoordWidget);
		new_origin.x = old_origin.x - (dest_p[0].x - dest_p[1].x);
		new_origin.y = old_origin.y - (dest_p[0].y - dest_p[1].y);
		fig_new = StPicCopyFigure(pic1, fig_old,
				&new_origin, StCoordWidget);
		StPicDrawFigure(pic1, fig_new);
		StPicSelectFigure(pic1, fig_old,
				StunsetSelect, StunsetHandle);
		StPicSelectFigure(pic1, fig_new,
				StsetSelect, StsetHandle);
		fig_list = NULL;
		COUNT = 0;
		mode = icon;
	}
}

void Toptobottom()
{
	StFigureList *fig_list;
	StRegion region;
	int i;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list) {
		StPicMirrorFigure(pic1, fig_list->figures[0],
				SthorizontalMirror);
		StPicGetFigureRegion(pic1, fig_list->figures[0],
				&region, StCoordWidget);
		Sizecalc(&region);
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
		StPicRedrawPart(pic1, &region, StCoordWidget);
		StPicSelectFigure(pic1, fig_list->figures[0],
				StsetSelect, StsetHandle);
		mode = icon;
	}
}

void Lefttoright()
{
	StFigureList *fig_list;
	StRegion region;
	int i;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list) {
		StPicMirrorFigure(pic1, fig_list->figures[0],
				StverticalMirror);
		StPicGetFigureRegion(pic1, fig_list->figures[0],
				&region, StCoordWidget);
		Sizecalc(&region);
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
		StPicRedrawPart(pic1, &region, StCoordWidget);
		StPicSelectFigure(pic1, fig_list->figures[0],
				StsetSelect, StsetHandle);
		mode = icon;
	}
}

void Rotate()
{
	StFigureList *fig_list;
	static StRegion region;
	int i;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list) {
		StPicGetFigureRegion(pic1, fig_list->figures[0],
				&region, StCoordWidget);
		StPicRotateFigure(pic1, fig_list->figures[0], ANGLE);
		Sizecalc(&region);
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
		StPicRedrawPart(pic1, &region, StCoordWidget);
		StPicGetFigureRegion(pic1, fig_list->figures[0],
				&region, StCoordWidget);
		Sizecalc(&region);
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
		StPicRedrawPart(pic1, &region, StCoordWidget);
		StPicSelectFigure(pic1, fig_list->figures[0],
				StsetSelect, StsetHandle);
		mode = icon;
	}
}

void Deleteone()
{
        StFigureList *fig_list;
        static StRegion region;

	StPicGetSelectedFigure(pic1, &fig_list);
	if (fig_list) {
		StPicSelectFigure(pic1, fig_list->figures[0],
				StunsetSelect, StunsetHandle);
       		StPicGetFigureRegion(pic1, fig_list->figures[0],
				&region, StCoordWidget);
		Sizecalc(&region);
       		StPicDeleteFigure(pic1, fig_list->figures[0]);
       		StPicRedrawPart(pic1, &region, StCoordWidget);
		mode = icon;
	}
}

void RectC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Strectangle);
}

void CirC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Stcircle);
}

void EllipsC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Stellipse);
}

void LineC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Stline);
}

void ArcC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Starc);
}

void EllipsarcC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, StellipticArc);
}

void ArcpieC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Stpie);
}

void EllipspieC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, StellipticPie);
}

void PlineC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Stpolyline);
}

void PolygonC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Stpolygon);
}

void FlineC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Stfreehand);
}

void StringC(w, wa, non2)
Widget w;
Widget wa;
caddr_t non2;
{
	if (mode == setgroup)
		Setgroupready();
	mode = icon;
	Unsetfigure();
	StPicSetFigureType(pic1, Sttext);
}

void command_clbk()
{
	switch ((int)mode) {
		case icon:
			break;
		case setgroup:
			Setgroup();
			break;
		case unsetgroup:
			Unsetgroup();
			break;
		case top:
			Top();
			break;
		case bottom:
			Bottom();
			break;
		case resize:
			Resize();
			break;
		case move:
			Move();
			break;
		case copy:
			Copy();
			break;
		case onedelete:
			Deleteone();
			break;
		case toptobottom:
			Toptobottom();
			break;
		case lefttoright:
			Lefttoright();
			break;
		case rotate:
			Rotate();
			break;
		default:
			break;
	}
}
