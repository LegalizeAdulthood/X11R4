	/* $Header: /u/04c/mnt/new_integ/saw/include/RCS/xdb.h,v 1.3 89/10/08 01:20:33 serpent Exp $ */

/*--------------------------------------------------------------------------*\
|
|  Name:
|	xdb.h
|
|  Description:
|	contains external declarations of the *_debug variables and a routine
|	for parsing command lines of the form:
|		-* level
|	where * = {isd, idd, ipc, ids, ism, isc, ubs, xht, ... } and
|	<level> = {0, 1, 2, 3}.
|
|  Component of:
|	SerpentX
|
|  Related files:
|	xdb.c
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
$Log:	xdb.h,v $
 * Revision 1.3  89/10/08  01:20:33  serpent
 * was referring to old sxt debugs in error
 * 
 * Revision 1.2  89/02/09  20:24:43  ejh
 * added copyright notice
 * 
 * Revision 1.1  88/11/14  16:54:16  ejh
 * Initial revision
 * 
|	 2 Nov 88	project standards imposed	erik
|	14 Sep 88	updated to reflect xpw	erik
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

#ifndef XDB_H
#define XDB_H

#include	<stdio.h>

extern	int	isd_debug;
extern	int	idd_debug;
extern	int	iid_debug;
extern	int	ipc_debug;
extern	int	ids_debug;
extern	int	ism_debug;
extern	int	isc_debug;
extern	int	ubs_debug;

#ifdef	xdb_TOP

int	x22_debug = 0;
int	xht_debug = 0;
int	xct_debug = 0;
int	xcp_debug = 0;
int	xfw_debug = 0;
int	xlw_debug = 0;
int	xcm_debug = 0;
int	xcn_debug = 0;
int	xbw_debug = 0;
int	xsb_debug = 0;
int	xvp_debug = 0;
int	xvt_debug = 0;
int	xdw_debug = 0;
int	xtw_debug = 0;
int	xpw_debug = 0;

#else

extern	int	x22_debug;
extern	int	xht_debug;
extern	int	xct_debug;
extern	int	xcp_debug;
extern	int	xfw_debug;
extern	int	xlw_debug;
extern	int	xcm_debug;
extern	int	xbw_debug;
extern	int	xsb_debug;
extern	int	xvp_debug;
extern	int	xvt_debug;
extern	int	xdw_debug;
extern	int	xtw_debug;
extern	int	xpw_debug;

#endif

#define	xpw_start_print()\
	xpw_print (start printing widgets, NULL)

#define	xpw_set_printing_widget(thing)\
	xpw_print (current printing widget: %s, thing)

#define xpw_print(quote,value)\
	{\
	xpw_DEBUG2 (fprintf (stdout, "xpw ***: "));\
	xpw_DEBUG2 (fprintf (stdout, "quote", value));\
	xpw_DEBUG2 (fprintf (stdout, "\n"));\
	}

#define xpw_DEBUG2(statement)\
	if (xpw_debug > 1)\
		statement

/*--------------------------------------------------------------------------*\
|  Name: xdb_parse_debug
|
|
\* Syntax:								    */

	void	xdb_parse_debug	(
	/*	argc:	in	int,
		argv:	in	string
	*/
	);

/*
|  Parameters:
|	argc	length of argv
|	argv	command line arguments from main
|
|  Description
|	xdb_parse_debug will find the switches it wants and set the
|	appropriate debug variables.
\*--------------------------------------------------------------------------*/

#endif
