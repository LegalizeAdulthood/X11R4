/* $Header: assert.h,v 1.3 89/07/02 15:58:03 moraes Exp $ */
#ifdef DEBUG
#define ASSERT(p, s) if(!(p))botch(s, __FILE__, __LINE__);else
extern int Dying();
static botch(s, filename, linenumber)
char *s;
char *filename;
/*
 *  Simple botch routine - writes directly to stderr
 */
{
	static char linebuf[16];	/* Enough to contain a BIG linenumber! */
	(void) sprintf(linebuf, "%d", linenumber);
	(void) write(2, "assertion in file ", 18);
	(void) write(2, filename, strlen(filename));
	(void) write(2, ", line ", 7);
	(void) write(2, linebuf, strlen(linebuf));
	(void) write(2, " botched: ", 10);
	(void) write(2, s, strlen(s)); write(2, "\n", 1);
	(void) Dying(0, 0, NULL);
	abort();
}
#else
#define ASSERT(p, s)
#endif

