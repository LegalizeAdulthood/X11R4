/* 
 * @(#) ol_init.c 1.16 89/11/21 
 */

/*
 * OPEN LOOK object drawing package
 * Sun Microsystems, Inc.,
 */

#include <stdio.h>
#include <X11/Xlib.h>
#include <olgx_private/olgx_impl.h>

extern char *calloc();

/*
 * olgx_initialize(dpy, screen)
 *
 * Mallocs up a Graphics_info structure and initializes the values properly.
 */

Graphics_info *
olgx_initialize(dpy, screen,d_flag)
	Display *dpy;
	int screen;
	int d_flag;
{
	Graphics_info *info;
	Window root;
	int x_ret, y_ret;
	unsigned int w_ret, h_ret, bw_ret;
	int i;

	/*
	 * Malloc a graphics info structure.
	 */
	if (!(info = (Graphics_info *) olgx_malloc(sizeof(Graphics_info))) ) {
		olgx_error("malloc of graphics info failed.");
	}

	/* set the display info */
	info->dpy = dpy;
	info->scrn = screen;
	info->three_d = d_flag;

	/* Set the depth to be that of the root window */
	XGetGeometry(info->dpy, 
		RootWindow(info->dpy, info->scrn), 
		&root, &x_ret, &y_ret, &w_ret, &h_ret, &bw_ret,
		&info->depth);

	/* clear the GC's */
	for (i = 0; i < OLGX_NUM_GCS; i++) {
		info->gc[i] = (GC) 0;
	}

	/* initialize stipples to zero */
	info->busy_stipple = (Pixmap) 0;
	info->grey_stipple = (Pixmap) 0;

	/* everything was successful; return a ptr to the graphics info */
	return(info);
}

/*
 * olgx_set_glyph_font(info, font_info)
 * 
 * Sets the glyph font associated with the given Graphics_info structure, and
 * determines the sizes of certain OPEN LOOK items associated with that font.
 */

void
olgx_set_glyph_font(info, font_info)
	Graphics_info *info;
	XFontStruct *font_info;
{
	int direct, ascent, descent;
	XCharStruct overall;
	char string[2];
	register int i;
	register int num_colors;

	/* don't bother setting this information if it's already valid */
	if (info->glyphfont == font_info) {
		return;
	}

	/* set the font ID */
	info->glyphfont = font_info;

	/* if the glyph/color GC's have not already been created, do so now */
	if (!info->gc[OLGX_BLACK]) {
		olgx_create_GCs(info);
	}
         num_colors = (info->three_d)? OLGX_NUM_COLORS:2;
	for (i = 0; i < num_colors; i++) {
		XSetFont(info->dpy, info->gc[i], info->glyphfont->fid);
	}

	/* get the button height from the size of the endcaps */
	string[0] = BUTTON_UL;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->button_height = (overall.ascent + overall.descent)  ;
	info->endcap_width = overall.width;

	/* get the scrollbar height and width */
	string[0] = VERT_SB_UL;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->sb_height = (overall.ascent + overall.descent);
	info->sb_width = overall.width;

	/* get the abbreviated menu button height and width */
	string[0] = ABBREV_MENU_UL;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->abbrev_height = (overall.ascent + overall.descent);
	info->abbrev_width = overall.width;

	/* get the slider height and width */
	string[0] = HORIZ_SLIDER_CONTROL_UL;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->slider_height = (overall.ascent + overall.descent);
	info->slider_width = overall.width;

	/* get the menu mark height and width */
	string[0] = HORIZ_MENU_MARK_FILL;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->mm_height = (overall.ascent + overall.descent);
	info->mm_width = overall.width;

	/* get the slider height and width */
	string[0] = HORIZ_SLIDER_UL;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->se_height = (overall.ascent + overall.descent);
	info->se_width = overall.width;

	/* get the base_off height */
	string[0] = BASE_OFF_SPECIALCHAR;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->base_off = (overall.ascent + overall.descent);

	/* get the slider_offset height */
	string[0] = SLIDER_CHANNEL_OFFSET;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->slider_offset = overall.width ;

	/* get the push pin height and width */
	 string[0] = PUSHPIN_OUT_BOTTOM;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
       /* 1 has been added to get the correct rect height
	  works with all point sizes */
	info->pp_height = overall.ascent + overall.descent + 1;
	info->pp_width = overall.width;

	/* get the check box height and width */
	string[0] = UNCHECKED_BOX_UL;
	XTextExtents(font_info, string, 1, &direct, &ascent, &descent,
		&overall);
	info->cb_height = (overall.ascent + overall.descent);
	info->cb_width = overall.width;
}


