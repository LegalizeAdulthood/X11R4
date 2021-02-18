.de iX
.am xX
@@eindexentry{\$1;\n%}
\..
..
.de dX
.if \nx{\
.br
.nf
INDEX_XEDNI_INDEX_XEDNI_INDEX_XEDNI_INDEX_XEDNI
.xX
XEDNI_INDEX_XEDNI_INDEX_XEDNI_INDEX_XEDNI_INDEX
.}
..
.em dX
.AF ""
.MT 4
.rs
.DS C O
.sp 1i
.B
\&AIX Input Method
\&(Language Independent Input)
\&Version 1.2.1
.R
.sp 1i
.sp 2
\&July 10th, 1989
.sp 1i
\&Makoto Nishino
\&Frank Rojas
\&AIX Systems Development
\&IBM Japan, Ltd.
.sp 0.5i
.DE
.SK
.bp
/***********************************************************
Copyright International Business Machines Corporation 1989

		    All Rights Reserved

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the names of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
.bp
.bp
.H 1 "Introduction"
.P
This document will define the AIX Input Method Interface.  It is intended
for programmers who want to write applications that will
support input of multiple languages.
.P
The AIX Input Method Interface provides a programming interface that:
is language independent,  will support multiple keyboards
and provides 
MultiByte Character String (MBCS) input processing.  It is intended that
application programs, toolkits/graphic libraries and ADE's will
use this interface for their MBCS input support.  The AIX Input Method
Interface will be defined as of part future AIX products and
will be supported on all its environment.
.P
Each language will be packaged as a separate "Input Method" that
can be dynamically linked into an application at run time.  The
set of Input Methods to be provided by AIX is:

.BL
.LI
SBCS (English, French, Spanish, German, etc.)
.LI
Japanese
.LI
Chinese
.LI
Hangul
.LE 1
.P
The notion of "plugable" input methods is central to the 
AIX Input Method design.  The objective is to support a vast range 
of input methods to insulate users/applications from different 
localization
demands.  For example, some Chinese input methods use HW adapters to
perform their dictionary lookup, some IM's have extensive use of
menus to help 1st time users get accustomed to the system then
switch to a faster processing model, and other's are very simple.
The AIX Input Method will allow standard input methods to be supplied,
eg. Wnn/Jserver as well as allow vendor unique methods to 
be provided.  The user can make the choice.        
.P
AIX Input Method not only takes into consideration the simple task of 
converting keysyms to a string but also the interaction needed 
by local demands.
.P
\c
.B
Document Overview\c
.R \c
\&
.P
The Introduction chapter will provide the objectives and
terminology of the AIX Input Method.
.P
The the High Level Description chapter provides
an overview of the
Input Method Interface as well as a section on how to use the
interface.  It is intended for the general audience.
.P
The following chapter defines the C Language Interface for both
the IM API and the IM-CB API.  This chapter is intended to
be read by Input Method developers as well as programmers who
will use the AIX Input Method.
.P
.bp
.H 2 "Objectives"
.P
This document will define the Input Method API (IM-API) used
by applications to map keyboard input to a particular character set and
allow both SBCS and MBCS data entry.
In addition, this document will define the
Input Method Callback API (IM-CB API) used by Input
Methods to perform graphic dependent functions.
The low level graphic interface will be the X Library C Language
interface but others are possible.
.P
The following shows the goals of the AIX Input Method:
.BL
.LI
Phonic language independent interface.
.LI
Simple and well-defined API.
.LI
Independence of each graphic library and application.
.LI
Consistency across the entire AIX platform for language input.
.LE 1
.bp
.H 2 "Terminology"
.P
Two very important concepts needed to understand the AIX Input Method are
"\c
.B
pre-editing\c
.R \c
\&" and "\c
.B
real-editing\c
.R \c
\&".  These
two terms in addition to other common terms are provided here:
.VL 20 0
.LI "real-editing"
An editor such as vi or INed, or an editing object which provides
the user editing functions to make a suitable string before passing
it to the application program.
.br
The Kill & Erase edit or the Enhanced edit in the terminal driver
is an example of the real-editing which enables the user to edit a
character string prior to the sh or the other AIX command.
.LI "pre-editing"
In the MBCS countries, because of the complexity of the language system,
additional editing is required prior to the real-editing.
This additional editing is called the pre-editing in this document and
it includes RKC and KKC functions in the Japanese case.
.br
In the SBCS world, basically no pre-editing function exists.
However,
we can consider two key sequences for the diacritical character
and ALT-Numkey sequence as pre-editing functions in the SBCS world
because such character construction is done prior to the real-editing.
(BUT, no feedback is provided while composing the final string)
.LI "Input Method"
Input Method defines both the set of functions that provide the
input processing for a particular language and the pre-editing
rules required for that language.
.LI "application"
For the sake of this document, the term application defines the
level of code using the AIX Input Method.  This may include an application
program (eg. xterm or some editor), X Toolkits,  Motif,
ADE's or other graphic libraries.
.LE
.P
The following chart may help to understand the real-editing, the
pre-editing, and the Input Method.
.DS L
 
  #--------------$   #--------------$ 
  | AIX command  |   |    file      | 
  |              |   |              | 
  %--------------&   %--------------& 
         A                   A        
         |                   |        
  #--------------$   #--------------$ 
  |              |   |              | 
  |   termio     |   |  vi / INed   | 
  |              |   |              | 
  %--------------&   %--------------& 
      |      A           |      A     
      V      |           V      |     
  #--------------$   #--------------$ 
  | terminal  A  |   | terminal  A  | 
  |  emulator |  |   |  emulator |  | 
  |   |   #------'   |   |   #------' 
  |   |   |  IM  |   |   |   |  IM  | 
  |   |   %------'   |   |   %------' 
  |   |     |  A |   |   |     |  A | 
  |   V     V  | |   |   V     V  | | 
  %--------------&   %--------------& 
      |      A           |      A     
      V      |           V      |     
  #--------------$   #--------------$ 
  |    H / W     |   |    H / W     | 
  %--------------&   %--------------& 
 
    where,
       down arrow : echo back
       up arrow   : primary data flow
       IM         : Input Method
.DE
.bp
.H 2 "Related Documentation"
.BL
.LI
X Window System(Version 11 Release 2) Kanjification Specification
from the X Window Japanese Version Study Group, February 15, 1989.
.LI
X Window System (Version 11 Release 3) Specification of the
Kanji Input System
from the X Window Japanese Version Study Group, February 15, 1989.
.LE 1
.bp
.H 1 "High Level Description"
.P
The purpose of the AIX Input Method is to provide an API
by which an application can convert a key stroke into
a string in the character set it is using.  This conversion
process needs to take into account the keyboard being
used and the character set desired by the caller.  The
AIX Input Method will provide
various
\c
.B
"Input Method Editors" (IMED)\c
.R \c
\&
Input Method Editors
IMED
which perform the
input processing logic for MBCS and/or SBCS data entry
including: key code translation,  MBCS pre-editing,
and unique language conversion.
The simplest of these is the SBCS IM editor which
is a simple keyboard lookup table.  Yet, it does provide
such primitive pre-editing of Alt-Numpad sequences
and diacritical data entry.
.H 2 "Input Method Interface"
.P
The following is an architectural description of the components
of the AIX Input Method and their relationships to the application layer:
.br
.DS L
 
 #--------------------------------------------$
 |                                            |
 |     Application / Graphic Library / ADE    |
 |                                            |
 |   #----------------------------------------&
 |   |
 |   | >---------------- IM API ------------<
 |   |
 |   |                 #------------------------------------$
 |   |                 |        Chinese Input Method        |
 |   |               #-'-----------------------------------$|
 | R |               |        Japanese Input Method        ||
 | A | #----------$#-'------------------------------------$||
 | W | |          ||       SBCS Input Method              |||
 |   | |          |>--------------------------------------<||
 |   | |  IM Core || IM Front End Processing | IM Editor  |>&
 |   | |  (IMCORE)||          (IMFEP)        |  (IMED)    >&
 | I | %----------&%--------------------------------------&
 | N |
 | P |             >------- IM-CB API -------<
 | U |
 | T |             #-------------------------$
 |   |             |     Graphic Dependent   |
 |   |             |        Functions        |
 |   |             |       (CALLBACKS)       |
 %---&             %-------------------------&
 
 >------------- Graphic API -----------------<
 
 #-------------------------------------------$
 |                  XLIB                     |
 %-------------------------------------------&
   A                        |
   |                        V
  KEYBOARD               DISPLAY
 
.DE
.P
Normal input processing begins with the application obtaining a
raw keyboard event then passing it to the
the IMFEP for processing.  Depending on the key
code and the language processing of the IMFEP, a string is either
returned in the character set desired or nothing is
returned.
.P
For some Input Methods (in particular MBCS IM's),
the IMFEP may need to communicate directly with the end user,
if so, it will use the
\c
.B
"Input Method Callback API" (IM-CB API)\c
.R \c
\&
to access the graphic dependent
functions (CALLBACKS) provided by the application.  These CALLBACKS
are attached to the IMFEP by the application at initialization time.
Note that the CALLBACKS
are only used to perform output functions (and some query
information).
All input is still channeled via the application.
.P
\c
.B
Component Description\c
.R \c
\&
.P
.AL
.LI
IMCORE - IM Core
.P
The IMCORE is common to all Input Methods and its purpose is
to initialize a given language IMFEP/IMED.  The IMCORE is
always resident in any application and will perform the dynamic
linking to a particular language IMFEP.  As a result of
initializing, an IMFEP structure is created which contains
IMCORE information and pointers to the public functions of an
IMFEP (see IMFEP below).
.P
IMCORE provides the following public function:
.VL 20 0
.LI "IMInitialize()"
initializes a language Input Method and returns a handle to
this particular language processing.
.LI "IMClose()"
closes down a language Input Method.
.LI "IMQueryLanguage()"
checks if a particular language is available.
.LE
.LI
IMFEP - IM Front End Processing
IMFEP
.P
The main job of each language Input Method is to provide an
IMFEP to its IMED.  The IMFEP must provide the full set of
public
functions as defined by the IM-API and is restricted to using the
IM-CB API when interfacing with the end user for any pre-editing
or real-editing.
.P
All public functions of the IMFEP are
accessed indirectly via the IMFEP structure.  The following
are the functions provided by each IMFEP:
.VL 20 0
.LI "IMCreate()"
creates an Input Method object (IMObject).
Each object will
carry its own translation and internal state and is only accessible
by the IMFEP.  The definition of the IMFEP structure is dependent
on each language Input Method.
Each IMFEP will be able to manage multiple IMObjects.
IMObject
Input Method object
.LI "IMDestroy()"
destroys an IM object.
.LI "IMProcess()"
notifies the Input Method to process a key event for a
particular IMObject.  The return value depends on the type of IMFEP
and the key code passed in.
.LI "IMProcessAuxiliary()"
This is used to notify the Input Method about
input related to an Auxiliary area.
Refer below to IMAuxCreate for details about Auxiliary areas.
.LI "IMIoctl()"
used to control the state of an IMObject.  It can also be used
to suppress pre-editing.
.LE
.P
.LI
IMED - IM Editor
.P
The heart of each language Input Method is the IM Editor (IMED)
which does the real work for each
language Input Method yet its functions are hidden from the
application.  The reason for this is there are so many
MBCS Editors that have no common interface that can be agreed to.  For
example, there are client/server MBCS editors, phonic editors,
stoke editors or any combination of these.
.P
For the sake of understanding, it is best to view
the IMED as a string editor who only interfaces via internal
buffers.  Its job is to accept key events and fill in the
internal buffer.  It is IMFEP's job to present this internal buffer
either to the end user or the application.
.LI
CALLBACKS - Graphic Dependent Functions
CALLBACK function
.P
The CALLBACK functions are defined by the IM-CB API and are
attached at creation time (IMCreate) to an IMObject
via a CALLBACK structure.  The functions provided by the
CALLBACK function are:
.VL 20 0
.LI "IMTextDraw()"
whenever the IMFEP needs to draw the contents of the
internal buffer, it will call IMTextDraw() indirectly via the
CALLBACK structure.  The CALLBACK function is responsible for handling the
resources needed to draw the string on the screen, e.g. the
CALLBACK function may need to perform a window mapping function prior to
performing the drawing.
.P
The CALLBACK function is responsible for updating the screen properly
according to the changes in the string from the last IMTextDraw
operation, i.e. the CALLBACK function must track changes to draw correctly.
.LI "IMTextHide()"
this function is called to take down the string previously
drawn.  Any previous strings may be flushed.
.LI "IMTextStart()"
this function is called just after the pre-editing starts.
This function can be used to change the maximum length of
the text depending on the current text cursor position.
.LI "IMTextCursor()"
this function is called to interpret an up
or down keysym to determine the
new cursor position.  It takes into account if
there are multiple lines, whether the text wraps or not, and
the length of the text string.
.LI "IMAuxCreate()"
this function is called when an Input Method
needs to present some type of selection menu
or message to the user.
The intent is to separate these Auxiliary areas from the
normal processing of the application.
Auxiliary areas are usually separated from the
application's work area, a la separate windows.
.P
The IM-CB API is intended to be policy free
such that an input method could run under
various environments, e.g., X Windows, GSL, graPHIGS,
Display PostScript, Objective C, C++,
Presentation Manager, and even on a Asynch terminal
(given proper font support).
.P
Callbacks must be prepared to handle multiple
Auxiliary areas if they truly wish to be
multi-lingual.
.P
Without calling this CALLBACK function, the other CALLBACK functions for an Auxiliary area
are not invoked by the \c
.B
IMFEP\c
.R \c
\&.
.LI "IMAuxDraw()"
when a IMFEP needs the user to select a choice from a menu
or present some message, it will use the IMAuxDraw function to
draw into the Auxiliary area.  The CALLBACK function is responsible
for allocating the area needed to draw the menu/message on
the screen.
.LI "IMAuxHide()"
takes down the Auxiliary area from the screen.
.LI "IMAuxDestroy()"
this function is called to destroy the specified Auxiliary area.
.LI "IMIndicatorDraw()"
when the internal state of the IMED changes, the IMFEP will
call IMIndicatorDraw() to update the Indicator on the screen.
.LI "IMIndicatorHide()"
if the Indicator should not be showing, IMIndicatorHide()
will be called (for example, if the IMObject is suppressed).
.LI "IMBeep()"
causes the system to beep.
.LE
.LE 1
The following gives an overview of a single IMObject:
.DS L
 
 RUN TIME STRUCTURES ( and who creates them )
 
   IMInitialize        IMCreate            IM Caller
 
   #-----------$      #-----------$       #-----------$
   | Kanji FEP |      | IM Object |       | CALLBACK  |
   |-----------|      |-----------|       |-----------|
   |           |<--$  |           |       | callback  |---> NULL or
   |           |   %--| fep       |   #-->| text      |--$  other
   |           |      | callbacks |---&   | auxiliary |  |  CALLBACK
   |           |      | udata     |--$    | indicator |  |
   |           |      %-----------&  |    | beep      |  |
   | create    |--$                  |    %-----------&  |
   | destroy   |  |                  |                   |
   | process   |  |                  %--->#-----------$  |
   | ioctl     |  |                       | udata     |  |
   |           |  |                       |-----------|  |
   %-----------&  |                       | CB info   |  |
                  |                       | APPL info |  |
                  |                       %-----------&  |
--------------------------------------------------------------------
--------------------------------------------------------------------
                  |                                      |
 Executable Code  |                                      |
                  V                                      V
               #------------------$        #--------------------$
               | IMFEP Functions  |        | Callback Functions |
               |------------------|        |--------------------|
               | KanjiCreate()    |        | TextDraw()         |
               | KanjiDestroy()   |        | AuxilaryDraw()     |
               | KanjiProcess()   |        | IndicatorDraw()    |
               | KanjiIoctl()     |        | Beep()             |
               %------------------&        %--------------------&
 
.DE
.P
Both the CALLBACK and udata structures are allocated by the application
and are passed in at IMCreate() time.  The CALLBACK structure
is defined by the IM-CB API but it is the responsibility
of the application to fill it in.
.P
The udata structure may contain both CALLBACK function information and
application specific information.
Usually, the presentation specific attribute of each IMObject
will be contained in the udata passed in at IMCreate().  As an example,
the udata may contain x-y coordinates, application's cursor position,
width-height and/or font, or graphic context information.
The Input Method itself doesn't make any
restriction on the udata content.
.P
If the application wants to use multiple languages, an
IMFEP must be initialized for each language.
.P
If the application wants to use multiple IMObjects,
it must create multiple instances for each IMFEP.  The following
demonstrates a multiple IMObject configuration.
.P
.DS L
                   #-------$
                   | IMFEP |
                   %-------&
                       A
                       |
         --------------------------------
         A              A               A
         |              |               |
  #-------------$  #-------------$   #-------------$
  | IM Object A |  | IM Object B |   | IM Object C |
  %-------------&  %-------------&   %-------------&
         |               |               |
         %---------------&               |
                 |                       |
                 V                       V
            #-----------$          #-----------$
            | CALLBACKs |          | CALLBACKs |
            %-----------&          %-----------&
             |        |               |       |
             |        %---------------&       |
             |                |               |
             V                V               V
      IMTextDraw_1()    IMAuxDraw()        IMTextDraw_2()
                        IMIndicatorDraw()
                        IMBeep()
 
.DE
.P
In this example, objects (A, B & C) share the
same auxiliary, indicator, and beep functions.  While
only object A & B share the same text draw functions.
Object C uses the IMTextDraw_2 functions.
Usually, all of the CALLBACK functions are shared by all IMObjects
within a single application, but this is an implementer's choice
controlled by the CALLBACK structure.
.P
.H 3 "Input Method processing scheme"
.P
The AIX Input Method provides only the pre-editing functions.
This means the real-editing is controlled exterior to the
Input Method.
As long as the input key isn't a target of the pre-editing,
the translated string is \c
.B
immediately\c
.R \c
\& returned to
application.
Once pre-editing is started,
all input is used by or discarded by the Input Method
until the pre-editing ends by some terminating condition.
.P
Which key can be a target and which is can not is determined by the
IMED and varies according to the internal state of the IMED.
In the case of the Japanese Input Method, Hiragana characters
are usually the target of the pre-editing.
However, once the pre-editing starts,
Alpha-numeric characters also become targets.
.P
\c
.B
The most important purpose of this is
to provide the user the same interface to
the English input mechanism as long as the user doesn't input
MBCS characters.\c
.R \c
\&
.P
All character sets (Japanese SJIS, SBCS, Chinese and Korean)
include the ASCII character set (7-bit ASCII), the common character set
usually used to communicate with the computer system.
.P
The following chart shows typical input stream  (
for the IBM AIX Japanese input method).
.P
.DS L
 APPL -> IMFEP    -> CALLBACKS     -> APPL         explanation
---------------   ------------   ------------   ----------------------
   XK_a                          a              Appl. gets these
   XK_b                          b              characters immediately.
   XK_PF1                        esc[110q       Translated
Hiragana shift
   XK_a           H1                            Enter pre-editing
   XK_a           H1H2
Alphanum shift
   XK_a           H1H2a
   XK_b           H1H2ab
   XK_PF1                                       discarded
  Enter           (cleared)      H1H2ab         Exit pre-editing
                                                No Enter is returned
  Enter (again)                  Enter          Passed to APPL.
 
           where, H1, H2 indicates Hiragana characters.


.DE
.P
Once the pre-editing starts, input keys which don't have any
meaning to the Input Method are discarded and the
function returns a NULL string.
For example, PF keys don't have any
meaning for the Japanese Input Method.
The Japanese Input Method returns a NULL string
for these keys while in the pre-editing.
Of course, when not in the pre-editing phase,
these keys are returned as in the English case.
.P
In more detail, a certain string may be returned by the IMFEP
even if it's in the pre-editing phase. When and what string is
returned from the Input Method is completely controlled
by the Input Method.
The application can not make any assumptions and
should only handle the returned string when it is returned.
.P
Since "echo back" of the pre-editing is done by the IMFEP via
CALLBACK function, the input processing of the application must be able to be
preempted for output.
.P
CALLBACK functions should be written to be independent of the application
presentation if possible.  In a windowing environment this
is possible, but in a non-windowing environment special services
may be needed.
.sp v
.H 3 "Input to the Input Method"
.P
The AIX Input Method is intended to run on many keyboards as well
as be able to fit into a heterogeneous environment with other
vendors.  This causes a problem as to what the definition of
the key codes should be.
.P
To solve this difficulty, it has been decided that
input to IMProcess() will be based on the key symbol definitions
(keysym) defined by X Windows.  This definition encompasses all the major
keyboards defined and is maintained by the X Consortium.
.P
The definition of all possible keysyms is found in
/usr/include/X11/keysymdef.h.
Under the X environment the application should use the XLookupKeysym()
function using the base index (0) which should give the id of the keytop
on the keyboard. The Input Method will be responsible for translating the
base keysym for a given modifier state, see IMProcess().
.P
Input Methods will declare which modifier and which keysym set they
will use. AIX Input Method will also provide some convienient keyboard
mapping functions to handle modifier states from different vendors.
(NOTE: this can also be handle at the server if xmodmap is available.)
.sp v
.H 3 "Handling MBCS Shift States"
.P
Because the Input Method can not assume that ALL keyboard input
is passed into it, controlling of shift states  becomes a problem
in a multi-window environment.
This causes synchronization problems when a shift state of the
Input Method is changed while the focus is outside of the Input
Method.
.P
To maximize portability it is recommended that 
control of unique MBCS shift states
is done inside of the Input Method.  Also, Shift states should
be avoided when a Lock state will do.  
For example, the IBM Japanese Input Method assumes that 
the following modifiers are
controlled outside of the Input Method:
Shift, Ctrl, Alt-Right(Mod2Mask), Alt-Left(Mod1Mask) and 
Caps-Lock.  Yet, the 
Katakana and Hiragana states are handled internally.
.P
.H 3 "Character String Manipulated by AIX Input Method"
.P
Some character sets include characters whose display width
differs from the byte length of a code point.
( display width means a width of character
measured by column unit, namely, ASCII
characters have display width 1, SJIS Kanji characters
have display width 2.
These are called \c
.B
Half Width Character\c
.R \c
\& and \c
.B
Full
Width Character\c
.R \c
\&, respectively. )
Therefore, CALLBACK functions should not assume a particular character
set.
.P
The CALLBACK functions should treat all length from the input method as
the number of columns and not the number of bytes in the buffer.
.P
Usually, since all strings are allocated by the AIX Input Method, the caller
or CALLBACK functions do not need to treat the variable length of buffers.
.H 3 "Structure of the Auxiliary area"
.P
This section gives the structure of the Auxiliary area
to make it clear.
.P
The Auxiliary area contains 4 different types of data,
\c
.B
Title\c
.R \c
\&, \c
.B
Message\c
.R \c
\&, \c
.B
Button\c
.R \c
\&, and
\c
.B
Selection\c
.R \c
\&, although some of them may not be
used concurrently ( but could be ).
.VL 25 0 1
.LI "Title"
Title of the Auxiliary area. This is a string.
.LI "Message"
Message from the AIX Input Method. This is an array of
strings.
.LI "Button"
This indicates an action taken by the user.
.LI "Selection"
List of selectable items.
.LE
.P
The Auxiliary area can have multiple lists of selection items
concurrently.
Each selection list is called a panel.
Panel can be considered as a sub area of an Auxiliary area.
The following figure illustrates a sample appearance of
an Auxiliary area including 4 panels.
.DS L
 
      #---------------------------------------------$
      |#----------------$ #------------------------$|
      || item01  item02 | | item09  item10  item11 ||
      || item03  item04 | | item12  item13  item14 ||
      || item05  item06 | | item15  item16  item17 ||
      || item07  item08 | | item18  item19  item18 ||
      |%----------------& %------------------------&|
      |#----------------$ #------------------------$|
      || item19  item20 | | item23  item24  item25 ||
      || item21  item22 | | item26  item27  item28 ||
      |%----------------& %------------------------&|
      %---------------------------------------------&
 
.DE
.P
The most outer box designates the Auxiliary area and it includes
4 panels ( inner boxes ).
Each panel includes selectable items.
.P
Usually, the Auxiliary area will have one panel with
one column of items.
But CALLBACK functions must be prepared to handle multiple columns
as well as multiple panels.
Special representation is left to the CALLBACK functions.
.P
The following figure shows a sample data structure corresponding to
the above Auxiliary area ( some portions are omitted ).
.DS L
 
    IMAuxiliary
  #-----------------$     array of
  | panel_row = 2   |     IMPanel
  | panel_col = 2   |   #--------------$       array of
  | panel          -+-->| item_row = 4 |        IMItem
  |                 |   | item_col = 2 |        #-------$
  |                 |   | item        -+------->|       |   #--------$
  %-----------------&   (--------------)        | str  -+-->| item01 |
                        |              |        (-------)   %--------&
                        |              |        |       |   #--------$
                        |             -+-->     | str  -+-->| item02 |
                        (--------------)        (-------)   %--------&
                        |              |        |       |   #--------$
                        |              |        | str  -+-->| item03 |
                        |             -+-->     (-------)   %--------&
                        (--------------)            :            .
                        |              |        (-------)        .
                        |              |        |       |   #--------$
                        |             -+-->     | str  -+-->| item08 |
                        %--------------&        %-------&   %--------&
 
.DE
.P
These data structures are allocated by the \c
.B
IMFEP\c
.R \c
\& and
the \c
.B
IMFEP\c
.R \c
\& passes a pointer to the \c
.B
IMAuxiliary\c
.R \c
\&
structure to the CALLBACK functions.
.bp
.H 2 "Using the AIX Input Method"
.P
.P
This section describes what is required of the application writer and
clarifies the responsibility of the Input Method and that of the application.
The following are the steps required to be done for each application:
.AL
.LI
Initialization

.BL
.LI
Call IMQueryLanguages() to check if the desired language(s)
is/are available.
.LI
Call IMIntialize() to initialize the IMFEP.
.LI
Open the required fonts or other resources that may be needed
by the CALLBACK function.  Initialize the CALLBACK and udata structure.
.LI
Call IMCreate() to create each instance of input processing
desired.
.LE 1
.LI
Normal Processing
.BL
.LI
Application obtains a raw key event from graphic library (XNextEvent()
).
.LI
Application converts key code to keysym.  Xlib provides
function XLookupKeysym(ev,0).  
.P
.B Note:
the index should be 0.
.LI
Application routes the "base keysym" to the instance of IMObject
which contains the focus.  The keysym and modifier are passed to
the Input Method via IMProcess().  For single instances of IMObject
this is no problem.  For multiple instances the X Intrinsic
library can perform this routing automatically.
.LI
On return from IMProcess(), handle the returned string if any.
Usually, application treats this returned string as an input to the
real-editing.
.LE 1
.LI
CALLBACK Setup
.BL
.LI
The application defines the CALLBACK functions
which map to the IM-CB API.
.LI
Build its own CALLBACK structure containing the
address of the functions defined above.
.LE 1
.bp
.H 1 "C Language Interface"
.H 2 "Data Structures"
.P
The following is a description of the major structures
used throughout the API.
.H 3 "IMFepRec"
.P
is a structure which has information
for the \c
.B
IMCORE\c
.R \c
\&.
This structure is also defined as \c
.B
IMFepCommon\c
.R \c
\& structure.
\c
.B
IMFep\c
.R \c
\& is a type of pointer to this structure.
\c
.B
IMInitialize()\c
.R \c
\& allocates this structure and returns
a pointer to it.
.P
\c
.B
iminitialize\c
.R \c
\&is a function which returns a pointer to \c
.B
IMFepRec\c
.R \c
\& of each language Input Method.
See the description of \c
.B
IMInitialize()\c
.R \c
\&.
.br
\c
.B
version\c
.R \c
\& member holds the current version number of
the Input Method.
.br
\c
.B
language\c
.R \c
\& member points to NULL terminated character
string which is passed via \c
.B
IMInitialize()\c
.R \c
\& function.
.br
\c
.B
imerrno\c
.R \c
\& member is an area in which the error number
is stored by the \c
.B
IMFEP\c
.R \c
\&.
\c
.B
IMCORE\c
.R \c
\& copies the value of this to the global
variable \c
.B
imerrno\c
.R \c
\& if the error happens.
.H 3 "IMObjectRec"
.P
is a structure for
the common part of Input Method objects.
This is also called as \c
.B
IMObjectCommon\c
.R \c
\&.
\c
.B
IMObject\c
.R \c
\& is a pointer type which points to this
structure.
All \c
.B
IMOBJ\c
.R \c
\& must have the \c
.B
IMObjectCommon\c
.R \c
\&
structure as their first element.
The \c
.B
IMFEP\c
.R \c
\& is responsible for creating
the \c
.B
IMObject\c
.R \c
\& and initializing the
\c
.B
IMObjectCommon\c
.R \c
\& part.
.P
\c
.B
imfep\c
.R \c
\& is a pointer to the \c
.B
IMFepRec\c
.R \c
\& structure.
\c
.B
IMCreate()\c
.R \c
\& function binds this.
.br
\c
.B
cb\c
.R \c
\& is a pointer to the \c
.B
IMCallback\c
.R \c
\& structure
allocated and passed by the caller.
.br
\c
.B
mode\c
.R \c
\& member indicates the Processing Mode.
All AIX Input Methods have two modes,
\c
.B
Normal\c
.R \c
\& mode and \c
.B
Suppressed\c
.R \c
\& mode.
The initial mode is \c
.B
Normal\c
.R \c
\&, namely,
\c
.B
IMCreate()\c
.R \c
\& sets the Processing Mode to \c
.B
Normal\c
.R \c
\&
mode.
.br
In the \c
.B
Suppressed\c
.R \c
\& mode,
all key input is treated as the
English Input Method.
This means that the no pre-editing function
is done in this mode, in other words,
any CALLBACK functions never invoked by AIX Input Method in the \c
.B
Suppressed\c
.R \c
\&
mode.
.br
The valid value of \c
.B
arg\c
.R \c
\& is one of the following:
.VL 25 0 1
.LI "IMNormalMode"
Normal mode. Pre-editing is active.
.LI "IMSuppressedMode"
Suppressed mode. The pre-editing is suppressed.
.LE
The Processing Mode can be changed via \c
.B
IM_ChangeMode\c
.R \c
\&
command of \c
.B
IMIoctl()\c
.R \c
\&.
.br
\c
.B
udata\c
.R \c
\& is an optional area which the caller can use freely.
It has a type \c
.B
caddr_t\c
.R \c
\&
.H 3 "IMCallback"
.P
structure is used to register
CALLBACK functions to \c
.B
IMFEP\c
.R \c
\&.
.P
\c
.B
textmaxwidth\c
.R \c
\& is used to specify the display
maximum length of the Text area, namely,
maximum number of Half Width Characters.
The caller can not change the length beyond this value by
IM_ChangeLength command of IMIoctl().
Recommended value is 256 although no logical limitation exists.
.br
The other members of this structure are pointers to the
CALLBACK functions registering.
.P
CALLBACK functions are provided by the application, however, they
can be \c
.B
NULL\c
.R \c
\& pointers.
In this case, the corresponding CALLBACK functions are not invoked
although the internal information ( i.e., contents of
Text area, Auxiliary areas and so on ) are updated.
See the description of the
\c
.B
IMProcess()\c
.R \c
\& function.
.H 3 "IMTextInfo"
.P
holds information about Text area, namely,
the \c
.B
pre-editing\c
.R \c
\& string.
.P
\c
.B
text\c
.R \c
\& is a structure of type \c
.B
IMSTRATT\c
.R \c
\& and
it always points to the top of the whole string,
not the top of the changed portion.
.br
\c
.B
cur\c
.R \c
\& specifies the display location of the cursor in
the Text area.
.br
\c
.B
chgtop\c
.R \c
\& specifies the top of the changed portion
in the string as an offset value
and \c
.B
chglen\c
.R \c
\& specifies the length
of the changed portion.
These values are given as the offset and the length in byte.
.br
\c
.B
maxwidth\c
.R \c
\& specifies maximum width of the Text area.
This value specifies the width on the display.
It is a constant value and equal to the value specified in
the \c
.B
IMCallback\c
.R \c
\& structure at the \c
.B
IMCreate()\c
.R \c
\&
time.
.H 3 "IMAuxInfo"
.P
this structure defines the contents of the
Auxiliary area and the type of processing requested of it.
.P
\c
.B
title\c
.R \c
\& is structure of type \c
.B
IMSTR\c
.R \c
\& and
contains a title string.
\c
.B
title.str\c
.R \c
\& points to the title string and its length in
byte is given by \c
.B
title.len\c
.R \c
\&.
If \c
.B
title.len\c
.R \c
\& is \c
.B
0\c
.R \c
\&, this Auxiliary area
has no title.
.P
\c
.B
message\c
.R \c
\& is a structure of type \c
.B
IMMessage\c
.R \c
\&
and it contains message texts.
\c
.B
message.nline\c
.R \c
\& indicates the number of lines the
messages occupy.
If this value is \c
.B
0\c
.R \c
\&, no message exists.
Each message text and its attributes are accessed by
\c
.B
message.text#n$->str\c
.R \c
\& and
\c
.B
message.text#n$->att\c
.R \c
\&, respectively.
Length is given by \c
.B
message.text#n$->len\c
.R \c
\&.
( where, n of #n$ is an index to the array of \c
.B
IMSTRATT\c
.R \c
\&
structure. )
\c
.B
message.cursor\c
.R \c
\& indicates whether or not this message
associates a text cursor.
If its value is \c
.B
TRUE\c
.R \c
\&, the message has a text cursor.
In this case, \c
.B
message.cur_row\c
.R \c
\& and
\c
.B
message.cur_col\c
.R \c
\& designates the cursor position on
the display.
\c
.B
maxwidth\c
.R \c
\& specifies the maximum width of text on the
display.
.P
\c
.B
selection\c
.R \c
\& is a structure of type \c
.B
IMSelection\c
.R \c
\&.
\c
.B
selection.panel\c
.R \c
\& is a pointer points to the array of
\c
.B
IMPanel\c
.R \c
\& structure.
This array is 2-dimensional array and its size is specified by
\c
.B
selection.panel_row\c
.R \c
\& and \c
.B
selection.panel_col\c
.R \c
\&.
Each panel contains a 2-dimensional array of items of type
\c
.B
IMItem\c
.R \c
\&.
Size of the items array is given by the members of \c
.B
IMPanel\c
.R \c
\&
structure, \c
.B
item_row\c
.R \c
\&, and \c
.B
item_col\c
.R \c
\&.
\c
.B
maxwidth\c
.R \c
\& member of this structure gives the maximum
length on the display of the item string so that the panel width
can be calculated by the formula,
.P
\c
.B
maxwidth * item_col * pixels_per_column\c
.R \c
\&
.P
\c
.B
IMItem\c
.R \c
\& structure contains a flag named \c
.B
selectable\c
.R \c
\&.
\c
.B
TRUE\c
.R \c
\& value for this member indicates the item is
selectable.
.br
If the value \c
.B
selection.panel_row * selection.panel_col\c
.R \c
\&
is \c
.B
0\c
.R \c
\&, this Auxiliary area doesn't have selection.
.P
\c
.B
button\c
.R \c
\& member indicates what type of button this
Auxiliary area has:
.VL 24 0 1
.LI "IM_OK"
OK button.
.LI "IM_OKCANCEL"
OK and CANCEL button.
.LI "IM_ENTER"
ENTER button.
.LI "IM_ENTERCANCEL"
ENTER and CANCEL button.
.LI "IM_RETRYCANCEL"
RETRY and CANCEL button.
.LI "IM_ABORTRETRYIGNORE"
.br
Abort, Retry and Ignore button.
.LI "IM_YESNO"
YES/NO button.
.LI "IM_YESNOCANCEL"
YES/NO and CANCEL button.
.LI "IM_HELP"
HELP button.
.LI "IM_NONE"
no button.
.LE
.P
\c
.B
hint\c
.R \c
\& gives the CALLBACK functions where the Auxiliary is
displayed.
The following are valid values for \c
.B
hint\c
.R \c
\&.
.VL 30 0 1
.LI "IM_AtTheEvent"
AIX Input Method expects the Auxiliary area is shown at the position
the event occured.
The recommendation is the position near the current text cursor
position.
.LI "IM_UpperLeft"
The Auxiliary area is expected to be located aligned from
the upper left corner.
.LI "IM_UpperRight"
The Auxiliary area is expected to be located aligned from
the upper right corner.
.LI "IM_LowerLeft"
The Auxiliary area is expected to be located aligned from
the lower left corner.
.LI "IM_LowerRight"
The Auxiliary area is expected to be located aligned from
the lower right corner.
.sp v
.LE
.br
\c
.B
status\c
.R \c
\& member holds one of the following values:
.VL 30 0 1
.LI "IMAuxShowing"
If the Auxiliary area is shown both before and after the last call
of the \c
.B
IMProcess()\c
.R \c
\& and no updates were made.
.LI "IMAuxUpdated"
If the Auxiliary area is shown both before and after the last call
of the \c
.B
IMProcess()\c
.R \c
\& and its contents have been updated.
.LI "IMAuxShown"
If the last \c
.B
IMProcess()\c
.R \c
\& call caused the Auxiliary
area to be shown.
.LI "IMAuxHiding"
If the Auxiliary area is hidden both before and after the last call
of the \c
.B
IMProcess()\c
.R \c
\&.
.LI "IMAuxHidden"
If the last \c
.B
IMProcess()\c
.R \c
\& call caused
the Auxiliary area to be hidden.
.sp v
.LE
.br
The other functions such as
\c
.B
IMProcessAuxiliary()\c
.R \c
\& and \c
.B
IMIoctl()\c
.R \c
\& also
affect the value of \c
.B
status\c
.R \c
\&, although non destructive
functions such as Query commands of \c
.B
IMIoctl()\c
.R \c
\&
are not a subject of this.
.H 3 "IMIndicatorInfo"
.P
is defined as bit field and
indicates the current value of the Indicators.
.P
Some fields have common meanings across the
(phonic) language (size, insert).
.P
\c
.B
size\c
.R \c
\& field indicates the size of the character.
.VL 30 0 1
.LI "IMHalfWidth"
Half Width Character input mode.
.LI "IMFullWidth"
Full Width Character input mode.
.LE
\c
.B
insert\c
.R \c
\& field indicates \c
.B
insert/replace\c
.R \c
\& mode.
.VL 30 0 1
.LI "IMInsertMode"
Half Width Character input mode.
.LI "IMReplaceMode"
Full Width Character input mode.
.LE
.sp v
The last field \c
.B
unique\c
.R \c
\& is
specific to the (phonic) languages.
.P
CALLBACK function can either display their own indicator or can
request the default string representing it using the
\c
.B
IMQueryIndicatorString\c
.R \c
\& command of
the \c
.B
IMIoctl()\c
.R \c
\& function.
.H 3 "IMSTR"
.P
is a structure type to designate non NULL
terminated strings.
.P
\c
.B
str\c
.R \c
\& member is a pointer to the string and its length
is specified by \c
.B
len\c
.R \c
\& member in byte.
.H 3 "IMSTRATT"
.P
is a structure type to designate the non NULL
terminated string and its attribute.
.P
\c
.B
str\c
.R \c
\& member is a pointer to the string and its length
is specified by \c
.B
len\c
.R \c
\& member in byte.
.br
\c
.B
att\c
.R \c
\& member is a pointer to the attribute string and
its length in byte is also designated by \c
.B
len\c
.R \c
\& member.
.bp
.H 2 "IM API"
.H 3 "IMQueryLanguage"
.DS L
uint        IMQueryLanguage(language)
IMLanguage  language;
.DE
.P
This function is used to check to see if the specified (phonic)
\c
.B
language\c
.R \c
\& is supported or not.
.br
This function returns \c
.B
TRUE\c
.R \c
\& if the specified language is
supported. Otherwise, \c
.B
FALSE\c
.R \c
\& is returned.
.P
.H 3 "IMInitialize"
.DS L
IMFep      IMInitialize(language)
IMLanguage language;
.DE
.P
This function initializes the \c
.B
IMFEP\c
.R \c
\& of the AIX Input Method
for a particular language.
Each \c
.B
IMFEP\c
.R \c
\& can produce one or more IMObject which are
created calling the \c
.B
IMCreate()\c
.R \c
\& function.
.P
\c
.B
language\c
.R \c
\& specifies the language to be used and
will enable the \c
.B
IMCORE\c
.R \c
\& to dynamically link to
the \c
.B
IMFEP\c
.R \c
\&.
Language is defined by each Input Method installed. 
.P
If \c
.B
IMInitialize()\c
.R \c
\& succeeds, it returns a handle of type
\c
.B
IMFep\c
.R \c
\&. Otherwise, \c
.B
NULL\c
.R \c
\& is returned and
error code is set to the external variable \c
.B
imerrno\c
.R \c
\&.
.H 3 "IMClose"
.DS L
void    IMClose(imfep)
IMFep   imfep;
.DE
.P
This function closes the \c
.B
IMFEP\c
.R \c
\&.
All IMObjects
created must be destroyed using the \c
.B
IMDestroy()\c
.R \c
\&
before calling this function,
otherwise some garbage remains in the memory.
.H 3 "IMCreate"
.DS L
IMObject    IMCreate(imfep, imcallback, udata)
IMFep       imfep;
IMCallback  *imcallback;
caddr_t     udata;
.DE
.P
This function creates one instance of an IMObject
for a particular \c
.B
IMFEP\c
.R \c
\&.
Several IMObjects can be created under one \c
.B
IMFEP\c
.R \c
\&.
This function returns an IMObject identifier of type
\c
.B
IMObject\c
.R \c
\&.
.P
\c
.B
imfep\c
.R \c
\& is a handle to the \c
.B
IMFEP\c
.R \c
\&.
.br
\c
.B
imcallback\c
.R \c
\& is a pointer to the caller supplied
\c
.B
IMCallback\c
.R \c
\& structure.
.br
\c
.B
udata\c
.R \c
\& is an optional one. It may be used to
pass an application's own information to the CALLBACK functions.
Using this, the application
can avoid the external references from the CALLBACK functions.
The AIX Input Method never changes this parameter, it merely passes it to
the CALLBACK functions.
\c
.B
udata\c
.R \c
\& is usually a pointer to the application data structure
which may contain the information about location, font id and so forth.
.P
This function returns \c
.B
NULL\c
.R \c
\& if the error happens.
In this case,
error code is set to the external variable \c
.B
imerrno\c
.R \c
\&.
.H 3 "IMDestroy"
.DS L
void     IMDestroy(im)
IMObject im;
.DE
.P
This destroys an IMObject specified by the parameter \c
.B
im\c
.R \c
\&.
.H 3 "IMProcess"
.DS L
int         IMProcess(im, keysym, state, str, len)
IMObject    im;
KeySym      keysym;
uint        state;
caddr_t     *str;              /* RETURN  :  not null terminate */
uint        *len;              /* RETURN  :  length in byte */
.DE
.P
This is the main processing function of the AIX Input Method.
.P
\c
.B
IMProcess()\c
.R \c
\& processes one keyboard event
at a time.
.P
Refer to keysymdef.h for definition of all possible keysym.
.P
\c
.B
str\c
.R \c
\& holds the returned string.
Returning \c
.B
NULL\c
.R \c
\& means
that the input is used or discarded by the AIX Input Method.
Length of \c
.B
str\c
.R \c
\& in byte
is stored in the \c
.B
len\c
.R \c
\& parameter.
.br
\c
.B
NOTE :\c
.R \c
\& \c
.B
str\c
.R \c
\& is \c
.B
NOT\c
.R \c
\& a null
terminated string.
.P
The return code of this function has one of the following meanings:
.VL 22 0 1
.LI "IMError"
error caused during this function
.LI "IMTextAndAuxiliaryOff"
no text string in the Text area and the Auxiliary area
is not shown
.LI "IMTextOn"
text string in the Text area but no Auxiliary area
.LI "IMAuxiliaryOn"
no text string in the Text area but the Auxiliary area is shown
.LI "IMTextandAuxiliaryOn"
text string in the Text area and the Auxiliary area is shown
.LE
.P
With this return value,
the application can know about the condition of
the Text area and the Auxiliary area.
Some applications may want to suppress output
if the Auxiliary area is shown
while some applications may want to suppress output
if the Text area has positive length string.
.P
Processing of the \c
.B
IMProcess()\c
.R \c
\& may look like
the following:
.AL
.LI
Validates the \c
.B
im\c
.R \c
\& and does any internal state checking
needed to interface with its \c
.B
IMED\c
.R \c
\&.
E.g., deactivation of the last IMObject,
activation of the input IMObject.
.LI
Keyboard translation for all its
supported modifier states.
.LI
Invokes internal function of \c
.B
IMED\c
.R \c
\& to do language
dependent processing.
.LI
Performs any necessary CALLBACK function depending on the internal
state.
.LI
Returns to application setting str & len appropriately.
.LE 1
.P
This function returns \c
.B
IMError\c
.R \c
\& if the error happens.
In this case, error code is set to the
external variable \c
.B
imerrno\c
.R \c
\&.
.H 3 "IMProcessAuxiliary"
.DS L
int      IMProcessAuxiliary(im, auxid, button, panel_row, panel_col,
                            item_row, item_col)
