#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)txt_menu.c 20.58 89/11/02";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Text subwindow menu creation and support.
 */

#include <xview_private/primal.h>
#include <xview/textsw.h>
#include <xview_private/txt_impl.h>
#include <xview_private/ev_impl.h>
#include <errno.h>
#include <pixrect/pr_util.h>
#include <pixrect/memvar.h>
#include <pixrect/pixfont.h>
#include <xview/defaults.h>
#include <xview/win_input.h>
#include <xview/win_struct.h>
#include <xview/fullscreen.h>
#include <xview/notice.h>
#include <xview/frame.h>
#include <xview/openmenu.h>
#include <xview/window.h>
#include <xview/cursor.h>
#include <xview/screen.h>
#include <xview/svrimage.h>

#define HELP_INFO(s) XV_HELP_DATA, s,

Pkg_private Es_index ev_index_for_line();


#define			MAX_SIZE_MENU_STRING	30


typedef enum {
    TEXTSWMENU,
    TERMSWMENU
}               Menu_type;

typedef struct local_menu_object {
    int             refcount;	/* refcount for textsw_menu */
    Menu            menu;	/* Let default to NULL */
    Menu           *sub_menus;	/* Array of the sub menu handles */
    Menu_item      *menu_items /* [TEXTSW_MENU_LAST_CMD] */ ;
}               Local_menu_object;


static int      textsw_menu_refcount;	/* refcount for textsw_menu */
static Menu     textsw_menu;	/* Let default to NULL */
static Menu    *textsw_sub_menus;	/* Array of the sub menu handles */
static Menu_item *textsw_menu_items /* [TEXTSW_MENU_LAST_CMD] */ ;
static Menu    *textsw_file_menu;
static short    set_def  = FALSE;

extern int      textsw_has_been_modified();
pkg_private void textsw_get_extend_to_edge();
pkg_private void textsw_set_extend_to_edge();
Pkg_private Textsw_view_handle textsw_create_view();
int             textsw_file_do_menu_action();

extern Menu     textsw_menu_init();
extern void     textsw_do_menu();
extern Menu     textsw_get_unique_menu();
extern Menu_item textsw_extras_gen_proc();
extern void     termsw_menu_clr(), termsw_menu_set();

int             STORE_FILE_POPUP_KEY;
int             LOAD_FILE_POPUP_KEY;
int             FILE_STUFF_POPUP_KEY;
int             SEARCH_POPUP_KEY;
int             MATCH_POPUP_KEY;
int             SEL_LINE_POPUP_KEY;
int             EXTRASMENU_FILENAME_KEY;
int             TEXTSW_MENU_DATA_KEY;


/* Menu strings for File sub menu */
#define	SAVE_FILE	"Save Current File"
#define	STORE_NEW_FILE	"Store as New File..."
#define	LOAD_FILE	"Load File..."
#define	INCLUE_FILE	"Include File..."
#define	EMPTY_DOC	"Empty Document"

/* Menu strings for Edit sub menu */
#define	AGAIN_STR	"Again"
#define	UNDO_STR	"Undo"
#define	COPY_STR	"Copy"
#define	PASTE_STR	"Paste"
#define	CUT_STR		"Cut"

/* Menu strings for View sub menu */
#define	SEL_LINE_AT_NUM		"Select Line at Number..."
#define	WHAT_LINE_NUM		"What Line Number?"
#define	SHOW_CARET_AT_TOP	"Show Caret at Top"
#define	CHANGE_LINE_WRAP	"Change Line Wrap"

/* Menu strings for Find sub menu */
#define	FIND_REPLACE		"Find and Replace..."
#define	FIND_SELECTION		"Find Selection"
#define	FIND_MARKED_TEXT	"Find Marked Text..."
#define	REPLACE_FIELD		"Replace |>field<| "

static Defaults_pairs line_break_pairs[] = {
    "TEXTSW_CLIP", (int) TEXTSW_CLIP,
    "TEXTSW_WRAP_AT_CHAR", (int) TEXTSW_WRAP_AT_CHAR,
    "TEXTSW_WRAP_AT_WORD", (int) TEXTSW_WRAP_AT_WORD,
    NULL, (int) TEXTSW_WRAP_AT_WORD
};



