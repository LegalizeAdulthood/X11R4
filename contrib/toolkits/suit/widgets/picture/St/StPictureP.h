/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/* 
 * StPictureP.h - Private definitions for StPicture widget
 * 
 */

#ifndef _StPictureP_h
#define _StPictureP_h

#include <St/StPictureA.h>
#include <St/StPicture.h>
#include <St/StPictureD.h>

#include <X11/SimpleP.h>

/***********************************************************************
 *
 * StPicture Widget Private Data
 *
 ***********************************************************************/

#define		DEFAULT_WIDTH	500
#define		DEFAULT_HEIGHT	500
#define		DEFAULT_MAX	32767
#define		DEFAULT_MIN	-32768
#define		StpopupTime	280
#define		MAIN_POPUP_MARGIN_X	20
#define		MAIN_POPUP_MARGIN_Y	55

#define PDMain		(1L<<0)
#define PDAttributes	(1L<<1)
#define PDDraw		(1L<<2)
#define PDGroup		(1L<<3)
#define PDPriority	(1L<<4)
#define PDRotate	(1L<<5)
#define PDWrite		(1L<<6)
#define PDRead		(1L<<7)
#define PDImage		(1L<<8)
#define PDAll		PDMain|PDAttributes|PDDraw|PDGroup|PDPriority|\
			PDRotate|PDWrite|PDRead|PDImage
#define PDSubAll	PDAttributes|PDDraw|PDGroup|PDPriority|\
			PDRotate|PDWrite|PDRead|PDImage

typedef  struct {
	String  bits  ;
	int     width  ;
	int     height  ;
}  _MapTable  ;

/************************************
 *
 *  Class structure
 *
 ***********************************/

   /* New fields for the StPicture widget class record */
typedef struct _StPictureClass 
  {
    int makes_compiler_happy;  /* not used */
  } StPictureClassPart;

   /* Full class record declaration */
typedef struct _StPictureClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    StPictureClassPart	stPicture_class;
} StPictureClassRec;

extern StPictureClassRec stPictureClassRec;

/***************************************
 *
 *  Instance (widget) structure 
 *
 **************************************/

    /* New fields for the StPicture widget record */
