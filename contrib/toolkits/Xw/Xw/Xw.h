/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        Xw.h
 **
 **   Project:     X Widgets
 **
 **   Description: This include file contains public defines and structures
 **                needed by all XToolkit applications using the X Widget Set.
 **                Included in the file are the definitions for all common
 **                resource types, defines for resource or arg list values,
 **                and the class and instance record definitions for all meta
 **                classes.
 **
 *****************************************************************************
 **   
 **   Copyright (c) 1988 by Hewlett-Packard Company
 **   Copyright (c) 1988 by the Massachusetts Institute of Technology
 **   
 **   Permission to use, copy, modify, and distribute this software 
 **   and its documentation for any purpose and without fee is hereby 
 **   granted, provided that the above copyright notice appear in all 
 **   copies and that both that copyright notice and this permission 
 **   notice appear in supporting documentation, and that the names of 
 **   Hewlett-Packard or  M.I.T.  not be used in advertising or publicity 
 **   pertaining to distribution of the software without specific, written 
 **   prior permission.
 **   
 *****************************************************************************
 *************************************<+>*************************************/


#ifndef _XtXw_h
#define _XtXw_h

/************************************************************************
 *
 *  Good 'ole Max and Min from the defunct <X11/Misc.h>
 *
 ************************************************************************/
#define Max(x, y)       (((x) > (y)) ? (x) : (y))
#define Min(x, y)       (((x) > (y)) ? (y) : (x))

/************************************************************************
 *
 *  Resource manager definitions
 *
 ************************************************************************/

#define XtNtraversalOn         "traversalOn"
#define XtCTraversalOn         "TraversalOn"

#define XtNtraversalType       "traversalType"
#define XtCTraversalType       "TraversalType"
#define XtRTraversalType       "TraversalType"

#define XtNhighlightStyle      "highlightStyle"
#define XtCHighlightStyle      "HighlightStyle"
#define XtNhighlightTile       "highlightTile"
#define XtCHighlightTile       "HighlightTile"
#define XtRHighlightStyle      "HighlightStyle"
#define XtNhighlightThickness  "highlightThickness"
#define XtCHighlightThickness  "HighlightThickness"
#define XtNhighlightColor      "highlightColor"

#define XtNbackgroundTile      "backgroundTile"
#define XtCBackgroundTile      "BackgroundTile"

#define XtNcursor		"cursor"

#define XtNrecomputeSize	"recomputeSize"
#define XtCRecomputeSize	"RecomputeSize"

#define XtNlayout              "layout"
#define XtCLayout              "Layout"
#define XtRLayout              "Layout"

#define XtRTileType             "tileType"

#define XtRTranslationTable     "TranslationTable"
#define XtNlabelLocation        "labelLocation"
#define XtCLabelLocation        "LabelLocation"
#define XtRLabelLocation        "LabelLocation"
#define XtNsensitiveTile	"sensitiveTile"
#define XtCSensitiveTile	"SensitiveTile"

#define XtNcolumns              "columns"
#define XtCColumns              "Columns"
#define XtNmode                 "mode"
#define XtCMode                 "Mode"
#define XtRRCMode               "Mode"
#define XtNset                  "set"
#define XtCSet                  "Set"

#define XtNselect              "select"
#define XtNrelease             "release"
#define XtNnextTop             "nextTop"

#define XtNtitleShowing        "titleShowing"
#define XtCTitleShowing        "TitleShowing"
#define XtNmgrTitleOverride    "mgrTitleOverride"
#define XtCTitleOverride       "TitleOverride"
#define XtNtitleType           "titleType"
#define XtCTitleType           "TitleType"
#define XtRTitleType           "TitleType"
#define XtNtitleString         "titleString"
#define XtCTitleString         "TitleString"
#define XtNtitleImage          "titleImage"
#define XtCTitleImage          "TitleImage"
#define XtNfontColor           "fontColor"
#define XtNmnemonic            "mnemonic"
#define XtCMnemonic            "Mnemonic"
#define XtNunderlineTitle      "underlineTitle"
#define XtCUnderlineTitle      "UnderlineTitle"
#define XtNmgrUnderlineOverride "mgrUnderlineOverride"
#define XtCUnderlineOverride   "UnderlineOverride"
#define XtNunderlinePosition   "underlinePosition"
#define XtCUnderlinePosition   "UnderlinePosition"
#define XtNattachTo            "attachTo"
#define XtCAttachTo            "AttachTo"
#define XtNkbdAccelerator      "kbdAccelerator"
#define XtCKbdAccelerator      "KbdAccelerator"

