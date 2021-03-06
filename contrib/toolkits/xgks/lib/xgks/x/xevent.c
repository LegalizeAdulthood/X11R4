/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 *  XGKS -- Xevent interrupt handeling and process routines
 * 
 * $Header: xevent.c,v 1.1 89/09/18 17:25:13 jim Exp $
 *
 * $Source: /xsrc/contrib/toolkits/xgks/lib/x/RCS/xevent.c,v $
 *
 * $Log:	xevent.c,v $
 * Revision 1.1  89/09/18  17:25:13  jim
 * Initial revision
 * 
 * Revision 4.0  89/08/31  16:28:38  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.28  89/06/05  10:26:36  bruce
 * DCR# d1:	Changed include file name from gks_implement.h to gks_implem.h
 * 		for the AIX compiler.  Also added a display argument to all
 * 		calls to SIGIO functions.
 * 
 * Revision 3.27  89/03/15  16:37:51  amy
 * PTR c1164	xProcessEvents:  add check for ws_is_closing before 
 * 		continuing with event processing.
 * 
 * Revision 3.26  89/02/13  13:26:37  amy
 * PTR c1147	XgksSIGIO_ON defined as an int function.
 * 
 * Revision 3.25  89/02/04  15:28:43  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.24  89/01/27  12:53:08  amy
 * Cosmetic change to Log.
 * 
 * Revision 3.23  89/01/27  12:41:34  amy
 * PTR c1145	xProcessEvents:  switch on just xev.type, without masking out the old
 * 		XSendEvent bit; there is now a field in the event structure containing 
 *		this information.  
 *		Rewrite switch statement so that action to take at each case is 
 * 		contained in asubroutine.
 * 		On FocusIn event, update the focus_ws pointer in the GKS state list,  
 * 		and change color table if necessary.
 * 
 * Revision 3.22  89/01/26  11:45:29  amy
 * Cosmetic change to Log.
 * 
 * Revision 3.21  89/01/26  11:42:16  amy
 * PTR c1127	xProcessEvents:    case FocusIn,  call DefaultColormap with correct  
 * 		display pointer.  Same for case LeaveNotify.
 * 
 * Revision 3.20  88/12/08  17:52:29  amy
 * Changed Colourmap back to Colormap.
 * 
 * Revision 3.19  88/12/08  14:06:54  amy
 * Changed spelling of color to colour.
 * 
 * Revision 3.18  88/12/01  12:30:20  amy
 * New 4.3 C compiler--
 * 	SIGIO_ON and SIGIO_OFF defined as void functions, since no callers 
 * 	expect a return value from these functions.
 * 
 * Revision 3.17  88/11/02  09:59:23  amy
 * Edited the log for neatness.
 * 
 * Revision 3.16  88/11/02  09:56:04  amy
 * PTR c1129	xProcessEvents:  for expose events, added check to see if
 * 		there is another expose event in the queue for this window,
 * 		and if so, whether there is more than one.  Then, only process
 * 		the last expose event in the queue for this window.  
 * 		This prevents double refresh under the uwm window manager.
 * 		
 * Revision 3.15  88/08/18  14:55:02  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:51:57  david
 * Initial revision
 *  
 *
 */

static char rcsid[] = "$Header: xevent.c,v 1.1 89/09/18 17:25:13 jim Exp $";


#include "string.h"
#include "gks_implem.h"		/* d1 */
#include <signal.h>
#include <sys/ioctl.h>		/* d1 */

#ifdef EVENTDEBUG
#define PRINT_EVENT(evnt) print_event(evnt)
#else
#define PRINT_EVENT(evnt) 
#endif

#ifdef SIGNALRETURNSINT
#define VOIDTYPE int
#define VOIDVAL(val) val
#else
#define VOIDTYPE void
#define VOIDVAL(val) /**/
#endif

/*
 * the XEvent interrupt processing routine
 */

