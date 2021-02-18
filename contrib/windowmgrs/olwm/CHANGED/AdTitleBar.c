/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) AdTitleBar.c 4.1 89/12/06 Crucible";
#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/Xatom.h>
#include "bitmaps/pushpin.h"
#include "bitmaps/windowmark.h"
#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>

/* Externals. */
extern Menu     	FrameFullMenu;
extern int		FlashTime;
extern int		FocusFollowsMouse;
extern Graphics_info	*olgx_gisbold;
extern void		*IntMalloc();
extern Bool		f3dUsed;
extern GC		DrawNormalGC;
extern GC		DrawReverseGC;
extern GC		DrawBusyGC;
extern XFontStruct	*NormalFontInfo;

/* Events in the adornment window that are interesting. */
#define TITLE_EVENT_MASK        ( ExposureMask)

/* Attributes change mask. */
#define ATTR_MASK	(CWEventMask |CWBorderPixel |CWBackPixel)


/*
 * MakeTitleBar  -- create the title bar to a window.  Since window marks
 *	push pins, and menu stacks are windows on top of the title bar, deal
 *	with them as well.  Return a WinInfo structure.
 */
WinInfo *
MakeTitleBar(dpy, frame, winDecors, x, y, titleWidth, titleHeight)
Display	*dpy;
WinInfo	*frame;
WMDecorations	*winDecors;
int	x, y, titleHeight;
int	titleWidth;
{
	int	winstep = 0;	/* Stepping through the winInfo structures. */
	WinInfo	*MakeShineMark();
	WinInfo	*MakePushPin();
	Window	newwin;
	XSetWindowAttributes attributes;
	WinInfo	*winInfo;
	TitleInfo	*titleInfo;
	int	DrawTitleBar(), EventTitleBar(), ConfigTitleBar();
	int	FocusTitleBar(), PropTitleBar();
	int	DestroyTitleBar();
	NewXSizeHints	*hints;


	attributes.event_mask = TITLE_EVENT_MASK;
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	newwin = XCreateWindow(dpy, frame->self,
			 x, y,
			 titleWidth, titleHeight,
			 0,
			 DefaultDepth(dpy, DefaultScreen(dpy)),
			 InputOutput,
			 DefaultVisual(dpy, DefaultScreen(dpy)),
			 ATTR_MASK,
			 &attributes);

	/* Map the title bar. */
	XMapWindow(dpy, newwin);

	XRaiseWindow(dpy, newwin);

	winInfo = WICreateInfo(newwin);
	winInfo->drawfunc = DrawTitleBar;
	winInfo->eventfunc = EventTitleBar;
	winInfo->propfunc = PropTitleBar;
	winInfo->configfunc = ConfigTitleBar;
	winInfo->frame = frame;
	winInfo->destroyfunc = DestroyTitleBar;
	winInfo->focusfunc = FocusTitleBar;
	winInfo->width = titleWidth;
	winInfo->height = titleHeight;
	winInfo->x = x;
	winInfo->y= y;

	/* Make private title bar data. */
	titleInfo = (TitleInfo *)IntMalloc((unsigned)sizeof(TitleInfo));
	winInfo->priv.ptr = titleInfo;

	/* Here we figure out, among other things, how much space
	 * the decorations will take up in the title bar.  Also, we
	 * set the leftmost point at which the title string can be
	 * drawn without interfering with the decoration, if any,
	 * on the left hand side of the title bar.  'titleInfo->titleOff'
	 *
	 * Note:  We assume that if the title bar contains ONLY a
	 * pushpin it will be the leftmost decoration.  If it contains
	 * a close button, 'shine mark', we assume it will be the
	 * leftmost decoration.
	 *
	 * The latest and greatest revision seems to say that a window
	 * can not have both a close button and a pushpin.  So, if they
	 * ask for both, they only get the pushpin.
	 * This mutual exclusion was taken care of in GetOLWinDecors in
	 * AdFrame.c
	 */
	titleInfo->decorWidth = 0;
	titleInfo->children.num = 0;
	if (winDecors->flags & WMDecorationPushPin)
	{
		titleInfo->titleOff = PUSHOFF + pushpin_width ;
		titleInfo->decorWidth = pushpin_width + PUSHOFF;
		titleInfo->children.info[winstep++] = 
				MakePushPin(dpy,newwin,titleWidth, frame);
		titleInfo->children.num++;
	}

	if (winDecors->flags & WMDecorationCloseButton)
	{
		titleInfo->titleOff = SHINEOFF + windowmark_width ;
		titleInfo->decorWidth += titleInfo->titleOff;
		titleInfo->children.info[winstep++] = MakeShineMark(dpy,newwin,
								    frame);
		titleInfo->children.num++;
	}

	/* Add decoration's size to minimum width of window. */
	hints = GetSizeHints(frame);

	hints->flags |= PMinSize;
	hints->min_width += titleInfo->decorWidth;

	SetTitleName(dpy, frame, winInfo);

	return winInfo;
}


