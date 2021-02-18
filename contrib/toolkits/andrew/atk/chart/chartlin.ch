/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidchartlin_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/chart/RCS/chartlin.ch,v 1.2 89/06/02 16:53:29 tom Exp $";
#endif

/*
    $Log:	chartlin.ch,v $
Revision 1.2  89/06/02  16:53:29  tom
Provide default HitHandler.

Revision 1.1  89/05/04  19:59:10  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Line Chart View-object

MODULE	chartlin.ch

VERSION	0.0

NOTICE	IBM Internal Use Only

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Chart View-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  05/04/89	Created (TCP)
  06/02/89	Use super-class Hit method (TCP)

END-SPECIFICATION  ************************************************************/

#define  chartlin_VERSION		    1

class chartlin : chartobj
  {
overrides:

  DrawChart();
  PrintChart();
  Moniker()				    returns char *;
  SetDebug( boolean state );

methods:

classprocedures:

  InitializeObject( struct chartlin *self ) returns boolean;
  FinalizeObject();

data:

  };
