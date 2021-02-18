/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/fdplumb.h,v 2.3 89/02/16 12:58:06 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/fdplumb.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/fdplumb.h,v 2.3 89/02/16 12:58:06 ghoti Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

#ifndef PLUMBFDLEAKS
#define PLUMBFDLEAKS
#define open dbg_open
#define fopen dbg_fopen
#define close dbg_close
#define fclose dbg_fclose
#define vclose dbg_vclose
#define vfclose dbg_vfclose
#define popen dbg_popen
#define pclose dbg_pclose
#define topen dbg_topen
#define tclose dbg_tclose
#define t2open dbg_t2open
#define t2close dbg_t2close
#define qopen dbg_qopen
#define qclose dbg_qclose
#define creat dbg_creat
#define dup dbg_dup
#define dup2 dbg_dup2
#define pipe dbg_pipe
#define socket dbg_socket
#define socketpair dbg_socketpair
#define opendir dbg_opendir
#define closedir dbg_closedir

extern int fdplumb_LogAllFileAccesses;
extern int fdplumb_SpillGuts();
#endif /* PLUMBFDLEAKS */
