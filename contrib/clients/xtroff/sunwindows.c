/*
 *	SunTroff -  A program to display the output of Device Independent
 *		Troff as a window on a Sun Workstation.
 *
 *	Windows.c - This file contains the user interface and most of the
 *		high level window manager code.  This file provides a page
 *		size bitmap for the actual drawing routines to work on.
 *
 *	Authors - The original version of this program was written by
 *			Richard L. Hyde (Purdue)
 *			David Slattengren (Berkeley)
 *		It was nearly all rewritten, cleaned up and a more elegant
 *		user interface installed by
 *			Malcolm Slaney (Schlumberger Palo Alto Research)
 *
 *	Legalese -  This command was developed as an independent project
 *		to satisfy a need of the author.  This program may contain
 *		bugs and the user is cautioned to independently verify that
 *		the program is suitable for the user's intended purpose.
 *		The program is made available on an ``as is'' basis with
 *		all faults and without any implied or expressed warranties
 *		or support from either the author, Malcolm Slaney, or
 *		Schlumberger Palo Alto Research Laboratory.
 *
 *		I am putting this program in the public domain.  You are
 *		free to use it as you wish.  In return I ask two things.
 *		First, that you do not remove the names of the authors
 *		from this work.  Secondly, if you make changes or
 *		improvements to this program that you pass these back to
 *		the author so that everybody can benefit from the
 *		improvements.
 *
 *					Malcolm Slaney  (December 1986)
 *					Schlumberger Palo Alto Research
 *					3340 Hillview Avenue
 *					Palo Alto, CA 94304
 *					(415) 496-4669
 *					spar!malcolm@decwrl.dec.com
 *					malcolm@ecn.purdue.edu
 *					malcolm@spar.slb.com (Someday)
 */

#ifdef SUNTOOLS
#include	<suntool/sunview.h>
#include	<suntool/panel.h>
#include	<suntool/canvas.h>
#include	<suntool/scrollbar.h>
#include	<signal.h>
#include	<sys/file.h>
#include	"suntroff.h"

Frame	BaseFrame;		/* Top Level Window Frame */
Canvas	DrawingCanvas;		/* Main Drawing Window for Text */
Menu	MainMenu;		/* Top Level User Menu */
Scrollbar HorzScroll;		/* Horizontal Scroll Bar */
Scrollbar VertScroll;		/* Vertical Scroll Bar */
struct pixrect *PagePixRect;	/* Full Page Pix Rect for Drawing Page */
Cursor	OriginalCursor;
int	ViewLeft = 0, ViewTop = 0;	/* Page Offset for display */
int	OriginalX = -1;
int	OriginalY = -1;
int	Scrollbars = 1;		/* Does User want scrollbars????? */

static int	CurrentPage = 1;/* Current Page Number Being Displayed */
char	FileName[BUFSIZ];	/* File containing ditroff output */
char	TempFileName[BUFSIZ];	/* Temp file that can be deleted. */
char	CommandString[BUFSIZ];	/* String pointer to ditroff command */
char	*ActualFileName;	/* File that buffers ditroff output */
int	PageRequest = 0;	/* Partially read page number request */
int	CommandMode = 0;	/* Taking input from file or command line */
int	ErrorsPending = 0;	/* Set if errors are pending and need to be
				 * displayed.  Used in case tool is iconic
				 * when error message is needed.  This boolean
				 * variable is checked at redisplay time.
				 * This was done at the suggestion of
				 * davy@ecn.purdue.edu.......11/28/86
				 */

Panel	CommandPanel;
Frame	CommandFrame;
Panel_item InputItem;
Panel_item SearchItem;
Panel_item PageItem;
Panel_item PrinterItem;
Panel	ErrorPanel = 0;
Frame	ErrorFrame = 0;

int     SUNRES = 120;

