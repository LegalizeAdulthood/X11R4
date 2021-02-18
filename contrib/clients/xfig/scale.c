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
#include "resources.h"
#include "func.h"
#include "object.h"
#include "paintop.h"

#define			TOLERANCE	7
#define			min(a, b)	(((a) < (b)) ? (a) : (b))
#define			max(a, b)	(((a) > (b)) ? (a) : (b))
#define			round(a)	(int)(a + 0.5)

extern int		fix_x, fix_y, cur_x, cur_y;
extern F_pos		last_position, new_position;  /* used in undo */

extern F_compound	*compound_point_search(), *compound_search();
extern			(*canvas_kbd_proc)();
extern			(*canvas_locmove_proc)();
extern			(*canvas_leftbut_proc)();
extern			(*canvas_middlebut_proc)();
extern			(*canvas_rightbut_proc)();
extern			null_proc();
extern			set_popupmenu();

			prescale_compound(), elastic_compoundbox();

static enum		{WEST, NORTH, EAST, SOUTH, CORNER} side;
static int		save_coord;
static F_compound	*compound;
static double		cosa, sina;
static int		fromx, fromy;
			init_scale_compound();

scale_compound_selected()
{
	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_scale_compound;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick15_cursor);
	reset_action_on();
	}

init_scale_compound(x, y)
int	x, y;
{
	double	dx, dy, l;
	
	if ((compound = compound_point_search(x, y, TOLERANCE, &cur_x,
					      &cur_y)) != NULL)
	{
		if (compound->nwcorner.x == cur_x) 
			fix_x = compound->secorner.x;
		else
			fix_x = compound->nwcorner.x;
		if (compound->nwcorner.y == cur_y) 
			fix_y = compound->secorner.y;
		else
			fix_y = compound->nwcorner.y;
		
		side = CORNER;
		dx = cur_x - fix_x;  dy = cur_y - fix_y;
		l = sqrt(dx*dx + dy*dy);
		cosa = fabs(dx / l);  sina = fabs(dy / l);
		
		fromx = cur_x;  fromy = cur_y;
		win_setmouseposition(canvas_swfd, cur_x, cur_y);
		draw_compoundbox(compound, INV_PAINT);
		draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
		set_latestcompound(compound);
		canvas_locmove_proc = elastic_compoundbox;
		canvas_middlebut_proc = prescale_compound;
		canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
		set_action_on();
		set_temp_cursor(&null_cursor);
		win_setmouseposition(canvas_swfd, x, y);
	}
	else if ((compound = compound_search(x, y, TOLERANCE, &cur_x, &cur_y))
		 != NULL )
	{
		/* figure out which side the cursor is close to. */
		if (compound->nwcorner.x == cur_x)
			side = WEST;
		else if (compound->nwcorner.y == cur_y)
			side = NORTH;
		else if (compound->secorner.x == cur_x)
			side = EAST;
		else if (compound->secorner.y == cur_y)
			side = SOUTH;
		else
			return;
		
		/* figure out a fixed point of the compound for scaling */
		switch (side) {
		case WEST:
		case NORTH:
			fix_x = compound->secorner.x;
			fix_y = compound->secorner.y;
			cur_x = compound->nwcorner.x;
			cur_y = compound->nwcorner.y;
			break;
		case EAST:
		case SOUTH:
			fix_x = compound->nwcorner.x;
			fix_y = compound->nwcorner.y;
			cur_x = compound->secorner.x;
			cur_y = compound->secorner.y;
			break;
		}
		
		/* save initial corner position */
		fromx = cur_x;  fromy = cur_y;
		/* save x or y coordinate for restoring the cursor */
		/* at the end of the scaling */
		switch (side) {
		case NORTH: case SOUTH:
			save_coord = x;
			break;
		case EAST: case WEST:
			save_coord = y;
			break;
		}
		/*  erase the compound box. */
		win_setmouseposition(canvas_swfd, cur_x, cur_y);
		draw_compoundbox(compound, INV_PAINT);
		draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
		set_latestcompound(compound);
		canvas_locmove_proc = elastic_compoundbox;
		canvas_middlebut_proc = prescale_compound;
		canvas_leftbut_proc = canvas_rightbut_proc = null_proc;
		set_action_on();
		set_temp_cursor(&null_cursor);
		win_setmouseposition(canvas_swfd, x, y);
	}
}


elastic_compoundbox(x, y)
int	x, y;
{
	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	compute_adjustposition(x, y);
	win_setmouseposition(canvas_swfd, cur_x, cur_y);
	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	}

compute_adjustposition(x, y)
int	x, y;
{
	double	xx, yy, d;

	switch(side) {
	case CORNER:
		xx = x - fix_x;
		yy = y - fix_y;
		d = sqrt(xx*xx + yy*yy);
		if (xx < 0)
			cur_x = fix_x - round(d * cosa);
		else
			cur_x = fix_x + round(d * cosa);
		if (yy < 0)
			cur_y = fix_y - round(d * sina);
		else
			cur_y = fix_y + round(d * sina);
		break;
	case NORTH:
	case SOUTH:
		cur_y = y;
		break;
	case EAST:
	case WEST:
		cur_x = x;
		break;
	}
}

