/*
 *      (c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *      pending in the U.S. and foreign countries. See LEGAL_NOTICE
 *      file for terms of the license.
 *
 *	Written for Sun Microsystems by Crucible, Santa Cruz, CA.
 */

static	char	sccsid[] = "@(#) Dialogue.c 5.1 89/12/14 Crucible";

#include <errno.h>
#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>

#include "menu.h"
#include "olwm.h"
#include <olgx/olgx.h>

extern Graphics_info	*olgx_gismisc;
extern Graphics_info	*olgx_gisbold;
extern Graphics_info	*olgx_gismenu;
extern XFontStruct	*MenuTitleFontInfo;

extern int		PointInRect();

#define	DIALOGUE_EVENT_MASK	( ButtonPressMask | ButtonReleaseMask \
				  | PointerMotionMask )
#define	DIALOGUE_ATTR_MASK	( CWBorderPixel | CWBackPixel | CWSaveUnder )

#define	MIN_BOX_SIDE		15	
#define	MIN_BOX_TOP		15
#define	MIN_BOX_BOTTOM		20
#define MIN_BUTTON_SPACE	15
#define MIN_BUTTON_VSPACE	10
/* space between actual text and edge of button */
#define	BUTTON_VGAP		3	/* should be available from olgx */
/* ??? we shouldn't have to look in there for this info, but we need it */
#define	BUTTON_ENDCAP		olgx_gismisc->endcap_width

typedef struct {
	char		*text;
	int		x;
	int		y;
	Bool		defaultKey;
	unsigned int	width;		/* space taken up by text */
	unsigned int	fullWidth;	/* width including endcaps */
} dialogueButtonDetails;

typedef struct {
	Display			*dpy;
	Window			window;
	unsigned int		buttonHeight;
	int			x;
	int			y;
	dialogueButtonDetails	button0;
	dialogueButtonDetails	button1;

} dialogueBoxDetails;

/*
 * DialogueBox -- pop up a box which forces the user to answer
 * 		  a question using the buttons
 *
 * 	Currently only allows two buttons - returns 0 if the
 *	zero-th button is pressed, and 1 if the other button is pressed.
 *	Uses the MenuTitleFont for text.
 *
 * Arguments:
 *	dpy	    - pointer to current display
 *	screen	    - index to current screen
 *	string      - text which will be displayed ("question" for the user)
 *	button0Text - text to appear on the first button (#0)
 *	button1Text - text to appear on the second button (#1)
 *	warpButton  - button to warp pointer to (0 or 1)
 *	boxX	    - value to use for window origin (if -1, use default)
 *	boxY	    - value to use for window origin (if -1, use default)
 *
 *	Default placement of the box is centered in the display
 *	Returns -1 on failure (0 for 0th button, 1 for other button)
 */
int
DialogueBox( dpy, screen, string, button0Text, button1Text, 
	     warpButton, boxX, boxY )
