.\" $Id: PROTOCOL.ms,v 1.1 89/10/26 20:52:30 ishisone Rel $
.de dS
.DS
.ft CW
..
.de dE
.ft R
.DE
..
.TL
X11 Input Conversion
.AU
Makoto Ishisone
ishisone@sra.co.jp
\s-2English translation by\s0
Erik M. van der Poel
erik@sra.co.jp
.AI
Software Research Associates, Inc.
Tokyo, Japan
.AB
For languages such as Japanese which cannot be typed directly at the
keyboard, a method called input conversion can be used.
This document describes an approach for input conversion in X11
applications, and defines a protocol to be used in systems based on
this approach.
.AE
.NH
Introduction
.PP
Asian languages such as Japanese and Chinese have several thousand
characters that cannot be typed directly at the keyboard.
In these languages, it is common to type some text, and then to
convert to the desired characters.
.PP
Japanese, for example, is written using several thousand complex
characters called Kanji and less than a hundred phonetic characters
called Kana.
Typically, the user types a phrase "phonetically" using Kana, and then
the system parses the phrase and looks up words in a dictionary to
convert to a phrase containing both Kanji and Kana.
.PP
Once the phrase is converted, it is often necessary to correct the
system's initial guess at word boundaries within the phrase, or to
choose the desired Kanji among a number of possibilities that have the
same phonetic reading.
This input method is complex and completely different from European
input methods.
.NH
Approach
.PP
It has been decided to take an approach where the conversion is
separated from the input itself by carrying out the conversion in a
separate server.
.PP
This decision was based on the following considerations.
.IP "(1) Easy integration into an application"
Rather than aiming for a sophisticated user interface, the method
should be generalized as much as possible, so that it can be
integrated into any kind of application easily.
It is desirable to allow integration just by linking a simple library.
As a result, the user interface may be somewhat inferior, but if a
sophisticated user interface is required, a specialized widget should
be constructed.
.IP "(2) Easy substitution of the conversion module"
Putting the conversion in a separate server allows the user to
substitute a preferred conversion server.
.IP "(3) Easy extensibility to multiple languages"
If language-dependent functionality is carefully avoided, the system
can easily be adapted to other languages by substituting the
conversion server.
.IP "(4) Crash resistance"
Input conversion is a complex process, particularly in languages such
as Japanese which require syntactic analysis.
Due to various reasons, the probability of a crash in the conversion
part is relatively high.
By putting the conversion in a separate server, the client will not be
seriously affected by a crash.
.PP
In a system based on this approach, the client desiring input sends
requests to the conversion server, and the server returns converted
results.
If a protocol for communication between the conversion server and the
client is fixed, then input conversion should be possible in any
compliant client and conversion server combination.
This document, then, will only lay down the protocol, and will leave
the rest to the implementors.
.NH
Protocol Definition
.PP
The input conversion protocol is explained here.
The application and conversion server communicate using the X11
inter-client communication facilities.
.NH 2
Declaring a Conversion Service
.PP
Upon invocation, the conversion server declares that it will provide a
conversion service by becoming a selection owner using
\fBSetSelectionOwner\fP.
As there may be a conversion server for each language, the selection
atom is used to distinguish the language.
As a rule, the name \fILanguage\fP\fB_CONVERSION\fP is used for this
atom.
For example, for Japanese it would be \fBJAPANESE_CONVERSION\fP, and
for Chinese it would be \fBCHINESE_CONVERSION\fP.
.dS
SetSelectionOwner
	selection: "<Language>_CONVERSION"
	owner: conversion server's window ID
	time: timestamp or CurrentTime
.dE
.LP
Of course, if the conversion server is to support multiple language
input it becomes a multiple selection owner.
.NH 2
Input Conversion Request
.PP
A client that wishes to use the input service looks for the conversion
server using \fBGetSelectionOwner\fP.
.dS
GetSelectionOwner
	selection: "<Language>_CONVERSION"
