/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1985, 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1985.
 *	1st revision : August 1985.
 *	2nd revision : March 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "func.h"
#include "object.h"
#include "paintop.h"

extern int		pointmarker_shown, compoundbox_shown;

extern int		foreground_color, background_color;
extern int		last_action, last_object;
extern int		last_axis, last_rotateangle;
extern int		movedpoint_num;
extern int		fix_x, fix_y;
extern F_pos		last_position, new_position;

extern F_compound	objects;
extern F_compound	saved_objects;
extern F_compound	object_tails;

extern F_point		*left_point; 
extern F_point		*moved_point;
extern F_point		*deleted_point;
extern F_point		*added_point;

undo()
{
	switch(last_action) {
	    case F_CREATE :
		undo_create();
		break;
	    case F_REMOVE :
		undo_remove();
		break;
	    case F_MOVE :
		undo_move();
		break;
	    case F_MOVE_POINT :
		undo_movepoint();
		break;
	    case F_FLIP :
		undo_flip();
		break;
	    case F_ROTATE :
		undo_rotate();
		break;
	    case F_GLUE :
		undo_glue();
		break;
	    case F_BREAK :
		undo_break();
		break;
	    case F_SCALE :
		undo_scale();
		break;
	    case F_ADD_POINT :
		undo_addpoint();
		break;
	    case F_DELETE_POINT :
		undo_deletepoint();
		break;
	    default :
		return;
	    }	
	}

undo_addpoint()
{
	if (last_object == O_POLYLINE)
	    linepoint_deleting(saved_objects.lines);
	else
	    splinepoint_deleting(saved_objects.splines);
	last_action = F_DELETE_POINT;
	deleted_point = added_point;
	}

undo_deletepoint()
{
	if (last_object == O_POLYLINE) 
	    linepoint_adding(saved_objects.lines, deleted_point);
	else
	    splinepoint_adding(saved_objects.splines, deleted_point);
	last_action = F_ADD_POINT;
	added_point = deleted_point;
	}

undo_break()
{
	if (compoundbox_shown)
	    draw_compoundbox(saved_objects.compounds, INV_PAINT);
	cut_objects(&objects, &object_tails);
	insert_compound(&objects.compounds, saved_objects.compounds);
	last_action = F_GLUE;
	}

undo_glue()
{
	if (compoundbox_shown)
	    draw_compoundbox(saved_objects.compounds, INV_PAINT);
	delete_compound(&objects.compounds, saved_objects.compounds);
	append_objects(&objects, saved_objects.compounds, &object_tails);
	last_action = F_BREAK;
	}

/* 
When a single object is created, it is inserted as the first object in
the appropriate list in objects.  It is also placed in the appropriate
list in saved_objects.

However when a number of objects are created (usually by reading them
in from a file or undoing a remove-all action), they are appended to
the lists in objects and also saved in saved_objects.  The pointers
in object_tails will be set to point to the last members of the lists
in objects prior to the appending.

Note: The read operation will set the pointers in object_tails
while the remove-all operation will zero pointers in objects.
*/

undo_create()
{
	switch(last_object) {
	    case O_POLYLINE :
		objects.lines = saved_objects.lines->next;
		saved_objects.lines->next = NULL;
		erase_lines(saved_objects.lines);
		break;
	    case O_ELLIPSE :
		objects.ellipses = saved_objects.ellipses->next;
		saved_objects.ellipses->next = NULL;
		erase_ellipses(saved_objects.ellipses);
		break;
	    case O_TEXT :
		objects.texts = saved_objects.texts->next;
		saved_objects.texts->next = NULL;
		erase_texts(saved_objects.texts);
		break;
	    case O_SPLINE :
	        objects.splines = saved_objects.splines->next;
	        saved_objects.splines->next = NULL;
		erase_splines(saved_objects.splines);
		break;
	    case O_ARC :
		objects.arcs = saved_objects.arcs->next;
		saved_objects.arcs->next = NULL;
		erase_arcs(saved_objects.arcs);
		break;
	    case O_COMPOUND :
		objects.compounds = saved_objects.compounds->next;
		saved_objects.compounds->next = NULL;
		erase_compounds(saved_objects.compounds);
		break;
	    case O_ALL_OBJECT :
		cut_objects(&objects, &object_tails);
		redisplay_canvas();
		break;
	    }
	last_action = F_REMOVE;
	}

undo_remove()
{
	switch (last_object) {
	    case O_POLYLINE :
		draw_lines(saved_objects.lines);
		insert_line(&objects.lines, saved_objects.lines);
		break;
	    case O_ELLIPSE :
		draw_ellipses(saved_objects.ellipses);
		insert_ellipse(&objects.ellipses, saved_objects.ellipses);
		break;
	    case O_TEXT :
		draw_texts(saved_objects.texts);
		insert_text(&objects.texts, saved_objects.texts);
		break;
	    case O_SPLINE :
		draw_splines(saved_objects.splines);
		insert_spline(&objects.splines, saved_objects.splines);
		break;
	    case O_ARC :
		draw_arcs(saved_objects.arcs);
		insert_arc(&objects.arcs, saved_objects.arcs);
		break;
	    case O_COMPOUND :
		draw_compounds(saved_objects.compounds);
		insert_compound(&objects.compounds, saved_objects.compounds);
		break;
	    case O_ALL_OBJECT :
		append_objects(&objects, &saved_objects, &object_tails);
		redisplay_canvas();
		break;
	    }
	last_action = F_CREATE;
	}

