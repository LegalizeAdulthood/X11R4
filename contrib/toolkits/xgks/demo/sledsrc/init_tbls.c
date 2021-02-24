/*
 *              Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *  this file includes:
 *      init_bundle_tbls
 *      init_text_bundle_tbl
 *      init_char_hts_n_wds
 */

#include <xgks.h>

#include <stdio.h>
#include <stdlib.h>

#include "menu.h"
#include "popup.h"
#include "defs.h"
#include "func.h"
#include "key.h"
#include "color.h"
#include "draw.h"
#include "ws.h"
#include "screen_items.h"
#include "functbl.h"
#include "palette.h"
#include "add_objects.h"
#include "grid.h"
#include "add_obj_func.h"
#include "error.h"
#include "io.h"

void init_text_bundle_tbl(char *filename);
void init_char_hts_n_wds(char *filename, Gfloat **wd_tbl, Gfloat **ht_tbl);

/*
 * screen_tbl initializations
 */

int screen_tbl_sz = 14;
SCREEN_AREA_TBL_ENTRY screen_tbl[] =
{
        {
                /* primary menu area */
        
                PRI_MENU_AREA,          /* area */
                display_pri_menu,       /* draw function */
                menu_area_exec,         /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                .910,.0,1.0,0.970       /* viewport */
        },

        {
                /* secondary menu area */
        
                SEC_MENU_AREA,          /* area */
                display_sec_menu,       /* draw function */
                menu_area_exec,         /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                .815,.0,.905,0.970      /* viewport */
        },

        {
                /* message area */
        
                MSG_AREA,               /* area */
                display_msg_area,       /* draw function */
                NULL,                   /* no exec func 'cause
                                           not pickable */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.0,0.975,1.0,1.0       /* viewport */
        },

        {
                /* palette area */
        
                PAL_AREA,               /* area */
                display_palette,        /* draw function */
                palette_area_exec,      /* exec function */
                FALSE,                  /* active */
                0.0,0.0,31.5,3.5,       /* window */
                .315,0.0,.81,.10        /* viewport */
        },

        {
                /* picture area */
        
                PICT_AREA,              /* area */
                display_picture_area,   /* draw function */
                picture_area_exec,      /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.0,0.17,.81,.970       /* viewport */
        },

        {
                /* foreground clr area */
        
                FGDCLR_AREA,            /* area */
                display_fgdclr_area,    /* draw function */
                fgdclr_area_exec,       /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.0,0.0,.100,.10        /* viewport */
        },

        {
                /* background clr area */
        
                BGDCLR_AREA,            /* area */
                display_bgdclr_area,    /* draw function */
                bgdclr_area_exec,       /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.105,0.0,.205,.10      /* viewport */
        },

        {
                /* fill clr area */
        
                FILLCLR_AREA,           /* area */
                display_fillclr_area,   /* draw function */
                fillclr_area_exec,      /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.210,0.0,.310,.10      /* viewport */
        },

        {
                /* popup area */
        
                POPUP_AREA,             /* area */
                display_popup_area,     /* draw function */
                popup_area_exec,        /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.0,.105,.81,.165       /* viewport */
        },

        {
                /* color selector palette area */
        
                CLR_SELECTOR_PAL_AREA,  /* area */
                display_clr_selector_pal_area,  /* draw function */
                clr_selector_pal_area_exec,     /* exec function */
                FALSE,                  /* active */
                0.0,0.0,31.5,3.5,       /* window */
                .315,0.0,.81,.10        /* viewport */
        },

        {
                /* color picked area */
        
                CLR_PICKED_AREA,        /* area */
                display_clr_picked_area,        /* draw function */
                NULL,                   /* no exec func 'cause
                                           not pickable */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.105,0.0,.205,.10      /* viewport */
        },

        {
                /* color mixer area */
        
                MIXER_AREA,             /* area */
                display_clr_mixer_area, /* draw function */
                clr_mixer_area_exec,    /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.0,0.21,.81,.430       /* viewport */
        }, 

        {
                /* color wheel area */
        
                CLR_WHEEL_AREA,         /* area */
                display_clr_wheel_area, /* draw function */
                clr_wheel_area_exec,    /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.0,0.43,.30,.945       /* viewport */
        },

        {
                /* fine tuner area */
        
                TUNER_AREA,             /* area */
                display_fine_tuner_area,        /* draw function */
                fine_tuner_area_exec,   /* exec function */
                FALSE,                  /* active */
                0.0,0.0,1.0,1.0,        /* window */
                0.35,0.5,.81,.85        /* viewport */
        }
};


