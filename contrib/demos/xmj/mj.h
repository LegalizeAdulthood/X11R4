/*
 * $XConsortium: mj.h,v 1.6 89/05/05 14:24:03 jim Exp $
 *
 * Original source Copyright 1988 Exceptions
 * X11 portions Copyright 1989 Concurrent Computer Corporation
 * 
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Exceptions and Concurrent not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Exceptions and Concurrent make
 * no representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 * 
 * EXCEPTIONS AND CONCURRENT DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
 * IN NO EVENT SHALL EXCEPTIONS OR CONCURRENT BE LIABLE FOR ANY SPECIAL,
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Original Author:  Mark Holm, Exceptions, nosun!tessi!exc!markh@sun.com
 * 
 * Acknowledgments to Dorothy Robinson for her artistic abilities in drawing
 * the icons and to Jim Batch for technical support and graphical concepts 
 * Jim Batch for techical support and graphical concepts.
 * 
 * X11 port by:  Jay Estabrook, Concurrent Computer Corporation
 */

#ifndef lint
static char *rcsh = "$header$ Copyright 1988 Mark A. Holm";
#endif !lint

#include <X11/Xlib.h>
#include <X11/Xutil.h>


/*
 * $log$
 */

#include "casts.h"

/* X11 objects */

Display *display;
Window Draw, Root;
Colormap cmap;
GC ctxFG, ctxBG, ctxXOR;
XSetWindowAttributes att;
XSizeHints hints;
Visual *viz;
int screen, depth;
int ForeColor, BackColor;

/*
 * font stuff
 */
#define DEFAULT_FONT_NAME	"8x13"
char *font_name;
XFontStruct *big_font;

/* miscellaneous */

#ifndef BSD_RANDOM
#ifdef SYSV
#define BSD_RANDOM 0
#else
#define BSD_RANDOM 1
#endif
#endif

#if BSD_RANDOM
#define RANDOM(x)	((random() >> 8) % x)
#else
#define RANDOM(x)	((rand() >> 8) % x)
#endif

#define boolean int
#define NULL 0
#define TRUE 1
#define FALSE 0
#define MAX_COLORS		7	/* maximum colors in color map excludingblack */

/* extra window definitions */
#define BORDER		5	/* border width between panels */

/* define tile playing area offsets from frame edges */
#define X_LOC 30
#define Y_LOC 40

#define FRAME_X		50
#define FRAME_Y		50

#define W_BASE_TILE     58
#define H_BASE_TILE     59
#define S_TILE_SHADOW	6
#define B_TILE_SHADOW	5
#define TILE_WIDTH	(W_BASE_TILE + S_TILE_SHADOW)
#define TILE_HEIGHT	(H_BASE_TILE + B_TILE_SHADOW)
#define WIDE_MAX_TILES  15
#define HIGH_MAX_TILES  8
#define PLAY_X_MAX	((W_BASE_TILE * WIDE_MAX_TILES) + S_TILE_SHADOW + (2 * X_LOC))
#define PLAY_Y_MAX	((H_BASE_TILE * HIGH_MAX_TILES)  + B_TILE_SHADOW + (2 * Y_LOC))
#define MESS_X_MAX	(PLAY_X_MAX)
#define MESS_Y_MAX	(H_BASE_TILE * 3)
#define FRAME_X_MAX	(PLAY_X_MAX )
#define FRAME_Y_MAX	(PLAY_Y_MAX + MESS_Y_MAX + BORDER)

/* define rows and cols */

#define ROW1	(Y_LOC + MESS_Y_MAX)
#define ROW2	(Y_LOC + MESS_Y_MAX + H_BASE_TILE)
#define ROW3	(Y_LOC + MESS_Y_MAX + (H_BASE_TILE * 2))
#define ROW4	(Y_LOC + MESS_Y_MAX + (H_BASE_TILE * 3))
#define ROW4pt5	(Y_LOC + MESS_Y_MAX + (H_BASE_TILE * 3) + (H_BASE_TILE / 2))
#define ROW5	(Y_LOC + MESS_Y_MAX + (H_BASE_TILE * 4))
#define ROW6	(Y_LOC + MESS_Y_MAX + (H_BASE_TILE * 5))
#define ROW7	(Y_LOC + MESS_Y_MAX + (H_BASE_TILE * 6))
#define ROW8	(Y_LOC + MESS_Y_MAX + (H_BASE_TILE * 7))

#define COL1	(X_LOC)
#define COL2	(X_LOC + W_BASE_TILE)
#define COL3	(X_LOC + (W_BASE_TILE * 2))
#define COL4	(X_LOC + (W_BASE_TILE * 3))
#define COL5	(X_LOC + (W_BASE_TILE * 4))
#define COL6	(X_LOC + (W_BASE_TILE * 5))
#define COL7	(X_LOC + (W_BASE_TILE * 6))
#define COL7pt5	(X_LOC + (W_BASE_TILE * 6) + (W_BASE_TILE / 2))
#define COL8	(X_LOC + (W_BASE_TILE * 7))
#define COL9	(X_LOC + (W_BASE_TILE * 8))
#define COL10	(X_LOC + (W_BASE_TILE * 9))
#define COL11	(X_LOC + (W_BASE_TILE * 10))
#define COL12	(X_LOC + (W_BASE_TILE * 11))
#define COL13	(X_LOC + (W_BASE_TILE * 12))
#define COL14	(X_LOC + (W_BASE_TILE * 13))
#define COL15	(X_LOC + (W_BASE_TILE * 14))

/* Tile structure */

typedef struct tile {
    Pixmap 	 image;
    int		 value;
    boolean	 left_free;
    boolean	 right_free;
    boolean	 top_free;
    int		 left_next[2];
    int		 right_next[2];
    int		 covered[4];
    boolean	 removed;
    int		 x, y;
} Tiles;

typedef struct sel {
        boolean		filled;
        Tiles          *item;
        Event           event;
	int		item_index;
} Selected;

/* common playing and number tiles Pixmaps */

#define UNIQUE_TILES	42
#define NBUTTONS	5
#define BUTTON_WIDTH	64
#define BUTTON_HEIGHT	16

Pixmap Playing_tiles [UNIQUE_TILES];
Pixmap Number_tiles [10];
Pixmap Button_tiles [NBUTTONS];
Pixmap Blank_tile;
Pixmap Black_tile;

/* global externals */

extern char *BandW_num_bitmaps[];
extern char *BandW_tile_bitmaps[];
extern char *BandW_button_bitmaps[];
extern char *BandW_blank_bitmaps[];
extern char *Color_num_bitmaps[];
extern char *Color_tile_bitmaps[];
extern char *Color_button_bitmaps[];

/* misc defines */

#define NORMAL_TILE	0
#define HILITE_TILE	1
#define REVERT_TILE	2

/* macros for calculating font character width/height */

#define YCOORD	MESS_Y_MAX-BORDER-H_BASE_TILE-B_TILE_SHADOW

#define l_width(font) \
    ((font)->max_bounds.rbearing-(font)->min_bounds.lbearing)
#define l_height(font) \
    ((font)->max_bounds.ascent+(font)->max_bounds.descent)

#define center_x(m)	(middle - (l_width(big_font) * strlen((m)) / 2))
#define line_y(r)	(BORDER + ((l_height(big_font) + 1) * (r)))
#define center_message(m,r)	show_message((m), center_x((m)), line_y((r)))
#define update_message(m)	show_message((m), middle, line_y(11) )

/* global flags */

boolean exiting;
boolean exposing;

