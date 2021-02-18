/************************************************************
Copyright 1989 by Information-Technology Promotion Agency Japan (IPA),
Mitsubishi Research Institute, Inc. (MRI), and Fuji Xerox Co., Ltd. (FX)

                        All Rights Reserved

Permission to use, copy, modify and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of IPA, MRI or FX not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IPA, MRI or FX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
EVENT SHALL IPA, MRI or FX BE LIABLE FOR ANY SPECIAL, INDIRECT OR
CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFTWARE.

********************************************************/

/*
 * StPicture.c - StPicture widget
 *
 */

#define XtStrlen(s)     ((s) ? strlen(s) : 0)

#include <ctype.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Cardinals.h>
#include <X11/Scroll.h>
#include <X11/Dialog.h>
#include <X11/Logo.h>

#include  <X11/bitmaps/black>
#include  <X11/bitmaps/boxes>
#include  <X11/bitmaps/cross_weave>
#include  <X11/bitmaps/dimple1>
#include  <X11/bitmaps/dimple3>
#include  <X11/bitmaps/flipped_gray>
#include  <X11/bitmaps/gray1>
#include  <X11/bitmaps/gray3>
#include  <X11/bitmaps/light_gray>
#include  <X11/bitmaps/root_weave>
#include  <X11/bitmaps/scales>
#include  <X11/bitmaps/stipple>
#include  <X11/bitmaps/wide_weave>
#define hollow_width 16
#define hollow_height 16
static char hollow_bits[] = {
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

#include <St/StPictureP.h>
#include <St/StPicPopup.h>

#define ST_DC_TEXT_BUF_SIZE   1024

/****************************************************************
 *
 * Full class record constant
 *
 ****************************************************************/

/* Private Data */

static char defaultTranslations[] =
    "Ctrl<Key>D:	delete-character()		\n\
     Ctrl<Key>H:	delete-previous-character()	\n\
     Ctrl<Key>K:	erase-to-end-of-line()		\n\
     Ctrl<Key>M:	newline()			\n\
     Ctrl<Key>U:	erase-line()			\n\
     <Key>Delete:	delete-previous-character()	\n\
     <Key>BackSpace:	delete-previous-character()	\n\
     <Key>Return:	newline()			\n\
     <Key>:		insert-char()			\n\
     Shift<Btn1Down>,Shift<Btn1Up>:	move-hand() \
					notify-before(move-hand) \
					notify-after(move-hand)	\n\
     <Btn1Down>:	select-start()			\n\
     <Btn1Motion>:	select-adjust()			\n\
     <Btn1Up>:		select-end() notify-after(select-end)	\n\
     Shift<Btn2Down>,Shift<Btn2Up>:	pline-end() \
					notify-after(pline-end) \n\
     <Btn2Down>,<Btn2Up>:		main-popup()    \n\
     <Btn3Down>:	extend-start()			\n\
     <Btn3Motion>:	extend-adjust()			\n\
     <Btn3Up>:		extend-end() 			\n\
     <Motion>:		motion() 			";

static char scrollbarTranslations[] =
    "Ctrl<Key>D:        do-nothing() \n\
     Ctrl<Key>H:        do-nothing() \n\
     Ctrl<Key>K:        do-nothing() \n\
     Ctrl<Key>M:        do-nothing() \n\
     Ctrl<Key>U:        do-nothing() \n\
     <Key>Delete:       do-nothing() \n\
     <Key>BackSpace:    do-nothing() \n\
     <Key>Return:       do-nothing() \n\
     <Key>:             do-nothing() \n\
     <Btn2Motion>:      MoveThumb() NotifyThumb() \n\
     <Motion>:          do-nothing()"  ;
static char coreTranslations[] =
    "Ctrl<Key>D:        do-nothing() \n\
     Ctrl<Key>H:        do-nothing() \n\
     Ctrl<Key>K:        do-nothing() \n\
     Ctrl<Key>M:        do-nothing() \n\
     Ctrl<Key>U:        do-nothing() \n\
     <Key>Delete:       do-nothing() \n\
     <Key>BackSpace:    do-nothing() \n\
     <Key>Return:       do-nothing() \n\
     <Key>:             do-nothing() \n\
     Shift<Btn1Down>,Shift<Btn1Up>: do-nothing() \n\
     <Btn1Down>:	do-nothing() \n\
     <Btn1Motion>:	do-nothing() \n\
     <Btn1Up>:		do-nothing() \n\
     <Btn2Down>,<Btn2Up>: do-nothing() \n\
     <Btn3Down>:	do-nothing() \n\
     <Btn3Motion>:	do-nothing() \n\
     <Btn3Up>:		do-nothing() \n\
     <Motion>:          do-nothing() \n\
     <Expose>:          do-nothing()"  ;

static Boolean defTrue  = True  ;
static Boolean defFalse = False  ;

static String  defaultMenuEntries[]={
	"Attributes->",
	"    Draw  ->",
	"End Draw",
	"    Group ->",
	" Priority ->",
	"Top<->Bottom",
	"Left<->Right",
	"   Rotate ->",
	"Delete",
	"Copy",
	"Move",
	"Resize",
	"    Write ->",
	"    Read  ->",
	"Read Image->",
	"Undo",
	"Redraw",
	NULL
	}  ;
static String  defaultMenuEntriesDraw[]={
	"Rectangle",
	"Circle",
	"Ellipse",
	"Line",
	"Arc",
	"Elliptic Arc",
	"Pie",
	"Elliptic Pie",
	"Polyline",
	"Polygon",
	"Freehand",
	"Text",
	NULL
	}  ;
static caddr_t defaultMenuEntriesPtr= (caddr_t)defaultMenuEntries  ;
static caddr_t defaultMenuEntriesDrawPtr= (caddr_t)defaultMenuEntriesDraw  ;

static StPictureFunction  defaultMenuFunctions[]={
	StpictureAttributes,
	StpictureDraw,
	StpictureDrawEnd,
	StpictureGroup,
	StpicturePriority,
	StpictureTopBottom,
	StpictureLeftRight,
	StpictureRotate,
	StpictureDelete,
	StpictureCopy,
	StpictureMove,
	StpictureResize,
	StpictureWrite,
	StpictureRead,
	StpictureImage,
	StpictureUndo,
	StpictureRedraw,
	StpictureEnd,
	}  ;
static StDrawFunction  defaultMenuFunctionsDraw[]={
	StdrawRectangle,
	StdrawCircle,
	StdrawEllipse,
	StdrawLine,
	StdrawArc,
	StdrawEllipticArc,
	StdrawPie,
	StdrawEllipticPie,
	StdrawPolyline,
	StdrawPolygon,
	StdrawFreehand,
	StdrawText,
	StdrawEnd,
	}  ;
static caddr_t defaultMenuFunctionsPtr= (caddr_t)defaultMenuFunctions  ;
static caddr_t defaultMenuFunctionsDrawPtr= (caddr_t)defaultMenuFunctionsDraw  ;

#define offset(field) XtOffset(StPictureWidget, field)
static XtResource resources[] = { 

   {XtNborderWidth, XtCBorderWidth, XtRDimension, sizeof(Dimension),
	offset(core.border_width), XtRImmediate, (caddr_t) 4},
   {XtNfile, XtCFile, XtRString, sizeof(String),
	offset(stPicture.file_name), XtRString, NULL},
   {XtNforceBars, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(stPicture.forcebars), XtRBoolean, (caddr_t)&defFalse },
   {XtNforeground, XtCForeground, XtRPixel, sizeof (int),
	offset(stPicture.foreground), XtRString, "Black"},
   {XtNfont, XtCFont, XtRFontStruct, sizeof (XFontStruct *),
	offset(stPicture.font), XtRString, "Fixed"},
   {XtNheight, XtCHeight, XtRDimension, sizeof(Dimension),
	offset(core.height), XtRImmediate, (caddr_t) DEFAULT_HEIGHT},
   {XtNuseBottom, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(stPicture.usebottom), XtRBoolean, (caddr_t)&defFalse },
   {XtNuseRight, XtCBoolean, XtRBoolean, sizeof(Boolean),
	offset(stPicture.useright), XtRBoolean, (caddr_t)&defFalse },
   {XtNwidth, XtCWidth, XtRDimension, sizeof(Dimension),
	offset(core.width), XtRImmediate, (caddr_t) DEFAULT_WIDTH},

   {StNafterCallbacks, StCAfterCallbacks,
	XtRPointer, sizeof(XtCallbackList *),
	offset(stPicture.after), XtRPointer, (caddr_t)NULL},
   {StNafterCallbacksDraw, StCAfterCallbacksDraw,
	XtRPointer, sizeof(XtCallbackList  *),
	offset(stPicture.after_draw), XtRPointer, (caddr_t)NULL},
   {StNallowProtrusion, StCAllowProtrusion, XtRBoolean, sizeof(Boolean),
	offset(stPicture.protrusion), XtRBoolean, (caddr_t)&defTrue },
   {StNbeforeCallbacks, StCBeforeCallbacks,
	XtRPointer, sizeof(XtCallbackList  *),
	offset(stPicture.before), XtRPointer, (caddr_t)NULL},
   {StNbeforeCallbacksDraw, StCBeforeCallbacksDraw,
	XtRPointer, sizeof(XtCallbackList  *),
	offset(stPicture.before_draw), XtRPointer, (caddr_t)NULL},
   {StNcoordinateXMax, StCCoordinateXMax, XtRPosition, sizeof(Position),
	offset(stPicture.coor_xmax), XtRImmediate, (caddr_t) DEFAULT_MAX},
   {StNcoordinateXMin, StCCoordinateXMin, XtRPosition, sizeof(Position),
	offset(stPicture.coor_xmin), XtRImmediate, (caddr_t) DEFAULT_MIN},
   {StNcoordinateYMax, StCCoordinateYMax, XtRPosition, sizeof(Position),
	offset(stPicture.coor_ymax), XtRImmediate, (caddr_t) DEFAULT_MAX},
   {StNcoordinateYMin, StCCoordinateYMin, XtRPosition, sizeof(Position),
	offset(stPicture.coor_ymin), XtRImmediate, (caddr_t) DEFAULT_MIN},
   {StNmenuEntries, StCMenuEntries, XtRPointer, sizeof(char **),
	offset(stPicture.entries),
	XtRPointer, (caddr_t)&defaultMenuEntriesPtr},
   {StNmenuEntriesDraw, StCMenuEntriesDraw, XtRPointer, sizeof(char **),
	offset(stPicture.entries_draw),
	XtRPointer, (caddr_t)&defaultMenuEntriesDrawPtr},
   {StNmenuFunctions, StCMenuFunctions,
	XtRPointer, sizeof(StPictureFunction *),
	offset(stPicture.functions),
	XtRPointer, (caddr_t)&defaultMenuFunctionsPtr},
   {StNmenuFunctionsDraw, StCMenuFunctionsDraw,
	XtRPointer, sizeof(StDrawFunction *),
	offset(stPicture.functions_draw),
	XtRPointer, (caddr_t)&defaultMenuFunctionsDrawPtr},
   {StNmenuTitle, StCMenuTitle, XtRString, sizeof(String),
	offset(stPicture.title), XtRString, NULL},
   {StNmenuTitleDraw, StCMenuTitleDraw, XtRString, sizeof(String),
	offset(stPicture.title_draw), XtRString, NULL},
   {StNviewX, StCViewX, XtRPosition, sizeof(Position),
	offset(stPicture.view_x), XtRImmediate, (caddr_t) 0},
   {StNviewY, StCViewY, XtRPosition, sizeof(Position),
	offset(stPicture.view_y), XtRImmediate, (caddr_t) 0},

   {StNtempCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
	offset(stPicture.temp_callback), XtRCallback, (caddr_t)NULL},
};
#undef offset

static void  DeleteCharacter()  ;
static void  DeletePreviousCharacter()  ;
static void  EraseToEndOfLine()  ;
static void  Newline()  ;
static void  EraseLine()  ;
static void  InsertChar()  ;
static void  NotifyBefore()  ;
static void  MoveHand()  ;
static void  NotifyAfter()  ;
static void  SelectStart()  ;
static void  SelectAdjust()  ;
static void  SelectEnd()  ;
static void  PlineEnd()  ;
static void  MainPopup()  ;
static void  MenuFunction()  ;
static void  ExtendStart()  ;
static void  ExtendAdjust()  ;
static void  ExtendEnd()  ;
static void  Motion()  ;
static void  DoNothing()  ;
static void  SubPopup()  ;
static void  Parse()  ;
static void  ThirdPopup()  ;

static XtActionsRec actionsList[] =
{
  {"delete-character",		DeleteCharacter},
  {"delete-previous-character",	DeletePreviousCharacter},
  {"erase-to-end-of-line",	EraseToEndOfLine},
  {"newline",			Newline},
  {"erase-line",		EraseLine},
  {"insert-char",		InsertChar},
  {"notify-before",		NotifyBefore},
  {"move-hand",			MoveHand},
  {"notify-after",		NotifyAfter},
  {"select-start",		SelectStart},
  {"select-adjust",		SelectAdjust},
  {"select-end",		SelectEnd},
  {"pline-end",			PlineEnd},
  {"main-popup",		MainPopup},
  {"menu-function",		MenuFunction},
  {"extend-start",		ExtendStart},
  {"extend-adjust",		ExtendAdjust},
  {"extend-end",		ExtendEnd},
  {"motion",			Motion},
  {"do-nothing",		DoNothing},
  {"subpop",			SubPopup},
  {"parse",			Parse},
  {"thirdpop",			ThirdPopup},
};

static void Initialize();
static void Realize();
static void Destroy();
static void Resize();
static void Redisplay();
static Boolean SetValues();

StPictureClassRec stPictureClassRec = {
  {
/* core_class fields */
#define superclass              (&simpleClassRec)
    /* superclass               */      (WidgetClass)superclass,
    /* class_name               */      "StPicture",
    /* widget_size              */      sizeof(StPictureRec),
    /* class_initialize         */      NULL,
    /* class_part_initialize    */      NULL,
    /* class_inited             */      FALSE,
    /* initialize               */      Initialize,
    /* initialize_hook          */      NULL,
    /* realize                  */      Realize,
    /* actions                  */      actionsList,
    /* num_actions              */      XtNumber(actionsList),
    /* resources                */      resources,
    /* num_resources            */      XtNumber(resources),
    /* xrm_class                */      NULLQUARK,
    /* compress_motion          */      TRUE,
    /* compress_exposure        */      FALSE,
    /* compress_enterleave      */      TRUE,
    /* visible_interest         */      FALSE,
    /* destroy                  */      Destroy,
    /* resize                   */      Resize,
    /* expose                   */      Redisplay,
    /* set_values               */      SetValues,
    /* set_values_hook          */      NULL,
    /* set_values_almost        */      XtInheritSetValuesAlmost,
    /* get_values_hook          */      NULL,
    /* accept_focus             */      NULL,
    /* version                  */      XtVersion,
    /* callback_private         */      NULL,
    /* tm_table                 */      defaultTranslations,
    /* query_geometry           */      XtInheritQueryGeometry,
    /* display_accelerator      */      XtInheritDisplayAccelerator,
    /* extension                */      NULL
  }
};
#undef  superclass
WidgetClass stPictureWidgetClass = (WidgetClass) &stPictureClassRec;

/****************************************************************
 *
 * Menu Translation
 *
 ****************************************************************/

static  String  _EntriesTrans[]={
/* NULL      */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,0)              ",
/* Draw      */	"<EnterWindow>:highlight() subpop(Draw) \n\
		 <Btn1Up>     :unset() parse(Main,1)              ",
/* DrawEnd   */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,2)              ",
/* Attributes*/	"<EnterWindow>:highlight() subpop(Attributes) \n\
		 <Btn1Up>     :unset() parse(Main,3)              ",
