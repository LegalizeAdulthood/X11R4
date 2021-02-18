/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "paintop.h"
#include "font.h"

#define	CTRL_H	8
#define	NL	10
#define	CR	13
#define	CTRL_U	21
#define	CTRL_X	24
#define	DEL	127

pr_size pf_textwidth(n, f, s)
	int		n;
	XFontStruct	*f;
	char		*s;
{
	int		dummy;
	XCharStruct	ch;
	pr_size		ret;

	XTextExtents(f, s, n, &dummy, &dummy, &dummy, &ch);
	ret.x = ch.width;
	ret.y = ch.ascent + ch.descent;
	return (ret);
}

static GC makegc(op, fg, bg)
int	op;
Pixel	fg;
Pixel	bg;
{
	register GC	ngc;
	XGCValues	gcv;
	int		gcmask;

	gcv.font = roman_font->fid;
	gcmask = GCFunction|GCForeground|GCBackground|GCFont;
	switch (op) {
	case PAINT:
		gcv.foreground = fg;
		gcv.background = bg;
		gcv.function = GXcopy;
		break;
	case ERASE:
		gcv.foreground = bg;
		gcv.background = bg;
		gcv.function = GXcopy;
		break;
	case INV_PAINT:
		gcv.foreground = fg ^ bg;
		gcv.background = bg;
		gcv.function = GXxor;
		break;
	case MERGE:
		gcv.foreground = fg;
		gcv.background = bg;
		gcv.function = GXor;
		break;
	}
	
	ngc = XCreateGC(tool_d, XtWindow(canvas_sw), gcmask, &gcv);
	XCopyGC(tool_d, gc, ~(gcmask), ngc);
	return (ngc);
}

static GC	msg_gccache[0x10];

init_gc()
{
	Pixel	bg, fg;
	Arg	tmp_arg[2];
	
	gccache[PAINT] = makegc(PAINT, x_fg_color.pixel, x_bg_color.pixel);
	gccache[ERASE] = makegc(ERASE, x_fg_color.pixel, x_bg_color.pixel);
	gccache[INV_PAINT] = makegc(INV_PAINT, x_fg_color.pixel,
				    x_bg_color.pixel);
	gccache[MERGE] = makegc(MERGE, x_fg_color.pixel, x_bg_color.pixel);

	/* Need to get the values so we can make GC's which XOR correctly */
	XtSetArg(tmp_arg[0], XtNforeground, &fg);
	XtSetArg(tmp_arg[1], XtNbackground, &bg);
	XtGetValues(msg_sw, tmp_arg, 2);

	msg_gccache[PAINT] = makegc(PAINT, fg, bg);
	msg_gccache[ERASE] = makegc(ERASE, fg, bg);
	
	XtSetArg(tmp_arg[0], XtNforeground, &fg);
	XtSetArg(tmp_arg[1], XtNbackground, &bg);
	XtGetValues(topruler_sw, tmp_arg, 2);

	topgc = makegc(INV_PAINT, fg, bg);
	
	XtSetArg(tmp_arg[0], XtNforeground, &fg);
	XtSetArg(tmp_arg[1], XtNbackground, &bg);
	XtGetValues(sideruler_sw, tmp_arg, 2);
	
	sidegc = makegc(INV_PAINT, fg, bg);
}

/*
**	The next routine is easy to implement, but I haven't missed
**	it yet. Generally it is a bad idea to warp the mouse without
**	the users consent, so maybe the original code is wrong?
*/
win_setmouseposition(w,x,y)
	FDTYPE		w;
	int		x, y;
{
}