int popup_func_tbl_sz = 29;
struct popup_tbl_entry popup_func_tbl[] =
{
        {
                "verbose popup",        /* area */
                init_verbose_popup,     /* init function */
                switch_verbose_state,   /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "update popup",         /* area */
                init_update_popup,      /* init function */
                switch_update_state,    /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "grid popup",           /* area */
                init_grid_popup,        /* init function */
                switch_grid_state,      /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "grid color popup",     /* area */
                init_grid_color_popup,  /* init function */
                switch_grid_color_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "line connected popup", /* area */
                init_line_connected_popup,      /* init function */
                switch_line_connected_state,    /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "line width popup",     /* area */
                init_line_width_popup,  /* init function */
                switch_line_width_popup,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "line style popup",     /* area */
                init_line_style_popup,  /* init function */
                switch_line_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "box fill style popup", /* area */
                init_fill_style_popup,  /* init function */
                switch_fill_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "box line width popup", /* area */
                init_line_width_popup,  /* init function */
                switch_line_width_popup,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "box line style popup", /* area */
                init_line_style_popup,  /* init function */
                switch_line_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "polygon fill style popup",     /* area */
                init_fill_style_popup,  /* init function */
                switch_fill_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "polygon line width popup",     /* area */
                init_line_width_popup,  /* init function */
                switch_line_width_popup,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "polygon line style popup",     /* area */
                init_line_style_popup,  /* init function */
                switch_line_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "circle fill style popup",      /* area */
                init_fill_style_popup,  /* init function */
                switch_fill_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "circle line width popup",      /* area */
                init_line_width_popup,  /* init function */
                switch_line_width_popup,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "circle line style popup",      /* area */
                init_line_style_popup,  /* init function */
                switch_line_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "conic fill style popup",       /* area */
                init_fill_style_popup,  /* init function */
                switch_fill_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "conic line width popup",       /* area */
                init_line_width_popup,  /* init function */
                switch_line_width_popup,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "conic line style popup",       /* area */
                init_line_style_popup,  /* init function */
                switch_line_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "curve connected popup",        /* area */
                init_line_connected_popup,      /* init function */
                switch_curve_connected_state,   /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "curve width popup",    /* area */
                init_line_width_popup,  /* init function */
                switch_line_width_popup,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "curve style popup",    /* area */
                init_line_style_popup,  /* init function */
                switch_line_style_state,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "text style popup",     /* area */
                init_font_style_popup,  /* init function */
                switch_font_style_popup,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "text height popup",    /* area */
                init_char_height_popup, /* init function */
                switch_char_height_popup,       /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "text width popup",     /* area */
                init_char_width_popup,  /* init function */
                switch_char_width_popup,        /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "text path popup",      /* area */
                init_text_path_popup,   /* init function */
                switch_text_path_popup, /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "text horizontal justification popup",  /* area */
                init_horz_just_popup,   /* init function */
                switch_horz_just_popup, /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "text vertical justification popup",    /* area */
                init_vert_just_popup,   /* init function */
                switch_vert_just_popup, /* exec function */
                erase_popup             /* cleanup function */
        },

        {
                "rotation angle popup", /* area */
                init_angle_popup,       /* init function */
                switch_angle_state,     /* exec function */
                erase_popup             /* cleanup function */
        }
};

int menu_item_func_tbl_sz = 61;
struct menu_item_tbl_entry menu_item_func_tbl[] =
{
        {
                "help menu item",       /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "up menu item",         /* key */
                up,                     /* init function */
                up,                     /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "update menu item",     /* key */
                update,                 /* init function */
                update,                 /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "add objects menu item",        /* key */
                add_objects_init,       /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "add objects undo menu item",   /* key */
                add_objects_undo,       /* init function */
                add_objects_undo,       /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "line menu item",       /* key */
                add_line_init,          /* init function */
                add_line_restart,       /* restart function */
                add_line_exec,          /* exec function */
                add_line_cleanup        /* cleanup function */
        },

