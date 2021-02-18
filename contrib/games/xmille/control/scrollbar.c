/*
 *	scrollbar.c
 */
#if 0
# include	<X11/Xlib.h>
# include	<X11/Xutil.h>
# include	"control.h"

struct scrollbar {
	Scrollbar	scrollbar;
	int		forecolor, backcolor;
	int		orientation;
	int		borderwidth;
	Pixmap		border;
	Pixmap		background;
};

struct perwindow {
	int			length;
	Window			parent;
	int			locatorpos;
	int			locatormille;
	int			locatormax;
	int			locatormaxmille;
	int			state;
	struct scrollbar	*class;
	int			(*notify)();
	int			(*onetrip)();
};

/*
 *	possible values of ->state
 */	

# define GENERAL		0
# define DRAGGING_LOCATOR	1
# define DOWN			2
# define DOWN_IN_DOWN_BOX	3
# define DOWN_IN_UP_BOX		4

static XContext	perwindowContext;

static XContext	assocContext;
static int		nextScrollbar;
extern	Display	*dpy;

# define minpos(p)		(0)
# define maxpos(p)		((p)->length - 2 * ARROWWIDTH)
# define milletopos(m,p)	((m) * maxpos(p) / 1000)
# define postomille(l,p)	((l) * 1000 / maxpos (p))
# define postoloc(l)		((l) + ARROWWIDTH)
# define loctopos(l)		((l) - ARROWWIDTH)

# define ARROWHEIGHT	SCROLLWIDTH
# define ARROWWIDTH	SCROLLWIDTH
# define LOCATORWIDTH	SCROLLWIDTH / 3

static short arrowDownOff[50] = {
	0xffff, 0x01ff,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0xff80, 0x0003,
	0x0080, 0x0002,
	0x0080, 0x0002,
	0x0080, 0x0002,
	0x0080, 0x0002,
	0x0080, 0x0002,
	0x0080, 0x0002,
	0x0080, 0x0002,
	0x00f8, 0x003e,
	0x0010, 0x0010,
	0x0020, 0x0008,
	0x0040, 0x0004,
	0x0080, 0x0002,
	0x0100, 0x0001,
	0x8200, 0x0000,
	0x4400, 0x0000,
	0x2800, 0x0000,
	0x1000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
};

static short arrowDownOn[50] = {
	0xffff, 0x01ff,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0xff80, 0x0003,
	0xff80, 0x0003,
	0xff80, 0x0003,
	0xff80, 0x0003,
	0xff80, 0x0003,
	0xff80, 0x0003,
	0xff80, 0x0003,
	0xff80, 0x0003,
	0xfff8, 0x003f,
	0xfff0, 0x001f,
	0xffe0, 0x000f,
	0xffc0, 0x0007,
	0xff80, 0x0003,
	0xff00, 0x0001,
	0xfe00, 0x0000,
	0x7c00, 0x0000,
	0x3800, 0x0000,
	0x1000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
	0x0000, 0x0000,
};

static short arrowUpOff [50], arrowRightOff [50], arrowLeftOff [50];
static short arrowUpOn  [50], arrowRightOn  [50], arrowLeftOn  [50];

static Cursor	dragCursor;
static short	dragBits[16];

static int arrowsSet;

Scrollbar
CcreateScrollbar (orientation, forecolor, backcolor,
	 borderwidth, border, background)
int	orientation;
int	forecolor, backcolor;
int	borderwidth;
Pixmap	border;
Pixmap	background;
{
	char			*malloc ();
	struct scrollbar	*s;
	
	if (!assocContext)
		assocContext = XUniqueContext();
	if (!arrowsSet) {
		setArrows();
		arrowsSet = 1;
	}
	s = (struct scrollbar *) malloc (sizeof (struct scrollbar));
	nextScrollbar++;
	s->scrollbar = nextScrollbar;
	XSaveContext (dpy, assocContext, nextScrollbar, (char *) s);
	s->forecolor = forecolor;
	s->backcolor = backcolor;
	s->orientation = orientation;
	s->borderwidth = borderwidth;
	s->border = border;
	s->background = background;
	return s->scrollbar;
}

