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
 * SIGMA User Interface Tool Picture Widget Attribute Setting Functions 
 */

#include  <stdio.h>
#include  <ctype.h>
#include  <X11/Xlib.h>
#include  <X11/Intrinsic.h>
#include  <X11/StringDefs.h>
#include  <St/StPictureP.h>
#include  <St/StPicPopup.h>

#define BITMAPPATHLEN   1024

#define LINE_ATR        0
#define PAINT_ATR       1
#define TEXT_ATR        2

/* font management macros. */
#define CharHeight(font)	( (font)->max_bounds.ascent+(font)->max_bounds.descent )
#define CharWidth(font, char)	(((font)->min_byte1 == 0 && \
				(font)->max_byte1 == 0) ? \
				((font)->per_char[((char)-(font)->min_char_or_byte2)].width) : \
				((font)->per_char[(char)].width))

#define CharAdvance(font,char) (((font)->per_char)? \
                                (CharWidth(font,char)) :\
                                ((font)->max_bounds.width))
#define BaseLinePosX(font, x)	( x + (font)->max_bounds.lbearing )
#define BaseLinePosY(font, y)	( y + (font)->max_bounds.ascent )

extern XFontStruct *XLoadQueryFont();
extern Pixmap   XCreateBitmapFromData();

static void     _ChangeLineAttributes();
static void     _ChangeFillAttributes();
static void     _ChangeTextAttributes();
static void     _InitializeFigureAttr();
static void     _ChangeMenuAttr();

static void     _CurrentTextStatus();
static void     _ExitTextEdit();
static void     _TextEditing();
static char    *_SaveString();
static void     _GetTextRegion();
static void     _DrawTextObject();
static int      _SetupBitmap();
static void     _StDrawBitmapPrimitive();
static void     _DrawImageFile();
static void     _SetTextCursor();
static void	_ClearText();

StAttrFunctionDescRec attrfunctiondesc = {
					  _ChangeLineAttributes,
					  _ChangeFillAttributes,
					  _ChangeTextAttributes,
					  _InitializeFigureAttr,
					  _ChangeMenuAttr,
					  _CurrentTextStatus,
					  _ExitTextEdit,
					  _TextEditing,
					  _SaveString,
					  _GetTextRegion,
					  _DrawTextObject,
					  _SetupBitmap,
					  _StDrawBitmapPrimitive,
					  _DrawImageFile,
};

static struct _StMyFont {
	char           *font_name;
	XFontStruct    *font_info;
} FontTable[] = {
	                "-bitstream-charter-medium-r-normal--*-80-*", (Font) 0,
	                "-bitstream-charter-medium-r-normal--*-140-*", (Font) 0,
	                "-bitstream-charter-medium-r-normal--*-240-*", (Font) 0,
	                "-bitstream-charter-medium-i-normal--*-80-*", (Font) 0,
	                "-bitstream-charter-medium-i-normal--*-140-*", (Font) 0,
	                "-bitstream-charter-medium-i-normal--*-240-*", (Font) 0,
	                "-bitstream-charter-bold-r-normal--*-80-*", (Font) 0,
	                "-bitstream-charter-bold-r-normal--*-140-*", (Font) 0,
	                "-bitstream-charter-bold-r-normal--*-240-*", (Font) 0,
	                "-bitstream-charter-bold-i-normal--*-80-*", (Font) 0,
	                "-bitstream-charter-bold-i-normal--*-140-*", (Font) 0,
	                "-bitstream-charter-bold-i-normal--*-240-*", (Font) 0,
	                (char *) NULL, (Font) 0
},
  DefaultFontTable[] = {
	                "fixed", (Font) 0,
	                "vtsingle", (Font) 0,
	                (char *) NULL, (Font) 0
};

static char    *BitmapDir = INCDIR;
static char	BitmapPath[BITMAPPATHLEN];
static int      LineStyleConvert[] = {LineSolid, LineOnOffDash, LineDoubleDash};
static int      CapStyleConvert[] = {CapButt, CapRound, CapButt};
static int      JoinStyleConvert[] = {JoinMiter, JoinRound, JoinBevel};

static Pixel   *RealColor;
static Pixmap   RealTile[14];

static void     _InitializeTextCursor();

/*
 * Change Line Attributes. 
 */
static void
_ChangeLineAttributes(display, gc, line_atr)
	Display        *display;
	GC              gc;
	StLineAtr      *line_atr;
{
	XGCValues       gcvalues;

	/* change function */
	gcvalues.function = line_atr->function;

	/* change color pixel */
	gcvalues.foreground = RealColor[line_atr->line_foreground];
	gcvalues.background = RealColor[line_atr->line_background];

	/* change line attributes */
	gcvalues.line_style = LineStyleConvert[(int) line_atr->line_style];
	gcvalues.line_width = line_atr->line_width;
	gcvalues.join_style = JoinStyleConvert[(int) line_atr->join_style];

	gcvalues.fill_style = FillSolid;

	XChangeGC(display, gc,
		  GCFunction |
		  GCForeground |
		  GCBackground |
		  GCLineStyle |
		  GCFillStyle |
		  GCLineWidth |
		  GCJoinStyle,
		  &gcvalues);
}

/*
 * Change Fill Attributes. 
 */
