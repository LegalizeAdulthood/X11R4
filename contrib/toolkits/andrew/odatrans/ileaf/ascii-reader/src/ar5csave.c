
/*
 * Copyright 1989 by Interleaf, Inc, 10 Canal Park,  Cambridge, MA. 02141
 *
 *		All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Interleaf not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  Interleaf, Inc. makes no representations 
 * about the suitability of this software for any purpose. 
 * It is provided "AS IS" without express or implied warranty.
 *
 * INTERLEAF DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL INTERLEAF BE LIABLE FOR ANY SPECIAL, 
 * INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING 
 * FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR
 * IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * 
 */

/*************************************************/
/*	Copyright 1987, Interleaf Inc.		 */
/*	10 Canal Park,  Cambridge, Mass		 */
/*See andrew/odatrans/ileaf/ascii-reader/COPYRIGHT*/
/*************************************************/

/*  History:
 *  07/87    BT	Original coding
 *  07/88    BT re-use in AR4
 */
/** realloc storage for chars/strings that may be arbitrarily long **/
/** see ar5csave.h for an interface description. **/
/**--------------------------------------------------------------------------**/
#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#include "sysstdio.h"
#else
extern char *ArSafeMalloc(), *ArSafeRealloc();
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#include <stdio.h>
#endif
#include "ar5csave.h"

#ifndef YES
#define YES (1)
#define NO  (0)
#endif

/**--------------------------------------------------------------------------**/
/**	These internal values can be changed by CsInit(). **/
static long cs_initalloc = CsINITALLOC;
static long cs_growalloc = CsGROWALLOC;
#define SsINITALLOC (CsINITALLOC)
#define SsGROWALLOC (CsGROWALLOC)
#define NSH(nshorts) (sizeof(short)*(nshorts))

/**--------------------------------------------------------------------------**/
/** Debug info.								     **/
#ifdef BT_DEBUG
static int ssmalloc_slop  = 0;
static int ssrealloc_slop = 0;
#define MSLOP (ssmalloc_slop)
#define RSLOP (ssrealloc_slop)
#else
#define MSLOP (0)
#define RSLOP (0)
#endif
/**--------------------------------------------------------------------------**/
/** These structures can be used to iitialize client copies via		     **/
/** SSInitEmpty() and CsInitEmpty().					     **/
Csave_t Cs_empty = { NULL, NULL, NULL, NULL };
Ssave_t Ss_empty = { NULL, NULL, NULL, NULL };
/**--------------------------------------------------------------------------**/
/**	Calling this function is optional; default values are used otherwise. **/
void CsInit(init, grow)
  register long init, grow;
{
    cs_initalloc = init;
    cs_growalloc = grow;
}

/**--------------------------------------------------------------------------**/
/**	This creates a new Csave_t object and returns a pointer to it.	     **/
Csave_t *CsNew()
{
  register Csave_t *result = (Csave_t *)ArSafeMalloc(sizeof(Csave_t));

    if (result == NULL) {
	ArFprintf(stderr, "CsNew() -- can't allocate storage.\n");
	exit(-1);
    }
    *result = Cs_empty;
    return( result );
}

/**--------------------------------------------------------------------------**/
/**	This creates a new Ssave_t object and returns a pointer to it.	     **/
Ssave_t *SsNew()
{
  register Ssave_t *result = (Ssave_t *)ArSafeMalloc(sizeof(Ssave_t));

    if (result == NULL) {
	ArFprintf(stderr, "SsNew() -- can't allocate storage.\n");
	exit(-1);
    }
    *result = Ss_empty;
    return( result );
}

/**--------------------------------------------------------------------------**/
/**	This returns the storage and resets the pointers. **/
void CsFree(csP)
  register Csave_t *csP;
{
    if (csP && csP->baseP) {
	free(csP->baseP);
	*csP = Cs_empty;
    }
}

/**--------------------------------------------------------------------------**/
/**	This returns the storage and resets the pointers. **/
void SsFree(ssP)
  register Ssave_t *ssP;
{
    if (ssP && ssP->baseP) {
	free(ssP->baseP);
	*ssP = Ss_empty;
    }
}

