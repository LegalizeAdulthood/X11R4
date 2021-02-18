#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)font_x.c 20.21 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <sys/types.h>
#include <xview/pkg.h>
#include <X11/Xlib.h>
#include <pixrect/pixrect.h>
#include <xview/font.h>		/* for FONT_INFO */

extern Pixrect *xv_mem_create();

/*
 * PERFORMANCE BUG!  [Space traded for time] glyph_pixmap/_gc/_pr are left
 * around for possible re-use.
 */
 /* static Display *glyph_display; */	/* = 0 for -A-R */
 /* static Pixmap   glyph_pixmap; */	/* = 0 for -A-R */
 /* static GC       glyph_gc; */	/* = 0 for -A-R */
 /* static int      gp_height, gp_width; */	/* = 0 for -A-R */
 /* static Pixrect *glyph_pr; */

Xv_private      XID
xv_load_x_font(display, name, want_glyph, font_opaque, default_x, default_y,
	       max_char, min_char)
    register Display *display;
    char           *name;
    Xv_opaque      *font_opaque;
    int            *default_x, *default_y, *max_char, *min_char;
    int             want_glyph;
{
    register XFontStruct *font;

#ifdef _XV_DEBUG
#define ERROR	abort()
#else
#define ERROR	goto Error_Return
#endif

    /* Complain iff error from server is not BadName (which we patch up). */
    xv_set_error_handler((int (*)) 2, BadName);
    font = XLoadQueryFont(display, name);
    xv_set_error_handler((int (*)) 0, FALSE);
    *font_opaque = (Xv_opaque) font;
    if (font) {
	/* Extract the global information from the font */
	/*
	 * default_x = font->max_bounds.lbearing + font->max_bounds.rbearing;
	 */
	*default_x = font->max_bounds.width;
	*default_y = font->ascent + font->descent;
	if (font->min_byte1 || font->max_byte1)
	    ERROR;
	*max_char = MIN(255, font->max_char_or_byte2);	/* pixfont compat */
	*min_char = MIN(255, font->min_char_or_byte2);	/* pixfont compat */
	return (font->fid);
    } else {
Error_Return:
	if (font) {
	    XFreeFont(display, font);
	}
	return (None);
    }
#undef ERROR
}

Xv_private void
xv_x_char_info(font, i, x_home, y_home, x_advance, y_advance, pr)
    XFontStruct    *font;
    int             i;
    int            *x_home, *y_home, *x_advance, *y_advance;
    Pixrect       **pr;
/* Caller must guarantee that i is a valid character index for this font. */
{
    register XCharStruct *per_char;

    per_char = (font->per_char) ? &(font->per_char[i]) : &font->max_bounds;
#ifdef XV_DEBUG_OLD
    if (per_char->width != per_char->rbearing || per_char->descent != 0) {
	printf("xv_pf_textbound is screwed up! Tell someone!\n");
    }
#endif				/* XV_DEBUG_OLD */
    *x_advance = per_char->width;
    *y_advance = 0;
    *x_home = per_char->lbearing;
    *y_home = -font->ascent;
    /*
     * PERFORMANCE/BUG:  We are creating a 0-width 0-height memory pixrect,
     * just so pr->pr_height/width can hold the correct size of the
     * character.  This is done because some old SunView1 library code
     * reaches into the height and width of a pixrect representing a
     * character.  It's too much pain at this point to try to rewrite that
     * part of textsw and ttysw, so we have this disgusting hack.  Note that
     * this increases the size of data segment by 20 bytes per character in
     * the font.
     */
    *pr = xv_mem_create(0, 0, 1);
    (*pr)->pr_height = font->ascent + font->descent;
    (*pr)->pr_width = per_char->width;
    /*
     * (*pr)->pr_width = per_char->rbearing - per_char->lbearing;
     */
}

Xv_public void
xv_real_baseline_when_using_pf(font, ch, x_x, x_y, pr_x, pr_y)
    Xv_opaque       font;
    int             ch, *x_x, *x_y, pr_x, pr_y;
{
    XCharStruct    *per_char;
    XFontStruct    *x_font;

    x_font = (XFontStruct *) xv_get(font, FONT_INFO);
    per_char = (x_font->per_char) ? &(x_font->per_char[ch])
	: &(x_font->max_bounds);
    *x_x = pr_x;
    *x_y = pr_y + per_char->ascent;
}

Xv_private void
xv_unload_x_font(display, font_opaque)
    register Display *display;
    Xv_opaque       font_opaque;
{
    XFreeFont(display, (XFontStruct *) font_opaque);
}
