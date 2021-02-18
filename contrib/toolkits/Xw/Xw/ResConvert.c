/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        ResConvert.c
 **
 **   Project:     X Widgets
 **
 **   Description: Contains a function to register all of the resource
 **                type converters used within Xw and the converter
 **                routines.
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


#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/TextEdit.h>
#include <X11/StringDefs.h>


static void _XwCvtStringToTraversalType ();
static void _XwCvtStringToHighlightStyle ();
static void _XwCvtStringToTileType();
static void _XwCvtStringToSlideOrientation();
static void _XwCvtStringToArrowDirection();
static void _XwCvtStringToRowColMode();
static void _XwCvtStringToLayoutPolicy();
static void _XwCvtStringToLabelLocation();
static Boolean StringsAreEqual();
static void _XwCvtStringToAlignment();
static void _XwCvtStringToWidgetType();
static void _XwCvtStringToLayoutType();
static void _XwCvtStringToSeparatorType();
static void _XwCvtStringToSelectionMethod();
static void _XwCvtStringToSelectionStyle();
static void _XwCvtStringToSelectionBias();
static void _XwCvtStringToElementHighlight();
static void _XwCvtStringToDestroyMode();
static void _XwCvtStringToGravity ();
static void _XwCvtStringToTitlePositionType();
static void _XwCvtStringToLabelType();
static void _XwCvtStringToTitleType();
static void  CvtStringToEditMode();
static void  CvtStringToSourceType();
static void  CvtStringToWrap();
static void  CvtStringToWrapBreak();
static void  CvtStringToWrapForm();
static void  CvtStringToScroll();
static void  CvtStringToGrow();

typedef struct {
  unsigned char *string;
  int value;
} XwSetValuePair;

#define NUMVALUESETS            7
#define XwEditTypeValueSetNum   0
#define XwSourceTypeValueSetNum 1
#define XwWrapValueSetNum       2
#define XwWrapFormValueSetNum   3  
#define XwWrapBreakValueSetNum  4
#define XwScrollValueSetNum     5
#define XwGrowValueSetNum       6

static XwSetValuePair XwEditTypeValueSet[] =
    { {(unsigned char *)"edit",   (int) XwtextEdit},
      {(unsigned char *)"append", (int) XwtextAppend},
      {(unsigned char *)"read",   (int) XwtextRead},
	NULL };
static XwSetValuePair XwSourceTypeValueSet[] =
    {{(unsigned char *)"stringsrc",   (int) XwstringSrc},
     {(unsigned char *)"disksrc",     (int) XwdiskSrc},
     {(unsigned char *)"userdefined", (int) XwprogDefinedSrc},
     NULL };
static XwSetValuePair XwWrapValueSet[] =
    {{(unsigned char *)"wrapoff",     (int) XwWrapOff},
     {(unsigned char *)"softwrap",    (int) XwSoftWrap},
     {(unsigned char *)"hardwrap",    (int) XwHardWrap},
     NULL };
static XwSetValuePair XwWrapFormValueSet[] =
    {{(unsigned char *)"sourceform",  (int) XwSourceForm},
     {(unsigned char *)"displayform", (int) XwDisplayForm},
     NULL };
static XwSetValuePair XwWrapBreakValueSet[] =
    {{(unsigned char *)"wrapany",        (int) XwWrapAny},
     {(unsigned char *)"wrapwhitespace", (int) XwWrapWhiteSpace},
     NULL };
static XwSetValuePair XwScrollValueSet[] =
    {{(unsigned char *)"autoscrolloff",        (int) XwAutoScrollOff},
     {(unsigned char *)"autoscrollvertical",   (int) XwAutoScrollVertical},
     {(unsigned char *)"autoscrollhorizontal", (int) XwAutoScrollHorizontal},
     {(unsigned char *)"autoscrollboth",       (int) XwAutoScrollBoth},
     NULL };
static XwSetValuePair XwGrowValueSet[] =
    {{(unsigned char *)"growoff",        (int) XwGrowOff},
     {(unsigned char *)"growhorizontal", (int) XwGrowHorizontal},
     {(unsigned char *)"growvertical",   (int) XwGrowVertical},
     {(unsigned char *)"growboth",       (int) XwGrowBoth},
     NULL };

