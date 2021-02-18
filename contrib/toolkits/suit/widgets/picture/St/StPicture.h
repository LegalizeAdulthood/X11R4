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

#ifndef _StPicture_h
#define _StPicture_h

/***********************************************************************
 *
 * StPicture Widget
 *
 ***********************************************************************/

#include <X11/Simple.h>

/* Resources:

 Name		     Class		RepType		Default Value
 ----		     -----		-------		-------------
 background	     Background		Pixel		XtDefaultBackground
 borderColor	     BorderColor	Pixel		XtDefaultForeground
 borderWidth	     BorderWidth	Dimension	4
 destroyCallback     Callback		CallbackList	NULL
 file                File               String          NULL
 forceBars           Boolean            Boolean         False
 foreground	     Foreground		Pixel		Black
 font		     Font		XFontStruct *	Fixed
 height		     Height		Dimension	DEFAULT_HEIGHT
 mappedWhenManaged   MappedWhenManaged	Boolean		True
 sensitive	     Sensitive		Boolean		True
 useBottom           Boolean            Boolean         False
 useRight            Boolean            Boolean         False
 width		     Width		Dimension	DEFAULT_WIDTH
 x		     Position		Position	0
 y		     Position		Position	0
 afterCallbacks      AfterCallbacks     XtCallbackList* NULL
 afterCallbacksDraw  AfterCallbacksDraw XtCallbackList* NULL
 allowProtrusion     AllowProtrusion    Boolean         True
 beforeCallbacks     BeforeCallbacks    XtCallbackList* NULL
 beforeCallbacksDraw BeforeCallbacksDraw XtCallbackList* NULL
 coordinateXMax      CoordinateXMax     Position	32767
 coordinateXMin      CoordinateXMin     Position	-32768
 coordinateYMax      CoordinateYMax     Position	32767
 coordinateYMin      CoordinateYMin     Position	-32768
 menuEntries         MenuEntries        String *        (internal)
 menuEntriesDraw     MenuEntriesDraw    String *        (internal)
 menuFunctions       MenuFunctions      StPictureFunction* (internal)
 menuFunctionsDraw   MenuFunctionsDraw  StDrawFunction* (internal)
 menuTitle	     MenuTitle		String		NULL
 menuTitleDraw	     MenuTitleDraw	String		NULL
 viewX		     ViewX		Position	0
 viewY		     ViewY		Position	0

*/

#define StNafterCallbacks	"afterCallbacks"
#define StNafterCallbacksDraw   "afterCallbacksDraw"
#define StNallowProtrusion	"allowProtrusion"
#define StNbeforeCallbacks	"beforeCallbacks"
#define StNbeforeCallbacksDraw	"beforeCallbacksDraw"
#define StNcoordinateXMax	"coordinateXMax"
#define StNcoordinateXMin	"coordinateXMin"
#define StNcoordinateYMax	"coordinateYMax"
#define StNcoordinateYMin	"coordinateYMin"
#define StNmenuEntries		"menuEntries"
#define StNmenuEntriesDraw	"menuEntriesDraw"
#define StNmenuFunctions	"menuFunctions"
#define StNmenuFunctionsDraw	"menuFunctionsDraw"
#define StNmenuTitle		"menuTitle"
#define StNmenuTitleDraw	"menuTitleDraw"
#define StNviewX		"viewX"
#define StNviewY		"viewY"
#define StNtempCallback		"tempCallback"

#define StCViewX		"ViewX"
#define StCViewY		"ViewY"
#define StCCoordinateXMax	"CoordinateXMax"
#define StCCoordinateXMin	"CoordinateXMin"
#define StCCoordinateYMax	"CoordinateYMax"
#define StCCoordinateYMin	"CoordinateYMin"
#define StCAllowProtrusion	"AllowProtrusion"
#define StCMenuTitle		"MenuTitle"
#define StCMenuTitleDraw	"MenuTitleDraw"
#define StCMenuEntries		"MenuEntries"
#define StCMenuEntriesDraw	"MenuEntriesDraw"
#define StCAfterCallbacks	"AfterCallbacks"
#define StCAfterCallbacksDraw   "AfterCallbacksDraw"
#define StCBeforeCallbacks	"BeforeCallbacks"
#define StCBeforeCallbacksDraw	"BeforeCallbacksDraw"
#define StCMenuFunctions	"MenuFunctions"
#define StCMenuFunctionsDraw	"MenuFunctionsDraw"

#define	StcallbacksEnd		(XtCallbackList)-1

typedef enum {
	StpictureNull,
	StpictureDraw,
        StpictureDrawEnd,
        StpictureAttributes,
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
        StpictureHand,
        StpictureEnd,
	}  StPictureFunction  ;
typedef enum {
        StdrawNull,
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
	}  StDrawFunction  ;

extern WidgetClass     stPictureWidgetClass;

typedef struct _StPictureClassRec   *StPictureWidgetClass;
typedef struct _StPictureRec        *StPictureWidget;


#endif _StPicture_h

extern int StPicGetMouseLocation(); /* w, point_return, coordinate */
    /*  Widget      w;			*/
    /*  StPoint *   point_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicSetFigureType(); /* w, type */
    /*  Widget      w;			*/
    /*  StFigureType type;		*/

extern int StPicBreakDraw(); /* w */
    /*  Widget      w;			*/

