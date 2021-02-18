/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) AdFrame.c 5.1 89/12/14 Crucible";
/*
 * This file contains all of the functions related to the adornment
 * of new windows.
 */

#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>

#include "bitmaps/ulresize.h"
#include "bitmaps/llresize.h"
#include "bitmaps/urresize.h"
#include "bitmaps/lrresize.h"
#include "bitmaps/pushpin.h"
#include "bitmaps/pushpinin.h"

#define	MINSIZE		5			/* minimum size of pane */

/* Externals */
extern	Atom		AtomLeftFooter;
extern	Atom		AtomRightFooter;
extern	Atom		AtomColorMapWindows;
extern  Atom		AtomOlwmTimestamp;
extern	XFontStruct	*NormalFontInfo;
extern	Bool		f3dUsed;
extern	void		*IntMalloc();
extern	Bool		FocusLenience;

/* Globals */
Colormap	CurrColormap;
Window		CurrColormapWindow;

/* Edge clearance for adornment window.  This is for a 12-point
 * header.
 */
#define LEFT_EDGE	4
#define RIGHT_EDGE	4
#define BOTTOM_EDGE	llresize_height + 1
#define TOP_EDGE	5

#define TITLE_HEIGHT	16
#define	FOOTER_HEIGHT	TITLE_HEIGHT
#define	HEADER_HEIGHT	TITLE_HEIGHT +3

/* Events in the adornment window that are interesting. */
#define FRAME_EVENT_MASK        (ButtonPressMask | ButtonReleaseMask | \
				 ExposureMask | Button1MotionMask | \
				 EnterWindowMask | LeaveWindowMask | \
				 SubstructureRedirectMask | \
				 FocusChangeMask | PropertyChangeMask)


/* Macros to get to frame's children. */
#define	TITLECHILDINFO(fi)	((fi)->children.info[0])
#define	LLRESIZECHILDINFO(fi)	((fi)->children.info[1])
#define	ULRESIZECHILDINFO(fi)	((fi)->children.info[2])
#define	LRRESIZECHILDINFO(fi)	((fi)->children.info[3])
#define	URRESIZECHILDINFO(fi)	((fi)->children.info[4])
#define	ICONCHILDINFO(fi)	((fi)->children.info[5])
#define FOOTERCHILDINFO(fi)	((fi)->children.info[7])

/* Number of pixels user must move before we will think
 * s/he wants to move the window.
 */
#define MOVETHRESH	5

/* Graphics context with which to draw adornments. */
extern GC	DrawNormalGC;
extern GC	DrawReverseGC;
extern GC	DrawFrameGC;
extern GC	DrawSelectGC;
extern	int	DoubleClickTime;
extern	int	FocusFollowsMouse;  /* True if in focus follows mouse mode. */
extern Graphics_info *olgx_gismisc;

/* We set the focus to this invisible window when none of the
 * client windows have the focus.  This allows us to beep at
 * the user when s/he hits a key while no clients have the
 * focus.
 */
extern	Window	NoFocusWin;

/*
 * MakeFrame	-- make frame 
 *
 * This function assumes that the caller has already checked to see
 * if the window really needs adornment.
 *
 * This function creates a frame which will contain the client window
 * and all the decoration windows (resize corners, titlebar).  The
 * frame window is made the parent of the client window.  The
 * client window is now called the 'pane'.
 */
WinInfo *
MakeFrame(dpy, pane, winX, winY, winWidth, winHeight, ourWindow, hints, transient)
Display		*dpy;
Window		pane;
int		winX, winY;
unsigned int	winWidth, winHeight;
Bool		ourWindow;
NewXSizeHints	*hints;
Bool		transient;
{
	FrameInfo	*frameInfo;
	XSetWindowAttributes attributes;
	unsigned long	valuemask;
	Window	frame;
	WinInfo	*MakeURResize();
	WinInfo	*MakeULResize();
	WinInfo	*MakeLRResize();
	WinInfo	*MakeLLResize();
	WinInfo	*MakeTitleBar();
	WinInfo	*MakeIcon();
	WinInfo	*MakeFooter();
	int	DrawFrame();
	int	ConfigFrame();
	int	DestroyFrame();
	int	EventFrame();
	int	EventPane();
	int	FocusFrame();
	int	PropFrame();
	int	SelectFrame();
	void	CalcOffset();
	WMDecorations	*winDecors;
	WinInfo		*winInfo;
	WinInfo		*paneInfo;
	char		*name;
	Atom 		actType;
	int		actFormat;
	unsigned long	nItems, remain;
	int		topEdge;
	int		bottomEdge;
	int		winType;
	Window		wRoot, wChild;
	int		mask, childX, childY;
	Atom		*protocols;
	int		count;
	Bool		unnamed = False;	/* true iff win has no name */
	XWindowAttributes	paneAttr;
	int		xoff, yoff;

	/* get all the info about the new pane */
	XGetWindowAttributes(dpy, pane, &paneAttr);

	/*
	 * The first thing we want to do is find out how big the frame
	 * will be.
	 */
	winDecors = GetOLWinDecors(dpy, pane, &winType, transient);
	winWidth += RIGHT_EDGE + LEFT_EDGE;
	winHeight += TOP_EDGE + BOTTOM_EDGE;
	
	bottomEdge = BOTTOM_EDGE;
	if (winDecors->flags & WMDecorationFooter) {
		winHeight += FOOTER_HEIGHT;
		bottomEdge += FOOTER_HEIGHT;
	}

	topEdge = TOP_EDGE;
	if (winDecors->flags & WMDecorationHeader)
	{
		winHeight += HEADER_HEIGHT;
		topEdge += HEADER_HEIGHT;
	}

	CalcOffset(hints,topEdge,bottomEdge,LEFT_EDGE,RIGHT_EDGE,paneAttr,
		   &xoff,&yoff);

	/* Create frame window. */
	valuemask = CWEventMask | CWBorderPixel | CWBackPixel | CWBackingStore;
	attributes.event_mask = FRAME_EVENT_MASK;
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	attributes.backing_store = paneAttr.backing_store;

	frame = XCreateWindow(dpy, RootWindow(dpy, DefaultScreen(dpy)),
			 winX-xoff, winY-yoff, winWidth, winHeight, 
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 InputOutput,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 valuemask,
			 &attributes);

	/* Create the wininfo structure. */
	winInfo = WICreateInfo(frame);

	/* Add the frame to our list of frames. */
	AddFrame(dpy, winInfo);

	winInfo->frame = winInfo;
	winInfo->self = frame;
	winInfo->winState = Withdrawn;
	winInfo->winType = winType;
	winInfo->colormap = None;
	winInfo->pendingUnmaps = 0;

	/* Initialize private data of frame. */
	frameInfo = (FrameInfo *)IntMalloc((unsigned)sizeof(FrameInfo));
	winInfo->priv.ptr = (void *)frameInfo;

	frameInfo->winDecors = winDecors;
	frameInfo->pane = pane;
	frameInfo->isFocus = 0;
	frameInfo->offsetX = xoff;
	frameInfo->offsetY = yoff;
	frameInfo->configMask = 0L;
	frameInfo->oldBorderWidth = paneAttr.border_width;

	/* tweek pane attributes */
	if (paneAttr.backing_store != NotUseful)
	{
		/* since the pane is reparented, its backing store
		 * is not useful. In the code above the backing store
		 * attribute is propogated to the frame, so it is safe to
		 * remove it here.
		 */
		attributes.backing_store = NotUseful;
		XChangeWindowAttributes(dpy,
					pane,
					CWBackingStore,
					&attributes);
	}

	if (hints)
	    frameInfo->normHints = hints;
	else
	    hints = frameInfo->normHints 
		= (NewXSizeHints *)IntMalloc((unsigned)sizeof(NewXSizeHints));

	XGetWindowProperty(dpy,
                         GetPane(winInfo->frame),
                         AtomProtocols,
                         0L,
                         3*LONG_LENGTH(Atom),
                         False,
                         (Atom)AnyPropertyType,
                         &actType,
                         &actFormat,
                         &nItems,
                         &remain,
			 /* lint will complain about this cast */
                         (unsigned char **) &protocols);

	frameInfo->protocols = 0;
	for (count = 0; count < nItems; count++)
	{
		if (protocols[count] == AtomTakeFocus)
			frameInfo->protocols |= TAKE_FOCUS;
		else if (protocols[count] == AtomSaveYourself)
			frameInfo->protocols |= SAVE_YOURSELF;
		else if (protocols[count] == AtomDeleteWindow)
			frameInfo->protocols |= DELETE_WINDOW;
	}

	/* If the hints structure did not have a min size set in
	 * it, zero the fields out so the MAX function below will
	 * work.  We can't be sure that the min width and height
	 * fields have zero in them just because they haven't been
	 * set.
	 */
	if (!(hints->flags & PMinSize))
	{
		hints->min_width = 0;
		hints->min_height = 0;
	}

	/* We set up the min size to be at least MINSIZE (for pane). */
	hints->flags |= PMinSize;
	hints->min_width = MAX(MINSIZE, hints->min_width);
	hints->min_height = MAX(MINSIZE, hints->min_height);

	/* Set up functions. */
	winInfo->drawfunc = DrawFrame;
	winInfo->eventfunc = EventFrame;
	winInfo->configfunc = ConfigFrame;
	winInfo->destroyfunc = DestroyFrame;
	winInfo->focusfunc = FocusFrame;
	winInfo->selectfunc = SelectFrame;
	winInfo->propfunc = PropFrame;

	/*
	 * If this window is not one of ours (a menu window) create the 
	 * WinInfo structure for the pane so events on it can find the frame.
	 */
	if (!ourWindow)
	{
		paneInfo = WICreateInfo(pane);
		paneInfo->frame = winInfo;
		paneInfo->self = pane;
		paneInfo->eventfunc = EventPane;
		if (paneAttr.colormap)
			paneInfo->colormap = paneAttr.colormap;
		else
			paneInfo->colormap = None;

		trackSubwindows(dpy, pane, winInfo);
	}
	else
		paneInfo = WIGetInfo(pane);
	
	/* Get window names. */
	if (XFetchName(dpy, frameInfo->pane, &name))
		frameInfo->name = name;
	else
	{
		frameInfo->name = DEFAULTWINNAME;
		unnamed = True;
	}

	if (XGetIconName(dpy, frameInfo->pane, &name))
		frameInfo->iconname = name;
	else
		frameInfo->iconname = frameInfo->name;

	/* Read the left footer, if any. */
	XGetWindowProperty(dpy, 
			   frameInfo->pane, 
			   AtomLeftFooter,
			   0L, 
			   FOOTLEN,
			   False, 
			   (Atom)AnyPropertyType, 
			   &actType, 
			   &actFormat, 
			   &nItems, 
			   &remain, 
			   (unsigned char **)&(frameInfo->leftfooter));

	/* Read the right footer, if any. */
	XGetWindowProperty(dpy, 
			   frameInfo->pane, 
			   AtomRightFooter,
			   0L, 
			   FOOTLEN, 
			   False, 
			   (Atom)AnyPropertyType, 
			   &actType, 
			   &actFormat, 
			   &nItems, 
			   &remain, 
			   (unsigned char **)&(frameInfo->rightfooter));

	XGetWindowProperty(dpy, 
			frameInfo->pane, 
			XA_WM_HINTS,
			0L, 
			LONG_LENGTH(XWMHints),
			False, 
			(Atom)AnyPropertyType, 
			&actType, 
			&actFormat, 
			&nItems, 
			&remain, 
			/* lint will complain about this cast */
			(unsigned char **)&(frameInfo->wmHints));

	if (nItems == sizeof(XWMHints)/sizeof(int))
		frameInfo->hasMessages = True;
	else
		frameInfo->hasMessages = False;

	/* We need to check to see if this window is a group leader
	 * or not.  If it is a group leader then we do nothing with 
	 * it.  If it is not a group leader, we need to find the group
	 * leader's frame and add ourselves as a follower.
	 */
	if (frameInfo->wmHints)
		if ((frameInfo->wmHints->flags & WindowGroupHint) &&
		    (frameInfo->wmHints->window_group != frameInfo->pane))
		{
			frameInfo->leader = 
			WIGetInfo(frameInfo->wmHints->window_group);
			InsertFollower( frameInfo->leader, winInfo);
		}

	/* figure out what focus mode this window intends */
	if ((frameInfo->wmHints) && (frameInfo->wmHints->input))
	{
		if (frameInfo->protocols & TAKE_FOCUS)
			frameInfo->focusMode = LocallyActive;
		else
			frameInfo->focusMode = Passive;
	}
	else /* wmHints->input == False */
	{
		if (frameInfo->protocols & TAKE_FOCUS)
			frameInfo->focusMode = GloballyActive;
		else
			frameInfo->focusMode = NoInput;
	}

	/*
	 * Focus Lenience.  Be lenient about enforcing the requirement that
	 * clients set the input hint in WM_HINTS before they can get keyboard
	 * input.  If this flag is set, and if someone has selected for
	 * keyboard input on this window (presumably this client), and if the
	 * focus mode is NoInput, then force it to be passive.  This way, if a
	 * client either fails to set the WM_HINTS.input field, or fails to
	 * write WM_HINTS at all, it can still get keyboard input.  This kind
	 * of flag should be implemented on a client-by-client basis, not on a
	 * global basis.
	 */
	if (FocusLenience &&
	    (paneAttr.all_event_masks & (KeyPressMask|KeyReleaseMask)) &&
	    frameInfo->focusMode == NoInput ) {
	    frameInfo->focusMode = Passive;
	}

	/* Enter window's geometry. */
	winInfo->x = winX-xoff;
	winInfo->y = winY-yoff;
	winInfo->width = winWidth;
	winInfo->height = winHeight;

	/* Make sub-windows for adornments. */
	frameInfo->children.num = 0;
	if (winDecors->flags & WMDecorationResizeable)
	{
		/* Increase the minimum size.
		 * All resize corners are same size
		 * so just use the measurements of
		 * one corner.
		 */
		hints->min_width += ulresize_width;
		hints->min_height += ulresize_height;

		URRESIZECHILDINFO(frameInfo) =
				MakeURResize(dpy, winInfo, (int)winWidth);

		ULRESIZECHILDINFO(frameInfo) = MakeULResize(dpy, winInfo);

		LRRESIZECHILDINFO(frameInfo) = 
				MakeLRResize(dpy, winInfo, 
					     (int)winWidth, (int)winHeight);

		LLRESIZECHILDINFO(frameInfo) = 
				MakeLLResize(dpy, winInfo, (int)winHeight);
		
		frameInfo->children.num += 4;
	}

	if (winDecors->flags & WMDecorationFooter)
	{
		/* Increase the minimum size. */
		hints->min_height += FOOTER_HEIGHT;

		FOOTERCHILDINFO(frameInfo) = 
				MakeFooter(dpy, winInfo, LEFT_EDGE, 
					   (int)(winHeight 
					         - FOOTER_HEIGHT - BOTTOM_EDGE),
					   (int)(winWidth 
						 - LEFT_EDGE - RIGHT_EDGE),
					   FOOTER_HEIGHT);
		frameInfo->children.num++;
	}

	if ((winDecors->flags & WMDecorationHeader) &&
	    !(transient && unnamed))
	{
		/* Increase the minimum size because a window
		 * can't be any smaller that all the decorations
		 * around it.
		 */
		hints->flags |= PMinSize;

		/* All resize corners are same size. */
		hints->min_height += TITLE_HEIGHT;

		TITLECHILDINFO(frameInfo) = 
				MakeTitleBar(dpy, winInfo, winDecors, 
					     LEFT_EDGE+1, TOP_EDGE,
					     (int)(winWidth - LEFT_EDGE - 
					           RIGHT_EDGE-(2*1)),
					     TITLE_HEIGHT);
		frameInfo->children.num++;
	}

	/* Determine which menu should come up when menus are requested
	 * for this frame. */
	switch(winDecors->menu_type)
	{
		case MENU_FULL:
			frameInfo->frameMenu = &FrameFullMenu;
			break;

		case MENU_LIMITED:
			frameInfo->frameMenu = &FrameLimMenu;
			break;

		case MENU_NONE:
			frameInfo->frameMenu = 0;
			break;
	}

	ICONCHILDINFO(frameInfo) = MakeIcon(dpy, winInfo, winWidth);
	frameInfo->children.num++;

	/* Make the frame the parent of the original window. */
	XReparentWindow(dpy,  pane, frame, LEFT_EDGE, topEdge);
	paneInfo->x = LEFT_EDGE;
	paneInfo->y = topEdge;
	paneInfo->width = paneAttr.width;
	paneInfo->height = paneAttr.height;
	
	/* Map the windows. */
	XMapWindow(dpy, pane);

	/* Warp the pointer if necessary. */
	if (winInfo->warpWin)
	{
		winInfo->warpBack = True;
		/* First find the current position. */
		if (XQueryPointer(dpy, 
			RootWindow(dpy, DefaultScreen(dpy)),
			&wRoot,
			&wChild,
			&winInfo->warpRetX, 
			&winInfo->warpRetY,
			&childX, &childY,
			&mask))
		{
			/* Pointer is on our root window. */
			/* Warp the pointer to our warp win. */
			XWarpPointer(dpy, 
				None, winInfo->warpWin, 
				0, 0, 0, 0, 
				10, 10);
		}
	}
	else
		winInfo->warpBack = False;

	/* This is some fancy grab stuff that is done to make sure that
	 * the first click that the user does to set the focus
	 * also gets passed to the window.
	 * This is only important in ClickToFocus mode.
	 *
	 * When the user clicks in this window the grab is activated.
	 * The code then sets the input focus to the Pane for this frame and
	 * then do an XAllowEvents to let the button press fall through
	 * to the the client window.
	 */
	if (!FocusFollowsMouse)
	{
		/* If client is Passive or Locally Active then do a passive
		 * grab on button 1. */
		if ((frameInfo->focusMode == Passive) ||
		    (frameInfo->focusMode == LocallyActive))
		{
			XGrabButton(dpy, Button1, 0, frame, True, 
				    ButtonPressMask | ButtonReleaseMask | 
				    Button1MotionMask,
			    	    GrabModeSync, GrabModeAsync, None, None);
		}
	} 

	return winInfo;
}

