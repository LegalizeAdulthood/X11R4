#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)sb_set.c 1.30 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Module:	sb_set.c
 * 
 * Description:
 * 
 * sets scrollbar attributes
 * 
 */


/*
 * Include files:
 */

#include <xview_private/sb_impl.h>
#include <xview/xv_error.h>

/*
 * Declaration of Functions Defined in This File (in order):
 */

Pkg_private Xv_opaque scrollbar_set_internal();

static int      scrollbar_parse_attr();

/******************************************************************/

Pkg_private     Xv_opaque
scrollbar_set_internal(scroll_public, avlist)
    Scrollbar       scroll_public;
    Attr_avlist     avlist;
{
    Xv_scrollbar_info *sb = SCROLLBAR_PRIVATE(scroll_public);

    scrollbar_parse_attr(sb, avlist);

    return (XV_OK);
}

static int
scrollbar_parse_attr(sb, argv)
    Xv_scrollbar_info *sb;
    Attr_avlist     argv;
{
    Scrollbar_attribute attr;
    int             position_elevator = FALSE;
    long unsigned   view_start = 0;
    int             view_start_set = FALSE;

    while (attr = (Scrollbar_attribute) * argv++) {
	switch (attr) {
	  case SCROLLBAR_PIXELS_PER_UNIT:
	    if ((int) *argv > 0) {
		sb->pixels_per_unit = (int) *argv;
	    }
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_OBJECT_LENGTH:
	    if ((int) *argv >= 0 && (int) *argv != sb->object_length) {
		sb->object_length = (long unsigned) *argv;
		if (!sb->creating) {
		    position_elevator = TRUE;
		}
	    }
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_OVERSCROLL:
	    if ((int) *argv >= 0 && (int) *argv <= 100
		&& (int) *argv != sb->overscroll) {
		sb->overscroll = (long unsigned) *argv;
		if (!sb->creating) {
		    position_elevator = TRUE;
		}
	    }
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_VIEW_START:
	    view_start = (long unsigned) *argv;
	    view_start_set = TRUE;
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_VIEW_LENGTH:
	    if ((int) *argv >= 0 && (int) *argv != sb->view_length) {
		sb->view_length = (int) *argv;
	    }
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_PAGE_LENGTH:
	    if ((int) *argv >= 0) {
		sb->page_length = (int) *argv;
	    }
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_SPLITTABLE:
	    sb->can_split = (int) *argv;
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_NORMALIZE_PROC:
	    sb->normalize_proc = (int (*) ()) *argv;
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_COMPUTE_SCROLL_PROC:
	    sb->compute_scroll_proc = (void (*) ()) *argv;
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_NOTIFY_CLIENT:
	    sb->managee = (Xv_opaque) * argv;
	    argv = attr_skip(attr, argv);
	    break;
	  case SCROLLBAR_DIRECTION:
	    sb->direction = (Scrollbar_setting) * argv;
	    if (!sb->creating) {
		if (sb->direction == SCROLLBAR_VERTICAL) {
		    sb->top_anchor_glyph = OLG_VSB_ANCHOR;
		    sb->bottom_anchor_glyph = OLG_VSB_ANCHOR;
		    sb->elevator_glyph = OLG_VSB_ELEVATOR;
		} else {
		    sb->top_anchor_glyph = OLG_HSB_ANCHOR;
		    sb->bottom_anchor_glyph = OLG_HSB_ANCHOR;
		    sb->elevator_glyph = OLG_HSB_ELEVATOR;
		}
		scrollbar_init_positions(sb);
	    }
	    argv = attr_skip(attr, argv);
	    break;
	  case XV_END_CREATE:
	    sb->creating = FALSE;
	    if (sb->direction == SCROLLBAR_VERTICAL) {
		sb->top_anchor_glyph = OLG_VSB_ANCHOR;
		sb->bottom_anchor_glyph = OLG_VSB_ANCHOR;
		sb->elevator_glyph = OLG_VSB_ELEVATOR;
	    } else {
		sb->top_anchor_glyph = OLG_HSB_ANCHOR;
		sb->bottom_anchor_glyph = OLG_HSB_ANCHOR;
		sb->elevator_glyph = OLG_HSB_ELEVATOR;
	    }
	    scrollbar_init_positions(sb);
	    argv = attr_skip(attr, argv);
	    break;

	  case WIN_CMS_CHANGE:{
		Scrollbar       sb_public = SCROLLBAR_PUBLIC(sb);

		scrollbar_clear_damage(sb_public, win_get_damage(sb_public));
		scrollbar_paint(sb_public);
		break;
	    }

	  default:
	    /* both vertical and horizontal share the same attrs */
	    xv_check_bad_attr(SCROLLBAR, attr);
	    argv = attr_skip(attr, argv);
	    break;
	}
    }

    /* Process a change in view_start */
    if (view_start_set && view_start <= Max_offset(sb)) {
	if (!sb->creating) {
	    /* normalize first */
	    if (sb->normalize_proc != NULL)
		sb->normalize_proc(SCROLLBAR_PUBLIC(sb),
			       view_start, SCROLLBAR_ABSOLUTE, &view_start);
	    if (scrollbar_scroll_to_offset(sb, (long unsigned) view_start) !=
		SCROLLBAR_POSITION_UNCHANGED)
		position_elevator = TRUE;
	} else
	    sb->view_start = view_start;
    }
    /*
     * Calculate new elevator position, but don't paint unless already
     * painted.
     */
    if (position_elevator)
	scrollbar_position_elevator(sb, sb->painted, TRUE);

    if (!sb->managee && sb->can_split) {
	sb->can_split = FALSE;
	xv_error(0, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
		 XV_ERROR_STRING,
		 "Cannot split a scrollbar created with scrollbar_create()",
		 XV_ERROR_PKG_NAME, "Scrollbar",
		 0);
    }
}