static								/* c1147 */
VOIDTYPE xProcessEvents()
{
   Display *dpy;
   XEvent xev;
   Gint i;

   /* The check "&& (!xgks_state.openedws[i].ws->ws_is_closing)" */ /* c1164 */
   /* was added to fix c1164 (X error if focus change as the     */ /* c1164 */
   /* window is closing)  Now ws_is_closing is set to true early */ /* c1164 */
   /* in the window closing process.  This blocks events from    */ /* c1164 */
   /* getting past this "if" once the window has started to      */ /* c1164 */
   /* close.                                   DWO               */ /* c1164 */

	for (i=0; i<MAX_OPEN_WS; i++)
	   {
	   if ((xgks_state.openedws[i].ws_id != INVALID)  /*c1147*/
             && ((dpy = xgks_state.openedws[i].ws->dpy) != NULL)  /*c1147*/
             && (!xgks_state.openedws[i].ws->ws_is_closing) )     /* c1164 */
		while (XPending(dpy)) 
	   	   {		/* return # pending events */
	   	   XNextEvent(dpy, &xev);

		   /* call debugging printer if EVENTDEBUG is defined */
                   PRINT_EVENT(&xev);

                   /* NOTE: this used to say "xev.type & 0177" to */ /* c1145 */
                   /*       mask out the "XSendEvent" bit that    */ /* c1145 */
                   /*       indicated that the event had been sent*/ /* c1145 */
                   /*       with XSendEvent.  This is obsolete now*/ /* c1145 */
                   /*       Now there is a field called send_event*/ /* c1145 */
                   /*       that indicates the same.  (DWO)       */ /* c1145 */
                   switch( xev.type )                                /* c1145 */
			{

			case Expose:
				XgksExposeEvent(&xev,dpy);		/*c1145*/
                		break;

			case KeyPress:		/* Physical device triggered */
			case MotionNotify:	/* Physical device triggered */
			case ButtonPress:
			case ButtonRelease:
				XgksIProcessXEvent( &xev );
				break;

		case EnterNotify:
		case FocusIn:
			XgksFocusInEvent(&xev,dpy);		/*c1145*/
			break;

		case FocusOut:
		case LeaveNotify:
			XgksFocusOutEvent(dpy);		/*c1145*/
			break;

	}
   }
}
	return VOIDVAL(0);
}



/*
 * start the SIGIO interrupt system 
 * This function was rewritten for the AIX PORT
 */
int xXgksSIGIOStart(ws)					/*c1147*/
WS_STATE_PTR ws;
{
	Display *dpy;                           /* d1 */
	int one = 1;                            /* d1 */
	int pid = getpid();                     /* d1 */
	struct sigvec invec,outvec;             /* d1 */

	dpy = ws->dpy;
	if (dpy == NULL)		/* not opened yet */
		return (INVALID);
        invec.sv_handler = SIG_IGN;             /* d1 */
        invec.sv_mask = 0;                      /* d1 */
        invec.sv_onstack = 0;                   /* d1 */
        sigvec(SIGIO,&invec,&outvec);           /* d1 */
        ioctl( dpy->fd, SIOCSPGRP, &pid);       /* d1 */
        ioctl( dpy->fd, FIOASYNC, &one);        /* d1 */
	return (0);
}

static int SigCount = 0;

int XgksSIGIO_OFF(dpy)				/*c1147*//* d1 */
Display *dpy;                                   /* d1 */
{
        int zero = 0;                           /* d1 */
        struct sigvec invec,outvec;             /* d1 */
	SigCount++;

#ifdef SIGDEBUG
	fprintf(stderr, "XgksSIGIO_OFF SigCount == %d\n", SigCount);
#endif
	if (SigCount > 1)	/* already off */
		return(0);
/* if socket does not exist io is not possible     d1 */
        if (dpy == NULL)                        /* d1 */
                return(0);                      /* d1 */
        invec.sv_handler = SIG_IGN;             /* d1 */
        invec.sv_mask = 0;                      /* d1 */
        invec.sv_onstack = 0;                   /* d1 */
        sigvec( SIGIO, &invec,&outvec);         /* d1 */
        ioctl( dpy->fd, FIOASYNC, &zero);       /* d1 */
}

int XgksSIGIO_ON(dpy)						/*c1147*//* d1 */
Display *dpy;                                   /* d1 */
{
        int one = 1;                            /* d1 */
        int pid = getpid();                     /* d1 */
        struct sigvec invec,outvec;             /* d1 */

	SigCount--;

#ifdef SIGDEBUG
	fprintf(stderr, "XgksSIGIO_ON SigCount == %d\n", SigCount);
#endif
	if (SigCount > 0)	/* only on last request */
		return(0);

/* if socket does not exist io is not possible     d1 */
        if (dpy == NULL)                        /* d1 */
                return(0);                      /* d1 */
        xProcessEvents(one);                    /* d1 */
        invec.sv_handler = xProcessEvents;      /* d1 */
        invec.sv_mask = 0;                      /* d1 */
        invec.sv_onstack = 0;                   /* d1 */
        sigvec( SIGIO, &invec,&outvec);         /* d1 */
        ioctl( dpy->fd, SIOCSPGRP, &pid);       /* d1 */
        ioctl( dpy->fd, FIOASYNC, &one);        /* d1 */

}

