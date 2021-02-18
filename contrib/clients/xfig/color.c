/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *      March 1988.
 *
 *	%W%	%G%
*/

int			background_color = 0;	/* black */
int			foreground_color = 1;	/* white */

invert_color()
{
	int	c;

	c = background_color;
	background_color = foreground_color;
	foreground_color = c;
	}