main(argc, argv)
int	argc;
char	**argv;
{
	int	RepaintCanvas(), CanvasInput();
	Pixwin	*PixWin;

	BaseFrame = window_create(NULL,FRAME,
				  FRAME_ARGC_PTR_ARGV, &argc, argv,
				  FRAME_LABEL,"Generic Previewer 0.0",
				  WIN_ERROR_MSG,"Can't create window.",
				  FRAME_INHERIT_COLORS, 1, 
#if	FULLPAGE > 0
				  WIN_WIDTH, PAGE_PIXEL_WIDTH, 
#endif	FULLPAGE
				  0);

	CreatePopUpPanel();

	DrawingCanvas = window_create(BaseFrame,CANVAS,
				      CANVAS_REPAINT_PROC, RepaintCanvas,
				      WIN_EVENT_PROC, CanvasInput,
				      0);

	PagePixRect = mem_create(PAGE_PIXEL_WIDTH,PAGE_PIXEL_HEIGHT,1);
	if (!PagePixRect){
		fprintf(stderr,"Can't create page pixrect.\n");
		exit(1);
	}


	window_set(DrawingCanvas,WIN_CONSUME_PICK_EVENTS,
		   WIN_IN_TRANSIT_EVENTS,
		   WIN_MOUSE_BUTTONS,WIN_ASCII_EVENTS,0,
		   0);

	MainMenu = menu_create(MENU_STRINGS,"Show Status", "Rerasterize",
			       "Print", "Print Page", "Previous Page",
			       "Next Page", "Quit",0);

	PixWin = canvas_pixwin(DrawingCanvas);
#ifdef	WINDOWDEBUG	
	pr_vector(PagePixRect,0,0,
		              PAGE_PIXEL_WIDTH,PAGE_PIXEL_HEIGHT,PIX_SRC,1);
	pr_vector(PagePixRect,PAGE_PIXEL_WIDTH,0,
		              0,PAGE_PIXEL_HEIGHT,PIX_SRC,1);
#endif	WINDOWDEBUG
	OriginalCursor = cursor_copy(window_get(DrawingCanvas,WIN_CURSOR));
	InitializeApplication(BaseFrame, DrawingCanvas);

	argv++;
	while (*argv && argv[0][0] == '-'){
		if (strncmp(argv[0],"-scrollbar",4) == NULL){
			Scrollbars = 0;
		} else if (strncmp(argv[0],"-command",3) == NULL){
			strcpy(CommandString,argv[1]);
			CommandMode = 1;
			PanelInputItem();
			argv++;
		} else {
			fprintf(stderr,"Unknown option %s\n",argv[0]);
		}
		if (*argv){
			argv++;
		}
	}
	if (argv[0] && argv[0][0]){
		CommandMode = 0;
		strcpy(FileName,argv[0]);
		PanelInputItem();
	}

	if (Scrollbars) {
		HorzScroll = scrollbar_create(0);
		VertScroll = scrollbar_create(0);
		window_set(DrawingCanvas,
			   WIN_VERTICAL_SCROLLBAR,VertScroll,
			   WIN_HORIZONTAL_SCROLLBAR,HorzScroll,
			   0);
		SetScrollBarSize(BaseFrame,DrawingCanvas,
				 VertScroll,HorzScroll);
	}

	Rerasterize();

	window_main_loop(BaseFrame);
	if (*TempFileName)
		unlink(TempFileName);
	exit(0);
}

RepaintCanvas(canvas,pw,repaint_area)
Canvas		canvas;
Pixwin		*pw;
Rectlist	*repaint_area;
{
	pw_write(pw,0,0,PAGE_PIXEL_WIDTH,PAGE_PIXEL_HEIGHT,PIX_SRC,
		 PagePixRect,ViewLeft,ViewTop);

	if (ErrorsPending && !window_get(BaseFrame, FRAME_CLOSED)){
		ErrorsPending = 0;
		window_set(ErrorFrame,WIN_SHOW,TRUE,0);
	}
}
	
