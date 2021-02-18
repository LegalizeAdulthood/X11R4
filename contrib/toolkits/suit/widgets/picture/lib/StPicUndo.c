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

#define FAILURE -1
#define SUCCESS 0

static void     _FreeUndoPacket();
static void     _NewUndoPacket();
static void     _NewUndoObjects();
static void     _RealDeleteObjects();
static void     _SetUndoCopy();
static void     _SetUndoMove();
static void     _SetUndoResize();
static void     _SetUndoDraw();
static void     _SetUndoPrio();
static void     _SetUndoMirror();
static void     _SetUndoRotate();
static void     _SetUndoDelete();
static void     _DoUndoMove();
static void     _DoUndoResize();
static void     _DoUndoPrio();
static void     _DoUndoMirror();
static void     _DoUndoRotate();
static void     _DoUndoDelete();
static void     _DoUndoCreate();
static void     _UndoObject();

StUndoFunctionDescRec undofunctiondesc = {
					  _FreeUndoPacket,
					  _NewUndoPacket,
					  _NewUndoObjects,
					  _RealDeleteObjects,
					  _SetUndoCopy,
					  _SetUndoMove,
					  _SetUndoResize,
					  _SetUndoDraw,
					  _SetUndoPrio,
					  _SetUndoMirror,
					  _SetUndoRotate,
					  _SetUndoDelete,
					  _DoUndoMove,
					  _DoUndoResize,
					  _DoUndoPrio,
					  _DoUndoMirror,
					  _DoUndoRotate,
					  _DoUndoDelete,
					  _DoUndoCreate,
					  _UndoObject
};

typedef struct {
	StSetPriority   priority;	/* 0 top 1 buttom */
	int             n_fig;
	StFigure       *flst;
}               UndoPrio;

typedef struct {
	StSetMirror     mirror;
	int             n_fig;
	StFigure       *flst;
}               UndoMirror;

typedef struct {
	int             angle;
	int             n_fig;
	StFigure       *flst;
}               UndoRotate;

typedef struct {
	int             n_fig;
	StFigure       *flst;
}               UndoFList;

typedef struct {
	int             n_fig;
	StFigure       *flst;
	StRegion       *oldloc;
}               UndoMove;

typedef struct {
	int             n_fig;
	StFigure       *flst;
	StRegion       *oldloc;
}               UndoSize;

typedef struct {
	UndoOperation   opcode;
	union {
		UndoPrio        undo_prio;
		UndoMirror      undo_mirror;
		UndoRotate      undo_rotate;
		UndoFList       undo_delete;
		UndoFList       undo_copy;
		UndoMove        undo_move;
		UndoSize        undo_size;
		UndoFList       undo_ucreate;
		UndoFList       undo_uprio;
	}               prm;
}               UndoPacket;

static UndoPacket undo_packet;

static void
SetFnumber(fl, no, fno)
	StFigure       **fl;
	int             no;
	StFigure        fno;
{
	*fl = (StFigure *) realloc(*fl, sizeof(StFigure) * (no + 1));
	(*fl)[no] = fno;
}

static void
SetRegion(loc, no, x, y, w, h)
	StRegion       **loc;
	int             no;
	short           x, y, w, h;
{
	*loc = (StRegion *) realloc(*loc, sizeof(StRegion) * (no + 1));
	(*loc)[no].x = x;
	(*loc)[no].y = y;
	(*loc)[no].width = w;
	(*loc)[no].height = h;
}

static void
UndoJoinSelected(w, fn, fp, xb, yb, xe, ye)
	Widget          w;
	int             fn;
	StFigure       *fp;
	short          *xb, *yb, *xe, *ye;
{
	StObject       *obj;
	short           ixb, iyb, ixe, iye;
	register        i;

	for (i = 0; i < fn; i++) {
		obj = GetObjAddr(w, fp[i]);
		ixb = obj->body.p_group.obj_pos.x;
		iyb = obj->body.p_group.obj_pos.y;
		ixe = ixb + obj->body.p_group.w;
		iye = iyb - obj->body.p_group.h;
		JoinBox(xb, yb, xe, ye, ixb, iyb, ixe, iye);
	}
}

