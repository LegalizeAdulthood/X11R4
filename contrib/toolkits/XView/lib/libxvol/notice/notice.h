/*      @(#)notice.h 20.12 89/08/18  */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_notice_DEFINED
#define xview_notice_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/attrol.h>

/*
 ***********************************************************************
 *			Definitions and Macros	
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */

/*
 * the following constant, NOTICE_FAILED is returned if notice_prompt() 
 *  failed for an unspecified reason.
 */
#define NOTICE_YES			 1
#define NOTICE_NO			 0
#define NOTICE_FAILED			-1
#define NOTICE_TRIGGERED		-2

/*
 * PRIVATE #defines 
 */

#define NOTICE_ATTR(type, ordinal)	ATTR(ATTR_PKG_NOTICE, type, ordinal)
#define NOTICE_ATTR_LIST(ltype, type, ordinal) \
	NOTICE_ATTR(ATTR_LIST_INLINE((ltype), (type)), (ordinal))
#define NOTICE_BUTTON_VALUE_PAIR	ATTR_INT_PAIR

#define notice_attr_next(attr) (Notice_attribute *)attr_next((caddr_t *)attr)


/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */

typedef enum {
	/*
	 * Public attributes 
	 */
	NOTICE_BUTTON		= NOTICE_ATTR(NOTICE_BUTTON_VALUE_PAIR,	 1),
	NOTICE_BUTTON_NO	= NOTICE_ATTR(ATTR_STRING,		 5),
	NOTICE_BUTTON_YES	= NOTICE_ATTR(ATTR_STRING,		10),
	NOTICE_FOCUS_XY		= NOTICE_ATTR(ATTR_XY,			15),
	NOTICE_FONT		= NOTICE_ATTR(ATTR_PIXFONT_PTR,		20),
	NOTICE_MESSAGE_STRINGS	
			= NOTICE_ATTR_LIST(ATTR_NULL, ATTR_STRING,	25),
	NOTICE_MESSAGE_STRINGS_ARRAY_PTR
			= NOTICE_ATTR(ATTR_STRING,			30),
	NOTICE_NO_BEEPING	= NOTICE_ATTR(ATTR_BOOLEAN,	 	35),
	NOTICE_TRIGGER		= NOTICE_ATTR(ATTR_INT,			40)
} Notice_attribute;


/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * Public Functions 
 */
extern int
notice_prompt(/*client_frame, event, attr_list*/);

#endif ~xview_notice_DEFINED
