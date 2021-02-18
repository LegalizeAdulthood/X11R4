/*	@(#)charimage.h 20.8 89/08/18 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Definitions relating to maintenance of virtual screen image.
 */

/*
 * Screen is maintained as an array of characters.
 * Screen is bottom lines and right columns.
 * Each line has length and array of characters.
 * Characters past length position are undefined.
 * Line is otherwise null terminated.
 */
extern char	**image;
extern char	**screenmode;
extern int	ttysw_top, ttysw_bottom, ttysw_left, ttysw_right;
extern int	cursrow, curscol;

#define length(line)	((unsigned char)((line)[-1]))

#define MODE_CLEAR	0
#define MODE_INVERT	1
#define MODE_UNDERSCORE	2
#define MODE_BOLD	4

#define	setlinelength(line, column) \
	{ int _col = ((column)>ttysw_right)?ttysw_right:(column); \
	  (line)[(_col)] = '\0'; \
	  line[-1] = (unsigned char) (_col);}
