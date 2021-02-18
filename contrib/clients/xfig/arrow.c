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
#include "alloc.h"
#include "object.h"

static float		forward_arrow_wid = 4;
static float		forward_arrow_ht = 8;
static int		forward_arrow_type = 0;
static int		forward_arrow_style = 0;
static float		forward_arrow_thickness = 1;

static float		backward_arrow_wid = 4;
static float		backward_arrow_ht = 8;
static int		backward_arrow_type = 0;
static int		backward_arrow_style = 0;
static float		backward_arrow_thickness = 1;

F_arrow *
forward_arrow()
{
	F_arrow		*a;

	if (NULL == (Arrow_malloc(a))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	a->type = forward_arrow_type;
	a->style = forward_arrow_style;
	a->thickness = forward_arrow_thickness;
	a->wid = forward_arrow_wid;
	a->ht = forward_arrow_ht;
	return(a);
	}

F_arrow *
backward_arrow()
{
	F_arrow		*a;

	if (NULL == (Arrow_malloc(a))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	a->type = backward_arrow_type;
	a->style = backward_arrow_style;
	a->thickness = backward_arrow_thickness;
	a->wid = backward_arrow_wid;
	a->ht = backward_arrow_ht;
	return(a);
	}

F_arrow *
make_arrow(type, style, thickness, wid, ht)
int	type, style;
float	thickness, wid, ht;
{
	F_arrow		*a;

	if (NULL == (Arrow_malloc(a))) {
	    put_msg(Err_mem);
	    return(NULL);
	    }
	a->type = type;
	a->style = style;
	a->thickness = thickness;
	a->wid = wid;
	a->ht = ht;
	return(a);
	}
