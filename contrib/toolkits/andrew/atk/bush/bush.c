/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidbush_c = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/bush/RCS/bush.c,v 1.16 89/11/07 10:33:06 cfe Exp $";
#endif

/*
    $Log:	bush.c,v $
 * Revision 1.16  89/11/07  10:33:06  cfe
 * Eliminate dependency on afs/param.h .
 * 
 * Revision 1.15  89/10/23  16:16:30  cfe
 * Typo in patch: use the result of the UidUnameMap cache if it's found, even in non-AFS circumstances.
 * 
 * Revision 1.14  89/10/23  15:52:44  gk5g
 * Added patch for bug 105 regarding incomplete handling of vanilla unix uid-lookups in getname().
 * Added bush_PerformSystemAction and call to it in bush_DestroyEntry().
 * 
 * Revision 1.13  89/10/16  11:03:12  cfe
 * compiler warnings
 * 
 * Revision 1.12  89/09/01  13:31:02  gk5g
 * Made change from tree_NodeData to tree_NodeDatum.
 * 
 * Revision 1.11  89/08/03  12:31:56  ghoti
 * added include of andrewos.h (HPUX)
 * 
 * Revision 1.10  89/08/02  12:14:32  gk5g
 * Added code to do uid->uname mappings if not an AFS site.  This code just does a normal getpwuid().
 * 
 * Revision 1.9  89/08/01  17:38:19  gk5g
 * Added #include <andyenv.h>
 * 
 * Revision 1.8  89/07/31  12:08:00  gk5g
 * In the last entry log I entered a line that included a comment.  This was stupid because it caused the $Log: to be prematurely truncated and all compiles will fail.  I simply seperated the /-char and the *-char.
 * 
 * Revision 1.7  89/07/31  11:06:17  gk5g
 * Changed all #endif foo tp #endif /^*foo*^/.
 * 
 * Revision 1.6  89/07/28  19:25:32  gk5g
 * Added #ifdef WHITEPAGES_ENV around #include <wp.h>.
 * 
 * Revision 1.5  89/07/28  15:43:16  gk5g
 * Added AFS_ENV defines to exclude the AFS-specific uid->uname mapping code for those non-AFS sites.
 * 
 * Revision 1.4  89/07/19  18:39:27  gk5g
 * Added getcell() and gethomecell() to support white page uid lookups across cells.
 * 
 * Revision 1.3  89/06/15  16:12:20  gk5g
 * Added fix to bush_BuildSubDirs() to initialize DirPath(tn) correctly if the parent directory is /.
 * 
 * Revision 1.2  89/06/06  17:29:28  gk5g
 * Added initialization code to bush_Read() to support loading as an inset; Made a simple change to bush_ViewName().
 * 
 * Revision 1.1  89/04/28  21:24:10  tom
 * Initial revision
 * 
 * Revision 1.1  89/04/28  21:20:45  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Bush Data-object

MODULE	bush.c

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This is the suite of Methods that support the Bush Data-object.

    NB: The comment-symbol "===" indicates areas which are:
	    1 - Questionable
	    OR
	    2 - Arbitrary
	    OR
	    3 - Temporary Hacks
    Such curiosities need be resolved prior to Project Completion...


HISTORY
  08/21/85	Created (TCP)
  01/15/89	Convert to ATK (GW Keim)
  06/06/89	Added initialization code to bush_Read() 
                to support loading as an inset; (GW Keim)
                Made a simple change to bush_ViewName(); (GW Keim)
  06/15/89	Added fix to bush_BuildSubDirs() to initialize DirPath(tn)
		correctly if the parent directory is /.
  07/16/89	Added getname() to determine the uid of the owner of a file. Will work 
		across cells if the proper args are provided to getname(self,uid,cell); (GW Keim)
  07/19/89	Added getcell() and gethomecell() to support white page uid lookups 
		across cells; (GW Keim)
  07/28/89	Added AFS_ENV defines to exclude the AFS-specific uid->uname mapping
		code for those non-AFS sites; (GW Keim)
  07/28/89	Added #ifdef WHITEPAGES_ENV around #include <wp.h>; (GW Keim)
  08/01/89	Added #include <andyenv.h>; (GW Keim)
   08/02/89	Added code to do uid->uname mappings if not an AFS site.  This code
                            just does a normal getpwuid(); (GW Keim)

END-SPECIFICATION  ************************************************************/


