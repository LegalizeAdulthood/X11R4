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
 * Sigma User Interface Tools Picture Widget Definition of Data Structure for
 * drawing a figure 
 */

typedef enum {
	PicStartDraw,
	PicStartSelect,
	PicStartMove,
	PicStartCopy,
	PicStartResize,

	PicInDraw,
	PicInPolygon,
	PicInArcStep1,
	PicInMove,
	PicInMove_SB1,
	PicInCopy,
	PicInResize,
	PicInSelect,
	PicInSelect_B3,
	PicInScrool,

	PicSetBeginArc,
	PicSetFinArc,

	PicClickSelect,
	PicClickSelect_B3,

	PicEndArcStep1,
	PicEndBeginArc,
	PicEndFinArc,
	PicEndDraw,
	PicEndMove,
	PicEndCopy,
	PicEndResize,

	PicInTextDraw,
	PicEndTextDraw,
	PicSelectText,
}StPicDrawStat;

typedef struct {
	StFigureType    type;
	int             npoints;
	XPoint         *points;
	int             width, height;
	StArcDirection  direction;
	int             angle1, angle2;
}StDragImage;

typedef struct _StDrawPrimDesc {
	StDrawFunction  type;
	int             (*dragfun) ();
	int             (*movefun) ();
	int             (*resizefun) ();
}StDrawPrimDesc;

/*
 * Data structure for attributes of drawing primitives. 
 */
typedef struct {
	StFigureType    type;	/* figure type, Stcircle, Stellipse, Starc,
				 * Steparc, Stpie, Steppie, StellipticArc,
				 * StellipticPie */
	StLineAtr       line_atr;/* line attributes */
	StPaintAtr      paint_atr;/* fill attributes */
	StArc           arc;	/* structure of arc */
}StArcPrimitive;

typedef struct {
	StFigureType    type;	/* figure type, Stline, Stpolygon,
				 * Stpolyline, Stfreehand */
	StLineAtr       line_atr;/* line attributes */
	StPaintAtr      paint_atr;/* fill attributes */
	StPointList     point_list;/* point list */
}StLinePrimitive;

typedef struct {
	StFigureType    type;	/* figure type */
	StLineAtr       line_atr;/* line attributes */
	StPaintAtr      paint_atr;/* fill attributes */
	StRectangle     rectangle;/* rectangle infomation */
}StRectPrimitive;

typedef struct {
	StFigureType    type;	/* figure type */
	StTextAtr       text_atr;/* text attributes */
	StText          text;	/* text infomation */
}StTextPrimitive;

typedef struct {
	StFigureType    type;	/* figure type */
	StPaintAtr      paint_atr;/* fill attributes */
	StBitmap        bitmap;	/* bitmap data structure */
}StBitmapPrimitive;

/*
 * Data structure for figure object. 
 */
typedef struct _object {
	StFigureType    type;	/* object type */
	StFigure        obj_no;	/* object identifire */
	unsigned int    n_obj;	/* number of object */
	struct _object *parent;	/* pointor for parent object node */
	union _body {
		struct _group {
			StSetSelect     is_selected;	/* flag for selected or
							 * not */
			StSetHandle     is_handled;	/* flag for show handle
							 * or not */
			StPoint         obj_pos;	/* object coordinate */
			unsigned short  w, h;	/* object size */
			struct _object *succ;	/* successive object node */
			struct _object *pred;	/* predecessive object node */
			struct _object *first;	/* first child object node */
			struct _object *last;	/* last child object node */
		}p_group;	/* grouped figure */

		StArcPrimitive  pd_arc;		/* Arc primitive */
		StLinePrimitive pd_line;	/* Line primitive */
		StRectPrimitive pd_rect;	/* Rectangle primitive */
		StTextPrimitive pd_text;	/* Text primitive */
		StBitmapPrimitive pd_bitmap;	/* Bitmap primitive */
	}body;
}StObject;

typedef struct _group StGroup;
typedef union _body StObjBody;

/*
 * Data structure for hash-table of figure number 
 */