static Boolean
StuffSuccFigure(w, fno)
	Widget          w;
	StFigure        fno;
{
	StDrawControl  *dc = GetDrawControl(w);
	StFigureList   *fp = &(dc->undo_succ_figs);
	StFigure       *fpf;

	fp->n_figures++;
	fpf = (StFigure *) realloc(fp->figures, sizeof(StFigure) * fp->n_figures);
	if (fpf == (StFigure *) NULL) {
		fp->n_figures--;
		return FAILURE;
	}
	fp->figures = fpf;
	fp->figures[fp->n_figures - 1] = fno;
	return SUCCESS;
}

static void
LinkUndoChain(w, fno)
	Widget          w;
	StFigure        fno;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj = GetObjAddr(w, fno);

	StuffSuccFigure(w,
		  (obj->body.p_group.succ ? (obj->body.p_group.succ)->obj_no
		   : StNullFigure));
	BreakObject(&(dc->first_grp), &(dc->last_grp),
		    obj->body.p_group.pred, obj->body.p_group.succ);
	LinkObject(&(dc->first_undo), &(dc->last_undo), (StFigure *) NULL, obj);
}

static void
RechainUndoChain(w, obj, succ_fno)
	Widget          w;
	StObject       *obj;
	StFigure        succ_fno;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *succ_obj = (StObject *) NULL;

	if (succ_fno != StNullFigure)
		succ_obj = GetObjAddr(w, succ_fno);
	BreakObject(&(dc->first_undo), &(dc->last_undo),
		    obj->body.p_group.pred, obj->body.p_group.succ);
	LinkObject(&(dc->first_grp), &(dc->last_grp), succ_obj, obj);
}

static void
_RealDeleteObjects(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj;
	StObject       *side_ptr;

	if (dc->undo_succ_figs.figures)
		free((char *) dc->undo_succ_figs.figures);
	dc->undo_succ_figs.figures = (StFigure *) NULL;
	dc->undo_succ_figs.n_figures = 0;

	obj = dc->first_undo;
	while (obj) {
		side_ptr = obj->body.p_group.succ;
		BreakObject(&(dc->first_undo), &(dc->last_undo),
			    obj->body.p_group.pred, obj->body.p_group.succ);
		LinkObject(&(dc->first_grp), &(dc->last_grp), (StObject *) NULL, obj);
		StPicDeleteFigure(w, obj->obj_no);
		obj = side_ptr;
	}
	dc->first_undo = (StObject *) NULL;
	dc->last_undo = (StObject *) NULL;
}

static void
_FreeUndoPacket(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) undo_packet.opcode) {
	case UPrio:
	case UUprio:
		if (dc->undo_succ_figs.figures)
			free((char *) dc->undo_succ_figs.figures);
		dc->undo_succ_figs.figures = (StFigure *) NULL;
		dc->undo_succ_figs.n_figures = 0;
		if (undo_packet.prm.undo_prio.flst != (StFigure *) NULL)
			free((char *) undo_packet.prm.undo_prio.flst);
		undo_packet.prm.undo_prio.n_fig = 0;
		undo_packet.prm.undo_prio.flst = (StFigure *) NULL;
		undo_packet.prm.undo_prio.priority = (StSetPriority) NULL;
		break;
	case UMirror:
		if (undo_packet.prm.undo_mirror.flst != (StFigure *) NULL)
			free((char *) undo_packet.prm.undo_mirror.flst);
		undo_packet.prm.undo_mirror.n_fig = 0;
		undo_packet.prm.undo_mirror.flst = (StFigure *) NULL;
		undo_packet.prm.undo_mirror.mirror = (StSetMirror) NULL;
		break;
	case URotate:
		if (undo_packet.prm.undo_rotate.flst != (StFigure *) NULL)
			free((char *) undo_packet.prm.undo_rotate.flst);
		undo_packet.prm.undo_rotate.n_fig = 0;
		undo_packet.prm.undo_rotate.flst = (StFigure *) NULL;
		undo_packet.prm.undo_rotate.angle = 0;
		break;
	case UDelete:
		if (undo_packet.prm.undo_delete.flst != (StFigure *) NULL)
			free((char *) undo_packet.prm.undo_delete.flst);
		undo_packet.prm.undo_delete.n_fig = 0;
		undo_packet.prm.undo_delete.flst = (StFigure *) NULL;
		break;
	case UMove:
		if (undo_packet.prm.undo_move.flst != (StFigure *) NULL)
			free((char *) undo_packet.prm.undo_move.flst);
		if (undo_packet.prm.undo_move.oldloc != (StRegion *) NULL)
			free((char *) undo_packet.prm.undo_move.oldloc);
		undo_packet.prm.undo_move.n_fig = 0;
		undo_packet.prm.undo_move.flst = (StFigure *) NULL;
		undo_packet.prm.undo_move.oldloc = (StRegion *) NULL;
		break;
	case UResize:
		if (undo_packet.prm.undo_size.flst != (StFigure *) NULL)
			free((char *) undo_packet.prm.undo_size.flst);
		if (undo_packet.prm.undo_size.oldloc != (StRegion *) NULL)
			free((char *) undo_packet.prm.undo_size.oldloc);
		undo_packet.prm.undo_size.n_fig = 0;
		undo_packet.prm.undo_size.flst = (StFigure *) NULL;
		undo_packet.prm.undo_size.oldloc = (StRegion *) NULL;
		break;
	case UUcreate:
		if (undo_packet.prm.undo_ucreate.flst != (StFigure *) NULL)
			free((char *) undo_packet.prm.undo_ucreate.flst);
		undo_packet.prm.undo_ucreate.n_fig = 0;
		undo_packet.prm.undo_ucreate.flst = (StFigure *) NULL;
		break;
	default:
		;
	}
	undo_packet.opcode = UNull;
}

