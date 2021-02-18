#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)p_list.c 1.47 89/10/24";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/panel_impl.h>
#include <xview/defaults.h>
#include <xview_private/p_lst_impl.h>
#include <xview/scrollbar.h>
#include <xview/sel_attrs.h>
#include <xview/screen.h>
#include <xview/openmenu.h>
#include <X11/X.h>
#include <ctype.h>

#define PANEL_LIST_PRIVATE(item)        \
        XV_PRIVATE(Panel_list_info, Xv_panel_list, item)
#define PANEL_LIST_PUBLIC(item)         XV_PUBLIC(item)
#define PANEL_LIST_ROW_GAP	5
#define PANEL_LIST_COL_GAP	4	/* space between glyph and string */
#define ROW_MARGIN	9 /* space between vertical borders and box */

Pkg_private int panel_list_init();
Pkg_private int panel_list_destroy();
Pkg_private Xv_opaque panel_list_set_avlist();
Pkg_private Xv_opaque panel_list_get_attr();

static char    *selection_string();
static int      sizeof_selection();
static Panel_setting change_done();
static Row_info *next_row();
static Row_info *find_or_create_nth_row();
static Row_info	*find_row();
static Row_info *panel_list_insert_row();
static Seln_result get_selection();
static Seln_result panel_list_reply_proc();
static void	accept_change();
static void	accept_insert();
static void     fit_panel_to_items();
static void	handle_menu_event();
static void	paint();
static void     panel_list_bg_proc();
static void     panel_list_create_nrows();
static void     panel_list_delete_row();
static void     panel_list_event_proc();
static void     panel_list_seln_function();
static void	set_current_item();
static void	set_edit_item();
static void     set_row_n_col();
static void	show_feedback();
static void     unset_all_selected_items();
static Xv_opaque change_proc();
static Xv_opaque clear_all_choices();
static Xv_opaque delete_proc();
static Xv_opaque enter_edit_mode();
static Xv_opaque enter_read_mode();
static Xv_opaque insert_proc();
static Xv_opaque locate_next_choice();

extern void     panel_default_handle_event();

static struct panel_ops ops = {
    (void (*) ()) panel_nullproc,	/* handle_event() */
    (void (*) ()) panel_nullproc,	/* begin_preview() */
    (void (*) ()) panel_nullproc,	/* update_preview() */
    (void (*) ()) panel_nullproc,	/* cancel_preview() */
    (void (*) ()) panel_nullproc,	/* accept_preview() */
    (void (*) ()) panel_nullproc,	/* accept_menu() */
    (void (*) ()) panel_nullproc,	/* accept_key() */
    (void (*) ()) paint,		/* paint() */
    (void (*) ()) panel_nullproc,	/* remove() */
    (void (*) ()) panel_nullproc,	/* restore() */
    (void (*) ()) panel_nullproc	/* layout() */
};


typedef enum {
    INSERT_BEFORE,
    INSERT_AFTER
} Insert_pos_t;



Pkg_private int
panel_list_init(parent, panel_list_public, avlist)
    Xv_Window       parent;
    Xv_Window       panel_list_public;
    Attr_avlist     avlist;
{
    Panel_list_info *panel_list_info;
    Xv_panel_list  *panel_list = (Xv_panel_list *) panel_list_public;
    Xv_object       server = xv_get(xv_get(parent, XV_SCREEN), SCREEN_SERVER);
    Xv_Window	    parent_pw;	/* parent panel's first canvas paint window */
    Item_info      *ip = ITEM_PRIVATE(panel_list_public);

    ip->item_type = PANEL_LIST_ITEM;
    if (ops_set(ip))
	*ip->ops = ops;		/* copy the panel list ops vector */
    else
	ip->ops = &ops;		/* use the static panel list ops vector */
    panel_set_bold_label_font(ip);
    panel_list_info = xv_alloc(Panel_list_info);

    /* link to object */
    panel_list->private_data = (Xv_opaque) panel_list_info;
    panel_list_info->public_self = panel_list_public;

    /* Initialize the object */
    panel_list_info->nlevels = 1;
    panel_list_info->parent_panel = parent;
    panel_list_info->row_height = 0; /* initialize, 0 => font height */
    panel_list_info->choose_one = TRUE; /* exclusive scrolling list */
    panel_list_info->edit_op = OP_NONE;
    panel_list_info->string_item_x = ROW_MARGIN;
    parent_pw = xv_get(parent, CANVAS_NTH_PAINT_WINDOW, 0);
    panel_list_info->row_panel =
	xv_create(parent, SCROLLABLE_PANEL,
		  PANEL_BACKGROUND_PROC, panel_list_bg_proc,
		  WIN_CMS, xv_get(parent_pw, WIN_CMS),
		  WIN_FRONT,
		  XV_X, ip->panel->item_x,
		  XV_Y, ip->panel->item_y,
		  WIN_ROW_GAP, PANEL_LIST_ROW_GAP,	/* used by xv_row() */
		  XV_SHOW, FALSE,
		  XV_KEY_DATA, PANEL_LIST, panel_list_info,
		  0);
    xv_set(xv_get(panel_list_info->row_panel, CANVAS_NTH_PAINT_WINDOW, 0),
	   WIN_RETAINED, FALSE,
	   0);

    panel_list_info->seln_client = selection_create(server,
			    panel_list_seln_function, panel_list_reply_proc,
						  (char *) panel_list_info);
    (void) panel_append(ip);
    return XV_OK;
}


/*
 * BUG: complete panel_list_destroy routine
 */
Pkg_private int
panel_list_destroy(panel_list_public, status)
    Panel_item      panel_list_public;
    Destroy_status  status;
{
    Panel_list_info *panel_list_info = PANEL_LIST_PRIVATE(panel_list_public);
    Row_info       *row = panel_list_info->rows;
    Row_info       *next;

    if ((status == DESTROY_CHECKING) || (status == DESTROY_SAVE_YOURSELF)) {
	return XV_OK;
    }
    while (row) {
	if (row->glyph_item) {
	    xv_destroy(row->glyph_item);
	    row->glyph_item = NULL;
	}
	if (row->string_item) {
	    xv_destroy(row->string_item);
	    row->string_item = NULL;
	}
	if (row->font)
	    (void) xv_set(row->font, XV_DECREMENT_REF_COUNT, 0);
	next = row->next;
	free(row);
	row = next;
    }
    if (panel_list_info->selection[0])
	free(panel_list_info->selection[0]);
    if (panel_list_info->selection[1])
	free(panel_list_info->selection[1]);
    xv_destroy(panel_list_info->row_panel);

    return XV_OK;
}


/*
 * Create container panel, choice_stack, text_item and a panel for containing
 * the list of items. Change their owner to the panel_list
 */
static void
panel_list_create_displayarea(panel_list)
    Panel_list_info *panel_list;
{
    int             number_rows;
    
    if (panel_list->rows_displayed == 0) {
	if (panel_list->nrows < PANEL_LIST_DEFAULT_ROW)
	    panel_list->rows_displayed = panel_list->nrows;
	else
	    panel_list->rows_displayed = PANEL_LIST_DEFAULT_ROW;
    }
    number_rows = panel_list->rows_displayed;

    /*
     * By this time all the row items have been created. Reparent them and
     * set their rows and columns. Calculate the size of the panel (depends
     * on the number of rows to be displayed and the size of the fonts).
     */
    xv_set(panel_list->row_panel,
	   XV_HEIGHT, PANEL_ITEM_Y_START +
		      xv_rows(panel_list->row_panel, number_rows) +
		      PANEL_LIST_ROW_GAP,
	   XV_SHOW, TRUE,
	   0);
    panel_list->row_panel_sb = (Scrollbar)xv_create(panel_list->row_panel,
                                                    SCROLLBAR,
                                                    0);
}


/*
 * fit the scrolling list
 */
