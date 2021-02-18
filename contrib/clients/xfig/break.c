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

			init_break();

break_selected()
{
	extern	(*canvas_kbd_proc)();
	extern	(*canvas_locmove_proc)();
	extern	(*canvas_leftbut_proc)();
	extern	(*canvas_middlebut_proc)();
	extern	(*canvas_rightbut_proc)();
	extern	null_proc();
	extern	set_popupmenu();

	canvas_kbd_proc = null_proc;
	canvas_locmove_proc = null_proc;
	canvas_leftbut_proc = init_break;
	canvas_middlebut_proc = null_proc;
	canvas_rightbut_proc = set_popupmenu;
	set_cursor(&pick15_cursor);
	}

init_break(x, y)
int	x, y;
{
	extern F_compound	*compound_search();
	extern F_compound	objects, saved_objects;
	extern int		compoundbox_shown;
	F_compound	*c;

	if ((c = compound_search(x, y, TOLERANCE, &x, &y)) == NULL) return;
	clean_up();
	if (compoundbox_shown) draw_compoundbox(c, INV_PAINT);
	delete_compound(&objects.compounds, c);
	break_compound(c);
	set_action(F_BREAK);
	saved_objects.compounds = c;
	break_selected();
	}

break_compound(c)
F_compound	*c;
{
	extern F_compound	objects, object_tails;
	extern int		pointmarker_shown;

	if (pointmarker_shown) toggle_compoundpointmarker(c);
	tail(&objects, &object_tails);
	append_objects(&objects, c, &object_tails);
	}
