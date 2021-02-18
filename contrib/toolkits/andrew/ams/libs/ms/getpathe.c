/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getpathe.c,v 2.4 89/02/08 15:33:43 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getpathe.c,v $
*/
#include <ms.h>
#include <mailconf.h>

extern char home[];
extern Boolean DidInit;

MS_GetSearchPathEntry(which, buf, lim)
int     which,
        lim;
char   *buf;
{
    int     i;

    if (!DidInit) {		/* First time-only -- initialization
				   section */
	if ((i = InitializeSearchPaths()) != 0)
	    return(i);
    }
    switch(which) {
	case AMS_MAILPATH:
	    /* Wants *mail* directory */
	    strncpy(buf, home, lim);
	    strncat(buf, MAILSEARCHPATHTEMPLATE, lim);
	    break;
	case AMS_OFFICIALPATH:
	    strncpy(buf, OFFICIALSEARCHPATHTEMPLATE, lim);
	    break;
	case AMS_LOCALPATH:
	    strncpy(buf, LOCALSEARCHPATHTEMPLATE, lim);
	    break;
	case AMS_EXTERNALPATH:
	    strncpy(buf, EXTERNALSEARCHPATHTEMPLATE, lim);
	    break;
	default:
	    if (which < 0 || which >= MS_NumDirsInSearchPath) {
		AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_GETSEARCHPATHENTRY);
	    }
	    if (strlen(SearchPathElements[which].Path) > lim) {
		AMS_RETURN_ERRCODE(ERANGE, EIN_PARAMCHECK, EVIA_GETSEARCHPATHENTRY);
	    }
	    strncpy(buf, SearchPathElements[which].Path, lim);
	    break;
    }
    return(0);
}
