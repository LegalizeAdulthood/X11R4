/*
 * aquarium.c
 *
 * an xfish derivative.
 *
 * Create a collection of fish shaped windows and move them
 * randomly around the screen
 */

#include <stdio.h>
#include <math.h>
#include <strings.h>
#include <signal.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/extensions/shape.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/timeb.h>

#ifndef AQUARIUM_DIR
#define AQUARIUM_DIR	"/usr/local/lib/aquarium"
#endif

#define DEFAULT_BEASTIE_COUNT	5

#define SWIMMERS    "blimp,eye,face,fork,pirhanna,fish,bob"

#define TOP_X_SPEED	10
#define TOP_Y_SPEED	3

#define DEFAULT_SLEEP		300

#define FACE_LEFT		0
#define FACE_RIGHT		1
#define FACE_NONE		2

#define TURN_CHANCE		4
#define NEW_CHANCE		10
#define RAISE_CHANCE		2
#define LOWER_CHANCE		20

#ifndef FALSE
#define FALSE 0
#define TRUE 1
#endif

typedef struct _Bits {
	unsigned int	width, height;
	int		x_hot, y_hot;
	Pixmap		pixmap;
} BitsRec, *BitsPtr;

typedef struct _Beastie {
	struct _Beastie	*next;
	int		x, y;
	int		dx, dy;
	int		width, height;
	int		x_hot, y_hot;
	int		face;
	int		raised;
	Pixel		fg, bg;
	Window		w;
	BitsRec		bits[4];
} BeastieRec, *BeastiePtr;

#define LEFT_IMAGE	0
#define LEFT_MASK	1
#define RIGHT_IMAGE	2
#define RIGHT_MASK	3

int	max_beasties = DEFAULT_BEASTIE_COUNT;
int	pause_time = DEFAULT_SLEEP;
char	*DisplayName;
char	*ProgramName;

struct _resources {
    int		max_beasties;
    int		pause_time;
    char	*library;
    char	*bitmapFiles;
} app_resources;

static char *beastie_dirs[] = {
    ".", 0, 0
};

#define offset(field)	XtOffset (struct _resources *, field)

static XtResource resources[] = {
    { "count", "Count",	XtRInt, sizeof (int),
	offset (max_beasties), XtRString, "5" },
    { "update", "Update", XtRInt, sizeof (int),
	offset (pause_time), XtRString, "300" },
    { "library", "Library", XtRString, sizeof (char *),
	offset (library), XtRString, AQUARIUM_DIR },
    { "swimmers", "Swimmers", XtRString, sizeof (char *),
	offset (bitmapFiles), XtRString, SWIMMERS },
};
#undef offset

#define offset(field)	XtOffset (BeastiePtr, field)

static XtResource subResources[] = {
    { "foreground", "Foreground", XtRPixel, sizeof (Pixel),
	offset (fg), XtRString, XtDefaultForeground },
    { "background", "Background", XtRPixel, sizeof (Pixel),
	offset (bg), XtRString, XtDefaultBackground },
};

static XrmOptionDescRec options[] = {
{"-count",	"*count",	    XrmoptionSepArg,    NULL},
{"-update",	"*update",	    XrmoptionSepArg,    NULL},
};

BeastiePtr	MakeRandomBeastie(), MakeBeastie();

extern char	*malloc ();

BeastiePtr	pBeasties;
Display		*display;
Window		rootWindow;
int		screen;
int		width, height;
int		depth;
int		fg, bg;
Widget		toplevel;

