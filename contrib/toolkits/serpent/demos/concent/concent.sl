/* $Header: /u/04c/mnt/new_integ/demos/concent/RCS/concent.sl,v 1.1 89/10/10 20:11:20 serpent Exp $ */

#include "saw.ill"
#include "concent.ill"

|||

/*--------------------------------------------------------------------------*\
|   
|  Name:  concent.sl
|
|  Description:  
|     This Slang program implements the once-popular game of Concentration.
|
|     The prizes and their square numbers are sent over from an application,
|     which chooses the prizes randomly from a large list, and locates
|     them randomly on the board.
|
|  Component of:
|     Serpent Demos.
|
|  Related files:
|     ./dm.sdd - Shared data descriptions used within program.
|
|  Project:
|     Serpent
|     User Interface Project
|     Software Engineering Institute
|     Carnegie Mellon University
|     Pittsburgh, PA  15213
|
|  Version: 1.0
|
|  Project Leader:  
|     Len Bass
|     ljb@sei.cmu.edu
|
|  Author:  
|     Brian M. Clapper
|     bmc@sei.cmu.edu  
|
$Log:	concent.sl,v $
% Revision 1.1  89/10/10  20:11:20  serpent
% Initial revision
% 
Revision 1.4  89/10/09  12:12:03  serpent
short names: arithmetic.ext => arith.ext; slang_string.ext => sslang.ext

Revision 1.3  89/09/14  14:01:14  bmc
Now gets prizes and square numbers from application.  Changes to the
shared data definition also prompted changes to dialogue.

Revision 1.2  89/09/13  13:39:36  bmc
'row' and 'column' are no longer passed to the 'square_vc' through shared
data.  Instead, they are calculated from the 'number' value in the VC
itself.

Revision 1.1  89/09/12  23:24:58  bmc
Initial revision

|
\*--------------------------------------------------------------------------*\
|
| Distribution: Approved  for  public  release;  distribution  is  unlimited.
| 
| Copyright  (c)  1989  by  Carnegie  Mellon  University, Pittsburgh, PA. The
| Software Engineering Institute (SEI) is a  federally  funded  research  and
| development  center  established and operated by Carnegie Mellon University
| (CMU). Sponsored by the U.S. Department of Defense under  contract  F19628-
| 85-C-0003,  the SEI is supported by the services and defense agencies, with
| the U.S. Air Force as the executive contracting agent.
| 
| Permission to use, copy, modify, or distribute this software and its  docu-
| mentation  for any purpose and without fee is hereby granted, provided that
| such use, copying, modification  or  distribution  is  not  for  commercial
| advantage and that the above copyright notice appear in all copies and that
| both that copyright notice and this permission notice appear in  supporting
| documentation.  Further, the names Software Engineering Institute or Carne-
| gie Mellon University may not be used in advertising or publicity  pertain-
| ing to distribution of the software without specific, written prior permis-
| sion. CMU makes no claims or representations about the suitability of  this
| software  for  any  purpose.  This software is provided "as is" and no war-
| ranty, express or implied, is made by the SEI or CMU, as  to  the  accuracy
| and  functioning of the program and related program material, nor shall the
| fact of distribution constitute any such  warranty.  No  responsibility  is
| assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#include "arith.ext"
#include "sstring.ext"

EXTERNALS:
    void printf (string, integer, string, integer);
/*
    MACROS:
*/

#include "shared.h"

/* 
** ---------------------------------------------------------------------------
** Size Information.
** 
** Heights and Widths are in pixels.
** ---------------------------------------------------------------------------
*/

#define SQUARE_WIDTH		(150)
#define SQUARE_HEIGHT		(100)
#define SQUARES_PER_ROW		(5)
#define SQUARES_PER_COL		(TOTAL_SQUARES / SQUARES_PER_ROW)
#define TITLE_WINDOW_HEIGHT	(50)
#define CONTROL_PANEL_HEIGHT	(50)
#define PLAYER_STATUS_HEIGHT	(50)

#define MATRIX_HEIGHT		(SQUARES_PER_COL * SQUARE_HEIGHT)
#define BOARD_WIDTH		(SQUARES_PER_ROW * SQUARE_WIDTH)

#define BOARD_HEIGHT \
    (MATRIX_HEIGHT + \
    PLAYER_STATUS_HEIGHT + \
    CONTROL_PANEL_HEIGHT + \
    TITLE_WINDOW_HEIGHT)

#define TITLE_WINDOW_WIDTH	BOARD_WIDTH
#define PLAYER_SCORE_WIDTH	(200)
#define PLAYER_STATUS_WIDTH	(BOARD_WIDTH - (2 * PLAYER_SCORE_WIDTH))

