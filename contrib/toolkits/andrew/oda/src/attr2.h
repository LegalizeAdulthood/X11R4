
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr2.h,v 1.2 89/10/26 14:06:17 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr2.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr2.h,v $
 * Revision 1.2  89/10/26  14:06:17  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.1  89/09/21  06:19:44  mss
 * Initial revision
 * 
 * Revision 1.25  89/09/11  14:31:18  mss
 * removed leading #
 * 
 * Revision 1.24  88/09/30  15:40:30  annm
 * fixing VMS bugs
 * 
 * Revision 1.23  88/09/06  13:17:56  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.22  88/09/06  12:03:46  annm
 * 
 * no change
 * 
 * Revision 1.21  88/08/25  16:11:51  annm
 * added tree unthreader for delete constituent
 * 
 * Revision 1.20  88/08/16  18:12:25  annm
 * added tree threading for setattr of at_SUBORDINATES
 * 
 * Revision 1.19  88/08/10  15:44:00  annm
 * changes for inheritance
 * 
 * Revision 1.18  88/08/01  18:44:41  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.17  88/07/25  14:39:58  annm
 * changed switch in ifndef around proc dcls to USE_LONG_NAMES
 * 
 * Revision 1.16  88/07/18  18:08:01  annm
 * removed the get utility routines
 * 
 * Revision 1.15  88/07/15  18:32:02  annm
 * deleted the IsIntAttr and IsValueAttr procs
 * 
 * Revision 1.14  88/07/14  17:48:28  annm
 * changes to use canonical procedures for operating on aunion.h types for the SetAttr stuff
 * 
 * Revision 1.13  88/06/17  15:03:49  annm
 * changes to Set/Get Attr to:
 * 	1) on set attr side: plug in
 * 		more attributes whose
 * 		types are already there.
 * 	2) on Get side: Get now does all
 * 	the types and attributes that 
 * 	Set does
 * 
 * Revision 1.12  88/06/09  15:56:23  annm
 * changed interfaces to Get the value for
 * get attr subr's to returning INT_type
 * 
 * Revision 1.11  88/06/08  16:54:37  annm
 * dealt with line too long in copyright and changed the blasted #end
 * to #endif
 * 
 * Revision 1.10  88/06/08  14:34:15  annm
 * added Copyright notice
 * 
*/

#ifndef TK_INCLUDED_ATTR2
#define TK_INCLUDED_ATTR2

#undef EXTERN

#ifdef TK_GVARS_ATTR2
#define EXTERN
#else
#define EXTERN extern
#endif


#ifdef TK_IMPL_ATTR

/* real names for routines in attr2.c */

#ifndef USE_LONG_NAMES

#define ATTR_AddToConstituent	    gatatc
#define ATTR_Hash		    gathsh
#define ATTR_FindAndRemoveAttr	    gatfar
#define ATTR_FindAttr		    gfndat
#define ATTR_KindOfAttr		    gatkoa
#define	ATTR_SterilizeParmMask	    gatsmk
#define ATTR_CheckAndCleanMask	    gatccm
#define ATTR_ThreadTree		    gatttr
#define ATTR_UnThreadTree	    gatutt

#endif

/* declarations for routines in attr2.c */

EXTERN INT_type ATTR_AddToConstituent();
EXTERN INT_type ATTR_Hash();
EXTERN ATTR_attribute_t ATTR_FindAndRemoveAttr();
EXTERN ATTR_attribute_t ATTR_FindAttr();
EXTERN INT_type ATTR_KindOfAttr();
EXTERN INT_type ATTR_SterilizeParmMask();
EXTERN INT_type ATTR_CheckAndCleanMask();
EXTERN INT_type ATTR_ThreadTree();
EXTERN INT_type ATTR_UnThreadTree();

#define	ATTR_DEFAULTS_NO_PARM	    ((INT_type)1)
#define	ATTR_INDEPENDENT_PARM	    ((INT_type)2)
#define	ATTR_RELATED_PARM	    ((INT_type)3)
#define ATTR_DOESNT_DEFAULT	    ((INT_type)4)

#endif

#endif

