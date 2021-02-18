/*
 * xless - X Window System file browser.
 *
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
 *	   carlo@athena.mit.edu
 *
 * Date: October 11, 1989
 *
 * $Header: /mit/tax/Xless/RCS/main.c,v 1.3 89/10/11 20:33:51 carlo Exp $
 */

#include "xless.h"
#include "XlessTop.icon"

extern char *InitData();
extern SearchData *MakeDialog();
extern Widget MakeTitle();
extern void OpenFonts();
extern void MakeToolbox();
extern Widget MakeText();
extern Widget MakeMainFrame();

Widget help_widget;
Display *disp;
char sel[MAX_INPUT];
char new_sel[MAX_INPUT];
char new_sel_file[MAX_INPUT];

XlessResources resources;

#define Offset(field) (XtOffset(XlessResources *, field))

static XtResource priv_resources[] = {
  {"helpFile", XtCFile, XtRString, sizeof(char *),
     Offset(help_file), XtRString, HELPFILE},
  {"standardCur", XtCCursor, XtRCursor, sizeof(Cursor),
     Offset(cursors.top), XtRString, STANDARDCUR},
  {"dialogCur", XtCCursor, XtRCursor, sizeof(Cursor),
     Offset(cursors.dialog), XtRString, STANDARDCUR},
  {"standardFont", XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(fonts.standard), XtRString, STANDARDFONT},
  {"textFont", XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(fonts.text), XtRString, TEXTFONT},
  {"labelFont", XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(fonts.label), XtRString, LABELFONT},
  {"buttonFont", XtCFont, XtRFontStruct, sizeof(XFontStruct *),
     Offset(fonts.button), XtRString, BUTTONFONT}
};

void main(argc,argv)
    unsigned int argc;
    char **argv;
{
    static Arg	arg[10];
    FILE * file;
    char which_file[128];
    char icon_name[MAX_INPUT];
    Widget text, main_frame;
    char *memory;
    Widget top, outer, lab;
    SearchData *dialog_struct;
    char *fname = NULL;
    int n;

	top = XtInitialize(NULL, "Xless", NULL, (unsigned int)0,
				(Cardinal *)&argc, argv);

	disp = XtDisplay(top);

	XtGetApplicationResources( (Widget) top, (caddr_t) &resources,
		priv_resources, XtNumber(priv_resources), NULL, (Cardinal) 0);

	n = 0;
	XtSetArg(arg[n], XtNiconPixmap,
		XCreateBitmapFromData(disp, XRootWindow(disp,0),
			XlessTop_bits, XlessTop_width,
			XlessTop_height)); n++;
	XtSetValues(top, arg, n);
		
	if (argc > 1) {
		if (((strcmp(argv[1],"-help")) == 0) ||
		    ((strcmp(argv[1],"-h")) == 0)) {
			fprintf(stdout,"usage:\txless filename\n\t(xless also takes input from stdin)\n");
			exit(0);
		}
		if ( (file = fopen(argv[1],"r")) == NULL) {
		   fprintf(stderr,"xless: could not open file: %s\n",argv[1]);
		   exit(1);
		}
		strcpy(which_file,argv[1]);
		fname = which_file;
	}
	else {
		file = stdin;
		/*
		 * Not really necessary to call this an error,
		 * but if the control terminal (for commands)
		 * and the input file (for data) are the same,
		 * we get weird results at best.
		 */
		if (isatty(fileno(file))) {
		  fprintf(stderr,"xless: can't take input from terminal\n");
		  exit(1);
		}
		strcpy(which_file,"stdin");
	}

	strcpy(icon_name, which_file);
	n = 0;
	XtSetArg(arg[n], XtNiconName, icon_name); n++;
	XtSetValues(top, arg, n);

	memory = InitData(file);

	outer = XtCreateManagedWidget("outer",vPanedWidgetClass,top,NULL,0);

	CheckFonts();

	lab = MakeTitle(which_file,outer);

	main_frame = MakeMainFrame(outer);

	text = MakeText(main_frame,memory);

	dialog_struct = MakeDialog(top);
	dialog_struct->text = text;
	dialog_struct->memory = memory;
	dialog_struct->lab = lab;

	MakeToolbox(main_frame,dialog_struct,0,NULL,fname);

	MakeCommandBox(top,outer);

	XtRealizeWidget (top);
	XtMainLoop ();
}