CanvasInput(window,event,arg)
Window	window;
Event	*event;
caddr_t	arg;
{
	int	i;

	switch(event_id(event)){
        case MS_RIGHT:
		PageRequest = 0;
		event = canvas_window_event(DrawingCanvas,event);
		PopUpMenu(window,event,arg);
		break;
	case MS_MIDDLE:
		PageRequest = 0;
		if (event_is_down(event)) {
			StartPan(event);
		} else {
			EndPan(event);
	        } break;
	case LOC_WINEXIT:
		EndPan(event);
		break;
	case LOC_STILL:
	case LOC_MOVE:
		if (OriginalX > 0){
			if (event_is_up(event)){
				EndPan();
			} else {
				PagePan(event);
			}
	        } 
		break;
	case WIN_REPAINT:
		break;
	case WIN_RESIZE:
		if (Scrollbars){
			SetScrollBarSize(BaseFrame,DrawingCanvas,
					 VertScroll,HorzScroll);
		}
		break;
	case SCROLL_ENTER:
		break;
	case SCROLL_REQUEST:
		if (Scrollbars){
			scrollbar_paint((Scrollbar)arg);
#ifdef	SCROLLDEBUG			
			printf("Scrollbars are at (%d,%d).\n",
			       scrollbar_get(HorzScroll,SCROLL_VIEW_START),
			       scrollbar_get(VertScroll,SCROLL_VIEW_START));
#endif	SCROLLDEBUG
					/* 
					 * Due to a bug in suntools, these
					 * two calls ALWAYS seem to return
					 * zero.
					 */
			ViewLeft = (int)scrollbar_get(HorzScroll,
						      SCROLL_VIEW_START);
			ViewTop = (int)scrollbar_get(VertScroll,
						     SCROLL_VIEW_START);
			RepaintCanvas(DrawingCanvas,
				      canvas_pixwin(DrawingCanvas),0);
		}
		break;
	case '0': case '1': case '2': case '3': case '4':
	case '5': case '6': case '7': case '8': case '9':
		PageRequest = PageRequest*10 + event_id(event) - '0';
		break;
	case 'g': case 'G': case '\n':
		CurrentPage = ShowPage(PageRequest);
		PageRequest = 0;
		break;
	case 033:				/* Escape */
		PageRequest = 0;
	default:
#ifdef	WINDOWDEBUG
		printf("Got an event of id %d (%x).\n",
		       event_id(event),event_id(event));
#endif	WINDOWDEBUG
		break;
	}
}

/*
 *	The following icon of a hand was generated by Stefano Concino@SPAR.
 */
static short	hand_cursor_data[16] = {
		0x0100, 0x0290, 0x12A8, 0x2AA8, 0x2AA8, 0x2AA8, 0x2AA8, 0x2AA8,
		0x2E4B, 0x2015, 0x1015, 0x1815, 0x0C09, 0x0402, 0x0404, 0x0618,

		};
mpr_static(hand_cursor_pixrect,16,16,1,hand_cursor_data);
Cursor	HandCursor;

		
StartPan(event)
Event	*event;
{
	OriginalX = event_x(event);
	OriginalY = event_y(event);
	if (!HandCursor){
		HandCursor = cursor_create(CURSOR_IMAGE,
					   &hand_cursor_pixrect,0);
	}
	window_set(DrawingCanvas,WIN_CURSOR,HandCursor,0);

}

EndPan(event)
Event	*event;
{
	if (OriginalX != -1 && OriginalY != -1){
		OriginalX = OriginalY = -1;
		window_set(DrawingCanvas,
			   WIN_CURSOR,OriginalCursor,
			   0);
	}
}