static XwSetValuePair *valuepairsets[NUMVALUESETS];

/************************************************************************
 *
 *  XwRegisterConverters
 *	Register all of the Xw resource type converters.  Retain a
 *	flag indicating whether the converters have already been
 *	registered.
 *
 ************************************************************************/

XwRegisterConverters ()
{
   static Boolean registered = False;

   if (!registered)
   {
      XtAddConverter (XtRString, XtRTraversalType,
                      _XwCvtStringToTraversalType, NULL, 0);
      XtAddConverter (XtRString, XtRRCMode,
		      _XwCvtStringToRowColMode, NULL, 0);
      XtAddConverter (XtRString, XtRLayout,
		      _XwCvtStringToLayoutPolicy, NULL, 0);
      XtAddConverter (XtRString, XtRHighlightStyle,
		      _XwCvtStringToHighlightStyle, NULL, 0);
      XtAddConverter (XtRString, XtRTileType,
		      _XwCvtStringToTileType, NULL, 0);
      XtAddConverter (XtRString, XtRArrowDirection,
                      _XwCvtStringToArrowDirection, NULL, 0);
      XtAddConverter (XtRString, XtRLabelLocation,
                      _XwCvtStringToLabelLocation, NULL, 0);
      XtAddConverter (XtRString, XtRSlideOrientation,
                      _XwCvtStringToSlideOrientation, NULL, 0);
      XtAddConverter (XtRString, XtRAlignment,
                      _XwCvtStringToAlignment, NULL, 0);
      XtAddConverter (XtRString, XtRWidgetType,
                      _XwCvtStringToWidgetType, NULL, 0);
      XtAddConverter (XtRString, XtRLayoutType,
                      _XwCvtStringToLayoutType, NULL, 0);
      XtAddConverter (XtRString, XtRSeparatorType,
                      _XwCvtStringToSeparatorType, NULL, 0);
      XtAddConverter (XtRString, XtRSelectionMethod,
		      _XwCvtStringToSelectionMethod, NULL,0);
      XtAddConverter (XtRString, XtRSelectionStyle,
		      _XwCvtStringToSelectionStyle, NULL,0);
      XtAddConverter (XtRString, XtRSelectionBias,
		      _XwCvtStringToSelectionBias, NULL,0);
      XtAddConverter (XtRString, XtRElementHighlight,
		      _XwCvtStringToElementHighlight, NULL,0);
      XtAddConverter (XtRString, XtRDestroyMode,
		      _XwCvtStringToDestroyMode, NULL,0);
      XtAddConverter (XtRString, XtRGravity,
		      _XwCvtStringToGravity, NULL,0);
      XtAddConverter (XtRString, XtRTitlePositionType,
                      _XwCvtStringToTitlePositionType, NULL, 0);
      XtAddConverter (XtRString, XtRTitleType, _XwCvtStringToTitleType, 
                      NULL, 0);
      XtAddConverter (XtRString, XtRLabelType, _XwCvtStringToLabelType, 
                      NULL, 0);

  valuepairsets[XwEditTypeValueSetNum] = XwEditTypeValueSet;  
  valuepairsets[XwSourceTypeValueSetNum] = XwSourceTypeValueSet;
  valuepairsets[XwWrapValueSetNum] = XwWrapValueSet;
  valuepairsets[XwWrapFormValueSetNum] = XwWrapFormValueSet;
  valuepairsets[XwWrapBreakValueSetNum] = XwWrapBreakValueSet;
  valuepairsets[XwScrollValueSetNum] = XwScrollValueSet;
  valuepairsets[XwGrowValueSetNum] = XwGrowValueSet;

  XtAddConverter(XtRString, XtREditMode, CvtStringToEditMode, NULL, 0);
  XtAddConverter(XtRString, XtRSourceType, CvtStringToSourceType, NULL, 0);
  XtAddConverter(XtRString, XtRWrap, CvtStringToWrap, NULL, 0);
  XtAddConverter(XtRString, XtRWrapBreak, CvtStringToWrapBreak, NULL, 0);
  XtAddConverter(XtRString, XtRWrapForm, CvtStringToWrapForm, NULL, 0);
  XtAddConverter(XtRString, XtRScroll, CvtStringToScroll, NULL, 0);
  XtAddConverter(XtRString, XtRGrow, CvtStringToGrow, NULL, 0);

  }
   registered = True;
}