/*
 * GetFramedSize -- fills in winInfo structure with framed width 
 *		    and height
 *	width	- width of unframed window
 *	height	- height of unframed window
 */
GetFramedSize( winInfo, width, height)
WinInfo	*winInfo;
int	width, height;
{
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);

	winInfo->width = width + RIGHT_EDGE + LEFT_EDGE;
	winInfo->height = height + TOP_EDGE + BOTTOM_EDGE;
	
	if (frameInfo->winDecors->flags & WMDecorationFooter)
		winInfo->height += FOOTER_HEIGHT;

	if (frameInfo->winDecors->flags & WMDecorationHeader)
		winInfo->height += HEADER_HEIGHT;
}

/*
 * GetUnFramedSize -- returns width and height of window if un-framed
 *      retWidth- width of unframed window
 *      retHeight-height of unframed window
 */
GetUnFramedSize( winInfo, retWidth, retHeight )
WinInfo	*winInfo;
int     *retWidth, *retHeight;
{
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);

	*retWidth = winInfo->width - RIGHT_EDGE - LEFT_EDGE;
	*retHeight = winInfo->height - TOP_EDGE - BOTTOM_EDGE;
	
	if (frameInfo->winDecors->flags & WMDecorationFooter)
		*retHeight -= FOOTER_HEIGHT;

	if (frameInfo->winDecors->flags & WMDecorationHeader)
		*retHeight -= HEADER_HEIGHT;
}


/*
 * CalcOffset -- calculate the offset from the origin of the frame window to 
 * the virtual origin of the client's window (the pane).  The virtual origin 
 * is where the origin of the client's window would be if we hadn't reparented 
 * it and set its border width to zero.  We use this offset to "lie" to the 
 * client a little bit in synthetic ConfigureNotify events so that it has can 
 * give a reasonable geometry specification in response to WM_SAVE_YOURSELF.
 */
void
CalcOffset(hints,TE,BE,LE,RE,attr,xoff,yoff)
    NewXSizeHints *hints;
    int TE, BE, LE, RE;
    XWindowAttributes attr;
#   define BW (attr.border_width)
    int *xoff, *yoff;
{
    int gravity;

    if (hints &&
	hints->flags & PWinGravity )
	gravity = hints->win_gravity;
    else
	gravity = NorthWestGravity;

    switch ( gravity ) {
    default:
    case NorthWestGravity:
	*xoff = 0;			*yoff = 0;
	break;
    case NorthGravity:
	*xoff = LE-BW;			*yoff = 0;
	break;
    case NorthEastGravity:
	*xoff = RE+LE-2*BW;		*yoff = 0;
	break;
    case WestGravity:
	*xoff = 0;			*yoff = TE-BW;
	break;
    case CenterGravity:
	*xoff = LE-BW;			*yoff = TE-BW;
	break;
    case EastGravity:
	*xoff = RE+LE-2*BW;		*yoff = TE-BW;
	break;
    case SouthWestGravity:
	*xoff = 0;			*yoff = TE+BE-2*BW;
	break;
    case SouthGravity:
	*xoff = LE-BW;			*yoff = TE+BE-2*BW;
	break;
    case SouthEastGravity:
	*xoff = RE+LE-2*BW;		*yoff = TE+BE-2*BW;
	break;
    }
}

#undef BW

/*
 * SetFocus -- tell the pane that it should be focused
 *
 * This routine is unused at the moment.  All calls to
 * it have been commented out.
 */
SetFocus(dpy, frame)
Display	*dpy;
WinInfo	*frame;
{
	FrameInfo	*frameInfo = (FrameInfo *)(frame->priv.ptr);
	XEvent		clientEvent;

	/* We need to set the focus to this window. */
	if (frameInfo->protocols & TAKE_FOCUS)
	{
		/* This is an Open Look client. */
		clientEvent.xclient.type = ClientMessage;
		clientEvent.xclient.message_type = AtomProtocols;
		clientEvent.xclient.format = 32;
		clientEvent.xclient.display = dpy;
		clientEvent.xclient.window = frameInfo->pane;
		clientEvent.xclient.data.l[0] = AtomTakeFocus;

		/* This should really be the time of the event
		 * which caused us to set the focus. */
		clientEvent.xclient.data.l[1] = CurrentTime;

		/* Send the event. */
		XSendEvent(dpy, 
			   clientEvent.xclient.window, 
			   False,
			   NoEventMask, 
			   &clientEvent);
	}
	else
		if (!frameInfo->wmHints) 
			/* Set the focus. */
			XSetInputFocus(dpy, 
				       frameInfo->pane,
				       /*RevertToNone, */
				       RevertToParent, 
				       CurrentTime);
}



/*
 * GetSizeHints -- return size hints from frame structure
 */
NewXSizeHints *
GetSizeHints(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->normHints;
}


/*
 * GetLeader 	-- return leader from frame structure
 */
WinInfo *
GetLeader(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->leader;
}

/*
 * GetFollowers 	-- return followers from frame structure
 */
FollowBlock *
GetFollowers(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->followers;
}

/*
 * GetDecors 		-- return decoration hints from frame structure
 */
WMDecorations *
GetDecors(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->winDecors;
}

/*
 * GetWMHints 		-- return wm hints from frame structure
 */
XWMHints *
GetWMHints(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->wmHints;
}

/*
 * GetName 		-- return  name from passed frame 
 */
char *
GetName(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->name;
}


/*
 * GetIcon		-- return icon's WinInfo structure for passed
 *			   frame
 */
