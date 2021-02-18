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

#include  <X11/Xlib.h>
#include  <X11/Intrinsic.h>
#include  <X11/StringDefs.h>
#include  <St/StPictureP.h>

static void     _TgleGroup();
static void     _TgleHandle();
static void     _TgleSelect();
static void     _SelectAll();
static Boolean  _IsSelectedFigure();
static int      _FindHandle();

StSelFunctionDescRec selfunctiondesc = {
					_TgleGroup,
					_TgleHandle,
					_TgleSelect,
					_SelectAll,
					_IsSelectedFigure,
					_FindHandle,
};

static void
CalcButton(x, y, xb, yb, width, height)
	short           x, y;
	short          *xb, *yb, *width, *height;	/* Xlib coordinates */
{
	*xb = x - SEL_BUTTON_SIZE / 2;
	*yb = y - SEL_BUTTON_SIZE / 2;
	*width = SEL_BUTTON_SIZE;
	*height = SEL_BUTTON_SIZE;
}

static void
CalcFrame(w, grp_ptr, xb, yb, xc, yc, xe, ye)
	Widget          w;
	StObject       *grp_ptr;
	short          *xb, *yb, *xc, *yc, *xe, *ye;	/* Xlib coordinates */
{
	short           x1, y1, x2, y2;	/* Xlib coordinates */

	ToGlobalXY(w, grp_ptr->body.p_group.obj_pos.x,
		   grp_ptr->body.p_group.obj_pos.y,
		   &x1, &y1);
	x2 = x1 + grp_ptr->body.p_group.w;
	y2 = y1 + grp_ptr->body.p_group.h;

	*xc = (x1 + x2) / 2;
	*yc = (y1 + y2) / 2;
	*xb = Min(*xc - SEL_BUTTON_SIZE - 1, x1);
	*yb = Min(*yc - SEL_BUTTON_SIZE - 1, y1);
	*xe = Max(*xc + SEL_BUTTON_SIZE + 1, x2);
	*ye = Max(*yc + SEL_BUTTON_SIZE + 1, y2);
}

static Boolean
CheckButton(w, button_x, button_y, mouse_x, mouse_y)
	Widget          w;
	short           button_x, button_y;	/* Xlib coordinates */
	short           mouse_x, mouse_y;	/* Xlib coordinates */
{
	short           xb, yb, width, height;	/* Xlib coordinates */
	short           mx, my, x, y;	/* Widget coordinate */

	CalcButton(button_x, button_y, &xb, &yb, &width, &height);
	ToWidgetXY(w, mouse_x, mouse_y, &mx, &my);
	ToWidgetXY(w, xb, yb, &xb, &yb);
	return IsInBox(mx, my, xb - BUTTON_EXTEND, yb + BUTTON_EXTEND,
		   xb + width + BUTTON_EXTEND, yb - height - BUTTON_EXTEND);
}

static int
CheckFrame(w, grp_ptr, mouse_x, mouse_y)
	Widget          w;
	StObject       *grp_ptr;
	short           mouse_x, mouse_y;	/* Xlib coordinates */
{
	short           xb, yb, xc, yc, xe, ye;	/* Xlib coordinates */

	CalcFrame(w, grp_ptr, &xb, &yb, &xc, &yc, &xe, &ye);

	if (grp_ptr->body.p_group.is_selected) {
		if (CheckButton(w, xb, yb, mouse_x, mouse_y))
			return SLUPPER_LEFT;
		if (CheckButton(w, xc, yb, mouse_x, mouse_y))
			return SLUPPER_MIDDLE;
		if (CheckButton(w, xe, yb, mouse_x, mouse_y))
			return SLUPPER_RIGHT;
		if (CheckButton(w, xb, yc, mouse_x, mouse_y))
			return SLLEFT;
		if (CheckButton(w, xe, yc, mouse_x, mouse_y))
			return SLRIGHT;
		if (CheckButton(w, xb, ye, mouse_x, mouse_y))
			return SLLOWER_LEFT;
		if (CheckButton(w, xc, ye, mouse_x, mouse_y))
			return SLLOWER_MIDDLE;
		if (CheckButton(w, xe, ye, mouse_x, mouse_y))
			return SLLOWER_RIGHT;
	}
	return SLNONE;
}

