/*
 * $XConsortium: Perfmon.c,v 1.3 89/09/19 14:44:04 jim Exp $
 * 
 * Perfmon Performance Monitor Widget
 *
 * Copyright 1989 PCS Computer Systeme GmbH, West Germany
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of PCS not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  PCS makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * PCS DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL PCS
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Emanuel Jay Berkenbilt, MIT Project Athena
 * Author:           Thomas A. Baghli, PCS Computer Systeme GmbH, West Germany
 *                   tom@meepmeep.pcs.com
 */


#include <stdio.h>

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Xmu/Misc.h>
#include <X11/Xlib.h>

#include "PerfmonP.h"
#include "graph.h"

extern char *calloc(), *malloc();
extern void exit();

extern int sys_setup();
extern stat_type stats;

static void HandleKey();

static char defaultTranslations[] =
    "<KeyDown>:    HandleKey()    ";

static XtActionsRec actionsTable[] = {
     { "HandleKey",    HandleKey },
     { NULL,           NULL      },
};

static Dimension defDimension = 200;
static int defInterval = 5;
static int defStepSize = 1;

#define offset(field) XtOffset(PerfmonWidget, perfmon.field)
#define goffset(field) XtOffset(Widget, core.field)

static XtResource resources[] = {
    { XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
	goffset(width), XtRDimension, (caddr_t) &defDimension },
    { XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
	goffset(height), XtRDimension, (caddr_t) &defDimension },
    { XtNbackground, XtCBackground, XtRPixel, sizeof(Pixel),
	goffset(background_pixel), XtRString, "White" },
    { XtNforeground, XtCForeground, XtRPixel, sizeof(Pixel),
        offset(fgpixel), XtRString, "Black" },
    { XtNhighlight, XtCForeground, XtRPixel, sizeof(Pixel),
        offset(hipixel), XtRString, "Black" },
    { XtNfont, XtCFont, XtRFontStruct, sizeof(XFontStruct *),
        offset(font), XtRString, XtDefaultFont },
    { XtNreverseVideo, XtCReverseVideo, XtRBoolean, sizeof (Boolean),
	offset(reverse_video), XtRString, "FALSE" },

    { XtNstepSize, XtCStepSize, XtRInt, sizeof(int),
       offset(step_size), XtRInt, (caddr_t)&defStepSize },
    { XtNupdate, XtCInterval, XtRInt, sizeof(int),
       offset(update), XtRInt, (caddr_t)&defInterval },

    { XtNuserCpu, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[USER_CPU_PERCENTAGE]), XtRString, "FALSE" },
    { XtNsystemCpu, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[SYSTEM_CPU_PERCENTAGE]), XtRString, "FALSE" },
    { XtNidleCpu, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[IDLE_CPU_PERCENTAGE]), XtRString, "FALSE" },
    { XtNfreeMem, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[FREE_MEM]), XtRString, "FALSE" },
    { XtNdiskTransfers, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[DISK_TRANSFERS]), XtRString, "FALSE" },
    { XtNinterrupts, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[INTERRUPTS]), XtRString, "FALSE" },
    { XtNinterrupts, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[INTERRUPTS]), XtRString, "FALSE" },
    { XtNinputPackets, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[INPUT_PACKETS]), XtRString, "FALSE" },
    { XtNoutputPackets, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[OUTPUT_PACKETS]), XtRString, "FALSE" },
    { XtNcollisionPackets, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(sys_params_used[COLLISION_PACKETS]), XtRString, "FALSE" },
};

#undef offset
#undef goffset

static void ClassInitialize();
static void Initialize(), Realize(), Redisplay(), Resize(), Destroy();
static Boolean SetValues();

