	/* $Header: /u/04c/mnt/integ/sxt/include/RCS/sxt.h,v 1.3 89/02/17 01:17:02 ejh Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:
|	sxt.h
|
|  Description:
|	The interface between the dialogue manager and X11 with the X Toolkit.
|	This includes the Athena widgets.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	sxt.c
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
$Log:	sxt.h,v $
 * Revision 1.3  89/02/17  01:17:02  ejh
 * no change
 * 
 * Revision 1.2  89/02/09  20:18:51  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:48:14  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	21 Mar 88	added Canvas and Line widgets	kwh
|	11 Mar 88	created		kwh
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

#ifndef SXT_H
#define SXT_H

static	char	*change_type_names[] =
	{"isd_create", "isd_modify", "isd_remove"};
static	char	*data_type_names[] =
	{"idd_boolean", "idd_integer", "idd_real", "idd_string", "idd_record",
	"idd_id", "idd_buffer"};

static	char	*event_names[] =
	{"Protocol Error", "Protocol Error", "KeyPress", "KeyRelease",
	"ButtonPress", "ButtonRelease", "MotionNotify", "EnterNotify",
	"LeaveNotify", "FocusIn", "FocusOut", "KeymapNotify", "Expose",
	"GraphicExpose", "NoExpose", "VisibilityNotify", "CreateNotify",
	"DestroyNotify", "UnmapNotify", "MapNotify", "MapRequest",
	"ReparentNotify", "ConfigureNotify", "ConfigureRequest",
	"GravityNotify", "ResizeRequest", "CirculateNotify",
	"CirculateRequest", "PropertyNotify", "SelectionClear",
	"SelectionRequest", "SelectionNotify", "ColormapNotify",
	"ClientMessage", "MappingNotify", "LASTEvent"};

#endif
