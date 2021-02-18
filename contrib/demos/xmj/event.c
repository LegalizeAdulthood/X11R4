/*
 * $XConsortium: event.c,v 1.5 89/05/08 14:50:50 keith Exp $
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


/*
** This file has the event handlers for the background and
** tiles in the play panel
*/

#include <stdio.h>
#include <sys/types.h>

#include "mj.h"

/* external definitions */

extern Frame		main_frame;
extern Panel		play_panel, message_panel;
extern Panel_item	TL_hundred;
extern Panel_item	TL_ten;
extern Panel_item	TL_one;
extern Panel_item	message;
extern Panel_item	tile[144];
extern Panel_item	tiles_left[3];
extern Panel_item	board_num;
extern Cursor		play_cursor;

extern boolean		BandW;
extern Tiles		*board[144];
extern int		tile_count;
extern int		seed;
extern Selected		last_item;
extern Selected		selected[2];
extern int 		middle;

/* local globals */

Selected		undo_tiles[144][2];
int			undo_count;
boolean			help_mode = FALSE;

/*******************************************/

void help_proc(event)
    Event		*event;
{
    int 		i;
    Tiles 		*data[2];
    static int	parse[2];

    if(!(selected[0].filled)) {
	if(!(help_mode)) { /* Just starting. init and recall */

	    help_mode = TRUE;
	    parse[0] = 143;
	    parse[1] = 142;
	    help_proc(event);

	} else { /* find next match */
	    for(; parse[0] >= 0; parse[0]--) {
	    	if (!((board[parse[0]]->top_free &&		    /* uncovered */
		    (board[parse[0]]->left_free || board[parse[0]]->right_free) && /* open */
		    (!(board[parse[0]]->removed)))))  /* not already used */
		    continue; /* not available go to next */

		for(; parse[1] >= 0; parse[1]--) { /* check for second tile */
	    	    if ((board[parse[0]]->value == board[parse[1]]->value) && /* right value */
			(parse[0] != parse[1]) &&    /* different item */
			(board[parse[1]]->top_free &&		    /* uncovered */
			(board[parse[1]]->left_free || board[parse[1]]->right_free) && /* open */
			(!(board[parse[1]]->removed)))) { /* not already used */

			/* Found a match, show it */

			/* flag found items */
			selected[0].filled = TRUE;
			selected[1].filled = TRUE;

			/* fake in some data */
			selected[0].item = board[parse[0]];
			selected[0].item_index = parse[0];
			selected[0].event = *event;

			selected[1].item = board[parse[1]];
			selected[1].item_index = parse[1];
			selected[1].event = *event;

			/* Muppet news flash */

			show_tile( selected[0].item_index, HILITE_TILE );
			show_tile( selected[1].item_index, HILITE_TILE );

    			/* show next move message */
			update_message("Show next move? [Y] [] [N]");

			parse[1]--; /* do loop step */
			return; /* all done this rotation */

		    }
		} /* else go to next */

		parse[1] = parse[0] - 2; /* going around again */

	    }

	    /* no more moves beep and show message */

	    update_message("No more moves. [DONE] [] []");
	    complain();
        }

    } else { /* search for available match */

	data[0] = selected[0].item;

	for(i = 143; i >= 0; i--) {
	    if ((board[i]->value == data[0]->value) && /* right value */
		(board[i] != selected[0].item) &&    /* different item */
		(board[i]->top_free &&		    /* uncovered */
		(board[i]->left_free || board[i]->right_free) && /* open */
		(!(board[i]->removed)))) { /* not already used */

		/* found one */

		/* fake in some selected data */

		selected[1].item = board[i];
		selected[1].item_index = i;
		selected[1].event = *event;
		selected[1].filled = TRUE;

		/* turn on preview */

		show_tile( selected[1].item_index, HILITE_TILE );

		/* set confirm message */

		update_message("Please confirm. [Y] [] [N]");

		/* return to sender */

		return;

	    }
	}

        selected[0].filled = FALSE;
	show_tile( selected[0].item_index, REVERT_TILE );
	complain();
    }
}

