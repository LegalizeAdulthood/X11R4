
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/init1.h,v 1.2 89/10/26 14:10:16 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/init1.h,v $

		(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	init1.h,v $
 * Revision 1.2  89/10/26  14:10:16  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:31:10  mss
 * Initial revision
 * 
 * Revision 1.23  89/09/01  15:48:42  annm
 * fixing USE_LONG_NAMES problems (there is a parameter called InitParms
  * used in tkerr and wodif which would conflict with the global procedure name
  * if left unmapped, or so I think-mss, 89/10/26)
 * 
 * Revision 1.22  89/08/29  16:00:34  annm
 * added real names for too long names
 * 
 * Revision 1.21  88/09/14  17:04:16  mz0k
 * LookupObjectId->LookUpObjectId
 * 
 * Revision 1.20  88/09/08  10:07:22  jr
 * Define ERROR_id
 * 
 * Revision 1.19  88/09/06  13:20:39  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.18  88/09/06  12:04:54  annm
 * 
 * no change
 * 
 * Revision 1.17  88/08/31  13:19:46  jr
 * Remove dupl defn for LookupObjectId
 * 
 * Revision 1.16  88/08/31  12:27:25  jr
 * Add LookupObjectId
 * 
 * Revision 1.15  88/08/15  18:18:11  annm
 * no change
 * 
 * Revision 1.14  88/08/01  11:18:21  jr
 * INT_TYPE -> INT_type
 * 
 * Revision 1.13  88/07/28  14:03:57  jr
 * Add defn for NULL_FILTER_ROUTINE
 * 
 * Revision 1.12  88/07/15  15:37:51  mss
 * Added include of tags.h
 * 
 * Revision 1.11  88/07/08  13:37:09  jr
 * Init ASN1 Object ID's
 * 
 * Revision 1.10  88/06/08  16:56:50  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.9  88/06/08  14:35:57  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_INIT1
#define TK_INCLUDED_INIT1

#undef EXTERN

#ifdef TK_GVARS_INIT1
#define EXTERN
#else
#define EXTERN extern
#endif

/*
    Declarations seen by everyone go here.
*/

#include <apityp.h>
#include <tags.h>

#define NULL_FILTER_ROUTINE	((INT_type (*)()) 0)

    /* For use in LookupObjectId */
#define ERROR_id	    ((VALUE_type) 1)
#define FORM_CC_id	    ((VALUE_type) 2)
#define PROC_CC_id	    ((VALUE_type) 3)
#define FP_CC_id	    ((VALUE_type) 4)
#define FORM_RG_id	    ((VALUE_type) 5)
#define FP_RG_id	    ((VALUE_type) 6)
#define FORM_GG_id	    ((VALUE_type) 7)
#define CC_CODING_id	    ((VALUE_type) 8)
#define RG_T6_CODING_id	    ((VALUE_type) 9)
#define RG_T4_1_CODING_id   ((VALUE_type) 10)
#define RG_T4_2_CODING_id   ((VALUE_type) 11)
#define RG_BITMAP_CODING_id ((VALUE_type) 12)
#define GG_CODING_id	    ((VALUE_type) 13)
#define UNKNOWN_id	    ((VALUE_type) 14)

#ifndef USE_LONG_NAMES

#define at_CONTENT_ARCH_CLASS_form_cc	cacfc
#define at_CONTENT_ARCH_CLASS_proc_cc	cacpc
#define at_CONTENT_ARCH_CLASS_fp_cc	cacfpc
#define at_CONTENT_ARCH_CLASS_form_rg	cacfr
#define at_CONTENT_ARCH_CLASS_fp_rg	cacfpr
#define at_CONTENT_ARCH_CLASS_form_gg	cacfg

#define at_TYPE_OF_CODING_cc		toccc
#define at_TYPE_OF_CODING_T6		toct6
#define at_TYPE_OF_CODING_T4_1		toct41
#define at_TYPE_OF_CODING_T4_2		toct42
#define at_TYPE_OF_CODING_bitmap	tocbit
#define at_TYPE_OF_CODING_gg		tocgg

#define InitObjectIds			ginioi
#define InitRouts			ginirt
#define object_ids			ginobj

#endif

EXTERN ASN1_OBJECT_ID_type at_CONTENT_ARCH_CLASS_form_cc;
EXTERN ASN1_OBJECT_ID_type at_CONTENT_ARCH_CLASS_proc_cc;
EXTERN ASN1_OBJECT_ID_type at_CONTENT_ARCH_CLASS_fp_cc;
EXTERN ASN1_OBJECT_ID_type at_CONTENT_ARCH_CLASS_form_rg;
EXTERN ASN1_OBJECT_ID_type at_CONTENT_ARCH_CLASS_fp_rg;
EXTERN ASN1_OBJECT_ID_type at_CONTENT_ARCH_CLASS_form_gg;

EXTERN ASN1_OBJECT_ID_type at_TYPE_OF_CODING_cc;
EXTERN ASN1_OBJECT_ID_type at_TYPE_OF_CODING_T6;
EXTERN ASN1_OBJECT_ID_type at_TYPE_OF_CODING_T4_1;
EXTERN ASN1_OBJECT_ID_type at_TYPE_OF_CODING_T4_2;
EXTERN ASN1_OBJECT_ID_type at_TYPE_OF_CODING_bitmap;
EXTERN ASN1_OBJECT_ID_type at_TYPE_OF_CODING_gg;

EXTERN VALUE_type LookUpObjectId();
  
/*
    Declarations seen only inside the tool kit are seen here
*/

#ifdef TK_INIT

#ifndef USE_LONG_NAMES

#define INIT_Init   ginint	
#define INIT_Final  ginfnl	

#endif

EXTERN INT_type INIT_Final();
EXTERN INT_type INIT_Init();

#endif

/*
    Declarations seen within the mfrats module go here.
*/

#ifdef TK_IMPL_INIT

#ifndef USE_LONG_NAMES

#define	InitParms  gintpm

#else

#define InitParms TheInitParms

#endif

/* location for remembering the init params */
EXTERN INIT_PARMS_type *InitParms;

#endif

#endif
