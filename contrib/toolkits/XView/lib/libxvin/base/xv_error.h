/*
 * @(#)xv_error.h 1.15 89/08/31 SMI
 *
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_xv_error_DEFINED
#define xview_xv_error_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/attr.h>

#ifndef sunwindow_sun_DEFINED
#include <xview/sun.h>
#endif

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PRIVATE #defines 
 */

#define XV_ERROR_ATTR(type, ordinal)      ATTR(ATTR_PKG_ERROR, type, ordinal)
#define XV_ERROR_ATTR_LIST(ltype, type, ordinal) \
		XV_ERROR_ATTR(ATTR_LIST_INLINE((ltype), (type)), (ordinal))

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

/*
 * PRIVATE enums 
 */

typedef enum {
	XV_ERROR_CONTINUE,
	XV_ERROR_RETRY,
	XV_ERROR_ABORT
}   Xv_error_action;

typedef enum {
	XV_ERROR_RECOVERABLE,
	XV_ERROR_NON_RECOVERABLE
}   Xv_error_severity;

typedef enum {
	XV_ERROR_BAD_VALUE
			= XV_ERROR_ATTR_LIST(ATTR_RECURSIVE, ATTR_AV,	 2),
	XV_ERROR_CANNOT_GET	= XV_ERROR_ATTR(ATTR_OPAQUE,		 5),
	XV_ERROR_CANNOT_SET	= XV_ERROR_ATTR(ATTR_OPAQUE,		 4),
	XV_ERROR_CREATE_ONLY	= XV_ERROR_ATTR(ATTR_OPAQUE,		 3),
	XV_ERROR_INTERNAL	= XV_ERROR_ATTR(ATTR_NO_VALUE,		 9),
	XV_ERROR_INVALID_OBJ	= XV_ERROR_ATTR(ATTR_STRING,		 8),
	XV_ERROR_PKG		= XV_ERROR_ATTR(ATTR_OPAQUE,		10),
	XV_ERROR_PKG_NAME	= XV_ERROR_ATTR(ATTR_STRING,		11),
	XV_ERROR_SERVER		= XV_ERROR_ATTR(ATTR_INT,		 6),
	XV_ERROR_STRING		= XV_ERROR_ATTR(ATTR_STRING,		 7),
	XV_ERROR_SYSTEM		= XV_ERROR_ATTR(ATTR_NO_VALUE,		 1),
} Xv_error_attr;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PRIVATE functions 
 */

extern void xv_error();

#endif ~xview_xv_error_DEFINED