extern StFigure StPicCreateRectangle(); /* w, rectangle, coordinate */
    /*  Widget      w;			*/
    /*  StRectangle *rectangle;		*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreateEllipse(); /* w, ellipse, coordinate */
    /*  Widget      w;			*/
    /*  StArc       *ellipse;		*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreateArc(); /* w, arc, coordinate */
    /*  Widget      w;			*/
    /*  StArc       *arc;		*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreatePie(); /* w, pie, coordinate */
    /*  Widget      w;			*/
    /*  StArc       *pie;		*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreateLine(); /* w, x1, y1, x2, y2, coordinate */
    /*  Widget      w;			*/
    /*  short       x1, y1;		*/
    /*  short       x2, y2;		*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreatePolyline(); /* w, point_list, coordinate */
    /*  Widget      w;			*/
    /*  StPointList *point_list;	*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreatePolygon(); /* w, vertices, coordinate */
    /*  Widget      w;			*/
    /*  StPointList *vertices;		*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreateFreehand(); /* w, point_list, coordinate */
    /*  Widget      w;			*/
    /*  StPointList *point_list;	*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreateText(); /* w, text, coordinate */
    /*  Widget      w;			*/
    /*  StText      *text;		*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCreateBitmap(); /* w, bitmap, coordinate */
    /*  Widget      w;			*/
    /*  StBitmap    *bitmap;		*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetFigure(); /* w, pos, figure_list_return, coordinate */
    /*  Widget      w;			*/
    /*  StPoint *   pos;		*/
    /*  StFigureList **figure_list_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetSelectedFigure(); /* w, figure_list_return */
    /*  Widget      w;			*/
    /*  StFigureList **figure_list_return;	*/

extern int StPicSelectFigure(); /* w, figure, select, handle */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StSetSelect select;		*/
    /*  StSetHandle handle;		*/

extern int StPicMoveFigure(); /* w, figure, dest_pos, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StPoint *   dest_pos;		*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicCopyFigure(); /* w, figure, dest_pos, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StPoint *   dest_pos;		*/
    /*  StCoordinate coordinate;	*/

extern int StPicSetLineAtr(); /* w, figure, line_atr */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StLineAtr   *line_atr;	*/

extern int StPicSetPaintAtr(); /* w, figure, paint_atr */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StPaintAtr  *paint_atr;		*/

extern int StPicSetTextAtr(); /* w, figure, text_atr */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StTextAtr   *text_atr;		*/

extern int StPicGetLineAtr(); /* w, figure, line_atr_return */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StLineAtr   *line_atr_return;	*/

extern int StPicGetPaintAtr(); /* w, figure, paint_atr_return */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StPaintAtr  *paint_atr_return;	*/

extern int StPicGetTextAtr(); /* w, figure, text_atr_return */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StTextAtr   *text_atr_return;	*/

extern int StPicDrawFigure(); /* w, figure */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/

extern int StPicRedrawPart(); /* w, region, coordinate */
    /*  Widget      w;			*/
    /*  StRegion    *region;		*/
    /*  StCoordinate coordinate;	*/

extern int StPicRedrawAll(); /* w */
    /*  Widget      w;			*/

extern int StPicGetAllFigure(); /* w, figure_list_return */
    /*  Widget      w;			*/
    /*  StFigureList **figure_list_return;	*/

extern StFigureType StPicGetFigureType(); /* w, figure */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/

extern int StPicGetlineInfo(); /* w, figure, point_list_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StPointList **point_list_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetRectInfo(); /* w, figure, rect_info_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StRectangle *rect_info_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetPolylineInfo(); /* w, figure, point_list_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StPointList **point_list_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetPolygonInfo(); /* w, figure, vertices_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StPointList **vertices_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetFreehandInfo(); /* w, figure, point_list_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StPointList **point_list_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetEllipseInfo(); /* w, figure, ellp_info_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StArc       *ellp_info_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetArcInfo(); /* w, figure, arc_info_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StArc       *arc_info_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetPieInfo(); /* w, figure, pie_info_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StArc       *pie_info_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetTextInfo(); /* w, figure, text_info_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StText      *text_info_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetBitmapInfo(); /* w, figure, bitmap_info_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StBitmap    *bitmap_info_return;	*/
    /*  StCoordinate coordinate;	*/

extern int StPicGetFigureRegion(); /* w, figure, region_return, coordinate */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StRegion    *region_return;	*/
    /*  StCoordinate coordinate;	*/

extern StFigure StPicGroupFigure(); /* w, figure_list */
    /*  Widget      w;			*/
    /*  StFigureList figure_list;	*/

extern int StPicUngroupFigure(); /* w, figure, figure_list_return */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StFigureList **figure_list_return;	*/

extern int StPicChangePriority(); /* w, figure, priority */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StSetPriority priority;		*/

extern int StPicMirrorFigure(); /* w, figure, mirror */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  StSetMirror mirror;		*/

extern int StPicRotateFigure(); /* w, figure, angle */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  int         angle;		*/

extern int StPicDeleteFigure(); /* w, figure */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/

extern int StPicResizeFigure(); /* w, figure, width, height */
    /*  Widget      w;			*/
    /*  StFigure    figure;		*/
    /*  unsigned short width, height;	*/

extern int StPicReinitialize(); /* w */
    /*  Widget      w;			*/

extern int StPicSetViewArea(); /* w, move_x, move_y */
    /*  Widget      w;			*/
    /*  int         move_x, move_y;	*/

extern int StPicClearView(); /* w */
    /*  Widget      w;			*/

extern int StPicReadFile(); /* w, file_name */
    /*  Widget      w;			*/
    /*  char        *file_name;		*/

extern int StPicWriteFile(); /* w, file_name */
    /*  Widget      w;			*/
    /*  char        *file_name;		*/

extern int StPicGetStatus(); /* w */
    /*  Widget      w;			*/