=>
	owner: owner's window ID or None
.dE
.LP
If the owner is not None, a
.B ClientMessage
is sent to the owner using \fBSendEvent\fP.
.dS
SendEvent
	destination: Selection Owner
	propagate: False
	event-mask: NoEventMask
	event: ClientMessage event
.dE
The event's contents are as follows.
.dS
ClientMessage
	window: Selection Owner
	type: Atom "CONVERSION_REQUEST"
	format: 32
	data:
		l[0]: selection atom
		l[1]: requestor
			ID of the window requesting service
		l[2]: target
			conversion data type (e.g. COMPOUND_TEXT)
		l[3]: property
			property name or None
		l[4]: option
			property name or None
.dE
.LP
The
.I requestor
is the ID of the window requesting input conversion.
The result of the conversion is stored in a property of this window.
Also, depending on the implementation of the conversion server,
keyboard input may be taken from this window, so a window whose
keyboard events may be selected directly should be specified.
.PP
.I Target
specifies the type of converted text to be returned by the conversion
server.
If the conversion server supports this type, the requested type will
be returned, but if this is not the case, a different type will be
returned.
So this should not be viewed as a specification but rather as a hint
to the conversion server.
.I Property
is the name of the property in which converted data is returned.
If this is specified as None, the conversion server chooses a suitable
property name.
.PP
.I Option
is used to name a property containing various hints for the conversion
server.
However, this is intended for future extension and it can be specified
as None for now.
.NH 2
The Reply to the Input Conversion Request
.PP
The conversion server that received the CONVERSION_REQUEST event
prepares for input conversion for the client (for example, by popping
up an input conversion window).
At the same time, the server acknowledges receipt of the request by
sending a ClientMessage event to the client.
.dS
SendEvent
	destination: requestor window
	propagate: False
	event-mask: NoEventMask
	event: ClientMessage event
.dE
The contents of the event are as follows.
.dS
ClientMessage
	window: Selection Owner
	type: Atom "CONVERSION_NOTIFY"
	format: 32
	data:
		l[0]: selection atom
		l[1]: target
			conversion data type
		l[2]: property
			property name or None
		l[3]: conversion window
			ID of window where actual conversion is done
		l[4]: unused
.dE
.I Target
and
.I property
are used to inform the client of the type of conversion data and the
name of the property that stores the data, respectively.
If the conversion server is busy, or for some other reason cannot
honor the request, the
.I property
is set to None.
In this case,
.I target
and
.I
conversion window
.R
are undefined.
.PP
The
.I
conversion window
.R
is the ID of the window where the conversion actually takes place.
This ID is used if the client wishes to send events that arrive at the
requestor window to the conversion server.
(However, this is an implementation issue and will be discussed later
in a section about event handling.)
This window may or may not be the same as the selection owner window.
.PP
The client that receives this CONVERSION_NOTIFY event checks the
\fIproperty\fP, and if it is not None, it waits thereafter for changes
to the contents of the property.
.NH 2
Returning the Converted Data
.PP
Having accepted the conversion request, the conversion server begins
converting input.
The converted data is stored in the requestor's property using
\fBChangeProperty\fP.
.dS
ChangeProperty
	window: requestor window
	property: property name
	type: data type
	format: 8/16/32
	mode: Append
	data: converted data
.dE
.I Property
and
.I type
are as specified in the CONVERSION_NOTIFY event.
.PP
Upon receiving a PropertyNotify event, the client extracts the
converted data from the specified property, and processes the input
accordingly.
.PP
Data is sent repeatedly in this way until either the user tells the
conversion server to end the conversion, or the client terminates the
conversion through a CONVERSION_END_REQUEST, which is described below.
.NH 2
Conversion Termination
.PP
When conversion ends, the server notifies the client by sending a
ClientMessage event.
.dS
SendEvent
	destination: requestor window
	propagate: False
	event-mask: NoEventMask
	event: ClientMessage event
