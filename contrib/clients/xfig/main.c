/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "const.h"
#include "version.h"

extern int		borderwid, windowspacing, toolstripeht;
extern int		ICON_COLUMN;
extern int		CANVAS_HEIGHT, CANVAS_WIDTH;
extern int		CANVAS_LEFT, CANVAS_TOP;
extern int		PANEL_LEFT, PANEL_TOP, PANEL_HEIGHT, PANEL_WID;
extern int		MSG_LEFT, MSG_TOP, MSG_WIDTH;
extern int		SIDERULER_LEFT, SIDERULER_TOP;
extern int		SIDERULER_WIDTH, SIDERULER_HEIGHT;
extern int		SIDERULER_START;
extern int		TOPRULER_LEFT, TOPRULER_TOP;
extern int		TOPRULER_WIDTH, TOPRULER_HEIGHT;
extern			(*canvas_rightbut_proc)();
extern			set_popupmenu();

#ifdef	X11
#include "fig.icon.X"
Pixmap	fig_icon;
#else
#include "fig.icon"
#define 		FIG_ICON_WIDTH		MY_ICON_WIDTH
#define 		FIG_ICON_HEIGHT		MY_ICON_HEIGHT

mpr_static(fig_icon_pr, FIG_ICON_WIDTH, FIG_ICON_HEIGHT, 1, icon_data);
static struct icon	fig_icon  =  {
				FIG_ICON_WIDTH+4, FIG_ICON_HEIGHT+4, NULL,
				{2, 2, FIG_ICON_WIDTH, FIG_ICON_HEIGHT},
				&fig_icon_pr, {0, 0, 0, 0}, NULL, NULL,
				ICON_BKGRDSET };
#endif	/* X11*/

#include "resources.h"
#define			SCREEN_WIDTH		1152
#define			SCREEN_HEIGHT		900
#define			min(a,b)		(((a) < (b)) ? (a) : (b))

static char	tool_name[80] = " FIG : FACILITY FOR INTERACTIVE GENERATION OF FIGURES ";
static RECT		icon_rect, tool_rect;
static			sigwinched();

/************** FIG options ******************/

extern int		DEBUG;
extern int		RHS_PANEL;
extern int		INVERSE;
extern int		TRACKING;

TOOL			tool;
int			WINDOW_WIDTH, WINDOW_HEIGHT;
char			file_header[32] = "#FIG ";
static char		*file = NULL;

get_args(argc, argv)
int	argc;
char	**argv;
{
	char	*option;

        CANVAS_HEIGHT = DEF_CANVAS_HEIGHT;
        CANVAS_WIDTH = DEF_CANVAS_WIDTH;
        ICON_COLUMN = ICON_COLUMN_PORT;
	while (--argc > 0)  {
	   option = *++argv;
	   if (*option == '-') {
		 while(*++option != '\0') {
			 switch(*option) {
			 case 'r':	/* put panel on right side */
				 RHS_PANEL = 1;
				 break;
				 
			 case 'd':	/* turn on debugging */
				 DEBUG = 1;
				 break;
				 
			 case 'p':   /* 8 X 10 portrait mode */
			 case 'P':
				 CANVAS_HEIGHT = DEF_CANVAS_HEIGHT;
				 CANVAS_WIDTH = DEF_CANVAS_WIDTH;
				 ICON_COLUMN = ICON_COLUMN_PORT;
				 break;
				 
			 case 'l':   /* 10 X 8 landscape mode */
			 case 'L':
				 CANVAS_HEIGHT = DEF_CANVAS_WIDTH;
				 CANVAS_WIDTH = DEF_CANVAS_HEIGHT;
				 ICON_COLUMN = ICON_COLUMN_LAND;
				 break;
				 
			 case 'w':	/* set width to size you want */
				 CANVAS_WIDTH = atof(*++argv) * PIX_PER_INCH;
				 argc--;
				 break;
				 
			 case 'h':	/* set height to size you want */
				 CANVAS_HEIGHT = atof(*++argv) * PIX_PER_INCH;
				 if( CANVAS_HEIGHT < DEF_CANVAS_HEIGHT)
					 ICON_COLUMN = ICON_COLUMN_LAND;
				 argc--;
				 break;
				 
			 case 'i':	/* reverse video */
				 INVERSE = 1;
				 invert_color();
				 break;

			 case 't':	/* turn on tracking */
				 TRACKING = 1;
				 break;

			 case 'n':	/* turn off tracking */
				 TRACKING = 0;
				 break;
					 
			 default:
				 printf ("%s: Flag <%c> unrecognized\n", argv[0], *option);
				 exit(1);
				 
			 }
		 }
	 }
	 else
	 {
		 file = option;
	 }
    }
}