/************************************************************************
 *
 *  Resource Conversion Functions
 *     The following set of functions are used by the resource
 *     manager to convert the resource types defined for this class.
 *
 ************************************************************************/


/*  Convert a string to a traversal type.  */

static void _XwCvtStringToTraversalType (args, numArgs, fromVal, toVal) 
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   i = XwHIGHLIGHT_OFF;

   if (((*in_str) == 'H') || ((*in_str) == 'h'))
   {
      if (StringsAreEqual (in_str, "highlight_traversal"))
         i = XwHIGHLIGHT_TRAVERSAL;
      else if (StringsAreEqual (in_str, "highlight_enter"))
         i = XwHIGHLIGHT_ENTER;
   }

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}

/*  Convert a string to a Selection Style.  */

static void _XwCvtStringToSelectionStyle (args, numArgs, fromVal, toVal) 
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   static int i;

   if (StringsAreEqual ("instant", (char *) fromVal -> addr) )
      i = XwINSTANT;
   else if (StringsAreEqual("sticky", (char *) fromVal -> addr))
      i = XwSTICKY;
   else
      i = XwSTICKY;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}

/*  Convert a string to a Selection Method.  */

static void _XwCvtStringToSelectionMethod (args, numArgs, fromVal, toVal) 
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   static int i;

   if (StringsAreEqual ("single", (char *) fromVal -> addr))
      i = XwSINGLE;
   else if (StringsAreEqual ("multiple", (char *) fromVal -> addr))
      i = XwMULTIPLE;
   else
      i = XwSINGLE;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}

/*  Convert a string to a Selection Bias.  */

static void _XwCvtStringToSelectionBias (args, numArgs, fromVal, toVal) 
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   static int i;

   if (StringsAreEqual ("col_bias", (char *) fromVal -> addr) )
      i = XwCOL_BIAS;
   else if (StringsAreEqual ("row_bias", (char *) fromVal -> addr))
      i = XwROW_BIAS;
   else
      i = XwNO_BIAS;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}

/*  Convert a string to a element highlight.  */

static void _XwCvtStringToElementHighlight (args, numArgs, fromVal, toVal) 
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   static int i;

   if (StringsAreEqual ("border", (char *) fromVal -> addr))
      i = XwBORDER;
   else if (StringsAreEqual ("invert", (char *) fromVal -> addr))
      i = XwINVERT;
   else
      i = XwINVERT;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}

/*  Convert a string to a destroy mode.  */

static void _XwCvtStringToDestroyMode (args, numArgs, fromVal, toVal) 
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   static int i;

   if (StringsAreEqual ("no_shrink", (char *) fromVal -> addr))
      i = XwNO_SHRINK;
   else if (StringsAreEqual ("shrink_all", (char *) fromVal -> addr))
      i = XwSHRINK_ALL;
   else
      i = XwSHRINK_COLUMN;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}

/******************************************************/
/*  Convert a string to a ButtonClass Label Location. */
/******************************************************/
static void _XwCvtStringToLabelLocation (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

    if (((*in_str)== 'L') || ((*in_str)=='l'))
        {
           if (StringsAreEqual(in_str,"left")) i = XwLEFT;
	}
   else
      i = XwRIGHT;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}



/*****************************************************/
/*  Convert a string to a Row/Column selection type. */
/*****************************************************/
static void _XwCvtStringToRowColMode (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

    if (((*in_str)== 'O') || ((*in_str)=='o'))
        {
           if (StringsAreEqual(in_str,"one_of_many")) i = XwONE_OF_MANY;
	}
   else
      i = XwN_OF_MANY;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}



