#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)es_cp_file.c 20.14 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Routines to copy a file.  Stolen from cp.c, then modified.
 */
#include <stdio.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <sys/dir.h>
#include <xview/pkg.h>
#include <xview/attrol.h>

#define	BSIZE	8192

char           *rindex(), *sprintf();

es_copy_file(from, to)
    char           *from, *to;
{
    int             from_fd = open(from, 0), result;

    if (from_fd < 0)
	return 1;
    result = es_copy_fd(from, to, from_fd);
    (void) close(from_fd);
    return (result);
}

#define FSTAT_FAILED	-1
#define WILL_OVERWRITE	 1
Pkg_private int
es_copy_status(to, fold, from_mode)
    char           *to;
    int             fold;
    int            *from_mode;
{
    struct stat     stfrom, stto;

    if (fstat(fold, &stfrom) < 0)
	return (FSTAT_FAILED);
    if (stat(to, &stto) >= 0) {
	if (stfrom.st_dev == stto.st_dev &&
	    stfrom.st_ino == stto.st_ino) {
	    return (WILL_OVERWRITE);
	}
    }
    *from_mode = (int) stfrom.st_mode;
    return (0);
}

#define	es_Perror(s)
Pkg_private int
es_copy_fd(from, to, fold)
    char           *from, *to;
    int             fold;
{
    int             fnew, fnew_mode, n;
    char           *last, destname[BSIZE], buf[BSIZE];
    struct stat     stto;

    if (stat(to, &stto) >= 0 &&
	(stto.st_mode & S_IFMT) == S_IFDIR) {
	last = rindex(from, '/');
	if (last)
	    last++;
	else
	    last = from;
	if (strlen(to) + strlen(last) >= BSIZE - 1) {
	    return (1);
	}
	(void) sprintf(destname, "%s/%s", to, last);
	to = destname;
    }
    switch (es_copy_status(to, fold, &fnew_mode)) {
      case FSTAT_FAILED:
	es_Perror(from);
	return (1);
      case WILL_OVERWRITE:
	return (1);
      default:
	break;
    }
    fnew = creat(to, fnew_mode);
    if (fnew < 0) {
	es_Perror(to);
	return (1);
    }
    for (;;) {
	n = read(fold, buf, BSIZE);
	if (n == 0)
	    break;
	if (n < 0) {
	    es_Perror(from);
	    (void) close(fnew);
	    return (1);
	}
	if (write(fnew, buf, n) != n) {
	    es_Perror(to);
	    (void) close(fnew);
	    return (1);
	}
    }
    (void) close(fnew);
    return (0);
}