static int XgksExposeEvent (xev,dpy)
   XEvent *xev;
   Display *dpy;
   {
   XEvent tmpxev;
   Gint ii;
   Window  win;
   WS_STATE_PTR ws;
   XWindowAttributes win_att;


   /* if this is not the latest Expose Event, ignore it */
   XSync(dpy, 0);
   if (xev->xexpose.count != 0)
      return(0);

   /* if there is not another expose event further on in the queue, then we
      have to process this expose.  Otherwise, we can bag the current expose,
      and if the expose count on the one we found is greater than 0, then we
      can bag that one also because we know another one has to follow it.  If
      the expose count on the one we found is 0 then it has to be re-sent to
      put it back at the tail of the queue.  (Got it?)    PTR# c1129  */

   if (XCheckTypedWindowEvent (dpy, xev->xexpose.window, Expose, &tmpxev))
      {
      /*  Found another expose  */
      if (tmpxev.xexpose.count == 0) /*  And we are re-sending it.  */
         XSendEvent (dpy, xev->xexpose.window, True, ExposureMask, &tmpxev);
      return(0); /* We know there's an expose after this one so don't process */
      }      /*  End of changes for PTR# c1129  */

   /* get the event window and the related wslist pointer */

   for (ii=0; ii < MAX_OPEN_WS; ii++)
      if ((xgks_state.openedws[ii].win == xev->xexpose.window) && 
              (xgks_state.openedws[ii].ws->dpy == dpy)) /*c1147*/
         break;							/* DWO */

   if (ii >= MAX_OPEN_WS)
      return(0);

   ws  = xgks_state.openedws[ii].ws; /*c1147*/
   win = xgks_state.openedws[ii].win; /*c1147*/

   /* disable all input devices */
   XgksIDevDisable( ws );

   /* get current window width and height values */

   XGetWindowAttributes(dpy, win, &win_att);    

   ws->wbound.x = win_att.width;
   ws->wbound.y = win_att.height;        

   /* for the latest Expose Event redraw the window contents */

   xXgksUpdateTrans(ws);					 /* c1147 */

   XClearArea(dpy, win, 0, 0, 0, 0, False);  

   XSync(dpy, 0);

   XgksXReDrawWs(ws);  						/* c1147 */
        
   /* if necessary call user defined redraw notifying function */
   if (ws->redrawfuncp != NULL)
      (*(ws->redrawfuncp))( ws->ws_id, GRD_X );

   /* enable the input devices */
   XgksIDevEnable( ws );

   }
   
   
static int XgksFocusInEvent (xev,dpy)
   XEvent *xev;
   Display *dpy;
   {
   int n;
   Colormap dclmp, *clmp_installed;
   Gint ii;
   Window  win;
   WS_STATE_PTR ws;

   /* get the event window and the related wslist pointer */

   if (xev->xfocus.detail == NotifyNonlinearVirtual)
      return(0);
   
   clmp_installed = XListInstalledColormaps(dpy, xev->xfocus.window, &n);
   
   if (xev->xfocus.window != DefaultRootWindow(xev->xfocus.display)) 
      {
      for (ii=0; ii < MAX_OPEN_WS; ii++)
      if ((xgks_state.openedws[ii].win == xev->xfocus.window)  
           && (xgks_state.openedws[ii].ws->dpy == dpy)) /*c1147*/
         break;							/* DWO */
   
      if (ii >= MAX_OPEN_WS)
         return(0);
   
      ws  = xgks_state.openedws[ii].ws; /*c1147*/
      win = xgks_state.openedws[ii].win; /*c1147*/
   
      /* remember who has the focus */   /* c1145 */
      xgks_state.focus_ws = ws;                     /* c1145 */ /*c1147*/
   
      /* Instal a new color map when necessary */
   
      if (ws->wclmp != *clmp_installed)
         {
         XInstallColormap(dpy, ws->wclmp);
         XFlush (dpy);                                    /* c1145 */
         /* NOTE: If we don't flush, switching quickly */ /* c1145 */
         /*       in and out of the window could cause */ /* c1145 */
         /*       the wrong color map to be displayed. */ /* c1145 */
         /*                                (DWO)       */ /* c1145 */
         }                                                /* c1145 */
   
      }
   else
      {
      if (*clmp_installed != (dclmp = DefaultColormap(dpy, DefaultScreen(dpy)))) /* c1127 */
         {
         XInstallColormap(dpy, dclmp);
         XFlush (dpy);                                    /* c1145 */
         /* NOTE: If we don't flush, switching quickly */ /* c1145 */
         /*       in and out of the window could cause */ /* c1145 */
         /*       the wrong color map to be displayed. */ /* c1145 */
         /*                                (DWO)       */ /* c1145 */
         }
      /* NOTE: Someday (when they finish defining standards for window  */
      /*       managers) all the code that deals with colormaps may have */
      /*       to be rewritten or removed.  It is expected that window   */
      /*       managers will be at least partially responsible for        */
      /*       managing colormaps.  (DWO)                                */
      }
   }



