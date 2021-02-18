/*
 * Copyright 1989 Dirk Grunwald
 * 
 * Permission to use, copy, modify, distribute, and sell this software
 * and its documentation for any purpose is hereby granted without fee,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Dirk Grunwald or M.I.T.
 * not be used in advertising or publicity pertaining to distribution of
 * the software without specific, written prior permission.  Dirk
 * Grunwald and M.I.T. makes no representations about the suitability of
 * this software for any purpose.  It is provided "as is" without express
 * or implied warranty.
 * 
 * DIRK GRUNWALD AND M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS
 * SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND
 * FITNESS, IN NO EVENT SHALL M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT
 * OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
 * OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE
 * OR PERFORMANCE OF THIS SOFTWARE.
 * 
 * Author:
 * 	Dr. Dirk Grunwald
 * 	Dept. of Computer Science
 * 	Campus Box 430
 * 	Univ. of Colorado, Boulder
 * 	Boulder, CO 80309
 * 
 * 	grunwald@colorado.edu
 * 	
 */ 

/*
 * Print an error message with an optional system error number, and
 * optionally quit.
 *
 */

#include <stdio.h>
#include <varargs.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/ShellP.h>

#ifdef X11R4
#  include <X11/Xaw/AsciiText.h>
#else
#  include <X11/AsciiText.h>
#endif

#include "xtex.h"
#include "widgets.h"

#ifdef __STDC__
  extern char* malloc(int);
  extern char* realloc(char *, int);
#else
  extern char* malloc();
  extern char* realloc();
#endif

#define ERROR_BUFFER_SIZE 2048
static char errorMessageBuffer[ ERROR_BUFFER_SIZE ];

void
DialogMessage(str, addNewLine)
     char *str;
     int addNewLine;
{
  
  if ( errorText ) {
    int len = strlen(str);
    char *p;
    char *currentString;

#ifdef	X11R4
    Arg argList[20];
    Cardinal args;

    XawTextPosition start;
    XawTextPosition end;
    XawTextBlock tblk;
#else
    XtTextPosition start;
    XtTextPosition end;
    XtTextBlock tblk;
#endif

    if ( len <= 0 )
      return;

    tblk.firstPos = 0;
    tblk.length = len;
    tblk.ptr = str;
    tblk.format = FMT8BIT;

    if ( len > 0 && addNewLine && str[len-1] != '\n' ) {
      str[len] = '\n';
      tblk.length = len+1;
    }

    str[tblk.length] = 0;

#ifdef X11R4
    args = 0;
    XtSetArg(argList[args], XtNstring, &currentString); args++;
    XtGetValues(errorText, argList, args);
#else
    currentString = errorMessageBuffer;
#endif

    start = strlen( currentString );

    while ( start + len > ERROR_BUFFER_SIZE ) {

      /* find first line and delete it */

      char *eol = strchr( currentString, '\n');

      if ( eol > 0 && eol < &currentString[ERROR_BUFFER_SIZE] ) {
	int howmuch = (eol+1) - currentString;
	int newSize = ERROR_BUFFER_SIZE - howmuch;
	bcopy( eol+1, currentString, newSize );
	currentString[ newSize + 1 ] = 0;
      }
      else {
	static void pushClearButton();
	pushClearButton(0,0,0);
      }
      start = strlen( currentString );
    }


    end = start;
#ifdef X11R4
    XawTextDisableRedisplay(errorText);
    XawTextSetInsertionPoint(errorText, start);
    XawTextReplace( errorText, start, end, &tblk );
    XawTextEnableRedisplay(errorText);
#else
    XtTextDisableRedisplay(errorText);
    XtTextSetInsertionPoint(errorText, start);
    XtTextSetLastPos( errorText, start );
    XtTextReplace( errorText, start, end, &tblk );
    XtTextEnableRedisplay(errorText);
#endif
  }
}

static void
pushClearButton(w, clientData,  callData)
Widget w;
caddr_t clientData;	/* unused */
caddr_t callData; /* unused */
{
  bzero( errorMessageBuffer , ERROR_BUFFER_SIZE );

#ifdef X11R4
  XawTextSetInsertionPoint(errorText, 0);
  XawTextEnableRedisplay(errorText);
#else
  XtTextSetInsertionPoint(errorText, 0);
  XtTextSetLastPos( errorText, 0 );
  XtTextEnableRedisplay(errorText);
#endif
} 