static void
fit_panel_to_items(panel_list)
    Panel_list_info *panel_list;
{
    register Item_info *ip;
    Row_info       *row;
    int             view_width = 0;	/* width of view window of row panel */
    int             delta;	/* difference between view width and panel
				 * width */

    /* BUG ALERT: Why is this font stuff here? */
    for (row=panel_list->rows; row; row=row->next) {
	if (row->font)
	    xv_set(row->string_item,
		   PANEL_LABEL_FONT, row->font,
		   0);
    }

    if (!panel_list->width) {
	for (ip = PANEL_PRIVATE(panel_list->row_panel)->items;
	     ip;
	     ip = ip->next)
	    view_width = MAX(view_width, ip->rect.r_left + ip->rect.r_width);
    } else
	view_width = panel_list->width;
    delta = xv_get(panel_list->row_panel, XV_WIDTH) -
	xv_get(xv_get(panel_list->row_panel, OPENWIN_NTH_VIEW, 0), XV_WIDTH);
    /*
     * New width = view width + delta + 2 1-pixel borders + 1 ROW_MARGIN
     * (one row margin and the max glyph width is already accounted for
     * in ip->rect.r_left)
     *
     * From left to right in the row panel, we have:
     *    1-pixel border, row margin, string, row margin and 1-pixel border.
     */ 
    xv_set(panel_list->row_panel,
	   XV_WIDTH, view_width + delta + 2 + ROW_MARGIN,
	   XV_SHOW, TRUE,
	   0); 
}


static void
set_row_n_col(panel_list, row)
    Panel_list_info *panel_list;
    Row_info       *row;
{
    if (row->glyph_item) {
	xv_set(row->glyph_item,
	       PANEL_ITEM_X, 1,
	       PANEL_ITEM_Y, xv_row(panel_list->row_panel, row->row),
	       0);
    }
    if (row->string_item) {
	xv_set(row->string_item,
	       /* PANEL_ITEM_X set at end of _set call for alignment */
	       PANEL_ITEM_Y, xv_row(panel_list->row_panel, row->row),
	       0);
    }
}