/* VARARGS0 */
static void
textsw_new_menu(folio)
    Textsw_folio    folio;
{
    register Menu  *sub_menu;
    register Menu_item *menu_items;
    Menu            undo_cmds, break_mode, find_sel_cmds, select_field_cmds, top_menu;
    Textsw          textsw = FOLIO_REP_TO_ABS(folio);
    Frame           frame = xv_get(textsw, WIN_FRAME);
    Menu_item       break_mode_item, undo_cmds_item, find_sel_cmds_item,
                    select_field_cmds_item;
    static int      textsw_edit_do_menu_action(), textsw_view_do_menu_action(),
                    textsw_find_do_menu_action();
    int             index;
    PIXFONT        *font;	/* laf */
    Pkg_private char *textsw_get_extras_filename();
    Pkg_private int textsw_build_extras_menu_items();
    char           *filename;
    char           *def_str;
    int             line_break;

    STORE_FILE_POPUP_KEY = xv_unique_key();
    LOAD_FILE_POPUP_KEY = xv_unique_key();
    FILE_STUFF_POPUP_KEY = xv_unique_key();
    SEARCH_POPUP_KEY = xv_unique_key();
    MATCH_POPUP_KEY = xv_unique_key();
    SEL_LINE_POPUP_KEY = xv_unique_key();
    EXTRASMENU_FILENAME_KEY = xv_unique_key();
    TEXTSW_MENU_DATA_KEY = xv_unique_key();

    menu_items = (Menu_item *) malloc((unsigned) TEXTSW_MENU_LAST_CMD *
				      sizeof(Menu_item));
    sub_menu = (Menu *) malloc(((unsigned) TXTSW_EXTRAS_SUB_MENU + 1) * (sizeof(Menu)));

    break_mode = xv_create(NULL, MENU_CHOICE_MENU,
			   MENU_LEFT_MARGIN, 6,
			   HELP_INFO("textsw:mbreakmode")
			   0);
    menu_items[(int) TEXTSW_MENU_WRAP_LINES_AT_WORD] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Wrap at Word",
		  MENU_VALUE, TEXTSW_MENU_WRAP_LINES_AT_WORD,
		  HELP_INFO("textsw:mwrapwords")
		  0);
    menu_items[(int) TEXTSW_MENU_WRAP_LINES_AT_CHAR] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Wrap at Character",
		  MENU_VALUE, TEXTSW_MENU_WRAP_LINES_AT_CHAR,
		  HELP_INFO("textsw:mwrapchars")
		  0);
    menu_items[(int) TEXTSW_MENU_CLIP_LINES] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Wrap at Character",
		  MENU_STRING, "Clip Lines",
		  MENU_VALUE, TEXTSW_MENU_CLIP_LINES,
		  HELP_INFO("textsw:mcliplines")
		  0);
    def_str = defaults_get_string("text.lineBreak", "Text.LineBreak", (char *) 0);
    if (def_str == NULL || def_str[0] == '\0' ||
	(line_break = (int) defaults_lookup(def_str, line_break_pairs)) == TEXTSW_WRAP_AT_WORD)
	xv_set(break_mode,
	 MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_WRAP_LINES_AT_WORD],
	 MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_WRAP_LINES_AT_CHAR],
	       MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_CLIP_LINES],
	       0);
    else if (TEXTSW_WRAP_AT_CHAR == line_break)
	xv_set(break_mode,
	 MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_WRAP_LINES_AT_CHAR],
	 MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_WRAP_LINES_AT_WORD],
	       MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_CLIP_LINES],
	       0);
    else
	xv_set(break_mode,
	       MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_CLIP_LINES],
	 MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_WRAP_LINES_AT_CHAR],
	 MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_WRAP_LINES_AT_WORD],
	       0);

    undo_cmds = xv_create(NULL, MENU,
			  MENU_LEFT_MARGIN, 6,
			  HELP_INFO("textsw:mundocmds")
			  0);
    menu_items[(int) TEXTSW_MENU_UNDO] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Undo Last Edit",
		  MENU_VALUE, TEXTSW_MENU_UNDO,
		  HELP_INFO("textsw:mundolast")
		  0);
    menu_items[(int) TEXTSW_MENU_UNDO_ALL] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Undo All Edits",
		  MENU_VALUE, TEXTSW_MENU_UNDO_ALL,
		  HELP_INFO("textsw:mundoall")
		  0);
    xv_set(undo_cmds,
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_UNDO],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_UNDO_ALL],
	   0);
    xv_set(undo_cmds, XV_KEY_DATA, TEXTSW_MENU_DATA_KEY, textsw, 0);

    select_field_cmds = xv_create(NULL, MENU,
				  MENU_LEFT_MARGIN, 6,
				  HELP_INFO("textsw:mselfieldcmds")
				  0);
    menu_items[(int) TEXTSW_MENU_SEL_ENCLOSE_FIELD] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Expand",
		  MENU_VALUE, TEXTSW_MENU_SEL_ENCLOSE_FIELD,
		  HELP_INFO("textsw:mselexpand")
		  0);
    menu_items[(int) TEXTSW_MENU_SEL_NEXT_FIELD] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Next",
		  MENU_VALUE, TEXTSW_MENU_SEL_NEXT_FIELD,
		  HELP_INFO("textsw:mselnext")
		  0);
    menu_items[(int) TEXTSW_MENU_SEL_PREV_FIELD] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Previous",
		  MENU_VALUE, TEXTSW_MENU_SEL_PREV_FIELD,
		  HELP_INFO("textsw:mselprevious")
		  0);
    xv_set(select_field_cmds,
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_SEL_ENCLOSE_FIELD],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_SEL_NEXT_FIELD],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_SEL_PREV_FIELD],
	   0);


    find_sel_cmds = xv_create(NULL, MENU,
			      MENU_LEFT_MARGIN, 6,
			      HELP_INFO("textsw:mfindselcmds")
			      0);
    menu_items[(int) TEXTSW_MENU_FIND] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Forward",
		  MENU_VALUE, TEXTSW_MENU_FIND,
		  HELP_INFO("textsw:mfindforward")
		  0);
    menu_items[(int) TEXTSW_MENU_FIND_BACKWARD] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, "Backward",
		  MENU_VALUE, TEXTSW_MENU_FIND_BACKWARD,
		  HELP_INFO("textsw:mfindbackward")
		  0);
    xv_set(find_sel_cmds,
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_FIND],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_FIND_BACKWARD],
	   0);

    menu_items[(int) TEXTSW_MENU_LOAD] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, LOAD_FILE,
		  MENU_VALUE, TEXTSW_MENU_LOAD,
		  HELP_INFO("textsw:mloadfile")
		  0);
    menu_items[(int) TEXTSW_MENU_SAVE] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, SAVE_FILE,
		  MENU_VALUE, TEXTSW_MENU_SAVE,
		  HELP_INFO("textsw:msavefile")
		  0);
    menu_items[(int) TEXTSW_MENU_STORE] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, STORE_NEW_FILE,
		  MENU_VALUE, TEXTSW_MENU_STORE,
    /* MENU_LINE_AFTER_ITEM,	MENU_HORIZONTAL_LINE, */
		  HELP_INFO("textsw:mstorefile")
		  0);
    menu_items[(int) TEXTSW_MENU_FILE_STUFF] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, INCLUE_FILE,
		  MENU_VALUE, TEXTSW_MENU_FILE_STUFF,
		  HELP_INFO("textsw:mincludefile")
		  0);
    menu_items[(int) TEXTSW_MENU_RESET] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, EMPTY_DOC,
		  MENU_VALUE, TEXTSW_MENU_RESET,
		  HELP_INFO("textsw:memptydoc")
		  0);

    sub_menu[(int) TXTSW_FILE_SUB_MENU] = xv_create(NULL, MENU,
						    MENU_LEFT_MARGIN, 6,
					       HELP_INFO("textsw:mfilecmds")
						    0);

    xv_set(sub_menu[(int) TXTSW_FILE_SUB_MENU],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_LOAD],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_SAVE],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_STORE],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_FILE_STUFF],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_RESET],
	   0);

    menu_items[(int) TEXTSW_MENU_AGAIN] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, AGAIN_STR,
		  MENU_VALUE, TEXTSW_MENU_AGAIN,
		  HELP_INFO("textsw:meditagain")
		  0);
    undo_cmds_item = xv_create(NULL,
			       MENUITEM,
			       MENU_STRING, UNDO_STR,
			       MENU_PULLRIGHT, undo_cmds,
    /* MENU_LINE_AFTER_ITEM,	 MENU_HORIZONTAL_LINE, */
			       HELP_INFO("textsw:meditundo")
			       0);
    menu_items[(int) TEXTSW_MENU_COPY] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, COPY_STR,
		  MENU_VALUE, TEXTSW_MENU_COPY,
		  HELP_INFO("textsw:meditcopy")
		  0);
    menu_items[(int) TEXTSW_MENU_PASTE] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, PASTE_STR,
		  MENU_VALUE, TEXTSW_MENU_PASTE,
		  HELP_INFO("textsw:meditpaste")
		  0);
    menu_items[(int) TEXTSW_MENU_CUT] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, CUT_STR,
		  MENU_VALUE, TEXTSW_MENU_CUT,
    /* MENU_LINE_AFTER_ITEM,	MENU_HORIZONTAL_LINE, */
		  HELP_INFO("textsw:meditcut")
		  0);
    sub_menu[(int) TXTSW_EDIT_SUB_MENU] = xv_create(NULL, MENU,
						    MENU_LEFT_MARGIN, 6,
					       HELP_INFO("textsw:meditcmds")
						    0);
    xv_set(sub_menu[(int) TXTSW_EDIT_SUB_MENU],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_AGAIN],
	   MENU_APPEND_ITEM, undo_cmds_item,
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_COPY],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_PASTE],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_CUT],
	   0);


    menu_items[(int) TEXTSW_MENU_NORMALIZE_LINE] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, SEL_LINE_AT_NUM,
		  MENU_VALUE, TEXTSW_MENU_NORMALIZE_LINE,
		  HELP_INFO("textsw:mselectline")
		  0);
    menu_items[(int) TEXTSW_MENU_COUNT_TO_LINE] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, WHAT_LINE_NUM,
		  MENU_VALUE, TEXTSW_MENU_COUNT_TO_LINE,
    /* MENU_LINE_AFTER_ITEM,	MENU_HORIZONTAL_LINE, */
		  HELP_INFO("textsw:mwhatline")
		  0);
    menu_items[(int) TEXTSW_MENU_NORMALIZE_INSERTION] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, SHOW_CARET_AT_TOP,
		  MENU_VALUE, TEXTSW_MENU_NORMALIZE_INSERTION,
		  HELP_INFO("textsw:mshowcaret")
		  0);
    break_mode_item = xv_create(NULL,
				MENUITEM,
				MENU_STRING, CHANGE_LINE_WRAP,
				MENU_PULLRIGHT, break_mode,
				HELP_INFO("textsw:mchangelinewrap")
				0);

    sub_menu[(int) TXTSW_VIEW_SUB_MENU] = xv_create(NULL, MENU,
						    MENU_LEFT_MARGIN, 6,
					    HELP_INFO("textsw:mdisplaycmds")
						    0);
    xv_set(sub_menu[(int) TXTSW_VIEW_SUB_MENU],
	   MENU_CLIENT_DATA, textsw,
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_NORMALIZE_LINE],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_COUNT_TO_LINE],
	MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_NORMALIZE_INSERTION],
	   MENU_APPEND_ITEM, break_mode_item,
	   0);

    menu_items[(int) TEXTSW_MENU_FIND_AND_REPLACE] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, FIND_REPLACE,
		  MENU_VALUE, TEXTSW_MENU_FIND_AND_REPLACE,
    /* MENU_LINE_AFTER_ITEM,	MENU_HORIZONTAL_LINE, */
		  HELP_INFO("textsw:mfindreplace")
		  0);
    find_sel_cmds_item = xv_create(NULL,
				   MENUITEM,
				   MENU_STRING, FIND_SELECTION,
				   MENU_PULLRIGHT, find_sel_cmds,
				   HELP_INFO("textsw:mfindselcmds")
				   0);
    menu_items[(int) TEXTSW_MENU_SEL_MARK_TEXT] =
	xv_create(NULL,
		  MENUITEM,
		  MENU_STRING, FIND_MARKED_TEXT,
		  MENU_VALUE, TEXTSW_MENU_SEL_MARK_TEXT,
		  HELP_INFO("textsw:mfindtext")
		  0);
    select_field_cmds_item = xv_create(NULL,
				       MENUITEM,
				       MENU_STRING, REPLACE_FIELD,
				       MENU_PULLRIGHT, select_field_cmds,
				       HELP_INFO("textsw:mselfieldcmds")
				       0);

    sub_menu[(int) TXTSW_FIND_SUB_MENU] = xv_create(NULL, MENU,
						    MENU_LEFT_MARGIN, 6,
					       HELP_INFO("textsw:mfindcmds")
						    0);
    xv_set(sub_menu[(int) TXTSW_FIND_SUB_MENU],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_FIND_AND_REPLACE],
	   MENU_APPEND_ITEM, find_sel_cmds_item,
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_SEL_MARK_TEXT],
	   MENU_APPEND_ITEM, select_field_cmds_item,
	   0);

    sub_menu[(int) TXTSW_EXTRAS_SUB_MENU] = xv_create(NULL, MENU,
						      MENU_LEFT_MARGIN, 6,
					      HELP_INFO("textsw:extrasmenu")
						      0);


    top_menu = xv_create(NULL, MENU,
			 MENU_LEFT_MARGIN, 6,
			 MENU_TITLE_ITEM, "Text Pane",
			 HELP_INFO("textsw:mtopmenu")
			 0);
    menu_items[(int) TEXTSW_MENU_FILE_CMDS] = xv_create(NULL,
							MENUITEM,
							MENU_STRING, "File",
			MENU_PULLRIGHT, sub_menu[(int) TXTSW_FILE_SUB_MENU],
					       HELP_INFO("textsw:mfilecmds")
							0);


    menu_items[(int) TEXTSW_MENU_VIEW_CMDS] = xv_create(NULL,
							MENUITEM,
							MENU_STRING, "View",
			MENU_PULLRIGHT, sub_menu[(int) TXTSW_VIEW_SUB_MENU],
					    HELP_INFO("textsw:mdisplaycmds")
							0);
    menu_items[(int) TEXTSW_MENU_EDIT_CMDS] = xv_create(NULL,
							MENUITEM,
							MENU_STRING, "Edit",
			MENU_PULLRIGHT, sub_menu[(int) TXTSW_EDIT_SUB_MENU],
					       HELP_INFO("textsw:meditcmds")
							0);
    menu_items[(int) TEXTSW_MENU_FIND_CMDS] = xv_create(NULL,
							MENUITEM,
							MENU_STRING, "Find",
			MENU_PULLRIGHT, sub_menu[(int) TXTSW_FIND_SUB_MENU],
					       HELP_INFO("textsw:mfindcmds")
							0);
    menu_items[(int) TEXTSW_MENU_EXTRAS_CMDS] = xv_create(NULL,
							  MENUITEM,
				      MENU_GEN_PROC, textsw_extras_gen_proc,
		      MENU_PULLRIGHT, sub_menu[(int) TXTSW_EXTRAS_SUB_MENU],
						      MENU_STRING, "Extras",
						   MENU_CLIENT_DATA, textsw,
						 HELP_INFO("textsw:mextras")
							  0);

    textsw_file_menu = (Menu *)menu_items[(int) TEXTSW_MENU_FILE_CMDS];

    filename = textsw_get_extras_filename(menu_items[(int) TEXTSW_MENU_EXTRAS_CMDS]);
    (void) textsw_build_extras_menu_items(textsw, filename, sub_menu[(int) TXTSW_EXTRAS_SUB_MENU]);

    xv_set(top_menu,
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_FILE_CMDS],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_VIEW_CMDS],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_EDIT_CMDS],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_FIND_CMDS],
	   MENU_APPEND_ITEM, menu_items[(int) TEXTSW_MENU_EXTRAS_CMDS],
	   0);

    for (index = (int) TEXTSW_MENU_LOAD;
	 index <= (int) TEXTSW_MENU_RESET;
	 index++) {
	if (menu_items[index]) {
	    menu_set(menu_items[index],
		     MENU_ACTION, textsw_file_do_menu_action,
		     0);
	}
    }
    for (index = (int) TEXTSW_MENU_AGAIN;
	 index <= (int) TEXTSW_MENU_CUT;
	 index++) {
	if (menu_items[index]) {
	    menu_set(menu_items[index],
		     MENU_ACTION, textsw_edit_do_menu_action,
		     0);
	}
    }
    for (index = (int) TEXTSW_MENU_NORMALIZE_LINE;
	 index <= (int) TEXTSW_MENU_CLIP_LINES;
	 index++) {
	if (menu_items[index]) {
	    menu_set(menu_items[index],
		     MENU_ACTION, textsw_view_do_menu_action,
		     0);
	}
    }
    for (index = (int) TEXTSW_MENU_FIND_AND_REPLACE;
	 index <= (int) TEXTSW_MENU_SEL_PREV_FIELD;
	 index++) {
	if (menu_items[index]) {
	    menu_set(menu_items[index],
		     MENU_ACTION, textsw_find_do_menu_action,
		     0);
	}
    }


    xv_set(sub_menu[(int) TXTSW_EDIT_SUB_MENU], MENU_GEN_PIN_WINDOW, frame, "Edit",
	   0);

    folio->sub_menu_table = sub_menu;
    folio->menu_table = menu_items;

    font = (PIXFONT *) xv_get(textsw, WIN_FONT);
    menu_set(top_menu, MENU_FONT, font, 0);
    folio->menu = (caddr_t) top_menu;

}