prescale_compound(x, y)
int	x, y;
{
	float		scalex, scaley;

	draw_rectbox(fix_x, fix_y, cur_x, cur_y, INV_PAINT);
	compute_adjustposition(x, y);
	erase_compound(compound);

	scalex = ((float) (cur_x - fix_x)) / (fromx - fix_x);
	scaley = ((float) (cur_y - fix_y)) / (fromy - fix_y);
	scale_compound(compound, scalex, scaley, fix_x, fix_y);

	draw_compound(compound);
	draw_compoundbox(compound, INV_PAINT);
	last_position.x = fromx;
	last_position.y = fromy;
	new_position.x = cur_x;
	new_position.y = cur_y;
	/* update cursor position for single axis scaling. */
	switch (side) {
	case NORTH: case SOUTH:
		/* maintain old horiz. pos. */
		cur_x = save_coord;
		break;
	case EAST: case WEST:
		/* maintain old vert. pos. */
		cur_y = save_coord;
		break;
	}
	clean_up();
	set_action_object(F_SCALE, O_COMPOUND);
	set_latestcompound(compound);
	set_modifiedflag();
	scale_compound_selected();
	}

scale_compound(c, sx, sy, refx, refy)
F_compound	*c;
float		sx, sy;
int		refx, refy;
{
	F_line		*l;
	F_spline	*s;
	F_ellipse	*e;
	F_text		*t;
	F_arc		*a;
	F_compound	*c1;
	int		x1, y1, x2, y2;

	x1 = round(refx + (c->nwcorner.x - refx) * sx);
	y1 = round(refy + (c->nwcorner.y - refy) * sy);
	x2 = round(refx + (c->secorner.x - refx) * sx);
	y2 = round(refy + (c->secorner.y - refy) * sy);
	c->nwcorner.x = min(x1, x2);
	c->nwcorner.y = min(y1, y2);
	c->secorner.x = max(x1, x2);
	c->secorner.y = max(y1, y2);

	for (l = c->lines; l != NULL; l = l->next) {
	    scale_line(l, sx, sy, refx, refy);
	    }
	for (s = c->splines; s != NULL; s = s->next) {
	    scale_spline(s, sx, sy, refx, refy);
	    }
	for (a = c->arcs; a != NULL; a = a->next) {
	    scale_arc(a, sx, sy, refx, refy);
	    } 
	for (e = c->ellipses; e != NULL; e = e->next) {
	    scale_ellipse(e, sx, sy, refx, refy);
	    }
	for (t = c->texts; t != NULL; t = t->next) {
	    scale_text(t, sx, sy, refx, refy);
	    }
	for (c1 = c->compounds; c1 != NULL; c1 = c1->next) {
	    scale_compound(c1, sx, sy, refx, refy);
	    }
	}

scale_line(l, sx, sy, refx, refy)
F_line	*l;
float	sx, sy;
int	refx, refy;
{
	F_point		*p;

	for (p = l->points; p != NULL; p = p->next) {
	    p->x = round(refx + (p->x - refx) * sx);
	    p->y = round(refy + (p->y - refy) * sy);
	    }
	}

scale_spline(s, sx, sy, refx, refy)
F_spline	*s;
float		sx, sy;
int		refx, refy;
{
	F_point		*p;
	F_control	*c;

	for (p = s->points; p != NULL; p = p->next) {
	    p->x = round(refx + (p->x - refx) * sx);
	    p->y = round(refy + (p->y - refy) * sy);
	    }
	for (c = s->controls; c != NULL; c = c->next) {
	    c->lx = refx + (c->lx - refx) * sx;
	    c->ly = refy + (c->ly - refy) * sy;
	    c->rx = refx + (c->rx - refx) * sx;
	    c->ry = refy + (c->ry - refy) * sy;
	    }
	}

scale_arc(a, sx, sy, refx, refy)
F_arc	*a;
float	sx, sy;
int	refx, refy;
{
	int		i;

	for (i = 0; i < 3; i++) {
	    a->point[i].x = round(refx + (a->point[i].x - refx) * sx);
	    a->point[i].y = round(refy + (a->point[i].y - refy) * sy);
	    }
	compute_arccenter(a->point[0], a->point[1], a->point[2], 
		&a->center.x, &a->center.y);
	a->direction = compute_direction(a->point[0], a->point[1], a->point[2]);
	} 

scale_ellipse(e, sx, sy, refx, refy)
F_ellipse	*e;
float		sx, sy;
int		refx, refy;
{
	e->center.x = round(refx + (e->center.x - refx) * sx);
	e->center.y = round(refy + (e->center.y - refy) * sy);
	e->start.x = round(refx + (e->start.x - refx) * sx);
	e->start.y = round(refy + (e->start.y - refy) * sy);
	e->end.x = round(refx + (e->end.x - refx) * sx);
	e->end.y = round(refy + (e->end.y - refy) * sy);
	e->radiuses.x = round(e->radiuses.x * sx);
	e->radiuses.y = round(e->radiuses.y * sy);
	}

scale_text(t, sx, sy, refx, refy)
F_text	*t;
float	sx, sy;
int	refx, refy;
{
	t->base_x = round(refx + (t->base_x - refx) * sx);
	t->base_y = round(refy + (t->base_y - refy) * sy);
	}
