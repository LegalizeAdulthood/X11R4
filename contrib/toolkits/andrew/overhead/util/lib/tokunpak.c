/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
		tokunpak.c -- Subroutines for unpacking Venus tokens
			from datagrams and setting them
			as current with Venus.
*/
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/tokunpak.c,v 2.15 89/09/07 18:09:37 cfe Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/tokunpak.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/tokunpak.c,v 2.15 89/09/07 18:09:37 cfe Exp $";
#endif /* lint */

#include <andyenv.h>
#include <stdio.h>
#include <andrewos.h>		/* sys/types.h sys/time.h */
#include <netinet/in.h>
#include <svcconf.h>

#ifdef AFS_ENV
#include <rx/xdr.h>
#include <afs/afsint.h>
#include <afs/comauth.h>
#include <afs/auth.h>
#include <tokens.h>
#include <afs/cellconfig.h>
#define KEYSIZE	(sizeof(auth_EncryptionKey))
#include <errno.h>
#include <ctype.h>
#endif /* AFS_ENV */

#define NIL 0
extern int errno;
extern char *malloc(), *realloc();

#ifdef AFS_ENV
static void JustUnpack(tokens, ctok, stok, debug)
char *tokens;
ClearToken *ctok;
SecretToken *stok;
int debug;
{
#ifdef DEBUG
    if (debug) {
	register int i;
	register char *p;

	p = tokens;
	fputs("Packed tokens: 0x", stderr);
	for (i=0; i<TOKENS_LEN; i++)
	    fprintf(stderr, "%02x", (unsigned char) *p++);
	fputc('\n', stderr);
    }
#endif /* DEBUG */

    /* First, do secret token */
    stok->AuthHandle = * (long *) tokens;
    tokens += sizeof(long);
    stok->Noise1 = * (long *) tokens;
    tokens += sizeof(long);
    stok->ViceId = * (long *) tokens;
    tokens += sizeof(long);
    stok->BeginTimestamp = * (long *) tokens;
    tokens += sizeof(long);
    stok->Noise2 = * (long *) tokens;
    tokens += sizeof(long);
    stok->EndTimestamp = * (long *) tokens;
    tokens += sizeof(long);
    stok->Noise3 = * (long *) tokens;
    tokens += sizeof(long);
    stok->Noise4 = * (long *) tokens;
    tokens += sizeof(long);

    /* Now do clear token */
    ctok->AuthHandle = ntohl(* (long *) tokens);
    tokens += sizeof(long);
    ctok->ViceId = ntohl(* (long *) tokens);
    tokens += sizeof(long);
    ctok->BeginTimestamp = ntohl(* (long *) tokens);
    tokens += sizeof(long);
    ctok->EndTimestamp = ntohl(* (long *) tokens);
    tokens += sizeof(long);
    bcopy(tokens, stok->MagicString, sizeof(AuthMagic));
    tokens += sizeof(AuthMagic);
    bcopy(tokens, &stok->HandShakeKey, KEYSIZE);
    tokens += KEYSIZE;
    bcopy(tokens, &ctok->HandShakeKey, KEYSIZE);

#ifdef DEBUG
    if (debug) {
	register int i;
	register char *p;

	p = (char *) ctok;
	fputs("Clear token: 0x", stderr);
	for (i=0; i<sizeof(ClearToken); ++i)
	    fprintf(stderr, "%02x", (unsigned char) *p++);
	fputs("\nSecret token: 0x", stderr);
	p = (char *) stok;
	for (i=0; i<sizeof(SecretToken); ++i)
	    fprintf(stderr, "%02x", (unsigned char) *p++);
	fputc('\n', stderr);
    }
#endif /* DEBUG */
}
#endif /* AFS_ENV */

int unpacktokens(tokens, ctoken, stoken, debug, set)
#ifdef AFS_ENV
char *tokens;
ClearToken *ctoken;
SecretToken *stoken;
int debug, set;
{
    SecretToken s;
    ClearToken c;
    int rc;

    if (ctoken == NIL) ctoken = &c;
    if (stoken == NIL) stoken = &s;

    JustUnpack(tokens, ctoken, stoken, debug);

    if (set != 0) {
	rc = U_SetLocalTokens(1, &c, &s);
	return (rc == 0);
    } else
	return 1;
}
#else /* AFS_ENV */
{    return 0; }
#endif /* AFS_ENV */

#ifdef AMS_DELIVERY_ENV
int tok_GetStr(pRead, EndP, outStr, sizeOutStr)
char **pRead, *EndP, *outStr; int sizeOutStr;
{/* Get a null-terminated string from *pRead, putting it into outStr. */
    int CharsLeft;
    char *Out, *In;

    CharsLeft = sizeOutStr - 1;
    Out = outStr;
    for (In = *pRead; In < EndP && *In != '\0' && CharsLeft > 0; ++In, --CharsLeft) {
	*Out++ = *In;
    }
    if (CharsLeft > 0) *Out++ = '\0';
    if (CharsLeft == 0 || In == EndP) return 0;	/* If counts ran out, there was an error. */
    else {*pRead = In; return 1;}
}
#endif /* AMS_DELIVERY_ENV */

