/* 
 *	FIG : Facility for Interactive Generation of figures
 *
 *	Copyright (c) 1988 by Supoj Sutanthavibul (supoj@sally.UTEXAS.EDU)
 *	January 1988.
 *
 *	%W%	%G%
*/

EXTERN PIX_FONT		canvas_font;
EXTERN PIX_FONT		bold_font;
EXTERN PIX_FONT		roman_font;

#ifndef	X11

#define			char_height(font)	((font)->pf_defaultsize.y)
#define			char_width(font)	((font)->pf_defaultsize.x)
#define			char_advance(font,char)	((font)->pf_char[(char)].pc_adv.x)

#else

#define		NORMAL_FONT	"fixed"
#define		BOLD_FONT	"8x13bold"

#define			char_height(font)	((font)->max_bounds.ascent + (font)->max_bounds.descent)
#define			char_width(font)	((font)->max_bounds.width)
#define			char_advance(font,char)	(\
			((font)->per_char)?\
				((font)->per_char[(char)].width):\
				((font)->max_bounds.width))

#endif	X11
