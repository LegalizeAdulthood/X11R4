/*
 * $XConsortium: mj.c,v 1.8 89/05/05 14:24:07 jim Exp $
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


#include <stdio.h>
#include <X11/Xos.h>
#include <sys/ioctl.h>

#include "mj.h"

#define SELECT_MASK \
    (ButtonPressMask | ButtonReleaseMask | PointerMotionMask | \
     KeyPressMask | KeyReleaseMask | StructureNotifyMask | \
     SubstructureNotifyMask | VisibilityChangeMask | ExposureMask )

void			die();
void			build_image();
void			place_tiles();


/*
** Black and white closed icon image
*/
#include "mj.icon"
DEFINE_ICON_FROM_IMAGE(icon, icon_image); /* Black and white icon */
struct icon *cicon;			  /* storage for color icon */

/*
** externals
*/
extern void	quit_proc();
extern void	undo_proc();
extern void	new_proc();
extern void	again_proc();
extern void	help_proc();
extern void	board_num_proc();
extern void	play_back_proc();
extern void	play_event_proc();

extern short	stick_image[];
extern int	undo_count;

/*
** overlap globals
*/
Panel_item	tile[144];
Panel_item	tiles_left[3];
Frame		main_frame;
Cursor		play_cursor;
boolean		BandW = TRUE;
Tiles		*board[144];
Selected	selected[2];
Selected	last_item;
int		tile_count;
#if BSD_RANDOM
char		state[256];
#endif

/*
** local globals
*/
struct timeval	*tp;
struct timezone	*tz;
int		seed;
int		middle;

/*
** define color map
*/
#include "color.h"

char *ProgramName;

