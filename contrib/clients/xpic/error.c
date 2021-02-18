/* $Header: error.c,v 1.3 89/07/02 15:58:08 moraes Exp $ */
extern char *progname;

/*
 *  Error is fatal. One day, a fancy varargs version might be nice.
 */
error (s)
char *s;
{
	if (progname)
		(void) write(2, progname, strlen(progname));
	(void) write(2, ": fatal error", 8);
	if (s)
		(void) write(2, s, strlen(s));
	else
		(void) write(2, "NULL error message.", 19);
	(void) write(2, "\n", 1);
	exit(-1);
	/*NOTREACHED*/
}

