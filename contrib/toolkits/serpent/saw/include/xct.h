	/* $Header: /u/04c/mnt/integ/sxt/include/RCS/xct.h,v 1.3 89/02/17 03:21:56 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xct.h
|
|  Description:
|	The interface to the color table for SerpentX.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xct.c
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
$Log:	xct.h,v $
 * Revision 1.3  89/02/17  03:21:56  ejh
 * fixed x includes
 * 
 * Revision 1.2  89/02/09  20:24:01  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:53:38  ejh
 * Initial revision
 * 
|	28 Oct 88	project standards imposed	erik
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

#ifndef XCT_H
#define XCT_H

#include <X11/Intrinsic.h>

/*--------------------------------------------------------------------------*\
|  Name: xct_make_color_table
|
|  Description:
|	xct_make_color_table constructs a hash table of XColor structures
|	which match the string color names in the array color_names. The table
|	key is the string itself. The hash table is called x_color_table. This
|	is a global variable which is changed by this function.
|
\*  Syntax:                                                                 */
	void xct_make_color_table (
	/*	NONE
	*/
	);
/*                                                                          *\
|  Parameters: None.
|
|  Returns: Nothing.
\*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*
|  Name: xct_get_pixel
|
|  Description:
|	xct_get_pixel retrieves the pixel value for a color from
|	xt_color_table. If the color does not exist, it returns (~0L).
|
\*  Syntax:                                                                 */
	Pixel	xct_get_pixel (
	/*
		color_name:	in	*char
	*/);
/*                                                                          *\
|  Parameters:
|	color_name	the color for which the user needs the pixel value.
|
|  Returns:
|	(~0L) means the color does not exist in the table; other values are
|	the X Pixel values corresponding to the color named in color_name.
\*--------------------------------------------------------------------------*/

#endif
