/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/texttag.c,v 1.1 89/07/13 16:29:48 tpn Exp $ */
/* $ACIS:texttag.c 1.3$ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/texttag.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/text/RCS/texttag.c,v 1.1 89/07/13 16:29:48 tpn Exp $";
#endif /* lint */

#include <class.h>
#include <texttag.eh>
char * texttag__ViewName(self)
struct texttag *self;
{
    return "texttagv";
}
static char *texttag__GetTag(self,size,buf)
struct texttag *self;
long size;
char *buf;
{
    long i;
    char *c;
    long realsize;
    realsize = texttag_GetLength(self);
    i = 0;
    while(realsize > 0 && texttag_GetChar(self,i) == ' ') {i++; realsize--;}
    if(size > realsize) size = realsize;
    texttag_CopySubString(self,i,size,buf, FALSE);
    c = buf;
    for(c = buf; *c != '\0'; c++) if(*c == ' ' || *c == '\t') *c = '-'; 
    c--;
    while(*c == '-') *c-- = '\0';
    return buf;
}