/********************************************************/
/*  Convert a string to ManagerClass layout mode type.  */
/********************************************************/
static void _XwCvtStringToLayoutPolicy (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   i = XwMINIMIZE;

   switch(*in_str)
   {
      case 'I':
      case 'i':
           if (StringsAreEqual(in_str,"ignore")) i = XwIGNORE;
	   break;
	   
      case 'M':
      case 'm':
           if (StringsAreEqual(in_str,"maximize")) i = XwMAXIMIZE;

   }

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}



static Boolean StringsAreEqual(in_str, test_str)
char *in_str, *test_str;
{
   int i, j;

   for (;;)
   {
      i = *in_str;
      j = *test_str;

      if (isupper(i)) i = tolower(i);
      if (isupper(j)) j = tolower(j);
      if (i != j) return (False);
      if (i == 0) return (True);

      in_str++;
      test_str++;
   }
}   


/*  Convert a string to a highlight type.  */

static void _XwCvtStringToHighlightStyle (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   i = XwWIDGET_DEFINED;

   if (((*in_str) == 'P') || ((*in_str) == 'p'))
      if (StringsAreEqual (in_str, "pattern_border"))
         i = XwPATTERN_BORDER;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}




/*  Convert a string to an tile type.  */

static void _XwCvtStringToTileType (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   i = Xw50_FOREGROUND;

   switch (*in_str)
   {
      case 'F':
      case 'f':
         if (StringsAreEqual (in_str, "foreground")) i = XwFOREGROUND;
      break;

      case 'B':
      case 'b':
         if (StringsAreEqual (in_str, "background")) i = XwBACKGROUND;
      break;

      case '2':
         if (StringsAreEqual (in_str, "25_foreground")) i = Xw25_FOREGROUND;
      break;

      case '5':
         if (StringsAreEqual (in_str, "50_foreground")) i = Xw50_FOREGROUND;
      break;

      case '7':
         if (StringsAreEqual (in_str, "75_foreground")) i = Xw75_FOREGROUND;
      break;

      case 'V':
      case 'v':
         if (StringsAreEqual (in_str, "vertical_tile")) i = XwVERTICAL_TILE;
      break;

      case 'H':
      case 'h':
         if (StringsAreEqual(in_str,"horizontal_tile")) i = XwHORIZONTAL_TILE;
      break;

      case 'S':
      case 's':
         if (StringsAreEqual (in_str, "slant_left")) i = XwSLANT_LEFT;
         else if (StringsAreEqual (in_str, "slant_right")) i = XwSLANT_RIGHT;
      break;
   }


   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}




/*  Convert a string to a slide orientation.  */

static void _XwCvtStringToSlideOrientation (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   i = XwVERTICAL;

   if (((*in_str) == 'H') || ((*in_str) == 'h'))
      if (StringsAreEqual (in_str, "horizontal")) i = XwHORIZONTAL;

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}



/*  Convert a string to an arrow direction.  */

static void _XwCvtStringToArrowDirection (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   i = XwARROW_UP;

   if (((*in_str) == 'A') || ((*in_str) == 'a'))
   {
      if (StringsAreEqual (in_str, "arrow_down")) i = XwARROW_DOWN;
      else if (StringsAreEqual (in_str, "arrow_left")) i = XwARROW_LEFT;
      else if (StringsAreEqual (in_str, "arrow_right")) i = XwARROW_RIGHT;
   }

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}


/*************************************<->*************************************
 *
 *   static  void _XwCvtStringToAlignment()
 *		Display	*display;
 *		XrmValue	*p_fromVal;
 *		XrmValue	*toVal;
 *
 *	Description:
 *	-----------
 *		This is a type conversion procedure for the resource manager.
 *		converts strings to type XwAlignment.  The conversion is case
 *		insensitive.
 *
 *	Inputs:
 *	------
 *		See "Xlib - C Language X Interface Protocol Version 11"
 *
 *	Outputs:
 *	-------
 *		See "Xlib - C Language X Interface Protocol Version 11"
 *
 *	Procedures Called
 *	-----------------
 *		StringsAreEqual
 *
 *************************************<->***********************************/
