.\" Use tbl and -ms
.de Cp
.QP
Convention:
.I
..
.de Ce
.R
..
.de Pp
.QP
Problem:
.I
..
.de Pe
.R
..
.de Ip
.IP \(bu 3
..
.de Ls
.DS L
..
.de Le
.DE
..
.TL
_MIKEW_RESTORE_STATE proposal
.AU
Mike Wexler
.AI
FXD/Telerate, Inc.
.AB
.LP
This is a proposal for a new protocol  which I'm sending to this list for 
discussion. It could be a part of a Window and Session Manager's document, 
a stand-alone protocol registered with the X Registry, or both.
.LP
It intended to provide a reliable, efficient, and portable way for a SM to 
restore client properties such as window geometry and state. It does this 
by introducing a new protocol for SMs and WMs to communicate about client 
start-up.
.AE
.NH
Protocol
.LP
This is a description of the protocol. Which consists of several ClientMessage
events with type _MIKEW_RESTORE_STATE. In each of them data[2] will 
represent a specific state in the progress of the protocol. The states
are listed in Table 1.
.KF
.TS
center, box;
c s s
c c c
l l l.
Table 1 \- _MIKEW_RESTORE_STATE protcols states
_
State	Value	Comments
_
Initiating	0	SM about to start a client
Accepted	1	WM has accepted client startup
Rejected	2	WM has rejected client startup
ExecutionFailed	3	SM couldn't execute client
ExecutionFailedAck	4	WM acknowledges execution failure
WindowMapped	5	WM has mapped the clients window
.TE
.KE
.NH 2
WM's top-level window.
.LP
When a WM starts, it will create a top-level window with the class portion of the WM_CLASS property containing "WM" and a WM_PROTOCOLS property that 
contains the atom _MIKEW_RESTORE_STATE.
.NH 2
Initiation
.LP
The SM generates a unique id for the transaction it is about
to start. 
This id can't be reused until the WM replies with WindowMapped, Rejected,
or ExecutionFailedAck,  
and is used in further communications to uniquely identify this window.
It then attaches several properties to its top-level window.
The first three are used to identify a client window when it is mapped:
_MIKEW_WM_CLASS_XXX, _MIKEW_WM_CLIENT_MACHINE_XXX, and _MIKEW_WM_COMMAND_XXX.
In each of these properties XXX is replaced by the unique id.
These properties should be filled in with the types and values of 
WM_CLASS, WM_CLIENT_MACHINE, and WM_COMMAND 
when the client's state was saved. 
In addition there should be a property _MIKEW_WM_STATE of type _MIKEW_WM_STATE,
see Table 2.
The x location, y location, width, height, state, and gravity are used by 
the WM as if the user had specified them to the client program. 
.KF
.TS
center, box;
c s s
c c c
l l l.
Table 1 \- _MIKEW_WM_STATE type property contents
_
Field	Type	Comments
_
state	CARD32	see WM_STATE in ICCCM
x	INT32	Location of outer corner of window as specified by win_gravity
y	INT32
width	INT32	size of window
height	INT32
win_gravity	INT32	see WM_NORNAL_HINTS is ICCCM
.TE
.KE
.KS
.LP
Then the SM sends a ClientMessage event to the WM's top-level window
with:
.Ip
``window'' == SM's top level window
.Ip
``type'' == WM_PROTOCOLS
.Ip
``format'' == 32
.Ip
``data[0]'' == _MIKEW_RESTORE_STATE
.Ip
``data[1]`` == timestamp
.Ip
``data[2]'' == Initiating
.Ip
``data[3]`` == id
.KE
.NH 2
WM response
.KS
.LP
The WM responds with a ClientMessage event with:
.Ip
``type'' == WM_PROTOCOLS
.Ip
``format'' == 32
.Ip
``data[0]'' == _MIKEW_RESTORE_STATE
.Ip
``data[1]`` == timestamp
.Ip
``data[2]'' == Accepted | Rejected
.Ip
``data[3]`` == id
.KE
.LP
If the window manager replies with Rejected it means that the request made
will never succeed and should not be tried again.
.NH 2
Client startup
.LP
The SM starts the client on the host specified by client machine and 
with the command specified by command. Note that the client program is 
supposed to ensure that both WM_CLIENT_MACHINE, WM_COMMAND remain as 
specified at start-up until it receives a map notify from the WM. It is 
also expected to create a WM_CLASS property with the same contents as its 
prior invocation. The client should also create a WM_PROTOCOLS property 
containing _MIKEW_RESTORE_STATE to indicate that it follows these rules.
.KS
.LP
If execution fails the SM sends a ClientMessageEvent with:
.Ip
``type'' == WM_PROTOCOLS
.Ip
``format'' == 32
.Ip
``data[0]'' == _MIKEW_RESTORE_STATE
.Ip
``data[1]`` == timestamp
.Ip
``data[2]'' == ExecutionFailed
.Ip
``data[3]`` == id
.KE
.NH 2
Acknowledgement of execution failure
.KS
.LP
If the WM receives a execution failure message it respinds with a 
ClientMessage event with:
.Ip
``type'' == WM_PROTOCOLS
.Ip
``format'' == 32
.Ip
``data[0]'' == _MIKEW_RESTORE_STATE
.Ip
``data[1]`` == timestamp
.Ip
``data[2]'' == ExecutionFailedAck
.Ip
``data[3]`` == id
.KE
.NH 2
Window mapping
.KS
.LP
When the WM receives a map request for a window with the appropriate 
WM_CLIENT_MACHINE, WM_COMMAND, and WM_CLASS, it maps it as if the user 
requested the x location, y location, width, height, state, and window 
gravity specified in the initial _MIKEW_RESTORE_STATE event. It then 
sends a ClientMessage of type _MIKEW_RESTORE_STATE with:
.Ip
``type'' == WM_PROTOCOLS
.Ip
``format'' == 32
.Ip
``data[0]'' == _MIKEW_RESTORE_STATE
.Ip
``data[1]`` == timestamp
.Ip
``data[2]'' == WindowMapped
.Ip
``data[3]`` == id
.KE
.NH
Notes
.LP
Only one request can be pending with a particular host, class, command 
triple.
.LP
The clients can't change the WM_COMMAND, WM_CLASS, or WM_CLIENT_MACHINE 
until after they've received a map notify.
.LP
It is unclear whether this will support clients with multiple top-level 
windows.
