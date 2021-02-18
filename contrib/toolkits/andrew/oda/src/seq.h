
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/seq.h,v 1.2 89/09/29 11:41:14 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/seq.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	seq.h,v $
 * Revision 1.2  89/09/29  11:41:14  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:38:58  mss
 * Initial revision
 * 
 * Revision 1.21  89/09/11  14:38:11  mss
 * removed leading #
 * 
 * Revision 1.20  89/03/16  15:30:03  annm
 * changes for SEQUENCE_REVISION_HISTORY_type and at_SAME_LAYOUT_OBJECT_type
 * 
 * Revision 1.19  89/02/24  18:55:24  annm
 * changed REVISOR to REVISER
 * 
 * Revision 1.18  89/01/16  15:39:30  annm
 * renumbered the sequence tags to make them sequential
 * 
 * Revision 1.17  89/01/11  10:14:43  mss
 * Got rid of SEQUENCE_FILING_tag
 * 
 * Revision 1.16  88/12/13  19:56:28  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.15  88/09/06  13:22:05  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.14  88/09/06  12:06:20  annm
 * 
 * no change
 * 
 * Revision 1.13  88/07/15  15:38:05  mss
 * Added include of tags.h
 * 
 * Revision 1.12  88/07/08  17:39:59  annm
 * added new tags per the IS upgrade
 * 
 * Revision 1.11  88/07/05  16:27:29  jr
 * Remove refs to at_LAYOUT_TEXTURE
 * 
 * Revision 1.10  88/06/17  15:06:26  annm
 * wrote ContractSequence; buffed rough edges in ExpandSequence (TKError setting on can't realloc); can not copy sequences of resource maps
 * 
 * Revision 1.9  88/06/08  16:58:17  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.8  88/06/08  14:37:23  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_SEQ
#define	TK_INCLUDED_SEQ

#ifdef TK_TKI_SEQ
#define TK_SEQ
#else
#ifdef TK_IMPL_SEQ
#define	TK_SEQ
#else
#define AP_SEQ
#endif
#endif

#include <lcsite.h>

#include <errors.h>
#include <apirt.h>
#include <apityp.h>
#include <tags.h>

#include <tkerr.h>

/*******************************
* Everyone sees the stuff here *
********************************/

/* Tag values for various SEQUENCE element types */
#define SEQUENCE_ASN1_OBJECT_ID_tag	((TAG_type) 1)
#define SEQUENCE_ATOMIC_STR_EXPR_tag	((TAG_type) 2)
#define SEQUENCE_BINDING_PAIR_tag	((TAG_type) 3)
#define SEQUENCE_BYTE_tag		((TAG_type) 4)
#define SEQUENCE_CHAR_tag		((TAG_type) 5)
#define SEQUENCE_CONSTITUENT_tag	((TAG_type) 6)
#define SEQUENCE_CONSTRUCTION_TERM_tag	((TAG_type) 7)
#define SEQUENCE_COPYRIGHT_INFO_tag	((TAG_type) 8)
/* SEQUENCE_FILING_tag used to be ((TAG_type) 9) */
#define SEQUENCE_FONT_PAIR_tag		((TAG_type) 9)
#define SEQUENCE_INT_tag		((TAG_type) 10)
#define SEQUENCE_INT_PAIR_tag		((TAG_type) 11)
#define SEQUENCE_ISO9541_ID_tag		((TAG_type) 12)
#define SEQUENCE_at_MEDIUM_TYPE_tag	((TAG_type) 13)
#define SEQUENCE_NAME_ORG_tag		((TAG_type) 14)
#define SEQUENCE_PERSONAL_NAME_tag	((TAG_type) 15)
#define SEQUENCE_RESOURCE_MAP_tag	((TAG_type) 16)
#define SEQUENCE_REVISION_HISTORY_tag	((TAG_type) 17)
#define SEQUENCE_REVISER_tag		((TAG_type) 18)
#define SEQUENCE_STRING_tag		((TAG_type) 19)
#define SEQUENCE_STRING_OR_ID_tag       ((TAG_type) 20)
#define SEQUENCE_TAB_STOP_tag		((TAG_type) 21)
#define SEQUENCE_VALUE_tag		((TAG_type) 22)


#ifdef  TK_SEQ

#endif

/*
    put module internal #defines, macros, typedefs after the
    #ifdef TK_IMPL_<filename>.

*/

#ifdef TK_IMPL_SEQ


#endif

/* 
    Include(s) for the subordinate .h file(s)
    N.B. the order here shouldn't matter
*/

#include <seq1.h>

#endif
