/*  @(#)p_lst_impl.h 1.20 89/08/18 SMI */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */
#ifndef __panel_list_impl_h
#define __panel_list_impl_h

#include <xview/panel.h>
#include <xview/font.h>
#include <xview/openmenu.h>
#include <xview/scrollbar.h>

typedef struct panel_list_struct		Panel_list_info;
typedef struct panel_list_row_struct		Row_info;

typedef enum {
    OP_NONE,
    OP_CHANGE,
    OP_INSERT,
    OP_DELETE
} Edit_op;

#define PRIMARY_CHOICE	0
#define SHELF_CHOICE	1
#define PANEL_LIST_DEFAULT_ROW	5

struct panel_list_row_struct {
	unsigned short	row;            /* Row number */
	Panel_item	string_item;	/* Item in the row */
	Panel_item	glyph_item;	/* Item in the row */
	Xv_opaque	client_data;	/* Client data with each row */
	Xv_Font		font;
	unsigned selected : 1;          /* selected in read mode (= current) */
	unsigned edit_selected : 1;	/* selected in edit mode */

	/* Chaining */
	struct panel_list_row_struct *next;
	struct panel_list_row_struct *prev;
};
 
struct panel_list_struct {
	Panel_item	public_self;
        Panel		parent_panel;   /* Panel we're in */
	Panel		row_panel;	/* Panel containing items */
	Scrollbar	row_panel_sb;	/* Scrollbar for panel */
	Menu		edit_menu;	/* Row panel edit mode menu */
	Edit_op		edit_op;	/* current edit operation, if any */
	Menu		read_menu;	/* Row panel read mode menu */
	Panel_item	text_item;	/* Text item used during editing */
	void		(*event_proc)();
					/* Client panel event proc for list */

	/* control */
	unsigned choose_none: 1;	/* no current item is okay */
	unsigned choose_one : 1;    /* TRUE: exclusive, FALSE: non-exclusive */
	unsigned edit_mode : 1;		/* TRUE: read-write, FALSE: read */
	unsigned initialized : 1;	/* set of XV_END_CREATE has occurred */
	unsigned read_only : 1;		/* TRUE: read, FALSE: read/write */

	/* sizes and positions */
	unsigned short	nlevels;	/* Number of levels */
	unsigned short	height;		/* Height of the scroll list */
	unsigned short	width;		/* Width of the scroll list */
	unsigned short	nrows;		/* Number of rows */
	unsigned short	rows_displayed;	/* Number of rows displayed */
	unsigned short	row_height;  /* Height of each row. 0 => font height */
	unsigned short  string_item_x;	/* left margin of row string items */

	/* Current data */
	Row_info	*rows;
        char	 	*selection[2];  /* Seln string cache */
        int	 	have_seln[2];   /* Seln ownership info cache */
	char		*seln_client;   /* Selection client */
	Panel_item	current_item;	/* last item selected */
};

#endif __panel_list_impl_h



