#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)icon.c 20.13 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * icon.c - Display icon.
 */

#include <stdio.h>
#include <pixrect/pixrect_hs.h>
#include <xview/rect.h>
#include <xview/rectlist.h>
#include <xview/pixwin.h>
#include <xview_private/icon_impl.h>

Xv_private void
icon_display(icon_public, x, y)
    Icon            icon_public;
    register int    x, y;
{
    struct rect     textrect;
    /* BUG: pkg_private */
    extern Pixrect *frame_bkgrd;
    register Xv_icon_info *icon = ICON_PRIVATE(icon_public);
    register Xv_Window pixwin = icon_public;

    if (icon->ic_flags & ICON_BKGRDGRY || icon->ic_flags & ICON_BKGRDPAT) {
	/*
	 * Cover the icon's rect with pattern
	 */
	(void) xv_replrop(pixwin, x, y,
			icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height,
		   PIX_SRC, (icon->ic_flags & ICON_BKGRDGRY) ? frame_bkgrd :
			  icon->ic_background, 0, 0);
    } else if (icon->ic_flags & ICON_BKGRDCLR ||
	       icon->ic_flags & ICON_BKGRDSET) {
	/*
	 * Cover the icon's rect with solid.
	 */
	(void) pw_writebackground(pixwin, x, y,
			icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height,
		      (icon->ic_flags & ICON_BKGRDCLR) ? PIX_CLR : PIX_SET);
    }
    if (icon->ic_mpr)
	/*
	 * Copy image over gray
	 */
	(void) xv_rop(pixwin,
		    icon->ic_gfxrect.r_left + x, icon->ic_gfxrect.r_top + y,
		      icon->ic_gfxrect.r_width, icon->ic_gfxrect.r_height,
		      PIX_SRC, icon->ic_mpr, 0, 0);
    if (icon->ic_text && (icon->ic_text[0] != '\0')) {
	PIXFONT        *font = (PIXFONT *) xv_get(icon_public, WIN_FONT);
	int             line_leading = font->pf_defaultsize.y;

	if (rect_isnull(&icon->ic_textrect))
	    /* Set text rect to accomodate 1 line at bottom. */
	    rect_construct(&icon->ic_textrect,
			   0, icon->ic_gfxrect.r_height - line_leading,
			   icon->ic_gfxrect.r_width, line_leading);
	/* Blank out area onto which text will go. */
	(void) pw_writebackground(pixwin,
		  icon->ic_textrect.r_left + x, icon->ic_textrect.r_top + y,
		      icon->ic_textrect.r_width, icon->ic_textrect.r_height,
				  PIX_CLR);
	/* Format text into textrect */
	textrect = icon->ic_textrect;
	textrect.r_left += x;
	textrect.r_top += y;
	/* BUG: pkg_private */
	(void) frame_format_string_to_rect(pixwin, icon->ic_text,
					   font, &textrect);
    }
}