main(argc, argv)
int argc;
char **argv;
{
    int i;

    int ac = argc;
    char **av = argv;
    char *fc = NULL, *bc = NULL, *def;
    char *pname = *argv;
    char *display_name = NULL;
    boolean rvflag = FALSE;
    XColor xcolor;
    GC ctxDF;
    XEvent event;
    int but_x, but_y;
    XSetWindowAttributes attr;
    int bandw_forced = FALSE;

    ProgramName = argv[0];

    /* parse arguments */

    for(argc--, argv++; argc > 0; argc--, argv++)
	if (argv[0][0] = '-')
	    switch (argv[0][1]) {
		case 'b': /* force black and white */
		    BandW = TRUE;
		    bandw_forced = TRUE;
		    break;
		case 'c': /* force color */
		    BandW = FALSE;
		    bandw_forced = TRUE;
		    break;
		case 'd':
		    display_name = *++argv;
		    argc--;
		    break;
		case 'n': /* set board number */
		    if(argc-- == 0)
			die("Usage: %s [-b] [-c] [-n #]\n", pname);
		    argv++;
    		    sscanf(argv[0] , "%d", &seed);
		    if (seed > 20011) {
			printf("Board numbers must be < 20011");
			seed %= 20011;
		    }
		    break;
		case 'r':
		    rvflag = TRUE;
		    break;
		default:
		    die("Usage: %s [-b] [-c] [-n #]\n", pname);
		    break;
	    }
	else
	    die("Usage: %s [-b] [-c] [-n #]\n", pname);

    /* Open up the display. */
    /* determine type of frame buffer and set BandW accordingly */


    if ((display = XOpenDisplay(display_name)) == NULL) {
        fprintf(stderr, "%s: Can't open display '%s'\n",
                         pname, display_name);
        exit(1);
    }

    /*
     * get all the defaults we can
     */
    screen = XDefaultScreen( display );
    Root = XRootWindow( display, screen );
    depth = XDefaultDepth( display, screen );
    cmap = XDefaultColormap( display, screen );
    ctxDF = DefaultGC( display, screen );
    viz = DefaultVisual( display, screen );
    if (!bandw_forced) {
	XVisualInfo template, *vip;
	int nvips;

	template.visualid = XVisualIDFromVisual (viz);
	vip = XGetVisualInfo (display, VisualIDMask, &template, &nvips);
	if (!vip) {
	    fprintf (stderr, "%s:  unable to get visual info for 0x%lx\n",
		     ProgramName, template.visualid);
	} else {
	    if (vip->depth > 1 && (vip->class & 1)) BandW = FALSE;
	    XFree (vip);
	}
    }

    if (depth < 3)
	BandW = TRUE;

    /*
     * get X defaults from the ".Xdefaults" file
     */
    if (!fc)
	fc  = XGetDefault(display, ProgramName, "Foreground");
    if (!bc)
	bc  = XGetDefault(display, ProgramName, "Background");

    if (!fc)
	fc = "black";
    if (!bc)
	bc = "white";

    font_name = XGetDefault (display, ProgramName, "Font");
    if (!font_name) font_name = DEFAULT_FONT_NAME;

    /*
     * Set normal default colors
     */
    if (rvflag)
    {
    	def = fc;
    	fc = bc;
    	bc = def;
	ForeColor = WhitePixel( display, screen );
	BackColor = BlackPixel( display, screen );
    }
    else
    {
	ForeColor = BlackPixel( display, screen );
	BackColor = WhitePixel( display, screen );
    }

    /* apply color maps to frame pixwin */

    if (XParseColor(display, cmap, fc, &xcolor)
      && XAllocColor(display, cmap, &xcolor))
        ForeColor = xcolor.pixel;

    if (XParseColor(display, cmap, bc, &xcolor)
      && XAllocColor(display, cmap, &xcolor))
        BackColor = xcolor.pixel;

    /* initialize random number generator seed */

    tp = (struct timeval *) malloc(sizeof(struct timeval));
    tz = (struct timezone *) malloc(sizeof(struct timezone));
    gettimeofday(tp, tz);
#if BSD_RANDOM
    (void) initstate((unsigned) (tp->tv_sec % 255), state, 256);
#else
    (void) srand ((unsigned) (tp->tv_sec % 255));
#endif
    seed = RANDOM(20011);
    free(tp);
    free(tz);

    Draw = XCreateSimpleWindow( display, Root, FRAME_X, FRAME_Y,
				FRAME_X_MAX, FRAME_Y_MAX,
				1, ForeColor, BackColor );
    if (!Draw) {
        printf("Can't open Draw window\n");
        exit( 1 );
    }

    /* if color then apply color icon to window icon */

    icon_image = XCreateBitmapFromData( display, Root, mahjongg_bits,
				       mahjongg_width, mahjongg_height );

    if (!BandW)
	init_colormap();

    /*
     * make the graphics contexts for the program
     */
    ctxFG = XCreateGC( display, Draw, 0, NULL );
    ctxBG = XCreateGC( display, Draw, 0, NULL );
    ctxXOR = XCreateGC( display, Draw, 0, NULL );

    /*
     * setup window manager hints
     */
    hints.x = FRAME_X;
    hints.y = FRAME_Y;
    hints.width = FRAME_X_MAX;
    hints.height = FRAME_Y_MAX;
    hints.flags = PPosition | USSize;

    XSetStandardProperties( display, Draw, pname, "XMAH",
			    icon_image, av, ac, &hints);

    /* determine middle of panel */

    middle = (MESS_X_MAX / 2);

    /* do font init stuff */

    if ((big_font = XLoadQueryFont( display, font_name )) == NULL)
    {
    	fprintf (stderr, "unable to load font %s\n", font_name);
	if ((big_font = XLoadQueryFont( display, "variable" )) == NULL) {
	    exit (1);
	}
	
    } 

    XCopyGC( display, ctxDF, -1, ctxFG );
    XSetFillStyle( display, ctxFG, FillSolid );
    XSetGraphicsExposures( display, ctxFG, FALSE );
    XSetFunction ( display, ctxFG, GXcopy );
    XSetBackground( display, ctxFG, BackColor );
    XSetForeground( display, ctxFG, ForeColor );
    XSetPlaneMask( display, ctxFG, AllPlanes );
    XSetFont( display, ctxFG, big_font->fid );

    XCopyGC( display, ctxFG, -1, ctxBG );
    XSetForeground( display, ctxBG, BackColor );
    XSetBackground( display, ctxBG, ForeColor );

    XCopyGC( display, ctxFG, -1, ctxXOR );
    XSetFunction( display, ctxXOR, GXxor );
    XSetForeground( display, ctxXOR, 7);
    XSetBackground( display, ctxXOR, 0);
    XSetPlaneMask( display, ctxXOR, ForeColor ^ BackColor );
/*    XSetFont( display, ctxXOR, big_font->fid ); */

    /*
     * finally, map the little devil
     */
    XMapWindow( display, Draw );

    /* build board image */

    init_tiles(BandW);
    build_image(FALSE);
    place_tiles(TRUE);
    redraw_all();

    /* start main processing */

    XSelectInput( display, Draw, SELECT_MASK );

    exiting = FALSE;
    while (!exiting)
    {
	exposing = FALSE;
	do
	{
    	    XNextEvent( display, &event );
    	    switch( event.type )
    	    {
		case ButtonPress:
		case ButtonRelease:
		    /* check for in "play" area */
		    if (event_y(&event) > MESS_Y_MAX)
			play_event_proc( &event );
		    else
			msg_event_proc( &event );
		    break;

		case VisibilityNotify:
		    if (event.xvisibility.window != Draw)
		        break;
		    if (event.xvisibility.state == VisibilityUnobscured)
			exposing = TRUE;
		    break;

		case Expose:
		    exposing = TRUE;
		    break;

		case MapNotify:
		    if (event.xmap.window != Draw)
			break;
		    exposing = TRUE;
		    break;

		default:
		    break;
    	    } /* end switch */
    	} while (XPending( display ));

	if (exposing)
	    redraw_all();

    } /* end while (!exiting) */

    XCloseDisplay (display);
    exit(0);
}