/* Group     */	"<EnterWindow>:highlight() subpop(Group) \n\
		 <Btn1Up>     :unset() parse(Main,4)              ",
/* Priority  */	"<EnterWindow>:highlight() subpop(Priority) \n\
		 <Btn1Up>     :unset() parse(Main,5)              ",
/* TopBottom */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,6)              ",
/* LeftRight */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,7)              ",
/* Rotate    */	"<EnterWindow>:highlight() subpop(Rotate) \n\
		 <Btn1Up>     :unset() parse(Main,8)              ",
/* Delete    */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,9)              ",
/* Copy      */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,a)              ",
/* Move      */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,b)              ",
/* Resize    */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,c)              ",
/* Write     */	"<EnterWindow>:highlight() subpop(Write) \n\
		 <Btn1Up>     :unset() parse(Main,d)              ",
/* Read      */	"<EnterWindow>:highlight() subpop(Read) \n\
		 <Btn1Up>     :unset() parse(Main,e)              ",
/* Image     */	"<EnterWindow>:highlight() subpop(Image) \n\
		 <Btn1Up>     :unset() parse(Main,f)              ",
/* Undo      */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,g)              ",
/* Redraw    */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,h)              ",
/* Hand      */	"<EnterWindow>:highlight() subpop(Down) \n\
		 <Btn1Up>     :unset() parse(Main,i)              ",
	}  ;

static  String  _EntriesTrans_draw[]={
/* NULL      */	"<Btn1Up>     :unset() parse(Draw,0)  ",
/* Rectangle */	"<Btn1Up>     :unset() parse(Draw,1)  ",
/* Circle    */	"<Btn1Up>     :unset() parse(Draw,2)  ",
/* Ellipse   */	"<Btn1Up>     :unset() parse(Draw,3)  ",
/* Line      */	"<Btn1Up>     :unset() parse(Draw,4)  ",
/* Arc       */	"<Btn1Up>     :unset() parse(Draw,5)  ",
/* Elli Arc  */	"<Btn1Up>     :unset() parse(Draw,6)  ",
/* Pie       */	"<Btn1Up>     :unset() parse(Draw,7)  ",
/* Elli Pie  */	"<Btn1Up>     :unset() parse(Draw,8)  ",
/* Polyline  */	"<Btn1Up>     :unset() parse(Draw,9)  ",
/* Polygon   */	"<Btn1Up>     :unset() parse(Draw,a)  ",
/* Freehand  */	"<Btn1Up>     :unset() parse(Draw,b)  ",
/* Text      */	"<Btn1Up>     :unset() parse(Draw,c)  ",
	}  ;

static  String  _EntriesTrans_attributes[]={
/* Line      */	"<EnterWindow>:highlight() thirdpop(Line) ",
/* Fill      */	"<EnterWindow>:highlight() thirdpop(Fill) ",
/* Char      */	"<EnterWindow>:highlight() thirdpop(Char) ",

/* Line-Ok   */	"<Btn1Up>     :unset() parse(Attributes,0)",
/* Fill-Ok   */	"<Btn1Up>     :unset() parse(Attributes,1)",
/* Char-Ok   */	"<Btn1Up>     :unset() parse(Attributes,2)",
	}  ;

static  String  _EntriesTrans_group[]={
/* Set       */	"<Btn1Up>     :unset() parse(Group,0)  ",
/* Reset     */	"<Btn1Up>     :unset() parse(Group,1)  ",
	}  ;

static  String  _EntriesTrans_priority[]={
/* High    */	"<Btn1Up>     :unset() parse(Priority,0)  ",
/* Low     */	"<Btn1Up>     :unset() parse(Priority,1)  ",
	}  ;

static  String  _EntriesTrans_rotate[]={
/* +90     */	"<Btn1Up>     :unset() parse(Rotate,0)  ",
/* +180    */	"<Btn1Up>     :unset() parse(Rotate,1)  ",
/* +270    */	"<Btn1Up>     :unset() parse(Rotate,2)  ",
	}  ;

static  String  _EntriesTrans_write[]={
/* Ok      */	"<Btn1Up>     :unset() parse(Write)  ",
	}  ;

static  String  _EntriesTrans_read[]={
/* Merge   */	"<Btn1Up>     :unset() parse(Read,0)  ",
/* Override*/	"<Btn1Up>     :unset() parse(Read,1)  ",
	}  ;

static  String  _EntriesTrans_image[]={
/* Ok      */	"<Btn1Up>     :unset() parse(Image)  ",
	}  ;

/****************************************************************
 *
 * Attributes Table
 *
 ****************************************************************/

static  String  WidthTable[]={
		" 0"," 1"," 2"," 3"," 4"," 5"," 6"," 7"," 8"," 9","10",
		"11","12","13","14","15","16","17","18","19","20",
		NULL,
	}  ;

static  String  LineTypeTable[]={
		"Solid     ","OnOffDash ","DoubleDash",
		NULL,
	}  ;

static  String  LineCapTable[]={
		"Butt ","Round","Arrow",
		NULL,
	}  ;

static  String  LineJoinTable[]={
		"Miter","Round","Bevel",
		NULL,
	}  ;

static  String  ColorTable1[]={
		"Black","White","Red","Green",
		"Blue","Cyan","Yellow","Magenta",
		"LightGray","Gray","Orange","LimeGreen",
		"SteelBlue","MediumTurquoise","MediumGoldenrod","Orchid",
	}  ;

static  String  ColorTable2[]={
		"Black","White","Black","White","Black","White","Black","White",
		"White","Black","White","Black","White","Black","White","Black",
	}  ;

static  Pixel  RealColor[16]  ;

static  String  FunctionTable[]={
		"Clear     ","And       ","AndReverse","Copy      ",
		"AndInvert ","Noop      ","Xor       ","Or        ",
		"Nor       ","Equiv     ","Invert    ","OrReverse ",
		"CopyInvert","OrInvert  ","Nand      ","Set       ",
		NULL,
	}  ;

static  String  CharKindTable[]={
		"Roman ","Italic","Bold-R","Bold-I",
		NULL,
	}  ;

static  String  CharSizeTable[]={
		"8 ","16","24",
		NULL,
	}  ;

static  _MapTable  MapTable[]={
		{boxes_bits,boxes_width,boxes_height},
		{cross_weave_bits,cross_weave_width,cross_weave_height},
		{dimple1_bits,dimple1_width,dimple1_height},
		{dimple3_bits,dimple3_width,dimple3_height},
		{flipped_gray_bits,flipped_gray_width,flipped_gray_height},
		{gray1_bits,gray1_width,gray1_height},
		{gray3_bits,gray3_width,gray3_height},
		{light_gray_bits,light_gray_width,light_gray_height},
		{root_weave_bits,root_weave_width,root_weave_height},
		{scales_bits,scales_width,scales_height},
		{stipple_bits,stipple_width,stipple_height},
		{wide_weave_bits,wide_weave_width,wide_weave_height},
		{black_bits,black_width,black_height},
		{hollow_bits,hollow_width,hollow_height},
	}  ;

/****************************************************************
 *
 * Private Procedures
 *
 ****************************************************************/

static Boolean 
Vcheck(w, mode)
	StPictureWidget w;
	int             mode;
{
	Boolean         error = True;
	Boolean         normal = False;

	if (w->stPicture.coor_xmax <= w->stPicture.coor_xmin) {
		if (!mode) {
			XtWarning("StPicture Resource Error:coor_xmax -> default used");
			XtWarning("StPicture Resource Error:coor_xmin -> default used");
			w->stPicture.coor_xmax = DEFAULT_MAX;
			w->stPicture.coor_xmin = DEFAULT_MIN;
		} else
			return error;
	}
	if (w->stPicture.coor_ymax <= w->stPicture.coor_ymin) {
		if (!mode) {
			XtWarning("StPicture Resource Error:coor_ymax -> default used");
			XtWarning("StPicture Resource Error:coor_ymin -> default used");
			w->stPicture.coor_ymax = DEFAULT_MAX;
			w->stPicture.coor_ymin = DEFAULT_MIN;
		} else
			return error;
	}
	if ((w->stPicture.view_x < w->stPicture.coor_xmin) ||
	    (w->stPicture.view_x >= w->stPicture.coor_xmax) ||
	    (w->stPicture.view_y < w->stPicture.coor_ymin) ||
	    (w->stPicture.view_y >= w->stPicture.coor_ymax)) {
		if (!mode) {
			XtWarning("StPicture Resource Error:view_x & view_y -> middle_position used");
			w->stPicture.view_x =
				(w->stPicture.coor_xmax + w->stPicture.coor_xmin) / 2;
			w->stPicture.view_y =
				(w->stPicture.coor_ymax + w->stPicture.coor_ymin) / 2;
		} else
			return error;
	}
	return normal;
}


static void 
CreateScrollbar(w)
	StPictureWidget w;
{
	void            VScrollUpDown(), VScrollThumb(), HScrollUpDown(), HScrollThumb();
	Arg             args[1];
	Widget          wtemp, wtemp2;

	/* make Vertical & Horizontal & OutofViewArea */
	w->stPicture.sbver = wtemp =
		XtCreateWidget("vertical_scrollbar", scrollbarWidgetClass, w, NULL, NULL);
	XtOverrideTranslations(wtemp,
			    XtParseTranslationTable(scrollbarTranslations));
	XtAddCallback(wtemp, XtNscrollProc, VScrollUpDown, (caddr_t) w);
	XtAddCallback(wtemp, XtNjumpProc, VScrollThumb, (caddr_t) w);

	XtSetArg(args[0], XtNorientation, XtorientHorizontal);
	w->stPicture.sbhor = wtemp2 =
		XtCreateWidget("horizontal_scrollbar", scrollbarWidgetClass, w, args, ONE);
	XtOverrideTranslations(wtemp2,
			    XtParseTranslationTable(scrollbarTranslations));
	XtAddCallback(wtemp2, XtNscrollProc, HScrollUpDown, (caddr_t) w);
	XtAddCallback(wtemp2, XtNjumpProc, HScrollThumb, (caddr_t) w);
}