#define OK_BUTTON_WIDTH		(50)
#define QUIT_BUTTON_WIDTH	(50)
#define MSG_WINDOW_WIDTH \
    (BOARD_WIDTH - OK_BUTTON_WIDTH - QUIT_BUTTON_WIDTH)

/*
** ---------------------------------------------------------------------------
** Location information, in pixels from the upper left corner
** of the board.
** ---------------------------------------------------------------------------
*/

#define TITLE_WINDOW_ROW  	(0)
#define TITLE_WINDOW_COL  	(0)
#define GAME_STATUS_ROW	  	(TITLE_WINDOW_ROW + TITLE_WINDOW_HEIGHT)
#define QUIT_BUTTON_COL	  	(0)
#define MSG_WINDOW_COL	  	(QUIT_BUTTON_COL + QUIT_BUTTON_WIDTH - 1)
#define OK_BUTTON_COL	  	(MSG_WINDOW_COL + MSG_WINDOW_WIDTH - 1)
#define FIRST_SQUARE_ROW  	(GAME_STATUS_ROW + CONTROL_PANEL_HEIGHT)
#define FIRST_SQUARE_COL  	(0)
#define PLAYER_STATUS_ROW 	(FIRST_SQUARE_ROW + MATRIX_HEIGHT)
#define PLAYER_1_SCORE_COL	(0)
#define PLAYER_STATUS_COL 	(PLAYER_1_SCORE_COL + PLAYER_SCORE_WIDTH)
#define PLAYER_2_SCORE_COL	(PLAYER_STATUS_COL + PLAYER_STATUS_WIDTH)

/*
** ---------------------------------------------------------------------------
** Text justification constants.
** ---------------------------------------------------------------------------
*/

#define LEFT_JUSTIFY	  	(0)
#define CENTER_JUSTIFY	  	(1)
#define RIGHT_JUSTIFY	  	(2)

/*
** ---------------------------------------------------------------------------
** Fonts
** ---------------------------------------------------------------------------
*/

#define TITLE_FONT	  	"vr-20"
#define NUMBERS_FONT 		"vr-40"
#define NORMAL_TEXT_FONT	"9x15"
#define PRIZE_NAME_FONT		"8x13"

/*
** ---------------------------------------------------------------------------
** Square creation macros.
** ---------------------------------------------------------------------------
*/
			/*
			** ----------------------------------------------------
			** square_to_row
			**
			** Converts a square number (in the presumed range of
			** [1,30]) to a row coordinate in the interval
			** [0, SQUARES_PER_ROW - 1].
			** ----------------------------------------------------
			*/
#define square_to_row(square_number) \
    ( div ((square_number - 1), SQUARES_PER_ROW) )

			/*
			** ----------------------------------------------------
			** square_to_col
			**
			** Converts a square number (in the presumed range of
			** [1,30]) to a col coordinate.
			** ----------------------------------------------------
			*/
#define square_to_col(square_number) \
    ( mod ((square_number - 1), SQUARES_PER_ROW) )

			/*
			** ----------------------------------------------------
			** set_VC_variable
			**
			** Sets a variable in the VC bound to a specific shared
			** data instance.
			**
			** shared_data_id    - The id of the shared data
			** VC__name          - The name of the VC in quotes
			** variable_name     - The name of the variable to set
			**                     (also in quotes)
			** value             - The value to be assigned
			**
			** Requires the existence of a global VC_id variable.
			** ----------------------------------------------------
			*/
#define set_VC_variable(shared_data_id, VC_name, variable_name, value) \
    VC_id := get_vc (shared_data_id, VC_name); \
    put_variable_value (VC_id, variable_name, value)

/*
** ---------------------------------------------------------------------------
** Start of Main VC
** ---------------------------------------------------------------------------
*/

VARIABLES:
    VC_id           : NULL;		/* used by set_VC_variable macro */
    choice_1_str    : "";		/* temp storage for 1st chosen prize */
    choice_1_id     : NULL;		/* ID of 1st chosen prize */
    choice_2_id     : NULL;		/* ID of 2nd chosen prize */
    choice_count    : 0;		/* how many are currently chosen? */
    msg             : "Choose a square";/* action message buffer */
    requesting_ack  : false;		/* true -> enable OK button */
    match           : false;		/* true -> chosen squares match */
    game_over	    : false;
    ok_to_choose    : true;		/* false -> only OK button active */
    which_player    : 1;		/* # of current player */
    player_1_score  : 0;
    player_2_score  : 0;

					/*
					** squares_left counts the number
					** of squares currently active.  It
					** serves both to keep track of how
					** many squares are left and to prevent
					** the application from sending over
					** too many prizes.
					*/
    squares_left : 0;