static void
_ChangeFillAttributes(display, gc, paint_atr)
	Display        *display;
	GC              gc;
	StPaintAtr     *paint_atr;
{
	unsigned long   value_mask = 0L;
	XGCValues       gcvalues;

	if (paint_atr->fill_style == StfillHollow)
		return;

	/* change function */
	gcvalues.function = GXcopy;

	/* change color pixel */
	gcvalues.foreground = RealColor[paint_atr->fill_foreground];
	gcvalues.background = RealColor[paint_atr->fill_background];

	/* change fill style */
	if (paint_atr->fill_style == StfillSolid)
		gcvalues.fill_style = FillSolid;
	else {
		gcvalues.fill_style = FillOpaqueStippled;
		gcvalues.stipple = RealTile[(int) paint_atr->fill_style];
		value_mask = GCStipple;
	}

	XChangeGC(display, gc,
		  value_mask |
		  GCFunction |
		  GCFillStyle |
		  GCForeground |
		  GCBackground,
		  &gcvalues);
}

/*
 * Change Text Attributes. 
 */
static void
_ChangeTextAttributes(display, gc, text_atr)
	Display        *display;
	GC              gc;
	StTextAtr      *text_atr;
{
	XGCValues       gcvalues;
	unsigned long   value_mask = 0L;

	/* change function */
	gcvalues.function = text_atr->function;

	/* change color pixel */
	gcvalues.foreground = RealColor[text_atr->text_color];

	gcvalues.background = WhitePixel(display, DefaultScreen(display));

	gcvalues.fill_style = FillOpaqueStippled;
	gcvalues.stipple = RealTile[12];

	/* change font. */
	if (gcvalues.font = FontTable[(int) text_atr->font].font_info->fid)
		value_mask = GCFont;

	XChangeGC(display, gc,
		  GCFunction |
		  GCForeground |
		  GCBackground |
		  GCFillStyle |
		  GCStipple |
		  value_mask,
		  &gcvalues);
}

/*
 * Initialize attribute field in DrawControl 
 */
static void
_InitializeFigureAttr(w)
	Widget          w;
{
	XGCValues       gcvalues;
	StDrawControl  *dc;
	struct _StMyFont *pic_font;
	_MapTable      *pic_map;
	int             screen, i;

	dc = GetDrawControl(w);

	/* setup own color map */
	screen = DefaultScreen(dc->dpy);

	if (DefaultDepth(dc->dpy, screen) > 1) {
		/* display type is color. */
		RealColor = dc->colors;
	} else {
		RealColor = dc->colors;
	}

	/* Open and setup my Own Font. */
	DefaultFontTable[0].font_info
		= XLoadQueryFont(dc->dpy, DefaultFontTable[0].font_name);
	DefaultFontTable[1].font_info
		= XLoadQueryFont(dc->dpy, DefaultFontTable[1].font_name);

	for (i = 0; i < 12; ++i) {
		if ((FontTable[i].font_info = XLoadQueryFont(dc->dpy,
					 FontTable[i].font_name)) == NULL) {
			FontTable[i].font_info = DefaultFontTable[0].font_info;
		}
	}

	/* Create Own Bitmap for Stipple */
	pic_map = (_MapTable *) dc->tiles;
	for (i = 0; i < 13; ++i) {
		RealTile[i] = XCreateBitmapFromData(dc->dpy,
						    (Drawable) dc->window,
						    pic_map[i].bits,
						    pic_map[i].width,
						    pic_map[i].height);
	}

	/* Create Graphic Context */
	gcvalues.function = GXcopy;
	gcvalues.foreground = BlackPixel(dc->dpy, screen);
	gcvalues.background = WhitePixel(dc->dpy, screen);
	gcvalues.line_width = 1;
	gcvalues.line_style = LineSolid;
	gcvalues.cap_style = CapButt;
	gcvalues.join_style = JoinMiter;
	gcvalues.fill_style = FillOpaqueStippled;
	gcvalues.fill_rule = EvenOddRule;
	gcvalues.stipple = RealTile[12];
	gcvalues.ts_x_origin = 0;
	gcvalues.ts_y_origin = 0;
	gcvalues.font = FontTable[0].font_info->fid;
	gcvalues.clip_x_origin = 0;
	gcvalues.clip_y_origin = 0;
	gcvalues.arc_mode = ArcPieSlice;

	dc->figure_gc = XCreateGC(dc->dpy, dc->window,
				  GCFunction |
				  GCForeground |
				  GCBackground |
				  GCLineWidth |
				  GCLineStyle |
				  GCCapStyle |
				  GCJoinStyle |
				  GCFillStyle |
				  GCFillRule |
				  GCStipple |
				  GCTileStipXOrigin |
				  GCTileStipYOrigin |
				  GCFont |
				  GCClipXOrigin |
				  GCClipYOrigin |
				  GCArcMode,
				  &gcvalues);

	_InitializeTextCursor(dc);
}

