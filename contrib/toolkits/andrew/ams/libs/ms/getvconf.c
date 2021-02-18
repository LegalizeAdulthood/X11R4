/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/getvconf.c,v 1.6 89/02/10 09:13:10 ghoti Exp $
*/

#include <andyenv.h>
#include <andrewos.h>
#include <ms.h>
#include <stdio.h>

#define StrEql(a,b)(strcmp(a,b)==0)
/*
  dummy routine to get the rest of ams working again
*/
long MS_GetVConfig(key,vers,result)
char *key;
char *vers;
char *result;
{if(key==0)key="(null)";
 if(vers==0)vers="(null)";
 if (!access("/debug.getvconfig", F_OK)) /*print debugging info?*/
  fprintf(stderr,"MS_GetVConfig:key=%s,vers=%s",key,vers); 
 fflush(stderr);
 if(key==0)
  strcpy(result,"");
 else if(StrEql(key,"motd"))
  strcpy(result,"/etc/motd");
 else if(StrEql(key,"expire"))
  strcpy(result,"0,");
 else
  strcpy(result,"");
 return 0;
}