OBJECTS:

    playing_field : form_widget
	{
	ATTRIBUTES:
	    height           : BOARD_HEIGHT;
	    width            : BOARD_WIDTH;
	    background_color : "black";
	}

    title_area : label_widget
	{
	ATTRIBUTES:
	    parent           : playing_field;
	    horiz_distance   : TITLE_WINDOW_COL;
	    vert_distance    : TITLE_WINDOW_ROW;
	    width            : TITLE_WINDOW_WIDTH;
	    height           : TITLE_WINDOW_HEIGHT;
	    label_text       : "Concentration";
	    background_color : "black";
	    foreground_color : "white";
	    justify	     : CENTER_JUSTIFY;
	    font	     : TITLE_FONT;
	}

    player_status : label_widget
	{
	ATTRIBUTES:
	    parent           : playing_field;
	    horiz_distance   : PLAYER_STATUS_COL;
	    vert_distance    : PLAYER_STATUS_ROW;
	    width            : PLAYER_STATUS_WIDTH;
	    height           : PLAYER_STATUS_HEIGHT;
	    label_text:
		{
		IF ( game_over ) THEN
		    IF ( player_1_score > player_2_score ) THEN
			label_text := "Player 1 is the Winner!";
		    ELSE IF ( player_1_score < player_2_score ) THEN
			label_text := "Player 2 is the Winner!";
		    ELSE
			label_text := "TIE SCORE";
		    END IF;

		ELSE /* game not over */
		    IF ( which_player = 1) THEN
			label_text := "Player 1 is up";
		    ELSE
			label_text := "Player 2 is up";
		    END IF;
		END IF;
		}
	    background_color : "white";
	    foreground_color : "black";
	    justify	     : CENTER_JUSTIFY;
	    font	     : NORMAL_TEXT_FONT;
	}

    player_1_score_pad  : label_widget
	{
	ATTRIBUTES:
	    parent           : playing_field;
	    horiz_distance   : PLAYER_1_SCORE_COL;
	    vert_distance    : PLAYER_STATUS_ROW;
	    width            : PLAYER_SCORE_WIDTH;
	    height           : PLAYER_STATUS_HEIGHT;
	    background_color : "white";
	    foreground_color : "black";
	    justify	     : CENTER_JUSTIFY;
	    font	     : NORMAL_TEXT_FONT;
	    label_text : string_append ("Player 1 Score: ", player_1_score);
	}

    player_2_score_pad  : label_widget
	{
	ATTRIBUTES:
	    parent           : playing_field;
	    horiz_distance   : PLAYER_2_SCORE_COL;
	    vert_distance    : PLAYER_STATUS_ROW;
	    width            : PLAYER_SCORE_WIDTH;
	    height           : PLAYER_STATUS_HEIGHT;
	    background_color : "white";
	    foreground_color : "black";
	    justify	     : CENTER_JUSTIFY;
	    font	     : NORMAL_TEXT_FONT;
	    label_text : string_append ("Player 2 Score: ", player_2_score);
	}

    msg_area : label_widget
	{
	ATTRIBUTES:
	    parent           : playing_field;
	    horiz_distance   : MSG_WINDOW_COL;
	    vert_distance    : GAME_STATUS_ROW;
	    width            : MSG_WINDOW_WIDTH;
	    height           : CONTROL_PANEL_HEIGHT;
	    label_text       : msg;
	    background_color : "white";
	    foreground_color : "black";
	    font	     : NORMAL_TEXT_FONT;
	}

    ok_button : command_widget
	{
	ATTRIBUTES:
	    parent           : playing_field;
	    horiz_distance   : OK_BUTTON_COL;
	    vert_distance    : GAME_STATUS_ROW;
	    height           : CONTROL_PANEL_HEIGHT;
	    width            : OK_BUTTON_WIDTH;
	    label_text	     : "OK";
	    background_color : "white";
	    foreground_color : "black";
	    font	     : NORMAL_TEXT_FONT;

	METHODS:
	    notify:
		{
		IF ( requesting_ack ) THEN
		    ok_to_choose := true;
		    requesting_ack := false;
				/*
				** Marking the squares as unavailable has to
				** happen here, since it causes a change in 
				** the display.
				*/
		    IF (match) THEN
			set_VC_variable (choice_1_id,
					 "square_vc",
					 "unavailable",
					 true);
			set_VC_variable (choice_2_id,
					 "square_vc",
					 "unavailable",
					 true);

			IF ( which_player = 1 ) THEN
			    player_1_score := player_1_score + 1;
			ELSE
			    player_2_score := player_2_score + 1;
			END IF;

			IF ( squares_left = 0 ) THEN
			   game_over := true;
			END IF;
				/*
				** Don't switch players.  Matching entitles
				** current player to continue.
				*/
		    ELSE /* no match */
				/*
				** If there ** are only two squares left and 
				** we didn't match, then there's no way
				** they'll ever match.
				*/
			IF ( squares_left = 2 ) THEN
			    game_over := true;
			    set_VC_variable (choice_1_id,
					     "square_vc",
					     "unavailable",
					     true);
			    set_VC_variable (choice_2_id,
					     "square_vc",
					     "unavailable",
					     true);

			ELSE
				/* Switch players. */

			    IF ( which_player = 1 ) THEN
				which_player := 2;
			    ELSE
				which_player := 1;
			    END IF;
			END IF;
		    END IF;

				/* Reset all status variables. */

		    set_VC_variable (choice_1_id,
				     "square_vc",
				     "chosen",
				     false);
		    set_VC_variable (choice_2_id,
				     "square_vc",
				     "chosen",
				     false);
		    choice_1_id := NULL;
		    choice_1_str := "";
		    choice_2_id := NULL;

		    IF ( game_over ) THEN
			msg := "G A M E    O V E R";
		    ELSE
			msg := "Choose a square.";
		    END IF;
		END IF; /* requesting_ack */
		}
	    }

    quit_button : command_widget
	{
	ATTRIBUTES:
	    parent           : playing_field;
	    horiz_distance   : QUIT_BUTTON_COL;
	    vert_distance    : GAME_STATUS_ROW;
	    height           : CONTROL_PANEL_HEIGHT;
	    width            : QUIT_BUTTON_WIDTH;
	    label_text	     : "QUIT";
	    background_color : "white";
	    foreground_color : "black";
	    font	     : NORMAL_TEXT_FONT;

	METHODS:
	    notify:
		{
		exit ();
		}
	}

