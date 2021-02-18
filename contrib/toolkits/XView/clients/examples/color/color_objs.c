/*
 * color_objs.c --
 *    This program demonstrates the use of color in XView. It allows
 *    the user to choose the foreground and background colors of the
 *    various objects in an interactive manner.
 */
#include <X11/Xlib.h>
#include <xview/xview.h>
#include <xview/svrimage.h>
#include <xview/textsw.h>
#include <xview/panel.h>
#include <xview/icon.h>
#include <xview/cms.h>

#define SELECT_TEXTSW           0
#define SELECT_TEXTSW_VIEW      1
#define SELECT_SCROLLBAR        2
#define SELECT_PANEL            3
#define SELECT_ICON             4

#define NUM_COLORS              8

/* Icon data */
static short icon_bits[256]={
#include "cardback.icon"
};

Panel_item      objects;
Textsw          textsw;
Panel           panel;
Icon            icon;

/*
 * main()
 *    Create a panel and panel items. The application uses panel items
 *    to choose a particular object and change its foreground and
 *    background colors in an interactive manner. Create a textsw.
 *    Create an icon. All the objects share the same colormap segment.
 */
main(argc,argv)
int     argc;
char    *argv[];
{
    Frame         frame;
    Panel_item    panel_palette, panel_fg_bg;
    Xv_cmsdata    cms_data;
    unsigned char red[NUM_COLORS], green[NUM_COLORS], blue[NUM_COLORS];
    Server_image  chips[NUM_COLORS], icon_image;
    void          color_notify();
    extern void   exit();

    xv_init(XV_INIT_ARGC_PTR_ARGV, &argc, argv, NULL);

    /* Set up the RGB values and initialize the cms */
    initialize_cms_data(&cms_data, red, green, blue);

    frame = (Frame)xv_create(NULL, FRAME,
        FRAME_LABEL,    argv[0],
        NULL);

    /* Create panel and set the colormap segment on the panel */
    panel = (Panel)xv_create(frame, PANEL,
        PANEL_LAYOUT,   PANEL_VERTICAL,
        WIN_CMS_NAME,   "palette",
        WIN_CMS_DATA,   &cms_data,
        NULL);

    /* Create panel items */
    objects = (Panel_item)xv_create(panel, PANEL_TOGGLE,
        PANEL_LABEL_STRING,     "Objects",
        PANEL_LAYOUT,           PANEL_HORIZONTAL,
        PANEL_CHOICE_STRINGS,   "Textsw", "Textsw View",
                                "Scrollbar", "Panel", "Icon", NULL,
        NULL);

    panel_fg_bg = (Panel_item)xv_create(panel, PANEL_CHECK_BOX,
        PANEL_LABEL_STRING,     "Fg/Bg",
        PANEL_CHOOSE_ONE,       TRUE,
        PANEL_LAYOUT,           PANEL_HORIZONTAL,
        PANEL_CHOICE_STRINGS,   "Background", "Foreground", NULL,
        NULL);

    /* Initialize colored chips to be used as panel choice images */
    initialize_chips(panel, chips);
    panel_palette = (Panel_item)xv_create(panel, PANEL_CHOICE,
        PANEL_LAYOUT,           PANEL_HORIZONTAL,
        PANEL_LABEL_STRING,     "Colors",
        PANEL_CLIENT_DATA,      panel_fg_bg,
        XV_X,                   (int)xv_get(panel_fg_bg, XV_X),
        PANEL_NEXT_ROW,         15,
        PANEL_CHOICE_IMAGES,
            chips[0], chips[1], chips[2], chips[3], chips[4],
            chips[5], chips[6], chips[7], NULL,
        PANEL_NOTIFY_PROC,      color_notify,
        NULL);

    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,     "Quit",
        PANEL_NOTIFY_PROC,      exit,
        NULL);
    (void)window_fit_height(panel);

    /* create textsw and set the colormap segment for it */
    textsw = (Textsw)xv_create(frame, TEXTSW,
       WIN_CMS_NAME,    "palette",
       WIN_BELOW,       panel,
       WIN_ROWS,        15,
       WIN_COLUMNS,     80,
       NULL);

    /* adjust panel dimensions */
    (void)xv_set(panel, WIN_WIDTH, xv_get(textsw, WIN_WIDTH), NULL);

    icon_image = (Server_image)xv_create(NULL, SERVER_IMAGE,
        XV_WIDTH,               64,
        XV_HEIGHT,              64,
        SERVER_IMAGE_DEPTH,     1,
        SERVER_IMAGE_BITS,      icon_bits,
        NULL);
    /* associate icon with the base frame */
    icon = (Icon)xv_create(XV_NULL, ICON,
         ICON_IMAGE,    icon_image,
         WIN_CMS_NAME,  "palette",
         NULL);
    xv_set(frame, FRAME_ICON, icon, NULL);

    window_fit(frame);

    xv_main_loop(frame);
}

