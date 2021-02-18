/*
** Xtank
**
** Copyright 1988 by Terry Donahue
**
** gr.c
*/

#include "xtank.h"
#include "gr.h"

extern char *malloc();

int num_terminals = 0;
Terminal *terminal[MAX_TERMINALS];

/* Application information for the graphics toolkit */
char *program_name = "xtank";

/* The current terminal */
Terminal *term;

/*
** Sets the global terminal pointer term to the specified terminal number.
*/
set_terminal(terminal_num)
     int terminal_num;
{
  if((terminal_num < num_terminals) && (terminal_num >= 0)) {
    term = terminal[terminal_num];
    set_video((Video *) term->video);
  }
}

/*
** Opens the specified display and makes a terminal with it.
** Opens and maps windows, sets the cursor and makes objects.
** Returns -1 if terminal could not be made.
*/
make_terminal(display_name)
     char *display_name;
{
  Boolean taken[MAX_TERMINALS];
  Terminal *t;
  int i;

  /* Check if we have room for another terminal */
  if(num_terminals >= MAX_TERMINALS) return -1;

  t = (Terminal *) malloc(sizeof(Terminal));
  t->video = (char *) make_video(display_name);
  if(t->video == NULL) {
    free((char *) t);
    return -1;
  }

  /* Set active terminal so we can initialize it */
  term = t;
  if(open_windows()) {
     close_terminal(term);
    return -1;
  }
  set_cursor(CROSS_CURSOR);

  /* Map the windows and wait for the animation window to get exposed */
  map_windows();

  /* Print out a message to keep the user happy while objects are being made */
  draw_text_rc(ANIM_WIN,0,0,"Initializing terminal...",M_FONT,WHITE);
  sync_output(TRUE);

  if(make_objects()) {
    close_terminal(term);
    return -1;
  }

  /* Unexpose all the terminal's windows */
  for(i = 0 ; i < MAX_WINDOWS ; i++)
    expose_win(i,FALSE);

  /* Figure out the lowest available terminal number */
  for(i = 0 ; i < num_terminals + 1 ; i++)
    taken[i] = FALSE;
  for(i = 0 ; i < num_terminals ; i++)
    taken[terminal[i]->num] = TRUE;
  for(i = 0 ; i < num_terminals + 1 ; i++)
    if(!taken[i]) {
      t->num = i;
      break;
    }

  terminal[num_terminals++] = t;

  /* Setup the message menu system for the terminal */
  init_msg_terminal(t->num);

  /* Initialize 3D values for terminal */
  init_terminal_3d(t);

  return 0;
}

/*
** Synchronizes all terminals.
*/
sync_terminals(discard)
     Boolean discard;
{
  Video *video[MAX_TERMINALS];
  int i;

#ifdef MULTI_SYNC
  /* Synchronize in parallel using multi_sync */
  for(i = 0 ; i < num_terminals ; i++)
    video[i] = (Video *) terminal[i]->video;

  multi_sync(video,num_terminals,discard);
#else
  /* Flush output to all terminals and then synchronize in serial */
  for(i = 0 ; i < num_terminals ; i++) {
    set_terminal(i);
    flush_output();
  }

  for(i = 0 ; i < num_terminals ; i++) {
    set_terminal(i);
    sync_output(discard);
  }
#endif
}

/*
** Frees all storage for the terminal.
*/
close_terminal(t)
     Terminal *t;
{
  close_video((Video *) t->video);
  free((char *) t);
}

/*
** Opens all the windows used in the game.
** Returns non-zero value if a window could not be made.
*/
open_windows()
{
  int i;
  int ret = 0;

  ret += make_window(ANIM_WIN,ANIM_WIN_X,ANIM_WIN_Y,ANIM_WIN_WIDTH,
		     ANIM_WIN_HEIGHT,0);
  ret += make_window(GAME_WIN,GAME_WIN_X,GAME_WIN_Y,GAME_WIN_WIDTH,
		     GAME_WIN_HEIGHT,BORDER);
  ret += make_window(CONS_WIN,CONS_WIN_X,CONS_WIN_Y,CONS_WIN_WIDTH,
		     CONS_WIN_HEIGHT,BORDER);
  ret += make_window(MAP_WIN,MAP_WIN_X,MAP_WIN_Y,MAP_WIN_WIDTH,
		     MAP_WIN_HEIGHT,BORDER);
  ret += make_window(HELP_WIN,HELP_WIN_X,HELP_WIN_Y,HELP_WIN_WIDTH,
		     HELP_WIN_HEIGHT,BORDER);
  ret += make_window(MSG_WIN,MSG_WIN_X,MSG_WIN_Y,MSG_WIN_WIDTH,
		     MSG_WIN_HEIGHT,BORDER);

  for(i = 0 ; i < MAX_STAT_WINDOWS ; i++)
    ret += make_window(STAT_WIN + i,
		       STAT_WIN_X,
		       STAT_WIN_Y + STAT_WIN_HEIGHT * i,
		       STAT_WIN_WIDTH,STAT_WIN_HEIGHT,STAT_BORDER);
  return ret;
}

/*
** Maps all the non-battle windows and waits for the animation window
** to be exposed.
*/
map_windows()
{
  Event event;
  int w,num_events;

  /* Map all the windows */
  for(w = 0 ; w < STAT_WIN ; w++)
    map_window(w);

  /* Wait for the animation window to be exposed */
  do {
    num_events = 1;
    get_events(&num_events,&event);
  } while(!win_exposed(ANIM_WIN));
}

/*
** Clears all the non-battle windows.
*/
clear_windows()
{
  int w;

  for(w = 0 ; w < STAT_WIN ; w++)
    clear_window(w);
}

/*
** Maps a battle window for each vehicle.
*/
map_battle_windows()
{
  extern int num_vehicles;
  int w,mn;

  mn = STAT_WIN + min(MAX_STAT_WINDOWS,num_vehicles);
  for(w = STAT_WIN ; w < mn ; w++)
    map_window(w);
}

/*
** Unmaps all the battle windows that are up.
*/
unmap_battle_windows()
{
  int w;

  for(w = STAT_WIN ; w < STAT_WIN + MAX_STAT_WINDOWS ; w++)
    unmap_window(w);
}