Pkg_private     Xv_opaque
panel_list_set_avlist(panel_list_public, avlist)
    Panel_item      panel_list_public;
    Attr_avlist     avlist;
{
    register Panel_list_info *panel_list =
			PANEL_LIST_PRIVATE(panel_list_public);
    Pixrect        *glyph_pr;
    Item_info      *ip = ITEM_PRIVATE(panel_list_public);
    int             max_glyph_width;
    int		    nrows_attr_seen = FALSE;
    Attr_avlist     orig_avlist = avlist;
    int             reset_rows = FALSE;
    Xv_opaque	    result;
    Row_info       *row;
    int		    row_height_attr_seen = FALSE;
    Panel_info	   *row_panel_private;
    int		    row_panel_view_width;
    Xv_opaque	    row_panel_view_window;
    int             xv_end_create = FALSE;

    /* Call generic item set code to handle layout attributes */
    if ((Xv_generic_attr) * avlist != XV_END_CREATE) {
	result = xv_super_set_avlist(panel_list_public, &xv_panel_list_pkg,
				     avlist);
    	if (result != XV_OK)
	    return result;
    }

    for ( ; *avlist; avlist = attr_next(avlist)) {
	switch ((int) avlist[0]) {
	  case PANEL_LIST_ROW_HEIGHT:
	        if (!panel_list->initialized)
		    panel_list->row_height = (unsigned short) avlist[1];
	        row_height_attr_seen = TRUE;
	        break;
	  case PANEL_LIST_NROWS:
	        panel_list->nrows = (int) avlist[1];
	        panel_list_create_nrows(panel_list);
	        nrows_attr_seen = TRUE;
	        break;
	  default:
	        break;
	}
    }

    if (!panel_list->initialized || row_height_attr_seen) {
	xv_set(panel_list->row_panel,
	       WIN_ROW_HEIGHT, panel_list->row_height,	/* 0 => font height */
	       0);
	/* Get actual row height */
	panel_list->row_height = (int) xv_get(panel_list->row_panel,
					      WIN_ROW_HEIGHT);
    }
    /*
     * Process the attributes that rely on the already processed attributes.
     */
    avlist = orig_avlist;
    for (avlist = orig_avlist; *avlist; avlist = attr_next(avlist)) {
	switch ((int) avlist[0]) {
	  case PANEL_CHOOSE_NONE:
		panel_list->choose_none = avlist[1] ? TRUE : FALSE;
		break;
	  case PANEL_CHOOSE_ONE:
		panel_list->choose_one = avlist[1] ? TRUE : FALSE;
		break;
	  case PANEL_EVENT_PROC:
		panel_list->event_proc = (void (*)()) avlist[1];
		break;
	  case PANEL_LIST_HEIGHT:
	        panel_list->height = (int) avlist[1];
	        break;
	  case PANEL_LIST_WIDTH:
	        panel_list->width = (int) avlist[1];
	        break;
	  case PANEL_LIST_DISPLAY_ROWS:
	        panel_list->rows_displayed = (int) avlist[1];
	        reset_rows = TRUE;
	        break;

	  case PANEL_ITEM_COLOR:
	    for (row = panel_list->rows; row; row = row->next)
		if (row->string_item)
		    xv_set(row->string_item,
			   PANEL_ITEM_COLOR, (int) avlist[1],
			   0);
	    break;

	  case PANEL_ITEM_MENU:
		if (panel_list->edit_mode)
		    panel_list->edit_menu = (Menu) avlist[1];
		else
		    panel_list->read_menu = (Menu) avlist[1];
	        ATTR_CONSUME(*((Attr_avlist) & avlist[0]));
	        break;

	  case PANEL_LIST_SELECT:{
		int             which_row = (int) avlist[1];
		int             selected = (int) avlist[2];

		row = find_or_create_nth_row(panel_list, which_row, FALSE);
		if (row->selected != selected)
		    set_current_item(panel_list, row->string_item, NULL);

		break;
	    }

	  case PANEL_LIST_NROWS: {
	        int number_of_rows = (int) avlist[1];
	    
	        if (!nrows_attr_seen) {
	            panel_list->nrows = number_of_rows;
	            panel_list_create_nrows(panel_list);
	        }
	        break;
	        }

	  case PANEL_LIST_STRINGS:{
		int             n = 0;
		row = panel_list->rows;

		while (avlist[n+1]) {
		    row = next_row(panel_list, row, n++);
		    if (row->string_item) {
			xv_set(row->string_item,
			       PANEL_LABEL_STRING, avlist[n],
			       0);
		    } else {
			row->string_item = 
			    xv_create(panel_list->row_panel, PANEL_MESSAGE,
				      PANEL_LABEL_STRING, avlist[n],
				      PANEL_CLIENT_DATA, panel_list,
				      PANEL_EVENT_PROC, panel_list_event_proc,
				      PANEL_ITEM_X, panel_list->string_item_x,
				      PANEL_ITEM_Y,
					  xv_row(panel_list->row_panel, 
				              row->row),
				      PANEL_ITEM_COLOR, ip->color_index,
				      0);
		    }
		}
		row = row->next;
		while (row) {
		    if (!row->string_item) {
			row->string_item = 
			    xv_create(panel_list->row_panel, PANEL_MESSAGE,
				      PANEL_LABEL_STRING, "",
				      PANEL_CLIENT_DATA, panel_list,
				      PANEL_EVENT_PROC,
				      panel_list_event_proc,
				      PANEL_ITEM_X, panel_list->string_item_x,
				      PANEL_ITEM_Y,
					  xv_row(panel_list->row_panel,
				              row->row),
				      PANEL_ITEM_COLOR, ip->color_index,
				      0);
		    }
		    row = row->next;
		}
		break;
	    }

	  case PANEL_LIST_STRING:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(panel_list, which_row,
					     TRUE);
		if (row->string_item) {
		    xv_set(row->string_item,
			   PANEL_LABEL_STRING, avlist[2],
			   0);
		} else {
		    row->string_item = 
		        xv_create(panel_list->row_panel,
				  PANEL_MESSAGE,
				  PANEL_LABEL_STRING, avlist[2],
				  PANEL_CLIENT_DATA, panel_list,
				  PANEL_EVENT_PROC,
				  panel_list_event_proc,
				  PANEL_ITEM_X, panel_list->string_item_x,
				  PANEL_ITEM_Y,
				      xv_row(panel_list->row_panel,
				             row->row),
				  PANEL_ITEM_COLOR, ip->color_index,
				  0);
		}
		break;
	    }

	  case PANEL_LIST_GLYPHS:{
		int             n = 0;
		Pixrect        *glyph_pr;
		
		row = panel_list->rows;
		while (avlist[n+1]) {
		    row = next_row(panel_list, row, n++);
		    glyph_pr = (Pixrect *) avlist[n];
		    if (glyph_pr->pr_height <= panel_list->row_height) {
			row->glyph_item =
			    xv_create(panel_list->row_panel,
				      PANEL_MESSAGE,
				      PANEL_LABEL_IMAGE, glyph_pr,
				      PANEL_ITEM_X, 1,
				      PANEL_ITEM_Y, xv_row(panel_list->row_panel,
				          row->row),
				      PANEL_ITEM_COLOR, ip->color_index,
				      PANEL_CLIENT_DATA, panel_list,
				      0);
		    } else {
			xv_error(glyph_pr,
				 XV_ERROR_RECOVERABLE, XV_ERROR_INTERNAL,
				 XV_ERROR_STRING,
				 "Panel List glyph height exceeds row height; glyph ignored",
				 XV_ERROR_PKG_NAME, "Panel",
				 0);
		    }
		}
		break;
	    }

	  case PANEL_LIST_GLYPH:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(panel_list,
					     which_row, TRUE);
		if (row->glyph_item) {
		    xv_destroy(row->glyph_item);
		    row->glyph_item = NULL;
		}
		row->glyph_item =
		    xv_create(panel_list->row_panel,
			      PANEL_MESSAGE,
			      PANEL_LABEL_IMAGE, avlist[2],
			      PANEL_ITEM_X, 1,
			      PANEL_ITEM_Y, xv_row(panel_list->row_panel,
			          row->row),
			      PANEL_ITEM_COLOR, ip->color_index,
			      PANEL_CLIENT_DATA, panel_list,
			      0);
		break;
	    }


	  case PANEL_LIST_CLIENT_DATAS:{
	  	int n=1;
		row = panel_list->rows;
		while (avlist[n]) {
		    if (row) {
			row->client_data = (Xv_opaque) avlist[n];
		    }
		    row = row->next;
		    n++;
		}
		break;
	    }
	    

	  case PANEL_LIST_CLIENT_DATA:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(panel_list,
					     which_row, FALSE);
		if (row)
		    row->client_data = (Xv_opaque) avlist[2];
		break;
	    }

	  case PANEL_LIST_FONTS:{
	  	int n=1;
		row = panel_list->rows;
		while (avlist[n]) {
		    if (row) {
			row->font = (Xv_Font) avlist[n];
		    }
		    row = row->next;
		    n++;
		}
		break;
	    }

	  case PANEL_LIST_FONT:{
		int             which_row = (int) avlist[1];
		row = find_or_create_nth_row(panel_list,
					     which_row, FALSE);
		if (row)
		    row->font = (Xv_opaque) avlist[2];
		break;
	    }

	  case PANEL_LIST_INSERT:{
		int             which_row = (int) avlist[1];
		panel_list_insert_row(panel_list, which_row, FALSE);
		break;
	    }

	  case PANEL_LIST_DELETE:{
		int             which_row = (int) avlist[1];
		Row_info       *node = panel_list->rows;

		while (node && (node->row != which_row))
		    node = node->next;
		if (node)
		    panel_list_delete_row(panel_list, node);
		break;
	    }

	  case PANEL_READ_ONLY:
	    panel_list->read_only = avlist[1] ? TRUE : FALSE;
	    break;

	  case XV_END_CREATE:
	    panel_list_create_displayarea(panel_list);
	    panel_list->initialized = TRUE;
	    xv_end_create = TRUE;
	    break;

	  default:
	    break;
	}
    }

    /* Non-exclusive lists can have no current item */
    if (!panel_list->choose_one)
	panel_list->choose_none = TRUE;

    if (!panel_list->choose_none && panel_list->rows) {
        /* If no row is selected, then select the first row */
        for (row = panel_list->rows; row; row = row->next)
            if (row->selected)
            	break;
        if (!row) {
            panel_list->rows->selected = TRUE;
    	    panel_list->current_item = panel_list->rows->string_item;
            show_feedback(panel_list, panel_list->current_item, TRUE, NULL);
        }
    }

    if (reset_rows && panel_list->initialized) {
	xv_set(panel_list->row_panel,
	       XV_HEIGHT, PANEL_ITEM_Y_START +
			  xv_rows(panel_list->row_panel,
	       			  panel_list->rows_displayed) +
			  PANEL_LIST_ROW_GAP,
	       0);
    }

    /* Horizontally align string items in row panel */
    max_glyph_width = 0;
    for (row = panel_list->rows; row; row = row->next)
	if (row->glyph_item) {
	    glyph_pr = (Pixrect *) xv_get(row->glyph_item,
	    				  PANEL_LABEL_IMAGE);
	    if (glyph_pr->pr_width > max_glyph_width)
		max_glyph_width = glyph_pr->pr_width;
	}
    max_glyph_width += max_glyph_width ? PANEL_LIST_COL_GAP : 0;
    if (max_glyph_width) {
	panel_list->string_item_x = ROW_MARGIN + max_glyph_width;
	for (row = panel_list->rows; row; row = row->next)
	    if (row->string_item)
		xv_set(row->string_item,
		       PANEL_ITEM_X, panel_list->string_item_x,
		       0);
    }

    if (panel_list->initialized) {
	fit_panel_to_items(panel_list);
	/*
	 * Note: item_set_generic sets ip->rect to the enclosing rect of
	 * ip->label_rect and ip->value_rect. However, on XV_END_CREATE,
	 * item_set_generic gets called before panel_list_set_avlist.  So, if
	 * we're processing the XV_END_CREATE, we must also set ip->rect.
	 */
	ip->value_rect.r_width = (short)
	    xv_get(panel_list->row_panel, XV_WIDTH);
	ip->value_rect.r_height = (short)
	    xv_get(panel_list->row_panel, XV_HEIGHT);
	switch (ip->layout) {
	  case PANEL_HORIZONTAL:
	    ip->value_rect.r_left = rect_right(&ip->label_rect) + 1 +
		(ip->label_rect.r_width ? LABEL_X_GAP : 0);
	    ip->value_rect.r_top = ip->label_rect.r_top;
	    break;
	  case PANEL_VERTICAL:
	    ip->value_rect.r_left = ip->label_rect.r_left;
	    ip->value_rect.r_top = rect_bottom(&ip->label_rect) + 1 +
		(ip->label_rect.r_height ? LABEL_Y_GAP : 0);
	    break;
	}
	xv_set(panel_list->row_panel,
	    XV_X, ip->value_rect.r_left,
	    XV_Y, ip->value_rect.r_top,
	    0);
	/*
	 * adjust the rects for the string items to extend the width
	 * of the list box's view window, less the borders and row margins.
	 */
	row_panel_view_window = xv_get(panel_list->row_panel,
				       OPENWIN_NTH_VIEW, 0);
	row_panel_view_width = (int) xv_get(row_panel_view_window, XV_WIDTH);
	for (row = panel_list->rows; row; row = row->next) {
	    if (row->string_item) {
	        Item_info *txt_ip = ITEM_PRIVATE(row->string_item);
	        
	        txt_ip->label_rect.r_width = (short)(row_panel_view_width -
	            max_glyph_width - 2 - 2*ROW_MARGIN);
	        txt_ip->label_rect.r_height = (short)xv_rows(
		    panel_list->row_panel, 1);
		/* Message items do not have value rects, so the item rect
		 * is equal to the label rect.
		 */
		txt_ip->rect.r_width = txt_ip->label_rect.r_width;
		txt_ip->rect.r_height = txt_ip->label_rect.r_height;
	    }
	}
	row_panel_private = PANEL_PRIVATE(panel_list->row_panel);
	if (panel_painted(row_panel_private))
	    panel_paint(panel_list->row_panel, PANEL_CLEAR);
	if (xv_end_create) {
	    ip->rect = panel_enclosing_rect(&ip->label_rect, &ip->value_rect);

	    /* Create Row Panel menus, if not user defined.  */
	    if (!panel_list->read_menu) {
		panel_list->read_menu = xv_create(NULL, MENU,
		    XV_KEY_DATA, PANEL_LIST, panel_list,
		    MENU_TITLE_ITEM, "Scrolling List",
		    MENU_ITEM,
			MENU_STRING, "Locate Next Choice",
			MENU_ACTION,	locate_next_choice,
			0,
		    0);
		if (!panel_list->choose_one)
		    xv_set(panel_list->read_menu,
			MENU_APPEND_ITEM,
			    xv_create(NULL, MENUITEM,
				MENU_STRING, "Clear All Choices",
				MENU_ACTION, clear_all_choices,
				0),
			0);
		if (!panel_list->read_only) {
		    xv_set(panel_list->read_menu,
			MENU_APPEND_ITEM,
			    xv_create(NULL, MENUITEM,
				MENU_STRING, "Edit List",
				MENU_ACTION, enter_edit_mode,
				0),
		        0);
		    if (!panel_list->edit_menu) {
			panel_list->edit_menu = xv_create(NULL, MENU,
			    XV_KEY_DATA, PANEL_LIST, panel_list,
			    MENU_TITLE_ITEM, "Scrolling List",
			    MENU_ITEM,
				MENU_STRING, "Change",
				MENU_ACTION, change_proc,
				0,
			    MENU_ITEM,
				MENU_STRING, "Insert",
				MENU_PULLRIGHT,
				    xv_create(NULL, MENU,
					XV_KEY_DATA, PANEL_LIST, panel_list,
					MENU_ITEM,
					    MENU_STRING, "Before",
					    MENU_ACTION, insert_proc,
					    XV_KEY_DATA, PANEL_INSERT,
						INSERT_BEFORE,
					    0,
					MENU_ITEM,
					    MENU_STRING, "After",
					    MENU_ACTION, insert_proc,
					    XV_KEY_DATA, PANEL_INSERT,
						INSERT_AFTER,
					    0,
					0),
				0,
			    MENU_ITEM,
				MENU_STRING, "Delete",
				MENU_ACTION, delete_proc,
				0,
			    MENU_ITEM,
				MENU_STRING,	"",
				MENU_FEEDBACK, FALSE,
				0,
			    MENU_ITEM,
				MENU_STRING, "End Editing",
				MENU_ACTION, enter_read_mode,
				0,
			    0);
		    }   /* if (!panel_list->edit_menu) */
		}   /* if (!panel_list->read_only) */
	    }   /* if (!panel_list->read_menu) */
	}   /* if (xv_end_create) */
    }	/* if (panel_list->initialized) */

    return XV_OK;
}


