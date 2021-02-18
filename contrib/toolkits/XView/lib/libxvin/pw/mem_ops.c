#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)mem_ops.c 1.9 89/08/18";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <xview/xv_error.h>

extern int      xv_mem_destroy();
static int      xv_pr_error();

/*
 * "Pixrect" operations vector
 */
struct pixrectops mem_ops = {
    xv_pr_error,		/* mem_rop, */
    xv_pr_error,		/* mem_stencil, */
    xv_pr_error,		/* mem_batchrop, */
    0,
    xv_mem_destroy,
    xv_pr_error,		/* mem_get, */
    xv_pr_error,		/* mem_put, */
    xv_pr_error,		/* mem_vector, */
    (Pixrect * (*) ()) xv_pr_error,	/* mem_region, */
    xv_pr_error,		/* mem_putcolormap, */
    xv_pr_error,		/* mem_getcolormap, */
    xv_pr_error,		/* mem_putattributes, */
    xv_pr_error,		/* mem_getattributes */
};


static int
xv_pr_error(pr)
    Pixrect        *pr;
{
    xv_error(pr, XV_ERROR_NON_RECOVERABLE, XV_ERROR_INTERNAL,
	     XV_ERROR_STRING, "Unsupported pixrect operation attempted",
	     0);
    /* doesn't return */
}
