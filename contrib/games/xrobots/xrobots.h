/*
 * xrobots.h  --  xrobots
 * 
 * Copyright 1989 Brian Warkentine
 * 
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided that
 * the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the author's name not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  The author makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 * 
 * THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL THE 
 * AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY 
 * DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN 
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF 
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * The author's current employer has no connection with this software, as it
 * was written before being employed at Sun.  The following address is for 
 * contacting the author only and does not imply any responsibility on the 
 * behalf of Sun Microsystems.
 * 
 * Contact the author at:
 * 	Brian Warkentine  (brianw@Sun.COM)
 * 	Sun Microsystems
 * 	2550 Garcia Avenue
 * 	Mountain View, Ca 94043-1100
 *
 */

/* 
 * from main.c
 */
extern Display 	*display;
extern Window 	playfield;
extern Widget 	playfield_widget;
extern GC 	gc,
		cleargc;
extern Pixel 	fg, 
		bg;
extern Boolean 	spiffy,
		autoteleport,
		autoteleportalways,
		showjumps,
		diewaiting;
extern unsigned int
		chasetime;

extern void 	update_score();

/*
 * from graphics.c
 */
extern XtEventHandler redisplay_level();

extern void 	init_pixmaps(), 
		auto_teleport(),
		display_possible_moves(), 
		display_level(), 
		update_pointer(), 
		free_pixmaps(), 
		do_death(), 
		show_teleport(), 
		show_sonic(), 
		show_movement();

#define CELLSIZE 21

#define pos_to_coord( _pos_ ) (( _pos_ ) * CELLSIZE + 5)

/*
 * from actions.c
 */

extern void 		sonic_action(),
			reset_sonic_button();

extern XtActionProc	do_nothing_action(),
			move_action(),
			jump_action(),
			go_here_action();

extern XtEventHandler 	pointer_moved();

extern int  		determine_direction();

/*
 * from score.c
 */

#ifndef SCORE_FILE
#  define SCORE_FILE "/usr/games/lib/xrobotscores"
#endif

#ifndef MAXSCORES
#  define MAXSCORES 20
#endif

extern void 	check_score(), 
		create_high_score_popup();

extern void	show_scores_callback();

extern char *score_filename;

/*
 * from game.c
 */

#ifndef MAXX
# 	define 	MAXX 35
#endif
#ifndef MAXY
#  	define 	MAXY 20
#endif

#define EMPTY 	0
#define ROBOT 	1
#define HEAP  	2
#define REDRAW 	3

#define LEFT    1
#define RIGHT   2
#define UP      4
#define DOWN    8
#define STILL   16

#define for_each	for(x=0;x<MAXX;x++) \
			  for(y=0;y<MAXY;y++)

/* I know, I KNOW... global variables! */

extern int 	human_x, human_y,
           	last_human_x, last_human_y;

extern int	robot_array[MAXX][MAXY],
		robot_array_bak[MAXX][MAXY];

extern int 	score,
		num_robots,
		game_active,
		sonic_used;

#define MIN(a,b) ((a<b)?a:b)
#define MAX(a,b) ((a>b)?a:b)

#define INXRANGE( _x_ )  (((_x_) >=0) && ((_x_)<MAXX))
#define INYRANGE( _y_ )  (((_y_) >=0) && ((_y_)<MAXY))

extern void 	new_game(), 
		add_score(), 
		new_level();
extern int  	chase();
extern void 	undo_chase(), 
		teleport(), 
		sonic_screwdriver(), 
		wait_for_em();
extern int  	can_go();




