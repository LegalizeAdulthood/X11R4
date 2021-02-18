/*
 * init.c
 */

# include	<X11/Xlib.h>
# include	"control.h"
# include	<assert.h>
# include	"co_class.h"
# include	<stdio.h>

Button		co_OK, co_CANCEL, co_NEW;
Button		co_QUIT, co_SAVE;
Scrollbar	co_SCROLL_LEFT_MENU, co_SCROLL_BOTTOM_MENU;
XFontStruct	*co_font;
int		co_fore, co_back, co_background, co_border;
int		co_inited;
GC		co_fore_gc, co_back_gc;
extern Display	*dpy;

co_init (font_name, bold_font_name)
char	*font_name, *bold_font_name;
{
	XColor	hard_def, exact_def;
	int	screen;
	Colormap	def_cm;
	XGCValues	xcv;
	
	screen = DefaultScreen(dpy);
	def_cm = DefaultColormap(dpy, screen);
	
/*	ui_getcolors (&co_fore, &co_back, &co_fore, &co_back); */
#ifdef COLOR
	XAllocNamedColor (dpy, def_cm, "black", &hard_def, &exact_def);
	co_fore = co_border = hard_def.pixel;
	XAllocNamedColor (dpy, def_cm, "white", &hard_def, &exact_def);
	co_back = co_background = hard_def.pixel;
#else
	co_fore = co_border = BlackPixel(dpy, screen);
	co_back = co_background = WhitePixel(dpy, screen);
#endif
	co_font = XLoadQueryFont (dpy, bold_font_name);
	assert (co_font);

	xcv.font = co_font->fid;
	xcv.foreground = co_fore;
	xcv.background = co_back;
	co_fore_gc = XCreateGC(dpy, RootWindow(dpy, screen),
			       GCBackground | GCForeground | GCFont, &xcv);
	
	xcv.foreground = co_back;
	xcv.background = co_fore;
	co_back_gc = XCreateGC(dpy, RootWindow(dpy, screen),
			       GCBackground | GCForeground | GCFont, &xcv);
	
	co_OK = CcreateButton ("OK", 75, co_fore_gc, co_font, co_back, 1);
	co_CANCEL = CcreateButton ("NO", 75, co_fore_gc, co_font, co_back, 1);
	co_NEW = CcreateButton ("NEW", 75, co_fore_gc, co_font, co_back, 1);
	co_QUIT = CcreateButton ("QUIT", 75, co_fore_gc, co_font, co_back, 1);
	co_SAVE = CcreateButton ("SAVE", 75, co_fore_gc, co_font, co_back, 1);
	co_inited = 1;
	return 0;
}