static void
_ChangeMenuAttr(w, type)
	Widget          w;
	int             type;	/* attribute type 0 : line 1 : paint 2 : text */
{
	StDrawControl  *dc = GetDrawControl(w);
	StFigureList   *fig_list;
	StRegion        region, t_region;
	short           xb, yb, xe, ye;
	register        i;

	switch (type) {
	case LINE_ATR:
		StPicSetLineAtr(w, StNullFigure,
				popupfunctiondesc._StMenuLineAtr);
		break;
	case PAINT_ATR:
		StPicSetPaintAtr(w, StNullFigure,
				 popupfunctiondesc._StMenuPaintAtr);
		break;
	case TEXT_ATR:
		StPicSetTextAtr(w, StNullFigure,
				popupfunctiondesc._StMenuTextAtr);
		break;
	}
	if (dc->n_selected < 1)
		return;
	SetupTergetFigures(w, True);	/* get current selected figre
					 * and collect disending
					 * order */
	SelectAll(w, StunsetSelect);	/* erase old handle */
	fig_list = dc->fig_buf;
	for (i = 0; i < fig_list->n_figures; i++) {
		StPicGetFigureRegion(w, fig_list->figures[i],
					     &region, StCoordWidget);
		switch (type) {
		case LINE_ATR:
			StPicSetLineAtr(w, fig_list->figures[i],
				  popupfunctiondesc._StMenuLineAtr);
			break;
		case PAINT_ATR:
			StPicSetPaintAtr(w, fig_list->figures[i],
				 popupfunctiondesc._StMenuPaintAtr);
			break;
		case TEXT_ATR:
			StPicSetTextAtr(w, fig_list->figures[i],
				  popupfunctiondesc._StMenuTextAtr);
			StPicGetFigureRegion(w,
					     fig_list->figures[i],
					  &t_region, StCoordWidget);
			if (region.width < t_region.width)
				region.width = t_region.width;
			if (region.height < t_region.height)
				region.height = t_region.height;
			break;
		}
		if (i == 0) {
			xb = region.x;
			yb = region.y;
			xe = region.x + region.width;
			ye = region.y - region.height;
		}
		JoinBox(&xb, &yb, &xe, &ye, region.x, region.y,
			 region.x + region.width, region.y - region.height);
	}
	region.x = xb;
	region.y = yb;
	region.width = xe - xb;
	region.height = yb - ye;
	AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
	region.width += 40;
	region.height += 40;
	CheckRegionLimit(w, &region);
	StPicRedrawPart(w, &region, StCoordWidget);
	for (i = 0; i < fig_list->n_figures; i++) {
		TgleHandle(w, fig_list->figures[i]);
		TgleSelect(w, fig_list->figures[i]);
	}
	return;
}

/*
 * get cursor locate position in text buffer for text editing. 
 */
/* return value : text position (index) */
static int
_LocateCursor(w, x, y, text_x, text_y, text_w, text_h, text_prm)
	Widget          w;	/* widget identifire */
	short           x, y;	/* mouse cursor locate position (Event
				 * Coordinate) */
	short           text_x, text_y;	/* position of text object (Event
					 * Coordinate) */
	unsigned short  text_w, text_h;	/* position of text object */
	StTextPrimitive *text_prm;	/* text primitive */
{
	StDrawControl  *dc = GetDrawControl(w);
	XFontStruct    *font_info;
	short           base_x, base_y;
	int             line, pos;
	StText         *text;
	StTextAtr      *text_atr;
	unsigned short  height, width;

	text = &text_prm->text;
	text_atr = &text_prm->text_atr;

	/* font infomation. */
	if (!(font_info = FontTable[(int) text_atr->font].font_info))
		return -1;

	/* logical font size. */
	height = CharHeight(font_info);

	/* test mouse locate position is in text region. */
	if (IsInBox(x, y, text_x, text_y, (text_x + text_w), (text_y - text_h)))
		return -1;

	line = (y - text_y) / height;
	base_x = BaseLinePosX(font_info, text_x);
	base_y = BaseLinePosY(font_info, (text_y + (height * line)));

	/* skip character until locate line. */
	for (pos = 0; line && pos <= text->n_bytes; ++pos)
		if (text->string[pos] == '\n')
			--line;

	/* get a column taht mouse locate line. */
	for (; (text->string[pos] != '\n' && pos <= text->n_bytes); ++pos) {
		width = CharAdvance(font_info, text->string[pos]);
		if (base_x + width >= x)
			break;
		else
			base_x += width;
	}
	if (pos >= text->n_bytes) {
		pos = 0;
		base_x = BaseLinePosX(font_info, text_x);
		base_y = BaseLinePosY(font_info, (text_y + (height * line)));
	}
	dc->cursor_pos.x = base_x;
	dc->cursor_pos.y = base_y;

	return pos;
}

/*
 * set cursor by index to text buffer for text editing. 
 */
/* return value : text position (index) */
static void
_ResetCurser(dc, c_pos, text_atr)
	StDrawControl  *dc;
	int             c_pos;
	StTextAtr      *text_atr;
{
	XFontStruct    *font_info;
	char           *s = dc->text_buf;
	unsigned short  height, width;
	char            space = ' ';
	register        i;

	/* font infomation. */
	if (!(font_info = FontTable[(int) text_atr->font].font_info))
		return;

	dc->cursor_pos = dc->text_pos;
	dc->cursor_pos.x = BaseLinePosX(font_info, dc->cursor_pos.x);

	height = CharHeight(font_info);

	/* get a column taht mouse locate line. */
	for (i = 0; i < c_pos; ++i) {
		if (s[i] == '\n') {
			dc->cursor_pos.y += height;
			dc->cursor_pos.x
				= BaseLinePosX(font_info, dc->text_pos.x);
			width = CharAdvance(font_info, space);
			width = BaseLinePosX(font_info, width);
		} else {
			width = CharAdvance(font_info, s[i]);
			dc->cursor_pos.x += width;
		}
	}

	if (c_pos == 0)
		dc->cursor_pos.x = BaseLinePosX(font_info, dc->text_pos.x);
	if (c_pos == dc->text_end || s[c_pos] == '\n') {
		width = CharAdvance(font_info, space);
		width = BaseLinePosX(font_info, width);
	} else
		width = CharAdvance(font_info, s[c_pos]);

	_SetTextCursor(dc, width, height, text_atr->text_color);

	return;
}

