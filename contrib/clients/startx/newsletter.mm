.\" 
.\" ltroff -tbl -pic startx.newletter
.\" 
.\" 
.\" eqn tbl
.\" 
.nr O 1i
.nr W 6.5i
.so /usr/lib/tmac/tmac.m
.fp 4 HX
.de TP
.vs 12
.sp 2
.sp .5
.ps 16
.if e 'tl '\f(HXannouncements''\fR'
.if o 'tl '''\f(HXannouncements''\fR'
.sp -2
.ps 160
.tl '\fB___'___'___\fR'
.sp 6
..
.BS
.ps 80
\l'6.5i'
.ps 10
.sp 1
.BE
.EF "'\s+6\f(HX%\s-4''PUCC Newsletter\fR\s-2'"
.OF "'\s+2\f(HXNovember 1989''\s+4%\fR\s-6'"
.EQ
delim off
gfont italic, gsize 10
.EN
.S 10 12
.nr Pi 5
.nr Pt 1
.SA 1
.de HX
.if \\n(;1=1 \{\
.sp 2
.ps 80
\l'2.75'
.ps 10
.ft HX
.sp .15\}
.if \\n(;1=1 .vs 18
..
.de HZ
.vs 12
.sp .5
..
.ds HP 16 16 16 16 16 16 16
.ds HF 4 1 1 1 1 1 1
.HM 1 1 1 1 1 1 1
.nr Hb 5
.nr Hs 5
.nr Hi 1
.nr Hc 0
.nr Hu 1
.nr Cl 7
.nr Ci 10 5 5 5 5 5 5
.de ES
.ft CW
'in +3
.nf
.S 9 10
'sp .75
..
.de EE
.if "\\$1""  .sp .75  \" spacing before regular following text
.if "\\$1"1" .sp .75  \" spacing before regular following text
.if "\\$1"2" .sp .25  \" spacing before paragraph .P
.if "\\$1"3" .sp 0    \" 0 extra spacing before subheading
.\" takes one argument (null, 1, 2, or 3) to determine spacing 
.\" after example end
.S 10 12
'in
.ft R
.fi
..
.de )k
..
.2C
.bp
.1C
.PS
box wid 6.5i ht 2.5i
.PF
.sp 1.5
.ll -2
.in +2
.ce
\s+2\f(HXThe Syntax of the Startx UNIX Utility and Its Options\fR\s-2
.sp
The syntax of the UNIX \fIstartx\fR command and its options is
.ES
startx [\|-d | -w\|] [\|-x \fIstartup\fP | [\|-r \fIresources\fP | -m \fIwindow_manager\fP\|] ]
.EE
which represents two sets of options.  The first set consists of
the options \fB-d\fR
or \fB-w\fR which helps \fIstartx\fR
distinguish which type of terminal you are using.
These options will only be used in rare circumstances and you
should consult the manual page for more details.
.sp
The second
set of options consists of a choice between setting up your
X Windows System environment with one shell script file
using the \fB-x\fR option,
or by specifying your X resources and window manager separately
using the \fB-r\fR and \fB-m\fR options.
You should note that you can use one or both
of the \fB-m\fR and \fB-r\fR options,
or the \fB-x\fR option,
but you cannot use the \fB-x\fR option
in conjunction with the \fB-m\fR and \fB-r\fR options.
.ll +2
.in -2
.PE
.sp 2
.2C
.HU "Startx UNIX Utility Makes the X Windows System Easy"
\fRThe \fIX Windows System\fR is
a network-based, graphics window system which
was developed at MIT in 1984.
The most recent version, X11, was 
released in 1987,
and is the version we currently have running.
The X Windows System is available on all of our Sequent Symmetries
running the DYNIX operating system, Sun workstations running SunOS,
and the VAX 8800 running the ULTRIX operating system.
.P
There are many ways to initiate an X Windows System session
which require you to go through several steps to create a working
environment.
The \fIStartx\fR UNIX utility
is designed to ease your burden
by allowing you to start your window manager,
define and load your X resources, and
initiate other X clients in one command.  
.sp
\fBWindow Managers, X Resources,
.br
X Servers and X Clients\fR
.sp
In order for you to understand how \fIstartx\fR works it is important
that you know the meanings of the terms \fIwindow manager, X resources, 
\fIX server\fR and \fIX clients\fR.
.BL 2
.LI
A \fIwindow manager\fR is a program which allows you to move, resize and
circulate windows on your display.
These windows act like individual
screens which you can manipulate, layer and move around.
.LI
\fIX resources\fR are personalized 
attributes of your environment such as color, cursor symbol, etc.  
You can tailor your X Windows Session environment to make the cursor 
symbol change when moved to other windows, make a scroll bar,
or make the windows change
colors for easier identification.  These are just a few examples of
\fIX resources\fR.
.LI
Your \fIX server\fR communicates and translates all input so that your
particular display can understand all the information it receives.
It is a combination of the graphics display, keyboard,
mouse, hardware, and X server software which accepts input from these
devices and provides display services for clients.
The \fIX server\fR software must run locally in order to communicate
with these devices.  
.LI
\fIX clients\fR are programs that perform tasks 
such as terminal emulation or window management
to shape your X Windows System environment.
.LE
.sp
\fIStartx\fR sets your
DISPLAY environment variable
to identify the X server to the X clients.
This enables your X clients to communicate with it
and set up your X Windows System session.
.sp
You can find out more about your environment variables
by checking the man pages of the UNIX shell you are using
(\fIsh\fR(1), \fIcsh\fR(1), or \fIksh\fR(1L))
or by asking a PUCC general consultant for assistance.
.sp
\fBCreating a Startup Shell Script File\fR
.sp 
You can define and load your X resources 
and your window manager easily using a
startup shell script file.
\fIStartx\fR reads this file if you use the \fB-x\fR option
on the command line,
if you name the file in your XINITRC environment variable, or
if you named the 
file\fI .Xinit, .xinit, .Xinitrc, .xinitrc\fR, or \fI.xsession\fR.
In this file, 
it is your responsibility to start
a window manager, load X resources and create X clients.
A working example of a startup shell script file
follows:
.ES
.in -.5i
#!/bin/csh
(twm &)
xrdb -load .xdefaults
(xclock -g 75x75+0+0 &)
(xbiff  -g 75x75+101-0 &)
if ("/dev/console" == "`tty`") then
\ \ xterm -g 80x24+0+0 +ut -C -T `hostname`
else
\ \ xterm -g 80x24+0+0 +ut -T `hostname`
endif
.EE 1
.in -.5i
For a workstation, it is important that
the last line in your startup shell script file
be \fIxterm\fR(1L)
running in the foreground with the \fB-C\fR option for console
messages.
.sp
For an X display station, the last line in the startup script
should be \fIxterm\fR(1L) running in the foreground
\fIwithout\fR the \fB-C\fR option.
Be aware that some X display stations will not
terminate the \fItelnet\fR(1C) session on closing,
therefore, you should remember to manually logoff
the session before going to,
or after leaving,
the X Windows session.
.sp
Now that you know what a startup file looks like
you can tell \fIstartx\fR to read this file in one of three ways.
.BL 2
.LI
\fRYou can use the \fB-x\fR option and a startup
shell script file to invoke \fIstartx\fR
by entering
.ES
startx -x \fIstartup
.EE 2
In this example, \fIstartx\fR will run the given shell script file,
\fIstartup\fR.
\fIStartx\fR looks for a file specified with the \fB-x\fR option
before looking for any other initialization files.
.LI
\fRYou can set your XINITRC environment variable to the
startup shell script file name you want to use.
If you don't give any startup shell script file name
at the command line using the \fB-x\fR option,
then \fIstartx\fR looks for a startup shell script
file named by your XINITRC 
environment variable.  
.sp
Again, you can find out more about setting your environment variables
by checking the man pages of the UNIX shell you are using
(\fIsh\fR(1), \fIcsh\fR(1), or \fIksh\fR(1L))
or by asking a PUCC general consultant for assistance.
.LI
You can name your startup shell script 
file \fI.Xinit, .xinit, .Xinitrc, .xinitrc,\fR 
or \fI.xsession,\fR to begin the X client programs.
If you haven't used the \fB-x\fR option or set XINITRC,
then \fIstartx\fR
looks in your home directory for any file with the above
names in their respective order.
It looks for these file names 
specifically because they
are the most common names for initialization files, however,
we recommend using the name \fI.xsession\fR
for your startup shell script file. 
.LE
.sp
\fBDefining X Attributes Without a Startup File\fR
.sp
\fRIf \fIstartx\fR doesn't find a startup 
shell script file, it 
loads a \fIresources\fR file which you supply at the command line
using the \fB-r\fR option. 
This is done by entering
.ES
startx -r \fIresources 
.EE 1
where \fIresources\fR is the name of the X resources configuration
file you wish to use.
(See the \fIxrdb\fR(1L) and \fIx\fR(1L) man pages
for more information on X resources.)
.sp
If you don't use the \fB-r\fR option, \fIstartx\fR looks for 
a file in your home directory 
called \fI.Xdefaults, .xdefaults, .Xresources,\fR 
or \fI.xresources,\fR respectively.
If it doesn't find any of these X resource files,
then your X Windows System session will not have personalized
attributes.
.sp
\fRAgain, if no startup shell script file
is specified using the \fB-x\fR option, \fIstartx\fR 
looks for a window manager
indicated at the command line using the \fB-m\fR option.
This is done by entering
.ES
startx -m \fIwindow_manager
.EE 2
If you don't use the \fB-m\fR option, then
\fIstartx\fR invokes the 
window manager \fItwm\fR(1L), \fIawm\fR(1L), or \fIuwm\fR(1L) if it finds
an associated configuration file 
in your home directory named \fI.twmrc, .awmrc,\fR 
or \fI.uwmrc\fR, respectively.
If \fIstartx\fR can't find such a window manager configuration 
file in your home directory, it initiates 
an \fIxterm\fR(1L) client and the \fItwm\fR(1L) window manager
by default.
.sp
\fBTracing Errors\fR
.sp
Not only does \fIstartx\fR start all your X clients for you, it also 
redirects error messages into a file
for any errors encountered 
while trying to start your X Windows System session.
If you set your environment variable XERRORS
to the name of an errors file you want, \fIstartx\fR will redirect the
error messages to that file.
.sp
If you don't set your XERRORS environment variable,
\fIstartx\fR
creates an errors file called \fI.xerrors\fR in you home directory
and redirects any error messages to that file by default.
.sp
\fBAdditional References\fR
.sp
For more information on the X Windows System see
the \fIX Window System User's Guide, Volume Three,\fR 
by O'Reilly & Associates, Inc.
This manual is available for loan
at the PUCC Reference/Loan Library in MATH B6.
For a general overview of the X Windows System
consult the November 1989 issue of the \fIPUCC Newsletter\fR. 
.sp -.5
.TS
rw(3.06i) .
\(em \fIGeorge Kraft &\fR
\fIMichael Melton\fR
.TE