PagePan(event)
Event	*event;
{
	ViewLeft -= event_x(event) - OriginalX;
	if (ViewLeft + (int)window_get(DrawingCanvas,WIN_WIDTH) >
	    PAGE_PIXEL_WIDTH)
	        ViewLeft = PAGE_PIXEL_WIDTH -
		                  (int)window_get(DrawingCanvas,WIN_WIDTH);
	if (ViewLeft < 0)
		ViewLeft = 0;

	ViewTop -= event_y(event) - OriginalY;
	if (ViewTop + (int)window_get(DrawingCanvas,WIN_HEIGHT) >
	    PAGE_PIXEL_HEIGHT)
	        ViewTop = PAGE_PIXEL_HEIGHT -
		                  (int)window_get(DrawingCanvas,WIN_HEIGHT);
	if (ViewTop < 0)
		ViewTop = 0;
	if ((OriginalX != event_x(event)) || (OriginalY != event_y(event))){
		RepaintCanvas(DrawingCanvas,canvas_pixwin(DrawingCanvas),0);
		OriginalX = event_x(event);
		OriginalY = event_y(event);
		SetScrollBar();
	}
}

PopUpMenu(window,event,arg)
Window	*window;
Event	*event;
caddr_t	arg;
{
	int	i;
	i = (int) menu_show(MainMenu,window,event,0);
	switch(i){
	case 1:
		PopUpPanel();
		break;
	case 2:
		Rerasterize();
		break;
	case 3:
		MousePrintDocument();
		break;
	case 4:
		MousePrintPage();
		break;
	case 5:
		if (CurrentPage > 0){
			CurrentPage = ShowPage(--CurrentPage);
		}
		break;
	case 6:
		CurrentPage = ShowPage(++CurrentPage);
		break;
	case 7:
		window_destroy(BaseFrame);
		break;
	}
}

