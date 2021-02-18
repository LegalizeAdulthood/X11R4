#include <X11/copyright.h>

/* $XConsortium: Bar.c,v 1.2 88/10/25 17:40:25 swick Exp $ */
/* Copyright	Massachusetts Institute of Technology	1987, 1988 */

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include "BarP.h"

static Dimension def_dimension	= ~0;
static XtOrientation def_orient = XtorientHorizontal;
#define DEF_LENGTH  ((Dimension) 200)
#define DEF_THICKNESS	((Dimension) 15)

static XtResource resources[] = {
#define offset(field) XtOffset(BarWidget, bar.field)
#define Offset(field) XtOffset(BarWidget, field)
    /* {name, class, type, size, offset, default_type, default_addr}, */
    /*
     * We define these core class values to set defaults, so we can
     * initialize correctly
     */
    {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
	Offset(core.width), XtRDimension, (caddr_t)&def_dimension},
    {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
	Offset(core.height), XtRDimension, (caddr_t)&def_dimension},
    /* Private bar widget resources */
    {XtNlength, XtCLength, XtRDimension, sizeof(Dimension),
	offset(length), XtRDimension, (caddr_t) &def_dimension},
    {XtNthickness, XtCThickness, XtRDimension, sizeof(Dimension),
	offset(thickness), XtRDimension, (caddr_t) &def_dimension},
    {XtNorientation, XtCOrientation, XtROrientation, sizeof(XtOrientation),
	offset(orientation), XtROrientation, (caddr_t) &def_orient},
#undef offset
#undef Offset
};

static void ClassInitialize();
static void Initialize();
static void Resize();
static void Redisplay();