WinInfo *
GetIcon(frame)
WinInfo	*frame;
{
	return ICONCHILDINFO((FrameInfo *)(frame->priv.ptr));
}


/*
 * GetRightFooter 	-- return right footer string
 */
char *
GetRightFooter(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->rightfooter;
}


/*
 * GetLeftFooter 	-- return left footer string
 */
char *
GetLeftFooter(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->leftfooter;
}


/*
 * GetIconName 		-- return icon name from passed frame 
 */
char *
GetIconName(frame)
WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->iconname;
}

/*
 * GetPane		-- returns framed pane
 */
Window 
GetPane( frame )
register WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->pane;
}

/*
 * CheckFocus		-- returns True if frame is current focus
 */
Bool
CheckFocus( frame )
register WinInfo	*frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->isFocus;
}

/*
 * CheckSelect		-- returns True iff frame is selected
 *
 *	Currently unused.
 */
Bool
CheckSelect( frame )
register WinInfo *frame;
{
	return ((FrameInfo *)(frame->priv.ptr))->isSelected;
}

/*
 * GetWMState		-- get the contents of the WM_STATE property.
 */
Bool
GetWMState( dpy, win, state, iconwin )
    Window win;
    int *state;
    Window *iconwin;
{
    int status;
    Atom type;
    int format, nitems, remain;
    int *data;
    status = XGetWindowProperty(dpy, win, AtomWMState, 0L, 2L, False,
				AtomWMState, &type, &format, &nitems,
				&remain, (unsigned char *)&data);
    if ( status != Success
	|| type != AtomWMState
	|| format != 32
	|| nitems != 2 )
	return False;
    *state = data[0];
    *iconwin = (Window) data[1];
    return True;
}

/*
 * SetWMState		-- set the contents of the WM_STATE property,
 *			   given the information in the WinInfo struct.
 */
void
SetWMState( dpy, frame )
    Display *dpy;
    WinInfo *frame;
{
    unsigned long data[2];
    FrameInfo *frameInfo = (FrameInfo *)(frame->priv.ptr);
    WinInfo *iconWinInfo = GetIcon(frame);
    Window pane = GetPane(frame);
    IconInfo *iconInfo;

    switch ( frame->winState ) {
    default:
    case Normal:
	data[0] = NormalState;
	break;
    case Iconic:
	data[0] = IconicState;
	break;
    case Withdrawn:
	data[0] = WithdrawnState;
	break;
    }
    if ( iconWinInfo ) {
	iconInfo = (IconInfo *)(iconWinInfo->priv.ptr);
	if ( iconInfo->iconWin )
	    data[1] = iconInfo->iconWin;
	else
	    data[1] = iconWinInfo->self;
    } else {
	data[1] = None;
    }
    XChangeProperty(dpy, pane, AtomWMState, AtomWMState,
		    32, PropModeReplace, (unsigned char *)data, 2);
}

/* end globals */

/*
 * trackSubwindows	-- check for the WM_COLORMAP_WINDOWS prop
 *			on a pane, if it exists, track the subwindows.
 */
static
trackSubwindows(dpy, pane, frameInfo)
Display	*dpy;
Window	pane;
WinInfo	*frameInfo;
{
	int		EventSubwin();
	Atom 		actType;
	int		actFormat;
	unsigned long	nItems, remain;
	long		itemOffset = 0;
	Window		subwin;
	WinInfo		*subwinInfo;
	Window		*prop;
	XWindowAttributes	subwinAttr;

	/* create winInfos for colormapped subwindows,
	 * if any.
	 */
	for (;;)
	{
		if (XGetWindowProperty(dpy, pane, AtomColorMapWindows,
				 itemOffset++, LONG_LENGTH(Window),
				 False,
				 (Atom)AnyPropertyType,
				 &actType, &actFormat, &nItems,
				 &remain,
			 	 /* lint will complain about this cast */
				 (unsigned char **)&prop) == Success)
		{
			/* if the atom type is incorrect,
			 * or there is no actual data associated,
			 * then break out of the loop.
			 */
			if ((actType != XA_WINDOW) ||
			    (nItems == 0))
			{
				break;
			}
			
			/* register this subwindow that has a
			 * unique colormap.
			 */
			subwin = *prop;
			if (WIGetInfo(subwin))
			{
				/* don't molest if already tracked */
				continue;
			}
			subwinInfo = WICreateInfo(subwin);
			subwinInfo->frame = frameInfo;
			subwinInfo->self = subwin;
			subwinInfo->eventfunc = EventSubwin;

			/* ask for enter/exit notify events on the subwin */

			/* get the colormap */
			XGetWindowAttributes(dpy, subwin, &subwinAttr);
			subwinInfo->colormap = subwinAttr.colormap;

			/* start paying attention to enter/exits on the win */
			XSelectInput(dpy,
				     subwin,
				     subwinAttr.your_event_mask |
					EnterWindowMask | LeaveWindowMask);

			XFree(prop);
		}
		else
		{
			/* no more colormap sub windows */
			break;
		}
	}
}

/*
 * EventPane -- handle events that we get that are for the pane
 */
static
EventPane(dpy, event, winInfo)
Display *dpy;
XEvent  *event;
WinInfo *winInfo;
{
	WinInfo		*leader;
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->frame->priv.ptr);

#define CMapEvent	(event->xcolormap)

	switch (event->type)
	{
		case EnterNotify:
		case LeaveNotify:
			decodeCrossing(dpy, event, winInfo);
			break;

		case ColormapNotify:
			/* Setup a new color map for the pane. */
			if (CMapEvent.new)
				winInfo->colormap = CMapEvent.colormap;
			break;

		case MappingNotify:
			/* We ignore these notifications. */
			break;

		case ConfigureRequest:
			break;

		case UnmapNotify:	/* Transition to WithdrawnState */
			if (winInfo->frame->pendingUnmaps > 0)
			{
			    --winInfo->frame->pendingUnmaps;
			    break;
			}
			/* This may be a duplicate unmap event,
			 * check that the window is not already mapped.
			 */
			if (winInfo->winState == Withdrawn)
				break;

			/*
			 * If this window had the focus then we need to
			 * set the focus back to the NoFocusWin.  We need
			 * a timestamp to give to XSetInputFocus, but the 
			 * UnmapNotify event doesn't have one.  So we
			 * get one by writing a null property to the
			 * frame window.  If we used CurrentTime instead of 
			 * getting a timestamp this way, it could cause 
			 * subsequent focus change requests with timestamps in 
			 * the past to be ignored.  This occurs most often 
			 * when you type "exit" to a shell window, and the 
			 * symptom is that the window underneath doesn't get 
			 * the input focus (in follows-mouse mode).
			 *
	 		 * 	(RevertToPointerRoot is cast from long to 
			 *	int - lint will warn)
			 */
			if (frameInfo->isFocus == 1) {
			    XEvent e;
			    Time timestamp;

			    XChangeProperty(dpy,winInfo->frame->self,
					    AtomOlwmTimestamp,XA_INTEGER,
					    32,PropModeReplace,
					    &winInfo,0);
			    XSync(dpy,0);
			    if (XCheckTypedWindowEvent(dpy,
						       winInfo->frame->self,
						       PropertyNotify,&e))
				timestamp = e.xproperty.time;
			    else
				timestamp = CurrentTime;
			    XSetInputFocus(dpy, NoFocusWin, 
					   RevertToPointerRoot,
					   timestamp);
			}

			/* Return the pointer if necessary */
			if (winInfo->frame->warpBack)
			{
				XWarpPointer(dpy, 
				     None, 
				     RootWindow(dpy,DefaultScreen(dpy)), 
				     0, 0, 0, 0, 
				     winInfo->frame->warpRetX, 
				     winInfo->frame->warpRetY);
				winInfo->frame->warpBack = False;
			}

			/* Unmap the frame. */
			RemoveSelection(dpy, winInfo->frame);
			XUnmapWindow(dpy, winInfo->frame->self);

			/* Unmap the pane so we get a map
			 * request when the client wants a transition
			 * from Withdrawn to Normal or Iconic.
			 */
			XUnmapWindow(dpy, winInfo->self);
#ifdef LATER
			/* In the future, for full ICCCM compliance,
			 * the pane will be reparented to root and
			 * completely ignored until it remaps.
			 * Until then the pane is tracked through
			 * all of its configurations while it
			 * is unmapped.
			 */

			/* Reparent the pane */
			XReparentWindow(dpy,
					winInfo->self,
					RootWindow(dpy, DefaultScreen(dpy)),
					winInfo->frame->x+frameInfo->offsetX,
					winInfo->frame->y+frameInfo->offsetY);
#endif /* LATER */

			/* Mark current state */
			winInfo->winState = Withdrawn;
			winInfo->frame->winState = Withdrawn;
			SetWMState(dpy, winInfo->frame);
			break;

		case MapNotify:
			/* These are handled as MapRequests in EventFrame() */
			break;

		case DestroyNotify:
			/* Ooops.  Our pane went away. */
			/* Return the pointer */
			if (winInfo->frame->warpBack)
			{
				XWarpPointer(dpy, 
				     None, 
				     RootWindow(dpy,DefaultScreen(dpy)), 
				     0, 0, 0, 0, 
				     winInfo->frame->warpRetX, 
				     winInfo->frame->warpRetY);
				winInfo->frame->warpBack = False;
			}

			RemoveSelection(dpy, winInfo->frame);

			/* See if we are a follower. */
			leader=
			      ((FrameInfo *)(winInfo->frame->priv.ptr))->leader;
			if (leader)
				DeleteFollower(leader, winInfo->frame);

			if (winInfo->colormap)
			{
				/* since the window was destroyed,
				 * its colormap has been deinstalled.
				 * This will install the default colormap,
				 * though on a servers with colormap stacks 
				 * that are 1 deep, the default will already
				 * have been installed.
				 */
				InstallColormap(dpy,
					RootWindow(dpy,DefaultScreen(dpy)),
					DefaultColormap(dpy,DefaultScreen(dpy)));
			}
			WIDeleteInfo(winInfo->self);
			DestroyFrame(dpy, winInfo->frame);
			break;

		case FocusIn:
 			if (event->xfocus.mode == NotifyNormal &&
 			    winInfo->frame->focusfunc != NULL)
  				(*winInfo->frame->focusfunc)(dpy, 
  							winInfo->frame, True);
			break;

		case FocusOut:
 			if (event->xfocus.mode == NotifyNormal &&
 			    winInfo->frame->focusfunc != NULL)
  				(*winInfo->frame->focusfunc)(dpy, 
  							winInfo->frame, False);
			break;

		case PropertyNotify:
			/* If this is a colormap windows change,
			 * start tracking the new windows.
			 * THIS DOES NOT STOP TRACKING THE OLD WINDOWS ???
			 */
			if (event->xproperty.atom == AtomColorMapWindows)
			{
				/* WM_COLORMAP_WINDOWS changed */
				if (event->xproperty.state == PropertyNewValue)
				{
					trackSubwindows(dpy,
							winInfo->self,
							winInfo->frame);
				}
				else		/* PropertyDelete */
				{
					/* Not handled yet ??? */
				}
			}
			else
			{
				/* Call the frame's property change function with
				 * the event. */
				(*winInfo->frame->propfunc)(dpy,
							    winInfo->frame,
							    event);
			}
			break;

		case ClientMessage:
			if (event->xclient.message_type == AtomChangeState)
			{
				if (event->xclient.data.l[0] == IconicState)
					Iconify(dpy, winInfo->frame);
			}
			break;

		default:
			if (PrintOrphans)
				DebugEvent(event, "EventPane");
			break;
	}
}

/*
 * EventSubwin -- handle special case events for sub widows of a pane
 */
static
EventSubwin(dpy, event, winInfo)
Display *dpy;
XEvent  *event;
WinInfo *winInfo;
{
	switch (event->type)
	{
	case DestroyNotify:
		WIDeleteInfo(winInfo->self);
		break;
	
	case EnterNotify:
	case LeaveNotify:
		decodeCrossing(dpy, event, winInfo);
		break;

	default:
		if (PrintOrphans)
			DebugEvent(event, "EventSubwin");
		break;
	}
}

