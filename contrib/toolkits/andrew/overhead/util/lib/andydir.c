/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* andydir.c */

#include <stdio.h>

extern char *malloc();
extern char *getenv();

/* Return a string with the current value for ANDYDIR imbedded in it. */
char *AndyDir(str)
char *str;
{
    char *p = NULL;
    int addLen;
    static int andyLen = 0;
    static int bufSize = -1;
    static char *buffer;

    if (str != NULL)  {
	addLen = strlen(str);
    }
    else  {
	addLen = 0;
    }

    if (bufSize == -1) {
	if (((p = getenv("ANDYDIR")) == NULL || *p == '\0' ) && ((p = (char *) GetConfiguration("AndyDir")) == NULL || *p == '\0'))  {
	    p = "/usr/andy";
	}
	andyLen = strlen(p);

	bufSize = addLen + andyLen + 1;
	if ((buffer = (char *) malloc(bufSize)) == NULL)  {
	    bufSize = -1;
	    return NULL;
	}
	strcpy(buffer, p);
    }

    if (bufSize < andyLen + addLen + 1)  {
	bufSize = addLen + andyLen + 1;
	if ((buffer = (char *) realloc(buffer, bufSize)) == NULL)  {
	    bufSize = -1;
	    return NULL;
	}
    }

    if (str != NULL)  {
	strcpy(&(buffer[andyLen]), str);
    }
    else  {
	buffer[andyLen] = '\0';
    }

    return buffer;
}
