/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/localdir.c,v 2.4 89/09/12 13:34:10 cfe Exp $*/
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/localdir.c,v $*/

#ifndef lint
static char *rcsid = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/localdir.c,v 2.4 89/09/12 13:34:10 cfe Exp $";
#endif /* lint */

#include <stdio.h>
#include <andrdir.h>

extern char *malloc();
extern char *getenv();



/* Return a string with the current value for LOCALDIR imbedded in it. */
char *LocalDir(str)
char *str;
{
    char *p = NULL;
    int addLen;
    static int locyLen = 0;
    static int bufSize = -1;
    static char *buffer;

    if (str != NULL)  {
	addLen = strlen(str);
    }
    else  {
	addLen = 0;
    }

    if (bufSize == -1) {
	if (((p = getenv("LOCALDIR")) == NULL || *p == '\0' ) && ((p = (char *) GetConfiguration("LocalDir")) == NULL || *p == '\0'))  {
/*	    p = "/usr/local"; */
	    p = QUOTED_DEFAULT_LOCALDIR_ENV;
	}
	locyLen = strlen(p);

	bufSize = addLen + locyLen + 1;
	if ((buffer = (char *) malloc(bufSize)) == NULL)  {
	    bufSize = -1;
	    return NULL;
	}

	strcpy(buffer, p);
    }

    if (bufSize < locyLen + addLen + 1)  {
	bufSize = addLen + locyLen + 1;
	if ((buffer = (char *) realloc(buffer, bufSize)) == NULL)  {
	    bufSize = -1;
	    return NULL;
	}
    }

    if (str != NULL)  {
	strcpy(&(buffer[locyLen]), str);
    }
    else  {
	buffer[locyLen] = '\0';
    }

    return buffer;
}
