\begindata{text,2126960}
\textdsversion{12}
\template{default}
\define{concat
menu:[Font,Concat]
attr:[Script PreviousScriptMovement Point 6]
attr:[FontSize PreviousFontSize Point 2]}
\define{sans
menu:[Font,Sans]
attr:[FontFamily AndySans Int 0]}
\flushright{7 December 1989}



\italic{\{\{Future: This version of the manual includes planned but 
non-existent features.  These are delimited by double curly brackets so they 
can be removed to form the manual itself.\}\}}




\majorheading{Ness Language Reference Manual}


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
 


This document is a reference manual describing the language.  For a 
description of other Ness documents, see the \bold{Ness User's Manual}.



   1. Program Structure

   2. Statement Forms

   3. Expression Forms

   4. Extending Insets

   5. Tokens Rules

   6. Recent Changes



\smaller{CAVEAT: This manual describes an early version of the Ness language 
and system.  You are invited to contribute suggestions for change to the 
author (\italic{wjh+@andrew.cmu.edu}) or the bboard (\italic{andy.ness}).  But 
please realize that your suggestions and those of others may evolve the 
language enough to force you to revise or abandon programs written to this 
document.}\bold{

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

/* $Source: /xsrc/contrib/toolkits/andrew/atk/ness/doc/RCS/nessman.d,v $ */

/* $Log:	nessman.d,v $
Revision 1.2  89/12/07  12:15:31  xguest
added author's name
 Revision 1.1  89/12/07  11:50:42  xguest Initial 
revision

Revision 1.3  89/10/14  09:26:02  wjh added the requirement that if a 
statement is not the first thing on a line it must be preceded by a semicolon. 
 Revision 1.2  89/10/12  13:53:53  wjh added character constants  Revision 1.1 
 89/10/11  09:26:45  wjh Initial revision

Revision 1.4  89/10/11  09:25:12  wjh Major revision  */

}\formatnote{.bp}



Ness \italic{programs} reside in their own files and process other files. 
 Ness \italic{scripts} reside within Ness insets in  a document or 
application; from there they extend and influence the behavior of other 
insets.  In contrast with other languages, Ness is rather simplistic:  it has 
arithmetic expressions, functions, if-then-else, while-do, and little else. 
 The two major innovations are the control of insets--with \bold{extend} and 
\bold{on}--and an algebra of functions for string operations.


This manual includes a description of Ness's various statement and expression 
forms.  If you are familiar with other programming languages you should review 
the section on statement and expression forms and the various sections on 
functions available.  You should also review the sections on the string 
algebra in the Tutorial.


\{\{Future:  It will be possible to compile Ness programs into C and thence 
into machine language.  It will then be feasible to do considerable amounts of 
system development in Ness alone.  Consideration for compilation has been an 
integral part of planning for Ness;  indeed, the present interpreter 
interprets a byte code that simulates a possible real machine.\}\}


In the template forms below, bold words are to appear in programs as shown. 
 Words that are to be replaced with statements, expressions, or other elements 
are hyphenated compounds as in "statement-list".



\section{1. Program Structure}


A Ness program is a sequence of 


	global-variable-declarations, 

	function-definitions,  and

	object-extension-specifications.


If the program is standalone, execution begins by an artificially generated 
call to the function main(), which will be defined with a function-definition. 
 If the program is a script in an inset, execution begins when one of the 
events specified in an object-extension-specification occurs.



Global-variable-declarations are written in the same form as a 
variable-declaration inside a function-definition;  see the description below. 
 A global variable may be referred to from within function definitions 
anywhere after the point at which it appears in the program.  An 
initialization expression in a global declaration may refer to other global 
variables previously defined and to functions defined anywhere;  however, all 
global variables referred to by such function calls must already have values 
by earlier initializations.



A function-definition gives the type returned, the name of the function, a 
list of the parameters expected, and a list of statements to be executed when 
this function is called.  The form of these elements is:


\leftindent{\leftindent{ness-type  \bold{function}  }

function-name  \bold{(}  parameter-list  \bold{)}

\leftindent{statement-list  }

\bold{end}  \bold{function}

}
The ness-type indicates what type of value this function will return.  Valid 
types are \bold{marker}, \bold{integer}, \bold{real}, \bold{Boolean}, and 
\bold{object}, for all of which the function must execute a \bold{return} 
statement returning a value of the appropriate type.  The ness-type may also 
be \bold{void}, in which case the function must not return a value.  (It can 
return via \bold{exit function}.)  It is permitted to omit the ness-type, in 
which case the function either returns a \bold{marker} value or none at all. 
 If the function is called in a context that requires a particular type of 
returned value and none is returned, an error will be indicated when the 
caling statement is executed.


\{\{Future: Possibly we will implement a type \bold{any}, to which values of 
any type may be passed.  Their type would need to be tested with typename() or 
a special form of the \bold{select} statement.\}\}


The parameter-list is a list of names separated by commas.  Each name may have 
a type-name in front of it; if not, the type is \bold{marker}.  When the 
function is called, the arguments are evaluated and the value of each is 
assigned to a parameter variable allocated for the duration of execution of 
the function.  Thereafter parameter variables behave the same as variables 
declared within the statement-list.  Before executing the statements in 
statement-list space is set aside for all variables declared within the list. 
 Then execution begins with the first statement and continues to the last, 
except as directed by \bold{if}, \bold{while}, \bold{return}, or \bold{exit 
function} statements.


Suggested indentation is shown in the syntax form above.  This is not 
mandatory, but it is of value when a syntax error is detected during 
compilation.  In that case, the parse skips ahead to the next 
function-definition, which is recognized partially on the basis of 
indentation.  For a function definition to be recognized, at least one of 
  function-type,  \bold{function},  and  function-name   must be directly at 
the left margin.


The function  main(args)  has a special role.  Main() is called whenever 
nessrun is applied to the file;  at that time, the value passed for args is 
evrything from the command line after the command name ("nessrun") and the 
\italic{filename}.n.


Semi-colons are allowed anywhere between global variable definitions, function 
definitions, and extends.


\{\{Future: The <statement-list> may be replaced with the word \bold{forward}. 
 Then the procedure can be declared later in the text.\}\}



An object-extension-specification is a block with the form


\leftindent{\bold{extend} "insetName"

	event-interceptor-list

\bold{end exend}}


where "insetName" is the name of an inset in the same document or application 
with the Ness.  For details, see section 4 below.



\section{2. Statement Forms}


The statement-list in a function-definition consists of a sequence of 
statements.  Statements are executed in succession unless dictated otherwise 
by function-call, \bold{if}, or \bold{while}.  The remainder of this section 
describes the various statement forms.



\subsection{Semi-colon}


A single semi-colon as a statement is ignored.  This rule permits semi-colons 
in a script anywhere between statements.  If the start of a statement is 
preceded on a line by other than spaces or tabs, it must be preceded by a 
semi-colon.



\subsection{Declaration}


Declarations are of the form


\leftindent{ness-type   variable-name   \bold{:=}   ness-expression}

\leftindent{ness-type   variable-name}

\leftindent{ness-type   variable-name \bold{,}   variable-name 

ness-type   variable-name \bold{,}   variable-name \bold{,}   variable-name 

. . .}



where ness-type is one of \bold{integer}, \bold{real}, \bold{marker}, 
\bold{object}, or \bold{Boolean}.  The variable(s) named will be of the given 
type throughout the remainder of the statement-list, even beyond an \bold{end 
if} or \bold{end while} for an enclosing \bold{if} or \bold{while} statement. 
 Although declarations are thus allowed to appear anywhere, it is conventional 
to collect them at the beginning of a statement-list.   In the first form of 
declaration, the single variable is declared and will be initialized to have 
the value of the ness-expression.  The computation of the expression can 
involve function calls and references to variables initialized prior to this 
one.  If there is an initialization, there can be only one variable declared. 
 With no initialization, the other forms of declaration are possible;  that 
is, the ness-type may be followed by a comma-separated list of variables.  All 
are declared and given no legal initial value.



\subsection{Assignment}


A variable is given a value with the assignment statement:


\leftindent{variable-name  \bold{:= } ness-expression}


For assignment of types other than object and marker, the value of the 
ness-expression is copied and the copy is stored in the named variable.  When 
the variable and expression are of type \bold{object}, only a reference to the 
object is assigned, the object is not copied.   When the variable and 
expression are of type \bold{marker},  the target variable receives only a 
reference to the appropriate substring of the base.  No new string text is 
created.



\subsection{String append}


One way to build up string values from smaller strings is to append values 
onto the end of the string.  A special statement form is provided for this:


\leftindent{variable-name  \concat{~}\bold{:=}  ness-expression}


This concatenates the value of the expression onto the end of the base of the 
string referred to by variable-name and changes variable-name to refer to the 
entire resulting base string.    (The following explanation will make more 
sense after reading Section 3.5.)  One way to think of  \concat{~}\bold{:=} 
 is that it concatenates the value of the expression to the value of the 
variable, as would be done by:


\leftindent{var  \bold{:= } base(var)  \concat{~}  expr		-- inaccurate model}


This model is inaccurate because in that it implies that all markers to the 
old value of var will no longer refer to the same string as var does (because 
concatenation creates a new string value).  The accurate description of the 
append statement is this:


\leftindent{var \bold{:=} base(replace(finish(base(var)), expr)) 	-- accurate 
definition of \concat{~}:=}


In other words, the value of the expression is inserted at the end of the base 
string of var and then var is assigned a marker for the entire base string, 
including the new piece.


A common programming error is to assign 


	v := ""		-- initialize v


and then attempt to extend the value of v with 


	v ~:= somemarkerexpression	-- FAILS after v := ""


This fails because "" is a constant whose value cannot be modified.  Instead 
the initialization assignment should have been


	v := newbase()		-- initialize v


\{\{Future: As a special case, if variable-name is a read-only value on an 
empty base, the old value is discarded and the variable will be made to refer 
to a new base string consisting of a copy of the ness-expression.  (This 
provision solves the problem posed by programmers who forget to use newbase(). 
 However, it may lead to worse problems because now the program may have two 
variables referring to the same empty constant and after the string append 
they will refer to different bases so extent will not work.) \}\}



\{\{Future:  \subsection{Execute}

This statement, whose syntax is yet to be defined, compiles the enclosed code 
and executes it.  There is provision for passing values to the expression and 
retrieving the value returned by  it, but the expression does not have access 
to the global variables of the script containing the execute statement.  


One important application for the execute statement is to be able to read a 
search query from the user and compile it for execution to do the search.\}\}



\subsection{Function call}


A function call by itself constitutes a statement:


\leftindent{function-name  \bold{(}  argument-list  \bold{)} }


where the argument-list is a sequence of zero or more expressions separated by 
commas.


This is the only context in which it is acceptable to call a function declared 
to have type "\bold{void}"; however, functions of other types can also be 
called and the value they return is ignored.



\subsection{Return} 


The \bold{return} statement terminates execution of the enclosing function or 
event body.  If the function has a type, the return statement must have an 
expression that gives a value of that type:


\leftindent{\bold{return}  ness-expression }


For a return from an event body or a function declared to have the type 
\bold{void}, use the form


\bold{\leftindent{exit function}}


\{The error checks implied in this section are not made.  Any function or 
event body can return a value of any type or none at all.\}  \{\{Future: 
Eventually this will be fixed.\}\}



\subsection{If}


The \bold{if} statement tests a Boolean expression and conditionally executes 
one of a set of clauses.  The simplest form is:


\leftindent{\bold{if}  Boolean-expression  \bold{then} 

\leftindent{then-clause-statement-list 

}\bold{end if}}


If the Boolean expression is \bold{True}, the statements in the 
then-clause-statement-list are executed.  Sometimes alternative statements 
need to be done when the Boolean value is \bold{False}.  This is done by 
adding an else-clause-statement-list:


\leftindent{\bold{if}  Boolean-expression  \bold{then} 

\leftindent{then-clause-statement-list

}\bold{else} 

\leftindent{else-clause-statement-list

}\bold{end if}}


(Note the indentation;  it is standard for Ness programs.)  Here the 
else-clause-statement-list is done if the Boolean value is \bold{False}.  When 
more than two alternatives exist, the sequence \bold{else if} may be used, but 
this requires an \bold{end if} for each \bold{if}.  To reduce the number of 
\bold{end if}s, any number of \bold{elif} forms can be inserted after the 
then-clause-statement-list:


\leftindent{\bold{elif}  Boolean-expression  \bold{then}

\leftindent{elif-clause-statement-list}}


This construct is executed in place of an \bold{else} and first evaluates its 
own Boolean-expression.  If \bold{True}, the elif-clause-statement-list is 
executed and control continues after the \bold{end if}.  If \bold{False}, 
controls proceeds to the next corresponding  \bold{elif},  \bold{else}, or 
 \bold{end if}.


\{\{Future: The statement \bold{gotoelse} transfers control to just after the 
next succeeding \bold{else} or \bold{elif} which corresponds to a preceding 
\bold{then}.   (This operation is the same as \bold{fail}, as described and 
motivated by Kovats, T.A., "Comments on innovative control constructs in 
Pancode and EPN", \italic{SIGPLAN Notices}, Vol. 23, No. 12, pp. 151-157.) 
\}\}



\subsection{While}


The \bold{while} construct provides for repeated execution of a sequence of 
statements until some condition is met:


\leftindent{\bold{while}  Boolean-expression  \bold{do} 

\leftindent{do-clause-statement-list

}\bold{end while}}


(Again note the indentation standard.)  When control reaches the \bold{while}, 
the Boolean expression is tested.  If \bold{False}, control proceeds to after 
the \bold{end while};  otherwise the do-clause-statement-list is executed and 
control returns to the \bold{while} for a repeat of the whole process.


The statement 


\leftindent{\bold{exit while} }


transfers control to just after the next succeeding \bold{end while} which 
corresponds to a preceding \bold{while}.



\{\{Future: \subsection{Forall}


The \bold{forall} construct provides for executing a <statement-list> 
repeatedly, once for each instance of some pattern in a sequence:

\leftindent{
\bold{forall}  variable-name  \bold{:=} \leftindent{

\bold{find}  string-pattern 

\bold{in}  marker-expression}

\bold{do} 

\leftindent{statement-list}

\bold{end forall}}


The variable is declared by its appearance after forall to be a marker value. 
 For each instance of the string-pattern in the marker-expression, 
variable-name is set to a marker for the instance and the statement-list is 
executed.  variable-name is not the same as any other variable by the same 
name, and others of the same name cannot be accessed inside the 
statement-list.


For the simple pattern of selecting each element of the sequence in turn, the 
construct can be written without the colon-equal, the \bold{find}, and the 
string-pattern.


The statement   

\leftindent{\bold{exit forall}}

transfers control to just after the \bold{end forall} corresponding to the 
nearest preceding \bold{forall}.


\}\}

\{\{Future: \subsection{Select}


The select construction provides for execution of one of a number of different 
statement-lists depending on the value of an expression:


\leftindent{\bold{select}  ness-expression  \bold{from}

\leftindent{\bold{case} ness-expression  \bold{:}  statement-list

\bold{case} ness-expression  \bold{:}  statement-list

	. . .}

\bold{end select}}


The first expression is evaluated and its value is tested against each of the 
other expressions in turn.  When an equal match is found, the corresponding 
statement-list is executed and control proceeds to just after the \bold{end 
select}.  The word \bold{else} after \bold{case} will match True against any 
other expression; this option should obviously be the last in the sequence.  

\}\}



\section{3. Expression Forms}


The common forms of expression in Ness are 


\leftindent{constants, 

variables, 

parenthesized expressions,   and 

function calls.

}
Only a few infix and prefix operators are defined; they are described in the 
discussion of each data type below.


Variables must be declared before they are used and may be global to an entire 
program, a formal parameter to a function, or local to a function or event.  A 
declaration is written as the type name followed by the identifier.


An execution error occurs if an attempt is made to utilize the value of an 
uninitialized variable or if the type of an expression does not match the type 
required by the context..


\{\{Future: Types should be coerced in most cases.\}\}


For all ness-types function calls are of the form:


	function-name  \bold{(}  argument-list  \bold{)}


where argument-list is a sequence of zero or more ness-expressions separated 
with commas.  By default, parameters and the function return value are of type 
\bold{marker}.   If the function call is a forward reference, the type 
returned is expected to be \bold{marker}


The precedence of operators is:


\indent{\bold{or}		lowest

\bold{and}

\bold{not}

\bold{=  /= <  <=  >  >=

+  -

*  /  %}

\concat{~}		highest}



\subsection{3.1 Boolean expressions}


The two Boolean constants are \bold{True} and \bold{False}.  Each Boolean 
variable and expression has one of these two values.  The following operators 
apply to, or generate, Boolean values:


\description{\bold{and } \bold{or } \bold{not}:  The usual Boolean operations. 
 Execution of expressions involving \bold{and} and \bold{or} is 
"short-circuited;"  operands whose values are immaterial to the value of the 
expression are not evaluated.  That is, the second operand is not evaluated if 
the first is \bold{True} for \bold{or} or \bold{False} for \bold{and}.


\bold{=  /=  <  <=  >  >=   }The usual comparison operations.  (Note that 
not-equal is written as "\bold{/=}".)  All six comparisons are defined for 
integer, real, and marker values.  Only equality and inequality are defined 
for Booleans and pointers.  (Marker comparisons test the values of the strings 
referenced and not the relative positions indicated by the markers, but see 
the discussion in the Section 3.5 on marker expressions.)}


\{\{Future:  When used as a string expression a Boolean value is converted to 
one of the strings "True" or "False".\}\}



\formatnote{.ne 1.5i}

\subsection{3.2 Integer expressions}


Integer constants are described in Section 5 on "Tokens."  The usual 
arithmetic operations are defined: \bold{+}, \bold{-}, \bold{*}, \bold{/}, and 
\bold{%}.  Unary-plus and unary-minus are both defined.  Division (\bold{/}) 
discards the remainder;  another operator (\bold{%}) returns the remainder. 
Overflow is ignored by the interpreter;  divide-by-zero is a run-time error. 
 All six comparison operators are defined for integer values.


\{\{Future: When used in a string expression, an integer value is converted to 
the corresponding string of decimal digit characters.  When assigned to a real 
value, passed to a real parameter, or in a mixed expression, an integer value 
is converted to real.\}\}



\formatnote{.ne 1.5i}

\subsection{3.3 Real expressions}


Real constants are described in Section 5 on "Tokens."  The usual arithmetic 
operations are defined: \bold{+}, \bold{-}, \bold{*}, and \bold{/}. 
 Unary-plus and unary-minus are both defined.  Overflow, underflow, and divide 
by zero are ignored by the interpreter; the results can be tested for validity 
with isnan() and finite() \{These functions are not available on all 
hardware.\}.  All six comparison operators are defined for real values.


\{\{Future: Comparison for equality accepts a small epsilon of error.  This 
includes the test for zero in sqrt() and log().\}\}


At present, there are no automatic conversions between integer and real.   Use 
float(), round(), floor(), and ceil().


\{\{Future: Conversion occurs as required.  For assignment and passing to an 
integer parameter, real is converted to integer by rounding.  When used as a 
string value, reals are converted according to %g.\}\}



\formatnote{.ne 1.5i}

\subsection{3.4 Object expressions}


An object expression refers to some object in the system, usually an inset in 
the document or application.  Equality and inequality are defined for object 
values.  The constant \bold{null} is an object value which can be assigned as 
value to any object variable in order to indicate that it refers to no object.


For full details of functions and predefined identifiers related to objects 
see the \bold{Ness Function Reference Manual}.  Here are a few:


The \bold{inset}() function has one marker value argument;  it returns a 
pointer to the object for the inset named by the marker value.  The inset must 
have had its name registered with the surrounding arbiter;  see Section 4 on 
"Extending Insets".


The class() function returns a quintessential object for the class named by 
its argument.  The new() function has as its argument an object and returns a 
brand new object of the same class.  The predefined object variables 
defaulttext, currentinset, currentinputfocus, and currentwindow refer to 
specific objects in the environment at the given time.   


The principal utility of object values is to pass them to functions defined in 
C code.  One set of functions are those in the "proctable" and specified in 
the documentation as being available to be bound to keys and menu entries. 
 Another set are the methods, class procedures and instances variables defined 
in the class declaration for the class (the .ch file).  When calling functions 
from either of these sets, the Ness interpreter takes care to get the proper 
object from the two or more that constitute an inset.  The inset() function 
actually returns a \italic{celview} object which points to the \italic{data} 
object and \italic{view} object that actually produce the visible image.  If 
the function does not apply to the celview class, a check is made to see if it 
applies to the class of the data object, the view, or one of several other 
possibilities.


\{\{Future: When used in a string expression, object values are converted to 
strings by producing the hex representation of their address.  If a string 
containing object o is desired, use a sequence expression: \{o\}.  \}\}



\subsection{3.5 Marker expressions}


A marker value refers to a substring of contiguous characters in some 
underlying base string.  String constants are described below in Section 5 on 
"Tokens".  The value of a string constant is a marker value referring to the 
entire string.  New, non-constant string values are created only by 
\italic{newbase}(), concatenation, \italic{readfile}(), and \italic{system}().


Assignment of a marker value to a marker variable does NOT create a new string 
value;  the marker variable will subsequently refer to the same base string as 
did the original value.  All six comparison operators are defined for marker 
values.  The values compared are the strings referred to by the markers, 
whether or not they are on the same base.  A marker for a string of length 
zero is always equal to any other marker for a string of length zero and less 
than any marker for a non-zero length string.


\{\{Future: Maybe we need a different assignment operator for marker 
assignment.  Say ":-".  Some cT authors get confused between copying strings 
and assigning marker values.\}\}


String concatenation is indicated with the tilde operator as in


\leftindent{<string-expression> \concat{~} <string-expression>

}
This expression constructs a brand new string having a base different from all 
other strings, including its arguments.  The value of the expression is a 
marker referring to the entirety of the new string.


One way to append to an existing string is with the string-append statement 
using \concat{~}:= as described in Section 2.  Another way is to replace a 
portion of a base string with the replace() function, one of the six primitive 
marker functions.  For details on these operation and other marker functions 
see the \bold{Ness Function Reference Manual}:


\indent{newbase() - returns a marker for a new, empty base string.  This is 
not a constant string and thus it can be modified with replace() or 
string-append.


base(m) - returns a marker for the entire base string of which m refers to a 
portion.


start(m) - returns a marker for the empty strings which starts where m does.


next(m) - returns a marker for the single character following m in its base.


extent(m, n) - returns a marker for the substring which starts where m does 
and ends where n does.  If they are on different bases, the value is an empty 
marker on an empty constant string.  If they are on the same base but the end 
of n precedes the end of m, the value is an empty marker at the end of n.


replace(m, n) - returns a marker for a copy of n which has replaced m in 
base(m).}


Given two markers m and p on the same base, their relative start positions in 
the base can be determined by testing   extent(start(m), start(p)) .  This 
expression will be non-empty if start(m) strictly precedes start(p), but will 
be empty if they are at the same position or start(p) precedes start(m).  


\{\{Future:  Is some way needed to specify the collating sequence?  Will this 
slow down string operations unacceptably?\}\}


\{\{Future: Marker values can also be generated by sequence expressions.  Such 
an expression is delimited by curly braces and contains a sequence of items, 
separated with commas.  For example,


	\{ 3 , "abc", True, 5 ... 7 \}


An item can be an expression or an integer enumeration.  Non-marker 
expressions ( 3 and True in the example) are evaluated to produce a value 
which is then stored as an object in the sequence.  (If viewed, a default view 
is used.)  Marker expressions ("abc" in the example) are evaluated and the 
components of the resulting string are copied as components of the sequence. 
 An integer enumeration (5 ... 7 in the example) has the form


	integer-expression ... integer-expression


or


	integer-expression ... \bold{by} integer-expression ... integer-expression


Both of the latter forms denote the sequence of integers ranging from the left 
limit to the right;  in the second form the increment is given by the middle 
expression.  The sequence generated by the example is a marker value with the 
elements


	 3 a b c True 5 6 7 


where the letters are stored as characters and the other elements are objects 
of one form or another.


The function markerobject(), can be applied to a marker to yield an object 
which can be stored in the sequence (in this case, we have general lists.)  A 
sequence expression returns a marker for the entire generated sequence.\}\}



\section{4. Extending Insets}


When a Ness script is embedded as an inset in a document or application (or 
drawing or table or ...), it can specify extensions to other named insets in 
its surroundings.  For details of how insets become named and nested within 
cel insets, see \bold{Ness Author's Reference Manual} and the documents in 
/usr/andrew/doc/adew.


Given an inset with the name <celname>, its behavior can be controlled through 
Ness with an object-extension-specification.  These have the form


\leftindent{\bold{extend} "insetName"

	event-interceptor-list

\bold{end exend}}


where "insetName" is a constant naming one of the insets in the same document 
or application.  The effect is that the enclosed event-interceptor-list 
applies to the named cel and its embedded inset.  When one of the intercepted 
events occurs for that inset, the corresponding Ness code is executed.  


Since it is crucial that operations added by extension should work properly as 
soon as the extended objects are visible, embedded Ness objects prompt for 
empowerment (compilation) as soon as they are displayed to the user. 
 (Therefore, every Ness that extends an object should be visible in the 
display "close" to where the extended object lies.)  When a Ness is compiled, 
the arbiter is informed of it and any cels which should be extended are 
extended as soon as they become visible.



The event-interceptor-list may include  global-variable-declarations  and 
 function-definitions as described above in Section 1.  These apply only 
within the limits of the current \bold{extend} ... \bold{end extend}.  In 
addition, the list may include event-interception-specifications, each of 
which begins with the keyword \bold{on} followed by one of the event-class 
names \bold{keys}, \bold{menu}, \bold{mouse}, or \bold{event}.  The form of 
these specifications is


\indent{\bold{on} event-class "event-specification"

\indent{statement-list}

\bold{end} event-class}


The specification has a different interpretation for each event-class and the 
statement list is executed exactly as would be one in the body of a function. 
 Each of the event-classes is described in ensuing subsections.


There is one additional option for the "insetName";  it can be a string 
beginning with 'view:' and continuing with the name of a class of  view 
objects--textview, rasterview, zipview, ... .  In this case the key and menu 
elements in the event-interceptor-list apply to all views of the given class 
in any window created after this compilation.  This is primarily of interest 
when ~/.atkinit or some other initialization file loads the Ness file with 
lines like


	load ness

	call ness-load /usr/andrew/lib/ness/wc.n


(The first line is needed once, there can a call ness-load line for each of 
any number of files.)  See Section 3 of the \bold{Ness Hacker's Manual} for 
details.



\subsection{4.1 On Keys}


For \bold{on keys}, the event-specification is a sequence of keys.  When that 
sequence of keys is typed by the user, the event occurs and the statement-list 
is executed.  \{The keys are also recognized if sent via dokeys(), but this 
causes a Ness failure.\}


During execution of the statement-list, two predefined variables have values. 
 


\indent{\italic{currentinset} is an object value referring to the object which 
is extended by this event-interceptor.


\italic{lastkeys }is a constant marker value having the event-specification 
string.}


Relying on these variables, the statement-list can call on a generalized 
function to handle interception for a number of objects or menu items and this 
function can determine exactly which ones it has been called for this time.



\subsection{4.2 On Menu}


For \bold{on menu}, the event-specification is a menu string.  When that menu 
item is selected by the user, the event occurs and the statement-list is 
executed.  The full syntax of a menu string is


	[ menu-card-name [\concat{~}<n>]]\bold{, }menu-item-name [\concat{~}<n>]


where brackets indicate optionality and <n> is a two digit number.  There are 
no spaces, except possibly within the names.  If no menu-card-name is 
specified, the item will be on the front card instead of one of the named 
cards.  If an <n> is specified, the position of the card in the stack or the 
item in the card is influenced;  items are in numerical order of <n> 
 (Although there is little documentation on the value of <n> for existing menu 
items).  If two items have <n>s differing in the tens digits, there will be a 
blank line between those items.


During execution of the statement-list, two predefined variables have values. 
 


\indent{\italic{currentinset} is an object value referring to the object which 
is extended by this event-interceptor.


\italic{lastmenu }is a constant marker value having the event-specification 
string.}


Relying on these variables, the statement-list can call on a generalized 
function to handle interception for a number of objects or menu items and this 
function can determine exactly which ones it has been called for this time.



\subsection{4.3 On Mouse}


For \bold{on mouse}, the event-specification is "any".  When any mouse event 
occurs to the inset, the event occurs and the statement-list is executed.  If 
more than one \bold{on} \bold{mouse} appears within an \bold{extend}, block 
only one of them will be honored.


During execution of the statement-list, several predefined variables have 
values.  


\indent{\italic{currentinset} is an object value referring to the object which 
is extended by this event-interceptor.


\italic{mouseaction }is an integer value encoding which button this event is 
for and whether it went up or down or the mouse moved.  It should be tested by 
comparing it with the mouse constants below.


\italic{mousex} is an integer giving the x coordinate of the mouse within the 
object image.\indent{

}
\italic{mousey} is an integer giving the y coordinate of the mouse within the 
object image.\indent{

}}
The following predefined constants are possible \italic{mouseaction} values: 
 \italic{mouseleftup}, \italic{mouseleftdown}, \italic{mouseleftmove}, 
\italic{mouserightup}, \italic{mouserightdown}, \italic{mouserightmove}.  Each 
indicates which button the action is for and whether the button has gone down 
or up or the mouse has moved with the button down.


It may be necessary within a mouse event interception to pass the event along 
to the object so it can update itself for the mouse hit.  This is done with 
the function \italic{dohit} which causes an object to believe a mouse hit has 
occurred for it.   Dohit has four arguments: the object to be moused, a 
mouseaction value, and the x and y coordinates of the hit.  


Given the above, a typical mouse interception routine looks like this:


	on mouse "any"

		dohit(currentinset, mouseaction, mousex, mousey)

		if mouseaction = mouseleftup then

			--do something interesting

		end if

	end mouse


\{\{Future: The event specification for -on mouse- may be one of  \italic{any, 
 up,  down,  left},  \italic{right},  \italic{left up},  \italic{leftup}, 
 \italic{right up},  \italic{rightup},  \italic{left down,  leftdown,  right 
down,  rightdown}.  The word \italic{pass} may also be included; it indicates 
that the mouse hit is to be sent through to the object with 
dohit(currentinset, mouseaction, mousex, mousey).  Pass may appear at the 
beginning or end of the specification, in which case the event is passed to 
the object before or after execution of the statement list, respectively.\}\}



\subsection{4.4 On Event}


For \bold{on event}, the event-specification is the name of a \italic{trigger} 
as defined by the inset that is named in the surrounding \bold{extend} block. 
 For example, the hyplink inset defines a trigger "buttonpushed"; whenever the 
button is pushed hyplink pulls this trigger.  If an extend block for a hyplink 
inset has


	on event "buttonpushed"

		-- action statements

	end event


and the mouse is clicked on the button, the action statements will be 
executed.  Other triggers are defined in the documentation for individual 
insets.


\{\{Future:  /on event "save"/ specifies an event which occurs whenever the 
component is saved.  Are there other generic events which are needed?\}\}



\section{5. Tokens}


Ness programs and scripts can be enhanced with styles, including bold and 
italic.  These styles are ignored except within string constants.  Comments 
begin with "--" and extend to the end of the line.  There is no difficulty 
with inserting insets in comments and string constants.


Identifiers begin with a letter and continue with letters and digits.  The 
case of a letter is ignored; "aToZ" refers to the same thing as "AtoZ".  The 
following may not be used as identifiers:


\bold{\indent{and  boolean  do  elif  else  end  event exit extend  false 
 function  if  integer  keys  marker  menu  mouse  not  null  object  on  or 
 real  return  then  true  void  while        }}(and for future 
expansion:\bold{ \indent{by  case  find  forall  forward  from  gotoelse  in 
 select}})


\{\{Future: Alphabetic characters from the Latin-1 ISO-8859 set are accepted 
in identifiers.\}\}


Integer and real constants are written as they are in C.  Integer values may 
also be expressed with octal, hexadecimal, and character constants.  Here are 
some examples:


\leftindent{Decimal integers:   0   123123  91  

Real values:  12.3   12e3   12E-3  .2  2.  2.e+5  

Octal integers:  0123   0001   077

Hexadecimal integer:  0x12abc   0x0   0xFFFF0000

Character constant:  'a'   '@'}


Characters in character and string constants may utilize the escape sequences 
found in C and some additional ones:

\leftindent{
\\n		newline

\\r		carriage return

\\b		backspace

\\t		tab

\\f		formfeed

\\v		vertical tab

\\\\		backslash

\\'		apostrophe

\\"		double-quote

\\\italic{ooo}		octal digit representation

		

\\e		escape 

\\^@		\\000

\\^A		control-A

...

\\^Z		control-Z

\\^[		\\033

\\^\\		\\034

\\^]		\\035

\\^^		\\036

\\^_		\\037

\\?		\\377}


String constants are delimited with double quote (") at both ends and must not 
cross a newline character.  They may include the special characters shown as 
special character constants.    


Another form of string constant is the\italic{ long string}.  Such constants 
are delimited at the start by a line containing only "//" and at the finish by 
a line starting with "\\\\".  Neither the initial nor final line or newline is 
part of the constant.


\{\{Future: Within a long string there may appear a marker expression 
delimited by <| ... |>.  The string expression will be evaluated and will 
replace the delimiters and the expression.  Evaluation will occur whenever the 
string constant is fetched from memory to be used in an expression; the result 
will be a read-write copy of the constant, which latter will remain for the 
next time it is accessed.\}\}


Whitespace characters are space, tab, newline, DEL, all other control 
characters, and all characters with the high order bit set.  They terminate 
reserved words, identifiers, and numbers.  They are included in strings; 
except for newline, which causes an error.  Otherwise they are ignored until a 
syntax error is detected.


When a syntax error occurs, the compilation skips immediately to the beginning 
of the next function, extend, or event.  Functions and extends are detected 
with the aid of unindented text.  To be recognized as a resynchronization 
point after a syntax error, a line must begin with an unindented element of 


	function-type  \bold{function } function-name


or an unindented element of


	\bold{extend}  inset-name-string


or the possibly-indented sequence 


	\bold{on}  event-keyword


\{\{Future: Curly braces will be used to delimit sequence constants.\}\}


\{\{Future: When an embedded Ness is saved, a first line is inserted--or 
revised if present-- to show the author, date, and checksum for the program. 
 The author and date provide help for a user deciding whether to execute the 
program.  The checksum encodes the syntax level so the system can 
automatically convert the program from one syntax to the next; it also 
provides some assurance that the program is indeed the one saved by the given 
author on the given date.\}\}


\section{
6. Recent Changes}


May, 1989.  Initial release to CMU campus.



\subsection{September-December 1989}


o   Removed the need for semicolons.  They are now optional everywhere.



o   Allowed multiple variable names in declarations.  One may now declare 
three marker variables with


\leftindent{marker a, b, c}


o   Allowed initialization in declarations.  On may now declare a variable and 
initialize it at the same time:


\leftindent{marker a := "This is the life" ~ punctuation}


Note that if a declaration has an initialization, it cannot be part of a list. 
 The initialization expression can be any expression as long as the variables 
mentioned have been given values before this statement (that is, either 
earlier in the same function definition or as global values.



o   Added \bold{exit function} statement form.   This form transfer control 
from this function back to the calling function.  It serves as would a 
"return" statement without an expression value.  (One cannot use just plain 
"return" any longer because there are no longer any semicolons.)



o   Added \bold{exit while}  statement form.  This form transfers control from 
its location to just after the "end while" of the smallest enclosing 
while-loop.



o   Changed the semantics of the five search functions: search, match, anyof, 
span, token.  The old semantics had two properties which seemed wrong in light 
of experience:  


\indent{The search area extended from the beginning of the first argument to 
the end of the base.


The value if the search failed was the beginning of the first argument.

}
The new semantics are these:


\indent{The search area extends only through the first argument.  However, if 
the first argument is empty the search area extends from the first argument to 
the end of the base.


The value for a failed search is the finish of the first argument.  }


The first of these rules means the search can be restricted to any portion of 
the document.  The second provides more flexibility in nesting search 
requests.  You now have two options for a search that is to have an area from 
a given point to the end of the base:


\indent{a) Have the first argument extend from the given point to the end of 
the base.  For failure the result is the end of the base.


b) Have the first argument be an empty marker.  For failure the result is the 
beginning of the search area (which is the end of the first argument).}


Notice that the second of these options gives exactly the original semantics. 
 Therefore: In converting a program from the old semantics to the new it is 
sufficient to wrap the first argument expression for each search-type function 
with start().



o   Replaced \bold{extend} "proctable:..."  with  \bold{extend} "view:..." and 
eliminated  \bold{on proc} "...".   In the old way, a function was inserted in 
the proctable with on proc and the .XXXinit file contained addkey or addmenu 
operations to refer to the proctable function.  After the revision, the key or 
menu binding is specified directly in the Ness file.  Example:


\leftindent{extend "view:textview"

	on keys "\\030\\014" 	-- control-X control-L

		wc_showcount()

	end keys

end extend}


This binds control-X control-L to count the words in the current selection.


o   Added the event type \bold{event} to reflect the addition of triggers to 
the C definition of objects.  The event specification


	\bold{on event} "trigger-name" ... \bold{end event}


must appear with the extend block for an inset which defines the trigger 
"trigger-name".  When the C code pulls the trigger, the code within the 
\bold{on event} ... \bold{end event} will be executed.

\enddata{text,2126960}
