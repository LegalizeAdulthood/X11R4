#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)om_set.c 20.44 89/12/01";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/* --------------------------------------------------------------------- */

#include <sys/types.h>
#include <stdio.h>
#include <pixrect/pixrect.h>
#include <pixrect/pixfont.h>
#include <pixrect/pr_util.h>
#include <pixrect/memvar.h>

#include <xview_private/om_impl.h>
#include <xview/font.h>
#include <xview_private/draw_impl.h>

/* -------------------------------------------------------------------- */

/*
 * Public
 */
/* None */

/*
 * XView Private
 */
Xv_private void screen_set_cached_window_busy();
 
/*
 * Package private
 */

Pkg_private Xv_opaque menu_sets();
Pkg_private Xv_opaque menu_item_sets();
Pkg_private void menu_cache_release_menu();
Pkg_private void menu_destroys();
Pkg_private void menu_item_destroys();
Pkg_private void menu_update_pin_image();
Pkg_private void menu_create_pin_window();
Pkg_private Xv_opaque menu_return_no_value();

/*
 * Private
 */
static int      extend_item_list();
static int      remove_item();
static int      replace_item();
static int      insert_item();
static int      lookup();
static void	menu_create_title();

/*
 * Private defs
 */
/* None */

/* ------------------------------------------------------------------------- */


