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
 *	   carlo@athena.mit.edu
 *
 * $Header: /mit/tax/Xless/RCS/init.c,v 1.5 89/10/11 20:33:42 carlo Exp $
 */

#include "xless.h"

extern XFontStruct *buttonfont;
extern XFontStruct *labelfont;
extern XFontStruct *textfont;
extern Cursor stdcur;

extern void PopupHelp();
extern void Search();
extern void Quit();
extern void CallEditor();
extern void Reload();
extern void ChangeFile();
extern void AskFile();
extern void CloseWindow();
extern void CDoSearch();

extern void MakeToolbox();
extern Widget MakeText();

Widget box;

/*
 *	Function Name: InitData
 *	Description:   This function reads a file and sets it up for
 *		       display in the asciiStringWidget.
 *	Arguments:     file - a file pointer to the file that we are opening.
 *	Returns:       data - a pointer to the data read from the file
 *			      and stored in dynamic memory.
 */

char *InitData(file)
FILE *file;
{
	char *data;		/* pointer to the data stored
				   in dynamic memory */
	struct stat fileinfo;	/* file information from fstat */
	int sz, newsz;		/* # of chars fread has written into data */
	char *tmp, *where;

/*
 * Get file size and allocate a chunk of memory for the file to be 
 * copied into.
 */

	if (file == stdin) {
	   if ((data = (char *) malloc(MEMBUF)) == NULL) {
	      fprintf(stderr,"xless: not enough memory available.\n");
	      exit(1);
	   }
	   fileinfo.st_size = 0;
	   where = data;
           while ((sz = fread(where,sizeof(char),MEMBUF,file)) != 0) {
                  fileinfo.st_size += sz;
		  /* remember old data in case of realloc failure */
		  tmp = data;
		  newsz = fileinfo.st_size+MEMBUF;
		  if ((data = (char *)realloc(data,(unsigned)newsz)) == NULL) {
	            fprintf(stderr,"xless: not enough memory available.\n");
		    data = tmp;
		    break;
		  }
		  where = data + fileinfo.st_size;
	   }
	   tmp = data;
	   newsz = fileinfo.st_size + 1;
	   if ((data = (char *)realloc(data,(unsigned)newsz)) == NULL) {
	     fprintf(stderr,"xless: not enough memory available.\n");
	     data = tmp;
	   }
	   *(data + fileinfo.st_size) = '\0';
	}
	else {
		if (fstat(fileno(file), &fileinfo)) {
		    fprintf(stderr,"xless: can't get file size.\n");
		    exit(1);
		}
		/*leave space for the NULL */
		data = (char *) malloc(fileinfo.st_size + 1);
		fread(data,sizeof(char),fileinfo.st_size,file); 
		*(data + fileinfo.st_size) = '\0';
		fclose(file);
	}

	return(data);

}

/*
 *	Function Name: MakeTitle
 *	Description:   This function create all the widgets necessary
 *		       to build the title label with the name of the 
 *		       file currently displayed.
 *	Arguments:     file - file name of the current file
 *		       outer - father widget. 
 *	Returns:       The label widget so created.
 */