usage ()
{
    fprintf (stderr,
    "usage: %s [-display <dpy>] [-count <count>] [-update <ms>]\n",
    ProgramName);
    exit (1);
}
main (argc, argv)
char	**argv;
{
    int		i;
    BeastiePtr	pBeastie, pHit, pPrev;
    extern long	time();
    fd_set	reads;
    struct timeval  pause;
    XEvent	xev;
    XButtonEvent    *xbev;
    Window	    hitWin;
    int		    beastie_count;
    
    ProgramName = argv[0];
    /*
     * use Xt as it sets lots of useful stuff for us
     */
    toplevel = XtInitialize (NULL, "Aquarium", options, XtNumber (options),
			     &argc, argv);
    
    if (argc != 1)
    	usage ();
    
    XtGetApplicationResources (toplevel, (caddr_t) &app_resources,
			    	resources, XtNumber (resources),
			    	NULL, (Cardinal) 0);
    
    beastie_dirs[1] = app_resources.library;

    ParseBeastieFiles (app_resources.bitmapFiles);

    display = XtDisplay (toplevel);
    screen = DefaultScreen (display);
    rootWindow = RootWindow (display, screen);
    
    width = DisplayWidth (display, screen);
    height = DisplayHeight (display, screen);
    depth = DisplayPlanes (display, screen);
    srandom ((int) time ((long *) 0));
    for (i = 0; i < app_resources.max_beasties; i++)
    {
    	pBeastie = MakeRandomBeastie ();
    	if (pBeastie)
    	{
	    pBeastie->next = pBeasties;
	    pBeasties = pBeastie;
	    beastie_count++;
    	}
    }
    pBeastie = pBeasties;
    while (pBeasties)
    {
    	FD_ZERO (&reads);
    	FD_SET (ConnectionNumber (display), &reads);
    	pause.tv_sec = app_resources.pause_time / 1000;
    	pause.tv_usec = (app_resources.pause_time % 1000) * 1000;
    	XFlush (display);
    	if (select (ConnectionNumber (display) + 1, &reads, 0, 0, &pause) > 0)
    	{
	    while (XPending (display)) {
	    	XNextEvent (display,&xev);
	    	switch (xev.type)
	    	{
	    	case ButtonPress:
		    xbev = (XButtonEvent *) &xev;
		    pPrev = 0;
		    for (pHit = pBeasties; pHit; pHit=pHit->next)
		    {
		    	if (pHit->w == xbev->window)
			    break;
		    	pPrev = pHit;
		    }
		    if (pHit)
		    {
		    	if (pPrev)
			    pPrev->next = pHit->next;
		    	else
			    pBeasties = pHit->next;
		    	if (pBeastie == pHit)
		    	{
			    pBeastie = pHit->next;
			    if (!pBeastie)
			    	pBeastie = pBeasties;
		    	}
		    	DestroyBeastie (pHit);
		    	--beastie_count;
		    }
		    break;
	    	}
	    }
    	}
    	else
    	{
	    RandomMoveBeastie (pBeastie);
	    pBeastie = pBeastie->next;
	    if (!pBeastie)
	    	pBeastie = pBeasties;
	    if (beastie_count < app_resources.max_beasties &&
	    	random() % 100 < NEW_CHANCE)
	    {
	    	pHit = MakeRandomBeastie ();
	    	if (pHit)
	    	{
		    pHit->next = pBeasties;
		    pBeasties = pHit;
		    beastie_count++;
	    	}
	    }
    	}
    }
}

#define SIZE_BEASTIE_FILES  256
char	*BeastieFiles[SIZE_BEASTIE_FILES + 1];
int	NumBeastieFiles;

ParseBeastieFiles (names)
    char    *names;
{
    char    *new;
    char    *comma;
    int	    len;
    int	    i;

    i = 0;
    do {
	comma = index (names, ',');
	if (comma)
	    len = comma - names;
	else
	    len = strlen (names);
	new = malloc (len + 1);
	strncpy (new, names, len);
	new[len] = '\0';
	names = comma + 1;
	BeastieFiles[i++] = new;
	if (i == SIZE_BEASTIE_FILES)
	    break;
    } while (comma);
    BeastieFiles[i] = 0;
    NumBeastieFiles = i;
}