#ifndef	X11
main(argc, argv)
int	argc;
char	**argv;
{
	ICON_COLUMN = ICON_COLUMN_PORT;
	get_args(argc, argv);
	strcat(tool_name, FIG_VERSION);
	strcat(file_header, FIG_VERSION);
	printf("Initializing FIG %s%s (SunView) ...\n", FIG_VERSION, MINOR_VERSION);
	tool = tool_create(tool_name, TOOL_NAMESTRIPE, NULL, &fig_icon);
	if (tool == (TOOL)0) exit(1);
	compute_window_sizes();
	set_window_size();

	init_font();
	if (init_panel(tool) == 0) exit(1);
	if (init_canvas(tool) == 0) exit(1);
	if (init_msg(tool) == 0) exit(1);
	if (init_sideruler(tool) == 0) exit(1);
	if (init_topruler(tool) == 0) exit(1);
	set_subwindow_sizes();

	if (file != NULL) {
	    edit_file(file);
	    }
	signal(SIGWINCH, sigwinched);
	canvas_rightbut_proc = set_popupmenu;
	tool_install(tool);
	for(;;) {
	    tool_select(tool, 0);
	    quit();	/* This is where one of the exit points is */
	    }
}
#else

static int true = True;
static int false = False;
static int zero = 0;
int	landscape;
float	tmp_width = 0.0, tmp_height = 0.0;

#define XtRFloat                "Float" 

static XtResource application_resources[] = {
	{XtNjustify, XtCJustify, XtRBoolean, sizeof(int),
		 (Cardinal)&RHS_PANEL, XtRBoolean, (caddr_t)&false},
	{"debug", "Debug", XtRBoolean, sizeof(int),
		 (Cardinal)&DEBUG, XtRBoolean, (caddr_t)&false},
	{"landscape", XtCOrientation, XtRBoolean, sizeof(int),
		 (Cardinal)&landscape, XtRBoolean, (caddr_t)&false},
	{XtNwidth, XtCWidth, XtRFloat, sizeof(float),
		 (Cardinal)&tmp_width, XtRInt, (caddr_t)&zero},
	{XtNheight, XtCHeight, XtRFloat, sizeof(float),
		 (Cardinal)&tmp_height, XtRInt, (caddr_t)&zero},
	{XtNreverseVideo, XtCReverseVideo, XtRBoolean, sizeof(int),
		 (Cardinal)&INVERSE, XtRBoolean, (caddr_t)&false},
	{"trackCursor", "Track", XtRBoolean, sizeof(int),
		 (Cardinal)&TRACKING, XtRBoolean, (caddr_t)&true},
};

static XrmOptionDescRec options[] =
{
	{"-right", ".justify", XrmoptionNoArg, "True" },
	{"-left", ".justify", XrmoptionNoArg, "False"},
	{"-debug", ".debug", XrmoptionNoArg, "True"},
	{"-landscape", ".landscape", XrmoptionNoArg, "True"},
	{"-Landscape", ".landscape", XrmoptionNoArg, "True"},
	{"-portrait", ".landscape", XrmoptionNoArg, "False"},
	{"-Portrait", ".landscape", XrmoptionNoArg, "False"},
	{"-width", ".width", XrmoptionSepArg, 0},
	{"-height", ".height", XrmoptionSepArg, 0},
	{"-inverse", ".reverseVideo", XrmoptionNoArg, "True"},
	{"-notrack", ".trackCursor", XrmoptionNoArg, "False"},
	{"-track", ".trackCursor", XrmoptionNoArg, "True"},
};

