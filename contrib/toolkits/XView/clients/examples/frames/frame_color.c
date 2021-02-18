#include <xview/xview.h>
#include <xview/panel.h>
#include <xview/cms.h>

main()
{
    Frame               frame;
    Panel               panel;
    Xv_singlecolor      fg, bg;

    fg.red = 250, fg.green = 230, fg.blue = 20;
    bg.red = 180, bg.green = 100, bg.blue = 20;

    frame = (Frame)xv_create(NULL, FRAME,
        FRAME_BACKGROUND_COLOR, &bg,
        FRAME_FOREGROUND_COLOR, &fg,
        FRAME_INHERIT_COLORS,   TRUE,
        NULL);

    panel = (Panel)xv_create(frame, PANEL, NULL);
    (void) xv_create(panel, PANEL_BUTTON,
        PANEL_LABEL_STRING,    "Push Me",
        NULL);

    xv_main_loop(frame);
}
