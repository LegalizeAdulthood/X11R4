
/*
$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr.h,v 1.3 89/10/26 14:04:27 mss Exp $
$Source: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/src/RCS/attr.h,v $

(c) Copyright 1988 Carnegie Mellon University

See "The CMU ODA Tool Kit" (available as CMU/ITC tech report 
CMU-ITC-071) for full text & permitted uses.

$Log:	attr.h,v $
 * Revision 1.3  89/10/26  14:04:27  mss
 * casted constants as needed for PC compatibility
 * 
 * Revision 1.2  89/09/29  11:39:37  mss
 * Changed site.h to lcsite.h for compatibility with Andrew site files
 * 
 * Revision 1.1  89/09/21  06:15:28  mss
 * Initial revision
 * 
 * Revision 1.42  89/09/12  08:53:57  mss
 * Removed redundant include of aunion.d
 * 
 * Revision 1.41  89/09/11  14:30:42  mss
 * removed leading #
 * 
 * Revision 1.40  89/04/11  15:36:43  annm
 * I can't remember what I did here
 * 
 * Revision 1.39  89/03/16  15:27:18  annm
 * *** empty log message ***
 * 
 * Revision 1.38  89/02/23  20:30:20  annm
 * minor tweek
 * 
 * Revision 1.37  89/02/02  16:46:09  annm
 * added include of attr22.h
 * 
 * Revision 1.36  89/02/01  16:59:57  mss
 * Changes to support use of attr16-18 for initializing
 * attribute tables in attr0
 * 
 * Revision 1.35  89/01/30  13:21:14  annm
 * tweek
 * 
 * Revision 1.34  89/01/26  12:36:35  annm
 * added include of attr20.h
 * 
 * Revision 1.33  88/12/13  19:51:44  annm
 * changes for NULL mumble - now a storage location in memory NOT (mumble) 0
 * 
 * Revision 1.32  88/11/01  16:00:33  annm
 * added include of attr15.h
 * 
 * Revision 1.31  88/10/14  16:08:36  annm
 * added include of attr14.h
 * 
 * Revision 1.30  88/09/30  15:39:44  annm
 * fixing VMS bugs
 * 
 * Revision 1.29  88/09/07  18:36:45  annm
 * support for get/set attr for at_DIMENSIONS_type 
 * 
 * Revision 1.28  88/09/06  13:17:33  annm
 * changed include ""'s to <>'s
 * 
 * Revision 1.27  88/09/06  12:03:36  annm
 * 
 * no change
 * 
 * Revision 1.26  88/08/22  19:42:16  annm
 * added include of attr10.h
 * 
 * Revision 1.25  88/08/16  18:11:29  annm
 * added include of attr9.h
 * 
 * Revision 1.24  88/08/15  18:15:48  annm
 * added include of attr8.h
 * 
 * Revision 1.23  88/08/10  15:41:16  annm
 * added inheritance from object/style, objectclass/sytle and iso 8613 defaults
  * for the currently implemented aunion.h types
 * 
 * Revision 1.22  88/08/01  18:43:14  annm
 * OH BOY WHAT A MESS - THE DASTERLY -I BUG STRIKES AGAIN
  * (AND, OH BOY, IS IT HARD TO FIND... THANK GOD FOR CC -E !!!)
 * 
 * Revision 1.21  88/07/21  16:34:17  annm
 * added include of attr5.h
 * 
 * Revision 1.20  88/07/20  19:20:21  annm
 * now includes the new attr4.h (for LAYOUT_OBJECT_type)
 * 
 * Revision 1.19  88/07/18  18:09:33  annm
 * changes associated with having GetAttr
 * use the canonical procedures for operating on types
 * 
 * Revision 1.18  88/07/15  18:34:44  annm
 * no change
 * 
 * Revision 1.17  88/07/14  17:47:10  annm
 * no change
 * 
 * Revision 1.16  88/06/17  15:01:28  annm
 * changes to Set/Get Attr to:
 * 	1) on set attr side: plug in
 * 		more attributes whose
 * 		types are already there.
 * 	2) on Get side: Get now does all
 * 	the types and attributes that 
 * 	Set does
 * 
 * Revision 1.15  88/06/08  16:52:52  annm
 * dealt with line too long in copyright notice and changed
 * #end's to #endif's
 * 
 * Revision 1.14  88/06/08  14:33:53  annm
 * added Copyright notice
 * 
*/


#ifndef	TK_INCLUDED_ATTR
#define	TK_INCLUDED_ATTR

#ifdef TK_TKI_ATTR
#define TK_ATTR
#else
#ifdef TK_IMPL_ATTR
#define	TK_ATTR
#else
#define AP_ATTR
#endif
#endif

#include <lcsite.h>
#include <errors.h>
#include <apirt.h>

#include <tkerr.h>

#include <aunion.h>

/* constants that all can see */
#define HAS_ATTR	((INT_type)1)
#define NO_ATTR		((INT_type)2)

/*
    Return values for GetContentKind
*/
#define ERROR_kind	((VALUE_type) 1)
#define FORM_CC_kind	((VALUE_type) 2)
#define PROC_CC_kind	((VALUE_type) 3)
#define FP_CC_kind	((VALUE_type) 4)
#define FORM_RG_kind	((VALUE_type) 5)
#define FP_RG_kind	((VALUE_type) 6)
#define FORM_GG_kind	((VALUE_type) 7)

#ifdef TK_IMPL_ATTR


/* data structure for an attribute */

typedef struct ATTR_attribute_struct {


    /* 
	pointer for stringing attributes together for the hash list
	    in a singly linked grounded list, i.e. non-circular.
    */
    struct ATTR_attribute_struct    *next;

    INT_type	attribute_id;

    PARM_MASK_type  parm_mask;

    /*
	AND... NOW... FOR ... THE... MONSTER UNION !!! 
    */

    ATTR_UNION_t attribute_value;    

} *ATTR_attribute_t;


#endif


#include <attr1.h>
#include <attr2.h>
#include <attr3.h>
#include <attr4.h>
#include <attr5.h>
#include <attr6.h>
#include <attr7.h>
#include <attr8.h>
#include <attr9.h>
#include <attr10.h>
#include <attr11.h>
#include <attr12.h>
#include <attr13.h>
#include <attr15.h> /* attr15.h must come before attr14.h */
#include <attr14.h>
#include <attr16.h>
#include <attr17.h>
#include <attr18.h>
#include <attr19.h>
#include <attr20.h>
#include <attr21.h>
#include <attr22.h>
#include <attr23.h>
#include <attr24.h>
#include <attr25.h>
#include <attr26.h>

#endif
