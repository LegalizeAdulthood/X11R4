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
#include "font.h"

PIX_FONT		bold_font;
PIX_FONT		roman_font;
PIX_FONT		canvas_font;

#ifndef	X11
init_font()
{
	bold_font = pf_open("/usr/lib/fonts/fixedwidthfonts/screen.b.14");
	roman_font = pf_open("/usr/lib/fonts/fixedwidthfonts/screen.r.14");
	if (bold_font == NULL) bold_font = pf_default();
	if (roman_font == NULL) roman_font = pf_default();
	canvas_font = roman_font;
	}
#else
init_font()
{
	canvas_font = roman_font = XLoadQueryFont(tool_d, NORMAL_FONT);
	bold_font = XLoadQueryFont(tool_d, BOLD_FONT);
}
#endif	X11
