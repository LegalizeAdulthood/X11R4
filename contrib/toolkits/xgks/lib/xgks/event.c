/*
 *              Copyright IBM Corporation 1989
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
 * Urbana, IL   61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 *
 * Event Queue maintenance routines.
 *
 */

#include <signal.h>
#include <strings.h>
#include <sys/time.h>
#include <unistd.h>

#include "gks_implem.h"

/*
 * enqueue an event.  Memory pointed to by data must be malloc'ed by the
 * caller but NOT freeed.
 */
void XgksEnqueueEvent( Gint ws, Gint dev, Giclass class, char *data, int event_id )
{
        EQEntry *e;

        if ( (e = (EQEntry *)malloc( sizeof( EQEntry ) )) == NULL) {
                gerrorhand(300,errXgksEnqueueEvent,xgks_state.gks_err_file);
                return;
        }

        e->id           = event_id;
        e->event.ws     = ws;
        e->event.dev    = dev;
        e->event.class  = class;
        e->data         = data;
        e->next         = NULL;

        if (xgks_state.event_queue_head == NULL) {
                xgks_state.event_queue_head = e;
                xgks_state.event_queue_tail = e;
        }
        else {
                xgks_state.event_queue_tail->next = e;
                xgks_state.event_queue_tail = e;
        }

/* If await event is waiting, then stop it */
        kill( getpid(), SIGALRM );
}

/*
 *  AWAIT EVENT
 *
 * Errors 0, 7, 147, 151
 */

static Bool Waiting;            /* True if await event is waiting */

/* has to be global so set mode can set up SIGALRM */
void XgksAwaitInterrupt(int signum)
{
        Waiting = False;        /* timeout reached, give up */
}

Gint gawaitevent(Gfloat timeout, Gevent *event)
{
        struct itimerval itime;

/* check for proper state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgawaitevent)

/* valid timeout value */
        GKSERROR( (timeout < 0.0), 151, errgawaitevent)

/* set up the return values */
        if ( xgks_state.event_queue_head != (EQEntry *)NULL) {
                xgks_state.CurEvent = *xgks_state.event_queue_head;
                *event = xgks_state.CurEvent.event;
                xgks_state.event_queue_head = xgks_state.event_queue_head->next;
                return( 0 );
        }
/* no current values, need to set up the timer */
        if ( timeout == 0.0 ) {
                xgks_state.CurEvent.event.class = GNCLASS;
                event->class = GNCLASS;
                return( 0 );
        }

        itime.it_interval.tv_sec = 0;
        itime.it_interval.tv_usec = 0;
        itime.it_value.tv_sec = (long) timeout;
        itime.it_value.tv_usec = (long) (100000 * (timeout - (long)timeout));
        setitimer( ITIMER_REAL, &itime, (struct itimerval *)NULL);

        signal( SIGALRM, XgksAwaitInterrupt);
/*
 * Stop and wait for the timeout to expire or a logical input device to enque
 * an event.  This looping will be broken by XgksAwaitInterrupt() or EnqueueEvent()
 */

        Waiting = True;
        while( (xgks_state.event_queue_head == (EQEntry *)NULL) && (Waiting == True) )
                pause();
        Waiting = False;

/* set up the return values */
        if ( xgks_state.event_queue_head != (EQEntry *)NULL) {
                xgks_state.CurEvent = *xgks_state.event_queue_head;
                *event = xgks_state.CurEvent.event;
                if ( xgks_state.event_queue_head->data != NULL)
                        free( xgks_state.event_queue_head->data);
                free( xgks_state.event_queue_head );
                xgks_state.event_queue_head = xgks_state.CurEvent.next;
        }
        else {
                xgks_state.CurEvent.event.class = GNCLASS;
                event->class = GNCLASS;
        }

        return(0);
}

/*
 *  FLUSH DEVICE EVENTS
 *
 * NOTE: if called with class == GNCLASS then all events for the
 *      specified workstation will be flushed.
 *
 * Returns: 0, 7, 20, 25, 38, 140, 147
 */

Gint gflushevents(Gint ws_id, Giclass class, Gint dev)
{
        WS_STATE_ENTRY *ws;
        EQEntry *e, *prev;

/* check gks state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errgflushevents)

/* check for invalid workstation id */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgflushevents)

/* open workstation */
        GKSERROR( !(ws=OPEN_WSID(ws_id)), 25, errgflushevents)

/* valid workstation category */
        GKSERROR( (WS_CAT(ws) != GOUTIN && WS_CAT(ws) != GINPUT), 38, errgflushevents)

