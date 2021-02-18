#ifndef lint
#ifdef sccs
static char     sccsid[] = "@(#)wmgr_menu.c 20.28 89/11/20";
#endif
#endif

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * Window mgr menu handling.
 */

#include <sys/types.h>
#include <stdio.h>
#include <ctype.h>
#include <errno.h>
#ifdef sparc
#include <vfork.h>
#endif
#include <strings.h>
#include <xview/defaults.h>
#include <xview/alert.h>
#include <xview/frame.h>

#ifndef	pixrect_hs_DEFINED
#define	pixrect_hs_DEFINED
/* <pixrect/pixrect_hs.h> without frame buffer variable include files */
#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <pixrect/pr_dblbuf.h>
#include <pixrect/pr_line.h>
#include <pixrect/pr_planegroups.h>
#include <pixrect/pr_util.h>
#include <pixrect/traprop.h>
#include <pixrect/memvar.h>
#include <pixrect/pixfont.h>
#include <rasterfile.h>
#include <pixrect/pr_io.h>
#endif	pixrect_hs_DEFINED

#include <xview/window_hs.h>
#include <xview/openmenu.h>
#include <xview/wmgr.h>
#include <xview/frame.h>
#include <xview/icon.h>
#include <X11/Xlib.h>
#include <xview_private/draw_impl.h>
#include <xview_private/fm_impl.h>


extern int      (*win_errorhandler()) ();
extern void     wmgr_changerect(), wmgr_refreshwindow();
extern int      errno;

static void     wmgr_top_bottom();


void
wmgr_open(frame_public)
    Frame           frame_public;
{
    Xv_Drawable_info *info;
    Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);

    DRAWABLE_INFO_MACRO(frame_public, info);
    frame->wmhints.initial_state = NormalState;
    frame->wmhints.flags |= StateHint;
    frame_setwmhints(xv_display(info), xv_xid(info), &(frame->wmhints));
}

void
wmgr_close(frame_public)
    Frame           frame_public;
{
    Frame_class_info *frame = FRAME_CLASS_PRIVATE(frame_public);
    Xv_Drawable_info *info;

    DRAWABLE_INFO_MACRO(frame_public, info);
    frame->wmhints.initial_state = IconicState;
    frame->wmhints.flags |= StateHint;
    frame_setwmhints(xv_display(info), xv_xid(info), &(frame->wmhints));
}

void
wmgr_top(frame)
    Frame           frame;
{
    wmgr_top_bottom(frame, WL_COVERED);
}

void
wmgr_bottom(frame)
    Frame           frame;
{
    wmgr_top_bottom(frame, WL_COVERING);
}

static void
wmgr_top_bottom(frame, link)
    Frame           frame;
    int             link;
{
    Xv_Window       window;
    /*
     * we always get passed the frame.  If frame is currently open, then
     * bring frame to top; otherwise, it is the frame's icon that is getting
     * the top request
     */
    if (!frame_is_iconic(FRAME_CLASS_PRIVATE(frame))) {
	window = frame;
    } else {
	Icon            icon = xv_get(frame, FRAME_ICON);

	if (icon)
	    window = icon;
	else
	    return;
    }
    win_setlink(window, link, 0 /* None */ );
}


#define ARGS_MAX        100

wmgr_forktool(programname, otherargs, rectnormal, recticon, iconic)
    char           *programname, *otherargs;
    struct rect    *rectnormal, *recticon;
    int             iconic;
{
    int             pid;
    char           *args[ARGS_MAX];
    char           *otherargs_copy;
    extern char    *calloc();

    (void) we_setinitdata(rectnormal, recticon, iconic);
    /*
     * Copy otherargs because using vfork and don't want to modify otherargs
     * that is passed in.
     */
    if (otherargs) {
        otherargs_copy = calloc(1, (unsigned) strlen(otherargs) + 1);
        if (otherargs_copy == NULL) {
            perror("calloc");
            return (-1);
        }
        (void) strcpy(otherargs_copy, otherargs);
    } else {
        otherargs_copy = NULL;
    }
    pid = vfork();
    if (pid < 0) {
        perror("fork");
        return (-1);
    }
    if (pid) {
        if (otherargs)
            free(otherargs_copy);
        return (pid);
    }
    /* 
     * Could nice(2) here so that window manager has higher priority but this
     * also has the affect of making some of the deamons higher priority.
     * This can be a problem because when they startup they preempt the user.
     */
    /*
     * Separate otherargs into args
     */
    (void) wmgr_constructargs(args, programname, otherargs_copy, ARGS_MAX);
    execvp(programname, args);
    perror(programname);
    _exit(1);
    /* NOTREACHED */
}

int
wmgr_constructargs(args, programname, otherargs, maxargcount)
    char           *args[], *programname, *otherargs;
    int             maxargcount;
{    
#define terminatearg() {*cpt = NULL;needargstart = 1;}
#define STRINGQUOTE     '"'
    int             argindex = 0, needargstart = 1, quotedstring = 0;
    register char  *cpt;
     
    args[argindex++] = programname;
    for (cpt = otherargs; (cpt != 0) && (*cpt != NULL); cpt++) {
        if (quotedstring) {
            if (*cpt == STRINGQUOTE) {
                terminatearg();
                quotedstring = 0;
            } else {            /* Accept char in arg */
            }
        } else if (isspace(*cpt)) {
            terminatearg();
        } else {
            if (needargstart && (argindex < maxargcount)) {
                args[argindex++] = cpt;
                needargstart = 0;
            }
            if (*cpt == STRINGQUOTE) {
                /*
                 * Advance cpt in current arg
                 */
                args[argindex - 1] = cpt + 1;
                quotedstring = 1;
            }
	}    
    }    
    args[argindex] = '\0';
    return (argindex);
}
 
