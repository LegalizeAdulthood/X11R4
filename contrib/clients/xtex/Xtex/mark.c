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
 *	Handle printing of marked pages
 */

#include <stdio.h>
#include <X11/Xos.h>  
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

#include <assert.h>
#include <sys/wait.h>

#include "dvi-simple.h"
#include "xtex.h"
#include "mark.h"
#include "page.h"
#include "widgets.h"

typedef struct MarkStackStruct MarkStack;

struct MarkStackStruct {
  Bool *mark;
  short pages;
  MarkStack *next;
};

MarkStack *CurrentMarks = 0;


/*
 *	Widgets for menu of marked pages
 */
typedef struct {
    int *listOffset;
    String *listItems;
    String listStorage;
    int stringSize;
    int stringLast;
} MarkMenu;

MarkMenu TheMarkMenu = {0,0,0,0,0};

/* update the displayed marks & cause the menu to be refreshed */

static void
TeXMarkMenuRefresh()
{
  int page;

  for ( page = 0; page < DviTotalPages; page ++ ) {
    char *item = TheMarkMenu.listItems[ page ];

    *(item) = (CurrentMarks -> mark [ page ]) ? '*' : ' ';
  }

#ifdef X11R4
  XawListChange( markMenuList, TheMarkMenu.listItems, -1, -1, True );
#else
  XtListChange( markMenuList, TheMarkMenu.listItems, -1, -1, True );
#endif
}

void
TeXMarkMenuSelect(w, client_data, call_data)
Widget w;
caddr_t client_data;
caddr_t call_data;
{
#ifdef X11R4
  XawListReturnStruct *item = (XawListReturnStruct *) call_data;
#else
  XtListReturnStruct *item = (XtListReturnStruct *) call_data;
#endif

  if ( item != 0 ) {
#ifdef X11R4
    int page = item -> list_index;
#else
    int page = item -> index;
#endif
    TeXMark( markToggle, page);
  }
}

void BuildMarkMenuPopUp()
{
  if ( markMenuList && DviTotalPages > 0 ) {
    int mallocSize;
    int page;
    
    /* Build the list - one item for each page, one null terminator
     * We don't know what the various count0 fields enclose, so we
     * make a guess for the string space needed and realloc it as
     * needed.
     */
    if ( TheMarkMenu.stringSize != 0 ) {
      XtFree( TheMarkMenu.listStorage );
      TheMarkMenu.listStorage = 0;
    }
    
    TheMarkMenu.stringSize = DviTotalPages * 6;
    TheMarkMenu.stringLast = TheMarkMenu.stringSize;
    TheMarkMenu.listStorage = XtMalloc( TheMarkMenu.stringSize );
    
    mallocSize = ( sizeof(String) * (DviTotalPages + 2) );
    TheMarkMenu.listItems = (String *) XtMalloc( mallocSize );
    bzero(TheMarkMenu.listItems, mallocSize);
    
    mallocSize = ( sizeof(int) * (DviTotalPages + 2) );
    TheMarkMenu.listOffset = (int *) XtMalloc( mallocSize );
    bzero(TheMarkMenu.listItems, mallocSize);
    
    for (page = 0; page < DviTotalPages; page++) {
      char thisPage[128];
      int len;
      
      sprintf(thisPage," %d", DviCount[0][page]);

      len = strlen(thisPage) + 1;
      if ( (TheMarkMenu.stringSize - TheMarkMenu.stringLast) <= len ) {
	TheMarkMenu.listStorage
	  = XtRealloc( TheMarkMenu.listStorage, TheMarkMenu.stringSize * 2 );
      }
      
      TheMarkMenu.listOffset[page] = TheMarkMenu.stringLast;
      bcopy(thisPage,
	    TheMarkMenu.listStorage + TheMarkMenu.listOffset[page], len);
      TheMarkMenu.stringLast += len;
    }
    
    /*
     * Convert offsets to actual addresses
     */
    for (page = 0; page < DviTotalPages; page++) {
      TheMarkMenu.listItems[ page ] =
	TheMarkMenu.listStorage + TheMarkMenu.listOffset[ page ];
    }
    
    TeXMarkMenuRefresh();
    XtMapWidget(markMenuViewPort);
    TeXMarkMenuRefresh();
  }
}


void
TeXMarkPush()
{
  MarkStack *stk = (MarkStack *) malloc( sizeof(MarkStack) );
  int len = DviTotalPages * sizeof(Bool);
  stk -> mark = (Bool *) malloc( len );

  assert(stk && stk -> mark);

  if ( (int) False == 0 ) {
    bzero(stk -> mark, len);
  }
  else {
    int page;
    for ( page = 0; page< DviTotalPages; page++ ) {
      CurrentMarks -> mark [ page ] = False;
    }
  }
  stk -> next = CurrentMarks;
  CurrentMarks = stk;
}

