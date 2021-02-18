/*	@(#)wmgr.h 20.13 89/10/10 SMI	*/

#ifndef xview_wmgr_DEFINED
#define xview_wmgr_DEFINED	1

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * This header file describes the interface to a window management mechanism.
 * A menu interface to these functions is also provided.
 * Typically, a tool window is responsible for window management.
 */
 
#define	WMGR_ICONIC	WUF_WMGR1	/* Indicates window is iconic
					   in user flags of window */
#define WMGR_SUBFRAME   WUF_WMGR3   /* Indicates window is a sub-frame
					   in user flags of window */

#define	WMGR_SETPOS	-1		/* Indicates "use default" in
					   wmgr_figure*rect calls	*/
/* for XGetProperty call, indicate no deleting the existing property after
 * a get
 */
#define WMGR_NO_DELETE  FALSE

typedef enum  {
	WM_None, WM_N, WM_NE, WM_E, WM_SE, WM_S, WM_SW, WM_W, WM_NW
}   WM_Direction;

/*
 * Basic window management operations.
 * Move and stretch require user interaction.
 */
void	wmgr_open(/* int toolfd */);
void	wmgr_close(/* int toolfd */);
void	wmgr_top(/* int toolfd */);
void	wmgr_bottom(/* int toolfd */);

/*
 * Exported by wmgr_rect.c:
 */
void		wmgr_completechangerect(/* fd, newrect, oldrect, parent_left, parent_top */ );
void		wmgr_refreshwindow(/* int windowfd */);
int		wmgr_get_placeholders(/* root, orect, irect */);
int		wmgr_set_placeholders(/* root, orect, irect */);

#ifdef WMGR_POLICY
/*
 * The following routines are exported by wmgr_policy.c;  they implement
 * the default positioning of icons and open windows for tools.
 * Init_xxx_posiiton sets the initial position from which a sequence
 *	of icons / open windows will propagate.
 * Figure_xxx_rect establishes a new rect for one tool, without
 *	requiring the tool's windowfd;  this implies the window should
 *	not yet be in the display tree when these routines are called.
 * Acquire_xxx_rect does the same job, but takes care of removing / 
 *	reinserting the window in the display tree.
 * Inquire_xxx_rect returns the same information Aquire would,
 *	without advancing the global positioning information.
 * Set_xxx_rect handle the rect changes for operations Front / Back
 *	and Full / Open / Close
 */
int	wmgr_init_icon_position();
int	wmgr_init_tool_position();
int	wmgr_acquire_icon_rect(/* int windowfd; struct rect *iconrect */);
int	wmgr_acquire_tool_rect(/* int windowfd; struct rect *toolrect */);
int	wmgr_inquire_icon_rect(/* int windowfd; struct rect *iconrect */);
int	wmgr_inquire_tool_rect(/* int windowfd; struct rect *toolrect */);
int	wmgr_set_icon_rect(/* int windowfd, op */);
int	wmgr_set_tool_rect(/* int windowfd, op */);
#endif

/*
 * Exported by wmgr_state.c:
 */
void	wmgr_changestate(/* int windowfd, rootfd, close */);
void	wmgr_setlevel(/* int windowfd, prevlink, nextlink */);
void	wmgr_full(/* struct tool *tool, int rootfd */);

/*
 * Fork programname with otherargs.  Place its normal rect at normalrect.
 * Place its icon rect at iconrect.  Iconicflag indicates the original
 * state of the tool.  Positioning/state information are only hints.
 * The tool can ignore them.
 */
int	wmgr_forktool(/* char *programname, *otherargs, struct rect *normalrect,
	   struct rect *iconrect, int iconicflag */);

#endif