/*
 * decodeCrossing -- a special case handler for Enter/Leave notify events, as
 *		relates to colormaps.
 */
static
decodeCrossing(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinInfo	*winInfo;
{
	switch (event->xcrossing.type)
	{
	case EnterNotify:
		if (winInfo->colormap)
			InstallColormap(dpy, winInfo->self, winInfo->colormap);
		break;
	
	case LeaveNotify:
		break;
	}

#ifdef COLORMAP_DEBUG
	switch (event->xcrossing.detail)
	{
	case NotifyAncestor:
		printf("NotifyAncestor\n");
		break;
	case NotifyVirtual:
		printf("NotifyVirtual\n");
		break;
	case NotifyInferior:
		printf("NotifyInferior\n");
		break;
	case NotifyNonlinear:
		printf("NotifyNonLinear\n");
		break;
	case NotifyNonlinearVirtual:
		printf("NotifyNonLinearVirtual\n");
		break;
	}
#endif /* COLORMAP_DEBUG */
}

/*
 * EventFrame -- handle events for the frame
 *
 * The reader should be aware of the fact that both the titlebar
 * window and the frame window are affected when the window's
 * focus and select state is changed.  The window manager was
 * being written before the OpenLook spec. was completed so it is
 * not the most efficient design and is rather awkward in places.
 *
 * The way focusing is handled deserves some attention.  For a
 * detailed description of how focusing should be handled see
 * the Inter-Client Communication Conventions Manual.  I'll give
 * a rough overview below.
 *
 * A client can use one of four input models:  No Input, Passive,
 * Locally Active, Globally Active.  When OLWM is in focus-follows-
 * mouse mode, focus is handled in a fairly straightforward manner.
 * When the cursor enters a window, signaled by the frame getting
 * an EnterNotify event, OLWM sets the focus like this:
 *
 *	No Input 	- Do nothing.
 *	Passive  	- Set the focus using XSetInputFocus.
 *	Locally Active  - Set the focus using XSetInputFocus.
 *	Globally Active	- Set the focus by sending a message 
 *			  to the client.
 *
 * When OLWM is in click-to-focus mode, focus is a bit more complicated.
 * This is due to the fact that the user can press down in the decoration
 * around the client window and drag the window, and NOT set the focus.
 * If the user just clicks, without moving, in the decoration then we
 * set the focus.  So, we don't know whether to set the focus until
 * the button release event.  But, if the user presses down in the
 * client window we must set the focus immediately.  This is so the user
 * can go to an xterm which does not have the focus, press down, (which
 * sets the focus), move the cursor, and release the button to select 
 * some text, for example.  If the client is Passive or Locally Active,
 * we have a passive grab on the SELECT button.  This is how we handle
 * setting the focus upon button press and release for the four input
 * modes:
 *
 * 	ButtonPress
 *		No Input	- Do Nothing.
 *		Passive		- If the press was in the client,
 *				  set the focus
 *		Locally Active	- If the press was in the client,
 *				  set the focus
 *		Globally Active	- Do Nothing.
 *
 * 	ButtonRelease
 *		No Input	- Do Nothing.
 *		Passive		- Set the focus.  We only get here 
 *				  if the button press was NOT in 
 *				  the client. 
 *		Locally Active	- Set the focus.  We only get here 
 *				  if the button press was NOT in 
 *				  the client. 
 *		Globally Active	- Send a message to the client.
 */
static
EventFrame(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinInfo	*winInfo;
{
	WinInfo		*selInfo;
	int		offX, offY, oldWidth, oldHeight;
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);
	XEvent		clientEvent;

	/* We sometimes get MotionNotify events when the button
	 * is not pressed (even though we set Button1MotionMask).
	 * Is this a bug in the X11R3 server?  To handle this
	 * we need to record Button1 having been pressed.
	 * When the MB_SELECT button, (Button1), has been pressed
	 * hadSelect is set to true.
	 */
static	Bool		hadSelect = False;
static	Time		timeOfLastButPress = 0;
static	int		buttonPressX;
static	int		buttonPressY;
static	XButtonEvent	lastSelect;
static	Time		lastDoubleClickTime;

#define	bevent	(event->xbutton)
#define ConfEvent	(event->xconfigurerequest)

	switch (event->type)
	{
	case MapRequest:
		/* transition from Iconic or Withdrawn */
		if (winInfo->winState == Iconic ) {
		    DeIconify(dpy, winInfo);
		} else {
		    if (winInfo->winState == Withdrawn) {
			/*
			 * REMIND
			 * Check configuration of window,
			 * the window may have changed size, properties,
			 * hints, or may want different adornments.
			 */
		    }
		    winInfo->winState = Normal;
		    WIGetInfo(frameInfo->pane)->winState = Normal;
		    SetWMState(dpy, winInfo);
		    XMapWindow(dpy, frameInfo->pane);
		    XMapWindow(dpy, winInfo->self);
		}
		break;

	case ConfigureRequest:
		/*
		 * REMIND
		 * This ignores changes to the border width.  We should really 
		 * recalculate the frame offset and change the value stored in 
		 * frameInfo->oldBorderWidth, but without changing the window's
		 * true border size.
		 */
		frameInfo->configMask = ConfEvent.value_mask & ~CWBorderWidth;
		oldWidth = winInfo->width;
		oldHeight = winInfo->height;

		/* Note: the following alters winInfo->width and ->height */
		if (ConfEvent.value_mask & (CWWidth | CWHeight))
			GetFramedSize(winInfo->frame, 
				ConfEvent.width, ConfEvent.height);

		if (ConfEvent.value_mask & CWX) {
		    winInfo->x = ConfEvent.x - frameInfo->offsetX;
		} else if ((ConfEvent.value_mask & CWWidth) &&
			   frameInfo->normHints &&
			   (frameInfo->normHints->flags & PWinGravity)) {
		    switch (frameInfo->normHints->win_gravity) {
		    case NorthEastGravity:
		    case EastGravity:
		    case SouthEastGravity:
			winInfo->x -= winInfo->width - oldWidth;
			frameInfo->configMask |= CWX;
			break;
		    case NorthGravity:
		    case CenterGravity:
		    case SouthGravity:
			winInfo->x -= (winInfo->width - oldWidth) / 2;
			frameInfo->configMask |= CWX;
			break;
		    }
		}

		if (ConfEvent.value_mask & CWY) {
		    winInfo->y = ConfEvent.y - frameInfo->offsetY;
		} else if ((ConfEvent.value_mask & CWHeight) &&
			   frameInfo->normHints &&
			   (frameInfo->normHints->flags & PWinGravity)) {
		    switch (frameInfo->normHints->win_gravity) {
		    case SouthWestGravity:
		    case SouthGravity:
		    case SouthEastGravity:
			winInfo->y -= winInfo->height - oldHeight;
			frameInfo->configMask |= CWY;
			break;
		    case WestGravity:
		    case CenterGravity:
		    case EastGravity:
			winInfo->y -= (winInfo->height - oldHeight)/2;
			frameInfo->configMask |= CWY;
			break;
		    }
		}

		(*(winInfo->frame)->configfunc)(dpy,winInfo->frame);
		break;

	case MappingNotify:
		/* we ignore these notifications */
		break;

	case Expose:
		DrawFrame(dpy, winInfo);
		break;

	case ButtonRelease:

		switch (MouseButton(dpy, event))
		{
		case MB_SELECT:
			ClearSelections(dpy);
			AddSelection(dpy, winInfo, event->xbutton.time);
			XRaiseWindow(dpy, winInfo->frame->self);
			hadSelect = False;

			if (!FocusFollowsMouse)
			{
				switch (frameInfo->focusMode)
				{
				case NoInput:
					/* Do Nothing */
					break;

				case Passive:
					/* If we get to here then the
					 * button press was not within
					 * the client window.
					 */
					XSetInputFocus(dpy,
						       GetPane(winInfo->frame),
						       /*RevertToNone,*/
						       RevertToParent,
						       event->xbutton.time);

				case LocallyActive:
					/* If we get to here then the
					 * button press was not within
					 * the client window.
					 */
					XSetInputFocus(dpy, 
						       GetPane(winInfo->frame),
						       /*RevertToNone, */
						       RevertToParent,
						       event->xbutton.time);
					break;

				case GloballyActive:
					/* The button press was NOT in the
					 * client window so send WM_TAKE_FOCUS.
					 */
					clientEvent.xclient.type = 
						ClientMessage;
					clientEvent.xclient.message_type = 
						AtomProtocols;
					clientEvent.xclient.format = 32;
					clientEvent.xclient.display = dpy;
					clientEvent.xclient.window = 
						frameInfo->pane;
					clientEvent.xclient.data.l[0] = 
						AtomTakeFocus;
					clientEvent.xclient.data.l[1] = 
						event->xbutton.time;

					/* send the event */
					XSendEvent(dpy, 
						   clientEvent.xclient.window, 
						   False,
						   NoEventMask, 
						   &clientEvent);
					break;
				} /* End switch */
			}  /* End if not FocusFollowsMouse */	
			break;  /* out of MB_SELECT case */
		}  /* End switch */
	break;  /* out of ButtonRelease */

	case MotionNotify:
		/* We get this only after a button1 press */
		if (hadSelect == False)	/* watch for erroneous motions */ 
		{
			break;
		}

		/* See if we have moved more than the threshold amount. */
		if ((ABS(event->xmotion.x - buttonPressX) < MOVETHRESH) &&
		    (ABS(event->xmotion.y - buttonPressY) < MOVETHRESH))
			break;

		/* If we are not on the selection list move just ourself,
		 * else we are on the selection list so move all the windows
		 * on the selection list.
		 */
		if (!IsSelected(winInfo))
		{
			UserMoveWin(dpy, &lastSelect, &offX, &offY, winInfo);

			/* if frame is iconified, we move icon */
			if (winInfo->winState == Iconic)
				winInfo = GetIcon(winInfo);
			winInfo->x += offX;
			winInfo->y += offY;

			/* call configuration function */
			frameInfo->configMask = CWX | CWY;
			(*winInfo->configfunc)(dpy, winInfo);
		}
		else
		{
			UserMoveWin(dpy, &lastSelect, &offX, &offY, 
				    (WinInfo *)0);

			/* move all the windows that were moved */
			selInfo = (WinInfo *)0;
			while(selInfo = EnumSelections(selInfo))
			{
				/* if frame is iconified, we move icon */
				if (selInfo->winState == Iconic) {
				    selInfo = GetIcon(selInfo);
				} else {
				    ((FrameInfo *)(selInfo->priv.ptr))->
					configMask = CWX | CWY;
				}
	
				selInfo->x += offX;
				selInfo->y += offY;
	
				/* call configuration function */
				(*selInfo->configfunc)(dpy, selInfo);
			}
		}  /* End if this window is on the selection list. */

		/*
		 * The user has moved the window and released the button
		 * -- in UserMoveWin() -- so we clear the flag.
		 */
		hadSelect = 0;
		break;


	case ButtonPress:

		/* Really we should not be doing anything on the
		 * ButtonPress event.  We should wait for the
		 * ButtonRelease before deciding what to do.
		 */

		switch (MouseButton(dpy, event))
		{
		case MB_MENU:
			if (WINDOWMENU(winInfo))
				MenuShow(dpy, winInfo, WINDOWMENU(winInfo), 
					 event->xbutton.x_root, 
					 event->xbutton.y_root);
			break;

		case MB_SELECT:
			/* Save the location where the button went down so we
			 * can see if the user moves the mouse more than
			 * MOVETHRESH, and wants to move the window.
			 */
			buttonPressX = event->xbutton.x;
			buttonPressY = event->xbutton.y;

			if (!FocusFollowsMouse)
			{
			/* It is possible for us to replay the event and
			 * have the window, (decoration window, e.g. the
			 * resize corner, the titlebar), in which the 
			 * button press happened to ignore it.  
			 * In this case we would get the event
			 * again.  For example, the user could button press
			 * in the title bar, (which doesn't select this event),
			 * and have this EventFrame routine get the same event
			 * twice.  So, we check that the time stamp of this 
			 * button press is different than the last.
			 */
				if (event->xbutton.time == timeOfLastButPress)
					/* We already dealt with this event. */
					break;
				timeOfLastButPress = event->xbutton.time;
	
				switch (frameInfo->focusMode)
				{
				case NoInput:
					/* Do Nothing, not even on the
					 * button release event.
					 */
					 break;

				case Passive:
					/* If the button press was in the
					 * client, set the input focus.
					 */
					if (event->xbutton.subwindow ==
					    GetPane(winInfo->frame))
					{
						XSetInputFocus(dpy, 
						       GetPane(winInfo->frame),
						       RevertToParent,
						       event->xbutton.time);
					}
					/* else press was not in the client
					 * and we'll set the focus on the
					 * button release event.
					 */
					break;

				case LocallyActive:
					/* If the button press was in the
					 * client, set the input focus.
					 */
					if (event->xbutton.subwindow ==
					    GetPane(winInfo->frame))
					{
						XSetInputFocus(dpy, 
						       GetPane(winInfo->frame),
						       RevertToParent,
						       event->xbutton.time);
					}
					/* else press was not in the client
					 * and we'll set the focus on the
					 * button release event.
					 */
					 break;

				case GloballyActive:
					/* If the button press was NOT in the
					 * client window we'll send
					 * WM_TAKE_FOCUS on the button 
					 * release event.
					 */
					 break;
				} /* End switch */

				/* Let the button press through 
				 * if we had grabbed it.
				 */
				XAllowEvents(dpy, ReplayPointer, CurrentTime);
			}  /* End if not FocusFollowsMouse */	

			/* If the click happend in the decoration windows,
			 * (i.e. not in the pane), check for a double click.
			 * Check last click time to see if we are in double
			 * click land.  Also check that the user hasn't just
			 * hit the button 3 times very rapidly.  This would
			 * cause a cycle of the window changing size.
			 */
			if ((bevent.subwindow != GetPane(winInfo->frame)) &&
			   ((bevent.time-lastSelect.time)<=DoubleClickTime) &&
			   ((bevent.time-lastDoubleClickTime)>DoubleClickTime))
			{
				/* we have a double click */
				if (frameInfo->frameMenu == &FrameNormMenu)
					NormSize(dpy, winInfo);
				else
					FullSize(dpy, winInfo);

				lastDoubleClickTime = bevent.time;
		   	 }
			lastSelect = event->xbutton;
			hadSelect = True;
			break;  /* Break case MB_SELECT */

		case MB_ADJUST:
			ToggleSelection(dpy, winInfo, event->xbutton.time);
			break;
		}  /*  End switch on button pressed */
		break;  /*  Break case ButtonPress  */

	case EnterNotify:
		if (FocusFollowsMouse)
		{
			if (event->xcrossing.detail != NotifyInferior)
			{
				switch (frameInfo->focusMode)
				{
				case NoInput:
					/* Do nothing */
					break;

				case Passive:
					XSetInputFocus(dpy, 
						       GetPane(winInfo->frame),
						       /*RevertToNone, */
						       RevertToParent,
					               event->xcrossing.time);
					break;

				case LocallyActive:
					XSetInputFocus(dpy, 
						       GetPane(winInfo->frame),
						       /*RevertToNone, */
						       RevertToParent,
					               event->xcrossing.time);
					break;

				case GloballyActive:
					clientEvent.xclient.type = 
						ClientMessage;
					clientEvent.xclient.message_type = 
						AtomProtocols;
					clientEvent.xclient.format = 32;
					clientEvent.xclient.display = dpy;
					clientEvent.xclient.window = 
						frameInfo->pane;
					clientEvent.xclient.data.l[0] = 
						AtomTakeFocus;
					clientEvent.xclient.data.l[1] = 
						event->xcrossing.time;

					/* send the event */
					XSendEvent(dpy, 
						   clientEvent.xclient.window, 
						   False,
			  			   NoEventMask, 
						   &clientEvent);
					break;
				}
			}  /* End if crossing to an inferior. */
		}  /* End if FocusFollowsMouse Mode. */
		break;

	case LeaveNotify:
		if (FocusFollowsMouse)
		{
			/* If we didn't leave to a child, then we
			 * are out of this frame, so set the focus
			 * to the root window.
			 */
			if (event->xcrossing.detail != NotifyInferior)
			{
				XSetInputFocus(dpy, NoFocusWin,
					       /*RevertToNone, */
					       RevertToParent,
					       event->xcrossing.time);
			}
		}  /* End if FocusFollowsMouse */
		break;

#ifdef NOTDEF
	case FocusIn:
		/* If we get a focus in event because our child
		 * died, set the focus to the no focus window.
		 * I can't think of an instance when this would
		 * happen and our frame window wasn't killed
		 * at the same time.
	 	 * 	(RevertToPointerRoot is cast from long to 
		 *	int - lint will warn)
		 */
		if (event->xfocus.detail == NotifyInferior)
		 	XSetInputFocus(dpy, NoFocusWin, RevertToPointerRoot,
				       CurrentTime);
		break;
#endif /* NOTDEF */

	default:
		if (PrintOrphans)
			DebugEvent(event, "EventFrame");
		break;
	} /* End switch (event->type) */
}

/*
 * DrawFrame -- draw the frame window.
 */
static
DrawFrame(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);

	if (!f3dUsed)
		XDrawRectangle(dpy, winInfo->self, DrawFrameGC,
			       FRAME_OUTLINE_WIDTH/2, FRAME_OUTLINE_WIDTH/2,
			       winInfo->width - FRAME_OUTLINE_WIDTH,
			       winInfo->height - FRAME_OUTLINE_WIDTH);
	else
		olgx_draw_box(olgx_gismisc, winInfo->self,
			      0, 0, winInfo->width, winInfo->height,
			      frameInfo->isSelected ?
					OLGX_INVOKED : OLGX_NORMAL, True);
	
	if (!f3dUsed)
	{
		if(frameInfo->isSelected == True)
		{
			XDrawRectangle(dpy, winInfo->self, DrawNormalGC,
			       FRAME_OUTLINE_WIDTH + FRAME_SELECTED_WIDTH/2, 
			       FRAME_OUTLINE_WIDTH + FRAME_SELECTED_WIDTH/2,
			       winInfo->width - FRAME_OUTLINE_WIDTH*2 -
			       FRAME_SELECTED_WIDTH,
			       winInfo->height - FRAME_OUTLINE_WIDTH*2 - 
			       FRAME_SELECTED_WIDTH);
		}
		else
		{

			/* clear any select frame */
			XDrawRectangle(dpy, winInfo->self, DrawReverseGC,
			       FRAME_OUTLINE_WIDTH + FRAME_SELECTED_WIDTH/2, 
			       FRAME_OUTLINE_WIDTH + FRAME_SELECTED_WIDTH/2,
			       winInfo->width - FRAME_OUTLINE_WIDTH*2 - 
			       FRAME_SELECTED_WIDTH,
			       winInfo->height - FRAME_OUTLINE_WIDTH*2 -
			       FRAME_SELECTED_WIDTH);
		}
	}


	/* If this window is focused, draw in the what is necessary
	 * to show that it has the focus.  This is necessary because
	 * highlighting the header, or drawing the pair of lines in
	 * the header, to show focus changes both the titlebar and
	 * the frame.
	 */
	if (!f3dUsed)
	{
		if (CheckFocus(winInfo->frame))
		{
			if (FocusFollowsMouse)
			{
				/* Draw the lines to show that this 
				 * window has the focus. */

				/* Draw the bottom two lines. */
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  LEFT_EDGE, 21,
					  winInfo->width - RIGHT_EDGE - 1, 21);
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  LEFT_EDGE, 22,
					  winInfo->width - RIGHT_EDGE - 1, 22);

				/* The top double lines are moved one
				 * pixel down if this window is selected.
				 */
				if(frameInfo->isSelected == True)
				{

					/* The top double lines are shorter
					 * if this window has resize corners.
					 */
					if (frameInfo->winDecors->flags & 
					    WMDecorationResizeable)
					{
					/* Erase the topmost line if it was
					 * there before.
					 */
					XDrawLine(dpy, winInfo->self,
						  DrawReverseGC,
						  12, 3,
						  winInfo->width-12-1, 3);

					XDrawLine(dpy, winInfo->self,
						  DrawNormalGC,
						  12, 4,
						  winInfo->width-12-1, 4);
					/* The second line is inside the titlebar
					 * window.
					 */
					}  /* End if win has resize corners. */
					else
					{
					/* Erase the topmost line if it was
					 * there before.
					 */
					XDrawLine(dpy, winInfo->self,
						  DrawReverseGC,
						  LEFT_EDGE, 3,
						  winInfo->width-RIGHT_EDGE-1,3);

					XDrawLine(dpy, winInfo->self,
						  DrawNormalGC,
						  LEFT_EDGE, 4,
						  winInfo->width-RIGHT_EDGE-1,4);
					/* The second line is inside
					 * the titlebar window.
					 */
					}  /* End else no resize corners. */
				}
				else  /* Not selected. Lines are in frame. */
				{
					/* The top double lines are shorter
					 * if this window has resize corners.
					 */
					if (frameInfo->winDecors->flags & 
					    WMDecorationResizeable)
					{
					XDrawLine(dpy, winInfo->self,
						  DrawNormalGC,
						  12, 3,
						  winInfo->width - 12 - 1, 3);
					XDrawLine(dpy, winInfo->self,
						  DrawNormalGC,
						  12, 4,
						  winInfo->width - 12 - 1, 4);
					}
					else
					{
					XDrawLine(dpy, winInfo->self,
						  DrawNormalGC,
						  LEFT_EDGE, 3,
						  winInfo->width-RIGHT_EDGE-1,3);
					XDrawLine(dpy, winInfo->self,
						  DrawNormalGC,
						  LEFT_EDGE, 4,
						  winInfo->width-RIGHT_EDGE-1,4);
					}
				}
			}
			else  /* Click to focus */
			{
				/* Draw in the extra line of pixels around 
				 * the sides of the highlighted titlebar. */
				 /*
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  11, LEFT_EDGE,
					  winInfo->width - 11, LEFT_EDGE);
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  LEFT_EDGE, 21,
					  winInfo->width - RIGHT_EDGE - 1, 21);
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  LEFT_EDGE, 11,
					  LEFT_EDGE, 21);
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  winInfo->width - RIGHT_EDGE - 1, 11,
					  winInfo->width - RIGHT_EDGE - 1, 21);
				*/
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  LEFT_EDGE, TOP_EDGE - 1,
					  winInfo->width - LEFT_EDGE - 1,
					  TOP_EDGE - 1);
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  LEFT_EDGE, 21,
					  winInfo->width - RIGHT_EDGE - 1, 21);
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  LEFT_EDGE, TOP_EDGE - 1,
					  LEFT_EDGE, 21);
				XDrawLine(dpy, winInfo->self, DrawNormalGC,
					  winInfo->width - RIGHT_EDGE - 1,
					  TOP_EDGE - 1,
					  winInfo->width - RIGHT_EDGE - 1, 21);

			}
		}
		else  /* The frame does not have the focus. */
		{
			if (FocusFollowsMouse)
			{
				/* Erase the focus lines. */

				/* Erase the bottom two lines. */
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  LEFT_EDGE, 21,
					  winInfo->width - RIGHT_EDGE - 1, 21);
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  LEFT_EDGE, 22,
					  winInfo->width - RIGHT_EDGE - 1, 22);

				/* The top double lines are moved one
				 * pixel vertically depending upon whether
				 * this window is selected.
				 */
				if(frameInfo->isSelected == True)
				{
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  LEFT_EDGE, 4,
					  winInfo->width - RIGHT_EDGE, 4);
				/* The second line is inside the titlebar
				 * window.
				 */
				}
				else
				{
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  LEFT_EDGE, 3,
					  winInfo->width - RIGHT_EDGE - 1, 3);
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  LEFT_EDGE, 4,
					  winInfo->width - RIGHT_EDGE - 1, 4);
				}
			}
			else
			{
				/* Erase the extra line of pixels below 
				 * and at the sides of the highlighted titlebar.
				 */
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  LEFT_EDGE, TOP_EDGE - 1,
					  winInfo->width - LEFT_EDGE - 1,
					  TOP_EDGE - 1);
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  LEFT_EDGE, 21,
					  winInfo->width - RIGHT_EDGE - 1, 21);
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  LEFT_EDGE, TOP_EDGE - 1,
					  LEFT_EDGE, 21);
				XDrawLine(dpy, winInfo->self, DrawReverseGC,
					  winInfo->width - RIGHT_EDGE - 1,
					  TOP_EDGE - 1,
					  winInfo->width - RIGHT_EDGE - 1, 21);
			}
		}  /* End Else */

		/* Because the indication of selection also effects the title
		 * bar if we are in focus follows mouse mode call the titlebar
		 * drawing routine if this frame has a titlebar.
		 */
		if (FocusFollowsMouse && (TITLECHILDINFO(frameInfo))) 
			DrawTitleBar(dpy, TITLECHILDINFO(frameInfo));
	}

}  /* End Draw Frame */