extern          Menu
textsw_menu_init(folio)
    Textsw_folio    folio;
{

    if (textsw_menu != 0) {
	folio->menu = (caddr_t) textsw_menu;
	folio->menu_table = textsw_menu_items;
	folio->sub_menu_table = textsw_sub_menus;
    } else {
	(void) textsw_new_menu(folio);

	textsw_menu = (Menu) folio->menu;
	textsw_menu_items = folio->menu_table;
	textsw_sub_menus = folio->sub_menu_table;
	textsw_menu_refcount = 0;
    }
    textsw_menu_refcount++;
    return (Menu) folio->menu;
}

extern          Menu
textsw_get_unique_menu(folio)
    Textsw_folio    folio;
{
    if ((Menu) folio->menu == textsw_menu) {
	/* refcount == 1 ==> textsw is the only referencer */
	if (textsw_menu_refcount == 1) {
	    textsw_menu = 0;
	    textsw_menu_items = 0;
	    textsw_menu_refcount = 0;
	} else {
	    (void) textsw_new_menu(folio);
	    textsw_menu = (Menu) folio->menu;
	    textsw_menu_refcount--;
	}
    }
    return (Menu) folio->menu;
}

extern void
textsw_do_menu(view, ie)
    Textsw_view_handle view;
    Event          *ie;
{
    register Textsw_folio folio = FOLIO_FOR_VIEW(view);
    Textsw_view     textsw_view = VIEW_REP_TO_ABS(view);

    /* insure that there are no caret render race conditions */
    termsw_menu_set();

    xv_set(folio->menu, XV_KEY_DATA, TEXTSW_MENU_DATA_KEY,
	   textsw_view, MENU_DONE_PROC, termsw_menu_clr, 0);

    menu_show(folio->menu, textsw_view, ie, 0);

}