BeastiePtr
MakeRandomBeastie ()
{
    int		i;
    int		face;
    BeastiePtr  pBeastie;
    
    if (!NumBeastieFiles)
	return 0;
    i = random () % NumBeastieFiles;
    pBeastie = MakeBeastie (BeastieFiles[i]);
    if (pBeastie)
    {
    	RandomPlaceBeastie (pBeastie);
    	face = (random() % 1 ? FACE_RIGHT : FACE_LEFT);
    	FaceBeastie (pBeastie, face);
	RandomBeastieSpeed (pBeastie);
	XLowerWindow (display, pBeastie->w);
	pBeastie->raised = FALSE;
    	XMapWindow (display, pBeastie->w);
    }
    return pBeastie;
}

static char *extensions[] ={
    ".li", ".lm", ".ri", ".rm"
};


BeastiePtr
MakeBeastie (file_base)
	char	*file_base;
{
    int		i, status;
    BeastiePtr	pBeastie;
    char	fileName[1024];
    BitsPtr	bits;
    char	**dir;
    int		x_hot = -1000, y_hot = -1000;
    int		width, height;
    XSetWindowAttributes    xswa;
    
    pBeastie = (BeastiePtr) malloc (sizeof (BeastieRec));
    if (!pBeastie)
	return 0;
    pBeastie->x_hot = -100000;
    pBeastie->y_hot = -100000;
    for (i=0; i<4; i++) {
	bits = &pBeastie->bits[i];
	bits->x_hot = 0;
	bits->y_hot = 0;
	for (dir = beastie_dirs; *dir; dir++) {
	    sprintf (fileName, "%s/%s%s", *dir, file_base, extensions[i]);
	    status = XReadBitmapFile(display, rootWindow, fileName,
				    &bits->width, &bits->height,
				    &bits->pixmap,
				    &bits->x_hot, &bits->y_hot);
	    if (status == BitmapSuccess)
		break;
	    else if (status == BitmapOpenFailed)
		continue;
	    else if (status == BitmapFileInvalid)
		return 0;
	    else if (status == BitmapNoMemory)
		return 0;
	}
	if (!*dir)
	    return 0;
	if (bits->x_hot > pBeastie->x_hot)
	    pBeastie->x_hot = bits->x_hot;
	if (bits->y_hot > pBeastie->y_hot)
	    pBeastie->y_hot = bits->y_hot;
    }
    pBeastie->width = 0;
    pBeastie->height = 0;
    for (i = 0; i < 4; i++)
    {
	bits = &pBeastie->bits[i];
	width = pBeastie->x_hot - bits->x_hot + bits->width;
	height = pBeastie->y_hot - bits->y_hot + bits->height;
	if (width > pBeastie->width)
	    pBeastie->width = width;
	if (height > pBeastie->height)
	    pBeastie->height = height;
    }
    XtGetSubresources (toplevel, (caddr_t) pBeastie, file_base, "Swimmer",
			subResources, XtNumber (subResources),
			NULL, (Cardinal) 0);

    xswa.background_pixel = pBeastie->fg;
    xswa.border_pixel = pBeastie->bg;
    xswa.event_mask = ButtonPress;
    xswa.override_redirect = True;
    pBeastie->w = XCreateWindow
	(display, rootWindow, 0, 0,
	 pBeastie->width, pBeastie->height,
	 0, CopyFromParent, InputOutput,
	 CopyFromParent,
	 CWBackPixel|CWBorderPixel|CWEventMask|CWOverrideRedirect,
	 &xswa);
    pBeastie->x = 0;
    pBeastie->y = 0;
    pBeastie->face = FACE_NONE;
    return pBeastie;
}

DestroyBeastie (pBeastie)
    BeastiePtr	pBeastie;
{
    int		i;

    for (i = 0; i < 4; i++)
    {
	XFreePixmap (display, pBeastie->bits[i].pixmap);
    }
    XDestroyWindow (display, pBeastie->w);
    free ((char *) pBeastie);
}