static void
_NewUndoPacket(op)
	UndoOperation   op;
{
	switch ((int) op) {
	case UDraw:
	case UDelete:
	case UCopy:
		undo_packet.opcode = UDelete;
		undo_packet.prm.undo_delete.n_fig = 0;
		undo_packet.prm.undo_delete.flst
			= (StFigure *) malloc(1);
		break;
	case UPrio:
		undo_packet.opcode = UPrio;
		undo_packet.prm.undo_prio.n_fig = 0;
		undo_packet.prm.undo_prio.flst
			= (StFigure *) malloc(1);
		break;
	case UMirror:
		undo_packet.opcode = UMirror;
		undo_packet.prm.undo_mirror.n_fig = 0;
		undo_packet.prm.undo_mirror.flst
			= (StFigure *) malloc(1);
		break;
	case URotate:
		undo_packet.opcode = URotate;
		undo_packet.prm.undo_rotate.n_fig = 0;
		undo_packet.prm.undo_rotate.flst
			= (StFigure *) malloc(1);
		break;
	case UMove:
		undo_packet.opcode = UMove;
		undo_packet.prm.undo_move.n_fig = 0;
		undo_packet.prm.undo_move.flst
			= (StFigure *) malloc(1);
		undo_packet.prm.undo_move.oldloc
			= (StRegion *) malloc(1);
		break;
	case UResize:
		undo_packet.opcode = UResize;
		undo_packet.prm.undo_size.n_fig = 0;
		undo_packet.prm.undo_size.flst
			= (StFigure *) malloc(1);
		undo_packet.prm.undo_size.oldloc
			= (StRegion *) malloc(1);
		break;
	default:
		;
	}
}

static void
_NewUndoObjects(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);

	if (dc->undo_succ_figs.figures)
		free((char *) dc->undo_succ_figs.figures);
	dc->undo_succ_figs.figures = (StFigure *) malloc(1);
	dc->undo_succ_figs.n_figures = 0;

	dc->first_undo = (StObject *) NULL;
	dc->last_undo = (StObject *) NULL;
}

static void
_SetUndoCopy(fno)
	StFigure        fno;
{
	SetFnumber(&(undo_packet.prm.undo_delete.flst),
		   undo_packet.prm.undo_delete.n_fig, fno);
	undo_packet.prm.undo_delete.n_fig++;
}

static void
_SetUndoMove(fno, org_x, org_y)
	StFigure        fno;
	short           org_x;
	short           org_y;
{
	SetFnumber(&(undo_packet.prm.undo_move.flst),
		   undo_packet.prm.undo_move.n_fig, fno);
	SetRegion(&(undo_packet.prm.undo_move.oldloc),
		  undo_packet.prm.undo_move.n_fig,
		  org_x, org_y, 0, 0);
	undo_packet.prm.undo_move.n_fig++;
}

