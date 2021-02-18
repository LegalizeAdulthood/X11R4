/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/winsys.ch,v 2.7 89/05/21 22:58:43 wjh Exp $ */
/* $ACIS:winsys.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/winsys.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidwindowsystem_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/common/RCS/winsys.ch,v 2.7 89/05/21 22:58:43 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* Window system package.
 * Provides an abstraction for dealing with multiple window management systems
 * using a singe executable.
 */

class windowsystem[winsys] {
    methods:
        CreateIM() returns struct im *;
        CreateGraphic() returns struct graphic *;
        CreateCursor() returns struct cursor *;
        CreateFontdesc() returns struct fontdesc *;
	CreateOffScreenWindow(char * host, long width, long height) returns struct offscrwin *;
        FlushAllWindows();
        HandleFiles(long WaitTime, boolean beCheap) returns boolean;
};