/*
 * ConfigFrame -- reconfigure the size/position of a frame
 *  This also tells the frames children to resize themselves.
 */
static
ConfigFrame(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	XWindowChanges	wc;
	unsigned int	mask;
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);
	WinInfo		*paneInfo = WIGetInfo(frameInfo->pane);
	XConfigureEvent ce;

#ifdef DEBUG
	if ( frameInfo->configMask == 0 ) {
	    fprintf(stderr,
		    "olwm: internal error: ConfigureWindow mask == 0\n");
	}
#endif /* DEBUG */

/*
 * The code within the following #ifdef is necessary to work around a bug in
 * some older versions of the DECwindows X server.  This bug will crash the
 * server when a window is resized.  If you have a server with a fix for this
 * bug, you can safely remove this ifdef and the one about 90 lines below.
 */
#ifdef ultrix
	XUnmapWindow(dpy, winInfo->self);
#endif /* ultrix */

	if (frameInfo->configMask & (CWWidth|CWHeight)) {
	    /*
	     * We are reshaping, not just repositioning.  First reshape the 
	     * frame, then the client.
	     */
	    mask = frameInfo->configMask;
	    if ( mask & CWX )
		wc.x = winInfo->x;
	    if ( mask & CWY )
		wc.y = winInfo->y;
	    if ( mask & CWWidth )
		wc.width = winInfo->width;
	    if ( mask & CWHeight )
		wc.height = winInfo->height;
	    XConfigureWindow(dpy, winInfo->self, mask, &wc);

	    /* configure the client window */
	    mask = CWX | CWY | CWWidth | CWHeight; 

	    wc.x = LEFT_EDGE;
	    wc.y = TOP_EDGE;
	    wc.width = winInfo->width - RIGHT_EDGE - LEFT_EDGE;
	    wc.height = winInfo->height - TOP_EDGE - BOTTOM_EDGE;

	    /*
	     * REMIND
	     * This is a kludge because its not clear why its needed.  The
	     * *EDGE numbers seem inconsistent in their use.  Are they the gap
	     * between or the starting pixel????
	     */
	    wc.height -= 2;

	    /* does it have a footer */
	    if (frameInfo->winDecors->flags & WMDecorationFooter)
		    wc.height -= FOOTER_HEIGHT;

	    /* does it have a header */
	    if (frameInfo->winDecors->flags & WMDecorationHeader)
	    {
		    wc.height -= HEADER_HEIGHT;
		    wc.y += HEADER_HEIGHT;
	    }

	    /*
	     * Update the pane information with the new size.
	     * We should really update this only when we receive a
	     * ConfigureNotify event on the pane.
	     */
	    paneInfo->x = wc.x;
	    paneInfo->y = wc.y;
	    paneInfo->width = wc.width;
	    paneInfo->height = wc.height;
	    XConfigureWindow(dpy, frameInfo->pane, mask, &wc);

	} else {
	    /*
	     * We are repositioning.  Send a synthetic ConfigureNotify event.
	     */
	    ce.type = ConfigureNotify;
	    ce.serial = 0L;
	    ce.event = frameInfo->pane;
	    ce.window = frameInfo->pane;
	    ce.x = winInfo->x + paneInfo->x - frameInfo->oldBorderWidth;
	    ce.y = winInfo->y + paneInfo->y - frameInfo->oldBorderWidth;
	    ce.width = paneInfo->width;
	    ce.height = paneInfo->height;
	    ce.border_width = frameInfo->oldBorderWidth;
	    ce.above = None;
	    ce.override_redirect = False;

	    XSendEvent(dpy, frameInfo->pane, False, StructureNotifyMask,&ce);

	    /*
	     * REMIND
	     * This does not handle stack_mode.
	     */
	    mask = frameInfo->configMask & (CWX | CWY);
	    if ( mask & CWX )
		wc.x = winInfo->x;
	    if ( mask & CWY )
		wc.y = winInfo->y;
	    XConfigureWindow(dpy, winInfo->self, mask, &wc);
	}

