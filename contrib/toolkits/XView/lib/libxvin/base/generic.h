/*      @(#)generic.h 20.17 89/08/17 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_generic_DEFINED
#define xview_generic_DEFINED

/*
 * Generic attributes fall into two classes:
 *	(1) Truly generic, implemented by attr_xxx.o or generic.o, use the
 * package ATTR_PKG_GENERIC, shared with attr.h.
 *	(2) Common but not truly generic, implemented by .o's spread
 * across many sub-systems, use the package ATTR_PKG_SV, shared with xview.h.
 * Many of these common attributes pertain to server properties and thus only
 * apply to objects with a window server component.
 *
 * Implementation dependent notes on generic X attributes:
 *	XV_XNAME has the format
 * "<host name>:<display number in decimal>:<xid in decimal>".
 *	XV_DEVICE_NUMBER is the XID of the underlying X object.  XV_XID is
 * provided when a piece of code wants to emphasize that the "X id" is what
 * is needed, rather than an abstract "tree link".
 * 	Most of these attributes are only supported on Drawable objects,
 * but some, like XV_XID, are supported by all objects that have direct
 * underlying X components, e.g. Fonts.
 */

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */


#include <xview/pkg_public.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */

#define XV_GENERIC_OBJECT	&xv_generic_pkg

/* XV_COPY is "magic" package xv_create checks for to distinguish
 * creation of a new object from creation of a copy of an existing object.
 */
#define XV_COPY			(Xv_pkg *)1

/*
 * Accelerator for XV_HELP 
 */
#define XV_HELP_DATA 		XV_KEY_DATA, XV_HELP

#define XV_XID			XV_DEVICE_NUMBER

#define	XV_DUMMY_WINDOW		0x77777777



/*
 * PRIVATE #defines 
 */

#define	XV_RC_SPECIAL		0x20000
#define	XV_RESET_REF_COUNT	XV_REF_COUNT, XV_RC_SPECIAL
#define	XV_INCREMENT_REF_COUNT	XV_REF_COUNT, XV_RC_SPECIAL+1
#define	XV_DECREMENT_REF_COUNT	XV_REF_COUNT, XV_RC_SPECIAL-1

#define	XV_ATTR(type, ordinal)	ATTR(ATTR_PKG_SV, type, ordinal)

/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */

/*
 * WARNING: GENERIC_ATTR shared with attr.h (which claims [0..64)) 
 */
