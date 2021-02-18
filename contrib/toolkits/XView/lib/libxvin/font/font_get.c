#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)font_get.c 20.17 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <stdio.h>
#include <xview/attr.h>
#include <xview_private/font_impl.h>

/*
 * Externed functions
 */

extern struct pr_size xv_pf_textwidth();

/*
 * Private
 */

Pkg_private     Xv_opaque
font_get_attr(font_public, status, attr, args)
    Xv_font_struct *font_public;
    int            *status;	/* initialized by caller */
    Font_attribute  attr;
    va_list         args;
{
    register Font_info *font = FONT_PRIVATE(font_public);
    register Pixfont *pf = (Pixfont *) (font_public->pixfont);
    Xv_opaque       v;
    int             attr_is_char_width = 0;
    Attr_avlist     avlist = (Attr_avlist) args;

    switch (attr) {
      case XV_XID:
	v = (Xv_opaque) font->xid;
	break;

      case FONT_NAME:
	v = (Xv_opaque) font->name;
	break;

      case FONT_FAMILY:
	if (font->family) {
	    v = (Xv_opaque) font->family;
	    break;
	} else {
	    *status = XV_ERROR;
	    v = (Xv_opaque) 0;
	    break;
	}

      case FONT_STYLE:
	if (font->style) {
	    v = (Xv_opaque) font->style;
	    break;
	} else {
	    *status = XV_ERROR;
	    v = (Xv_opaque) 0;
	    break;
	}

      case FONT_SIZE:
	if (font->size) {
	    v = (Xv_opaque) font->size;
	    break;
	} else {		/* BUG: could query X font property?? */
	    *status = XV_ERROR;
	    v = (Xv_opaque) 0;
	    break;
	}

      case FONT_INFO:
	if (font->x_font_info) {
	    v = (Xv_opaque) font->x_font_info;
	    break;
	} else {		/* BUG: could query X font property?? */
	    *status = XV_ERROR;
	    v = (Xv_opaque) 0;
	    break;
	}

      case FONT_SCALE:
	if (font->scale != FONT_NO_SCALE) {
	    v = (Xv_opaque) font->scale;
	    break;
	} else {
	    *status = XV_ERROR;
	    v = (Xv_opaque) 0;
	    break;
	}

      case FONT_TYPE:
	v = (Xv_opaque) font->type;
	break;

      case FONT_DEFAULT_CHAR_HEIGHT:
	v = (Xv_opaque) pf->pf_defaultsize.y;
	break;

      case FONT_DEFAULT_CHAR_WIDTH:
	v = (Xv_opaque) pf->pf_defaultsize.x;
	break;

      case FONT_CHAR_WIDTH:
	attr_is_char_width = TRUE;
      case FONT_CHAR_HEIGHT:{
	    char            font_char = (char) va_arg(args, int);
	    char            font_char_array[2];
	    struct pr_size  my_pf_size;

	    font_char_array[0] = font_char;
	    font_char_array[1] = (char) 0;
	    my_pf_size = xv_pf_textwidth(1, pf, font_char_array);
	    if (attr_is_char_width) {
		v = (Xv_opaque) my_pf_size.x;
	    } else
		v = (Xv_opaque) my_pf_size.y;
	    break;
	}

      case FONT_STRING_DIMS:{
	    char           *string = (char *) avlist[0];
	    Font_string_dims *size = (Font_string_dims *) avlist[1];
	    struct pr_size  my_pf_size;
	    if (string) {
		my_pf_size = xv_pf_textwidth(strlen(string), pf, string);
		size->width = my_pf_size.x;
		size->height = my_pf_size.y;
		v = (Xv_opaque) size;
		break;
	    } else {
		*status = XV_ERROR;
		v = (Xv_opaque) size;
		break;
	    }
	}

      default:
	if (xv_check_bad_attr(&xv_font_pkg, attr) == XV_ERROR) {
	    *status = XV_ERROR;
	}
	v = (Xv_opaque) 0;
	break;

    }
    return (Xv_opaque) v;
}