struct map_item { 
    int		 uid;
    char	*uname;
    char	*ucell;
};

#include <stdio.h>
#include <andyenv.h>
#include <class.h>
#include <andrewos.h>
#include <pwd.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <ctype.h>
#include <sys/dir.h>
#include <errno.h>
#ifdef AFS_ENV
#include <util.h>		    /*getv*(), getc*() routine family*/
#ifdef WHITEPAGES_ENV
#include <wp.h>			    /*White Pages*/
#endif /* WHITEPAGES_ENV */
#include <afs/vice.h>
#include <afs/venus.h>
#endif /* AFS_ENV */
#include <apt.h>
#include <dataobj.ih>
#include <filetype.ih>
#include <im.ih>
#include <vector.ih>
#include <bush.h>
#include <tree.ih>
#include <bush.eh>

#define GivenDirName	(self->given_dir_name)
#define RootPath	(self->root_pathname)
#define	UidUnameMap	(self->uid_uname_map)
#define	MyCellName	(self->mycellname)
#define Debug		(self->debug)

tree_Specification DirTree[] = {tree_Order(tree_PreOrder),NULL};

char				baseName[] = "/afs"; /*Pathname to give to pioctl()*/
#define	MAX_PIOCTL_BUFF_SIZE	1000
static char			*gethomecell(), *getcell();

static void
AllocNameSpace( target, source )
    char	    **target, *source;
{
    if(!target) return;
    if(*target) free(*target);
    if(!source) {
	*target = NULL;
	return;
    }
    if(*target = malloc(strlen(source)+1)) 
	strcpy(*target,source);
}

static int
NodeFilter( dir )
    struct direct    *dir;
{
    return(!(*dir->d_name == '.' && (*(dir->d_name+1) == '.' || 
	*(dir->d_name+1) == '\0')));
}

struct bush *
bush__Create( ClassID, init_dir )
     struct classheader	    *ClassID;
     char		    *init_dir;
{
    register struct bush    *self = NULL;

    if(self = bush_New()) {
	if(init_dir && (init_dir[0] != '\0')) 
	    strcpy(GivenDirName,init_dir);
	else im_GetDirectory(GivenDirName);
	bush_InitTree(self,GivenDirName);
	bush_BuildSubDirs(self,bush_TreeRoot(self));
    }
    OUT(bush_Create);
    return(self);
}

boolean
bush__InitializeObject( ClassID, self )
    struct classheader	*ClassID;
    struct bush		*self;
{
    Tree = tree_Create(DirTree,self);
    RootPath = NULL;
    *GivenDirName = 0;
    Debug = 0;
    UidUnameMap = vector_Create(30,3);
    MyCellName[0] = '\0';
    return(TRUE);
}

static int
ExtractNodePath( self, source, path )
    struct bush	    *self;
    char	    *source, **path;
{
    register long     status = 0, i = 0;
    char	     full_path[MAXPATHLEN + 1], 
		     workingPathName[MAXPATHLEN + 1];
    register char   *ptr;

    IN(ExtractNodePath);
    getwd(workingPathName);
    strcpy(full_path,workingPathName);
    switch(*source) {
	case '.':
	    while(*(source + i) == '.' && *(source + i + 1) == '.' &&
		  (*(source + i + 2) == '\0' || *(source + i + 2) == '/')) {
		if(ptr = (char*)rindex(full_path,'/')) *ptr = '\0';
		if(*(source + i + 2) == '\0') {
		    i += 2;
		    break;
		}
		else i += 3;
	    }
	    if(i && *(source + i) != '\0') {
		strcat(full_path,"/");
		strcat(full_path,source + i);
	    }
	    source = full_path;
	    break;
	default:
	    if(*source != '/') {
		strcat(full_path,"/");
		strcat(full_path,GivenDirName);
		source = full_path;
	    }
    }
    if(!status && (*path = (char*)malloc(strlen(source)+1)))
	strcpy(*path,source);
    else status = -1;
    OUT(ExtractNodePath);
    return(status);
}

static int
ExtractNodeName( source, name )
    register char	*source;
    register char      **name;
{
    register long	 status = 0;
    register char	*ptr = NULL;

    if((ptr = (char*)rindex(source,'/')) && *(source+1) != '\0')
	source = ++ptr;
    if(*name = (char*)malloc(strlen(source)+1)) strcpy(*name,source);
    else status = -1;
    return(status);
}

