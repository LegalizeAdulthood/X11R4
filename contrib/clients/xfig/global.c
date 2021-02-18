/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : Aug 1985.
 *
 *	%W%	%G%
*/
#include "fig.h"
#define	GLOBAL
#include "resources.h"
#include "font.h"
#undef	GLOBAL
#include "const.h"
#include "func.h"
#include "object.h"
#include "paintop.h"

/**********************  canvas variables  ************************/

int			(*canvas_kbd_proc)();
			(*canvas_locmove_proc)();
			(*canvas_leftbut_proc)();
			(*canvas_middlebut_proc)();
			(*canvas_rightbut_proc)();
int			fix_x, fix_y;
int			cur_x, cur_y;

int			receiving_msg = 0;
int			action_on = 0;
int			pointmarker_shown = 0;
int			compoundbox_shown = 0;

int			ICON_COLUMN;
int			CANVAS_HEIGHT, CANVAS_WIDTH;
int			CANVAS_LEFT, CANVAS_TOP;
int			PANEL_LEFT, PANEL_TOP, PANEL_HEIGHT, PANEL_WID;
int			MSG_LEFT, MSG_TOP, MSG_WIDTH;
int			SIDERULER_LEFT, SIDERULER_TOP;
int			SIDERULER_WIDTH, SIDERULER_HEIGHT;
int			SIDERULER_START;
int			TOPRULER_LEFT, TOPRULER_TOP;
int			TOPRULER_WIDTH, TOPRULER_HEIGHT;
int			borderwid, windowspacing, toolstripeht;

int			num_point;
F_point			*first_point, *cur_point;

/************************  Objects  **********************/

/*
Object_tails (not always) point to the last objects in each linked list
in objects.  It is used to speed up an undo-read action.  When a file
is read, the lists of the objects read are stored in saved_objects
and the pointers to tails of the lists in objects would kept in object_tails
the "next" members of the tail objects point to the lists in saved_objects.
To undo, one would only set all the "next" of tail object to NULL;

Object_tails is also used for speeding up the undo of compound breaking
action in similar fashion.
*/
F_compound		object_tails = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
F_compound		objects = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
F_compound		saved_objects = {NULL,NULL,NULL,NULL,NULL,NULL,NULL};
F_compound		*saved_compound = NULL;	/* Saved the broken compound */

/*************************  Undo  variables  **********************/

int			last_action = F_NULL;
int			last_object;
int			last_axis;
int			last_rotateangle;
struct {int x, y;}	last_position, new_position;
F_point			*moved_point;
F_point			*deleted_point;
F_point			*added_point;
F_point			*left_point;
F_point			*right_point;
int			movedpoint_num;

/***************************  Modes  ****************************/

int			manhattan_mode		= 0;
int			mountain_mode		= 0;
int			autoforwardarrow_mode	= 0;
int			autobackwardarrow_mode	= 0;
int			latexline_mode		= 0;
int			latexarrow_mode		= 0;
int			magnet_mode		= 0;
int			line_thickness		= 1;
int			pen_size		= 0;
int			pen_type		= 0;
int			flip_axis		= -1;
int			rotate_angle		= 0;
int			cur_line_style		= -1;
float			cur_dashlength		= .05*PIX_PER_INCH;
float			cur_dotgap		= .04*PIX_PER_INCH;
float			cur_styleval		= 0.0;
float			cur_angle		= 0.0;
int			cur_color		= BLACK;
int			cur_textstyle		= PLAIN;

short			dot_image[16] = {
				0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
				0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
				0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
				0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF,
				};
mpr_static(dot, 16, 16, 1, dot_image);

/*************************  Flags  *******************************/

int			cur_command = -1;
int			DEBUG = 0;
int			RHS_PANEL = 0;
int			INVERSE = 0;
int			TRACKING = 1;

/************************  Status  ****************************/

char			directory[128] = "";
char			current_file[32] = "";
int			figure_modified = 0;

/************************  Error messages  ****************************/

char		Err_incomp[] = "Incomplete %s object at line %d.";
char		Err_mem[] = "Running out of memory.";

/************************  Routines  ****************************/

null_proc()
{
	}

set_modifiedflag()
{
	figure_modified = 1;
	}

set_action_on()
{
	action_on = 1;
	}

