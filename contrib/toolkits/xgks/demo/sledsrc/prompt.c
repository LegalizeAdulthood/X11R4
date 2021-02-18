/*
 *		Copyright IBM Corporation 1989
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
 *	prompt
 *	reprompt
 *	full_prompt
 *	display_msg
 *	erase_msg
 *	display_msg_area
 *	rm_msg_area
 *	restore_msg_area
 */

static char rcsid[] = "$Header: prompt.c,v 4.0 89/09/01 15:55:24 amy Exp $";

#include <strings.h>
#include "key.h"
#include "draw.h"
#include "trans.h"
#include "color.h"
#include "popup.h"
#include "functbl.h"
#include <xgks.h>


#define NO_PROMPT -1

/*
 *  prompt messages
 */

char *prompt_messages[] =
{
	"NOT IMPLEMENTED",					/* 0 */
	"INVALID SELECTION",					/* 1 */
	"SELECT MENU ITEM",					/* 2 */
	"ADIOS AMIGO!",						/* 3 */
	"SELECT FIRST POINT",					/* 4 */
	"SELECT NEXT POINT",					/* 5 */
	"PLACE COLOR IN PALETTE, MIXER OR TUNER",		/* 6 */
	"PICK COLOR FROM WHEEL, PALETTE, MIXER OR TUNER",	/* 7 */
	"SELECT CENTER POINT",					/* 8 */
	"SELECT EXTERIOR POINT",				/* 9 */ 
	"SELECT FIRST POINT OF BOUNDING BOX",			/* 10 */
	"SELECT SECOND POINT OF BOUNDING BOX",			/* 11 */
	"SELECT TEXT LOCATION",					/* 12 */
	"ENTER TEXT",						/* 13 */
	"SELECT OBJECT TO DELETE",				/* 14 */
	"NO OBJECT SELECTED",					/* 15 */
	"SELECT OBJECT TO COPY",				/* 16 */
	"SELECT LOCATION FOR COPY",				/* 17 */
	"SELECT OBJECT TO MOVE",				/* 18 */
	"SELECT LOCATION FOR OBJECT",				/* 19 */
	"NO OBJECTS TO EDIT",					/* 20 */
	"NO ACTION TO UNDO",					/* 21 */
	"ERROR OPENING INPUT FILE",				/* 22 */
	"ERROR OPENING OUTPUT FILE",				/* 23 */
	"SELECT FIRST POINT OF BOUNDING BOX",			/* 24 */
	"SELECT SECOND POINT OF BOUNDING BOX",			/* 25 */
	"NO OBJECTS SELECTED",					/* 26 */
	"SELECT OBJECT TO SPLIT",				/* 27 */
	"ERROR READING INPUT FILE",				/* 28 */
	"ERROR WRITING OUTPUT FILE",				/* 29 */
	"SELECT NEW LOCATION FOR A GRID POINT",			/* 30 */
	"SELECT STATIONERY GRID POINT",				/* 31 */
	"SELECT NEW POSITION OF NEAREST GRID POINT",		/* 32 */
	"INVALID SELECTION: GRID NOT VISIBLE",			/* 33 */
	"SELECT OBJECT TO ROTATE",				/* 34 */
	"ENTER FILE NAME",					/* 35 */
	"SELECT FIRST CONTROL POINT OF CURVE",			/* 36 */
	"SELECT SECOND CONTROL POINT OF CURVE",			/* 37 */
	"SELECT THIRD CONTROL POINT OF CURVE",			/* 38 */
	"SELECT LAST CONTROL POINT OF CURVE",			/* 39 */
	"NO OBJECTS IN PICTURE",				/* 40 */
	"ONE MOMENT, PLEASE.",					/* 41 */
	"NOTHING TO PHOTOGRAPH",				/* 42 */
	"FILE NOT BY CURRENT SLED VERSION"			/* 43 */
};

IDX msgno1 = NO_PROMPT;			/* current msg on screen */
IDX msgno2 = NO_PROMPT;			/* is "prompt_message[msgno1]:
					   prompt_messaage[msgno2]"
					   unless one of msgno1 or
					   msgno2 = 0 */


/*
 *  prompt
 *
 *  description:	if in verbose mode, display 
 *			prompt_messages[msgno]
 *
 *  parameters:		msgno (IDX) - msgno2
 */

prompt(msgno)
IDX msgno;
{

	/* if system is not in verbose mode, do not display messages */

	if (get_verbose_mode() == VERBOSE_MODE_ON)
	{
		erase_msg();
		msgno1 = NO_PROMPT;
		msgno2 = msgno;
		display_msg(white);
	}
	else
	{
		msgno1 = NO_PROMPT;
		msgno2 = msgno;
	}

} /* end prompt */


/*
 *  reprompt
 *
 *  description:	if in verbose mode, redisplay prompt message
 *			preceeded by "prompt_messages[msgno]: "
 *
 *  parameters:		msgno (IDX) - msgno1
 */

reprompt(msgno)
IDX msgno;				/* msgno1 */
{
	/* if system is not in verbose mode, do not display messages */

	if (get_verbose_mode() == VERBOSE_MODE_ON)
	{
		erase_msg();
		msgno1 = msgno;
		display_msg(white);
	};

} /* end reprompt */