        {
                "curve menu item",      /* key */
                add_curve_init,         /* init function */
                add_curve_restart,      /* restart function */
                add_curve_exec,         /* exec function */
                add_curve_cleanup       /* cleanup function */
        },

        {
                "box menu item",        /* key */
                add_box_init,           /* init function */
                add_box_restart,        /* restart function */
                add_box_exec,           /* exec function */
                add_box_cleanup         /* cleanup function */
        },

        {
                "polygon menu item",    /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "circle menu item",     /* key */
                add_circle_init,        /* init function */
                add_circle_restart,     /* restart function */
                add_circle_exec,        /* exec function */
                add_circle_cleanup      /* cleanup function */
        },

        {
                "conic menu item",      /* key */
                add_conic_init,         /* init function */
                add_conic_restart,      /* restart function */
                add_conic_exec,         /* exec function */
                add_conic_cleanup       /* cleanup function */
        },

        {
                "text string menu item",        /* key */
                add_text_init,          /* init function */
                add_text_restart,       /* restart function */
                add_text_exec,          /* exec function */
                add_text_cleanup        /* cleanup function */
        },

        {
                "text block menu item", /* key */
                display_child_menu,     /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "text block undo menu item",    /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "create block menu item",       /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "split block menu item",        /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "join blocks menu item",        /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "enter text menu item", /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "move text menu item",  /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "copy text menu item",  /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "delete text menu item",        /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "change text color menu item",  /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "align vertically menu item",   /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "change text style menu item",  /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "change text size menu item",   /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "symbols menu item",    /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "edit objects menu item",       /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "edit picture menu item",       /* key */
                edit_picture_init,      /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "edit picture undo menu item",  /* key */
                edit_picture_undo,      /* init function */
                edit_picture_undo,      /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "move menu item",       /* key */
                move_init,              /* init function */
                move_restart,           /* restart function */
                move_exec,              /* exec function */
                move_cleanup            /* cleanup function */
        },

        {
                "copy menu item",       /* key */
                copy_init,              /* init function */
                copy_restart,           /* restart function */
                copy_exec,              /* exec function */
                copy_cleanup            /* cleanup function */
        },

        {
                "delete menu item",     /* key */
                delete_init,            /* init function */
                delete_restart,         /* restart function */
                delete_exec,            /* exec function */
                delete_cleanup          /* cleanup function */
        },

        {
                "erase menu item",      /* key */
                erase,                  /* init function */
                erase,                  /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "rotate menu item",     /* key */
                rotate_init,            /* init function */
                rotate_restart,         /* restart function */
                rotate_exec,            /* exec function */
                rotate_cleanup          /* cleanup function */
        },

        {
                "combine menu item",    /* key */
                combine_init,           /* init function */
                combine_restart,        /* restart function */
                combine_exec,           /* exec function */
                combine_cleanup         /* cleanup function */
        },

        {
                "split menu item",      /* key */
                split_init,             /* init function */
                split_restart,          /* restart function */
                split_exec,             /* exec function */
                split_cleanup           /* cleanup function */
        },

        {
                "rotate menu item",     /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "scale menu item",      /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "shear menu item",      /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "pan menu item",        /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "zoom menu item",       /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "adjust grid menu item",        /* key */
                adjust_grid,            /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "adjust grid undo menu item",   /* key */
                adjust_grid_undo,       /* init function */
                adjust_grid_undo,       /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "move grid menu item",  /* key */
                move_grid_init,         /* init function */
                move_grid_restart,      /* restart function */
                move_grid_exec,         /* exec function */
                move_grid_cleanup       /* cleanup function */
        },

        {
                "resize grid menu item",        /* key */
                resize_grid_init,       /* init function */
                resize_grid_restart,    /* restart function */
                resize_grid_exec,       /* exec function */
                resize_grid_cleanup     /* cleanup function */
        },