Pkg_private     Textsw_view
textsw_from_menu(menu)
    Menu            menu;
{
    Textsw_view     textsw_view = NULL;
    Menu            temp_menu;
    Menu_item       temp_item;

    while (menu) {
	temp_item = xv_get(menu, MENU_PARENT);
	if (temp_item) {
	    temp_menu = xv_get(temp_item, MENU_PARENT);
	    menu = temp_menu;
	} else {
	    textsw_view = (Textsw_view) xv_get(menu, XV_KEY_DATA,
					       TEXTSW_MENU_DATA_KEY);
	    break;
	}
    }
    return (textsw_view);
}

int
textsw_file_do_menu_action(cmd_menu, cmd_item)
    Menu            cmd_menu;
    Menu_item       cmd_item;
{
    Textsw          abstract;
    Textsw_view     textsw_view = textsw_from_menu(cmd_menu);
    register Textsw_view_handle view;
    register Textsw_folio textsw;
    Textsw_menu_cmd cmd = (Textsw_menu_cmd)
    menu_get(cmd_item, MENU_VALUE, 0);
    register Event *ie = (Event *)
    menu_get(cmd_menu, MENU_FIRST_EVENT, 0);
    register int    locx, locy;

    if AN_ERROR
	(textsw_view == 0)
	    return;

    view = VIEW_ABS_TO_REP(textsw_view);
    textsw = FOLIO_FOR_VIEW(view);
    abstract = TEXTSW_PUBLIC(textsw);

    if AN_ERROR
	(ie == 0) {
	locx = locy = 0;
    } else {
	locx = ie->ie_locx;
	locy = ie->ie_locy;
    }

    switch (cmd) {

      case TEXTSW_MENU_RESET:
	textsw_empty_document(abstract, ie);
	(void) xv_set( cmd_menu, MENU_DEFAULT, 1, 0 );
	break;

      case TEXTSW_MENU_LOAD:{
	    Frame           base_frame = (Frame) xv_get(abstract, WIN_FRAME);
	    Frame           popup = (Frame) xv_get(base_frame, XV_KEY_DATA,
						   LOAD_FILE_POPUP_KEY);

	    if (textsw->state & TXTSW_NO_LOAD) {
		(void) notice_prompt(
				     FRAME_FROM_FOLIO_OR_VIEW(view),
				     (Event *) 0,
				     NOTICE_MESSAGE_STRINGS,
				     "Illegal Operation.",
				     "Load File Has Been Disabled.",
				     0,
				     NOTICE_BUTTON_YES, "Continue",
				     NOTICE_TRIGGER, ACTION_STOP,
				     0);
		break;
	    }
	    if (popup) {
		(void) textsw_set_dir_str((int) TEXTSW_MENU_LOAD);
		(void) textsw_get_and_set_selection(popup, view, (int) TEXTSW_MENU_LOAD);
	    } else {
		(void) textsw_create_popup_frame(view, (int) TEXTSW_MENU_LOAD);
	    }
	    break;
	}

      case TEXTSW_MENU_SAVE:
	if (textsw_has_been_modified(abstract)) {
	    Es_handle       original;

	    original = (Es_handle) es_get(textsw->views->esh, ES_PS_ORIGINAL);
	    if ((TXTSW_IS_READ_ONLY(textsw)) ||
		(original == ES_NULL) ||
		((Es_enum) es_get(original, ES_TYPE) != ES_TYPE_FILE)) {

		(void) notice_prompt(
				     FRAME_FROM_FOLIO_OR_VIEW(view),
				     (Event *) 0,
				     NOTICE_MESSAGE_STRINGS,
				     "Unable to Save Current File.",
				     "You are not currently editing a file.",
			     "Use \"Store as New File\" to save your work.",
				     0,
				     NOTICE_BUTTON_YES, "Continue",
				     NOTICE_TRIGGER, ACTION_STOP,
				     0);
		break;		/* jcb */
	    }
	} else {
	    (void) notice_prompt(
				 FRAME_FROM_FOLIO_OR_VIEW(view),
				 (Event *) 0,
				 NOTICE_MESSAGE_STRINGS,
				 "File has not been modified.",
				 "Save File operation ignored.",
				 0,
				 NOTICE_BUTTON_YES, "Continue",
				 NOTICE_TRIGGER, ACTION_STOP,
				 0);
	    break;
	}
	textsw_save(VIEW_REP_TO_ABS(view), locx, locy);
	break;

      case TEXTSW_MENU_STORE:{
	    Frame           base_frame = (Frame) xv_get(abstract, WIN_FRAME);
	    Frame           popup = (Frame) xv_get(base_frame, XV_KEY_DATA,
						   STORE_FILE_POPUP_KEY);
	    if (popup) {
		(void) textsw_set_dir_str((int) TEXTSW_MENU_STORE);
		(void) textsw_get_and_set_selection(popup, view, (int) TEXTSW_MENU_STORE);
	    } else {
		(void) textsw_create_popup_frame(view, (int) TEXTSW_MENU_STORE);
	    }
	    break;
	}

      case TEXTSW_MENU_FILE_STUFF:{
	    Frame           base_frame = (Frame) xv_get(abstract, WIN_FRAME);
	    Frame           popup = (Frame) xv_get(base_frame, XV_KEY_DATA,
						   FILE_STUFF_POPUP_KEY);
	    if (popup) {
		(void) textsw_set_dir_str((int) TEXTSW_MENU_FILE_STUFF);
		(void) textsw_get_and_set_selection(popup, view,
					      (int) TEXTSW_MENU_FILE_STUFF);
	    } else {
		(void) textsw_create_popup_frame(view, (int) TEXTSW_MENU_FILE_STUFF);
	    }
	    break;
	}

      default:
	break;
    }
}