/**--------------------------------------------------------------------------**/
/**	This resets the pointers. **/
void CsReset(csP)
  register Csave_t *csP;
{
    if (csP) {
	csP->outP = csP->inP = csP->baseP;
    }
}

/**--------------------------------------------------------------------------**/
/**	This resets the pointers. **/
void SsReset(ssP)
  register Ssave_t *ssP;
{
    if (ssP) {
	ssP->outP = ssP->inP = ssP->baseP;
    }
}

/**--------------------------------------------------------------------------**/
/**	This returns the number of objects presently in the storage.	     **/
long CsAmount(csP)
  register Csave_t *csP;
{
    if (csP->baseP == NULL || csP->outP >= csP->inP) {
	return( 0 );
    }
    else {
	/** it might be safe enough to just return this value. **/
	return( csP->inP - csP->outP );
    }
}

/**--------------------------------------------------------------------------**/
/**	This returns the number of objects presently in the storage.	     **/
long SsAmount(ssP)
  register Ssave_t *ssP;
{
    if (ssP->baseP == NULL || ssP->outP >= ssP->inP) {
	return( 0 );
    }
    else {
	return( ssP->inP - ssP->outP );
    }
}

/**--------------------------------------------------------------------------**/
/**	This aligns the input ptr to a certain alignment, for placing	     **/
/**	structures into the buffer memory at a legal alignment. 	     **/
/**	not yet used; present to preserve symmetry.			     **/
/**	(RT & LONGALIGNED)						     **/
void CsAlign(csP, boundary, value)
  register Csave_t *csP;
  int boundary;
  shar value;
{
    if (csP->baseP != NULL) {
	while (((long)csP->inP % boundary) != 0) {
	    CsSaveC(value, csP);
	}
    }
}

/**--------------------------------------------------------------------------**/
/**	This aligns the input ptr to a certain alignment, for placing	     **/
/**	structures into the buffer memory at a legal alignment. 	     **/
/**	Basically, it is used to put one padding short, getting 4-byte align.**/
/**	(RT & LONGALIGNED)						     **/
void SsAlign(ssP, boundary, value)
  register Ssave_t *ssP;
  int boundary;
  shar value;
{
    if (ssP->baseP != NULL) {
	while (((long)ssP->inP % boundary) != 0) {
	    SsSaveS(value, ssP);
	}
    }
}

/**--------------------------------------------------------------------------**/
/**	This retrieves a single byte character. EOF in none present.	     **/
short CsGetC(csP)
  register Csave_t *csP;
{
    if (csP->baseP == NULL || csP->outP >= csP->inP) {
	return( EOF );
    }
    else {
	return( *csP->outP++ );
    }
}

/**--------------------------------------------------------------------------**/
/**	This retrieves a single short character. EOF in none present.	     **/
short SsGetS(ssP)
  register Ssave_t *ssP;
{
    if (ssP->baseP == NULL || ssP->outP >= ssP->inP) {
	return( EOF );
    }
    else {
	return( *ssP->outP++ );
    }
}

/**--------------------------------------------------------------------------**/
/**	This peeks at a single byte character. EOF in none present.	     **/
/**	No change to the actual state of storage.			     **/
short CsPeekC(csP)
  register Csave_t *csP;
{
    if (csP->baseP == NULL || csP->outP >= csP->inP) {
	return( EOF );
    }
    else {
	return( *csP->outP );
    }
}

/**--------------------------------------------------------------------------**/
/**	This peeks at a single short character. EOF in none present.	     **/
/**	No change to the actual state of storage.			     **/
short SsPeekS(ssP)
  register Ssave_t *ssP;
{
    if (ssP->baseP == NULL || ssP->outP >= ssP->inP) {
	return( EOF );
    }
    else {
	return( *ssP->outP );
    }
}