PerfmonClassRec perfmonClassRec = {
    { /* core fields */
    /* superclass		*/	&widgetClassRec,
    /* class_name		*/	"Perfmon",
    /* size			*/	sizeof(PerfmonRec),
    /* class_initialize		*/	ClassInitialize,
    /* class_part_initialize	*/	NULL,
    /* class_inited		*/	FALSE,
    /* initialize		*/	Initialize,
    /* initialize_hook		*/	NULL,
    /* realize			*/	Realize,
    /* actions			*/	actionsTable,
    /* num_actions		*/	XtNumber(actionsTable),
    /* resources		*/	resources,
    /* num_resources		*/	XtNumber(resources),
    /* xrm_class		*/	NULL,
    /* compress_motion		*/	TRUE,
    /* compress_exposure	*/	TRUE,
    /* compress_enterleave	*/	TRUE,
    /* visible_interest		*/	FALSE,
    /* destroy			*/	Destroy,
    /* resize			*/	NULL,
    /* expose			*/	Redisplay,
    /* set_values		*/	SetValues,
    /* set_values_hook		*/	NULL,
    /* set_values_almost	*/	NULL,
    /* get_values_hook		*/	NULL,
    /* accept_focus		*/	NULL,
    /* version			*/	XtVersion,
    /* callback_private		*/	NULL,
    /* tm_table			*/	defaultTranslations,
    /* query_geometry		*/	XtInheritQueryGeometry,
    /* display_accelerator	*/	XtInheritDisplayAccelerator,
    /* extension		*/	NULL
    }
};

WidgetClass perfmonWidgetClass = (WidgetClass) &perfmonClassRec;

static void check_stats_specified();
static void draw_background(), draw_it();
static void graph(), graph_setup(), init_graph_list();
static void refresh_graph(), shift_graph();
static void adjust_timeout();

static int *values;
static int num_stats;
static int orig_update;

static void
ClassInitialize()
{
    /* DO NOTHING */
}

/* ARGSUSED */
static void
Initialize(request, new)
    Widget request, new;
{
    PerfmonWidget pw = (PerfmonWidget) new;
    XtGCMask valuemask;
    XGCValues myXGCV;

    int i, min_size, stat;
    Display *dpy = XtDisplay(pw);
    int screen = DefaultScreen(XtDisplay(pw));
    /* The maximum number of values that can be saved - this
     * is the number of horizontal pixels in the screen.
     */
    int NUM_VALS = DisplayWidth(dpy, screen);

    if (pw->perfmon.reverse_video) {
	Pixel fg = pw->perfmon.fgpixel;
	Pixel bg = pw->core.background_pixel;

	if (pw->core.border_pixel == fg)
	     pw->core.border_pixel = bg;
	if (pw->perfmon.hipixel == pw->perfmon.fgpixel)
	     pw->perfmon.hipixel = bg;
	pw->perfmon.fgpixel = bg;
	pw->core.background_pixel = fg;
    }

    valuemask = GCForeground | GCBackground;
    myXGCV.foreground = pw->perfmon.fgpixel;
    if (pw->perfmon.font) {
	 myXGCV.font = pw->perfmon.font->fid;
	 valuemask |= GCFont;
    }
    myXGCV.background = pw->core.background_pixel;
    pw->perfmon.fgGC = XtGetGC(new, valuemask, &myXGCV);
    myXGCV.foreground = pw->perfmon.hipixel;
    pw->perfmon.hiGC = XtGetGC(new, valuemask, &myXGCV);

    if (pw->perfmon.font == NULL)
	pw->perfmon.font = XQueryFont(XtDisplay(pw),
	    XGContextFromGC(DefaultGCOfScreen(XtScreen(pw))));

    pw->perfmon.font_height = pw->perfmon.font->max_bounds.ascent
	 + pw->perfmon.font->max_bounds.descent;

    if (!VALID_UPDATE(pw->perfmon.update))
	 pw->perfmon.update = defInterval;
    orig_update = pw->perfmon.update;
    pw->perfmon.interval_id =
	 XtAddTimeOut(pw->perfmon.update * 1000, draw_it, (caddr_t) pw);

    check_stats_specified(pw);

    /* Allow system.c to do whatever setup it needs and find out
     * the number of stats that are actually being defined.
     */
    if ((num_stats = sys_setup(pw->perfmon.sys_params_used)) == 0) {
	 fprintf(stderr,"Sorry - no stats to display!\n");
	 exit(1);
    }

    /* Set up the graph structures and initialize them with
     * the appropriate information from the stats variable.
     */
    graph_setup(num_stats, LINES_PER_LABEL);
    FORALLSTATS(stat)
	init_graph_list(stats[stat].label, stats[stat].min_val,
	    stats[stat].max_val);

    if ((values = (int *) calloc(num_stats, sizeof(int))) == NULL) {
	perror("Failure getting memory for values.");
	exit(1);
    }

    /* Allocate memory for saved_values.  This has to be done after
     * the display is opened because before that there is no way of
     * finding out the width of the screen.
     */
    if ((graph_data.saved_values =
	(int **) calloc(NUM_VALS, sizeof(int *))) == NULL) {
	perror("Failure getting memory for saved_values.");
	exit(1);
    } else {
	for (i = 0; i < NUM_VALS; i++)
	    if ((graph_data.saved_values[i] =
		(int *) calloc(graph_set.num_graphs, sizeof(int))) == NULL) {
		perror("Failure getting memory for saved_values[i].");
		exit(1);
	    }
    }

    /* Calculate the minimum width and height of the window
     * so that the text and graphs fit reasonably well.
     */
    min_size = 2 * label_width(pw);
    if (request->core.width < min_size)
	 pw->core.width = min_size;

    pw->core.height = HOST_HEIGHT + 2 * graph_set.num_graphs * HOST_HEIGHT
	 + TOPSTUFF + BOTTOMSTUFF + 2 * INNER_BORDER;

    graph_data.g_width = 0;
}