/* die because of some UNIX error */
void die(message, pperr)
char *message;
int pperr;
{
    fprintf(stderr, message);
    if (pperr)
	perror("xmj");
    exit(1);
}

void place_tiles(newboard)
boolean		newboard;

{
    int i;
    int j;
    int k;
    int x_loc;
    int y_loc;

    /* check if not new and destroy existing panel buttons */

    XClearArea( display, Draw, 0, MESS_Y_MAX, PLAY_X_MAX, PLAY_Y_MAX, FALSE );

    /* place tiles */

        /* row 1 level 1 */

	for(i = 0,
	    x_loc = COL2;
	    i < 12;
	    i++,
	    x_loc += W_BASE_TILE)
	    {
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = x_loc;
		board[i]->y = ROW1;
/*		show_tile( i, NORMAL_TILE ); */
	    }

	/* row 2 level 1 */

	for(x_loc = COL4,
	    j = 0;
	    j < 8;
	    j++,
	    i++,
	    x_loc += W_BASE_TILE)
	    {
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = x_loc;
		board[i]->y = ROW2;
/*		show_tile( i, NORMAL_TILE ); */
	    }

	/* row 3 level 1 */

	for(x_loc = COL3,
	    j = 0;
	    j < 10;
	    j++,
	    i++,
	    x_loc += W_BASE_TILE)
	    {
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = x_loc;
		board[i]->y = ROW3;
/*		show_tile( i, NORMAL_TILE ); */
	    }

	/* row 4 1/2 level 1 */

	/* Left */
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = COL1;
		board[i]->y = ROW4pt5;
/*		show_tile( i, NORMAL_TILE ); */
		i++; /* increment tile counter */

	/* row 4 level 1 */

	for(x_loc = COL2,
	    j = 0;
	    j < 12;
	    j++,
	    i++,
	    x_loc += W_BASE_TILE)
	    {
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = x_loc;
		board[i]->y = ROW4;
/*		show_tile( i, NORMAL_TILE ); */
	    }

	/* row 5 level 1 */

	for(x_loc = COL2,
	    j = 0;
	    j < 12;
	    j++,
	    i++,
	    x_loc += W_BASE_TILE)
	    {
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = x_loc;
		board[i]->y = ROW5;
/*		show_tile( i, NORMAL_TILE ); */
	    }

	/* row 4 1/2 level 1 */

	/* Right */
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = COL14;
		board[i]->y = ROW4pt5;
/*		show_tile( i, NORMAL_TILE ); */
		i++; /* increment tile counter */

		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = COL15;
		board[i]->y = ROW4pt5;
/*		show_tile( i, NORMAL_TILE ); */
		i++; /* increment tile counter */

	/* row 6 level 1 */

	for(x_loc = COL3,
	    j = 0;
	    j < 10;
	    j++,
	    i++,
	    x_loc += W_BASE_TILE)
	    {
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = x_loc;
		board[i]->y = ROW6;
/*		show_tile( i, NORMAL_TILE ); */
	    }

	/* row 7 level 1 */

	for(x_loc = COL4,
	    j = 0;
	    j < 8;
	    j++,
	    i++,
	    x_loc += W_BASE_TILE)
	    {
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = x_loc;
		board[i]->y = ROW7;
/*		show_tile( i, NORMAL_TILE ); */
	    }

        /* row 8 level 1 */

	for(j = 0,
	    x_loc = COL2;
	    j < 12;
	    j++,
	    i++,
	    x_loc += W_BASE_TILE)
	    {
		tile[i] = (board[i]->top_free) ?
			   board[i]->image :
			   Blank_tile;
		board[i]->x = x_loc;
		board[i]->y = ROW8;
/*		show_tile( i, NORMAL_TILE ); */
	    }

	/* rows 1-6 level 2 */

	for(y_loc = ROW2 - B_TILE_SHADOW,
	    j = 0;
	    j < 6;
	    j++,
	    y_loc += H_BASE_TILE)
	    {
		for(x_loc = COL5 - S_TILE_SHADOW,
		    k = 0;
		    k < 6;
		    i++,
		    k++,
		    x_loc += W_BASE_TILE)
		    {
			tile[i] = (board[i]->top_free) ?
				   board[i]->image :
				   Blank_tile;
			board[i]->x = x_loc;
			board[i]->y = y_loc;
/*			show_tile( i, NORMAL_TILE ); */
		    }
	    }

	/* rows 1-4 level 3 */

	for(y_loc = ROW3 - (B_TILE_SHADOW * 2),
	    j = 0;
	    j < 4;
	    j++,
	    y_loc += H_BASE_TILE)
	    {
		for(x_loc = COL6 - (S_TILE_SHADOW * 2),
		    k = 0;
		    k < 4;
		    i++,
		    k++,
		    x_loc += W_BASE_TILE)
		    {
			tile[i] = (board[i]->top_free) ?
				   board[i]->image :
				   Blank_tile;
			board[i]->x = x_loc;
			board[i]->y = y_loc;
/*			show_tile( i, NORMAL_TILE ); */
		    }
	    }

	/* rows 1-2 level 4 */

	for(y_loc = ROW4 - (B_TILE_SHADOW * 3),
	    j = 0;
	    j < 2;
	    j++,
	    y_loc += H_BASE_TILE)
	    {
		for(x_loc = COL7 - (S_TILE_SHADOW * 3),
		    k = 0;
		    k < 2;
		    i++,
		    k++,
		    x_loc += W_BASE_TILE)
		    {
			tile[i] = board[i]->image;
			board[i]->x = x_loc;
			board[i]->y = y_loc;
/*			show_tile( i, NORMAL_TILE ); */
		    }
	    }

	/* Cap tile */
		tile[i] = board[i]->image;
		board[i]->x = COL7pt5 - (S_TILE_SHADOW * 4);
		board[i]->y = ROW4pt5 - (B_TILE_SHADOW * 4);
/*		show_tile( i, NORMAL_TILE ); */

    XFlush( display );
}