Window
CmapScrollbar (parent, scrollbar, notify, onetrip)
Window		parent;
Scrollbar	scrollbar;
int		(*notify)(), (*onetrip)();
{
	struct scrollbar	*s;
	struct perwindow	*p;
	Window			w;
	int			x, y, width, height;
	Window			grandParent;
	Window			*siblings;
	int			nsiblings;
	
	if( XFindContext (dpy, assocContext, scrollbar, &s) )
	{
		printf ("scroll bar %d doesn't exist\n", scrollbar);
		return 0;
	}
	if( !XQueryTree (parent, &grandParent, &nsiblings, &siblings)) {
		printf ("can't get tree\n");
		return 0;
	}
	if (siblings)
		free (siblings);
	if (!computePosition (parent, s, &x, &y, &width, &height))
		return 0;
	w = XCreateWindow (grandParent, x, y, width, height, s->borderwidth,
		s->border, s->background);
	XMapWindow (w);
	if (!perwindowContext)
		perwindowContext = XUniqueContext();
	p = (struct perwindow *) malloc (sizeof (struct perwindow));
	switch (s->orientation) {
	case SCROLL_TOP:
	case SCROLL_BOTTOM:
		p->length = width;
		break;
	case SCROLL_RIGHT:
	case SCROLL_LEFT:
		p->length = height;
		break;
	}
	p->state = GENERAL;
	p->parent = parent;
	p->class = s;
	p->locatorpos = -1;
	p->notify = notify;
	p->onetrip = onetrip;
	XSaveContext (dpy, perwindowContext, w, p);
	drawArrows (w, s, p);
	return w;
}

CunmapScrollbar (w)
Window		w;
{
	struct perwindow	*p;

	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	XDeleteContext (dpy, perwindowContext, w);
	free (p);
	XDestroyWindow (w);
	return 1;
}

CresizeScrollbar (w)
Window		w;
{
	struct perwindow	*p;
	struct scrollbar	*s;
	int			x, y, width, height;

	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	s = p->class;
	if (!computePosition (p->parent, s, &x, &y, &width, &height))
		return 0;
	XConfigureWindow (w, x, y, width, height);
	switch (s->orientation) {
	case SCROLL_TOP:
	case SCROLL_BOTTOM:
		p->length = width;
		break;
	case SCROLL_RIGHT:
	case SCROLL_LEFT:
		p->length = height;
		break;
	}
	if (p->locatorpos >= 0)
		p->locatorpos = milletopos (p->locatormille, p);
	if (p->locatormax >= 0)
		p->locatormax = milletopos (p->locatormaxmille, p);
	return 1;
}

Window
CgetScrollbarParent (w)
Scrollbar	w;
{
	struct perwindow	*p;
	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	return p->parent;
}

CredrawScrollbar (w)
Window		w;
{
	struct scrollbar	*s;
	struct perwindow	*p;

	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	s = p->class;
	drawArrows (w, s, p);
	moveLocator (w, s, p, p->locatorpos, p->locatormax);
	return 1;
}

CsetScrollbarLocator (w, permillemin, permillemax)
Window		w;
int		permillemin, permillemax;
{
	struct perwindow	*p;
	struct scrollbar	*s;

	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	s = p->class;
	if (permillemin < 0)
		permillemin = 0;
	if (permillemax > 1000)
		permillemax = 1000;
	moveLocator (w, s, p,
		milletopos (permillemin, p),
 		milletopos (permillemax, p));
	return 1;
}

# define NOWHERE		0
# define IN_UP_BOX		1
# define IN_DOWN_BOX		2
# define IN_UP_AREA		3
# define IN_DOWN_AREA		4
# define IN_LOCATOR		5

/*
 * used for dragging the locator around
 */

static int	downmouse;	/* mouse position at down click */
static int	downloc;	/* locator position at down click */

