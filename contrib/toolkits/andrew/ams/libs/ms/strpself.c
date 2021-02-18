/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
  $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/strpself.c,v 2.9 89/05/16 10:09:24 cfe Exp $
  $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/strpself.c,v $
  */
#include <ms.h>
#include <parseadd.h>
#include <mail.h>
#include <andrewos.h>	/* <strings.h> */

extern char *malloc(), Me[], MeInFull[], MyMailDomain[], *MyShortAddress;

static void StripList(AddrList)
PARSED_ADDRESS *AddrList;
{/* Recursively strip Me@MyMailDomain from the list. */
    int outType, la_errcode;
    char *IDpart, *PostID, *s;

    FOR_ALL_ADDRESSES(tmpaddr, AddrList, {
		       switch (tmpaddr->Kind) {
			   case SIMPLE_ADDRESS:
			       IDpart = PostID = NULL;
			       la_errcode = la_KindDomain(tmpaddr, &outType, &IDpart, &PostID, MyMailDomain);
			       if (la_errcode != laerr_NoError || IDpart == NULL) break;
			       if (outType == latype_LocalID || outType == latype_LocalName) {
				   if (ULstrcmp(IDpart, Me) == 0) {
				       /* We could do a zillion heuristics in the LocalName case. instead, we just do the most obvious, specifically to catch canonical addresses on non-AMS delivery configurations */
				       /* You might think these next two lines are necessary, but it turns out that FreeAddress does it for us automatically, according to CFE.  I've left them in so that nobody else will make the same mistake I did.  -- NSB */
				       /*		tmpaddr->Prev->Next = tmpaddr->Next; */
				       /*		tmpaddr->Next->Prev = tmpaddr->Prev; */
				       FreeAddress(tmpaddr);
				   } else if (outType == latype_LocalName) {
				       s = rindex(MyShortAddress, '@');
				       if (s != NULL && *s == '@') {
					   if (strlen(IDpart) == (s-MyShortAddress) && ULstlmatch(MyShortAddress, IDpart) == 1) {
					       FreeAddress(tmpaddr);
					   }
				       }
				   }
			       }
			       free(IDpart);
			       break;
			   case GROUP_ADDRESS:
			       StripList(tmpaddr->Members);
			       break;
			   default:
			       break;
		       }
		       })
}

StripMyselfFromAddressList(Old, New)
char *Old, **New;
{
    PARSED_ADDRESS *AddrList;
    int Newsize, Dummy;

    if (!Old || ParseAddressList(Old, &AddrList) != PA_OK) {
	return(-1);
    }
    StripList(AddrList);
    Newsize = 2*strlen(Old);
    *New = malloc(Newsize);
    if (!*New) {
	FreeAddressList(AddrList);
	return(-1);
    }
    if (UnparseAddressList(AddrList, UP_SPACES_TO_DOTS, *New, Newsize, "", "	", 69, &Dummy) != PA_OK) {
	free(*New);
	FreeAddressList(AddrList);
	return -1;
    }
    FreeAddressList(AddrList);
    return(0);
}