IMObject im;
caddr_t  auxid;
uint     button;
uint     panel_row;
uint     panel_col;
uint     item_row;
uint     item_col;
.DE
.P
This function is used to notify the Input Method
of input for an Auxiliary area.
.P
\c
.B
auxid\c
.R \c
\& identifies the Auxiliary area that
has process.
.P
\c
.B
button\c
.R \c
\& tells the type of input.
.VL 24 0 1
.LI "IM_OK"
OK button is pushed.
.LI "IM_CANCEL"
CANCEL button is pushed.
.LI "IM_ENTER"
ENTER button is pushed.
.LI "IM_RETRY"
RETRY button is pushed.
.LI "IM_ABORT"
ABORT button is pushed.
.LI "IM_IGNORE"
IGNORE button is pushed.
.LI "IM_YES"
YES button is pushed.
.LI "IM_NO"
NO button is pushed.
.LI "IM_HELP"
HELP button is pushed.
.LI "IM_SELECTED"
Selection has been made.
Only in this case, \c
.B
panel_row\c
.R \c
\&, \c
.B
panel_col\c
.R \c
\&,
\c
.B
item_row\c
.R \c
\&, and \c
.B
item_col\c
.R \c
\&
have meaningful values.
.LE
.P
\c
.B
panel_row\c
.R \c
\& and \c
.B
panel_col\c
.R \c
\& indicate the panel
on which the selection event occurred.
\c
.B
item_row\c
.R \c
\& and \c
.B
item_col\c
.R \c
\& indicate the selected
item.
.H 3 "IMIoctl"
.DS L
int      IMIoctl(im, op, arg)
IMObject im;
int      op;
char     *arg;
.DE
.P
The \c
.B
IMIoctl()\c
.R \c
\& function performs a variety of control
or query operations on the AIX Input Method specified
by the \c
.B
im\c
.R \c
\& parameter.
.P
The \c
.B
op\c
.R \c
\& parameter specifies the operation,
and the use of the \c
.B
arg\c
.R \c
\&
parameter depends on the particular operation performed.
In addition \c
.B
IMIoctl()\c
.R \c
\& can be used to control the
unique functions of each language Input Method.
.P
This function returns \c
.B
IMError\c
.R \c
\& if the error happens.
In this case, error code is set to the
external variable \c
.B
imerrno\c
.R \c
\&.
.P
The following commands are valid for the AIX Input Method:
.VL 18 0 1
.LI "IM_Refresh"
The application can use this command to
refresh Text area, Auxiliary area, and Indicator
calling CALLBACK functions.
CALLBACK functions are invoked only if
their contents are not empty.
\c
.B
arg\c
.R \c
\& is meaningless.
.LI "IM_GetString"
The application can use this command to get the current pre-editing
string.
\c
.B
arg\c
.R \c
\& is an address of the
\c
.B
IMSTR\c
.R \c
\& structure supplied by the caller.
\c
.B
IMFEP\c
.R \c
\& sets \c
.B
arg->len\c
.R \c
\& and
\c
.B
arg->str\c
.R \c
\& member.
.br
This command updates ( clears )
the screen invoking the CALLBACK function
\c
.B
IMTextDraw()\c
.R \c
\& and the current
contents of the Text area are discarded if it exists.
.br
If the \c
.B
pre-editing\c
.R \c
\& string doesn't exist,
\c
.B
len\c
.R \c
\& member of the \c
.B
IMSTR\c
.R \c
\& is set to \c
.B
0\c
.R \c
\&.
.P
\c
.B
calling sequence\c
.R \c
\&
.DS L
IMSTR     imstr;
 
