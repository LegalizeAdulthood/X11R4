
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr21.h,v 1.2 89/11/01 15:47:33 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr21.h,v $

(c) Copyright 1988 Carnegie Mellon University

    See "The CMU ODA Tool Kit" (available as CMU/ITC tech report
    CMU-ITC-071) for full text & permitted uses.

$Log:	attr21.h,v $
 * Revision 1.2  89/11/01  15:47:33  mss
 * Changed some casting and declarations for passing lint
 * 
 * Revision 1.1  89/09/21  06:20:25  mss
 * Initial revision
 * 
 * Revision 1.8  89/09/11  14:35:09  mss
 * removed leading #
 * 
 * Revision 1.7  89/08/29  13:22:45  annm
 * added real names for too long names
 * 
 * Revision 1.6  89/04/21  18:15:52  annm
 * chnages for deleteseq/deletesequencedeeply
 * 
 * Revision 1.5  89/03/16  15:28:21  annm
 * changes for SEQUENCE_REVISION_HISTORY_type and at_SAME_LAYOUT_OBJECT_type
 * 
 * Revision 1.4  89/02/01  18:01:10  annm
 * added canonical procedures for SEQUENCE_RESOURCE_MAP_type
 * 
 * Revision 1.3  89/02/01  14:57:10  annm
 * added #ifndef USE_LONG_NAMES
 * around real name #define's
 * 
 * Revision 1.2  89/01/31  19:05:40  annm
 * added the canonical procedures for SEQUENCE_COPYRIGHT_INFO
 * 
 * Revision 1.1  89/01/30  13:14:23  annm
 * Initial revision
 * 
*/

#ifndef TK_INCLUDED_ATTR21
#define TK_INCLUDED_ATTR21

#undef EXTERN

#ifdef TK_GVARS_ATTR21
#define EXTERN
#else
#define EXTERN extern
#endif

/* everyone sees the procedure declarations */

#ifdef TK_IMPL_ATTR

#ifndef USE_LONG_NAMES

#define ATTR_AllocNameOrg   gatano
#define ATTR_CopyNameOrg    gatcno
#define ATTR_FreeNameOrg    gatfno

#define ATTR_AllocPersonalName	gatapn
#define ATTR_CopyPersonalName	gatcpn
#define ATTR_FreePersonalName	gatfpn

#define ATTR_CopyCopyRight	gatccr
#define ATTR_AllocCopyright	gatacr
#define ATTR_FreeCopyrightInfo	gatfcp

#define ATTR_CopyResourceMap	gatcrm
#define ATTR_AllocResourceMap	gatarm
#define ATTR_FreeResourceMap	gatfrm

#endif

EXTERN INT_type SEQUENCE_NAME_ORG_Copy();
EXTERN INT_type ATTR_CopyNameOrg();
EXTERN INT_type ATTR_FreeNameOrg();
EXTERN INT_type ATTR_CopyPersonalName();
EXTERN POINTER_type SEQUENCE_NAME_ORG_Alloc();
EXTERN INT_type ATTR_AllocNameOrg();
EXTERN INT_type ATTR_AllocPersonalName();
EXTERN INT_type SEQUENCE_NAME_ORG_Free();
EXTERN INT_type ATTR_FreePersonalName();
EXTERN INT_type SEQUENCE_NAME_ORG_Set();
EXTERN POINTER_type SEQUENCE_NAME_ORG_Get();
EXTERN INT_type SEQUENCE_NAME_ORG_CkVal();
EXTERN INT_type SEQUENCE_NAME_ORG_Deflt();

EXTERN INT_type SEQUENCE_COPYRIGHT_INFO_Copy();
EXTERN INT_type ATTR_CopyCopyRight();
EXTERN POINTER_type SEQUENCE_COPYRIGHT_INFO_Alloc();
EXTERN INT_type ATTR_AllocCopyright();
EXTERN INT_type SEQUENCE_COPYRIGHT_INFO_Free();
EXTERN INT_type SEQUENCE_COPYRIGHT_INFO_Set();
EXTERN POINTER_type SEQUENCE_COPYRIGHT_INFO_Get();
EXTERN INT_type SEQUENCE_COPYRIGHT_INFO_CkVal();
EXTERN INT_type SEQUENCE_COPYRIGHT_INFO_Deflt();

EXTERN INT_type ATTR_FreeCopyrightInfo();

EXTERN INT_type SEQUENCE_RESOURCE_MAP_Copy();
EXTERN INT_type ATTR_CopyResourceMap();
EXTERN POINTER_type SEQUENCE_RESOURCE_MAP_Alloc();
EXTERN INT_type ATTR_AllocResourceMap();
EXTERN INT_type SEQUENCE_RESOURCE_MAP_Free();
EXTERN INT_type ATTR_FreeResourceMap();
EXTERN INT_type SEQUENCE_RESOURCE_MAP_Set();
EXTERN POINTER_type SEQUENCE_RESOURCE_MAP_Get();
EXTERN INT_type SEQUENCE_RESOURCE_MAP_CkVal();
EXTERN INT_type SEQUENCE_RESOURCE_MAP_Deflt();

#endif

#endif
