/*	@(#)scrn_vis.h 20.11 89/11/09 SMI	*/

/****************************************************************************/
/*	
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license. 
 */
/****************************************************************************/

#ifndef _xview_screen_visual_h_already_included
#define _xview_screen_visual_h_already_included

#include <xview/base.h>
#include <X11/Xlib.h>

typedef struct screen_visual {
    Xv_object	server;
    Xv_object	screen;
    Xv_object	root_window;
    short		depth;
    Display		*display;
    GC			gc;
    XImage		*image_1;
    XImage		*image_8;
    struct screen_visual *next;
} Screen_visual;

#endif