void remove_tiles(REMOVE)
boolean	REMOVE;

{
Tiles	*data[2];
int	 i;
int	 tiles_left_hun;
int	 tiles_left_ten;
int	 tiles_left_one;
Pixwin  *pw;
/* Rect    *r; */

    if (REMOVE) {
	/* get data from items to be removed */
	data[0] = selected[0].item;
	data[1] = selected[1].item;

    } else {
	/* get data from items to be replaced */
	data[0] = undo_tiles[undo_count][0].item;
	data[1] = undo_tiles[undo_count][1].item;
    }

    /* adjust adjacent tiles */
    for(i = 0; i < 2 && data[0]->left_next[i] != 999;
        board[data[0]->left_next[i]]->right_free = REMOVE, i++);
    for(i = 0; i < 2 && data[1]->left_next[i] != 999;
        board[data[1]->left_next[i]]->right_free = REMOVE, i++);
    for(i = 0; i < 2 && data[0]->right_next[i] != 999;
        board[data[0]->right_next[i]]->left_free = REMOVE, i++);
    for(i = 0; i < 2 && data[1]->right_next[i] != 999;
        board[data[1]->right_next[i]]->left_free = REMOVE, i++);

    /* adjust covered tiles and images */
    for (i = 0;
         i < 4 && data[0]->covered[i] != 999;
         board[data[0]->covered[i]]->top_free = REMOVE, i++)
    {
	tile[data[0]->covered[i]] =
		(REMOVE || data[0]->covered[i] >= 139)
		? board[data[0]->covered[i]]->image : Blank_tile;
    }
    for (i = 0;
         i < 4 && data[1]->covered[i] != 999;
         board[data[1]->covered[i]]->top_free = REMOVE, i++)
    {
	tile[data[1]->covered[i]] =
		(REMOVE || data[1]->covered[i] >= 139)
		? board[data[1]->covered[i]]->image : Blank_tile;
    }

    /* set removed flags */

    data[0]->removed = REMOVE;
    data[1]->removed = REMOVE;

    if (REMOVE) {
	/* turn off preview */
	show_blank_tile( selected[0].item_index );
	show_blank_tile( selected[1].item_index );
    } else {  /* check to see if previewing an item and un-preview and select */
	if (selected[0].filled) {
	    show_tile( selected[0].item_index, REVERT_TILE );
	    selected[0].filled = FALSE;
	}
    }

    /* fix playing field */
    show_all_tiles( FALSE );

    /* deselect tiles */

    selected[0].filled = FALSE;
    selected[1].filled = FALSE;

    /* fix tile counter */

    tile_count += (REMOVE) ? -2 : 2;

    tiles_left_hun = tile_count / 100;
    tiles_left_ten = (tile_count - (tiles_left_hun * 100)) / 10;
    tiles_left_one = tile_count - (tiles_left_hun * 100) - (tiles_left_ten * 10);

    show_num_tiles();

    if (REMOVE) {

	/* update undo_count */
	undo_count++;

	/* update removed array */
	undo_tiles[undo_count][0].item = selected[0].item;
	undo_tiles[undo_count][0].filled = TRUE;

	undo_tiles[undo_count][1].item = selected[1].item;
	undo_tiles[undo_count][1].filled = TRUE;

	/* remove confirm message */

	update_message("                                               ");

	/* check for clean board and congrat them */

	if ( tiles_left_hun == 0 && tiles_left_ten == 0 &&
	     tiles_left_one == 0)
	    {
		update_message("Congratulations!! Press 'AGAIN' or 'NEW'");
	    }
    } else { /* decrement undo_count */
	undo_tiles[undo_count][0].filled = FALSE;
	undo_tiles[undo_count][1].filled = FALSE;
	undo_count--;
    }
}