/* show text cursor */
static void
_DrawCursor(dc)
	StDrawControl  *dc;
{
	XFillRectangle(dc->dpy, dc->window, dc->cursor_gc,
	    dc->cursor_pos.x, dc->cursor_pos.y, dc->cursor_w, dc->cursor_h);
}

static void
_SetTextCursor(dc, c_w, c_h, color)
	StDrawControl  *dc;
	unsigned short  c_w, c_h;
	int             color;
{
	XGCValues       gcvalue;

	/* change GC for text cursor. */
	gcvalue.foreground = RealColor[color];

	XChangeGC(dc->dpy, dc->cursor_gc, GCForeground, &gcvalue);

	/* cursor color is same as text color. */
	dc->cursor_w = c_w;
	dc->cursor_h = c_h;
}

/*
 * initialize cursor GC. 
 */
static void
_InitializeTextCursor(dc)
	StDrawControl  *dc;
{
	XGCValues       gcvalue;

	/* Create GC for text cursor. */
	if (DefaultDepth(dc->dpy, DefaultScreen(dc->dpy)) > 1)
		gcvalue.function = GXxor;
	else
		gcvalue.function = GXinvert;

	gcvalue.foreground = RealColor[(int) StCBlack];

	dc->cursor_gc = XCreateGC(dc->dpy, dc->window,
				  GCFunction | GCForeground, &gcvalue);

}

static char    *
_SaveString(string)
	char           *string;
{
	char           *p;

	if (!(p = (char *) malloc(strlen(string) + 1)))
		return NULL;
	strcpy(p, string);
	return p;
}

static void
_GetTextRegion(dc, x, y, text_atr, string, n_bytes, width, height)
	StDrawControl  *dc;
	short           x, y;	/* Text Object position (Event Coordinate) */
	StTextAtr      *text_atr;
	char           *string;
	unsigned int    n_bytes;
	unsigned short *width, *height;
{
	XFontStruct    *font_info;
	unsigned short  line_width = 0;
	char            space = ' ';

	if (!(font_info = FontTable[(int) text_atr->font].font_info))
		return;
	*width = 0;
	line_width = BaseLinePosX(font_info, line_width);
	*height = CharHeight(font_info);
	*width = CharAdvance(font_info, *string);

	while (n_bytes--) {
		if (*string == '\n') {
			*height += CharHeight(font_info);
			line_width += CharAdvance(font_info, space);
			if (*width < line_width)
				*width = line_width;
			line_width = 0;
			line_width = BaseLinePosX(font_info, line_width);
		} else {
			line_width += CharAdvance(font_info, *string);
		}
		++string;
	}
	if (*width < line_width)
		*width = line_width;
}

static void
SelectFigure(w, select)
	Widget          w;
	StSetSelect     select;
{
	StFigureList   *fig_list = NULL;
	register        i;

	StPicGetSelectedFigure(w, &fig_list);
	if (fig_list == NULL)
		return;
	for (i = 0; i < fig_list->n_figures; ++i) {
		StPicSelectFigure(w, fig_list->figures[i], select,
		     (select == StsetSelect ? StsetHandle : StunsetHandle));
	}
	SelectAll(w, select);
}

static int
CreateTextFigure(w, dc)
	Widget          w;
	StDrawControl  *dc;
{
	StText          text_info;
	StRegion        region;
	StFigure        fno;

	text_info.x = dc->text_pos.x;
	text_info.y = dc->text_pos.y;
	text_info.string = dc->text_buf;
	text_info.n_bytes = dc->text_end;

	if ((fno = StPicCreateText(w, &text_info, StCoordEvent))
	    == StNullFigure)
		return Failure;
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
	NewUndoPacket(UDraw);
	SetUndoDraw(fno);

	return Success;
}

static StObject *
IsPointInTextNode(x, y, top_node)
	short           x, y;	/* mouse locate position (StCoordWidget) */
	StObject       *top_node;
{
	StObject       *obj, *text_obj;
	int             n_obj;

	text_obj = (StObject *) NULL;
	obj = top_node->body.p_group.first;
	for (n_obj = top_node->n_obj; n_obj; --n_obj) {
		if (obj->n_obj == 0) {
			if (IsInBox(x, y,
				    obj->body.p_group.obj_pos.x,
				    obj->body.p_group.obj_pos.y,
				    obj->body.p_group.obj_pos.x
				    + obj->body.p_group.w,
				    obj->body.p_group.obj_pos.y
				    - obj->body.p_group.h) == True) {
				text_obj = obj;
				break;
			}
		} else if (!(obj = IsPointInTextNode(x, y, obj))) {
			text_obj = obj;
			break;
		}
		obj = obj->body.p_group.succ;
	}
	return text_obj;
}

static StObject *
IsTextFigure(w, dc, x, y)
	Widget          w;
	StDrawControl  *dc;
	short           x, y;	/* mouse locate position */
{
	StFigureList   *fig_list;
	StObject       *obj;
	StPoint         mouse_pos;
	StFigureType    fig_type;
	register        i;

	mouse_pos.x = x;
	mouse_pos.y = y;
	StPicGetFigure(w, &mouse_pos, &fig_list, StCoordEvent);
	if (fig_list == NULL)
		return NULL;
	if (fig_list->n_figures > 0) {
		/* figure list is set by ascendent order. */
		fig_type = StPicGetFigureType(w, fig_list->figures[0]);
		obj = GetObjAddr(w, fig_list->figures[0]);
		if (fig_type == Sttext)
			return obj;
		else if (fig_type == Stgroup) {
			ToWidgetXY(w, x, y, &x, &y);
			if ((obj = IsPointInTextNode(x, y, obj)))
				return obj;
		}
	}
	return NULL;
}