/**--------------------------------------------------------------------------**/
/**	This saves a single byte character.				     **/
short CsSaveC(c, csP)
  register char c;
  register Csave_t *csP;
{
    if (csP->baseP == NULL) {
	if ((csP->baseP = (char *)ArSafeMalloc(MSLOP+CsINITALLOC)) == NULL) {
	    ArFprintf(stderr, "CsSaveC can't allocate space.\n");
	    return( NO );
	}
	else {
	    csP->outP   = csP->baseP;
	    csP->inP    = csP->baseP;
	    csP->limitP = csP->baseP + CsINITALLOC;
	}
    }
    else if (csP->inP == csP->limitP) {
	char *tempP = (char *)ArSafeRealloc(csP->baseP, 
			RSLOP+(csP->limitP - csP->baseP) + CsGROWALLOC);
	if (tempP == NULL ) {
	    ArFprintf(stderr, "CsSaveC can't re-allocate space.\n");
	    return( NO );
	}
#       if defined(BT_DEBUG)
	else {
	    ArFprintf(stderr, "CsSaveC() growing %d --> %d\n",
		(csP->limitP - csP->baseP),
		(csP->limitP - csP->baseP) + CsGROWALLOC);
	}
#       endif
	csP->outP    = tempP + (csP->outP   - csP->baseP);
	csP->inP     = tempP + (csP->inP    - csP->baseP);
	csP->limitP  = tempP + (csP->limitP - csP->baseP);
        csP->limitP += CsGROWALLOC;
	csP->baseP   = tempP;
    }
    *csP->inP++ = c;
    return( YES );
}

/**--------------------------------------------------------------------------**/
/**	This saves a single shar character.				     **/
short SsSaveS(s, ssP)
  register shar s;
  register Ssave_t *ssP;
{
  extern short ld_error;

#       if defined(BT_DEBUG)
	ArFprintf(stderr, "SsSaveS(%#C, %#x)\n", s, ssP);
#       endif	/** BT **/
    if (ssP->baseP == NULL) {
	if ((ssP->baseP=(shar *)ArSafeMalloc(NSH(MSLOP+SsINITALLOC))) == NULL) {
	    ArFprintf(stderr, "SsSaveS can't allocate space.\n");
	    return( NO );
	}
	else {
	    ssP->outP   = ssP->baseP;
	    ssP->inP    = ssP->baseP;
	    ssP->limitP = ssP->baseP + SsINITALLOC;
#           if defined(BT_DEBUG)	/** BT **/
	    ArFprintf(stderr, "SsSaveS(%#C, %#x) ArSafeMalloc(%d)\n", 
		s, ssP, NSH(MSLOP+SsINITALLOC));
	    show_Ssave(ssP);
#           endif	/** BT **/
	}
    }
    else if (ssP->inP == ssP->limitP) {
	shar *tempP;
	long new_size;
	new_size = NSH(RSLOP+(ssP->limitP - ssP->baseP)+SsGROWALLOC);
	tempP = (shar *)ArSafeRealloc(ssP->baseP, new_size);
	if (tempP == NULL ) {
	    ArFprintf(stderr, "SsSaveS can't re-allocate space.\n");
	    ld_error = YES;
	    /** exit( -1 );	BT **/
	    return( NO );
	}
#        if defined(BT_DEBUG)	/** BT **/
	else {
	    ArFprintf(stderr, 
		"SsSaveS(%#C, %#x) base %#x, new base %#x, new size %d\n", 
		s, ssP, ssP->baseP, tempP, new_size);
	    show_Ssave(ssP);
	}
#       endif
	ssP->outP    = tempP + (ssP->outP   - ssP->baseP);
	ssP->inP     = tempP + (ssP->inP    - ssP->baseP);
	ssP->limitP  = tempP + (ssP->limitP - ssP->baseP);
        ssP->limitP += SsGROWALLOC;
	ssP->baseP   = tempP;
    }
    *ssP->inP++ = s;
    return( YES );
}