#define XtNassociateChildren   "associateChildren"
#define XtCAssociateChildren   "AssociateChildren"
#define XtNmenuPost            "menuPost"
#define XtCMenuPost            "MenuPost"
#define XtNmenuSelect          "menuSelect"
#define XtCMenuSelect          "MenuSelect"
#define XtNpostAccelerator     "postAccelerator"
#define XtCPostAccelerator     "PostAccelerator"
#define XtNmenuUnpost          "menuUnpost"
#define XtCMenuUnpost          "MenuUnpost"
#define XtNkbdSelect           "kbdSelect"
#define XtCKbdSelect           "KbdSelect"


/*  Resources for the List Widget */

#define XtNnumColumns	 	"numColumns"
#define XtCNumColumns	 	"NumColumns"

#define XtNrowPosition		"rowPosition"
#define XtCRowPosition		"RowPosition"

#define XtNcolumnPosition	"columnPosition"
#define XtCColumnPosition	"ColumnPosition"

#define XtNselectionMethod	"selectionMethod"
#define XtCSelectionMethod	"SelectionMethod"
#define XtRSelectionMethod	"SelectionMethod"

#define XtNelementHighlight	"elementHighlight"
#define XtCElementHighlight	"ElementHighlight"
#define XtRElementHighlight	"ElementHighlight"

#define XtNselectionBias	"selectionBias"
#define XtCSelectionBias	"SelectionBias"
#define XtRSelectionBias	"SelectionBias"

#define XtNselectionStyle	"selectionStyle"
#define XtCSelectionStyle	"SelectionStyle"
#define XtRSelectionStyle	"SelectionStyle"

#define XtNcolumnWidth		"columnWidth" 
#define XtCColumnWidth		"ColumnWidth"

#define XtNelementHeight	"elementHeight"
#define XtCElementHeight	"ElementHeight"

#define XtNselectedElements	"selectedElements"
#define XtCSelectedElements	"SelectedElements"
#define XtNnumSelectedElements	"numSelectedElements"
#define XtCNumSelectedElements  "NumSelectedElements"

#define XtNdestroyMode		"destroyMode"
#define XtCDestroyMode		"DestroyMode"
#define XtRDestroyMode		"DestroyMode"

/*  Resource definition for the row column manager  */

#define XtNlayoutType      "layoutType"
#define XtCLayoutType      "LayoutType"
#define XtRLayoutType      "LayoutType"

#define XtNforceSize       "forceSize"
#define XtCForceSize       "ForceSize"

#define XtNsingleRow	   "singleRow"
#define XtCSingleRow	   "SingleRow"


/*  Resource definition for the menu separator  */

#define XtNseparatorType	"separatorType"
#define XtCSeparatorType	"SeparatorType"
#define XtRSeparatorType	"SeparatorType"

/****************
 *
 *  New resource names for Scrolled Window
 *
 ****************/

#define XtNvsbX          "vsbX"
#define XtNvsbY          "vsbY"
#define XtNvsbWidth	 "vsbWidth"
#define XtNvsbHeight	 "vsbHeight"
#define XtNhsbX	 	 "hsbX"
#define XtNhsbY	 	 "hsbY"
#define XtNhsbWidth	 "hsbWidth"
#define XtNhsbHeight	 "hsbHeight"
#define XtNvSliderMin	 "vSliderMin"
#define XtNvSliderMax	 "vSliderMax"
#define XtNvSliderOrigin "vSliderOrigin"
#define XtNvSliderExtent "vSliderExtent"
#define XtNhSliderMin	 "hSliderMin"
#define XtNhSliderMax	 "hSliderMax"
#define XtNhSliderOrigin "hSliderOrigin"
#define XtNhSliderExtent "hSliderExtent"

