#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fs_compat.c 20.18 89/11/21";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview/fullscreen.h>

/*
 * Backwards compatible fullscreen utilities
 */

struct fullscreen *
fullscreen_init(window)
    Xv_Window       window;
{
    struct fullscreen *fs_result;

    fs_result = (struct fullscreen *) xv_create(XV_NULL, FULLSCREEN,
						FULLSCREEN_INPUT_WINDOW, window,
						NULL);
    if (!fs_result)
	return ((struct fullscreen *)NULL);
    else
	return (fs_result);
}

fullscreen_set_cursor(fs, cursor)
    struct fullscreen *fs;
    Xv_Cursor       cursor;
{
    (void) xv_set((Xv_opaque)fs, WIN_CURSOR, cursor, NULL);
}

fullscreen_set_inputmask(fs, im)
    struct fullscreen *fs;
    Inputmask      *im;
{
    (void) xv_set((Xv_opaque)fs, WIN_INPUT_MASK, im, NULL);
}


fullscreen_destroy(fs)
    struct fullscreen *fs;
{
    (void) xv_destroy((Xv_opaque)fs);
}