typedef enum {
	/*
	 * PUBLIC and Generic 
	 */
	/*
	 * For "contexts": key & data (& optional destroy for data) 
	 */
	XV_KEY_DATA		= GENERIC_ATTR(ATTR_INT_PAIR,	 64),
	XV_KEY_DATA_COPY_PROC	= GENERIC_ATTR(ATTR_OPAQUE_PAIR, 65),
	XV_KEY_DATA_REMOVE	= GENERIC_ATTR(ATTR_INT,	 66), /* -S- */
	XV_KEY_DATA_REMOVE_PROC = GENERIC_ATTR(ATTR_OPAQUE_PAIR, 67),
	/*
	 * For "reference count" on shared objects, e.g. fonts & menus 
	 */
	XV_REF_COUNT		= GENERIC_ATTR(ATTR_INT,	 68),
	/*
	 * Type of object 
	 */
	XV_TYPE 		= GENERIC_ATTR(ATTR_OPAQUE,	 69), /* --G */
	XV_IS_SUBTYPE_OF 	= GENERIC_ATTR(ATTR_OPAQUE,	 70), /* --G */
	/*
	 * Miscellaneous 
	 */
	XV_LABEL		= GENERIC_ATTR(ATTR_STRING,	 71),
	XV_NAME			= GENERIC_ATTR(ATTR_STRING,	 72),
	XV_STATUS		= GENERIC_ATTR(ATTR_INT,	 73),
	XV_STATUS_PTR		= GENERIC_ATTR(ATTR_OPAQUE,	 74),
	XV_HELP			= GENERIC_ATTR(ATTR_STRING,	 80),
	XV_SHOW			= GENERIC_ATTR(ATTR_BOOLEAN,	 81),
	/*
	 * Required by package implementations, used only by xv_create 
	 */
	XV_COPY_OF		= GENERIC_ATTR(ATTR_OPAQUE,	 75), /* -S- */
	XV_END_CREATE		= GENERIC_ATTR(ATTR_NO_VALUE,	 76), /* -S- */
	/*
	 * To simplify SunView1.X compatibility 
	 */
	XV_SELF			= GENERIC_ATTR(ATTR_OPAQUE,	 77), /* --G */
	/*
	 * Managing (usually containing) object 
	 */
	XV_OWNER		= GENERIC_ATTR(ATTR_OPAQUE,	 78),
    	/*
	 * Required by package implementations, used only by xv_find 
	 */
	XV_AUTO_CREATE		= GENERIC_ATTR(ATTR_INT,	 79), /* C-- */
	/*
	 * PUBLIC but only Common 
	 */
	/*
	 * For layout 
	 */
	XV_FONT			= XV_ATTR(ATTR_OPAQUE,		 64),
	XV_MARGIN		= XV_ATTR(ATTR_INT,		 65),
	XV_LEFT_MARGIN		= XV_ATTR(ATTR_INT,		 66),
	XV_TOP_MARGIN		= XV_ATTR(ATTR_INT,		 67),
	XV_RIGHT_MARGIN		= XV_ATTR(ATTR_INT,		 68),
	XV_BOTTOM_MARGIN	= XV_ATTR(ATTR_INT,		 69),
	/*
	 * Origin is usually parent's most upper-left coord inside margins 
	 */
	XV_X			= XV_ATTR(ATTR_X,		 70),
	XV_Y			= XV_ATTR(ATTR_Y,		 71),
	XV_WIDTH		= XV_ATTR(ATTR_X,		 72),
	XV_HEIGHT		= XV_ATTR(ATTR_Y,		 73),
	XV_RECT			= XV_ATTR(ATTR_RECT_PTR,	 74),
	/*
	 * Server specific or dependent 
	 */
	XV_XNAME		= XV_ATTR(ATTR_STRING,		 96), /* C-G */
	XV_DEVICE_NUMBER	= XV_ATTR(ATTR_LONG,		 97), /* C-G */
	XV_ROOT			= XV_ATTR(ATTR_OPAQUE,		 98), /* --G */
	XV_VISUAL		= XV_ATTR(ATTR_OPAQUE,		 99), /* C-G */
	/*
	 * PRIVATE now, but ... 
	 */
	XV_DISPLAY		= XV_ATTR(ATTR_OPAQUE,		110), /* --G */
	XV_GC			= XV_ATTR(ATTR_OPAQUE,		113), /* --G */
	XV_SCREEN		= XV_ATTR(ATTR_OPAQUE,		116), /* --G */
} Xv_generic_attr;

/*
 * Generic package definition	
 */
typedef struct {
    Xv_base	parent_data;
    Xv_opaque	private_data;
} Xv_generic_struct;

typedef enum {
    XV_INIT_ARGS             = XV_ATTR(ATTR_INT_PAIR,         3),
    XV_INIT_ARGC_PTR_ARGV    = XV_ATTR(ATTR_INT_PAIR,         6),  /* -S- */
    XV_USAGE_PROC       = XV_ATTR(ATTR_FUNCTION_PTR,     9),    /* -S- */
    XV_ERROR_PROC       = XV_ATTR(ATTR_FUNCTION_PTR,    12),
} Xv_attr;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

extern Xv_pkg		xv_generic_pkg;

/*
 * PUBLIC functions 
 */

extern void             xv_init( /* attrs */ );

extern Attr_attribute   xv_unique_key();

#endif ~xview_generic_DEFINED