#define XtNhScrollEvent	 "hScrollEvent"
#define XtNvScrollEvent	 "vScrollEvent"

#define XtNvScrollBarWidth	 "vScrollBarWidth"
#define XtNvScrollBarHeight	 "vScrollBarHeight"
#define XtNhScrollBarWidth	 "hScrollBarWidth"
#define XtNhScrollBarHeight	 "hScrollBarHeight"

#define XtNforceVerticalSB	 "forceVerticalSB"
#define XtNforceHorizontalSB	 "forceHorizontalSB"

#define XtNinitialX	 "initialX"
#define XtNinitialY	 "initialY"

#define XtNborderPad	 "borderPad"

#define XtCVsbX	 	 "VsbX"
#define XtCVsbY	 	 "VsbY"
#define XtCVsbWidth	 "VsbWidth"
#define XtCVsbHeight	 "VsbHeight"
#define XtCHsbX	 	 "HsbX"
#define XtCHsbY	 	 "HsbY"
#define XtCHsbWidth	 "HsbWidth"
#define XtCHsbHeight	 "HsbHeight"
#define XtCVSliderMin	 "VSliderMin"
#define XtCVSliderMax	 "VSliderMax"
#define XtCVSliderOrigin "VSliderOrigin"
#define XtCVSliderExtent "VSliderExtent"
#define XtCHSliderMin	 "HSliderMin"
#define XtCHSliderMax	 "HSliderMax"
#define XtCHSliderOrigin "HSliderOrigin"
#define XtCHSliderExtent "HSliderExtent"

#define XtCVScrollBarWidth	 "VScrollBarWidth"
#define XtCVScrollBarHeight	 "VScrollBarHeight"
#define XtCHScrollBarWidth	 "HScrollBarWidth"
#define XtCHScrollBarHeight	 "HScrollBarHeight"

#define XtCForceVerticalSB	 "ForceVerticalSB"
#define XtCForceHorizontalSB	 "ForceHorizontalSB"

#define XtCInitialX	 "InitialX"
#define XtCInitialY	 "InitialY"
#define XtCBorderPad	 "BorderPad"

/****************
 *
 *  New resource types for Static Raster
 *
 ****************/

#define XtNsRimage      "rasterImage"
#define XtCSRimage      "RasterImage"

#define XtNshowSelected	       "showSelected"
#define XtCShowSelected	       "ShowSelected"

/*  List Manager Defines */

#define XwSINGLE		0
#define XwMULTIPLE		1
#define XwBORDER		0
#define XwINVERT		1
#define XwNO_BIAS		0
#define XwROW_BIAS		1
#define XwCOL_BIAS		2
#define XwINSTANT		0
#define XwSTICKY		1
#define XwNO_SHRINK		0
#define XwSHRINK_COLUMN		1
#define XwSHRINK_ALL		2


/****************************************************************
 *
 * TextEdit widget
 *
 ****************************************************************/

#define XtNdisplayPosition      "displayPosition"
#define XtNinsertPosition	"insertPosition"
#define XtNleftMargin		"leftMargin"
#define XtNrightMargin		"rightMargin"
#define XtNtopMargin		"topMargin"
#define XtNbottomMargin		"bottomMargin"
#define XtNselectionArray	"selectionArray"
#define XtNtextSource		"textSource"
#define XtNselection		"selection"
#define XtNmaximumSize		"maximumSize"
#define XtCMaximumSize          "MaximumSize"

#define XtNeditType		"editType"
#define XtNfile			"file"
#define XtNstring		"string"
#define XtNlength		"length"
#define XtNfont			"font"
#define XtNdiskSrc              "disksrc"
#define XtNstringSrc            "stringsrc"
#define XtNexecute              "execute"

#define XtNsourceType           "sourceType"
#define XtCSourceType           "SourceType"
#define XtRSourceType           "SourceType"

#define XtNmotionVerification	"motionVerification"
#define XtNmodifyVerification	"modifyVerification"
#define XtNleaveVerification	"leaveVerification"

