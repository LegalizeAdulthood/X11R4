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

#include "xtex.h"
#include "mark.h"
#include <string.h>

char *ProgName, *ClassName;

extern void BuildTopLevelWidget();

xtexResourceStruct xtexResources;

#define Offset(field) (XtOffset(xtexResourceStruct *, field))

static XtResource resources[] = {
  {"print", XtCString, XtRString, sizeof(char *),
     Offset(printProgram), XtRString, "lpr -d"},

  {"tfmPath", XtCString, XtRString, sizeof(char *),
     Offset(tfmPath), XtRString, ".:/usr/local/lib/tex82/fonts:/usr/local/lib/tex/fonts:/usr/lib/tex82/fonts:/usr/lib/tex/fonts"},

  {"mag", XtCString, XtRInt, sizeof(int),
     Offset(userMag), XtRString, "500"},

  {"topOffset", XtCString, XtRString, sizeof(char *),
     Offset(topOffsetString), XtRString, "1"},

  {"voffset", XtCString, XtRString, sizeof(char *),
     Offset(topOffsetString), XtRString, "1"},

  {"leftOffset", XtCString, XtRString, sizeof(char *),
     Offset(leftOffsetString), XtRString, "1"},

  {"hoffset", XtCString, XtRString, sizeof(char *),
     Offset(leftOffsetString), XtRString, "1"},

  {"paperWidth", XtCString, XtRString, sizeof(char *),
     Offset(paperWidthString), XtRString, "8.5"},

  {"paperHeight", XtCString, XtRString, sizeof(char *),
     Offset(paperHeightString), XtRString, "11"},

  {"smallMag", XtCString, XtRInt, sizeof(int),
     Offset(smallMag), XtRString, "333"},

  {"largeMag", XtCString, XtRInt, sizeof(int),
     Offset(largeMag), XtRString, "500"},

  {"dpiHoriz", XtCString, XtRString, sizeof(char *),
     Offset(dpiHorizString), XtRString, "300"},

  {"dpiVert", XtCString, XtRString, sizeof(char *),
     Offset(dpiVertString), XtRString, "-1"},

  {"debug", XtCString, XtRBool, sizeof(Bool),
     Offset(debug), XtRString, "True"},

  {"pageMaxWd", XtCString, XtRInt, sizeof(int),
     Offset(pageMaxWd), XtRString, "0"},

  {"pageMaxHt", XtCString, XtRInt, sizeof(int),
     Offset(pageMaxHt), XtRString, "0"},

  {"autoSize", XtCString, XtRBool, sizeof(Bool),
     Offset(autoSize), XtRString, "True"},

  {"useButtons", XtCString, XtRBool, sizeof(Bool),
     Offset(useButtons), XtRString, "True"},

/*
 *	the following options overide various globals, but aren't set
 *	via command line switches.
 */
  {"helpText", "HelpText",
     XtRString, sizeof(String),
     Offset(helpText),
     XtRString, "Warning -- Someone forgot to install APPDEFAULTSDIR/Xtex;\n\
(You can also set the XAPPLRESDIR environment variable to point to it)"}
};

static XrmOptionDescRec Options[] = {
  {"-print", "*.print", XrmoptionSepArg, (caddr_t) NULL},
  {"-mag", "*.mag", XrmoptionSepArg, (caddr_t) NULL},
  {"-topOffset", "*.topOffset", XrmoptionSepArg, (caddr_t) NULL},
  {"-leftOffset", "*.leftOffset", XrmoptionSepArg, (caddr_t) NULL},
  {"-paperWidth", "*.paperWidth", XrmoptionSepArg, (caddr_t) NULL},
  {"-paperHeight", "*.paperHeight", XrmoptionSepArg, (caddr_t) NULL},
  {"-smallMag", "*.smallMag", XrmoptionSepArg, (caddr_t) NULL},
  {"-largeMag", "*.largeMag", XrmoptionSepArg, (caddr_t) NULL},
  {"-dpi", "*.dpiHoriz", XrmoptionSepArg, (caddr_t) NULL},
  {"-dpiVert", "*.dpiVert", XrmoptionSepArg, (caddr_t) NULL},
  {"-debug", "*.debug", XrmoptionSepArg, (caddr_t) NULL},
  {"-pageMaxWd", "*.pageMaxWd", XrmoptionSepArg, (caddr_t) NULL},
  {"-pageMaxHt", "*.pageMaxHt", XrmoptionSepArg, (caddr_t) NULL},
  {"-autoSize", "*.autoSize", XrmoptionSepArg, (caddr_t) NULL},
  {"-useButtons", "*.useButtons", XrmoptionSepArg, (caddr_t) NULL}
};

int Argc;
char **Argv;

Widget TopLevelWidget;

void 
main(argc,argv)
char ** argv;
int argc;
{

  Argc = argc;
  Argv = argv;

  ClassName = "Xtex";

  if( (ProgName = index(argv[0], '/')) == NULL )
	  ProgName = argv[0];

  TopLevelWidget = XtInitialize(ProgName, ClassName,
				Options, XtNumber(Options),
				&Argc, Argv);

  XtGetApplicationResources(TopLevelWidget,
			    (caddr_t) &xtexResources, 
			    resources, XtNumber(resources),
			    NULL, (Cardinal) 0);

  TeXFileInstallActions();
  TeXPageInstallActions();
  TeXMarkInstallActions();

  BuildTopLevelWidget(TopLevelWidget);
  XtRealizeWidget(TopLevelWidget);

  if ( Argc > 1 ) {
    register char *p;

    /* set the directory name */
    setDirectory( Argv[1] );

    /* peel off directory names & set the file name with the remainder */

    p = Argv[1];
    p += strlen(Argv[1]) - 1;
    while( p != Argv[1] && *p != '/' )
      --p;
    if( *p == '/' ) p++;
    setFileName( p );
  }
  else {
    setDirectory(".");
  }

  if ( Argc > 2 ) {
    error(0,0,"Additional arguments ignored");
  }

  XtMainLoop();
  exit(0);
}