int wmgr_confirm(w, message)
	Window		w;
	char		*message;
{
	static TOOL	label = NULL, confirm = NULL;
	XEvent		event;
	Arg	tmp_args[2];

	if( confirm == NULL )
	{
		int		rootx, rooty;
		Window		win;
		Arg		confirm_args[5];
		
		XTranslateCoordinates(tool_d, w, XDefaultRootWindow(tool_d),
				      150, 200, &rootx, &rooty, &win);
		XtSetArg(confirm_args[0], XtNallowShellResize, True);
		XtSetArg(confirm_args[1], XtNx, rootx);
		XtSetArg(confirm_args[2], XtNy, rooty);
		confirm = XtCreatePopupShell("confirm",
					     overrideShellWidgetClass, tool,
					     confirm_args, 3);
		XtSetArg(tmp_args[0], XtNfont, bold_font);
		label = XtCreateManagedWidget("label", labelWidgetClass,
					      confirm, tmp_args, 1);
	}
	
	XtSetArg(tmp_args[0], XtNlabel, message);
	XtSetValues(label, tmp_args, 1);
	XtPopup(confirm, XtGrabNone);
	for (;;)
	{
		XMaskEvent(tool_d, ButtonPressMask|ExposureMask, &event);
		if (event.type == ButtonPress)
		{
			XtPopdown(confirm);
			if (((XButtonEvent *)&event)->button == Button1)
				return (-1);
			else
				return (0);
		}
		/* pass all other events */
		XtDispatchEvent(&event);
	}
/*NOTREACHED*/
}

prompt_string(prompt, reply)
	char		*prompt, *reply;
{
	register int	x, y;
	register int	len, width;
	register char	*r = reply;
	XEvent		event;
	register XKeyEvent	*ke = (XKeyEvent *)&event;
	char		buf[1];
	XWindowAttributes	attr;

	XClearArea(tool_d, msgswfd, 0, 0, 0, 0, False);
	/* uses knowledge that msg_gccache[PAINT] uses roman_font - tsk */
	len = strlen(prompt);
	width = char_width(roman_font);
	y = char_height(roman_font) + 2;
	XDrawString(tool_d, msgswfd, msg_gccache[PAINT], 2, y, prompt, len);
	x = width * len + 4;
	XGetWindowAttributes(tool_d, msgswfd, &attr);
	XSelectInput(tool_d, msgswfd, attr.your_event_mask | KeyPressMask);
	for (;;)
	{
		XWindowEvent(tool_d, msgswfd, KeyPressMask, &event);
		if (event.type != KeyPress)
			continue;
		if (XLookupString(ke, buf, sizeof(buf), NULL, NULL) <= 0)
			continue;
		switch (buf[0])
		{
		case CTRL_H:
		case DEL:
			if (r != reply)
			{
				x -= width;
				--r;
				XDrawString(tool_d, msgswfd, msg_gccache[ERASE],
					x, y, r, 1);
			}
			break;
		case CTRL_U:
		case CTRL_X:
			while (r != reply)
			{
				x -= width;
				--r;
				XDrawString(tool_d, msgswfd, msg_gccache[ERASE],
					x, y, r, 1);
			}
			break;
		case CR:
		case NL:
			*r = '\0';
			XClearArea(tool_d, msgswfd, 0, 0, 0, 0, False);
			return;
		default:
			if (buf[0] < ' ' || buf[0] > '~')
				continue;
			XDrawString(tool_d, msgswfd, msg_gccache[PAINT],
				x, y, buf, 1);
			x += width;
			*r++ = buf[0];
			break;
		}
	}
}

static void
CvtStringToFloat(args, num_args, fromVal, toVal)
XrmValuePtr	args;
Cardinal	*num_args;
XrmValuePtr	fromVal;
XrmValuePtr	toVal;
{
	static	float	f;

	if(*num_args != 0 )
		XtWarning("String to Float conversion needs no extra arguments");
	if(sscanf((char *)fromVal->addr, "%f", &f) == 1)
	{
		(*toVal).size = sizeof(float);
		(*toVal).addr = (caddr_t) &f;
	}
	else
		XtStringConversionWarning((char *) fromVal->addr, "Float");
}

static void
CvtIntToFloat(args, num_args, fromVal, toVal)
XrmValuePtr	args;
Cardinal	*num_args;
XrmValuePtr	fromVal;
XrmValuePtr	toVal;
{
	static	float	f;

	if(*num_args != 0 )
		XtWarning("Int to Float conversion needs no extra arguments");
	f = *(int *)fromVal->addr;
	(*toVal).size = sizeof(float);
	(*toVal).addr = (caddr_t) &f;
}

fix_converters()
{
	XtAddConverter("String", "Float", CvtStringToFloat, NULL, 0);
	XtAddConverter("Int", "Float", CvtIntToFloat, NULL, 0);
}