#define XtNwrap			"wrap"
#define XtCWrap			"Wrap"
#define XtRWrap			"Wrap"

#define XtNwrapForm		"wrapForm"
#define XtCWrapForm		"WrapForm"
#define XtRWrapForm		XtCWrapForm

#define XtNwrapBreak		"wrapBreak"
#define XtCWrapBreak		"WrapBreak"
#define XtRWrapBreak		XtCWrapBreak

#define XtNscroll		"scroll"
#define XtCScroll		"Scroll"
#define XtRScroll		XtCScroll

#define XtNgrow			"grow"
#define XtCGrow			"Grow"
#define XtRGrow			XtCGrow

#define XwAutoScrollOff		0
#define XwAutoScrollHorizontal	1
#define XwAutoScrollVertical	2
#define XwAutoScrollBoth	3  /* must be bitwise OR of horiz. and vert. */

#define XwGrowOff		0
#define XwGrowHorizontal	1
#define XwGrowVertical		2
#define XwGrowBoth		3  /* must be bitwise OR of horiz. and vert. */

/* Viable Selection Modes For RowCol Manager */

#define XwN_OF_MANY  0
#define XwONE_OF_MANY 1


/*  Defines for the row col manager layout types  */

#define XwREQUESTED_COLUMNS             0
#define XwMAXIMUM_COLUMNS               1
#define XwMAXIMUM_UNALIGNED             2



/* Valid Label Location Settings for Button */

#define XwRIGHT		0
#define XwLEFT		1
#define XwCENTER	2


/* Valid Title Type Values For MenuPane */

#define XwSTRING  0
#define XwIMAGE   1


/* New resource manager types */

#define XrmRImage	      	"Image"
#define XtRImage	      	XrmRImage
#define XtNalignment	"alignment"
#define XtCAlignment	"Alignment"
#define XtRAlignment	"Alignment"
#define XtNlineSpace 	"lineSpace"
#define XtCLineSpace 	"LineSpace"
#define XtNgravity		"gravity"
#define XtCGravity		"Gravity"
#define XtRGravity		"Gravity"

typedef enum {
	XwALIGN_NONE,
	XwALIGN_LEFT,
	XwALIGN_CENTER,
	XwALIGN_RIGHT
} XwAlignment;

typedef enum {
	XwUNKNOWN,
	XwPULLDOWN,
	XwTITLE,
	XwWORK_SPACE
} XwWidgetType;



/*  Drawing types for the menu separator.  */

#define XwNO_LINE               0
#define XwSINGLE_LINE           1
#define XwDOUBLE_LINE           2
#define XwSINGLE_DASHED_LINE    3
#define XwDOUBLE_DASHED_LINE    4


/*  Defines for the valuator background and slider visual types */

#define XwSOLID		0
#define XwPATTERN	1
#define XwTRANSPARENT	2

#define XwHORIZONTAL	0
#define XwVERTICAL	1


/*  Define the new resource types used by Valuator  */

#define XtNsliderMin		"sliderMin"
#define XtCSliderMin		"SliderMin"
#define XtNsliderMax		"sliderMax"
#define XtCSliderMax		"SliderMax"
#define XtNsliderOrigin		"sliderOrigin"
#define XtCSliderOrigin		"SliderOrigin"
#define XtNsliderExtent		"sliderExtent"
#define XtCSliderExtent		"SliderExtent"
#define XtNsliderColor		"sliderColor"

#define XtNslideOrientation     "slideOrientation"
#define XtCSlideOrientation     "SlideOrientation"
#define XtRSlideOrientation     "slideOrientation"

#define XtNslideMoveType        "slideMoveType"
#define XtCSlideMoveType        "SlideMoveType"
#define XtRSlideMoveType        "slideMoveType"

#define XtNslideAreaTile        "slideAreaTile"
#define XtCSlideAreaTile        "SlideAreaTile"

#define XtNsliderMoved		"sliderMoved"
#define XtNareaSelected		"areaSelected"

#define XtNsliderTile	"sliderTile"
#define XtCSliderTile	"SliderTile"