/**--------------------------------------------------------------------------**/
/**	This saves a single byte character at the front of the storage.	     **/
short CsSaveCFront(c, csP)
  register char c;
  register Csave_t *csP;
{
    if (csP->baseP == NULL) {
	if ((csP->baseP = (char *)ArSafeMalloc(MSLOP+CsINITALLOC)) == NULL) {
	    ArFprintf(stderr, "CsSaveCFront can't allocate space.\n");
	    return( NO );
	}
	else {
	    csP->outP   = csP->baseP;
	    csP->inP    = csP->baseP;
	    csP->limitP = csP->baseP + CsINITALLOC;
	}
    }
    if (csP->outP == csP->inP) {
	/** it is empty.  add it in the usual way. **/
	return( CsSaveC(c, csP) );
    }
    else
    if (csP->outP < csP->baseP) {
	/** can't happen. **/
	ArFprintf(stderr, "CsSaveCFront: outP < baseP!!!\n");
	return( NO );
    }
    else
    if (csP->outP == csP->baseP) {
	/** no room at the front.check for room, shift, then store at front. **/
	if (csP->inP > csP->limitP) {
	    /** can't happen. **/
	    ArFprintf(stderr, "CsSaveCFront: inP > limitP!!!\n");
	    return( NO );
	}
	else if (csP->inP == csP->limitP) {
	    char *tempP = (char *)ArSafeRealloc(csP->baseP, 
			RSLOP+(csP->limitP - csP->baseP) + CsGROWALLOC);
	    if (tempP == NULL ) {
		ArFprintf(stderr, "CsSaveCFront can't re-allocate space.\n");
		return( NO );
	    }
#           if defined(BT_DEBUG)
	    else {
		ArFprintf(stderr, "CsSaveCFront() growing %d --> %d\n",
		    (csP->limitP - csP->baseP),
		    (csP->limitP - csP->baseP) + CsGROWALLOC);
	    }
#           endif
	    csP->outP    = tempP + (csP->outP   - csP->baseP);
	    csP->inP     = tempP + (csP->inP    - csP->baseP);
	    csP->limitP  = tempP + (csP->limitP - csP->baseP);
	    csP->limitP += CsGROWALLOC;
	    csP->baseP   = tempP;
	}
	/** now there is room to shift by one. note direction. **/
	{ register char *srcP, *dstP; register int i;
	    dstP = csP->inP;
	    srcP = csP->inP-1;
	    i    = csP->inP - csP->baseP;
	    for ( ; i--; ) { *dstP-- = *srcP--; }
	    csP->inP++;
	}
    }
    else {
	/** there is room at the front. adjust pointer. **/
	--csP->outP;
    }
    *csP->outP = c;
    return( YES );
}

/**--------------------------------------------------------------------------**/
/**	This saves a single shar character at the front of the storage.	     **/
short SsSaveSFront(s, ssP)
  register shar s;
  register Ssave_t *ssP;
{
#   if defined(BT_DEBUG)
    if (SsAmount(ssP) > 0) {
	ArFprintf(stderr, "SsSaveSFront(%#C, %#x) amt %d\n", 
	    s, ssP, SsAmount(ssP));
    }
#   endif
    if (ssP->baseP == NULL) {
	if ((ssP->baseP=(shar *)ArSafeMalloc(NSH(MSLOP+SsINITALLOC))) == NULL){
	    ArFprintf(stderr, "SsSaveSFront can't allocate space.\n");
	    return( NO );
	}
	else {
	    ssP->outP   = ssP->baseP;
	    ssP->inP    = ssP->baseP;
	    ssP->limitP = ssP->baseP + SsINITALLOC;
	}
    }
    if (ssP->outP == ssP->inP) {
	/** it is empty.  add it in the usual way. **/
#       if defined(BT_DEBUG)
	if (SsAmount(ssP) > 0) {
	    ArFprintf(stderr, "SsSaveSFront(%#C, %#x) using SsSaveS()\n");
	}
#       endif
	return( SsSaveS(s, ssP) );
    }
    else
    if (ssP->outP < ssP->baseP) {
	/** can't happen. **/
	ArFprintf(stderr, "SsSaveSFront: outP < baseP!!!\n");
	return( NO );
    }
    else
    if (ssP->outP == ssP->baseP) {
	/** no room at the front. check for room, shift, then store at front.**/
	if (ssP->inP > ssP->limitP) {
	    /** can't happen. **/
	    ArFprintf(stderr, "SsSaveSFront: inP > limitP!!!\n");
	    return( NO );
	}
	else if (ssP->inP == ssP->limitP) {
	    shar *tempP = (shar *)ArSafeRealloc(ssP->baseP, 
			   NSH(RSLOP+(ssP->limitP - ssP->baseP) + SsGROWALLOC));
	    if (tempP == NULL ) {
		ArFprintf(stderr, "SsSaveSFront can't re-allocate space.\n");
		return( NO );
	    }
#           if defined(BT_DEBUG)
	    else {
		ArFprintf(stderr, 
		 "SsSaveSFront(%#C, %#x) base %#x, new base %#x, new size %d\n",
		 s, ssP, ssP->baseP, tempP, 
		 NSH(RSLOP+(ssP->limitP - ssP->baseP) + SsGROWALLOC) );
		show_Ssave(ssP);
	    }
#           endif
	    ssP->outP    = tempP + (ssP->outP   - ssP->baseP);
	    ssP->inP     = tempP + (ssP->inP    - ssP->baseP);
	    ssP->limitP  = tempP + (ssP->limitP - ssP->baseP);
	    ssP->limitP += SsGROWALLOC;
	    ssP->baseP   = tempP;
	}
	/** now there is room to shift by (at least) one. note direction. **/
	{ register shar *srcP, *dstP; register int i;
	    dstP = ssP->inP;
	    srcP = ssP->inP-1;
	    i    = ssP->inP - ssP->baseP;
	    for ( ; i--; ) { *dstP-- = *srcP--; }
	    ssP->inP++;
	}
    }
    else {
	/** there is room at the front. adjust pointer. **/
	--ssP->outP;
    }
    *ssP->outP = s;
    return( YES );
}