static void
Realize(w, valueMask, attrs)
    Widget w;
    XtValueMask *valueMask;
    XSetWindowAttributes *attrs;
{
    XtCreateWindow(w, (unsigned)InputOutput, (Visual *)CopyFromParent,
	 *valueMask, attrs);
}

/* ARGSUSED */
static void
Redisplay(w, event, region)
    Widget w;
    XEvent *event;
    Region region;
{
    PerfmonWidget pw = (PerfmonWidget) w;

    if (XtIsRealized((Widget) pw)) {
	XClearWindow(XtDisplay(pw), XtWindow(pw));
	draw_background(pw);
	if ((graph_data.cur_value + 1) * pw->perfmon.step_size
	    > graph_data.g_width)
	    shift_graph(pw);
	if (graph_data.g_width > 0)
	    refresh_graph(pw);
    }
}

static void
Destroy(w)
    Widget w;
{
    PerfmonWidget pw = (PerfmonWidget) w;

    if (pw->perfmon.interval_id)
	XtRemoveTimeOut(pw->perfmon.interval_id);
    XtDestroyGC(pw->perfmon.fgGC);
    XtDestroyGC(pw->perfmon.hiGC);
}

/* ARGSUSED */
static Boolean
SetValues(current, request, new)
    Widget current, request, new;
{
    PerfmonWidget oldpw = (PerfmonWidget) current;
    PerfmonWidget pw = (PerfmonWidget) new;

    if (pw->perfmon.update != oldpw->perfmon.update) {
	XtRemoveTimeOut(oldpw->perfmon.interval_id);
	pw->perfmon.interval_id =
	    XtAddTimeOut(pw->perfmon.update * 1000, draw_it, (caddr_t) pw);
    }
    return(FALSE);
}

static void
check_stats_specified(pw)
    PerfmonWidget pw;
{
    int stat = 0;
    Boolean stats_specified = FALSE;

    /* If any sys_param_used is TRUE then the user has specified
     * something, so only those specified will be displayed.  If
     * all sys_params_used are FALSE then the user expects to have
     * all stats displayed.
     */
    do
	stats_specified = pw->perfmon.sys_params_used[stat];
    while (++stat < NUM_POSSIBLE_STATS && stats_specified == FALSE);

    if (!stats_specified)
	/* The user has not specified any system parameters, therefore,
	 * set all to be TRUE so that they will be displayed.
	 */
	FORALLPOSSIBLESTATS(stat)
	    pw->perfmon.sys_params_used[stat] = TRUE;

#ifdef PCS
    pw->perfmon.sys_params_used[DISK_TRANSFERS]
	= pw->perfmon.sys_params_used[INTERRUPTS]
	= pw->perfmon.sys_params_used[INPUT_PACKETS]
	= pw->perfmon.sys_params_used[OUTPUT_PACKETS]
	= pw->perfmon.sys_params_used[COLLISION_PACKETS] = FALSE;
#endif /* PCS */
}