#ifdef ultrix
	XMapWindow(dpy, winInfo->self);
#endif /* ultrix */

	/* call the childrens configfunc */

	/* if we have a footer child with a configfunc call it */
	if ((FOOTERCHILDINFO(frameInfo)) &&
	    (FOOTERCHILDINFO(frameInfo)->configfunc))
	{
		(*FOOTERCHILDINFO(frameInfo)->configfunc)(dpy, 
			FOOTERCHILDINFO(frameInfo),	/* child's winInfo */
			LEFT_EDGE, 
			winInfo->height - FOOTER_HEIGHT - BOTTOM_EDGE,
			winInfo->width - LEFT_EDGE - RIGHT_EDGE,
			FOOTER_HEIGHT);
	}

	/* if we have a title child with a configfunc call it */
	if ((TITLECHILDINFO(frameInfo)) &&
	    (TITLECHILDINFO(frameInfo)->configfunc))
	{
		(*TITLECHILDINFO(frameInfo)->configfunc)(dpy, 
			TITLECHILDINFO(frameInfo),	/* child's winInfo */
			LEFT_EDGE +1, TOP_EDGE, 	/* x, y */
			winInfo->width - RIGHT_EDGE - LEFT_EDGE - (2*1), /* width */
			TITLE_HEIGHT);				/* height */
	}

	frameInfo->configMask = 0L;
}

/*
 * DestroyFrame --  called when the frame is to be destroyed.  This will
 *	need to call the children to destroy themselves.
 */
static
DestroyFrame(dpy, winInfo)
Display	*dpy;
WinInfo *winInfo;
{
	FrameInfo	*frameInfo;
	int		ii;
	FollowBlock	*next, *followBlock;

	/*  we always de-select the window */
	RemoveSelection(dpy, winInfo->frame);

	/* remove frame from list of frames */
	RemoveFrame(winInfo);

	/* call the children to destroy themselves */
	frameInfo = (FrameInfo *)(winInfo->priv.ptr);
	for(ii=0; ii < MAX_CHILDREN; ii++)
		if ((frameInfo->children.info[ii]) &&
		    (frameInfo->children.info[ii]->destroyfunc))
			(*frameInfo->children.info[ii]->destroyfunc)(dpy, 
				frameInfo->children.info[ii]);
		
	for(followBlock = frameInfo->followers;
	    followBlock;
	    followBlock = next)
	{
		for ( ii=0; ii < FOLLOWNUM; ++ii ) {
		    if ( followBlock->followers[ii] != (WinInfo *)0 ) {
			FrameInfo *follower = (FrameInfo *)
			    followBlock->followers[ii]->frame->priv.ptr;
			follower->leader = (WinInfo *) 0;
			followBlock->followers[ii] = (WinInfo *) 0;
		    }
		}
		next = followBlock->next;
		/* lint will complain about this cast */
		free((char *)followBlock);
	}


	/* free up our malloced data */
	/* lint will complain about this cast */
	free(frameInfo->winDecors);
	free((char *)(winInfo->priv.ptr));

	/* destroy our window */
	XDestroyWindow(dpy, winInfo->self);

	/* get us out of the hash table */
	WIDeleteInfo(winInfo->self);
}

/*
 * PropFrame -- called when a property has changed in the pane.  Calls
 *	sub-functions to allow them to determine if they care about the
 *	event or not.
 */
static
PropFrame(dpy, winInfo, pEvent)
Display *dpy;
WinInfo *winInfo;
XEvent  *pEvent;
{
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);
	int		ii;
	char		*name;
	Atom 		actType;
	int		actFormat;
	unsigned long	nItems, remain;

	if (pEvent->xproperty.atom == AtomRightFooter)
	{
		/* read the right footer (if any) */
		XGetWindowProperty(dpy, 
				   GetPane(winInfo->frame), 
				   AtomRightFooter,
				   0L, 
				   FOOTLEN, 
				   False, 
				   (Atom)AnyPropertyType, 
				   &actType, 
				   &actFormat, 
				   &nItems, 
				   &remain, 
				   (unsigned char **)&(frameInfo->rightfooter));
	} 
	else if (pEvent->xproperty.atom == AtomLeftFooter)
	{
		/* read the right footer (if any) */
		XGetWindowProperty(dpy, 
				   GetPane(winInfo->frame), 
				   AtomLeftFooter,
				   0L, 
				   FOOTLEN, 
				   False, 
				   (Atom)AnyPropertyType, 
				   &actType, 
				   &actFormat, 
				   &nItems, 
				   &remain, 
			   	   (unsigned char **)&(frameInfo->leftfooter));
	}
	else if (pEvent->xproperty.atom == AtomColorMapWindows)
	{
		/* WM_COLORMAP_WINDOWS changed */
		if (pEvent->xproperty.state == PropertyNewValue)
		{
			trackSubwindows(dpy,
					GetPane(winInfo->frame),
					winInfo->frame);
		}
		else		/* PropertyDelete */
		{
			/* Not handled yet ??? */
		}
	}
	else
	{
		if (pEvent->xproperty.atom == XA_WM_NAME)
		{
			if ((pEvent->xproperty.state == PropertyNewValue) &&
			   (XFetchName(dpy, frameInfo->pane, &name)))
				frameInfo->name = name;
			else
				frameInfo->name = DEFAULTWINNAME;
		}
		else if (pEvent->xproperty.atom == XA_WM_ICON_NAME)
		{
			if ((pEvent->xproperty.state == PropertyNewValue) &&
			   (XGetIconName(dpy, frameInfo->pane, &name)))
				frameInfo->iconname = name;
			else
				frameInfo->iconname = DEFAULTWINNAME;
		}
	}

	/* call the children to look at property change */
	for(ii=0; ii < MAX_CHILDREN; ii++)
		if ((frameInfo->children.info[ii])
		 && (frameInfo->children.info[ii]->propfunc))
			(*frameInfo->children.info[ii]->propfunc)(dpy, 
					frameInfo->children.info[ii],
						pEvent);
}


/*
 * FocusFrame --  called when the focus is changing to or away from this frame.
 *	This calls all of the children, and tells them to [un]focus.
 */
