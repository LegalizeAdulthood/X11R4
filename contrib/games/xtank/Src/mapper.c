/*
** Xtank
**
** Copyright 1988 by Terry Donahue
**
** mapper.c
*/

#include "xtank.h"
#include "gr.h"
#include "map.h"

/*
** Defines for the north and west wall symbols.
** Should be >255 to avoid conflicting with the landmarks.
*/
#define NORTH_SYM 256
#define WEST_SYM 257
#define NORTH_DEST_SYM 258
#define WEST_DEST_SYM 259

#define draw_marker(x,y) \
  draw_filled_square(MAP_WIN,x,y,MAP_BOX_SIZE-3,DRAW_XOR,WHITE)

#define draw_grid_marker(x,y) \
  draw_marker(grid2map(x+NUM_BOXES/2) + 2,grid2map(y+NUM_BOXES/2) + 2)

/*
** Handles all mapper actions for the specified vehicle.
*/
special_mapper(v,record,action)
     Vehicle *v;
     char *record;
     unsigned int action;
{
  Mapper *m;
  Symbol *s;
  Box *b,*mb;
  int left,top,right,bottom;
  int left_x,top_y,line_x,line_y;
  unsigned int flags;
  int i,x,y;

  m = (Mapper *) record;

  switch(action) {
    case SP_update:
      /* If the vehicle hasn't move to a new box, do nothing */
      if((v->loc->grid_x == v->old_loc->grid_x) &&
	 (v->loc->grid_y == v->old_loc->grid_y) &&
	 (m->initial_update == FALSE)) {
	m->need_redisplay = FALSE;
	break;
      }

      /* Otherwise, we need to redisplay */
      m->initial_update = FALSE;
      m->need_redisplay = TRUE;
      m->num_symbols = 0;

      /* Update the marker location */
      m->old_marker = m->marker;
      m->marker.x = grid2map(v->loc->grid_x) + 2;
      m->marker.y = grid2map(v->loc->grid_y) + 2;

      left = v->loc->grid_x - (NUM_BOXES + 1)/2;
      top = v->loc->grid_y - (NUM_BOXES + 1)/2;
      right = left + NUM_BOXES;
      bottom = top + NUM_BOXES;

      left_x = grid2map(left);
      top_y = grid2map(top);

      /* Find inconsistencies between the maze and the map */
      for(x=left, line_x=left_x ; x <= right ; x++, line_x += MAP_BOX_SIZE)
	for(y=top, line_y=top_y ; y <= bottom ; y++, line_y += MAP_BOX_SIZE) {
	  b = &box[x][y];
	  mb = &m->map[x][y];

	  flags = b->flags ^ mb->flags;
	  if(flags & NORTH_WALL && y != top) {
	    s = &m->symbol[m->num_symbols++]; /* add symbol to display list */
	    s->type = NORTH_SYM;
	    s->x = line_x;
	    s->y = line_y;
	    m->map[x][y].flags ^= NORTH_WALL; /* Fix the map */
	  }

	  if(flags & WEST_WALL && x != left) {
	    s = &m->symbol[m->num_symbols++];
	    s->type = WEST_SYM;
	    s->x = line_x;
	    s->y = line_y;
	    m->map[x][y].flags ^= WEST_WALL;
	  }

	  if(b->type != mb->type) {
	    /* Change from one symbol to the next by xoring both on */
	    if(b->type != NORMAL) {
	      s = &m->symbol[m->num_symbols++];
	      s->type = b->type;
	      s->x = line_x;
	      s->y = line_y;
	    }
	    if(mb->type != NORMAL) {
	      s = &m->symbol[m->num_symbols++];
	      s->type = mb->type;
	      s->x = line_x;
	      s->y = line_y;
	    }

	    mb->type = b->type;

	    /* Put a symbol in the landmark array so xtank programs
	    ** can find the landmarks more easily
	    */
	    if(m->num_landmarks < MAX_LANDMARKS) {
	      s = &m->landmark[m->num_landmarks++];
	      s->type = mb->type;
	      s->x = x;
	      s->y = y;
	    }
	  }
      }
      break;
    case SP_redisplay:
      if(!m->need_redisplay) break;

      /* Redisplay the symbols that have changed */
      for(i = 0 ; i < m->num_symbols ; i++)
	draw_symbol(&m->symbol[i]);

      /* Redisplay the vehicle marker */
      draw_marker(m->old_marker.x,m->old_marker.y);
      draw_marker(m->marker.x,m->marker.y);
      break;
    case SP_draw:
    case SP_erase:
      /* Draw/erase all the maze symbols that are on the map */
      draw_full_map(m->map);

      /* Draw/erase the vehicle marker */
      draw_marker(m->marker.x,m->marker.y);
      break;
    case SP_activate:
      /* compute initial location of the marker */
      m->marker.x = grid2map(v->loc->grid_x) + 2;
      m->marker.y = grid2map(v->loc->grid_y) + 2;

      m->initial_update = TRUE;
      break;
    case SP_deactivate:
      break;
  }
}

/*
** Draws all walls and landmarks in specified map onto the map window.
*/
draw_full_map(map)
     Box map[GRID_WIDTH][GRID_HEIGHT];
{
  int x,y,line_x,line_y;
  Symbol sym;
  Box *b;

  for(x = 0, line_x = MAP_BORDER ; x < GRID_WIDTH ;
      x++, line_x += MAP_BOX_SIZE)
    for(y = 0, line_y = MAP_BORDER ; y < GRID_HEIGHT ;
	y++, line_y += MAP_BOX_SIZE) {
      b = &map[x][y];
      sym.x = line_x;
      sym.y = line_y;

      if(b->flags & NORTH_WALL) {
	sym.type = NORTH_SYM;
	draw_symbol(&sym);
      }
      if(b->flags & WEST_WALL) {
	sym.type = WEST_SYM;
	draw_symbol(&sym);
      }
      if(b->type != NORMAL) {
	sym.type = b->type;
	draw_symbol(&sym);
      }
    }
}

/*
** Xors the symbol s onto the map.
*/
draw_symbol(s)
     Symbol *s;
{
  extern Object *landmark_obj[];
  Picture *pic;

  switch(s->type) {
    case NORTH_SYM:
    case NORTH_DEST_SYM:
      draw_hor(MAP_WIN,s->x,s->y,MAP_BOX_SIZE,DRAW_XOR,WHITE);
      break;
    case WEST_SYM:
    case WEST_DEST_SYM:
      draw_vert(MAP_WIN,s->x,s->y,MAP_BOX_SIZE,DRAW_XOR,WHITE);
      break;
    default:
      pic = &landmark_obj[1]->pic[s->type - 1];
      draw_picture(MAP_WIN,s->x,s->y,pic,DRAW_XOR,WHITE);
    }
}

/*
** Displays the entire map. Highlights the location of the terminal.
*/
full_mapper(status)
     unsigned int status;
{
  if(status == ON) {
    draw_full_map(box);
    draw_grid_marker(term->loc.grid_x,term->loc.grid_y);
  }
  else if(status == REDISPLAY) {
    if(term->loc.grid_x != term->old_loc.grid_x ||
       term->loc.grid_y != term->old_loc.grid_y) {
      draw_grid_marker(term->old_loc.grid_x,term->old_loc.grid_y);
      draw_grid_marker(term->loc.grid_x,term->loc.grid_y);
    }
  }
}