/**--------------------------------------------------------------------------**/
/**	This saves a zero-terminated string of single-byte characters.	     **/
short CsSaveStr(cP, csP)
  register char *cP;
  register Csave_t *csP;
{
  register long len = strlen(cP);

    if (csP->baseP == NULL) {
	if ((csP->baseP = (char *)ArSafeMalloc(MSLOP+CsINITALLOC)) == NULL) {
	    ArFprintf(stderr, "CsSaveStr can't allocate space.\n");
	    return( NO );
	}
	else {
	    csP->outP   = csP->baseP;
	    csP->inP    = csP->baseP;
	    csP->limitP = csP->baseP + CsINITALLOC;
	}
    }
    if (csP->inP + len >= csP->limitP) {
	char *tempP = (char *)ArSafeRealloc(csP->baseP,
			     RSLOP+(csP->inP - csP->baseP) + len + CsGROWALLOC);
	if (tempP == NULL ) {
	    ArFprintf(stderr, "CsSaveStr can't re-allocate space.\n");
	    return( NO );
	}
#        if defined(BT_DEBUG)
	else {
	    ArFprintf(stderr, "CsSaveStr() growing %d --> %d\n",
		(csP->limitP - csP->baseP),
		(csP->limitP - csP->baseP) + len + CsGROWALLOC);
	}
#       endif
	csP->limitP = tempP + (csP->limitP  - csP->baseP) 
			    + len + CsGROWALLOC;
	csP->outP   = tempP + (csP->outP - csP->baseP);
	csP->inP    = tempP + (csP->inP  - csP->baseP);
	csP->baseP  = tempP;
    }
    while (*cP) {
	*csP->inP++ = *cP++;
    }
    return( YES );
}