int tok_GenTokens(pWhere, pWhereLen, ctokP, stokP, begdP, expdP, vidP, cell, primP, locP, debug)
#ifdef AFS_ENV
char **pWhere;
int *pWhereLen;
ClearToken *ctokP; SecretToken *stokP;
int *begdP, *expdP, *vidP;
char *cell;
int *primP, *locP;
int debug;
{/* Read the next authentication from the sequence of them that starts at *pWhere, and return it.  Return >0 if successful, =0 if all done, and <0 on any error.
    After a successful return, ctokP and stokP will be loaded with tokens, begdP and expdP will have the beginning and ending times, vidP and cell the authentication (where cell is of size at least MAXCELLCHARS), primP and locP the indication of whether the auth is ``primary'' and/or workstation-local.
*/
    char *Read, *EndP, *Ck;
    char StrCellName[1+MAXCELLCHARS+1];
    int IsPrimary, IsLocal, CharFlag;

    Read = *pWhere;
    EndP = &Read[*pWhereLen];
    if ((Read + TOKENS_LEN) > EndP) return 0;	/* Finished */
    JustUnpack(Read, ctokP, stokP, debug);
    Read += TOKENS_LEN;
    if (tok_GetStr(&Read, EndP, StrCellName, sizeof(StrCellName)) == 0) {
	*pWhere += *pWhereLen;
	return -1;
    }
    ++Read;		/* must skip over the terminating NUL in any case. */
    while (((Read - (*pWhere)) % sizeof(long)) != 0) ++Read;
    *pWhereLen += (Read - *pWhere);	/* Advance the generator. */
    *pWhere = Read;

    /* We've now read the tokens, the primary flag, and the cell name.  Check 'em. */
    IsPrimary = IsLocal = CharFlag = 0;
    switch (StrCellName[0]) {
	case TokLocalPrimary:
	    IsLocal = IsPrimary = 1; break;
	case TokLocalNotPrimary:
	    IsLocal = 1; break;
	case TokIsPrimary:
	    IsPrimary = 1; break;
	case TokNotPrimary:
	    break;
	default:
	    CharFlag = 1; break;
    }
    if (CharFlag != 0 || StrCellName[1] == '\0') return -1;
    CharFlag = 0;
    for (Ck = &StrCellName[1]; *Ck != '\0'; ++Ck)
	if (!isascii(*Ck)) CharFlag = 1;
	else if (!isalnum(*Ck) && *Ck != '.' && *Ck != '-') CharFlag = 1;
    if (CharFlag != 0) return -1;

    /* OK: let's return this one. */
    *begdP = ctokP->BeginTimestamp;
    *expdP = ctokP->EndTimestamp;
    *vidP = ctokP->ViceId;
    strncpy(cell, &StrCellName[1], MAXCELLCHARS);
    *primP = IsPrimary;
    *locP = IsLocal;
    return 1;
}
#else /* AFS_ENV */
{    return 0; }
#endif /* AFS_ENV */

int GenTokens(pWhere, pWhereLen, ctokP, stokP, expdP, vidP, cell, primP, locP, debug)
#ifdef AFS_ENV
char **pWhere;
int *pWhereLen;
ClearToken *ctokP; SecretToken *stokP;
int *expdP, *vidP;
char *cell;
int *primP, *locP;
int debug;
{/* Read the next authentication from the sequence of them that starts at *pWhere, and return it.  Return >0 if successful, =0 if all done, and <0 on any error.
    After a successful return, ctokP and stokP will be loaded with tokens, expdP will have the expiration date, vidP and cell the authentication (where cell is of size at least MAXCELLCHARS), primP and locP the indication of whether the auth is ``primary'' and/or workstation-local.
*/
    int Beg;

    return tok_GenTokens(pWhere, pWhereLen, ctokP, stokP, &Beg, expdP, vidP, cell, primP, locP, debug);
}
#else /* AFS_ENV */
{    return 0; }
#endif /* AFS_ENV */