#define XtNsliderReleased	"sliderReleased"



/*  Form resource definitions  */

#define XtNxRefName		"xRefName"
#define XtCXRefName		"XRefName"
#define XtNxRefWidget		"xRefWidget"
#define XtCXRefWidget		"XRefWidget"
#define XtNxOffset		"xOffset"
#define XtCXOffset		"XOffset"
#define XtNxAddWidth		"xAddWidth"
#define XtCXAddWidth		"XAddWidth"
#define XtNxVaryOffset		"xVaryOffset"
#define XtCXVaryOffset		"XVaryOffset"
#define XtNxResizable		"xResizable"
#define XtCXResizable		"XResizable"
#define XtNxAttachRight		"xAttachRight"
#define XtCXAttachRight		"XAttachRight"
#define XtNxAttachOffset	"xAttachOffset"
#define XtCXAttachOffset	"XAttachOffset"

#define XtNyRefName		"yRefName"
#define XtCYRefName		"YRefName"
#define XtNyRefWidget		"yRefWidget"
#define XtCYRefWidget		"YRefWidget"
#define XtNyOffset		"yOffset"
#define XtCYOffset		"YOffset"
#define XtNyAddHeight		"yAddHeight"
#define XtCYAddHeight		"YAddHeight"
#define XtNyVaryOffset		"yVaryOffset"
#define XtCYVaryOffset		"YVaryOffset"
#define XtNyResizable		"yResizable"
#define XtCYResizable		"YResizable"
#define XtNyAttachBottom	"yAttachBottom"
#define XtCYAttachBottom	"YAttachBottom"
#define XtNyAttachOffset	"yAttachOffset"
#define XtCYAttachOffset	"YAttachOffset"



/*  ImageEdit resource definitions  */

#define XtNpixelScale		"pixelScale"
#define XtCPixelScale		"PixelScale"

#define XtNgridThickness	"gridThickness"
#define XtCGridThickness	"GridThickness"

#define XtNimage		"image"
#define XtCImage		"Image"

#define XtNdrawColor		"drawColor"
#define XtNeraseColor		"eraseColor"

#define XtNeraseOn		"eraseOn"
#define XtCEraseOn		"EraseOn"


/*  MenuBtn esource manager definitions  */

#define XtNlabelType	      "labelType"
#define XtCLabelType	      "LabelType"
#define XtRLabelType	      "LabelType"
#define XtNlabelImage         "labelImage"
#define XtCLabelImage         "LabelImage"
#define XtNcascadeImage       "cascadeImage"
#define XtCCascadeImage       "CascadeImage"
#define XtNmarkImage          "markImage"
#define XtCMarkImage          "MarkImage"
#define XtNsetMark            "setMark"
#define XtCSetMark            "SetMark"
#define XtNcascadeOn          "cascadeOn"
#define XtCCascadeOn	      "CascadeOn"
#define XtNinvertOnEnter	"invertOnEnter"
#define XtCInvertOnEnter	"InvertOnEnter"
#define XtNmgrOverrideMnemonic  "mgrOverrideMnemonic"
#define XtCMgrOverrideMnemonic  "MgrOverrideMnemonic"
#define XtNcascadeSelect      "cascadeSelect"
#define XtNcascadeUnselect    "cascadeUnselect"
#define XtNmenuMgrId	      "menuMgrId"
#define XtCMenuMgrId	      "MenuMgrId"


/*  ScrollBar resource definitions  */

#define XtNscrollbarOrientation		"scrollbarOrientation"
#define XtNselectionColor		"selectionColor"

#define XtNinitialDelay			"initialDelay"
#define XtCInitialDelay			"InitialDelay"
#define XtNrepeatRate			"repeatRate"
#define XtCRepeatRate			"RepeatRate"
#define XtNgranularity			"granularity"
#define XtCGranularity			"Granularity"

/*  Resources for PushButton  */

#define XtNinvertOnSelect	"invertOnSelect"
#define XtCInvertOnSelect	"InvertOnSelect"
#define XtNtoggle               "toggle"
#define XtCToggle               "Toggle"


/*  Resources for Toggle  */

