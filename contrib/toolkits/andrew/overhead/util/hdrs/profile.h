/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/profile.h,v 2.3 89/02/16 12:58:17 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/profile.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/profile.h,v 2.3 89/02/16 12:58:17 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

struct proFile {
    long lastmod;
    struct proFile *next;
    char name[1];
};

struct proFile *proFileList();