        {
                "create menu item",     /* key */
                display_child_menu,     /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "colors menu item",     /* key */
                colors_mitem_exec,      /* init function */
                NULL,                   /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "colors undo menu item",        /* key */
                area_undo,              /* init function */
                area_undo,              /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "fonts menu item",      /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "patterns menu item",   /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "user defined symbols menu item",       /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "user defined symbols menu item",       /* key */
                not_implemented,        /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "create pic file menu item",    /* key */
                create_pic_file,        /* init function */
                create_pic_file,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "photograph menu item", /* key */
                photo,                  /* init function */
                not_implemented,        /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "4 x 5 menu item",      /* key */
                four_by_five,           /* init function */
                four_by_five,           /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "8 x 10 menu item",     /* key */
                eight_by_ten,           /* init function */
                eight_by_ten,           /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "konica ft1 menu item", /* key */
                konica_ft1,             /* init function */
                konica_ft1,             /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "1 to 1 menu item",     /* key */
                one_to_one,             /* init function */
                one_to_one,             /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "quit menu item",       /* key */
                quit,                   /* init function */
                quit,                   /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "save menu item",       /* key */
                save,                   /* init function */
                save,                   /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        },

        {
                "retrieve menu item",   /* key */
                retrieve,               /* init function */
                retrieve,               /* restart function */
                NULL,                   /* exec function */
                NULL                    /* cleanup function */
        }
};


int menu_func_tbl_sz = 8;
struct menu_tbl_entry menu_func_tbl[] =
{
        {
                "main menu",            /* key */
                display_menu_n_popups,  /* init function */
                cleanup_menu_n_popups,  /* cleanup function */
        },

        {
                "add objects menu",     /* key */
                display_menu_n_popups,  /* init function */
                cleanup_menu_n_garbage, /* cleanup function */
        },

        {
                "text block menu",      /* key */
                display_menu_n_popups,  /* init function */
                cleanup_menu_n_popups,  /* cleanup function */
        },

        {
                "edit picture menu",    /* key */
                display_menu_n_popups,  /* init function */
                cleanup_menu_n_garbage, /* cleanup function */
        },

        {
                "adjust grid menu",     /* key */
                init_grid_menu,         /* init function */
                cleanup_menu_n_popups,  /* cleanup function */
        },

        {
                "create menu",          /* key */
                display_menu_n_popups,  /* init function */
                cleanup_menu_n_popups,  /* cleanup function */
        },

        {
                "photograph menu",      /* key */
                display_menu_n_popups,  /* init function */
                cleanup_menu_n_popups,  /* cleanup function */
        },

        {
                "colors menu",          /* key */
                init_colors_menu,       /* init function */
                cleanup_colors_menu     /* cleanup function */
        }
};



/*
 *  init_bundle_tbls
 *
 *  description:        loads bundle tables with appropriate
 *                      combinations to draw screen items
 *
 *  parameters:         none
 */

void init_bundle_tbls(void)
{
        Gflbundl frep;
        Glnbundl lrep;

        /* set up fill_area_bundle_tables */

        frep.style = 1;
        frep.color = grey;
        frep.inter = GHOLLOW;
        gsetfillrep(ws_id,GREY_HOLLOW,&frep);
        frep.inter = GSOLID;
        gsetfillrep(ws_id,GREY_SOLID,&frep);
        frep.color = white;
        gsetfillrep(ws_id,WHITE_SOLID,&frep);
        frep.inter = GHOLLOW;
        gsetfillrep(ws_id,WHITE_HOLLOW,&frep);
        frep.color = black;
        gsetfillrep(ws_id,BLACK_HOLLOW,&frep);
        frep.inter = GSOLID;
        gsetfillrep(ws_id,BLACK_SOLID,&frep);
        frep.color = display_fgdclr;
        gsetfillrep(ws_id,FGDCLR_SOLID,&frep);
        frep.color = display_bgdclr;
        gsetfillrep(ws_id,BGDCLR_SOLID,&frep);
        frep.color = display_fillclr;
        gsetfillrep(ws_id,FILLCLR_SOLID,&frep);


        /* set up polyline_bundle_tables */

        lrep.type = GLN_SOLID;
        lrep.width = 1.0;
        lrep.color = grey;
        gsetlinerep(ws_id,GREY_THIN_SOLID,&lrep);
        lrep.color = white;
        gsetlinerep(ws_id,WHITE_THIN_SOLID,&lrep);
        lrep.color = black;
        gsetlinerep(ws_id,BLACK_THIN_SOLID,&lrep);


        /* set up text_bundle_tables */

        init_text_bundle_tbl("text_bndl_tbl");
        init_char_hts_n_wds("char_hts_n_wds",&char_wd_tbl,&char_ht_tbl);

}  /* end init_bundle_tbls */