#define XtNsquare	"square"
#define XtCSquare	"Square"
#define XtNselectColor	"selectColor"

/*  Resources for Vertical Paned Window  */

#define XtNallowResize		"allowResize"
#define XtNsashIndent		"sashIndent"
#define XtCSashIndent           "SashIndent"
#define XtNrefigureMode		"refigureMode"
#define XtNpadding              "padding"
#define XtCPadding              "Padding"
#define XtNmin			"min"
#ifndef XtCMin
#define XtCMin			"Min"
#endif
#define XtNmax			"max"
#ifndef XtCMax
#define XtCMax			"Max"
#endif
#define XtNskipAdjust		"skipAdjust"
#define XtNframed               "framed"
#define XtNborderFrame          "borderFrame"


/*  WorkSpace resources  */

#define XtNexpose	"expose"
#ifndef XtNresize
#define XtNresize	"resize"
#endif
#define XtNkeyDown	"keyDown"


/***********************************************************************
 *
 * Cascading MenuPane Widget Resources
 *
 ***********************************************************************/

#define XtRTitlePositionType    "TitlePositionType"


/***********************************************************************
 *
 * Popup Menu Manager Widget Resources
 *
 ***********************************************************************/

#define XtNstickyMenus        "stickyMenus"
#define XtCStickyMenus        "StickyMenus"


/****
 *
 * Menu stuff from hpwm
 *
 ****/
#define XtNtopShadowColor	"topShadowColor"
#define XtNbottomShadowColor	"bottomShadowColor"
#define XtNbottomShadowTile	"bottomShadowTile"
#define XtCBottomShadowTile	"BottomShadowTile"
#define XtNtopShadowTile	"topShadowTile"
#define XtCTopShadowTile	"TopShadowTile"


/***********************************************************************
 *
 * Pulldown Menu Manager Widget Resources
 *
 ***********************************************************************/


#define XtNallowCascades        "allowCascades"
#define XtCAllowCascades        "AllowCascades"
#define XtNpulldownBarId        "pulldownBarId"
#define XtCPulldownBarId        "PulldownBarId"


/***********************************************************************
 *
 * Cascading MenuPane Widget Attribute Definitions
 *
 ***********************************************************************/

#define XwTOP    0x01
#define XwBOTTOM 0x02
#define XwBOTH   XwTOP|XwBOTTOM

/***********************************************************************
 *
 * Static Text resource string definitions
 *
 ***********************************************************************/
#define XtNstrip	"strip"
#define XtCStrip	"Strip"

/***********************************************************************
 *
 * TitleBar resource string definitions
 *
 ***********************************************************************/
#define XtRTilePix           	"TilePix"
#define XtNtitlePrecedence    	"titlePrecedence"
#define XtCTitlePrecedence    	"TitlePrecedence"
#define XtNtitleForeground   	"titleForeground"
#define XtNtitleBackground   	"titleBackground"
#define XtNtitleRegion      	"titleRegion"
#define XtCTitleRegion      	"TitleRegion"
#define XtNtitlePosition    	"titlePosition"
#define XtCTitlePosition    	"TitlePosition"
#define XtNtitleRPadding    	"titleRPadding"
#define XtCTitleRPadding    	"TitleRPadding"
#define XtNtitleLPadding    	"titleLPadding"
#define XtCTitleLPadding    	"TitleLPadding"
#define XtNtitleBorderWidth 	"titleBorderWidth"
#define XtCTitleBorderWidth 	"TitleBorderWidth"
#define XtNtitleTranslations	"titleTranslations"
#define XtCTitleTranslations	"TitleTranslations"
#define XtNtitleHSpace      	"titleHSpace"
#define XtCTitleHSpace      	"TitleHSpace"
#define XtNtitleVSpace      	"titleVSpace"
#define XtCTitleVSpace      	"TitleVSpace"
#define XtNtitleSelect      	"titleSelect"
#define XtNtitleRelease      	"titleRelease"
#define XtNtitleNextTop      	"titleNextTop"
#define XtNtitlebarTile     	"titlebarTile"
#define XtNenter             	"enter"
#define XtNleave             	"leave"
#define XtNregion           	"region"
#define XtNposition         	"position"
#define XtNlPadding         	"lPadding"
#define XtCLPadding         	"LPadding"
#define XtNrPadding         	"rPadding"
#define XtCRPadding         	"RPadding"
#define XtNprecedence       	"precedence"
#define XtCPrecedence       	"Precedence"