void
bush__InitTree( self, root_path )
    struct bush		*self;
    char		*root_path;
{
    tree_type_node	 root = NULL;
    struct Dir_		*rootDir = (struct Dir_ *)calloc(1,sizeof(struct Dir_));
    char		*nodeName = NULL, tmp[MAXPATHLEN];
    struct stat		 stats;

    IN(bush_InitTree);
    if(RootPath) {
	free(RootPath);
	RootPath = NULL;
    }
    ExtractNodePath(self,root_path,&RootPath);
    strcpy(GivenDirName,root_path);
    strcpy(tmp,RootPath);
    ExtractNodeName(tmp,&nodeName);
    AllocNameSpace(&rootDir->name,nodeName);
    if(stat(RootPath,&stats)) return;
    else {
	gethomecell(self,baseName);
	root = tree_CreateRootNode(Tree,rootDir->name,(char*)rootDir);
	rootDir->tn = root;
	rootDir->detail = -1;
	rootDir->sortmode = -1;
	AllocNameSpace(&rootDir->path,RootPath);
	bush_ScanDir(self,root);
	DirMode(root).selected = TRUE;
    }
    OUT(bush_InitTree);
}

void
bush__DestroySubDirs( self, tn )
    struct bush		*self;
    tree_type_node	 tn;
{
    IN(bush_DestroySubDirs);
    bush_FreeSubDirs(self,tn);
    tree_DestroyNodeChildren(Tree,tn);
    IN(bush_DestroySubDirs);
}

static char *
gethomecell( self, filename )
    struct bush		    *self;
    char		    *filename;
{
#ifdef AFS_ENV
    struct ViceIoctl	     blob;
    int			     outcome;

    blob.in_size  = sizeof(baseName);
    blob.in       = baseName;
    blob.out_size = MAX_PIOCTL_BUFF_SIZE;
    blob.out      = MyCellName;

    outcome = pioctl(baseName,VIOC_GET_PRIMARY_CELL,&blob,1);
    if(outcome) {
	blob.in_size  = sizeof(baseName);
	blob.in       = baseName;
	blob.out_size = MAX_PIOCTL_BUFF_SIZE;
	blob.out      = MyCellName;

	outcome = pioctl(baseName,VIOC_GET_WS_CELL,&blob,1);
	if (outcome) sprintf(MyCellName,"%s","andrew.cmu.edu");
	return(MyCellName);
    }
#else
    return("");
#endif /* AFS_ENV */
}

static char *
getcell( self, filename )
    struct bush		*self;
    char		*filename;
{
#ifdef AFS_ENV
    struct ViceIoctl	 blob;
    static char		 residence[MAX_PIOCTL_BUFF_SIZE];

    blob.in_size  = sizeof(filename);
    blob.in       = filename;
    blob.out_size = MAX_PIOCTL_BUFF_SIZE;
    blob.out      = residence;

    if(pioctl(filename,VIOC_FILE_CELL_NAME,&blob,1))
	return(MyCellName);
    return(residence);
#else
    return("");
#endif /* AFS_ENV */
}

static char *
getname( self, uid, cell )
    struct bush			*self;
    int				 uid;
    char			*cell;
{
    register int		 i = 0;
    register struct map_item	*item = NULL;
    char			*uname = NULL;
    register struct passwd	*pw = NULL;
#ifdef AFS_ENV
    for( i = 0 ; i < vector_Count(UidUnameMap) ; i++ ) {
	item = (struct map_item*)vector_Item(UidUnameMap,i);
	if((uid == item->uid) && !strcmp(cell,item->ucell)) {
	    uname = item->uname;
	    break;
	}
    }
    if(!uname) {
	if(pw = (struct passwd *) getcpwuid(uid,cell)) {
	    item = (struct map_item*)calloc(1,sizeof(struct map_item));
	    item->uid = uid;
	    AllocNameSpace(&item->uname,pw->pw_name);
	    AllocNameSpace(&item->ucell,cell);
	    vector_AddItem(UidUnameMap,(long)item);
	    uname = item->uname;
	}
	else {
	    char	    uid_str[200];

	    item = (struct map_item*)calloc(1,sizeof(struct map_item));
	    item->uid = uid;
	    sprintf(uid_str,"%u@%s",uid,cell);
	    AllocNameSpace(&item->uname,uid_str);
	    AllocNameSpace(&item->ucell,cell);
	    vector_AddItem(UidUnameMap,(long)item);
	    uname = item->uname;
	}
    }
#else /* AFS_ENV */
    for( i = 0 ; i < vector_Count(UidUnameMap) ; i++ ) {
	item = (struct map_item*)vector_Item(UidUnameMap,i);
	if(uid == item->uid) {
	    uname = item->uname;
	    break;
	}
    }
    if (!uname) {
	if(pw = getpwuid(uid)) {
	    item = (struct map_item*)calloc(1,sizeof(struct map_item));
	    item->uid = uid;
	    AllocNameSpace(&item->uname,pw->pw_name);
	    AllocNameSpace(&item->ucell,"");
	    vector_AddItem(UidUnameMap,(long)item);
	    uname = item->uname;
	}
	else {
	    char	    uid_str[200];

	    item = (struct map_item*)calloc(1,sizeof(struct map_item));
	    item->uid = uid;
	    sprintf(uid_str,"%u",uid);
	    AllocNameSpace(&item->uname,uid_str);
	    AllocNameSpace(&item->ucell,"");
	    vector_AddItem(UidUnameMap,(long)item);
	    uname = item->uname;
	}
    }
#endif /* AFS_ENV */
    return(uname);
}

