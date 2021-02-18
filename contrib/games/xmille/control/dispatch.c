/*
 * dispatch.c
 *
 * pass out X events to appropriate recipiants
 */

# include	<X11/Xlib.h>

extern char	*malloc ();

struct eventGroup {
	struct eventGroup	*prev;
	Window			w;
	unsigned long		e;
	int			(*f)();
};

int	eventToMask[] = {
	0,
	0,
	KeyPressMask,
	KeyReleaseMask,
	ButtonPressMask,
	ButtonReleaseMask,
	PointerMotionMask | Button1MotionMask | Button2MotionMask
		| Button3MotionMask | Button4MotionMask | Button5MotionMask
		| ButtonMotionMask,
	EnterWindowMask,
	LeaveWindowMask,
	FocusChangeMask,
	FocusChangeMask,
	KeymapStateMask,
	ExposureMask,
			0,
	VisibilityChangeMask,
	SubstructureNotifyMask,
	StructureNotifyMask | SubstructureNotifyMask,
	StructureNotifyMask | SubstructureNotifyMask,
	StructureNotifyMask | SubstructureNotifyMask,
	SubstructureRedirectMask,
	StructureNotifyMask | SubstructureNotifyMask,
	StructureNotifyMask | SubstructureNotifyMask,
	SubstructureRedirectMask,
	StructureNotifyMask | SubstructureNotifyMask,
	ResizeRedirectMask,
	StructureNotifyMask | SubstructureNotifyMask,
	SubstructureRedirectMask,
	PropertyChangeMask,
			0,
			0,
			0,
	ColormapChangeMask,
	OwnerGrabButtonMask,
			0,
			0
};


struct eventGroup	*eventStack, *allocGroup();
extern	Display		*dpy;

/*
 * actually allow an event to occur
 */

bindEvent (window, eventMask, func)
Window		window;
unsigned long	eventMask;
int		(*func)();
{
	struct eventGroup	*g;

	g = allocGroup ();
	g->w = window;
	g->e = eventMask;
	g->f = func;
	g->prev = eventStack;
	eventStack = g;
	dequeueEvents (window, eventMask, func);
	interestEvents (window, eventMask);
}

/*
 * express interest in events
 */

struct interest {
	struct interest	*next;
	Window		window;
	unsigned long	eventMask;
};

struct interest	*interestQueue;

interestEvents (window, eventMask)
Window		window;
unsigned long	eventMask;
{
	struct interest	*q;

	for (q = interestQueue; q; q = q->next)
		if (q->window == window)
			break;
	if (!q) {
		q = (struct interest *) malloc (sizeof (struct interest));
		q->next = interestQueue;
		q->window = window;
		q->eventMask = 0;
		interestQueue = q;
	}
	if ((q->eventMask | eventMask) != q->eventMask) {
		q->eventMask |= eventMask;
		XSelectInput (dpy, window, q->eventMask);
	}
}

noInterestEvents (window)
Window		window;
{
	struct interest	*q, *p;

	p = 0;
	for (q = interestQueue; q; q = q->next) 
		if (q->window == window) {
			if (p)
				p->next = q->next;
			else
				interestQueue = q->next;
			free ((char *) q);
			return;
		}
}

interested (window, type)
Window	window;
int	type;
{
	struct interest	*q;

	for (q = interestQueue; q; q = q->next)
		if (q->window == window && q->eventMask | type)
			return 1;
	return 0;
}

unbindEvent (window, eventMask)
Window		window;
unsigned long	eventMask;
{
	struct eventGroup	*g, *n, *p;
	unsigned long		t;

	n = 0;
	for (g = eventStack; g; g = p) {
		p = g->prev;
		if (g->w == window) {
 			if (g->e & eventMask) {
				t = eventMask;
				eventMask &= ~g->e;
				g->e &= ~t;
			}
			if (g->e == 0) {
				if (n)
					n->prev = p;
				else
					eventStack = p;
				freeGroup (g);
			}
		}
	}
}

sendEvent (rep)
XAnyEvent	*rep;
{
	struct eventGroup	*g;
	int	type;
	
	type = eventToMask[rep->type];
	for (g = eventStack; g; g = g->prev) {
		if (rep->window == g->w && (type & g->e)) {
			g->f (rep);
			return 1;
		}
	}
	return 0;
}

struct eventQueue {
	struct eventQueue	*next;
	int			type;
	XEvent			event;
};

struct eventQueue	*head, *tail;

dequeueEvents (w, eventMask, func)
Window		w;
unsigned long	eventMask;
int		(*func)();
{
	struct eventQueue	*q, *n, *p;

	p = 0;
	for (q = head; q; q = n) {
		n = q->next;
		if (q->event.xany.window == w && (q->type & eventMask)) {
			func (&q->event);
			if (p)
				p->next = q->next;
			else
				head = q->next;
			if (q == tail)
				tail = p;
			free ((char *) q);
		} else
			p = q;
	}
}

queueEvent (rep)
XEvent	*rep;
{
	struct eventQueue	*q;
	int			type;

	type = eventToMask [rep->type];
	if (interested (rep->xany.window, type)) {
		q = (struct eventQueue *) malloc (sizeof (struct eventQueue));
		q->event = *rep;
		q->next = 0;
		q->type = type;
		if (tail) 
			tail->next = q;
		else
			head = q;
		tail = q;
	}
}

int
noFunc (rep)
XEvent	*rep;
{
	return;
}

flushEvents (w, eventMask)
Window		w;
unsigned long	eventMask;
{
	dequeueEvents (w, eventMask, noFunc);
}

dispatch ()
{
	XEvent	event;

	XNextEvent (dpy, &event);
	if (!sendEvent (&event))
		queueEvent (&event);
}

static struct eventGroup *
allocGroup ()
{
	char	*malloc ();

	return (struct eventGroup *) malloc (sizeof (struct eventGroup));
}

freeGroup (g)
struct eventGroup	*g;
{
	free ((char *) g);
}