/*
 * store text buffer string to text object. 
 */
static Boolean
_StoreTextToObject(w, dc)
	Widget          w;
	StDrawControl  *dc;
{
	StObject       *obj, *first;
	StTextPrimitive *text_prm;
	StRegion        region;
	char           *p;
	Boolean         stat;

	if ((obj = dc->edit_text) == NULL)
		return False;

	first = obj->body.p_group.first;
	text_prm = &first->body.pd_text;

	/* save previous region. */
	region.x = obj->body.p_group.obj_pos.x;
	region.y = obj->body.p_group.obj_pos.y;
	region.width = obj->body.p_group.w;
	region.height = obj->body.p_group.h;

	if (dc->text_end < 1
	    || (dc->text_end == 1 && dc->text_buf[0] == '\n')) {
		StPicDeleteFigure(w, obj->obj_no);
		stat = False;
	} else {
		if (!(p = _SaveString(dc->text_buf)))
			return False;

		if (text_prm->text.string)
			free(text_prm->text.string);
		*(dc->text_buf + dc->text_end) = '\0';

		text_prm->text.string = p;
		text_prm->text.n_bytes = dc->text_end;

		_GetTextRegion(dc, dc->text_pos.x, dc->text_pos.y,
			       &text_prm->text_atr, text_prm->text.string,
			       text_prm->text.n_bytes,
			       &obj->body.p_group.w, &obj->body.p_group.h);

		if (region.width < obj->body.p_group.w)
			region.width = obj->body.p_group.w;
		if (region.height < obj->body.p_group.h)
			region.height = obj->body.p_group.h;

		stat = True;
	}
	AddDisp(w, region.x, region.y, -20, -20, &region.x, &region.y);
	region.width += 40;
	region.height += 40;
	CheckRegionLimit(w, &region);
	StPicRedrawPart(w, &region, StCoordWidget);
	if (stat == True) {
		TgleHandle(w, obj->obj_no);
		TgleSelect(w, obj->obj_no);
	}

	return stat;
}

static void
InitializeTextEdit(w, dc, x, y, text_obj)
	Widget          w;
	StDrawControl  *dc;
	short           x, y;	/* mouse locate position */
	StObject       *text_obj;
{
	StObject       *first;
	StTextAtr      *text_atr;

	if ((dc->edit_text = text_obj) != NULL) {
		first = text_obj->body.p_group.first;
		/* convert to Event coordinate. */
		ToGlobalXY(w, text_obj->body.p_group.obj_pos.x,
			   text_obj->body.p_group.obj_pos.y,
			   &dc->text_pos.x, &dc->text_pos.y);
		strncpy(dc->text_buf, first->body.pd_text.text.string,
			first->body.pd_text.text.n_bytes);
		dc->text_end = (int) first->body.pd_text.text.n_bytes;
		dc->text_index = _LocateCursor(w, x, y,
					     dc->text_pos.x, dc->text_pos.y,
			 text_obj->body.p_group.w, text_obj->body.p_group.h,
					       &first->body.pd_text);
		text_atr = &first->body.pd_text.text_atr;
	} else {
		dc->text_index = 0;
		dc->text_end = 0;
		dc->text_pos.x = dc->cursor_pos.x = x;
		dc->text_pos.y = dc->cursor_pos.y = y;
		text_atr = &dc->gl_text_atr;
	}
	_ResetCurser(dc, dc->text_index, text_atr);
}

/*
 * test current mouse location, and set text status. 
 */
static void
_CurrentTextStatus(w, event)
	Widget          w;
	XEvent         *event;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj;
	short           x = event->xbutton.x, y = event->xbutton.y;

	switch ((int) dc->drawstat) {
	case PicInTextDraw:
	case PicSelectText:
		printf("<_CurrentTextStatus>\n");
		printf("dc->drawstat = %d --> PicSelectText\n",
		       dc->drawstat);
		break;
	case PicStartSelect:
		if (!(obj = IsTextFigure(w, dc, x, y)))
			break;
		if (dc->n_selected > 0)
			SelectFigure(w, StunsetSelect);
		InitializeTextEdit(w, dc, x, y, obj);
		StPicDrawFigure(w, obj->obj_no);
		_DrawCursor(dc);
		SetDrawStat(w, PicSelectText);
		break;
	case PicStartDraw:
	case PicInDraw:
		if (spw->stPicture.mode_draw == StdrawText) {
			SetDrawStat(w, PicInTextDraw);
			InitializeTextEdit(w, dc, x, y, (StObject *) NULL);
			_DrawCursor(dc);
		}
		break;
	default:
		break;
	}
	return;
}

/*
 * exit text edit mode. 
 */