RandomPlaceBeastie (pBeastie)
    BeastiePtr	pBeastie;
{
    int		x, y;

    x = random() % (width - pBeastie->width);
    y = random() % (height - pBeastie->height);
    PlaceBeastie (pBeastie, x + pBeastie->x_hot, y + pBeastie->y_hot);
}

RandomMoveBeastie (pBeastie)
    BeastiePtr	pBeastie;
{
    int	    i;

    if (random () % 100 < TURN_CHANCE)
    {
	if (pBeastie->face == FACE_LEFT)
	    FaceBeastie (pBeastie, FACE_RIGHT);
	else
	    FaceBeastie (pBeastie, FACE_LEFT);
    }
    if (random () % 100 < TURN_CHANCE)
	pBeastie->dy = -pBeastie->dy;
    if (!pBeastie->raised)
    {
	if (random() % 100 < RAISE_CHANCE)
	{
	    XRaiseWindow (display, pBeastie->w);
	    pBeastie->raised = TRUE;
	}
    }
    else
    {
	if (random() % 100 < LOWER_CHANCE)
	{
	    XLowerWindow (display, pBeastie->w);
	    pBeastie->raised = FALSE;
	}
    }
    MoveBeastie (pBeastie);
}

MoveBeastie (pBeastie)
    BeastiePtr	pBeastie;
{
    int	    newx, newy;
    int	    dx = pBeastie->dx;

    newx = pBeastie->x + (pBeastie->face == FACE_LEFT ? -dx : dx);
    if (newx < 0)
    {
	newx = 0;
	FaceBeastie (pBeastie, FACE_RIGHT);
    }
    if (newx >= width)
    {
	newx = width - 1;
	FaceBeastie (pBeastie, FACE_LEFT);
    }
    newy = pBeastie->y + pBeastie->dy;
    if (newy < 0)
	newy = 0;
    if (newy >= height)
	newy = height - 1;
    PlaceBeastie (pBeastie, newx, newy);
}

PlaceBeastie (pBeastie, x, y)
    BeastiePtr	pBeastie;
    int		x, y;
{
    pBeastie->x = x;
    pBeastie->y = y;
    XMoveWindow (display, pBeastie->w,
		 x - pBeastie->x_hot, y - pBeastie->y_hot);
}

RandomBeastieSpeed (pBeastie)
    BeastiePtr	pBeastie;
{
    int	    dx, dy;

    dx = random() % TOP_X_SPEED + 1;
    dy = random() % (TOP_Y_SPEED * 2 + 1) - TOP_Y_SPEED;
    BeastieSpeed (pBeastie, dx, dy);
}

BeastieSpeed (pBeastie, dx, dy)
    BeastiePtr	pBeastie;
    int		dx, dy;
{
    pBeastie->dx = dx;
    pBeastie->dy = dy;
}

FaceBeastie (pBeastie, face)
    BeastiePtr	pBeastie;
    int		face;
{
    BitsPtr pBounding, pClip;

    if (pBeastie->face != face)
    {
	pBeastie->face = face;
	if (face == FACE_LEFT)
	{
	    pBounding = &pBeastie->bits[LEFT_MASK];
	    pClip = &pBeastie->bits[LEFT_IMAGE];
	}
	else
	{
	    pBounding = &pBeastie->bits[RIGHT_MASK];
	    pClip = &pBeastie->bits[RIGHT_IMAGE];
	}
	ShapeBeastie (pBeastie, pBounding, pClip);
    }
}

ShapeBeastie (pBeastie, pBounding, pClip)
    BeastiePtr	pBeastie;
    BitsPtr	pBounding, pClip;
{
    XShapeCombineMask (display, pBeastie->w, ShapeBounding,
		       pBeastie->x_hot - pBounding->x_hot,
		       pBeastie->y_hot - pBounding->y_hot,
		       pBounding->pixmap, ShapeSet);
    XShapeCombineMask (display, pBeastie->w, ShapeClip,
		       pBeastie->x_hot - pClip->x_hot,
		       pBeastie->y_hot - pClip->y_hot,
		       pClip->pixmap, ShapeSet);
}
