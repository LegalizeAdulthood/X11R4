/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/* ************************************************************ *\
	btr1.c
	More library routines for reading B-trees.
	Include file ``bt.h'' declares the procedures for clients.
	Include file ``btint.h'' declares common structures for the implementation modules.
\* ************************************************************ */

static char rcsid[] = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/btr1.c,v 2.6 89/02/16 13:09:15 ghoti Exp $";

#include <andyenv.h>
#include <sys/param.h>
#include <sys/dir.h>
#include <stdio.h>
#include <andrewos.h>	/* file, time, strings */
#include <ctype.h>
#include <errno.h>
#include <truth.h> /* itc.h -> truth.h DMT */
#include <sys/stat.h>
#include <netinet/in.h>
#include <util.h>
#ifdef WHITEPAGES_ENV  /* avoid makedepend "errors" */
#include <btint.h>
#endif /* WHITEPAGES_ENV   */

extern int errno;

extern char *malloc();
extern char *realloc();

extern int br_Debugging;

/* Advance the cursor to the next key-value pair.
Declaration:
	extern bt_ErrorCode bt_NextCursor(curs);
	struct btCursor *curs;
*/
bt_ErrorCode bt_NextCursor(curs)
struct btCursor *curs;
{
    struct btC *bC = (struct btC *) curs;
    int Idx, Exact, Flags, RootNameLength, ValueLength, ThisByte;
    bt_ErrorCode RetVal;
    auto char NodeFileName[MAXPATHLEN+1];
    char *NFNEnd, *NFNPtr, *OldKey;
    struct btFile *bF;

    if (bC == NULL || bC->Tag != btCTag
	 || bC->Tree == NULL || bC->Tree->Tag != BTrTag)
	return bterr_CursorTreeDamaged;
    if (bC->State == UnInitialized) return bterr_UninitializedCursor;
    if (bC->FP == NULL || bC->FP->Tag != btFileTag) return bterr_CursorTreeDamaged;
    if (bC->FP->Head.BTDepth != 0) return bterr_NotAtKey;
    switch (bC->State) {
	case Null: return bterr_EmptyTree;
	case BeforeFirst: {
	    /*			if (bC->FP->IndexNum == 0)
	    {bC->State = Null; return bterr_EmptyTree;} */
	    Idx = 0;
	    break;
	    }
	case AfterLast: return bterr_NoNextKey;
	default: {Idx = bC->IndexPos + 1; break;}
    }
    if (Idx < bC->FP->IndexNum) {   /* the typical case: see if next key is a brother link */
	RetVal = b_GetFlags(bC->FP, Idx, &Flags);
	if (RetVal != bterr_NoError) return RetVal;
	if ((Flags & BTIsBrotherLink) == 0) {
	    bC->IndexPos = Idx;
	    bC->State = AtKey;
	    return bterr_NoError;
	}
    } else {			/* see if the old (current) key was a brother link */
	--Idx;
	RetVal = b_GetFlags(bC->FP, Idx, &Flags);
	if (RetVal != bterr_NoError) return RetVal;
	if ((Flags & BTIsBrotherLink) == 0) {
	    bC->State = AfterLast;	/* was no brother link */
	    return bterr_NoNextKey;
	}
    }
    /* OK, follow the brother link.  First we copy the key for the brother link. */
    bC->State = AtKey; bC->IndexPos = Idx;	/* Get the brother-link key */
    RetVal = bt_GetCursorKey(curs, &OldKey);	/* copy key for searching next node */
    if (RetVal != bterr_NoError) return RetVal;
    /* Follow a right-brother link */
    strcpy(NodeFileName, bC->Tree->Root->FileName);
    RootNameLength = strlen(NodeFileName);
    NFNEnd = &NodeFileName[RootNameLength];
    *NFNEnd++ = '.';
    for (;;) {
	RetVal = b_GetValueLength(bC->FP, Idx, &ValueLength);
	if (RetVal != bterr_NoError) {free(OldKey); return RetVal;}
	if (ValueLength <= 0) {free(OldKey); return bterr_BTreeDamaged;}
	if (ValueLength + RootNameLength >= MAXPATHLEN)
	{free(OldKey); return bterr_IntermediateNameTooLong;}
	NFNPtr = NFNEnd;
	while ((--ValueLength) >= 0) {
	    ThisByte = fgetc(bC->FP->File);
	    if (ThisByte == EOF) {free(OldKey); return bterr_BTreeDamaged;}
	    *NFNPtr++ = ThisByte;
	}
	*NFNPtr = '\0';		/* terminate it */
	bF = b_NewbtFileStr();
	if (bF == NULL) {free(OldKey); return bterr_OutOfMemory;}
	bF->RefCount = 1;		/* preset the ref count */
	RetVal = b_ReadbtFile(bF, NodeFileName, FALSE);
	if (RetVal != bterr_NoError) {free(bF); free(OldKey); return RetVal;}
	if (bF->Head.BTDepth != 0)
	{b_DecrRefCount(&bF); free(OldKey); return bterr_BTreeDamaged;}
	b_StoreFilePtr(&bC->FP, bF);
	b_DecrRefCount(&bF);	/* and decrement it when safe */
	RetVal = b_ScanNode(bC->FP, OldKey, &Idx, &Exact, &Flags);
	if (RetVal != bterr_NoError) {free(OldKey); return RetVal;}
	if (Exact == 0) {	/* we landed between keys; advance. */
	    if ((Idx+1) < bC->FP->IndexNum) {	/* not looking past last entry */
		++Idx;
		RetVal = b_GetFlags(bC->FP, Idx, &Flags);
		if (RetVal != bterr_NoError) {free(OldKey); return RetVal;}
	    } else {
		if ((Flags & BTIsBrotherLink) == 0) {
		    bC->State = AfterLast;
		    free(OldKey);
		    return bterr_NoNextKey;
		}
	    }
	}
	if (Idx < 0 || (Flags & BTIsBrotherLink) == 0) {
	    bC->IndexPos = Idx;
	    if (bC->IndexPos < 0) bC->IndexPos = 0;
	    bC->State = AtKey;
	    free(OldKey);
	    if (Idx >= 0 && (Flags & BTIsLeafPair) == 0)
		return bterr_BTreeDamaged;
	    else return bterr_NoError;
	} /* else is another brother link--search it, too. */
    }
}
