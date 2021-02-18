/* ********************************************************************** *\
 *         Copyright IBM Corporation 1988,1989 - All Rights Reserved      *
 *        For full copyright information see:'andrew/config/COPYRITE'     *
\* ********************************************************************** */
/*
		tokpak.c -- Subroutines for getting and packing
			Venus tokens into datagrams.
*/
/* $Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/tokpak.c,v 2.15 89/10/16 10:40:25 cfe Exp $ */
/* $ACIS: $ */
/* $Source: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/tokpak.c,v $ */

#ifndef lint
static char *rcsid = "$Header: /afs/.andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/overhead/util/lib/RCS/tokpak.c,v 2.15 89/10/16 10:40:25 cfe Exp $";
#endif /* lint */

#include <andyenv.h>
#include <stdio.h>
#include <andrewos.h>		/* sys/types.h sys/time.h*/
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
static int JustPack(ctok, stok, where, debug)
ClearToken *ctok;
SecretToken *stok;
register char *where;
int debug;
{
    register char *p;

#ifdef DEBUG
    if (debug) {
	register int i;

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

    /* Pack them */
    p = where;
    * (long int *) p = stok->AuthHandle;
    p += sizeof(long int);
    * (long int *) p = stok->Noise1;
    p += sizeof(long int);
    * (long int *) p = stok->ViceId;
    p += sizeof(long int);
    * (long int *) p = stok->BeginTimestamp;
    p += sizeof(long int);
    * (long int *) p = stok->Noise2;
    p += sizeof(long int);
    * (long int *) p = stok->EndTimestamp;
    p += sizeof(long int);
    * (long int *) p = stok->Noise3;
    p += sizeof(long int);
    * (long int *) p = stok->Noise4;
    p += sizeof(long int);
    * (long int *) p = htonl(ctok->AuthHandle);
    p += sizeof(long int);
    * (long int *) p = htonl(ctok->ViceId);
    p += sizeof(long int);
    * (long int *) p = htonl(ctok->BeginTimestamp);
    p += sizeof(long int);
    * (long int *) p = htonl(ctok->EndTimestamp);
    p += sizeof(long int);
    bcopy(stok->MagicString, p, sizeof(AuthMagic));
    p += sizeof(AuthMagic);
    bcopy(&stok->HandShakeKey, p, KEYSIZE);
    p += KEYSIZE;
    bcopy(&ctok->HandShakeKey, p, KEYSIZE);

#ifdef DEBUG
    if (debug) {
	register int i;

	p = where;
	fputs("Packed tokens: 0x", stderr);
	for (i=0; i<TOKENS_LEN; i++)
	    fprintf(stderr, "%02x", (unsigned char) *p++);
	fputc('\n', stderr);
    }
#endif /* DEBUG */
    return TOKENS_LEN;
}
#endif /* AFS_ENV */


int getandpacktokens(where, debug)
#ifdef AFS_ENV
register char *where;
int debug;
{
    ClearToken ctoken;
    SecretToken stoken;

    /* Get the tokens from Venus */
    U_GetLocalTokens(&ctoken, &stoken);

    return JustPack(&ctoken, &stoken, where, debug);
}
#else /* AFS_ENV */
{    return 0; }
#endif /* AFS_ENV */

int tok_AddStr(pOut, pOutL, pOutM, StrToAdd)
char **pOut; int *pOutL, *pOutM;
char *StrToAdd;
{
    char C, *S;

    S = StrToAdd;
    for (C = *S++; C != '\0'; C = *S++) {
	if ((*pOutL + 20) > *pOutM) {
	    *pOutM = 2 * (*pOutL + 20);
	    *pOut = realloc(*pOut, *pOutM);
	    if (*pOut == NULL) return 0;
	}
	(*pOut)[*pOutL] = C;
	++(*pOutL);
    }
    do {
	(*pOut)[*pOutL] = '\0';
	++(*pOutL);
    } while (((*pOutL) % sizeof(unsigned long)) != 0);
    return 1;
}

int GetAndPackAllTokens_Prim(pWhere, pWhereLen, pWhereMax, debug, PrimCell)
char **pWhere;
int *pWhereLen, *pWhereMax;
int debug; char *PrimCell;
{/* Extend *pWhere with an array of all tokens in all cells.  Maybe override definition of ``primary'' token. */
#ifdef AFS_ENV
    ClearToken ctoken;
    SecretToken stoken;
    char CellN[1+MAXCELLCHARS+1];
    int IsPrim, CellIx, NumPacked, EndTime;
#ifdef DEBUG
    int OldWhereLen;
#endif /* DEBUG */

    CheckServiceConfiguration();
    EndTime = osi_GetSecs() + (3*60);	/* Don't pack tokens that will expire in less than 3 minutes. */
    if (*pWhere == NULL) {
	*pWhereMax = TOKENS_LEN * 2;
	*pWhereLen = 0;
	*pWhere = malloc(*pWhereMax);
	if (*pWhere == NULL) return -1;
    }
#ifdef DEBUG
    OldWhereLen = *pWhereLen;
#endif /* DEBUG */
    NumPacked = 0;
    for (CellIx = 0; ; ++CellIx) {
	if (U_CellGetLocalTokens(1, CellIx, &ctoken, &stoken, &CellN[1], &IsPrim) != 0) {
	    if (errno == EDOM) break;
	    return(vdown(errno) ? -2 : -3);
	}
	if (ctoken.EndTimestamp < EndTime) continue;	/* Expired!  Don't keep it. */
	if ((*pWhereLen + TOKENS_LEN) >= *pWhereMax) {
	    *pWhereMax = 2 * (*pWhereLen + TOKENS_LEN);
	    *pWhere = realloc(*pWhere, *pWhereMax);
	    if (*pWhere == NULL) return -1;
	}
	(*pWhereLen) += JustPack(&ctoken, &stoken, &((*pWhere)[*pWhereLen]), debug);
	if (PrimCell == NULL) {
	    CellN[0] = (IsPrim ? TokIsPrimary : TokNotPrimary);
	} else {
	    CellN[0] = (ULstrcmp(PrimCell, &CellN[1]) == 0 ? TokIsPrimary : TokNotPrimary);
	}
	if (tok_AddStr(pWhere, pWhereLen, pWhereMax, CellN) == 0) return -1;
	++NumPacked;
    }
    bzero(&ctoken, sizeof ctoken);
    bzero(&stoken, sizeof stoken);
    ctoken.EndTimestamp = 0x37777777;
    ctoken.ViceId = getuid();
/* Someday make the following ``3'' into a ``0''--when the 5/2/89 tokunpak.c is released everywhere. */
    for (CellIx = 3; CellIx < 2; ++CellIx) {
	if ((*pWhereLen + TOKENS_LEN) >= *pWhereMax) {
	    *pWhereMax = 2 * (*pWhereLen + TOKENS_LEN);
	    *pWhere = realloc(*pWhere, *pWhereMax);
	    if (*pWhere == NULL) return -1;
	}
	(*pWhereLen) += JustPack(&ctoken, &stoken, &((*pWhere)[*pWhereLen]), debug);
	strcpy(&CellN[1], CellIx == 0 ? WorkstationName : ThisDomain);
	if (PrimCell == NULL) {
	    CellN[0] = TokLocalNotPrimary;
	} else {
	    CellN[0] = (ULstrcmp(PrimCell, &CellN[1]) == 0 ? TokLocalPrimary : TokLocalNotPrimary);
	}
	if (tok_AddStr(pWhere, pWhereLen, pWhereMax, CellN) == 0) return -1;
	++NumPacked;
	if (!AMS_ThisDomainAuthFromWS || ULstrcmp(WorkstationName, ThisDomain) == 0) break;
    }
#ifdef DEBUG
    if (debug) {
	register char *p;
	register int i;

	fputs("All tokens, packed: 0x", stderr);
	p = &((*pWhere)[OldWhereLen]);
	for (i = *pWhereLen - OldWhereLen; i > 0; --i) {
	    fprintf(stderr, "%02x", (unsigned char) *p++);
	}
	fputc('\n', stderr);
    }
#endif /* DEBUG */
    return NumPacked;	/* number of (AFS) tokens packed */
#else /* AFS_ENV */
    return 0;
#endif /* AFS_ENV */
}

int GetAndPackAllTokens(pWhere, pWhereLen, pWhereMax, debug)
char **pWhere;
int *pWhereLen, *pWhereMax;
int debug;
{/* Extend *pWhere with an array of all tokens in all cells. */
    return (GetAndPackAllTokens_Prim(pWhere, pWhereLen, pWhereMax, debug, NULL));
}

#ifdef TESTINGONLYTESTING
main()
{
    char *BigPacket = NULL;
    int BigLen, BigMax, RC;

    RC = GetAndPackAllTokens(&BigPacket, &BigLen, &BigMax, 1);
    fprintf(stderr, "GetAndPackAllTokens returns %d.\n", RC);
    exit(0);
}
#endif /* TESTINGONLYTESTING */