/*
 * olgx_set_text_font(info, font_info)
 * 
 * Sets the text font associated with the given Graphics_info structure.
 * Will create the text GC if necessary.
 */

void
olgx_set_text_font(info, font_info)
	Graphics_info *info;
	XFontStruct *font_info;
{
	XGCValues gcval;

	/* don't bother setting this stuff if it's already set */
	if (info->textfont == font_info) {
		return;
	}

	/* Create the text GC if it does not already exist */
	if (!info->gc[OLGX_TEXTGC]) {
		gcval.fill_style = FillSolid;
		gcval.function = GXcopy;
		info->gc[OLGX_TEXTGC] = XCreateGC(info->dpy, 
			RootWindow(info->dpy, info->scrn),
			(GCFunction | GCFillStyle),
			&gcval);
	
		if (!info->gc[OLGX_TEXTGC]) {
			olgx_error("Text GC creation failed.");
		}
	}

	info->textfont = font_info;
	XSetFont(info->dpy, info->gc[OLGX_TEXTGC], font_info->fid);

/* Create the 2d reverse video text GC if it does not already exist */
if (!info->three_d){
	if (!info->gc[OLGX_TEXTGC_REV]) {
		gcval.fill_style = FillSolid;
		gcval.function = GXcopy;
		info->gc[OLGX_TEXTGC_REV] = XCreateGC(info->dpy, 
			RootWindow(info->dpy, info->scrn),
			(GCFunction | GCFillStyle),
			&gcval);
	
		if (!info->gc[OLGX_TEXTGC_REV]) {
			olgx_error("Text GC creation failed.");
		}
	}
	XSetFont(info->dpy, info->gc[OLGX_TEXTGC_REV], font_info->fid);
    }

}

void
olgx_error(string)
	char *string;
{
	(void) fprintf(stderr,"%s\n",string);
	exit(1);
}

/*
 * olgx_create_GCs(info)
 *
 * Forces the creation of the glyph/color GC's associated with the given
 * Graphics_info structure. This routine does NOT check to see if they already 
 * exist; that is up to the caller. Hence this function is private to the 
 * OLGX library.  
 *
 * The GC's are automatically associated with the root window of the display 
 * and screen of the Graphics_info structure. 
 */

void
olgx_create_GCs(info)
	Graphics_info *info;
{
	register int i;
	XGCValues gcval;
	register int num_colors;

	/*
	 * Create the GC's we need.
	 */
	 if (info->three_d)
	       num_colors = OLGX_NUM_COLORS;
         else  num_colors = 2; /* 2d */

	for (i = 0; i < num_colors; i++) {
		gcval.fill_style = FillSolid;
		gcval.function = GXcopy;
		gcval.graphics_exposures = False;
		info->gc[i] = XCreateGC(info->dpy, 
			RootWindow(info->dpy, info->scrn),
			(GCFunction | GCFillStyle),
			&gcval);
	
		if (!info->gc[i]) {
			olgx_error("GC creation failed.");
		}
	}
}

/*
 * olgx_set_color(info, pixvals)
 *
 * Set the foreground and background colors of the given Graphics_info
 * to the specified pixvals. GC's are created if necessary.
 */

