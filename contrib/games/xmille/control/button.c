/*
 *	button.c
 */

# include	<X11/Xlib.h>
# include	<X11/Xutil.h>
# include	"control.h"

extern	Display	*dpy;

struct button {
	Button	button;
	int	width, height;
	int	x, y;
	GC	gc;
	long	backcolor;
	int	borderwidth;
	int	npoints;
	XPoint	*points;
	char	*text;
};

struct perwindow {
	struct button	*b;
	Window		wd;
	int		(*notify)();
	int		state;
};

static XContext	assocContext;
static XContext	perwindowContext;
static int		nextButton;

Button
CcreateButton (text, width, gc, font, backcolor, borderwidth)
char		*text;
int		width;
GC		gc;
XFontStruct	*font;
long		backcolor;
int		borderwidth;
{
	char		*malloc ();
	struct button	*b;
	int		round, off;
	int		hround;
	int		twidth;
	int		height;
	
	if (!assocContext)
		assocContext = XUniqueContext();
	b = (struct button *) malloc (sizeof (struct button));
	b->button = nextButton;
	XSaveContext (dpy, assocContext, nextButton++, (char *) b);
	height = font->ascent + font->descent;
	twidth = XTextWidth (font, text, strlen(text));
	if (width <= 0)
		b->width = twidth + font->max_bounds.width * 2;
	else
		b->width = width;
	b->height = height * 2;
	b->x = b->width / 2 - twidth / 2;
	b->y = height + font->descent/*/2*/;
	b->text = text;
	b->gc = gc;
	b->backcolor = backcolor;
	b->borderwidth = borderwidth;
	b->npoints = 9;
	b->points = (XPoint *) malloc (sizeof (XPoint) * 9);

	round = b->height / 8;
	off = borderwidth;

	b->points[0].x = round;
	b->points[0].y = 0;

	b->points[1].x = (b->width - off) - round;
	b->points[1].y = 0;

	b->points[2].x = (b->width - off);
	b->points[2].y = round;

	b->points[3].x = (b->width - off);
	b->points[3].y = (b->height - off) - round;

	b->points[4].x = (b->width - off) - round;
	b->points[4].y = (b->height - off);

	b->points[5].x = round;
	b->points[5].y = (b->height - off);

	b->points[6].x = 0;
	b->points[6].y = (b->height - off) - round;

	b->points[7].x = 0;
	b->points[7].y = round;

	b->points[8].x = round;
	b->points[8].y = 0;

	return b->button;
}

Window
CmapButton (parent, x, y, button, notify)
Window	parent;
int	x, y;
Button	button;
int	(*notify)();
{
	struct button		*b;
	struct perwindow	*p;
	Window			w;

	if( XFindContext (dpy, assocContext, button, &b) )
		return 0;
	
	w = XCreateSimpleWindow (dpy, parent, x, y, b->width, b->height,
		0, 0, b->backcolor);
	if (!w)
		return 0;
	XMapWindow (dpy, w);
	if (!perwindowContext)
		perwindowContext = XUniqueContext();
	p = (struct perwindow *) malloc (sizeof (struct perwindow));
	XSaveContext (dpy, perwindowContext, w, (char *) p);
	p->b = b;
	p->wd = w;
	p->notify = notify;
	p->state = 0;
	buttonOff (w, b, p);
	return w;
}

CunmapButton (w)
Window	w;
{
	struct button	*b;
	struct perwindow	*p;

	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	XDeleteContext (dpy, perwindowContext, w);
	free (p);
	XDestroyWindow (dpy, w);
	return 1;
}

CmanageButton (rep)
XAnyEvent	*rep;
{
	struct perwindow	*p;
	struct button		*b;
	XMotionEvent		*me;
	Window			w;

	w = rep->window;
	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	b = p->b;
	switch (rep->type) {
	case Expose:
		if (p->state)
			buttonOn (w, b, p);
		else
			buttonOff (w, b, p);
		break;
	case ButtonPress:
		buttonOn (w, b, p);
		p->state = 1;
		break;
	case ButtonRelease:
		if (p->state)
			p->notify (w);
		p->state = 0;
		break;
	case MotionNotify:
		me = (XMotionEvent *) rep;
		if (me->x < 0 || me->x > b->width ||
		    me->y < 0 || me->y > b->height)
		if (p->state) {
			buttonOff (w, b, p);
			p->state = 0;
		}
		break;
	case LeaveNotify:
		if (p->state) {
			buttonOff (w, b, p);
			p->state = 0;
		}
		break;
	}
}

CredrawButton (w)
Window		w;
{
	struct perwindow	*p;
	struct button		*b;

	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	b = p->b;
	buttonOff (w, b, p);
}

static
buttonOn (w, b)
Window	w;
struct button	*b;
{
	unsigned long	fg, bg;
	bg = b->gc->values.background;
	fg = b->gc->values.foreground;
	XSetForeground (dpy, b->gc, bg);
	XSetBackground (dpy, b->gc, fg);
	buttonOff (w, b);
	XSetForeground (dpy, b->gc, fg);
	XSetBackground (dpy, b->gc, bg);
}
	
static
buttonOff (w, b)
Window	w;
struct button	*b;
{
	unsigned long	fg, bg;

	bg = b->gc->values.background;
	fg = b->gc->values.foreground;
	XSetForeground (dpy, b->gc, bg);
	XSetBackground (dpy, b->gc, fg);
	XFillPolygon (dpy, w, b->gc, b->points, b->npoints, Convex, CoordModeOrigin);
	XSetForeground (dpy, b->gc, fg);
	XSetBackground (dpy, b->gc, bg);

	XDrawLines (dpy, w, b->gc, b->points, b->npoints, CoordModeOrigin);
	buttonText (w, b, b->gc);
}

static
buttonText (w, b, gc)
Window	w;
struct button	*b;
GC	gc;
{
	XDrawImageString (dpy, w, gc, b->x, b->y, b->text, strlen (b->text));
}
