/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/rm.ch,v 2.5 89/02/17 18:40:29 ghoti Exp $ */
/* $ACIS:rm.ch 1.5$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/rm.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidrm_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/rm.ch,v 2.5 89/02/17 18:40:29 ghoti Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


#include <atomlist.ih>

struct resourceList
{
  struct atomlist * name;
  struct atom * type;
  long data;
  short found;
};


package rm
{
 classprocedures:
  InitializeClass() returns boolean;
  ContextualPostResource( struct atomlist * context, * path,
			 long data, struct atom * type );
  PostResource( struct atomlist * path, long data,
	       struct atom * type );
  PostManyResources( struct resourceList * resources,
		    struct atomlist * context );
  GetResource( struct atomlist * name,
	      struct atomlist * className,
	      struct atom * type,
	      long * data ) returns short;
  GetManyResources( struct resourceList * resources,
		   struct atomlist * name,
		   struct atomlist * className );
  PostConverter( struct atom * fromtype, struct atom * totype, procedure converter );
};


/* conversion routines are called Convert( fromrock, outputrock )
   they should return FALSE (0) if the conversion fails */
