/*	@(#)item_impl.h 20.27 89/08/18 SMI	*/

/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */

#ifndef _xview_private_item_impl_already_included
#define _xview_private_item_impl_already_included

/* panels and panel_items are both of type Xv_panel_or_item so that we
 * can pass them to common routines.
 */
#define ITEM_PRIVATE(i)		XV_PRIVATE(Item_info, Xv_item, i)
#define ITEM_PUBLIC(item)	XV_PUBLIC(item)

/*                    per item status flags                              */


#define IS_PANEL		0x00000001  /* object is a panel */
#define IS_ITEM			0x00000002  /* object is an item */
#define HIDDEN			0x00000004  /* item is not currently displayed */
#define ITEM_X_FIXED		0x00000008  /* item's x coord fixed by user*/
#define ITEM_Y_FIXED		0x00000010  /* item's y coord fixed by user*/
#define LABEL_X_FIXED		0x00000020  /* label x coord fixed by user */
#define LABEL_Y_FIXED		0x00000040  /* label y coord fixed by user */
#define VALUE_X_FIXED		0x00000080  /* value x coord fixed by user */
#define VALUE_Y_FIXED		0x00000100  /* value y coord fixed by user */
#define CREATED			0x00000200	/* XV_END_CREATE received */
#define WANTS_KEY		0x00000400  /* item wants keystroke events */
#define LABEL_BOLD     	 	0x00000800  /* for panel-wide setting */
#define VALUE_BOLD      	0x00001000  /* for panel-wide setting */
#define SHOW_MENU_MARK		0x00004000  /* show the menu mark in the menu */
#define ITEM_NOT_SCROLLABLE     0x00008000  /* item will not scroll */
#define LABEL_INVERTED		0x00020000  /* invert the label */
#define OPS_SET			0x00040000  /* ops vector has been altered */
#define PREVIEWING		0x00080000
#define BUSY			0x00100000  /* laf */
#define INACTIVE		0x00200000  /* laf */
#define IS_MENU_ITEM	0x00400000	/* paint item like a menu item */

#define hidden(ip)		((ip)->flags & HIDDEN)
#define inactive(ip)		((ip)->flags & INACTIVE)	/* laf */
#define item_fixed(ip)		((ip)->flags & (ITEM_X_FIXED | ITEM_Y_FIXED))
#define label_fixed(ip)		((ip)->flags & (LABEL_X_FIXED|LABEL_Y_FIXED))
#define value_fixed(ip)		((ip)->flags & (VALUE_X_FIXED|VALUE_Y_FIXED))
#define created(ip)			((ip)->flags & CREATED)
#define item_not_scrollable(ip)	((ip)->flags & ITEM_NOT_SCROLLABLE)
#define is_menu_item(ip)	((ip)->flags & IS_MENU_ITEM)
#define wants_key(object)	((object)->flags & WANTS_KEY)
#define label_bold_flag(object)	((object)->flags & LABEL_BOLD)
#define label_inverted_flag(object)	((object)->flags & LABEL_INVERTED)
#define value_bold_flag(object)	((object)->flags & VALUE_BOLD)
#define is_panel(object)	((object)->flags & IS_PANEL)
#define is_item(object)		((object)->flags & IS_ITEM)
#define show_menu_mark(object)	((object)->flags & SHOW_MENU_MARK)
#define ops_set(object)		((object)->flags & OPS_SET)
#define previewing(object)	((object)->flags & PREVIEWING)


/* 			miscellaneous constants                          */

#define	BIG			0x7FFF
#define	KEY_NEXT		KEY_BOTTOMRIGHT
#define	ITEM_X_GAP		10	/* # of x pixels between items */
#define	ITEM_Y_GAP		13	/* laf: # of y pixels between items rows */
#define LABEL_X_GAP   		8	/* used in panel_attr.c */
#define LABEL_Y_GAP 		4	/* used in panel_attr.c */

/* 			structures                                      */


/*********************** panel_image **************************************/

#define	IM_PIXRECT	2	/* pixrect */
#define	IM_STRING	3	/* string */

typedef struct panel_image {
   unsigned int	im_type;
   unsigned int	inverted : 1;	/* true to invert the image */
   unsigned int boxed : 1;	/* true to enclose image in a box */
   union {
     struct {			
	 char           *text;
	 Pixfont	*font;
	 short 		 bold;	/* TRUE if text should be bold */
     } t;        			/*  IM_STRING arm	*/
     Pixrect		*pixrect;	/*  IM_PIXRECT arm  */
   } im_value;
} Panel_image;

