/*
 * XLife Copyright 1989 Jon Bennett jb7m+@andrew.cmu.edu, jcrb@cs.cmu.edu
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the copyright holders not be used in
 * advertising or publicity pertaining to distribution of the software without
 * specific, written prior permission.  The copyright holders make no
 * representations about the suitability of this software for any purpose.  It
 * is provided "as is" without express or implied warranty.
 *
 * THE COPYRIGHT HOLDERS DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
 * TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * CMU SUCKS
 */

#include "defs.h"
#include "icon.h"
#include "struct.h"
#include "cursor_data.h"
#include "cursor_mask.h"
#include "patchlevel.h"

main(argc, argv)
int argc;
char **argv;
{
    int i;
    Pixmap icon,cursor_data,cursor_mask;
    XSizeHints hints;
    XSetWindowAttributes winat;
    XFontStruct *efont;
    XColor white,black,dummy;
    
    if (!(disp = XOpenDisplay(getenv("DISPLAY"))))
	fatal("Can't open Display\n");

    screen = DefaultScreen(disp);
    rootw = RootWindow(disp, screen);
    fcolor = WhitePixel(disp, screen);
    bcolor = BlackPixel(disp, screen);

    mainw = XCreateSimpleWindow(disp, rootw, MAINX, MAINY, MAINWIDTH, MAINHEIGHT, BORDERWIDTH, fcolor, bcolor);

    if(!mainw)
	fatal("Can't open main window");

    hints.min_width = 0;
    hints.max_width = MAINWIDTH;
    hints.min_height = 0;
    hints.max_height = MAINHEIGHT;
    hints.flags = PMinSize | PMaxSize;
    
    icon = XCreateBitmapFromData(disp, mainw, icon_bits, icon_width, icon_height);
    XSetStandardProperties(disp, mainw, "Xlife: the game of bit manipulation", "Xlife", icon, argv, argc, &hints);

    xgcv.background = bcolor;
    xgcv.foreground = bcolor;
    blackgc = XCreateGC(disp, mainw, GCForeground | GCBackground, &xgcv);
    xgcv.foreground = fcolor;
    ntextgc = XCreateGC(disp, mainw, GCForeground | GCBackground, &xgcv);
    btextgc = XCreateGC(disp, mainw, GCForeground | GCBackground, &xgcv);
    whitegc = XCreateGC(disp, mainw, GCForeground | GCBackground, &xgcv);
    
    if(!((nfont = XLoadQueryFont(disp, NORMALFONT)) && (bfont = XLoadQueryFont(disp, BOLDFONT))))
	fatal("Can't load font\n");
    XSetFont(disp, ntextgc, nfont->fid);
    XSetFont(disp, btextgc, bfont->fid);
    xgcv.function = GXcopy;
    xgcv.plane_mask = 1;

    XAllocNamedColor(disp, DefaultColormap(disp, screen), "white", &white, &dummy);
    XAllocNamedColor(disp, DefaultColormap(disp, screen), "black", &black, &dummy);
    cursor_data = XCreateBitmapFromData(disp, mainw, cursor_data_bits, cursor_data_width, cursor_data_height);
    cursor_mask = XCreateBitmapFromData(disp, mainw, cursor_mask_bits, cursor_mask_width, cursor_mask_height);
    cursor = XCreatePixmapCursor(disp, cursor_data, cursor_mask, &white, &black, cursor_data_x_hot, cursor_data_y_hot);
    XDefineCursor(disp, mainw, cursor);
    
    inputgc = XCreateGC(disp, mainw, GCFunction | GCPlaneMask, &xgcv);

    lifew = XCreateSimpleWindow(disp, mainw, LIFEX, LIFEY, LIFEW, LIFEH, BORDERWIDTH, fcolor, bcolor);
    inputw = XCreateSimpleWindow(disp, mainw, INPUTX, INPUTY, INPUTW, INPUTH, BORDERWIDTH, fcolor, bcolor);

    winat.win_gravity = SouthGravity;
    XChangeWindowAttributes(disp,inputw,CWWinGravity,&winat);
    
    XSelectInput(disp,mainw, ExposureMask | StructureNotifyMask);
    XSelectInput(disp, inputw, KeyPressMask | ExposureMask);
    XSelectInput(disp, lifew, KeyPressMask | ButtonPressMask | Button1MotionMask | Button3MotionMask | ExposureMask);

    XMapWindow(disp, inputw);
    XMapWindow(disp, lifew);
    XMapWindow(disp, mainw);

    XFlush(disp);
    bzero(cells,sizeof(*cells));
    
    xpos = ypos = STARTPOS;
    dispcells=0;
    scale = 1;
    born = 8;
    live = 12;
    maxdead = 2;
    generations = 0;
    numboxes = 0;
    numcells = 0;
    width = MAINWIDTH;
    height = MAINHEIGHT;
    inputlength = width/FONTWIDTH;
    chdir(DIR);
    strcpy(loadirbuf,DIR);
    strcpy(savedirbuf,gethome());
    state=0;


    /* the one actual comment :-). only accept one pattern since it is highly
     *  unlikely that overlaying n patterns is what you want to do
     */
    if (argc > 1)
	do_loadfile(argv[1]);

    for(;;) {
	while(XCheckMaskEvent(disp, KeyPressMask | ButtonPressMask | Button1MotionMask | Button3MotionMask | ExposureMask | StructureNotifyMask,&event)){
	    switch(event.type) {
	      case KeyPress:
		DoKey();
		break;
	      case MotionNotify:
		Motion();
		break;
	      case ButtonPress:
		Button();
		break;
	      case ConfigureNotify:
		DoResize();
		break;
	      case Expose:
		DoExpose(ClassifyWin(event.xexpose.window));
	      default:
		break;
	    }
	}
	if ((state== RUN) |(state== HIDE)) generate();
    }
}