CmanageScrollbar (rep)
XEvent		*rep;
{
	struct perwindow	*p;
	struct scrollbar	*s;
	int			inwindow;
	int			region;
	XButtonEvent		*bevent;
	XMouseMovedEvent	*mevent;
	int			permille;
	Window			w;

	w = rep->window;
	if( XFindContext (dpy, perwindowContext, w, &p) )
		return 0;
	s = p->class;
	switch (rep->type) {
	case ExposeWindow:
		CredrawScrollbar (w);
		break;
	case ButtonPressed:
		switch (p->state) {
		case GENERAL:
			bevent = (XButtonEvent *) rep;
			region = findRegion (s, p, bevent->x, bevent->y);
			switch (region) {
			case IN_LOCATOR:
				p->state = DRAGGING_LOCATOR;
				downloc = p->locatorpos;
				switch (s->orientation) {
				case SCROLL_TOP:
				case SCROLL_BOTTOM:
					downmouse = bevent->x;
					break;
				case SCROLL_LEFT:
				case SCROLL_RIGHT:
					downmouse = bevent->y;
					break;
				}
				break;
			case IN_UP_BOX:
				p->state = DOWN_IN_UP_BOX;
				highlightBox (w, s, p, p->state);
				spinbutton (w, s, p, IN_UP_BOX);
				break;
			case IN_DOWN_BOX:
				p->state = DOWN_IN_DOWN_BOX;
				highlightBox (w, s, p, p->state);
				spinbutton (w, s, p, IN_DOWN_BOX);
				break;
			case IN_DOWN_AREA:
			case IN_UP_AREA:
				p->state = DOWN;
				switch (region) {
				case IN_DOWN_AREA:
					p->notify (p->parent, SCROLL_DOWN_AREA);
					break;
				case IN_UP_AREA:
					p->notify (p->parent, SCROLL_UP_AREA);
					break;
				}
			}
		}
		break;
	case ButtonReleased:
		switch (p->state) {		
		case DRAGGING_LOCATOR:
			p->state = GENERAL;
			break;
		case DOWN_IN_DOWN_BOX:
		case DOWN_IN_UP_BOX:
			unhighlightBox (w, s, p, p->state);
		case DOWN:
			p->state = GENERAL;
			break;
		}
	case MouseMoved:
		switch (p->state) {
		case DRAGGING_LOCATOR:
			p->notify (p->parent, updateLocator (w, s, p));
			p->onetrip (p->parent);
			break;
		}
		break;
	}
}

static
spinbutton (w, s, p, region)
Window			w;
struct scrollbar	*s;
struct perwindow	*p;
int			region;
{
	int	r;

	r = region;
	for (;;) {
		switch (p->state) {
		case DOWN_IN_UP_BOX:
		case DOWN_IN_DOWN_BOX:
			if (r != region) {
				unhighlightBox (w, s, p, p->state);
				p->state = GENERAL;
				return;
			}
			switch (region) {
			case IN_UP_BOX:
				p->notify (p->parent, SCROLL_UP_BUTTON);
				p->onetrip (p->parent);
				break;
			case IN_DOWN_BOX:
				p->notify (p->parent, SCROLL_DOWN_BUTTON);
				p->onetrip (p->parent);
				break;
			}
			break;
		default:
			return;
		}
		r = checkMouseRegion (w, s, p);
	}
}

static
findRegion (s, p, x, y)
struct scrollbar	*s;
struct perwindow	*p;
int			x,y;
{
	int	loc;
	switch (s->orientation) {
	case SCROLL_TOP:
	case SCROLL_BOTTOM:
		if (y < 0 || SCROLLWIDTH <= y)
			return NOWHERE;
		loc = x;
		break;
	case SCROLL_LEFT:
	case SCROLL_RIGHT:
		if (x < 0 || SCROLLWIDTH <= x)
			return NOWHERE;
		loc = y;
		break;
	}
	if (loc < 0 || loc >= p->length)
		return NOWHERE;
	if (loc <= ARROWWIDTH)
		return IN_UP_BOX;
	if (loc >= p->length - ARROWWIDTH)
		return IN_DOWN_BOX;
	if (loc < postoloc (p->locatorpos))
		return IN_UP_AREA;
	if (loc > postoloc (p->locatormax))
		return IN_DOWN_AREA;
	return IN_LOCATOR;
}