int
bush__ScanDir( self, tn )
    struct bush		    *self;
    tree_type_node	     tn;
{
    register long	     i = 0, status = ok, count = 0;
    register char	    *ptr = NULL;
    struct direct	   **anchor = NULL;
    struct stat		     stats;
    int			    alphasort();
    char		    fullEntryName[MAXPATHLEN+25];

    IN(bush_ScanDir);
    if(!tn) return (scan_failure);
    if(stat(DirPath(tn),&stats)) {
	status = scan_failure;
	DirMode(tn).stat_failed = TRUE;
    }
    else DirTimeStamp(tn) = stats.st_mtime;
    if(status == ok  && 
	(count = scandir(DirPath(tn),&anchor,NodeFilter,alphasort)) < 0) {
	    status = read_failure;
	    DirMode(tn).scan_failed = TRUE;
    }
    else if(status == ok) {
	if(Child(tn)) bush_DestroySubDirs(self,tn);
	else bush_DestroyDirEntries(self,tn);
	if(count)
	    if(!(DirEntries(tn) = (struct Dir_Entries*) 
		calloc(1,sizeof(struct Dir_Entries)))) status = no_space;
	    else {
		DirEntryPtr(tn) = 
		    (struct Dir_Entry **) calloc(count,sizeof(struct Dir_Entry*));
		DirEntriesCount(tn) = count;
	    }
	for( i = 0; i < count && status == ok; i++ ) {
	    DirEntry(tn,i) = (struct Dir_Entry*)calloc(1,sizeof(struct Dir_Entry));
	    DirEntryPos(tn,i) = i;
	    AllocNameSpace(&DirEntryName(tn,i),anchor[i]->d_name);
	    ptr = DirEntryName(tn,i);
	    while(*ptr != '\0') {
		if(!(isascii(*ptr) && isprint(*ptr))) *ptr = '.';
		ptr++;
	    }
	    sprintf(fullEntryName,"%s/%s",DirPath(tn),anchor[i]->d_name);
	    if(stat(fullEntryName,&stats)) DirEntryMode(tn,i).stat_failed = TRUE;
	    else {
		if((stats.st_mode & S_IFMT) == S_IFDIR)	
		    DirEntryType(tn,i).dir = TRUE;
		else if((stats.st_mode & S_IFMT) == S_IFLNK) 
		    DirEntryType(tn,i).soft_link = TRUE;
		else DirEntryType(tn,i).file = TRUE;
		AllocNameSpace(&DirEntryOwner(tn,i),
			getname(self,stats.st_uid,getcell(self,fullEntryName)));
		DirEntryTimeStamp(tn,i) = stats.st_mtime;
		DirEntrySize(tn,i) = stats.st_size;
		DirEntryPerms(tn,i) = stats.st_mode;
		DirEntryNLinks(tn,i) = stats.st_nlink;
	    }
	    DirEntryParent(tn,i) = tn;
	}
    }
    if(anchor) free(anchor);
    DirMode(tn).do_rescan = FALSE;
    OUT(bush_ScanDir);
    return(status);
}