Pkg_private     Xv_opaque
menu_sets(menu_public, attrs)
    Menu            menu_public;
    register Menu_attribute *attrs;
{
    register Xv_menu_info *m = MENU_PRIVATE(menu_public);
    Xv_opaque       pin_frame = 0;
    char           *pin_frame_label = 0;
    Xv_menu_info   *parent_menu;
    Xv_menu_item_info *mi;
    int             repaint = FALSE;

    for (; *attrs; attrs = menu_attr_next(attrs))
	switch (attrs[0]) {

	  case MENU_ACTION_IMAGE:
	  case MENU_ACTION_ITEM:
	  case MENU_GEN_PULLRIGHT_IMAGE:
	  case MENU_GEN_PULLRIGHT_ITEM:
	  case MENU_GEN_PROC_IMAGE:
	  case MENU_GEN_PROC_ITEM:
	  case MENU_IMAGE_ITEM:
	  case MENU_PULLRIGHT_IMAGE:
	  case MENU_PULLRIGHT_ITEM:
	  case MENU_STRING_ITEM:
	    if (m->nitems < m->max_nitems || extend_item_list(m)) {
		m->item_list[m->nitems++] = MENU_ITEM_PRIVATE(
						   xv_create(NULL, MENUITEM,
			    MENU_RELEASE, attrs[0], attrs[1], attrs[2], 0));
	    }
	    repaint = TRUE;
	    break;

	  case MENU_APPEND_ITEM:
	    if (m->nitems < m->max_nitems || extend_item_list(m))
		m->item_list[m->nitems++] = MENU_ITEM_PRIVATE(attrs[1]);
	    repaint = TRUE;
	    break;

	  case MENU_CENTER:
	    (void) image_set(&m->default_image, IMAGE_CENTER, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_CLIENT_DATA:
	    m->client_data = (Xv_opaque) attrs[1];
	    break;

	  case MENU_COL_MAJOR:
	    m->column_major = (int) attrs[1];
	    repaint = TRUE;
	    break;

	  case MENU_DEFAULT:
	    m->default_position = (int) attrs[1];
	    menu_update_pin_image(m);
	    repaint = TRUE;
	    break;

	  case MENU_DEFAULT_ITEM:
	    m->default_position = lookup(m->item_list, m->nitems,
					 MENU_ITEM_PRIVATE(attrs[1]));
	    menu_update_pin_image(m);
	    repaint = TRUE;
	    break;

	  case MENU_DEFAULT_IMAGE:
	    if (attrs[1]) {
		m->default_image = *(struct image *) attrs[1];
	    }
	    repaint = TRUE;
	    break;

	  case MENU_DEFAULT_SELECTION:
	    /* SunView1 compatibility only */
	    break;

	  case MENU_DONE_PROC:
	    m->done_proc = (void (*) ()) attrs[1];
	    break;

	  case XV_FONT:
	    *attrs = (Menu_attribute) ATTR_NOP(*attrs);
	    (void) image_set(&m->default_image, XV_FONT, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_GEN_PIN_WINDOW:
	    pin_frame = (Xv_opaque) attrs[1];
	    pin_frame_label = (char *) attrs[2];
	    break;

	  case MENU_GEN_PROC:
	    m->gen_proc = (Menu(*) ()) attrs[1];
	    break;

	  case MENU_IMAGES:
	    {
		char          **a = (char **) &attrs[1];
		while (*a) {
		    if (m->nitems < m->max_nitems || extend_item_list(m)) {
			m->item_list[m->nitems] = MENU_ITEM_PRIVATE(
						   xv_create(NULL, MENUITEM,
							     MENU_RELEASE,
						      MENU_IMAGE_ITEM, *a++,
							 m->nitems + 1, 0));
		    }
		    m->nitems++;
		}
	    }
	    repaint = TRUE;
	    break;

	  case MENU_INITIAL_SELECTION:
	  case MENU_INITIAL_SELECTION_EXPANDED:
	  case MENU_INITIAL_SELECTION_SELECTED:
	  case MENU_JUMP_AFTER_NO_SELECTION:
	  case MENU_JUMP_AFTER_SELECTION:
	  case MENU_PULLRIGHT_DELTA:
	    /* SunView1 compatibility only */
	    break;

	  case MENU_INSERT:
	    if (++m->nitems < m->max_nitems || extend_item_list(m)) {
		if (!insert_item(m->item_list, m->nitems,
			       (int) attrs[1], MENU_ITEM_PRIVATE(attrs[2])))
		    --m->nitems;
	    }
	    repaint = TRUE;
	    break;

	  case MENU_INSERT_ITEM:
	    if (++m->nitems < m->max_nitems || extend_item_list(m)) {
		if (!insert_item(m->item_list, m->nitems,
				 lookup(m->item_list, m->nitems,
					MENU_ITEM_PRIVATE(attrs[1])),
				 MENU_ITEM_PRIVATE(attrs[2])));
		--m->nitems;
	    }
	    repaint = TRUE;
	    break;

	  case MENU_ITEM:
	    if (m->nitems < m->max_nitems || extend_item_list(m)) {
		m->item_list[m->nitems] = MENU_ITEM_PRIVATE(xv_create_avlist(
						NULL, MENUITEM, &attrs[1]));
	    }
	    (void) xv_set(MENU_ITEM_PUBLIC(m->item_list[m->nitems++]),
			  MENU_RELEASE, 0);
	    repaint = TRUE;
	    break;

	  case XV_LEFT_MARGIN:
	    *attrs = (Menu_attribute) ATTR_NOP(*attrs);
	    (void) image_set(&m->default_image, XV_LEFT_MARGIN, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case XV_MARGIN:
	    *attrs = (Menu_attribute) ATTR_NOP(*attrs);
	    (void) image_set(&m->default_image, XV_MARGIN, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_NCOLS:
	    m->ncols = imax(0, (int) attrs[1]);
	    repaint = TRUE;
	    break;

	  case MENU_NROWS:
	    m->nrows = imax(0, (int) attrs[1]);
	    repaint = TRUE;
	    break;

	  case MENU_NOTIFY_PROC:
	    m->notify_proc = (Xv_opaque(*) ()) attrs[1];
	    if (!m->notify_proc)
		m->notify_proc = MENU_DEFAULT_NOTIFY_PROC;
	    break;

	  case MENU_NOTIFY_STATUS:
	    /* Set the notify status on the top level menu */
	    parent_menu = m;
	    while (parent_menu && (mi = parent_menu->parent))
		parent_menu = mi->parent;	/* get current enclosing menu */
	    parent_menu->notify_status = (int) attrs[1];
	    break;

	  case MENU_PIN:
	    if (m->pin = (int) attrs[1]) {
		/* Add pushpin-out image to menu title */
		if (!m->item_list[0] || !m->item_list[0]->title)
		    menu_create_title(m, 0, 0);
		m->item_list[0]->image.left_pr.pr_size.x = MENU_PIN_WIDTH;
		m->item_list[0]->image.left_pr.pr_size.y = MENU_PIN_HEIGHT;
		m->item_list[0]->image.left_pr.pr_depth = 0;	/* left_pr is a glyph */
		m->item_list[0]->image.left_pr.pr_data = (m->default_position == 1) ?
		    (caddr_t) OLG_MENU_DEFAULT_PIN_OUT : (caddr_t) OLG_MENU_PIN_OUT;
	    } else
		/* Remove pushpin image from menu title (if any) */
	    if (m->item_list[0] && m->item_list[0]->title)
		m->item_list[0]->image.left_pr.pr_data = 0;
	    repaint = TRUE;
	    break;

	  case MENU_PIN_PROC:
	    m->pin_proc = (void (*) ()) attrs[1];
	    break;

	  case MENU_PIN_WINDOW:
	    m->pin_window = (Xv_opaque) attrs[1];
	    break;

	  case MENU_REMOVE:
	    if (remove_item(m->item_list, m->nitems, (int) attrs[1]))
		--m->nitems;
	    repaint = TRUE;
	    break;

	  case MENU_REMOVE_ITEM:

	    if (remove_item(m->item_list, m->nitems,
			    lookup(m->item_list, m->nitems,
				   MENU_ITEM_PRIVATE(attrs[1]))))
		--m->nitems;
	    repaint = TRUE;
	    break;

	  case MENU_REPLACE:
	    (void) replace_item(m->item_list, m->nitems, (int) attrs[1],
				MENU_ITEM_PRIVATE(attrs[2]));
	    repaint = TRUE;
	    break;

	  case MENU_REPLACE_ITEM:
	    (void) replace_item(m->item_list, m->nitems,
				lookup(m->item_list, m->nitems,
				       MENU_ITEM_PRIVATE(attrs[1])),
				MENU_ITEM_PRIVATE(attrs[2]));
	    repaint = TRUE;
	    break;

	  case XV_RIGHT_MARGIN:
	    *attrs = (Menu_attribute) ATTR_NOP(*attrs);
	    (void) image_set(&m->default_image, XV_RIGHT_MARGIN, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_SELECTED:
	    m->selected_position = (int) attrs[1];
	    repaint = TRUE;
	    break;

	  case MENU_SELECTED_ITEM:
	    m->selected_position = lookup(m->item_list, m->nitems,
					  MENU_ITEM_PRIVATE(attrs[1]));
	    repaint = TRUE;
	    break;

	  case MENU_SHADOW:
	    /* SunView1 compatibility only; no effect */
	    break;

	  case MENU_STRINGS:
	    {
		char          **a = (char **) &attrs[1];
		while (*a) {
		    if (m->nitems < m->max_nitems || extend_item_list(m)) {
			m->item_list[m->nitems] = MENU_ITEM_PRIVATE(
						   xv_create(NULL, MENUITEM,
							     MENU_RELEASE,
							   MENU_STRING_ITEM,
						   *a++, m->nitems + 1, 0));
		    }
		    m->nitems++;
		}
	    }
	    repaint = TRUE;
	    break;

	  case MENU_TITLE_ITEM:/* Valid SunXView attribute */
	  case MENU_TITLE_IMAGE:	/* SunView1 compatibility */
	    if (!m->item_list[0] || !m->item_list[0]->title)
		menu_create_title(m,
				  MENU_TITLE_ITEM == attrs[0] ? MENU_STRING : MENU_IMAGE, attrs[1]);
	    else
		xv_set(MENU_ITEM_PUBLIC(m->item_list[0]),
		       MENU_TITLE_ITEM == attrs[0] ? MENU_STRING : MENU_IMAGE, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_VALID_RESULT:
	    m->valid_result = (int) attrs[1];
	    break;

	  case MENU_LINE_AFTER_ITEM:
	    switch ((int) attrs[1]) {
	      case MENU_HORIZONTAL_LINE:
		m->h_line = 1;
		break;
	      case MENU_VERTICAL_LINE:
		m->v_line = 1;
		break;
	      default:{
		    char            dummy[128];

		    (void) sprintf(dummy,
				   "Invalid argument for attribute MENU_LINE_AFTER_ITEM: %d",
				   (int) attrs[1]);
		    xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			     XV_ERROR_STRING, dummy,
			     XV_ERROR_PKG_NAME, "Menu",
			     0);
		}
	    }
	    repaint = TRUE;
	    break;

	  default:
	    (void) xv_check_bad_attr(MENU, attrs);
	    break;

	}

    if (pin_frame)
	menu_create_pin_window(menu_public, pin_frame, pin_frame_label);

    if (repaint)
	m->rendered = FALSE;

    return (Xv_opaque) XV_OK;
}


Pkg_private     Xv_opaque
menu_item_sets(menu_item_public, attrs)
    Menu_item       menu_item_public;
    register Menu_attribute *attrs;
{
    register Xv_menu_item_info *mi = MENU_ITEM_PRIVATE(menu_item_public);
    int             repaint = FALSE;

    for (; *attrs; attrs = menu_attr_next(attrs))
	switch (attrs[0]) {

	  case MENU_ACTION:	/* == MENU_ACTION_PROC == MENU_NOTIFY_PROC */
	    mi->notify_proc = (Xv_opaque(*) ()) attrs[1];
	    break;

	  case MENU_ACTION_ITEM:
	    (void) image_set(&mi->image, XV_LABEL, attrs[1], 0);
	    mi->notify_proc = (Xv_opaque(*) ()) attrs[2];
	    repaint = TRUE;
	    break;

	  case MENU_STRING_ITEM:
	    (void) image_set(&mi->image, XV_LABEL, attrs[1], 0);
	    mi->value = (Xv_opaque) attrs[2];
	    repaint = TRUE;
	    break;

	  case MENU_ACTION_IMAGE:
	    image_set(&mi->image, IMAGE_PIXRECT, attrs[1], 0);
	    mi->notify_proc = (Xv_opaque(*) ()) attrs[2];
	    repaint = TRUE;
	    break;

	  case MENU_IMAGE_ITEM:
	    (void) image_set(&mi->image, IMAGE_PIXRECT, attrs[1], 0);
	    mi->value = (Xv_opaque) attrs[2];
	    repaint = TRUE;
	    break;

	  case MENU_CENTER:
	    (void) image_set(&mi->image, IMAGE_CENTER, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_CLIENT_DATA:
	    mi->client_data = (Xv_opaque) attrs[1];
	    break;

	  case MENU_FEEDBACK:
	    mi->no_feedback = !(int) attrs[1];
	    break;

	  case XV_FONT:
	    *attrs = (Menu_attribute) ATTR_NOP(*attrs);
	    (void) image_set(&mi->image, XV_FONT, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_GEN_PROC:
	    mi->gen_proc = (Menu_item(*) ()) attrs[1];
	    break;

	  case MENU_GEN_PROC_IMAGE:
	    (void) image_set(&mi->image, IMAGE_PIXRECT, attrs[1], 0);
	    mi->gen_proc = (Menu_item(*) ()) attrs[2];
	    repaint = TRUE;
	    break;

	  case MENU_GEN_PROC_ITEM:
	    (void) image_set(&mi->image, XV_LABEL, attrs[1], 0);
	    mi->gen_proc = (Menu_item(*) ()) attrs[2];
	    repaint = TRUE;
	    break;

	  case MENU_GEN_PULLRIGHT:
	    mi->gen_pullright = (Menu(*) ()) attrs[1];
	    mi->pullright = mi->gen_pullright != NULL;
	    repaint = TRUE;
	    break;

	  case MENU_GEN_PULLRIGHT_IMAGE:
	    (void) image_set(&mi->image, IMAGE_PIXRECT, attrs[1], 0);
	    mi->gen_pullright = (Menu(*) ()) attrs[2];
	    mi->pullright = mi->gen_pullright != NULL;
	    mi->value = 0;
	    repaint = TRUE;
	    break;

	  case MENU_GEN_PULLRIGHT_ITEM:
	    (void) image_set(&mi->image, XV_LABEL, attrs[1], 0);
	    mi->gen_pullright = (Menu(*) ()) attrs[2];
	    mi->pullright = mi->gen_pullright != NULL;
	    mi->value = 0;
	    repaint = TRUE;
	    break;

	  case MENU_IMAGE:
	    (void) image_set(&mi->image, IMAGE_PIXRECT, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_INACTIVE:
	    if (((int) attrs[1] ? TRUE : FALSE) != mi->inactive) {
		mi->inactive = (int) attrs[1];
		repaint = TRUE;
	    }
	    break;

	  case MENU_INVERT:
	    (void) image_set(&mi->image, IMAGE_INVERT, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case XV_LEFT_MARGIN:
	    *attrs = (Menu_attribute) ATTR_NOP(*attrs);
	    (void) image_set(&mi->image, XV_LEFT_MARGIN, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case XV_MARGIN:
	    *attrs = (Menu_attribute) ATTR_NOP(*attrs);
	    (void) image_set(&mi->image, XV_MARGIN, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_PULLRIGHT:
	    mi->value = (Xv_opaque) attrs[1];
	    mi->pullright = mi->value != NULL;
	    repaint = TRUE;
	    break;

	  case MENU_PULLRIGHT_IMAGE:
	    (void) image_set(&mi->image, IMAGE_PIXRECT, attrs[1], 0);
	    mi->value = (Xv_opaque) attrs[2];
	    mi->pullright = mi->value != NULL;
	    repaint = TRUE;
	    break;

	  case MENU_PULLRIGHT_ITEM:
	    (void) image_set(&mi->image, XV_LABEL, attrs[1], 0);
	    mi->value = (Xv_opaque) attrs[2];
	    mi->pullright = mi->value != NULL;
	    repaint = TRUE;
	    break;

	  case MENU_RELEASE:
	    mi->free_item = TRUE;
	    break;

	  case MENU_RELEASE_IMAGE:
	    mi->image.free_string = TRUE;
	    mi->image.free_pr = TRUE;
	    break;

	  case XV_RIGHT_MARGIN:
	    *attrs = (Menu_attribute) ATTR_NOP(*attrs);
	    (void) image_set(&mi->image, XV_RIGHT_MARGIN, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_SELECTED:
	    mi->selected = (int) attrs[1];
	    repaint = TRUE;
	    break;

	  case MENU_STRING:
	    (void) image_set(&mi->image, XV_LABEL, attrs[1], 0);
	    repaint = TRUE;
	    break;

	  case MENU_TITLE:
	    mi->title = TRUE;
	    break;

	  case MENU_VALUE:
	    mi->value = (Xv_opaque) attrs[1];
	    mi->pullright = FALSE;
	    repaint = TRUE;
	    break;

	  case MENU_LINE_AFTER_ITEM:
	    switch ((int) attrs[1]) {
	      case MENU_HORIZONTAL_LINE:
		mi->h_line = 1;
		break;
	      case MENU_VERTICAL_LINE:
		mi->v_line = 1;
		break;
	      default:{
		    char            dummy[128];

		    (void) sprintf(dummy,
				   "Invalid argument for attribute MENU_LINE_AFTER_ITEM: %d",
				   (int) attrs[1]);
		    xv_error(NULL, XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
			     XV_ERROR_STRING, dummy,
			     XV_ERROR_PKG_NAME, "Menu",
			     0);
		}
	    }
	    repaint = TRUE;
	    break;

	  default:
	    (void) xv_check_bad_attr(MENUITEM, attrs);
	    break;

	}

    if (mi->parent && repaint)
	mi->parent->rendered = FALSE;

    return (Xv_opaque) XV_OK;
}


Pkg_private void
menu_destroys(m, destroy_proc)
    register Xv_menu_info *m;
    void            (*destroy_proc) ();
{
    register Xv_menu_item_info *mi;
    Xv_Drawable_info *info;

    if (!m || m->type != (int) MENU_MENU)
	return;
    if (m->item_list) {
	for (; m->nitems-- > 0;) {
	    mi = m->item_list[m->nitems];
	    if (mi->pullright && !mi->gen_pullright && mi->value) {
		menu_destroys((Xv_menu_info *) mi->value, destroy_proc);
	    }
	    free(mi);
	}
	free(m->item_list);
    }
    if (m->window) {
        DRAWABLE_INFO_MACRO(m->window, info);
        screen_set_cached_window_busy(xv_screen(info),
                                      m->window, FALSE);
    }
    if (m->shadow_window) {
        DRAWABLE_INFO_MACRO(m->shadow_window, info);
        screen_set_cached_window_busy(xv_screen(info),
                                      m->shadow_window, FALSE);
    }
    menu_cache_release_menu(m);
    if (destroy_proc)
	destroy_proc(m, MENU_MENU);
    free(m);
}


Pkg_private void
menu_item_destroys(mi, destroy_proc)
    register Xv_menu_item_info *mi;
    void            (*destroy_proc) ();
{
    if (!mi || !mi->free_item)
	return;
    image_destroy(&mi->image);
    if (destroy_proc)
	destroy_proc(MENU_ITEM_PUBLIC(mi), MENU_ITEM);
    free((Xv_opaque) mi);
}


static int
extend_item_list(m)
    register Xv_menu_info *m;
{
    m->max_nitems = m->max_nitems + MENU_FILLER;
    m->item_list = (Xv_menu_item_info **) realloc(
						  (Xv_opaque) m->item_list,
		       (u_int) (m->max_nitems * sizeof(Xv_menu_item_info)));
    if (!m->item_list) {
	xv_error(m, XV_ERROR_RECOVERABLE, XV_ERROR_SYSTEM,
		 XV_ERROR_STRING,
		 "menu_set: Malloc failed to allocate an item list",
		 XV_ERROR_PKG_NAME, "Menu",
		 0);
	m->max_nitems = m->max_nitems - MENU_FILLER;
	return FALSE;
    }
    return TRUE;
}


static int
remove_item(il, len, pos)
    register Xv_menu_item_info *il[];
{
    register int    i;
    if (pos < 1 || pos > len)
	return FALSE;
    for (i = pos; i < len; i++)
	il[i - 1] = il[i];
    return TRUE;
}


static int
replace_item(il, len, pos, mi)
    Xv_menu_item_info *il[], *mi;
{
    if (pos < 1 || pos > len)
	return FALSE;
    il[pos - 1] = mi;
    return TRUE;
}


static int
insert_item(il, len, pos, mi)
    register Xv_menu_item_info *il[];
    Xv_menu_item_info *mi;
{
    register int    i;
    if (pos < 0 || pos >= len)
	return FALSE;
    for (i = len - 1; i > pos; --i)
	il[i] = il[i - 1];
    il[i] = mi;
    return TRUE;
}


static int
lookup(il, len, mi)
    register Xv_menu_item_info *il[];
    Xv_menu_item_info *mi;
{
    int             i;

    for (i = 0; i < len; i++)
	if (il[i] == mi)
	    return i + 1;
    return -1;
}


static void
menu_create_title(m, type, arg1)
    register Xv_menu_info *m;
    int             type;	/* MENU_STRING, MENU_IMAGE or 0 (= no title) */
    Xv_opaque       arg1;	/* the string or pixrect */
{
    register int    i;
    Menu_item       menu_item;

    if (m->nitems < m->max_nitems || extend_item_list(m)) {
	m->nitems++;
	for (i = m->nitems - 1; i > 0; i--)
	    m->item_list[i] = m->item_list[i - 1];
	menu_item = xv_create(NULL, MENUITEM,
			      MENU_FEEDBACK, FALSE,
			      MENU_RELEASE,
			      MENU_TITLE,
			      MENU_NOTIFY_PROC, menu_return_no_value,
			      0);
	m->item_list[0] = MENU_ITEM_PRIVATE(menu_item);
	if (type)
	    xv_set(menu_item,
		   type, arg1,
		   MENU_CENTER, TRUE,
		   MENU_LINE_AFTER_ITEM, MENU_HORIZONTAL_LINE,
		   0);
	if (m->default_position == 1)
	    m->default_position++;
    }
}


Pkg_private void
menu_update_pin_image(m)
    Xv_menu_info   *m;
{
    if (m->pin)
	m->item_list[0]->image.left_pr.pr_data = (m->default_position == 1) ?
	    (caddr_t) OLG_MENU_DEFAULT_PIN_OUT : (caddr_t) OLG_MENU_PIN_OUT;
}