BarClassRec barClassRec = {
  { /* core fields */
    /* superclass		*/	(WidgetClass) &widgetClassRec,
    /* class_name		*/	"Bar",
    /* widget_size		*/	sizeof(BarRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	XtInheritRealize,
    /* actions			*/	NULL,
    /* num_actions		*/	0,
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULLQUARK,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	NULL,
    /* resize			*/	Resize,
    /* expose			*/	Redisplay,
    /* set_values		*/	NULL,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	XtInheritSetValuesAlmost,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	NULL,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
  },
  { /* bar fields */
    /* gray			*/	NULL
  }
};

WidgetClass barWidgetClass = (WidgetClass)&barClassRec;

static char gray0_bits[] = {
   0x00, 0x00, 0x00};
static char gray1_bits[] = {
   0x00, 0x02, 0x00};
static char gray2_bits[] = {
   0x00, 0x03, 0x00};
static char gray3_bits[] = {
   0x00, 0x03, 0x02};
static char gray4_bits[] = {
   0x00, 0x07, 0x02};
static char gray5_bits[] = {
   0x04, 0x07, 0x02};
static char gray6_bits[] = {
   0x04, 0x07, 0x03};
static char gray7_bits[] = {
   0x05, 0x07, 0x03};
static char gray8_bits[] = {
   0x05, 0x07, 0x07};
static char gray9_bits[] = {
   0xff, 0xff, 0xff};

static char *gray_bits[MAXGRAY] = {
    gray0_bits,
    gray1_bits,
    gray2_bits,
    gray3_bits,
    gray4_bits,
    gray5_bits,
    gray6_bits,
    gray7_bits,
    gray8_bits,
    gray9_bits,
};

static void
ClassInitialize()
{
    XtAddConverter( XtRString, XtROrientation, XmuCvtStringToOrientation,
		    NULL, (Cardinal)0 );
}

static void
SetDimensions(w)
BarWidget w;
{
    if (w->bar.orientation == XtorientVertical) {
	w->bar.length = w->core.height;
	w->bar.thickness = w->core.width;
    }
    else {
	w->bar.length = w->core.width;
	w->bar.thickness = w->core.height;
    }
}

/* ARGSUSED */
static void
Initialize(request, new)
Widget request;		/* what the client asked for */
Widget new;		/* what we're going to give him */
{
    BarWidget w = (BarWidget) new;
    XGCValues gcValues;
    BarClassPart *bcp = &(barClassRec.bar_class);
    int i;

    if (bcp->gray == (Pixmap *) NULL) {
	/*
	 *  We initialize this here instead of ClassInitialize because
	 *  we need a window to initialize this in. A cleaner way to do
	 *  this would be to use a separate set of Pixmaps for each
	 *  widget, so that widgets can be on different screen/visuals.
	 *  But then we'd have to fix it so this thing understood
	 *  colours, visuals, more resources...  Laziness wins.
	 */
	bcp->gray = (Pixmap *) XtMalloc(MAXGRAY * sizeof(Pixmap));
	for(i = 0; i < MAXGRAY; i++) {
	    bcp->gray[i] = XCreatePixmapFromBitmapData(XtDisplay(new),
	     DefaultRootWindow(XtDisplay(new)), gray_bits[i], 3, 3,
	     BlackPixelOfScreen(XtScreen(new)),
	     WhitePixelOfScreen(XtScreen(new)),
	     DefaultDepthOfScreen(XtScreen(new)));
	}
    }
    gcValues.tile = bcp->gray[0];
    gcValues.fill_style = FillTiled;
    w->bar.gc = XCreateGC(XtDisplay(new), DefaultRootWindow(XtDisplay(new)),
     GCFillStyle | GCTile, &gcValues);
    /* Width and height override length and thickness */
    if (w->bar.length == def_dimension)
	w->bar.length = DEF_LENGTH;
	
    if (w->bar.thickness == def_dimension)
	w->bar.thickness = DEF_THICKNESS;
	
    if (w->core.width == def_dimension)
	w->core.width = (w->bar.orientation == XtorientVertical)
	    ? w->bar.thickness : w->bar.length;

    if (w->core.height == def_dimension)
	w->core.height = (w->bar.orientation == XtorientHorizontal)
	    ? w->bar.thickness : w->bar.length;
    w->bar.values = NULL;
    w->bar.nvalues = 0;

    SetDimensions(w);
}

/* ARGSUSED */
static void
Resize(gw)
Widget gw;
{
    SetDimensions((BarWidget)gw);
    Redisplay(gw, (XEvent*)NULL, (Region)NULL);
}

/* ARGSUSED */
static void
Redisplay(gw, event, region)
Widget gw;
XEvent *event;
Region region;
{
    BarWidget w = (BarWidget) gw;
    int x, y;
    unsigned int width, height;
    int maxvalue, i;
    int inc_gray;
    int igray; 
    int vertical = (w->bar.orientation == XtorientVertical);;

    if (! XtIsRealized(gw))
	return;
    for(maxvalue = 0, i = 0; i < w->bar.nvalues; i++)
	maxvalue += w->bar.values[i];
    if(maxvalue <= 0)
	return;
    x = y = 0;
    if(vertical)
	width = w->bar.thickness;
    else
	height = w->bar.thickness;
    inc_gray = (w->bar.nvalues > 1) ? (MAXGRAY - 1) / (w->bar.nvalues - 1) :
	MAXGRAY - 1;
    for(igray = 0, i = 0; i < w->bar.nvalues; i++, igray += inc_gray) {
	unsigned int rectlen = (w->bar.length * w->bar.values[i] +
				maxvalue / 2) / maxvalue;
	if (rectlen == 0)
	    continue;
	if (i == w->bar.nvalues - 1) {
	    if (vertical)
		rectlen = w->core.height - y;
	    else
		rectlen = w->core.width - x;
	}
	XSetTile(XtDisplay(gw), w->bar.gc, barClassRec.bar_class.gray[igray]);
	if(vertical) {
	    XFillRectangle(XtDisplay(gw), XtWindow(gw), w->bar.gc,
	     x, y, width, rectlen);
	    y += rectlen;
	} else {
	    XFillRectangle(XtDisplay(gw), XtWindow(gw), w->bar.gc,
	     x, y, rectlen, height);
	    x += rectlen;
	}
    }
}

/* Public routines. */
void
SetBarValues(gw, values, nvalues)
Widget gw;
int *values;
int nvalues;
{
    BarWidget w = (BarWidget) gw;
    int i;

    if (nvalues > MAXGRAY) {
#define ERRFMT "Tried to set %d values for bar - maximum is %d\n"
	char errbuf[sizeof(ERRFMT) + 32];
	(void) sprintf(errbuf, ERRFMT, nvalues, MAXGRAY);
#undef ERRFMT
	XtWarning(errbuf);
	nvalues = MAXGRAY;
    }
    if (nvalues < 0) {
#define ERRFMT "Tried to set %d values for bar - it must be > 0\n"
	char errbuf[sizeof(ERRFMT) + 32];
	(void) sprintf(errbuf, ERRFMT, nvalues);
	XtWarning(errbuf);
#undef ERRFMT
	return;
    }
    w->bar.nvalues = nvalues;
    if (w->bar.values != NULL)
	XtFree((char *) w->bar.values);
    if (nvalues != 0)
	w->bar.values = (int *) XtMalloc(nvalues * sizeof(int));
    for(i = 0; i < nvalues; i++)
	w->bar.values[i] = values[i];
    Redisplay( gw, (XEvent*)NULL, (Region)NULL );
}