static int
textsw_edit_do_menu_action(cmd_menu, cmd_item)
    Menu            cmd_menu;
    Menu_item       cmd_item;
{
    Textsw          abstract;
    Textsw_view     textsw_view = textsw_from_menu(cmd_menu);
    register Textsw_view_handle view;
    register Textsw_folio textsw;
    Textsw_menu_cmd cmd = (Textsw_menu_cmd)
    menu_get(cmd_item, MENU_VALUE, 0);
    register Event *ie = (Event *)
    menu_get(cmd_menu, MENU_FIRST_EVENT, 0);
    register int    locx, locy;

    if AN_ERROR
	(textsw_view == 0)
	    return;

    view = VIEW_ABS_TO_REP(textsw_view);
    textsw = FOLIO_FOR_VIEW(view);
    abstract = TEXTSW_PUBLIC(textsw);

    if AN_ERROR
	(ie == 0) {
	locx = locy = 0;
    } else {
	locx = ie->ie_locx;
	locy = ie->ie_locy;
    }

    switch (cmd) {

      case TEXTSW_MENU_AGAIN:
	textsw_again(view, locx, locy);
	break;

      case TEXTSW_MENU_UNDO:
	if (textsw_has_been_modified(abstract))
	    textsw_undo(textsw);
	break;

      case TEXTSW_MENU_UNDO_ALL:
	if (textsw_has_been_modified(abstract)) {
	    int             result;

	    result = notice_prompt(
				   xv_get(abstract, WIN_FRAME),
				   (Event *) 0,
				   NOTICE_MESSAGE_STRINGS,
			       "Undo All Edits will discard unsaved edits.",
				   "Please confirm.",
				   0,
				NOTICE_BUTTON_YES, "Confirm, discard edits",
				   NOTICE_BUTTON_NO, "Cancel",
				   NOTICE_NO_BEEPING, 1,
				   0);
	    if (result == NOTICE_YES)
		textsw_reset_2(abstract, locx, locy, TRUE, TRUE);
	}
	break;

      case TEXTSW_MENU_CUT:{
	    Es_index        first, last_plus_one;

	    (void) ev_get_selection(textsw->views,
				    &first, &last_plus_one, EV_SEL_PRIMARY);
	    if (first < last_plus_one)	/* Local primary selection */
		textsw_function_delete(view);
	    else {
		(void) notice_prompt(
				   xv_get(VIEW_REP_TO_ABS(view), WIN_FRAME),
				     (Event *) 0,
				     NOTICE_MESSAGE_STRINGS,
		    "Please make a primary selection in this textsw first.",
				     "Press \"Continue\" to proceed.",
				     0,
				     NOTICE_BUTTON_YES, "Continue",
				     NOTICE_TRIGGER, ACTION_STOP,
				     0);
	    }
	    break;
	}
      case TEXTSW_MENU_COPY:{
	    if (textsw_is_seln_nonzero(textsw, EV_SEL_PRIMARY))
		textsw_put(view);
	    else {
		(void) notice_prompt(
				   xv_get(VIEW_REP_TO_ABS(view), WIN_FRAME),
				     (Event *) 0,
				     NOTICE_MESSAGE_STRINGS,
				   "Please make a primary selection first.",
				     "Press \"Continue\" to proceed.",
				     0,
				     NOTICE_BUTTON_YES, "Continue",
				     NOTICE_TRIGGER, ACTION_STOP,
				     0);
	    }
	    break;
	}
      case TEXTSW_MENU_PASTE:{
	    if (textsw_is_seln_nonzero(textsw, EV_SEL_SHELF))
		textsw_function_get(view);
	    else {
		(void) notice_prompt(
				   xv_get(VIEW_REP_TO_ABS(view), WIN_FRAME),
				     (Event *) 0,
				     NOTICE_MESSAGE_STRINGS,
				     "Nothing on the clipboard to paste in.",
				     "Press \"Continue\" to proceed.",
				     0,
				     NOTICE_BUTTON_YES, "Continue",
				     NOTICE_TRIGGER, ACTION_STOP,
				     0);
	    }
	    break;
	}
      default:
	break;
    }
}

