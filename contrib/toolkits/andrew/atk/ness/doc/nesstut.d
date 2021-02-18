\begindata{text,1831784}
\textdsversion{12}
\template{default}
\define{example
menu:[Region~4,Example~12]
attr:[LeftMargin LeftMargin Cm 83230]
attr:[Justification LeftJustified Point 0]
attr:[FontFace FixedFace Int Set]
attr:[FontFamily AndyType Int 0]}
\define{concat
}
\flushright{7 December 1989}



\italic{\{\{Future: This version of the manual includes planned but 
non-existent features.  These are delimited by double curly brackets so they 
can be removed to form the manual itself.\}\}}




\majorheading{Ness: A Short Tutorial}


\center{by Wilfred J. Hansen}


\indent{\flushright{\smaller{_________________________________


Information Technology Center,

Carnegie Mellon University


Copyright IBM Corporation 1988, 1989

__________________________________


}}
Ness is a programming language for the Andrew ToolKit.  With it, documents can 
be processed and can even contain active elements controlled by Ness scripts. 
 The language features an innovative substring algebra for string processing. 
 


This document is a tutorial description of the language itself.  For a 
description of other Ness documentation, see \bold{Ness User's Manual}.


   1. String Algebra

   5. An Example

   Appendix: /usr/andrew/lib/ness/demos


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

/* $Source: /xsrc/contrib/toolkits/andrew/atk/ness/doc/RCS/nesstut.d,v $ */

/* $Log:	nesstut.d,v $
Revision 1.2  89/12/07  12:15:43  xguest
added author's name
 Revision 1.1  89/12/07  11:51:00  xguest Initial 
revision

Revision 1.2  89/10/24  08:35:14  wjh updated  */

}\formatnote{.bp}


visible

This manual is a short tutorial on the Ness language.  For other related 
material, see the list of manuals in the \bold{Ness User's Manual}.  If you 
are familiar with other programming languages, you may still benefit from 
reading the sections of this manual that describe the string algebra.



The basic element of a Ness program is a function definition.  For instance:


\example{function main()

\leftindent{printline ("Hello, world")}

end function}


When executed, this program will print "Hello, world" in the typescript or 
shell window.


To execute a Ness program from a typescript, give the nessrun command.  If the 
text above is in file hello.n, the command would be


\example{nessrun hello.n}


Nessrun compiles the program into an internal form and executes it by calling 
the function main().  The name \italic{main} is magic--Nessrun always calls it 
first.  


If you want to pass parameters to main(), you can write a string after the 
name of the Ness program file:


\example{nessrun hello.n This is a string}


The string after the file name is passed as an argument to main().  To access 
it, main needs to declare an argument:


\example{function main(arg)

\leftindent{printline(arg)}

end function}


The printed output is a single line saying


\example{This is a string}


Ness programs can do arithmetic.  Here is a program to print the first few 
Fibonacci numbers:

\example{
function main()

\leftindent{integer i

i := 0

while i <= 12 do

\leftindent{printline(textimage(Fibonacci(i)))

i := i + 1}

end while}

end function;


integer function Fibonacci (integer x)

\leftindent{if x <= 1 then 

\leftindent{return x}

else

\leftindent{return Fibonacci(x-1) + Fibonacci(x-2)}

end if}

end function}


With the above in file /tmp/fib.n, a typical execution would look like this:


\example{% \bold{nessrun /tmp/fib.n}

Compile okay.  Elapsed time is 4360 msec.

0

1

1

2

3

5

8

13

21

34

55

89

144

Execution okay.  Elapsed time is 280 msec.}


Items to note about the program:


* The \bold{for} statement is currently not implemented, so it has to be 
simulated with a \bold{while} statement as shown in main().


* Since the Fibonacci function returns an integer value, it cannot be printed 
directly;  it must be converted to a string value by passing it to the 
function textimage(), which converts any value to an appropriate string. 
 (Plans are in the works for letting print() and printline() directly convert 
integer values to strings.)


* Function arguments and return values are by default marker values, so the 
types must be explicitly declared \bold{integer} for Fibonacci() and its 
argument x.


* The algorithm shown is highly inefficient;  consider how many times 
Fibonacci(1) is computed.  The reader is invited to write a better version of 
this program and try it out.



\section{1. String Algebra}


String values in Ness are formally known as \italic{marker} values because 
such a value does not actually denote a string, but rather refers to, or 
marks, a substring of an underlying base string.  A program can declare marker 
variables:


\example{marker s, m

}
and assign them references to constants:


\example{s := "abcdefg"

m := s}


Now s refers to the entirety of the base string  a b c d e f g .  And m refers 
to the same.  This is a fundamental property of assignment of marker values: 
 m \italic{does not} refer to a copy of \typewriter{s}--it refers to the 
\italic{same} base string as \typewriter{s}.  


Four primitive functions start(), next(), base(), and extent() are provided 
for manipulating string values.  (Many additional functions are defined in 
terms of these primitives.)


\bold{start(x)}  -  The value of start of x is a marker refering to a position 
between two characters.  In particular, the value refers to the position just 
before the start of the value referred to by x.  Suppose x refers to the 
substring  c d e  of the value  a b c d e f g  above, then the value of start 
of x is a marker for the empty position between b and c.


\bold{base(x)}  -  Returns a marker for the entire base string of which x 
refers to a part.  Suppose again that x refers to the substring  c d e  of the 
value  a b c d e f g ,  then the value of base(x) is a marker referring to the 
entire value  a b c d e f g .


To get an empty mark at the beginning of the underlying base string for marker 
p, you can use the composition of the functions start and base: 
 start(base(p)).   The opposite composition, base(start(p)), returns exactly 
the same value as base(p) because p and start(p) both refer to the same 
underlying base string.


\bold{next(x)}  -  This function returns a marker for the character following 
the value marked by x.  When x is  c d e , next(x) is a marker for f on the 
same base string.  If the argument x extends all the way to the end of its 
base string, then next(x) returns an empty mark for the position just after 
the last character of x.  


Now we can write expressions for more interesting subsequences relative to a 
given mark t.  The first character \italic{after the beginning of t} is 
next(start(t)) because start(t) computes the empty marker just before the 
first character and next() computes the character just after its argument. 
 Note the careful inclusion of the phrase "after the beginning of t"; 
 next(start(t)) does not compute the first character of t if t is a marker for 
an empty substring--such a marker has no characters and thus certainly no 
first character.


Similarly, the second character after the beginning of a given marker t is 
next(next(start(t))) and the first character of the base is 
next(start(base(t))).


\bold{extent(x, y)}  -  Computes a marker from two other markers on the same 
base.  The new marker value extends from


	the beginning of the first argument,  x


to


	the end of the second argument,  y.


So if x is  c d e  in  a b c d e f g  and y is  b c d e f  then extent(x, y) 
will extend from just before c, the first character of x, until just after f, 
the last character of y;  the value will be c d e f.  The two arguments need 
not overlap or even be contiguous;  if x is  b c  and y is  e f  the value of 
extent(x,y) is b c d e f.  


It may be that the first argument of extent() begins after the second ends. 
 In this case extent() is defined to return the a marker for the empty 
position just after the \italic{second} argument.  Note that this position 
comes \italic{before} the beginning of the first argument.


Another possible problem with the arguments to extent() is that they may be on 
different base strings.  In this case, the value returned is a marker on a 
unique base string whch has no characters.


With the aid of extent() we can write even more useful functions.  Suppose we 
really want the first character of a marker;  the result should be empty if 
the argument marker is empty.  We approach indirectly by first finding an 
expression for all characters after the first character of a marker value. 
 Let t be the marker value and assume for the moment it has at least one 
character.  From earlier work we know that the first character is given by 
next(start(t)) and the character after that by next(next(start(t))).  Now the 
sequence of all characters after the first character begins with that second 
character and extends to the end of t, so we can write the function rest():


\example{function rest(t)

\leftindent{return  extent(next(next(start(t))), t)}

end function}


Now imagine that the argument t for rest() has no characters.  The first 
argument to the extent function will be a marker which begins after the end of 
t, the second argument;  so the result of the extent() will be the end of that 
second argument.  But since t is empty, the end of t is identical to t itself 
and the function rest() applied to an empty marker returns a marker equal to 
its argument.


With the aid of rest() we can compute the first character of a marker:  it is 
the marker which extends from the beginning of the original marker up to the 
beginning of the rest().


\example{function first(t)

\leftindent{return extent(t, start(rest(t)))}

end function}


It may seem inefficient to compute first() and rest() by such round-about 
means, and it is.  These are simply formal definitions in terms of the four 
primitives start(), base(), next(), and empty();  in practice first() and 
rest() can be implemented more efficiently by writing them in the same low 
level language as is used for the primitives.


\{\{Future: This tutorial will discuss concatenation, replace, search\}\}



\section{5. An Example}


The string processing sub-language is illustrated in the example below. 
 String values are formally known as \italic{marker} values because they do 
not actually denote strings, but only refer to, or mark, substrings of 
underlying base strings.  The marker variables in the example are 
\italic{letters}, \italic{text}, \italic{args}, and \italic{filename}.


Concatentation, with \concat{~}, of marker values creates a brand new string 
value and returns a marker referring to its entirety.  Marker assignments 
assign only the reference and do NOT create a new string that is a copy of the 
old.  The functions token() and span() are built-in marker functions which 
begin scanning at the start of their first argument and look for a string 
meeting the pattern in the second argument.  Span() scans its first argument 
and returns a marker for the maximal initial substring consisting of 
characters in the second argument.  (The scan may extend beyond the end of the 
first argument.)  Token() is similar, but begins by skipping initial 
characters of the first argument until finding one from the second argument.

\begindata{bp,1513096}
\enddata{bp,1513096}
\view{bpv,1513096,0,0,0}
\example{\smaller{-- wc.n


-- Count the words in a text.

-- 	A word is a contiguous sequence of letters.



-- To use as a main program:

--	nessrun -b /usr/andrew/lib/ness/wc.n <filename>

-- the number of words in the file is printed.

--

-- To call from a Ness function:

--	wc_countwords( <a marker for the text> )

-- returns an integer value giving the number of words.




marker letters	-- a list of the letters that 

			-- may occur in words

	:= "abcdefghijklmnopqrstuvwxyz"

         ~ "ABCDEFGHIJKLMNOPQRSTUVWXYZ"



-- countwords(text) counts the number of sequences 

--    of adjacent letters

--

-- 'text' is a marker for a substring of the full text

--    token(x, m) searches forward from the beginning of

--    x through the rest of text for the first sequence 

--    of characters all of which are in m

--

--    Any Ness program can call wc_countwords().

--

\leftindent{integer}

function wc_countwords(text)

\leftindent{integer count

marker t

count := 0          	-- no  words so far

t := token(text,letters)	-- find first word

\leftindent{\leftindent{\leftindent{\leftindent{-- check to see if the token 
found

-- starts after the end of the text}}}}

while t /= "" and extent(t, text) /= "" do \leftindent{

count := count + 1	-- count this word

t :=         	 	-- find next word

		-- start search at next(t), 

		-- the first character after 

		-- the preceding word

  token(finish(t), letters)

		-- if no word was found, token()

		-- returns an empty string}

end while

return count}

end function 

\begindata{bp,1913432}
\enddata{bp,1913432}
\view{bpv,1913432,1,0,0}
-- the main program initializes the global variable,

--    reads a file, counts the number of words in it,

--    and prints a line

-- span(x, m) finds the longest initial substring of

--    x composed of characters from m

-- ~ indicates concatenation of string values

--

function main(args)\leftindent{

marker filename


-- extract the file name from argument list

-- find the initial substring of args which is 

-- composed of letters, digits, dots, and slashes.

filename := span(start(args), letters ~ "./0123456789")


-- read file, count words, and print result

printline ("The text of " ~ filename ~ " has "\leftindent{

~ textimage(wc_countwords(readfile(filename))) 

~ " words")}}

end function



-- Select some text.  Type ESC ESC, and respond to 

-- the "Ness:" prompt with

--	wc_showcount()

--

-- The following function will be called and will show 

-- the number of words in the current selection

--

function wc_showcount() \leftindent{

TellUser(textimage(wc_countwords(\leftindent{\leftindent{

currentselection(currentinset))))}}}

end function



-- To use the following entry point, 

-- put these lines in your .atkinit:

--	load ness

--	call ness-load /usr/andrew/lib/ness/wc.n

-- Then choosing the menu entry "Count words" will show 

-- a count of the words in the current selection

--

extend "view:textview"\leftindent{

on menu "Search/Spell,Count words~60"\leftindent{

wc_showcount()}

end menu}

end extend}}

\begindata{bp,1783016}
\enddata{bp,1783016}
\view{bpv,1783016,2,0,0}
Execution of this function for two examples went like this:

\leftindent{
% \bold{nessrun wc.n wc.n}

Compile okay.  Elapsed time is 2903 msec.

The text of wc.n has 407 words

Execution okay.  Elapsed time is 1532 msec.

% \bold{nessrun wc.n ness.c}

Compile okay.  Elapsed time is 1522 msec.

The text of ness.c has 4617 words

Execution okay.  Elapsed time is 12284 msec.}



\bold{Appendix:  /usr/andrew/lib/ness/demos}


These demonstration files each contain an explanation of how to use them.  In 
general, they should be copied to /tmp before editing them with ez;  they must 
be read-write in order to display their effects.  (And if they are editted 
directly in /usr/andrew/lib/ness/demos, they leave .CKP files around charged 
against your account.)


\leftindent{\description{birth.db - A data base.  Click on a reference in a CF 
field.  Try sorting.

capstest.d  -  Two different approaches to scanning text.

xmas.d  -  Music!  Pictures!

bank.d - A prototypical parametric letter.

calc.d - Yet another calculator.  The most truly programmable yet.

volume.d  -  Very simple illustration of something the NextStep application 
builder can't do.

}}\enddata{text,1831784}