static void
_SetUndoResize(fno, org_x, org_y, org_w, org_h)
	StFigure        fno;
	short           org_x, org_y, org_w, org_h;
{
	SetFnumber(&(undo_packet.prm.undo_size.flst),
		   undo_packet.prm.undo_size.n_fig, fno);
	SetRegion(&(undo_packet.prm.undo_size.oldloc),
		  undo_packet.prm.undo_size.n_fig,
		  org_x, org_y, org_w, org_h);
	undo_packet.prm.undo_size.n_fig++;
}

static void
_SetUndoDraw(fno)
	StFigure        fno;
{
	SetFnumber(&(undo_packet.prm.undo_delete.flst),
		   undo_packet.prm.undo_delete.n_fig, fno);
	undo_packet.prm.undo_delete.n_fig++;
}

static void
_SetUndoPrio(w, fno, priority)
	Widget          w;
	StFigure        fno;
	StSetPriority   priority;
{
	StObject       *obj = GetObjAddr(w, fno);

	SetFnumber(&(undo_packet.prm.undo_prio.flst),
		   undo_packet.prm.undo_prio.n_fig, fno);
	StuffSuccFigure(w,
		  (obj->body.p_group.succ ? (obj->body.p_group.succ)->obj_no
		   : StNullFigure));
	undo_packet.prm.undo_prio.priority = priority;
	undo_packet.prm.undo_prio.n_fig++;
}

static void
_SetUndoMirror(fno, mirror)
	StFigure        fno;
	StSetMirror     mirror;
{
	SetFnumber(&(undo_packet.prm.undo_mirror.flst),
		   undo_packet.prm.undo_mirror.n_fig, fno);
	undo_packet.prm.undo_mirror.mirror = mirror;
	undo_packet.prm.undo_mirror.n_fig++;
}

static void
_SetUndoRotate(fno, angle)
	StFigure        fno;
	int             angle;
{
	SetFnumber(&(undo_packet.prm.undo_rotate.flst),
		   undo_packet.prm.undo_rotate.n_fig, fno);
	undo_packet.prm.undo_rotate.angle = angle;
	undo_packet.prm.undo_rotate.n_fig++;
}

static void
_SetUndoDelete(fno)
	StFigure        fno;
{
	SetFnumber(&(undo_packet.prm.undo_delete.flst),
		   undo_packet.prm.undo_delete.n_fig, fno);
	undo_packet.prm.undo_delete.n_fig++;
}

static void
_DoUndoMove(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	short           dx, dy, src_x, src_y, dst_x, dst_y;
	StObject       *obj;
	register        i;

	dc->fig_buf = (StFigureList *) calloc((unsigned int) 1, sizeof(StFigureList));
	dc->fig_buf->n_figures = undo_packet.prm.undo_move.n_fig;
	dc->fig_buf->figures = (StFigure *) malloc(
				 sizeof(StFigure) * dc->fig_buf->n_figures);
	for (i = 0; i < dc->fig_buf->n_figures; i++)
		dc->fig_buf->figures[i] = undo_packet.prm.undo_move.flst[i];
	obj = GetObjAddr(w, undo_packet.prm.undo_move.flst[0]);
	src_x = obj->body.p_group.obj_pos.x;
	src_y = obj->body.p_group.obj_pos.y;
	dst_x = undo_packet.prm.undo_move.oldloc[0].x;
	dst_y = undo_packet.prm.undo_move.oldloc[0].y;
	ToGlobalXY(w, src_x, src_y, &src_x, &src_y);
	ToGlobalXY(w, dst_x, dst_y, &dst_x, &dst_y);
	dx = dst_x - src_x;
	dy = dst_y - src_y;
	MoveObject(w, dx, dy);
	FreeTergetFigures(w);
}