void
bush__BuildSubDirs( self, tn )
    struct bush		*self;
    tree_type_node	 tn;
{
    register long	 i = 0, count = 0;
    tree_type_node	 newTreeNode = NULL;
    struct Dir_		*newDir = NULL;
    char		 newDirPath[MAXPATHLEN];

    IN(bush_BuildSubDirs);
    if(tn && DirEntries(tn)) {
	count = DirEntriesCount(tn);
	while(i < count) {
	    if(DirEntry(tn,i) && !(DirEntryMode(tn,i).destroyed) && 
		DirEntryType(tn,i).dir) {
		    newDir = (struct Dir_ *) calloc(1,sizeof(struct Dir_));
		    sprintf(newDirPath,"%s%s%s",DirPath(tn),
			    !strcmp(DirPath(tn),"/") ? "": "/",DirEntryName(tn,i));
		    AllocNameSpace(&newDir->path,newDirPath);
		    AllocNameSpace(&newDir->name,DirEntryName(tn,i));
		    newTreeNode = tree_CreateChildNode(Tree,DirEntryName(tn,i),
			(char*)newDir,tn);
		    DirSortMode(newTreeNode) = -1;
		    DirDetail(newTreeNode) = -1;
		    DirMode(newTreeNode).do_rescan = TRUE;
		    DirEntryInstance(tn,i) = newTreeNode;
		    newDir->tn = newTreeNode;
	    }
	    i++;
	}
    }
    OUT(bush_BuildSubDirs);
}

void
bush__DestroyDirEntries( self, tn )
    struct bush		*self;
    tree_type_node	 tn;
{
    register long	 i = 0, count = 0;

    IN(bush_DestroyDirEntries);
    if(tn && DirEntries(tn)) {
	count = DirEntriesCount(tn);
	while(i < count) {
	    if(DirEntry(tn,i)) {
		if(DirEntryName(tn,i)) {
		    free(DirEntryName(tn,i));
		    DirEntryName(tn,i) = NULL;
		}
		if(DirEntry(tn,i)) {
		    free(DirEntry(tn,i));
		    DirEntry(tn,i) = NULL;
		}
	    }
    	    i++;
	}
	if(DirEntryPtr(tn)) {
	    free(DirEntryPtr(tn));
	    DirEntryPtr(tn) = NULL;
	}
	if(DirEntries(tn)) {
	    free(DirEntries(tn));
	    DirEntries(tn) = NULL;
	}
    }
    OUT(bush_DestroyDirEntries);
}

void
bush__DestroyDirEntry( self, tn )
    struct bush	    *self;
    tree_type_node   tn;
{
    IN(bush_DestroyDirEntry);
    if(tn && bush_Dir(self,tn) && DirEntries(tn)) {
	bush_DestroyDirEntries(self,tn);
	if(DirPath(tn)) {
	    free(DirPath(tn));
	    DirPath(tn) = NULL;
	}
	if(DirName(tn)) {
	    free(DirName(tn));
	    DirName(tn) = NULL;
	}
	free(bush_Dir(self,tn));
	tree_SetNodeDatum(Tree,tn,NULL);
    }
    OUT(bush_DestroyDirEntry);
}

void
bush__FreeSubDirs( self, tn )
    struct bush		    *self;
    tree_type_node	     tn;
{
    register tree_type_node  tmp = NULL;
    int 		     level = 0;

    IN(bush_FreeSubDirs);
    if((tmp = tn) && ((level = tree_NodeLevel(Tree,tmp)) > 0))
	while((tmp = tree_NextNode(Tree,tmp)) && (tree_NodeLevel(Tree,tmp) > level))
	    bush_DestroyDirEntry(self,tmp);
}

boolean
bush__ScanRequired( self, tn )
    struct bush	    *self;
    tree_type_node   tn;
{
    boolean	     status = FALSE;
    struct stat	     stats;

    IN(bush_ScanRequired);
    if(!DirMode(tn).destroyed &&
	(stat(DirPath(tn),&stats) ||
         DirTimeStamp(tn) != stats.st_mtime ||
         DirMode(tn).do_rescan))
	    status = TRUE;
    OUT(bush_ScanRequired);
    
    return(status);
}

int
bush__DestroyEntry( self, tn, Entry )
    struct bush		*self;
    tree_type_node	 tn;
    struct Dir_Entry	*Entry;
{
    char		 item[MAXPATHLEN*2];
    register long	 status = 0;