Pkg_private     Xv_opaque
panel_list_get_attr(panel_list_public, status, which_attr, valist)
    Panel_item      panel_list_public;
    int            *status;
    Panel_attr      which_attr;
    va_list         valist;
{
    Panel_list_info *panel_list =
    PANEL_LIST_PRIVATE(panel_list_public);

    switch (which_attr) {

      case PANEL_CHOOSE_NONE:
	return ((Xv_opaque) panel_list->choose_none);

      case PANEL_CHOOSE_ONE:
	return ((Xv_opaque) panel_list->choose_one);

      case PANEL_EVENT_PROC:
	return ((Xv_opaque) panel_list->event_proc);

      case PANEL_LIST_HEIGHT:
	return ((Xv_opaque) panel_list->height);

      case PANEL_LIST_WIDTH:
	return ((Xv_opaque) panel_list->width);

      case PANEL_LIST_DISPLAY_ROWS:
	return ((Xv_opaque) panel_list->rows_displayed);

      case PANEL_LIST_SELECTED:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(panel_list, row, FALSE);
	    return (node ? (Xv_opaque) node->selected : (Xv_opaque) XV_ERROR);
	    break;
	}

      case PANEL_LIST_CLIENT_DATA:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(panel_list, row, FALSE);
	    return (node ? (Xv_opaque) node->client_data : (Xv_opaque) XV_ERROR);
	    break;
	}

      case PANEL_LIST_FONT:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(panel_list, row, FALSE);
	    return (node ? (Xv_opaque) node->font : (Xv_opaque) XV_ERROR);
	    break;
	}

      case PANEL_LIST_STRING:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(panel_list, row, FALSE);
	    return (node ? (Xv_opaque) xv_get(node->string_item, PANEL_LABEL_STRING)
		    : (Xv_opaque) XV_ERROR);
	    break;
	}

      case PANEL_LIST_GLYPH:{
	    int             row = *(int *) valist;
	    Row_info       *node = find_or_create_nth_row(panel_list, row, FALSE);
	    return (node ? (Xv_opaque) xv_get(node->glyph_item,
	    				      PANEL_LABEL_IMAGE)
		    : (Xv_opaque) XV_ERROR);
	}

      case PANEL_LIST_NROWS:
	return ((Xv_opaque) panel_list->nrows);

      case PANEL_ITEM_MENU:
	return ((Xv_opaque) panel_list->edit_mode ? panel_list->edit_menu :
	    panel_list->read_menu);

      case PANEL_READ_ONLY:
	return ((Xv_opaque) panel_list->read_only);

      default:
	*status = XV_ERROR;
	return ((Xv_opaque) 0);
    }
}


/*
 * Create N items. Actually just assure that at least this many are created.
 * If this is called before the creation of strings or glyphs, it creates
 * blank item.  If called afterwards, it appends that many more blank items
 * at the of the list of rows.
 */
static void
panel_list_create_nrows(panel_list)
    Panel_list_info *panel_list;
{
    int             i;
    Row_info       *node = NULL;
    Row_info       *prev;

    for (i = 0; i < panel_list->nrows; i++) {
	prev = node;
	node = xv_alloc(Row_info);
	node->next = NULL;
	node->prev = prev;
	node->row = i;
	node->selected = FALSE;
	node->string_item = 0;
	node->glyph_item = 0;
	if (prev)
	    prev->next = node;
	else {
	    if (panel_list->rows) {
	        Row_info *last_item = panel_list->rows;
	        
	        while (last_item->next != (Row_info *)NULL) {
	            last_item = last_item->next;
	        }
	        last_item->next = node;
	        node->prev = last_item;
	    } else
	        panel_list->rows = node;
	}
    }
}


static void
paint(ip)
    register Item_info *ip;
{
    /* Paint the label.  The value is the scrolling list panel.  Since
     * the panel (which is a window) will get its own repaint event,
     * it is unnecessary to paint it here.
     */
    panel_paint_image(ip->panel, &ip->label, &ip->label_rect, inactive(ip),
		      ip->color_index);
}


/*
 * Find/Create the row
 */
static Row_info *
find_or_create_nth_row(panel_list, which_row, create)
    Panel_list_info *panel_list;
    int             which_row;
    int             create;
{
    Row_info       *prev = NULL;
    Row_info       *node = panel_list->rows;

    while (node && (node->row != which_row)) {
	prev = node;
	node = node->next;
    }
    if (!node && create) {
	node = xv_alloc(Row_info);
	if (prev) {
	    node->row = prev->row + 1;
	    prev->next = node;
	} else {
	    panel_list->rows = node;
	    node->row = 0;
	}
	node->next = NULL;
	node->prev = prev;
	/*
	 * It's possible that prev->row + 1 is not equal to which_row ie. the
	 * row is created at the end of the list
	 */
	node->string_item = 0;
	node->glyph_item = 0;
	panel_list->nrows++;
    }
    return (node);
}


