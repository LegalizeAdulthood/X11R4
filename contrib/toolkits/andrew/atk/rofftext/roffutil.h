/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffutil.h,v 2.3 89/02/17 17:06:13 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffutil.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/rofftext/RCS/roffutil.h,v 2.3 89/02/17 17:06:13 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* utilities for rofftext */
Add2Buf();
BUF NewBuf();
CreateEnvirons();
DestroyEnvirons();
PushEnviron();
PopEnviron();
struct diversionLevel *CreateDiversion();
DestroyDiversion();
PushDiversion();
PopDiversion();