    sprintf(item,"%s/%s",DirPath(tn),Entry->name);
    if(Entry->type.dir) {
	static	char	*argv[4] = {"rm","-rf",NULL,NULL};

	argv[2] = item;
	status = bush_PerformSystemAction(self,"/bin/rm",argv);
    }
    else status = unlink(item);
    if(!status) {
	Entry->mode.destroyed = TRUE;
	DirMode(tn).do_rescan = TRUE;
    }
    return(status);
}

int
bush__MoveEntry( self, tn, Entry, newName )
    struct bush		*self;
    tree_type_node	 tn;
    struct Dir_Entry	*Entry;
    char		*newName;
{
    char		 oldPath[MAXPATHLEN*2], newPath[MAXPATHLEN];
    register long	 status;

    sprintf(oldPath,"%s/%s",DirPath(tn),Entry->name);
    sprintf(newPath,"%s/%s",DirPath(tn),newName );
    if((status = rename(oldPath,newPath)) != -1 )
	AllocNameSpace(&Entry->name,newName);
    return(status);
}

int
bush__RenameDir( self, tn, newPath, newName )
    struct bush	    *self;
    tree_type_node   tn;
    char	    *newPath, *newName;
{
    register long     status = ok, i = 0;
    register char   *newFullName = NULL;

    IN(bush_RenameDir);
    newFullName = (char*)malloc(strlen(newPath)+strlen(newName)+2);
    sprintf(newFullName,"%s/%s",newPath,newName);
    if(status = rename(DirPath(tn),newFullName)) return(status);
    else {
	AllocNameSpace(&DirPath(tn),newFullName);
	AllocNameSpace(&DirName(tn),newName);
	for(i=0;i<DirEntriesCount(Parent(tn));i++)
	    if(DirEntryInstance(Parent(tn),i)==tn) {
		AllocNameSpace(&DirEntryName(Parent(tn),i),DirName(tn));
		break;
	    }
    }
    OUT(bush_RenameDir);
    return(status);
}

long
bush__Read( self, file, id )
    struct bush	    *self;
    FILE	    *file;
    long	     id;
{
    char	     RootPathIfInset[MAXPATHLEN];
    long	     status = dataobject_NOREADERROR;

    IN(bush_Read);
    self->header.dataobject.id = dataobject_UniqueID(self);
    fscanf(file,"%s",GivenDirName);
    im_GetDirectory(RootPathIfInset);
    bush_InitTree(self,RootPathIfInset);
    bush_BuildSubDirs(self,bush_TreeRoot(self));
    chdir(bush_DirPath(self,bush_TreeRoot(self)));
    fscanf(file,"%s",RootPathIfInset); /* to get past enddata token */
    OUT(bush_Read);
    return(status);
}

long
bush__Write( self, file, id, level )
    struct bush	    *self;
    FILE	    *file;
    long	     id;
    long	     level;
{
    IN(bush_Write);
    if(self->header.dataobject.writeID != id) {
	self->header.dataobject.writeID = id;
	if(level) {
		fprintf(file,"\\begindata{%s,%d}\n",
			class_GetTypeName(self),
			dataobject_UniqueID(&self->header.dataobject));
		fprintf(file,"%s",DirPath(bush_TreeRoot(self)));
		fprintf(file,"\n\\enddata{%s,%d}\n",
			class_GetTypeName(self),
			dataobject_UniqueID(&self->header.dataobject));
	}
	else {
		fprintf(file,"\\begindata{%s,%d}\n",
			class_GetTypeName(self),
			dataobject_UniqueID(&self->header.dataobject));
		fprintf(file,"\n%s\n",DirPath(bush_TreeRoot(self)));
		fprintf(file,"\n\\enddata{%s,%d}\n",
			class_GetTypeName(self),
			dataobject_UniqueID(&self->header.dataobject));
	}
    }
    OUT(bush_Write);
    return((long)self);
}

char *
bush__ViewName( self )
    struct bush	    *self;
{
    return("bushv");
}

int
bush__PerformSystemAction( self, name, argv )
    struct bush		*self;
    char		*name;
    char		*argv[];
{
    int			pid = 0, status = 0;

    if((pid = osi_vfork()) == 0) {
	extern		getpid();
	register int	fd;

	setpgrp(0,pid = getpid());
	fd = open("/dev/null",O_WRONLY,0644);
	if(fd >= 0) dup2(fd,1);
	close(fd);
	execvp(name,argv);
	/* flow should never reach here, but just in case.... */
	return(-1);
    }	
    while(pid != wait(&status));
    return(status);
}