static XtCallbackRec	callbacks[] =
{
	{ NULL, NULL },
};

static Arg	form_args[] =
{
	{ XtNcallback, (XtArgVal)callbacks },
};

main(argc,argv)
	int		argc;
	char		*argv[];
{
	TOOL		form, children[5];
	Arg		tmp_arg[2];
	int		init_panel(), init_msg(), init_canvas(),
			init_sideruler(), init_topruler();

	strcat(tool_name, FIG_VERSION);
	strcat(file_header, FIG_VERSION);
	printf("Initializing FIG %s%s (X11) ... \n", FIG_VERSION, MINOR_VERSION);
	tool = XtInitialize("fig", "Fig", options, XtNumber(options),
		&argc, argv);

	fix_converters();
	XtGetApplicationResources(tool, 0, application_resources,
                                   XtNumber(application_resources), NULL, 0 );
	if (argc > 1)
		file = argv[1];

	tool_d = XtDisplay(tool);
	tool_s = XtScreen(tool);
	tool_sn = DefaultScreen(tool_d);
	
	gc = DefaultGC(tool_d, tool_sn);

	if( CellsOfScreen(tool_s) == 2 && INVERSE )
	{
		XrmValue    value;
		XrmDatabase newdb = (XrmDatabase) 0;
		XrmDatabase XtDefaultDB;

		XtDefaultDB = XtDatabase(tool_d);
		value.size = sizeof("White");
		value.addr = "White";
		XrmPutResource(&newdb, "xfig*borderColor", "String",
			       &value);
		value.size = sizeof("White");
		value.addr = "White";
		XrmPutResource(&newdb, "xfig*foreground", "String",
			       &value);
		value.size = sizeof("Black");
		value.addr = "Black";
		XrmPutResource(&newdb, "xfig*background", "String",
			       &value);
		XrmMergeDatabases(newdb, &XtDefaultDB);

		/* now set the tool part, since its already created */
		XtSetArg(tmp_arg[0], XtNborderColor,
			 WhitePixelOfScreen(tool_s));
		XtSetArg(tmp_arg[1], XtNforeground,
			 WhitePixelOfScreen(tool_s));
		XtSetArg(tmp_arg[2], XtNbackground,
			 BlackPixelOfScreen(tool_s));
		XtSetValues(tool, tmp_arg, 3);
	}
	
	init_font();
	init_cursor();
	form = XtCreateManagedWidget("form", formWidgetClass, tool,
		form_args, XtNumber(form_args));
	
	ICON_COLUMN = ICON_COLUMN_PORT;
	CANVAS_WIDTH = tmp_width * PIX_PER_INCH;
	CANVAS_HEIGHT = tmp_height * PIX_PER_INCH;

	if( CANVAS_WIDTH == 0 )
		CANVAS_WIDTH = landscape ? DEF_CANVAS_HEIGHT :
				DEF_CANVAS_WIDTH;
	
	if( CANVAS_HEIGHT == 0 )
		CANVAS_HEIGHT = landscape ? DEF_CANVAS_WIDTH :
			DEF_CANVAS_HEIGHT;
	
	if( (CANVAS_HEIGHT < DEF_CANVAS_HEIGHT) ||
	   (HeightOfScreen(tool_s) < DEF_CANVAS_HEIGHT) )
		ICON_COLUMN = ICON_COLUMN_LAND;
		
	(void)init_panel(form);
	children[0] = panel_sw;
	(void)init_topruler(form);
	children[1] = topruler_sw;
	(void)init_menu(form);
	(void)init_canvas(form);
	children[2] = canvas_sw;
	(void)init_msg(form);
	children[3] = msg_sw;
	(void)init_sideruler(form);
	children[4] = sideruler_sw;
	XtManageChildren(children, 5);
	XtRealizeWidget(tool);
	if( RHS_PANEL )
	{
		XtSetArg(tmp_arg[0], XtNfromHoriz, 0);
		XtSetArg(tmp_arg[1], XtNhorizDistance, RULER_WIDTH+2);
		XtSetValues(topruler_sw, tmp_arg, 2);
		XtSetArg(tmp_arg[0], XtNfromHoriz, 0);
		XtSetArg(tmp_arg[1], XtNfromVert, topruler_sw);
		XtSetValues(sideruler_sw, tmp_arg, 2);
		XtUnmanageChild(msg_sw);
		XtSetArg(tmp_arg[0], XtNfromHoriz, sideruler_sw);
		XtSetArg(tmp_arg[1], XtNfromVert, topruler_sw);
		XtSetValues(canvas_sw, tmp_arg, 2);
		XtManageChild(msg_sw);
		XtSetArg(tmp_arg[0], XtNfromHoriz, canvas_sw);
		XtSetArg(tmp_arg[1], XtNfromVert, 0);
		XtSetValues(panel_sw, tmp_arg, 2);
		XtSetArg(tmp_arg[0], XtNfromHoriz, sideruler_sw);
		XtSetArg(tmp_arg[1], XtNfromVert, canvas_sw);
		XtSetValues(msg_sw, tmp_arg, 2);
		XtUnmanageChild(msg_sw);
		XtManageChild(msg_sw);
	}
	init_gc();
	setup_panel();
	setup_rulers();
	setup_canvas();
	setup_msg();
	canvas_rightbut_proc = set_popupmenu;
	if (file != NULL)
		edit_file(file);
	(void)signal(SIGINT, SIG_IGN);	/* so we don't lose work accidentally */
	fig_icon = XCreateBitmapFromData(tool_d, XtWindow(tool),
					 fig_bits, fig_width, fig_height);
	
	XtSetArg(tmp_arg[0], XtNtitle, tool_name);
	XtSetArg(tmp_arg[1], XtNiconPixmap, fig_icon);
	XtSetValues(tool, tmp_arg, 2);
	
	XtMainLoop();
}
#endif	X11