static void 
ResizeScrollbar(w)
	StPictureWidget w;
{
	Arg             args[3];
	Widget          wtemp, wtemp2;
	float           top, shown;
	float           len;
	int             clip_x, clip_y;

	/* Resize Scrollbar & OutofViewArea */
	wtemp = w->stPicture.sbver;
	wtemp2 = w->stPicture.sbhor;
	XtResizeWidget(wtemp, wtemp->core.width,
	   w->core.height - wtemp2->core.height - wtemp2->core.border_width,
		       wtemp->core.border_width);
	XtResizeWidget(wtemp2,
	       w->core.width - wtemp->core.width - wtemp->core.border_width,
		       wtemp2->core.height, wtemp2->core.border_width);

	/* if need , create or delete OutofViewArea  */
	clip_y = w->core.height - wtemp->core.height - wtemp->core.border_width;
	clip_x = w->core.width - wtemp2->core.width - wtemp2->core.border_width;
	if (clip_y > 0 && clip_x > 0) {
		if (!w->stPicture.outofview) {
			XtSetArg(args[0], XtNborderWidth, 0);
			XtSetArg(args[1], XtNwidth, clip_x);
			XtSetArg(args[2], XtNheight, clip_y);
			w->stPicture.outofview =
				XtCreateWidget("out_of_view", widgetClass, w, args, THREE);
			XtOverrideTranslations(w->stPicture.outofview,
				 XtParseTranslationTable(coreTranslations));
			if (XtIsRealized(w)) {
				XtRealizeWidget(w->stPicture.outofview);
				XtMapWidget(w->stPicture.outofview);
			}
		} else {
			XtResizeWidget(w->stPicture.outofview,
				 (Dimension) clip_x, (Dimension) clip_y, 0);
		}
	} else {
		if (w->stPicture.outofview) {
			XtDestroyWidget(w->stPicture.outofview);
			w->stPicture.outofview = NULL;
		}
	}

	/* SetThumb */
	if (w->stPicture.protrusion) {
		len = (float) (w->stPicture.coor_ymax -
			       w->stPicture.coor_ymin);
		top = ((float) (w->stPicture.view_y -
				w->stPicture.coor_ymin)) / len;
		shown = ((float) (w->core.height - wtemp2->core.height -
				  wtemp2->core.border_width)) / len;
		XtScrollBarSetThumb(wtemp, top, shown);

		len = (float) (w->stPicture.coor_xmax -
			       w->stPicture.coor_xmin);
		top = ((float) (w->stPicture.view_x -
				w->stPicture.coor_xmin)) / len;
		shown = ((float) (w->core.width - wtemp->core.width -
				  wtemp->core.border_width)) / len;
		XtScrollBarSetThumb(wtemp2, top, shown);
	} else {
		top = 0.0;
		shown = 1.0;
		XtScrollBarSetThumb(wtemp, top, shown);
		XtScrollBarSetThumb(wtemp2, top, shown);
	}

	/* move scrollbar */
	/* Right & Bottom */
	if (w->stPicture.useright && w->stPicture.usebottom) {
		XtMoveWidget(wtemp,
			     (Position) (w->core.width -
			      wtemp->core.width - wtemp->core.border_width),
			     -(Position) wtemp->core.border_width);

		XtMoveWidget(wtemp2,
			     -(Position) wtemp2->core.border_width,
			     (Position) (w->core.height -
			  wtemp2->core.height - wtemp2->core.border_width));

		if (w->stPicture.outofview)
			XtMoveWidget(w->stPicture.outofview,
				     (Position) (w->core.width - clip_x),
				     (Position) (w->core.height - clip_y));
	}
	/* Right & Top */
	else if (w->stPicture.useright && !w->stPicture.usebottom) {
		XtMoveWidget(wtemp,
			     (Position) (w->core.width -
			      wtemp->core.width - wtemp->core.border_width),
			     (Position) (-(wtemp->core.border_width) +
			  wtemp2->core.height + wtemp2->core.border_width));

		XtMoveWidget(wtemp2, -(Position) wtemp2->core.border_width,
			     -(Position) wtemp2->core.border_width);

		if (w->stPicture.outofview)
			XtMoveWidget(w->stPicture.outofview,
				     (Position) (w->core.width - clip_x), 0);
	}
	/* Left & Bottom */
	else if (!w->stPicture.useright && w->stPicture.usebottom) {
		XtMoveWidget(wtemp, -(Position) wtemp->core.border_width,
			     -(Position) wtemp->core.border_width);

		XtMoveWidget(wtemp2,
			     (Position) (-(wtemp2->core.border_width) +
			      wtemp->core.width + wtemp->core.border_width),
			     (Position) (w->core.height -
			  wtemp2->core.height - wtemp2->core.border_width));

		if (w->stPicture.outofview)
			XtMoveWidget(w->stPicture.outofview, 0,
				     (Position) (w->core.height - clip_y));
	}
	/* Left & Top */
	else if (!w->stPicture.useright && !w->stPicture.usebottom) {
		XtMoveWidget(wtemp, -(Position) wtemp->core.border_width,
			     (Position) (-(wtemp->core.border_width) +
			  wtemp2->core.height + wtemp2->core.border_width));

		XtMoveWidget(wtemp2,
			     (Position) (-(wtemp2->core.border_width) +
			      wtemp->core.width + wtemp->core.border_width),
			     -(Position) wtemp2->core.border_width);

		if (w->stPicture.outofview)
			XtMoveWidget(w->stPicture.outofview, 0, 0);
	}
}


static void 
SetGlobalAttr(w)
	Widget          w;
{
	StLineAtr       lineatr;
	StPaintAtr      paintatr;
	StTextAtr       textatr;

	lineatr.line_style = StlineSolid;
	lineatr.line_width = 1;
	lineatr.begin_cap = StcapButt;
	lineatr.end_cap = StcapButt;
	lineatr.join_style = StjoinMiter;
	lineatr.line_foreground = StCBlack;
	lineatr.line_background = StCWhite;
	lineatr.function = StGcopy;

	paintatr.fill_style = StfillHollow;
	paintatr.fill_foreground = StCBlack;
	paintatr.fill_background = StCWhite;

	textatr.font = StfontRoman8;
	textatr.text_color = StCBlack;
	textatr.function = StGcopy;

	StPicSetLineAtr(w, StNullFigure, &lineatr);
	StPicSetPaintAtr(w, StNullFigure, &paintatr);
	StPicSetTextAtr(w, StNullFigure, &textatr);
}


static void 
SetViewarea(w)
	StPictureWidget w;
{
	w->stPicture.orgin_x = 0;
	w->stPicture.orgin_y = 0;
	w->stPicture.view_w = w->core.width;
	w->stPicture.view_h = w->core.height;
}


static void 
SetupDrawControl(w)
	StPictureWidget w;
{
	StDrawControl  *dc;
	XGCValues       gcvalues;
	unsigned long   background, foreground;

	dc = GetDrawControl(w);
	dc->dpy = XtDisplay(w);
	dc->screen = XtScreen(w);
	dc->window = XtWindow(w);

	background = WhitePixel(dc->dpy, DefaultScreen(dc->dpy));
	foreground = BlackPixel(dc->dpy, DefaultScreen(dc->dpy));

	dc->colors = RealColor;
	dc->tiles = (char *) MapTable;

	InitializeFigureAttr(w);/* initiarize figure_gc */
	gcvalues.foreground = foreground ^ background;
	gcvalues.background = background;
	gcvalues.function = GXxor;
	gcvalues.line_width = 0;
	dc->handle_gc = XCreateGC(dc->dpy, dc->window,
				  GCFunction | GCForeground
				  | GCBackground | GCLineWidth,
				  &gcvalues);
	dc->edit_text = (StObject *) NULL;
	dc->text_buf = (char *) malloc(ST_DC_TEXT_BUF_SIZE);
	dc->text_index = -1;
	SetGlobalAttr(w);
	SetViewarea(w);
}

static void 
DestroyDrawControl(w)
	StPictureWidget w;
{
	StDrawControl  *dc = GetDrawControl(w);

	StPicReinitialize(w);
	XtFree((char *) dc->hash_tbl);	/* StPicReinitialize makes new hash
					 * table */
	XtFree((char *) dc->text_buf);	/* and temp text buffer also	    */
	XtFree((char *) dc);
}

static StDrawControl *
InitializeDrawControl(w)
	StPictureWidget w;
{
	StDrawControl  *dc;

	dc = (StDrawControl *) XtCalloc((unsigned) 1, sizeof(StDrawControl));
	/* dc->cur_menu = StpictureDrawEnd; */
	dc->drawstat = PicStartSelect;
	dc->image = (StDragImage *) NULL;
	dc->orgin.x = 0;
	dc->orgin.y = 0;
	/* dc->view_pos.x = 0; dc->view_pos.y = 0; */
	/* dc->view_w = ROOT_WIDTH; dc->view_h = ROOT_HEIGHT; */
	dc->hash_tbl = MakeHashTbl(HASH_ENTRY);
	dc->hash_size = HASH_ENTRY;
	dc->inq_buf = (char *) NULL;

	dc->n_group = dc->figure_no = 0;

	return dc;
}


static void 
Initialize(request, new)
	Widget          request, new;
{
	StPictureWidget spw = (StPictureWidget) new;
	int             i;
	String         *st;

	/* logic start */

	/***************************
	* initialize StPicturePart *
	***************************/
	if (spw->stPicture.file_name != NULL) {
		spw->stPicture.file_name =
			strcpy(XtMalloc(strlen(spw->stPicture.file_name) + 1),
			       spw->stPicture.file_name);
	}
	if (spw->stPicture.title != NULL) {
		spw->stPicture.title =
			strcpy(XtMalloc(strlen(spw->stPicture.title) + 1),
			       spw->stPicture.title);
	}
	if (spw->stPicture.title_draw != NULL) {
		spw->stPicture.title_draw =
			strcpy(XtMalloc(strlen(spw->stPicture.title_draw) + 1),
			       spw->stPicture.title_draw);
	}
	spw->stPicture.mode = StpictureNull;
	spw->stPicture.mode_draw = StdrawNull;
	spw->stPicture.mode_continue = False;
	spw->stPicture.write_file = NULL;
	spw->stPicture.read_file = NULL;
	spw->stPicture.image_file = NULL;

	spw->stPicture.dt = InitializeDrawControl(spw);	/* init drawcontrol  */

	/* set initial attribute datas flg */
	spw->stPicture.init_line = False;
	spw->stPicture.init_fill = False;
	spw->stPicture.init_char = False;
	spw->stPicture.localmap = NULL;

	/********************
	* check coor<->view *
	********************/
	if (spw->stPicture.protrusion)
		Vcheck(spw, 0);

	/*******************
	* create scrollbar *
	*******************/
	spw->stPicture.sbver = NULL;
	spw->stPicture.sbhor = NULL;
	spw->stPicture.outofview = NULL;
	spw->stPicture.logo1 = NULL;
	spw->stPicture.logo2 = NULL;
	if (spw->stPicture.forcebars)
		CreateScrollbar(spw);

	/****************************
	* create popup shell & item *
	****************************/
	spw->stPicture.entries_trans = _EntriesTrans;
	spw->stPicture.maked_entries = 0;
	spw->stPicture.popup = NULL;
	spw->stPicture.box = NULL;
	spw->stPicture.dummy = NULL;
	spw->stPicture.lab = NULL;
	spw->stPicture.timeid = 0;
	spw->stPicture.timeid_third = 0;

	spw->stPicture.entries_trans_draw = _EntriesTrans_draw;
	spw->stPicture.maked_entries_draw = 0;
	spw->stPicture.popup_draw = NULL;
	spw->stPicture.box_draw = NULL;
	spw->stPicture.dummy_draw = NULL;
	spw->stPicture.lab_draw = NULL;
	spw->stPicture.drawpopflg = False;

	spw->stPicture.entries_trans_attributes = _EntriesTrans_attributes;
	spw->stPicture.popup_attributes = NULL;
	spw->stPicture.attributespopflg = False;
	spw->stPicture.linepopflg = False;
	spw->stPicture.fillpopflg = False;
	spw->stPicture.charpopflg = False;

	spw->stPicture.entries_trans_group = _EntriesTrans_group;
	spw->stPicture.popup_group = NULL;
	spw->stPicture.grouppopflg = False;

	spw->stPicture.entries_trans_priority = _EntriesTrans_priority;
	spw->stPicture.popup_priority = NULL;
	spw->stPicture.prioritypopflg = False;

	spw->stPicture.entries_trans_rotate = _EntriesTrans_rotate;
	spw->stPicture.popup_rotate = NULL;
	spw->stPicture.rotatepopflg = False;

	spw->stPicture.entries_trans_write = _EntriesTrans_write;
	spw->stPicture.popup_write = NULL;
	spw->stPicture.writepopflg = False;

	spw->stPicture.entries_trans_read = _EntriesTrans_read;
	spw->stPicture.popup_read = NULL;
	spw->stPicture.readpopflg = False;

	spw->stPicture.entries_trans_image = _EntriesTrans_image;
	spw->stPicture.popup_image = NULL;
	spw->stPicture.imagepopflg = False;

	/* to separate stpicpopup.c */
	spw->stPicture.widthtable = WidthTable;
	spw->stPicture.linetypetable = LineTypeTable;
	spw->stPicture.linecaptable = LineCapTable;
	spw->stPicture.linejointable = LineJoinTable;
	spw->stPicture.colortable1 = ColorTable1;
	spw->stPicture.colortable2 = ColorTable2;
	spw->stPicture.realcolor = RealColor;
	spw->stPicture.functiontable = FunctionTable;
	spw->stPicture.charkindtable = CharKindTable;
	spw->stPicture.charsizetable = CharSizeTable;
	spw->stPicture.maptable = MapTable;

	if (spw->stPicture.entries) {
		for (i = 0, st = spw->stPicture.entries; *st != NULL; st++, i++);
		spw->stPicture.num_entries = i;
	} else
		spw->stPicture.num_entries = 0;
	if (spw->stPicture.num_entries)
		CreatePopup(spw);
}

static void 
Realize(w, valueMask, attributes)
	register Widget w;
	Mask           *valueMask;
	XSetWindowAttributes *attributes;
{
	StPictureWidget px = (StPictureWidget) w;

	/* logic start */

	*valueMask |= CWBitGravity;
	attributes->bit_gravity = NorthWestGravity;

	(*stPictureClassRec.core_class.superclass->core_class.realize)
		(w, valueMask, attributes);

	if (px->stPicture.outofview)
		XtRealizeWidget(px->stPicture.outofview);
	if (px->stPicture.sbver)
		XtRealizeWidget(px->stPicture.sbver);
	if (px->stPicture.sbhor)
		XtRealizeWidget(px->stPicture.sbhor);

	if (px->stPicture.forcebars) {
		if (px->stPicture.outofview)
			XtMapWidget(px->stPicture.outofview);
		XtMapWidget(px->stPicture.sbver);
		XtMapWidget(px->stPicture.sbhor);
	}
	SetupDrawControl(px);	/* setup GC for drawing */

	(*stPictureClassRec.core_class.resize) (w);

	/*
	 * revised 
	 */

	if (px->stPicture.forcebars) {
		if (!px->stPicture.useright)
			px->stPicture.orgin_x = px->stPicture.sbver->core.width +
				px->stPicture.sbver->core.border_width;
		if (!px->stPicture.usebottom)
			px->stPicture.orgin_y = -(px->stPicture.sbhor->core.height +
				    px->stPicture.sbhor->core.border_width);
	}
	
	if ( px->stPicture.file_name != NULL ) {
		StPicReadFile((Widget)px, px->stPicture.file_name);
		StPicRedrawAll((Widget)px);
	}
}


