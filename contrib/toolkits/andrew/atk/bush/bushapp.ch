/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidbushapp_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/bush/RCS/bushapp.ch,v 1.1 89/04/28 21:24:06 tom Exp $";
#endif

/*
    $Log:	bushapp.ch,v $
Revision 1.1  89/04/28  21:24:06  tom
Initial revision

Revision 1.1  89/04/28  21:20:42  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	Bush Application

MODULE	bushapp.ch

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the Application layer for the Bush Inset..

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  08/21/85	Created (TCP)
  01/15/89	Convert to ATK (GW Keim)

END-SPECIFICATION  ************************************************************/

class bushapp: application [app]{
    classprocedures:
	InitializeObject( struct bushapp *self ) returns boolean;
	FinalizeObject( struct bushapp *self ) returns void;
    overrides:
	ParseArgs( int argc, char **argv ) returns boolean;
	Start() returns boolean;
    data:
	struct im	    *im;
	struct frame	    *frame;
	char		     program_name[40];
	struct bush	    *bush;
	struct bushv	    *current_window;
	struct bushv	    *nodes_window;
	struct bushv	    *entries_window;
	struct bushv	    *entry_window;
	int		     argc;
	char		   **argv;
	int		     debug;
};

