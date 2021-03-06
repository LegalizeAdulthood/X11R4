/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/msparse.c,v 2.4 89/02/10 09:13:49 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/msparse.c,v $
*/
#include <ms.h>
#include <andrewos.h> /* sys/time.h */
#include <stdio.h>
#include <mail.h>

MS_ParseDate(indate, year, month, day, hour, min, sec, wday, gtm)
char *indate;
int *year, *month, *day, *hour, *min, *sec, *wday, *gtm;
{
    struct tm TmBuf;

    if (parsedateheader(indate, &TmBuf, 1, 1, 1, 0)) {
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARSEDATE, EVIA_PARSEDATE);
    }
    if (TmBuf.tm_hour == -1)
	TmBuf.tm_hour = 0;
    if (TmBuf.tm_min == -1)
	TmBuf.tm_min = 0;
    if (TmBuf.tm_sec == -1)
	TmBuf.tm_sec = 0;
    *year = TmBuf.tm_year;
    *month = TmBuf.tm_mon;
    *day = TmBuf.tm_mday;
    *hour = TmBuf.tm_hour;
    *min = TmBuf.tm_min;
    *sec = TmBuf.tm_sec;
    *wday = TmBuf.tm_wday;
    *gtm = gtime(&TmBuf);
    return(0);
}

/* A standin for parsedate() that gets rid of RFC822 comments */

parsedateheader(str, tmp, settm, select, err, gmt)
char *str;
struct tm *tmp;
int settm, select, err;
long *gmt;
{
    char *strstart, *strend, TokenBuf[500], FinalBuf[1500];
    int code822 = is822Atom, prevcode = is822Special;

    FinalBuf[0] = '\0';
    strstart = str;
    strend = str + strlen(str);
    while (code822 != is822End) {
	code822 = Next822Word(&strstart, strend, TokenBuf, sizeof(TokenBuf));
	if (((code822 == is822Atom) || (code822 == is822QuotedString)) && ((prevcode == is822Atom) || (prevcode == is822QuotedString))) {
	    strcat(FinalBuf, " ");
	}
	strcat(FinalBuf, TokenBuf);
	prevcode = code822;
    }
    return parsedate(FinalBuf, tmp, settm, select, err, gmt);
}