static Row_info *
next_row(panel_list, row, n)
    Panel_list_info *panel_list;
    Row_info       *row;
    int             n;
{
    Row_info       *node = NULL;

    if (!row) {
	panel_list->rows = row = xv_alloc(Row_info);
	panel_list->nrows = 1;
	row->prev = NULL;
    } else {
	if (n == 0) {		/* If it is the first row, return the row
				 * itself */
	    return (row);
	} else if (row->next)	/* Already created */
	    return (row->next);
	else {
	    node = xv_alloc(Row_info);
	    node->prev = row;
	    row->next = node;
	    row = node;
	    panel_list->nrows++;
	}
    }
    row->next = NULL;
    row->selected = FALSE;
    row->row = n;
    row->string_item = NULL;
    row->glyph_item = NULL;
    return (row);
}


static Row_info *
panel_list_insert_row(panel_list, which_row, repaint)
    Panel_list_info *panel_list;
    int             which_row;
    int		    repaint;
{
    Row_info       *node = panel_list->rows;
    Row_info       *prev = NULL;
    Row_info       *row = xv_alloc(Row_info);
    Item_info      *ip = ITEM_PRIVATE(panel_list->public_self);

    while (node && (node->row != which_row)) {
	prev = node;
	node = node->next;
    }
    row->selected = FALSE;
    row->next = node;
    row->prev = prev;
    row->glyph_item = NULL;

    /*
     * It's possible that prev->row + 1 is not equal to which_row ie. the row
     * is created at the end of the list
     */
    if (prev) {
	prev->next = row;
	row->row = prev->row + 1;
    } else {			/* Insert at the begining of the list */
	panel_list->rows = row;
	row->row = 0;
    }
    if (node) {
	node->prev = row;
    }
    row->string_item = xv_create(panel_list->row_panel, PANEL_MESSAGE,
				 PANEL_LABEL_STRING, "",
				 PANEL_ITEM_X, panel_list->string_item_x,
		      		 PANEL_ITEM_Y, xv_row(panel_list->row_panel, 
		      		 	 	      row->row),
				 PANEL_ITEM_COLOR, ip->color_index,
				 PANEL_CLIENT_DATA, panel_list,
				 PANEL_EVENT_PROC, panel_list_event_proc,
				 0);
    while (node) {
	node->row++;
	set_row_n_col(panel_list, node);
	node = node->next;
    }
    panel_list->nrows++;

    if (repaint)
	panel_paint(panel_list->row_panel, PANEL_CLEAR);

    return(row);
}


static void
panel_list_delete_row(panel_list, node)
    Panel_list_info *panel_list;
    Row_info        *node;
{
    Row_info       *prev = node->prev;

    if (prev) {
	prev->next = node->next;
    } else {
	panel_list->rows = node->next;
    }

    if (node->next) {
	node->next->prev = prev;
    }
    if (node->string_item) {
	xv_destroy(node->string_item);
	node->string_item = NULL;
    }
    if (node->glyph_item) {
	xv_destroy(node->glyph_item);
	node->glyph_item = NULL;
    }
    panel_list->nrows--;
    /* Adjust the row numbers */
    prev = node;
    node = node->next;
    xv_free(prev);
    while (node) {
	node->row--;
	set_row_n_col(panel_list, node);
	node = node->next;
    }
}


static void
panel_list_bg_proc(panel, event)
    Panel	    panel;
    Event          *event;
{
    Panel_list_info *panel_list;

    panel_list = (Panel_list_info *) xv_get(panel, XV_KEY_DATA, PANEL_LIST);
    if (panel_list->event_proc) {
	(*panel_list->event_proc)(panel_list->public_self, event);
	return;
    }
    if (event_action(event) == ACTION_MENU && event_is_down(event))
	handle_menu_event(panel_list, event);
}


static void
handle_menu_event(panel_list, event)
    Panel_list_info	*panel_list;
    Event		*event;
{
    int		    edit_cnt;
    Menu	    menu;
    Panel_item	    change_item;
    Panel_item	    delete_item;
    Panel_item	    insert_item;
    Row_info	    *edit_row;
    Row_info	    *row;

    if (panel_list->edit_mode) {
	menu = panel_list->edit_menu;
	if (menu) {
	    /* Note: The client can change the edit menu */
	    change_item = xv_find(menu, MENUITEM,
		    XV_AUTO_CREATE, FALSE,
		    MENU_STRING, "Change",
		    0);
	    delete_item = xv_find(menu, MENUITEM,
		    XV_AUTO_CREATE, FALSE,
		    MENU_STRING, "Delete",
		    0);
	    insert_item = xv_find(menu, MENUITEM,
		    XV_AUTO_CREATE, FALSE,
		    MENU_STRING, "Insert",
		    0);
	    edit_row = (Row_info *) xv_get(panel_list->text_item,
		XV_KEY_DATA, PANEL_LIST_SELECTED);
	    if (change_item || delete_item) {
		edit_cnt = 0;
		for (row=panel_list->rows; row; row=row->next)
		    if (row->edit_selected && row != edit_row)
			edit_cnt++;
		if (change_item)
		    xv_set(change_item,
			MENU_INACTIVE, edit_row || edit_cnt != 1,
			0);
		if (delete_item)
		    xv_set(delete_item,
			MENU_INACTIVE, edit_cnt == 0,
			0);
	    }
	    if (insert_item)
		xv_set(insert_item,
		    MENU_INACTIVE, edit_row ? TRUE : FALSE,
		    0);
	}
    } else
	menu = panel_list->read_menu;
    if (menu)
	menu_show(menu, event_window(event), event, 0);
}


static void
text_event_proc(item, event)
    Panel_item	    item;
    Event	    *event;
{
    Panel_list_info *panel_list = (Panel_list_info *) xv_get(item,
	XV_KEY_DATA, PANEL_LIST);

    if (event_action(event) == ACTION_MENU && event_is_down(event))
	handle_menu_event(panel_list, event);
    else
	panel_default_handle_event(item, event);
}


static void
panel_list_event_proc(item, event)
    Panel_item      item;
    Event          *event;
{
    Panel_list_info *panel_list = (Panel_list_info *)xv_get(item,
	PANEL_CLIENT_DATA);

    if (panel_list->event_proc) {
	(*panel_list->event_proc)(panel_list->public_self, event);
	return;
    }

    switch (event_action(event)) {

      case ACTION_SELECT:
	if (event_is_down(event)) {
	    if (panel_list->edit_mode)
	    	set_edit_item(panel_list, item, FALSE, event);
	    else
	        set_current_item(panel_list, item, event);
	}
	break;

      case ACTION_ADJUST:
	if (panel_list->edit_mode) {
	    if (event_is_down(event))
		set_edit_item(panel_list, item, TRUE, event);
	} else {
	/* BUG ALERT: display the question mark while ADJUST is down */
	}
	break;

      case ACTION_MENU:
	if (event_is_down(event))
	    handle_menu_event(panel_list, event);
	break;


      case PANEL_EVENT_CANCEL:
	/* Ignore cancel events */
	break;

      case PANEL_EVENT_DRAG_IN:
	if (event_left_is_down(event)) {
	    if (panel_list->edit_mode)
	    	set_edit_item(panel_list, item, TRUE, event);
	    else if (panel_list->current_item != item)
	        set_current_item(panel_list, item, event);
	} else if (event_middle_is_down(event) && panel_list->edit_mode) {
	    set_edit_item(panel_list, item, TRUE, event);
	}
	break;

      default:
	panel_default_handle_event(item, event);
	break;
    }
}