/*
 * SetTitleName -- fetch the name of the window from the pane and 
 * 	calculate the values needed to center the string
 */
/*ARGSUSED*/	/* dpy arg will be used when multiple Displays supported */
static
SetTitleName(dpy, frame, self)
Display	*dpy;
WinInfo	*frame;
WinInfo	*self;
{
	TitleInfo	*titleInfo = (TitleInfo *)(self->priv.ptr);
	int		font_height;
	char		*GetName(), *name;
	char		*strchr(), *ptr;
	int		titleWidth;
	int		rightEdge;

	/* Calculate title characteristics. */
	if (titleInfo->name)
		free(titleInfo->name);

	titleInfo->name = (char *)IntMalloc(
				(unsigned)(strlen(name = GetName(frame)) + 1)
			  );
	strcpy(titleInfo->name, name);

	titleInfo->nameLength = strlen(titleInfo->name);

	font_height = (NormalFontInfo->max_bounds.ascent 
			+ NormalFontInfo->max_bounds.descent);
	titleInfo->nameWidth = XTextWidth(NormalFontInfo, titleInfo->name, 
					titleInfo->nameLength);

	titleWidth = self->width - titleInfo->decorWidth;
	if (titleWidth < 0 )
		titleWidth = 0;

	/* A quick definition of what each of these variables
	 * should help you understand what's going on here.
	 *
	 * self->width 		- width of the titlebar window.
	 *
	 * titleInfo->nameWidth - width of the title string.
	 *
	 * titleInfo->decorWidth- width of all the decorations, and
	 *			  the padding space, that
	 *			  will go in the title bar.
	 *
	 * titleWidth		- width of space left over for the
	 *			  title string after allowing for 
	 *			  the decorations and their padding.
	 */

	if (titleWidth < titleInfo->nameWidth)
	{
		/* Must truncate the title.
		 * First we see if there is a colon and truncate
		 * all the chars up to the colon.
		 */
		if (ptr = strchr(titleInfo->name, ':'))
		{
			ptr++; /* after ':' */
			titleInfo->nameLength -= ptr - titleInfo->name;
			titleInfo->name = ptr;
			titleInfo->nameWidth = XTextWidth( NormalFontInfo, 
					titleInfo->name, 
					titleInfo->nameLength);
		}
	}

	while (titleWidth < titleInfo->nameWidth)
	{
		/* Truncate the title from the right. */
		titleInfo->name[strlen(titleInfo->name) - 1] = '\0';
		titleInfo->nameLength--;
		titleInfo->nameWidth = XTextWidth( NormalFontInfo, 
				titleInfo->name, 
				titleInfo->nameLength);
	}

			
	/* Center that title. */
	titleInfo->x = (self->width - titleInfo->nameWidth)/2;
	if (titleInfo->x < titleInfo->titleOff)
		titleInfo->x = titleInfo->titleOff;
	else 
	{
		rightEdge = titleInfo->titleOff + titleWidth;

		if (titleInfo->x + titleInfo->nameWidth > rightEdge)
			titleInfo->x  += rightEdge -
				(titleInfo->x + titleInfo->nameWidth);
	}
			
	titleInfo->y = NormalFontInfo->max_bounds.ascent + 2;
}

/*
 * FlashTitleBar -- flash the title bar
 */