static void
_DoUndoResize(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj;
	short           sxb, syb, sxe, sye, dxb, dyb, dxe, dye;	/* widget coord */
	short           x1, y1, x2, y2;	/* Xlib coord */
	unsigned short  s_width, s_height, d_width, d_height;
	StPoint         pos;
	StRegion        rgn;
	StFigureList   *flist;
	StFigure        fno = undo_packet.prm.undo_size.flst[0];
	register        i;

	obj = GetObjAddr(w, fno);
	sxb = obj->body.p_group.obj_pos.x;
	syb = obj->body.p_group.obj_pos.y;
	sxe = sxb + obj->body.p_group.w;
	sye = syb - obj->body.p_group.h;
	s_width = obj->body.p_group.w;
	s_height = obj->body.p_group.h;

	dxb = undo_packet.prm.undo_size.oldloc[0].x;
	dyb = undo_packet.prm.undo_size.oldloc[0].y;
	d_width = undo_packet.prm.undo_size.oldloc[0].width;
	d_height = undo_packet.prm.undo_size.oldloc[0].height;
	dxe = dxb + d_width;
	dye = dyb - d_height;

	StPicGetSelectedFigure(w, &flist);	/* get fno list of current
						 * selected fig */
	SelectAll(w, StunsetSelect);	/* erase old handle */
	StPicResizeFigure(w, fno, d_width, d_height);
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
	else
		TgleSelect(w, fno);

	FreeUndoPacket(w);
	NewUndoPacket(UResize);
	SetUndoResize(fno, sxb, syb, s_width, s_height);
}

static void
UndoChangePrior(w, priority)
	Widget          w;
	StSetPriority   priority;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj;
	StObject       *succ_obj;
	register        i;

	if (priority == StbottomPriority) {
		for (i = 0; i < undo_packet.prm.undo_prio.n_fig; i++) {
			obj = GetObjAddr(w, undo_packet.prm.undo_prio.flst[i]);
			BreakObject(&(dc->first_grp), &(dc->last_grp),
			    obj->body.p_group.pred, obj->body.p_group.succ);
			if (dc->undo_succ_figs.figures[i] == StNullFigure)
				succ_obj = (StObject *) NULL;
			else
				succ_obj = GetObjAddr(w, dc->undo_succ_figs.figures[i]);
			LinkObject(&(dc->first_grp), &(dc->last_grp), succ_obj, obj);
		}
	} else {
		for (i = undo_packet.prm.undo_prio.n_fig; i > 0; i--) {
			obj = GetObjAddr(w, undo_packet.prm.undo_prio.flst[i - 1]);
			BreakObject(&(dc->first_grp), &(dc->last_grp),
			    obj->body.p_group.pred, obj->body.p_group.succ);
			if (dc->undo_succ_figs.figures[i - 1] == StNullFigure)
				succ_obj = (StObject *) NULL;
			else
				succ_obj = GetObjAddr(w, dc->undo_succ_figs.figures[i - 1]);
			LinkObject(&(dc->first_grp), &(dc->last_grp), succ_obj, obj);
		}
	}
}

static void
_DoUndoPrio(w)
	Widget          w;
{
	StRegion        region;
	StObject       *obj;
	short           xb, yb, xe, ye;
	register        i;

	obj = GetObjAddr(w, undo_packet.prm.undo_prio.flst[0]);
	xb = obj->body.p_group.obj_pos.x;
	yb = obj->body.p_group.obj_pos.y;
	xe = xb + obj->body.p_group.w;
	ye = yb - obj->body.p_group.h;
	UndoJoinSelected(w, undo_packet.prm.undo_prio.n_fig,
			 undo_packet.prm.undo_prio.flst,
			 &xb, &yb, &xe, &ye);
	SelectAll(w, StunsetSelect);	/* erase old handle */
	UndoChangePrior(w, undo_packet.prm.undo_prio.priority);
	region.x = xb;
	region.y = yb;
	region.width = xe - xb + 1;
	region.height = yb - ye + 1;
	AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
	region.width += 40;
	region.height += 40;
	CheckRegionLimit(w, &region);
	StPicRedrawPart(w, &region, StCoordWidget);
	for (i = 0; i < undo_packet.prm.undo_prio.n_fig; i++) {
		TgleHandle(w, undo_packet.prm.undo_prio.flst[i]);
		TgleSelect(w, undo_packet.prm.undo_prio.flst[i]);
	}
	undo_packet.opcode = UUprio;
}

