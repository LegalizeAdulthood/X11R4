
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/utils.c,v 1.9 89/11/17 12:52:40 ricks Rel $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */


/*
 * utils.c: random utility functions for xrn
 */

#include "copyright.h"
#include <X11/Xos.h>
#include <stdio.h>
#include <ctype.h>
#include "config.h"
#include "utils.h"
#ifndef VMS
#include <sys/param.h>
#include <pwd.h>
#else
#define MAXPATHLEN 512
#define index strchr
#endif /* VMS */
#ifdef aiws
struct passwd *getpwuid();
struct passwd *getpwnam();
#endif /* aiws */

#define USER_NAME_SIZE 32

#ifdef SYSV_REGEX
/* 
 * kludge alert:  this is here because (on A/UX 1.1) linking with
 * the PW lib (necessary to get the regular expression routines, 
 * regcmp/regex), causes symbol 'Error' to be undefined.
 * 	glenn@mathcs.emory.edu 10/17/89 
 *
 * SYSV_REGEX may be overkill, need for macII and HPUX
 *    (Andy.Linton@comp.vuw.ac.nz 11/8/89)
 */
int Error; 
#endif

char *
utTrimSpaces(str)
char *str;
/*
 * trim leading and trailing spaces from a string
 *
 *   NOTE: this function modifiess the argument
 *
 *   returns: the modified string
 *
 */
{
    char *end = &str[utStrlen(str) - 1];

    while (*str == ' ') {
	str++;
    }
    while (*end == ' ') {
	*end = '\0';
	end--;
    }
    return(str);
}


char *
utTildeExpand(filename)
char *filename;    /* file name, possibly with a '~'             */
/*
 * tilde expand a file name
 *
 *   returns: the expanded name of the file (in a static buffer)
 *            or NIL(char) 
 */
{
#ifdef aiws
    static char dummy[MAXPATH];
#else
    static char dummy[MAXPATHLEN];
#endif /* aiws */
    char username[USER_NAME_SIZE], *loc;
    struct passwd *pw;
    
    if ((filename == NIL(char)) || STREQ(filename, "")) {
	return(NIL(char));
    }

    if (filename[0] != '~') {
	(void) strcpy(dummy, filename);
	return(dummy);
    }

    /* tilde at the beginning now */
    if (filename[1] == '/') {
	/* current user */
	char *home, *getenv();
	
	if ((home = getenv("HOME")) == NIL(char)) {
#ifndef VMS
	    /* fall back on /etc/passwd */
	    if ((pw = getpwuid(getuid())) == NIL(struct passwd)) {
		return(NIL(char));
	    }
	    (void) sprintf(dummy, "%s%s", pw->pw_dir, &filename[1]);
#else
	    return (NIL(char));
#endif
	} else {
	    (void) sprintf(dummy, "%s%s", home, &filename[1]);
	}
	    
    } else {
	if ((loc = index(filename, '/')) == NIL(char)) {
	    /* error - bad filename */
	    return(NIL(char));
	}
	(void) strncpy(username, &filename[1], loc - &filename[1]);
	username[loc - &filename[1]] = '\0';
#ifndef VMS
	if ((pw = getpwnam(username)) == NIL(struct passwd)) {
	    return(NIL(char));
	}
	(void) sprintf(dummy, "%s%s", pw->pw_dir, loc);
#else
	return(getenv("USER"));
#endif
    }
    return(dummy);
}


int
utSubstring(string, sub)
char *string;
char *sub;
{
    int i;

    
    for (i = 0; i < utStrlen(string) - utStrlen(sub) + 1; i++) {
	if (STREQN(&string[i], sub, utStrlen(sub))) {
	    return(1);
	}
    }
    return(0);
}


void
utDowncase(string)
char *string;
/*
 * down case the characters in a string (in place)
 */
{
    for ( ; *string != '\0'; string++) {
	if (isupper(*string)) {
	    *string = tolower(*string);
	}
    }
    return;
}


#ifdef VMS    
int
utGroupToVmsFilename(filename,group)
    char *filename;
    char *group;
{
    char *ptr;
    int i=0;

    for (ptr = group; *ptr != 0; ptr++)
    {
	if (isalnum(*ptr))
	{
	    filename[i++] = *ptr;
	}
	else
	{
	    switch (*ptr)
	    {
		case '.' :
		    filename[i++] = '_';
		    break;
		case '-' :
		    filename[i++] = '-';
		    break;
		default :
		    i = i + sprintf(&filename[i], "$X%X$", *ptr);
		    break;
	    }
	}
    }
    filename[i] = 0;
    return (i);
}
#endif


#ifdef VMS    
extern int delete();
int unlink(foo)
char *foo;
{
    return(delete(foo));
}
#endif VMS


/* case insensitive, 24 character max, comparision for subjects */

int
utSubjectCompare(str1, str2)
char *str1, *str2;
{
    int count = 0;
    char c1, c2;

    while (count++ < 24) {
	if (!*str1 && !*str2) {
	    return 0;
	}
	if (!*str1) {
	    return -1;
	}
	if (!*str2) {
	    return 1;
	}
	if (isupper(*str1)) {
	    c1 = tolower(*str1);
	} else {
	    c1 = *str1;
	}
	if (isupper(*str2)) {
	    c2 = tolower(*str2);
	} else {
	    c2 = *str2;
	}
	if (c1 != c2) {
	    return (c1 - c2);
	}
	str1++;
	str2++;
    }
    return 0;
}
