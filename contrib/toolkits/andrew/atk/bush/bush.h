/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */

/* $Header $ */
/* $Source $ */

#ifndef lint
static char *rcsidbush_h = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/atk/bush/RCS/bush.h,v 1.2 89/09/01 13:30:33 gk5g Exp $";
#endif

/*
    $Log:	bush.h,v $
 * Revision 1.2  89/09/01  13:30:33  gk5g
 * Made change from tree_NodeData to tree_NodeDatum.
 * 
 * Revision 1.1  89/04/28  21:24:00  tom
 * Initial revision
 * 
 * Revision 1.1  89/04/28  21:20:38  tom
 * Initial revision
 * 
*/

/**  SPECIFICATION -- External Facility Suite  *********************************

TITLE	Bush Internal Symbolics

MODULE	bush.h

VERSION	0.0

AUTHOR	TC Peters & GW Keim
	Information Technology Center, Carnegie-Mellon University 

DESCRIPTION
	This internal symbolics for the Bush modules.

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

#define		Tree			    ((self)->tree)
#define		Root			    (tree_RootNode(Tree))
#define		RootPathName		    ((self)->root_pathname)
#define		Dir(tn)			    ((struct Dir_*)tree_NodeDatum(Tree,tn))
#define		DirMode(tn)		    (Dir(tn)->mode)
#define		DirPath(tn)		    (Dir(tn)->path)
#define		DirName(tn)		    (Dir(tn)->name)
#define		DirTimeStamp(tn)	    (Dir(tn)->time_stamp)
#define		DirSortMode(tn)		    (Dir(tn)->sortmode)
#define		DirDetail(tn)		    (Dir(tn)->detail)    
#define		RootDirPath		    (DirPath(Root))
#define		DirEntries(tn)		    (Dir(tn)->Dir_Entries)
#define		DirEntriesCount(tn)	    (DirEntries(tn)->count)
#define		DirEntryPtr(tn)		    (DirEntries(tn)->Entry)    
#define		DirEntry(tn,i)		    (DirEntries(tn)->Entry[i])
#define		DirEntryMode(tn,i)	    (DirEntry(tn,i)->mode)
#define		DirEntryPos(tn,i)	    (DirEntry(tn,i)->position)
#define		DirEntryName(tn,i)	    (DirEntry(tn,i)->name)
#define		DirEntryType(tn,i)	    (DirEntry(tn,i)->type)
#define		DirEntryOwner(tn,i)	    (DirEntry(tn,i)->owner)
#define		DirEntryNLinks(tn,i)	    (DirEntry(tn,i)->nlinks)
#define		DirEntryTimeStamp(tn,i)	    (DirEntry(tn,i)->time_stamp)
#define		DirEntrySize(tn,i)	    (DirEntry(tn,i)->size)
#define		DirEntryPerms(tn,i)	    (DirEntry(tn,i)->permissions)
#define		DirEntryParent(tn,i)	    (DirEntry(tn,i)->parent)
#define		DirEntryInstance(tn,i)	    (DirEntry(tn,i)->instance)
#define		DirEntryParentDir(tn,i)	    (Dir(DirEntryParent(tn,i)))    
#define		RootDir			    (Dir(Root))
#define		Parent(tn)		    (tree_ParentNode(Tree,tn))
#define		Child(tn)		    (tree_ChildNode(Tree,tn))
#define		Left(tn)		    (tree_LeftNode(Tree,tn))
#define		Right(tn)		    (tree_RightNode(Tree,tn))
#define		ParentDir(tn)		    (Dir(Parent(tn)))
#define		ChildDir(tn)		    (Dir(Child(tn)))
#define		LeftDir(tn)		    (Dir(Left(tn)))
#define		RightDir(tn)		    (Dir(Right(tn)))

#define		no_space		    2
#define		no_superior		    3
#define		no_inferior		    4
#define		scan_failure		    5
#define		read_failure		    6