static void
DoUndoUprio(w)
	Widget          w;
{
	StRegion        region;
	StObject       *obj;
	short           xb, yb, xe, ye;
	register        i;

	obj = GetObjAddr(w, undo_packet.prm.undo_prio.flst[0]);
	xb = obj->body.p_group.obj_pos.x;
	yb = obj->body.p_group.obj_pos.y;
	xe = xb + obj->body.p_group.w;
	ye = yb - obj->body.p_group.h;
	UndoJoinSelected(w, undo_packet.prm.undo_prio.n_fig,
			 undo_packet.prm.undo_prio.flst,
			 &xb, &yb, &xe, &ye);
	SelectAll(w, StunsetSelect);	/* erase old handle */
	for (i = 0; i < undo_packet.prm.undo_prio.n_fig; i++)
		StPicChangePriority(w, undo_packet.prm.undo_prio.flst[i],
				    undo_packet.prm.undo_prio.priority);
	region.x = xb;
	region.y = yb;
	region.width = xe - xb + 1;
	region.height = yb - ye + 1;
	AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
	region.width += 40;
	region.height += 40;
	CheckRegionLimit(w, &region);
	StPicRedrawPart(w, &region, StCoordWidget);
	for (i = 0; i < undo_packet.prm.undo_prio.n_fig; i++) {
		TgleHandle(w, undo_packet.prm.undo_prio.flst[i]);
		TgleSelect(w, undo_packet.prm.undo_prio.flst[i]);
	}
	undo_packet.opcode = UPrio;
}

static void
_DoUndoMirror(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StRegion        region;
	StObject       *obj;
	short           xb, yb, xe, ye;
	short           ixb, iyb, ixe, iye;
	register        i;

	obj = GetObjAddr(w, undo_packet.prm.undo_mirror.flst[0]);
	xb = obj->body.p_group.obj_pos.x;
	yb = obj->body.p_group.obj_pos.y;
	xe = xb + obj->body.p_group.w;
	ye = yb - obj->body.p_group.h;
	UndoJoinSelected(w, undo_packet.prm.undo_mirror.n_fig,
			 undo_packet.prm.undo_mirror.flst,
			 &xb, &yb, &xe, &ye);
	SelectAll(w, StunsetSelect);	/* erase old handle */
	for (i = 0; i < undo_packet.prm.undo_mirror.n_fig; i++)
		StPicMirrorFigure(w, undo_packet.prm.undo_mirror.flst[i],
				  undo_packet.prm.undo_mirror.mirror);
	region.x = xb;
	region.y = yb;
	region.width = xe - xb + 1;
	region.height = yb - ye + 1;
	AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
	region.width += 40;
	region.height += 40;
	CheckRegionLimit(w, &region);
	StPicRedrawPart(w, &region, StCoordWidget);
	for (i = 0; i < undo_packet.prm.undo_mirror.n_fig; i++) {
		TgleHandle(w, undo_packet.prm.undo_mirror.flst[i]);
		TgleSelect(w, undo_packet.prm.undo_mirror.flst[i]);
	}
}

static void
_DoUndoRotate(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StRegion        region;
	StObject       *obj;
	short           xb, yb, xe, ye;
	short           ixb, iyb, ixe, iye;
	register        i;

	obj = GetObjAddr(w, undo_packet.prm.undo_rotate.flst[0]);
	xb = obj->body.p_group.obj_pos.x;
	yb = obj->body.p_group.obj_pos.y;
	xe = xb + obj->body.p_group.w;
	ye = yb - obj->body.p_group.h;
	UndoJoinSelected(w, undo_packet.prm.undo_rotate.n_fig,
			 undo_packet.prm.undo_rotate.flst,
			 &xb, &yb, &xe, &ye);
	SelectAll(w, StunsetSelect);	/* erase old handle */
	for (i = 0; i < undo_packet.prm.undo_rotate.n_fig; i++)
		StPicRotateFigure(w, undo_packet.prm.undo_rotate.flst[i],
				  undo_packet.prm.undo_rotate.angle);
	UndoJoinSelected(w, undo_packet.prm.undo_rotate.n_fig,
			 undo_packet.prm.undo_rotate.flst,
			 &xb, &yb, &xe, &ye);
	region.x = xb;
	region.y = yb;
	region.width = xe - xb + 1;
	region.height = yb - ye + 1;
	AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
	region.width += 40;
	region.height += 40;
	CheckRegionLimit(w, &region);
	StPicRedrawPart(w, &region, StCoordWidget);
	for (i = 0; i < undo_packet.prm.undo_rotate.n_fig; i++) {
		TgleHandle(w, undo_packet.prm.undo_rotate.flst[i]);
		TgleSelect(w, undo_packet.prm.undo_rotate.flst[i]);
	}
	undo_packet.prm.undo_rotate.angle = 360 * 64 -
		undo_packet.prm.undo_rotate.angle;
}