IMIoctl(im, IM_GetString, &imstr);
.DE
.LI "IM_Clear"
This command clears the Text area and also clears ( hides )
the Auxiliary area if they are currently displayed.
.br
If the parameter \c
.B
arg\c
.R \c
\& is not \c
.B
NULL\c
.R \c
\&,
it invokes the CALLBACK functions to clear the screen.
However, if it's \c
.B
NULL\c
.R \c
\&,
the CALLBACK functions are not invoked, only the internal information
is cleared.
.sp v
.LI "IM_Reset"
This command clears ( hides ) the Auxiliary area if it's
currently displayed.
.br
If \c
.B
arg\c
.R \c
\& is not \c
.B
NULL\c
.R \c
\&,
this function invokes the CALLBACK function
\c
.B
IMAuxHide()\c
.R \c
\& to hide the Auxiliary area,
however doesn't call the CALLBACK function \c
.B
IMTextDraw()\c
.R \c
\&.
.P
If \c
.B
arg\c
.R \c
\& is \c
.B
NULL\c
.R \c
\&,
this command only clears the
internal Auxiliary area, but doesn't call the CALLBACK function.
.sp v
.LI "IM_ChangeLength"
This command can be used to change the maximum length of the text
string.
The length is a length of string on the display ( multiplication
of column ).
\c
.B
arg\c
.R \c
\& must be equal or less than the textmaxwidth value
specified in the \c
.B
IMCreate()\c
.R \c
\& function.
Also the length must be equal or greater than
the current text length ( if exists ),
otherwise it causes an error.
The caller is recommended to invoke \c
.B
IMClear()\c
.R \c
\&
before calling this function.
.LI "IM_QueryState"
.DS L
typedef struct  {
	uint    mode;         /* Processing Mode */
	uint    text;         /* Text area */
	uint    aux;          /* Auxiliary area */
	uint    indicator;    /* Inidcator */
	int     beep;         /* Beep percent */
}       IMQueryState;
.DE
.P
This command can be used to see the status of the AIX Input Method.
.br
\c
.B
arg\c
.R \c
\& is an address of the caller supplied
\c
.B
IM_QueryState\c
.R \c
\& structure shown above.
\c
.B
IMFEP\c
.R \c
\& sets the state information to this sturcture
pointed by \c
.B
arg\c
.R \c
\&.
.br
\c
.B
mode\c
.R \c
\& member is set to the current Processing Mode,
\c
.B
IMNormalMode\c
.R \c
\& or \c
.B
IMSuppressedMode\c
.R \c
\&.
.br
Members \c
.B
text\c
.R \c
\&, \c
.B
aux\c
.R \c
\& and \c
.B
indicator\c
.R \c
\&
are set to \c
.B
TRUE\c
.R \c
\&
if any changes of the Text area, the Auxiliary areas, and
the indicator area, respectively, have been
made by the last call of \c
.B
IMProcess()\c
.R \c
\&,
\c
.B
IMProcessAuxiliary()\c
.R \c
\&, or \c
.B
IMIoctl()\c
.R \c
\&.
However, NON-Destructive calls, namely, IM_QueryState and
the other query commands, are not a subject.
These are set to \c
.B
FALSE\c
.R \c
\& if there are no changes.
.br
\c
.B
beep\c
.R \c
\& can be used to check
if the last invoking of \c
.B
IMProcess()\c
.R \c
\& or
the other destructive functions
caused beep or not.
The level of beep is set
to this member and the value \c
.B
-100\c
.R \c
\& means no beep.
.br
This command can be used from the CALLBACK functions.
.P
\c
.B
calling sequence\c
.R \c
\&
.DS L
IMQueryState    state;
 