static int
textsw_view_do_menu_action(cmd_menu, cmd_item)
    Menu            cmd_menu;
    Menu_item       cmd_item;
{
    extern int      textsw_match_selection_and_normalize();
    Textsw          abstract;
    Textsw_view     textsw_view = textsw_from_menu(cmd_menu);
    register Textsw_view_handle view;
    register Textsw_folio textsw;
    Textsw_menu_cmd cmd = (Textsw_menu_cmd)
    menu_get(cmd_item, MENU_VALUE, 0);
    register Event *ie = (Event *)
    menu_get(cmd_menu, MENU_FIRST_EVENT, 0);
    register int    locx, locy;
    Es_index        first, last_plus_one;

     if AN_ERROR
	(textsw_view == 0)
	    return;

    view = VIEW_ABS_TO_REP(textsw_view);
    textsw = FOLIO_FOR_VIEW(view);
    abstract = TEXTSW_PUBLIC(textsw);

    if AN_ERROR
	(ie == 0) {
	locx = locy = 0;
    } else {
	locx = ie->ie_locx;
	locy = ie->ie_locy;
    }

    switch (cmd) {

      case TEXTSW_MENU_CLIP_LINES:
	xv_set(FOLIO_REP_TO_ABS(FOLIO_FOR_VIEW(view)),
	       TEXTSW_LINE_BREAK_ACTION, TEXTSW_CLIP,
	       0);
	break;

      case TEXTSW_MENU_WRAP_LINES_AT_CHAR:
	xv_set(FOLIO_REP_TO_ABS(FOLIO_FOR_VIEW(view)),
	       TEXTSW_LINE_BREAK_ACTION, TEXTSW_WRAP_AT_CHAR,
	       0);
	break;

      case TEXTSW_MENU_WRAP_LINES_AT_WORD:
	xv_set(FOLIO_REP_TO_ABS(FOLIO_FOR_VIEW(view)),
	       TEXTSW_LINE_BREAK_ACTION, TEXTSW_WRAP_AT_WORD,
	       0);
	break;

      case TEXTSW_MENU_NORMALIZE_INSERTION:{
	    Es_index        insert;
	    int             upper_context;
	    insert = EV_GET_INSERT(textsw->views);
	    if (insert != ES_INFINITY) {
		upper_context = (int)
		    ev_get(view->e_view, EV_CHAIN_UPPER_CONTEXT);
		textsw_normalize_internal(view, insert, insert, upper_context, 0,
					  TXTSW_NI_DEFAULT);
	    }
	    break;
	}

      case TEXTSW_MENU_NORMALIZE_LINE:{
	    Frame           base_frame = (Frame) xv_get(abstract, WIN_FRAME);
	    Frame           popup = (Frame) xv_get(base_frame, XV_KEY_DATA,
						   SEL_LINE_POPUP_KEY);
	    if (popup) {
		(void) textsw_get_and_set_selection(popup, view,
					  (int) TEXTSW_MENU_NORMALIZE_LINE);
	    } else {
		(void) textsw_create_popup_frame(view,
					  (int) TEXTSW_MENU_NORMALIZE_LINE);
	    }
	    break;
	}

      case TEXTSW_MENU_COUNT_TO_LINE:{
	    char            msg[200];
	    int             count;
	    if (!textsw_is_seln_nonzero(textsw, EV_SEL_PRIMARY)) {
		(void) notice_prompt(
				   xv_get(VIEW_REP_TO_ABS(view), WIN_FRAME),
				     (Event *) 0,
				     NOTICE_MESSAGE_STRINGS,
				   "Please make a primary selection first.",
				     "Press \"Continue\" to proceed.",
				     0,
				     NOTICE_BUTTON_YES, "Continue",
				     NOTICE_TRIGGER, ACTION_STOP,
				     0);
		break;
	    }
	    ev_get_selection(
		     textsw->views, &first, &last_plus_one, EV_SEL_PRIMARY);
	    if (first >= last_plus_one)
		break;
	    count = ev_newlines_in_esh(textsw->views->esh, 0, first);
	    (void) sprintf(msg, "Selection starts in line %d.", count + 1);
	    (void) notice_prompt(
				 xv_get(abstract, WIN_FRAME),
				 (Event *) 0,
				 NOTICE_NO_BEEPING, 1,
				 NOTICE_MESSAGE_STRINGS,
				 msg,
				 "Press \"Continue\" to proceed.",
				 0,
				 NOTICE_BUTTON_YES, "Continue",
				 NOTICE_TRIGGER, ACTION_STOP,
				 0);
	    break;
	}

      default:
	break;
    }
}
static int
textsw_find_do_menu_action(cmd_menu, cmd_item)
    Menu            cmd_menu;
    Menu_item       cmd_item;
{
    extern void     textsw_find_selection_and_normalize();
    Textsw          abstract;
    Textsw_view     textsw_view = textsw_from_menu(cmd_menu);
    register Textsw_view_handle view;
    register Textsw_folio textsw;
    Textsw_menu_cmd cmd = (Textsw_menu_cmd)
    menu_get(cmd_item, MENU_VALUE, 0);
    Event          *ie = (Event *)
    menu_get(cmd_menu, MENU_FIRST_EVENT, 0);
    register int    locx, locy;
    register long unsigned find_options = 0L;

     if AN_ERROR
	(textsw_view == 0)
	    return;

    view = VIEW_ABS_TO_REP(textsw_view);
    textsw = FOLIO_FOR_VIEW(view);
    abstract = TEXTSW_PUBLIC(textsw);

    if AN_ERROR
	(ie == 0) {
	locx = locy = 0;
    } else {
	locx = ie->ie_locx;
	locy = ie->ie_locy;
    }

    switch (cmd) {

      case TEXTSW_MENU_FIND_BACKWARD:
	find_options = TFSAN_BACKWARD;
	/* Fall through */
      case TEXTSW_MENU_FIND:
	find_options |= (EV_SEL_PRIMARY | TFSAN_SHELF_ALSO);
	if (textsw_is_seln_nonzero(textsw, find_options))
	    textsw_find_selection_and_normalize(view, locx, locy, find_options);
	else
	    window_bell(WINDOW_FROM_VIEW(view));
	break;

      case TEXTSW_MENU_SEL_ENCLOSE_FIELD:{
	    int             first, last_plus_one;

	    first = last_plus_one = EV_GET_INSERT(textsw->views);
	    (void) textsw_match_field_and_normalize(view, &first, &last_plus_one, "<|", 2, TEXTSW_FIELD_ENCLOSE, FALSE);
	    break;
	}
      case TEXTSW_MENU_SEL_NEXT_FIELD:
	(void) textsw_match_selection_and_normalize(view, "|>", TEXTSW_FIELD_FORWARD);
	break;

      case TEXTSW_MENU_SEL_PREV_FIELD:
	(void) textsw_match_selection_and_normalize(view, "<|", TEXTSW_FIELD_BACKWARD);
	break;

      case TEXTSW_MENU_SEL_MARK_TEXT:{
	    Frame           base_frame = (Frame) xv_get(abstract, WIN_FRAME);
	    Frame           popup = (Frame) xv_get(base_frame, XV_KEY_DATA,
						   MATCH_POPUP_KEY);
	    if (popup) {
		(void) xv_set(popup, XV_SHOW, TRUE,
			      WIN_CLIENT_DATA, view, 0);
	    } else {
		(void) textsw_create_popup_frame(view,
					   (int) TEXTSW_MENU_SEL_MARK_TEXT);
	    }
	    break;
	}

      case TEXTSW_MENU_FIND_AND_REPLACE:{
	    Frame           base_frame = (Frame) xv_get(abstract, WIN_FRAME);
	    Frame           popup = (Frame) xv_get(base_frame, XV_KEY_DATA,
						   SEARCH_POPUP_KEY);
	    if (popup) {
		(void) textsw_get_and_set_selection(popup, view,
					(int) TEXTSW_MENU_FIND_AND_REPLACE);
	    } else {
		(void) textsw_create_popup_frame(view,
					(int) TEXTSW_MENU_FIND_AND_REPLACE);
	    }
	    break;
	}

      default:
	break;
    }
}

