/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#ifdef	GLOBAL
#define	EXTERN
#else
#define	EXTERN	extern
#endif

#ifdef	X11
typedef struct
{
	unsigned int	x, y, z;
	caddr_t		*m;
} MprData;

#define	mpr_static(name,x,y,z,pix)	\
XImage name	= \
{ \
(x), (y), 0, XYBitmap, (char *)(pix),\
MSBFirst, 8, MSBFirst, 8, (z), (x+7)/8, 1, 0, 0, 0, NULL \
}

typedef struct
{
	int		x, y;
} pr_size;

typedef struct
{
	unsigned int	hotx, hoty;
	unsigned int	graphop;
	XImage		*bitmap;
} CursorRec;

typedef struct
{
	unsigned int	r_width, r_height, r_left, r_top;
} RectRec;

typedef struct
{
	int		type;
	char		*label;
	caddr_t		info;
} MenuItemRec;

struct Menu
{
	int		m_imagetype;
#define	MENU_IMAGESTRING	0x00	/* imagedata is char * */
#define MENU_GRAPHIC            0x01    /* imagedata is pixrect * */
	caddr_t		m_imagedata;
	int		m_itemcount;
	MenuItemRec	*m_items;
	struct Menu	*m_next;
	caddr_t		m_data;
};

typedef	struct Menu	MenuRec;
#endif	X11

#ifndef	X11
typedef		struct pixwin *		PIXWIN;
typedef		struct pixrect		PIXRECTREC;
typedef		struct pixrect *	PIXRECT;
typedef		struct pixfont *	PIX_FONT;
/* pixrect/pixfont.h already took PIXFONT, foo! */
typedef		struct mpr_data		MPR_DATA;
typedef		struct cursor		CURSORREC;
typedef		struct cursor *		CURSOR;
typedef		struct tool *		TOOL;
typedef		struct toolsw *		TOOLSW;
typedef		int			FDTYPE;
typedef		struct inputevent	INPUTEVENT;
typedef		struct inputmask	INPUTMASK;
typedef		struct pr_size		PR_SIZE;
typedef		struct rect		RECT;
typedef		struct menuitem		MENUITEM;
typedef		struct menu		MENU;
#else
typedef		Window			PIXWIN;
typedef		XImage			PIXRECTREC;
typedef		XImage *		PIXRECT;
typedef		XFontStruct *		PIX_FONT;
typedef		MprData			MPR_DATA;
typedef		CursorRec		CURSORREC;
typedef		CursorRec *		CURSOR;
typedef		Widget			TOOL;
typedef		Widget			TOOLSW;
typedef		Window			FDTYPE;
typedef		XButtonEvent		INPUTEVENT;
typedef		unsigned long		INPUTMASK;
typedef		pr_size			PR_SIZE;
typedef		RectRec			RECT;
typedef		MenuItemRec		MENUITEM;
typedef		MenuRec			MENU;
#endif	X11

#ifndef	X11
#define	set_marker(win,x,y,w,h,op,pix,z1,z2) \
	pw_write((win),(x),(y),(w),(h),(op),(pix),(z1),(z2))
#else
#define	set_marker(win,x,y,w,h,op,pix,z1,z2) \
	XDrawRectangle(tool_d,(win),gccache[(op)],(x),(y),(w),(h))
#endif	X11

EXTERN PIXWIN		canvas_pixwin,
			msg_pixwin,
			panel_pixwin,
			sideruler_pixwin,
			topruler_pixwin;

EXTERN CURSOR		cur_cursor;
EXTERN CURSORREC	arrow_cursor,
			bull_cursor,
			buster_cursor,
			crosshair_cursor,
			null_cursor,
			pencil_cursor,
			pick15_cursor,
			pick9_cursor,
			wait_cursor;

EXTERN TOOL		tool;

EXTERN TOOLSW		canvas_sw,
			msg_sw,
			panel_sw,
			sideruler_sw,
			topruler_sw;

EXTERN FDTYPE		canvas_swfd,
			panel_swfd,
			msgswfd,
			trswfd,
			srswfd;

#ifdef	X11
EXTERN Display		*tool_d;
EXTERN Screen		*tool_s;
EXTERN int		tool_sn;
EXTERN GC		gc, gccache[0x10], topgc, sidegc;
EXTERN XColor		x_fg_color, x_bg_color;

struct icon {
	short	 	ic_width, ic_height;	/* overall icon dimensions */
	PIXRECT	 	ic_background;	/* background pattern (mem pixrect) */
	RECT		ic_gfxrect;	/* where the graphic goes */
	PIXRECT 	ic_mpr;		/* the graphic (a memory pixrect) */
	RECT		ic_textrect;	/* where text goes */
	char	       *ic_text;	/* the text */
	PIX_FONT 	ic_font;	/* Font with which to display text */
	int		ic_flags;
};

/* flag values */
#define	ICON_BKGRDPAT	0x02		/* use ic_background to prepare image*/
#define	ICON_BKGRDGRY	0x04		/* use std gray to prepare image*/
#define	ICON_BKGRDCLR	0x08		/* clear to prepare image*/
#define	ICON_BKGRDSET	0x10		/* set to prepare image*/
#define	ICON_FIRSTPRIV	0x0100		/* start of private flags range */
#define	ICON_LASTPRIV	0x8000		/* end of private flags range */

#include "xtra.h"

#endif	X11