void
olgx_set_color(info, pixvals)
	Graphics_info *info;
	unsigned long pixvals[OLGX_NUM_COLORS];
{
	register int i;
	XGCValues gcval;

	/* 
	 * If the glyph font GC's have not yet been created, create them now 
	 */
	if (!info->gc[OLGX_BLACK]) {
		olgx_create_GCs(info);
	}

	/* 
	 * set the colors in the glyph font GC's 
	 */
	 if (info->three_d){
	for (i = 0; i < OLGX_NUM_COLORS; i++) {
		info->pixvals[i] = pixvals[i];
		gcval.foreground = pixvals[i];
		gcval.background = pixvals[OLGX_BG1];
		XChangeGC(info->dpy, info->gc[i], 
			(GCForeground | GCBackground),
			&gcval);
	}
       }
	else
           {
		info->pixvals[OLGX_WHITE] = pixvals[OLGX_WHITE];
		gcval.foreground = pixvals[OLGX_WHITE];
		gcval.background = pixvals[OLGX_BLACK];
		XChangeGC(info->dpy, info->gc[OLGX_WHITE], 
			(GCForeground | GCBackground),
			&gcval);
		info->pixvals[OLGX_BLACK] = pixvals[OLGX_BLACK];
		gcval.foreground = pixvals[OLGX_BLACK];
		gcval.background = pixvals[OLGX_WHITE];
		XChangeGC(info->dpy, info->gc[OLGX_BLACK], 
			(GCForeground | GCBackground),
			&gcval);
			}
	  

	if (!info->gc[OLGX_TEXTGC]) {
		/* 
		 * Create the text GC if it does not already exist 
		 */
		 
		gcval.foreground =info->pixvals[OLGX_BLACK];
		if (info->three_d)
		gcval.background = info->pixvals[OLGX_BG1];
		else
		gcval.background = info->pixvals[OLGX_WHITE];

		gcval.fill_style = FillSolid;
		gcval.function = GXcopy;
		gcval.graphics_exposures = False;
		info->gc[OLGX_TEXTGC] = XCreateGC(info->dpy, 
			RootWindow(info->dpy, info->scrn),
			(GCFunction | GCFillStyle | GCForeground 
				| GCBackground),
			&gcval);
	
		if (!info->gc[OLGX_TEXTGC]) {
			olgx_error("Text GC creation failed.");
		}
	}
	else {
		/* 
		 * just modify the text GC if it does exist 
		 */
		gcval.foreground = info->pixvals[OLGX_BLACK];
		if (info->three_d)
		gcval.background = info->pixvals[OLGX_BG1];
		else
		gcval.background = info->pixvals[OLGX_WHITE];
		XChangeGC(info->dpy, info->gc[OLGX_TEXTGC], 
			(GCForeground | GCBackground),
			&gcval);
	}
	if (!info ->three_d){
	if (!info->gc[OLGX_TEXTGC_REV]) {
		/* 
		 * Create the text GC if it does not already exist 
		 */
		 
		gcval.foreground =info->pixvals[OLGX_WHITE];
		gcval.background = info->pixvals[OLGX_BLACK];
		gcval.fill_style = FillSolid;
		gcval.function = GXcopy;
		gcval.graphics_exposures = False;
		info->gc[OLGX_TEXTGC_REV] = XCreateGC(info->dpy, 
			RootWindow(info->dpy, info->scrn),
			(GCFunction | GCFillStyle | GCForeground 
				| GCBackground),
			&gcval);
	
		if (!info->gc[OLGX_TEXTGC_REV]) {
			olgx_error("Text GC creation failed.here");
		}
	}
	else {
		/* 
		 * just modify the text-rev GC if it does exist 
		 */
		gcval.foreground = info->pixvals[OLGX_WHITE];
		gcval.background = info->pixvals[OLGX_BLACK];
		XChangeGC(info->dpy, info->gc[OLGX_TEXTGC_REV], 
			(GCForeground | GCBackground),
			&gcval);
	}
   }
}

/*
 * olgx_destroy(info)
 *
 * Destroy all the information and server resources associated with
 * a Graphics_info structure, then free the structure itself.
 *
 * Does not destroy the font structures; these are maintained by the
 * application.
 */

void
olgx_destroy(info)
	Graphics_info *info;
{
	register int i;

	/*
	 * Destroy the stipples, if they exist
	 */

	if (info->busy_stipple) {
		XFreePixmap(info->dpy, info->busy_stipple);
	}

	if (info->grey_stipple) {
		XFreePixmap(info->dpy, info->grey_stipple);
	}

	/*
	 * Destroy the GC's, if they exist
	 */
	for (i = 0; i < OLGX_NUM_GCS; i++) {
		if (info->gc[i]) {
			XFreeGC(info->dpy, info->gc[i]);
		}
	}

	/*
	 * Free the Graphics_info structure
	 */
	
	olgx_free((char *) info);
}

/*
 * olgx_malloc(nbytes)
 *
 * Allocate a given number of bytes and return a pointer to it.
 */

char *
olgx_malloc(nbytes)
	unsigned int nbytes;
{
	return(calloc(nbytes,1));
}

/*
 * olgx_free(memloc)
 *
 * Free space allocated by olgx_malloc().
 */

void
olgx_free(memloc)
	char *memloc;
{
	(void) free(memloc);
}