undo_flip()
{
	switch (last_object) {
	    case O_POLYLINE :
		if (pointmarker_shown)
		    toggle_linepointmarker(saved_objects.lines);
		draw_line(saved_objects.lines, ERASE);
		flip_line(saved_objects.lines, 
			last_position.x, last_position.y,
			last_axis);
		draw_line(saved_objects.lines, PAINT);
		if (pointmarker_shown)
		    toggle_linepointmarker(saved_objects.lines);
		break;
	    case O_ELLIPSE :
		if (pointmarker_shown)
		    toggle_ellipsepointmarker(saved_objects.ellipses);
		pw_batch_on(canvas_pixwin);
		draw_ellipse(saved_objects.ellipses, background_color);
		flip_ellipse(saved_objects.ellipses, 
			last_position.x, last_position.y,
			last_axis);
		draw_ellipse(saved_objects.ellipses, foreground_color);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_ellipsepointmarker(saved_objects.ellipses);
		break;
	    case O_SPLINE :
		if (pointmarker_shown)
		    toggle_splinepointmarker(saved_objects.splines);
		pw_batch_on(canvas_pixwin);
		draw_spline(saved_objects.splines, ERASE);
		flip_spline(saved_objects.splines,
			last_position.x, last_position.y,
			last_axis);
		draw_spline(saved_objects.splines, PAINT);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_splinepointmarker(saved_objects.splines);
		break;
	    case O_ARC :
		if (pointmarker_shown)
		    toggle_arcpointmarker(saved_objects.arcs);
		pw_batch_on(canvas_pixwin);
		draw_arc(saved_objects.arcs, background_color);
		flip_arc(saved_objects.arcs, 
			last_position.x, last_position.y,
			last_axis);
		draw_arc(saved_objects.arcs, foreground_color);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_arcpointmarker(saved_objects.arcs);
		break;
	    case O_COMPOUND :
		if (compoundbox_shown) 
		    draw_compoundbox(saved_objects.compounds, INV_PAINT);
		erase_compound(saved_objects.compounds);
		flip_compound(saved_objects.compounds, 
			last_position.x, last_position.y,
			last_axis);
		draw_compound(saved_objects.compounds);
		if (compoundbox_shown) 
		    draw_compoundbox(saved_objects.compounds, INV_PAINT);
		break;
	    }
	}

undo_move()
{
	int	dx, dy;

	dx = last_position.x - new_position.x;
	dy = last_position.y - new_position.y;
	switch (last_object) {
	    case O_POLYLINE :
		if (pointmarker_shown)
		    toggle_linepointmarker(saved_objects.lines);
		draw_line(saved_objects.lines, ERASE);
		translate_line(saved_objects.lines, dx, dy);
		draw_line(saved_objects.lines, PAINT);
		if (pointmarker_shown)
		    toggle_linepointmarker(saved_objects.lines);
		break;
	    case O_ELLIPSE :
		if (pointmarker_shown)
		    toggle_ellipsepointmarker(saved_objects.ellipses);
		pw_batch_on(canvas_pixwin);
		draw_ellipse(saved_objects.ellipses, background_color);
		translate_ellipse(saved_objects.ellipses, dx, dy);
		draw_ellipse(saved_objects.ellipses, foreground_color);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_ellipsepointmarker(saved_objects.ellipses);
		break;
	    case O_TEXT :
		draw_text(saved_objects.texts, INV_PAINT);
		translate_text(saved_objects.texts, dx, dy);
		draw_text(saved_objects.texts, PAINT);
		break;
	    case O_SPLINE :
		if (pointmarker_shown)
		    toggle_splinepointmarker(saved_objects.splines);
		pw_batch_on(canvas_pixwin);
		draw_spline(saved_objects.splines, ERASE);
		translate_spline(saved_objects.splines, dx, dy);
		draw_spline(saved_objects.splines, PAINT);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_splinepointmarker(saved_objects.splines);
		break;
	    case O_ARC :
		if (pointmarker_shown)
		    toggle_arcpointmarker(saved_objects.arcs);
		pw_batch_on(canvas_pixwin);
		draw_arc(saved_objects.arcs, background_color);
		translate_arc(saved_objects.arcs, dx, dy);
		draw_arc(saved_objects.arcs, foreground_color);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_arcpointmarker(saved_objects.arcs);
		break;
	    case O_COMPOUND :
		if (compoundbox_shown) 
		    draw_compoundbox(saved_objects.compounds, INV_PAINT);
		erase_compound(saved_objects.compounds);
		translate_compound(saved_objects.compounds, dx, dy);
		draw_compound(saved_objects.compounds);
		if (compoundbox_shown) 
		    draw_compoundbox(saved_objects.compounds, INV_PAINT);
		break;
	    }
	swap_newp_lastp();
	}