/* ARGSUSED */
static void
draw_it(client_data, id)
    caddr_t client_data;
    XtIntervalId id;		/* unused */
{
    PerfmonWidget pw = (PerfmonWidget) client_data;
    int stat;

    if (pw->perfmon.update > 0)
	pw->perfmon.interval_id =
	     XtAddTimeOut(pw->perfmon.update * 1000, draw_it, (caddr_t) pw);

    update_stats();
    FORALLSTATS(stat)
        values[stat] = stats[stat].value;
    graph(pw, values);
}


/****************************************************************************

   These routines take an array of integers and graph them by drawing
   parallel line graphs inside of a single window.  Each graph is called
   a "graph."  The entire set of graphs is called a "graph set."

   This program keeps track of a few kinds of values.  There are
   1.  Raw values: These are the values that are passed to this
       section of the program from main_event_loop() in xperfmon.c.  
       They are simply a list of numbers as far as window.c is concerned.
       Before they can be used by X, they need to be scaled.  See the
       comment on the SCALE macro below for more information.
   2.  Pixel values: These values represent actual numbers of pixels.
   3.  Saved values: These correspond to values on the graph.  If
       the stepsize of the graph were always one, there would be no 
       difference between these and pixel values, but since the step 
       size can be set by the user of the program from the commandline,
       there must be a distinction.  Since it is important to know 
       which values are Pixel values and which ones are Saved values, 
       the comments on the data structure and variables will tell 
       which type of values each variable or field is.  A lot of subtle
       bugs (especially in refresh) can occur if the values are mixed up.

*****************************************************************************/
 
/* Array of pointers to one graph structure per graph. */
struct graph **graph_list;

/* Convenient macros to do text stuff in the window. */
#define XTW(s) XTextWidth(pw->perfmon.font, s, strlen(s))
#define XDS(s) XDrawString(XtDisplay(pw), XtWindow(pw), \
			   pw->perfmon.hiGC, x, y, s, strlen(s))

static void
draw_background(pw)
    PerfmonWidget pw;