static void
set_current_item(panel_list, item, event)
    Panel_list_info	*panel_list;
    Panel_item		item;
    Event		*event;	/* NULL => not event generated */
{
    Row_info	*row;
    int		new_state = TRUE;
    int		toggle = FALSE;
    
    if (panel_list->choose_one) {
    	if (panel_list->current_item == item) {
    	    if (panel_list->choose_none)
    	    	toggle = TRUE;
    	} else if (panel_list->current_item) {
    	    if (row = find_row(panel_list, panel_list->current_item))
    	    	row->selected = FALSE;
    	    show_feedback(panel_list, panel_list->current_item, FALSE, event);
    	}
    	if (row = find_row(panel_list, item)) {
    	    if (toggle)
    	        new_state = row->selected ? FALSE : TRUE;
    	    row->selected = new_state;
    	    show_feedback(panel_list, item, new_state, event);
    	}
    } else if (row = find_row(panel_list, item)) {
    	new_state = row->selected ? FALSE : TRUE;
    	row->selected = new_state;
    	show_feedback(panel_list, item, new_state, event);
    }
    panel_list->current_item = item;
}


static void
set_edit_item(panel_list, item, toggle, event)
    Panel_list_info	*panel_list;
    Panel_item		item;	/* row's string message item */
    int			toggle;	/* TRUE or FALSE */
    Event		*event;
{
    Row_info	*row;
    int		new_state = TRUE;
    
    if (!toggle) {
    	for (row=panel_list->rows; row; row=row->next) {
    	    if (row->string_item == item) {
    	    	show_feedback(panel_list, row->string_item, TRUE, event);
    	    	row->edit_selected = TRUE;
    	    } else if (row->edit_selected) {
    	        show_feedback(panel_list, row->string_item, FALSE, event);
    	    	row->edit_selected = FALSE;
    	    }
    	}
    } else if (row = find_row(panel_list, item)) {
    	new_state = row->edit_selected ? FALSE : TRUE;
    	show_feedback(panel_list, item, new_state, event);
    	row->edit_selected = new_state;
    }
}


static Row_info *
find_row(panel_list, item)
    Panel_list_info *panel_list;
    Panel_item      item;
{
    Row_info	*row;
    
    for(row=panel_list->rows; row; row=row->next)
    	if (row->string_item == item)
    	    return(row);
    return(NULL);
}


static void
unset_all_selected_items(panel_list)
    Panel_list_info *panel_list;
{
    Row_info       *row = panel_list->rows;

    while (row) {
	if (row->selected) {
	    show_feedback(panel_list, row->string_item, FALSE, NULL);
	    row->selected = FALSE;
	}
	row = row->next;
    }
}


static void
panel_list_seln_function(panel_list, buffer)
    Panel_list_info *panel_list;
    Seln_function_buffer *buffer;
{
    Xv_object       server = xv_get(xv_get(panel_list->row_panel, XV_SCREEN),
				    SCREEN_SERVER);
    Seln_holder    *holder;

    switch (selection_figure_response(server, buffer, &holder)) {
      case SELN_IGNORE:
      case SELN_REQUEST:
      case SELN_FIND:
      case SELN_DELETE:
	break;

      case SELN_SHELVE:
	if (selection_acquire(server, panel_list->seln_client, SELN_SHELF)
	    == SELN_SHELF) {
	    panel_list->have_seln[SHELF_CHOICE] = TRUE;
	    (void) selection_string(panel_list, SHELF_CHOICE);
	}
	break;
      default:
	break;
    }
}


static          Seln_result
panel_list_reply_proc(attr, context, max_length)
    Seln_attribute  attr;
    Seln_replier_data *context;
    int             max_length;
{
    Panel_list_info *panel_list = (Panel_list_info *) context->client_data;
    Bool            seln;

    switch (context->rank) {
      case SELN_PRIMARY:
	seln = panel_list->have_seln[PRIMARY_CHOICE];
	break;
      case SELN_SHELF:
	seln = panel_list->have_seln[SHELF_CHOICE];
	break;
      default:
	seln = FALSE;
	break;
    }

    if (!seln)
	return (SELN_DIDNT_HAVE);

    switch (attr) {
      case SELN_REQ_BYTESIZE:
	if (context->rank == SELN_PRIMARY)
	    *context->response_pointer++ =
		(char *) sizeof_selection(panel_list, seln);
	else
	    *context->response_pointer++ =
		(char *) strlen(panel_list->selection[SHELF_CHOICE]);
	return (SELN_SUCCESS);

      case SELN_REQ_CONTENTS_ASCII:
	return (get_selection(panel_list, context, max_length));

      case SELN_REQ_YIELD:
	unset_all_selected_items(panel_list);
	*context->response_pointer++ = (char *) SELN_SUCCESS;
	return (SELN_SUCCESS);

      case SELN_REQ_END_REQUEST:
	return (SELN_SUCCESS);

      default:
	return (SELN_UNRECOGNIZED);
    }
}


static int
sizeof_selection(panel_list)
    Panel_list_info *panel_list;
{
    Row_info       *row = panel_list->rows;
    int             nbytes = 0;
    int             how_many = 0;
    while (row) {
	if (row->selected && row->string_item) {
	    how_many++;
	    nbytes += strlen(xv_get(row->string_item, PANEL_LABEL_STRING));
	}
	row = row->next;
    }
    return (nbytes + how_many);
}


static          Seln_result
get_selection(panel_list, context, length)
    Panel_list_info *panel_list;
    Seln_replier_data *context;
    int             length;
{
    int             size;
    char           *destp;
    int             needed;

    if (context->context == NULL) {
	/*
	 * if this was called the first time, get selection from the selected
	 * items. If the rank is SELN_SHELF, selection_string already exists
	 */
	if (context->rank == SELN_PRIMARY)
	    context->context = selection_string(panel_list, PRIMARY_CHOICE);
	else
	    context->context = panel_list->selection[SHELF_CHOICE];
    }
    size = strlen(context->context);
    destp = (char *) context->response_pointer;

    needed = size + 4;
    if (size % 4 != 0)
	needed += 4 - size % 4;
    if (needed <= length) {
	/* Entire selection fits */
	strcpy(destp, context->context);
	destp += size;
	while ((int) destp % 4 != 0)
	    *destp++ = '\0';
	context->response_pointer = (char **) destp;
	*context->response_pointer++ = 0;
	return (SELN_SUCCESS);
    } else {
	strncpy(destp, context->context, length);
	destp += length;
	context->response_pointer = (char **) destp;
	context->context += length;
	return (SELN_CONTINUED);
    }
}


static char *
selection_string(panel_list, choice)
    Panel_list_info *panel_list;
    int             choice;
{
    char           *temp;
    int             size = sizeof_selection(panel_list);
    Row_info       *row = panel_list->rows;
    int             how_many = 0;
    char           *str;

    if (panel_list->selection[choice] != NULL)
	xv_free(panel_list->selection[choice]);
    panel_list->selection[choice] = temp = xv_alloc_n(char *, size);
    while (row) {
	if (row->selected && row->string_item) {
	    if (how_many)
		*temp++ = '\n';
	    how_many++;
	    str = (char *) xv_get(row->string_item, PANEL_LABEL_STRING);
	    bcopy(str, temp, strlen(str));
	    temp += strlen(str);
	}
	row = row->next;
    }
    return (panel_list->selection[choice]);
}


static void
show_feedback(panel_list, item, selected, event)
    Panel_list_info *panel_list;
    Panel_item item;
    int selected;	/* TRUE or FALSE */
    Event *event;	/* NULL => feedback was not event generated */
{
    Item_info	*panel_list_ip;
    int		(*notify_proc)();

    xv_set(item,
	panel_list->edit_mode ? PANEL_LABEL_INVERTED : PANEL_LABEL_BOXED,
	    selected,
	XV_KEY_DATA, PANEL_LIST_ROW_HIGHLIGHTED, selected,
	0);
    if (!panel_list->edit_mode) {
	panel_list_ip = ITEM_PRIVATE(panel_list->public_self);
	notify_proc = panel_list_ip->notify;
	if (notify_proc && event)
	    (*notify_proc) (item,
		selected ? PANEL_LIST_OP_SELECT : PANEL_LIST_OP_DESELECT,
		event);
    }
}