.dE
The contents of the event are as follows.
.dS
ClientMessage
	window: requestor window
	type: Atom "CONVERSION_END"
	format: 32
	data:
		l[0]: selection atom
		l[1]: owner
		    selection owner's window
		l[2], l[3], l[4]: unused
.dE
.NH 2
Conversion Termination Request
.PP
The client can end the conversion by sending the conversion server a
request.
This could be used when, for example, the cancel button of an input
conversion dialog is pressed.
.PP
The client requests conversion termination by sending the conversion
server a ClientMessage.
.dS
SendEvent
	destination: selection owner
	propagate: False
	event-mask: NoEventMask
	event: ClientMessage event
.dE
The contents of the event are as follows.
.dS
ClientMessage
	window: selection owner
	type: Atom "CONVERSION_END_REQUEST"
	format: 32
	data:
		l[0]: selection atom
		l[1]: requestor
		    requestor window
		l[2], l[3], l[4]: unused
.dE
.PP
Upon receiving this event, the server sends the client any unsent data
and then a CONVERSION_END ClientMessage.
.NH
Implementation
.PP
A few comments are given here about probable problems in implementing
this protocol.
Note that what follows is about implementation problems, and is not
part of the protocol definition.
.NH 2
User Feedback
.PP
When the conversion server receives a request from a client, it should
somehow let the user know about this.
For example, a conversion window could be popped up beneath the
requestor window.
.NH 2
Events
.PP
The protocol does not specify how to deal with keyboard events that
arrive at the requestor window while input conversion is taking place.
Some possible ways of dealing with these events are given here.
.IP "(1) Do nothing"
The easiest way is to do nothing in particular.
However, in this case, the user will have to take some special action
in order to have events sent to the conversion server.
For example, the user may have to move the pointer to a conversion
window.
This probably is not a very good interface.
.IP "(2) Client sends events to the conversion server"
Another way is to have the client forward keyboard events that arrive
at the requestor window during input conversion.
The client forwards the events to the conversion server using
XSendEvent().
This way, there is no need to move the pointer for input conversion.
However, forwarding keyboard events can be difficult, particularly if
the toolkit's translation table mechanism is used to dispatch events.
Also, the performance may worsen, as the events must go to the client
before reaching the conversion server.
In addition, if a suitable error handler is not installed, a
conversion server crash may kill the client as well.
.IP "(3) The conversion server selects requestor window events"
In this method, the conversion server selects keyboard events from the
requestor window.
In this case, in order to avoid processing of the keyboard events by
both the client and the conversion server, a convention will be needed
whereby the client does not select keyboard events or ignores them
during input conversion.
However, changing the event mask or ignoring keyboard events can be
difficult particularly when using the toolkit.
Also, if the conversion server crashes and the client does not receive
a conversion termination event, the client window will be left in a
state unresponsive to further keyboard input.
So this method should not be used.
.IP "(4) The conversion server steals requestor window events"
In this method, the conversion server intercepts events that would
normally go to the requestor window.
In order to do this, the conversion server can create an InputOnly
child window on top of the requestor window, and select keyboard
events.
This way, it is not necessary to move the pointer for every input
conversion, and the client does not need to take any special action.
However, if the requestor window has the input focus and the pointer
is not in the window, events cannot be intercepted.
.PP
Taking into account the above considerations and the desire to allow
easy integration into applications, method (4) would seem to be the
best.
In fact,
.I kinput
adopts this method.
.NH
Sample Client Library
.PP
Although it was stated above that only a protocol would be provided,
if there is no library to support it, its implementation can be
troublesome.
Therefore, a sample library that implements this protocol was built on
top of the X Toolkit Intrinsics (Xt).
This library makes it easy for a toolkit-based program to use a system
based on the suggested approach.
For more details, see
.B convlib.c
in the kterm source directory.
.PP
The R4
.I kterm
includes this sample library, and, together with \fIkinput\fP, allows
Japanese input.
