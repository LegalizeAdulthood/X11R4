/*
 *	convlib.c -- conversion input library for X11 Toolkit (Xt)
 *
 *		ishisone@sra.co.jp
 */

/*
 * Copyright (C) 1988 by Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

/*
 * This library is for languages that need complex input method
 * (such as Japanese).
 * It communicates with the conversion server using the standard X11
 * inter-client communication mechanism (selection/property/clientmessage),
 * so that you can easily input these languages (if you have the conversion
 * server).
 */


/*
 * see protocol document for the protocol used between client and 
 * conversion server.
 */

/*
 * How to use this library:
 *
 *	1. add the following entry to the widget's action table.
 *
 *		{"begin-conversion",	beginConversion},
 *
 *	2. add the following entry to the translation table.
 *
 *		[trigger-event]: begin-conversion(<language>_CONVERSION)
 *
 *	where:
 *		[trigger-event]	- an trigger event of starting conversion
 *		<language> - the language you want to input
 *	ex)
 *		Ctrl<Key>Kanji: begin-conversion(JAPANESE_CONVERSION)
 *
 *	3. write a function beginConversion().
 *	Since function beginConversion is called by the translation manager,
 *	the arguments passed are Widget, XEvent*, and parameter strings.
 *	This function should call _beginConversion() defined in this library.
 *	_beginConversion()'s interface is:
 *
 *	void _beginConversion(Widget w, Atom catom, Atom tatom,
 *			      void (*inputproc)(), caddr_t client_data);
 *	where:
 *		catom - Selection Atom (JAPANESE_CONVERSION for Japanese)
 *		tatom - Property type of the conversion text
 *				(e.g. COMPOUND_TEXT)
 *		inputproc - conversion text callback function (see below)
 *		client_data - client_data passed to inputproc
 *
 *	this callback function interface is:
 *
 *		textInput(Widget w, Atom selection,
 *				Atom proptype, int propformat,
 *				unsigned long propsize,
 *				unsigned char *propvalue,
 *				caddr_t client_data);
 *
 *	the callback function should free propvalue data area by calling
 *	XtFree().
 *
 *	So, the simplest beginConversion() will be like this:
 *
 *	/-* ARGSUSED *-/
 *	void beginConvesion(w, event, params, nparams)
 *	Widget	w;
 *	XEvent	*event;
 *	String	*params;
 *	Cardinal	*nparams;
 *	{
 *		void	textInput();
 *		Atom	catom, tatom;
 *
 *		if (*nparams != 1) {
 *			/-* Error *-/
 *		}
 *		catom = XInternAtom(XtDisplay(w), *params, False);
 *		tatom = XInternAtom(XtDisplay(w), "COMPOUND_TEXT", False);
 *		_beginConversion(w, catom, tatom, textInput, NULL);
 *	}
 */

#include <X11/StringDefs.h>
#include <X11/Intrinsic.h>
#include <X11/Xutil.h>

#ifndef lint
static char	*rcsid = "$Id: convlib.c,v 1.1 89/11/21 21:01:17 kagotani Rel $";
#endif

/*
 * Atoms:
 *	reqAtom ("CONVERSION_REQUEST")
 *		message_type in ClientMessage event that is sent
 *		when requesting conversion service to conversion server.
 *	notifyAtom ("CONVERSION_NOTIFY")
 *		message_type in ClientMessage event that is sent
 *		from conversion server.
 *	endAtom ("CONVERSION_END")
 *		message_type in ClientMessage event that is sent
 *		from conversion server when conversion ends.
 *	endReqAtom ("CONVERSION_END_REQUEST")
 *		message_type in ClientMessage event that is sent
 *		from clients for stopping conversion.
 */

typedef struct {
	Display	*display;
	Atom	reqAtom;	/* "CONVERSION_REQUEST" */
	Atom	notifyAtom;	/* "CONVERSION_NOTIFY" */
	Atom	endAtom;	/* "CONVERSION_END" */
	Atom	endReqAtom;	/* "CONVERSION_END_REQUEST" */
} ConversionAtoms;

typedef struct {
	Atom	convatom;
	Window	convowner;
	Atom	property;
	void	(*inputproc)();
	caddr_t	closure;
} ConversionContext;

static XContext	convertPrivContext;

#if __STDC__ == 1
/* function prototype */
static ConversionAtoms *getAtoms(Widget);
static ConversionContext *getConversionContext(Widget);
static void recvConvAck(Widget, caddr_t, XEvent *);
static void getConv(Widget, caddr_t, XEvent *);
#else
static ConversionAtoms *getAtoms();
static ConversionContext *getConversionContext();
static void recvConvAck();
static void getConv();
#endif