IMIoctl(im, IM_QueryState, &state);
.DE
.LI "IM_QueryText"
This command returns information related to the Text area.
.br
\c
.B
arg\c
.R \c
\& is a pointer to \c
.B
IMQueryText\c
.R \c
\& structure
defined as follows:
.DS L
typedef struct {
	IMTextInfo    *textinfo;        /* RETURN */
}       IMQueryText;
.DE
.br
\c
.B
IMFEP\c
.R \c
\& allocates \c
.B
IMTextInfo\c
.R \c
\& structure
and sets \c
.B
textinfo\c
.R \c
\& member of the above structure
to point to this \c
.B
IMTextInfo\c
.R \c
\& structure.
If the Text area doesn't exist, \c
.B
NULL\c
.R \c
\& is set to
\c
.B
textinfo\c
.R \c
\& member.
.br
This command can be issued from the CALLBACK functions.
.P
\c
.B
calling sequence\c
.R \c
\&
.DS L
IMQueryText     text;
 
IMIoctl(im, IM_QueryText, &text);
.DE
.LI "IM_QueryAuxiliary"
This command returns information about the specified
Auxiliary area if it's shown now.
.br
\c
.B
arg\c
.R \c
\& is a pointer to the \c
.B
IMQueryAuxiliary\c
.R \c
\&
structure defined as follows:
.DS L
typedef struct  {
	caddr_t    auxid;        /* id of Auxiliary area */
	IMAuxInfo  *auxinfo;     /* RETURN */
}       IMQueryAuxiliary;
.DE
.br
\c
.B
auxid\c
.R \c
\& is an identifier of the Auxiliary area
whose information is to be gotten.
\c
.B
IMFEP\c
.R \c
\& allocates an \c
.B
IMAuxInfo\c
.R \c
\& structure
and sets the \c
.B
auxinfo\c
.R \c
\& member to point to this
\c
.B
IMAuxInfo\c
.R \c
\& structure.
\c
.B
auxinfo\c
.R \c
\& member is set to \c
.B
NULL\c
.R \c
\& if the
specified Auxiliary area is not shown now.
.br
This command can be issued from the CALLBACK functions.
.P
\c
.B
calling sequence\c
.R \c
\&
.DS L
IMQueryAuxiliary   aux;
 