Pkg_private void
textsw_set_extend_to_edge(view, height, width)
    Textsw_view_handle view;
    int             height, width;

{
    if (view) {
	if (height == WIN_EXTEND_TO_EDGE)
	    xv_set(VIEW_REP_TO_ABS(view),
		   WIN_DESIRED_HEIGHT, WIN_EXTEND_TO_EDGE,
		   0);
	if (width == WIN_EXTEND_TO_EDGE)
	    xv_set(VIEW_REP_TO_ABS(view),
		   WIN_DESIRED_WIDTH, WIN_EXTEND_TO_EDGE,
		   0);
    }
}

Pkg_private void
textsw_get_extend_to_edge(view, height, width)
    Textsw_view_handle view;
    int            *height, *width;

{
    *height = 0;
    *width = 0;

    if (view) {
	*height = (int) xv_get(VIEW_REP_TO_ABS(view), WIN_DESIRED_HEIGHT);
	*width = (int) xv_get(VIEW_REP_TO_ABS(view), WIN_DESIRED_WIDTH);
    }
}

/*
 * called after a file is loaded, this sets the menu default to save file
 */

Pkg_private void
textsw_set_file_menu_default_to_savefile()
{ 
  if( textsw_file_menu != NULL )
	(void) xv_set( textsw_file_menu, MENU_DEFAULT, 2, 0 );
  else
         set_def = TRUE;
}