void
TeXMarkPop()
{
  if ( CurrentMarks != 0 ) {
    MarkStack *stk = CurrentMarks;
    CurrentMarks = stk -> next;
    free( stk -> mark );
    free( stk );
  }
}

static void
insureMarks()
{
  if ( CurrentMarks == 0 ) TeXMarkPush();
}

void
TeXMarkNewFile()
{
  int page;

  while (CurrentMarks != 0) {
    TeXMarkPop();
  }

  if ( DviFile != 0 ) {
    TeXMarkPush();
    BuildMarkMenuPopUp();
  }
}

/*
 *	Simply return the state
 */
Bool
TeXMarkState(page)
int page;
{
  if ( page >=0 && page < DviTotalPages ) {
    insureMarks();
    return( CurrentMarks -> mark[  page  ] );
  }
  else {
    return( 0 );
  }
}

Bool
TeXMark( md, page )
MarkDirective md;
int page;
{
  if ( page >=0 && page < DviTotalPages ) {
    insureMarks();
    
    switch ( md ) {
    case markSet:
      CurrentMarks -> mark[  page  ]  = True;
      break;
    case markClear:
      CurrentMarks -> mark[  page  ]  = False;
      break;
    case markToggle:
      CurrentMarks -> mark[  page  ]  = ~ CurrentMarks -> mark[ page ];
      break;
    default:
      error(0,0,"Unknown mark directive in TeXMark: %d\n", (int) md);
    }
    TeXMarkMenuRefresh();
    return( CurrentMarks -> mark[  page  ] );
  }
  else {
    return( False );
  }
}

void
TeXMarkAll( md )
MarkDirective md;
{
  int page;

  insureMarks();
  
  switch ( md ) {
    
  case markSet:
    for (page = 0; page < DviTotalPages; page++) {
      CurrentMarks -> mark[  page  ] = True;
    }
    break;
    
  case markClear:
    for (page = 0; page < DviTotalPages; page++) {
      CurrentMarks -> mark[  page  ] = False;
    }
    break;
    
  case markToggle:
    for (page = 0; page < DviTotalPages; page++) {
      CurrentMarks -> mark[  page  ] = ~CurrentMarks -> mark[ page ];
    }
    break;
    
  default:
    error(0,0,"Unknown mark directive in TeXMark: %d\n", (int) md);
  }
  
  TeXMarkMenuRefresh();
}

/*
 *	Do a system call, while handling X events. This is used to print
 *	marked pages while still doing other X things.
 */

static void
XtSystemRead( f )
caddr_t f;
{
  int fd = (int) f;
  char buffer[1024];
  int bytes;
  bytes = read(fd, buffer, 1024);
  if ( bytes > 0 ) {
    buffer[bytes] = 0;
    DialogMessage(buffer,0);
  }
}

void
XtSystem(str)
char *str;
{
  int fildes[2];
  int child;

  if ( pipe(fildes) < 0 ) {
    perror("pipe");
    exit(1);
  }

  if (child = fork()) {
    /* ye olde parent, who will read from fildes[0] */

    int filemask = (1 << fildes[0]);

    XtAppContext context = 
     XtWidgetToApplicationContext( TopLevelWidget );
    
    XtInputId inputId = XtAppAddInput(context, fildes[0], XtInputReadMask,
		   XtSystemRead, (caddr_t) fildes[0]);
		   
    for(;;) {

      int pid;
      union wait status;

      XtAppProcessEvent(context, XtIMXEvent | XtIMTimer | XtIMAlternateInput);

      pid = wait3(&status, WNOHANG, 0);
      if ( pid == child ) {
	break;
      }
    }
    XtRemoveInput( inputId );
    close(fildes[0]);
  }
  else {
    /* ye newe child, who will write to fildes[1] */
    dup2(fildes[1],1);
    dup2(fildes[1],2);
    fprintf(stderr,"[ exec ]\n%s\n", str);
    system( str );

    /* terminate the line */

    DialogMessage("\n",0);
    close(1);
    close(2);
    exit(0);
  }
}