aux.auxid = auxid;
IMIoctl(im, IM_QueryAuxiliary, &aux);
.DE
.LI "IM_QueryIndicator"
This command returns the current indicator value if the
Indicator exists.
.br
\c
.B
arg\c
.R \c
\& is a pointer to \c
.B
IMQueryIndicator\c
.R \c
\&
structure defined as follows:
.DS L
typedef struct  {
	IMIndicatorInfo   *indicatorinfo; /* RETURN */
}       IMQueryIndicator;
.DE
.br
\c
.B
IMFEP\c
.R \c
\& allocates a \c
.B
IMIndicatorInfo\c
.R \c
\& structure
and sets \c
.B
indicatorinfo\c
.R \c
\& member to point to it.
.br
The \c
.B
indcatorinfo\c
.R \c
\& member is set to \c
.B
NULL\c
.R \c
\& if the
Indicator area doesn't exist.
.br
This command can be issued from the CALLBACK functions.
.P
\c
.B
calling sequence\c
.R \c
\&
.DS L
IMQueryIndicator   ind;
 
IMIoctl(im, IM_QueryIndicator, &ind);
.DE
.LI "IM_QueryIndicatorString"
.DS L
typedef struct  {
    uint         format;   /* IMShortForm/IMLongForm */
    IMSTR        indicator;  /* RETURN */
}   IMQueryIndicatorString;
.DE
.P
This command returns the formatted indicator corresponding
to the current indicator value.
.br
Each AIX Input Method would have two types of indicator format,
.VL 18 0 1
.LI "IMShortForm"
short format.
.LI "IMLongForm"
long format.
.LE
.br
Indicator string is set to \c
.B
indicator.str\c
.R \c
\& and
its length is set to \c
.B
indicator.len\c
.R \c
\&.
.br
The caller is requested to supply this structure and passes
a pointer to this structure as the \c
.B
arg\c
.R \c
\& parameter.
.br
If no Indicator exists now, \c
.B
indicator.len\c
.R \c
\& member
is set to \c
.B
NULL\c
.R \c
\&.
This command can be invoked from the CALLBACK functions.
.P
\c
.B
calling sequence\c
.R \c
\&
.DS L
IMQueryIndicatorString    indstr;
 