CreatePopUpPanel()
{
	int	LineNumber = 0;
	int	PanelCommandSpec(), ErasePopUpPanel(),
	        SearchForward(), SearchBackward(),
		GotoPage(), MousePrintPage(), MousePrintDocument(),
		ClosePanel(), EnterFileMode(), EnterCommandMode(),
		Rerasterize();
	char	*p;

#ifdef	TOGETHER
	CommandFrame = BaseFrame;
#else   SEPERATE
	CommandFrame = window_create(BaseFrame, FRAME, 0);
#endif  TOGETHER
	CommandPanel = window_create(CommandFrame,PANEL,0);

	panel_create_item(CommandPanel,PANEL_MESSAGE,
			  PANEL_LABEL_STRING,"SunTroff Status and Command Panel",
			  PANEL_ITEM_X,ATTR_COL(29),
			  PANEL_ITEM_Y,ATTR_COL(LineNumber++),
			  0);
	
	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,
					     "Close Status Window",0,0),
			  PANEL_NOTIFY_PROC,ClosePanel,
			  PANEL_ITEM_X,ATTR_COL(0),
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  0);
			  
	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,
					     "Specify Input File",0,0),
			  PANEL_NOTIFY_PROC,EnterFileMode,
			  PANEL_ITEM_X, ATTR_COL(24), 
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  0);
			  
	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,
					     "Specify Formatting Command",
					     0,0),
			  PANEL_NOTIFY_PROC,EnterCommandMode,
			  PANEL_ITEM_X, ATTR_COL(47), 
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  0);

	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,
					     "Rerasterize",
					     0,0),
			  PANEL_NOTIFY_PROC,Rerasterize,
			  PANEL_ITEM_X, ATTR_COL(76), 
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  0);

	LineNumber++;

	InputItem = 
		  panel_create_item(CommandPanel,PANEL_TEXT,
				    PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
				    PANEL_LABEL_STRING,"Enter file name here: ",
				    PANEL_NOTIFY_STRING, "\033\04\03\012\015",
				    PANEL_ITEM_X,ATTR_COL(0),
				    PANEL_ITEM_Y,ATTR_COL(LineNumber++)+2,
				    PANEL_VALUE_DISPLAY_LENGTH,60,
				    0);

	PanelInputItem();

	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,"Search Backward",
					     0,0),
			  PANEL_NOTIFY_PROC, SearchBackward,
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  0);
	
	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,"Search Forward",
					     0,0),
			  PANEL_NOTIFY_PROC, SearchForward,
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  0);

	SearchItem =
	panel_create_item(CommandPanel,PANEL_TEXT,
			  PANEL_LABEL_STRING, "Enter search string here: ",
			  PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
			  PANEL_NOTIFY_STRING, "\033\04\03\012\015",
			  PANEL_ITEM_Y,ATTR_COL(LineNumber)+2,
			  PANEL_ITEM_X,ATTR_COL(35),
			  PANEL_VALUE_DISPLAY_LENGTH,27,
			  0);

	LineNumber++;
	
	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,"Goto Page",
					     0,0),
			  PANEL_NOTIFY_PROC, GotoPage,
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  PANEL_ITEM_X,ATTR_COL(11),
			  0);

	PageItem = 
	panel_create_item(CommandPanel,PANEL_TEXT,
			  PANEL_LABEL_STRING, "Enter page number here: ",
			  PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
			  PANEL_NOTIFY_STRING, "\033\04\03\012\015",
			  PANEL_ITEM_Y,ATTR_COL(LineNumber)+2,	
			  PANEL_ITEM_X,ATTR_COL(35),
			  PANEL_VALUE_DISPLAY_LENGTH,29,
			  0);

	LineNumber++;

	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,"Print Page",
					     0,0),
			  PANEL_NOTIFY_PROC, MousePrintPage,
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  PANEL_ITEM_X,ATTR_COL(2),
			  0);

	panel_create_item(CommandPanel,PANEL_BUTTON,
			  PANEL_LABEL_IMAGE,
			  panel_button_image(CommandPanel,"Print Document",
					     0,0),
			  PANEL_NOTIFY_PROC, MousePrintDocument,
			  PANEL_ITEM_Y,ATTR_COL(LineNumber),
			  0);

	PrinterItem = 
	panel_create_item(CommandPanel,PANEL_TEXT,
			  PANEL_LABEL_STRING, "Enter printer name here: ",
			  PANEL_NOTIFY_LEVEL, PANEL_SPECIFIED,
			  PANEL_NOTIFY_STRING, "\033\04\03\012\015",
			  PANEL_VALUE, getenv("PRINTER"),
			  PANEL_ITEM_Y,ATTR_COL(LineNumber)+2,
			  PANEL_ITEM_X,ATTR_COL(35),
			  PANEL_VALUE_DISPLAY_LENGTH,28,
			  0);

	
	window_fit(CommandPanel);
#ifndef	TOGETHER
	window_fit(CommandFrame);
#endif	!TOGETHER
}

SetPrinter(Name)
char	*Name;
{
				    /* If you want the typesetter document to
				       set the printer name then remove this
				       ifdef.  The Adobe LaserWriter support
				       always puts psc out as the printer name
				       and since we don't have a printer
				       called psc (instead we use lw{1,2,3,4})
				       we lose.  With this ifdef then the
				       user's PRINTER environment variable is
				       always used as the default printer. */
	
#ifdef	NEVER
	panel_set(PrinterItem, PANEL_VALUE, Name,
			       0);
#endif
}

PanelInputItem(){
	if (CommandMode){
		panel_set(InputItem,
			  PANEL_LABEL_STRING,"Enter formatting command here: ",
			  PANEL_VALUE,CommandString,
			  0);
	} else {
		panel_set(InputItem,
			  PANEL_LABEL_STRING, "Enter file name here: ",
			  PANEL_VALUE,FileName,
			  0);
	}
	
}

ClosePanel()
{
	window_set(CommandFrame,WIN_SHOW,FALSE,0);
}

