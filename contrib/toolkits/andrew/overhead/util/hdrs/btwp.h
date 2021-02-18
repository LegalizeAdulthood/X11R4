/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/btwp.h,v 2.6 89/05/19 17:39:00 cfe Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/btwp.h,v $ */

#if !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS)
static char *rcsid_h = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/hdrs/RCS/btwp.h,v 2.6 89/05/19 17:39:00 cfe Exp $ ";
#endif /* !defined(lint) && !defined(LOCORE) && defined(RCS_HDRS) */

/* ************************************************************ *\
	btwp.h
	Declare the format for the B-tree that represents the white pages database.
	Also, define the particular fields making up the WP database.
	For use only internally to the WP library.
\* ************************************************************ */

/* IF YOU CHANGE THE SET OF FldXXX NAMES, CHANGE ``btwp.c'' AND ``makeboth.c'' ALSO. */
/* Attributes making up the White Pages */
#define FldN 0
#define FldTk 1
#define FldWN 2
#define FldID 3
#define FldEK 4
#define FldNI 5
#define FldGI 6
#define FldPW 7
#define FldHD 8
#define FldSh 9
#define FldAf 10
#define FldFwd 11
#define FldDK 12
#define FldDP 13
#define FldD 14
#define FldX 15
#define FldSI 16
#define FldNPA 17
#define FldDt 18
#define FldA2 19
#define FldHA 20
#define FldHP 21
#define FldCA 22
#define FldCX 23
#define FldOA 24
#define FldOP 25
#define FldMAX 25
#define FldCOUNT 26
extern char *wpFieldName[FldCOUNT+1];

/* Define the kinds of indices we'll use. */
/* IF YOU ADD A FIELD, CHANGE ``btIndexNames'' in ``btwp.c'' ALSO. */
#define KeyTagSep '"'	/* separates key value from key kind tag */
#define KeyTagR ('r')	/* for the full record, indexed by the magic prime key */
#define KeyTagTk	('t')
#define KeyTagNI ('#')
#define KeyTagID ('i')
#define KeyTagN ('n')	/* also indexes FldWN */
#define KeyTagSk ('s')	/* Surname-only copy of Tk */
#define KeyTagCS ('d')	/* Canonicalized surname */
#define KeyTagCG ('x')	/* Canonicalized given-name */
#define KeyTagNk ('k')	/* Maps nicknames to given-names */
#define KeyTagOv ('v')	/* Override a nickname */

/* IF YOU CHANGE THE FOLLOWING, CHANGE ``makeboth.c'' ALSO. */
#define NumBTIndices 9
extern char BTIxTags[];
extern int BTIndices[];
extern int BTAuxIndices[];
#define BTIxTk 0
#define BTIxNI 1
#define BTIxID 2
#define BTIxNWN 3
#define BTIxSk 4
#define BTIxCS 5
#define BTIxCG 6
#define BTIxNk 7
#define BTIxOv 8
#define BTIxCOUNT 9
extern char *btIndexNames[BTIxCOUNT];

/* Length of the prime keys in the index arrays */
#define PKLEN 8
typedef struct {char RecID[PKLEN];} btwp_identifier;

/* Data structures necessary for searching the database.
	An IdSet is a set of btwp_identifier's for use in NameBase.
	This is analogous to the PrimeKeySet that these routines return.
*/
#define IdSetTag -71
struct IdSet {
		int		Tag;
		int		IdCount;
		int		IdMax;
		btwp_identifier	*Ids;};
#define Undef_IdSet ((struct IdSet *) 0)
#define Empty_IdSet ((struct IdSet *) -1)

/* Structures private to the WP that get manipulated by the client */
#define ConstraintTag	-68
struct wp_Constraint	{
		int		Tag;
		struct wp_Constraint	*Next;
		wp_FieldIndex	FieldNum;
		char		*FieldContent;
		enum wp_ConstraintKind	ConstraintKind;
			};

#define SrchTokenTag	-66
struct wp_SrchToken	{
		int		Tag;
		struct wp_Constraint	*Constraints;
		char		*Probe;
		int		SearchKind;
			};

/* In wpbase.c. */
extern wp_ErrorCode w_LoadEntry();
extern wp_ErrorCode w_GrowString();
extern void w_LowerAll();
extern wp_ErrorCode w_ProbeMatches();
extern void w_ZapIdSet();
extern wp_ErrorCode w_IDtoPKSet();

/* The TokenChar table tells whether we believe that a given character should be a searchable part of a person's name. */
extern char wpTokenChar[256];

extern int wpSortIxValue();
/* Sort the array of index values at IxLoc (of length IxSize, in units of PKLEN), in place, and flush duplicates.  Return the size of the canonicalized array, in units of PKLEN. */

extern char *CanonSurn(), *CanonGiven(), *CanonNick();
/* These each return a freshly-malloc'ed piece of storage containing a phonetically-canonicalized version of the given Surname, GivenName, or (possible) Nickname. */

extern struct BTree *w_WPTree;
extern struct btCursor *w_WPCursor;
extern wp_ErrorCode w_BTreeErr();

/* Cell/directory token returned by wp_InitializeCell and wp_InitializeDir and passed to cwp_Search, cwp_Lookup, cwp_Read, and cwp_Generate. */
#define wpcdTag	-106
struct wp_CD {
	int		Tag;
	struct wp_CD	*Next, *Prev;
	char		*CellName, *DirName;
	int		TimesInited;
	struct BTree	*tree;
	struct btCursor	*cursor;
	btwp_identifier	LastRetrieved;
	char		*Entries[FldCOUNT];
	int		EntriesSize[FldCOUNT];
	char		PrevPK[PKLEN+1];
};
