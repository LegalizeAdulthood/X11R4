/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/rebldmap.c,v 2.7 89/04/20 15:28:08 cfe Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/rebldmap.c,v $
*/
#include <andrewos.h>
#include <stdio.h>
#include <ms.h>

extern FILE *fopen();
extern char *rindex();

MS_RebuildSubscriptionMaps() {
    int i = 0, code;
    char PathElt[1+MAXPATHLEN];

    while (MS_GetSearchPathEntry(i++, PathElt, MAXPATHLEN) == 0) {
	code = MS_RebuildOneSubscriptionMap(PathElt);
	if (code && ((errno != EACCES) || (i > 1))) return(code);
    }
    return(0);
}

MS_RebuildOneSubscriptionMap(PathElt)
char *PathElt;
{
    int code;
    char MapFile[MAXPATHLEN+1], RealMapFile[MAXPATHLEN+1], RealPath1[MAXPATHLEN+1], RealPath2[MAXPATHLEN+1], *sdum;
    FILE *mfp;

    debug(1, ("MS_RebuildOneSubscriptionMap: %s\n", PathElt));
    sdum = rindex(PathElt, '/');
    if (!sdum) {
	AMS_RETURN_ERRCODE(EMSNOTTREEROOT, EIN_PARAMCHECK, EVIA_REBUILDSUBSCRIPTIONMAP)
    }
    if (strncmp(++sdum, MS_TREEROOT, sizeof(MS_TREEROOT)-1)) {
	AMS_RETURN_ERRCODE(EMSNOTTREEROOT, EIN_PARAMCHECK, EVIA_REBUILDSUBSCRIPTIONMAP);
    }
    (void) DeSymLink(PathElt, RealPath1, 0);
    if (abspath(RealPath1, RealPath2) != 0) strcpy(RealPath2, RealPath1);
    sprintf(RealMapFile, "%s/%s", RealPath2, AMS_SUBSCRIPTIONMAPFILE);
    strcpy(MapFile, RealMapFile);
    strcat(MapFile, ".NEW");
    debug(4, ("Rebuilding file %s\n", MapFile));
    if ((mfp = fopen(MapFile, "w")) == NULL) {
	AMS_RETURN_ERRCODE(errno, EIN_FOPEN, EVIA_REBUILDSUBSCRIPTIONMAP);
    }
    if ((code = SubsTreeWalk(mfp, RealPath2)) != 0) {
	fclose(mfp);
	return(code);
    }
    if (ferror(mfp) || feof(mfp)) {
	fclose(mfp);
	AMS_RETURN_ERRCODE(errno, EIN_FERROR, EVIA_REBUILDSUBSCRIPTIONMAP);
    }
    if (vfclose(mfp) != 0) {
	AMS_RETURN_ERRCODE(errno, EIN_VCLOSE, EVIA_REBUILDSUBSCRIPTIONMAP);
    }
    if (RenameEvenInVice(MapFile, RealMapFile)) {
	AMS_RETURN_ERRCODE(errno, EIN_RENAME, EVIA_REBUILDSUBSCRIPTIONMAP);
    }
    return(0);
}    
