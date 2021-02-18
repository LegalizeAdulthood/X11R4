/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/update.c,v 2.8 89/08/29 10:39:48 mcinerny Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/update.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/update.c,v 2.8 89/08/29 10:39:48 mcinerny Exp $ ";
#endif /* lint */

#include <andyenv.h>
#include <ms.h>
#include <mailconf.h>
#ifndef hp9000s300
#include <sys/wait.h>
#else /* hp9000s300 */
#include <sys/time.h>
#include <signal.h>
#endif /* hp9000s300 */

#ifdef WHITEPAGES_ENV
#define CLOSE_WP_DELTA (10*60)	/* close WP every 10 minutes */
#endif /* WHITEPAGES_ENV */

extern int SubscriptionsAreDirty;
int NeedToTimeOut = 0;
static int MS_ShouldCleanZombies = 1;

/* This is a FAKE routine -- only used in non-snap version, and faked in cuisnap.c */

MS_SetCleanupZombies(value)
int value;
{
    MS_ShouldCleanZombies = value;
}

MS_Die() {
    MS_UpdateState();
    safeexit(0);
}

MS_FastUpdateState() {
    return(UpdateState(FALSE));
}

MS_UpdateState() {
    return(UpdateState(TRUE));
}

VitalCheckpoints(DoEverything, errct)
Boolean DoEverything;
int *errct;
{
    long retcode = 0;

    if (CloseNeedyDirs(DoEverything)) {
	retcode = mserrcode;
	++*errct;
    }
    if (SubscriptionsAreDirty && WriteSubs()) {
	retcode = mserrcode;
	++*errct;
    }
    return(retcode);
}
    
#ifdef hp9000s300
setup_sigAlrm()
{ }
#endif /* hp9000s300 */


UpdateState(DoEverything)
Boolean DoEverything;
{
#ifndef hp9000s300
    union wait status;
#endif /* hp9000s300 */
    int pid, retcode = 0, errct = 0;
#ifdef WHITEPAGES_ENV
    static long time_when_wp_last_closed = 0;
    long time_now;
#endif /* WHITEPAGES_ENV */

#ifdef WHITEPAGES_ENV
    time_now = time(0);
    if (time_when_wp_last_closed == 0) time_when_wp_last_closed = time_now;
#endif /* WHITEPAGES_ENV */
    retcode = VitalCheckpoints(DoEverything, &errct);
    if (PrintPendingRequests(DoEverything)) {
	retcode = mserrcode;
	++errct;
    }

    if (DoEverything) {
#ifdef WHITEPAGES_ENV
	time_when_wp_last_closed = time_now;
	CloseWhitePages();
#endif /* WHITEPAGES_ENV */
	getla_ShutDown();
	dropoff_Reset();
    } else {			/* if not DoEverything */
#ifdef WHITEPAGES_ENV
				/* on fast updates, close WP every
				  CLOSE_WP_DELTA seconds */
      if ((time_now - time_when_wp_last_closed) > CLOSE_WP_DELTA) {
	time_when_wp_last_closed = time_now;
	CloseWhitePages();
      }
#endif /* WHITEPAGES_ENV */
    }

    if (MS_ShouldCleanZombies) {
#ifdef hp9000s300
	int status;
	struct sigvec vecAlrm;
	struct itimerval timer;

	/** enable an interval timer so we can escape from wait(); **/
	vecAlrm.sv_handler = setup_sigAlrm;
	vecAlrm.sv_mask = 0;
	vecAlrm.sv_flags = 0;
	sigvector(SIGALRM, &vecAlrm, 0);
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 100000;
	timer.it_interval.tv_sec = 0;
	timer.it_interval.tv_usec = 100000;
	setitimer(ITIMER_REAL, &timer, 0);

	while ((pid = wait(&status)) > 0) ;

	/** disable the timer **/
	timer.it_value.tv_sec = 0;
	timer.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &timer, 0);
#else /* hp9000s300 */
	do { /* Have you checked the children? */
	    pid = wait3(&status, WNOHANG, NULL);
	} while (pid > 0);
#endif /* hp9000s300 */
    }
    NeedToTimeOut = 0;
    if (errct > 1) {
	char ErrorText[256];
	sprintf(ErrorText, "There were %d different errors in updating messagserver state!", errct);
	NonfatalBizarreError(ErrorText);
    }

    return(retcode);
}