static void 
Destroy(w)
	Widget          w;
{
	StPictureWidget spw = (StPictureWidget) w;
	XtCallbackStatus status;

	DestroyDrawControl(spw);/* Free DrawControl area */

	if (spw->stPicture.timeid)
		XtRemoveTimeOut(spw->stPicture.timeid);
	if (spw->stPicture.timeid_third)
		XtRemoveTimeOut(spw->stPicture.timeid_third);

	if ((status = XtHasCallbacks(w, StNtempCallback)) != XtCallbackNoList)
		XtRemoveAllCallbacks(w, StNtempCallback);

	if (spw->stPicture.file_name)
		XtFree(spw->stPicture.file_name);
	if (spw->stPicture.title)
		XtFree(spw->stPicture.title);
	if (spw->stPicture.title_draw)
		XtFree(spw->stPicture.title_draw);
	if (spw->stPicture.write_file)
		XtFree(spw->stPicture.write_file);
	if (spw->stPicture.read_file)
		XtFree(spw->stPicture.read_file);
	if (spw->stPicture.image_file)
		XtFree(spw->stPicture.image_file);

	if (spw->stPicture.localmap)
		XFreePixmap(XtDisplay(w), spw->stPicture.localmap);

	if (spw->stPicture.sbver)
		XtDestroyWidget(spw->stPicture.sbver);
	if (spw->stPicture.sbhor)
		XtDestroyWidget(spw->stPicture.sbhor);
	if (spw->stPicture.outofview)
		XtDestroyWidget(spw->stPicture.outofview);
	if (spw->stPicture.logo1)
		XtDestroyWidget(spw->stPicture.logo1);
	if (spw->stPicture.logo2)
		XtDestroyWidget(spw->stPicture.logo2);

	if (spw->stPicture.item)
		XtFree((char *) spw->stPicture.item);
	if (spw->stPicture.item_draw)
		XtFree((char *) spw->stPicture.item_draw);
}


static void 
Resize(w)
	Widget          w;
{
	StPictureWidget spw = (StPictureWidget) w;
	Widget          ver = spw->stPicture.sbver;
	Widget          hor = spw->stPicture.sbhor;
	int             view_width, view_height;
	int             keep_width, keep_height;
	int             over_width, over_height;
	Arg             args[5];
	Pixel           white;

	keep_width = spw->core.width;
	keep_height = spw->core.height;

	if (spw->stPicture.logo1) {
		XtDestroyWidget(spw->stPicture.logo1);
		spw->stPicture.logo1 = NULL;
	}
	if (spw->stPicture.logo2) {
		XtDestroyWidget(spw->stPicture.logo2);
		spw->stPicture.logo2 = NULL;
	}
	if (spw->stPicture.protrusion) {
		/*******/
		if (spw->stPicture.forcebars) {
			view_width = spw->core.width -
				ver->core.width - ver->core.border_width;
			view_height = spw->core.height -
				hor->core.height - hor->core.border_width;
		} else {
			view_width = spw->core.width;
			view_height = spw->core.height;
		}

		over_width = spw->stPicture.view_x + view_width -
			spw->stPicture.coor_xmax;
		over_height = spw->stPicture.view_y + view_height -
			spw->stPicture.coor_ymax;
		if (over_width > 0 && over_height > 0) {
			white = WhitePixel(XtDisplay(w), DefaultScreen(XtDisplay(w)));
			if (spw->core.background_pixel == white)
				XtSetArg(args[0], XtNreverseVideo, True);
			else
				XtSetArg(args[0], XtNreverseVideo, False);
			XtSetArg(args[1], XtNborderWidth, 0);
			XtSetArg(args[2], XtNwidth, over_width);
			XtSetArg(args[3], XtNheight, keep_height - over_height);
			spw->stPicture.logo1 =
				XtCreateWidget("logo1", logoWidgetClass, w, args, FOUR);
			XtOverrideTranslations(spw->stPicture.logo1,
				 XtParseTranslationTable(coreTranslations));

			XtSetArg(args[2], XtNwidth, keep_width);
			XtSetArg(args[3], XtNheight, over_height);
			spw->stPicture.logo2 =
				XtCreateWidget("logo2", logoWidgetClass, w, args, FOUR);
			XtOverrideTranslations(spw->stPicture.logo2,
				 XtParseTranslationTable(coreTranslations));

			XtMoveWidget(spw->stPicture.logo1,
				     (Position) (keep_width - over_width),
				     (Position) 0);
			XtRealizeWidget(spw->stPicture.logo1);
			XtMapWidget(spw->stPicture.logo1);
			XtMoveWidget(spw->stPicture.logo2,
				     (Position) 0,
				     (Position) (keep_height - over_height));
			XtRealizeWidget(spw->stPicture.logo2);
			XtMapWidget(spw->stPicture.logo2);
			spw->core.width = spw->core.width - over_width;
			spw->core.height = spw->core.height - over_height;
		} else if (over_width > 0) {
			white = WhitePixel(XtDisplay(w), DefaultScreen(XtDisplay(w)));
			if (spw->core.background_pixel == white)
				XtSetArg(args[0], XtNreverseVideo, True);
			else
				XtSetArg(args[0], XtNreverseVideo, False);
			XtSetArg(args[1], XtNborderWidth, 0);
			XtSetArg(args[2], XtNwidth, over_width);
			XtSetArg(args[3], XtNheight, keep_height);
			spw->stPicture.logo1 =
				XtCreateWidget("logo1", logoWidgetClass, w, args, FOUR);
			XtOverrideTranslations(spw->stPicture.logo1,
				 XtParseTranslationTable(coreTranslations));
			XtMoveWidget(spw->stPicture.logo1,
				     (Position) (keep_width - over_width),
				     (Position) 0);
			XtRealizeWidget(spw->stPicture.logo1);
			XtMapWidget(spw->stPicture.logo1);
			spw->core.width = spw->core.width - over_width;
		} else if (over_height > 0) {
			white = WhitePixel(XtDisplay(w), DefaultScreen(XtDisplay(w)));
			if (spw->core.background_pixel == white)
				XtSetArg(args[0], XtNreverseVideo, True);
			else
				XtSetArg(args[0], XtNreverseVideo, False);
			XtSetArg(args[1], XtNborderWidth, 0);
			XtSetArg(args[2], XtNwidth, keep_width);
			XtSetArg(args[3], XtNheight, over_height);
			spw->stPicture.logo2 =
				XtCreateWidget("logo2", logoWidgetClass, w, args, FOUR);
			XtOverrideTranslations(spw->stPicture.logo2,
				 XtParseTranslationTable(coreTranslations));
			XtMoveWidget(spw->stPicture.logo2,
				     (Position) 0,
				     (Position) keep_height - over_height);
			XtRealizeWidget(spw->stPicture.logo2);
			XtMapWidget(spw->stPicture.logo2);
			spw->core.height = spw->core.height - over_height;
		}
		/*******/
	}
	if (spw->stPicture.forcebars)
		ResizeScrollbar(spw);

	spw->core.width = keep_width;
	spw->core.height = keep_height;

	/*
	 * revised 
	 */

	spw->stPicture.view_w = spw->core.width;
	spw->stPicture.view_h = spw->core.height;
	if (spw->stPicture.forcebars) {
		spw->stPicture.view_w -= spw->stPicture.sbver->core.width +
			spw->stPicture.sbver->core.border_width;
		spw->stPicture.view_h -= spw->stPicture.sbhor->core.height +
			spw->stPicture.sbhor->core.border_width;
	}
	if (spw->stPicture.logo1)
		spw->stPicture.view_w -= spw->stPicture.logo1->core.width;
	if (spw->stPicture.logo2)
		spw->stPicture.view_h -= spw->stPicture.logo2->core.height;
}


/*
 * Repaint the widget window 
 */
static void 
Redisplay(w, event, region)
	Widget          w;
	XEvent         *event;
	Region          region;	/* NULL */
{
	StPictureWidget spw = (StPictureWidget) w;
	XExposeEvent   *ev = (XExposeEvent *) event;
	Widget          ver = spw->stPicture.sbver;
	Widget          hor = spw->stPicture.sbhor;
	int             x_max, x_min, y_max, y_min;
	StRegion /* XRectangle */ reg;
	StFigureList   *flst = (StFigureList *) NULL;
	register        i;

	if (spw->stPicture.forcebars) {
		if (spw->stPicture.useright && spw->stPicture.usebottom) {
			x_min = 0;
			x_max = spw->core.width -
				ver->core.width - ver->core.border_width;
			y_min = 0;
			y_max = spw->core.height -
				hor->core.height - hor->core.border_width;
		} else if (spw->stPicture.useright && !spw->stPicture.usebottom) {
			x_min = 0;
			x_max = spw->core.width -
				ver->core.width - ver->core.border_width;
			y_min = hor->core.height + hor->core.border_width;
			y_max = spw->core.height;
		} else if (!spw->stPicture.useright && spw->stPicture.usebottom) {
			x_min = ver->core.width + ver->core.border_width;
			x_max = spw->core.width;
			y_min = 0;
			y_max = spw->core.height -
				hor->core.height - hor->core.border_width;
		} else if (!spw->stPicture.useright && !spw->stPicture.usebottom) {
			x_min = ver->core.width + ver->core.border_width;
			x_max = spw->core.width;
			y_min = hor->core.height + hor->core.border_width;
			y_max = spw->core.height;
		}
	} else {
		x_min = 0;
		x_max = spw->core.width;
		y_min = 0;
		y_max = spw->core.height;
	}

	if (spw->stPicture.protrusion) {
		if (spw->stPicture.view_x + (x_max - x_min) >
		    spw->stPicture.coor_xmax)
			x_max = spw->stPicture.coor_xmax -
				spw->stPicture.view_x + x_min;
		if (spw->stPicture.view_y + (y_max - y_min) >
		    spw->stPicture.coor_ymax)
			y_max = spw->stPicture.coor_ymax -
				spw->stPicture.view_y + y_min;
	}
	if (ev->x < x_min || ev->x >= x_max ||
	    ev->y < y_min || ev->y >= y_max)
		return;

	reg.x = ev->x;
	reg.y = ev->y;
	reg.width = ev->width;
	reg.height = ev->height;

	StPicGetSelectedFigure(w, &flst);
	SelectAll(w, StunsetSelect);
	StPicRedrawPart(w, &reg, StCoordEvent);
	if (flst) {
		for (i = 0; i < flst->n_figures; i++)
			StPicSelectFigure(w, flst->figures[i],
					  StsetSelect, StsetHandle);
	}
}


/*
 * Set specified arguments into widget 
 */