void play_back_proc(event)
Event		*event;
{

    if (!(selected[0].filled) && help_mode) { /* stop helping */
	if (event_button(event) == MS_LEFT) { /* all done */

    	    /* remove message */

	    update_message("                                               ");
	    /* exit help_mode */

	    help_mode = FALSE;

	}
    } else { /* doing confirm or next help */

    if (selected[1].filled) { /* doing confirm  or next help */

        switch (event_button(event)) {

	    case MS_LEFT:
		    /* confirmed selection */
		    if (event_is_up(event)) {
			if (help_mode) { /* do next help */

			    /* cancel preview of selected tiles */
			    show_tile( selected[0].item_index, REVERT_TILE );
			    show_tile( selected[1].item_index, REVERT_TILE );

			    /* Clean up selected's variables */
			    selected[0].filled = FALSE;
			    selected[1].filled = FALSE;

    			    /* remove confirm message */
	    update_message("                                               ");

			    /* do next help */
			    help_proc(event);

			} else { /* confirmed selection. remove them */
			    remove_tiles(TRUE);
			}
		    }
		    break;

	    case MS_RIGHT:
		    /* refused selection */
		    if (event_is_down(event)) {
			    /* cancel preview of selected tiles */
			    show_tile( selected[0].item_index, REVERT_TILE );
			    show_tile( selected[1].item_index, REVERT_TILE );

			    /* Clean up selected's variables */
			    selected[0].filled = FALSE;
			    selected[1].filled = FALSE;

    			    /* remove confirm message */
	    update_message("                                               ");

			    /* if in help mode toggle out */
			    if (help_mode) help_mode = FALSE;

		    }
		    break;
	}
    } else {

        /* check for help requests */

	if ((event_button(event) == MS_MIDDLE) && event_is_up(event) &&
	     selected[0].filled) {

	    help_proc(event);

	} else  /* or MS_LEFT up outside of playing area */
	    if ((event_button(event) == MS_LEFT) && event_is_up(event) &&
		 last_item.filled) { /* went down on something */

		complain();
		last_item.filled = FALSE;

		if (!selected[0].filled || (selected[0].item != last_item.item)) {
		    /* not aborting deselection */
		    show_tile( last_item.item_index, REVERT_TILE );
		}
	    }
    }
   }
}