static Xv_opaque
locate_next_choice(menu, menu_item)
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *panel_list;
    Row_info *row;
    Scrollbar sb;
    int object_length;
    int view_end;
    int view_length;
    int view_start;
    
    panel_list = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    sb = xv_get(panel_list->row_panel, WIN_VERTICAL_SCROLLBAR);

    /* Determine the range of the view */
    view_start = (int) xv_get(sb, SCROLLBAR_VIEW_START);
    view_end = view_start + (int) xv_get(sb, SCROLLBAR_VIEW_LENGTH) - 1;

    /* Find the first row beyond the view (if any) */
    for (row=panel_list->rows; row; row=row->next) {
	if ((int) xv_get(row->string_item, PANEL_ITEM_Y) >= view_end)
	    break;
    }
    if (!row)
	return(XV_OK);	/* last row is in view */

    /* Find the next selected row (if any) */
    for (; row; row=row->next) {
	if (row->selected)
	    break;
    }
    if (!row)
	return(XV_OK);	/* last selected row is not in a later view */

    /* Scroll the display to this row */
    view_start = (int) xv_get(row->string_item, PANEL_ITEM_Y);
    object_length = (int) xv_get(sb, SCROLLBAR_OBJECT_LENGTH);
    view_length = (int) xv_get(sb, SCROLLBAR_VIEW_LENGTH);
    view_start = MIN(view_start, object_length - view_length);
    xv_set(sb, SCROLLBAR_VIEW_START, view_start, 0);
    return(XV_OK);
}


static Xv_opaque
clear_all_choices(menu, menu_item)
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *panel_list;
    Event	*event = (Event *) xv_get(menu, MENU_LAST_EVENT);
    Row_info	*row;
    
    panel_list = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    for (row=panel_list->rows; row; row=row->next) {
	if (row->selected) {
	    show_feedback(panel_list, row->string_item, FALSE, event);
	    row->selected = FALSE;
	}
    }
    return(XV_OK);
}


static Xv_opaque
enter_edit_mode(menu, menu_item)
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *panel_list;
    int			display_length;
    int			text_width;
    Item_info		*panel_list_ip;
    Pixfont		*font;
    
    panel_list = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    panel_list->edit_mode = TRUE;

    /*
     *** Create a text field for editing ***
     */
    /* Determine the panel text field's display length, in characters. */
    font = (Pixfont *) xv_get(panel_list->row_panel, XV_FONT);
    text_width = (int) xv_get(
	xv_get(panel_list->row_panel, OPENWIN_NTH_VIEW, 0),
	XV_WIDTH, 0) - ROW_MARGIN - panel_list->string_item_x;
    display_length = text_width / font->pf_defaultsize.x;
    
    /* Create the text field */
    panel_list_ip = ITEM_PRIVATE(panel_list->public_self);
    panel_list->text_item = xv_create(panel_list->row_panel, PANEL_TEXT,
	PANEL_EVENT_PROC, text_event_proc,
    	PANEL_ITEM_X,	panel_list->string_item_x,
    	PANEL_NOTIFY_STRING,	"\r",	/* RETURN only */
    	PANEL_VALUE_DISPLAY_LENGTH,	display_length,
	XV_KEY_DATA, PANEL_LIST, panel_list,
    	XV_KEY_DATA, PANEL_LIST_SELECTED, 0,   /* no row being edited */
    	XV_KEY_DATA, PANEL_NOTIFY_PROC, panel_list_ip->notify,
	XV_SHOW, FALSE,
    	0);
    return(XV_OK);
}


static void
accept_change(text_item, edit_row)
    Panel_item	text_item;
    Row_info	*edit_row;
{
    Item_info	*string_ip = ITEM_PRIVATE(edit_row->string_item);
    Rect	string_rect;
    
    string_rect = string_ip->rect;

    xv_set(text_item,
	   XV_KEY_DATA, PANEL_LIST_SELECTED, 0,   /* no row being edited */
	   XV_SHOW, FALSE,
	   0);
    xv_set(edit_row->string_item,
    	   PANEL_LABEL_STRING, xv_get(text_item, PANEL_VALUE),
    	   0);

    /* Restore string item label and item rect.
     * Note: Message items do not have values, so the item rect
     *       is the same as the label rect.
     */
    string_ip->label_rect = string_rect;
    string_ip->rect = string_rect;

    xv_set(edit_row->string_item,
    	   XV_SHOW, TRUE,
	   0);
}


static Panel_setting
change_done(text_item, event)
    Panel_item	text_item;
    Event	*event;
{
    int		(*notify_proc)();
    Panel_list_info *panel_list;
    Row_info	*edit_row;
    
    /* Validate entry */
    notify_proc = (int (*)()) xv_get(text_item,
    	XV_KEY_DATA, PANEL_NOTIFY_PROC);
    if (notify_proc &&
	(*notify_proc) (text_item, PANEL_LIST_OP_VALIDATE, event) == XV_ERROR)
	return(PANEL_NONE);

    /* Entry validated.  Update edit row. */
    edit_row = (Row_info *) xv_get(text_item,
				   XV_KEY_DATA, PANEL_LIST_SELECTED); 
    accept_change(text_item, edit_row);

    panel_list = (Panel_list_info *) xv_get(text_item, XV_KEY_DATA, PANEL_LIST);
    panel_list->edit_op = OP_NONE;
    return(PANEL_NONE);
}


static Xv_opaque
change_proc(menu, menu_item)
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *panel_list;
    Row_info		*edit_row = NULL;

    panel_list = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    for (edit_row=panel_list->rows; edit_row; edit_row=edit_row->next) {
    	if (edit_row->edit_selected)
	    break;
    }
    if (!edit_row || !panel_list->text_item)
    	return(XV_OK);	/* shouldn't occur, but let's be safe */

    xv_set(edit_row->string_item, XV_SHOW, FALSE, 0);
    xv_set(panel_list->text_item,
    	PANEL_ITEM_Y,	xv_get(edit_row->string_item, PANEL_ITEM_Y),
    	PANEL_NOTIFY_PROC,	change_done,
    	PANEL_VALUE,	xv_get(edit_row->string_item, PANEL_LABEL_STRING),
    	XV_KEY_DATA,	PANEL_LIST_SELECTED, edit_row,
	XV_SHOW,	TRUE,
	0);
    
    panel_list->edit_op = OP_CHANGE;
    return(XV_OK);
}


static void
check_row_in_view(panel_list, item_y, item_height)
    Panel_list_info *panel_list;
    int item_y;
    int item_height;
{
    int			view_length;
    int			view_start;
    Scrollbar		sb;

    /* If item_y is out of the view window, then scroll item_y
     * to the top of the view window.
     */
    sb = xv_get(panel_list->row_panel, WIN_VERTICAL_SCROLLBAR);
    view_start = (int) xv_get(sb, SCROLLBAR_VIEW_START);
    view_length = (int) xv_get(sb, SCROLLBAR_VIEW_LENGTH);
    if (item_y < view_start || item_y + item_height > view_start + view_length)
	xv_set(sb, SCROLLBAR_VIEW_START, item_y, 0);
}


