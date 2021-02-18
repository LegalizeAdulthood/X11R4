/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidorg_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/org/RCS/org.ch,v 1.3 89/08/23 15:39:29 tom Exp $";
#endif

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Org Data-Class

MODULE	org.ch

VERSION	1.0

AUTHOR	TC Peters
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Org Data-Class.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  01/19/89	Created (TCP)
  08/23/89	Remove Create method (TCP)

END-SPECIFICATION  ************************************************************/


#define  org_VERSION	      1

class org : apt
  {
overrides:

  Read( FILE *file, long id )				returns long;
  Write( FILE *file, long id, long level )		returns long;
  ViewName()						returns char *;

methods:

  SetFileName( char *filename )				returns boolean;
  SetDebug( boolean state );
  NodeName( node )					returns char *;

macromethods:

  FileName()		((self)->tree_file_name)

classprocedures:

  InitializeObject( struct org *self )			returns boolean;
  FinalizeObject( struct org *Self );

data:

  struct tree		  	 *tree_data_object;
  char				 *tree_file_name;
  };
