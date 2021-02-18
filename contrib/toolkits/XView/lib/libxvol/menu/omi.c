#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)omi.c 20.33 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Image layout:
 * 
 * Image width = M, Lm, LPr, Pr, Rpr, Rm, M Image height = M, max(LPr, Pr, Rpr),
 * M
 * 
 * laf -- for choice & toggle menus: Image width = M, M, LPr, M, STRING, M, M
 * 
 * M = Margin, Lm = Left Margin, LPr = Left Pixrect, Pr = Pixrect or String, Rpr
 * = Right Pixrect, Rm = Right Margin,
 * 
 */

#include <sys/types.h>
#include <stdio.h>

#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <pixrect/pr_line.h>

#include <X11/Xlib.h>

#include <xview/pkg.h>
#include <xview/generic.h>
#include <xview/pixwin.h>
#include <xview/window.h>
#include <xview_private/omi_impl.h>

extern struct pr_size xv_pf_textwidth();
extern struct pixrect menu_gray50_pr;

#define IMAX(a, b) ((int)(b) > (int)(a) ? (int)(b) : (int)(a))
#define INHERIT_VALUE(f) im->f ? im->f : std_image ? std_image->f : 0

Pkg_private void image_compute_size();
Pkg_private struct pixfont *image_get_font();


/* VARARGS1 */
Pkg_private int
image_set(im, va_alist)
    register struct image *im;
va_dcl				/* WARNING:  ATTR_LIST is not supported. */
{
    register Image_attribute *attr;

    if (!im)
	return FALSE;

    /* zero width/height if any size changes */
    for (attr = (Image_attribute *) & va_alist;
	 *attr; attr = image_attr_next(attr)) {
	switch (attr[0]) {

	  case IMAGE_ACTIVE:
	    im->inactive = FALSE;
	    break;

	  case IMAGE_CENTER:
	    im->center = (int) attr[1];
	    break;

	  case XV_FONT:
	    if (im->font) {
		(void) xv_set(im->font, XV_DECREMENT_REF_COUNT, 0);
	    }
	    im->font = (struct pixfont *) attr[1];
	    if (im->font) {
		(void) xv_set(im->font, XV_INCREMENT_REF_COUNT, 0);
	    }
	    im->width = im->height = 0;
	    break;

	  case IMAGE_INACTIVE:
	    im->inactive = TRUE;
	    break;

	  case IMAGE_INVERT:
	    im->invert = (int) attr[1];
	    break;

	  case XV_LEFT_MARGIN:
	    im->left_margin = (int) attr[1];
	    im->width = im->height = 0;
	    break;

	  case XV_MARGIN:
	    im->margin = (int) attr[1];
	    im->width = im->height = 0;
	    break;

	  case IMAGE_PIXRECT:
	    if (im->free_pr && im->pr)
		(void) pr_destroy(im->pr);
	    im->pr = (struct pixrect *) attr[1];
	    im->width = im->height = 0;
	    break;

	  case IMAGE_RELEASE:
	    im->free_image = TRUE;
	    break;

	  case IMAGE_RELEASE_STRING:
	    im->free_string = TRUE;
	    break;

	  case IMAGE_RELEASE_PR:
	    im->free_pr = TRUE;
	    break;

	  case XV_RIGHT_MARGIN:
	    im->right_margin = (int) attr[1];
	    im->width = im->height = 0;
	    break;

	  case IMAGE_RIGHT_PIXRECT:
	    im->right_prpos.pr = (struct pixrect *) attr[1];
	    im->width = im->height = 0;
	    break;

	    /* laf */
	  case IMAGE_LEFT_PIXRECT:
	    im->left_pr = *(struct pixrect *) attr[1];
	    im->width = im->height = 0;
	    break;

	    /* laf */
	  case IMAGE_WIDTH:
	    im->width = (int) attr[1];
	    break;

	    /* laf */
	  case IMAGE_HEIGHT:
	    im->height = (int) attr[1];
	    break;

	  case XV_LABEL:
	    if (im->free_string && im->string)
		free(im->string);
	    im->string = (char *) attr[1];
	    im->width = im->height = 0;
	    break;

	  default:
	    if (ATTR_PKG_IMAGE == ATTR_PKG(attr[0])) {
		char            dummy[128];

		(void) sprintf(dummy,
		    "image_set: 0x%x not recognized as an image attribute.",
			       attr[0]);
		xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			 XV_ERROR_STRING, dummy,
			 XV_ERROR_PKG_NAME, "Menu",
			 0);
	    }
	    break;

	}
    }

    return TRUE;
}


