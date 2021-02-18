/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
		Reauthenticate to Vice repeatedly.

	Run in background with nohup.

*/


#include <stdio.h>
#include <sys/wait.h>
#include <mailconf.h>
#include <errprntf.h>
#include <andrewos.h> /* sys/time.h */
#include <signal.h>
#include <errno.h>

#define NIL	0

static char myname[] = "reauth";

extern int errno;

/*
	reauth time who password
*/

/* basically a no-op - used mostly just to register the fact that the signal was indeed caught */
dolog()
{
    errprintf(myname, ERR_CRITICAL, NIL, NIL, "Signal caught...");
}

static int logme(name, password)
char *name, *password;
{
    register int pid;
    int outStatus, valWait;

    errprintf(myname, ERR_CRITICAL, NIL, NIL, "Attempting Re-Authentication (%s)", name);
    /* Fork */
    pid = fork();
    if (pid < 0) {
	errprintf(myname, ERR_CRITICAL, NIL, NIL, "Can't fork: %d", errno);
	return -2;
    }
    if (pid > 0) {
	do valWait = wait(&outStatus); while (valWait != pid && valWait != -1);
	if (valWait != pid) {
	    errprintf(myname, ERR_CRITICAL, NIL, NIL, "Wait returned %d, not %d",
		      valWait, pid);
	    return -1;
	}
	return outStatus;
    }

    /* Here's the child */
    execl(logpgm, "log", name, password, NIL);
    errprintf(myname, ERR_CRITICAL, NIL, NIL, "Execl failed: %d", errno);
    _exit(1);
}

main(argc, argv)
int argc;
char *argv[];
{
    register int len, nap, pid, LogStatus;
    struct timeval tv;
    char password[512];	/* Should be big enough */

    if (argc != 4) {
	errprintf(myname, ERR_CRITICAL, NIL, NIL, "Wrong number of args: %d", argc);
	exit(1);
    }

    /* Copy password & clobber */
    len = strlen(argv[3]);
    if (len >= sizeof(password)) {
	errprintf(myname, ERR_CRITICAL, NIL, NIL, "Password too long: %d", len);
	exit(1);
    }
    strcpy(password, argv[3]);
    bzero(argv[3], len);

    CheckAMSConfiguration();

    /* Fork & become daemon */
    pid = fork();
    if (pid < 0) {
	errprintf(myname, ERR_CRITICAL, NIL, NIL, "Fork failed: %d", errno);
	exit(1);
    }
    if (pid > 0) exit(0);
    setpgrp(0, getpid());

    /* Get sleep time */
    nap = atoi(argv[1]);
    signal(SIGHUP, dolog);
#ifdef SIGXFSZ
    signal(SIGXFSZ, dolog);
#else /* SIGXFSZ */
#ifdef SIGUSR2
    signal(SIGUSR2, dolog);
#endif /* SIGUSR2 */
#endif /* SIGXFSZ */


    for (;;) {
	LogStatus = logme(argv[2], password);
	if (LogStatus == 0){		/* forked, execed, no signals, exit stat == 0 */
	    tv.tv_sec = nap;
	    tv.tv_usec = 0;
	    select(0, NULL, NULL, NULL, &tv);
	}
	else{	/* only five minutes if we weren't able to authenticate */
	    tv.tv_sec = 5 * 60;
	    tv.tv_usec = 0;
	    select(0, NULL, NULL, NULL, &tv);
	}
    }
}