static ConversionAtoms *
getAtoms(w)
Widget	w;
/* make atoms used for communicating with conversion server
 * and a context for holding the property name
 */
{
	int			i;
	Display			*disp = XtDisplay(w);
	ConversionAtoms		*cap;
	static ConversionAtoms	*convatomp;
	static Cardinal		ndisp = 0;
#define nalloc	2

	/*
	 * Atoms should be made for each display
	 */

	/* check if the atoms are already made */
	cap = convatomp;
	for (i = 0; i < ndisp; i++, cap++) {
		if (cap->display == disp)
			return cap;
	}

	/* not made yet, so create them */
	if (ndisp == 0) {
		/* it's a first time, so a context should also be made */
		convertPrivContext = XUniqueContext();
		convatomp = (ConversionAtoms *)
		    XtMalloc(sizeof(ConversionAtoms) * nalloc);
		cap = convatomp;
	} else if (ndisp % nalloc == 0) {
		/* increase size */
		convatomp = (ConversionAtoms *)
		    XtRealloc(convatomp, sizeof(ConversionAtoms) * (ndisp +nalloc));
		cap = convatomp + ndisp;
	} else {
		cap = convatomp + ndisp;
	}

	cap->display = disp;

	/* create atoms */
	cap->reqAtom = XInternAtom(disp, "CONVERSION_REQUEST", False);
	cap->notifyAtom = XInternAtom(disp, "CONVERSION_NOTIFY", False);
	cap->endAtom = XInternAtom(disp, "CONVERSION_END", False);
	cap->endReqAtom = XInternAtom(disp, "CONVERSION_END_REQUEST", False);

	ndisp++;

	return cap;
}

static ConversionContext *
getConversionContext(w)
Widget	w;
/* gets the context from widget window
 * returns NULL if there is no such context
 */
{
	ConversionContext	*context;

	if (XFindContext(XtDisplay(w), XtWindow(w),
			 convertPrivContext, (caddr_t *)&context)) {
		/* error -- maybe the context is not found */
		return NULL;
	} else {
		return context;
	}
}

static void
recvConvAck(w, closure, ev)
Widget	w;
caddr_t	closure;
XEvent	*ev;
{
	XClientMessageEvent	*cev = &(ev->xclient);
	ConversionAtoms		*cap;
	ConversionContext	*context;

	if (ev->type != ClientMessage)
		return;

	cap = getAtoms(w);
	context = getConversionContext(w);

	/* check if the event is correct */
	if (cev->window != XtWindow(w) ||
	    cev->message_type != cap->notifyAtom ||
	    cev->data.l[0] != context->convatom)
		return;

	/* this event handler is no longer needed. so, remove it */
	XtRemoveEventHandler(w, NoEventMask, True, recvConvAck, closure);

	if (cev->data.l[2] == None) {
		XtWarning("selection request failed");
		XDeleteContext(XtDisplay(w), XtWindow(w), convertPrivContext);
		XtFree((char *)context);
		return;
	}

	/*
	 * add an event handler for PropertyNotify and CONVERSION_END
	 */
	XtAddEventHandler(w, PropertyChangeMask, True, getConv, closure);

	context->property = cev->data.l[2];
}

static void
getConv(w, closure, ev)
Widget	w;
caddr_t	closure;
XEvent	*ev;
/* event handler that deals with PropertyNotify and ClientMessage events */
{
	ConversionAtoms		*cap;
	ConversionContext	*context;

	/* check event type */
	if (ev->type != PropertyNotify &&
	    ev->type != ClientMessage)
		return;

	cap = getAtoms(w);
	context = getConversionContext(w);

	if (ev->type == ClientMessage) {
		XClientMessageEvent	*cev = &(ev->xclient);

		/* is it really CONVERSION_END event? */
		if (cev->message_type == cap->endAtom &&
		    cev->format == 32 &&
		    cev->data.l[0] == context->convatom) {
			/* delete window context */
			XDeleteContext(XtDisplay(w), XtWindow(w),
				       convertPrivContext);
			/* remove this event handler */
			XtRemoveEventHandler(w, PropertyChangeMask, True,
					     getConv, closure);
			XtFree((char *)context);
		}
	} else {	/* PropertyNotify */
		XPropertyEvent	*pev = &(ev->xproperty);
		Atom		proptype;
		int		propformat;
		unsigned long	propsize, rest;
		unsigned char	*propvalue;

		if (context->property == None)
			return;

		/* check if the event is what I need */
		if (pev->window != XtWindow(w) ||
		    pev->atom != context->property ||
		    pev->state != PropertyNewValue) {
			return;
		}

		/* if there is no callback function, we simply
		 * delete property and return
		 */
		if (context->inputproc == NULL) {
			XDeleteProperty(XtDisplay(w), XtWindow(w),
					context->property);
			return;
		}

		/* get conveted string from property */
		XGetWindowProperty(XtDisplay(w), XtWindow(w),
				   context->property,
				   0, 100000, True, AnyPropertyType,
				   &proptype, &propformat, &propsize, &rest,
				   &propvalue);

		if (proptype == None) {
			/* property not exist -- but it's not an error */
			return;
		}

		/* call callback */
		(*context->inputproc)(w, context->convatom,
				      proptype, propformat,
				      propsize, propvalue,
				      context->closure);
	}
}