typedef struct _HashTbl {
	StFigure        figure;
	StObject       *addr;
	struct _HashTbl *succ;
}StHashTbl;

/*
 * Data structure for drawing control. 
 */
typedef struct {
	Display        *dpy;		/* Graphic Display */
	Screen         *screen;		/* Graphic Screen */
	Window          window;		/* Graphic Window  */
	GC              figure_gc;	/* Graphic context for picture */
	GC              handle_gc;	/* Graphic context for picture handle */
	StPoint         orgin;		/* Origin of Picture Widget Window(DC) */
	unsigned short  win_w, win_h;	/* size of Picture Widget Window */
	StPoint         redraw_pos;	/* position of Redraw area (DC) */
	unsigned short  redraw_w, redraw_h;/* size of Redraw area */
	short           fixx, fixy;	/* click point (DC cordinate) */
	Boolean         first_move;	/* first time at move action */
	short           old_disp_x;	/* for move action */
	short           old_disp_y;	/* for move action */
	StDragImage    *image;		/* Figure image on drawing */
	char           *inq_buf;	/* Buffer for inquire function */
	StFigureList   *fig_buf;	/* selected figures MOVE COPY use */
	int             select;		/* click point on handle of figure */
	int             force_grid;	/* flag for grid alignment */
	int             visible_grid;	/* flag for grid visible or not */
	int             grid_interval;	/* grid interval */
	StLineAtr       gl_line_atr;	/* global line attribute */
	StPaintAtr      gl_paint_atr;	/* global fill attribute */
	StTextAtr       gl_text_atr;	/* global text attribute */
	Pixel          *colors;		/* real colors. */
	char           *tiles;		/* real tiles */
	unsigned int    figure_no;	/* current group node ID */
	unsigned int    n_group;	/* total number of group node */
	unsigned int    n_selected;	/* total number of selected node */
	StDrawFunction  cur_menu;	/* current menu item for drawing */
	StPicDrawStat   drawstat;	/* statictics for mouse action */
	StPicDrawStat   oldstat;	/* save statictics for mouse action */
	long            hash_size;	/* hash-table size */
	StHashTbl      *hash_tbl;	/* hash-table of figure number */
	StObject       *first_grp;	/* first object node */
	StObject       *last_grp;	/* last object node */
	StFigureList    undo_succ_figs;	/* undo succ object number list */
	StObject       *first_undo;	/* first 'undo' object node */
	StObject       *last_undo;	/* last 'undo' object node */
	StObject        dummy_object;	/* dummy area for calc obj size */

	StObject       *edit_text;	/* current editing text */
	StPoint         text_pos;	/* origin of text object(Event coord) */
	StPoint         cursor_pos;	/* text cursor position(Event coord) */
	unsigned short  cursor_w, cursor_h;/* text cursor size */
	GC              cursor_gc;	/* Graphic context for text cursor */
	char           *text_buf;	/* text editing buffer */
	int             text_index;	/* text buffer index */
	int             text_end;	/* index for end of text */
}StDrawControl;

#include <St/StPicUi.h>
#include <St/StPicMod.h>
#include <St/StPicObj.h>
#include <St/StPicUtl.h>
#include <St/StPicPf.h>
#include <St/StPicAtr.h>
#include <St/StPicSel.h>
#include <St/StPicMisc.h>
#include <St/StPicIo.h>
#include <St/StPicUndo.h>

#define SEL_BUTTON_SIZE   5	/* handle button size(must be odd number) */
#define BUTTON_EXTEND     2	/* handle button extend size */
#define HASH_ENTRY	1024	/* hash table entry size */
#define Min(v1, v2) (v1<v2 ? v1 : v2)
#define Max(v1, v2) (v1>v2 ? v1 : v2)
#define Abs(v1)     (v1<0 ? (-v1) : v1)

#define KeyDeletePrivious	0
#define KeyEraseLine		1
#define KeyEraseToEnd		2
#define KeyDelete		3
#define KeyNewline		4
#define KeyInsertChar		5
