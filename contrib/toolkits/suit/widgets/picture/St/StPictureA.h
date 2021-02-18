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
 * Sigma User Interface Tools Picture Widget Definition of Data Structure of
 * Picture Objects 
 */

#ifndef	MR_HIRATA
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#endif

#define StNullFigure	-1L	/* NULL figure ID */

/* Error Code */
#define	StBadWidget	1001	/* parameter not a widget */
#define	StBadFigure	1002	/* parameter not a figure */
#define	StBadValue	1003	/* parameter out of range */
#define	StBadAlloc	1004	/* insufficient resources */
#define	StBadIO		1005	/* file I/O error		  */

/* operation code of figure handling */
typedef enum {
	Strectangle,
	Stcircle,
	Stellipse,
	Stline,
	Starc,
	StellipticArc,
	Stpie,
	StellipticPie,
	Stpolygon,
	Stpolyline,
	Stfreehand,
	Sttext,
	Stgroup,
	Stbitmap
}StFigureType;

typedef enum {
	StunsetSelect,
	StsetSelect
}StSetSelect;

typedef enum {
	StunsetHandle,
	StsetHandle
}StSetHandle;

typedef enum {
	StbottomPriority,
	SttopPriority
}StSetPriority;

typedef enum {
	SthorizontalMirror,
	StverticalMirror
}StSetMirror;

typedef enum {
	StantiClockwise,
	Stclockwise
}StArcDirection;

typedef enum {
	StCoordEvent,
	StCoordWidget
}StCoordinate;

/* data structure for picture */

typedef long    StFigure;

typedef struct {
	short           x, y;
}StPoint;

typedef struct {
	short           x, y;
	unsigned short  width, height;
}StRectangle;

typedef struct {
	StArcDirection  direction;	/* direction of arc */
	short           x, y;		/* position of arc */
	unsigned short  width, height;	/* rectangle size surounding arc */
	short           angle1, angle2;	/* arc size */
}StArc;

typedef struct {
	short           x, y;		/* position of bitmap */
	char           *bitmap_file;	/* bitmap file name */
	unsigned int    n_bytes;	/* byte length */
}StBitmap;

typedef struct {
	short           x, y;		/* position of text */
	char           *string;		/* text string */
	unsigned int    n_bytes;	/* byte length */
}StText;

typedef StRectangle StRegion;

typedef struct {
	StFigure       *figures;	/* figure list */
	unsigned int    n_figures;	/* number of figure */
}StFigureList;

typedef struct {
	StPoint        *points;		/* point list */
	unsigned int    n_points;	/* number of point */
}StPointList;

typedef enum {
	StlineSolid,
	StlineOnOffDash,
	StlineDoubleDash
}StLineStyle;

typedef enum {
	StcapButt,
	StcapRound,
	StcapArrow
}StCapStyle;

typedef enum {
	StjoinMiter,
	StjoinRound,
	StjoinBevel
}StJoinStyle;

/* graphics functions, as in GC.alu */
#define	StGclear	GXclear		/* 0 */
#define StGand		GXand		/* src AND dst */
#define StGandReverse	GXandReverse	/* src AND NOT dst */
#define StGcopy		GXcopy		/* src */
#define StGandInverted	GXandInverted	/* NOT src AND dst */
#define	StGnoop		GXnoop		/* dst */
#define StGxor		GXxor		/* src XOR dst */
#define StGor		GXor		/* src OR dst */
#define StGnor		GXnor		/* NOT src AND NOT dst */
#define StGequiv	GXequiv		/* NOT src XOR dst */
#define StGinvert	GXinvert	/* NOT dst */
#define StGorReverse	GXorReverse	/* src OR NOT dst */
#define StGcopyInverted	GXcopyInverted	/* NOT src */
#define StGorInverted	GXorInverted	/* NOT src OR dst */
#define StGnand		GXnand		/* NOT src OR NOT dst */
#define StGset		GXset		/* 1 */

/* color index */
#define	StCBlack	0	/* black/Black */
#define	StCWhite	1	/* white/White */
#define	StCRed		2	/* red/Red */
#define	StCGreen	3	/* green/Green */
#define	StCBlue		4	/* blue/Blue */
#define	StCCyan		5	/* cyan/Cyan */
#define	StCYellow	6	/* yellow/Yellow */
#define	StCMagenta	7	/* magenta/Magenta */
#define	StCGray		8	/* gray/Gray */
#define	StCLightGray	9	/* light gray/LightGray */
#define	StCDarkRed	10	/* orange/Orange */
#define	StCDarkGreen	11	/* lime green/LimeGreen */
#define	StCDarkBlue	12	/* steel blue/SteelBlue */
#define	StCDarkCyan	13	/* midium turquoise/MidiumTurquoise */
#define	StDarkYellow	14	/* midium goldenrod/MidiumGoldenrod */
#define	StCDarkMagenta	15	/* orchid/Orchid */

/* fill style */
typedef enum {
	StfillBoxes,		/* bitmap pattern `boxes' */
	StfillCrossWeave,	/* bitmap pattern `CrossWeave' */
	StfillDimple1,		/* bitmap pattern `Dimple1' */
	StfillDimple3,		/* bitmap pattern `Dimple3' */
	StfillFlippedGray,	/* bitmap pattern `FlippedGray' */
	StfillGray1,		/* bitmap pattern `Gray1' */
	StfillGray3,		/* bitmap pattern `Gray3' */
	StfillLightGray,	/* bitmap pattern `LightGray' */
	StfillRootWeave,	/* bitmap pattern `RootWeave' */
	StfillScales,		/* bitmap pattern `Scales' */
	StfillWideWeave,	/* bitmap pattern `WideWeave' */
	StfillStipple,		/* bitmap pattern `Stipple' */
	StfillSolid,		/* fill solid */
	StfillHollow		/* fill transparent */
}StFillStyle;

/* font style and size */
typedef enum {
	StfontRoman8,		/* bitstream-charter-medium-r-80 */
	StfontRoman16,		/* bitstream-charter-medium-r-140 */
	StfontRoman24,		/* bitstream-charter-medium-r-240 */
	StfontItalic8,		/* bitstream-charter-medium-i-80 */
	StfontItalic16,		/* bitstream-charter-medium-i-140 */
	StfontItalic24,		/* bitstream-charter-medium-i-240 */
	StfontBold8,		/* bitstream-charter-bold-r-80 */
	StfontBold16,		/* bitstream-charter-bold-r-140 */
	StfontBold24,		/* bitstream-charter-bold-r-240 */
	StfontBoldItalic8,	/* bitstream-charter-bold-i-80 */
	StfontBoldItalic16,	/* bitstream-charter-bold-i-140 */
	StfontBoldItalic24	/* bitstream-charter-bold-i-240 */
}StFont;

/* data structure of drawing attributes */
typedef struct {
	StLineStyle     line_style;	/* line style */
	int             line_width;	/* line width(1~20) */
	StCapStyle      begin_cap;	/* cap style of begin of line */
	StCapStyle      end_cap;	/* cap style of end of line */
	StJoinStyle     join_style;	/* joint style */
	int             line_foreground;/* line color index(0~15) */
	int             line_background;/* line color index(0~15) */
	int             function;	/* logical operation(0~15) */
}StLineAtr;

typedef struct {
	StFillStyle     fill_style;	/* fill style */
	int             fill_foreground;/* foreground color index(0~15) */
	int             fill_background;/* background color index(0~15) */
}StPaintAtr;

typedef struct {
	StFont          font;		/* text font for text operations */
	int             text_color;	/* text color index(0~15) */
	int             function;	/* logical operation(0~15) */
}StTextAtr;