static void
_ExitTextEdit(w, event, menu_flag)
	Widget          w;
	XEvent         *event;
	Boolean		menu_flag;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);
	StObject       *obj, *first;
	short           x = event->xbutton.x, y = event->xbutton.y,
			text_x, text_y;
	unsigned short  text_w, text_h;

	switch ((int) dc->drawstat) {
	case PicInTextDraw:
	case PicEndTextDraw:
		if (dc->text_end == 0) {
			_DrawCursor(dc);
			break;
		}
		if (dc->n_selected > 0)
			SelectFigure(w, StunsetSelect);
		/* clear previous text cursor. */
		_DrawCursor(dc);
		if (CreateTextFigure(w, dc) == Failure)
			break;
		SetDrawStat(w, PicStartSelect);
		if (spw->stPicture.mode_continue)
			spw->stPicture.mode_continue = False;
		break;
	case PicSelectText:
		if (menu_flag == True) {
			_StoreTextToObject(w, dc);
			SetDrawStat(w, PicStartSelect);
			break;
		}
		if (!(obj = IsTextFigure(w, dc, x, y))) {
			if (dc->edit_text == NULL) {
				SetDrawStat(w, PicStartSelect);
				break;
			} else {
				_StoreTextToObject(w, dc);
				SetDrawStat(w, PicStartSelect);
				break;
			}
		}
		if (obj->obj_no != dc->edit_text->obj_no) {
			/* store text buffer to object */
			if (_StoreTextToObject(w, dc) == False)
				break;
			InitializeTextEdit(w, dc, x, y, obj);
			_DrawCursor(dc);
		} else {
			/* move text cursor. */
			first = dc->edit_text->body.p_group.first;
			_DrawCursor(dc);
			dc->text_index = _LocateCursor(w, x, y,
					     dc->text_pos.x, dc->text_pos.y,
					      dc->edit_text->body.p_group.w,
					      dc->edit_text->body.p_group.h,
						       &first->body.pd_text);
			_ResetCurser(dc, dc->text_index,
				     &first->body.pd_text.text_atr);
			_DrawCursor(dc);
		}
		break;
	default:
		break;
	}
	return;
}

static void
_DrawTextBuffer(dc, x, y, start, end, text_atr)
	StDrawControl  *dc;
	short           x, y;
	int             start, end;
	StTextAtr      *text_atr;
{
	StTextAtr       local_text_atr;

	/* draw charcter. */
	local_text_atr = *text_atr;

	local_text_atr.function = StGcopy;

	/* clear previous cursor. */
	_ChangeTextAttributes(dc->dpy, dc->figure_gc, &local_text_atr);
	_DrawTextObject(dc->dpy, dc->window, dc->figure_gc, x, y,
	   text_atr, (dc->text_buf + start), (end - start), dc->text_pos.x);
}

static void
_ClearText(dc, x, y, start, end, text_atr)
	StDrawControl  *dc;
	short           x, y;
	int             start, end;
	StTextAtr      *text_atr;
{
	StTextAtr       local_text_atr;

	/* clear charcter. */
	local_text_atr = *text_atr;

	if (DefaultDepth(dc->dpy, DefaultScreen(dc->dpy)) > 1)
		local_text_atr.function = StGxor;
	else
		local_text_atr.function = StGcopyInverted;

	_ChangeTextAttributes(dc->dpy, dc->figure_gc, &local_text_atr);
	_DrawTextObject(dc->dpy, dc->window, dc->figure_gc, x, y,
		     &local_text_atr, (dc->text_buf + start), (end - start),
			dc->text_pos.x);

}

static char     TextBuffer[256];

/*
 * draw text object. 
 */
static void
_DrawTextObject(display, window, gc, x, y, text_atr, string, n_bytes, x_origin)
	Display        *display;
	Window          window;
	GC              gc;
	short           x, y;	/* Text Object position (Event Coordinate) */
	StTextAtr      *text_atr;
	char           *string;
	unsigned int    n_bytes;
	short           x_origin;	/* origin x base line */
{
	XFontStruct    *font_info;
	char           *s;
	short           base_x, base_y;
	register        i, j;

	if (!(font_info = FontTable[(int) text_atr->font].font_info))
		return;
	s = TextBuffer;
	if (x_origin == x)
		base_x = BaseLinePosX(font_info, x);
	else
		base_x = x;
	base_y = BaseLinePosY(font_info, y);

	for (i = j = 0; j < n_bytes; ++j) {
		if ((*(s + i) = *(string + j)) == '\n' || *(s + i) == '\0') {
			*(s + i) = '\0';
			XDrawString(display, window, gc,
				    base_x, base_y, s, strlen(s));
			base_y += CharHeight(font_info);
			base_x = BaseLinePosX(font_info, x_origin);
			i = 0;
		} else
			++i;
	}
	*(s + i) = '\0';
	XDrawString(display, window, gc, base_x, base_y, s, strlen(s));
	return;
}

static int
GetNextNewlinePos(s, c_pos, end)
	char           *s;
	int             c_pos, end;
{
	for (; c_pos < end; ++c_pos)
		if (*(s + c_pos) == '\n')
			break;
	return c_pos;
}

static int
GetPreviousNewlinePos(s, c_pos, start)
	char           *s;
	int             c_pos, start;
{
	register        i;

	for (i = c_pos; i > start; --i)
		if (*(s + i) == '\n' && i < c_pos)
			break;
	return i;
}

static void
ActionDeletePrivious(dc, text_atr)
	StDrawControl  *dc;
	StTextAtr      *text_atr;
{
	char            c, prev, *s;
	int             c_pos, end, nl, i;

	if ((c_pos = dc->text_index) < 1)
		return;
	end = dc->text_end;
	s = dc->text_buf;

	_DrawCursor(dc);
	prev = s[c_pos - 1];
	_ResetCurser(dc, c_pos - 1, text_atr);
	nl = (prev == '\n' ? end : GetNextNewlinePos(s, c_pos, end));
	if (end > c_pos) {
		_ClearText(dc, dc->cursor_pos.x, dc->cursor_pos.y,
			   c_pos - 1, nl, text_atr);
		--end;
		nl = (prev == '\n' ? end : --nl);
		for (i = c_pos - 1; i < end; ++i)
			s[i] = s[i + 1];
		_DrawTextBuffer(dc, dc->cursor_pos.x, dc->cursor_pos.y,
				c_pos - 1, nl, text_atr);
	} else {
		_ClearText(dc, dc->cursor_pos.x, dc->cursor_pos.y,
			   c_pos - 1, nl, text_atr);
		--end;
	}
	_ResetCurser(dc, c_pos - 1, text_atr);
	_DrawCursor(dc);
	dc->text_end = end;
	--dc->text_index;
}