/**--------------------------------------------------------------------------**/
/**	This saves a zero-terminated string of shar characters.		     **/
short SsSaveShr(sP, ssP)
  register shar *sP;
  register Ssave_t *ssP;
{
  register long len = ArShrlen(sP);

#   if defined(BT_DEBUG)
    ArFprintf(stderr, "SsSaveShr(%#x, %#x) amt %d\n", sP, ssP, SsAmount(ssP));
#   endif
    if (ssP->baseP == NULL) {
	if ((ssP->baseP=(shar *)ArSafeMalloc(NSH(MSLOP+SsINITALLOC))) == NULL) {
	    ArFprintf(stderr, "SsSaveShr can't allocate space.\n");
	    return( NO );
	}
	else {
	    ssP->outP   = ssP->baseP;
	    ssP->inP    = ssP->baseP;
	    ssP->limitP = ssP->baseP + SsINITALLOC;
	}
    }
    if (ssP->inP + len >= ssP->limitP) {
	shar *tempP = (shar *)ArSafeRealloc(ssP->baseP, 
			NSH(RSLOP+(ssP->inP - ssP->baseP) + len + SsGROWALLOC));
	if (tempP == NULL ) {
	    ArFprintf(stderr, "SsSaveShr can't re-allocate space.\n");
	    return( NO );
	}
	/* note compiler scaling */
	ssP->limitP = tempP + (ssP->limitP  - ssP->baseP) + len + SsGROWALLOC;
	ssP->outP   = tempP + (ssP->outP - ssP->baseP);
	ssP->inP    = tempP + (ssP->inP  - ssP->baseP);
	ssP->baseP  = tempP;
    }
    while (*sP) {
	*ssP->inP++ = *sP++;
    }
    return( YES );
}
/**--------------------------------------------------------------------------**/
/**	This saves a zero-terminated string of single-byte characters.	 MS  **/
short SsSaveChr(sP, ssP)
  register char *sP;
  register Ssave_t *ssP;
{
  register long len = strlen(sP);

#   if defined(BT_DEBUG)
    ArFprintf(stderr, "SsSaveChr(%#x, %#x) amt %d\n", sP, ssP, SsAmount(ssP));
#   endif
    if (ssP->baseP == NULL) {
	if ((ssP->baseP=(shar *)ArSafeMalloc(NSH(MSLOP+SsINITALLOC))) == NULL) {
	    ArFprintf(stderr, "SsSaveShr can't allocate space.\n");
	    return( NO );
	}
	else {
	    ssP->outP   = ssP->baseP;
	    ssP->inP    = ssP->baseP;
	    ssP->limitP = ssP->baseP + SsINITALLOC;
	}
    }
    if (ssP->inP + len >= ssP->limitP) {
	shar *tempP = (shar *)ArSafeRealloc(ssP->baseP, 
			NSH(RSLOP+(ssP->inP - ssP->baseP) + len + SsGROWALLOC));
	if (tempP == NULL ) {
	    ArFprintf(stderr, "SsSaveShr can't re-allocate space.\n");
	    return( NO );
	}
	/* note compiler scaling */
	ssP->limitP = tempP + (ssP->limitP  - ssP->baseP) + len + SsGROWALLOC;
	ssP->outP   = tempP + (ssP->outP - ssP->baseP);
	ssP->inP    = tempP + (ssP->inP  - ssP->baseP);
	ssP->baseP  = tempP;
    }
    while (*sP) {
	*ssP->inP++ = *sP++;
    }
    return( YES );
}
/**--------------------------------------------------------------------------**/
#ifdef MAINTEST
main(argc, argv, env)
  int argc;
  char **argv, **env;
{
  short c;
  Csave_t it;
  short result = YES;
  char * xxx = "\nxxxxxx == %d\t";
  static char * fmt = "\nstrlen == %d\t";

    if (argc == 3) {
	CsInit(atoi(argv[1]), atoi(argv[2]));
    }
    while ((c = getc(stdin)) != EOF) {
	if (c == '!') {
	    if ((result = CsSaveC((char)'\0', &it)) == NO) { break; }
	    printf(fmt, strlen(it.baseP));
	    printf("[%s]\n", it.baseP);
	    CsFree(&it);
	}
	else {
	    if ((result = CsSaveC(c, &it)) == NO) { break; }
	}
    }
    if (result == NO) {
	printf("result == NO!\n");
	printf("string == `%s'\n", it.baseP);
    }
    printf("That's all, folks!\n");
    exit( 0 );
}
#endif
/**--------------------------------------------------------------------------**/
#ifdef BT_DEBUG
FILE *AR_errfile = stderr;
FILE *AR_outfile = stdout;
#endif

show_Ssave(ssP)
  register Ssave_t *ssP;
{
  register shar *sP;

    ArFprintf(stderr, 
	"ssP %#x, amt %d, base %#x, out %#x, in %#x, lim %#x\n",
	ssP, SsAmount(ssP), ssP->baseP, ssP->outP, ssP->inP, ssP->limitP);
    ArFprintf(stderr, "  : ");
    for (sP = ssP->baseP; sP <= ssP->inP; sP++) {
	if (sP == ssP->outP) {
	    ArFprintf(stderr, "[");
	}
	if (sP == ssP->inP) {
	    ArFprintf(stderr, "]");
	}
	ArFprintf(stderr, "%#C", *sP);
    }
    ArFprintf(stderr, "\n");
}
/**--------------------------------------------------------------------------**/