static void _XwCvtStringToAlignment(args,num,p_fromVal,toVal)
	XrmValue	*args;
	Cardinal	num;
	XrmValue	*p_fromVal;
	XrmValue	*toVal;
{
	static XwAlignment	Align = XwALIGN_LEFT;
	char	*from_str;
	Boolean	match = FALSE;

	from_str = (char *) (p_fromVal->addr);
	toVal->addr = (caddr_t) &Align;
	toVal->size = sizeof(XwAlignment);

	switch(*from_str)
	{
		case 'L':
		case 'l':
			if (match = StringsAreEqual(from_str,"left"))
				Align = XwALIGN_LEFT;
		break;
		case 'C':
		case 'c':
			if (match = StringsAreEqual(from_str,"center"))
				Align = XwALIGN_CENTER;
		break;
		case 'R':
		case 'r':
			if (match = StringsAreEqual(from_str,"right"))
				Align = XwALIGN_RIGHT;
		break;
		case 'N':
		case 'n':
			if (match = StringsAreEqual(from_str,"none"))
				Align = XwALIGN_NONE;
		break;
		default:
			match = FALSE;
		break;
	}

	if(!match)
	{
		toVal->addr = NULL;
		toVal->size = 0;
	}
}


/*************************************<->*************************************
 *
 *   static  void _XwCvtStringToWidgetType()
 *		Display	*display;
 *		XrmValue	*p_fromVal;
 *		XrmValue	*toVal;
 *
 *	Description:
 *	-----------
 *		This is a type conversion procedure for the resource manager.
 *		converts strings to type XwWidgetType.  The conversion is case
 *		insensitive.
 *
 *	Inputs:
 *	------
 *		See "Xlib - C Language X Interface Protocol Version 11"
 *
 *	Outputs:
 *	-------
 *		See "Xlib - C Language X Interface Protocol Version 11"
 *
 *	Procedures Called
 *	-----------------
 *		StringsAreEqual
 *
 *************************************<->***********************************/
static void _XwCvtStringToWidgetType(args,num,p_fromVal,toVal)
	XrmValue	*args;
	Cardinal	num;
	XrmValue	*p_fromVal;
	XrmValue	*toVal;
{
	static XwWidgetType	WType = XwUNKNOWN;
	char	*from_str;
	Boolean	match = FALSE;

	from_str = (char *) (p_fromVal->addr);
	toVal->addr = (caddr_t) &WType;
	toVal->size = sizeof(XwWidgetType);

	switch(*from_str)
	{
		case 'T':
		case 't':
			if (match = StringsAreEqual(from_str,"title"))
				WType = XwTITLE;
		break;
		case 'P':
		case 'p':
			if (match = StringsAreEqual(from_str,"pulldown"))
				WType = XwPULLDOWN;
		break;
		case 'W':
		case 'w':
			if (match = StringsAreEqual(from_str,"work space"))
				WType = XwWORK_SPACE;
		break;
		case 'U':
		case 'u':
			if (match = StringsAreEqual(from_str,"unknown"))
				WType = XwUNKNOWN;
		break;
		default:
			match = FALSE;
		break;
	}

	if(!match)
	{
		toVal->addr = NULL;
		toVal->size = 0;
	}
}


/************************************************************************
 *
 *  _XwCvtStringToLayoutType
 *	Convert a resource string to a layout type.
 *
 ************************************************************************/

static void _XwCvtStringToLayoutType (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   i = XwREQUESTED_COLUMNS;

   if (((*in_str) == 'M') || ((*in_str) =='m'))
   {
      if (StringsAreEqual (in_str, "maximum_columns")) i = XwMAXIMUM_COLUMNS;
      else if (StringsAreEqual (in_str, "maximum_unaligned"))
         i = XwMAXIMUM_UNALIGNED;
   }

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}



/************************************************************************
 *
 *  _XwCvtStringToSeparatorType
 *      Convert a resource string to a menu separator type.
 *
 ************************************************************************/