static void
WireFrame(w, xb, yb, xe, ye)
	Widget          w;
	short           xb, yb, xe, ye;	/* Xlib coordinates */
{
	StDrawControl  *dc = GetDrawControl(w);
	short           width, height;
	short           fxb, fxe, fyb, fye;

	if (xb == xe || yb == ye) {
		XDrawLine(dc->dpy, dc->window, dc->handle_gc,
			  xb, yb, xe, ye);
		return;
	}
	fxb = Min(xb, xe);
	fyb = Min(yb, ye);
	width = xb + xe - fxb - fxb;
	height = yb + ye - fyb - fyb;
	XDrawRectangle(dc->dpy, dc->window, dc->handle_gc,
		       fxb, fyb, width, height);
}

static void
DrawButton(w, x, y)
	Widget          w;
	short           x, y;	/* Xlib coordinates */
{
	StDrawControl  *dc = GetDrawControl(w);
	short           xb, yb, width, height;	/* Xlib coordinates */

	CalcButton(x, y, &xb, &yb, &width, &height);
	XFillRectangle(dc->dpy, dc->window, dc->handle_gc,
		       xb, yb, width, height);
}

static void
DrawSelect(w, grp_ptr)
	Widget          w;
	StObject       *grp_ptr;
{
	short           xb, yb, xc, yc, xe, ye;	/* xlib coordinates */

	CalcFrame(w, grp_ptr, &xb, &yb, &xc, &yc, &xe, &ye);

	WireFrame(w, xb, yb, xe, ye);

	DrawButton(w, xb, yb);	/* uppper left button */
	DrawButton(w, xc, yb);	/* uppper middle button */
	DrawButton(w, xe, yb);	/* uppper right button */
	DrawButton(w, xb, yc);	/* left button */
	DrawButton(w, xe, yc);	/* right button */
	DrawButton(w, xb, ye);	/* lower left button */
	DrawButton(w, xc, ye);	/* lower middle button */
	DrawButton(w, xe, ye);	/* lower right button */

}

static void
_TgleGroup(w, grp_ptr)
	Widget          w;
	StObject       *grp_ptr;
{
	StDrawControl  *dc = GetDrawControl(w);
	int             sign = (grp_ptr->body.p_group.is_selected) ? -1 : 1;

	dc->n_selected += sign;
	grp_ptr->body.p_group.is_selected =
		(StSetSelect) ! grp_ptr->body.p_group.is_selected;
	DrawSelect(w, grp_ptr);
}

static void
_TgleHandle(w, figure)
	Widget          w;
	StFigure        figure;
{
	StObject       *grp_ptr = GetObjAddr(w, figure);

	grp_ptr->body.p_group.is_handled =
		(StSetHandle) ! grp_ptr->body.p_group.is_handled;
}

static void
_TgleSelect(w, figure)
	Widget          w;
	StFigure        figure;
{
	StObject       *grp_ptr = GetObjAddr(w, figure);

	_TgleGroup(w, grp_ptr);
}

static void
_SelectAll(w, state)
	Widget          w;
	StSetSelect     state;
{
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *grp_ptr = dc->first_grp;

	while (grp_ptr != NULL) {
		if (grp_ptr->body.p_group.is_selected != state)
			_TgleGroup(w, grp_ptr);
		grp_ptr = grp_ptr->body.p_group.succ;
	}
}

static Boolean
_IsSelectedFigure(w, figure)
	Widget          w;
	StFigure        figure;
{
	StObject       *grp_ptr = GetObjAddr(w, figure);

	return (Boolean) (grp_ptr->body.p_group.is_selected);
}

static int
_FindHandle(w, x, y, figure)
	Widget          w;
	short           x, y;	/* Widget coordinate */
	StFigure        figure;
{
	StObject       *grp_ptr = GetObjAddr(w, figure);
	short           mx, my;	/* Xlib coordinate */

	ToGlobalXY(w, x, y, &mx, &my);
	return CheckFrame(w, grp_ptr, mx, my);
}