#ifndef	X11
#define		TOPRULER_CANVAS_SPACING		5
#define		SIDERULER_CANVAS_SPACING	5
#define		PANEL_CANVAS_SPACING		5
#define		PANEL_MSG_SPACING		5

compute_window_sizes()
{
	borderwid = tool_borderwidth(tool);
	windowspacing = tool_subwindowspacing(tool);
	toolstripeht = tool_stripeheight(tool);
	PANEL_WID = ICON_COLUMN * SWITCH_ICON_WIDTH;
	PANEL_HEIGHT = RULER_WIDTH + CANVAS_HEIGHT + TOPRULER_CANVAS_SPACING;
	PANEL_TOP = toolstripeht + 2;
	MSG_WIDTH = PANEL_WID + RULER_WIDTH + CANVAS_WIDTH
		+ SIDERULER_CANVAS_SPACING + PANEL_CANVAS_SPACING;
	MSG_TOP = PANEL_HEIGHT + PANEL_TOP + PANEL_MSG_SPACING;
	MSG_LEFT = borderwid;

	TOPRULER_TOP = PANEL_TOP;
	TOPRULER_WIDTH = CANVAS_WIDTH;
	TOPRULER_HEIGHT = RULER_WIDTH;

	CANVAS_TOP = TOPRULER_TOP + RULER_WIDTH + TOPRULER_CANVAS_SPACING;

	SIDERULER_TOP = TOPRULER_TOP;
	SIDERULER_WIDTH = RULER_WIDTH;
	SIDERULER_HEIGHT = PANEL_HEIGHT;
	SIDERULER_START = RULER_WIDTH + TOPRULER_CANVAS_SPACING;

	WINDOW_WIDTH = borderwid + borderwid + MSG_WIDTH;
	WINDOW_HEIGHT = MSG_TOP + MSG_HEIGHT + borderwid;

	if (RHS_PANEL) { /* panel on the right of drawing area */
	    SIDERULER_LEFT = borderwid;
	    CANVAS_LEFT = SIDERULER_LEFT + RULER_WIDTH
			+ SIDERULER_CANVAS_SPACING;
	    PANEL_LEFT = CANVAS_LEFT + CANVAS_WIDTH + PANEL_CANVAS_SPACING;
	    TOPRULER_LEFT = CANVAS_LEFT;
	    }
	else {
	    PANEL_LEFT = borderwid;
	    CANVAS_LEFT = PANEL_LEFT + PANEL_WID + PANEL_CANVAS_SPACING;
	    SIDERULER_LEFT = CANVAS_LEFT + CANVAS_WIDTH
			+ SIDERULER_CANVAS_SPACING;
	    TOPRULER_LEFT = CANVAS_LEFT;
	    }
	}

