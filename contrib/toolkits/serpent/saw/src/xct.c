	static char rcsid [] = "$Header: /u/04c/mnt/integ/sxt/src/RCS/xct.c,v 1.3 89/02/17 01:52:56 ejh Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xct.c
|
|  Description:
|	The implementation of the color lookup table for SerpentX; the main
|	routine transforms a string color name into the appropriate X Pixel
|	value
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xct.h
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version:
|	1.0
|
|  Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|  Authors:
|	Kurt W. Hoyt
|	kwh@sei.cmu.edu
|
|	Erik J. Hardy
|	erik@sei.cmu.edu
|
|  Change History:
$Log:	xct.c,v $
 * Revision 1.3  89/02/17  01:52:56  ejh
 * no changes
 * 
 * Revision 1.2  89/02/09  21:47:05  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:02:07  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	30 May 88	created		kwh
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#define memoryPack

#include	<stdio.h>
#include	"xct.h"
#include	"hashPack.h"

#include	"xdb.h"

static	char	*color_names [] =
    {
    "aquamarine", "medium aquamarine", "black", "blue", "cadet blue",
    "cornflower blue", "dark slate blue", "light blue", "light steel blue",
    "medium blue", "medium slate blue", "midnight blue", "navy blue",
    "navy", "sky blue", "slate blue", "steel blue", "coral", "cyan",
    "firebrick", "gold", "goldenrod", "medium goldenrod", "green",
    "dark green", "dark olive green", "forest green", "lime green",
    "medium forest green", "medium sea green", "medium spring green",
    "pale green", "sea green", "spring green", "yellow green",
    "dark slate grey", "dark slate gray", "dim grey", "dim gray",
    "light grey", "light gray", "khaki", "magenta", "maroon", "orange",
    "orchid", "dark orchid", "medium orchid", "pink", "plum", "red",
    "indian red", "medium violet red", "orange red", "violet red", "salmon",
    "sienna", "tan", "thistle", "turquoise", "dark turquoise",
    "medium turquoise", "violet", "blue violet", "wheat", "white", "yellow",
    "green yellow"
    };

#define	NUM_COLORS	68

#define	HASH_SIZE	1013
HASH	x_color_table;

typedef	struct	{
    char	*name;
    XColor	color;
    }	xct_color_node;

/*--------------------------------------------------------------------------*\
|  Name: xct_hash_string
|
|  Description:
|	xct_hash_string is the hashing function for the Zimmerman hash table
|	package when using a color string as the lookup key. This function is
|	called by the Zimmerman package.
|
|  Parameters:
|	name	the string used to identify a particular node in a hash table;
|		converted by this function into an integer between 0 and
|		(HASH_SIZE - 1).
|
|  Returns: the sum of the individual characters in name modulo HASH_SIZE.
\*--------------------------------------------------------------------------*/

int	xct_hash_string (name)
char	*name;

    {
    register	int	hash_value = 0;
    register	char	*pointer;

    if (xct_debug > 1)
	{
	fprintf (stderr, "xct_hash_string: Entering.\n");
	fprintf (stderr, "xct_hash_string: name = %s.\n", name);
	}

/*	loop through each character in the string 'name' and add the character
	value to the accumulator 'hash_value'. */

    for (pointer = name; *pointer != '\0'; pointer++)
	hash_value += *pointer;

/*	return a value in the right range. */

    if (xct_debug > 1)
	fprintf (stderr, "xct_hash_string: Leaving.\n");

    return (hash_value % HASH_SIZE);
    }

/*--------------------------------------------------------------------------*\
|  Name: xct_match_component_name
|
|  Description:
|	xct_match_component is called by the Zimmerman hash table package in
|	order to match queries to nodes in a table.
|
|  Parameters:
|	id	the string used to look up a particular node in a hash table
|		keyed by a string.
|	color	the node from the hash table whose hash value matched the hash
|		value of id and is possibly the node being looked up.
|
|  Returns: true if a match occurs and false if there is not a match.
\*--------------------------------------------------------------------------*/

int	xct_match_component_name (id, color)
char		*id;
xct_color_node	*color;

    {
    if (xct_debug > 1)
	{
	fprintf (stderr, "xct_match_component_name: Entering.\n");
	fprintf (stderr, "xct_match_component_name: id = %s\n", id);
	fprintf (stderr, "xct_match_component_name: color = %s\n",
	    color->name);
	fprintf (stderr, "xct_match_component_name: Leaving.\n");
	}

    return (strcmp (id, color->name) == 0);
    }

/*--------------------------------------------------------------------------*\
|  Name: xct_make_color_table
|
|  Description:
|	xct_make_color_table constructs a hash table of XColor structures
|	which match the string color names in the array color_names; the table
|	key is the string itself; the hash table is called x_color_table; this
|	is a global variable which is changed by this function.
|
|  Parameters: None.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

void	xct_make_color_table (display)
Display	*display;

    {
    xct_color_node	*temp;
    int			i;
    XColor		dummy;

    if (xct_debug > 1)
	fprintf (stderr, "xct_make_color_table: Entering.\n");

    x_color_table = make_hashtable (HASH_SIZE, xct_hash_string,
	xct_match_component_name);

    for (i = 0; i < NUM_COLORS; i++)
	{
	temp = (xct_color_node *) make_node (sizeof (xct_color_node));
	temp->name = color_names [i];

	XAllocNamedColor (display, DefaultColormap (display,
	    DefaultScreen (display)), color_names [i], &(temp->color),&dummy);

	add_to_hashtable (x_color_table, temp, temp->name);
	}

    if (xct_debug > 1)
	fprintf (stderr, "xct_make_color_table: Leaving.\n");
    }

/*--------------------------------------------------------------------------*
|  Name: xct_get_pixel
|
|  Description:
|	xct_get_pixel retrieves the pixel value for a color from
|	xt_color_table. If the color does not exist, it returns (~0L).
|
|  Parameters:
|	color_name	the color for which the user needs the pixel value.
|
|  Returns: nothing
\*-------------------------------------------------------------------------*/

Pixel	xct_get_pixel (color_name)
char	*color_name;

    {
    xct_color_node	*temp;

    if (xct_debug > 1)
	{
	fprintf (stderr, "xct_get_pixel: Entering.\n");
	fprintf (stderr, "xct_get_pixel: color_name = %s.\n", color_name);
	}

    temp = (xct_color_node *) get_from_hashtable (x_color_table, color_name);

    if (temp == NULL)
	{
	if (xct_debug > 1)
	    {
	    fprintf (stderr, "xct_get_pixel: Error -- No color found.\n");
	    fprintf (stderr, "xct_get_pixel: Leaving.\n");
	    }
	return (~0L);
	}
    else
	{
	if (xct_debug > 1)
	    {
	    fprintf (stderr, "xct_get_pixel: pixel is %d.\n",
		temp->color.pixel);
	    fprintf (stderr, "xct_get_pixel: Leaving.\n");
	    }
	return (temp->color.pixel);
	}
    }