static Boolean 
SetValues(current, request, new)
	Widget          current, request, new;
{
	StPictureWidget oldw = (StPictureWidget) current;
	StPictureWidget spw = (StPictureWidget) new;
	Boolean         redisplay = False;
	Widget          parent;
	Arg             args[2];
	int             i;
	String         *st;

	if (oldw->stPicture.file_name != spw->stPicture.file_name) {
		XtFree((char *) oldw->stPicture.file_name);
		if (spw->stPicture.file_name)
			spw->stPicture.file_name =
				strcpy(XtMalloc(strlen(spw->stPicture.file_name) + 1),
				       spw->stPicture.file_name);
		if (spw->stPicture.popup_write) {
			parent = spw->stPicture.popup_write->core.parent;
			XtDestroyWidget(spw->stPicture.popup_write);
			CreatePopupWrite(parent);
		}
	}
	if (oldw->stPicture.title_draw != spw->stPicture.title_draw ||
	    oldw->stPicture.entries_draw != spw->stPicture.entries_draw ||
	  oldw->stPicture.functions_draw != spw->stPicture.functions_draw) {
		if (oldw->stPicture.title_draw != spw->stPicture.title_draw) {
			XtFree((char *) oldw->stPicture.title_draw);
			if (spw->stPicture.title_draw)
				spw->stPicture.title_draw =
					strcpy(XtMalloc(strlen(spw->stPicture.title_draw) + 1),
					       spw->stPicture.title_draw);
		}
		if (oldw->stPicture.entries_draw !=
		    spw->stPicture.entries_draw) {
			if (spw->stPicture.entries_draw) {
				for (i = 0, st = spw->stPicture.entries_draw;
				     *st != NULL; st++, i++);
				spw->stPicture.num_entries_draw = i;
			} else
				spw->stPicture.num_entries_draw = 0;
		}
		if (spw->stPicture.num_entries_draw &&
		    spw->stPicture.popup_draw) {
			parent = spw->stPicture.popup_draw->core.parent;
			CreatePopupDraw(parent);
		}
	}
	if (oldw->stPicture.title != spw->stPicture.title ||
	    oldw->stPicture.entries != spw->stPicture.entries ||
	    oldw->stPicture.functions != spw->stPicture.functions) {
		if (oldw->stPicture.title != spw->stPicture.title) {
			XtFree((char *) oldw->stPicture.title);
			if (spw->stPicture.title)
				spw->stPicture.title =
					strcpy(XtMalloc(strlen(spw->stPicture.title) + 1),
					       spw->stPicture.title);
		}
		if (oldw->stPicture.entries != spw->stPicture.entries) {
			if (spw->stPicture.entries) {
				for (i = 0, st = spw->stPicture.entries;
				     *st != NULL; st++, i++);
				spw->stPicture.num_entries = i;
			} else
				spw->stPicture.num_entries = 0;
		}
		if (spw->stPicture.num_entries)
			CreatePopup(spw);
	}
	/********/

	if (oldw->stPicture.protrusion && spw->stPicture.protrusion) {
		if (oldw->stPicture.coor_xmax != spw->stPicture.coor_xmax ||
		    oldw->stPicture.coor_xmin != spw->stPicture.coor_xmin ||
		    oldw->stPicture.coor_ymax != spw->stPicture.coor_ymax ||
		    oldw->stPicture.coor_ymin != spw->stPicture.coor_ymin ||
		    oldw->stPicture.view_x != spw->stPicture.view_x ||
		    oldw->stPicture.view_y != spw->stPicture.view_y) {
			if (Vcheck(spw, 1)) {	/* error */
				XtWarning("StPicture Resource Error:coordinate**** & view* -> no change");
				spw->stPicture.coor_xmax = oldw->stPicture.coor_xmax;
				spw->stPicture.coor_xmin = oldw->stPicture.coor_xmin;
				spw->stPicture.coor_ymax = oldw->stPicture.coor_ymax;
				spw->stPicture.coor_ymin = oldw->stPicture.coor_ymin;
				spw->stPicture.view_x = oldw->stPicture.view_x;
				spw->stPicture.view_y = oldw->stPicture.view_y;
			}
			if (oldw->stPicture.forcebars &&
			    !spw->stPicture.forcebars) {
				if (XtIsRealized(new)) {
					XtUnmapWidget(spw->stPicture.sbver);
					XtUnmapWidget(spw->stPicture.sbhor);
					if (spw->stPicture.outofview)
						XtUnmapWidget(spw->stPicture.outofview);
				}
			} else if (!oldw->stPicture.forcebars &&
				   spw->stPicture.forcebars) {
				if (!spw->stPicture.sbver) {
					CreateScrollbar(spw);
					if (XtIsRealized(new)) {
						XtRealizeWidget(spw->stPicture.sbver);
						XtMapWidget(spw->stPicture.sbver);
						XtRealizeWidget(spw->stPicture.sbhor);
						XtMapWidget(spw->stPicture.sbhor);
					}
				} else {
					if (XtIsRealized(new)) {
						XtMapWidget(spw->stPicture.sbver);
						XtMapWidget(spw->stPicture.sbhor);
						if (spw->stPicture.outofview)
							XtMapWidget(spw->stPicture.outofview);
					}
				}
			}
			if (XtIsRealized(new))
				(*stPictureClassRec.core_class.resize) (new);
		} else {
			if (oldw->stPicture.forcebars &&
			    !spw->stPicture.forcebars) {
				if (XtIsRealized(new)) {
					XtUnmapWidget(spw->stPicture.sbver);
					XtUnmapWidget(spw->stPicture.sbhor);
					if (spw->stPicture.outofview)
						XtUnmapWidget(spw->stPicture.outofview);
				}
			} else if (oldw->stPicture.forcebars &&
				   spw->stPicture.forcebars) {
				if (oldw->stPicture.usebottom !=
				    spw->stPicture.usebottom ||
				    oldw->stPicture.useright !=
				    spw->stPicture.useright) {
					if (XtIsRealized(new))
						(*stPictureClassRec.core_class.resize)
							(new);
				}
			} else if (!oldw->stPicture.forcebars &&
				   spw->stPicture.forcebars) {
				if (!spw->stPicture.sbver) {
					CreateScrollbar(spw);
					if (XtIsRealized(new)) {
						XtRealizeWidget(spw->stPicture.sbver);
						XtMapWidget(spw->stPicture.sbver);
						XtRealizeWidget(spw->stPicture.sbhor);
						XtMapWidget(spw->stPicture.sbhor);
					}
				} else {
					if (XtIsRealized(new)) {
						XtMapWidget(spw->stPicture.sbver);
						XtMapWidget(spw->stPicture.sbhor);
						if (spw->stPicture.outofview)
							XtMapWidget(spw->stPicture.outofview);
					}
				}

				if (XtIsRealized(new))
					(*stPictureClassRec.core_class.resize) (new);
			}
		}
	} else if (oldw->stPicture.protrusion && !spw->stPicture.protrusion) {
		if (oldw->stPicture.forcebars && !spw->stPicture.forcebars) {
			if (XtIsRealized(new)) {
				XtUnmapWidget(spw->stPicture.sbver);
				XtUnmapWidget(spw->stPicture.sbhor);
				if (spw->stPicture.outofview)
					XtUnmapWidget(spw->stPicture.outofview);
			}
		} else if (!oldw->stPicture.forcebars &&
			   spw->stPicture.forcebars) {
			if (!spw->stPicture.sbver) {
				CreateScrollbar(spw);
				if (XtIsRealized(new)) {
					XtRealizeWidget(spw->stPicture.sbver);
					XtMapWidget(spw->stPicture.sbver);
					XtRealizeWidget(spw->stPicture.sbhor);
					XtMapWidget(spw->stPicture.sbhor);
				}
			} else {
				if (XtIsRealized(new)) {
					XtMapWidget(spw->stPicture.sbver);
					XtMapWidget(spw->stPicture.sbhor);
					if (spw->stPicture.outofview)
						XtMapWidget(spw->stPicture.outofview);
				}
			}
		}
		if (XtIsRealized(new))
			(*stPictureClassRec.core_class.resize) (new);
	} else if (!oldw->stPicture.protrusion && spw->stPicture.protrusion) {
		if (oldw->stPicture.coor_xmax != spw->stPicture.coor_xmax ||
		    oldw->stPicture.coor_xmin != spw->stPicture.coor_xmin ||
		    oldw->stPicture.coor_ymax != spw->stPicture.coor_ymax ||
		    oldw->stPicture.coor_ymin != spw->stPicture.coor_ymin ||
		    oldw->stPicture.view_x != spw->stPicture.view_x ||
		    oldw->stPicture.view_y != spw->stPicture.view_y) {
			if (Vcheck(spw, 1)) {	/* error */
				XtWarning("StPicture Resource Error:coordinate**** & view* -> no change");
				spw->stPicture.coor_xmax = oldw->stPicture.coor_xmax;
				spw->stPicture.coor_xmin = oldw->stPicture.coor_xmin;
				spw->stPicture.coor_ymax = oldw->stPicture.coor_ymax;
				spw->stPicture.coor_ymin = oldw->stPicture.coor_ymin;
				spw->stPicture.view_x = oldw->stPicture.view_x;
				spw->stPicture.view_y = oldw->stPicture.view_y;
				Vcheck(spw, 0);
			}
		} else
			Vcheck(spw, 0);

		if (oldw->stPicture.forcebars && !spw->stPicture.forcebars) {
			if (XtIsRealized(new)) {
				XtUnmapWidget(spw->stPicture.sbver);
				XtUnmapWidget(spw->stPicture.sbhor);
				if (spw->stPicture.outofview)
					XtUnmapWidget(spw->stPicture.outofview);
			}
		} else if (!oldw->stPicture.forcebars &&
			   spw->stPicture.forcebars) {
			if (!spw->stPicture.sbver) {
				CreateScrollbar(spw);
				if (XtIsRealized(new)) {
					XtRealizeWidget(spw->stPicture.sbver);
					XtMapWidget(spw->stPicture.sbver);
					XtRealizeWidget(spw->stPicture.sbhor);
					XtMapWidget(spw->stPicture.sbhor);
				}
			} else {
				if (XtIsRealized(new)) {
					XtMapWidget(spw->stPicture.sbver);
					XtMapWidget(spw->stPicture.sbhor);
					if (spw->stPicture.outofview)
						XtMapWidget(spw->stPicture.outofview);
				}
			}
		}
		if (XtIsRealized(new))
			(*stPictureClassRec.core_class.resize) (new);
	} else if (!oldw->stPicture.protrusion && !spw->stPicture.protrusion) {
		if (oldw->stPicture.forcebars && !spw->stPicture.forcebars) {
			if (XtIsRealized(new)) {
				XtUnmapWidget(spw->stPicture.sbver);
				XtUnmapWidget(spw->stPicture.sbhor);
				if (spw->stPicture.outofview)
					XtUnmapWidget(spw->stPicture.outofview);
			}
		} else if (oldw->stPicture.forcebars &&
			   spw->stPicture.forcebars) {
			if (oldw->stPicture.usebottom !=
			    spw->stPicture.usebottom ||
			    oldw->stPicture.useright !=
			    spw->stPicture.useright) {
				if (XtIsRealized(new))
					ResizeScrollbar(spw);
			}
		} else if (!oldw->stPicture.forcebars &&
			   spw->stPicture.forcebars) {
			if (!spw->stPicture.sbver) {
				CreateScrollbar(spw);
				if (XtIsRealized(new)) {
					XtRealizeWidget(spw->stPicture.sbver);
					XtMapWidget(spw->stPicture.sbver);
					XtRealizeWidget(spw->stPicture.sbhor);
					XtMapWidget(spw->stPicture.sbhor);
				}
			} else {
				if (XtIsRealized(new)) {
					XtMapWidget(spw->stPicture.sbver);
					XtMapWidget(spw->stPicture.sbhor);
					if (spw->stPicture.outofview)
						XtMapWidget(spw->stPicture.outofview);
				}
			}

			if (XtIsRealized(new))
				ResizeScrollbar(spw);
		}
	}
	/* redisplay or not */
	if (oldw->stPicture.forcebars && !spw->stPicture.forcebars ||	/* 1 */
	    (oldw->stPicture.forcebars && spw->stPicture.forcebars &&
	     (oldw->stPicture.usebottom != spw->stPicture.usebottom ||
	      oldw->stPicture.useright != spw->stPicture.useright)) ||	/* 2 */
	    !oldw->stPicture.forcebars && spw->stPicture.forcebars ||	/* 3 */
	    (oldw->stPicture.protrusion && spw->stPicture.protrusion &&
	     (oldw->stPicture.view_x != spw->stPicture.view_x ||
	      oldw->stPicture.view_y != spw->stPicture.view_y)) ||	/* 4 */
	    !oldw->stPicture.protrusion && spw->stPicture.protrusion)	/* 5 */
		redisplay = True;

	/*
	 * revised 
	 */

	if (oldw->stPicture.forcebars != spw->stPicture.forcebars ||
	    oldw->stPicture.usebottom != spw->stPicture.usebottom ||
	    oldw->stPicture.useright != spw->stPicture.useright) {
		if (spw->stPicture.forcebars) {
			if (!spw->stPicture.useright)
				spw->stPicture.orgin_x =
					spw->stPicture.sbver->core.width +
					spw->stPicture.sbver->core.border_width;
			else
				spw->stPicture.orgin_x = 0;
			if (!spw->stPicture.usebottom)
				spw->stPicture.orgin_y =
					-(spw->stPicture.sbhor->core.height +
				   spw->stPicture.sbhor->core.border_width);
			else
				spw->stPicture.orgin_y = 0;
		} else {
			spw->stPicture.orgin_x = 0;
			spw->stPicture.orgin_y = 0;
		}
	}
	spw->stPicture.view_w = spw->core.width;
	spw->stPicture.view_h = spw->core.height;
	if (spw->stPicture.forcebars) {
		spw->stPicture.view_w -= spw->stPicture.sbver->core.width +
			spw->stPicture.sbver->core.border_width;
		spw->stPicture.view_h -= spw->stPicture.sbhor->core.height +
			spw->stPicture.sbhor->core.border_width;
	}
	if (spw->stPicture.logo1)
		spw->stPicture.view_w -= spw->stPicture.logo1->core.width;
	if (spw->stPicture.logo2)
		spw->stPicture.view_h -= spw->stPicture.logo2->core.height;

	return redisplay;
}

/***************************
*
*  Action Procedures
*
***************************/

static void 
DeleteCharacter(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	if (dc->drawstat == PicInTextDraw || dc->drawstat == PicSelectText)
		TextEditing(w, KeyDelete, (char) NULL);
}

static void 
DeletePreviousCharacter(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	if (dc->drawstat == PicInTextDraw || dc->drawstat == PicSelectText)
		TextEditing(w, KeyDeletePrivious, (char) NULL);
}

static void 
EraseToEndOfLine(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	if (dc->drawstat == PicInTextDraw || dc->drawstat == PicSelectText)
		TextEditing(w, KeyEraseToEnd, (char) NULL);
}

static void 
Newline(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	if (dc->drawstat == PicInTextDraw || dc->drawstat == PicSelectText)
		TextEditing(w, KeyNewline, (char) NULL);
}

static void 
EraseLine(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	if (dc->drawstat == PicInTextDraw || dc->drawstat == PicSelectText)
		TextEditing(w, KeyEraseLine, (char) NULL);
}

static void 
InsertChar(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);
	char            tmp_buf[8];
	register        i;

	if (dc->drawstat == PicInTextDraw || dc->drawstat == PicSelectText) {
		tmp_buf[0] = '\0';
		XLookupString(((XKeyEvent *) event), tmp_buf,
			      sizeof(tmp_buf), NULL, NULL);
		TextEditing(w, KeyInsertChar, tmp_buf[0]);
	}
}
static void 
NotifyBefore(w, event, params, num_params)
	Widget          w;	/* StPicture Widget */
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StPictureWidget spw = (StPictureWidget) w;
	int             i, num, num2;
	Widget         *itemPtr;
	XtCallbackList *temp;
	StPictureFunction *st;

	if (!strcmp(*params, "menu-function")) {
		for (num = 0, itemPtr = spw->stPicture.item; 1; num++)
			if (itemPtr[num] == spw->stPicture.callback_item)
				break;

		if (spw->stPicture.callback_item_draw &&
		    spw->stPicture.before_draw) {
			for (num2 = 0, itemPtr = spw->stPicture.item_draw; 1;
			     num2++)
				if (itemPtr[num2] ==
				    spw->stPicture.callback_item_draw)
					break;
			for (i = 0, temp = spw->stPicture.before_draw;
			     *temp != StcallbacksEnd; i++, temp++);
			if (num2 < i && spw->stPicture.before_draw[num2]) {
				XtRemoveAllCallbacks(w, StNtempCallback);
				XtAddCallbacks(w, StNtempCallback,
					  spw->stPicture.before_draw[num2]);
				XtCallCallbacks(w, StNtempCallback, NULL);
			}
		}
		if (spw->stPicture.before) {
			for (i = 0, temp = spw->stPicture.before;
			     *temp != StcallbacksEnd; i++, temp++);
			if (num < i && spw->stPicture.before[num]) {
				XtRemoveAllCallbacks(w, StNtempCallback);
				XtAddCallbacks(w, StNtempCallback,
					       spw->stPicture.before[num]);
				XtCallCallbacks(w, StNtempCallback, NULL);
			}
		}
	} else if (!strcmp(*params, "move-hand")) {
		if (!spw->stPicture.before)
			return;
		if (!spw->stPicture.mode_continue ||
		    spw->stPicture.mode != StpictureMove)
			return;

		if (!spw->stPicture.functions)
			return;
		for (num = 0, st = spw->stPicture.functions;
		     *st != StpictureEnd; st++, num++)
			if (*st == StpictureMove)
				break;
		if (*st == StpictureEnd)
			return;

		for (i = 0, temp = spw->stPicture.before;
		     *temp != StcallbacksEnd; i++, temp++);
		if (num < i && spw->stPicture.before[num]) {
			XtRemoveAllCallbacks(w, StNtempCallback);
			XtAddCallbacks(w, StNtempCallback,
				       spw->stPicture.before[num]);
			XtCallCallbacks(w, StNtempCallback, NULL);
		}
	}
}


