/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/freemsg.c,v 2.5 89/02/17 18:57:37 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/freemsg.c,v $
*/
#include <ms.h>
#include <hdrparse.h>

/* 2 routines for backward compatibility */

FreeMessage(Msg, FreeSnapshot)
struct MS_Message *Msg;
Boolean FreeSnapshot;
{
    debug(1, ("FreeMessage\n"));
    if (Msg) {
	if (Msg->OpenFD >= 0) {
	    if (Msg->OpenFD == 0) {
		CriticalBizarreError("Warning!  FreeMessage is closing file descriptor zero, which is *probably* a mistake!");
	    }
	    close(Msg->OpenFD);
	    Msg->OpenFD = -1; /* Just for sheer paranoia */
	}
	FreeMessageContents(Msg, FreeSnapshot);
	if (Msg->AuthCell) free(Msg->AuthCell);
	if (Msg->AuthName) free(Msg->AuthName);
	free(Msg);
    }
    return(0);
}

FreeMessageContents(Msg, FreeSnapshot)
struct MS_Message *Msg;
Boolean FreeSnapshot;
{
    if (!Msg) return(0);
    if (FreeSnapshot && Msg->Snapshot) {free (Msg->Snapshot); Msg->Snapshot = NULL;}
    if (Msg->RawBits) {free (Msg->RawBits); Msg->RawBits = NULL;}
    if (Msg->ReplyTo) {free (Msg->ReplyTo); Msg->ReplyTo = NULL;}
    if (Msg->WideReply) {free (Msg->WideReply); Msg->WideReply = NULL;}
    FreeParsedStuff(Msg);
    return(0);
}

FreeParsedStuff(Msg)
struct MS_Message *Msg;
{
    int i;
    if (Msg->ParsedStuff) {
	for (i = HP_END_PREDEFINED; Msg->ParsedStuff->HeadName[i]; ++i) {
	    free(Msg->ParsedStuff->HeadName[i]);
	}
	free(Msg->ParsedStuff->HeadName);
	free(Msg->ParsedStuff->HeadBody);
	free(Msg->ParsedStuff->HeadBodyLen);
	free(Msg->ParsedStuff->IsDuplicated);
	free (Msg->ParsedStuff);
	Msg->ParsedStuff = NULL;
    }
    return(0);
}

