
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/vms/RCS/local.h,v 1.2 89/09/29 11:59:36 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/site/vms/RCS/local.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	local.h,v $
 * Revision 1.2  89/09/29  11:59:36  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:11:15  mss
 * Initial revision
 * 
 * Revision 1.12  89/09/06  17:20:40  annm
 * fixing problems with names
 * 
 * Revision 1.11  89/09/05  14:38:06  mss
 * Removed redundant mapping of local names;
 * Made init/final exported interfaces
 * 
 * Revision 1.10  89/09/01  07:24:04  mss
 * Fixed GVARS switch for local
 * 
 * Revision 1.9  89/08/31  18:24:40  mss
 * Fixed scoping of LCLError
 * 
 * Revision 1.8  89/08/31  17:57:53  mss
 * Chnaged to be made indpendent of tk
 * 
 * Revision 1.7  89/08/15  10:34:42  mss
 * Changed apirt.h to lclrt.h
 * 
 * Revision 1.6  88/10/05  23:40:44  mss
 * Changed "s to <>s
 * 
 * Revision 1.5  88/09/30  14:58:57  mss
 * Made undefining of EXTERN dependent on whether it had already been defined
 * 
 * Revision 1.4  88/07/29  15:29:26  mz0k
 * Changes about the Readbytes and ReadChars
 * 
 * 
 * Revision 1.3  88/06/08  17:26:30  annm
 * no change
 * 
 * Revision 1.2  88/06/08  15:54:47  annm
 * added copyright notice
 * 
*/

#ifndef TK_INCLUDED_LOCAL
#define	TK_INCLUDED_LOCAL

#ifdef TK_TKI_LOCAL
#define TK_LOCAL
#else
#ifdef TK_IMPL_LOCAL
#define	TK_LOCAL
#else
#define AP_LOCAL
#endif
#endif

#include <lcsite.h>


/* 
    NOTE: If called with -DTK_GVARS_<sourcefilename> then must also call with
    -DTK_IMPL_LOCAL.

*/

#ifdef EXTERN
#undef EXTERN
#endif

#ifdef TK_GVARS_LOCAL
#define EXTERN
#else
#define EXTERN extern
#endif

/* Everyone sees this stuff */

/* Name remapping */

#ifndef USE_LONG_NAMES

#define LOCAL_Bcopy	lclbcp
#define LOCAL_Final	lclfin
#define LOCAL_Free	lclfre
#define LOCAL_Init	lclini
#define LOCAL_Malloc	lclmal
#define LOCAL_ReadBytes	lrdbyt
#define LOCAL_ReadChars	lrdchr
#define LOCAL_Realloc	lclrlc
#define LOCAL_SwapIn	lclswi
#define LOCAL_SwapOut	lclswo
#define LOCAL_WriteBytes	lwrbyt
#define LOCAL_WriteChars	lwrchr
#define	LCLError	lclerr

#endif

/* API Interfaces */

EXTERN	INT_type	LOCAL_Bcopy();
EXTERN	INT_type	LOCAL_Free();
EXTERN	POINTER_type	LOCAL_Malloc();
EXTERN	INT_type	LOCAL_ReadBytes();
EXTERN	INT_type	LOCAL_ReadChars();
EXTERN	POINTER_type	LOCAL_Realloc();
EXTERN	INT_type	LOCAL_WriteBytes();
EXTERN	INT_type	LOCAL_WriteChars();

EXTERN INT_type		LOCAL_Final();
EXTERN INT_type		LOCAL_Init();

#ifdef SWAPPING
EXTERN SWAP_INFO_type	*LOCAL_SwapOut();
EXTERN INT_type		LOCAL_SwapIn();
#endif

EXTERN INT_type		LCLError;

/* Error definitions -- 
    note for historical reasons, tied to ODA error encodings */

#define LCL_ERROR_INT	(-1)
#define LCL_SUCCESS_INT	(0)

#define LCL_BADPARM_err	(2)
#define LCL_INTERNAL_err	(5)
#define LCL_LOCAL_err	(6)
#define LCL_NOFINAL_err	(9)
#define LCL_NOMEM_err	(11)
#define LCL_UNIMPL_err	(16)

/* For reading the code's intentions, but otherwise meaningless */
#ifndef IN
#define IN
#endif
#ifndef OUT
#define OUT
#endif

/* 
    put the stuff seen inside the toolkit after the
    #ifdef TK_<filename>: i.e. TK_TKI or TK_IMPL/TK_GVARS
    sees this section.

*/

#ifdef  TK_LOCAL


#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_LOCAL
#endif

#endif
