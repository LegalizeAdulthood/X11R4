/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/fdplumbi.h,v 2.4 89/02/16 12:58:11 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/fdplumbi.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/fdplumbi.h,v 2.4 89/02/16 12:58:11 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#define FDLEAK_OPENCODE_OPEN 1
#define FDLEAK_OPENCODE_FOPEN 2
#define FDLEAK_OPENCODE_POPEN 3
#define FDLEAK_OPENCODE_QOPEN 4
#define FDLEAK_OPENCODE_TOPEN 5
#define FDLEAK_OPENCODE_T2OPEN 6
#define FDLEAK_OPENCODE_CREAT 7
#define FDLEAK_OPENCODE_DUP 8
#define FDLEAK_OPENCODE_PIPE 9
#define FDLEAK_OPENCODE_SOCKET 10
#define FDLEAK_OPENCODE_OPENDIR 11