FlashTitleBar(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	GC	gc;
	int	ii;
	TitleInfo	*titleInfo = (TitleInfo *)(winInfo->priv.ptr);
	int 	state;
	int olwm_usleep();

	if (!f3dUsed)
	{
		if (!CheckFocus(winInfo->frame))
			gc = DrawNormalGC;
		else
			gc = DrawReverseGC;

		for(ii=0; ii < 6; ii++)
		{
			XFillRectangle(dpy, winInfo->self, gc, 0, 0, 
					winInfo->width, winInfo->height);

			gc = (gc == DrawNormalGC) ?
				DrawReverseGC : DrawNormalGC;
			XDrawString(dpy, winInfo->self, gc,
				    titleInfo->x, titleInfo->y,
				    titleInfo->name, titleInfo->nameLength);
			XFlush(dpy);
			olwm_usleep((unsigned)(FlashTime));
		}
	}
	else
	{
		if (CheckFocus(winInfo->frame))
			state = OLGX_NORMAL;
		else
			state = OLGX_INVOKED;

		for (ii=0; ii<6; ii++)
		{
			olgx_draw_box(olgx_gisbold, winInfo->self, 0, 0,
				      winInfo->width, winInfo->height,
				      state, True);
			olgx_draw_text(olgx_gisbold, winInfo->self,
				       titleInfo->name,
				       titleInfo->x, titleInfo->y,
				       titleInfo->nameWidth,
				       False, state);
			state = (state == OLGX_NORMAL) ?
				OLGX_INVOKED : OLGX_NORMAL;
			XFlush(dpy);
			olwm_usleep((unsigned)(FlashTime));
		}
	}
}
		
/*
 * DrawTitleBar -- draw the title bar
 *
 * The drawing of the title bar has been greatly complicated by
 * the fact that the window's focus state and selection state
 * affects the frame, titlebar, and pushpin.  In fact, when a
 * window's selection state changes while in focus follows
 * mouse mode, the lines which show the window has the focus
 * need to be drawn differently.
 *
 * If a window has the focus while in focus follows mouse mode
 * the header has a two pixel wide line near the top and a
 * two pixel wide line near the bottom.  A problem arises
 * because the upper two pixel wide line is completely in the frame
 * window when the window is unselected, and partly in the
 * frame and partly in the titlebar when the window is selected.
 *
 * Also, when a window has the focus in click to focus mode, the
 * header is filled in black.  The title bar window is one pixel to small
 * in each direction.  We can't make the title bar bigger because
 * it will overlap the resize corners.  So, the DrawFrame routine
 * has to draw a one pixel wide line all the way around the title bar
 * when the window has the focus in click to focus mode.  Ugh!
 *
 * The design of having a separate window for the title bar and resize
 * corners was started before the OpenLook spec. was completed so there
 * are much better ways to do this.
 *
 * Here is a rough outline of the code:
 *
 *	If we don't have the focus
 *		Clear the title bar window.
 *	Else we have the focus
 * 		If we are in focus follows mouse mode
 *			If we are selected
 *				Draw the bottom line of the upper
 *				2 pixel wide line which shows we
 *				have the focus.  The upper line is
 *				in the frame and is handled by it.
 *			Else we are not selected
 *				Erase the bottom line of the upper
 *				2 pixel wide line.
 *		Else we are in click to focus mode
 *			Fill the title bar black.
 */