static
updateLocator (w, s, p)
Window			w;
struct scrollbar	*s;
struct perwindow	*p;
{
	int	pos, loc;
	int	subw, x, y;
	int	len;

	XUpdateMouse (w, &x, &y, &subw);
	switch (s->orientation) {
	case SCROLL_TOP:
	case SCROLL_BOTTOM:
		loc = x;
		break;
	case SCROLL_LEFT:
	case SCROLL_RIGHT:
		loc = y;
		break;
	}
	pos = loctopos (postoloc (downloc) + loc - downmouse);
	if (pos < minpos(p))
		pos = minpos(p);
	if (pos > maxpos(p))
		pos = maxpos (p);
	return postomille (pos, p);
}

static
checkMouseRegion (w, s, p)
Window			w;
struct scrollbar	*s;
struct perwindow	*p;
{
	int	x, y, subw, state;
	int	region;
	
	XQueryMouseButtons (w, &x, &y, &subw, &state);

	/*
	 * Note that if we are playing back a recording then the button
	 * state will not be as it was during the recording (pressed),
	 * so we short circuit it.
	 */

	region = findRegion (s, p, x, y);
	if (region == IN_UP_BOX && p->state == DOWN_IN_UP_BOX)
		return IN_UP_BOX;
	if (region == IN_DOWN_BOX && p->state == DOWN_IN_DOWN_BOX)
		return IN_DOWN_BOX;
	return NOWHERE;
}

static
computePosition (parent, s, xp, yp, wp, hp)
Window			parent;
struct scrollbar	*s;
int			*xp, *yp, *wp, *hp;
{
	WindowInfo	parentInfo;

	/* make sure the server has the most recent parent info */
	XSync (0);
	if (!XQueryWindow (parent, &parentInfo)) {
		printf ("can't get window info for %d\n", parent);
		return 0;
	}
	switch (s->orientation) {
	case SCROLL_TOP:
		*xp = 0;
		*yp = -SCROLLWIDTH - parentInfo.bdrwidth;
		*wp = parentInfo.width;
		*hp = SCROLLWIDTH;
		break;
	case SCROLL_BOTTOM:
		*xp = 0;
		*yp = parentInfo.height + parentInfo.bdrwidth; 
		*wp = parentInfo.width;
		*hp = SCROLLWIDTH;
		break;
	case SCROLL_LEFT:
		*xp = -SCROLLWIDTH - parentInfo.bdrwidth;
		*yp = 0;
		*wp = SCROLLWIDTH;
		*hp = parentInfo.height;
		break;
	case SCROLL_RIGHT:
		*xp = parentInfo.width + parentInfo.bdrwidth;
		*yp = 0;
		*wp = SCROLLWIDTH;
		*hp = parentInfo.height;
		break;
	}
	*xp += parentInfo.x;
	*yp += parentInfo.y;
	return 1;
}

static
drawArrows (w, s, p)
Window			w;
struct scrollbar	*s;
struct perwindow	*p;
{
	int	x, y;
	short	*up, *down;
	
	switch (s->orientation) {
	case SCROLL_TOP:
	case SCROLL_BOTTOM:
		up = arrowLeftOff;
		down = arrowRightOff;
		x = p->length - ARROWWIDTH;
		y = 0;
		switch (p->state) {
		case DOWN_IN_DOWN_BOX:
			down = arrowRightOn;
			break;
		case DOWN_IN_UP_BOX:
			up = arrowLeftOn;
			break;
		}
		break;
	case SCROLL_LEFT:
	case SCROLL_RIGHT:
		up = arrowUpOff;
		down = arrowDownOff;
		x = 0;
		y = p->length - ARROWWIDTH;
		switch (p->state) {
		case DOWN_IN_DOWN_BOX:
			down = arrowDownOn;
			break;
		case DOWN_IN_UP_BOX:
			up = arrowUpOn;
			break;
		}
		break;
	}
	XBitmapBitsPut (w, 0, 0,
		ARROWWIDTH, ARROWHEIGHT,
		up, s->forecolor, s->backcolor,
		0, GXcopy, AllPlanes);
	XBitmapBitsPut (w, x, y,
		ARROWWIDTH, ARROWHEIGHT,
		down, s->forecolor, s->backcolor,
		0, GXcopy, AllPlanes);
}

