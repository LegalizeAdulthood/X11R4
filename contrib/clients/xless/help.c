/*
 * Copyright 1989 Massachusetts Institute of Technology
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of M.I.T. not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  M.I.T. makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL M.I.T.
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Carlo Lisa
 *	   MIT Project Athena
 *
 * $Header: /mit/tax/Xless/RCS/help.c,v 1.4 89/10/11 20:33:34 carlo Exp $
 */

#include "xless.h"
#include "XlessHelp.icon"

extern XlessResources resources;       /* The application resources */
extern Widget help_widget;             /* The help widget. */
extern XFontStruct *buttonfont;
extern XFontStruct *textfont;
extern void PopdownHelp();
extern Display *disp;

/*	Function Name: CreateHelp.
 *	Description: This function creates the help widget so that it will be
 *                   ready to be displayed.
 *	Arguments:
 *	Returns: FALSE if it could not create help window.
 */

Boolean
CreateHelp()
{
  struct stat fileinfo;		/* file information from fstat. */
  FILE * help_file;		/* The stream of the help file. */
  char * help_page;		/* The help text strored in memory. */
  int help_width;		/* The width of the help window. (default). */
  Arg arglist[10];		/* The arglist */
  Cardinal num_args;		/* The number of arguments. */
  Widget pane;			/* The Vpane, that will contain the help info.
				   */
  static XtCallbackRec Callbacks[] = {
    { PopdownHelp, NULL },
    { NULL, NULL },
  };
  char icon_name[MAX_INPUT];
  
  if (help_widget != NULL)	/* If we already have a help widget. 
				   then do not create one. */
    return(TRUE);

/* Open help_file and read it into memory. */

/*
 * Get file size and allocate a chunk of memory for the file to be 
 * copied into.
 */

  if( (help_file = fopen(resources.help_file, "r")) == NULL ) {
    fprintf(stderr,"Could not open help file: %s\nNO HELP WILL BE AVALIABLE.\n",
		resources.help_file);
    return(FALSE);
  }

  if ( stat(resources.help_file, &fileinfo) ) {
    fprintf(stderr,"Failure in fstat, NO HELP WILL BE AVALIABLE.\n");
    return(FALSE);
  }

  /* leave space for the NULL */
  help_page = (char *) malloc(fileinfo.st_size + 1);	

  if (help_page == NULL) {
    fprintf(stderr,
      "Could not allocate memory for help file, NO HELP WILL BE AVALIABLE.\n");
    return(FALSE);
  }

/*
 * Copy the file into memory. 
 */
 
  fread(help_page,sizeof(char),fileinfo.st_size,help_file); 
  fclose(help_file);
    
/* put NULL at end of buffer. */

  *(help_page + fileinfo.st_size) = '\0';

/*
 * Help file now loaded in to memory. Create widgets do display it. 
 */

  num_args = 0;
  XtSetArg(arglist[num_args], XtNiconPixmap, 
	XCreateBitmapFromData(disp, XRootWindow(disp,0), XlessHelp_bits,
		XlessHelp_width, XlessHelp_height));
  num_args++;

  strcpy(icon_name, "xless: help");
  XtSetArg(arglist[num_args], XtNiconName, icon_name);
  num_args++;

  XtSetArg(arglist[num_args], XtNallowShellResize, TRUE);
  num_args++;

  help_widget = XtCreateApplicationShell("help", applicationShellWidgetClass, 
				   arglist, num_args);

  num_args = 0;
  help_width = DisplayWidth(XtDisplay(help_widget), 
			    DefaultScreen(XtDisplay(help_widget)));
  help_width /= 2;
  XtSetArg(arglist[num_args], XtNwidth, help_width);
  num_args++;

  pane = XtCreateWidget("Help_VPaned",vPanedWidgetClass,help_widget,
			arglist,num_args);

  num_args = 0;
  XtSetArg(arglist[num_args], XtNfont,textfont );
  num_args++;
  XtSetArg(arglist[num_args], XtNborderWidth, 0);
  num_args++;
  XtSetArg(arglist[num_args], XtNstring, help_page);
  num_args++;
  XtSetArg(arglist[num_args], XtNtextOptions, scrollVertical);
  num_args++;
  /* make the text shown a square box. */
  XtSetArg(arglist[num_args], XtNheight, help_width);
  num_args++;
  

  (void) XtCreateManagedWidget("Help_Text",asciiStringWidgetClass, pane,
			       arglist, num_args);

  num_args = 0;
  XtSetArg(arglist[num_args], XtNfont,buttonfont );
  num_args++;
  XtSetArg(arglist[num_args], XtNborderWidth, 0);
  num_args++;
  XtSetArg(arglist[num_args], XtNlabel, "Done With Help");
  num_args++;
  XtSetArg(arglist[num_args], XtNcallback, Callbacks);
  num_args++;

  (void) XtCreateManagedWidget("Help_Quit",commandWidgetClass, pane,
			       arglist, num_args);

  XtManageChild(pane);
  XtRealizeWidget(help_widget);

  return(TRUE);
}

/*	Function Name: PopdownHelp
 *	Description: This function pops down the help widget.
 *	Arguments: w - the widget we are calling back from. 
 *                 number - (closure) the number to switch on.
 *                 junk - (call data) not used.
 *	Returns: none.
 */

/* ARGSUSED */

void
PopdownHelp(w,number,junk)
Widget w;
caddr_t number,junk;
{
  XtPopdown(help_widget);
}

/*	Function Name: PopupHelp
 *	Description: This function pops up the help widget, unless no
 *                   help could be found.
 *	Arguments: w - the widget we are calling back from. 
 *                 number - (closure) the number to switch on.
 *                 junk - (call data) not used.
 *	Returns: none.
 */

/* ARGSUSED */

void
PopupHelp(w,number,junk)
Widget w;
caddr_t number,junk;
{
  if (CreateHelp())
    XtPopup(help_widget,XtGrabNone);
}
