/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: */
/* $ACIS: */
/* $Source: */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */


#include <system.h>
#include <stdio.h>
#include <signal.h>
#include <andrewos.h>		/* sys/time.h */
#include <setjmp.h>
#include <errno.h>
extern int errno;
static int *popen_pid = NULL;
static int dtablesize = -1;

#ifdef VMUNIX
#define	mask(s)	(1<<((s)-1))
#endif /* VMUNIX */

static int (*oldfunc)();

int t2open(name, argv, r, w)
char *name, *argv[];
FILE **r, **w;
{
    int p1[2], p2[2], pgrp;

    if (popen_pid == NULL) {
	dtablesize = getdtablesize();
	if (dtablesize > 0) {
	    popen_pid = (int *) malloc(dtablesize * sizeof(int));
	}
    }
    if (popen_pid == NULL) {errno = ENOMEM; return -1;}

    /* Set up to ignore broken pipe signals */
    oldfunc = (int (*)()) signal(SIGPIPE, SIG_IGN);

    if(pipe(p1) < 0 || pipe(p2) < 0) return -1;
    /* p1 is for writing from child to parent, p2 vice versa */

    pgrp = osi_vfork();
    if(pgrp == 0) {
	int pid, fd;

	/* This is child */
	pid = getpid();
#ifdef hpux
	setpgrp();
#else /* hpux */
	setpgrp(pid, pid);
#endif /* hpux */

	/* Set up my write side */
	close(p1[0]);		/* I don't read from p1 */
	dup2(p1[1], 1);		/* My stdout is now p1 */
	close(p1[1]);

	/* Set up my read side */
	close(p2[1]);
	dup2(p2[0], 0);
	close(p2[0]);

	for (fd = dtablesize; fd > 2; --fd) close(fd);
	execv(name, argv);
	_exit(0377);
    }
    if (pgrp == -1) return -1;	/* VFORK failed */

    /* I am parent */
    popen_pid[p2[1]] = pgrp;	/* Index by write descriptor */

    /* Set up my write side */
    close(p2[0]);
    *w = fdopen(p2[1], "w");
    if (*w == NULL) return -1;

    /* Set up read side */
    close(p1[1]);
    *r = fdopen(p1[0], "r");
    if (*r == NULL) return -1;
    return pgrp;
}

static jmp_buf env;

static lclalarm()
{
    longjmp(env, 1);
}

int t2close(ptr, seconds, timedout)
    FILE *ptr;
    int seconds, *timedout;
{
    int status, omask;
    register int f, r;
    struct itimerval val;
    int (*old)();

    f = fileno(ptr);
    fclose(ptr);
#ifdef VMUNIX
    omask = sigblock(mask(SIGINT)|mask(SIGQUIT)|mask(SIGHUP));
#endif /* VMUNIX */

    if (timedout != 0) {
	/* Enable signal handler */
	old = (int (*)()) signal(SIGALRM, lclalarm);

	/* Set timer */
	val.it_interval.tv_sec = 0;
	val.it_interval.tv_usec = 0;
	val.it_value.tv_sec = seconds;
	val.it_value.tv_usec = 0;
	setitimer(ITIMER_REAL, &val, 0);

	if (setjmp(env) == 0) {
	    while((r = wait(&status)) != popen_pid[f] && r != -1) ;

	    /* Turn off timer */
	    val.it_value.tv_sec = 0;
	    val.it_value.tv_usec = 0;
	    setitimer(ITIMER_REAL, &val, 0);
	    *timedout = 0;
	} else {
	    /* Timer expired */
	    status = -1;
	    *timedout = 1;
	}
	(void) signal(SIGALRM, old);	/* Restore old value */
    } else {
	while ((r=wait(&status)) != popen_pid[f] && r != -1) ;
    }

#ifdef VMUNIX
    sigsetmask(omask);
#endif /* VMUNIX */

    /* Reenable old SIGPIPE action */
    (void) signal(SIGPIPE, oldfunc);

    return status;
}