static
highlightBox (w, s, p, state)
Window			w;
struct scrollbar	*s;
struct perwindow	*p;
int			state;
{
	drawoneArrow (w, s, p, state, 1);
}

static
unhighlightBox (w, s, p, state)
Window			w;
struct scrollbar	*s;
struct perwindow	*p;
int			state;
{
	drawoneArrow (w, s, p, state, 0);
}

static
drawoneArrow (w, s, p, state, on)
Window			w;
struct scrollbar	*s;
struct perwindow	*p;
int			state;
int			on;
{
	int	x, y;
	short	*bitsup, *bitsdown, *bits;
	switch (s->orientation) {
	case SCROLL_TOP:
	case SCROLL_BOTTOM:
		x = p->length - ARROWWIDTH;
		y = 0;
		if (on) {
			bitsup = arrowLeftOn;
			bitsdown = arrowRightOn;
		} else {
			bitsup = arrowLeftOff;
			bitsdown = arrowRightOff;
		}
		break;
	case SCROLL_LEFT:
	case SCROLL_RIGHT:
		x = 0;
		y = p->length - ARROWWIDTH;
		if (on) {
			bitsup = arrowUpOn;
			bitsdown = arrowDownOn;
		} else {
			bitsup = arrowUpOff;
			bitsdown = arrowDownOff;
		}
		break;
	}
	switch (state) {
	case DOWN_IN_UP_BOX:
		x = 0;
		y = 0;
		bits = bitsup;
		break;
	case DOWN_IN_DOWN_BOX:
		bits = bitsdown;
		break;
	default:
		return;
	}
	XBitmapBitsPut (w, x, y, ARROWWIDTH, ARROWHEIGHT,
		bits, s->forecolor, s->backcolor,
		0, GXcopy, AllPlanes);
}

static
moveLocator (w, s, p, pos, max)
Window			w;
struct scrollbar	*s;
struct perwindow	*p;
int			pos, max;
{
	int	x, y, lw, lh;
	int	x0, y0, w0, h0;
	int	x1, y1, w1, h1;
	int	oldpos, oldmax, oldlen;

	if (max > maxpos (p))
		max = maxpos (p);
	if (max < 0)
		max = 0;
	if (pos > maxpos (p))
		pos = maxpos (p);
	if (pos < 0)
		pos = 0;
	oldpos = p->locatorpos;
	oldmax = p->locatormax;
	oldlen = oldmax - oldpos;
	w0 = h0 = w1 = h1 = 0;
	switch (s->orientation) {
	case SCROLL_TOP:
	case SCROLL_BOTTOM:
		x = postoloc (pos);
		y = SCROLLWIDTH / 2 - LOCATORWIDTH / 2;
		y0 = y;
		y1 = y;
		lh = LOCATORWIDTH;
		lw = max - pos;
		h0 = lh;
		h1 = lh;
		if (pos < 0) {
			/*
			 * erase the whole thing
			 */
			x0 = postoloc (oldpos);
			w0 = oldlen;
		} else if (p->locatorpos < 0) {
			;
		} else {
			if (pos > oldpos) {
				/*
				 * erase before portion
				 */
				x0 = postoloc (oldpos);
				w0 = pos - oldpos;
			}
			if (max < oldmax) {
				/* 
				 * erase after portion
				 */
				x1 = postoloc (max);
				w1 = oldmax - max;
			}
		}
		break;
	case SCROLL_LEFT:
	case SCROLL_RIGHT:
		x = SCROLLWIDTH / 2 - LOCATORWIDTH / 2;
		y = postoloc (pos);
		x0 = x;
		x1 = x;
		lw = LOCATORWIDTH;
		lh = max - pos;
		w0 = lw;
		w1 = lw;
		if (pos < 0) {
			/*
			 * erase the whole thing
			 */
			y0 = postoloc (oldpos);
			h0 = oldlen;
		} else if (p->locatorpos < 0) {
			;
		} else {
			if (pos > oldpos) {
				/*
				 * erase before portion
				 */
				y0 = postoloc (oldpos);
				h0 = pos - oldpos;
			}
			if (max < oldmax) {
				/*
				 * erase after portion
				 */
				y1 = postoloc (max);
				h1 = oldmax - max;
			}
		}
		break;
	}
	if (pos >= 0) {
		Vertex v[5];

		v[0].x = x;	v[0].y = y;	v[0].flags = 0;
		v[1].x = lw-1;	v[1].y = 0;	v[1].flags = VertexRelative;
		v[2].x = 0;	v[2].y = lh-1;	v[2].flags = VertexRelative;
		v[3].x = 1-lw;	v[3].y = 0;	v[3].flags = VertexRelative;
		v[4].x = 0;	v[4].y = 1-lh;	v[4].flags = VertexRelative;
		XDraw (w, v, 5, 1, 1, s->forecolor, GXcopy, AllPlanes);
		XPixSet (w, x+1, y+1, lw-2, lh-2, s->backcolor);
	}
	if (oldpos >= 0) {
		if (w0 != 0 && h0 != 0)
			XTileSet (w, x0, y0, w0, h0, s->background);
		if (w1 != 0 && h1 != 0)
			XTileSet (w, x1, y1, w1, h1, s->background);
	}
	p->locatorpos = pos;
	p->locatormax = max;
	p->locatormille = postomille (p->locatorpos, p);
	p->locatormaxmille = postomille (p->locatormax, p);
}