/*
 * initialize_cms_data()
 *    Initialize the colormap segment data and setup the RGB values.
 */
initialize_cms_data(cms_data, red, green, blue)
Xv_cmsdata      *cms_data;
unsigned char   *red, *green, *blue;
{
    /* white  */ (red)[0] = 255;  (green)[0] = 255;  (blue)[0] = 255;
    /* red    */ (red)[1] = 255;  (green)[1] = 0;    (blue)[1] = 0;
    /* green  */ (red)[2] = 0;    (green)[2] = 255;  (blue)[2] = 0;
    /* blue   */ (red)[3] = 0;    (green)[3] = 0;    (blue)[3] = 255;
    /* yellow */ (red)[4] = 255;  (green)[4] = 255;  (blue)[4] = 0;
    /* brown  */ (red)[5] = 188;  (green)[5] = 143;  (blue)[5] = 143;
    /* gray   */ (red)[6] = 220;  (green)[6] = 220;  (blue)[6] = 220;
    /* black  */ (red)[7] = 0;    (green)[7] = 0;    (blue)[7] = 0;

    cms_data->type = XV_STATIC_CMS;
    cms_data->size = NUM_COLORS;
    cms_data->rgb_count = NUM_COLORS;
    cms_data->index = 0;
    cms_data->red = red;
    cms_data->green = green;
    cms_data->blue = blue;
}

/*
 * initialize_chips()
 *    Create colored server images (pixmaps) to be used as
 * panel choice images.
 */
initialize_chips(panel, chips)
Panel           panel;
Server_image    chips[];
{
    register int   i;
    Display       *dpy = (Display *)xv_get(panel, XV_DISPLAY);
    unsigned long  mask, *pixels;
    XGCValues      values;
    Pixmap         pixmap;
    GC             gc;

    if (DefaultDepth(dpy, DefaultScreen(dpy)) == 1) {
        puts("You need a color display to run this program");
        exit(1);
    }
    /* setup gc for drawing colord squares (chips) */
    values.graphics_exposures = False;
    values.foreground = 1;
    mask = GCGraphicsExposures | GCForeground;
    gc = XCreateGC(dpy,
        RootWindow(dpy, DefaultScreen(dpy)), mask, &values);

    /* Get the actual X pixel values from the colormap.  The colormap
     * has been installed on the panel, so use that.  Any window with
     * a cms colormap will do.
     */
    pixels = (unsigned long *)xv_get(panel, WIN_X_COLOR_INDICES);
    for(i = 0; i < NUM_COLORS; i++) {
        chips[i] = (Server_image)xv_create(XV_NULL, SERVER_IMAGE,
            SERVER_IMAGE_COLORMAP, "palette",
            XV_WIDTH,           16,
            XV_HEIGHT,          16,
            SERVER_IMAGE_DEPTH, 8,
            NULL);
        XSetForeground(dpy, gc, pixels[i]);
        pixmap = (Pixmap)xv_get(chips[i], XV_XID);
        XFillRectangle(dpy, pixmap, gc, 0, 0, 16, 16);
    }
}

/*
 * This routine gets called when a color selection is made.
 * Set the foreground or background on the currently selected object.
 * WIN_FOREGROUND_COLOR & WIN_BACKGROUND_COLOR allow the application
 * to specify indices into the colormap segment as the foreground
 * and background values.
 */
void
color_notify(panel_item, choice, event)
Panel_item      panel_item;
int             choice;
Event           *event;
{
    int cnt;
    Xv_opaque get_object();
    unsigned objs = (unsigned)xv_get(objects, PANEL_VALUE);
    int fg = (int)xv_get(xv_get(panel_item, PANEL_CLIENT_DATA),
                     PANEL_VALUE);

    /* the value of the objects panel item is a bit mask ... "on" bits
     * mean that the choice is selected.  Get the object associated
     * with the choice and set it's color.  "&" tests bits in a mask.
     */
    for (cnt = 0; objs; cnt++, objs >>= 1)
        if (objs & 1)
            xv_set(get_object(cnt),
                fg? WIN_FOREGROUND_COLOR : WIN_BACKGROUND_COLOR,
                choice, NULL);
}

/*
 *    Return the XView handle to nth object.
 */
Xv_opaque
get_object(n)
int n;
{
    switch (n) {
        case SELECT_TEXTSW:
            return textsw;
        case SELECT_TEXTSW_VIEW:
            return xv_get(textsw, OPENWIN_NTH_VIEW, 0);
        case SELECT_SCROLLBAR:
            return xv_get(textsw, OPENWIN_VERTICAL_SCROLLBAR,
                                  xv_get(textsw, OPENWIN_NTH_VIEW, 0));
        case SELECT_PANEL:
            return panel;
        case SELECT_ICON:
            return icon;
        default:
            return textsw;
    }
}
