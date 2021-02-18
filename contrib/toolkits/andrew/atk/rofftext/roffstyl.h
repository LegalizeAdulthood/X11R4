/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffstyl.h,v 2.5 89/02/17 17:05:32 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffstyl.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffstyl.h,v 2.5 89/02/17 17:05:32 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */


/* style code for be2roff */

InitText();
WriteText();
TextPut();
ChangeStyle();
CloseStyle();
CloseAllStyles();
