#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)fmcmd_dsty.c 1.13 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <xview_private/fm_impl.h>
#include <xview_private/frame_cmd.h>

static void     frame_cmd_free();

/* Destroy the frame struct */
Pkg_private int
frame_cmd_destroy(frame_public, status)
    Frame           frame_public;
    Destroy_status  status;
{
    Frame_cmd_info *frame = FRAME_CMD_PRIVATE(frame_public);

    if (status == DESTROY_CLEANUP) {	/* waste of time if ...PROCESS_DEATH */
	xv_destroy(frame->panel);
	frame_cmd_free(frame);
    }
    if (status == DESTROY_PROCESS_DEATH)
	xv_destroy(frame->panel);

    return XV_OK;
}

/*
 * free the frame struct and all its resources.
 */
static void
frame_cmd_free(frame)
    Frame_cmd_info *frame;
{
    /* Free frame struct */
    free((char *) frame);
}