undo_movepoint()
{
	switch (last_object) {
	    case O_POLYLINE :
		relocate_linepoint(saved_objects.lines, 
			last_position.x, last_position.y, 
			saved_objects.lines->points->x,
			saved_objects.lines->points->y, 
			moved_point, left_point);
		break;
	    case O_SPLINE :
		relocate_splinepoint(saved_objects.splines, last_position.x,
			last_position.y, moved_point);
		break;
	    case O_ARC :
		relocate_arcpoint(saved_objects.arcs, last_position.x, 
			last_position.y, movedpoint_num);
		break;
	    case O_ELLIPSE :
		relocate_ellipsepoint(saved_objects.ellipses, last_position.x,
			last_position.y, movedpoint_num);
		break;
	    }
	swap_newp_lastp();
	}

undo_rotate()
{
	switch (last_object) {
	    case O_POLYLINE :
		if (pointmarker_shown)
		    toggle_linepointmarker(saved_objects.lines);
		draw_line(saved_objects.lines, ERASE);
		if (last_rotateangle == 90) last_rotateangle = 270;
		else last_rotateangle = 90;
		rotate_line(saved_objects.lines, 
			last_position.x, last_position.y,
			last_rotateangle);
		draw_line(saved_objects.lines, PAINT);
		if (pointmarker_shown)
		    toggle_linepointmarker(saved_objects.lines);
		break;
	    case O_ELLIPSE :
		if (pointmarker_shown)
		    toggle_ellipsepointmarker(saved_objects.ellipses);
		pw_batch_on(canvas_pixwin);
		draw_ellipse(saved_objects.ellipses, background_color);
		if (last_rotateangle == 90) last_rotateangle = 270;
		else last_rotateangle = 90;
		rotate_ellipse(saved_objects.ellipses, 
			last_position.x, last_position.y,
			last_rotateangle);
		draw_ellipse(saved_objects.ellipses, foreground_color);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_ellipsepointmarker(saved_objects.ellipses);
		break;
	    case O_SPLINE :
		if (pointmarker_shown)
		    toggle_splinepointmarker(saved_objects.splines);
		pw_batch_on(canvas_pixwin);
		draw_spline(saved_objects.splines, ERASE);
		if (last_rotateangle == 90) last_rotateangle = 270;
		else last_rotateangle = 90;
		rotate_spline(saved_objects.splines,
			last_position.x, last_position.y,
			last_rotateangle);
		draw_spline(saved_objects.splines, PAINT);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_splinepointmarker(saved_objects.splines);
		break;
	    case O_ARC :
		if (pointmarker_shown)
		    toggle_arcpointmarker(saved_objects.arcs);
		pw_batch_on(canvas_pixwin);
		draw_arc(saved_objects.arcs, background_color);
		if (last_rotateangle == 90) last_rotateangle = 270;
		else last_rotateangle = 90;
		rotate_arc(saved_objects.arcs, 
			last_position.x, last_position.y,
			last_rotateangle);
		draw_arc(saved_objects.arcs, foreground_color);
		pw_batch_off(canvas_pixwin);
		if (pointmarker_shown)
		    toggle_arcpointmarker(saved_objects.arcs);
		break;
	    case O_COMPOUND :
		if (compoundbox_shown)
		    draw_compoundbox(saved_objects.compounds, INV_PAINT);
		erase_compound(saved_objects.compounds);
		if (last_rotateangle == 90) last_rotateangle = 270;
		else last_rotateangle = 90;
		rotate_compound(saved_objects.compounds, 
			last_position.x, last_position.y,
			last_rotateangle);
		draw_compound(saved_objects.compounds);
		if (compoundbox_shown)
		    draw_compoundbox(saved_objects.compounds, INV_PAINT);
		break;
	    }
	}

undo_scale()
{
	float	scalex, scaley;

	if (compoundbox_shown)
	    draw_compoundbox(saved_objects.compounds, INV_PAINT);
	erase_compound(saved_objects.compounds);
	scalex = ((float)(last_position.x-fix_x)) / (new_position.x-fix_x);
	scaley = ((float)(last_position.y-fix_y)) / (new_position.y-fix_y);
	scale_compound(saved_objects.compounds, scalex, scaley, fix_x, fix_y);
	draw_compound(saved_objects.compounds);
	if (compoundbox_shown) draw_compoundbox(saved_objects.compounds, INV_PAINT);
	swap_newp_lastp();
	}

swap_newp_lastp()
{
	int	t;  /*  swap new_position and last_position  */

	t = new_position.x; 
	new_position.x = last_position.x;
	last_position.x = t;
	t = new_position.y; 
	new_position.y = last_position.y;
	last_position.y = t;
	}
