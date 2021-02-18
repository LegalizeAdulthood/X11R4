/*
 * game.c -- xrobots
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

#include <X11/Intrinsic.h>
#include "xrobots.h"

/* some of these are global */
int human_x, human_y, last_human_x, last_human_y;

int robot_array[MAXX][MAXY],
    robot_array_bak[MAXX][MAXY];	
	/* These arrays are filled with the robots and the heaps 
	 * They do not include the player(human).
	 */
int	score,
	num_robots,
	level,
	game_active,
	sonic_used;

static void autochase();

/*----------------------------------------------------------------------*/
void
new_game()
{
  score = 0;
  level = 0;
  sonic_used = 1;
  game_active = 1;
  new_level();
  update_score(score);
}

/*----------------------------------------------------------------------*/
void
add_score(n)
  int n;
{
  score += 10 * n; /* ten points for each wasted robot. */
  num_robots -= n;
  if(n) update_score(score);
}

/*----------------------------------------------------------------------*/
void
new_level()
{
  int x,y,tmp;

  reset_sonic_button();
  if(!sonic_used) {
    score += 50; 	/* bonus for not using sonic screwdriver on last level*/
    update_score(score);
  }
  sonic_used = 0;
  level++;
  num_robots = tmp = level*5;

  for_each                            /* clean out both the robot arrays */
  {
    robot_array[x][y] = EMPTY;
    robot_array_bak[x][y] = EMPTY;
  }
  
  human_x = (int)random()%MAXX;                  /* assign human to new space */
  human_y = (int)random()%MAXY;
  robot_array[human_x][human_y] = ROBOT;

  while(tmp--)
  {
    x = (int)random()%MAXX;
    y = (int)random()%MAXY;
    if(robot_array[x][y] == ROBOT) tmp++;        /* space already occupied */
    robot_array[x][y] = ROBOT;
  }
  robot_array[human_x][human_y] = EMPTY;
  display_level();
  display_possible_moves();
}

/*----------------------------------------------------------------------*/
int
chase()
{
  /* chase() returns the number of robots that were wasted in each call. */
  /* after each call, be sure to check if all the robots are dead */
  int x,y;
  int newx,newy;
  int num_wasted = 0;

  for_each
  {
    robot_array_bak[x][y] = robot_array[x][y];
    if(robot_array[x][y] != HEAP)
      robot_array[x][y] = 0;
  }

  for_each
  {
    if(robot_array_bak[x][y] != ROBOT)
      continue;

    if(x>human_x)                           /* move toward the human */
      newx = x-1;
    else  
      if(x<human_x)
        newx = x+1;
      else
        newx = x;

    if(y>human_y) 
      newy = y-1;
    else  
      if(y<human_y)
        newy = y+1;
      else
        newy = y;

#   ifdef DEBUG
    printf("moving (%d,%d) to (%d,%d)\n",x,y,newx,newy);
#   endif

    /* check to see if a robot or heap was already in that spot */
    if(robot_array[newx][newy] == ROBOT)
    {
      robot_array[newx][newy] = HEAP;
      num_wasted += 2;
      continue;
    }
    if(robot_array[newx][newy] == HEAP)
    {
      num_wasted++;
      continue;
    }
    /* move robot to new location */
    robot_array[newx][newy] = ROBOT;
  }

  return(num_wasted);
}

/*----------------------------------------------------------------------*/
void
undo_chase()
{
  int x,y;
  for_each
    robot_array[x][y] = robot_array_bak[x][y];
}

/*----------------------------------------------------------------------*/
void
teleport()
{
  int num_wasted;

  if(!game_active) return;
  do
  {
    human_x = (int)random()%MAXX;        /* pick a spot not already occupied */
    human_y = (int)random()%MAXY;
  }
  while(robot_array[human_x][human_y] != EMPTY);

  show_teleport();

  num_wasted = chase(0);

  if(robot_array[human_x][human_y] != EMPTY)
  {
    /* death... */
    undo_chase();    
    /* it is a matter of preference to clear the screen when you die... */
    display_level();
    do_death();
    check_score(score);
    game_active = 0;
    return;
  }

  display_level();

  score += num_robots;       /* bonus for teleporting */
  score += num_wasted * 10;  /* score for any that collided */
  num_robots -= num_wasted;
  update_score(score);

  if(!num_robots) 
    new_level();
  else 
    display_possible_moves();
  auto_teleport();
  XFlush(display);
}

/*----------------------------------------------------------------------*/
void
sonic_screwdriver()
{
  /*
   * a trick - remove the neighboring robots by calling chase() and 
   * letting them crash into each other in the human's spot, then clear out 
   * the human's position of the remains.
   */
  int num_wasted;

  if(!game_active) return;
  if(sonic_used) return;
  sonic_used = 1;
  show_sonic();
  num_wasted = chase(1);
  if(robot_array[human_x][human_y] == ROBOT)
    add_score(1);
  robot_array[human_x][human_y] = EMPTY;

  last_human_x = human_x;
  last_human_y = human_y;
  show_movement();

  add_score(num_wasted);
  if(!num_robots) 
    new_level();
  else 
    display_possible_moves();
  auto_teleport();
  XFlush(display);
}

/*----------------------------------------------------------------------*/
void
wait_for_em()
{
  /*
   * the trick - call chase() until any robot is in the same spot as the
   * human, then backout as if the last chase() didn't happen.
   */
  int num_wasted;

  if(!game_active) return;

  while(1) {
    num_wasted = chase(1);
    if(robot_array[human_x][human_y] != EMPTY) {
      /* backout of latest chase() and break loop */
      undo_chase();

      if(diewaiting) {  	/* for those risk takers out there */
        display_level();
        do_death();
        check_score(score);
        game_active = 0;
        return;
      }

      break;
    }
    add_score(num_wasted);
    if(!num_robots)
	break;
    if(showjumps)
      show_movement();
    XFlush(display);
  }
  if(!num_robots) 
    new_level();
  else
    if(!showjumps)
      display_level();
  display_possible_moves();
  auto_teleport();
  XFlush(display);
}

/*----------------------------------------------------------------------*/


int
can_go(x,y)
  int x,y;
{
/* check if (x,y) is a legit move. */
  if( INYRANGE(y-1) ) {
    if( INXRANGE(x-1) )
      if(robot_array[x-1][y-1] == ROBOT)  return 0;
    if( INXRANGE(x) )
      if(robot_array[x][y-1] == ROBOT)  return 0;
    if( INXRANGE(x+1) )
      if(robot_array[x+1][y-1] == ROBOT)  return 0;
  }

  if( INYRANGE(y) ) {
    if( INXRANGE(x-1) )
      if(robot_array[x-1][y] == ROBOT)  return 0;
    if( INXRANGE(x) ) {
      if(robot_array[x][y] == ROBOT)  return 0;
      if(robot_array[x][y] == HEAP)   return 0;
      }
    if( INXRANGE(x+1) )
      if(robot_array[x+1][y] == ROBOT)  return 0;
  }

  if( INYRANGE(y+1) ) {
    if( INXRANGE(x-1) )
      if(robot_array[x-1][y+1] == ROBOT)  return 0;
    if( INXRANGE(x) )
      if(robot_array[x][y+1] == ROBOT)  return 0;
    if( INXRANGE(x+1) )
      if(robot_array[x+1][y+1] == ROBOT)  return 0;
  }

  if( !INXRANGE(x) )  return 0;
  if( !INYRANGE(y) )  return 0;

  return 1;
}