static void 
MoveHand(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicStartDraw:
	case PicStartSelect:
	case PicStartMove:
	case PicStartCopy:
	case PicStartResize:
		SaveOldDrawStat(w);
		SavePoint(w, event);
		if (IsMoveAction(w, event))
			SetDrawStat(w, PicInMove_SB1);
		else {
			SetDrawStat(w, PicInScrool);
			StartScrool(w);
		}
		spw->stPicture.mode_continue = True;
		break;
	case PicInScrool:
		RestorOldDrawStat(w);
		FixupScrool(w, event);
		spw->stPicture.mode_continue = False;
		break;
	case PicInMove_SB1:
		RestorOldDrawStat(w);
		FixupMoveObject(w, event);
		spw->stPicture.mode_continue = False;
		break;
	default:
		;
	}
}


static void 
NotifyAfter(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StPictureWidget spw = (StPictureWidget) w;
	int             i, num, num2;
	Widget         *itemPtr;
	XtCallbackList *temp;
	StPictureFunction *st;
	StDrawControl  *dc = GetDrawControl(w);

	if (spw->stPicture.mode_continue)
		return;

	if (!strcmp(*params, "move-hand")) {
		if (spw->stPicture.mode != StpictureMove)
			return;

		if (!spw->stPicture.after ||
		    !spw->stPicture.functions)
			goto fin;
		for (num = 0, st = spw->stPicture.functions;
		     *st != StpictureEnd; st++, num++)
			if (*st == StpictureMove)
				break;
		if (*st == StpictureEnd)
			goto fin;

		for (i = 0, temp = spw->stPicture.after;
		     *temp != StcallbacksEnd; i++, temp++);
		if (num < i && spw->stPicture.after[num]) {
			XtRemoveAllCallbacks(w, StNtempCallback);
			XtAddCallbacks(w, StNtempCallback,
				       spw->stPicture.after[num]);
			XtCallCallbacks(w, StNtempCallback, NULL);
		}
	} else if (!strcmp(*params, "select-end") ||
		   !strcmp(*params, "pline-end")) {
		if (!strcmp(*params, "select-end")) {
			if (spw->stPicture.mode_draw == StdrawPolyline ||
			    spw->stPicture.mode_draw == StdrawPolygon)
				return;
			if (spw->stPicture.mode != StpictureDraw &&
			    spw->stPicture.mode != StpictureCopy &&
			    spw->stPicture.mode != StpictureMove &&
			    spw->stPicture.mode != StpictureResize)
				goto fin;
		} else {
			if (spw->stPicture.mode != StpictureDraw ||
			    (spw->stPicture.mode_draw != StdrawPolyline &&
			     spw->stPicture.mode_draw != StdrawPolygon))
				return;
		}

		for (num = 0, itemPtr = spw->stPicture.item; 1; num++)
			if (itemPtr[num] == spw->stPicture.callback_item)
				break;

		if (spw->stPicture.callback_item_draw &&
		    spw->stPicture.after_draw) {
			for (num2 = 0, itemPtr = spw->stPicture.item_draw; 1;
			     num2++)
				if (itemPtr[num2] ==
				    spw->stPicture.callback_item_draw)
					break;
			for (i = 0, temp = spw->stPicture.after_draw;
			     *temp != StcallbacksEnd; i++, temp++);
			if (num2 < i && spw->stPicture.after_draw[num2]) {
				XtRemoveAllCallbacks(w, StNtempCallback);
				XtAddCallbacks(w, StNtempCallback,
					   spw->stPicture.after_draw[num2]);
				XtCallCallbacks(w, StNtempCallback, NULL);
			}
		}
		if (spw->stPicture.after) {
			for (i = 0, temp = spw->stPicture.after;
			     *temp != StcallbacksEnd; i++, temp++);
			if (num < i && spw->stPicture.after[num]) {
				XtRemoveAllCallbacks(w, StNtempCallback);
				XtAddCallbacks(w, StNtempCallback,
					       spw->stPicture.after[num]);
				XtCallCallbacks(w, StNtempCallback, NULL);
			}
		}
	} else if (!strcmp(*params, "menu-function")) {
		for (num = 0, itemPtr = spw->stPicture.item; 1; num++)
			if (itemPtr[num] == spw->stPicture.callback_item)
				break;

		if (spw->stPicture.after) {
			for (i = 0, temp = spw->stPicture.after;
			     *temp != StcallbacksEnd; i++, temp++);
			if (num < i && spw->stPicture.after[num]) {
				XtRemoveAllCallbacks(w, StNtempCallback);
				XtAddCallbacks(w, StNtempCallback,
					       spw->stPicture.after[num]);
				XtCallCallbacks(w, StNtempCallback, NULL);
			}
		}
	}
fin:	if (spw->stPicture.mode != StpictureDraw) {
		spw->stPicture.mode = StpictureDrawEnd;
		spw->stPicture.mode_draw = StdrawNull;
		if (dc->drawstat != PicSelectText)
			SetDrawStat(spw, PicStartSelect);
	}
}


static void 
SelectStart(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicStartDraw:
		SavePoint(w, event);
		spw->stPicture.mode_continue = True;
		break;
	case PicStartSelect:
		SavePoint(w, event);
		SetDrawStat(w, PicClickSelect);
		break;
	case PicStartMove:
	case PicStartCopy:
	case PicStartResize:
		SavePoint(w, event);
		spw->stPicture.mode_continue = True;
		break;
	case PicInDraw:
		SavePoint(w, event);
		SetDrawStat(w, PicEndDraw);
		break;
	case PicInPolygon:
		SavePoint(w, event);
		break;
	case PicInArcStep1:
		SavePoint(w, event);
		SetDrawStat(w, PicEndFinArc);
		break;
	case PicSetBeginArc:
		SavePoint(w, event);
		SetDrawStat(w, PicEndBeginArc);
		break;
	case PicSetFinArc:
		SavePoint(w, event);
		SetDrawStat(w, PicEndFinArc);
		break;
	case PicInMove:
		SetDrawStat(w, PicEndMove);
		break;
	case PicInCopy:
		SetDrawStat(w, PicEndCopy);
		break;
	case PicInResize:
		SetDrawStat(w, PicEndResize);
		break;
	case PicInTextDraw:
		SetDrawStat(w, PicEndTextDraw);
		break;
	default:
		;
	}
}


static void 
SelectAdjust(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicClickSelect:
		SetDrawStat(w, PicInSelect);
	case PicInSelect:
		SavePoint(w, event);
		HandleSelect(w);
		break;
	default:
		;
	}
}


static void 
SelectEnd(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicStartDraw:
		switch ((int) spw->stPicture.mode_draw) {
		case StdrawPolygon:
		case StdrawPolyline:
			SetDrawStat(w, PicInPolygon);
			break;
		case StdrawArc:
		case StdrawEllipticArc:
		case StdrawPie:
		case StdrawEllipticPie:
			SetDrawStat(w, PicInArcStep1);
			break;
		case StdrawText:
			CurrentTextStatus(w, event);
			break;
		default:
			SetDrawStat(w, PicInDraw);
		}
		break;
	case PicStartMove:
		if (IsMoveAction(w))
			SetDrawStat(w, PicInMove);
		else {
			SetDrawStat(w, PicStartDraw);
			spw->stPicture.mode_continue = False;
		}
		break;
	case PicStartCopy:
		if (IsMoveAction(w))
			SetDrawStat(w, PicInCopy);
		else {
			SetDrawStat(w, PicStartDraw);
			spw->stPicture.mode_continue = False;
		}
		break;
	case PicStartResize:
		if (IsResizeAction(w))
			SetDrawStat(w, PicInResize);
		else {
			SetDrawStat(w, PicStartDraw);
			spw->stPicture.mode_continue = False;
		}
		break;
	case PicInPolygon:
		if (dc->image) {
			dc->image->points[dc->image->npoints - 1].x = dc->fixx;
			dc->image->points[dc->image->npoints - 1].y = dc->fixy;
			StuffPoint(dc->image, dc->fixx, dc->fixy);
		}
		break;
	case PicEndArcStep1:
		SetDrawStat(w, PicSetBeginArc);
		break;
	case PicEndBeginArc:
		SetDrawStat(w, PicSetFinArc);
		break;
	case PicEndFinArc:
	case PicEndDraw:
		SetDrawStat(w, PicStartDraw);
		if (dc->image)
			FixupObject(w, event);
		spw->stPicture.mode_continue = False;
		break;
	case PicInMove_SB1:
		RestorOldDrawStat(w);
		FixupMoveObject(w, event);
		spw->stPicture.mode_continue = False;
		break;
	case PicEndMove:
		SetDrawStat(w, PicStartMove);
		FixupMoveObject(w, event);
		spw->stPicture.mode_continue = False;
		break;
	case PicEndCopy:
		SetDrawStat(w, PicStartCopy);
		FixupCopyObject(w, event);
		spw->stPicture.mode_continue = False;
		break;
	case PicEndResize:
		SetDrawStat(w, PicStartResize);
		FixupResizeObject(w, event);
		spw->stPicture.mode_continue = False;
		break;
	case PicClickSelect:
		SetDrawStat(w, PicStartSelect);
		SavePoint(w, event);
		FixupSelectObject(w, Single);
		break;
	case PicInSelect:
		SetDrawStat(w, PicStartSelect);
		SavePoint(w, event);
		FixupSelectObject(w, Single);
		CurrentTextStatus(w, event);
		break;
	case PicSelectText:
		ExitTextEdit(w, event, False);
		break;
	case PicEndTextDraw:
		ExitTextEdit(w, event, False);
		SetDrawStat(w, PicInDraw);
		CurrentTextStatus(w, event, False);
		break;
	default:
		;
	}
}


static void 
PlineEnd(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StPictureWidget spw = (StPictureWidget) w;
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicInPolygon:
		SetDrawStat(w, PicStartDraw);
		SavePoint(w, event);
		if (dc->image)
			FixupObject(w, event);
		spw->stPicture.mode_continue = False;
		break;
	default:
		;
	}
}


static void 
MainPopup(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StPictureWidget spw = (StPictureWidget) w;
	Arg             args[2];
	int             xx, yy, xx2, yy2;
	int             Width, Height;
	int             dest_x = 0, dest_y = 0;

	ExitTextEdit(w, event, True);

	if (spw->stPicture.mode_continue)
		return;

	spw->stPicture.mode = StpictureNull;
	spw->stPicture.mode_draw = StdrawNull;
	spw->stPicture.callback_item = NULL;
	spw->stPicture.callback_item_draw = NULL;

	if (spw->stPicture.num_entries) {
		Width = XtScreen(w)->width;
		Height = XtScreen(w)->height;
		xx = xx2 = ((XButtonReleasedEvent *) event)->x_root;
		yy = yy2 = ((XButtonReleasedEvent *) event)->y_root;

		if (!XtIsRealized((Widget) spw->stPicture.popup))
			XtRealizeWidget((Widget) spw->stPicture.popup);

		if ((Width - xx - MAIN_POPUP_MARGIN_X) <
		    (spw->stPicture.popup->core.width +
		     spw->stPicture.popup->core.border_width * 2))
			xx2 = Width - MAIN_POPUP_MARGIN_X
				- spw->stPicture.popup->core.width
				- spw->stPicture.popup->core.border_width * 2;

		if ((Height - yy - MAIN_POPUP_MARGIN_Y) <
		    (spw->stPicture.popup->core.height +
		     spw->stPicture.popup->core.border_width * 2))
			yy2 = Height - MAIN_POPUP_MARGIN_Y
				- spw->stPicture.popup->core.height
				- spw->stPicture.popup->core.border_width * 2;

		if (xx != xx2 || yy != yy2) {
			dest_x = xx2 - xx;
			dest_y = yy2 - yy;
			xx = xx2;
			yy = yy2;
		}
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup, args, TWO);
		XtPopup(spw->stPicture.popup, XtGrabExclusive);

		if (dest_x || dest_y)
			XWarpPointer(XtDisplay(w), None, None, 0, 0, 0, 0,
				     dest_x, dest_y);
	}
}


static void 
StPopdown(w)
	Widget          w;
{
	XWindowAttributes attri;

	while (1) {
		XGetWindowAttributes(XtDisplay(w), XtWindow(w), &attri);
		if (attri.map_state == IsViewable)
			break;
	}
	XtPopdown(w);
}