static void
_DoUndoDelete(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StRegion        region;
	StObject       *obj;
	short           xb, yb, xe, ye;
	register        i;

	obj = GetObjAddr(w, undo_packet.prm.undo_delete.flst[0]);
	xb = obj->body.p_group.obj_pos.x;
	yb = obj->body.p_group.obj_pos.y;
	xe = xb + obj->body.p_group.w;
	ye = yb - obj->body.p_group.h;
	UndoJoinSelected(w, undo_packet.prm.undo_delete.n_fig,
			 undo_packet.prm.undo_delete.flst,
			 &xb, &yb, &xe, &ye);
	SelectAll(w, StunsetSelect);	/* erase old handle */
	NewUndoObjects(w);
	for (i = 0; i < undo_packet.prm.undo_delete.n_fig; i++)
		LinkUndoChain(w, undo_packet.prm.undo_delete.flst[i]);
	region.x = xb;
	region.y = yb;
	region.width = xe - xb + 1;
	region.height = yb - ye + 1;
	AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
	region.width += 40;
	region.height += 40;
	CheckRegionLimit(w, &region);
	StPicRedrawPart(w, &region, StCoordWidget);
	undo_packet.opcode = UUcreate;
}

static void
_DoUndoCreate(w)
	Widget          w;
{
	StDrawControl  *dc = GetDrawControl(w);
	StRegion        region;
	StObject       *obj;
	StObject       *side_ptr;
	short           xb, yb, xe, ye;
	short           ixb, iyb, ixe, iye;
	register        i;

	SelectAll(w, StunsetSelect);	/* erase old handle */
	obj = dc->first_undo;
	xb = obj->body.p_group.obj_pos.x;
	yb = obj->body.p_group.obj_pos.y;
	xe = xb + obj->body.p_group.w;
	ye = yb - obj->body.p_group.h;
	while (obj) {
		ixb = obj->body.p_group.obj_pos.x;
		iyb = obj->body.p_group.obj_pos.y;
		ixe = ixb + obj->body.p_group.w;
		iye = iyb - obj->body.p_group.h;
		JoinBox(&xb, &yb, &xe, &ye, ixb, iyb, ixe, iye);
		obj = obj->body.p_group.succ;
	}

	obj = dc->last_undo;	/* access from last to first */
	for (i = dc->undo_succ_figs.n_figures; i > 0; i--) {
		side_ptr = obj->body.p_group.pred;
		RechainUndoChain(w, obj, dc->undo_succ_figs.figures[i - 1]);
		obj = side_ptr;
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
	for (i = 0; i < undo_packet.prm.undo_ucreate.n_fig; i++) {
		TgleHandle(w, undo_packet.prm.undo_ucreate.flst[i]);
		TgleSelect(w, undo_packet.prm.undo_ucreate.flst[i]);
	}
	dc->first_undo = (StObject *) NULL;	/* crean up undo buffer */
	dc->last_undo = (StObject *) NULL;
	free((char *) (dc->undo_succ_figs.figures));
	dc->undo_succ_figs.figures = (StFigure *) NULL;
	undo_packet.opcode = UDelete;	/* use same data as this function */
}

static void
_UndoObject(w)
	Widget          w;
{
	switch ((int) undo_packet.opcode) {
	case UPrio:
		DoUndoPrio(w);
		break;
	case UMirror:
		DoUndoMirror(w);
		break;
	case URotate:
		DoUndoRotate(w);
		break;
	case UDelete:
		DoUndoDelete(w);
		break;
	case UMove:
		DoUndoMove(w);
		break;
	case UResize:
		DoUndoResize(w);
		break;
	case UUcreate:
		DoUndoCreate(w);
		break;
	case UUprio:
		DoUndoUprio(w);
		break;
	default:
		;
	}
}
