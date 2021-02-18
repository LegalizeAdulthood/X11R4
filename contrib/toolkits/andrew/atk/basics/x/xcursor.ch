/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xcursor.ch,v 1.4 89/03/22 10:28:50 wjh Exp $ */
/* $ACIS:cursor.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xcursor.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidxcursor_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/x/RCS/xcursor.ch,v 1.4 89/03/22 10:28:50 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* 
 *	cursor.ch
 */

#include <rect.h>

class xcursor : cursor {
overrides:
	SetStandard(short ch);
	ChangeShape();
classprocedures:
	InitializeObject(struct xcursor *self) returns boolean;
	FinalizeObject(struct xcursor *self); /*  used to get rid of active cursors */
data:
	Cursor  Xc;
	Window Xw;
	Display * Xd;
	XColor *forecolor;
	XColor *backcolor;
};