indstr.format = IMLongForm;
IMIoctl(im, IM_QueryIndicatorString, &indstr);
.DE
.LI "IM_ChangeMode"
This command is used to set the Processing Mode of the AIX Input Method
to the mode specified by \c
.B
arg\c
.R \c
\&.
\c
.B
arg\c
.R \c
\& is \c
.B
IMNormalMode\c
.R \c
\& or
\c
.B
IMSuppressedMode\c
.R \c
\&.
.br
.LE
.bp
.H 2 "IM-CB API"
.P
This section describes the interface to each
CALLBACK function in the form that they would be written.
Yet, the functions are actually accessed indirectly
via an IMCallback structure.
.P
The CALLBACK functions return \c
.B
IMNoError\c
.R \c
\& on successful calling.
If an error happens during the call of CALLBACK function, the CALLBACK function returns
\c
.B
IMError\c
.R \c
\& and it
is a fatal to \c
.B
IMFEP\c
.R \c
\&, the \c
.B
IMFEP\c
.R \c
\&
returns \c
.B
IMCallbackError\c
.R \c
\& to the caller.
Otherwise, \c
.B
IMFEP\c
.R \c
\& takes a proper action, i.e., resetting
the internal information, and returns the caller \c
.B
IMNoError\c
.R \c
\&.
.H 3 "IMTextDraw"
.DS L
int         IMTextDraw(im, textinfo, udata)
IMObject    im;
IMTextInfo  *textinfo;
caddr_t     udata;
.DE
.P
This function is invoked by the \c
.B
IMFEP\c
.R \c
\& whenever it needs
to update the screen with it's internal string.
.P
\c
.B
textinfo\c
.R \c
\& is a pointer to
the \c
.B
IMTextInfo\c
.R \c
\& structure.
.br
\c
.B
udata\c
.R \c
\& is an application data specified
in the parameter of the
\c
.B
IMCreate()\c
.R \c
\& function.
.br
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMTextHide"
.DS L
int      IMTextHide(im, udata)
IMObject im;
caddr_t  udata;
.DE
.P
This function is invoked by the \c
.B
IMFEP\c
.R \c
\&
when the corresponding
IMObject is about to be cleared.
.br
\c
.B
udata\c
.R \c
\& is an application data specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.br
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMTextStart"
.DS L
int      IMTextStart(im, space, udata)
IMObject im;
int      *space;                /* RETURN */
caddr_t  udata;
.DE
.P
This function is invoked by the \c
.B
IMFEP\c
.R \c
\&
when the pre-editing is started, prior to
drawing anything.
The purpose of this function is to notify the \c
.B
IMFEP\c
.R \c
\&
of the length of the pre-editing space.
This function sets the length of the available
space ( >= 0 ) on the display
to the integer pointed by \c
.B
space\c
.R \c
\&.
Setting a value \c
.B
-1\c
.R \c
\& is acceptable to indicate
the pre-editing space is dynamic.
.P
For example, if the Text area where the pre-editing
string is drawn to has a fixed length, and growing the pre-editing
string beyond the right most boundary wouldn't be expected,
changing the maximum length of the pre-editing string
must be possible
because the pre-editing usually starts at the current cursor
position.
.P
This function is called by the IMProcess() before
the IMTextDraw() is called.
.P
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMTextCursor"
.DS L
int      IMTextCursor(im, direction, cur, udata)
IMObject im;
uint     direction;
int      *cur;                 /* RETURN */
caddr_t  udata;
.DE
.P
This function is invoked by the \c
.B
IMFEP\c
.R \c
\& when the cursor
up or down key is input to the \c
.B
IMProcess()\c
.R \c
\&.
.P
Up or Down is informed through the \c
.B
direction\c
.R \c
\& parameter.
.br
\c
.B
udata\c
.R \c
\& is an application data specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.br
This function sets the new display cursor position in the
Text area to the integer pointed by \c
.B
cur\c
.R \c
\&.
The cursor position is relative
to the top of the Text area or
\c
.B
-1\c
.R \c
\& if the cursor shouldn't be moved.
.P
This function is a hook of the AIX Input Method which always
treats a text string as one dimensional because the AIX Input Method
doesn't know about the actual screen. However, in the terminal emulator,
text string sometimes wraps to the next line (On The Spot), namely,
it occupies multiline. This single- to multi- line conversion is
done in this CALLBACK function. So the cursor up or down
should be interpreted by the CALLBACK function, and the CALLBACK function
informs the corresponding cursor position relative to the text string
to the AIX Input Method.
.P
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMAuxCreate"
.DS L
int        IMAuxCreate(im, auxid, udata)
IMObject   im;
caddr_t    *auxid;            /* RETURN */
caddr_t    udata;
.DE
.P
This function is invoked by \c
.B
IMFEP\c
.R \c
\& if
the Input Method wants to create it.
The Auxiliary area may contain several different forms
of data and is not restricted by the interface.
Callback functions must be ready to process
all possible forms of character sets.
.P
Most Input Methods will only have a single
Auxiliary area displayed at a time
but Callbacks must be capable of handling
multiple Auxiliary areas.
.P
\c
.B
udata\c
.R \c
\& is an application data specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.P
On the successful return, an id of the created Auxiliary area is
set to \c
.B
auxid\c
.R \c
\& and \c
.B
IMNoError\c
.R \c
\& is returned.
Otherwise, \c
.B
IMError\c
.R \c
\& is returned.
.H 3 "IMAuxDraw"
.DS L
int        IMAuxDraw(im, auxid, auxinfo, udata)
IMObject   im;
caddr_t    auxid;
IMAuxInfo  *auxinfo;
caddr_t    udata;
.DE
.P
This function is invoked by the \c
.B
IMFEP\c
.R \c
\&
when the Auxiliary area should be drawn.
The Auxiliary area should also be shown if it isn't already.
.P
\c
.B
auxinfo\c
.R \c
\& is a pointer to the \c
.B
IMAuxInfo\c
.R \c
\& structure.
.br
\c
.B
udata\c
.R \c
\& is an application data specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.P
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMAuxHide"
.DS L
int      IMAuxHide(im, auxid, udata)
IMObject im;
caddr_t  auxid;
caddr_t  udata;
.DE
.P
This function is called when the Auxiliary area should be hidden.
.P
\c
.B
auxid\c
.R \c
\& is an identifier of the Auxiliary area to be
hidden.
.br
\c
.B
udata\c
.R \c
\& is an application data specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.P
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMAuxDestroy"
.DS L
int      IMAuxDestroy(im, auxid, udata)
IMObject im;
caddr_t  auxid;
caddr_t  udata;
.DE
.P
Notifies the Callback to destroy any knowledge of the
Auxiliary area as defined by auxid.
.P
\c
.B
udata\c
.R \c
\& is an application structure specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.P
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMIndicatorDraw"
.DS L
int      IMIndicatorDraw(im, indicatorinfo, udata)
IMObject         im;
IMIndicatorInfo  *indicatorinfo;
caddr_t          udata;
.DE
.P
This function is called by the \c
.B
IMFEP\c
.R \c
\& when the
value of the indicator is changed.
.P
\c
.B
indicatorinfo\c
.R \c
\& is a pointer to the \c
.B
IMIndicatorInfo\c
.R \c
\&
structure that holds the current value of the Indicator.
However, the interpretation of this value varies among (phonic)
languages.
The AIX Input Method provides a function to interpret this value.
.P
\c
.B
udata\c
.R \c
\& is an application data specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.P
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMIndicatorHide"
.DS L
int      IMIndicatorHide(im, udata)
IMObject im;
caddr_t  udata;
.DE
.P
This function is called by the \c
.B
IMFEP\c
.R \c
\& when the
\c
.B
IMIndicator\c
.R \c
\& should be hidden
to go into the \c
.B
Suppressed Mode\c
.R \c
\&.
.P
\c
.B
udata\c
.R \c
\& is an application data specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.P
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.H 3 "IMBeep"
.DS L
int      IMBeep(im, percent, udata)
IMObject im;
int      percent;
caddr_t  udata;
.DE
.P
This function tells the application program to emit a beep sound.
.P
\c
.B
percent\c
.R \c
\& specifies the beep level.
The value range is \c
.B
-100\c
.R \c
\& to \c
.B
100\c
.R \c
\&
inclusively and the value \c
.B
-100\c
.R \c
\& means no beep.
Refer to XBell() of
IBM AIX X-Windows Programmer's Reference Manual.
.br
\c
.B
udata\c
.R \c
\& is an application data specified in the parameter
of the \c
.B
IMCreate()\c
.R \c
\& function.
.P
If the error happens, it returns \c
.B
IMError\c
.R \c
\&.
Otherwise, \c
.B
IMNoError\c
.R \c
\& is returned.
.bp
.H 2 "Input Method Convenient Functions"
.P
This section describes the convenient functions which will be
used by the \c
.B
IMFEP\c
.R \c
\& or \c
.B
IMED\c
.R \c
\& implementer.
.P
AIX Input Method provides several functions to provide mapping of
raw key events to strings.
The actual mapping is maintained in a database file
located in the particular language directory.
The database may be modified via the keycomp command.
Refer to X Window User's manual.
Refer to AIXkeymap.h for details on keymap database.
.H 3 "IMInitializeKeymap"
.DS L
IMMap       IMInitializeKeymap(language)
IMLanguage  language;
.DE
.P
This function initializes the keymap associated to the
specified language.
.br
This function returns an identifier of type \c
.B
IMMap\c
.R \c
\&.
Returning \c
.B
NULL\c
.R \c
\& means the occurrence of an error.
\c
.B
IMMap\c
.R \c
\& is typedef'd in the \c
.B
im.h\c
.R \c
\& as
\c
.B
caddr_t\c
.R \c
\&.
This identifier is used for the keymap manipulation functions
described later in this section.
.P
.H 3 "IMFreeKeymap"
.DS L
void     IMFreeKeymap(immap)
IMMap    immap;
.DE
This function frees resources allocated by
\c
.B
IMInitializeKeymap()\c
.R \c
\& function.
.P
\c
.B
immap\c
.R \c
\& is the identifier of keymap.
.H 3 "IMAIXMapping"
.DS L
caddr_t     IMAIXMapping(immap, keysym, state, nbytes)
IMMap       immap;
KeySym      keysym;
uint        state;
int         *nbytes;              /* RETURN */
.DE
.P
This function translates a pair of \c
.B
keysym\c
.R \c
\& and
\c
.B
state\c
.R \c
\& to string and returns a pointer to this
string.
.P
This function dose handle the diacritic character sequence and
ALT NumPad sequence.
.H 3 "IMSimpleMapping"
.DS L
caddr_t     IMSimpleMapping(immap, keysym, state, nbytes)
IMMap       immap;
KeySym      keysym;
uint        state;
int         *nbytes;              /* RETURN */
.DE
.P
IMSimpleMapping performs the translation of a keysym/modifier to a string.
If the keysym/modifier is found, IMSimpleMapping returns a pointer to a
string.  If not found, IMSimpleMapping returns with nbytes equal to zero.
.P
.VL 15 0
.LI "immap"
is an identifier of keymap. Returning string is not null terminated,
length of the string is set to nbytes.
.LI "keysym"
is a keyboard symbol as defined under MIT's X Window System Version II
Release 3.
.LI "state"
is an integer value used to index into the keymap.  This is usually the
modifier mask defined by X.
.LI "nbytes"
is a return value identifying the # of bytes returned from IMSimpleMapping
.LE
.H 3 "IMRebindCode"
.DS L
IMRebindCode(immap, keysym, state, str, nbytes)
IMMap       immap;
KeySym      keysym;
uint        state;
caddr_t     str;
int         nbytes;
.DE
.P
This function can be used to rebind the string to the
specified keysym and state pair.
.H 3 "immalloc"
.DS L
caddr_t   immalloc(size)
uint      size;
.DE
.P
This function returns a pointer to a block of memory of at least
\c
.B
size\c
.R \c
\& bytes.
The block is aligned so that it can be used for
any type of data.
.P
If an error happens during this function, this function calls
\c
.B
abort()\c
.R \c
\& system subroutine.
.H 3 "imrealloc"
.DS L
caddr_t   imrealloc(ptr, size)
caddr_t   ptr;
uint      size;
.DE
.P
This function changes the size of the block of memory
pointed by the \c
.B
ptr\c
.R \c
\& parameter to the
number of bytes specified by the \c
.B
size\c
.R \c
\& parameter,
and then it returns a pointer to the block.
The contents of the block remain unchanged up to
the lesser of the old and new sizes.
.P
If an error happens during this function, this function calls
\c
.B
abort()\c
.R \c
\& system subroutine.
.H 3 "imcalloc"
.DS L
caddr_t   imcalloc(nelem, elsize)
uint      nelem, elsize;
.DE
.P
This function allocates space for an array with the
number of elements specified by the \c
.B
nelem\c
.R \c
\& parameter.
Each element is of the size specified by the \c
.B
elsize\c
.R \c
\&
parameter.
The space is initialized to 0's.
.P
If an error happens during this function, this function calls
\c
.B
abort()\c
.R \c
\& system subroutine.
.H 3 "imfree"
.DS L
void     imfree(ptr)
caddr_t  ptr;
.DE
.P
This function frees the block of memory pointed
to by the \c
.B
ptr\c
.R \c
\& parameter.
The block pointed to by the \c
.B
ptr\c
.R \c
\& parameter must have
been previously allocated by the \c
.B
imalloc()\c
.R \c
\& or the
\c
.B
imcalloc()\c
.R \c
\& function.
.bp
.bp
.H 2 "Input Method Environment"
.br
.H 3 "Directory the IM module is stored"
.br
.P
AIX Input Method modules are usually stored in the fixed directory,
\u1\d
.FS 1
The AIX Input Method will allow the static binding for
operating systems that don't have dynamic binding
capability.
.FE
.DS L
 