Display	*dpy;
int	screen;
char	*string;
char	*button0Text;
char	*button1Text;
int	warpButton;
int	boxX;
int	boxY;
{
	XSetWindowAttributes	attributes;
	unsigned int		displayWidth, displayHeight;
	unsigned int		boxWidth, boxHeight;
	unsigned int		fontHeight;
	int			buttonState;
	int			buttonSelected;
	dialogueBoxDetails	boxDetails;
	int			pointerX, pointerY;
	int			dummyInt;
	Window			dummyWin;


	/* set up window attributes structure */
	attributes.border_pixel = borderColor;
	attributes.background_pixel = bg1Color;
	attributes.save_under = True;

	/* set up initial elements of boxDetails structure */
	boxDetails.dpy = dpy;
	boxDetails.button0.text = button0Text;
	boxDetails.button1.text = button1Text;

	if ( warpButton == 1 )
	{
		boxDetails.button0.defaultKey = False;
		boxDetails.button1.defaultKey = True;
	}
	else	/* this makes the default warpButton = 0 */
	{
		boxDetails.button0.defaultKey = True;
		boxDetails.button1.defaultKey = False;
	}

	/* figure out size to make window */
	fontHeight = MenuTitleFontInfo->ascent + MenuTitleFontInfo->descent;
	boxDetails.button0.width = XTextWidth( MenuTitleFontInfo, 
				   	       button0Text, 
					       strlen(button0Text) );
	boxDetails.button1.width = XTextWidth( MenuTitleFontInfo, 
				   	       button1Text, 
					       strlen(button1Text) );
	boxDetails.button0.fullWidth = boxDetails.button0.width 
				       + 2 * BUTTON_ENDCAP;
	boxDetails.button1.fullWidth = boxDetails.button1.width 
				       + 2 * BUTTON_ENDCAP;
	boxWidth = 2 * MIN_BOX_SIDE 
		   + MAX( XTextWidth( MenuTitleFontInfo, 
				      string, strlen(string) ),
			  ( boxDetails.button0.fullWidth 
			    + boxDetails.button1.fullWidth
			    + MIN_BUTTON_SPACE ) );
	boxDetails.buttonHeight = 2 * BUTTON_VGAP + fontHeight;
	boxHeight = MIN_BOX_TOP + MIN_BOX_BOTTOM + fontHeight 
		    + boxDetails.buttonHeight;
	displayWidth = DisplayWidth( dpy, screen );
	displayHeight = DisplayHeight( dpy, screen );
	/* if not set, create default "origin" for box: centered */
	boxDetails.x = ( boxX == -1 ) ?
			(int)( displayWidth - boxWidth )/2 : boxX ;
	boxDetails.y = ( boxY == -1 ) ? 
			(int)( displayHeight - boxHeight )/2 : boxY ;

	/* create window */
	boxDetails.window = XCreateWindow( dpy, 
					   RootWindow(dpy, 
						      DefaultScreen(dpy)),
				           boxDetails.x, boxDetails.y, 
					   boxWidth, boxHeight, 
					   0, DefaultDepth(dpy, DefaultScreen(dpy)), 
					   InputOutput, CopyFromParent, 
				           DIALOGUE_ATTR_MASK,
				           &attributes );
	
	/* grab server and map window so box can be drawn here */
	XGrabServer( dpy );
	XMapRaised( dpy, boxDetails.window );

	/* draw beveled box */
	olgx_draw_box( olgx_gismenu, boxDetails.window, 0, 0, 
		       boxWidth, boxHeight, OLGX_NORMAL, True );

	olgx_draw_box( olgx_gismenu, boxDetails.window, 5, 5, 
		       boxWidth-10, boxHeight-10, OLGX_NORMAL, True );

	/* put text in */
	olgx_draw_text( olgx_gisbold, boxDetails.window, string, 
			MIN_BOX_SIDE,
			MIN_BOX_TOP + MenuTitleFontInfo->ascent,
			0, False, OLGX_NORMAL );

	/* put buttons in */
	buttonState = ( boxDetails.button0.defaultKey == True ) 
			? OLGX_DEFAULT : OLGX_NORMAL;
	/* center button on first third of box */
	boxDetails.button0.x = boxWidth/3 - boxDetails.button0.fullWidth/2;
	boxDetails.button0.y = MIN_BOX_TOP + fontHeight + MIN_BUTTON_VSPACE;
	olgx_draw_button( olgx_gisbold, boxDetails.window, 
			  boxDetails.button0.x, boxDetails.button0.y,
			  boxDetails.button0.fullWidth, 
			  boxDetails.button0.text, 
			  buttonState );

	buttonState = ( boxDetails.button1.defaultKey == True ) 
			? OLGX_DEFAULT : OLGX_NORMAL;
	/* center button on second third of box */
	boxDetails.button1.x = 2*boxWidth/3 
			       - boxDetails.button1.fullWidth/2;
	boxDetails.button1.y = boxDetails.button0.y;

	olgx_draw_button( olgx_gisbold, boxDetails.window, 
			  boxDetails.button1.x, boxDetails.button1.y,
			  boxDetails.button1.fullWidth, 
			  boxDetails.button1.text, 
			  buttonState );

	/* save current mouse position */
	XQueryPointer(dpy, RootWindow(dpy, DefaultScreen(dpy)),
		      &dummyWin, &dummyWin,
		      &pointerX, &pointerY,
		      &dummyInt, &dummyInt, &dummyInt);

	/* warp pointer */
	if ( boxDetails.button1.defaultKey == True ) 
		XWarpPointer( dpy, None, boxDetails.window, 0, 0, 0, 0,
			      boxDetails.button1.x
			      + boxDetails.button1.fullWidth/2, 
			      boxDetails.button1.y + BUTTON_VGAP );
	else
		XWarpPointer( dpy, None, boxDetails.window, 0, 0, 0, 0,
			      boxDetails.button0.x
			      + boxDetails.button1.fullWidth/2, 
			      boxDetails.button0.y + BUTTON_VGAP );

	XSelectInput( dpy, boxDetails.window, DIALOGUE_EVENT_MASK );
	buttonSelected = dialogueEventLoop( &boxDetails );

	/* put the pointer back where we got it */
	XWarpPointer( dpy, None, RootWindow(dpy, DefaultScreen(dpy)),
		      0, 0, 0, 0,
		      pointerX, pointerY );

	/* free up the server as soon as we're done with it */
	XUngrabServer( dpy );

	/* destroy window */
	XDestroyWindow( dpy, boxDetails.window );
	return( buttonSelected );
}

/*
 * dialogueEventLoop -- watch for events while in dialogue box
 *
 */