/* This routine draws the background for a window - that is, the hostname,
 * the dividers, and the labels including maximum and minimum values.
 */
{
    int g_height;
    int x, x1, x2;
    int y, y1, y2;
    int i, j;
    char hostname[40];
    char numstring[20];
  
    /* Calculate the size of a single graph based on the size of the window. */
    graph_data.g_width = pw->core.width - label_width(pw);
    if (graph_data.g_width < 0)
	 graph_data.g_width = 0;
    g_height = (pw->core.height - TOPSTUFF - BOTTOMSTUFF)
	 / graph_set.num_graphs;

    graph_data.left_pix = label_width(pw);

    /* Draw the hostname. */
    x = INNER_BORDER;
    y = pw->perfmon.font_height + FONT_PAD + INNER_BORDER;
    (void) gethostname(hostname, sizeof(hostname));
    XDS(hostname);

    /* Draw the dividers. */
    for (i = 0; i <= graph_set.num_graphs; i++) {
	 /* Draw a line from the left of the drawing area to the right of the 
	  * window at the level of the top of each graph.  Also draw a line
	  * at the bottom of the last graph hence the <= graph_set.num_graphs
	  * rather than the < as in all other places.
	  */
	 x1 = graph_data.left_pix;
	 y1 = g_height * i + TOPSTUFF;
	 x2 = x1 + graph_data.g_width;
	 y2 = y1;
	 XDrawLine(XtDisplay(pw), XtWindow(pw), pw->perfmon.fgGC,
		   x1, y1, x2, y2);
    }

    /* draw labels */
    for (i = 0; i < graph_set.num_graphs; i++) {
	/* Set min_pix, max_pix, and zero values based on the
	 * graph number, remembering to add in the TOPSTUFF and
	 * GRAPH_BORDER - see comments in #defines section.
	 */
	graph_list[i]->min_pix = g_height * (i + 1) + TOPSTUFF - GRAPH_BORDER;
	graph_list[i]->max_pix = g_height * i + TOPSTUFF + GRAPH_BORDER;
	graph_list[i]->zero = graph_list[i]->min_pix
	    + (SCALE(i, graph_list[i]->min_value));
	/* Note that since vertical pixel values increase as
	 * you move down the screen, max_pix < min_pix.
	 *
	 * Note "%d " rather than "%d".  This to leave a space
	 * after the number before the beginning of the graph.
	 */
	(void) sprintf(numstring, "%d ", graph_list[i]->min_value);
	x = x1 - XTW(numstring);
	y = graph_list[i]->min_pix;
	XDS(numstring);
	(void) sprintf(numstring, "%d ", graph_list[i]->max_value);
	x = x1 - XTW(numstring);
	y = graph_list[i]->max_pix + pw->perfmon.font_height;
	XDS(numstring);
	x = INNER_BORDER;
	/* Set the y value for the first string so that the members
	 * of the label array will be centered in the graph area.
	 */
	y = graph_list[i]->max_pix + DRAW_GRAPH_HEIGHT(i) / 2
	    - (graph_set.lines_per_label - 2)
	    * (pw->perfmon.font_height / 2 + FONT_PAD);
	for (j = 0; j < graph_set.lines_per_label; j++) {
	    XDS(graph_list[i]->label[j]);
	    y = y + pw->perfmon.font_height + 2 * FONT_PAD;
	}
    }
    XFlush(XtDisplay(pw));
}

int
label_width(pw)
    PerfmonWidget pw;
/* This routine calcuates the the width in pixels of the label section of 
 * the graph set.  It does this by taking the width of the longest string
 * in the label array and adding to it the width of a five-digit number
 * and a space (as represented by "99999 " below).
 */
{
    int longest = 0;
    int graph;
    int line;

    for (graph = 0; graph < graph_set.num_graphs; graph++)
	 for (line = 0; line < graph_set.lines_per_label; line++)
	      if (XTW(graph_list[graph]->label[line]) > longest)
		   longest = XTW(graph_list[graph]->label[line]);
    /* If the min and max value labels ever start colliding with the names
     * of the graphs, "99999 " will have to be increased in length.
     */
    return(longest + XTW("99999 ") + INNER_BORDER + FONT_PAD);
}

static void
shift_graph(pw)
    PerfmonWidget pw;
{
/* This routine takes care of shifting the graph to the left when the
 * information gets drawn off the right end.  It is also called from
 * redisplay_window if the current_value is larger than will fit on the
 * graph.  It should be called whenever this occurs.  This routine 
 * moves the current_value to the center of the graph and takes care
 * of moving all saved information so that the graph will be updated 
 * properly.  In addition, this routine sees that the max_value field
 * for each graph is as high as it needs to be.
 */
    register int i,j,t;
    int num_vals_shift;
    int max;
    int min;

    if (graph_data.g_width == 0) {
	/* This means that the window has not been drawn yet. */
	return;
    }

    num_vals_shift = (graph_data.g_width / 2) / pw->perfmon.step_size;

    for (i = 0; i < num_vals_shift; i++)
	 graph_data.saved_values[i] =
	      graph_data.saved_values
		[graph_data.cur_value - num_vals_shift + i];
    for (j = 0; j < graph_set.num_graphs; j++) {
	 max = min = 0;
	 for (i = 0; i < num_vals_shift; i++) {
	      t = graph_data.saved_values[i][j];
	      max = (max > t) ? max : t;
	      min = (min < t) ? min : t;
	 }
	 max = (max == min) ? graph_list[j]->max_value : max;
	 if (max != graph_list[j]->max_value)
	      graph_list[j]->max_value = max;
	 min = (graph_list[j]->max_value == min) ? min - 1 : min;
	 if (min != graph_list[j]->min_value)
	      graph_list[j]->min_value = min;
    }
    graph_data.cur_value = num_vals_shift;
  
    XClearArea(XtDisplay(pw), XtWindow(pw), 0, 0,
	       graph_data.g_width, pw->core.height, True);
}