# define getbit(array, bit, word) ((word < 0 || word >= 50) ? 0 : ((array[word] >> bit) & 01))

static
setArrows ()
{
	int	i;
	int	word, bit;
	unsigned short	tempLeftOn[2], tempRightOn[2];
	unsigned short	tempLeftOff[2], tempRightOff[2];
	int	on, off;
	Bitmap	dragmap;
	int	k;
	
	for (i = 0; i < ARROWHEIGHT; i++)
		for (k = 0; k < 2; k++) {
			arrowUpOff[i*2+k] = arrowDownOff[(ARROWHEIGHT-1-i)*2 + k];
			arrowUpOn[i*2+k] = arrowDownOn[(ARROWHEIGHT-1-i)*2 + k];
		}
	for (i = 0; i < ARROWHEIGHT; i++) {
		for (k = 0; k < 2; k++) {
			tempLeftOn[k] = tempLeftOff[k] = 0;
			tempRightOn[k] = tempRightOff[k] = 0;
		}
		for (k = 0; k < 2; k++) {
			for (bit = 0; bit < 16; bit++) {
				off = getbit (arrowDownOff,
					i % 16, ((15-bit) + (1-k) * 16) * 2 + i/16);
				on = getbit (arrowDownOn,
					i % 16, ((15-bit) + (1-k) * 16) * 2 + i/16);
				if (off)
					tempRightOff[1-k] |= (1 << (15 - bit));
				if (on)
					tempRightOn[1-k] |= (1 << (15 - bit));
				off = getbit (arrowUpOff,
					i % 16, ((15-bit) + (1-k) * 16) * 2 + i/16);
				on = getbit (arrowUpOn,
					i % 16, ((15-bit) + (1-k) * 16) * 2 + i/16);
				if (off)
					tempLeftOff[1-k] |= (1 << (15 - bit));
				if (on)
					tempLeftOn[1-k] |= (1 << (15 - bit));
			}
		}
		arrowLeftOn[i*2] = tempLeftOn[0];
		arrowLeftOn[i*2+1] = tempLeftOn[1];
		arrowLeftOff[i*2] = tempLeftOff[0];
		arrowLeftOff[i*2+1] = tempLeftOff[1];
		arrowRightOn[i*2] = tempRightOn[0];
		arrowRightOn[i*2+1] = tempRightOn[1];
		arrowRightOff[i*2] = tempRightOff[0];
		arrowRightOff[i*2+1] = tempRightOff[1];
	}
	dragmap = XStoreBitmap (16, 16, dragBits);
	dragCursor = XStoreCursor (dragmap, dragmap, 8, 8, 1, 0, GXnoop);
}
#endif