static void _XwCvtStringToSeparatorType (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
   char * in_str = (char *) (fromVal->addr);
   static int i;

   i = XwSINGLE_LINE;

   switch(*in_str)
   {
      case 'D':
      case 'd':
         if (StringsAreEqual (in_str, "double_line")) i = XwDOUBLE_LINE;
         else if (StringsAreEqual (in_str, "double_dashed_line"))
            i = XwDOUBLE_DASHED_LINE;
      break;

      case 'S':
      case 's':
         if (StringsAreEqual (in_str, "single_dashed_line"))
            i = XwSINGLE_DASHED_LINE;
      break;

      case 'N':
      case 'n':
         if (StringsAreEqual (in_str, "no_line")) i = XwNO_LINE;
      break;
   }

   (*toVal).size = sizeof (int);
   (*toVal).addr = (caddr_t) &i;
}

/************************************************************************
 *
 *  _XwCvtStringToGravity
 *      Convert a resource string to a gravity value.
 *
 ************************************************************************/

static void _XwCvtStringToGravity (args, numArgs, fromVal, toVal)
XrmValue * args;
Cardinal   numArgs;
XrmValue * fromVal;
XrmValue * toVal;

{
	static int gravity = CenterGravity;
	char	*from_str;
	Boolean	match = FALSE;

	from_str = (char *) (fromVal->addr);
	toVal->addr = (caddr_t) &gravity;
	toVal->size = sizeof(int);

	switch (*from_str)
	{
		case 'N':
		case 'n':
			if (match = StringsAreEqual(from_str,"NorthGravity"))
				gravity = NorthGravity;
			else if (match = StringsAreEqual(from_str,"NorthWestGravity"))
				gravity = NorthWestGravity;
			else if (match = StringsAreEqual(from_str,"NorthEastGravity"))
				gravity = NorthEastGravity;
		break;
		case 'S':
		case 's':
			if (match = StringsAreEqual(from_str,"SouthGravity"))
				gravity = SouthGravity;
			else if (match = StringsAreEqual(from_str,"SouthWestGravity"))
				gravity = SouthWestGravity;
			else if (match = StringsAreEqual(from_str,"SouthEastGravity"))
				gravity = SouthEastGravity;
		break;
		case 'W':
		case 'w':
			if (match = StringsAreEqual(from_str,"WestGravity"))
				gravity = WestGravity;
		break;
		case 'E':
		case 'e':
			if (match = StringsAreEqual(from_str,"EastGravity"))
				gravity = EastGravity;
		break;
		case 'C':
		case 'c':
			if (match = StringsAreEqual(from_str,"CenterGravity"))
				gravity = CenterGravity;
		break;
		default:
			match = FALSE;
		break;
	}

	if(!match)
	{
		toVal->addr = NULL;
		toVal->size = 0;
	}
}


static void _XwCvtStringToTitleType (args, num_args, fromVal, toVal)

   XrmValuePtr args;
   Cardinal  * num_args;
   XrmValuePtr fromVal;
   XrmValuePtr toVal;

{
   static int i;
   String source = (String) fromVal->addr;

   if (source)
   {
      if (strcmp ("string", source) == 0)
         i = XwSTRING;
      else if (strcmp ("image", source) == 0)
         i = XwIMAGE;
      else
      {
         i = XwSTRING;
         XtWarning (
            "MenuPane: Unknown title type specified; using string title");
      }
   }
   else
   {
      i = XwSTRING;
      XtWarning ("MenuPane: Unknown title type specified; using string title");
   }

   (*toVal).size = sizeof(int);
   (*toVal).addr = (caddr_t) &i;
}

static void _XwCvtStringToLabelType (args, num_args, fromVal, toVal)

   XrmValuePtr args;
   Cardinal  * num_args;
   XrmValuePtr fromVal;
   XrmValuePtr toVal;

{
   static int i;

   if (strcmp ("string", (char *) fromVal->addr) == 0)
      i = XwSTRING;
   else if (strcmp ("image", (char *) fromVal->addr) == 0)
      i = XwIMAGE;
   else
   {
      i = XwSTRING;
      XtWarning
	 ("MenuButton: Unknown label type specified; using string label");
   }

   (*toVal).size = sizeof(int);
   (*toVal).addr = (caddr_t) &i;
}

static void _XwCvtStringToTitlePositionType (args, num_args, fromVal, toVal)

   XrmValuePtr args;
   Cardinal * num_args;
   XrmValuePtr fromVal;
   XrmValuePtr toVal;