static void
ActionEraseLine(dc, text_atr)
	StDrawControl  *dc;
	StTextAtr      *text_atr;
{
	char            c, prev, *s;
	int             c_pos, end, nl, pnl, i, j;

	c_pos = dc->text_index;
	end = dc->text_end;
	s = dc->text_buf;

	/* look up previous mewline. */
	pnl = GetPreviousNewlinePos(s, c_pos, 0);
	/* look up current mewline. */
	nl = GetNextNewlinePos(s, c_pos, end);
	_DrawCursor(dc);
	_ResetCurser(dc, pnl, text_atr);
	if (end > pnl)
		_ClearText(dc, dc->cursor_pos.x, dc->cursor_pos.y,
			   pnl, end, text_atr);
	/* reset text buffer */
	if (end > c_pos) {
		i = (pnl == 0 ? 0 : pnl + 1);
		for (j = nl + 1; i < end; ++i, ++j)
			s[i] = s[j];
	}
	end -= (nl - pnl);
	if (pnl == 0 && end > 0)
		end -= 1;
	if (end > pnl)
		_DrawTextBuffer(dc, dc->cursor_pos.x, dc->cursor_pos.y,
				pnl, end, text_atr);
	_ResetCurser(dc, pnl, text_atr);
	_DrawCursor(dc);
	dc->text_index = pnl;
	dc->text_end = end;
}

static void
ActionEraseToEnd(dc, text_atr)
	StDrawControl  *dc;
	StTextAtr      *text_atr;
{
	char            c, prev, *s;
	int             c_pos, end, nl, i, j;

	c_pos = dc->text_index;
	if ((end = dc->text_end) == c_pos)
		return;
	s = dc->text_buf;

	/* look up current mewline. */
	nl = GetNextNewlinePos(s, c_pos, end);
	_DrawCursor(dc);
	if (nl > c_pos)
		_ClearText(dc, dc->cursor_pos.x, dc->cursor_pos.y,
			   c_pos, nl, text_atr);
	if (end > c_pos) {
		for (i = c_pos, j = nl; j < end; ++i, ++j)
			s[i] = s[j];
	}
	end -= (nl - c_pos);
	_ResetCurser(dc, c_pos, text_atr);
	_DrawCursor(dc);
	dc->text_end = end;
}

static void
ActionDelete(dc, text_atr)
	StDrawControl  *dc;
	StTextAtr      *text_atr;
{
	char            c, prev, *s;
	int             c_pos, end, nl, i;

	if ((end = dc->text_end) < 1)
		return;
	if ((c_pos = dc->text_index) == end)
		return;
	s = dc->text_buf;

	_DrawCursor(dc);
	nl = (s[c_pos] == '\n' ? end : GetNextNewlinePos(s, c_pos, end));
	_ClearText(dc, dc->cursor_pos.x, dc->cursor_pos.y,
		   c_pos, nl, text_atr);
	--end;
	if (end > c_pos) {
		if (s[c_pos] == '\n' || nl > end)
			nl = end;
		for (i = c_pos; i < end; ++i)
			s[i] = s[i + 1];
		_DrawTextBuffer(dc, dc->cursor_pos.x, dc->cursor_pos.y,
				c_pos, nl, text_atr);
	}
	_ResetCurser(dc, c_pos, text_atr);
	_DrawCursor(dc);
	dc->text_end = end;
}

static void
ActionInsertCharOrNewline(dc, text_atr, type, code)
	StDrawControl  *dc;
	StTextAtr      *text_atr;
	int             type;	/* insert character type code */
	char            code;	/* insert character */
{
	char            c, prev, *s;
	int             c_pos, end, nl, i;

	if (type == KeyInsertChar && !isprint(code))
		return;

	c_pos = dc->text_index;
	end = dc->text_end;
	s = dc->text_buf;

	_DrawCursor(dc);
	nl = (type == KeyNewline ? end : GetNextNewlinePos(s, c_pos, end));
	if (end > c_pos) {
		_ClearText(dc, dc->cursor_pos.x, dc->cursor_pos.y,
			   c_pos, nl, text_atr);
		for (i = end; i > c_pos; --i)
			s[i] = s[i - 1];
	}
	s[c_pos] = (type == KeyNewline ? '\n' : code);
	++end;
	dc->text_end = end;
	nl = (type == KeyNewline ? end : GetNextNewlinePos(s, c_pos, end));
	_DrawTextBuffer(dc, dc->cursor_pos.x, dc->cursor_pos.y,
			c_pos, nl, text_atr);
	++c_pos;
	_ResetCurser(dc, c_pos, text_atr);
	_DrawCursor(dc);
	dc->text_index = c_pos;
}

