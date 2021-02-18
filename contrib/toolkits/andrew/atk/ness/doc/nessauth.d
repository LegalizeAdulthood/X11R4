\begindata{text,2065904}
\textdsversion{12}
\template{default}
\define{concat

attr:[Script PreviousScriptMovement Point 6]
attr:[FontSize PreviousFontSize Point 2]}
\define{footnote

attr:[Flags OverBar Int Set]
attr:[FontSize PreviousFontSize Point -2]}
\flushright{7 December 1989}



\italic{\{\{Future: This version of the manual includes planned but 
non-existent features.  These are delimited by double curly brackets so they 
can be removed to form the manual itself.\}\}}





\majorheading{Ness Author's Reference Manual}


\center{by Wilfred J. Hansen}

\indent{\flushright{\smaller{_________________________________


Information Technology Center,

Carnegie Mellon University


Copyright IBM Corporation 1988, 1989

__________________________________


}}
Ness is a programming language for the Andrew ToolKit.  With it, documents may 
contain active elements controlled by Ness scripts.  The language features an 
innovative substring algebra for string processing.  


This manual is intended for authors who want to write Ness programs or 
incorporate Ness scripts in documents or applications.  For a description of 
all documents describing Ness, see the \bold{Ness User's Manual}.


Contents

   1.  Writing Ness

   2.  Writing stand-alone programs

   3.  Extending objects in embedded scripts

   \{\{Future: 4.  Program development support\}\}



\smaller{CAVEAT: This manual describes an early version of the Ness language 
and system.  You are invited to contribute suggestions for change to the 
author (\italic{wjh+@andrew.cmu.edu}) or the bboard (\italic{andy.ness}).  But 
please realize that your suggestions and those of others may evolve the 
language enough to force you to revise or abandon programs written to this 
document.

}}\bold{
}\formatnote{.bp}


        Copyright IBM Corporation 1988, 1989 - All Rights Reserved        


\smaller{Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, provided that 
the above copyright notice appear in all copies and that both that copyright 
notice and this permission notice appear in supporting documentation, and that 
the name of IBM not be used in advertising or publicity pertaining to 
distribution of the software without specific, written prior permission.


\smaller{IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING 
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL IBM 
BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES 
WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION 
OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.

}
/* $Header: /afs/andrew.cmu.edu/usr13/wjh/h/atk/ness/doc/RCS/nessuser.d,v 1.1 
89/09/13 10:38:05 wjh Exp Locker: wjh $ */

/* $ACIS:$ */

/* $Source: /xsrc/contrib/toolkits/andrew/atk/ness/doc/RCS/nessauth.d,v $ */

/* $Log:	nessauth.d,v $
Revision 1.2  89/12/07  12:15:04  xguest
added author's name
 Revision 1.1  89/12/07  11:50:02  xguest Initial 
revision

Revision 1.1  89/10/11  09:27:04  wjh Initial revision  */

}\formatnote{.bp}



Ness \italic{programs} reside in their own files and process other files. 
 Ness \italic{scripts} reside as insets within a document or application and 
extend and control the behavior of other insets.  This manual describes how to 
write Ness programs or incorporate Ness scripts in documents or applications. 
 For details of writing the language itself, see Ness Language Reference 
Manual.  For a full list of the Ness documentation, see the \bold{Ness User's 
Manual}.


Some of the early parts below describe Ness cultural conventions which are 
unenforcable.  Warning, things you will need to know are embedded in the 
cultural stuff to give you incentive to wade through the culture.



\section{1. Writing Ness}


Several topics are of interest in writing Ness code, whether in standalone 
programs or scripts embedded in documents or applications.



\subsection{1.1 What a Ness script looks like}


Although the Ness compiler pays very little attention to program formatting 
and typography, you should always remember that the purpose of writing a 
program is to explain it to another human because humans have much lower 
tolerance for confusing programs than computers.  In the Ness security scheme, 
users are encouraged to read Ness programs and to ignore any they do not 
understand.  Not all users will avail themselves of this opportunity, but 
enough will that it well behooves you as author to take some care to produce a 
readable document.


Ness programs may contain typography, all of which is ignored except that 
within string constants.  Typical uses of typography:

	highlight function names and the names of global variables

	use indentation styles instead of tabs

	use headings and titles in comments

	put reserved words in italics  (\italic{function, if, return,} ...)

A comment can be one long line with the comment mark (--) only at its 
beginning. 


Where useful, include diagrams, rasters, and tables in comments to explain the 
program.


Comments are important for every global entity, both variable and function. 
 The comments should indicate the purpose of the entity sufficiently well that 
a reader of can understand any given function having read only the comments 
describing the functions and global variables it uses.  That is, without 
reading the contents of any other function.  For a function the initial 
comment should describe the arguments, the return value, and any exceptional 
circumstances that affect the behavior.  For instance if it is a string 
processing function what does it do differently at the ends of the string or 
in case it doesn't find what it is looking for.


Indentation should follow the examples or the indentation shown in the grammar 
in the \bold{Ness Language Reference Manual}.  In particular, if a form 
beginning with a reserved word "\bold{xyz}" has an "\bold{end xyz}", the first 
line of the form should begin with the reserved word, the last line should 
begin with the matching "\bold{end xyz}" indented the same amount, and all 
lines between should be indented further by at least one tab stop.  The 
reserved words \bold{else} and \bold{elif} are indented the same distance as 
their \bold{if}.  (If indentation gets too deep, either restructure the 
algorithm or put the inner portion in a function.)


In general global variables should be longer words and capitalized.  


Put a space after a comma and around the assignment and append operators (:= 
and  \concat{~}\bold{:=} ).  Very often there should be a space around plus 
(+) and minus(-).  After a function name, the left parenthesis should follow 
without an intervening space.


In writing functions that process strings, it is best to write them so they 
process their argument, rather than assuming they will process an entire base 
string.  In other words, searches should be limited by the end of the argument 
rather than by the end of the base.



\subsection{1.2 Errors}


The Ness processor indicates errors as precisely as possible: the exact piece 
of the program with the error is indicated.  For standalone programs the 
location is indicated in terms of character position from the beginning of the 
file and also by printing the line containing the error.  When editing the 
script you can get to character position \italic{N} by giving the command 
sequence ESC-<-control-U-\italic{N}-control-F;  the ESC-< moves the cursor to 
the front of the file and the rest of the sequence moves the cursor forward 
\italic{N} positions.  (The first position in the file is number zero.)  For 
embedded Ness programs executing interactively, the error position is shown by 
moving the selection to the erroneous token or tokens and displaying the 
accompanying message in the message line.  To get to the next error, choose 
the NextError option from the Ness menu card or type the sequence 
control-X-control-N.


Occasionally the item highlighted will be next to the error, rather than the 
error itself.  For a syntax error, the highlight often indicates the token 
after the erroneous one because the error token would have been legal in some 
other program.  A missing \bold{end} token will usually not be detected until 
the next \bold{end};  this is best detected by proper indentation.  


Errors during execution may sometimes fail to find the location of the error. 
 In this case the error indication may be the empty string at the beginning of 
a function or the beginning of the entire script.


If an error occurs in a function compiled from the ness library (see section 6 
of the \bold{Ness Function Reference Manual}), Ness does its best to show that 
error as well.  For standalone execution, the error is indicated in the error 
list.  For embedded execution another window, named NessLibrary, is brought up 
and the error is highlighted there.  The NessLibrary window behaves a lot like 
most ez windows, but does have some differences;  it is best when through with 
it to use ESC-D to discard it.  If the erroneous library function was not your 
own, you should contact its author and mention that the problem occurred.


The most imposing Ness error message is


\leftindent{! ! !   Disasterous Ness error in this function.   Quit soon. 
  !!!!!!!!!!}


This is displayed whenever the interpreter detects a bus error or segmentation 
fault during execution.  In general it is okay to continue with the same 
editor because, after all, the offending memory acces could not have done 
anything because there was no such address.  Consider, however, that the 
offending address is pseudo-randow so on other executions of the same 
statement it might refer to and damage some part of the memory that was 
crucial.  It is for this reason that the message is so strongly stated.



\subsection{1.3 Functions available}


There are five sources for functions to call from Ness programs.


\leftindent{1) Functions defined elsewhere in the same script

2) Ness builtin functions

3) Ness library functions