/* Print some pages. The mark selection dictates what pages get printed */
void
TeXMarkPrintUsing(pd, dvips)
char *dvips;
PrintDirective pd;
{
  int cnt;
  int i;
  char buf[20];

  char printCommand[1024];
  char tmpFileName[1024];
  
  char *fileToPrint;
  int deletePrintedFile;
    

  if ( pd == printAll ) {
    fileToPrint = DviFileName;
    deletePrintedFile = 0;
  }
  else {
    
    int dviSelectBufferLength = 2048;
    char dviSelectBuffer[ 2048 ];

#ifdef sun
    sprintf(tmpFileName,"%s.dvi", (char *) tempnam("/tmp","_texx"));
#else
    sprintf(tmpFileName,"%s.dvi", (char *) tmpnam(0));
#endif

    fileToPrint = tmpFileName;
    deletePrintedFile = 1;
    
    /* Now run dviselect to extract the pages */
    
    sprintf(dviSelectBuffer, "dviselect -i %s -o %s ",
	    DviFileName, fileToPrint);
    
    cnt = 0;
    for (i = 0; i < DviTotalPages; i++) {
      if (   (CurrentMarks -> mark[  i  ] && pd == printMarked )
	  || (!CurrentMarks -> mark[  i  ] && pd == printUnmarked )) {
	sprintf(buf, " =%d", i + 1);
	strcat(dviSelectBuffer, buf);
	cnt++;
      }
    }
    
    if (cnt != 0) {
      error(0,0, dviSelectBuffer);
      XtSystem(dviSelectBuffer);
    }
    else {
      unlink( fileToPrint );
      return;
    }
  }
  
  /* Now print pages */
  
  sprintf(printCommand, "%s %s ", dvips, fileToPrint);
  XtSystem(printCommand);
  
  if ( deletePrintedFile ) {
    unlink( fileToPrint );
  }
}

void
TeXMarkPrint(pd)
PrintDirective pd;
{
  char *dvips = xtexResources.printProgram;

  if ( dvips == 0 ) {
    extern char *getenv();
    dvips = getenv("DVIPRINT");
    if (dvips == NULL) {
      dvips = "lpr -d ";	/* a good guess on BSD systems */
    }
    error(0,0, "Using default ``%s'' to print\n", dvips);
  }

  TeXMarkPrintUsing( pd, dvips );
}

/*********************************************************/
/*	Action Interface				 */
/*********************************************************/


static void actionPrintUsing(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  char *directive = (char *) params[0];
  PrintDirective pd;

  if (*num_params < 1 ) {
    return;
  }
  
  if ( strcmp(directive,"all") == 0 ) {
    pd = printAll;
  }
  else if (strcmp(directive,"set") == 0) {
    pd = printMarked;
  }
  else if (strcmp(directive,"clear") == 0) {
    pd = printUnmarked;
  }
  else {
    return;
  }

  if (*num_params == 1) {
    TeXMarkPrint(pd);
  }
  else if (*num_params >= 2) {
    int totalLen;
    int i;
    char *printCommand;

    totalLen = 0;
    for ( i = 1; i < *num_params; i++ ) {
      if ( params[i] ) {
	totalLen += strlen( params[i] );
      }
      else {
	break;
      }
    }

    totalLen += (*num_params = 10);

    printCommand = (char *) malloc( sizeof(char) * totalLen + 1);
    bzero(printCommand, totalLen + 1);

    for ( i = 1; i < *num_params; i++ ) {
      if ( params[i] ) {
	strncat(printCommand, params[i], totalLen);
	strncat(printCommand, " ", totalLen);
      }
      else {
	break;
      }
    }
    TeXMarkPrintUsing(pd, printCommand);

    free( printCommand );
  }
}

static void actionMarkPage(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  MarkDirective d;

  d = markNone;

  if (*num_params < 1) {
    error(0,0,"Must specify set/clear/toggle to texx-mark-all");
  }
  else if ( strcmp(params[0],"set") == 0 ) {
    d = markSet;
  }
  else if ( strcmp(params[0],"clear") == 0 ) {
    d = markClear;
  }
  else if ( strcmp(params[0],"toggle") == 0 ) {
    d = markToggle;
  }
  else {
    error(0,0,"Must specify set/clear/toggle to texx-mark-all");
  }

  if (markNone) {
    return;
  }
  else {
    TeXPage *tp = TeXPageLocate(w);
    int num = tp -> pageNumber;
    TeXMark( d, num );
  }
} 

static void actionMarkAll(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  MarkDirective d;

  d = markNone;

  if (*num_params < 1) {
    error(0,0,"Must specify set/clear/toggle to texx-mark-all");
  }
  else if ( strcmp(params[0],"set") == 0 ) {
    d = markSet;
  }
  else if ( strcmp(params[0],"clear") == 0 ) {
    d = markClear;
  }
  else if ( strcmp(params[0],"toggle") == 0 ) {
    d = markToggle;
  }
  else {
    error(0,0,"Must specify set/clear/toggle to texx-mark-all");
  }

  if  ( d != markNone ) {
    TeXMarkAll( d );
  }
}

static void actionMarkPush(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXMarkPush();
} 

static void actionMarkPop(w, event, params, num_params)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *num_params;
{
  TeXMarkPop();
} 

static XtActionsRec texxMarkActionsTable[] = {
  {"xtex-mark-all",  actionMarkAll },
  {"xtex-mark-page",  actionMarkPage },
  {"xtex-mark-push",  actionMarkPush },
  {"xtex-mark-pop",  actionMarkPop },
  {"xtex-print-using",  actionPrintUsing }
};

void
TeXMarkInstallActions()
{
  XtAddActions(texxMarkActionsTable, XtNumber(texxMarkActionsTable));
}