DrawTitleBar(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	register int	ii;
	TitleInfo	*titleInfo = (TitleInfo *)(winInfo->priv.ptr);
	FrameInfo	*frameInfo = (FrameInfo *)(winInfo->frame->priv.ptr);

	if (titleInfo->busyState == WMWindowIsBusy)
	{
		/* need to convert this to olgx later ??? */
		XFillRectangle(dpy, winInfo->self, DrawBusyGC, 0, 0, 
				winInfo->width, winInfo->height);
		XDrawString(dpy, winInfo->self, DrawNormalGC,
			    titleInfo->x, titleInfo->y,
			    titleInfo->name, titleInfo->nameLength);
	}
	else if (!CheckFocus(winInfo->frame))
	{
		if (!f3dUsed)
		{
			XFillRectangle(dpy, winInfo->self, DrawReverseGC, 0, 0, 
					winInfo->width, winInfo->height);
			XDrawString(dpy, winInfo->self, DrawNormalGC,
				    titleInfo->x, titleInfo->y,
				    titleInfo->name, titleInfo->nameLength);
		}
		else
		{
			olgx_draw_box(olgx_gisbold, winInfo->self, 0, 0,
				      winInfo->width, winInfo->height,
				      OLGX_NORMAL, True);
			olgx_draw_text(olgx_gisbold, winInfo->self,
				       titleInfo->name,
				       titleInfo->x, titleInfo->y,
				       titleInfo->nameWidth,
				       False, OLGX_NORMAL);
		}
	}
	else  /* We have the focus */
	{
		if (FocusFollowsMouse)
		{
			if (!f3dUsed)
			{
				/* Blank out the title bar before we draw
				 * the focus lines on it.
				 */
				XFillRectangle(dpy, winInfo->self,
					       DrawReverseGC, 0, 0,
					       winInfo->width, winInfo->height);
				XDrawString(dpy, winInfo->self, DrawNormalGC,
					    titleInfo->x, titleInfo->y,
					    titleInfo->name,
					    titleInfo->nameLength);

				/* If this window is selected 
				 * the bottom line of the top double
				 * line (which shows that the window has focus)
				 * is in the top of the titlebar.
				 */
				if (IsSelected(winInfo->frame))
				{
					/* Draw the bottom line of the 
					 * top double line, which is the
					 * top line in the titlebar. */

					/* The top double lines are shorter
					 * if this window has resize corners.
					 */
					if (frameInfo->winDecors->flags &
					    WMDecorationResizeable)
						XDrawLine(dpy, winInfo->self, 
							  DrawNormalGC, 7, 0, 
							  winInfo->width-7-1, 0);
					else
						XDrawLine(dpy, winInfo->self, 
							  DrawNormalGC, 0, 0, 
							  winInfo->width-0-1, 0);
				}
				else 
				{
					/* Erase the line */
					/* The top double lines are shorter
					 * if this window has resize corners.
					 */
					if (frameInfo->winDecors->flags &
					    WMDecorationResizeable)
						XDrawLine(dpy, winInfo->self, 
							  DrawReverseGC, 7, 0, 
							  winInfo->width-7-1,0);
					else
						XDrawLine(dpy, winInfo->self, 
							  DrawReverseGC, 0, 0, 
							  winInfo->width-0-1,0);
				}

				XDrawString(dpy, winInfo->self, DrawNormalGC,
					    titleInfo->x, titleInfo->y,
					    titleInfo->name,
					    titleInfo->nameLength);
			}
			else
			{
				if (IsSelected(winInfo->frame))
				{
					olgx_draw_box(olgx_gisbold,
						      winInfo->self, 0, 0,
						      winInfo->width,
						      winInfo->height, 
						      OLGX_INVOKED, True);
					olgx_draw_text(olgx_gisbold,
						       winInfo->self, 
						       titleInfo->name,
						       titleInfo->x,
						       titleInfo->y, 
						       titleInfo->nameWidth,
						       False, OLGX_INVOKED);
				}
				else
				{
					olgx_draw_box(olgx_gisbold,
						      winInfo->self,
						      0, 0,
						      winInfo->width,
						      winInfo->height,
						      OLGX_INVOKED, True);
					olgx_draw_text(olgx_gisbold,
						       winInfo->self,
						       titleInfo->name,
						       titleInfo->x,
						       titleInfo->y,
						       titleInfo->nameWidth,
						       False, OLGX_INVOKED);
				}
			}
		}
		else  /* Click to focus, so reverse video the titlebar */
		{
			if (!f3dUsed)
			{
				XFillRectangle(dpy, winInfo->self,
					       DrawNormalGC,
					       0, 0,
					       winInfo->width,
					       winInfo->height);
				XDrawString(dpy, winInfo->self,
					    DrawReverseGC,
					    titleInfo->x, titleInfo->y,
					    titleInfo->name,
					    titleInfo->nameLength);
			}
			else
			{
				olgx_draw_box(olgx_gisbold, winInfo->self, 0, 0,
					      winInfo->width, winInfo->height,
					      OLGX_INVOKED, True);
				olgx_draw_text(olgx_gisbold, winInfo->self,
					       titleInfo->name,
					       titleInfo->x, titleInfo->y,
					       titleInfo->nameWidth,
					       False, OLGX_INVOKED);
			}
		}
	}

	/* Call the children to draw themselves. */
	for(ii=0; ii < titleInfo->children.num; ii++)
		if(titleInfo->children.info[ii]->drawfunc)
			(*titleInfo->children.info[ii]->drawfunc)(dpy, 
				titleInfo->children.info[ii]);

}