/usr/lpp/IM
 
.DE
but the user can also specify the directory using the
environment variable,
.DS L
 
IMPATH
 
.DE
Each language module has the same name,
.DS L
 
inputmethod
 
.DE
The directory /usr/lpp or the one specified by \c
.B
IMPATH\c
.R \c
\&
environment variable may
include the subdirectories which are for
each language Input Method module.
Input Method uses name of these subdirectories as
language identifiers.
.P
For example, Japanese Input Method module (SJIS) will
be included in the directory,
.DS L
 
/usr/lpp/IM/Jp_JP.pc932
 
       or
 
$IMDIR/Jp_JP.pc932
 
.DE
.P
The \c
.B
IMCORE\c
.R \c
\& function \c
.B
IMInitialize()\c
.R \c
\&
and \c
.B
IMQueryLanguage()\c
.R \c
\&
look for the Input Method module for the specified language
in those directory.
If the \c
.B
IMPATH\c
.R \c
\& environment variable is defined,
the \c
.B
IMCORE\c
.R \c
\& searches this directory for the module.
If this directory doesn't contain a proper module,
\c
.B
IMCORE\c
.R \c
\& returns error
instead of searching the directory /usr/lpp/IM.
If the \c
.B
IMPATH\c
.R \c
\& is not defined, \c
.B
IMCORE\c
.R \c
\&
looks for the /usr/lpp/IM directory.
.P
The following chart illustrates this scheme.
.DS L
 
       :
   IMInitialize("Jp_JP.pc932")
       :              |
                      |
                      |
                      V
      $IMDIR/Jp_JP.pc932/inputmethod
 
                     or
 
      /usr/lpp/IM/Jp_JP.pc932/inputmethod
 
.DE
.P
Alias may be provided by using links to the given
module name as the different directory.
For example, the Japanese Input Method may provide
several Input Method yet provide only one module.
In such a case, all aliases would be linked to the same module
yet these are in the different directories.
For example the following directories may exist and
include the same module "inputmethod", yet the later one
provides the only Double Byte Code handling.
.DS L
 
/usr/lpp/IM/Jp_JP.pc932
/usr/lpp/IM/Jp_JP.pc301
 
.DE
.P
.H 3 "Keyboard Mapping Table for the IM."
.P
Almost all Input Method will use a Keyboard Mapping Table ( keymap ).
.br
The keymap files also have the same name,
.DS L
 
imkeymap
 
.DE
and the default keymaps are stored in the same directory as the
Input Method module.
The name of directory that contains the keymap identifies
the language of that keyamp.
.P
The search order of the keymap is as follows:
.AL
.LI
If the \c
.B
IMKEYMAPPATH\c
.R \c
\& environment variable is defined
and the specified directory contains a directory which specifies
the language and that directory contains a proper keymap,
\c
.B
IMCORE\c
.R \c
\& uses it.
.LI
If the \c
.B
IMKEYMAPPATH\c
.R \c
\& environment variable is not
proper or it's not defined, \c
.B
IMCORE\c
.R \c
\& searches the user's
\c
.B
$HOME\c
.R \c
\& directory.
.LI
If the \c
.B
IMPATH\c
.R \c
\& environment variable is defined
properly, this directory is looked for next.
.LI
At last, the directory /usr/lpp/IM is searched for.
.LE 1
.P
If the appropriate keymap is not found, \c
.B
IMCORE\c
.R \c
\& will
fail to initialize.
.bp
.H 2 "Error Handling"
.P
The error number is set by the AIX Input Method into the external
variable \c
.B
imerrno\c
.R \c
\& if the error happens
during the call of the AIX Input Method functions.
.P
The application can refer this variable by declaring as follows:
.DS L
extern  int     imerrno;
.DE
.P
This external variable is defined by the IMCORE.
.P
The following shows the list of the error number and their meaning:
.VL 22 0 1
.LI "IMNoSuchLanguage"
The specified language is not correct.
.LI "IMCouldNotLoad"
The module could not be loaded.
.LI "IMInitializeError"
Error is detected during the call to Initialize function for
the specified Input Method module.
.LI "IMKeymapInitializeError"
.br
Keymap couldn't be initialized.
.LI "IMInvalidParameter"
The specified parameter for the function is invalid.
.LI "IMCallbackError"
The CALLBACK function returns an error.
.LE
( filled in later )
.bp
.H 2 "Attribute Handling"
.P
String for the Text Area and
string array for the Auxiliary area
have corresponding attribute strings or string arrays.
.P
Attribute string is a string of type char same to the string for
the Text area and the Auxiliary area.
It also has the same length.
.P
The following chart shows the correspondency between string and
attribute string.
.DS L
 
             byte
             #---$
             #---+---+---+---+---+---+---+---+---+---+---$
  string     |   |   |   |   |   |   |   |   |   |   |   |
             %---*---*---*---*---*---*---*---*---*---*---&
               |   |   |   |   |   |   |   |   |   |   |
               V   V   V   V   V   V   V   V   V   V   V
             #---+---+---+---+---+---+---+---+---+---+---$
  attribute  |   |   |   |   |   |   |   |   |   |   |   |
             %---*---*---*---*---*---*---*---*---*---*---&
 
.DE
.P
As shown in the above chart, each byte of the string has a
corresponding attribute byte which specifies the attribute of the
corresponding byte of the string.
.P
A Multibyte Byte Character occupies n bytes in the string. The
attribute of the character will occupy the same number of bytes.
However, both attributes of that character
are the same and this is guaranteed by the AIX Input Method.
.P
There are three attribute values defined as follows:
.DS L
#define IMAttNone          0
#define IMAttHighlight     (1 << 0)
#define IMAttAttention     (1 << 1)
.DE
.P
\c
.B
IMAttNone\c
.R \c
\& means no attribute,
and \c
.B
IMAttHighlight\c
.R \c
\& is realized by
the \c
.B
Underline\c
.R \c
\& and \c
.B
IMAttAttention\c
.R \c
\&
is realized by the \c
.B
Reverse\c
.R \c
\&.
These values can be bitwise OR'ed.
.bp
.H 2 "Programming Consideration"
.P
This chapter shows coding examples on
how to use the AIX Input Method.
.P
AIX Input Method is similar to the XLookupString() and the
XLookupString() in that the application programs can be replaced by the
IMProcess(). So the invoking way of this function is
also similar to that of the XLookupString().
.br
The following is a sample code fragment using the &imstr..
.DS L
        XNextEvent(&ev);
        switch (ev.type) {
        case XKeyPressed :
                keysym = XLookupKeysym(&ev, 0);
                IMProcess(im, keysym, ev.state, &str, &len);
                if (len > 0) {
                        takeaction(str, len);
                }
                break;
        case ..... :
              :
              :
              :
        }
.DE
.nr H1 0 1
.nr H2 0 1
.nr H3 0 1
.nr H4 0 1
.nr H5 0 1
.nr H6 0 1
.nr H7 0 1
.HM A
.de HX
.if \\$1=1 .ds }0 Appendix \\*(}0
..
.bp
.H 1 "AIX Input Method Header File"
.DS L
/******************** START OF MODULE SPECIFICATIONS ********************
 *
 * MODULE NAME:         im.h
 *
 * DESCRIPTIVE NAME:    AIX Input Method header file
 *
 * MODULE TYPE:         C
 *
 * COMPILER:            AIX C
 *
 * CHANGE ACTIVITY:     0%G% - Modified
 *
 * STATUS:              Input Method Version %Q%
 *
 * CATEGORY:            Header
 *
 ******************** END OF SPECIFICATIONS *****************************/
 
#ifndef _h_IM
#define _h_IM
 
/*-----------------------------------------------------------------------*
*       Include
*-----------------------------------------------------------------------*/
#include <sys/types.h>
#include <X11/X.h>                      /* for Keysym definition */
#include "imerrno.h"
 
#ifndef TRUE
#define TRUE    1
#endif  TRUE
#ifndef FALSE
#define FALSE   0
#endif  FALSE
#ifndef NULL
#define NULL    0
#endif  NULL
 
/*-----------------------------------------------------------------------*
*       IMDIRECTORY
*-----------------------------------------------------------------------*/
#define IMDIRECTORY     "/usr/lpp/IM"
#define IMMODULENAME    "inputmethod"
 
/*-----------------------------------------------------------------------*
*       IMLanguage
*-----------------------------------------------------------------------*/
typedef caddr_t IMLanguage;
 