/***********************************************************************
 *
 * Panel resource string definitions
 *
 ***********************************************************************/
#define XtNtitleToMenuPad        "titleToMenuPad"
#define XtCTitleToMenuPad        "TitleToMenuPad"
#define XtNworkSpaceToSiblingPad "workSpaceToSiblingPad"
#define XtCWorkSpaceToSiblingPad "WorkSpaceToSiblingPad"
#define XtNwidgetType            "widgetType"
#define XtCWidgetType            "WidgetType"
#define XtRWidgetType            "WidgetType"
#define XtNtopLevel              "topLevel"
#define XtCTopLevel              "TopLevel"
#define XtNdisplayTitle          "displayTitle"
#define XtCDisplayTitle          "DisplayTitle"
#define XtNcausesResize          "causesResize" 
#define XtCCausesResize          "CausesResize" 

/************************************************************************
 *
 *  Class record constants for Primitive Widget Meta Class
 *
 ************************************************************************/

extern WidgetClass XwprimitiveWidgetClass;

typedef struct _XwPrimitiveClassRec * XwPrimitiveWidgetClass;
typedef struct _XwPrimitiveRec      * XwPrimitiveWidget;


/*  Tile types used for area filling and patterned text placement  */

#define XwFOREGROUND		0
#define XwBACKGROUND		1
#define Xw25_FOREGROUND		2
#define Xw50_FOREGROUND		3
#define Xw75_FOREGROUND		4
#define XwVERTICAL_TILE		5
#define XwHORIZONTAL_TILE	6
#define XwSLANT_RIGHT		7
#define XwSLANT_LEFT		8


/*  Traversal type definitions  */

#define XwHIGHLIGHT_OFF		0
#define XwHIGHLIGHT_ENTER	1
#define XwHIGHLIGHT_TRAVERSAL	2



/*  Defines for the arrow directions  */

#define XwARROW_UP	0
#define XwARROW_DOWN	1
#define XwARROW_LEFT	2
#define XwARROW_RIGHT	3


/*  Define the new resource types used by Arrow  */

#define XtNarrowDirection	"arrowDirection"
#define XtCArrowDirection	"ArrowDirection"
#define XtRArrowDirection	"arrowDirection"



/*  Border highlighting type defines  */

#define XwPATTERN_BORDER   1
#define XwWIDGET_DEFINED   2


/* Manager Layout Info */

#define XwIGNORE           0
#define XwMINIMIZE         1
#define XwMAXIMIZE         2
#define XwSWINDOW          3   /* Special Setting for Scrolled Window */

#define XtNwidgetType            "widgetType"
#define XtCWidgetType            "WidgetType"
#define XtRWidgetType            "WidgetType"

/************************************************************************
 *
 *  Class record constants for Meta Class Widgets
 *
 ************************************************************************/

extern WidgetClass XwmanagerWidgetClass;

typedef struct _XwManagerClassRec * XwManagerWidgetClass;
typedef struct _XwManagerRec      * XwManagerWidget;



extern WidgetClass XwbuttonWidgetClass;

typedef struct _XwButtonClassRec * XwButtonWidgetClass;
typedef struct _XwButtonRec      * XwButtonWidget;


extern WidgetClass XwmenupaneWidgetClass;

typedef struct _XwMenuPaneClassRec *XwMenuPaneWidgetClass;
typedef struct _XwMenuPaneRec      *XwMenuPaneWidget;


extern WidgetClass XwmenumgrWidgetClass;

typedef struct _XwMenuMgrClassRec *XwMenuMgrWidgetClass;
typedef struct _XwMenuMgrRec      *XwMenuMgrWidget;



#endif _XtXw_h
/* DON'T ADD STUFF AFTER THIS #endif */