#define image_type(image)  	((image)->im_type)
#define image_inverted(image)   ((image)->inverted)
#define image_boxed(image)	((image)->boxed)
#define is_string(image)	(image_type(image) == IM_STRING)
#define is_pixrect(image)	(image_type(image) == IM_PIXRECT)
#define image_string(image)  	((image)->im_value.t.text)
#define image_font(image)    	((image)->im_value.t.font)
#define image_bold(image)    	((image)->im_value.t.bold)
#define image_pixrect(image) 	((image)->im_value.pixrect)

#define image_set_type(image, type)	  (image_type(image)    = type)
#define image_set_string(image, string)	  (image_string(image)	= (string))
#define image_set_pixrect(image, pixrect) (image_pixrect(image) = (pixrect))
#define image_set_font(image, font)	  panel_image_set_font(image, font)
#define image_set_bold(image, bold)  	(image_bold(image)	= (bold) != 0)
#define image_set_inverted(image, inverted) (image_inverted(image) = (inverted) != 0)
#define image_set_boxed(image, boxed)	(image_boxed(image) = (boxed) != 0)


/****************** ops vector for panels & items *****************************/

/* Note that the null_ops static struct
 * in panel_public.c must be updated when
 * this structure is changed.
 */
typedef struct panel_ops {
   void	   (*handle_event)();
   void	   (*begin_preview)();
   void	   (*update_preview)();
   void	   (*cancel_preview)();
   void	   (*accept_preview)();
   void	   (*accept_menu)();
   void	   (*accept_key)();
   void	   (*paint)();
   void	   (*remove)();
   void    (*restore)();
   void	   (*layout)();
} Panel_ops;


/*************************** panel item ***********************************/

/* NOTE: The first three fields of the
 * panel_item struct must match those of the panel
 * struct, since these are used interchangably in 
 * some cases.
 */
typedef struct item_info {
    /****  DO NOT CHANGE THE ORDER OR PLACEMENT OF THESE THREE FIELDS ****/
   Panel_ops		*ops;		/* item type specific operations */
   unsigned int		flags;		/* boolean attributes */
   Panel_item		public_self;	/* back pointer to object */
    /**** ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^ ****/

   Panel_item_type	item_type;	/* type of this item */
   struct panel_info	*panel;		/* panel subwindow for the item */
   Rect			painted_rect;	/* painted area in the pw */
   Rect			rect;		/* enclosing item rect */
   struct panel_image	label;		/* the label */
   int			label_width;	/* desired label width (0= fit to image) */
   Rect			label_rect;	/* enclosing label rect */
   Rect			value_rect;	/* enclosing value rect */
   Panel_setting	layout;	        /* HORIZONTAL, VERTICAL */
   int			(*notify)();	/* notify proc */
   int			notify_status;	/* notify proc status: XV_OK or XV_ERROR */
   Panel_setting        repaint;	/* item's repaint behavior */
   struct item_info     *next; 		/* next item */
   Xv_opaque	        client_data;	/* for client use */
   Xv_opaque		 menu;
   Pixrect 		*menu_mark_on;	/* selected menu mark image */
   Pixrect 		*menu_mark_off;		/* un-sel. menu mark image*/
   short 		 menu_mark_width;	/* width of menu mark */
   short		 menu_mark_height;	/* height of menu mark */
   int                   color_index;      /* for color panel items */
} Item_info;

/***********************************************************************/
/* external declarations of private functions                          */
/***********************************************************************/

Pkg_private	int 			item_init();
Pkg_private  Xv_opaque		item_set_avlist();
Pkg_private  Xv_opaque		item_get_attr();
Pkg_private	int			item_destroy();

Pkg_private	Pixfont			*item_get_panel_font();

/* laf begin */
Pkg_private	Pixrect			panel_choice_off_12_pr;
Pkg_private	Pixrect			panel_choice_on_12_pr;
Pkg_private	Pixrect			panel_choice_off_14_pr;
Pkg_private	Pixrect			panel_choice_on_14_pr;
Pkg_private	Pixrect			panel_choice_off_20_pr;
Pkg_private	Pixrect			panel_choice_on_20_pr;
Pkg_private	Pixrect			panel_choice_off_24_pr;
Pkg_private	Pixrect			panel_choice_on_24_pr;
Pkg_private	Pixrect			panel_choice_off_32_pr;
Pkg_private	Pixrect			panel_choice_on_32_pr;

Pkg_private	Pixrect			panel_toggle_off_10_pr;
Pkg_private	Pixrect			panel_toggle_on_10_pr;
Pkg_private	Pixrect			panel_toggle_off_12_pr;
Pkg_private	Pixrect			panel_toggle_on_12_pr;
Pkg_private	Pixrect			panel_toggle_off_14_pr;
Pkg_private	Pixrect			panel_toggle_on_14_pr;
Pkg_private	Pixrect			panel_toggle_off_19_pr;
Pkg_private	Pixrect			panel_toggle_on_19_pr;
/* laf end */

#endif