static int XgksFocusOutEvent (dpy)
   Display *dpy;
   {
   Colormap dclmp;

   dclmp = DefaultColormap(dpy, DefaultScreen(dpy)); /* c1127 */
   XInstallColormap(dpy, dclmp);
   XFlush(dpy);
   /* NOTE: If we don't flush, switching quickly */ /* c1145 */
   /*       in and out of the window could cause */ /* c1145 */
   /*       the wrong color map to be displayed. */ /* c1145 */
   /*                                (DWO)       */ /* c1145 */

   xgks_state.focus_ws = NULL;                 	    /* c1145 */ /*c1147*/
   /* focus_ws remains null until the next */       /* c1145 */
   /* FocusIn event. (DWO)                 */       /* c1145 */

   return(0);
   }





#ifdef EVENTDEBUG
static int print_event (evnt)
   XEvent *evnt;
   {
   switch (evnt->type)
      {
      case 0:
         fprintf( stderr, "some kind of error" );
         break;
      case 1:
         fprintf( stderr, "some kind of reply" );
         break;
      case KeyPress:
         fprintf( stderr, "KeyPress" );
         break;
      case KeyRelease:
         fprintf( stderr, "KeyRelease" );
         break;
      case ButtonPress:
         fprintf( stderr, "ButtonPress" );
         break;
      case ButtonRelease:
         fprintf( stderr, "ButtonRelease" );
         break;
      case MotionNotify:
         fprintf( stderr, "MotionNotify" );
         break;
      case EnterNotify:
         fprintf( stderr, "EnterNotify" );
         break;
      case LeaveNotify:
         fprintf( stderr, "LeaveNotify" );
         break;
      case FocusIn:
         fprintf( stderr, "FocusIn" );
         break;
      case FocusOut:
         fprintf( stderr, "FocusOut" );
         break;
      case KeymapNotify:
         fprintf( stderr, "KeymapNotify" );
         break;
      case Expose:
         fprintf( stderr, "Expose" );
         break;
      case GraphicsExpose:
         fprintf( stderr, "GraphicsExpose" );
         break;
      case NoExpose:
         fprintf( stderr, "NoExpose" );
         break;
      case VisibilityNotify:
         fprintf( stderr, "VisibilityNotify" );
         break;
      case CreateNotify:
         fprintf( stderr, "CreateNotify" );
         break;
      case DestroyNotify:
         fprintf( stderr, "DestroyNotify" );
         break;
      case UnmapNotify:
         fprintf( stderr, "UnmapNotify" );
         break;
      case MapNotify:
         fprintf( stderr, "MapNotify" );
         break;
      case MapRequest:
         fprintf( stderr, "MapRequest" );
         break;
      case ReparentNotify:
         fprintf( stderr, "ReparentNotify" );
         break;
      case ConfigureNotify:
         fprintf( stderr, "ConfigureNotify" );
         break;
      case ConfigureRequest:
         fprintf( stderr, "ConfigureRequest" );
         break;
      case GravityNotify:
         fprintf( stderr, "GravityNotify" );
         break;
      case ResizeRequest:
         fprintf( stderr, "ResizeRequest" );
         break;
      case CirculateNotify:
         fprintf( stderr, "CirculateNotify" );
         break;
      case CirculateRequest:
         fprintf( stderr, "CirculateRequest" );
         break;
      case PropertyNotify:
         fprintf( stderr, "PropertyNotify" );
         break;
      case SelectionClear:
         fprintf( stderr, "SelectionClear" );
         break;
      case SelectionRequest:
         fprintf( stderr, "SelectionRequest" );
         break;
      case SelectionNotify:
         fprintf( stderr, "SelectionNotify" );
         break;
      case ColormapNotify:
         fprintf( stderr, "ColormapNotify" );
         break;
      case ClientMessage:
         fprintf( stderr, "ClientMessage" );
         break;
      case MappingNotify:
         fprintf( stderr, "MappingNotify" );
         break;
      default:
         if (evnt->type >= LASTEvent)
         {
            fprintf( stderr, "extension event #%d", evnt->type );
         }
         else
         {
            fprintf( stderr, "\nInternal Error in XUnhandledWireEvent!" );
         }
         break;
      }
   return(0);
   }
#endif
   
  