static void
accept_insert(panel_list, row)
    Panel_list_info *panel_list;
    Row_info	    *row;
{
    int		row_panel_view_width;
    Xv_opaque	row_panel_view_window;
    Item_info	*ip;

    xv_set(panel_list->text_item, XV_SHOW, FALSE, 0);
    xv_set(row->string_item,
	PANEL_LABEL_STRING, xv_get(panel_list->text_item, PANEL_VALUE),
	XV_SHOW, TRUE,
	0);

    /* Adjust item rect to panel view window width.
     * Set height to a full row.
     */
    row_panel_view_window = xv_get(panel_list->row_panel,
				   OPENWIN_NTH_VIEW, 0);
    row_panel_view_width = (int) xv_get(row_panel_view_window, XV_WIDTH);
    ip = ITEM_PRIVATE(row->string_item);
    ip->label_rect.r_width = (short)(row_panel_view_width - 2 - ROW_MARGIN
	- panel_list->string_item_x);
    ip->label_rect.r_height = (short)xv_rows(panel_list->row_panel, 1);
    /* Message items do not have value rects, so the item rect
     * is equal to the label rect.
     */
    ip->rect.r_width = ip->label_rect.r_width;
    ip->rect.r_height = ip->label_rect.r_height;
}

	
static Panel_setting
insert_done(text_item, event)
    Panel_item	text_item;
    Event	*event;
{
    int		item_y;
    int		(*notify_proc)();
    Panel_list_info *panel_list;
    Row_info	*insert_row;
    
    panel_list = (Panel_list_info *) xv_get(text_item,
	XV_KEY_DATA, PANEL_LIST);
    insert_row = (Row_info *) xv_get(text_item,
	XV_KEY_DATA, PANEL_LIST_SELECTED); 

    /* If value is empty, then exit insert mode. */
    if (strlen(xv_get(text_item, PANEL_VALUE)) == 0) {
	xv_set(text_item,
	    XV_KEY_DATA, PANEL_LIST_SELECTED, 0,   /* no row being edited */
	    XV_SHOW, FALSE, 0);
	panel_list_delete_row(panel_list, insert_row);
	panel_list->edit_op = OP_NONE;
	return(PANEL_NONE);
    }

    /* Validate entry */
    notify_proc = (int (*)()) xv_get(text_item,
    	XV_KEY_DATA, PANEL_NOTIFY_PROC);
    if (notify_proc &&
	(*notify_proc) (text_item, PANEL_LIST_OP_VALIDATE, event) == XV_ERROR)
	return(PANEL_NONE);

    /* Entry validated.  Insert into Scrolling List */
    accept_insert(panel_list, insert_row);

    /* Create a new row before the next row.  If no next row, then append. */
    insert_row = panel_list_insert_row(panel_list,
	insert_row->next ? insert_row->next->row : -1, TRUE);

    /* Continue insert mode. insert_row now points to empty
     * row just inserted.
     */
    item_y = (int) xv_get(insert_row->string_item, PANEL_ITEM_Y);
    xv_set(panel_list->text_item,
    	PANEL_ITEM_Y,	item_y,
    	PANEL_NOTIFY_PROC,	insert_done,
	PANEL_VALUE,	"",
    	XV_KEY_DATA,	PANEL_LIST_SELECTED, insert_row,
	XV_SHOW,	TRUE,
	0);

    /* If new row is out of the view window, then scroll new row
     * to top of view window.
     */
    check_row_in_view(panel_list, item_y,
	xv_get(panel_list->text_item, XV_HEIGHT));

    return(PANEL_NONE);
}


static Xv_opaque
insert_proc(menu, menu_item)
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *panel_list;
    int			item_y;
    int			which_row;
    Insert_pos_t	insert_position;
    Row_info		*insert_row = NULL;
    
    panel_list = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    insert_position = (Insert_pos_t) xv_get(menu_item,
	XV_KEY_DATA, PANEL_INSERT);

    /* Find the first row selected for editing */
    for (insert_row=panel_list->rows; insert_row;
	 insert_row=insert_row->next) {
	if (insert_row->edit_selected)
	    break;
    }
    if (!insert_row)
	insert_row = panel_list->rows;

    /* Determine where to insert new row */
    if (insert_position == INSERT_AFTER) {
	if (insert_row)
	    insert_row = insert_row->next;
	if (insert_row)
	    which_row = insert_row->row;
	else
	    which_row = -1;   /* append to end of list */
    } else {
	if (insert_row)
	    which_row = insert_row->row;
	else
	    which_row = 0;   /* insert at beginning of list */
    }

    /* Create insert row */
    insert_row = panel_list_insert_row(panel_list, which_row, TRUE);

    /* Overlay text item over blank new row */
    item_y = (int) xv_get(insert_row->string_item, PANEL_ITEM_Y);
    xv_set(panel_list->text_item,
    	PANEL_ITEM_Y,	item_y,
    	PANEL_NOTIFY_PROC,	insert_done,
	PANEL_VALUE,	"",
    	XV_KEY_DATA,	PANEL_LIST_SELECTED, insert_row,
	XV_SHOW,	TRUE,
	0);

    /* If new row is out of the view window, then scroll new row
     * to top of view window.
     */
    check_row_in_view(panel_list, item_y,
	xv_get(panel_list->text_item, XV_HEIGHT));
    
    panel_list->edit_op = OP_INSERT;
    return(XV_OK);
}


static Xv_opaque
delete_proc(menu, menu_item)
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *panel_list;
    int		(*notify_proc)();
    int		row_deleted;
    Item_info	*panel_list_ip;
    Row_info	*edit_row;   /* Row being edited with a text field */
    Row_info	*row;
    Event	*event;
    
    panel_list = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    panel_list_ip = ITEM_PRIVATE(panel_list->public_self);
    notify_proc = panel_list_ip->notify;
    if (notify_proc)
	event = (Event *) xv_get(menu, MENU_LAST_EVENT);
    edit_row = (Row_info *) xv_get(panel_list->text_item,
				   XV_KEY_DATA, PANEL_LIST_SELECTED);
    do {
	row_deleted = FALSE;
	for (row=panel_list->rows; row; row=row->next)
	    if (row->edit_selected && row != edit_row) {
		if (notify_proc)
		    (*notify_proc) (row->string_item,
			PANEL_LIST_OP_DELETE, event);
		panel_list_delete_row(panel_list, row);
		row_deleted = TRUE;
		break;
	    }
    } while (row_deleted);
    return(XV_OK);
}


static Xv_opaque
enter_read_mode(menu, menu_item)
    Menu menu;
    Menu_item menu_item;
{
    register Panel_list_info *panel_list;
    Item_info	*panel_list_ip;
    int		(*notify_proc)();
    Event	*event = (Event *) xv_get(menu, MENU_LAST_EVENT);
    Row_info	*row;

    panel_list = (Panel_list_info *) xv_get(menu, XV_KEY_DATA, PANEL_LIST);
    for (row=panel_list->rows; row; row=row->next) {
    	if (row->edit_selected) {
    	    row->edit_selected = FALSE;
    	    show_feedback(panel_list, row->string_item, FALSE, event);
    	}
    }
    row = (Row_info *) xv_get(panel_list->text_item,
			      XV_KEY_DATA, PANEL_LIST_SELECTED);
    if (row) {
	if (strlen(xv_get(panel_list->text_item, PANEL_VALUE)) == 0) {
	    xv_set(panel_list->text_item, XV_SHOW, FALSE, 0);
	    panel_list_delete_row(panel_list, row);
	} else {
	    panel_list_ip = ITEM_PRIVATE(panel_list->public_self);
	    notify_proc = panel_list_ip->notify;
	    if (!notify_proc ||
		(*notify_proc) (panel_list->text_item,
		    PANEL_LIST_OP_VALIDATE, event)
		== XV_OK) {
		/* Text was validated */
		if (panel_list->edit_op == OP_CHANGE)
		    accept_change(panel_list->text_item, row);
		else
		    accept_insert(panel_list, row);
	    } else {
		/* Text was not validated */
		xv_set(panel_list->text_item, XV_SHOW, FALSE, 0);
		if (strlen(xv_get(row->string_item, PANEL_LABEL_STRING)) == 0)
		    panel_list_delete_row(panel_list, row);
		else
		    xv_set(row->string_item, XV_SHOW, TRUE, 0);
	    }
	}
    }
    panel_list->edit_op = OP_NONE;
    xv_destroy_safe(panel_list->text_item);
    panel_list->text_item = NULL;
    panel_list->edit_mode = FALSE;
    return(XV_OK);
}