static
FocusFrame(dpy, winInfo, setFocus)
Display	*dpy;
WinInfo *winInfo;
Bool	setFocus;
{
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);
	int		ii;

	/* set new focus for this frame */
	((FrameInfo *)(winInfo->priv.ptr))->isFocus = setFocus;

	/* The focus is noted in the titlebar as well as the
	 * frame so redraw the frame, which will also call
	 * DrawTitleBar.
	 */
	DrawFrame(dpy, winInfo); 

	/* call the children to focus or unfocus themselves */
	frameInfo = (FrameInfo *)winInfo->priv.ptr;
	for(ii=0; ii < MAX_CHILDREN; ii++)
		if ((frameInfo->children.info[ii]) &&
		   (frameInfo->children.info[ii]->focusfunc))
			(*frameInfo->children.info[ii]->focusfunc)(dpy, 
						frameInfo->children.info[ii],
						setFocus);
}

WMDecorations BaseWindow = 
{
	WMDecorationCloseButton | WMDecorationResizeable | WMDecorationHeader,
	MENU_FULL,
	PinIn
};

WMDecorations CmdWindow =
{
      WMDecorationCloseButton | WMDecorationResizeable | WMDecorationHeader |
      WMDecorationPushPin,
      MENU_LIMITED,
      PinIn
};

WMDecorations PropWindow =
{
      WMDecorationCloseButton | WMDecorationResizeable |
      WMDecorationHeader | WMDecorationPushPin,
      MENU_LIMITED,
      PinIn
};

WMDecorations NoticeWindow =
{
      WMDecorationHeader,
      MENU_NONE,
      PinIn
};

WMDecorations HelpWindow =
{
      WMDecorationCloseButton | WMDecorationHeader,
      MENU_LIMITED,
      PinIn
};

WMDecorations OtherWindow =
{
      0L,
      MENU_NONE,
      PinIn
};

WMDecorations TransientWindow =
{
      0L,
      MENU_NONE,
      PinIn
};
	
WMDecorations *
GetOLWinDecors(dpy, win, pwinType, transient)
Display	*dpy;
Window	win;
int	*pwinType;
{
	WMDecorations   *decors;
	Atom    actType;
	int     actFormat;
	unsigned        long    nItems, remain;
	long            offset;
	Atom    *decorChange;
	int	status;
	struct
	{
	      Atom    win_type;
	      Atom    menu_type;
	      Atom    pin_initial_state;
	} *winAttrs;

	decors = (WMDecorations *)IntMalloc((unsigned)sizeof(WMDecorations));

	XGetWindowProperty(dpy,
			 win,
			 AtomWinAttr,
			 0L,
			 3*LONG_LENGTH(Atom),	/* one winAttrs struct */
			 False,
			 (Atom)AnyPropertyType,
			 &actType,
			 &actFormat,
			 &nItems,
			 &remain,
			 /* lint will complain about this cast */
			 (unsigned char **)&winAttrs);

	/* if the property does not exist, then we assume that the
	* window is a base window
	*/
	if (((actType == None) && (actFormat == 0)) || (nItems != 3))
	{
		if (transient)
		{
			*decors = TransientWindow;
			*pwinType = TRANSIENTWINDOW;
		}
		else
		{
			*decors = BaseWindow;
			*pwinType = BASEWINDOW;
		}
	}
	else
	{
		if (winAttrs->win_type == AtomWTBase)
		{
			*decors = BaseWindow;
			*pwinType = BASEWINDOW;
		}
		else if (winAttrs->win_type == AtomWTCmd)
		{
			*decors = CmdWindow;
			*pwinType = CMDWINDOW;
		}
		else if (winAttrs->win_type == AtomWTProp)
		{
			*decors = PropWindow;
			*pwinType = PROPWINDOW;
		}
		else if (winAttrs->win_type == AtomWTHelp)
		{
			*decors = HelpWindow;
			*pwinType = HELPWINDOW;
		}
		else if (winAttrs->win_type == AtomWTNotice)
		{
			*decors = NoticeWindow;
			*pwinType = NOTICEWINDOW;
		}
		else if (winAttrs->win_type == AtomWTOther)
		{
			*decors = OtherWindow;
			*pwinType = OTHERWINDOW;

			/* this is the only case where we look at menu type */
			if (winAttrs->menu_type == AtomMenuFull)
				decors->menu_type = MENU_FULL;
			else if (winAttrs->menu_type == AtomMenuLimited)
				decors->menu_type = MENU_LIMITED;
			else if (winAttrs->menu_type == AtomNone)
				decors->menu_type = MENU_NONE;
		}

		/* check pushpin initial state */
		if (winAttrs->pin_initial_state == AtomPinIn)
			decors->pushpin_initial_state = PinIn;
		else if (winAttrs->pin_initial_state == AtomPinOut)
			decors->pushpin_initial_state = PinOut;
		else if (winAttrs->pin_initial_state == AtomNone)
			decors->flags &= ~(WMDecorationPushPin);
	}


	/* are there any non-standard decorations ? */
	/* first check for decorations added */
	offset = 0L;
	for(;;)
	{
		status = XGetWindowProperty(dpy,
					win,
					AtomDecorAdd,
					offset,
					LONG_LENGTH(Atom),
					False,
					(Atom)AnyPropertyType,
					&actType,
					&actFormat,
					&nItems,
					&remain,
			 		/* lint will complain about this cast */
					(unsigned char **)&decorChange);

		if ((nItems == 0) || (status != Success))
			break;

		offset++;

		if (*decorChange == AtomDecorClose)
			decors->flags |= WMDecorationCloseButton;
		else if (*decorChange == AtomDecorFooter)
			decors->flags |= WMDecorationFooter;
		else if (*decorChange == AtomDecorResize)
			decors->flags |= WMDecorationResizeable;
		else if (*decorChange == AtomDecorHeader)
			decors->flags |= WMDecorationHeader;
		else if (*decorChange == AtomDecorPin)
			decors->flags |= WMDecorationPushPin;
	}

	/* now check for decorations deleted */
	offset = 0L;
	for(;;)
	{
		status = XGetWindowProperty(dpy,
					win,
					AtomDecorDelete,
					offset,
					LONG_LENGTH(Atom),
					False,
					(Atom)AnyPropertyType,
					&actType,
					&actFormat,
					&nItems,
					&remain,
			 		/* lint will complain about this cast */
					(unsigned char **)&decorChange);

		if ((nItems == 0) || (status != Success))
			break;

		offset++;

		if (*decorChange == AtomDecorClose)
			decors->flags &= ~(WMDecorationCloseButton);
		else if (*decorChange == AtomDecorFooter)
			decors->flags &= ~(WMDecorationFooter);
		else if (*decorChange == AtomDecorResize)
			decors->flags &= ~(WMDecorationResizeable);
		else if (*decorChange == AtomDecorHeader)
			decors->flags &= ~(WMDecorationHeader);
		else if (*decorChange == AtomDecorPin)
			decors->flags &= ~(WMDecorationPushPin);
	}

	/* If the window wants both a pushpin and a close button it
	* only can have a pushpin.
	*/
	if ((decors->flags & WMDecorationCloseButton) &&
	    (decors->flags & WMDecorationPushPin)) 
		decors->flags &= ~(WMDecorationCloseButton);

	return  decors;
}

#ifdef NOTDEF
/*
 * ListFullAction is obsolete.  It was used when menu items operated on the 
 * selection.  It may someday be useful for Window Controls.
 */

/*
 * ListFullAction --  routine called when a menu action is selected
 *	from the full frame menu. We step through all the selected windows
 *	and perform the required action.
 */
ListFullAction(dpy, winInfo, menu, idx, pinnedMenu)
Display	*dpy;
WinInfo	*winInfo;
Menu	*menu;
int	idx;
Bool	pinnedMenu;
{
	WinInfo	*tempInfo = (WinInfo *)0;

	if (pinnedMenu || IsSelected(winInfo))
		while(tempInfo = EnumSelections(tempInfo))
			FrameFullAction(dpy, tempInfo, menu, idx, pinnedMenu);
	else
	{
		FrameFullAction(dpy, winInfo, menu, idx, pinnedMenu);
	}
}
#endif /* NOTDEF */


/*
 * FrameFullAction -- routine called for each window when a menu action is 
 *	selected from the full frame menu.
 */
FrameFullAction(dpy, winInfo, menu, idx, pinnedMenu)
Display	*dpy;
WinInfo	*winInfo;
Menu	*menu;
int	idx;
Bool	pinnedMenu;
{
	FrameInfo	*frameInfo;
	XEvent		clientEvent;
	Window		w;
	int		width, height;

#define	FFA_CLOSE		0
#define	FFA_FULLSIZE		1
#define	FFA_PROPERTIES		2
#define	FFA_BACK		3
#define	FFA_REFRESH		4
#define	FFA_QUIT		5

	frameInfo = (FrameInfo *)(winInfo->frame->priv.ptr);

	switch(idx)

	{
	case FFA_CLOSE:
		Iconify(dpy, winInfo);
		break;

	case FFA_FULLSIZE:
		/* we need to make the window as big as it can be.
		 * We determine this value by looking at the hints
		 * for the max value.
		 */

		if (*menu->buttons[FFA_FULLSIZE].label == 'F')	/* full size */
			FullSize(dpy, winInfo);
		else					/* norm size */
			NormSize(dpy, winInfo);
		break;

	case FFA_PROPERTIES:
		/* send a ClientMessage of type WM_SHOW_PROPERTIES */
		clientEvent.xclient.type = ClientMessage;
		clientEvent.xclient.message_type = AtomShowProperties;
		clientEvent.xclient.format = 32;
		clientEvent.xclient.display = dpy;
		clientEvent.xclient.window = GetPane(winInfo->frame);

		/* send the event */
		XSendEvent(dpy, clientEvent.xclient.window, False,
			   NoEventMask, &clientEvent);
		break;

	case FFA_BACK:
		/* lower the window */
		XLowerWindow(dpy, winInfo->frame->self);
		break;

	case FFA_REFRESH:
		/* refresh the window.  We do this by creating a window on top
		 * of the window to refresh and then immediately destroy it
		 */
		GetUnFramedSize(winInfo->frame, &width, &height);

		w = XCreateSimpleWindow(dpy, GetPane(winInfo->frame), 0, 0,
				   width, height, 0,
				   (Pixmap) 0, (Pixmap) 0);
		XMapRaised(dpy, w);
		XDestroyWindow(dpy, w);
		XFlush(dpy);
		break;

	case FFA_QUIT:
		/* If the client has WM_DELETE_WINDOW in its
		 * protocols atom we need to send a client message.
		 */
		if (frameInfo->protocols & DELETE_WINDOW)
		{
			/* send a ClientMessage of type WM_DELETE_WINDOW */
			/* this will do nothing on non-open look clients */
			clientEvent.xclient.type = ClientMessage;
			clientEvent.xclient.message_type = AtomProtocols;
			clientEvent.xclient.format = 32;
			clientEvent.xclient.display = dpy;
			clientEvent.xclient.window = GetPane(winInfo->frame);
			clientEvent.xclient.data.l[0] = AtomDeleteWindow;

			/* This should really be the time of the event. */
			clientEvent.xclient.data.l[1] = CurrentTime;

			/* send the event */
			XSendEvent(dpy, 
				   clientEvent.xclient.window, 
				   False,
				   NoEventMask, 
				   &clientEvent);
		}
		else	/* messy but guaranteed disposal of client */
			XKillClient(dpy, GetPane(winInfo->frame));
		break;
	}
}


#ifdef NOTDEF
/*
 * ListLimAction is obsolete.  It was used when menu items operated on the 
 * selection.  It may someday be useful for Window Controls.
 */

/*
 * ListLimAction --  routine called when a menu action is selected
 *	from the limited frame menu. We step through all the selected windows
 *	and perform the required action.
 */
ListLimAction(dpy, winInfo, menu, idx, pinnedMenu)
Display	*dpy;
WinInfo	*winInfo;
Menu	*menu;
int	idx;
Bool	pinnedMenu;
{
	WinInfo	*tempInfo = (WinInfo *)0;

	if (pinnedMenu || IsSelected(winInfo))
		while(tempInfo = EnumSelections(tempInfo))
			FrameLimAction(dpy, tempInfo, menu, idx, pinnedMenu);
	else
	{
		ClearSelections(dpy);
		AddSelection(dpy, winInfo);
		FrameLimAction(dpy, winInfo, menu, idx, pinnedMenu);
	}
}
#endif /* NOTDEF */


