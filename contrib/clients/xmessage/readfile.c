/*
 * xmessage - utility for querying users
 *
 * $XConsortium: readfile.c,v 1.3 88/11/10 12:20:24 jim Exp $
 *
 * Copyright 1988 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * Authors:  Jim Fulton, MIT X Consortium
 *           Chris D. Peterson, MIT Project Athena
 */

#include <stdio.h>
#include <X11/Xos.h>			/* get types.h */
#include <sys/stat.h>

extern char *malloc();

/*
 * get_data_from_file - read data from a file into a single buffer; meant 
 * for small files containing messages.
 */
static char *get_data_from_file (filename)
    char *filename;
{
    FILE *fp;
    struct stat statbuf;
    char *cp;


    if (stat (filename, &statbuf) != 0 || statbuf.st_size < 0) {
	return NULL;
    }

    cp = malloc (statbuf.st_size + 1);
    if (!cp) return NULL;

    fp = fopen (filename, "r");
    if (!fp) {
	(void) free (cp);
	return NULL;
    }

    if (fread (cp, 1, statbuf.st_size, fp) != statbuf.st_size) {
	(void) free (cp);
	(void) fclose (fp);
	return NULL;
    }

    cp[statbuf.st_size] = '\0';		/* since we allocated one extra */
    (void) fclose (fp);
    return cp;
}

/*
 * get_data_from_stdin - copy data from stdin to file, use get_data_from_file,
 * and then remove file.  Reads data twices, but avoid mallocing extra memory.
 * Meant for small files.
 */
static char *get_data_from_stdin ()
{
    char filename[80];
    char buf[BUFSIZ];
    FILE *fp;
    int n;
    char *cp;

    strcpy (filename, "/tmp/xq-XXXXXX");
    if (!mktemp (filename)) return NULL;

    fp = fopen (filename, "w");
    if (!fp) return NULL;
    while ((n = fread (buf, 1, BUFSIZ, stdin)) > 0) {
	(void) fwrite (buf, 1, n, fp);
    }
    (void) fclose (fp);

    cp = get_data_from_file (filename);
    (void) unlink (filename);
    return cp;
}


/*
 * read_file - read data from indicated file and return pointer to malloced
 * buffer.  Returns NULL on error or if no such file.
 */
char *read_file (filename)
    char *filename;
{
    if (filename[0] == '-' && filename[1] == '\0') {
	return (get_data_from_stdin ());
    } else {
	return (get_data_from_file (filename));
    }
}

