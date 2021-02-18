/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	Febuary 1988.
 *
 *	%W%	%G%
*/
#include "fig.h"
#include "resources.h"
#include "object.h"
#include "paintop.h"

extern int		pointmarker_shown;
extern int		compoundbox_shown;
extern int		foreground_color, background_color;

redisplay_objects(objects)
F_compound	*objects;
{
	if (objects == NULL) return;
	redisplay_textobject(objects->texts);
	redisplay_lineobject(objects->lines);
	redisplay_ellipseobject(objects->ellipses);
	redisplay_arcobject(objects->arcs);
	redisplay_splineobject(objects->splines);
	redisplay_compoundobject(objects->compounds);
	if (pointmarker_shown) toggle_pointmarker();
	if (compoundbox_shown) {
	    F_compound	*c;
	    for (c = objects->compounds; c != NULL; c = c->next)
		draw_compoundbox(c, INV_PAINT);
	    }
	}

redisplay_arcobject(arcs)
F_arc	*arcs;
{
	F_arc	*arc;

	for (arc = arcs; arc != NULL; arc = arc->next)
	    draw_arc(arc, foreground_color);
	}

redisplay_ellipseobject(ellipses)
F_ellipse	*ellipses;
{
	F_ellipse	*e;

	for (e = ellipses; e != NULL; e = e->next)
	    draw_ellipse(e, foreground_color);
	}

redisplay_lineobject(lines)
F_line	*lines;
{
	F_line	*line;

	for (line = lines; line != NULL; line = line->next) 
	    draw_line(line, PAINT);
	}

redisplay_splineobject(splines)
F_spline	*splines;
{
	F_spline	*s;

	for (s = splines; s != NULL; s = s->next)
	    draw_spline(s, PAINT);
	}

redisplay_textobject(texts)
F_text	*texts;
{
	F_text	*t;

	for (t = texts; t != NULL; t = t->next)
	    draw_text(t, PAINT);
	}

redisplay_compoundobject(compounds)
F_compound	*compounds;
{
	F_compound	*c;

	for (c = compounds; c != NULL; c = c->next) {
	    draw_compound(c);
	    }
	}

redisplay_canvas()
{
	extern F_compound	objects;

	clear_canvas();
	set_temp_cursor(&wait_cursor);
	pw_batch_on(canvas_pixwin);
	redisplay_objects(&objects);
	redisplay_grid();
	pw_batch_off(canvas_pixwin);
	reset_cursor();
	}