reset_action_on()
{
	action_on = 0;
	}

reset_cursor()
{
#ifndef	X11
	win_setcursor(canvas_swfd, cur_cursor);
#else
	XDefineCursor(tool_d, canvas_swfd, (Cursor)cur_cursor->bitmap);
#endif	X11
	}

set_temp_cursor(cursor)
CURSOR	cursor;
{
#ifndef	X11
	win_setcursor(canvas_swfd, cursor);
#else
	XDefineCursor(tool_d, canvas_swfd, (Cursor)cursor->bitmap);
#endif	X11
	}

set_cursor(cursor)
CURSOR	cursor;
{
	cur_cursor = cursor;
#ifndef	X11
	win_setcursor(canvas_swfd, cursor);
#else
	XDefineCursor(tool_d, canvas_swfd, (Cursor)cursor->bitmap);
#endif	X11
	}

set_lastaxis(a)
int	a;
{
	last_axis = a;
	}

set_lastangle(a)
int	a;
{
	last_rotateangle = a;
	}

set_lastposition(x, y)
int	x, y;
{
	last_position.x = x;
	last_position.y = y;
	}

set_action(action)
int	 action;
{
	last_action = action;
	}

set_action_object(action, object)
int	 action, object;
{
	last_action = action;
	last_object = object;
	}

/*
Clean_up should be called before committing a user's request.
Clean_up will attempt to free all the allocated memories which
resulted from delete/remove action.  It will set the last_action
to F_NULL.  Thus this routine should be before set_action_object().
if they are to be called in the same routine.
*/
clean_up()
{
	if (last_action == F_REMOVE) {
	    switch (last_object) {
		case O_ARC :
		    free_arc(&saved_objects.arcs);
		    break;
		case O_COMPOUND :
		    free_compound(&saved_objects.compounds);
		    break;
		case O_ELLIPSE :
		    free_ellipse(&saved_objects.ellipses);
		    break;
		case O_POLYLINE :
		    free_line(&saved_objects.lines);
		    break;
		case O_SPLINE :
		    free_spline(&saved_objects.splines);
		    break;
		case O_TEXT :
		    free_text(&saved_objects.texts);
		    break;
		case O_ALL_OBJECT :
		    free_arc(&saved_objects.arcs);
		    free_compound(&saved_objects.compounds);
		    free_ellipse(&saved_objects.ellipses);
		    free_line(&saved_objects.lines);
		    free_spline(&saved_objects.splines);
		    free_text(&saved_objects.texts);
		    break;
		}
	    }
	else if (last_action == F_DELETE_POINT) {
	    free((char*)deleted_point);
	    }
	else if (last_action == F_REMOVE_ALL || last_action == F_EDIT) {
	    free_arc(&saved_objects.arcs);
	    free_compound(&saved_objects.compounds);
	    free_ellipse(&saved_objects.ellipses);
	    free_line(&saved_objects.lines);
	    free_spline(&saved_objects.splines);
	    free_text(&saved_objects.texts);
	    }
	else if (last_action == F_BREAK) {
	    free_compound(&saved_compound);
	    }
	else if (last_action == F_CREATE) {
	    saved_objects.arcs = NULL;
	    saved_objects.compounds = NULL;
	    saved_objects.ellipses = NULL;
	    saved_objects.lines = NULL;
	    saved_objects.splines = NULL;
	    saved_objects.texts = NULL;
	    }
	else if (last_action == F_TURN) {
	    if (last_object == O_POLYLINE) {
		free_line(&saved_objects.lines);
		}
	    else {	/* last_object == O_SPLINE */
		free_spline(&saved_objects.splines);
		}
	    }
	last_action = F_NULL;
	}

set_latestarc(arc)
F_arc	*arc;
{
	saved_objects.arcs = arc;
	}

set_latestcompound(compound)
F_compound	*compound;
{
	saved_objects.compounds = compound;
	}

set_latestellipse(ellipse)
F_ellipse	*ellipse;
{
	saved_objects.ellipses = ellipse;
	}

set_latestline(line)
F_line	*line;
{
	saved_objects.lines = line;
	}

set_latestspline(spline)
F_spline	*spline;
{
	saved_objects.splines = spline;
	}

set_latesttext(text)
F_text	*text;
{
	saved_objects.texts = text;
	}