4) Proctable functions

5) Functions defined in C for object classes}


The first of these is described in the \bold{Ness Language Reference Manual}, 
the last in the \bold{Ness Hacker's Manual}, and the rest in the \bold{Ness 
Function Reference Manual}.  Note that if you want to call functions ofr 
objects classes your Ness program text must earlier contain the comment:


	--$enable class access


spelled and spaced exactly as shown.


There are \italic{three} different places a Ness script may appear in the 
editor:


\leftindent{as an embedded script in a document or application

as a Ness program in a file of its own

in the NessLibrary window}


It is unlikely that one script would appear in all three places, but quite 
conceivable that it could be editted as a file and in a NessLibrary winodw 
simultaneously.  Beware: these are two separate copies of the text; changes to 
one will not affect the other.  



\section{2.  Writing stand-alone programs}


To write a program in Ness, create a file with the extension ".n" and write 
the script in it. Execute the script with nessrun, as shown in the \bold{Ness 
User's Manual}.


Nessrun presents to the program the remainder of the command line after the 
program name, so the program can have any set of switches and arguments it 
wants.  This text is passed to the program as the single argument to the 
function 'main', which is where execution begins.  


The next two pages give a sample program which reads a file, "processes" it 
line-by-line, and writes the revised file.  In this case the processing is 
simply to make a copy of the contents of the line and create an identical line 
in the output file.  As the program notes, the entire processing step could be 
written as  

	return copy(text)

but then it would not illustrate how to find the lines of the file.


\begindata{bp,2017584}
\enddata{bp,2017584}
\view{bpv,2017584,3,0,0}
--  process.n   

--	Read a file, process each line, and write the result to filename.doc

--

--	Usage:   nessrun  process.n  filename


marker destext := "doc"	-- extension for destination file

marker letters := "qwertyuiopasdfghjklzxcvbnm"

\leftindent{~ "QWERTYUIOPASDFGHJKLZXCVBNM"

~ "_"}


-- Process(text)

--	process the text and 

--	return the revised version

--	The text may be read-only, so we use ~:= to build new text.

--	In this example, the new text is just 

--		a concatenation of the lines of the old

--

function Process(text)\leftindent{

marker new := newbase()		-- the output text

marker nl	-- a newline in the original text

marker line	-- one line of the original text 

		--  (without a newline at either end)

nl := search(text, "\\n")    		-- find first newline

line := extent(text, start(nl))    	-- find first line


while nl /= "" do


	-- "process" a line:

	new ~:= line ~ "\\n"    


	line := finish(nl)	-- start of next line

	nl := search(extent(line, text), "\\n")  -- next newline

	line := extent(line, start(nl))	-- the entire next line

end while


-- At this point, 'line' refers to everything 

--    from the finish of the last newline to the finish of 'text'


if line /= "" then

	-- there is more in 'text' after its last newline

	new ~:= line    	-- "process" the tail 

	new ~:= "\\n"    	-- gratuitously add a final newline

end if

return new}

end function

\begindata{bp,2063280}
\enddata{bp,2063280}
\view{bpv,2063280,4,0,0}
function main(args)

\leftindent{marker filename, outname, t


filename := token(args, letters ~ "./0123456789")

outname := search(filename, ".")

t := search(finish(outname), ".")

while t /= "" do	-- find the last "."

\leftindent{outname := t

t := search(finish(outname), ".")}

end while

--  append "." ~ destext if there is no "." in the filename,

--	or there is a slash after the last ".",

--	or filename already ends in "."~destext

if 		outname = "" 

		or search(finish(outname), "/") /= ""

		or extent(next(outname), filename) = destext

	then

\leftindent{outname := filename ~ "." ~ destext}

else 

\leftindent{outname := extent(filename, outname) ~ destext}

end if


printline("Process: " ~ filename ~ " -> " ~ outname)

t := readfile(filename)      -- t is read-only if the file is

t := Process(t)

-- save: the old ouput: 

system("mv " ~ outname ~ " " 

		~ outname ~ ".BAK 2> /dev/null")

writefile(outname, t)}

end function


The main function first extracts the input filename from the arguments.  Note 
that as written it only recognizes names consisting of letters, digits, 
underline, dot, and slash.   Next it uses a while loop to find the last dot, 
checks the extension, and generates the name for the output file.  After then 
printing a message it reads the old file, processes it, moves the old output 
file, and writes the new output file.


The function Process advances through the text one line at a time, setting the 
variable 'line' to have the contents of each successive line, without a 
newline at either end.  The "processed" text is accumulated in 'new' by 
appending to its end.  The loop is carefully written to process only the 
'text' value passed in, even if it is a substring of some longer string.


It is instructive to see why the loop leaves 'line'referring to everything 
after the last newline.  The essential cause is that when search() fails to 
find what it is looking for it returns finish() of its first argument.  In 
this case because the first argument ends at the end of the text, that 
location is returned for failure.  And then 'line' is set to everything from 
finish() of the last newline to finish() of 'text'.



Nessrun does have one switch of its own which, if used, is inserted in the 
command line just after 'nessrun' and before the filename.  The switch is -d 
and its effect is to force a dump of the code generated for the program.  (In 
most circumstances this is of little value to you as a Ness programmer.)  The 
code generated is that of a stack machine: operands are fetched to the stack 
with Load instructions, operators apply to the top elements of the stack, and 
results are put back in variables with store instructions.  For more 
information about the exact contents of the dump, you can try reading the 
source code of module atk/ness/objects/dump.c in the Ness source code.  


When writing a standalone program, you may wish to utilize the textview_ 
proctable functions.  To allow this, nessrun provides a value for 
'defaulttext';  the value is an empty text, so to use it you first copy the 
text with  replace(base(currentselection(defaulttext)), 
the-text-to-be-processed).


The nessview inset provides some tools of use when editing Ness code. 
 Primarily this means you can compile the program without leaving the editor. 
 To utilize the nessview inset to edit Ness programs, you can add the 
following to your ~/.atkinit file:


\leftindent{addfiletype .n ness "template=default"}


If you edit a new file with the extension .n, it will be editted with the 
nessview inset and saved with "\\begindata\{ness" so next time it will be read 
as Ness.  Previously existing xyz.n files that have styles have already been 
written with "\\begindata\{text", so editing them will not use the nessview 
inset.  Note also that the second time you edit a .n file with the ness inset, 
you will find the warning message wrapped around it.  You can click on the 
Author Mode button near the end of the file.  You can remove the warning from 
all your Nesses by adding to your preferences the line


	*.NessUseDialogBoxInsteadOfWarning: on


Thereafter a dialog box will appear whenever you see a Ness script.


The \bold{Ness User's Manual} explains that a Ness script may begin with 


	#!/usr/andrew/bin/nessrun


If the file it is in is made executable with something like


	chmod +x filename.n


then a user can execute the Ness program just by typing its name to the shell. 
 What the User's Manual does not mention, however, is that the #! line must be 
the absolutely first thing in the file.  If the file is originally created 
with styles or embedded objects, ez will expect the file to begin with 
"\\begindata" and will otherwise show it as plain ASCII.  It is easiest to 
simply create such programs without styles or embedded objects.  However, if 
you are editting the Ness code with the ness inset, it will get read properly.


When errors are encountered for stand-alone compilation or execution, a 
message is printed like this:


\leftindent{\smaller{Compile error(s) in /tmp/test.n

   at 2094 (len 8): the Ness library file for the given prefix, does not 
define this function

   source text is:  t := Process( >>> enum_foo <<< (t))


   at 2221 (len 3): syntax error

   source text is:   >>> end <<<  function}}


In each entry the first two numbers are the location of the error within the 
file.  The first number is the position of the first character of the 
erroneous text and the second is its length.  You can get to the indicated 
position as described above.  After the first two numbers and colon comes the 
error  message and then on the second line is the erroneous line from the 
source text with the error text indicated within >> and <<.



\section{3. Extending objects in embedded scripts}


Ness scripts embedded as insets in documents can extend the behavior of insets 
appearing elsewhere in the document.  You can insert a Ness inset just as any 
other inset: type ESC-TAB and respond to the prompt with "ness".  An embedded 
text will appear which differs in appearance from an ordinary text inset in 
that there is an additional "Ness" menu card.


The most direct way to take advantage of a Ness inset in a document is to 
insert within the inset a script which processes the document's text.  After 
Empowering the Ness, each time you select "Do main()" from the Ness menu card, 
the function main() in the script will be called (with no arguments).  For 
example, suppose you insert the following script in a Ness inset within this 
document and then choose Do main() from the menu.  The function counts the 
number of instances of "Ness" and shows the number in the message line.  (As 
of 10:04:12 EDT 10 Oct 1989, the value displayed is 104.)

\smaller{
\example{function main()

	-- count the occurrences of the string "Ness"

	integer count

	marker word

	count := 0

	word := start(base(currentselection(defaulttext)))

	while True do

		word := search(finish(word), "Ness")

		if word = "" then

			TellUser(textimage(count))

			exit function

		end if

		count := count + 1

	end while

end function}

}
The function currentselection() returns a marker for the text currently 
selected in the default text.  Base() then computes a marker for the entire 
text containing that selection.  Finally, start() computes an empty marker at 
the beginning of its argument.  In this way, the search for "Ness" begins at 
the front of the document, no matter what portion of the document is currently 
selected.  Each execution of search() within the loop finds a subsequent 
instance of "Ness" because it begins the search with the next() character 
after the current instance.


Warning: When I first wrote the above routine I omitted the call to finish(). 
 As a result the function went into a loop because it continuously found the 
same instance of "Ness".  As explained in the NessUser's Manual, you can use 
control-G to exit an embedded Ness script that is caught in a loop .


When a run-time error occurs or even when control-G is pressed, the Ness 
script is disabled until recompiled.  And the script will only recompile after 
you have changed its text.  You may have to go into Author mode, add and 
remove a single character, and then request the recompilation.


\{\{Future: We need to allow programs to be reexecuted even if a runtime error 
occurred.\}\}



\subsection{3.1 Embedding and naming}


When designing a document or application the first step is to design the 
appearance and behavior.  Decide what buttons, fields, and other insets you 
desire and what will happen for user actions to each.  For instance, you may 
want a click on a picture of a flower to scroll the document to a section on 
horticulture.  The next two steps are to layout the images and build the Ness 
script.


By convention, when you enhance a document with a Ness script you should 
include in the design of the document a section which describes to the reader 
the ways in which the document is enhanced. That way the reader can decide 
whether or not to empower the script.  


There are some constraints on the placement of the description of the Ness 
enhancements and the Ness script itself.  The description should appear close 
to the Ness so the reader can find it easily while deciding whether to Empower 
the script.   The script itself should appear close to the beginning of the 
document or close to the first inset controlled by the Ness.  Otherwise the 
user may not see the script and may thus not have a chance to empower it and 
enjoy its benefits.


\{\{Future: Invent conventions for showing what pieces of text are 
enlivened.\}\}


When placing insets in the document or application they need to be given names 
so the Ness script can refer to them.  The full description of creating and 
naming insets is covered by the documents in /usr/andrew/doc/adew/;  what 
follows is just one possible approach.


A name may be given to an inset by embedding it in a \italic{cel} inset, which 
is in turn embedded in the parent.  The cel inset remembers a name given to it 
and registers that name with its parent.  The name is passed up the parentage 
until it reaches an arbiter, which then remembers the name in conjunction with 
its cel.  Ness asks the arbiter surrounding it for cels by names.


The user interface to all this is covered in the documents in 
/usr/andre/doc/adew/.  Here's one approach:  use the \italic{arbcon} to create 
insets, paste them in your work, and finally use the arbcon again to give 
names to the insets.


If you have in your .atkinit the line "addkey ness-dostmt \\e\\e view" you can 
bring up an arbcon window by typing ESC-ESC and respond to the "Ness:" prompt 
with "arbcon_create()".  Create an inset with the arbcon by clicking one of 
the names in the list at the top middle.  The act of clicking puts a new 
instance of that sort of object into the cut buffer; from thence you can paste 
it into your document or application.  Whenever you click on an inset its name 
and attributes will be displayed in the lower portion of the arbcon window. 
 Initially the name will be some non-descript identification like  "value", 
"value-1", "value-2", ... .


Ness scripts refer to inset names in two ways.  The inset() function takes as 
its argument a marker value and tries to find an inset with that name, 
returning an object value for the inset.  The extend construct names an inset 
which will be the target for all the event specifiers within the extend.  If 
we give the flower the name "rosebud", the script might say


extend "rosebud"

\leftindent{on mouse "andy"

\leftindent{if mouseaction = mouseleftup then

\leftindent{marker b := base(currentselection(defaulttext))

b := search(b, "\\n3.1 Horticulture")

if b /= "" then\leftindent{

setcurrentselection(defaulttext, b)

textview_line_to_top(defaulttext)}

end if}

end if}

end mouse}

end extend


In this case when the mouse goes up in the flower the text will be searched 
for the header of the Horticulture section and that line will be moved tothe 
top of the window.


Within ATK, the names of various insets are managed by an arbiter object. 
 When a Ness is Empowered, it tells the arbiter it wants to know about named 
insets.  As insets become visible the Ness is informed of them and extends 
them as described in its script.


An arbiter is an inset that can contain any other sort of inset.  Thus you can 
insert an arbiter containin, say, a table.  Thereafter named insets within the 
table only send their names to the locally enclosing arbiter and not to the 
outermost one.  To extend these inner insets, a Ness must be inserted as one 
of the insets within the table.  In this way, there may be multiple copies of 
the table in a document each with its own script and each with subordinate 
insets having thee same names.xxx  Discuss hierarchical name spaces by 
embedding arbiters.



\bold{3.2 User Interface to Ness}


The menu options for Ness are documented in the \bold{Ness User's Manual}.  It 
is useful to point out that if you as author want to see what a script will 
look like to a user you can select the "Add Warning" option.  To resume 
editting you will then have to scroll to the end of the script and click on 
the box labelled "Author mode - Let me edit the script".


The following keystrokes are also defined by a Ness inset:


ctl-X ctl-N   -  Display the next error or the next warning message.  After 
the last the message line shows "No more errors".  Typing the keys again will 
restart with the first error.


ctl-U ctl-X ctl-N  -  Display the previous error or warning message.


ESC ctl-N shift-C  -  Compile the script.


ctl-X ctl-E  -  Compile the script and execute beginning with function main() 
or the currently selected startup function (see the next entry).


ctl-U ctl-X ctl-E  -  Prompts for a function name and sets that name to be the 
currently selected startup function.  Then goes ahead and does everything like 
ctl-X ctl-E  (just above).


ESC ctl-D shift-D  -  Turn on Ness debugging.  This is useful only for 
debugging the Ness processor and inset.  It is of no aid for debugging 
programs written in Ness.


Esc ~   -  Switch between author and user modes.  \{This is broken right now\}



\section{\{\{Future:  4. Ness Program Development Support}


The Ness system offers two levels of program development support: editing 
tools and debugging tools.


\subsection{Editing Tools}


The Ness editing tools can be associated with a Ness script or program in 
either of two ways.  If you are editing a script in a Ness inset, these tools 
are automatically in place.  If you are editing a Ness program you can add the 
following to your .atkinit file:

	

	addfiletype .n ntext


Then any file you edit with the extension ".n" will have these editing aids.


\italic{Automatic completion and indentation.}  As you type a newline after 
the first line of a statement group, the last line is inserted automatically 
and the contained lines are indented correctly.  Statement groups are 
introduced with if, while, function, extend, on, and select.


\italic{Automatic fonting.}  Function names and the strings for extend and on 
are displayed in bold.  Comments will be displayed in italic if a readable 
italic font is found.


\italic{Key and Menu mapping}.  The menu option GetProc will cause the next 
keystroke or menu selection to be inserted in the document as the name of the 
proctable entry which would have gotten called for the selection.


\italic{Parenthesis checking.}  As you type a right side parenthesis, brace, 
or bracket, the corresponding left symbol is flashed.  Similarly, closure of a 
string flashes the start.


\italic{Error detection}.  Various token level errors are checked and flashed 
such as failure to close a string on a single line, incorrect number tokens, 
illegal special characters, and a mismatch of the word after "end" with the 
word at the start of the staement group.


\italic{Inset identification}.  After entering inset(), a click on an inset 
will insert its name.


\italic{Declaration insertion}.  As new identifiers are used, declarations are 
automatically inserted with an inferred type.



\subsection{Debugging Tools}


Debugging comes into action during execution of a Ness script.  It is 
activated by various menu options.  To get debugging with nessrun, use the -d 
switch.  These options are available:


Stack dump.  Displays the contents of the stack.   Provides for modifying the 
values in a stack frame.


Breakpoints.  Execution can be intercepted at a given point in the program.


Single statement execution.  One at a time.


Interruption on variable change.  Any "watched" variable will cause execution 
to be interrupted when it is changed. 



\heading{\}\}}


\section{Appendix. Known bugs}


If the keys in the argument to a dokeys call a keysequence which is 
intercepted by \bold{on keys}, Ness crashes.


parseint(), parsereal(), and firstobject() do not obey the search conventions 
for limiting the search.


Hasstyles() does not always return the correct result.


Ness scripts modified in the NessLibrary window do not affect copies of the 
script in other windows.



xxx  list all error messages\
\enddata{text,2065904}
