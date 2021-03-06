/*
 * simple_notice.c -- Demonstrate the use of notices.
 */
#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/notice.h>

main(argc,argv)
int     argc;
char    *argv[];
{
    Frame       frame;
    Panel       panel;
    Xv_opaque   my_notify_proc();

    /*
     * Initialize XView, create a frame, a panel and one panel button.
     */
    xv_init(XV_INIT_ARGS, argc, argv, NULL);

    frame = (Frame)xv_create(XV_NULL, FRAME, NULL);
    panel = (Panel)xv_create(frame, PANEL, NULL);
    xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,     "Quit",
        PANEL_NOTIFY_PROC,      my_notify_proc,
        NULL);

    /* make sure everything looks good */
    window_fit(panel);
    window_fit(frame);

    /* start window event processing */
    xv_main_loop(frame);
}

/*
 * my_notify_proc() -- called when the user selects the Quit button.
 *      The notice appears as a result of notice_prompt().  Here
 *      the user must chooses YES or NO to confirm or deny quitting.
 */
Xv_opaque
my_notify_proc(item, event)
Panel_item  item;
Event      *event;
{
    int         result;
    Panel       panel = (Panel)xv_get(item, PANEL_PARENT_PANEL);
    Window      win;
    Window      root = (Window)xv_get(xv_get(panel, XV_ROOT), XV_XID);
    int         x, y;

    XTranslateCoordinates(xv_get(panel, XV_DISPLAY),
        xv_get(panel, XV_XID), root, event_x(event), event_y(event),
        &x, &y, &win);

    result = notice_prompt(panel, NULL,
        NOTICE_MESSAGE_STRINGS, "Do you really want to quit?", NULL,
        NOTICE_FOCUS_XY,        x, y,
        NOTICE_BUTTON_YES,      "Yes",
        NOTICE_BUTTON_NO,       "No",
        NULL);

    if (result == NOTICE_YES)
        exit(0);
}