/*
 *  init_text_bundle_tbl
 *
 *  description:        sets up text bndle table so that a key
 *                      will correspond to a particular text font, 
 *                      precision, expansion, spacing and height
 *
 *  parameters:         filename (char *) - file containing text info
 *
 */

void init_text_bundle_tbl(char *filename)
{
        FILE *fd;                       /* fd for file containing text 
                                           info */
        Gtxbundl text_bundle;           /* text bundle tbl entry info */
        char *precision;                /* text precision in chr form */
        IDX i;                          /* index in text bundle table */
        int n;

        extern char *get_char_str();

        /* open file containing text bundle tbl info */

        fd = fopen(filename,"r");
        if (fd == (FILE *) NULL)
                exit_error("init_text_bundle_tbl",3);

        /* read in no of entries in tbl */

        get_int(fd,&n,4);

        text_bndl_tbl = (TEXT_BNDL *) calloc((unsigned) n,
                sizeof(TEXT_BNDL));

        /* read in tbl entries and set up gks text bundle tbl */

        for (i=0; i<n; i++)
        {
                text_bndl_tbl[i].idx = i + 1;

                /* read in text entry key */

                text_bndl_tbl[i].key = get_char_str(fd,4);

                /* read in text info for entry */

                get_int(fd,&(text_bundle.fp.font),4);

                precision = get_char_str(fd,4);
                if (eq(precision,"GSTRING"))
                        text_bundle.fp.prec = GSTRING;
                else if (eq(precision,"GCHAR"))
                        text_bundle.fp.prec = GCHAR;
                else /* precision = "GSTROKE" */
                        text_bundle.fp.prec = GSTROKE;
                free((char *) precision);

                get_real(fd,&(text_bundle.ch_exp),4);
                get_real(fd,&(text_bundle.space),4);
                get_real(fd,&(text_bndl_tbl[i].char_ht),4);

                /* color does not matter because
                   color gets set when text procedure is called */

                text_bundle.color = white;

                /* set up gks tbl entry */

                gsettextrep(ws_id,i + 1,&text_bundle);
        }

        /* close file */

        fclose(fd);

}  /* end init_text_bundle_tbl */


/*
 * init_char_hts_n_wds
 *
 *  description:        reads in character heights and widths
 *                      from extenral file
 *
 *  parameters:         filename (char *) - file to read from
 *                      wd_tbl (Gfloat *) - table of char widths
 *                      ht_tbl (Gfloat *) - table of char widths
 */

void init_char_hts_n_wds(char *filename, Gfloat **wd_tbl, Gfloat **ht_tbl)
{
        FILE *fd;
        int n,i;

        /* open file containing char ht and width info */

        fd = fopen(filename,"r");
        if (fd == (FILE *) NULL)
                exit_error("init_char_hts_n_wds",16);

        /* read in no of entries in wd_tbl */

        get_int(fd,&n,17);
        *wd_tbl = (Gfloat *) calloc((unsigned) n, sizeof(Gfloat));

        for (i=0; i<n; i++)
                get_real(fd,&((*wd_tbl)[i]),17);

        /* read in no of entries in ht_tbl */

        get_int(fd,&n,17);
        *ht_tbl = (Gfloat *) calloc((unsigned) n, sizeof(Gfloat));

        for (i=0; i<n; i++)
                get_real(fd,&((*ht_tbl)[i]),17);

        /* close file */

        fclose(fd);
}  /* end init_char_hts_n_wds */