set_window_size()
{
	if (wmgr_iswindowopen(tool->tl_windowfd) )  {
	    win_getrect(tool->tl_windowfd, &tool_rect);
	    win_getsavedrect(tool->tl_windowfd, &icon_rect);
	    }
	else {
	    win_getrect(tool->tl_windowfd, &icon_rect);
	    win_getsavedrect(tool->tl_windowfd, &tool_rect);
	    }
	if (tool_rect.r_width != WINDOW_WIDTH
		|| tool_rect.r_height != WINDOW_HEIGHT) {
	    tool_rect.r_width = WINDOW_WIDTH;
	    tool_rect.r_height = WINDOW_HEIGHT;
	    icon_rect.r_left = min(icon_rect.r_left, SCREEN_WIDTH-FIG_ICON_WIDTH-4);
	    icon_rect.r_top = min(icon_rect.r_top, SCREEN_HEIGHT-FIG_ICON_HEIGHT-4);
	    if (tool_rect.r_left < 0
		    || tool_rect.r_left + tool_rect.r_width > SCREEN_WIDTH
		    || tool_rect.r_top < 0
		    || tool_rect.r_top + tool_rect.r_height > SCREEN_HEIGHT) {
		tool_rect.r_left  = 300;
		tool_rect.r_top   = 10;
		}
	    if (wmgr_iswindowopen(tool->tl_windowfd) )  {
		win_setrect(tool->tl_windowfd, &tool_rect);
		win_setsavedrect(tool->tl_windowfd, &icon_rect);
		}
	    else {
		win_setrect(tool->tl_windowfd, &icon_rect);
		win_setsavedrect(tool->tl_windowfd, &tool_rect);
		}
	    }
	}

static
sigwinched()
{
	tool_sigwinch(tool);
	set_window_size();
	set_subwindow_sizes();
	}

set_subwindow_sizes()
{
	set_window(panel_sw, PANEL_TOP, PANEL_LEFT, PANEL_WID, PANEL_HEIGHT);
	set_window(msg_sw, MSG_TOP, MSG_LEFT, MSG_WIDTH, MSG_HEIGHT);
	set_window(canvas_sw, CANVAS_TOP, CANVAS_LEFT, CANVAS_WIDTH,
			CANVAS_HEIGHT);
	set_window(sideruler_sw, SIDERULER_TOP, SIDERULER_LEFT, RULER_WIDTH,
			SIDERULER_HEIGHT);
	set_window(topruler_sw, TOPRULER_TOP, TOPRULER_LEFT, CANVAS_WIDTH,
			RULER_WIDTH);
	}

set_window(win, t, l, w, h)
TOOLSW		win;
int		t, l, w, h;
{
	RECT		r;

	if (wmgr_iswindowopen(tool->tl_windowfd) )
	    win_getrect(win->ts_windowfd, &r);
	else 
	    win_getsavedrect(win->ts_windowfd, &r);

	if (r.r_top!= t || r.r_left != l || r.r_width != w || r.r_height != h) {
	    r.r_top = t;
	    r.r_left = l;
	    r.r_width = w;
	    r.r_height = h;
	    if (wmgr_iswindowopen(tool->tl_windowfd))
		win_setrect(win->ts_windowfd, &r);
	    else 
		win_setsavedrect(win->ts_windowfd, &r);
	    }
	}
#endif	X11