/* laf */
/* VARARGS2 */
Pkg_private void
image_render(im, std_image, va_alist)
    struct image   *im;
    register struct image *std_image;
va_dcl				/* WARNING:  ATTR_LIST is not supported */
{
    register Image_attribute *attr;
    Xv_opaque       window;
    int             top, left, mid = 0;
    int             inactive = FALSE;
    register int    margin2;
    int             margin, left_margin, right_margin, width, height;
    int             x, y;	/* laf */
    int             left_height, left_width;	/* laf */
    int             render_string = TRUE;
    int             selected = FALSE;
    int             default_image = FALSE;
    int             title = FALSE;
    struct pixfont *font;

    if (!im)
	return;

    for (attr = (Image_attribute *) & va_alist;
	 *attr; attr = image_attr_next(attr)) {
	switch (attr[0]) {

	  case IMAGE_REGION:
	    window = (Xv_opaque) attr[1];
	    left = (int) attr[2];
	    top = (int) attr[3];
	    break;

	  case IMAGE_INACTIVE:
	    inactive = TRUE;
	    break;

	  case IMAGE_RENDER_STRING:
	    render_string = (int) attr[1];
	    break;

 	  case IMAGE_SELECTED:
	    selected = (int) attr[1];
	    break;

	  case IMAGE_DEFAULT:
	    default_image = (int) attr[1];
	    break; 

	  case IMAGE_TITLE:
	    title = (int) attr[1];
	    break;

	  default:
	    if (ATTR_PKG_IMAGE == ATTR_PKG(attr[0])) {
		char            dummy[128];

		(void) sprintf(dummy,
		  "image_render: 0x%x not recognized as a image attribute.",
			       attr[0]);
		xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			 XV_ERROR_STRING, dummy,
			 XV_ERROR_PKG_NAME, "Menu",
			 0);
	    }
	    break;
	}
    }
    margin = INHERIT_VALUE(margin);
    left_margin = INHERIT_VALUE(left_margin);
    right_margin = INHERIT_VALUE(right_margin);
    width = IMAX(im->width, std_image ? std_image->width : 0);
    height = IMAX(im->height, std_image ? std_image->height : 0);
    margin2 = margin << 1;

    mid = top + height / 2;
    left += margin, top += margin;

    if (im->left_pr.pr_data) {
	left_width = im->left_pr.pr_width;
	left_height = im->left_pr.pr_height;
	im->left_pos.x = x = left + margin;
	y = top;
	im->left_pos.y = top + (height - left_height) / 2;
	if (im->left_pr.pr_depth)
	    xv_rop(window, x, y, left_width, left_height, PIX_SRC, &im->left_pr,
		   0, 0);
	else
	    xv_glyph_char(window, im->left_pos.x, im->left_pos.y,
			  left_width, left_height,
		          xv_get(window, WIN_GLYPH_FONT), im->left_pr.pr_data,
		          -1);
    }
    if (im->string && render_string) {

	if (title)
	    font = im->bold_font ? im->bold_font :
		std_image ? std_image->bold_font : 0;
	else
	    font = image_get_font(im, std_image);

	if (*im->string == '\0')
	    return;
	if (INHERIT_VALUE(center)) {
	    struct pr_size  temp;
	    int             pad;

	    temp = xv_pf_textwidth(strlen(im->string), font, im->string);
	    pad = width - margin2 - temp.x;
	    left_margin = pad -
		IMAX(pad / 2, im->right_prpos.pr ?
		     im->right_prpos.pr->pr_width + right_margin + 1 : 0);
	}
	/* laf begin */

	if (im->left_pr.pr_data) {
	    x += (left_width + margin);
	    y += left_height;
	    y -= 2;		/* laf -- I don't know why... */
	} else {
	    x = left + left_margin - font->pf_char[im->string[0]].pc_home.x;
	    y = mid - font->pf_defaultsize.y / 2 -
		font->pf_char[im->string[0]].pc_home.y;
	}
	/* laf end */

	pw_text(window, x, y, PIX_SRC, font, im->string);

	if (im->inactive || inactive) {
	    pw_text(window, x + 1, y, PIX_SRC, font, im->string);
	    xv_replrop(window, left, top,
		       width - margin2, height - margin2,
		       PIX_SRC & PIX_DST, &menu_gray50_pr, 0, 0);
	}
    } else if (im->pr) {
	/* Write pixrect centered in menu item rectangle */
	pw_writebackground(window, left, top, width - margin2, height - margin2,
			   PIX_CLR);
	xv_rop(window, left + (width - margin2 - im->pr->pr_width) / 2,
	       top + (height - margin2 - im->pr->pr_height) / 2,
	       im->pr->pr_width, im->pr->pr_height, PIX_SRC, im->pr, 0, 0);
	/* FIXME: Add inactive mask for pixrects */
    }
    if (im->right_prpos.pr) {
	im->right_prpos.pos.x = left + width - (im->right_prpos.pr->pr_width
			       + (im->string ? right_margin : 0) + margin2);
	im->right_prpos.pos.y = mid - im->right_prpos.pr->pr_height / 2;
	xv_rop(window, im->right_prpos.pos.x, im->right_prpos.pos.y,
	       im->right_prpos.pr->pr_width, im->right_prpos.pr->pr_height,
	       PIX_SRC, im->right_prpos.pr, 0, 0);
    }
    if (im->invert) {
	(void) xv_rop(window,
		      left, top, width - margin2, height - margin2,
		      PIX_NOT(PIX_DST), im->pr, 0, 0);
    }
}