typedef struct {
    /* resources */
	String		file_name  ;
	Pixel		foreground  ;
	XFontStruct	*font  ;
	XtCallbackList	*after  ;
	XtCallbackList	*after_draw  ;
	XtCallbackList	*before  ;
	XtCallbackList	*before_draw  ;
	XtCallbackList	temp_callback  ;/* temporary List for XtCallCallbacks */
	String		*entries  ;
	String		*entries_draw  ;
	StPictureFunction *functions  ;
	StDrawFunction	*functions_draw  ;
	String		title  ;
	String		title_draw  ;
	Position	coor_xmax  ;
	Position	coor_xmin  ;
	Position	coor_ymax  ;
	Position	coor_ymin  ;
	Position	view_x  ;
	Position	view_y  ;
	Boolean		forcebars  ;
	Boolean		usebottom  ;
	Boolean		useright  ;
	Boolean		protrusion  ;

    /* private state */
	StDrawControl	*dt  ;		/* ptr to drawing-control table       */
	Position	orgin_x  ;	/* revised 			      */
	Position	orgin_y  ;	/* revised 			      */
	int		view_w  ;	/* revised 			      */
	int		view_h  ;	/* revised 			      */

	StPictureFunction  mode  ;	/* picture-mode -> set in popup&s-btn1*/
					/*               clear in notify-after*/
	StDrawFunction	mode_draw  ;	/* draw-mode    -> set in popup       */
					/*               clear in notify-after*/
	Boolean		mode_continue  ;/* True:mode continue , False:end     */
	String		write_file  ;	/* file_name of write		      */
	String		read_file  ;	/* file name of read                  */
	String		image_file  ;	/* file name of image                 */
	Boolean		group_flg  ;	/* True:set    , False:reset          */
	Boolean		priority_flg  ;	/* True:raise  , False:lower          */
	Boolean		rotate_flg  ;	/* 0:+90 , 1:+180 , 2:+270	      */
	Boolean		read_flg  ;	/* True:merge  , False:override       */

	Widget		sbver  ;	/* Vertical scrollbar		      */
	Widget		sbhor  ;	/* horizontal  scrollbar	      */
	Widget		outofview  ;	/* out of View-Area		      */
	Widget		logo1  ;	/* out of View-Area		      */
	Widget		logo2  ;	/* out of View-Area		      */

	String		*entries_trans ;/* new translation for main-menu-entry*/
	int		num_entries  ;	/* number of main-menu entries	      */
	int		maked_entries  ;/* number of maked main-menu entries  */
	Widget		popup  ;	/* main-popup shell		      */
	Widget		box  ;		/* main-popup box  		      */
	Widget		dummy  ;	/* dummy for MenuBox Widget	      */
	Widget		*item  ;	/* item of popup		      */
	Widget		lab  ;		/* label of main-popup		      */
	Widget		callback_item  ;/* for Before & After Callback	      */
	Widget		callback_item_draw;/* for Before & After Callback     */
	XtIntervalId	timeid  ;	/* timeid of Subpopup		      */
	XtIntervalId	timeid_third  ;	/* timeid of Thirdpopup		      */

	String		*entries_trans_draw;/* new translation for draw-menu-e*/
	int		num_entries_draw;/* number of draw-menu entries	      */
	int		maked_entries_draw;/* number of maked draw-menu entrie*/
	Widget		popup_draw  ;	/* draw-popup shell		      */
	Widget		box_draw  ;	/* draw-popup box  		      */
	Widget		dummy_draw  ;	/* dummy for MenuBox Widget	      */
	Widget		*item_draw  ;	/* item of draw-popup		      */
	Widget		lab_draw  ;	/* label of draw-popup		      */
	Boolean		drawpopflg  ;	/* True:pop continue  , False:unpop   */

	/* Attributes */
	String		*entries_trans_attributes;/* new translation for attri*/
	Widget		popup_attributes;/* attributes-popup shell	      */
	Widget		box_attributes;	/* attributes-popup box 	      */
	Widget		item_attributes[3];/* item of attributes-popup	      */
	Boolean		attributespopflg;/* True:pop continue  , False:unpop  */
	Pixmap		localmap  ;	/* temporary Pixmap		      */

	Widget		popup_line  ;	/* line-popup shell		      */
	Widget		form_line  ;	/* line-popup form 		      */
	Widget		list_line_type;	/* line-popup type_list		      */
	Widget		begin_line_width;/*min-value		      	      */
	Widget		scr_line_width;	/* line-popup width_scr		      */
	Widget		end_line_width;	/* max-value			      */
	Widget		real_line_width;/* real value			      */
	Widget		begin_cap_name;	/* name of cap1			      */
	Widget		end_cap_name;	/* name of cap2			      */
	Widget		list_line_cap1;	/* line-popup begin cap_list	      */
	Widget		list_line_cap2;	/* line-popup end   cap_list	      */
	Widget		list_line_join;	/* line-popup join_list		      */
	Widget		form_line_color1;/* line-popup color_form	      */
	Widget		line_color_item[16];/* item of color		      */
	Widget		line_color_name;/* real color			      */
	Widget		dash_back_name ;/* DobleDash background		      */
	Widget		form_line_color2;/* line-popup color_form_doubledash  */
	Widget		line_color_item2[16];/* item of color_doubledash      */
	Widget		line_color_name2;/* real color_doubledash	      */
	Widget		list_line_function;/* line-popup function_list	      */
	Widget		line_ok  ;	/* line-popup popdown command	      */
	Boolean		linepopflg  ;	/* True:pop continue  , False:unpop   */
	Boolean		init_line  ;	/* True:already initialize , False:not*/

	Widget		popup_fill  ;	/* fill-popup shell		      */
	Widget		form_fill  ;	/* fill-popup form 		      */
	Widget		form_fill_tile;	/* fill-popup tile_form		      */
	Widget		fill_tile_item[14];/* item of tile		      */
	Widget		form_fill_color1;/* fill-popup color_form	      */
	Widget		form_fill_color2;/* fill-popup color_form	      */
	Widget		fill_color_item[16];/* item of color		      */
	Widget		fill_color_item2[16];/* item of color		      */
	Widget		fore_color_name;/* foreground label		      */
	Widget		back_color_name;/* background label		      */
	Widget		fill_tile_name;/* real tile			      */
	Widget		fill_ok  ;	/* fill-popup popdown command	      */
	Boolean		fillpopflg  ;	/* True:pop continue  , False:unpop   */
	Boolean		init_fill  ;	/* True:already initialize , False:not*/

	Widget		popup_char  ;	/* char-popup shell		      */
	Widget		form_char  ;	/* char-popup form 		      */
	Widget		list_char_kind;	/* char-popup kind_list		      */
	Widget		list_char_size;	/* char-popup size_list		      */
	Widget		form_char_color1;/* char-popup color_form	      */
	Widget		char_color_item[16];/* item of color		      */
	Widget		char_color_name;/* color name			      */
	Widget		list_char_function;/* char-popup function_list	      */
	Widget		char_ok  ;	/* char-popup popdown command	      */
	Boolean		charpopflg  ;	/* True:pop continue  , False:unpop   */
	Boolean		init_char  ;	/* True:already initialize , False:not*/
	/* Attributes End */

	String		*entries_trans_group;/* new translation for group-menu*/
	Widget		popup_group  ;	/* group-popup shell		      */
	Widget		box_group  ;	/* group-popup box  		      */
	Widget		item_group[2] ;	/* item of group-popup		      */
	Boolean		grouppopflg  ;	/* True:pop continue  , False:unpop   */

	String		*entries_trans_priority;/* new translation for priorit*/
	Widget		popup_priority ;/* priority-popup shell		      */
	Widget		box_priority  ;	/* priority-popup box  		      */
	Widget		item_priority[2] ;/* item of priority-popup	      */
	Boolean		prioritypopflg ;/* True:pop continue  , False:unpop   */

	String		*entries_trans_rotate;/* new translation for rotate   */
	Widget		popup_rotate  ;	/* rotate-popup shell		      */
	Widget		box_rotate  ;	/* rotate-popup box  		      */
	Widget		item_rotate[3] ;/* item of rotate-popup		      */
	Boolean		rotatepopflg  ;	/* True:pop continue  , False:unpop   */

	String		*entries_trans_write;/* new translation for write-menu*/
	Widget		popup_write ;	/* write-popup shell		      */
	Widget		box_write  ;	/* write-popup box  		      */
	Widget		item_write ;	/* item of write-popup		      */
	Boolean		writepopflg ;	/* True:pop continue  , False:unpop   */

	String		*entries_trans_read; /* new translation for read-menu-*/
	Widget		popup_read ;	/* read-popup shell		      */
	Widget		box_read  ;	/* read-popup box  		      */
	Widget		item_read[2] ;	/* item of read-popup		      */
	Boolean		readpopflg ;	/* True:pop continue  , False:unpop   */

	String		*entries_trans_image;/* new translation for image-menu*/
	Widget		popup_image ;	/* image-popup shell		      */
	Widget		box_image  ;	/* image-popup box  		      */
	Widget		item_image ;	/* item of image-popup		      */
	Boolean		imagepopflg ;	/* True:pop continue  , False:unpop   */

	Widget		real_parent_draw;/* real parent of draw-pop	      */
	Widget		real_parent_attributes;/* real parent of attributes-po*/
	Widget		real_parent_group;/* real parent of group-pop	      */
	Widget		real_parent_priority;/* real parent of priority-pop   */
	Widget		real_parent_rotate;/* real parent of rotate-pop	      */
	Widget		real_parent_write;/* real parent of write-pop	      */
	Widget		real_parent_read;/* real parent of read-pop	      */
	Widget		real_parent_image;/* real parent of image-pop	      */

	String		*widthtable  ;	/* ptr to WidthTable		      */
	String		*linetypetable;	/* ptr to LineTypeTable		      */
	String		*linecaptable ;	/* ptr to LineCapTable		      */
	String		*linejointable;	/* ptr to LineJoinTable		      */
	String		*colortable1  ;	/* ptr to ColorTable1		      */
	String		*colortable2  ;	/* ptr to ColorTable2		      */
	Pixel		*realcolor  ;	/* ptr to RealColor		      */
	String		*functiontable;	/* ptr to FunctionTable		      */
	String		*charkindtable;	/* ptr to CharKindTable		      */
	String		*charsizetable;	/* ptr to CharSizeTable		      */
	_MapTable	*maptable  ;	/* ptr to MapTable		      */
} StPicturePart;

   /* Full widget declaration */
typedef struct _StPictureRec {
    CorePart         core;
    SimplePart	     simple;
    StPicturePart    stPicture;
} StPictureRec;


#endif _StPictureP_h