void
val_to_pixels(pw, graph_num, which_val, x, y)
    PerfmonWidget pw;
    int graph_num, which_val;
    int *x, *y;
/* This routine takes care of all the scaling of values.  This routine takes
 * the graph number and the raw value and sets the values of x and y to the
 * actual point on the window corresponding to those values.
 */
{
    register int tmp = (graph_list[graph_num]->zero -
			SCALE(graph_num,
			      graph_data.saved_values[which_val][graph_num]));
    *x = graph_data.left_pix + pw->perfmon.step_size * which_val;
    *y = Max(tmp, graph_list[graph_num]->max_pix);
    /* If the choice of Max and Min look odd, just remember that pixel values
     * go up as actual values go down; i.e., min_pix is greater than max_pix.
     */
}

static void
graph(pw, vals)
    PerfmonWidget pw;
    int vals[];
/* This routine adds the values that are given to it to the graph. */
{
    int i;
    int x1, y1, x2, y2;

    Display *dpy = XtDisplay(pw);
    int screen = DefaultScreen(XtDisplay(pw));
    /* The maximum number of values that can be saved - this
     * is the number of horizontal pixels in the screen.
     */
    int NUM_VALS = DisplayWidth(dpy, screen);

    if (graph_data.g_width == 0) {
	 /* This means that the window has not been drawn yet. */
	 return;
    }

    for (i = 0; i < graph_set.num_graphs; i++) {
	 graph_data.saved_values[graph_data.cur_value][i] = vals[i];
	 val_to_pixels(pw, i, graph_data.cur_value, &x2, &y2);
	 if (graph_data.cur_value == 0)	{
	      /* If this is the leftmost point, then just plot it. */
	      XDrawPoint(XtDisplay(pw), XtWindow(pw), pw->perfmon.fgGC, x2, y2);
	 } else	{
	      /* If this is not the left most point, then draw a line
	       * connecting this point with the one to its left.
	       */
	      val_to_pixels(pw, i, (graph_data.cur_value - 1) % NUM_VALS,
			    &x1, &y1);
	      XDrawLine(XtDisplay(pw), XtWindow(pw), pw->perfmon.fgGC,
			x1, y1, x2, y2);
	 }
    }
    /* Check to see whether the graph needs to be shifted. */
    if ((graph_data.cur_value + 1) * pw->perfmon.step_size > graph_data.g_width)
	 shift_graph(pw);

    /* Update the current value.  This needs to be done even when the
     * graph is shifted, as all shift_graph does is move the pointers.
     */
    graph_data.cur_value++;
}

static void
refresh_graph(pw)
    PerfmonWidget pw;
/* This routine redraws the graph from the values stored in
 * graph_data.saved_values[][].
 */
{
    int save_cur_value;

    save_cur_value = graph_data.cur_value;

    for (graph_data.cur_value = 0; graph_data.cur_value <= save_cur_value;)
	 graph(pw, graph_data.saved_values[graph_data.cur_value]);
}

static void
graph_setup(num_graphs, lines_per_label)
    int num_graphs;
    int lines_per_label;
