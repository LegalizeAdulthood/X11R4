/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/blddate.c,v 2.7 89/05/15 12:15:01 cfe Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/blddate.c,v $
*/
#include <ms.h>
#include <hdrparse.h>
#include <andrewos.h> /* sys/time.h */

BuildDateField(Msg, datetype)
struct MS_Message *Msg;
int datetype;
{
    unsigned long when;
    char DateBuf[250];
    struct tm TmBuf;

    debug(1, ("Build date field\n"));
    switch(datetype) {
	case DATETYPE_FROMHEADER:
	    if (Msg->ParsedStuff->HeadBody[HP_DATE]) {
		strncpy(DateBuf, Msg->ParsedStuff->HeadBody[HP_DATE], Msg->ParsedStuff->HeadBodyLen[HP_DATE]);
		DateBuf[Msg->ParsedStuff->HeadBodyLen[HP_DATE]] = '\0';
		if (!parsedateheader(DateBuf, &TmBuf, 1, 1, 1, 0 /* &when */)) {
		    when = (unsigned long) gtime(&TmBuf);
		    if (when <= ((unsigned long) time(0) + 7*24*60*60) && when <= (unsigned long) 0xc0000000) break;
		}
	    } /* drop through */
	case DATETYPE_FROMFILE:
	    when = (unsigned long) Msg->RawFileDate;
	    if (when <= ((unsigned long) time(0) + 7*24*60*60) && when <= (unsigned long) 0xc0000000) break;
	case DATETYPE_CURRENT:
	default:
	    when = (unsigned long) time(0);
	    if (when > ((unsigned long) time(0) + 7*24*60*60) || when > (unsigned long) 0xc0000000) {
		AMS_RETURN_ERRCODE(EMSCLOCKBOGUS, EIN_PARAMCHECK, EVIA_BUILDDATE);
	    }
	    break;
    }
    strcpy(AMS_DATE(Msg->Snapshot), convlongto64(when, 0));
    return(0);
}