static void 
AllPopdown(w, valuemask)
	StPictureWidget w;
	unsigned long   valuemask;
{
	if ((valuemask & PDDraw) && w->stPicture.drawpopflg) {
		StPopdown(w->stPicture.popup_draw);
		w->stPicture.drawpopflg = False;
	}
	if ((valuemask & PDAttributes) && w->stPicture.attributespopflg) {
		if (w->stPicture.linepopflg) {
			StPopdown(w->stPicture.popup_line);
			w->stPicture.linepopflg = False;
		}
		if (w->stPicture.fillpopflg) {
			StPopdown(w->stPicture.popup_fill);
			w->stPicture.fillpopflg = False;
		}
		if (w->stPicture.charpopflg) {
			StPopdown(w->stPicture.popup_char);
			w->stPicture.charpopflg = False;
		}
		StPopdown(w->stPicture.popup_attributes);
		w->stPicture.attributespopflg = False;
	}
	if ((valuemask & PDGroup) && w->stPicture.grouppopflg) {
		StPopdown(w->stPicture.popup_group);
		w->stPicture.grouppopflg = False;
	}
	if ((valuemask & PDPriority) && w->stPicture.prioritypopflg) {
		StPopdown(w->stPicture.popup_priority);
		w->stPicture.prioritypopflg = False;
	}
	if ((valuemask & PDRotate) && w->stPicture.rotatepopflg) {
		StPopdown(w->stPicture.popup_rotate);
		w->stPicture.rotatepopflg = False;
	}
	if ((valuemask & PDWrite) && w->stPicture.writepopflg) {
		StPopdown(w->stPicture.popup_write);
		w->stPicture.writepopflg = False;
	}
	if ((valuemask & PDRead) && w->stPicture.readpopflg) {
		StPopdown(w->stPicture.popup_read);
		w->stPicture.readpopflg = False;
	}
	if ((valuemask & PDImage) && w->stPicture.imagepopflg) {
		StPopdown(w->stPicture.popup_image);
		w->stPicture.imagepopflg = False;
	}
	if ((valuemask & PDMain))
		XtPopdown(w->stPicture.popup);
}


static void 
PopupCoordinate(w, x, y)
	Widget          w;
	int            *x, *y;
{
	Window          child;
	XWindowAttributes attri;
	int             xx, yy;

	XGetWindowAttributes(XtDisplay(w), XtWindow(w), &attri);
	xx = attri.width + attri.border_width * 2;
	yy = 0;
	XTranslateCoordinates(XtDisplay(w), XtWindow(w),
			      (XtScreen(w))->root, xx, yy, x, y, &child);
}


static void 
StPopup(w, id)
	Widget          w;	/* popup */
	XtIntervalId   *id;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent;

	spw->stPicture.timeid = 0;
	if (spw->stPicture.callback_item)
		return;

	if (w == spw->stPicture.popup_draw)
		spw->stPicture.drawpopflg = True;
	else if (w == spw->stPicture.popup_attributes)
		spw->stPicture.attributespopflg = True;
	else if (w == spw->stPicture.popup_group)
		spw->stPicture.grouppopflg = True;
	else if (w == spw->stPicture.popup_priority)
		spw->stPicture.prioritypopflg = True;
	else if (w == spw->stPicture.popup_rotate)
		spw->stPicture.rotatepopflg = True;
	else if (w == spw->stPicture.popup_write)
		spw->stPicture.writepopflg = True;
	else if (w == spw->stPicture.popup_read)
		spw->stPicture.readpopflg = True;
	else			/* if ( w == spw->stPicture.popup_image ) */
		spw->stPicture.imagepopflg = True;

	XtPopup(w, XtGrabNonexclusive);
}


static void 
StPopupThird(w, id)
	Widget          w;	/* popup */
	XtIntervalId   *id;
{
	StPictureWidget spw = (StPictureWidget) w->core.parent->core.parent->
	core.parent->core.parent->core.parent->core.parent->
	core.parent;
	Arg             args[1];
	Widget          spww;

	spww = (Widget) spw;

	spw->stPicture.timeid_third = 0;

	if (w == spw->stPicture.popup_line)
		spw->stPicture.linepopflg = True;
	else if (w == spw->stPicture.popup_fill)
		spw->stPicture.fillpopflg = True;
	else
		spw->stPicture.charpopflg = True;

	XtPopup(w, XtGrabNonexclusive);

	/* Initial Attributes set */
	/* Line */
	if (!spw->stPicture.init_line && w == spw->stPicture.popup_line) {
		XtListHighlight(spw->stPicture.list_line_type, 0);
		XtScrollBarSetThumb(spw->stPicture.scr_line_width,
				    1.0 - 1.0 / 20.0, 1.0);
		XtSetArg(args[0], XtNlabel, WidthTable[1]);
		XtSetValues(spw->stPicture.real_line_width, args, 1);
		XtListHighlight(spw->stPicture.list_line_cap1, 0);
		XtListHighlight(spw->stPicture.list_line_cap2, 0);
		XtListHighlight(spw->stPicture.list_line_join, 0);
		XtSetArg(args[0], XtNbackground, RealColor[0]);
		XtSetValues(spw->stPicture.line_color_name, args, 1);
		XtSetArg(args[0], XtNbackground, RealColor[1]);
		XtSetValues(spw->stPicture.line_color_name2, args, 1);
		XtListHighlight(spw->stPicture.list_line_function, 3);

		StMenuLineAtr.line_style = StlineSolid;
		StMenuLineAtr.line_width = 1;
		StMenuLineAtr.begin_cap = StcapButt;
		StMenuLineAtr.end_cap = StcapButt;
		StMenuLineAtr.join_style = StjoinMiter;
		StMenuLineAtr.line_foreground = StCBlack;
		StMenuLineAtr.line_background = StCWhite;
		StMenuLineAtr.function = StGcopy;

		spw->stPicture.init_line = True;
	}
	/* Fill */
	else if (!spw->stPicture.init_fill && w == spw->stPicture.popup_fill) {
		spw->stPicture.localmap = XCreatePixmapFromBitmapData(
							    XtDisplay(spww),
						 RootWindow(XtDisplay(spww),
					    DefaultScreen(XtDisplay(spww))),
					   spw->stPicture.maptable[13].bits,
					  spw->stPicture.maptable[13].width,
					 spw->stPicture.maptable[13].height,
			    spw->stPicture.form_fill->core.background_pixel,
			    spw->stPicture.form_fill->core.background_pixel,
						XtScreen(spww)->root_depth);
		XtSetArg(args[0], XtNbackgroundPixmap, spw->stPicture.localmap);
		XtSetValues(spw->stPicture.fill_tile_name, args, 1);

		StMenuPaintAtr.fill_style = StfillHollow;
		StMenuPaintAtr.fill_foreground = StCBlack;
		StMenuPaintAtr.fill_background = StCWhite;

		spw->stPicture.init_fill = True;
	}
	/* Char */
	else if (!spw->stPicture.init_char && w == spw->stPicture.popup_char) {
		XtListHighlight(spw->stPicture.list_char_kind, 0);
		XtListHighlight(spw->stPicture.list_char_size, 0);
		XtSetArg(args[0], XtNbackground, RealColor[0]);
		XtSetValues(spw->stPicture.char_color_name, args, 1);
		XtListHighlight(spw->stPicture.list_char_function, 3);

		StMenuTextAtr.font = StfontRoman8;
		StMenuTextAtr.text_color = StCBlack;
		StMenuTextAtr.function = StGcopy;

		spw->stPicture.init_char = True;
	}
}


static void 
ThirdPopup(w, event, params, nparams)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal        nparams;
{
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent->
			   core.parent->core.parent->core.parent);
	Arg             args[2];
	int             xx, yy;
	XWindowAttributes attri;

	/*------------------------------------------------------------------*/
	if (spw->stPicture.timeid) {
		XtRemoveTimeOut(spw->stPicture.timeid);
		spw->stPicture.timeid = 0;
	}
	/*------------------------------------------------------------------*/
	if (spw->stPicture.timeid_third) {
		XtRemoveTimeOut(spw->stPicture.timeid_third);
		spw->stPicture.timeid_third = 0;
	}
	/*------------------------------------------------------------------*/

	if (!spw->stPicture.attributespopflg)
		return;

	if (!strcmp(*params, "Line")) {
		if (spw->stPicture.linepopflg)
			return;
		if (spw->stPicture.fillpopflg) {
			StPopdown(spw->stPicture.popup_fill);
			spw->stPicture.fillpopflg = False;
		}
		if (spw->stPicture.charpopflg) {
			StPopdown(spw->stPicture.popup_char);
			spw->stPicture.charpopflg = False;
		}
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_line, args, TWO);
		spw->stPicture.timeid_third =
			XtAddTimeOut(StpopupTime, StPopupThird,
				     spw->stPicture.popup_line);
	} else if (!strcmp(*params, "Fill")) {
		if (spw->stPicture.fillpopflg)
			return;
		if (spw->stPicture.linepopflg) {
			StPopdown(spw->stPicture.popup_line);
			spw->stPicture.linepopflg = False;
		}
		if (spw->stPicture.charpopflg) {
			StPopdown(spw->stPicture.popup_char);
			spw->stPicture.charpopflg = False;
		}
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_fill, args, TWO);
		spw->stPicture.timeid_third =
			XtAddTimeOut(StpopupTime, StPopupThird,
				     spw->stPicture.popup_fill);
	} else if (!strcmp(*params, "Char")) {
		if (spw->stPicture.charpopflg)
			return;
		if (spw->stPicture.linepopflg) {
			StPopdown(spw->stPicture.popup_line);
			spw->stPicture.linepopflg = False;
		}
		if (spw->stPicture.fillpopflg) {
			StPopdown(spw->stPicture.popup_fill);
			spw->stPicture.fillpopflg = False;
		}
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_char, args, TWO);
		spw->stPicture.timeid_third =
			XtAddTimeOut(StpopupTime, StPopupThird,
				     spw->stPicture.popup_char);
	}
}


static void 
SubPopup(w, event, params, nparams)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal        nparams;
{
	StPictureWidget spw =
	(StPictureWidget) (w->core.parent->core.parent->core.parent);
	Arg             args[2];
	int             xx, yy;

	/*------------------------------------------------------------------*/
	if (spw->stPicture.timeid_third) {
		XtRemoveTimeOut(spw->stPicture.timeid_third);
		spw->stPicture.timeid_third = 0;
	}
	/*------------------------------------------------------------------*/
	if (spw->stPicture.timeid) {
		XtRemoveTimeOut(spw->stPicture.timeid);
		spw->stPicture.timeid = 0;
	}
	/*------------------------------------------------------------------*/

	if (!strcmp(*params, "Draw") && spw->stPicture.num_entries_draw) {
		if (spw->stPicture.drawpopflg)
			return;
		AllPopdown(spw,
			   PDAttributes | PDGroup | PDPriority | PDRotate |
			   PDWrite | PDRead | PDImage);

		spw->stPicture.real_parent_draw = w;
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_draw, args, TWO);
		spw->stPicture.timeid =
			XtAddTimeOut(StpopupTime, StPopup, spw->stPicture.popup_draw);
	} else if (!strcmp(*params, "Attributes")) {
		if (spw->stPicture.attributespopflg)
			return;
		AllPopdown(spw,
			   PDDraw | PDGroup | PDPriority | PDRotate | PDWrite | PDRead | PDImage);

		spw->stPicture.real_parent_attributes = w;
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_attributes, args, TWO);
		spw->stPicture.timeid =
			XtAddTimeOut(StpopupTime, StPopup,
				     spw->stPicture.popup_attributes);
	} else if (!strcmp(*params, "Group")) {
		if (spw->stPicture.grouppopflg)
			return;
		AllPopdown(spw,
			   PDDraw | PDAttributes | PDPriority | PDRotate |
			   PDWrite | PDRead | PDImage);

		spw->stPicture.real_parent_group = w;
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_group, args, TWO);
		spw->stPicture.timeid =
			XtAddTimeOut(StpopupTime, StPopup, spw->stPicture.popup_group);
	} else if (!strcmp(*params, "Priority")) {
		if (spw->stPicture.prioritypopflg)
			return;
		AllPopdown(spw,
			   PDDraw | PDAttributes | PDGroup | PDRotate | PDWrite | PDRead | PDImage);

		spw->stPicture.real_parent_priority = w;
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_priority, args, TWO);
		spw->stPicture.timeid =
			XtAddTimeOut(StpopupTime, StPopup,
				     spw->stPicture.popup_priority);
	} else if (!strcmp(*params, "Rotate")) {
		if (spw->stPicture.rotatepopflg)
			return;
		AllPopdown(spw,
			   PDDraw | PDAttributes | PDGroup | PDPriority | PDWrite | PDRead | PDImage);

		spw->stPicture.real_parent_rotate = w;
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_rotate, args, TWO);
		spw->stPicture.timeid =
			XtAddTimeOut(StpopupTime, StPopup,
				     spw->stPicture.popup_rotate);
	} else if (!strcmp(*params, "Write")) {
		if (spw->stPicture.writepopflg)
			return;
		AllPopdown(spw,
		   PDDraw | PDAttributes | PDGroup | PDPriority | PDRotate |
			   PDRead | PDImage);

		spw->stPicture.real_parent_write = w;
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_write, args, TWO);
		spw->stPicture.timeid =
			XtAddTimeOut(StpopupTime, StPopup, spw->stPicture.popup_write);
	} else if (!strcmp(*params, "Read")) {
		if (spw->stPicture.readpopflg)
			return;
		AllPopdown(spw,
		   PDDraw | PDAttributes | PDGroup | PDPriority | PDRotate |
			   PDWrite | PDImage);

		spw->stPicture.real_parent_read = w;
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_read, args, TWO);
		spw->stPicture.timeid =
			XtAddTimeOut(StpopupTime, StPopup, spw->stPicture.popup_read);
	} else if (!strcmp(*params, "Image")) {
		if (spw->stPicture.imagepopflg)
			return;
		AllPopdown(spw,
		   PDDraw | PDAttributes | PDGroup | PDPriority | PDRotate |
			   PDWrite | PDRead);

		spw->stPicture.real_parent_image = w;
		PopupCoordinate(w, &xx, &yy);
		XtSetArg(args[0], XtNx, xx);
		XtSetArg(args[1], XtNy, yy);
		XtSetValues(spw->stPicture.popup_image, args, TWO);
		spw->stPicture.timeid =
			XtAddTimeOut(StpopupTime, StPopup, spw->stPicture.popup_image);
	} else
		AllPopdown(spw, PDSubAll);
}


