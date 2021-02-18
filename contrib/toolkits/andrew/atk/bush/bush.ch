/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidbush_ch = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/bush/RCS/bush.ch,v 1.5 89/10/23 15:52:02 gk5g Exp $";
#endif

/*
    $Log:	bush.ch,v $
Revision 1.5  89/10/23  15:52:02  gk5g
Added method bush_PerformSystemAction.

Revision 1.4  89/09/01  13:30:57  gk5g
Made change from tree_NodeData to tree_NodeDatum.

Revision 1.3  89/07/28  15:43:54  gk5g
Change the token AptTree to just Tree.

Revision 1.2  89/07/19  18:45:30  gk5g
Added new instance vars (struct vector *) uid_name_map and char mycellname[] to support cross-cellular uid lookups.

Revision 1.1  89/04/28  21:24:03  tom
Initial revision

Revision 1.1  89/04/28  21:20:41  tom
Initial revision

*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	The Bush Data-object

MODULE	bush.ch

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

END-SPECIFICATION  ************************************************************/

struct entry_mode {
    unsigned	    do_rescan:1;
    unsigned	    destroyed:1;
    unsigned	    selected:1;
    unsigned	    detailed:1;
    unsigned	    stat_failed:1;
    unsigned	    scan_failed:1;
};

struct entry_kind {
    short	    dir:1;
    short	    soft_link:1;
    short	    file:1;
};

struct Dir_Entry {
    char		*name;
    struct entry_kind	 type;
    struct entry_mode	 mode;
    size_t		 size;
    time_t		 time_stamp;
    char		*owner;
    int			 nlinks;
    unsigned		 permissions;
    int			 position;
    tree_type_node	 parent, instance;
};

struct Dir_Entries {
    int			count;
    struct Dir_Entry  **Entry;
};

struct Dir_ {
    time_t		 time_stamp;
    char		*path, *name;
    int			 sortmode, detail;
    struct entry_mode	 mode;
    tree_type_node	 tn;
    struct Dir_Entries  *Dir_Entries;
};

class bush : apt {
    classprocedures:
	Create(char *init_dir)			returns struct bush*;
    methods:
	InitTree(char *root_path) returns void;
	ScanDir(tree_type_node tn) returns int;
	BuildSubDirs(tree_type_node tn)	returns void;
	DestroySubDirs(tree_type_node tn) returns void;
	DestroyDirEntries(tree_type_node tn) returns void;
	DestroyDirEntry(tree_type_node tn) returns void;
	FreeSubDirs(tree_type_node tn) returns void;
	DestroyEntry(tree_type_node tn,struct Dir_Entry *Entry) returns int; 
	ScanRequired(tree_type_node tn) returns boolean;
	RenameDir(tree_type_node tn,char *newPath, char *newName) returns int;
	MoveEntry(tree_type_node tn,struct Dir_Entry *Entry,char *newName) returns int;
	PerformSystemAction( char* name, char **argv ) returns int;
    overrides:
  	Read(FILE *file,long id) returns long;
  	Write(FILE *file,long id,long level) returns long;
	ViewName() returns char *;
    macromethods:
	Tree() (self->tree)
	TreeRoot() (tree_RootNode(bush_Tree(self)))
	RootPathName() (self->root_pathname)
	GivenRootPathName() (self->given_dir_name)
	Dir(tn) ((struct Dir_ *)tree_NodeDatum(bush_Tree(self),tn))
	DirMode(tn) (bush_Dir(self,tn)->mode)
	DirPath(tn) (bush_Dir(self,tn)->path)
	DirName(tn) (bush_Dir(self,tn)->name)
	DirSortMode(tn) (bush_Dir(self,tn)->sortmode)
	DirDetail(tn) (bush_Dir(self,tn)->detail)
	DirTimeStamp(tn) (bush_Dir(self,tn)->time_stamp)
	RootDirPath() (bush_DirPath(self,bush_TreeRoot(self)))
	DirEntries(tn) (bush_Dir(self,tn)->Dir_Entries)
	DirEntriesCount(tn) (bush_DirEntries(self,tn)->count)
	DirEntryPtr(tn) (bush_DirEntries(self,tn)->Entry)
	DirEntry(tn,i) (bush_DirEntries(self,tn)->Entry[i])
	DirEntryMode(tn,i) (bush_DirEntry(self,tn,i)->mode)
	DirEntryPos(tn,i) (bush_DirEntry(self,tn,i)->position)
	DirEntryName(tn,i) (bush_DirEntry(self,tn,i)->name)
	DirEntryType(tn,i) (bush_DirEntry(self,tn,i)->type)
	DirEntryOwner(tn,i) (bush_DirEntry(self,tn,i)->owner)
	DirEntryNLinks(tn,i) (bush_DirEntry(self,tn,i)->nlinks)
	DirEntryTimeStamp(tn,i) (bush_DirEntry(self,tn,i)->time_stamp)
	DirEntrySize(tn,i) (bush_DirEntry(self,tn,i)->size)
	DirEntryPerms(tn,i) (bush_DirEntry(self,tn,i)->permissions)
	DirEntryParent(tn,i) (bush_DirEntry(self,tn,i)->parent)
	DirEntryInstance(tn,i) (bush_DirEntry(self,tn,i)->instance)
	DirEntryParentDir(tn,i) (bush_Dir(self,bush_DirEntryParent(self,tn,i)))
	RootDir() (bush_Dir(self,bush_TreeRoot(self)))
	Parent(tn) (tree_ParentNode(bush_Tree(self),tn))
	Child(tn) (tree_ChildNode(bush_Tree(self),tn))
	Left(tn) (tree_LeftNode(bush_Tree(self),tn))
	Right(tn) (tree_RightNode(bush_Tree(self),tn))
	ParentDir(tn) (bush_Dir(self,bush_Parent(self,tn)))
	ChildDir(tn) (bush_Dir(self,bush_Child(self,tn)))
	LeftDir(tn) (bush_Dir(self,bush_Left(self,tn)))
	RightDir(tn) (bush_Dir(self,bush_Right(self,tn)))
    data:
	char		 given_dir_name[1025];
	struct tree	*tree;
	char		*root_pathname;
	struct vector	*uid_uname_map;
	char		 mycellname[1000];
	int		 debug;
};