/*
 *  full_prompt
 *
 *  description:	displays:
 *			"<prompt_msg[new1]>: <prompt_msg[new2]>"
 *
 *  parameters:		new1, new2 (IDX) - message numbers
 *
 */

full_prompt(new1,new2)
IDX new1,new2;
{
	/* if system is not in verbose mode, do not display messages */

	if (get_verbose_mode() == VERBOSE_MODE_ON)
	{
		erase_msg();
		msgno1 = new1;
		msgno2 = new2;
		display_msg(white);
	};
}  /* end full_promp */


/*
 *  display_msg
 *
 *  description:	draw prompt message on screen
 *			preceded by invalid input if invalid = TRUE
 *
 *  parameters:		(IDX) - color no
 */

display_msg(clrno)
IDX clrno;				/* color to use */
{
	int length;			/* length of prompt msg */
	char *prompt_str;		/* prompt msg */
	Gpoint window[2];		/* window[MIN] = (xmin,ymin)
					   window[MAX] = (xmax,ymax) */
	Gpoint center;			/* center of window */

	/* activate msg area and deactivate all others */

	push_curr_trans();
	activate(MSG_AREA);
	set_aspect_flags(GBUNDLED);

	/* determine prompt string */
	
	if ((msgno1 != NO_PROMPT) && (msgno2 != NO_PROMPT))
	{
		length = strlen(prompt_messages[msgno1])
			+ strlen(prompt_messages[msgno2]) + 3;
		prompt_str = (char *) calloc ((unsigned) length,
			sizeof(char));
		(void) strcpy(prompt_str,prompt_messages[msgno1]);
		(void) strcat(prompt_str,": ");
		(void) strcat(prompt_str,prompt_messages[msgno2]);
	}
	else if ((msgno1 == NO_PROMPT) && (msgno2 != NO_PROMPT))
	{
		length = strlen(prompt_messages[msgno2]) + 1;
		prompt_str = (char *) calloc ((unsigned) length,
			sizeof(char));
		(void) strcpy(prompt_str,prompt_messages[msgno2]);
	}
	else if ((msgno1 != NO_PROMPT) && (msgno2 == NO_PROMPT))
	{
		length = strlen(prompt_messages[msgno1]) + 1;
		prompt_str = (char *) calloc ((unsigned) length,
			sizeof(char));
		(void) strcpy(prompt_str,prompt_messages[msgno1]);
	}
	else	/* nothing to prompt */
		prompt_str = (char *) NULL;

	/* find center of prompt screen area */

	get_window(MSG_AREA,window);
	center.x = (window[MIN].x + window[MAX].x) / 2;
	center.y = (window[MIN].y + window[MAX].y) / 2;

	/* display prompt message */

	if (prompt_str != (char *) NULL)
	{
		center_text(1,&prompt_str,&center,clrno,PROMPT_TEXT);
		free((char *) prompt_str);
	}
	
	/* restore previously active windows */

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();

} /* end display_msg */


/*
 *  erase_msg
 *
 *  description:	erases prompt area and redraws border
 *			i.e. erases message
 *
 *  parameters:		none
 */

erase_msg()
{
	Gpoint window[2];
	Gpoint box[4];

	push_curr_trans();
	activate(MSG_AREA);
	set_aspect_flags(GBUNDLED);

	get_window(MSG_AREA,window);
	load_box(box,window[MIN].x,window[MAX].x,
		window[MIN].y,window[MAX].y);
	gsetfillind(BLACK_SOLID);
	gfillarea(4,box);

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();
}  /* end erase_msg */


/*
 *  display_msg_area
 *
 *  descrption:		displays msg area on screen
 *
 *  parameters:		none
 */

display_msg_area()
{
	Gpoint window[2];		/* boundary of prompt area 
					   window */
	Gpoint box[4];			/* pts of boundary */

	push_curr_trans();
	if (find_trans(MSG_AREA) == NON_EXISTENT)
		alloc_trans(MSG_AREA);
	activate(MSG_AREA);

	set_aspect_flags(GBUNDLED);

	/* draw window with black background outlined in white */

	get_window(MSG_AREA,window);
	load_box(box,window[MIN].x,window[MAX].x,
		window[MIN].y,window[MAX].y);
	gsetfillind(BLACK_SOLID);
	gfillarea(4,box);

	set_aspect_flags(GINDIVIDUAL);
	pop_curr_trans();

	display_msg(white);

} /* end display_msg_area */


/*
 *  rm_msg_area
 *
 *  description:	removes msg area from screen
 *
 *  parameters:		none
 */

rm_msg_area()
{
	msgno1 = 0;

	/* erase prompt area */

	erase_area(MSG_AREA);
	free_trans(MSG_AREA);

}  /* end rm_msg_area */


/*
 *  restore_msg_area
 *
 *  description:	replaces msg area on screen
 *
 *  parameters:		none
 */

restore_msg_area()
{
	/* redraw screen with prompt area */

	msgno1 = NO_PROMPT;
	(*(screen_tbl[find_screen_tbl_idx(MSG_AREA)].draw))();

}  /* end restore_msg_area */