/*
 * FrameLimAction -- routine called when a menu action is selected
 *	from the limited frame menu.
 */
/*ARGSUSED*/	/* menu arg will be used later */
FrameLimAction(dpy, winInfo, menu, idx, pinnedMenu)
Display	*dpy;
WinInfo	*winInfo;
Menu	*menu;
int	idx;
Bool	pinnedMenu;
{
	XEvent		clientEvent;
	Window		w;
	int		width, height;

#define	FLM_DISMISS		0
#define	FLM_BACK		1
#define	FLM_REFRESH		2
#define	FLM_OWNER		3

	switch(idx)
	{
	case FLM_DISMISS:
		/*
		 * REMIND
		 * This is probably redundant with DismissAction
		 */
		/* send a ClientMessage of type WM_DELETE_WINDOW */
		if (GetLeader(winInfo))	/* make sure is pop-up */
		{
			clientEvent.xclient.type = ClientMessage;
			clientEvent.xclient.message_type = AtomProtocols;
			clientEvent.xclient.format = 32;
			clientEvent.xclient.display = dpy;
			clientEvent.xclient.window = GetPane(winInfo->frame);
			clientEvent.xclient.data.l[0] = AtomDeleteWindow;

			/* send the event */
			XSendEvent(dpy, 
				clientEvent.xclient.window, 
				False,
				NoEventMask, 
				&clientEvent);
		}
		break;

	case FLM_BACK:
		/* lower the window */
		XLowerWindow(dpy, winInfo->frame->self);
		break;

	case FLM_REFRESH:
		/* refresh the window.  We do this by creating a window on top
		 * of the window to refresh and then immediately destroy it
		 */
		GetUnFramedSize(winInfo->frame, &width, &height);

		w = XCreateSimpleWindow(dpy, GetPane(winInfo->frame), 0, 0,
				   width, height, 0,
				   (Pixmap) 0, (Pixmap) 0);
		XMapRaised(dpy, w);
		XDestroyWindow(dpy, w);
		XFlush(dpy);
		break;
	case FLM_OWNER:
		/* find the owner of this pop-up */
		if (winInfo = GetLeader(winInfo))   /* make sure is pop-up */
		{
			XRaiseWindow(dpy, winInfo->frame->self);
			XFlush(dpy);

			FlashTitleBar(dpy, 
				TITLECHILDINFO(
				  (FrameInfo *)(winInfo->frame->priv.ptr)
				)
			);
		}
		break;
	}
}


/*
 * InsertFollower  --- passed a leader winInfo structure and a follower
 *	winInfo structure.  Records the follower in the leaders follower
 *	data structure.
 */
InsertFollower(leadInfo, folInfo)
WinInfo	*leadInfo, *folInfo;
{
	FollowBlock	**pfollowBlock;
	int		ii;

	if (leadInfo)
	{
		leadInfo = leadInfo->frame;
		pfollowBlock = &(((FrameInfo *)(leadInfo->priv.ptr))->followers);

		while (*pfollowBlock)
		{
			for(ii=0 ; ii < FOLLOWNUM; ii++)
			{
				if (!((*pfollowBlock)->followers[ii]))
				{
					/* open slot, insert */
					(*pfollowBlock)->followers[ii]=folInfo;
					return;
				}
			}

			/* look at next follow block in chain */
			pfollowBlock = &((*pfollowBlock)->next);
		}

		/*  didn't find empty slot.  We need to allocate a new
		 * FollowBlock and place the follower in it 
		 */
		*pfollowBlock = 
		      (FollowBlock *)IntMalloc((unsigned)sizeof(FollowBlock));
		(*pfollowBlock)->followers[0] = folInfo;
	}
}


/*
 * DeleteFollower  --- passed a leader winInfo structure and a follower
 *	winInfo structure.  Delete the follower in the leaders follower
 *	data structure.
 */
DeleteFollower(leadInfo, folInfo)
WinInfo	*leadInfo, *folInfo;
{
	FollowBlock	**pfollowBlock;
	int		ii;

	if (leadInfo)
	{
		/* make sure we are pointing at the frame and not the pane */
		leadInfo = leadInfo->frame;

		pfollowBlock = &(((FrameInfo *)(leadInfo->priv.ptr))->followers);

		while (*pfollowBlock)
		{
			for(ii=0 ; ii < FOLLOWNUM; ii++)
			{
				if ((*pfollowBlock)->followers[ii] == folInfo)
				{
					/* clear entry */
					(*pfollowBlock)->followers[ii] = 0;
					return;
				}
			}

			/* look at next follow block in chain */
			pfollowBlock = &((*pfollowBlock)->next);
		}
	}
}


/*
 * SelectFrame -- based on the value of select, make this frame selected
 *	or not.
 */
SelectFrame(dpy, winInfo, select)
Display	*dpy;
WinInfo	*winInfo;
Bool	select;
{
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);
	int		ii;

	frameInfo->isSelected = select;

	DrawFrame(dpy, winInfo);

	/* call the children to handle select loss/gain */
	frameInfo = (FrameInfo *)(winInfo->priv.ptr);
	for(ii=0; ii < MAX_CHILDREN; ii++)
		if ((frameInfo->children.info[ii]) &&
		   (frameInfo->children.info[ii]->selectfunc))
			(*frameInfo->children.info[ii]->selectfunc)(dpy, 
						frameInfo->children.info[ii],
						select);
}

/*
 * FrameInBox -- determine if the frame in its current state is within
 *	the passed bounding boxes.  If so, then call passed function on it.
 */
FrameInBox(dpy, winInfo, function, bx, by, bw, bh, timestamp)
Display	*dpy;
WinInfo	*winInfo;
int	(*function)();
short	bx, by, bw, bh;
Time	timestamp;
{
	switch(winInfo->winState)
	{
	case Iconic: /* we is iconic */
		winInfo = GetIcon(winInfo);
		/* fall through */
	case Normal: /* this means we are not iconic */
		if ((winInfo->x >= bx) && (winInfo->y >= by) &&
		    (winInfo->x + winInfo->width <= bx + bw) && 
		    (winInfo->y + winInfo->height <= by + bh))
			(*function)(dpy, winInfo->frame, timestamp);
		break;
	}
}

/* 
 * FullSize -- make the frame full size
 */
FullSize(dpy, winInfo)
Display		*dpy;
WinInfo		*winInfo;
{
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);

	/* substite "normal size" menu for "full size" menu */
	frameInfo->frameMenu = &FrameNormMenu;

	/* save restore size attributes */
	frameInfo->restoreWidth = winInfo->width;
	frameInfo->restoreHeight = winInfo->height;
	frameInfo->restoreY = winInfo->y;

	/* if there is a program specified max size */
	if ((frameInfo->normHints) && 
	    (frameInfo->normHints->flags & PMaxSize))
	{
		winInfo->width = frameInfo->normHints->max_width;
		winInfo->height = frameInfo->normHints->max_height;
	}
	else
	{
		/* move window to top of screen and change the
		 * height to Display Height */
		winInfo->y = 0;
		winInfo->height = DisplayHeight(dpy, DefaultScreen(dpy));
	}

	/* call the frame's config func to register new size */
	frameInfo->configMask = CWY | CWWidth | CWHeight;
	(*winInfo->configfunc)(dpy, winInfo);
}

/*
 * NormSize -- restore the frame to normal size
 */
NormSize(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->priv.ptr);

	/* substite "full size" menu for "normal size" menu */
	frameInfo->frameMenu = &FrameFullMenu;

	/* restore from saved values */
	winInfo->y = frameInfo->restoreY;
	winInfo->height = frameInfo->restoreHeight;
	winInfo->width = frameInfo->restoreWidth;

	/* call the frame's config func to register new size */
	frameInfo->configMask = CWY | CWWidth | CWHeight;
	(*winInfo->configfunc)(dpy, winInfo);
}

#ifdef NOTDEF
/*
 * ListDismissAction is obsolete.  It was used when menu items operated on the 
 * selection.  It may someday be useful for Window Controls.
 */

/*
 * ListDismissAction --  routine called when a menu action is selected
 *	from the dismiss menu. We step through all the selected windows
 *	and perform the required action.
 */
/*ARGSUSED*/	/* pinnedMenu arg will be used later */
ListDismissAction(dpy, winInfo, menu, idx, pinnedMenu)
Display	*dpy;
WinInfo	*winInfo;
Menu	*menu;
int	idx;
Bool	pinnedMenu;
{
	WinInfo	*tempInfo = (WinInfo *)0;

	if (IsSelected(winInfo))
		while(tempInfo = EnumSelections(tempInfo))
			DismissAction(dpy, tempInfo, menu, idx, pinnedMenu);
	else
	{
		ClearSelections(dpy);
		AddSelection(dpy, winInfo);
		DismissAction(dpy, winInfo, menu, idx, pinnedMenu);
	}
}
#endif /* NOTDEF */


/*
 * DismissAction -- called when Dismiss submenu is entered and an item
 *	selected.
 */
/*ARGSUSED*/	/* menu arg will be used later */
DismissAction(dpy, winInfo, menu, idx, pinnedMenu)
Display	*dpy;
WinInfo	*winInfo;
Menu	*menu;
int	idx;
Bool	pinnedMenu;
{
	FollowBlock	*followBlock;
	int		ii;
	XEvent		clientEvent;
	WinInfo		*tempInfo;
#define	DA_THIS		0
#define	DA_POPUPS	1

	switch(idx)
	{
	case DA_THIS:
		if (GetLeader(winInfo))	/* make sure is pop-up */
		{
			/* send a ClientMessage of type WM_DELETE_WINDOW */
			clientEvent.xclient.type = ClientMessage;
			clientEvent.xclient.message_type = AtomProtocols;
			clientEvent.xclient.format = 32;
			clientEvent.xclient.display = dpy;
			clientEvent.xclient.window = GetPane(winInfo->frame);
			clientEvent.xclient.data.l[0] = AtomDeleteWindow;

			/* send the event */
			XSendEvent(dpy, clientEvent.xclient.window, False,
				   NoEventMask, &clientEvent);
		}
		break;

	case DA_POPUPS:
		/*
		 * Find all pop-ups associated with this window, or with
		 * this window's leader, and send them a WM_DELETE_WINDOW
		 * message.
		 */
		clientEvent.xclient.type = ClientMessage;
		clientEvent.xclient.message_type = AtomProtocols;
		clientEvent.xclient.format = 32;
		clientEvent.xclient.display = dpy;
		clientEvent.xclient.data.l[0] = AtomDeleteWindow;

		tempInfo = GetLeader(winInfo);
		if ( tempInfo != NULL )
		    winInfo = tempInfo->frame;

		for(followBlock = GetFollowers(winInfo);
		    followBlock;
		    followBlock = followBlock->next)
		{
		    for(ii=0; ii < FOLLOWNUM; ii++) {
			if (followBlock->followers[ii]) {
			    clientEvent.xclient.window = 
				GetPane(followBlock->followers[ii]);
			    XSendEvent(dpy, clientEvent.xclient.window, 
				       False, NoEventMask, &clientEvent);
			}
		    }
		}
		break;
	}
}

/*
 * InstallColormap	-- make current the given colormap
 *	dpy	- current display pointer
 *	win	- window that uses the colormap
 *	colormap- colormap to be installed
 * No colormap installation is done if the colormap is already installed.
 */
InstallColormap(dpy, win, colormap)
Display		*dpy;
Window		win;
Colormap	colormap;
{
	if ((CurrColormap != colormap) || 
	    (CurrColormapWindow != win))
	{
		XInstallColormap(dpy, colormap);
		CurrColormap = colormap;
		CurrColormapWindow = win;
	}
}