/*
 *	public functions
 */

void
_beginConversion(w, catom, tatom, inputproc, client_data)
Widget	w;
Atom	catom;			/* Selection Atom e.g. JAPANESE_CONVERSION */
Atom	tatom;			/* Property Type Atom e.g. COMPOUND_TEXT */
void	(*inputproc)();		/* conversion text callback function */
caddr_t	client_data;		/* client_data passed to callback function */
/* request conversion-service to conversion-server.
 * language is specified by catom.
 */
{
	Window			owner;
	XEvent			event;
	ConversionAtoms		*cap;
	ConversionContext	*context;

	cap = getAtoms(w);

	/* find conversion server */
	if ((owner = XGetSelectionOwner(XtDisplay(w), catom)) == None) {
		/*
		 * No owner
		 * if I'm in conversion mode, stop it.
		 */
		XtWarning("Conversion Server not found");
		if ((context = getConversionContext(w)) != NULL) {
			XtRemoveEventHandler(w, NoEventMask, True,
					     recvConvAck, NULL);
			XtRemoveEventHandler(w, PropertyChangeMask, True,
					     getConv, NULL);
			XDeleteContext(XtDisplay(w), XtWindow(w),
				       convertPrivContext);
			XtFree((char *)context);
		}
		return;
	}

	/*
	 * Check if I am in conversion mode.
	 * If so, return immediately... No, things are not so easy.
	 * Because, if the conversion server was killed for some reason,
	 * sometimes the application didn't get CONVERSION_END.
	 * So, even if in conversion mode, get selection owner and
	 * compare with the one saved.
	 * Also I wanna check the time when it becomes the owner,
	 * but I can't get it by GetSelectionOwber() as ICCCM says.
	 */
	if ((context = getConversionContext(w)) != NULL) {
		Window	curOwner;
		curOwner = (catom == context->convatom) ? owner :
		    XGetSelectionOwner(XtDisplay(w), context->convatom);
		if (curOwner == context->convowner) {
			/* do nothing */
			return;
		}
		/*
		 * SelectionOwner has changed. The conversion server
		 * was crushed. So, stop conversion.
		 */
		XtRemoveEventHandler(w, NoEventMask, True, recvConvAck, NULL);
		XtRemoveEventHandler(w, PropertyChangeMask, True, getConv, NULL);
		XDeleteContext(XtDisplay(w), XtWindow(w), convertPrivContext);
		XtFree((char *)context);
	}

	/*
	 * register a handler for CONVERSION_NOTIFY
	 * from the conversion server.
	 */
	XtAddEventHandler(w, NoEventMask, True, recvConvAck, NULL);

	/* make a new context and save data */
	context = XtNew(ConversionContext);
	context->convatom = catom;
	context->convowner = owner;
	context->property = None;
	context->inputproc = inputproc;
	context->closure = client_data;
	XSaveContext(XtDisplay(w), XtWindow(w),
		     convertPrivContext, (caddr_t)context);

	/* request conversion by sending ClientMessage event */
	event.xclient.type = ClientMessage;
	event.xclient.window = owner;
	event.xclient.message_type = cap->reqAtom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = catom;
	event.xclient.data.l[1] = XtWindow(w);
	event.xclient.data.l[2] = tatom;
	event.xclient.data.l[3] = catom;
	event.xclient.data.l[4] = None;
	XSendEvent(XtDisplay(w), owner, False, NoEventMask, &event);
}

void
_endConversion(w, catom, throwaway)
Widget	w;
Atom	catom;		/* Selection Atom */
Boolean	throwaway;
/* force to terminate conversion.
 * catom is a selection atom that is to be terminated.
 * by specifying None to catom, you can terminate current conversion.
 * if throwaway is true, all converted text sent after this call are
 * discarded.
 */
{
	XEvent			event;
	ConversionAtoms		*cap;
	ConversionContext	*context;

	cap = getAtoms(w);
	context = getConversionContext(w);

	if (context == NULL ||
	    (catom != None && catom != context->convatom))
		return;

	if (XGetSelectionOwner(XtDisplay(w), context->convatom) !=
	    context->convowner) {
		/* conversion server is different, or does not exist */
		XtRemoveEventHandler(w, NoEventMask, True, recvConvAck, NULL);
		XtRemoveEventHandler(w, PropertyChangeMask, True, getConv, NULL);
		/* delete window context */
		XDeleteContext(XtDisplay(w), XtWindow(w), convertPrivContext);
		XtFree((char *)context);
		return;
	}

	if (throwaway)
		context->inputproc = NULL;

	event.xclient.type = ClientMessage;
	event.xclient.window = context->convowner;
	event.xclient.message_type = cap->endReqAtom;
	event.xclient.format = 32;
	event.xclient.data.l[0] = context->convatom;
	event.xclient.data.l[1] = XtWindow(w);
	XSendEvent(XtDisplay(w), context->convowner, False, NoEventMask, &event);
}