void
play_event_proc( event )
    Event *event;
{
Tiles	*data;
int	value;
int	i;
int	x;
int	y;
Tiles   *item;
int     item_index;

    /* check to see if in help_mode */

    if (help_mode) {
	play_back_proc(event);
	return;
    }

    /* check to see if just confirming */

    if (selected[1].filled) {
	play_back_proc(event);
	return;
    }

    /* translate item to top level available */

	/* if in overlap area, must check for stacks, so do top first */

	item = NULL;
	for (i = 143; i >= 0 ; i--) { /* check from top to bottom */

	    x = board[i]->x;
	    y = board[i]->y;

	    if ( (x <= event_x(event)) &&
		 (event_x(event) <= x + W_BASE_TILE) &&
		 (y <= event_y(event)) &&
		 (event_y(event) <= y + H_BASE_TILE) ) { /* right spot */

	        if ( !(board[i]->removed) )
	        {
		   item = board[i]; /* got it */
		   item_index = i;
		   break;
		}
#ifdef FAST_LOOK_UNDER
		else
		{ /* look on next layer down */
		   /* take first covered tile and add 1 for loop */
		   if (i != 143) i = board[i]->covered[0] + 1;
		}
#endif
	    } /* wrong location. try again */
	} /* next loop */
	if (!item)
	{
	    play_back_proc( event );
/*	    complain(); */
	    return;
	}
    /* get data from item selected */

    data = item;
    value = data->value;

    switch(event_button(event)) {

	case MS_LEFT:
	    /* Left button down begin selection */
		if (event_is_down(event)) {
    		    if ( data->top_free &&
	    		         (data->left_free || data->right_free))
    		    {
			if ( !(selected[0].filled) ||
			      (last_item.item != item) )
			{ /* don't double up */
			    show_tile( item_index, HILITE_TILE );
			}

		        /* file last_item slected for future reference */
		        last_item.item = data;
			last_item.item_index = item_index;
			last_item.event = *event;
		        last_item.filled = TRUE;

		    } else { /* beep at them */
			complain();
		    }

		} else {

		/* button went up, check same item as down and grab it */

		    if ((last_item.filled)) { /* else ignore it */

			if (item != last_item.item) { /* beep at them */

			    last_item.filled = FALSE;
			    complain();
			    if (!selected[0].filled ||
			         (selected[0].item != last_item.item))
			    { /* not aborting deselect */
				show_tile( last_item.item_index, REVERT_TILE );

			    }

		        } else {

			    if (!(selected[0].filled)) {

				/* fill first selection if empty */
				selected[0].item = item;
				selected[0].item_index = item_index;
				selected[0].event = *event;
				selected[0].filled = TRUE;

				/* clear last item */

				last_item.filled = FALSE;

		    	    } else {

				if (item == selected[0].item)
				{ /* deselect item */
				    show_tile( selected[0].item_index,
				    		REVERT_TILE );

				    /* clear last item and selected[0] */

				    selected[0].filled = FALSE;
				    last_item.filled   = FALSE;

				} else {
				    data = selected[0].item;
				    if ( value == data->value) {
				        /* fill second and show confirm message */

				        selected[1].item = item;
					selected[1].item_index = item_index;
					selected[1].event = *event;
				        selected[1].filled = TRUE;

				        /* clear last item */

				        last_item.filled = FALSE;
				update_message("Please confirm. [Y] [] [N]");

				    } else { /* beep at them */

					show_tile( last_item.item_index,
						   REVERT_TILE );
					show_tile( selected[0].item_index,
						   REVERT_TILE );
					complain();
			    		last_item.filled = FALSE;
			    		selected[0].filled = FALSE;

				    }
				}
			    }
		    	}
		    }
		}
		break;

	    case MS_MIDDLE:

		if (event_is_up(event) && selected[0].filled)
		{ /* request for help */
			help_proc(event);
		}
		break;

		/* and all else shall pass */
    }
}

void quit_proc()
{
    exiting = TRUE;
}

void new_proc()
{
    seed = random() % 20011;
    build_image(FALSE);
    place_tiles(FALSE);
    redraw_all();
}

void again_proc()
{
    build_image(TRUE);
    place_tiles(FALSE);
    redraw_all();
}

void undo_proc()
{
    if(undo_count < 0)
	complain();
    else
	remove_tiles(FALSE);
}

void board_num_proc()
{
    build_image(FALSE);
    place_tiles(FALSE);
}

msg_event_proc(event)
    XEvent *event;
{
int	i;
int	x;
int	y;

    for (i = 0; i < NBUTTONS ; i++)
    {
	x = middle + (i * 80);
	y = YCOORD;

	if ( (x <= event_x(event)) &&
	     (event_x(event) <= x + BUTTON_WIDTH) &&
	     (y <= event_y(event)) &&
	     (event_y(event) <= y + BUTTON_HEIGHT) )
	{ /* right spot */
	    switch( i )
	    {
	    	case 0:
		    if (event_is_up(event))
			help_proc(event);
	    	    break;
	    	case 1:
		    if (event_is_down(event))
			again_proc();
	    	    break;
	    	case 2:
		    if (event_is_down(event))
			new_proc();
	    	    break;
	    	case 3:
		    if (event_is_down(event))
			undo_proc();
	    	    break;
	    	case 4:
		    if (event_is_down(event))
			quit_proc();
	    	    break;
	    }
	    return;
	}

    } /* next loop */
}