/* This routine initializes the graph data structures.  It is called from
 * main() during the setup period of the program and is not needed after
 * that.  This routine must be called before any other calculations are
 * done because other routines assume that the value in these data
 * structures will be accurate.
 */
{
    int i;

    graph_set.num_graphs = num_graphs;
    graph_set.lines_per_label = lines_per_label;

    if ((graph_list =
	(struct graph **) calloc(num_graphs, sizeof(struct graph *)))
	== NULL) {
	perror("Failure getting memory for graph_list");
	exit(1);
    }

    for (i = 0; i < num_graphs; i++) {
	if ((graph_list[i] =
	    (struct graph *) malloc(sizeof(struct graph))) == NULL) {
	    perror("Failure getting memory for graph_list[i]");
	    exit(1);
	 }
    }

    /* Note that saved_values is not initialized until win_setup because 
     * the display has to be opened before the number of pixels in the
     * screen can be determined.
     */
}

static void
init_graph_list(label, min, max)
    char **label;
    int min;
    int max;
/* This routine is designed to be called exactly graph_set.num_graphs times.
 * Each time it is called, it initializes another element of graph_list by
 * keeping track of which element it is on via a statically declared index.
 */
{
    static int index = 0;
 
    if (index < graph_set.num_graphs) {
	graph_list[index]->label = label;
	graph_list[index]->min_value = min;
	graph_list[index]->max_value = max;
	index++;
    } else
	/* Do nothing - the list is already initialized. */ ;
}

static void
HandleKey(w, event, params, num_params)
    Widget w;
    XEvent *event;
    String *params;
    Cardinal *num_params;
/* This routine interprets the key that was pressed on top of the window. 
 * It returns TRUE if Quit has been selected, signaling that the program
 * is done.
 */
{
#define STRBUFSIZE 64
    char strbuf[STRBUFSIZE];
    int i, keycode, length = 0;
    PerfmonWidget pw = (PerfmonWidget) w;

    length = XLookupString(event, strbuf, STRBUFSIZE, &keycode, NULL);
    switch (keycode) {
    case 'Q':
    case 'q':
	free(values);
	free(graph_data.saved_values);
/*
	for (i = 0; i < NUM_VALS; i++)
	    free(graph_data.saved_values[i]);
*/
	free(graph_list);
/*
	for (i = 0; i < num_graphs; i++)
	    free(graph_list[i]);
*/
	exit(0);
	break;
    case 'R':
    case 'r':
	/* This routine resets the graph moving the current value
	 * pointer to zero so that all saved values are ignored.
	 *
	 * Clear the window, signaling an exposure event.
	 */
	XClearArea(XtDisplay(w), XtWindow(w), 0, 0,
	     pw->core.width, pw->core.height, True);
	graph_data.cur_value = 0;

	if (pw->perfmon.interval_id)
	    XtRemoveTimeOut(pw->perfmon.interval_id);
	pw->perfmon.update = orig_update;
	pw->perfmon.interval_id =
	    XtAddTimeOut(pw->perfmon.update * 1000, draw_it, (caddr_t) pw);
	break;
    case 's':
	/* Maybe the following adjustments should be calls to SetValues? */
	adjust_timeout(pw, SMALL_ADD_TIME);
	break;
    case 'S':
	adjust_timeout(pw, LARGE_ADD_TIME);
	break;
    case 'f':
	adjust_timeout(pw, SMALL_SUBTRACT_TIME);
	break;
    case 'F':
	adjust_timeout(pw, LARGE_SUBTRACT_TIME);
	break;
    case '?':
	puts("Q/q - Quit");
	puts("R/r - Reset graph and timer");
	puts("s - Decrease update interval (slower) by a small amount");
	puts("S - Decrease update interval (slower) by a large amount");
	puts("f - Increase update interval (faster) by a small amount");
	puts("F - Increase update interval (faster) by a large amount");
	puts("? - Help");
	break;
    default:
	break;
    }
}

static void
adjust_timeout(pw, delta)
    PerfmonWidget pw;
    int delta;
{
    if (pw->perfmon.interval_id)
	XtRemoveTimeOut(pw->perfmon.interval_id);

    while (!VALID_UPDATE(pw->perfmon.update + delta))
	delta /= 2;
    pw->perfmon.update += delta;

    pw->perfmon.interval_id =
	XtAddTimeOut(pw->perfmon.update * 1000, draw_it, (caddr_t) pw);
}
