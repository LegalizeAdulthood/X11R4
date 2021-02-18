/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/abbrpath.c,v 1.5 89/05/12 08:01:39 ghoti Exp $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/abbrpath.c,v $ */

#ifndef lint
static char *rcsid = "$Header: ";
#endif /* lint */

/*
  abbrpath.c

	ap_Shorten(pathname) tries to shorten pathname using the current home dir.
	ap_ShortenAlso(pathname, otherID, otherHD) does the same but uses otherID/otherHD also.
	ap_ShortenTo(pathname, maxlen) shortens to a maximum length, abbreviating the path prefix to hyphens.
	ap_ShortenAlsoTo(pathname, otherID, otherHD, maxlen) does it all.
      */

#include <stdio.h>
#include <sys/param.h>
#include <ctype.h>
#include <andrewos.h>
#include <util.h>

static int homeLen = -1;
static char myHome[150] = "";

static void initMyHome()
{
    char *cp;
    if (homeLen == -1) {
	cp = getMyHome();  /* only use home to shorten if it's 2 chars or more */
	if (cp != NULL && cp[0] != '\0' && cp[1] != '\0') {
	    strncpy(myHome, cp, sizeof(myHome));	
	    homeLen = strlen(myHome);
	} else {
	    myHome[0] = '\0';
	    homeLen = 0;
	}
    }
}

static char shortenRes[MAXPATHLEN+1] = "";

char *ap_Shorten(pathname)
char *pathname;
{/* Shorten it if we can. */
    initMyHome();
    if (homeLen > 0) {
	if (strncmp(pathname, myHome, homeLen) == 0) {
	    if (pathname[homeLen] == '\0') return "~";
	    else if (pathname[homeLen] == '/') {
		strcpy(shortenRes, "~/");
		strcpy(&shortenRes[2], &pathname[homeLen+1]);
		return shortenRes;
	    }
	}
    }
    return pathname;
}

char *ap_ShortenAlso(pathname, auxI, auxH)
char *pathname, *auxI, *auxH;
{/* Shorten it if we can. */
    int auxHLen, auxAbbr;

    auxHLen = auxAbbr = 0;
    if (auxH != NULL && auxI != NULL) {
	auxHLen = strlen(auxH);
	if (auxHLen == 1) auxHLen = 0;	/* short ones don't shorten the abbr. */
	else {
	    auxAbbr = auxHLen - strlen(auxI);	/* how much it will shorten */
	    if (auxAbbr <= 1) auxAbbr = auxHLen = 0;
	}
    }
    initMyHome();
    if (homeLen > auxAbbr) {	/* first try the one with the greater shortening */
	if (strncmp(pathname, myHome, homeLen) == 0) {
	    if (pathname[homeLen] == '\0') return "~";
	    else if (pathname[homeLen] == '/') {
		strcpy(shortenRes, "~/");
		strcpy(&shortenRes[2], &pathname[homeLen+1]);
		return shortenRes;
	    }
	}
    }
    if (auxAbbr > 0) {
	if (strncmp(pathname, auxH, auxHLen) == 0) {
	    strcpy(shortenRes, "~");
	    strcat(shortenRes, auxI);
	    if (pathname[auxHLen] == '\0') return shortenRes;
	    else if (pathname[auxHLen] == '/') {
		strcat(shortenRes, &pathname[auxHLen]);
		return shortenRes;
	    }
	}
    }
    if (homeLen > 0 && homeLen <= auxAbbr) {
	if (strncmp(pathname, myHome, homeLen) == 0) {
	    if (pathname[homeLen] == '\0') return "~";
	    else if (pathname[homeLen] == '/') {
		strcpy(shortenRes, "~/");
		strcpy(&shortenRes[2], &pathname[homeLen+1]);
		return shortenRes;
	    }
	}
    }
    return pathname;
}

static char pfx[] = "---";

char *ap_ShortenTo(pathname, maxLen)
char *pathname; int maxLen;
{/* Shorten it if we can. */
    char *res, *cp; int len;
    auto char toshortRes[MAXPATHLEN+1];

    res = ap_Shorten(pathname);
    len = strlen(res);
    if (len <= maxLen) return res;
    cp = index(&res[len - maxLen + sizeof(pfx)-2], '/');
    if (cp != NULL) {
	strcpy(toshortRes, pfx);
	strcat(toshortRes, ++cp);
	strcpy(shortenRes, toshortRes);	/* to static storage once it's complete */
	return shortenRes;
    }
    return NULL;	/* Have to give up--can't shorten to spec. */
}

char *ap_ShortenAlsoTo(pathname, auxI, auxH, maxLen)
char *pathname, *auxI, *auxH; int maxLen;
{/* Shorten it if we can. */
    char *res, *cp; int len;
    auto char toshortRes[MAXPATHLEN+1];

    res = ap_ShortenAlso(pathname, auxI, auxH);
    len = strlen(res);
    if (len <= maxLen) return res;
    cp = index(&res[len - maxLen + sizeof(pfx)-2], '/');
    if (cp != NULL) {
	strcpy(toshortRes, pfx);
	strcat(toshortRes, ++cp);
	strcpy(shortenRes, toshortRes);	/* to static storage once it's complete */
	return shortenRes;
    }
    return NULL;	/* Have to give up--can't shorten to spec. */
}

#ifdef TESTINGONLYTESTING
#include <stdio.h>
main()
{
    char a[300];
    char *cp; int maxLen;
    char auxID[300], auxHome[1000];

    cp = getMyHome();
    printf("Using %s for my home directory.\n", cp == NULL ? "NULL" : cp);
    auxID[0] = auxHome[0] = '\0';
    printf("Aux ID: "); fflush(stdout);
    if (gets(auxID) != NULL && auxID[0] != '\0') {
	printf("and its home dir: "); fflush(stdout);
	if (gets(auxHome) == NULL) auxHome[0] = '\0';
    }
    if (auxID[0] == '\0') auxHome[0] = '\0';
    if (auxHome[0] == '\0') auxID[0] = '\0';
    maxLen = -1;
    printf("Max length? "); fflush(stdout);
    if (gets(a) == NULL) exit(0);
    if (a[0] >= '0' && a[0] <= '9') maxLen = atoi(a);
    for (;;) {
	printf("A path to abbreviate, please: "); fflush(stdout);
	if (gets(a) == NULL) exit(0);
	printf("ap_Shorten(``%s'') results in ``%s''.\n", a, ap_Shorten(a));
	if (auxID[0] != '\0') {
	    printf("ap_ShortenAlso(``%s'', ...) results in ``%s''.\n", a, ap_ShortenAlso(a, auxID, auxHome));
	}
	if (maxLen >= 0) {
	    cp = ap_ShortenTo(a, maxLen);
	    printf("ap_ShortenTo(``%s'', %d) results in ``%s''.\n", a, maxLen,
		   cp == NULL ? "NULL" : cp);
	}
	if (auxID[0] != '\0' && maxLen >= 0) {
	    cp = ap_ShortenAlsoTo(a, auxID, auxHome, maxLen);
	    printf("ap_ShortenAlsoTo(``%s'', ...) results in ``%s''.\n", a,
		   cp == NULL ? "NULL" : cp);
	}
    }
}
#endif /* TESTINGONLYTESTING */