dialogueEventLoop( details )
dialogueBoxDetails	*details;
{
	XEvent	event;
	Bool	done = False;
	int	buttonSelected = -1;
	int	buttonDepressed = -1;
	Bool	buttonIsDown = False;
	dialogueButtonDetails *dbp0, *dbp1;
	unsigned int	b0Kind, b1Kind;

	/* take control until we get an answer */
	XGrabPointer( details->dpy, details->window, False, 
		      DIALOGUE_EVENT_MASK,
		      GrabModeAsync, GrabModeAsync,
		      None, None, CurrentTime );

	/* these pointers will simplify some of the statements */
	dbp0 = &details->button0;
	dbp1 = &details->button1;

	b0Kind = dbp0->defaultKey ? OLGX_DEFAULT : OLGX_NORMAL;
	b1Kind = dbp1->defaultKey ? OLGX_DEFAULT : OLGX_NORMAL;

	while ( !done )
	{
		XNextEvent( details->dpy, &event );

		switch (event.type)
		{
		case ButtonPress:
			/* if on one of the dialogue buttons, depress it */
			if ( PointInRect( event.xbutton.x, 
				     	  event.xbutton.y,
				     	  dbp0->x, dbp0->y,
				     	  dbp0->fullWidth,
				     	  details->buttonHeight ) )
			{
				buttonIsDown = True;
				buttonDepressed = 0;
				olgx_draw_button( olgx_gisbold, 
						  details->window, 
						  dbp0->x, dbp0->y,
						  dbp0->fullWidth,
						  dbp0->text, 
						  OLGX_INVOKED );
			}
			else if ( PointInRect( event.xbutton.x, 
					       event.xbutton.y,
					       dbp1->x, dbp1->y,
					       dbp1->fullWidth,
					       details->buttonHeight ) )
			{
				buttonIsDown = True;
				buttonDepressed = 1;
				olgx_draw_button( olgx_gisbold, 
						  details->window, 
						  dbp1->x, dbp1->y,
						  dbp1->fullWidth,
						  dbp1->text, 
						  OLGX_INVOKED );
			}
			break;

		case ButtonRelease:
			/* only a depressed button can be selected */
			switch ( buttonDepressed )
			{
			case 0:
				if ( PointInRect( event.xbutton.x, 
						  event.xbutton.y,
						  dbp0->x, dbp0->y,
						  dbp0->fullWidth,
						  details->buttonHeight ) )
				{
					buttonSelected = 0;
					done = True;
				}
				break;

			case 1:
				if ( PointInRect( event.xbutton.x, 
				     		  event.xbutton.y,
				     		  dbp1->x, dbp1->y,
				     		  dbp1->fullWidth,
				     		  details->buttonHeight ) )
				{
					buttonSelected = 1;
					done = True;
				}
				break;
			}

			if (!done)
			{
				buttonDepressed = -1;
			}
			break;
		
		case MotionNotify:
			/* check state of dialogue buttons */
			switch ( buttonDepressed )
			{
			case 0:
				if ( PointInRect( event.xbutton.x, 
					     	  event.xbutton.y,
					          dbp0->x, dbp0->y,
					          dbp0->fullWidth,
					          details->buttonHeight ) )
				{
					/* Moved within button 0,
					 * if the button was previously up,
					 * make it go down.
					 */
					if (!buttonIsDown)
					{
						olgx_draw_button( olgx_gisbold, 
							  details->window, 
							  dbp0->x, dbp0->y,
							  dbp0->fullWidth,
							  dbp0->text, 
							  OLGX_INVOKED );
						buttonIsDown = True;
					}
				}
				else
				{
					/* Moved out of button 0,
					 * if the button was previously down,
					 * make it go up.
					 */
					if (buttonIsDown)
					{
						olgx_draw_button( olgx_gisbold, 
							  details->window, 
							  dbp0->x, dbp0->y,
							  dbp0->fullWidth,
							  dbp0->text, 
							  b0Kind | OLGX_ERASE );
						buttonIsDown = False;
					}
				}
				break;
			case 1:
				if ( PointInRect( event.xbutton.x, 
					     	  event.xbutton.y,
					          dbp1->x, dbp1->y,
					          dbp1->fullWidth,
					          details->buttonHeight ) )
				{
					if (!buttonIsDown)
					{
						olgx_draw_button( olgx_gisbold, 
							  details->window, 
							  dbp1->x, dbp1->y,
							  dbp1->fullWidth,
							  details->button1.text,
							  OLGX_INVOKED );
						buttonIsDown = True;
					}
				}
				else
				{
					if (buttonIsDown)
					{
						olgx_draw_button( olgx_gisbold, 
							  details->window, 
							  dbp1->x, dbp1->y,
							  dbp1->fullWidth,
							  details->button1.text,
							  b1Kind | OLGX_ERASE );
						buttonIsDown = False;
					}
				}
				break;

			default:
				break;
		
			}
			break;
		default:
			break;
		}
	}

	/* release control */
	XUngrabPointer( details->dpy, CurrentTime );
	XFlush( details->dpy );

	return( buttonSelected);
}

