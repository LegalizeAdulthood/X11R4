/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/hdlpref.c,v 2.3 89/02/08 15:31:21 ghoti Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/hdlpref.c,v $
*/
#include <ms.h>

extern char *malloc(), *getprofile();

MS_HandlePreference(prog, pref, InVal, OutVal, OutLim, opcode, resulti, defaulti)
char *prog, *pref, *InVal; /* Passed IN */
char *OutVal; /* Passed OUT */
int OutLim, opcode, defaulti; /* Passed IN */
int *resulti; /* Passed OUT */
{
    char *s, *key;

    key = malloc(2+strlen(prog) + strlen(pref));
    if (!key) {
	AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_HANDLEPREFERENCE);
    }
    sprintf(key, "%s.%s", prog, pref);
    switch(opcode) {
	case AMS_GETPROFILESTRING:
	    s = getprofile(key);
	    if (s) {
		strncpy(OutVal, s, OutLim);
	    } else {
		OutVal[0] = '\0';
	    }
	    break;
	case AMS_GETPROFILEINT:
	    *resulti = getprofileint(key, defaulti);
	    break;
	case AMS_GETPROFILESWITCH:
	    *resulti = getprofileswitch(key, defaulti);
	    break;
	case AMS_SETPROFILESTRING:
	    if (setprofilestring(prog, pref, InVal)) {
		free(key);
		AMS_RETURN_ERRCODE(errno, EIN_SETPROF, EVIA_HANDLEPREFERENCE);
	    }
	    break;
	default:
	    free(key);
	    AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_HANDLEPREFERENCE);
    }
    free(key);
    return(0);
}