/* VARARGS2 */
Pkg_private     Xv_opaque
image_get(im, std_image, attr)
    struct image   *im, *std_image;
    Image_attribute attr;
{
    Xv_opaque       v = NULL;

    if (!im)
	return NULL;
    switch (attr) {

      case XV_FONT:
	v = (Xv_opaque) image_get_font(im, std_image);
	break;

      case XV_HEIGHT:
	if (!im->height)
	    image_compute_size(im, std_image);
	v = (Xv_opaque) im->height;
	break;

      case IMAGE_PIXRECT:
	v = (Xv_opaque) im->pr;
	break;

      case XV_LABEL:
	v = (Xv_opaque) im->string;
	break;

      case XV_WIDTH:
	if (!im->width)
	    image_compute_size(im, std_image);
	v = (Xv_opaque) im->width;
	break;

    }
    return v;
}


Pkg_private void
image_destroy(im)
    struct image   *im;
{
    if (!im || !im->free_image)
	return;
    if (im->free_string && im->string)
	free(im->string);
    if (im->free_pr && im->pr)
	(void) pr_destroy(im->pr);
}


Pkg_private void
image_compute_size(im, std_image)
    register struct image *im, *std_image;
{
    struct pixfont *font;
    register int    margin2;
    int             margin, left_margin, right_margin;
    int             left_height, new_height, above_baseline, below_baseline;	/* laf */


    margin = INHERIT_VALUE(margin);
    left_margin = INHERIT_VALUE(left_margin);
    right_margin = INHERIT_VALUE(right_margin);
    margin2 = margin << 1;

    if (im->pr) {
	im->button_size.x = im->pr->pr_width;
	im->button_size.y = im->pr->pr_height;
    } else if (im->string) {
	font = image_get_font(im, std_image);
	im->button_size = xv_pf_textwidth(strlen(im->string), font,
					  im->string);
	/* make every string menu item with the same font the same height */
	im->button_size.y = font->pf_defaultsize.y;
    }
    if (im->pr || im->string) {
	im->width = im->button_size.x;
	im->height = im->button_size.y;

	if (im->right_prpos.pr) {
	    im->width += im->right_prpos.pr->pr_width;
	    im->height = IMAX(im->right_prpos.pr->pr_height, im->height);
	}
	im->width += margin2;	/* laf */
	im->height += margin2;
	im->button_pos.x = im->button_pos.y = margin;

	/* laf begin */
	if (im->left_pr.pr_data) {
	    /* choice/toggle layout: M M LPR M STRING M M */
	    im->width += im->left_pr.pr_width;
	    im->width += margin + margin2;
	    left_height = im->left_pr.pr_height;
	    above_baseline = -font->pf_char[im->string[0]].pc_home.y;
	    below_baseline = font->pf_defaultsize.y - above_baseline;
	    new_height = left_height + below_baseline;
	    im->height = IMAX(new_height, im->height);
	} else {
	    /* command layout: M LM LPR STRING RPR RM M */
	    im->width += left_margin + right_margin;
	    im->button_pos.x += left_margin;
	}
	/* laf end */
    } else {
	im->height = im->width = 0;
    }
    im->width = IMAX(im->width, std_image->width);
    im->height = IMAX(im->height, std_image->height);
}

Pkg_private struct pixfont *
image_get_font(im, std_image)
    struct image   *im;
    struct image   *std_image;
{
    return INHERIT_VALUE(font);
}
