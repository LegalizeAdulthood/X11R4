/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/calc/RCS/calc.ch,v 1.2 89/08/04 17:09:31 tom Exp $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/calc/RCS/calc.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidcalc_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/calc/RCS/calc.ch,v 1.2 89/08/04 17:09:31 tom Exp $";
#endif

/*
    $Log:	calc.ch,v $
Revision 1.2  89/08/04  17:09:31  tom
Accept keyboard inputs;
Suppress Shrink & Help Icons.

Revision 1.1  89/05/10  20:57:54  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Calc Data-object

MODULE	calc.ch

VERSION	1.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that suport the Calc Data-object.

HISTORY
  02/23/88	Created (TCP)

END-SPECIFICATION  ************************************************************/


#define  calc_VERSION    1

#define  calc_value_changed				1


class calc : apt
  {

overrides:

  Read( FILE *file, long id )				returns long;
  Write( FILE *file, long id, long level )		returns long;
  ViewName()						returns char *;

methods:

  SetValue( double value );

macromethods:

  Value()			      ((self)->value)

classprocedures:

  InitializeObject( struct calc *self )			returns boolean;
  FinalizeObject( struct calc *self );

data:

  long				      memory;
  double			      value;
  long				      id;
  };

