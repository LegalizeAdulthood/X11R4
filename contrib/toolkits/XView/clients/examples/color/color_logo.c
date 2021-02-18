/*
 * color_logo.c --
 *  This program demonstrates the combined use of the XView color
 *  model/API and Xlib graphics calls. The program uses XView to
 *  create and manage its colormap segment while doing its actual
 *  drawing using Xlib routines.
 *  The program draws the X logo in red, green and blue in a canvas.
 */
#include <X11/Xlib.h>
#include <X11/bitmaps/xlogo64>
#include <xview/xview.h>
#include <xview/canvas.h>
#include <xview/cms.h>
#include <xview/xv_xrect.h>

/* Color indices */
#define WHITE           0
#define RED             1
#define GREEN           2
#define BLUE            3
#define NUM_COLORS      4

/* graphics context used for rendering logos */
GC      gc;

/* table of pixel values for colors */
unsigned long    *pixel_table;

/*
 * Create a frame and a canvas.
 * Allocate read-only colors (called a static colormap segment in
 * XView parlance) and associate colors with the canvas.  The indices
 * into an XView colormap segment always range from 0 to size-1, where
 * size is the number of colors allocated in the colormap segment.
 * These logical index values translate into actual indices into the
 * colormap map as allocated by the X server. The WIN_X_COLOR_INDICES
 * attribute returns the actual colormap indices. The indices are
 * returned as an array of unsigned longs.
 */
main(argc,argv)
int     argc;
char    *argv[];
{
    Frame         frame;
    Canvas        canvas;
    Xv_cmsdata    cms_data;       
    unsigned char red[NUM_COLORS], green[NUM_COLORS], blue[NUM_COLORS];
    Display       *display;
    XGCValues     gc_val;
    XID           xid;
    Pixmap        xlogo;
    XGCValues     gcvalues;
    int           gcvaluemask;
    void          canvas_repaint_proc();

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);

    /* initalize the cms data and set the required RGB values */
    initialize_cms_data(&cms_data, red, green, blue);

    frame = (Frame)xv_create(XV_NULL, FRAME,
        FRAME_LABEL,    argv[0],
        XV_WIDTH,       448,
        XV_HEIGHT,      192,
        NULL);

    canvas = (Canvas)xv_create(frame, CANVAS,
        CANVAS_X_PAINT_WINDOW,  TRUE,
        CANVAS_REPAINT_PROC,    canvas_repaint_proc,
        WIN_CMS_NAME,           "palette",
        WIN_CMS_DATA,           &cms_data,
        NULL);

    /* Get the actual indices into the colormap */
    pixel_table = (int *)xv_get(canvas, WIN_X_COLOR_INDICES);

    /* Get display and the XID of the canvas */
    display = (Display *)xv_get(canvas, XV_DISPLAY);
    xid = (XID)xv_get(canvas, XV_XID);

    /* create the stipple xlogo */
    xlogo = XCreateBitmapFromData(display, xid, xlogo64_bits,
                      xlogo64_width, xlogo64_height);

    /* setup gc for rendering logos to screen */
    gcvalues.function = GXcopy;
    gcvalues.stipple = xlogo;
    gcvalues.fill_style = FillStippled;
    gcvalues.graphics_exposures = False;
    gcvaluemask = GCFunction|GCStipple|GCFillStyle|GCGraphicsExposures;

    /* create normal render gc for logo rendering */
    gc = XCreateGC(display, xid, gcvaluemask, &gcvalues);

    /* Start event loop */
    xv_main_loop(frame);
}

/*
 * Draws onto the canvas using Xlib drawing functions.
 * Draw the X logo into the window in three colors. In each case,
 * change the GC's foreground color to the pixel value specified.
 */
void
canvas_repaint_proc(canvas, pw, display, xid, xrects)
Canvas      canvas;
Xv_Window   pw;
Display     *display;
Window      xid;
Xv_xrectlist *xrects;
{
    XGCValues       gc_val;
    unsigned long   pixel_value;

    /* draw the logos in red, green and blue */
    XSetForeground(display, gc, pixel_table[RED]);
    XFillRectangle(display, xid, gc, 64, 64,
        xlogo64_width, xlogo64_height);

    XSetForeground(display, gc, pixel_table[GREEN]);
    XFillRectangle(display, xid, gc, 192, 64,
        xlogo64_width, xlogo64_height);

    XSetForeground(display, gc, pixel_table[BLUE]);
    XFillRectangle(display, xid, gc, 320, 64,
        xlogo64_width, xlogo64_height);
}

/*
 * initialize_cms_data()
 *    Initialize the colormap segment data and setup the RGB values.
 */
initialize_cms_data(cms_data, red, green, blue)
Xv_cmsdata      *cms_data;
unsigned char   *red, *green, *blue;
{
    (red)[WHITE] = 255;  (green)[WHITE] = 255;  (blue)[WHITE] = 255;
    (red)[RED] = 255;    (green)[RED] = 0;      (blue)[RED] = 0;
    (red)[GREEN] = 0;    (green)[GREEN] = 255;  (blue)[GREEN] = 0;
    (red)[BLUE] = 0;     (green)[BLUE] = 0;     (blue)[BLUE] = 255;

    cms_data->type = XV_STATIC_CMS;
    cms_data->size = 4;
    cms_data->rgb_count = 4;
    cms_data->index = 0;
    cms_data->red = red;
    cms_data->green = green;
    cms_data->blue = blue;
}
