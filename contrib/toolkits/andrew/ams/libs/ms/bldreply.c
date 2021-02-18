/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/bldreply.c,v 2.6 89/04/23 18:36:46 pgc Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/bldreply.c,v $
*/
#include <ms.h>
#include <hdrparse.h>

extern char *malloc();

BuildReplyField(Msg)
struct MS_Message *Msg;
{
    struct ParsedMsg *PStuff;

    debug(1, ("Building Reply field\n"));
    PStuff = Msg->ParsedStuff;
    if (PStuff->HeadBody[HP_REPLY_TO]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_REPLY_TO]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_REPLY_TO], PStuff->HeadBodyLen[HP_REPLY_TO]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_REPLY_TO]] = '\0';
    } else if (PStuff->HeadBody[HP_FROM]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_FROM]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_FROM], PStuff->HeadBodyLen[HP_FROM]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_FROM]] = '\0';
    } 
/* The following are commented out on cfe's advice: */
#ifdef UNDEFINEDGARBAGE
    else if (PStuff->HeadBody[HP_RESENTFROM]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_RESENTFROM]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_RESENTFROM], PStuff->HeadBodyLen[HP_RESENTFROM]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_RESENTFROM]] = '\0';
    } else if (PStuff->HeadBody[HP_SENDER]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_SENDER]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_SENDER], PStuff->HeadBodyLen[HP_SENDER]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_SENDER]] = '\0';
    } else if (PStuff->HeadBody[HP_RETURNPATH]) {
	if (Msg->ReplyTo)
	    free(Msg->ReplyTo);
	Msg->ReplyTo = malloc(1+PStuff->HeadBodyLen[HP_RETURNPATH]);
	if (Msg->ReplyTo == NULL) {
	    AMS_RETURN_ERRCODE( ENOMEM, EIN_MALLOC, EVIA_BUILDREPLY);
	}
	strncpy(Msg->ReplyTo, PStuff->HeadBody[HP_RETURNPATH], PStuff->HeadBodyLen[HP_RETURNPATH]);
	Msg->ReplyTo[PStuff->HeadBodyLen[HP_RETURNPATH]] = '\0';
    }
#endif /* UNDEFINEDGARBAGE */
    debug(16, ("Reply to field is <%s>\n", Msg->ReplyTo));
    return(0);
}