void
build_image(oldimage)
boolean oldimage;
{
	int 	i;
	int 	j;
	int	pool[42];
	boolean ok;
	boolean dir;
	char	seed_text[80];

	/* initialize selected structures */

        selected[0].filled = FALSE;
        selected[1].filled = FALSE;
        last_item.filled   = FALSE;
        undo_count   	   = -1;

	/* initialize random number counter */

	(void) srandom(seed);

	tile_count = 144;

	/* initialize tile pool */
	for(i = 0; i < 34; i++) pool[i] = 4;
	for(; i < 42; i++) pool[i] = 1;

	/* assign values to each location. Board is built from upper left *
	 * to lower right, bottom to top. Exception are left tile for row *
	 * 4pt5 is before rows 4 and 5, and right tiles for row 4.5 are   *
	 * after rows 4 and 5                                             */

	for(j = 0; j < 144; j++) {
	    if (board[j] == NULL) /* intialize array */
		board[j] = (Tiles *) malloc(sizeof(Tiles));

	    if (!oldimage) { /* not repeating last board */

		/* Randomly seed index into pool. Randomly *
		 *  run up or down list until unused tile  *
		 *  is found or end of list. If end of     *
		 *  list reseed and run in opposite        *
		 *  direction in list until unused tile is *
		 *  found. If beginning of list found,     *
		 *  start over. 			   */

		ok = FALSE;
		while (ok == FALSE) {
			i = RANDOM(41);
			/* Up, up, up! */
			dir = random()&01;
			while ((i < 42 || i >=0) && pool[i] == 0) (dir) ? i++ : i--;
			if (i == 42 || i < 0) { /* Thud! Reverse march! */
				i = RANDOM(41);
				while ((i < 42 || i >= 0) && pool[i] == 0) (dir) ? i-- : i++;
			}
			if (i == 42 || i < 0) continue; /* Missed! try again */
			pool[i]--;
			ok = TRUE;
		}

		/* all flowers and all seasons */

		board[j]->value = (i >= 34) ? ((i >= 38) ? 35 : 34) : i;
		board[j]->image = Playing_tiles[i];

	}
	/* establish default values */

		board[j]->left_free     = FALSE;
		board[j]->right_free    = FALSE;
		board[j]->top_free      = TRUE;
		board[j]->left_next[0]  = j - 1;
		board[j]->left_next[1]  = 999;
		board[j]->right_next[0] = j + 1;
		board[j]->right_next[1] = 999;
		board[j]->covered[0]    = 999;
		board[j]->covered[1]    = 999;
		board[j]->covered[2]    = 999;
		board[j]->covered[3]    = 999;
		board[j]->removed       = FALSE;

	/* setup special cases */

		switch (j) {
		case 139:
		case 141:
			board[j]->top_free = FALSE;
		case 0:
		case 12:
		case 20:
		case 30:
		case 57:
		case 67:
		case 75:
		case 87:
		case 93:
		case 99:
		case 105:
		case 111:
		case 117:
		case 123:
		case 127:
		case 131:
		case 135:
			board[j]->left_free = TRUE;
			board[j]->left_next[0] = 999;
			break;
		case 140:
		case 142:
			board[j]->top_free = FALSE;
		case 11:
		case 19:
		case 29:
		case 56:
		case 66:
		case 74:
		case 86:
		case 92:
		case 98:
		case 104:
		case 110:
		case 116:
		case 122:
		case 126:
		case 130:
		case 134:
		case 138:
			board[j]->right_free = TRUE;
			board[j]->right_next[0] = 999;
			break;
		case 143:
			board[j]->right_free = TRUE;
			board[j]->left_next[0] = 999;
			board[j]->left_free = TRUE;
			board[j]->right_next[0] = 999;
			board[j]->covered[0] = 139;
			board[j]->covered[1] = 140;
			board[j]->covered[2] = 141;
			board[j]->covered[3] = 142;
			break;
		case 42:
			board[j]->right_next[0] = 55;
			break;
		case 43:
			board[j]->left_next[0] = 30;
			break;
		case 55:
			board[j]->left_next[1] = 42;
			break;
		}

	}

	/* special case (did not fit in above) */

	board[30]->right_next[1] = 43;

	/* set top_free flags  and covered pointers */

	for(i = 87, j = 13; i < 143; i++, j++) {
		board[i]->covered[0] = j;
		board[j]->top_free = FALSE;
		switch(j) {
			case 97:
			case 103:
			case 109:
			case 129:
				 j += 2;
				 break;
			case 18:
			case 64:
				 j += 3;
				 break;
			case 27:
			case 39:
				 j += 6;
				 break;
			case 51:
				 j += 7;
				 break;
			case 73:
				 j += 20;
				 break;
			case 115:
				 j += 12;
				 break;
		}
	}
}