/* valid device number */
        GKSERROR( (class != GNCLASS) && (dev < 1), 140, errgflushevents)

/* search the event queue for events to delete */
        for( e=xgks_state.event_queue_head, prev=(EQEntry *)NULL; e != (EQEntry *)NULL; )
                if ( (( class == GNCLASS ) && ( e->event.ws == ws_id))
                ||   ((e->event.ws == ws_id) && (e->event.class == class) && (e->event.dev == dev)) ) {
                        if (e == xgks_state.event_queue_head) {
                                xgks_state.event_queue_head = e->next;
                                if ( e->data != NULL)
                                        free( e->data);
                                free( e );
                                e = xgks_state.event_queue_head;
                        }
                        else {
                                prev->next = e->next;
                                if ( e->data != NULL)
                                        free( e->data);
                                free( e );
                                e = prev->next;
                        }
                }
                else {
                        prev = e;
                        e = e->next;
                }

        return(0);
}

/*
 * INQUIRE INPUT QUEUE OVERFLOW
 *
 * Returns: 0, 7, 148, 149
 */

Gint ginqinputoverflow(Gqueue *overflow)
{
/* check gks state */
        GKSERROR ( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqinputoverflow );

        overflow->class = GNCLASS;
        overflow->devno = 0;

        return( 148 );
}


/*
 * INQUIRE MORE SIMULTANEOUS EVENTS
 *
 * Errors: 0, 7
 */

Gint ginqmoreevents(Gsimultev *events)
{
/* check gks state */
        GKSERROR ( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqmoreevents );

        /* Each event has an ID that is set when  */
        /* the event is generated.  If two events */
        /* have the same ID, they were produced   */
        /* simultaneously by the same trigger!    */
        if ( (xgks_state.event_queue_head != NULL)
          && (xgks_state.CurEvent.id == xgks_state.event_queue_head->id) )
          *events = GMORE;
        else
          *events = GNOMORE;

        return( 0 );
}

/*
 * GET LOCATOR
 *
 * returns 0, 7, 150
 */

Gint ggetloc(Gloc *response)
{
/* check gks state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errggetloc );

/* Is the Current Event the proper a Locator event? */
        GKSERROR( (xgks_state.CurEvent.event.class != GLOCATOR), 150, errggetloc)

/* return the event data */
        *response = *(Gloc *)xgks_state.CurEvent.data;

        return(0);
}

/*
 * GET STROKE
 *
 * returns 0, 7, 150
 */

Gint ggetstroke(Gstroke *response)
{
/* check gks state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errggetstroke );

/* Is the Current Event the proper a Stroke event? */
        GKSERROR( (xgks_state.CurEvent.event.class != GISTROKE), 150, errggetstroke)

/* return the event data */
        *response = *(Gstroke *)xgks_state.CurEvent.data;

        return(0);
}

/*
 * GET CHOICE
 *
 * returns 0, 7, 150
 */

Gint ggetchoice(Gchoice *response)
{
/* check gks state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errggetchoice );

/* Is the Current Event the proper a Choice event? */
        GKSERROR( (xgks_state.CurEvent.event.class != GCHOICE), 150, errggetchoice)

/* return the event data */
        *response = *(Gchoice *)xgks_state.CurEvent.data;

        return(0);
}

/*
 * GET PICK
 *
 * returns 0, 7, 150
 */
Gint ggetpick(Gpick *response)
{
/* check gks state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errggetpick );

/* Is the Current Event the proper a Pick event? */
        GKSERROR( (xgks_state.CurEvent.event.class != GPICK), 150, errggetpick)

/* return the event data */
        *response = *(Gpick *)xgks_state.CurEvent.data;

        return(0);
}

/*
 * GET VALUATOR
 *
 * returns 0, 7, 150
 */
Gint ggetval(Gfloat *response)
{
/* check gks state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errggetval );

/* Is the Current Event the proper a valuator event? */
        GKSERROR( (xgks_state.CurEvent.event.class != GVALUATOR), 150, errggetval)

/* return the event data */
        *response = *(Gfloat *)xgks_state.CurEvent.data;

        return(0);
}

/*
 * GET STRING
 *
 * returns 0, 7, 150
 */
Gint ggetstring(Gchar *response)
{
/* check gks state */
        GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errggetstring );

/* Is the Current Event the proper a valuator event? */
        GKSERROR( (xgks_state.CurEvent.event.class != GISTRING), 150, errggetstring)

/* return the event data */
        STRCPY( response, (Gchar *)xgks_state.CurEvent.data );

        return(0);
}
