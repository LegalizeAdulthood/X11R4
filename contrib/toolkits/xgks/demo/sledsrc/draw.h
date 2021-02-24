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
 */

#if !defined(DRAW_H)
#define DRAW_H

#include "changes.h"
#include "clr_sel_draw.h"
#include "screen_items.h"

/*
 *  bundle tbl indices for fillarea tbl
 */

#define GREY_HOLLOW 1
#define GREY_SOLID 2
#define WHITE_SOLID 3
#define WHITE_HOLLOW 4
#define BLACK_SOLID 5
#define BLACK_HOLLOW 6
#define FGDCLR_SOLID 7
#define BGDCLR_SOLID 8
#define FILLCLR_SOLID 9
#define FREE_FILL_AREA_ENTRY 10

/*
 *  bundle tbl indices for polymarker bundle table
 */

#define GRID_MARKER 1

/*
 *  bundle tbl indices for polyline tbl
 */

#define WHITE_THIN_SOLID 1
#define GREY_THIN_SOLID 2
#define BLACK_THIN_SOLID 3

/*
 *  bundle tbl keys for text tbl
 */

#define MENU_TEXT "menu text key"
#define PROMPT_TEXT "prompt text key"
#define CLR_AREA_TEXT "clr area text key"
#define POPUP_TEXT "popup text key"
#define POPUP_NUMBER_TEXT "popup number text key"

/*
 * tbl of keys and text tbl indices
 */

typedef struct text_bndl TEXT_BNDL;
struct text_bndl
{
    KEY key;        /* text style key */
    IDX idx;        /* index into text bndl tbl */
    Gfloat char_ht; /* ht of text since not
                                           stored in real bndl tbl */
};

extern TEXT_BNDL *text_bndl_tbl; /* list of keys and indices */

#define PI 3.1415926536

#define NO_CONIC_LNS 100

void curve(Gpoint *c, int nopts, Gpoint *pts);
void load_box(Gpoint *box, Gfloat xmin, Gfloat xmax, Gfloat ymin, Gfloat ymax);
void circle(Gpoint center, Gfloat rad, int nopts, Gpoint *pts);
void conic(Gpoint center, Gfloat wd, Gfloat ht, int nopts, Gpoint *pts);
void get_font(KEY key, IDX *font, Gtxprec *precision);
void set_font(KEY key, IDX font, Gtxprec precision);
void set_aspect_flags(Gasf setting);
void set_free_fill_rep(IDX color, Gflinter style);
void draw_conic(Gpoint center, Gfloat wd, Gfloat ht);
void center_text_vert(char *line, Gpoint *center, IDX clrno, KEY key);
void center_text(int nolines, char **lines, Gpoint *center, IDX clrno, KEY key);

#endif