VC : square_vc

    CREATION CONDITION: ( new ("square_sdd") AND
			 (squares_left <= TOTAL_SQUARES) )

    VARIABLES:
	unavailable : false;
	chosen      : false;

    OBJECTS:
	square : command_widget
	    {
	    ATTRIBUTES:
		height           : SQUARE_HEIGHT;
		width            : SQUARE_WIDTH;
		parent           : playing_field;
		horiz_distance   : FIRST_SQUARE_COL + 
				   (square_to_col (square_sdd.number) * width);
		vert_distance    : FIRST_SQUARE_ROW +
				   (square_to_row (square_sdd.number) * height);
		label_text :
		    {
		    IF ( chosen ) THEN
			label_text := square_sdd.prize_name;
		    ELSE
			label_text := square_sdd.number;
		    END IF;
		    }

		background_color :
		    {
		    IF ( unavailable ) THEN
			background_color := "black";
		    ELSE
			background_color := "white";
		    END IF;
		    }
		foreground_color : "black";
		font :
		    {
		    IF ( chosen ) THEN
		    	font := PRIZE_NAME_FONT;
		    ELSE
			font := NUMBERS_FONT;
		    END IF;
		    }

	    METHODS:
		notify:
		    {
		    IF ( ok_to_choose AND
			 (NOT chosen) AND
			 (NOT unavailable) ) THEN
			choice_count := choice_count + 1;

			IF (choice_count = 1) THEN
			    choice_1_str := square_sdd.prize_name;
			    choice_1_id := square_sdd;
			    chosen := true;
			    msg := "Enter second choice";

			ELSE IF ( choice_count = 2) THEN
			    choice_2_id := square_sdd;
			    chosen := true;

			    /* Check for a match. */

			    IF ( (choice_1_str = square_sdd.prize_name) OR
				 (choice_1_str = WILD_CARD) OR
				 (square_sdd.prize_name = WILD_CARD) ) THEN

				match := true;
				msg := "MATCH!  Press OK to continue.";
				squares_left := squares_left - 2;

				    /* Evaluation of score is done during
				    ** handling of OK button to force all 
				    ** message updates to occur after the 
				    ** player presses OK to signify end of
				    ** turn.
				    */

			    ELSE
				match := false;
				msg := "No match.  Press OK to continue.";
					/*
					** Switching players is done during
					** handling of the OK button so that
					** on-screen messages don't change
					** until after player presses button.
					*/
			    END IF;
			    /*
			    ** Force the OK button to be pressed next.
			    ** (When "ok_to_choose" = "false", input will be
			    ** ignored by all command widgets except the ok
			    ** button.)
			    */
			    choice_count := 0;
			    requesting_ack := true;
			    ok_to_choose := false;
			END IF;
		    END IF;
		    }
	    }

    ON CREATE:
	{
	squares_left := squares_left + 1;

#ifdef DEBUG
	printf ("dialogue: VC %d created for prize %s at square %d\n",
	    squares_left, square_sdd.prize_name, square_sdd.number);
#endif DEBUG

	}

ENDVC square_vc;