EnterFileMode()
{
	if (CommandMode){
		strcpy(CommandString, panel_get_value(InputItem));
		
		CommandMode = 0;
		PanelInputItem();
		panel_paint(CommandPanel,PANEL_CLEAR);
	}
}

EnterCommandMode()
{
	if (!CommandMode){
		strcpy(FileName, panel_get_value(InputItem));
		
		CommandMode = 1;
		PanelInputItem();
		panel_paint(CommandPanel,PANEL_CLEAR);
	}
}

MousePrintPage()
{
	char	*Printer;

	Printer = panel_get_value(PrinterItem);
	if (!Printer){
		warning("No printer specified.\nUse 'Show Status' menu item to set printer name.\n");
		return;
	}
	PrintPage(CurrentPage,Printer);
}

MousePrintDocument()
{
	char	*Printer;

	Printer = panel_get_value(PrinterItem);
	if (!Printer){
		warning("No printer specified.\nUse 'Show Status' menu item to set printer name.\n");
		return;
	}
	PrintDocument(ActualFileName,Printer);
	
}

PopUpPanel(){
	window_set(CommandFrame,WIN_SHOW,TRUE,0);
}

ErasePopUpPanel(){
	window_set(CommandFrame,WIN_SHOW,FALSE,0);
}

SearchForward(){
	int	i;
	
	i = SearchFile(panel_get_value(SearchItem),CurrentPage,1);
	if (i){
		CurrentPage = i;
		ShowPage(i);
	} else {
		SetTitleBar("Displaying",CurrentPage);
	}
}

SearchBackward(){
	int	i;
	
	i = SearchFile(panel_get_value(SearchItem),CurrentPage,-1);
	if (i){
		CurrentPage = i;
		ShowPage(i);
	} else {
		SetTitleBar("Displaying",CurrentPage);
	}
}

GotoPage(){
	CurrentPage = ShowPage(atoi(panel_get_value(PageItem)));
}
		
					/*
					 * Set the size of the scrollbar
					 * to a fraction of the size of 
					 * the drawing canvas.  The size of
					 * the scrollbar area is given by
					 * the page size, the size of the
					 * actual scrollbar is given by the
					 * the canvas dimensions.
					 */
SetScrollBarSize(BaseFrame,DrawingCanvas,VertScroll,HorzScroll)
Frame	BaseFrame;
Canvas	DrawingCanvas;
Scrollbar VertScroll, HorzScroll;
{
	if (!Scrollbars)
		return;

	scrollbar_set(VertScroll,
		      SCROLL_OBJECT_LENGTH,PAGE_PIXEL_HEIGHT,
		      SCROLL_VIEW_LENGTH,window_get(DrawingCanvas,WIN_HEIGHT),
		      SCROLL_VIEW_START,ViewTop,
		      0);
	scrollbar_set(HorzScroll,
		      SCROLL_OBJECT_LENGTH,PAGE_PIXEL_WIDTH,
		      SCROLL_VIEW_LENGTH,window_get(DrawingCanvas,WIN_WIDTH),
		      SCROLL_VIEW_START,ViewLeft,
		      0);
}

SetScrollBar()
{
	if (Scrollbars){
		scrollbar_set(VertScroll,
			      SCROLL_VIEW_START,ViewTop,
			      0);
		scrollbar_set(HorzScroll,
			      SCROLL_VIEW_START,ViewLeft,
			      0);
		scrollbar_paint(VertScroll);
		scrollbar_paint(HorzScroll);
	}
}