/*
	if (wmgr_iswindowopen(tool->tl_windowfd) )
	    win_getrect(msg_sw->ts_windowfd, &msg_rect);
	else 
	    win_getsavedrect(msg_sw->ts_windowfd, &msg_rect);
	if ( msg_rect.r_top != MSG_TOP || msg_rect.r_left != MSG_LEFT ||  
		msg_rect.r_width != MSG_WIDTH ||  
		msg_rect.r_height != MSG_HEIGHT) {
	    msg_rect.r_top = MSG_TOP;
	    msg_rect.r_left = MSG_LEFT;
	    msg_rect.r_width = MSG_WIDTH;
	    msg_rect.r_height = MSG_HEIGHT;
	    if (wmgr_iswindowopen(tool->tl_windowfd))
		win_setrect(msg_sw->ts_windowfd, &msg_rect);
	    else 
		win_setsavedrect(msg_sw->ts_windowfd, &msg_rect);
	    }

	if (wmgr_iswindowopen(tool->tl_windowfd) )
	    win_getrect(canvas_sw->ts_windowfd, &canvas_rect);
	else 
	    win_getsavedrect(canvas_sw->ts_windowfd, &canvas_rect);
	if (canvas_rect.r_top != CANVAS_TOP || 
		canvas_rect.r_left != CANVAS_LEFT ||
		canvas_rect.r_width != CANVAS_WIDTH || 
		canvas_rect.r_height != CANVAS_HEIGHT) {
	    canvas_rect.r_top = CANVAS_TOP;
	    canvas_rect.r_left = CANVAS_LEFT;
	    canvas_rect.r_width = CANVAS_WIDTH;
	    canvas_rect.r_height = CANVAS_HEIGHT;
	    if (wmgr_iswindowopen(tool->tl_windowfd))
		win_setrect(canvas_sw->ts_windowfd, &canvas_rect);
	    else 
		win_setsavedrect(canvas_sw->ts_windowfd, &canvas_rect);
	    }

	if (wmgr_iswindowopen(tool->tl_windowfd) )
	    win_getrect(topruler_sw->ts_windowfd, &topruler_rect);
	else 
	    win_getsavedrect(topruler_sw->ts_windowfd, &topruler_rect);
	if (topruler_rect.r_top != TOPRULER_TOP || 
		topruler_rect.r_left != TOPRULER_LEFT ||
		topruler_rect.r_width != CANVAS_WIDTH || 
		topruler_rect.r_height != RULER_WIDTH) {
	    topruler_rect.r_top = TOPRULER_TOP;
	    topruler_rect.r_left = TOPRULER_LEFT;
	    topruler_rect.r_width = TOPRULER_WIDTH;
	    topruler_rect.r_height = TOPRULER_HEIGHT;
	    if (wmgr_iswindowopen(tool->tl_windowfd))
		win_setrect(topruler_sw->ts_windowfd, &topruler_rect);
	    else 
		win_setsavedrect(topruler_sw->ts_windowfd, &topruler_rect);
	    }

	if (wmgr_iswindowopen(tool->tl_windowfd) )
	    win_getrect(sideruler_sw->ts_windowfd, &sideruler_rect);
	else 
	    win_getsavedrect(sideruler_sw->ts_windowfd, &sideruler_rect);
	if (sideruler_rect.r_top != SIDERULER_TOP || 
		sideruler_rect.r_left != SIDERULER_LEFT ||
		sideruler_rect.r_width != RULER_WIDTH || 
		sideruler_rect.r_height != SIDERULER_HEIGHT) {
	    sideruler_rect.r_top = SIDERULER_TOP;
	    sideruler_rect.r_left = SIDERULER_LEFT;
	    sideruler_rect.r_width = SIDERULER_WIDTH;
	    sideruler_rect.r_height = SIDERULER_HEIGHT;
	    if (wmgr_iswindowopen(tool->tl_windowfd))
		win_setrect(sideruler_sw->ts_windowfd, &sideruler_rect);
	    else 
		win_setsavedrect(sideruler_sw->ts_windowfd, &sideruler_rect);
	    }
	}
*/