/* routine to init common tiles depending on color */

#define ReuseXImage(p,a) (p)->data = (a)

init_tiles( BWflag )
    int BWflag;
{
    int i;
    Pixmap pix_id;

    if (BWflag) /* B/W */
    {
	for (i = 0; i < UNIQUE_TILES; i++)
	{
	    pix_id = XCreateBitmapFromData( display, Draw,
					BandW_tile_bitmaps[i],
					TILE_WIDTH, TILE_HEIGHT );
	    if (pix_id == (Pixmap)0)
	    {
	    	die( "unable to create Pixmap\n", 0 );
	    	break;
	    }
	    Playing_tiles[i] = pix_id;
	}
	for (i = 0; i < 10; i++)
	{
	    pix_id = XCreateBitmapFromData( display, Draw,
					BandW_num_bitmaps[i],
					TILE_WIDTH, TILE_HEIGHT );
	    if (pix_id == (Pixmap)0)
	    {
	    	die( "unable to create Pixmap\n", 0 );
	    	break;
	    }
	    Number_tiles[i] = pix_id;
	}
	for (i = 0; i < NBUTTONS; i++)
	{
	    pix_id = XCreateBitmapFromData( display, Draw,
					BandW_button_bitmaps[i],
					BUTTON_WIDTH, BUTTON_HEIGHT );
	    if (pix_id == (Pixmap)0)
	    {
	    	die( "unable to create Pixmap\n", 0 );
	    	break;
	    }
	    Button_tiles[i] = pix_id;
	}
    }
    else /* COLOR!! */
    {
	XImage *xI;

	xI = XCreateImage( display, viz, depth, ZPixmap, 0, (char *)0,
			TILE_WIDTH, TILE_HEIGHT, 8, 0 );


	for (i = 0; i < UNIQUE_TILES; i++)
	{
	    ReuseXImage( xI, Color_tile_bitmaps[i] );
	    pix_id = XCreatePixmap( display, Draw,
				    TILE_WIDTH, TILE_HEIGHT, depth );
	    if (pix_id == (Pixmap)0)
	    {
	    	die( "unable to create Pixmap\n", 0 );
	    	break;
	    }
	    XPutImage( display, pix_id, ctxFG, xI, 0, 0, 0, 0,
			TILE_WIDTH, TILE_HEIGHT );
	    Playing_tiles[i] = pix_id;
	}
	for (i = 0; i < 10; i++)
	{
	    ReuseXImage( xI, Color_num_bitmaps[i] );
	    pix_id = XCreatePixmap( display, Draw,
				    TILE_WIDTH, TILE_HEIGHT, depth );
	    if (pix_id == (Pixmap)0)
	    {
	    	die( "unable to create Pixmap\n", 0 );
	    	break;
	    }
	    XPutImage( display, pix_id, ctxFG, xI, 0, 0, 0, 0,
			TILE_WIDTH, TILE_HEIGHT );
	    Number_tiles[i] = pix_id;
	}
	for (i = 0; i < NBUTTONS; i++)
	{
	    ReuseXImage( xI, Color_button_bitmaps[i] );
	    pix_id = XCreatePixmap( display, Draw,
				    BUTTON_WIDTH, BUTTON_HEIGHT, depth );
	    if (pix_id == (Pixmap)0)
	    {
	    	die( "unable to create Pixmap\n", 0 );
	    	break;
	    }
	    XPutImage( display, pix_id, ctxFG, xI, 0, 0, 0, 0,
			BUTTON_WIDTH, BUTTON_HEIGHT );
	    Button_tiles[i] = pix_id;
	}
    }

/*
 * do these for both always...
 */
    pix_id = XCreateBitmapFromData( display, Draw,
				    BandW_blank_bitmaps[0],
				    TILE_WIDTH, TILE_HEIGHT );
    if (pix_id == (Pixmap)0)
    {
	die( "unable to create Pixmap\n", 0 );
    }
    else
    {
	Blank_tile = pix_id;
    }
    pix_id = XCreateBitmapFromData( display, Draw,
				    BandW_blank_bitmaps[1],
				    TILE_WIDTH, TILE_HEIGHT );
    if (pix_id == (Pixmap)0)
    {
	die( "unable to create Pixmap\n", 0 );
    }
    else
    {
	Black_tile = pix_id;
    }
}

