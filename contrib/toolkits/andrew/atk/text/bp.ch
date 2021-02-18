/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
 
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bp.ch,v 1.2 89/06/08 10:15:54 tpn Exp $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_bp_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/bp.ch,v 1.2 89/06/08 10:15:54 tpn Exp $ ";
#endif

class bp : dataobject [dataobj] {
overrides:
      ViewName() returns char *;
};

