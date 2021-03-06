/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/uattrs.c,v 2.4 89/02/15 11:42:43 ghoti Exp $ */
/* $ACIS: $ */
/* $Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/uattrs.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/ams/libs/ms/RCS/uattrs.c,v 2.4 89/02/15 11:42:43 ghoti Exp $ ";
#endif /* lint */

#include <ms.h>

extern char *permanentmalloc();
static char *UnusedAttrName = UNUSEDATTRNAME;

MS_GetDirAttributes(Dirname, AttrCt, Attrs, SepChar, ShowEmpty)
char *Dirname; /* IN */
int *AttrCt; /* OUT */
char *Attrs; /* OUT */
int SepChar, ShowEmpty; /* IN */
{
    struct MS_Directory *Dir;
    int i;
    char SepString[2];

    if (SepChar == 0) AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_GETDIRATTRS);
    if (ReadOrFindMSDir(Dirname, &Dir, MD_READ) != 0) {
	return(mserrcode);
    }
    *AttrCt = Dir->AttrCount;
    Attrs[0] = '\0';
    SepString[0] = (char) SepChar;
    SepString[1] = '\0';
    for(i=0; i < Dir->AttrCount; ++i) {
	if (ShowEmpty || strcmp(Dir->AttrNames[i], UnusedAttrName)) {
	    strcat(Attrs, Dir->AttrNames[i]);
	    strcat(Attrs, SepString);
	} else {
	    --*AttrCt;
	}
    }
    CloseMSDir(Dir, MD_READ);
    return(0);
}

MS_AddAttribute(Dirname, Newname, AttNum)
char *Dirname, *Newname; /* both IN */
int *AttNum; /* OUT */
{
    int i, errsave;
    Boolean Reusing = FALSE;
    char *s;
    struct MS_Directory *Dir;

    if (ReadOrFindMSDir(Dirname, &Dir, MD_WRITE) != 0) {
	return(mserrcode);
    }
    for (i=0; i<Dir->AttrCount; ++i) {
	if (!strcmp(Dir->AttrNames[i], Newname)) {
	    CloseMSDir(Dir, MD_WRITE);
	    AMS_RETURN_ERRCODE(EMSATTREXISTS, EIN_PARAMCHECK, EVIA_ADDATTR);
	}
    }
    for (i=0; i<Dir->AttrCount; ++i) {
	if (!strcmp(Dir->AttrNames[i], UnusedAttrName)) {
	    Reusing = TRUE;
	    break;
	}
    }
    if (!Reusing) {
	if (i >= AMS_NUM_UATTRS) {
	    CloseMSDir(Dir, MD_WRITE);
	    AMS_RETURN_ERRCODE(ERANGE, EIN_PARAMCHECK, EVIA_ADDATTR);
	}
	++Dir->AttrCount;
    }
    s = permanentmalloc(1+strlen(Newname));
    if (!s) {
	CloseMSDir(Dir, MD_WRITE);
	AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_ADDATTR);
    }
    strcpy(s, Newname);
    if (!Dir->AttrNames) {
	Dir->AttrNames = (char **) permanentmalloc(AMS_NUM_UATTRS * sizeof(char *));
	if (!Dir->AttrNames) {
	    CloseMSDir(Dir, MD_WRITE);
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_ADDATTR);
	}
    }
    Dir->AttrNames[i] = s;
    if (Dir->DBMajorVersion < 4) {
	Dir->DBMajorVersion = 4;
    }
    if (DestructivelyWriteDirectoryHead(Dir)) {
	errsave = mserrcode;
	CloseMSDir(Dir, MD_WRITE);
	return(errsave);
    }
    if (CacheDirectoryForClosing(Dir, MD_WRITE)) {
	return(mserrcode);
    }
    *AttNum = i;
    return(0);
}

MS_DeleteAttr(DirName, AttrName)
char *DirName, *AttrName; /* BOTH IN */
{
    int i, errsave;
    char snapshot[AMS_SNAPSHOTSIZE];
    struct MS_Directory *Dir;

    if (ReadOrFindMSDir(DirName, &Dir, MD_WRITE) != 0) {
	return(mserrcode);
    }
    for (i=0; i<Dir->AttrCount; ++i) {
	if (!strcmp(Dir->AttrNames[i], AttrName)) {
	    break;
	}
    }
    if (i>=Dir->AttrCount) {
	CloseMSDir(Dir, MD_WRITE);
	AMS_RETURN_ERRCODE(EINVAL, EIN_PARAMCHECK, EVIA_DELATTR);
    }
    if (strlen(AttrName) < strlen(UnusedAttrName)) {
	Dir->AttrNames[i] = permanentmalloc(1+strlen(UnusedAttrName));
	if (!Dir->AttrNames[i]) {
	    CloseMSDir(Dir, MD_WRITE);
	    AMS_RETURN_ERRCODE(ENOMEM, EIN_MALLOC, EVIA_DELATTR);
	}
    }
    strcpy(Dir->AttrNames[i], UnusedAttrName);
    for (i=0; i<Dir->MessageCount; ++i) {
	if (GetSnapshotByNumber(Dir, i, snapshot)) {
	    CloseMSDir(Dir, MD_WRITE);
	    return(mserrcode);
	}
	AMS_UNSET_ATTRIBUTE(snapshot, AMS_ATT_UATTR(i));
	if (RewriteSnapshotInDirectory(Dir, i, snapshot)) {
	    CloseMSDir(Dir, MD_WRITE);
	    return(mserrcode);
	}
    }
    if (DestructivelyWriteDirectoryHead(Dir)) {
	errsave = mserrcode;
	CloseMSDir(Dir, MD_WRITE);
	return(errsave);
    }
    return(CacheDirectoryForClosing(Dir, MD_WRITE));
}