show_simple_tile( pix, xcoord, ycoord )
    Pixmap pix;
    int xcoord, ycoord;
{
    if (!BandW)
	XCopyArea( display, pix, Draw, ctxFG, 0, 0,
		TILE_WIDTH, TILE_HEIGHT, xcoord, ycoord);
    else
	XCopyPlane( display, pix, Draw, ctxFG, 0, 0,
		TILE_WIDTH, TILE_HEIGHT, xcoord, ycoord, 1);
}

int
show_tile( index, flag )
    int index, flag;
{
    if (flag == NORMAL_TILE)
    {
	show_simple_tile( board[index]->image,  board[index]->x,
			board[index]->y);
    }
    else if (flag == HILITE_TILE)
    {
	if (!BandW)
	    XCopyPlane( display, Black_tile, Draw, ctxXOR, 0, 0,
			TILE_WIDTH, TILE_HEIGHT, board[index]->x,
			board[index]->y, 1);
	else
	    XCopyPlane( display, board[index]->image, Draw, ctxBG, 0, 0,
			TILE_WIDTH, TILE_HEIGHT, board[index]->x,
			board[index]->y, 1);
    }
    else if (flag == REVERT_TILE)
    {
	if (!BandW)
	    XCopyPlane( display, Black_tile, Draw, ctxXOR, 0, 0,
			TILE_WIDTH, TILE_HEIGHT, board[index]->x,
			board[index]->y, 1);
	else
	    XCopyPlane( display, board[index]->image, Draw, ctxFG, 0, 0,
			TILE_WIDTH, TILE_HEIGHT, board[index]->x,
			board[index]->y, 1);
    }
}

