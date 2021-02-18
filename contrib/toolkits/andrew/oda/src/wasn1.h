
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn1.h,v 1.3 89/10/26 14:13:06 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/wasn1.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	wasn1.h,v $
 * Revision 1.3  89/10/26  14:13:06  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/09/29  11:41:49  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:41:42  mss
 * Initial revision
 * 
 * Revision 1.9  89/09/11  14:41:02  mss
 * removed leading #
 * 
 * Revision 1.8  88/10/21  16:26:51  mz0k
 * added EXPLICIT_TAG
 * added wasn15.h
 * 
 * Revision 1.7  88/09/29  10:07:09  mz0k
 * added tkerr.h
 * 
 * Revision 1.6  88/09/14  17:06:19  mz0k
 * removed the "ifdef NOETESTED" from around the "AppendSequence's"
 * 
 * Revision 1.5  88/09/06  13:22:31  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.4  88/08/04  14:47:17  mz0k
 * typo error
 * 
 * Revision 1.3  88/08/04  14:10:46  mz0k
 * added copyright notice
 */


#ifndef TK_INCLUDED_WASN1
#define	TK_INCLUDED_WASN1

#ifdef TK_TKI_WASN1
#define TK_WASN1
#else
#ifdef TK_IMPL_WASN1
#define	TK_WASN1
#else
#define AP_WASN1
#endif
#endif

#include <lcsite.h>
#include <tkerr.h>
#include <values.h>
#include <io.h>
#include <acd.h>

#include <asn1.h>
#include <wodif.h>

/* Everyone sees this stuff */

#define	MakeInt(bytevalue)	((INT_type) bytevalue)

#ifdef  TK_WASN1

/* Tool Kit definitions/declarations */

    /* ASN.1 Construction macros */
#define MakeId_Class(class)	((INT_type)(((class) << 6) & 0xC0))
#define MakeId_PC(pc)		((INT_type)(((pc) << 5) & 0x20))
#define MakeId_Tag(tag)		((INT_type)((tag) & 0x1F))
#define MakeId(class, pc, tag)	((BYTE_type) \
				    (MakeId_Class(class) | MakeId_PC(pc) | MakeId_Tag(tag)))
#define MakeTag(class, pc, tag) ((INT_type)((class << 6) | (pc << 5) | tag))

#endif

/*
    Put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_WASN1

/* The tag passed in to indicate that the parent tag should not be inheritated */
#define	EXPLICIT_TAG		((INT_type) -1)

#endif

/* 
    include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter

*/

#include <wasn11.h>
#include <wasn12.h>
#include <wasn13.h>
#include <wasn14.h>
#include <wasn15.h>

    /* Special hacking for determining what routine to call for outputting
       a negative integer.
    */
#ifdef ONES_COMPLEMENT
#define NegativeInteger	    OnesComplement
#else
#define NegativeInteger	    TwosComplement
#endif

#endif
