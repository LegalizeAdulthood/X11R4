/*
 * color_animate.c -- use glyphs from the "icon" font distributed
 * with XView to do frame-by-frame animation.
 */
#include <stdio.h>
#include <ctype.h>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xos.h>		/* for <sys/time.h> */
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/font.h>
#include <xview/notify.h>
#include <xview/cms.h>

Frame		  frame;
Canvas		  canvas;
Display		 *dpy;
GC		  gc;
Window		  canvas_win;
Notify_value	  animate();
struct itimerval  timer;

#define ArraySize(x)  (sizeof(x)/sizeof(x[0]))
char *horses[] = { "N", "O", "P", "Q", "R" };
char *eyes[] = { "2", "5", "4", "3", "4", "5", "2", "1", "0", "/", "0", "1" };
char *boys[] = { "\007", "\005", "\007", "\010" };
char *men[] = { "\\", "]", "Y", "Z", "[" };

/* indices into color table renders specified colors. */
#define WHITE   0
#define RED     1
#define GREEN   2
#define BLUE    3
#define ORANGE  4
#define AQUA    5
#define PINK    6
#define BLACK   7
#define RANDOM_COLOR   8

int max_images = ArraySize(horses);
char **images = horses;
int cnt;

main(argc, argv)
int	argc;
char	*argv[];
{
    unsigned char red[8], green[8], blue[8];
    Panel	panel;
    XGCValues	gcvalues;
    Xv_Font	_font;
    XFontStruct *font;
    void	start_stop(), adjust_speed(), change_glyph();
    Xv_cmsdata  cms_data;
    long	*colors;
    extern void	exit();

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);

    /* initialize RGB values for specified colors */
    red[WHITE] = 255;   green[WHITE] = 255;   blue[WHITE] = 255;
    red[RED] = 255;     green[RED] = 0;       blue[RED] = 0;
    red[GREEN] = 0;     green[GREEN] = 255;   blue[GREEN] = 50;
    red[BLUE] = 10;     green[BLUE] = 50;     blue[BLUE] = 255;
    red[ORANGE] = 250;  green[ORANGE] = 130;  blue[ORANGE] = 80;
    red[AQUA] = 30;     green[AQUA] = 230;    blue[AQUA] = 250;
    red[PINK] = 230;    green[PINK] = 30;     blue[PINK] = 250;

    cms_data.type = XV_STATIC_CMS;
    cms_data.size = 8;
    cms_data.rgb_count = 8;
    cms_data.index = 0;
    cms_data.red = red;
    cms_data.green = green;
    cms_data.blue = blue;

    frame = (Frame)xv_create(XV_NULL, FRAME,
	FRAME_LABEL,		argv[0],
	FRAME_SHOW_FOOTER,	TRUE,
	NULL);

    panel = (Panel)xv_create(frame, PANEL,
	PANEL_LAYOUT,		PANEL_VERTICAL,
	WIN_CMS_NAME,		"panel",
	WIN_CMS_DATA,		&cms_data,
	NULL);
    xv_create(panel, PANEL_BUTTON,
	PANEL_LABEL_STRING,	"Quit",
	PANEL_NOTIFY_PROC,	exit,
	PANEL_ITEM_COLOR,	RED,
	NULL);
    xv_create(panel, PANEL_SLIDER,
	PANEL_LABEL_STRING,	"Millisecs Between Frames",
	PANEL_VALUE,		0,
	PANEL_MAX_VALUE,	120,
	PANEL_ITEM_COLOR,	BLUE,
	PANEL_NOTIFY_PROC,	adjust_speed,
	NULL);
    xv_create(panel, PANEL_CHOICE,
	PANEL_LABEL_STRING,	"Glyphs",
	PANEL_LAYOUT,		PANEL_HORIZONTAL,
	PANEL_DISPLAY_LEVEL,	PANEL_ALL,
	PANEL_CHOICE_STRINGS,	"Horse", "Man", "Boy", "Eye", NULL,
	PANEL_ITEM_COLOR,	GREEN,
	PANEL_NOTIFY_PROC,	change_glyph,
	NULL);
    window_fit(panel);

    cms_data.type = XV_STATIC_CMS;
    cms_data.rgb_count = cms_data.size = 2;
    canvas = (Canvas)xv_create(frame, CANVAS,
	XV_WIDTH,		64,
	XV_HEIGHT,		64,
	WIN_CMS_NAME,		"rainbow",
	WIN_CMS_DATA,		&cms_data,
        CANVAS_X_PAINT_WINDOW,	TRUE,
        /* WIN_DYNAMIC_VISUAL,     TRUE, */
	CANVAS_RETAINED,	FALSE,
        NULL);
    canvas_win = (Window)xv_get(canvas_paint_window(canvas), XV_XID);

    window_fit(frame);

    dpy = (Display *)xv_get(frame, XV_DISPLAY);
    _font = (Xv_Font)xv_find(frame, FONT,
	FONT_NAME,	"icon",
	NULL);
    font = (XFontStruct *)xv_get(_font, FONT_INFO);

    colors = (long *)xv_get(canvas, WIN_X_COLOR_INDICES);
    gcvalues.font = font->fid;
    gcvalues.graphics_exposures = False;
    gcvalues.foreground = colors[1];
    gcvalues.background = WhitePixel(dpy, DefaultScreen(dpy));
    gc = XCreateGC(dpy, RootWindow(dpy, DefaultScreen(dpy)),
	GCForeground | GCBackground | GCFont | GCGraphicsExposures,
	&gcvalues);

    xv_main_loop(frame);
}

Notify_value
animate()
{
    static unsigned char red, green, blue;
    Xv_cmsdata cms_data;
    long *colors;

    red = (red+1) % 255;
    green = (green+2) % 255;
    blue = (blue+3) % 255;

    cms_data.type = XV_STATIC_CMS;
    cms_data.size = 2;
    cms_data.rgb_count = 2;
    cms_data.index = 1;
    cms_data.red = &red;
    cms_data.green = &green;
    cms_data.blue = &blue;
    xv_set(canvas, WIN_CMS_DATA, &cms_data, NULL);
    colors = (long *)xv_get(canvas, WIN_X_COLOR_INDICES);
    XSetForeground(dpy, gc, colors[1]);

    XDrawImageString(dpy, canvas_win, gc, 5, 40, images[cnt], 1);
    cnt = (cnt + 1) % max_images;

    return NOTIFY_DONE;
}

void
change_glyph(item, value)
Panel_item item;
int value;
{
    cnt = 0;
    if (value == 0) {
	max_images = ArraySize(horses);
	images = horses;
    } else if (value == 1) {
	max_images = ArraySize(men);
	images = men;
    } else if (value == 2) {
	max_images = ArraySize(boys);
	images = boys;
    } else if (value == 3) {
	max_images = ArraySize(eyes);
	images = eyes;
    }
    XClearWindow(dpy, canvas_win);
}

void
adjust_speed(item, value)
Panel_item item;
int value;
{
    if (value > 0) {
	timer.it_value.tv_usec = (value + 20) * 1000;
	timer.it_interval.tv_usec = (value + 20) * 1000;
	notify_set_itimer_func(frame, animate, ITIMER_REAL, &timer, NULL);
    } else
	/* turn it off */
	notify_set_itimer_func(frame, NULL, ITIMER_REAL, NULL, NULL);
}
