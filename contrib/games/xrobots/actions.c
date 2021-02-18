/*
 * actions.c  --  xrobots
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

#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <math.h>
#include "xrobots.h"

extern Widget sonic_command;

void
sonic_action()
{
  static Arg arg = { XtNsensitive, False};
  if(!game_active) return;
  XtSetValues(sonic_command,&arg,1);
  sonic_screwdriver();
}

void
reset_sonic_button()
{
  static Arg arg = { XtNsensitive, True };
  XtSetValues(sonic_command,&arg,1);
}

/*ARGSUSED*/
XtActionProc
do_nothing_action(w,event,params,num_params)
  Widget w;
  XEvent *event;
  String *params;
  Cardinal *num_params;
{
  /* do nothing */
}


/*----------------------------------------------------------------------*/


int 
determine_direction(button_x,button_y)
  int button_x,button_y;
{
/* 
 * Given the mouse's x&y position, this routine determines the direction
 * relative to the player, and returns the result coded into a int.
 */
  float slope, invslope;
  int direction = 0;
  int coord_x = pos_to_coord(human_x) + CELLSIZE/2,
      coord_y = pos_to_coord(human_y) + CELLSIZE/2;

  if( (abs(coord_x - button_x) < (CELLSIZE/2)+2) &&
      (abs(coord_y - button_y) < (CELLSIZE/2)+2)) 
    return(STILL);      /* cursor is directly over the player */ 

  if(button_x - coord_x != 0) {
    slope = fabs((float)(button_y - coord_y) / (float)(button_x - coord_x)); 

    if( button_x > coord_x ) {			/* in coordinates 1 or 4 */
      if( (slope < 2) && (human_x < MAXX) )
        direction = RIGHT;
    }
    else 					/* in coordinates 2 or 3 */
      if( (slope < 2) && (human_x > -1) )
        direction = LEFT;
  }

  if(button_y - coord_y != 0) {
    invslope = fabs((float)(button_x - coord_x) / (float)(button_y - coord_y)); 

    if( button_y > coord_y ) {			/* in coordinates 1 or 2 */
      if( (invslope < 2) && (human_y < MAXY) )
        direction |= DOWN;
    }

    else 					/* in coordinates 3 or 4 */
      if( (invslope < 2) && (human_y > -1) )
        direction |= UP;
  }
  return(direction);
}

/*----------------------------------------------------------------------*/

static int
get_next_position(diff_x, diff_y, mousex, mousey, params, num_params)
  int *diff_x, *diff_y, mousex, mousey, num_params;
  String *params;
{
  int direction;

  *diff_x = *diff_y = 0;

  if(!num_params) {	/* no parameters - the mouse was used pointer */

    direction = determine_direction(mousex,mousey);
    if(!direction)  return -1;

    if(direction & UP)    *diff_y = -1;
    if(direction & DOWN)  *diff_y = 1;
    if(direction & LEFT)  *diff_x = -1;
    if(direction & RIGHT) *diff_x = 1;

    return 0;
  } 

  while(num_params--) {
      /* else pull the direction out of the parameters. */

    if(!strcmp("right",*(params+num_params)))	*diff_x = 1;
    if(!strcmp("left", *(params+num_params)))	*diff_x = -1;
    if(!strcmp("up",   *(params+num_params)))	*diff_y = -1;
    if(!strcmp("down", *(params+num_params)))	*diff_y = 1;
  }
    return 0;
}

/*----------------------------------------------------------------------*/


/*ARGSUSED*/
XtActionProc
move_action(w,event,params,num_params)
  Widget w;
  XButtonEvent *event;
  String *params;
  Cardinal *num_params;
{
/* 
 *  Called to move the player's icon.  This action can be called 
 *  when a mouse button is pressed or when a key is pressed.
 */
  int diff_x, diff_y;
  int num_wasted;

  if(!game_active) return;

  if(get_next_position(&diff_x, &diff_y, event->x, event->y,
                        params, *num_params))
	return;

  last_human_x = human_x;
  last_human_y = human_y;

  if( can_go(human_x+diff_x,human_y+diff_y) ) {
    human_x += diff_x;
    human_y += diff_y;
    num_wasted = chase(0);
    show_movement();
    add_score(num_wasted);
    if(!num_robots)
      new_level();
    else
      display_possible_moves();
    auto_teleport();
    pointer_moved((Widget)0,(caddr_t)0,event);
  }
  XFlush(display);
}



/*ARGSUSED*/
XtActionProc
jump_action(w,event,params,num_params)
  Widget w;
  XButtonEvent *event;
  String *params;
  Cardinal *num_params;
{
/* 
 * don't just move once, move until it can't go any farther.
 */
  int diff_x, diff_y;
  int num_wasted;

  if(!game_active) return;

  if(get_next_position(&diff_x, &diff_y, event->x, event->y,
                        params, *num_params))
	return;

  if(! can_go(human_x+diff_x,human_y+diff_y) ) 
	return;

  while( can_go(human_x+diff_x,human_y+diff_y) ) {
    last_human_x = human_x;
    last_human_y = human_y;
    human_x += diff_x;
    human_y += diff_y;
    num_wasted = chase(0);
    if(showjumps)
      show_movement();
    add_score(num_wasted);
    if(!num_robots) 
      break;
    XFlush(display);
  }
  if(!num_robots)
    new_level();
  else 
    if(!showjumps)
      display_level();
  display_possible_moves();
  auto_teleport();
  pointer_moved((Widget)0,(caddr_t)0,event);
  XFlush(display);
}



/*ARGSUSED*/
XtActionProc
go_here_action(w,event,params,num_params)
  Widget w;
  XButtonEvent *event;
  String *params;
  Cardinal *num_params;
{
/* 
 * This action causes player's icon to try to go to a spot in the 
 * play area.  It stops if a move cannot be made.
 */
  int direction;
  int tmp_human_x, tmp_human_y;
  int num_wasted;

  if(!game_active) return;

  while(direction = determine_direction(event->x,event->y)) {
    if(direction == STILL) break;
    tmp_human_x = human_x;
    tmp_human_y = human_y;

    if(direction & UP)    tmp_human_y--;
    if(direction & DOWN)  tmp_human_y++;
    if(direction & LEFT)  tmp_human_x--;
    if(direction & RIGHT) tmp_human_x++;

    if( !can_go(tmp_human_x,tmp_human_y) ) 
      break;
    last_human_x = human_x;
    last_human_y = human_y;
    human_x = tmp_human_x;
    human_y = tmp_human_y;
    num_wasted = chase(0);
    if(showjumps)
      show_movement();
    add_score(num_wasted);
    if(!num_robots)
      break;
    if(spiffy) 
      pointer_moved((Widget)0,(caddr_t)0,event);
    XFlush(display);
  }
  if(!num_robots)
      new_level();
  else
    if(!showjumps)
      display_level();
  display_possible_moves();
  auto_teleport();
  pointer_moved((Widget)0,(caddr_t)0,event);
  XFlush(display);
}



/*ARGSUSED*/
XtEventHandler
pointer_moved(w, closure, event)
  Widget w;
  caddr_t closure;
  XPointerMovedEvent *event;
{
  if(game_active) 
    /* could probably suck up any other pointer motion events here... */
    update_pointer( determine_direction(event->x,event->y) );
}