int
show_blank_tile( index)
    int index;
{
	XCopyPlane( display, Blank_tile, Draw, ctxBG, 0, 0,
		   TILE_WIDTH, TILE_HEIGHT, board[index]->x,
		   board[index]->y, 1);
}

int
show_all_tiles( clear_flag )
    int clear_flag;
{
    int i;
    Pixmap pix;

    /*
     * NOTE: this is HIGHLY inefficient, SO FIX IT ALREADY!!!
     */
    if (clear_flag)
    {
	XClearArea( display, Draw, 0, MESS_Y_MAX,
		    PLAY_X_MAX, PLAY_Y_MAX, FALSE );
    }

    /* start at bottom and work to top */

    for (i=0; i<144; i++)
    {
    	if (!board[i]->removed)
    	    show_tile( i, NORMAL_TILE );
    }

    XFlush( display );
}

int
complain()
{
    XBell( display, 100 );
    XFlush( display );
}

init_colormap()
{
    int stat, i;
    unsigned long plane_masks[3];
    unsigned long pixels[8];
    XColor xc;

    cmap = XCreateColormap( display, Draw, viz, AllocAll );
    if (!cmap)
    	die("unable to alloc Colormap\n", 0);

    for (i=0; i<8; i++)
    {
    	stat = XParseColor( display, cmap, Color_names[i], &xc );
    	if (!stat)
	    die("unable to parse color\n");

	xc.pixel = i;
	xc.flags = DoRed | DoGreen | DoBlue;
	XStoreColor( display, cmap, &xc );
    }

    XSetWindowColormap( display, Draw, cmap );

    ForeColor = BLACK;
    BackColor = WHITE;

    XSetWindowBackground( display, Draw, WHITE );
}

show_num_tiles()
{
    int h,t,o;

    h = tile_count / 100;
    t = (tile_count / 10) % 10;
    o = tile_count % 10;
    tiles_left[0] = Number_tiles[h];
    tiles_left[1] = Number_tiles[t];
    tiles_left[2] = Number_tiles[o];
    show_simple_tile( tiles_left[0], X_LOC, YCOORD);
    show_simple_tile( tiles_left[1], X_LOC+W_BASE_TILE, YCOORD);
    show_simple_tile( tiles_left[2], X_LOC+(W_BASE_TILE*2), YCOORD);
    XFlush( display );
}

show_button( pix, xcoord, ycoord )
    Pixmap pix;
    int xcoord, ycoord;
{
    if (!BandW)
	XCopyArea( display, pix, Draw, ctxFG, 0, 0,
		BUTTON_WIDTH, BUTTON_HEIGHT, xcoord, ycoord);
    else
	XCopyPlane( display, pix, Draw, ctxFG, 0, 0,
		BUTTON_WIDTH, BUTTON_HEIGHT, xcoord, ycoord, 1);
}

show_message( msg, xcoord, ycoord )
    char *msg;
    int xcoord, ycoord;
{
    XDrawImageString( display, Draw, ctxFG, xcoord, ycoord, msg, strlen(msg) );
}

show_titles()
{
    int i;
    int y = YCOORD - ((l_height(big_font) * 3) / 2);

    center_message( "MAHJONGG", 1 );

    show_message ("Tiles Remaining", (X_LOC + 20), y);
    show_message ("Board Number", middle, y);


    for (i = 0; i < NBUTTONS; i++)
    {
	show_button (Button_tiles[i], middle + (80 * i), YCOORD);
    }

    update_message ("                              " );

}

redraw_all()
{
    XClearWindow( display, Draw );
    show_all_tiles( FALSE );
    show_num_tiles();
    show_titles();
}