#ifdef CLEAR_BUTTON
static XtCallbackRec clearButtonCallBacks [] = {
  {pushClearButton,NULL},
  {NULL, NULL}
};
#endif

void
BuildErrorBox()
{
  Arg argList[20];
  Cardinal args;
  
  if (xtexResources.helpText) {
    strncpy(errorMessageBuffer, xtexResources.helpText, ERROR_BUFFER_SIZE-1);
  }
  
#ifdef CLEAR_BUTTON
  args = 0;
  XtSetArg(argList[args], XtNcallback, (XtArgVal)clearButtonCallBacks); args++;
  clearButton =
    XtCreateManagedWidget("dialogClear",
			  commandWidgetClass, topPane,
			  argList, args);
#endif
  
  args = 0;
#ifdef X11R4
  XtSetArg(argList[args], XtNeditType, (XtArgVal) XawtextEdit); args++;
  XtSetArg(argList[args], XtRAsciiType, XawAsciiString); args++;
#else
  XtSetArg(argList[args], XtNeditType, (XtArgVal) XttextEdit); args++;
  XtSetArg(argList[args], XtNtextOptions,
	   (XtArgVal) ( scrollOnOverflow | wordBreak )); args++;
#endif
  
  XtSetArg(argList[args], XtNstring, (XtArgVal) errorMessageBuffer); args++;
  XtSetArg(argList[args], XtNlength, (XtArgVal) ERROR_BUFFER_SIZE); args++;
  
  errorText = 
    XtCreateManagedWidget("dialogText",
#ifdef X11R4
			  asciiTextWidgetClass, topPane,
#else
			  asciiStringWidgetClass, topPane,
#endif
			  argList, args);
}


#ifdef lint

/* VARARGS3 ARGSUSED */
error(quit, e, fmt) int quit, e; char *fmt; {;}

/* VARARGS1 ARGSUSED */
panic(fmt) char *fmt; { exit(1); /* NOTREACHED */ }

#else lint

extern char *ProgName;
extern int errno;
extern char *sys_errlist[];
extern int sys_nerr;

#define ERROR_MSG_LEN 1024
static char ErrorMsgBuffer[ ERROR_MSG_LEN ];

error(va_alist)
     va_dcl
{
  va_list l;
  int quit, e;
  char *fmt;
  char *p;

  p = ErrorMsgBuffer;

  va_start(l);
  /* pick up the constant arguments: quit, errno, printf format */
  quit = va_arg(l, int);
  e = va_arg(l, int);
  if (e < 0)
    e = errno;

  fmt = va_arg(l, char *);

#if defined(VSPRINTF) || defined( vsprintf ) || defined(sun) || defined(hpux) || defined(ultrix)
  (void) vsprintf(p, fmt, l);
#else
  /*	Does someone know how to do this properly? */
  bzero(p, ERROR_MSG_LEN);

  {
    struct _iobuf ungodlyhack;
    ungodlyhack._cnt = ERROR_MSG_LEN;
    ungodlyhack._ptr = p;
    ungodlyhack._base = p;
    ungodlyhack._bufsiz = ERROR_MSG_LEN;
    ungodlyhack._flag = _IOSTRG;
    ungodlyhack._file = 255;
    _doprnt(fmt, l, &ungodlyhack);
  }
#endif

  va_end(l);

  DialogMessage( ErrorMsgBuffer, 1);
}

panic(va_alist)
     va_dcl
{
  va_list l;
  char *fmt;
  char *p;

  p = ErrorMsgBuffer;
  sprintf(p, "panic: ");
  p += strlen(p);

  va_start(l);
  /* pick up the constant arguments: quit, errno, printf format */
  fmt = va_arg(l, char *);

#if defined(sun) || defined(hpux)
  (void) vsprintf(p, fmt, l);
#else
  /*	Does someone know how to do this properly? */

  {
    struct _iobuf ungodlyhack;
    ungodlyhack._cnt = ERROR_MSG_LEN;
    ungodlyhack._ptr = p;
    ungodlyhack._base = p;
    ungodlyhack._bufsiz = ERROR_MSG_LEN;
    ungodlyhack._flag = _IOSTRG;
    ungodlyhack._file = 255;
    _doprnt(fmt, l, &ungodlyhack);
  }
#endif

  va_end(l);

  DialogMessage(ErrorMsgBuffer, 1);
  exit(1);
}

#endif /* lint */
