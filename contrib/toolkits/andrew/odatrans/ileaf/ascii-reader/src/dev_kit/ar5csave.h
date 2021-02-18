
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
 *  03/88    BT add support for shar data.
 */
/** realloc storage for chars/strings that may be arbitrarily long **/

#ifndef _AR4CSAVE_H
#define _AR4CSAVE_H
/**--------------------------------------------------------------------------**/
/**	These functions offer dynamically growing character strings.         **/
/**	interface:							     **/
/**	void	CsInit(long init, long grow)				     **/
/**		How much to allocate and re-allocate. This is for user       **/
/**		control; it does not need to be called.			     **/
/**	Csave_t *CsNew( )						     **/
/**		malloc a new Csave_t object.				     **/
/**	Ssave_t *SsNew( )						     **/
/**		malloc a new Ssave_t object.				     **/
/**	Csave_t CsInitEmpty()						     **/
/**	Ssave_t SsInitEmpty()						     **/
/**		these macros return a NULL-ified structure,	    	     **/
/**		useful for initializing local variables.		     **/
/**	short	CsGetC(Csave_t *csP)				    	     **/
/**		structure of where to retrieve from			     **/
/**	short	SsGetS(Ssave_t *ssP)				     	     **/
/**		structure of where to retrieve from			     **/
/**	short	CsPeekC(Csave_t *csP)				    	     **/
/**		structure of where to retrieve from			     **/
/**	short	SsPeekS(Ssave_t *ssP)				     	     **/
/**		structure of where to retrieve from			     **/
/**	short	CsSaveC(char c, Csave_t *csP)				     **/
/**		char to save, 						     **/
/**		structure of where to save				     **/
/**	short	SsSaveS(shar s, Ssave_t *ssP)				     **/
/**		shar to save, 						     **/
/**		structure of where to save				     **/
/**	short	CsSaveCFront(char c, Csave_t *csP)			     **/
/**		char to save, 						     **/
/**		structure of where to save				     **/
/**	short	SsSaveSFront(shar s, Ssave_t *ssP)			     **/
/**		shar to save, 						     **/
/**		structure of where to save				     **/
/**	short	CsSaveStr(char *cP, Csave_t *csP)			     **/
/**		string to save, etc.					     **/
/**	short	SsSaveShr(shar *sP, Ssave_t *ssP)			     **/
/**		string to save, etc.					     **/
/**	void	CsFree(Csave_t *csP)					     **/
/**		free storage and NULL-ify pointers.			     **/
/**	void	SsFree(Ssave_t *ssP)					     **/
/**		free storage and NULL-ify pointers.			     **/
/**	void	CsReset(Csave_t *csP)					     **/
/**		NULL-ify pointers                			     **/
/**	void	SsReset(Ssave_t *ssP)					     **/
/**		NULL-ify pointers                               	     **/
/**	long 	CsAmount(Csave_t *csP)				    	     **/
/**		number of objects currently contained			     **/
/**	long 	SsAmount(Ssave_t *ssP)				    	     **/
/**		number of objects (not bytes!) currently contained	     **/
/**	void 	CsAlign(Csave_t *csP, boundary, value)		    	     **/
/**		align the buffer input ptr to an address modulo boundary,    **/
/**		padding with the given value.  (for RT and LONGALIGNED sys.) **/
/**	void 	SsAlign(Ssave_t *ssP, boundary, value)		    	     **/
/**		align the buffer input ptr to an address modulo boundary,    **/
/**		padding with the given value.  (for RT and LONGALIGNED sys.) **/

/**--------------------------------------------------------------------------**/
#ifdef INTERLEAF_INCLUDE_FILES
#include "cdefs.h"
#else
#ifndef AR_DEFINES
#define AR_DEFINES
typedef short bool;
typedef short rsu;
typedef long Rsu;
typedef short shar;
typedef unsigned short ushar;
#endif
#endif
#ifndef AR_DEFINES
#define AR_DEFINES
/** typedef short bool; BT **/
typedef short rsu;
typedef long Rsu;
/** typedef short shar; BT **/
/** typedef unsigned short ushar; BT **/
#endif
/**--------------------------------------------------------------------------**/
#define CsINITALLOC (4)
#define CsGROWALLOC (60)

/**--------------------------------------------------------------------------**/
typedef struct csave_t 
{
    char *baseP, 
	 *outP, 	/** next char comes from *outP, then increment. **/
	 *inP, 		/** next char goes into *inP, then increment. **/
	 *limitP;	/** limitP points to 1 beyond usable space      **/
#if defined(BT_DEBUG)
    int	  size;
    int	  realloc_count;
#endif
} Csave_t;
/**--------------------------------------------------------------------------**/
extern	void	        CsInit();
extern	void	        CsReset();
extern	Csave_t        *CsNew();
#define CsBufStart(csP) ((csP)->outP)
#define CsBufEnd(csP)   ((csP)->inP)
#define CsInitEmpty()   (Cs_empty);	/** macro! **/
extern  Csave_t         Cs_empty;
extern  long            CsAmount();
extern  void            CsAlign();
extern  short           CsGetC();
extern	short	        CsSaveC();
extern	short	        CsSaveCFront();
extern	short	        CsSaveStr();
extern	void	        CsFree();
/**--------------------------------------------------------------------------**/
typedef struct ssave_t 
{
    shar *baseP, 
	 *outP, 	/** next shar comes from *outP, then increment. **/
	 *inP, 		/** next shar goes into *inP, then increment. **/
	 *limitP;	/** limitP points to 1 beyond usable space      **/
#if defined(BT_DEBUG)
    int	  size;
    int	  realloc_count;
#endif
} Ssave_t;
/**--------------------------------------------------------------------------**/
extern	void	        SsInit();
extern	void	        SsReset();
extern	Ssave_t        *SsNew();
#define SsBufStart(ssP) ((ssP)->outP)
#define SsBufEnd(ssP)   ((ssP)->inP)
#define SsInitEmpty()   (Ss_empty);	/** macro! **/
extern  Ssave_t         Ss_empty;
extern  long            SsAmount();
extern  void            SsAlign();
extern  short           SsGetS();
extern	short	        SsSaveS();
extern	short	        SsSaveSFront();
extern	short	        SsSaveShr();
extern	void	        SsFree();
/**--------------------------------------------------------------------------**/
#endif