static void
_TextEditing(w, type, code)
	Widget          w;
	int             type;	/* insert character type code */
	char            code;	/* insert character */
{
	StDrawControl  *dc = GetDrawControl(w);
	StTextAtr       text_atr;

	if (dc->edit_text != NULL)
		text_atr =
			dc->edit_text->body.p_group.first->body.pd_text.text_atr;
	else
		text_atr = dc->gl_text_atr;

	switch (type) {
	case KeyDeletePrivious:
		ActionDeletePrivious(dc, &text_atr);
		break;
	case KeyEraseLine:
		ActionEraseLine(dc, &text_atr);
		break;
	case KeyEraseToEnd:
		ActionEraseToEnd(dc, &text_atr);
		break;
	case KeyDelete:
		ActionDelete(dc, &text_atr);
		break;
	case KeyNewline:
	case KeyInsertChar:
		ActionInsertCharOrNewline(dc, &text_atr, type, code);
		break;
	default:
		break;
	}
}

typedef struct _bitmap_data {
	unsigned int    width, height;
	char           *bitmap_file;
	Pixmap          bitmap;
	struct _bitmap_data *next;
}               BitmapData;

static BitmapData *bm_first = NULL, *bm_last = NULL;

static BitmapData *
GetBitmap(bitmap)
	StBitmap       *bitmap;
{
	BitmapData     *p;

	for (p = bm_first; p; p = p->next)
		if (!strcmp(bitmap->bitmap_file, p->bitmap_file))
			return p;
	return NULL;
}

static BitmapData *
ReadBitmap(w, bitmap)
	Widget          w;
	StBitmap       *bitmap;
{
	StDrawControl  *dc = GetDrawControl(w);
	BitmapData     *p;
	int             x_hot, y_hot;

	if (!(p = (BitmapData *) malloc(sizeof(BitmapData)))) {
		SetErrorStatus(StBadAlloc);
		return NULL;
	}
	if (!(p->bitmap_file = _SaveString(bitmap->bitmap_file))) {
		SetErrorStatus(StBadAlloc);
		free(p);
		return NULL;
	}
	if (XReadBitmapFile(dc->dpy, (Drawable) dc->window, bitmap->bitmap_file,
			    &p->width, &p->height, &p->bitmap,
			    &x_hot, &y_hot) != BitmapSuccess) {
		sprintf(BitmapPath, "%s/bitmaps/%s",
				BitmapDir, bitmap->bitmap_file);
		if (XReadBitmapFile(dc->dpy, (Drawable) dc->window,
			BitmapPath, &p->width, &p->height, &p->bitmap,
				    &x_hot, &y_hot) != BitmapSuccess) {
			SetErrorStatus(StBadIO);
			free(p->bitmap_file);
			free(p);
			return NULL;
		}
	}
	if (p->height == 0)
		p->height = p->width;

	p->next = NULL;

	if (bm_first == NULL)
		bm_first = bm_last = p;
	else {
		bm_last->next = p;
		bm_last = p;
	}

	return p;
}

static int
_SetupBitmap(w, bitmap, width, height)
	Widget          w;
	StBitmap       *bitmap;
	unsigned short *width, *height;
{
	BitmapData     *p;

	if (!(p = GetBitmap(bitmap))) {
		if (!(p = ReadBitmap(w, bitmap)))
			return Failure;
	}
	*width = p->width;
	*height = p->height;
	return Success;
}

static Boolean
_SetBitmapAttributes(w, display, gc, bitmap_prm, width, height)
	Widget          w;
	Display        *display;
	GC              gc;
	StBitmapPrimitive *bitmap_prm;
	unsigned short *width, *height;
{
	BitmapData     *p;
	StPaintAtr     *paint_atr = &bitmap_prm->paint_atr;
	unsigned long   value_mask = 0L;
	XGCValues       gcvalues;

	if (!(p = GetBitmap(&bitmap_prm->bitmap))) {
		if (!(p = ReadBitmap(w, &bitmap_prm->bitmap)))
			return Failure;
	}
	*width = p->width;
	*height = p->height;

	/* change function */
	gcvalues.function = GXcopy;

	/* change color pixel */
	gcvalues.foreground = RealColor[paint_atr->fill_foreground];
	gcvalues.background = RealColor[paint_atr->fill_background];

	/* change fill style */
	gcvalues.fill_style = FillOpaqueStippled;
	gcvalues.stipple = p->bitmap;
	gcvalues.ts_x_origin = bitmap_prm->bitmap.x;
	gcvalues.ts_y_origin = bitmap_prm->bitmap.y;

	XChangeGC(display, gc,
		  GCFunction |
		  GCFillStyle |
		  GCStipple |
		  GCTileStipXOrigin |
		  GCTileStipYOrigin |
		  GCForeground |
		  GCBackground,
		  &gcvalues);
	return Success;
}

static void
_StDrawBitmapPrimitive(w, display, d, gc, bitmap)
	Widget          w;
	Display        *display;
	Drawable        d;
	GC              gc;
	StBitmapPrimitive *bitmap;
{
	unsigned short  width, height;

	if (_SetBitmapAttributes(w, display, gc, bitmap, &width, &height)
	    == Failure)
		return;
	XFillRectangle(display, d, gc, bitmap->bitmap.x, bitmap->bitmap.y,
		       width, height);
	XFlush(display);
}

static void
_DrawImageFile(w)
	Widget          w;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);
	StBitmap        bitmap;
	StRegion        region;
	StFigure        fno;

	bitmap.x = spw->stPicture.view_x + (spw->stPicture.view_w / 2);
	bitmap.y = spw->stPicture.view_y - (spw->stPicture.view_h / 2);

	bitmap.bitmap_file = spw->stPicture.image_file;

	if (dc->n_selected > 0)
		SelectFigure(w, StunsetSelect);
	if ((fno = StPicCreateBitmap(w, &bitmap, StCoordWidget))
	    == StNullFigure)
		return;
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
	NewUndoPacket(UDraw);
	SetUndoDraw(fno);
}