/*-----------------------------------------------------------------------*
*       IMSTR and IMSTRATT
*-----------------------------------------------------------------------*/
typedef struct  {
        uint    len;                    /* length in byte */
        caddr_t str;                    /* string, not null terminated */
}       IMSTR;
 
typedef struct  {
        uint    len;                    /* length in byte */
        caddr_t str;                    /* string, not null terminated */
        caddr_t att;                    /* attributes of string */
}       IMSTRATT;
 
/* attribute value is one of... */
#define IMAttNone       0
#define IMAttHighlight  (1 << 0)
#define IMAttAttention  (1 << 1)
 
/*-----------------------------------------------------------------------*
*       IMFunc
*-----------------------------------------------------------------------*/
typedef int     (*IMFunc)();
 
/*-----------------------------------------------------------------------*
*       IMCallback
*-----------------------------------------------------------------------*/
typedef struct  {
        uint    textmaxwidth;           /* max width of Text area */
        IMFunc  textdraw;               /* IMTextDraw() */
        IMFunc  texthide;               /* IMTextHide() */
        IMFunc  textstart;              /* IMTextStart() */
        IMFunc  textcursor;             /* IMTextCursor() */
        IMFunc  auxcreate;              /* IMAuxCreate() */
        IMFunc  auxdraw;                /* IMAuxDraw() */
        IMFunc  auxhide;                /* IMAuxHide() */
        IMFunc  auxdestroy;             /* IMAuxDestroy() */
        IMFunc  indicatordraw;          /* IMIndicatorDraw() */
        IMFunc  indicatorhide;          /* IMIndicatorHide() */
        IMFunc  beep;                   /* IMBeep() */
}       IMCallback;
 
/*-----------------------------------------------------------------------*
*       IMFEP
*-----------------------------------------------------------------------*/
typedef struct  _IMFepCommon    {
        uint                    version;                /* version # */
        IMLanguage              language;               /* language str */
        int                     imerrno;                /* error # */
        struct  _IMFepCommon    *(*iminitialize)();     /* IMInitialize() */
        void                    (*imclose)();           /* IMClose() */
        struct  _IMObjectCommon *(*imcreate)();         /* IMCreate() */
        void                    (*imdestroy)();         /* IMDestroy() */
        int                     (*improcess)();         /* IMProcess() */
        int                     (*improcessaux)();      /* IMProcessAux() */
        int                     (*imioctl)();           /* IMIoctl() */
}       IMFepRec, IMFepCommon, *IMFep;
 
/* current version is 1.1 */
#define IMVersionNumber ((1 << 16) | 1)
 
/*-----------------------------------------------------------------------*
*       IMOBJ
*-----------------------------------------------------------------------*/
typedef struct  _IMObjectCommon  {
        IMFep           imfep;          /* pointer to IMFepRec */
        IMCallback      *cb;            /* pointer to CALLBACKs */
        uint            mode;           /* Processing Mode */
        caddr_t         udata;          /* caller's data */
}       IMObjectRec, IMObjectCommon, *IMObject;
 
/* mode member is one of */
#define IMSuppressedMode        0
#define IMNormalMode            (IMSuppressedMode + 1)
 
/*-----------------------------------------------------------------------*
*       IMTextInfo
*-----------------------------------------------------------------------*/
typedef struct  {
        uint            maxwidth;       /* maximum width on display */
        uint            cur;            /* cursor position on display */
        uint            chgtop;         /* top of change in byte offset */
        uint            chglen;         /* length of change in byte */
        IMSTRATT        text;           /* text string & attribute */
}       IMTextInfo;
 
/*-----------------------------------------------------------------------*
*       IMAuxInfo
*-----------------------------------------------------------------------*/
typedef IMSTR   IMTitle;
 
typedef struct  {
        uint            maxwidth;       /* maximum width of text */
        uint            nline;          /* nubmer of msg text line */
        uint            cursor;         /* TRUE if it has cursor */
        uint            cur_row;        /* cursor position in row */
        uint            cur_col;        /* cursor position in column */
        IMSTRATT        *text;          /* ptr to array of IMSTRATT struct */
}       IMMessage;
 
typedef uint    IMButton;
 
typedef struct  {
        uint    selectable;             /* TRUE if selectable */
        IMSTR   text;                   /* text of item */
}       IMItem;
 
typedef struct  {
        uint    maxwidth;               /* maximum width of item */
        uint    item_row;               /* # of items in row */
        uint    item_col;               /* # of items in column */
        IMItem  *item;                  /* array of items */
}       IMPanel;
 
typedef struct  {
        uint            panel_row;      /* # of panels in row */
        uint            panel_col;      /* # of panels in column */
        IMPanel         *panel;         /* ptr to panel array */
}       IMSelection;
 
typedef struct  {
        IMTitle         title;          /* title string */
        IMMessage       message;        /* message text */
        IMButton        button;         /* type of button */
        IMSelection     selection;      /* selections */
        uint            hint;           /* hint of location */
        uint            status;         /* shown or hidden or ...ing */
}       IMAuxInfo;
 
/* buttons member is one of */
#define IM_NONE                 0
#define IM_OK                   (1 << 0)
#define IM_YES                  (1 << 1)
#define IM_NO                   (1 << 2)
#define IM_ENTER                (1 << 3)
#define IM_ABORT                (1 << 4)
#define IM_RETRY                (1 << 5)
#define IM_IGNORE               (1 << 6)
#define IM_CANCEL               (1 << 7)
#define IM_HELP                 (1 << 8)
#define IM_SELECTED             (1 << 9)
#define IM_ERROR                (1 << 10)
#define IM_OKCANCEL             (IM_OK | IM_CANCEL)
#define IM_ENTERCANCEL          (IM_ENTER | IM_CANCEL)
#define IM_RETRYCANCEL          (IM_RETRY | IM_CANCEL)
#define IM_ABORTRETRYIGNORE     (IM_ABORT | IM_RETRY | IM_IGNORE)
#define IM_YESNO                (IM_YES | IM_NO)
#define IM_YESNOCANCEL          (IM_YES | IM_NO | IM_CANCEL)
 
/* hint member is one of */
#define IM_AtTheEvent   0
#define IM_UpperLeft    (IM_AtTheEvent + 1)
#define IM_UpperRight   (IM_UpperLeft + 1)
#define IM_LowerLeft    (IM_UpperRight + 1)
#define IM_LowerRight   (IM_LowerLeft + 1)
 
/* status member is one of */
#define IMAuxShowing    0
#define IMAuxUpdated    (IMAuxShowing + 1)
#define IMAuxShown      (IMAuxUpdated + 1)
#define IMAuxHiding     (IMAuxShown + 1)
#define IMAuxHidden     (IMAuxHiding + 1)
 
/*-----------------------------------------------------------------------*
*       IMIndicatorInfo
*-----------------------------------------------------------------------*/
typedef struct  {
        uint    size    :  4;           /* size of character */
        uint    insert  :  4;           /* insert/replace */
        uint    unique  : 24;
}       IMIndicatorInfo;
 
/* size member is one of */
#define IMHalfWidth             0
#define IMFullWidth             (IMHalfWidth + 1)
 
/* insert member is one of */
#define IMInsertMode            TRUE
#define IMReplaceMode           FALSE
 
/*-----------------------------------------------------------------------*
*       IM Query State
*-----------------------------------------------------------------------*/
typedef struct  {
        uint    mode;           /* IMNormalMode/IMSuppressedMode */
        uint    text;           /* TRUE/FALSE */
        uint    aux;            /* TRUE/FALSE */
        uint    indicator;      /* TRUE/FALSE */
        int     beep;           /* persent */
}       IMQueryState;
 
/*-----------------------------------------------------------------------*
*       IMQueryText
*-----------------------------------------------------------------------*/
typedef struct  {
        IMTextInfo      *textinfo;      /* RETURN */
}       IMQueryText;
 
/*-----------------------------------------------------------------------*
*       IMQueryAuxiliary
*-----------------------------------------------------------------------*/
typedef struct  {
        caddr_t         auxid;          /* id of the Auxiliary area */
        IMAuxInfo       *auxinfo;       /* RETURN */
}       IMQueryAuxiliary;
 
/*-----------------------------------------------------------------------*
*       IMQueryIndicator
*-----------------------------------------------------------------------*/
typedef struct  {
        IMIndicatorInfo         *indicatorinfo;         /* RETURN */
}       IMQueryIndicator;
 
/*-----------------------------------------------------------------------*
*       IM Query Indicator String
*-----------------------------------------------------------------------*/
typedef struct  {
        uint            format;                 /* format of indicator */
        IMSTR           indicator;              /* RETURN */
}       IMQueryIndicatorString;
 
/* format member is one of */
#define IMShortForm     0
#define IMLongForm      (IMShortForm + 1)
 
/*-----------------------------------------------------------------------*
*       Return Value of IMProcess() and IMIoctl()
*-----------------------------------------------------------------------*/
#define IMNoError       0
#define IMError         -1
 
/*-----------------------------------------------------------------------*
*       Return Value of IMProcess()
*-----------------------------------------------------------------------*/
#define IMTextAndAuxiliaryOff   0
#define IMTextOn                (IMTextAndAuxiliaryOff + 1)
#define IMAuxiliaryOn           (IMTextOn + 1)
#define IMTextAndAuxiliaryOn    (IMAuxiliaryOn + 1)
 
/*-----------------------------------------------------------------------*
*       Command of IMIoctl()
*-----------------------------------------------------------------------*/
#define IM_Refresh              0
#define IM_GetString            (IM_Refresh + 1)
#define IM_Clear                (IM_GetString + 1)
#define IM_Reset                (IM_Clear + 1)
#define IM_ChangeLength         (IM_Reset + 1)
#define IM_QueryState           (IM_ChangeLength + 1)
#define IM_QueryText            (IM_QueryState + 1)
#define IM_QueryAuxiliary       (IM_QueryText + 1)
#define IM_QueryIndicator       (IM_QueryAuxiliary + 1)
#define IM_QueryIndicatorString (IM_QueryIndicator + 1)
#define IM_ChangeMode           (IM_QueryIndicatorString + 1)
 
/* To add Language Dependent Commands */
 
/* SBCS Input Method */
#define SIMIOC  ('S' << 16)
 
/* Japanese Input Method */
#define JIMIOC  ('J' << 16)
 
/* Chinese (Hanji) Input Method */
#define HIMIOC  ('H' << 16)
 
/* Korean (Hangeul) Input Method */
#define KIMIOC  ('K' << 16)
 
/*-----------------------------------------------------------------------*
*       Constant Definitions for IMTextCursor() Callback
*-----------------------------------------------------------------------*/
#define IM_CursorUp     0
#define IM_CursorDown   1
 
/*-----------------------------------------------------------------------*
*       IM Keymap Handling Functions
*-----------------------------------------------------------------------*/
 
typedef caddr_t IMMap;          /* Internal State Structure */
IMMap   IMInitializeKeymap();   /* Do IM search for Keymap file */
void    IMFreeKeymap();         /* free resources allocated for IM Keymap */
char    *IMSimpleMapping();     /* map Kbd Element - see AIXKeymap.h*/
 
#endif  _h_IM
.DE
.nr H1 0 1
.nr H2 0 1
.nr H3 0 1
.nr H4 0 1
.nr H5 0 1
.nr H6 0 1
.nr H7 0 1
.HM 1
.rm HX
.de HX
.SK
.ds }0 "
.ds }2 "
.nr ;0 2
.br
..
.TC