int UnpackAndSetTokens(Where, WhereLen, debug, setPag)
#ifdef AFS_ENV
char *Where;
int WhereLen, debug, setPag;
{/* Unpack the result of the given Where string, up to its length, and set those tokens in those cells. */
    ClearToken ctoken;
    SecretToken stoken;
    char *Read;
    char StrCellName[1+MAXCELLCHARS+1];
    int IsPrimary, TokIx, Now, IsLocal, wantSetPag, Remaining, Begd, Expd, VID;

    wantSetPag = setPag;
    Now = osi_GetSecs() + 15;
    TokIx = 0;
    Read = Where; Remaining = WhereLen;
    while (tok_GenTokens(&Read, &Remaining, &ctoken, &stoken, &Begd, &Expd, &VID, StrCellName, &IsPrimary, &IsLocal, debug) > 0) {
#ifdef DEBUG
	if (debug) {
	    register char *p;
	    register int i;

	    fprintf(stderr, "About to set %s %s tokens, exp [%d..%d], for %d in cell ``%s''.\n",
		    (IsPrimary ? "primary" : "non-primary"),
		    (IsLocal ? "local" : "non-local"),
		    Begd, Expd, VID, StrCellName);
	    p = (char *) &ctoken;
	    fputs("---Clear token: 0x", stderr);
	    for (i=0; i<sizeof(ClearToken); ++i)
		fprintf(stderr, "%02x", (unsigned char) *p++);
	    fputs("\n---Secret token: 0x", stderr);
	    p = (char *) &stoken;
	    for (i=0; i<sizeof(SecretToken); ++i)
		fprintf(stderr, "%02x", (unsigned char) *p++);
	    fputc('\n', stderr);
	}
#endif /* DEBUG */
	/* OK: let's try this one. */
	if (IsLocal) {
	    if (ULstrcmp(WorkstationName, StrCellName) == 0) {
		if (Now < Begd || Expd < Now) return -1;
#if 0
		if (setPag) {  /* if we wanted to do any setpags, assume we want setuid also. */
		    if (setreuid(VID, -1) == 0) ++TokIx;  /* LOCAL AUTH! */
		} else
#endif /* 0 */
		    ++TokIx;	/* count the token if it matches this WS name. */
	    }
	} else {
	    if (U_CellSetLocalTokens(wantSetPag, &ctoken, &stoken, StrCellName, IsPrimary) != 0) {
		if (Begd < Now && Now < Expd)
		    return -1;	/* Times ok: must be some real problem. */
	    }
	    wantSetPag = 0;	/* Never setpag() more than once */
	    ++TokIx;
	}
    }
    return TokIx;	/* number of tokens successfully set */
}
#else /* AFS_ENV */
{    return 0; }
#endif /* AFS_ENV */

int ExtractCellID(Where, WhereLen, cellName, debug)
#ifdef AFS_ENV
char *Where, *cellName;
int WhereLen, debug;
{/* Unpack the tokens at Where (for WhereLen), looking for a token for cell cellName.  If one is found, return the ViceId for that cell. */
    ClearToken ctoken;
    SecretToken stoken;
    char *Read;
    char StrCellName[1+MAXCELLCHARS+1];
    int wantLocal, haveLocal, IsPrimary, Remaining, Begd, Expd, VID;

    wantLocal = 0;
    if (ULstrcmp(cellName, "localhost") == 0) wantLocal = 1;
    else if (cellName[0] == '&' && ULstrcmp(&cellName[1], WorkstationName) == 0) wantLocal = 1;
    Read = Where;
    Remaining = WhereLen;
    while (tok_GenTokens(&Read, &Remaining, &ctoken, &stoken, &Begd, &Expd, &VID, StrCellName, &IsPrimary, &haveLocal, debug) > 0) {
#ifdef DEBUG
	if (debug) {
	    fprintf(stderr, "Token in cell %s: VID %d.\n", StrCellName, VID);
	}
#endif /* DEBUG */
	/* Now check the value. */
	if (wantLocal) {
	    if (haveLocal
		&& (cellName[0] != '&' || ULstrcmp(StrCellName, WorkstationName) == 0)) return VID;
	} else {
	    if (ULstrcmp(cellName, StrCellName) == 0) return VID;
	}
    }
    return -1;	/* Didn't find a match. */
}
#else /* AFS_ENV */
{    return -1; }
#endif /* AFS_ENV */

#ifdef TESTINGONLYTESTING
main()
{
    char *BigPacket = NULL;
    int BigLen, BigMax, RC, ix;
    static char *cellNames[] = {
	"cs.cmu.edu",
	"andrew.cmu.edu",
	"ri.osf.org",
	"psy.cmu.edu",
	"athena.mit.edu",
    };

    RC = GetAndPackAllTokens(&BigPacket, &BigLen, &BigMax, 1);
    fprintf(stderr, "GetAndPackAllTokens returns %d.\n", RC);
    if (RC >= 0) {
	RC = UnpackAndSetTokens(BigPacket, BigLen, 1, 0);
	fprintf(stderr, "UnpackAndSetTokens returns %d.\n", RC);
	RC = ExtractCellID(BigPacket, BigLen, "andrew.cmu.edu", 1);
	fprintf(stderr, "ExtractCellID(\"andrew.cmu.edu\") returns %d.\n", RC);
	for (ix = 0; ix < (sizeof(cellNames)/sizeof(cellNames[0])); ++ix) {
	    RC = ExtractCellID(BigPacket, BigLen, cellNames[ix], 0);
	    if (RC >= 0) fprintf(stderr, "ExtractCellID(\"%s\") returns %d.\n",
				 cellNames[ix], RC);
	}
    }
    exit(0);
}
#endif /* TESTINGONLYTESTING */
