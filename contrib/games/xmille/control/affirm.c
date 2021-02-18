/*
 * affirm.c
 *
 * affirm an action (yes/no)
 */

# include	<X11/Xlib.h>
# include	"control.h"
# include	"co_class.h"
# include	<sys/time.h>
# include	<assert.h>

/*
 *	affirm window on screen
 */

# define AFFIRM_Y	350
# define AFFIRM_X	200
# define AFFIRM_HP	50	/* padding */
# define MIN_AFFIRM_W	(CANCEL_X + 80)
# define LABEL_Y	25

# define OK_X		(10)
# define OK_Y		(AFFIRM_HP)
# define CANCEL_X	(OK_X + 100)
# define CANCEL_Y	(OK_Y)

# define TEXT_OFF	5

extern int	co_inited;
static char	*label;
static int	affirm_h, affirm_w, label_x;
static Window	affirm, ok, cancel;
extern	GC	co_fore_gc;
static int	OKstate, CANCELstate;

int
co_affirm(title, parent)
char	*title;
Window	parent;
{
	XEvent		rep;
	int		state, affirm_x;
	int		label_w;
	int		co_OKstate (), co_CANCELstate (), co_affirmEvent (), CmanageButton ();
	int		mask;

	if (!co_inited)
		co_init();
	label = title;
	label_w = XTextWidth(co_font, label, strlen(label));

	if (MIN_AFFIRM_W+10 >= label_w)
		affirm_w = MIN_AFFIRM_W+10;
	else
		affirm_w = label_w + 10;

	OKstate = 0;
	CANCELstate = 0;
	label_x = affirm_w / 2 - label_w / 2;
	affirm_h = CANCEL_Y + 60;
	affirm_x = AFFIRM_X - affirm_w/2;
	affirm = XCreateSimpleWindow (dpy, parent, affirm_x, AFFIRM_Y,
			affirm_w, affirm_h, 1, co_border, co_background);
	
	ok = CmapButton (affirm, OK_X, OK_Y, co_OK, co_OKstate);
	cancel = CmapButton (affirm, CANCEL_X, CANCEL_Y, co_CANCEL, co_CANCELstate);
	bindEvent (affirm, ExposureMask, co_affirmEvent);
	bindEvent (ok, ExposureMask|ButtonPressMask|ButtonReleaseMask|
			Button1MotionMask|Button2MotionMask|Button3MotionMask|LeaveWindowMask, CmanageButton);
	bindEvent (cancel, ExposureMask|ButtonPressMask|ButtonReleaseMask|
			Button1MotionMask|Button2MotionMask|Button3MotionMask|LeaveWindowMask,
			CmanageButton);
	XMapWindow (dpy, affirm);

	while (!OKstate && !CANCELstate) {
		dispatch ();
	}

	unbindEvent (affirm, ExposureMask);
	unbindEvent (ok, ExposureMask|ButtonPressMask|ButtonReleaseMask|
			Button1MotionMask|Button2MotionMask|Button3MotionMask|LeaveWindowMask);
	unbindEvent (cancel, ExposureMask|ButtonPressMask|ButtonReleaseMask|
			Button1MotionMask|Button2MotionMask|Button3MotionMask|LeaveWindowMask);
	XUnmapWindow (dpy, affirm);
	CunmapButton (cancel);
	CunmapButton (ok);
	XDestroyWindow (dpy, affirm);
	noInterestEvents (affirm);
	noInterestEvents (cancel);
	noInterestEvents (ok);
	if (CANCELstate)
		return 0;
	if (OKstate)
		return 1;
}

co_affirmEvent (rep)
XEvent	*rep;
{
	switch (rep->type) {
	case Expose:
		redisplayLabel ();
	}
}

co_OKstate (n)
{
	OKstate = 1;
}

co_CANCELstate (n)
{
	CANCELstate = 1;
}

static
redisplayLabel ()
{
	XDrawImageString (dpy, affirm, co_fore_gc, label_x, LABEL_Y,
			  label, strlen (label));
	XFlush (dpy);
}