/*
 * PropTitleBar -- a property has changed on the title bar 
 */
static
PropTitleBar(dpy, winInfo, pEvent)
Display	*dpy;
WinInfo	*winInfo;
XEvent	*pEvent;
{
	TitleInfo	*titleInfo = (TitleInfo *)(winInfo->priv.ptr);
	long		*data;
	extern Atom	AtomWindowBusy;
	Atom 		actType;
	int		actFormat;
	unsigned long	nItems, remain;

	if (pEvent->xproperty.atom == AtomWindowBusy)
	{
		XGetWindowProperty(dpy, 
				   GetPane(winInfo->frame), 
				   AtomWindowBusy,
				   0L, 
				   LONG_LENGTH(long),	/* data */
				   False, 
				   (Atom)AnyPropertyType, 
				   &actType, 
				   &actFormat, 
				   &nItems, 
				   &remain, 
				   /* lint will complain about this cast */
				   (unsigned char **)&data);

		titleInfo->busyState = *data;
		/* Draw the busy/not busy title bar. */
		DrawTitleBar(dpy, winInfo);
	} 
	else if (pEvent->xproperty.atom == XA_WM_NAME)
	{
		SetTitleName(dpy, winInfo->frame, winInfo);
		DrawTitleBar(dpy, winInfo);
	}
}



/*
 * EventTitleBar -- handle events for the title bar
 */
static
EventTitleBar(dpy, event, winInfo)
Display	*dpy;
XEvent	*event;
WinInfo	*winInfo;
{
	switch (event->type)
	{
	case Expose:
		DrawTitleBar(dpy, winInfo);
		break;

	default:
		break;
	}
}

/*
 * ConfigTitleBar -- the size of our window has changed.  Resize ourselves.
 */
/*ARGSUSED*/	/* x, y args will be used later */
static
ConfigTitleBar(dpy, winInfo, x, y, titleWidth, titleHeight)
Display	*dpy;
WinInfo	*winInfo;
int x, y, titleWidth, titleHeight;
{
	XWindowChanges	wc;
	unsigned int	mask;

	mask =  CWWidth | CWHeight; 
	wc.width = titleWidth;
	wc.height = titleHeight;

	/* We keep track of only the width. */
	winInfo->width = titleWidth;

	SetTitleName(dpy, winInfo->frame, winInfo);
	XConfigureWindow(dpy, winInfo->self, mask, &wc);
}


/*
 * DestroyTitleBar -- destroy the title bar window and free any allocated
 *	data.
 */
static 
DestroyTitleBar(dpy, winInfo)
Display	*dpy;
WinInfo	*winInfo;
{
	TitleInfo	*titleBarInfo;
	int		ii;

	/* Call the children to destroy themselves. */
	titleBarInfo = (TitleInfo *)(winInfo->priv.ptr);
	for(ii=0; ii < titleBarInfo->children.num; ii++)
		if (titleBarInfo->children.info[ii]->destroyfunc)
			(*titleBarInfo->children.info[ii]->destroyfunc)(dpy, 
				titleBarInfo->children.info[ii]);

	/* Free our data and throw away window. */
	/* lint will complain about this cast */
	free((char *)winInfo->priv.ptr);
	XDestroyWindow(dpy, winInfo->self);
	WIDeleteInfo(winInfo->self);
}

/*
 * FocusTitleBar -- set the current state of the title bar to [un]focused
 */
static 
FocusTitleBar(dpy, winInfo, setFocus)
Display	*dpy;
WinInfo	*winInfo;
Bool	setFocus;
{
	int	ii;
	TitleInfo	*titleInfo = (TitleInfo *)(winInfo->priv.ptr);

	DrawTitleBar(dpy, winInfo);

	/* Call the children to focus themselves. */
	for(ii=0; ii < titleInfo->children.num; ii++)
		if ((titleInfo->children.info[ii]) &&
		    (titleInfo->children.info[ii]->focusfunc))
			(*titleInfo->children.info[ii]->focusfunc)(dpy, 
				titleInfo->children.info[ii], setFocus);
}