Rerasterize()
{
	static	FILE	*FilePointer = 0;
	static	FILE	*PipePointer = 0;
	static	int	ProcessPid = 0;

	if (FilePointer){
	        fclose(FilePointer);
	}
	if (PipePointer){
		fclose(PipePointer);
		signal(SIGKILL, ProcessPid);
	}
	ProcessPid = 0;
	PipePointer = FilePointer = 0;
			
	if (!CommandMode){
		strcpy(FileName,panel_get_value(InputItem));
		if (!FileName || !FileName[0]){
			warning("No input file specified.\nUse the status window to enter a file name.");
			return;
		}

		FilePointer = fopen(FileName,"r");
		if (!FilePointer){
			warning("Can't open %s for reading input.\n",
				FileName);
		} else {
			ActualFileName = FileName;
			InitializeFile(FilePointer, FilePointer);
		}
	} else {
		char	Buffer[BUFSIZ];
		static char FileName[BUFSIZ], *p;
		int	i, PipeFds[2];

		strcpy(CommandString,panel_get_value(InputItem));
		if (!CommandString){
			warning("Rasterize called with out a command.\nUse status panel to set command.");
			return;
		}
		p = CommandString;
		while (isspace(*p))
			p++;
		if (!*p){
			warning("Rasterize called without a command.\nUse the status panel to set a command.");
			return;
		}

		pipe(PipeFds);			/* Get two halves of pipe */
		
		ProcessPid = fork();		/* And then Fork */
		if (!ProcessPid){		/* First the Child */
			close(1);
			dup2(PipeFds[1], 1);
			close(PipeFds[0]);
			system(p);
			exit(0);
		}
	
		PipePointer = fdopen(PipeFds[0], "r");
		close(PipeFds[1]);
		
		if (*TempFileName) {
		    strcpy(FileName, TempFileName);
		} else {
		    strcpy(FileName,"/tmp/suntroff.XXXXXX");
		    mktemp(FileName);
		    strcpy(TempFileName, FileName);
		}
		
		FilePointer = fdopen(open(FileName,
					  O_RDWR|O_CREAT|O_TRUNC,0644), "r+");
		
		if (!FilePointer){
			fprintf(stderr,
			    "Can't open buffer file for the command:\n");
			fprintf(stderr,"\t%s\n", CommandString);
			exit(1);
		}
		ActualFileName = FileName;
		InitializeFile(PipePointer, FilePointer);
	}

	ShowPage(CurrentPage);
}

SetTitleBar(Status,PageNumber)
char	*Status;
int	PageNumber;
{
	extern char	*DefaultTitle;
	char	Buffer[BUFSIZ];
	char	TempStatus[BUFSIZ];
	int	FrameWidth, Width, i;
	static	char PageNumberValue[20];

	FrameWidth = (int) window_get(BaseFrame,WIN_WIDTH)/
	  		(int) window_get(BaseFrame,WIN_COLUMN_WIDTH) - 1;

	if (PageNumber >= 0)
		sprintf(TempStatus,"%s Page %d",Status,PageNumber);
	else
		sprintf(TempStatus, "%s", Status);

	strncpy(Buffer,DefaultTitle,BUFSIZ);
	i = strlen(DefaultTitle);
	
	Width = strlen(TempStatus);

	while (i + Width < FrameWidth){
		Buffer[i++] = ' ';
	}
	if (FrameWidth - i > 0){
		strncpy(&Buffer[i],TempStatus,FrameWidth-i);
	}
	window_set(BaseFrame,FRAME_LABEL,Buffer,0);
	if (PageNumber >= 0){
		sprintf(PageNumberValue,"%d",PageNumber);
		panel_set_value(PageItem,PageNumberValue);
	}
}

static char SavedTitleBar[BUFSIZ];

SaveTitleBar(){
	char	*p;

	p = window_get(BaseFrame, FRAME_LABEL, 0);
	if (p){
		strncpy(SavedTitleBar, p, BUFSIZ);
	}
}

RestoreTitleBar(){
	window_set(BaseFrame,FRAME_LABEL,SavedTitleBar,0);
}
	
#define MAXLINES 100
static int LineNumber = 0;
static Panel_item ItemList[MAXLINES];