Widget MakeTitle(file,outer)
char *file;
Widget outer;
{
  Arg xtargv[5];
  int xtargc = 0;
  char title[128];
  Widget lab;

	strcpy(title,"Current file is:  ");
	strcat(title,file);
	XtSetArg(xtargv[xtargc], XtNjustify,XtJustifyLeft ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNborderWidth,0 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNlabel, title ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	lab = XtCreateManagedWidget("title", labelWidgetClass, outer,
				xtargv, xtargc);
	return(lab);
}

/*
 *	Function Name: MakeMainFrame
 *	Description:   This function create the widget necessary
 *		       to build the frame cointaining toolbox and text.
 *	Arguments:     father - the father widget.
 *	Returns:       The mainframe widget so created.
 */

Widget MakeMainFrame(father)
Widget father;
{
  Arg xtargv[15];
  int xtargc;
  static XtCallbackRec        xtcalls[2];
  Widget main_frame;

	xtcalls[1].callback = NULL ;
	xtcalls[1].closure = (caddr_t) NULL ;
	xtargc = 0;
	main_frame  = XtCreateManagedWidget("mf", formWidgetClass, father,
                                xtargv, xtargc);

	return (main_frame);
}

/*
 *	Function Name: MakeToolbox
 *	Description:   This function create all the widgets necessary
 *		       to build the toolbox.
 *	Arguments:     father  - the father widget
 *		       cd      - pointer to the data structure containing
 *			         the application data.
 *		       new     - flag to indicate if is a new window
 *		       new_wdg - the shell widget that is the new window
 *		       fname   - the name of the file displaying in the window.
 *	Returns:       None.
 */

void MakeToolbox(father,cd,new,new_wdg,fname)
Widget father;
SearchData *cd;
int new;
Widget new_wdg;
char *fname;
{
  Arg xtargv[15];
  int xtargc;
  static XtCallbackRec        xtcalls[2];
  Widget tb_form,tb_view,search,but2,but3,but4,but5,but6,but7;
  CallData *callstruct;
  static String AccSearch = "#override Meta<Key>S:   set() notify() unset()\n";
  static String AccNext = "#override Meta<Key>N:   set() notify() unset()\n";
  static String AccReload = "#override Meta<Key>R:   set() notify() unset()\n";
  static String AccEdit = "#override Meta<Key>E:   set() notify() unset()\n";
  static String AccChange = "#override Meta<Key>C:   set() notify() unset()\n";
  static String AccWind = "#override Meta<Key>W:   set() notify() unset()\n";
  XtAccelerators Sacc, Nacc, Racc, Eacc, Cacc, Wacc;

        Sacc = XtParseAcceleratorTable(AccSearch);
        Nacc = XtParseAcceleratorTable(AccNext);
        Racc = XtParseAcceleratorTable(AccReload);
        Eacc = XtParseAcceleratorTable(AccEdit);
        Cacc = XtParseAcceleratorTable(AccChange);
        Wacc = XtParseAcceleratorTable(AccWind);

	/*
	 * Allocating memory for CloseWindow.
	 */
	if ((callstruct = (CallData *) malloc(sizeof(CallData))) == NULL){
          fprintf(stderr,"xless: could not allocate memory\n");
          exit(1);
        }

	xtcalls[1].callback = (XtCallbackProc) NULL ;
	xtcalls[1].closure = (caddr_t) NULL ;

	/*
	 * Creating the Viewport which contains the toolbox.
	 */
	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNallowVert, True ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNallowHoriz, True ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNallowResize, True ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNskipAdjust, False ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,1 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,cd->text ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	tb_view = XtCreateManagedWidget( "tb_view", viewportWidgetClass,
					father, xtargv, xtargc );

	xtargc = 0;
	tb_form = XtCreateManagedWidget("tb_form", formWidgetClass, tb_view,
                                xtargv, xtargc);

	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,stdcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,4 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,4 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	xtcalls[0].callback =  Search ;
	xtcalls[0].closure = (caddr_t)cd->popup;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	XtSetArg(xtargv[xtargc],XtNaccelerators,Sacc); xtargc++;
	search = XtCreateManagedWidget("Search", commandWidgetClass, tb_form,
                                xtargv, xtargc);
	XtInstallAccelerators(cd->text,search);

	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,stdcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,1 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,4 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,search ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	xtcalls[0].callback =  CDoSearch ;
	xtcalls[0].closure = (caddr_t)cd;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	XtSetArg(xtargv[xtargc],XtNaccelerators,Nacc); xtargc++;
	but2 = XtCreateManagedWidget("Search Next", commandWidgetClass, tb_form,
                                xtargv, xtargc);
	XtInstallAccelerators(cd->text,but2);

	xtargc = 0;
	if(fname == NULL)
		{XtSetArg(xtargv[xtargc], XtNsensitive,False ); xtargc++;}
	else
        	cd->file = fname;
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,stdcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,1 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,4 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,but2 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	xtcalls[0].callback =  CallEditor ;
	xtcalls[0].closure = (caddr_t)cd;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	XtSetArg(xtargv[xtargc],XtNaccelerators,Eacc); xtargc++;
	but3 = XtCreateManagedWidget("Editor", commandWidgetClass, tb_form,
                                xtargv, xtargc);
	XtInstallAccelerators(cd->text,but3);

	xtargc = 0;
	if(fname == NULL) {
		XtSetArg(xtargv[xtargc], XtNsensitive,False ); xtargc++;}
	else
        	cd->file = fname;
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,stdcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,1 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,4 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,but3 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	xtcalls[0].callback =  Reload ;
	xtcalls[0].closure = (caddr_t)cd;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	XtSetArg(xtargv[xtargc],XtNaccelerators,Racc); xtargc++;
	but4 = XtCreateManagedWidget("Reload", commandWidgetClass, tb_form,
                                xtargv, xtargc);
	XtInstallAccelerators(cd->text,but4);

	xtargc = 0;
	if(fname == NULL) {
		XtSetArg(xtargv[xtargc], XtNsensitive,False ); xtargc++;}
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,stdcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,1 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,4 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,but4 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	xtcalls[0].callback =  ChangeFile ;
	xtcalls[0].closure = (caddr_t)cd->change_pop;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	XtSetArg(xtargv[xtargc],XtNaccelerators,Cacc); xtargc++;
	but5 = XtCreateManagedWidget("Change file", commandWidgetClass, tb_form,
                                xtargv, xtargc);
	XtInstallAccelerators(cd->text,but5);

	xtargc = 0;
	XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNcursor,stdcur ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNvertDistance,1 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,4 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromVert,but5 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	xtcalls[0].callback =  AskFile ;
	xtcalls[0].closure = (caddr_t)cd->new_popup;
	XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	XtSetArg(xtargv[xtargc],XtNaccelerators,Wacc); xtargc++;
	but6 = XtCreateManagedWidget("New window", commandWidgetClass, tb_form,
                                xtargv, xtargc);
	XtInstallAccelerators(cd->text,but6);
	
	if (new){
	 callstruct->shl = new_wdg;
	 callstruct->mem = cd->memory;
	 xtargc = 0;
	 XtSetArg(xtargv[xtargc], XtNfont,buttonfont ); xtargc++;
	 XtSetArg(xtargv[xtargc], XtNcursor,stdcur ); xtargc++;
	 XtSetArg(xtargv[xtargc], XtNwidth,BUTTONWIDTH ); xtargc++;
	 XtSetArg(xtargv[xtargc], XtNvertDistance,1 ); xtargc++;
	 XtSetArg(xtargv[xtargc], XtNhorizDistance,4 ); xtargc++;
	 XtSetArg(xtargv[xtargc], XtNfromVert,but6 ); xtargc++;
	 XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	 XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	 XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	 xtcalls[0].callback =  CloseWindow ;
	 xtcalls[0].closure = (caddr_t)callstruct;
	 XtSetArg(xtargv[xtargc], XtNcallback, xtcalls); xtargc++;
	 but7 = XtCreateManagedWidget("Close window", commandWidgetClass,
				tb_form, xtargv, xtargc);
	}
}

/*
 *	Function Name: MakeText
 *	Description:   This function creates the text widget necessary
 *		       to display the data.
 *	Arguments:     father - the father widget
 *		       data   - pointer to data in memory.
 *	Returns:       tmp    - the text widget so created.
 */

Widget MakeText(father,data)
Widget father;
char *data;
{
	Widget txt;
	static Arg  xtargv[15];
	int xtargc = 0;

	XtSetArg(xtargv[xtargc], XtNstring, data ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtextOptions, scrollVertical ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNwidth, TEXTWIDTH ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNheight, TEXTHEIGHT ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNallowResize, True ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNskipAdjust, False ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfont, textfont ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNhorizDistance,1 ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNfromHoriz,NULL ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNtop,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNbottom,XtChainTop ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNright,XtChainRight ); xtargc++;
	XtSetArg(xtargv[xtargc], XtNleft,XtChainLeft ); xtargc++;
	txt = XtCreateManagedWidget( "text", asciiStringWidgetClass,
					father, xtargv, xtargc );

	return (txt);
}

/*
 *	Function Name: MakeCommandBox
 *	Description:   This function creates all the widgets necessary
 *		       to build the box containing the main command buttons.
 *	Arguments:     top   - the top widget
 *		       outer - the father widget.
 *	Returns:       None.
 */

void MakeCommandBox(top,outer)
Widget top;
Widget outer;
{
	static Arg  arg[10];
	static XtCallbackRec        callback[2];
	Widget viewport, quit, help;

	XtSetArg( arg[0], XtNallowVert, True );
	XtSetArg( arg[1], XtNallowResize, True );
	XtSetArg( arg[2], XtNskipAdjust, False );
	viewport = XtCreateManagedWidget( "viewport", viewportWidgetClass,
					outer, arg, 3 );

	XtSetArg( arg[0], XtNwidth, BOXWIDTH );
	XtSetArg( arg[1], XtNheight, BOXHEIGHT );
	box = XtCreateManagedWidget( "box", boxWidgetClass,
					viewport, arg, 2 );

	callback[0].callback = Quit;
	callback[0].closure = (caddr_t)top;
	XtSetArg( arg[0], XtNcallback, callback );
	XtSetArg( arg[1], XtNwidth, BUTTONWIDTH );
	XtSetArg( arg[3], XtNfont, buttonfont );
	XtSetArg( arg[4], XtNcursor, stdcur );
	quit = XtCreateManagedWidget("Quit", commandWidgetClass, box, arg, 5);

	callback[0].callback = PopupHelp;
	callback[0].closure = NULL;
	XtSetArg( arg[0], XtNcallback, callback );
	help = XtCreateManagedWidget("Help", commandWidgetClass, box, arg, 5);
}