static void 
Parse(w, event, params, nparams)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal        nparams;
{
	StPictureWidget spw;
	String          string[1];
	char           *ctmp;

	if (!strcmp(*params, "Main")) {
		spw =
			(StPictureWidget) w->core.parent->core.parent->core.parent;
		spw->stPicture.callback_item = w;
	} else if (!strcmp(*params, "Attributes")) {
		spw =
			(StPictureWidget) w->core.parent->core.parent->
			core.parent->core.parent->core.parent->core.parent->
			core.parent->core.parent->core.parent;
	} else
		spw =
			(StPictureWidget) w->core.parent->core.parent->core.parent->
			core.parent->core.parent->core.parent;

	/*--------------------------------------------------------------------*/

	if (!strcmp(*params, "Main")) {
		params++;
		if ((**params == '1' && spw->stPicture.num_entries_draw) ||
		    **params == '3' ||
		    **params == '4' ||
		    **params == '5' ||
		    **params == '8' ||
		    **params == 'd' ||
		    **params == 'e' ||
		    **params == 'f') {
			spw->stPicture.callback_item = NULL;
			return;
		}
		switch (**params) {
		case (int) '1':
			spw->stPicture.mode = StpictureDraw;
			break;
		case (int) '2':
			/* spw->stPicture.mode = StpictureNull  ; */
			spw->stPicture.mode = StpictureDrawEnd;
			spw->stPicture.mode_draw = StdrawNull;
			SetDrawStat(spw, PicStartSelect);
			break;
		case (int) '6':
			spw->stPicture.mode = StpictureTopBottom;
			break;
		case (int) '7':
			spw->stPicture.mode = StpictureLeftRight;
			break;
		case (int) '9':
			spw->stPicture.mode = StpictureDelete;
			break;
		case (int) 'a':
			spw->stPicture.mode = StpictureCopy;
			SetDrawStat(spw, PicStartCopy);
			break;
		case (int) 'b':
			spw->stPicture.mode = StpictureMove;
			SetDrawStat(spw, PicStartMove);
			break;
		case (int) 'c':
			spw->stPicture.mode = StpictureResize;
			SetDrawStat(spw, PicStartResize);
			break;
		case (int) 'g':
			spw->stPicture.mode = StpictureUndo;
			break;
		case (int) 'h':
			spw->stPicture.mode = StpictureRedraw;
			break;
		default:	/* '0' */
			spw->stPicture.mode = StpictureNull;
		}
	} else if (!strcmp(*params, "Draw")) {
		spw->stPicture.mode = StpictureDraw;
		spw->stPicture.callback_item =
			spw->stPicture.real_parent_draw;
		spw->stPicture.callback_item_draw = w;
		params++;
		SetDrawStat(spw, PicStartDraw);	/* revised */
		switch (**params) {
		case (int) '1':
			spw->stPicture.mode_draw = StdrawRectangle;
			break;
		case (int) '2':
			spw->stPicture.mode_draw = StdrawCircle;
			break;
		case (int) '3':
			spw->stPicture.mode_draw = StdrawEllipse;
			break;
		case (int) '4':
			spw->stPicture.mode_draw = StdrawLine;
			break;
		case (int) '5':
			spw->stPicture.mode_draw = StdrawArc;
			break;
		case (int) '6':
			spw->stPicture.mode_draw = StdrawEllipticArc;
			break;
		case (int) '7':
			spw->stPicture.mode_draw = StdrawPie;
			break;
		case (int) '8':
			spw->stPicture.mode_draw = StdrawEllipticPie;
			break;
		case (int) '9':
			spw->stPicture.mode_draw = StdrawPolyline;
			break;
		case (int) 'a':
			spw->stPicture.mode_draw = StdrawPolygon;
			break;
		case (int) 'b':
			spw->stPicture.mode_draw = StdrawFreehand;
			break;
		case (int) 'c':
			spw->stPicture.mode_draw = StdrawText;
			break;
		default:	/* '0' */
			spw->stPicture.mode_draw = StdrawNull;
		}
	} else if (!strcmp(*params, "Attributes")) {
		params++;
		spw->stPicture.mode = StpictureAttributes;
		spw->stPicture.callback_item =
			spw->stPicture.real_parent_attributes;
		if (**params == '0')
			ChangeMenuAttr(spw, 0);
		else if (**params == '1')
			ChangeMenuAttr(spw, 1);
		else
			ChangeMenuAttr(spw, 2);
	} else if (!strcmp(*params, "Group")) {
		params++;
		spw->stPicture.mode = StpictureGroup;
		spw->stPicture.callback_item =
			spw->stPicture.real_parent_group;
		if (**params == '0')
			spw->stPicture.group_flg = True;
		else
			spw->stPicture.group_flg = False;
	} else if (!strcmp(*params, "Priority")) {
		params++;
		spw->stPicture.mode = StpicturePriority;
		spw->stPicture.callback_item =
			spw->stPicture.real_parent_priority;
		if (**params == '0')
			spw->stPicture.priority_flg = True;
		else
			spw->stPicture.priority_flg = False;
	} else if (!strcmp(*params, "Rotate")) {
		params++;
		spw->stPicture.mode = StpictureRotate;
		spw->stPicture.callback_item =
			spw->stPicture.real_parent_rotate;
		if (**params == '0')
			spw->stPicture.rotate_flg = 1;
		else if (**params == '1')
			spw->stPicture.rotate_flg = 2;
		else
			spw->stPicture.rotate_flg = 3;
	} else if (!strcmp(*params, "Write")) {
		spw->stPicture.mode = StpictureWrite;
		spw->stPicture.callback_item =
			spw->stPicture.real_parent_write;

		if (spw->stPicture.write_file)
			XtFree(spw->stPicture.write_file);
		ctmp = XtDialogGetValueString(w->core.parent);
		for (; isspace((int) *ctmp); ctmp++);
		spw->stPicture.write_file =
			strcpy(XtMalloc(strlen(ctmp) + 1), ctmp);
	} else if (!strcmp(*params, "Read")) {
		params++;
		spw->stPicture.mode = StpictureRead;
		spw->stPicture.callback_item =
			spw->stPicture.real_parent_read;
		if (**params == '0')
			spw->stPicture.read_flg = True;
		else
			spw->stPicture.read_flg = False;

		if (spw->stPicture.read_file)
			XtFree(spw->stPicture.read_file);
		ctmp = XtDialogGetValueString(w->core.parent);
		for (; isspace((int) *ctmp); ctmp++);
		spw->stPicture.read_file =
			strcpy(XtMalloc(strlen(ctmp) + 1), ctmp);
	} else {		/* if ( !strcmp(*params,"Image") ) */
		spw->stPicture.mode = StpictureImage;
		spw->stPicture.callback_item =
			spw->stPicture.real_parent_image;

		if (spw->stPicture.image_file)
			XtFree(spw->stPicture.image_file);
		ctmp = XtDialogGetValueString(w->core.parent);
		for (; isspace((int) *ctmp); ctmp++);
		spw->stPicture.image_file =
			strcpy(XtMalloc(strlen(ctmp) + 1), ctmp);
	}

	AllPopdown(spw, PDAll);

	string[0] = "menu-function";
	NotifyBefore((Widget) spw, NULL, string, 1);

	MenuFunction(spw);

	switch ((int) spw->stPicture.mode) {
	case (int) StpictureAttributes:
	case (int) StpictureDrawEnd:
	case (int) StpictureGroup:
	case (int) StpicturePriority:
	case (int) StpictureTopBottom:
	case (int) StpictureLeftRight:
	case (int) StpictureRotate:
	case (int) StpictureDelete:
	case (int) StpictureWrite:
	case (int) StpictureRead:
	case (int) StpictureImage:
	case (int) StpictureUndo:
	case (int) StpictureRedraw:
	case (int) StpictureNull:
		string[0] = "menu-function";
		NotifyAfter((Widget) spw, NULL, string, 1);
		break;
	}
}


static void 
MenuFunction(spw)
	StPictureWidget spw;
{
	switch ((int) spw->stPicture.mode) {
	case StpictureDraw:
		break;
	case StpictureNull:
		spw->stPicture.mode_draw = StdrawNull;
		SetDrawStat(spw, PicStartSelect);
		break;
	case StpictureTopBottom:
		MirrorObject(spw, SthorizontalMirror);
		break;
	case StpictureLeftRight:
		MirrorObject(spw, StverticalMirror);
		break;
	case StpictureDelete:
		DeleteObject(spw);
		break;
	case StpictureCopy:
		SetDrawStat(spw, PicStartCopy);
		break;
	case StpictureMove:
		SetDrawStat(spw, PicStartMove);
		break;
	case StpictureResize:
		SetDrawStat(spw, PicStartResize);
		break;
	case StpictureUndo:
		UndoObject(spw);
		break;
	case StpictureRedraw:
		StPicRedrawAll(spw);
		break;
	case StpictureGroup:
		if (spw->stPicture.group_flg == True)
			GroupObject(spw);
		else
			UngroupObject(spw);
		break;
	case StpicturePriority:
		if (spw->stPicture.priority_flg == True)
			PriorObject(spw, SttopPriority);
		else
			PriorObject(spw, StbottomPriority);
		break;
	case StpictureRotate:
		if (spw->stPicture.rotate_flg == 1)
			RotateObject(spw, 90 * 64);
		else if (spw->stPicture.rotate_flg == 2)
			RotateObject(spw, 180 * 64);
		else if (spw->stPicture.rotate_flg == 3)
			RotateObject(spw, 270 * 64);
		else;
		break;
	case StpictureWrite:
		WriteFile(spw);
		break;
	case StpictureRead:
		if (spw->stPicture.read_flg == False)
			StPicReinitialize(spw);
		ReadFile(spw);
		break;
	case StpictureImage:
		DrawImageFile(spw);
		break;
	default:
		;
	}

}


static void 
ExtendStart(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicStartDraw:
	case PicStartSelect:
	case PicStartMove:
	case PicStartCopy:
	case PicStartResize:
		SaveOldDrawStat(w);
		SavePoint(w, event);
		SetDrawStat(w, PicClickSelect_B3);
		break;
	default:
		;
	}
}


static void 
ExtendAdjust(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicClickSelect_B3:
		SetDrawStat(w, PicInSelect_B3);
	case PicInSelect_B3:
		SavePoint(w, event);
		HandleSelectBox(w, event);
		break;
	default:
		;
	}
}


static void 
ExtendEnd(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicClickSelect_B3:
		SavePoint(w, event);
		FixupSelectObject(w, Multi);
		RestorOldDrawStat(w);
		break;
	case PicInSelect_B3:
		FixupSelectBox(w, event);
		RestorOldDrawStat(w);
		break;
	default:
		;
	}
}


static void 
Motion(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	StDrawControl  *dc = GetDrawControl(w);

	switch ((int) dc->drawstat) {
	case PicInDraw:
	case PicInPolygon:
	case PicInArcStep1:
		HandleDrag(w, event);
		break;
	case PicInMove:
	case PicInMove_SB1:
	case PicInCopy:
		HandleMove(w, event);
		break;
	case PicInResize:
		HandleResize(w, event);
		break;
	case PicInScrool:
		HandleScrool(w, event);
		SavePoint(w, event);
		break;
	default:
		;
	}
}


static void 
DoNothing(w, event, params, num_params)
	Widget          w;
	XEvent         *event;
	String         *params;
	Cardinal       *num_params;
{
	return;
}


/*******************************************************
*
*  Scrollbar Callback Procedures
*
*******************************************************/

static void 
MoveViewArea(spw, widget, x, y)
	Widget          spw, widget;
	int             x, y;
{
	StFigureList   *flist = (StFigureList *) NULL;
	StDrawControl  *dc = GetDrawControl(spw);
	register        i;

	if (dc->n_selected) {
		StPicGetSelectedFigure(spw, &flist);
		SelectAll(spw, StunsetSelect);
	}
	MoveView(spw, widget, x, y);
	if (flist)
		for (i = 0; i < flist->n_figures; i++)
			StPicSelectFigure(spw, flist->figures[i], StsetSelect, StsetHandle);
}

static void 
VScrollUpDown(widget, closure, callData)
	Widget          widget;
	caddr_t         closure;/* StPictureWidget */
	caddr_t         callData;
{
	StPictureWidget w;
	int             pix;

	w = (StPictureWidget) closure;
	pix = (int) callData;

	MoveViewArea(w, widget, 0, -pix);
}

static void 
HScrollUpDown(widget, closure, callData)
	Widget          widget;
	caddr_t         closure;/* StPictureWidget */
	caddr_t         callData;
{
	StPictureWidget w;
	int             pix;

	w = (StPictureWidget) closure;
	pix = (int) callData;

	MoveViewArea(w, widget, -pix, 0);
}

static void 
VScrollThumb(widget, closure, callData)
	Widget          widget;
	caddr_t         closure;/* StPictureWidget */
	float          *callData;
{
	register StPictureWidget w;
	int             y;

	w = (StPictureWidget) closure;

	y = (int) w->stPicture.coor_ymax - (int) (*callData * ((int) w->stPicture.coor_ymax - (int) w->stPicture.coor_ymin));

	MoveViewArea(w, FALSE, 0, -(y - (int) w->stPicture.view_y));
}

static void 
HScrollThumb(widget, closure, callData)
	Widget          widget;
	caddr_t         closure;/* StPictureWidget */
	float          *callData;
{
	register StPictureWidget w;
	int             x;

	w = (StPictureWidget) closure;

	x = (int) (*callData * ((int) w->stPicture.coor_xmax - (int) w->stPicture.coor_xmin) + (int) w->stPicture.coor_xmin);

	MoveViewArea(w, FALSE, x - (int) w->stPicture.view_x, 0);
}