fatal(String, a1, a2, a3, a4, a5, a6, a7, a8)
char	*String;
int	a1, a2, a3, a4, a5, a6, a7, a8;
{
	fprintf(stderr,"%s Fatal Error: ",DefaultTitle);
	fprintf(stderr,String,a1,a2,a3,a4,a5,a6,a7,a8);
	if (*TempFileName)
		unlink(TempFileName);
	exit(1);
}

warning(String,a1,a2,a3,a4,a5,a6,a7,a8)
char	*String;
int	a1, a2,a3,a4,a5,a6,a7,a8;
{
	char	*p, Message[BUFSIZ], *MessagePointer;
	int	c, i, ErrorDone();

	sprintf(Message,String,a1,a2,a3,a4,a5,a6,a7,a8);
	
	if (!ErrorFrame){
		for (i=0;i<MAXLINES;i++){
			ItemList[i] = 0;
		}
		LineNumber = 0;
		ErrorFrame = window_create(BaseFrame,FRAME,0);
		ErrorPanel = window_create(ErrorFrame,PANEL,0);
		panel_create_item(ErrorPanel,PANEL_BUTTON,
				  PANEL_LABEL_IMAGE,
				  panel_button_image(ErrorPanel,
						     "Ok!", 0, 0),
				  PANEL_ITEM_Y,1,
				  PANEL_NOTIFY_PROC, ErrorDone,
				  0);
				 
	}

/*	window_set(ErrorFrame,WIN_SHOW,FALSE,0);
 */
	MessagePointer = p = Message;
	while (*p) {
		for (;*p;p++){
			if (*p == '\n')
			  break;
		}
		
		c = *p;
		*p = NULL;

		ItemList[LineNumber] = panel_create_item(ErrorPanel,
							 PANEL_MESSAGE,
							 PANEL_LABEL_STRING,
							 MessagePointer,
							 PANEL_ITEM_Y,
							 ATTR_COL(LineNumber),
							 PANEL_ITEM_X,
							 ATTR_COL(7),
							 0);
		*p = c;
		if (c)
		  	p++;
		MessagePointer = p;
		LineNumber++;
	}
						/* Leave a blank line */
	ItemList[LineNumber] = panel_create_item(ErrorPanel,
						   PANEL_MESSAGE,
						    PANEL_LABEL_STRING, "",
						   PANEL_ITEM_Y,
						    ATTR_COL(LineNumber),
						   PANEL_ITEM_X,
						    ATTR_COL(7),
						   0);
    	LineNumber++;
	
	window_fit(ErrorPanel);
	window_fit(ErrorFrame);

	ErrorsPending++;
	if (!window_get(BaseFrame, FRAME_CLOSED)){
		ErrorsPending = 0;
		window_set(ErrorFrame,WIN_SHOW,TRUE,0);
	}
}

ErrorDone()
{
	int i;
	
	window_set(ErrorFrame,WIN_SHOW,FALSE,0);
	LineNumber = 0;

	for (i = 0; i < MAXLINES && ItemList[i]; i++){
		if (ItemList[i]){
			panel_free(ItemList[i]);
			ItemList[i] = 0;
		}
	}

}

#ifdef	STANDALONE

InitializeFile(){
        printf("Initialize File called.\n");
}

PrintDocument(){
	printf("Print Document called.\n");
}

PrintPage(){
	printf("Print Page called.\n");
}

InitializeApplication(){
	pr_vector(PagePixRect,0,0,
		              PAGE_PIXEL_WIDTH,PAGE_PIXEL_HEIGHT,PIX_SRC,1);
	pr_vector(PagePixRect,PAGE_PIXEL_WIDTH,0,
		              0,PAGE_PIXEL_HEIGHT,PIX_SRC,1);
        printf("Initialize Application Called.\n");
}

ShowPage(){
	printf("Show Page called.\n");
}

char	*DefaultTitle = "Standalone Window Code";

SearchFile(){
	printf("Search file called.\n");
}

#endif STANDALONE
#endif SUNTOOLS