{
   static int i;
   String source = (char *) fromVal->addr;

   if (source)
   {
      /* Force the comparison to be case insensitive */
      for (i = 0; i < XwStrlen(source); i++)
      {
         if (isupper (source[i]))
            source[i] = tolower(source[i]);
      }

      if (strcmp ("bottom", source) == 0)
         i = XwBOTTOM;
      else if (strcmp ("top", source) == 0)
         i = XwTOP;
      else if (strcmp ("both", source) == 0)
         i = XwTOP | XwBOTTOM;
      else
      {
         i = XwTOP;
         XtWarning ("Cascade: Invalid title position; position set to top");
      }
   }
   else
   {
      i = XwTOP;
      XtWarning ("Cascade: Invalid title position; position set to top");
   }

   (*toVal).size = sizeof(int);
   (*toVal).addr = (caddr_t) &i;
}


/*--------------------------------------------------------------------------+*/
static void SetConverter(fromVal, toVal, setNum)
/*--------------------------------------------------------------------------+*/
     XrmValuePtr fromVal, toVal;
     int setNum;
{
  int j;
  XwSetValuePair *ptr;
  static int value;
  unsigned char *fromstr;

  fromstr = (unsigned char *)fromVal->addr;
  ptr = valuepairsets[setNum];
  for(j=0; *ptr[j].string; j++)
    if ((strcmp(ptr[j].string, fromstr)) == NULL) {
      value = ptr[j].value;
      toVal->size = sizeof(int);
      toVal->addr = (caddr_t)&value;
      return;
    };
  toVal->size = 0;
  toVal->addr = NULL;
}

/*--------------------------------------------------------------------------+*/
static void CvtStringToEditMode(args, num_args, fromVal, toVal)
/*--------------------------------------------------------------------------+*/
    XrmValuePtr *args;  /* unused */
    Cardinal    *num_args; /* unused */
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
  SetConverter(fromVal, toVal, XwEditTypeValueSetNum);
}

/*--------------------------------------------------------------------------+*/
static void CvtStringToSourceType(args, num_args, fromVal, toVal)
/*--------------------------------------------------------------------------+*/
    XrmValuePtr *args;  /* unused */
    Cardinal    *num_args; /* unused */
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
  SetConverter(fromVal, toVal, XwSourceTypeValueSetNum);
}

/*--------------------------------------------------------------------------+*/
static void CvtStringToWrap(args, num_args, fromVal, toVal)
/*--------------------------------------------------------------------------+*/
    XrmValuePtr *args;  /* unused */
    Cardinal    *num_args; /* unused */
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
  SetConverter(fromVal, toVal, XwWrapValueSetNum);
}

/*--------------------------------------------------------------------------+*/
static void CvtStringToWrapForm(args, num_args, fromVal, toVal)
/*--------------------------------------------------------------------------+*/
    XrmValuePtr *args;  /* unused */
    Cardinal    *num_args; /* unused */
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
  SetConverter(fromVal, toVal, XwWrapFormValueSetNum);
}

/*--------------------------------------------------------------------------+*/
static void CvtStringToWrapBreak(args, num_args, fromVal, toVal)
/*--------------------------------------------------------------------------+*/
    XrmValuePtr *args;  /* unused */
    Cardinal    *num_args; /* unused */
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
  SetConverter(fromVal, toVal, XwWrapBreakValueSetNum);
}

/*--------------------------------------------------------------------------+*/
static void CvtStringToScroll(args, num_args, fromVal, toVal)
/*--------------------------------------------------------------------------+*/
    XrmValuePtr *args;  /* unused */
    Cardinal    *num_args; /* unused */
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
  SetConverter(fromVal, toVal, XwScrollValueSetNum);
}

/*--------------------------------------------------------------------------+*/
static void CvtStringToGrow(args, num_args, fromVal, toVal)
/*--------------------------------------------------------------------------+*/
    XrmValuePtr *args;  /* unused */
    Cardinal    *num_args; /* unused */
    XrmValuePtr	fromVal;
    XrmValuePtr	toVal;
{
  SetConverter(fromVal, toVal, XwGrowValueSetNum);
}
