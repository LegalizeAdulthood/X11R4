/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/wws.ch,v 1.5 89/05/21 23:11:39 wjh Exp $ */
/* $ACIS:ws.ch 1.2$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/wws.ch,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsidwmws_H = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/basics/wm/RCS/wws.ch,v 1.5 89/05/21 23:11:39 wjh Exp $";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* WM implementation of the windowsystem class.
 */

class wmws[wws] : windowsystem[winsys] {
    overrides:
        CreateCursor() returns struct wmcursor *;
        CreateFontdesc() returns struct wmfontdesc *;
        CreateGraphic() returns struct wmgraphic *;
        CreateIM() returns struct wmim *;
	CreateOffScreenWindow(char * host, long width, long height) returns struct wmoffscrwin *;
        FlushAllWindows();
        HandleFiles(long WaitTime, boolean beCheap) returns boolean;
    classprocedures:
        InitializeClass() returns boolean;
};
