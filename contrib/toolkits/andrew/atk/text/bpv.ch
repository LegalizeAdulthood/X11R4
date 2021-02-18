/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
 
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bpv.ch,v 1.1 89/05/22 16:11:06 tpn Exp $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_bpv_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bpv.ch,v 1.1 89/05/22 16:11:06 tpn Exp $ ";
#endif

class bpv: view {
overrides:
    FullUpdate(enum view_UpdateType type, long left, long top, long width, long right);
    DesiredSize(long width, long height, enum view_DSpass pass, long *dWidth, long *dheight) returns enum view_DSattributes;
    Print(FILE *file, char *processor, char *finalFormat, boolean topLevel);
methods:
classprocedures:
data:
};

