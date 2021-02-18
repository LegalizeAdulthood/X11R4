/* -*-C-*-
********************************************************************************
*
* File:         w_resources.c
* RCS:          $Header: w_resources.c,v 1.2 89/11/25 04:28:44 mayer Exp $
* Description:  WINTERP interfaces to Motif and Xtoolkit resources
* Author:       Niels Mayer, HPLabs
* Created:      Sat Jul 22 04:42:12 1989
* Modified:     Sat Nov 25 04:28:34 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r4 contrib tape release
*
* WINTERP 1.0 Copyright 1989 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
**
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: w_resources.c,v 1.2 89/11/25 04:28:44 mayer Exp $";

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Vendor.h>
#include <Xm/Xm.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"

extern char temptext[];	/* from winterp.c */
void Wres_Free_This_Later();

/*
 * The following are variables holding the bounds of various FIXNUM (long)
 * subranges before passing them as  XmRInt, XmRShort, and XmRDimension.
 * I'm assuming your machine uses 2's complement arithmetic, and that
 * your machine defines some machine-dependent values in <values.h>.
 * This will undoubtedly cause portability problems. ANSI C is supposed to
 * solve such problems.
 *
 * If you have any suggestions on doing this stuff correctly, I'm all
 * ears.  -- Niels.
 */
#include <values.h>
long max_long         = MAXLONG;
long min_long         = -MAXLONG - 1;
long max_int          = MAXINT;
long min_int          = -MAXINT - 1;
long max_short        = MAXSHORT;
long min_short        = -MAXSHORT - 1;
long max_unsigned_int = MAXINT; /* this isn't quite right, but it'll do */

/*
 * all the symbols below represent lisp versions of motif "constants"... 
 * NOTE: if I were to redesign the way I lookup symbols<--->motif-constants,
 * in this module, I'd place all the  constants and symbols in a static array
 * and have procedures that look up symbol-->constant values and
 * constant-->symbol values. I will probably make this change if and when
 * I add a resource browser/editor to WINTERP -- enumerated choices will come
 * up in a menu. For now, this will do. Why I didn't do that in the first place:
 * -- it's much easier to write editor macros than to think!
 */
static LVAL s_XmALIGNMENT_BEGINNING, s_XmALIGNMENT_CENTER, s_XmALIGNMENT_END;
static LVAL s_XmN_OF_MANY, s_XmONE_OF_MANY;
static LVAL s_XmMULTI_LINE_EDIT, s_XmSINGLE_LINE_EDIT;
static LVAL s_XmSTRING, s_XmPIXMAP;
static LVAL s_XmNO_ORIENTATION, s_XmVERTICAL, s_XmHORIZONTAL;
static LVAL s_XmNO_PACKING, s_XmPACK_TIGHT, s_XmPACK_COLUMN, s_XmPACK_NONE;
static LVAL s_XmWORK_AREA, s_XmMENU_BAR, s_XmMENU_POPUP, s_XmMENU_PULLDOWN, s_XmMENU_OPTION;
static LVAL s_XmSTRING_DIRECTION_L_TO_R, s_XmSTRING_DIRECTION_R_TO_L;
static LVAL s_Button1, s_Button2, s_Button3, s_Button4, s_Button5;
static LVAL s_XmPIXELS, s_Xm100TH_MILLIMETERS, s_Xm1000TH_INCHES, s_Xm100TH_POINTS, s_Xm100TH_FONT_UNITS;
static LVAL s_XmRESIZE_NONE, s_XmRESIZE_GROW, s_XmRESIZE_ANY;
static LVAL s_XmATTACH_NONE, s_XmATTACH_FORM, s_XmATTACH_OPPOSITE_FORM, s_XmATTACH_WIDGET, s_XmATTACH_OPPOSITE_WIDGET, s_XmATTACH_POSITION, s_XmATTACH_SELF;
static LVAL s_XmSHADOW_ETCHED_IN, s_XmSHADOW_ETCHED_OUT, s_XmSHADOW_IN, s_XmSHADOW_OUT;
static LVAL s_XmARROW_UP, s_XmARROW_DOWN, s_XmARROW_LEFT, s_XmARROW_RIGHT;
static LVAL s_XmNO_LINE, s_XmSINGLE_LINE, s_XmDOUBLE_LINE, s_XmSINGLE_DASHED_LINE, s_XmDOUBLE_DASHED_LINE;
static LVAL s_XmMAX_ON_TOP, s_XmMAX_ON_BOTTOM, s_XmMAX_ON_LEFT, s_XmMAX_ON_RIGHT;
static LVAL s_XmSINGLE_SELECT, s_XmMULTIPLE_SELECT, s_XmEXTENDED_SELECT, s_XmBROWSE_SELECT;
static LVAL s_XmVARIABLE, s_XmCONSTANT, s_XmRESIZE_IF_POSSIBLE;
static LVAL s_XmAUTOMATIC, s_XmAPPLICATION_DEFINED;
static LVAL s_XmSTATIC, s_XmAS_NEEDED;
static LVAL s_XmTOP_LEFT, s_XmBOTTOM_LEFT, s_XmTOP_RIGHT, s_XmBOTTOM_RIGHT;
static LVAL s_XmDIALOG_MODELESS, s_XmDIALOG_APPLICATION_MODAL, s_XmDIALOG_SYSTEM_MODAL;
static LVAL s_XmDIALOG_ERROR, s_XmDIALOG_INFORMATION, s_XmDIALOG_MESSAGE, s_XmDIALOG_QUESTION, s_XmDIALOG_WARNING, s_XmDIALOG_WORKING, s_XmDIALOG_PROMPT, s_XmDIALOG_SELECTION, s_XmDIALOG_COMMAND;
static LVAL s_XmDESTROY, s_XmUNMAP, s_XmDO_NOTHING, s_XmEXPLICIT, s_XmPOINTER;

/* these are used by :get_child methods on various widget classes. */
LVAL s_XmDIALOG_WORK_AREA, s_XmDIALOG_NONE, s_XmDIALOG_APPLY_BUTTON, s_XmDIALOG_CANCEL_BUTTON,
  s_XmDIALOG_DEFAULT_BUTTON, s_XmDIALOG_OK_BUTTON, s_XmDIALOG_FILTER_LABEL,
  s_XmDIALOG_FILTER_TEXT, s_XmDIALOG_HELP_BUTTON, s_XmDIALOG_LIST,
  s_XmDIALOG_HISTORY_LIST, s_XmDIALOG_LIST_LABEL, s_XmDIALOG_MESSAGE_LABEL,
  s_XmDIALOG_SELECTION_LABEL, s_XmDIALOG_PROMPT_LABEL, s_XmDIALOG_SYMBOL_LABEL,
  s_XmDIALOG_TEXT, s_XmDIALOG_VALUE_TEXT, s_XmDIALOG_COMMAND_TEXT,
  s_XmDIALOG_SEPARATOR, s_XmDIALOG_FILE_SELECTION;

typedef struct _Resource_Class {
  char*    XmR_type;
  int      LVAL_type;
  LVAL     (*resource_val_to_LVAL_converter)( /*caddr_t*/ );
  caddr_t  (*LVAL_to_resource_val_converter)( /*LVAL*/ );
} Resource_Class;



/******************************************************************************/
static LVAL Cvt_XmRAcceleratorTable_to_LVAL(res_val)
     XtAccelerators res_val;
{
  return (res_val ? cv_xtaccelerators(res_val) : NIL);
}

static caddr_t Cvt_LVAL_to_XmRAcceleratorTable(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  return ((caddr_t) get_xtaccelerators(lisp_val));
}

static Resource_Class resclass_XmRAcceleratorTable = {
  XmRAcceleratorTable,		
  XLTYPE_XtAccelerators,
  Cvt_XmRAcceleratorTable_to_LVAL, 
  Cvt_LVAL_to_XmRAcceleratorTable}; /* sizeof (XtAccelerators) */


/******************************************************************************/
static LVAL Cvt_XmRAlignment_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmALIGNMENT_BEGINNING:
    return (s_XmALIGNMENT_BEGINNING);
  case XmALIGNMENT_CENTER:
    return (s_XmALIGNMENT_CENTER);
  case XmALIGNMENT_END:
    return (s_XmALIGNMENT_END);
  default:
    xlfail("Internal error in Cvt_XmRAlignment_to_LVAL().");
  }
}
  

static caddr_t Cvt_LVAL_to_XmRAlignment(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmALIGNMENT_BEGINNING)
    result = XmALIGNMENT_BEGINNING;
  else if (lisp_val == s_XmALIGNMENT_CENTER)
    result = XmALIGNMENT_CENTER;
  else if (lisp_val == s_XmALIGNMENT_END)
    result = XmALIGNMENT_END;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRAlignment must be one of the following symbols [%s %s %s].",
	    (char*) getstring(getpname(s_XmALIGNMENT_BEGINNING)), 
	    (char*) getstring(getpname(s_XmALIGNMENT_CENTER)),
	    (char*) getstring(getpname(s_XmALIGNMENT_END)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRAlignment = {
  XmRAlignment,	
  SYMBOL,			/* XmRAlignment is an enumerated type */
  Cvt_XmRAlignment_to_LVAL, 
  Cvt_LVAL_to_XmRAlignment};	/* sizeof(unsigned char) */




/******************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRBool = {
  XmRBool,
  xxx,	
  Cvt_XmRBool_to_LVAL, 
  Cvt_LVAL_to_XmRBool}; 
/* the only resource found under this type is XmNinput, which I've placed under XmRBoolean */
#endif

/******************************************************************************/

static LVAL Cvt_XmRBoolean_to_LVAL(res_val)
     Boolean res_val;
{
  extern LVAL true;
  if (res_val)
    return (true);
  else 
    return (NIL);
}

static caddr_t Cvt_LVAL_to_XmRBoolean(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  Boolean result;
  if (lisp_val == NIL)
    result = FALSE;
  else
    result = TRUE;
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRBoolean = {
  XmRBoolean,
  FREE,			/* any kind of LVAL is ok here, if == NIL, then false, else true. */
  Cvt_XmRBoolean_to_LVAL, 
  Cvt_LVAL_to_XmRBoolean}; /* sizeof(BOOLEAN) == sizeof(char) */


/******************************************************************************/
static LVAL Cvt_XmRCallback_to_LVAL(res_val)
     caddr_t res_val;
{
    xlfail(":get_values on callbacks not supported.");
}
static caddr_t Cvt_LVAL_to_XmRCallback(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
    xlerror("Setting callbacks not supported. Use methods :set_callback or :add_callback instead.", lisp_val);
}
static Resource_Class resclass_XmRCallback = {
  XmRCallback,	
  XLTYPE_XtCallbackList,	
  Cvt_XmRCallback_to_LVAL, 
  Cvt_LVAL_to_XmRCallback};	/* sizeof(XtCallbackList) which
				   is a pointer to a struct holding a *fn()
				   and the closure data. */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRCallProc = {
  XmRCallProc,	
  xxx,	
  Cvt_XmRCallProc_to_LVAL, 
  Cvt_LVAL_to_XmRCallProc}; 
/* There are no Xt resources of this type. This resource type is used in 
Xt:Resources.c only.. It is used as a parameter in XtResource type 
specifications to indicate that the default value is to be taken from the
results of a function call. This is used, during widget creation to 
initialize resources to dynamic values, such as color, screen depth, etc. */
#endif


/********************************************************************************/
static LVAL Cvt_XmRChar_to_LVAL(res_val)
     char res_val;
{
  return (cvchar((int) res_val));
}

static caddr_t Cvt_LVAL_to_XmRChar(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  char result =  (char) getchcode(lisp_val);
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRChar = {
  XmRChar,
  CHAR,	
  Cvt_XmRChar_to_LVAL,		/* use cvchar of ascii value of character */
  Cvt_LVAL_to_XmRChar};		/* sizeof(char) */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRColor = {
  XmRColor,
  xxx,
  Cvt_XmRColor_to_LVAL, 
  Cvt_LVAL_to_XmRColor}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRCursor = {
  XmRCursor,
  xxx,	
  Cvt_XmRCursor_to_LVAL, 
  Cvt_LVAL_to_XmRCursor}; 
/* no resource names found for this type, except for 
   XmNmenuCursor, XmCCursor. These are only settable in your .Xdefaults
   and can only be set at application startup time. I'm ignoring these
   resources for now since they aren't associated with a widget, but rather
   with an application. An application may set this resource programatically 
   via  XmSetMenuCursor(). */
#endif


/********************************************************************************/

static LVAL Cvt_XmRDimension_to_LVAL(res_val)
     Cardinal res_val;
{
  return (cvfixnum((FIXTYPE) res_val));
}

static caddr_t Cvt_LVAL_to_XmRDimension(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned int result;
  long         value = (long) getfixnum(lisp_val); 

  if (value < 0L)
    xlerror("In Widget Arglist, resource values of type XmRDimension must be a FIXNUM >= 0.", lisp_val);
  else if (value > max_unsigned_int) {
    sprintf(temptext,
	    "In Widget Arglist, a resource value of type XmRDimension nust be a FIXNUM <= %ld.",
	    max_unsigned_int);
    xlerror(temptext, lisp_val);
  }
  result = (unsigned int) value;
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRDimension = {
  XmRDimension,	
  FIXNUM,			/* note that only positive FIXNUMS ok */
  Cvt_XmRDimension_to_LVAL, 
  Cvt_LVAL_to_XmRDimension}; /* sizeof(Cardinal) == sizeof(unsigned int) */

/*
 * Note potential BUG: in spapshot #11, XmNmarginWidth is declared as
 * short in Xm:BulletinB.c Xm:DrawingA.c Xm:Frame.c Xm:Label.c/Xm:LabelG.c
 * Xm:PanedW.c and Xm:Text.c, but declared as Dimension (unsigned int) for
 * RowColumn.c. I'm going with the majority here, but it'll make R/C buggy.
 *
 * static Resource_Instance resinst_XmNmarginWidth = { -- Xm:RowColumn.c --
 *   &resclass_XmRDimension,
 *   XmNmarginWidth};
 */
/* XmNrcmarginWidth is an alias for XmNmarginWidth until the fuckheads fix
   motif properly */
/*
 * Note potential BUG: in spapshot #11, XmNmarginHeight is declared as
 * short in Xm:BulletinB.c Xm:DrawingA.c Xm:Frame.c Xm:Label.c/Xm:LabelG.c
 * Xm:PanedW.c and Xm:Text.c, but declared as Dimension (unsigned int) for
 * RowColumn.c. I'm going with the majority here, but it'll make R/C buggy.
 *
 * static Resource_Instance resinst_XmNmarginHeight = { -- Xm:RowColumn.c --
 *  &resclass_XmRDimension,
 *  XmNmarginHeight};
 */
/* XmNrcmarginHeight is an alias for XmNmarginHeight until the fuckheads fix
   motif properly */
/*
 * Note potential BUG: in spapshot #11, XmNSpacing is declared as short
 * in ToggleB.c/ToggleBG.c; declared as Dimension (unsigned int) in 
 * RowColumn.c (listed as short in docs); and declared as int in PanedW.c.
 * It seems like this might cause weird results to be retrieved if we're
 * retrieving a short as an unsigned int. Perhaps we should retrieve as a
 * short? In either case this is a bug in Motif.
 */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRDisplay = {
  XmRDisplay,
  xxx,	
  Cvt_XmRDisplay_to_LVAL, 
  Cvt_LVAL_to_XmRDisplay}; 
/* no resource names found for this type */
#endif


/********************************************************************************/

static LVAL Cvt_XmREditMode_to_LVAL(res_val)
     int res_val;
{
  switch (res_val) {
  case XmMULTI_LINE_EDIT:
    return (s_XmMULTI_LINE_EDIT);
  case XmSINGLE_LINE_EDIT:
    return (s_XmSINGLE_LINE_EDIT);
  default:
    xlfail("Internal error in Cvt_XmREditMode_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmREditMode(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  int result;			/* all other enumerated types are unsigned int, but this is the way it's listed in the source */
  if (lisp_val == s_XmMULTI_LINE_EDIT)
    result = XmMULTI_LINE_EDIT;
  else if (lisp_val == s_XmSINGLE_LINE_EDIT)
    result = XmSINGLE_LINE_EDIT;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmREditMode must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmMULTI_LINE_EDIT)), 
	    (char*) getstring(getpname(s_XmSINGLE_LINE_EDIT)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmREditMode = {
  XmREditMode,	
  SYMBOL,			/* XmREditMode is an enumerated type */
  Cvt_XmREditMode_to_LVAL, 
  Cvt_LVAL_to_XmREditMode};	/* sizeof(int) !! */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRFile = {
  XmRFile,
  xxx,	
  Cvt_XmRFile_to_LVAL, 
  Cvt_LVAL_to_XmRFile}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRFont = {
  XmRFont,
  xxx,	
  Cvt_XmRFont_to_LVAL, 
  Cvt_LVAL_to_XmRFont}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
static LVAL Cvt_XmRFontList_to_LVAL(res_val)
     caddr_t res_val;
{
    xlfail("XmRFontList --> Lisp converter not implemented.");
}
static caddr_t Cvt_LVAL_to_XmRFontList(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  /*
    Note that all XmFontList resources are copied into the Motif
    widget's space and thereafter managed by that widget. That
    means we don't need to worry about making an additional
    reference to the lisp XmFontList value that was passed in.
    When the lisp XmString is no longer referenced, it will
    be freed via garbage collection. 
  */
  xlerror("Lisp --> XmRFontList converter not implemented.", lisp_val);
}
static Resource_Class resclass_XmRFontList = {
  XmRFontList,	
  XLTYPE_XmFontList,	
  Cvt_XmRFontList_to_LVAL, 
  Cvt_LVAL_to_XmRFontList};	/* sizeof (XmFontList) */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRFontStruct = {
  XmRFontStruct,	
  xxx, 
  Cvt_XmRFontStruct_to_LVAL, 
  Cvt_LVAL_to_XmRFontStruct}; -- sizeof(XFontStruct *)
/* Note that the following are the only resources names that had a
representation of XmRFontStruct. Since these don't seem to be
accessible by the user, I'm not going to bother to interface
these, especially since it would involve adding a new type to lisp...
     XmNfont				-- Xt:VendorI.c Xm:ResInd.c
     XmCFont				-- Xt:VendorI.c Xm:ResInd.c
*/
#endif

/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRFunction = {
  XmRFunction,	
  xxx,	
  Cvt_XmRFunction_to_LVAL, 
  Cvt_LVAL_to_XmRFunction}; 
/* 
Note that the following resources are the only ones having a representation
XmRFunction. The value associated with these resources is a C function
pointer. Since none of these resources are documented, I'm assuming that
they are for "internal" use only. Therefore I am not going to make a lisp
interface for this representation type and it's associated resource names.

XmNinsertPosition		-- Xt:Composite.c sizeof(XtOrderProc) 
XmNcreatePopupChildProc		-- Xt:Shell.c sizeof(XtCreatePopupChildProc) 
XmNoutputCreate			-- Xm:Text.c sizeof(OutputCreateProc) 
XmNinputCreate			-- Xm:Text.c sizeof(InputCreateProc) 
*/
#endif

/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRGeometry = {
  XmRGeometry,	
  xxx,	
  Cvt_XmRGeometry_to_LVAL, 
  Cvt_LVAL_to_XmRGeometry}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRImmediate = {
  XmRImmediate,	
  xxx, 
  Cvt_XmRImmediate_to_LVAL, 
  Cvt_LVAL_to_XmRImmediate}; 
/* There are no Xm resources of this type. This resource type is used in 
Xt:Resources.c only.. It is used as a parameter in XtResource type 
specifications to indicate that the default value is an immediate value that
will fit into a caddr_t, as opposed to a pointer to a value. */
#endif


/********************************************************************************/

static LVAL Cvt_XmRIndicatorType_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmN_OF_MANY:
    return (s_XmN_OF_MANY);
  case XmONE_OF_MANY:
    return (s_XmONE_OF_MANY);
  default:
    xlfail("Internal error in Cvt_XmRIndicatorType_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRIndicatorType(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmN_OF_MANY)
    result = XmN_OF_MANY;
  else if (lisp_val == s_XmONE_OF_MANY)
    result = XmONE_OF_MANY;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRIndicatorType must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmN_OF_MANY)), 
	    (char*) getstring(getpname(s_XmONE_OF_MANY)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRIndicatorType = {
  XmRIndicatorType,	
  SYMBOL,			/* XmRIndicatorType is an enumerated type */
  Cvt_XmRIndicatorType_to_LVAL, 
  Cvt_LVAL_to_XmRIndicatorType}; /* sizeof(unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRInt_to_LVAL(res_val)
     int res_val;
{
  return (cvfixnum((FIXTYPE) res_val));
}

static caddr_t Cvt_LVAL_to_XmRInt(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  int  result;
  long value = (long) getfixnum(lisp_val); 

  if (value < min_int) {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRInt must be a FIXNUM >= %ld.",
	    min_int);
    xlerror(temptext, lisp_val);
  }
  else if (value > max_int) {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRInt must be a FIXNUM <= %ld.",
	    max_int);
    xlerror(temptext, lisp_val);
  }
  result = (int) value;
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRInt = {
  XmRInt,
  FIXNUM,	
  Cvt_XmRInt_to_LVAL,	
  Cvt_LVAL_to_XmRInt};		/* sizeof(int) */

/* 
 * this has been moved to type XmRDimension...
 * static Resource_Instance resinst_XmNspacing = { -- Xm:PanedW.c Xm:ScrolledW.c --
 *   &resclass_XmRInt,
 *   XmNspacing};
 */


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRJustify = {
  XmRJustify,
  xxx,	
  Cvt_XmRJustify_to_LVAL, 
  Cvt_LVAL_to_XmRJustify}; 
/* no resource names found for this type */
#endif

/********************************************************************************/

static LVAL Cvt_XmRLabelType_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmPIXMAP:
    return (s_XmPIXMAP);
  case XmSTRING:
    return (s_XmSTRING);
  default:
    xlfail("Internal error in Cvt_XmRLabelType_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRLabelType(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmPIXMAP)
    result = XmPIXMAP;
  else if (lisp_val == s_XmSTRING)
    result = XmSTRING;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRLabelType must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmPIXMAP)), 
	    (char*) getstring(getpname(s_XmSTRING)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRLabelType = {
  XmRLabelType,	
  SYMBOL,			/* XmRLabelType is an enumerated type */
  Cvt_XmRLabelType_to_LVAL, 
  Cvt_LVAL_to_XmRLabelType};	/* sizeof(unsigned char) */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRLongBoolean = {
  XmRLongBoolean,	
  xxx, 
  Cvt_XmRLongBoolean_to_LVAL,	
  Cvt_LVAL_to_XmRLongBoolean}; 
/* no resource names found for this type */
#endif

/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRMenuWidget = {
  XmRMenuWidget,
  xxx,
  Cvt_XmRMenuWidget_to_LVAL,
  Cvt_LVAL_to_XmRMenuWidget};	
/* NOTE: all widgets of type XmRMenuWidget have been lumped in with the
   pseudo-representation-type "Widget", which takes the place of the
   missing XmRWidget type... */
#endif

/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRMnemonic = {
  XmRMnemonic,	
  xxx,	
  Cvt_XmRMnemonic_to_LVAL, 
  Cvt_LVAL_to_XmRMnemonic}; 
/* no resource names found for this type 
   note that the resources XmNmnemonic and XmCmnemonic are of type XmRChar.
   (bug?)
*/
#endif

/********************************************************************************/

static LVAL Cvt_XmROrientation_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmNO_ORIENTATION:
    return (s_XmNO_ORIENTATION);
  case XmVERTICAL:
    return (s_XmVERTICAL);
  case XmHORIZONTAL:
    return (s_XmHORIZONTAL);
  default:
    xlfail("Internal error in Cvt_XmROrientation_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmROrientation(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmNO_ORIENTATION)
    result = XmNO_ORIENTATION;
  else if (lisp_val == s_XmVERTICAL)
    result = XmVERTICAL;
  else if (lisp_val == s_XmHORIZONTAL)
    result = XmHORIZONTAL;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmROrientation must be one of the following symbols [%s %s %s].",
	    (char*) getstring(getpname(s_XmNO_ORIENTATION)), 
	    (char*) getstring(getpname(s_XmVERTICAL)), 
	    (char*) getstring(getpname(s_XmHORIZONTAL)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmROrientation = {
  XmROrientation,	
  SYMBOL,			/* XmROrientation is an enumerated type */
  Cvt_XmROrientation_to_LVAL,	
  Cvt_LVAL_to_XmROrientation};	/* sizeof(unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRPacking_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmNO_PACKING:
    return (s_XmNO_PACKING);
  case XmPACK_TIGHT:
    return (s_XmPACK_TIGHT);
  case XmPACK_COLUMN:
    return (s_XmPACK_COLUMN);
  default:
    xlfail("Internal error in Cvt_XmRPacking_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRPacking(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmNO_PACKING)
    result = XmNO_PACKING;
  else if (lisp_val == s_XmPACK_TIGHT)
    result = XmPACK_TIGHT;
  else if (lisp_val == s_XmPACK_COLUMN)
    result = XmPACK_COLUMN;
  else if (lisp_val == s_XmPACK_NONE)
    result = XmPACK_NONE;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRPacking must be one of the following symbols [%s %s %s %s].",
	    (char*) getstring(getpname(s_XmNO_PACKING)), 
	    (char*) getstring(getpname(s_XmPACK_TIGHT)), 
	    (char*) getstring(getpname(s_XmPACK_COLUMN)), 
	    (char*) getstring(getpname(s_XmPACK_NONE)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRPacking = {
  XmRPacking,
  SYMBOL,			/* XmRPacking is an enumerated type */
  Cvt_XmRPacking_to_LVAL, 
  Cvt_LVAL_to_XmRPacking};	/* sizeof(unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRPixel_to_LVAL(res_val)
     Pixel res_val;
{
  return (cv_pixel(res_val));
}

static caddr_t Cvt_LVAL_to_XmRPixel(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  return ((caddr_t) get_pixel(lisp_val));
}

static Resource_Class resclass_XmRPixel = {
  XmRPixel,
  XLTYPE_Pixel,	
  Cvt_XmRPixel_to_LVAL, 
  Cvt_LVAL_to_XmRPixel};	/* sizeof(Pixel) */



/********************************************************************************
 * Fixup of PIXMAPS in MOTIF source --
 *
 * Motif's 1.0's pixmap strategy is notoriously braindamaged. In an attempt
 * to work around the bad design and bugs in this version, I've had to do some
 * kludgery:
 * 
 * (1) In many cases, a single resource name is used with two different
 * representation types here -- one for gadgets (XmRMan*), and one for widgets
 * (XmRPrim*). When I designed winterp, I expected resource names to be used
 * consistently with respect to types and the resource converters invoked -- anybody
 * with half-a-brain would design an object oriented system using consistent
 * types/representation-kind for resources/methods with the same name. It
 * turns out that using a XmRMan* resource-type with a widget is ok, the colors
 * will just default to the colors from the manager. Using a XmRPrim*
 * resource-type with a gadget will cause a guaranteed core-dump since the
 * XmRString-->XmRPrim* converter will be looking up invalid fields in the
 * gadget's struct.
 * 
 * (2) When creating a widget, one may want to have give a
 * string-name-of-pixmap as a pixmap argument, and have the resource converters
 * create the pixmap for you. Unfortunately, XtConvert() expects that you have
 * the widget already created. To kludge this, I just pass in the toplevelshell
 * widget returned by XtInitialize() in cases where the widget hasn't been
 * created yet. If motif expected an XmRPrim* resource, you end up getting
 * random colors for the pixmap, however, things (seem to) default appropriately if
 * you give the equivalent XmRMan* resource. I'm open to suggestions on how to do
 * this right -- it's a severe limitation in the Xtoolkit's converters.
 * 
 * So in other words, in any case where a resource is of type XmRPrim*, winterp
 * will treat the resource as an XmRMan*. If you insist on getting different
 * colors than the manager's, use primitive XM_GET_PIXMAP to return a <pixmap-obj>
 * with specified fg/bg colors that can be passed as a value for XmR*Pixmap* resources.
 ***************************************************************************************
 * Class.Name					Motif-XmR-Type		Winterp-XmR-Type
 ***************************************************************************************
 * Core.XmNborderPixmap				XmRPixmap		XmRManForegroundPixmap
 * Core.XmNbackgroundPixmap			XmRPixmap		XmRManForegroundPixmap
 * Manager.XmNbackgroundPixmap			XmRPixmap		XmRManForegroundPixmap
 * Primitive.XmNbackgroundPixmap		XmRPixmap		XmRManForegroundPixmap
 * Shell.XmNiconPixmap				XmRPixmap		XmRManForegroundPixmap
 * Shell.XmNiconMask				XmRPixmap		XmRManForegroundPixmap
 *
 * CascadeB.XmNcascadePixmap			XmRPrimForegroundPixmap XmRManForegroundPixmap
 * CascadeBG.XmNcascadePixmap			XmRManForegroundPixmap  XmRManForegroundPixmap
 * Label.XmNlabelPixmap				XmRPrimForegroundPixmap XmRManForegroundPixmap
 * LabelG.XmNlabelPixmap			XmRManForegroundPixmap  XmRManForegroundPixmap
 * Label.XmNlabelInsensitivePixmap		XmRPixmap		XmRManForegroundPixmap -- (MOTIF BUG in Snap #11)
 * LabelG.XmNlabelInsensitivePixmap		XmRPixmap		XmRManForegroundPixmap -- (MOTIF BUG in Snap #11)
 * MessageB.XmNsymbolPixmap			XmRManForegroundPixmap  XmRManForegroundPixmap
 * PushB.XmNarmPixmap				XmRPrimForegroundPixmap XmRManForegroundPixmap
 * PushBG.XmNarmPixmap				XmRPrimForegroundPixmap XmRManForegroundPixmap -- (MOTIF BUG in Snap #11)
 * ToggleB.XmNselectPixmap			XmRPrimForegroundPixmap XmRManForegroundPixmap
 * ToggleBG.XmNselectPixmap			XmRPixmap		XmRManForegroundPixmap -- (MOTIF BUG in Snap #11)
 * ToggleB.XmNselectInsensitivePixmap		XmRPixmap		XmRManForegroundPixmap -- (MOTIF BUG in Snap #11)
 * ToggleBG.XmNselectInsensitivePixmap		XmRPixmap		XmRManForegroundPixmap -- (MOTIF BUG in Snap #11)
 *
 * Primitive.XmNhighlightPixmap			XmRPrimHighlightPixmap	XmRManHighlightPixmap
 * Manager.XmNhighlightPixmap			XmRManHighlightPixmap	XmRManHighlightPixmap
 *
 * Primitive.XmNtopShadowPixmap			XmRPrimTopShadowPixmap  XmRManTopShadowPixmap
 * Manager.XmNtopShadowPixmap			XmRManTopShadowPixmap   XmRManTopShadowPixmap

 * Primitive.XmNbottomShadowPixmap		XmRPrimBottomShadowPixmap XmRManBottomShadowPixmap
 * Manager.XmNbottomShadowPixmap		XmRManBottomShadowPixmap  XmRManBottomShadowPixmap
 ********************************************************************************/

static LVAL Cvt_Pixmap_to_LVAL(res_val)
     Pixmap res_val;
{
  return (res_val ? cv_pixmap(res_val) : NIL);
}

static caddr_t Cvt_LVAL_to_Pixmap(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;		/* XLTYPE_Pixmap */
     LVAL o_widget;
     LVAL lval_resname;
{
  extern void Wpm_Set_Pixmap_Reference(); /* w_pixmap.c */
  Pixmap result;
  
 /* pixmap is referenced inside widget, so don't allow pixmap to be garbage
    collected & XmDestroyPixmap()'d until it is no longer referenced by
    anybody. */
  Wpm_Set_Pixmap_Reference(lisp_val, o_widget, lval_resname);

  result = get_pixmap(lisp_val);
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRPixmap = {
  XmRManForegroundPixmap, /* changed from  XmRPixmap due to BUGs */
  XLTYPE_Pixmap,	
  Cvt_Pixmap_to_LVAL, 
  Cvt_LVAL_to_Pixmap};	/* sizeof(Pixmap) == sizeof(unsigned long) */

static Resource_Class resclass_XmRManForegroundPixmap = {
  XmRManForegroundPixmap, 
  XLTYPE_Pixmap, 
  Cvt_Pixmap_to_LVAL,	
  Cvt_LVAL_to_Pixmap};

static Resource_Class resclass_XmRManHighlightPixmap = {
  XmRManHighlightPixmap, 
  XLTYPE_Pixmap, 
  Cvt_Pixmap_to_LVAL, 
  Cvt_LVAL_to_Pixmap};

static Resource_Class resclass_XmRManTopShadowPixmap = {
  XmRManTopShadowPixmap, 
  XLTYPE_Pixmap, 
  Cvt_Pixmap_to_LVAL, 
  Cvt_LVAL_to_Pixmap};

static Resource_Class resclass_XmRManBottomShadowPixmap = {
  XmRManBottomShadowPixmap, 
  XLTYPE_Pixmap, 
  Cvt_Pixmap_to_LVAL, 
  Cvt_LVAL_to_Pixmap};



/********************************************************************************/

static LVAL Cvt_XmRPointer_to_LVAL(res_val)
     caddr_t res_val;
{
    xlfail("XmRPointer --> Lisp converter not implemented.");
}
static caddr_t Cvt_LVAL_to_XmRPointer(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
    xlerror("Lisp --> XmRPointer converter not implemented.", lisp_val);
}
static Resource_Class resclass_XmRPointer = {
  XmRPointer,
  XLTYPE_caddr_t,	
  Cvt_XmRPointer_to_LVAL, 
  Cvt_LVAL_to_XmRPointer};	/* sizeof(caddr_t) */

/* 
 * NOTE: since we're using the userdata field to hold a backpointer
 * to the widget object, then we shouldn't make it accessible to the user
 *        XmNuserData	-- Xm:Gadget.c Xm:Manager.c Xm:Primitive.c
 *        XmCUserData	-- Xm:Gadget.c Xm:Manager.c Xm:Primitive.c
 */

/* TODO -- make XmNselectionArray a separate resource class. This resource
takes an array of constants which define what actions are done on click
#1, #2, etc. the possible values are XmSELECT_POSITION,
XmSELECT_WHITESPACE, XmSELECT_WORD, XmSELECT_LINE, XmSELECT_ALL */



/********************************************************************************/

static LVAL Cvt_XmRPosition_to_LVAL(res_val)
     Position res_val;
{
  return (cvfixnum((FIXTYPE) res_val));
}

static caddr_t Cvt_LVAL_to_XmRPosition(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  Position result;
  long     value = (long) getfixnum(lisp_val); 

  /* The Motif Xtoolkit intrinsics list Position as being the following:
          typedef int Position;  -- Offset from 0 coordinate --
     I'm thereby assuming that position will always be represented as an int.
     If this changes, then we're hosed */

  if (value < min_int) {
    sprintf(temptext,
	    "In Widget Arglist, resource values of type XmRPosition must be a FIXNUM >= %ld.",
	    min_int);
    xlerror(temptext, lisp_val);
  }
  else if (value > max_int) {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRPosition must be a FIXNUM <= %ld.",
	    max_int);
    xlerror(temptext, lisp_val);
  }
  result = (Position) value;
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRPosition = {
  XmRPosition,	
  FIXNUM,	
  Cvt_XmRPosition_to_LVAL, 
  Cvt_LVAL_to_XmRPosition};	/* sizeof(Position) == sizeof(int) */


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRProc = {
  XmRProc,
  xxx,	
  Cvt_XmRProc_to_LVAL, 
  Cvt_LVAL_to_XmRProc}; 
/* 
The only resources with the representation type XmRProc are
from Xm:FileSB.c
        XmNfileSearchProc
        XmCFileSearchProc

Where the XmRProc is a function pointer to a function
  void DoFileSearch(fs, search_data)
	XmFileSelectionBoxWidget fs;
	XmFileSelectionBoxCallbackStruct *search_data;
  
I don't expect that people will be setting the file search procedure
inside lisp, since it needs to to alot of low level munging with the
list widget, therefore, for now, I'm going to ignore this type, and the
two associated resources. There's plenty of more important things to
get working... -- NPM.
*/
#endif

/********************************************************************************/

static LVAL Cvt_XmRRowColumnType_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmWORK_AREA:
    return (s_XmWORK_AREA);
  case XmMENU_BAR:
    return (s_XmMENU_BAR);
  case XmMENU_POPUP:
    return (s_XmMENU_POPUP);
  case XmMENU_PULLDOWN:
    return (s_XmMENU_PULLDOWN);
  case XmMENU_OPTION:
    return (s_XmMENU_OPTION);
  default:
    xlfail("Internal error in Cvt_XmRRowColumnType_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRRowColumnType(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmWORK_AREA)
    result = XmWORK_AREA;
  else if (lisp_val == s_XmMENU_BAR)
    result = XmMENU_BAR;
  else if (lisp_val == s_XmMENU_POPUP)
    result = XmMENU_POPUP;
  else if (lisp_val == s_XmMENU_PULLDOWN)
    result = XmMENU_PULLDOWN;
  else if (lisp_val == s_XmMENU_OPTION)
    result = XmMENU_OPTION;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRPacking must be one of the following symbols [%s %s %s %s %s].",
	    (char*) getstring(getpname(s_XmWORK_AREA)), 
	    (char*) getstring(getpname(s_XmMENU_BAR)), 
	    (char*) getstring(getpname(s_XmMENU_POPUP)), 
	    (char*) getstring(getpname(s_XmMENU_PULLDOWN)), 
	    (char*) getstring(getpname(s_XmMENU_OPTION)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRRowColumnType = {
  XmRRowColumnType,	
  SYMBOL,			/* XmRRowColumnType is an enumerated type */
  Cvt_XmRRowColumnType_to_LVAL, 
  Cvt_LVAL_to_XmRRowColumnType}; /* sizeof(unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRShort_to_LVAL(res_val)
     short res_val;
{
  return (cvfixnum((FIXTYPE) res_val));
}

static caddr_t Cvt_LVAL_to_XmRShort(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  short result;
  long  value = (long) getfixnum(lisp_val); 

  if (value < min_short) {
    sprintf(temptext,
	    "In Widget Arglist, resource values of type XmRShort must be a FIXNUM >= %ld.",
	    min_short);
    xlerror(temptext, lisp_val);
  }
  else if (value > max_short) {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRShort must be a FIXNUM <= %ld.",
	    max_short);
    xlerror(temptext, lisp_val);
  }
  result = (short) value;
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRShort = {
  XmRShort,
  FIXNUM,	
  Cvt_XmRShort_to_LVAL, 
  Cvt_LVAL_to_XmRShort};	/* sizeof(short) */


/* 
 * this has been moved to type XmRDimension...
 * static Resource_Instance resinst_XmNspacing = { -- Xm:ToggleB.c Xm:ToggleBG.c --
 *   &resclass_XmRShort,
 *   XmNspacing};
 */

/********************************************************************************/
/*
 * For getvalues, I'm assuming that we need to create our own copy of the string
 * in lisp and that the returned string is just a pointer into the widget's 
 * "memory space". I'm assuming that the string returned by getvalues does not
 * need to be deallocated.
 */
static LVAL Cvt_XmRString_to_LVAL(res_val)
     char* res_val;
{
  return (res_val ? cvstring(res_val) : NIL);
}

/* 
 * For setvalues and widget creation, I'm assuming that all string valued arguments
 * get copied in the widget, or are just temporarily referenced. Cases where this
 * doesn't happen should be considered a Motif bug. The reason to be concerned is
 * that Strings no longer being referenced are good candidates for being garbage
 * collected sometime later (the next time cons() or newnode() is called, for example).
 * Passing the string into setvalues/create  doesn't  count as a reference...
*/
static caddr_t Cvt_LVAL_to_XmRString(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  String result = (String) getstring(lisp_val); 
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRString = {
  XmRString,
  STRING,	
  Cvt_XmRString_to_LVAL, 
  Cvt_LVAL_to_XmRString};	/* sizeof(String) == sizeof(char*) */


/*
 * Note potential BUG: this resource name conflicts with XmNvalue which is 
 * used as an int in Xm:Scale.c and Xm:ScrollBar.c. 
 * DO NOT USE THIS RESOURCE WITH the XmText widget, instead, use methods 
 * :get_string and :set_string ==  (XmTextGetString() XmTextSetString()). 
 * These do the right thing w/r/t to copying the internal string. 
 * Doing XtGetValues(textwidget...XmNValue) will return a pointer to the 
 * internal buffer...
 *
 * static Resource_Instance resinst_XmNvalue = {	-- Xm:Text.c --
 *   &resclass_XmRString,
 *   XmNvalue};
 */



/********************************************************************************/

static LVAL Cvt_XmRStringDirection_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmSTRING_DIRECTION_L_TO_R:
    return (s_XmSTRING_DIRECTION_L_TO_R);
  case XmSTRING_DIRECTION_R_TO_L:
    return (s_XmSTRING_DIRECTION_R_TO_L);
  default:
    xlfail("Internal error in Cvt_XmRStringDirection_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRStringDirection(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmSTRING_DIRECTION_L_TO_R)
    result = XmSTRING_DIRECTION_L_TO_R;
  else if (lisp_val == s_XmSTRING_DIRECTION_R_TO_L)
    result = XmSTRING_DIRECTION_R_TO_L;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRStringDirection must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmSTRING_DIRECTION_L_TO_R)), 
	    (char*) getstring(getpname(s_XmSTRING_DIRECTION_R_TO_L)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRStringDirection = {
  XmRStringDirection,	
  SYMBOL,			/* XmRStringDirection is an enumerated type */
  Cvt_XmRStringDirection_to_LVAL, 
  Cvt_LVAL_to_XmRStringDirection}; 



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRStringTable = {
  XmRStringTable,	
  xxx, 
  Cvt_XmRStringTable_to_LVAL,	
  Cvt_LVAL_to_XmRStringTable}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRUnsignedChar = {
  XmRUnsignedChar,	
  xxx, 
  Cvt_XmRUnsignedChar_to_LVAL, 
  Cvt_LVAL_to_XmRUnsignedChar}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
static LVAL Cvt_XmRTranslationTable_to_LVAL(res_val)
     XtTranslations res_val;
{
  return (cv_xttranslations(res_val));
}

static caddr_t Cvt_LVAL_to_XmRTranslationTable(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  return ((caddr_t) get_xttranslations(lisp_val));
}

static Resource_Class resclass_XmRTranslationTable = {
  XmRTranslationTable, 
  XLTYPE_XtTranslations,	
  Cvt_XmRTranslationTable_to_LVAL, 
  Cvt_LVAL_to_XmRTranslationTable}; /* sizeof(XtTranslations) */


/********************************************************************************/

static LVAL Cvt_XmRWhichButton_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case Button1:
    return (s_Button1);
  case Button2:
    return (s_Button2);
  case Button3:
    return (s_Button3);
  case Button4:
    return (s_Button4);
  case Button5:
    return (s_Button5);
  default:
    xlfail("Internal error in Cvt_XmRWhichButton_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRWhichButton(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_Button1)
    result = Button1;
  else if (lisp_val == s_Button2)
    result = Button2;
  else if (lisp_val == s_Button3)
    result = Button3;
  else if (lisp_val == s_Button4)
    result = Button4;
  else if (lisp_val == s_Button5)
    result = Button5;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRStringDirection must be one of the following symbols [%s %s %s %s %s].",
	    (char*) getstring(getpname(s_Button1)), 
	    (char*) getstring(getpname(s_Button2)), 
	    (char*) getstring(getpname(s_Button3)), 
	    (char*) getstring(getpname(s_Button4)), 
	    (char*) getstring(getpname(s_Button5)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRWhichButton = {
  XmRWhichButton,	
  SYMBOL,			/* XmRWhichButton is an enumerated type */
  Cvt_XmRWhichButton_to_LVAL,	
  Cvt_LVAL_to_XmRWhichButton}; 



/********************************************************************************/
static LVAL Cvt_XmRWindow_to_LVAL(res_val)
     Window res_val;
{
  return (res_val ? cv_window(res_val) : NIL);
}

static caddr_t Cvt_LVAL_to_XmRWindow(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  return ((caddr_t) get_window(lisp_val));
}

static Resource_Class resclass_XmRWindow = {
  XmRWindow,
  XLTYPE_Window,
  Cvt_XmRWindow_to_LVAL, 
  Cvt_LVAL_to_XmRWindow};	/* sizeof(Window) == sizeof(XID) == sizeof(unsigned long) */



/********************************************************************************/

static LVAL Cvt_XmRWidget_to_LVAL(res_val)
     Widget res_val;
{
  extern LVAL Wcls_WidgetID_To_WIDGETOBJ(); /* w_classes.c */
  return (Wcls_WidgetID_To_WIDGETOBJ(res_val));
}

static caddr_t Cvt_LVAL_to_XmRWidget(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;		/* XLTYPE_WIDGETOBJ */
     LVAL o_widget;
     LVAL lval_resname;
{
  Widget widget_id;
  if (widget_id = get_widgetobj_widgetID(lisp_val))
    return ((caddr_t) widget_id);
  else
    xlerror("WIDGETOBJ has been :destroy'd or hasn't been initialized by :isnew.", lisp_val);
}

static Resource_Class resclass_XmRWidget = {
  "Widget",			
  XLTYPE_WIDGETOBJ,
  Cvt_XmRWidget_to_LVAL,	/* cvt WidgetID back to widget object */
  Cvt_LVAL_to_XmRWidget};	/* cvt widget object to widget ID (sizeof(Widget))
				   make sure this has <widgetclass> in super */

/* Note that there is no XmRWidget type... infact this [expletive deleted]
   Motif Toolkit uses representation XmRWindow to hold widgetID's, even though
   the intrinsics use XmRWindow to hold windowID's ...
*/
/* These are of type XmRMenuWidget, but I've thrown them in with the
   fake representation type "Widget" since they're all widgetID's:
   	XmNsubMenuId XmNmenuHelpWidget XmNmenuHistory
*/
/* The following are of type XmRWindow, but the source and the documentation
   indicate that the associated value should be a widgetID, not a windowID!:
	   XmNdefaultButton XmNcancelButton XmNtopWidget XmNbottomWidget
	   XmNleftWidget XmNrightWidget XmNhorizontalScrollBar
	   XmNverticalScrollBar XmNcommandWindow
	   XmNmenuBar XmNworkWindow XmNclipWindow
*/


/********************************************************************************/
static LVAL Cvt_XmRXmString_to_LVAL(res_val)
     XmString res_val;
{
  return (res_val ? cv_xmstring(XmStringCopy(res_val)) : NIL);
}

static caddr_t Cvt_LVAL_to_XmRXmString(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  /*
    Note that all XmString resources are copied into the Motif
    widget's space and thereafter managed by that widget. That
    means we don't need to worry about making an additional
    reference to the lisp XmString value that was passed in.
    When the lisp XmString is no longer referenced, it will
    be freed via garbage collection. 
  */
  return ((caddr_t) get_xmstring(lisp_val));
}

static Resource_Class resclass_XmRXmString = {
  XmRXmString,	
  XLTYPE_XmString,
  Cvt_XmRXmString_to_LVAL, 
  Cvt_LVAL_to_XmRXmString}; 


/********************************************************************************/
static LVAL Cvt_XmRXmStringTable_to_LVAL(res_val)
     caddr_t res_val;
{
  /*
    For doing XtGetValues(), we need to look at the size of the created
    XmStringTable divided by the sizeof(XmString), and use this value
    to return a vector of XmStrings... We need to make copies of these
    since it is just a pointer to internal data which is managed internally.
    Unfortunately, since we can't find out the size of the XmStringTable,
    nor are XmStringTable's null terminated, we cannot implement this
    converter.
    */
    xlfail("Cannot do :get_values on resource class XmRXmStringTable.");
}


/*
 *
 * For XmStringTable XtSetValues(), or for widget args during creation, the fn
 * Cvt_LVAL_to_XmRXmStringTable will convert a vector/list of strings/XmStrings
 * into an XmStringTable,  which is an array of XmStrings. Both the array and
 * it's XmStrings are copied into the widget upon :setvalues/:new and are
 * managed by the widget afterwards. Therefore, all the XmStrings created
 * here are eventually freed by calling Wres_Free_C_Arglist_Data(). Note that
 * the XmStringTable itself (global xm_stringtable) should never be freed.
 */
#define XMSTRINGTABLE_SIZE_INCREMENT 50
static XmString *xm_stringtable; /* init'd in Wres_Init() */
static int       xm_stringtable_size;  /* init'd in Wres_Init() */
static caddr_t Cvt_LVAL_to_XmRXmStringTable(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  LVAL elt;
  int size, i;

  if (vectorp(lisp_val)) {
    size = getsize(lisp_val);
    if (size > xm_stringtable_size) { /* make sure it'll fit into allocated xm_stringtable */
      xm_stringtable = (XmString *) XtRealloc(xm_stringtable, (unsigned) (size * sizeof(XmString)));
      xm_stringtable_size = size;
    }
    for (i = 0; i < size; i++) {
      elt = getelement(lisp_val, i);
      if stringp(elt)
	Wres_Free_This_Later((caddr_t) (xm_stringtable[i] = XmStringCreate((char*) getstring(elt), XmSTRING_DEFAULT_CHARSET)),
			     XmStringFree);
      else if xmstring_p(elt)
	xm_stringtable[i] = get_xmstring(elt); /* note -- this XmString will get freed by garbage collection */
      else
	xlerror("Bad XmStringTable element -- expected a String or an XmString.", elt);
    }
  }
  else if (consp(lisp_val)) {
    for (i = 0 ; (consp(lisp_val)) ; lisp_val = cdr(lisp_val), i++) {
      if (i >= xm_stringtable_size) { /* make sure it'll fit into allocated xm_stringtable */
	xm_stringtable = (XmString *) XtRealloc(xm_stringtable, (unsigned) (xm_stringtable_size + XMSTRINGTABLE_SIZE_INCREMENT) * sizeof(XmString));
	xm_stringtable_size += XMSTRINGTABLE_SIZE_INCREMENT;
      }
      elt = car(lisp_val);
      if (stringp(elt))
	Wres_Free_This_Later((caddr_t) (xm_stringtable[i] = XmStringCreate((char*) getstring(elt), XmSTRING_DEFAULT_CHARSET)),
			     XmStringFree);
      else if (xmstring_p(elt))
	xm_stringtable[i] = get_xmstring(elt); /* note -- this XmString will get freed by garbage collection */
      else
	xlerror("Bad XmStringTable element -- expected a String or an XmString.", elt);
    }
    if (lisp_val)		/* if loop terminated due to list pointer not being a CONS cell */
      xlerror("Bad XmStringTable list element -- expected a list of Strings or XmStrings.", lisp_val);
  }
  else
    xlerror("Bad argument type. Expected a list or vector of strings or XmStrings", lisp_val);

  return ((caddr_t) xm_stringtable);
}

static Resource_Class resclass_XmRXmStringTable = {
  XmRXmStringTable,	
  FREE,				/* can either be a vector or a list */
  Cvt_XmRXmStringTable_to_LVAL, 
  Cvt_LVAL_to_XmRXmStringTable}; 



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRSizePolicy = {
  XmRSizePolicy,	
  xxx, 
  Cvt_XmRSizePolicy_to_LVAL, 
  Cvt_LVAL_to_XmRSizePolicy};	
/* no resource names found for this type */
#endif


/********************************************************************************/

static LVAL Cvt_XmRUnitType_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmPIXELS:
    return (s_XmPIXELS);
  case Xm100TH_MILLIMETERS:
    return (s_Xm100TH_MILLIMETERS);
  case Xm1000TH_INCHES:
    return (s_Xm1000TH_INCHES);
  case Xm100TH_POINTS:
    return (s_Xm100TH_POINTS);
  case Xm100TH_FONT_UNITS:
    return (s_Xm100TH_FONT_UNITS);
  default:
    xlfail("Internal error in Cvt_XmRUnitType_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRUnitType(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmPIXELS)
    result = XmPIXELS;
  else if (lisp_val == s_Xm100TH_MILLIMETERS)
    result = Xm100TH_MILLIMETERS;
  else if (lisp_val == s_Xm1000TH_INCHES)
    result = Xm1000TH_INCHES;
  else if (lisp_val == s_Xm100TH_POINTS)
    result = Xm100TH_POINTS;
  else if (lisp_val == s_Xm100TH_FONT_UNITS)
    result = Xm100TH_FONT_UNITS;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRUnitType must be one of the following symbols [%s %s %s %s %s].",
	    (char*) getstring(getpname(s_XmPIXELS)), 
	    (char*) getstring(getpname(s_Xm100TH_MILLIMETERS)), 
	    (char*) getstring(getpname(s_Xm1000TH_INCHES)), 
	    (char*) getstring(getpname(s_Xm100TH_POINTS)), 
	    (char*) getstring(getpname(s_Xm100TH_FONT_UNITS)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRUnitType = {
  XmRUnitType,	
  SYMBOL,			/* XmRUnitType is enumerated */
  Cvt_XmRUnitType_to_LVAL, 
  Cvt_LVAL_to_XmRUnitType};	/* sizeof (unsigned char) */




/********************************************************************************/

static LVAL Cvt_XmRResizePolicy_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmRESIZE_NONE:
    return (s_XmRESIZE_NONE);
  case XmRESIZE_GROW:
    return (s_XmRESIZE_GROW);
  case XmRESIZE_ANY:
    return (s_XmRESIZE_ANY);
  default:
    xlfail("Internal error in Cvt_XmRResizePolicy_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRResizePolicy(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmRESIZE_NONE)
    result = XmRESIZE_NONE;
  else if (lisp_val == s_XmRESIZE_GROW)
    result = XmRESIZE_GROW;
  else if (lisp_val == s_XmRESIZE_ANY)
    result = XmRESIZE_ANY;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRResizePolicy must be one of the following symbols [%s %s %s].",
	    (char*) getstring(getpname(s_XmRESIZE_NONE)), 
	    (char*) getstring(getpname(s_XmRESIZE_GROW)), 
	    (char*) getstring(getpname(s_XmRESIZE_ANY)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRResizePolicy = {
  XmRResizePolicy,	
  SYMBOL,			/* XmRResizePolicy is enumerated type */
  Cvt_XmRResizePolicy_to_LVAL, 
  Cvt_LVAL_to_XmRResizePolicy};	/* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRAttachment_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmATTACH_NONE:
    return (s_XmATTACH_NONE);
  case XmATTACH_FORM:
    return (s_XmATTACH_FORM);
  case XmATTACH_OPPOSITE_FORM:
    return (s_XmATTACH_OPPOSITE_FORM);
  case XmATTACH_WIDGET:
    return (s_XmATTACH_WIDGET);
  case XmATTACH_OPPOSITE_WIDGET:
    return (s_XmATTACH_OPPOSITE_WIDGET);
  case XmATTACH_POSITION:
    return (s_XmATTACH_POSITION);
  case XmATTACH_SELF:
    return (s_XmATTACH_SELF);
  default:
    xlfail("Internal error in Cvt_XmRAttachment_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRAttachment(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmATTACH_NONE)
    result = XmATTACH_NONE;
  else if (lisp_val == s_XmATTACH_FORM)
    result = XmATTACH_FORM;
  else if (lisp_val == s_XmATTACH_OPPOSITE_FORM)
    result = XmATTACH_OPPOSITE_FORM;
  else if (lisp_val == s_XmATTACH_WIDGET)
    result = XmATTACH_WIDGET;
  else if (lisp_val == s_XmATTACH_OPPOSITE_WIDGET)
    result = XmATTACH_OPPOSITE_WIDGET;
  else if (lisp_val == s_XmATTACH_POSITION)
    result = XmATTACH_POSITION;
  else if (lisp_val == s_XmATTACH_SELF)
    result = XmATTACH_SELF;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRAttachment must be one of the following symbols [%s %s %s %s %s %s %s].",
	    (char*) getstring(getpname(s_XmATTACH_NONE)), 
	    (char*) getstring(getpname(s_XmATTACH_FORM)), 
	    (char*) getstring(getpname(s_XmATTACH_OPPOSITE_FORM)), 
	    (char*) getstring(getpname(s_XmATTACH_WIDGET)), 
	    (char*) getstring(getpname(s_XmATTACH_OPPOSITE_WIDGET)), 
	    (char*) getstring(getpname(s_XmATTACH_POSITION)), 
	    (char*) getstring(getpname(s_XmATTACH_SELF)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRAttachment = {
  XmRAttachment,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRAttachment_to_LVAL, 
  Cvt_LVAL_to_XmRAttachment};	/* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRShadowType_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmSHADOW_ETCHED_IN:
    return (s_XmSHADOW_ETCHED_IN);
  case XmSHADOW_ETCHED_OUT:
    return (s_XmSHADOW_ETCHED_OUT);
  case XmSHADOW_IN:
    return (s_XmSHADOW_IN);
  case XmSHADOW_OUT:
    return (s_XmSHADOW_OUT);
  default:
    xlfail("Internal error in Cvt_XmRShadowType_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRShadowType(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmSHADOW_ETCHED_IN)
    result = XmSHADOW_ETCHED_IN;
  else if (lisp_val == s_XmSHADOW_ETCHED_OUT)
    result = XmSHADOW_ETCHED_OUT;
  else if (lisp_val == s_XmSHADOW_IN)
    result = XmSHADOW_IN;
  else if (lisp_val == s_XmSHADOW_OUT)
    result = XmSHADOW_OUT;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRShadowType must be one of the following symbols [%s %s %s %s].",
	    (char*) getstring(getpname(s_XmSHADOW_ETCHED_IN)), 
	    (char*) getstring(getpname(s_XmSHADOW_ETCHED_OUT)), 
	    (char*) getstring(getpname(s_XmSHADOW_IN)), 
	    (char*) getstring(getpname(s_XmSHADOW_OUT)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRShadowType = {
  XmRShadowType,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRShadowType_to_LVAL, 
  Cvt_LVAL_to_XmRShadowType};	/* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRArrowDirection_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmARROW_UP:
    return (s_XmARROW_UP);
  case XmARROW_DOWN:
    return (s_XmARROW_DOWN);
  case XmARROW_LEFT:
    return (s_XmARROW_LEFT);
  case XmARROW_RIGHT:
    return (s_XmARROW_RIGHT);
  default:
    xlfail("Internal error in Cvt_XmRArrowDirection_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRArrowDirection(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmARROW_UP)
    result = XmARROW_UP;
  else if (lisp_val == s_XmARROW_DOWN)
    result = XmARROW_DOWN;
  else if (lisp_val == s_XmARROW_LEFT)
    result = XmARROW_LEFT;
  else if (lisp_val == s_XmARROW_RIGHT)
    result = XmARROW_RIGHT;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRArrowDirection must be one of the following symbols [%s %s %s %s].",
	    (char*) getstring(getpname(s_XmARROW_UP)), 
	    (char*) getstring(getpname(s_XmARROW_DOWN)), 
	    (char*) getstring(getpname(s_XmARROW_LEFT)), 
	    (char*) getstring(getpname(s_XmARROW_RIGHT)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRArrowDirection = {
  XmRArrowDirection,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRArrowDirection_to_LVAL, 
  Cvt_LVAL_to_XmRArrowDirection}; /* sizeof (unsigned char) */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRMargin = {
  XmRMargin,
  xxx,	
  Cvt_XmRMargin_to_LVAL, 
  Cvt_LVAL_to_XmRMargin}; 
/* no resource names found for this type */
#endif


/********************************************************************************/

static LVAL Cvt_XmRSeparatorType_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmNO_LINE:
    return (s_XmNO_LINE);
  case XmSINGLE_LINE:
    return (s_XmSINGLE_LINE);
  case XmDOUBLE_LINE:
    return (s_XmDOUBLE_LINE);
  case XmSINGLE_DASHED_LINE:
    return (s_XmSINGLE_DASHED_LINE);
  case XmDOUBLE_DASHED_LINE:
    return (s_XmDOUBLE_DASHED_LINE);
  case XmSHADOW_ETCHED_IN:
    return (s_XmSHADOW_ETCHED_IN);
  case XmSHADOW_ETCHED_OUT:
    return (s_XmSHADOW_ETCHED_OUT);
  default:
    xlfail("Internal error in Cvt_XmRSeparatorType_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRSeparatorType(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmNO_LINE)
    result = XmNO_LINE;
  else if (lisp_val == s_XmSINGLE_LINE)
    result = XmSINGLE_LINE;
  else if (lisp_val == s_XmDOUBLE_LINE)
    result = XmDOUBLE_LINE;
  else if (lisp_val == s_XmSINGLE_DASHED_LINE)
    result = XmSINGLE_DASHED_LINE;
  else if (lisp_val == s_XmDOUBLE_DASHED_LINE)
    result = XmDOUBLE_DASHED_LINE;
  else if (lisp_val == s_XmSHADOW_ETCHED_IN)
    result = XmSHADOW_ETCHED_IN;
  else if (lisp_val == s_XmSHADOW_ETCHED_OUT)
    result = XmSHADOW_ETCHED_OUT;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRSeparatorType must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmNO_LINE)), 
	    (char*) getstring(getpname(s_XmSINGLE_LINE)), 
	    (char*) getstring(getpname(s_XmDOUBLE_LINE)), 
	    (char*) getstring(getpname(s_XmSINGLE_DASHED_LINE)), 
	    (char*) getstring(getpname(s_XmDOUBLE_DASHED_LINE)), 
	    (char*) getstring(getpname(s_XmSHADOW_ETCHED_IN)), 
	    (char*) getstring(getpname(s_XmSHADOW_ETCHED_OUT)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRSeparatorType = {
  XmRSeparatorType,	
  SYMBOL, 
  Cvt_XmRSeparatorType_to_LVAL, 
  Cvt_LVAL_to_XmRSeparatorType}; /* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRProcessingDirection_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmMAX_ON_TOP:
    return (s_XmMAX_ON_TOP);
  case XmMAX_ON_BOTTOM:
    return (s_XmMAX_ON_BOTTOM);
#ifdef 0			/* can't put these in a case since XmMAX_ON_LEFT==XmMAX_ON_TOP, XmMAX_ON_BOTTOM==XmMAX_ON_RIGHT*/
  case XmMAX_ON_LEFT:
    return (s_XmMAX_ON_LEFT);
  case XmMAX_ON_RIGHT:
    return (s_XmMAX_ON_RIGHT);
#endif
  default:
    xlfail("Internal error in Cvt_XmRProcessingDirection_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRProcessingDirection(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmMAX_ON_TOP)
    result = XmMAX_ON_TOP;
  else if (lisp_val == s_XmMAX_ON_BOTTOM)
    result = XmMAX_ON_BOTTOM;
  else if (lisp_val == s_XmMAX_ON_LEFT)
    result = XmMAX_ON_LEFT;
  else if (lisp_val == s_XmMAX_ON_RIGHT)
    result = XmMAX_ON_RIGHT;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRProcessingDirection must be one of the following symbols [%s %s %s %s].",
	    (char*) getstring(getpname(s_XmMAX_ON_TOP)), 
	    (char*) getstring(getpname(s_XmMAX_ON_BOTTOM)), 
	    (char*) getstring(getpname(s_XmMAX_ON_LEFT)), 
	    (char*) getstring(getpname(s_XmMAX_ON_RIGHT)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRProcessingDirection = {
  XmRProcessingDirection, 
  SYMBOL,			/* enumerated type */
  Cvt_XmRProcessingDirection_to_LVAL,	
  Cvt_LVAL_to_XmRProcessingDirection}; /* sizeof (unsigned char) */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRListSpacing = {
  XmRListSpacing,	
  xxx, 
  Cvt_XmRListSpacing_to_LVAL,	
  Cvt_LVAL_to_XmRListSpacing}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRListMarginWidth = {
  XmRListMarginWidth,	
  xxx, 
  Cvt_XmRListMarginWidth_to_LVAL, 
  Cvt_LVAL_to_XmRListMarginWidth}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRListMarginHeight = {
  XmRListMarginHeight, 
  xxx,	
  Cvt_XmRListMarginHeight_to_LVAL, 
  Cvt_LVAL_to_XmRListMarginHeight}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRItems = {
  XmRItems,
  xxx,	
  Cvt_XmRItems_to_LVAL, 
  Cvt_LVAL_to_XmRItems}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRItemCount = {
  XmRItemCount,	
  xxx, 
  Cvt_XmRItemCount_to_LVAL, 
  Cvt_LVAL_to_XmRItemCount}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRSelectedItems = {
  XmRSelectedItems,	
  xxx, 
  Cvt_XmRSelectedItems_to_LVAL, 
  Cvt_LVAL_to_XmRSelectedItems}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRSelectedItemCount = {
  XmRSelectedItemCount, 
  xxx, 
  Cvt_XmRSelectedItemCount_to_LVAL, 
  Cvt_LVAL_to_XmRSelectedItemCount}; 
/* no resource names found for this type */
#endif


/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRVisibleItemCount = {
  XmRVisibleItemCount, 
  xxx,	
  Cvt_XmRVisibleItemCount_to_LVAL, 
  Cvt_LVAL_to_XmRVisibleItemCount}; 
/* no resource names found for this type */
#endif


/********************************************************************************/

static LVAL Cvt_XmRSelectionPolicy_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmSINGLE_SELECT:
    return (s_XmSINGLE_SELECT);
  case XmMULTIPLE_SELECT:
    return (s_XmMULTIPLE_SELECT);
  case XmEXTENDED_SELECT:
    return (s_XmEXTENDED_SELECT);
  case XmBROWSE_SELECT:
    return (s_XmBROWSE_SELECT);
  default:
    xlfail("Internal error in Cvt_XmRSelectionPolicy_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRSelectionPolicy(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmSINGLE_SELECT)
    result = XmSINGLE_SELECT;
  else if (lisp_val == s_XmMULTIPLE_SELECT)
    result = XmMULTIPLE_SELECT;
  else if (lisp_val == s_XmEXTENDED_SELECT)
    result = XmEXTENDED_SELECT;
  else if (lisp_val == s_XmBROWSE_SELECT)
    result = XmBROWSE_SELECT;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRSelectionPolicy must be one of the following symbols [%s %s %s %s].",
	    (char*) getstring(getpname(s_XmSINGLE_SELECT)), 
	    (char*) getstring(getpname(s_XmMULTIPLE_SELECT)), 
	    (char*) getstring(getpname(s_XmEXTENDED_SELECT)), 
	    (char*) getstring(getpname(s_XmBROWSE_SELECT)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRSelectionPolicy = {
  XmRSelectionPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRSelectionPolicy_to_LVAL, 
  Cvt_LVAL_to_XmRSelectionPolicy}; /* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRListSizePolicy_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmVARIABLE:
    return (s_XmVARIABLE);
  case XmCONSTANT:
    return (s_XmCONSTANT);
  case XmRESIZE_IF_POSSIBLE:
    return (s_XmRESIZE_IF_POSSIBLE);
  default:
    xlfail("Internal error in Cvt_XmRListSizePolicy_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRListSizePolicy(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmVARIABLE)
    result = XmVARIABLE;
  else if (lisp_val == s_XmCONSTANT)
    result = XmCONSTANT;
  else if (lisp_val == s_XmRESIZE_IF_POSSIBLE)
    result = XmRESIZE_IF_POSSIBLE;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRListSizePolicy must be one of the following symbols [%s %s %s].",
	    (char*) getstring(getpname(s_XmVARIABLE)), 
	    (char*) getstring(getpname(s_XmCONSTANT)), 
	    (char*) getstring(getpname(s_XmRESIZE_IF_POSSIBLE)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRListSizePolicy = {
  XmRListSizePolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRListSizePolicy_to_LVAL, 
  Cvt_LVAL_to_XmRListSizePolicy}; /* sizeof (unsigned char) */



/********************************************************************************/
#ifdef 0
static Resource_Class resclass_XmRDoubleClickInterval = {
  XmRDoubleClickInterval, 
  xxx, 
  Cvt_XmRDoubleClickInterval_to_LVAL,	
  Cvt_LVAL_to_XmRDoubleClickInterval}; 
/* no resource names found for this type */
#endif


/********************************************************************************/

static LVAL Cvt_XmRScrollingPolicy_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmAUTOMATIC:
    return (s_XmAUTOMATIC);
  case XmAPPLICATION_DEFINED:
    return (s_XmAPPLICATION_DEFINED);
  default:
    xlfail("Internal error in Cvt_XmRScrollingPolicy_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRScrollingPolicy(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmAUTOMATIC)
    result = XmAUTOMATIC;
  else if (lisp_val == s_XmAPPLICATION_DEFINED)
    result = XmAPPLICATION_DEFINED;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRScrollingPolicy must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmAUTOMATIC)), 
	    (char*) getstring(getpname(s_XmAPPLICATION_DEFINED)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRScrollingPolicy = {
  XmRScrollingPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRScrollingPolicy_to_LVAL, 
  Cvt_LVAL_to_XmRScrollingPolicy}; /* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRVisualPolicy_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmVARIABLE:
    return (s_XmVARIABLE);
  case XmCONSTANT:
    return (s_XmCONSTANT);
  default:
    xlfail("Internal error in Cvt_XmRVisualPolicy_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRVisualPolicy(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmVARIABLE)
    result = XmVARIABLE;
  else if (lisp_val == s_XmCONSTANT)
    result = XmCONSTANT;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRVisualPolicy must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmVARIABLE)), 
	    (char*) getstring(getpname(s_XmCONSTANT)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRVisualPolicy = {
  XmRVisualPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRVisualPolicy_to_LVAL, 
  Cvt_LVAL_to_XmRVisualPolicy};	/* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRScrollBarDisplayPolicy_to_LVAL(res_val)
     unsigned char res_val;
{
    switch (res_val) {
  case XmSTATIC:
    return (s_XmSTATIC);
  case XmAS_NEEDED:
    return (s_XmAS_NEEDED);
  default:
    xlfail("Internal error in Cvt_XmRScrollBarDisplayPolicy_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRScrollBarDisplayPolicy(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmSTATIC)
    result = XmSTATIC;
  else if (lisp_val == s_XmAS_NEEDED)
    result = XmAS_NEEDED;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRScrollBarDisplayPolicy must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmSTATIC)), 
	    (char*) getstring(getpname(s_XmAS_NEEDED)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRScrollBarDisplayPolicy = {
  XmRScrollBarDisplayPolicy, 
  SYMBOL,			/* enumerated type */
  Cvt_XmRScrollBarDisplayPolicy_to_LVAL, 
  Cvt_LVAL_to_XmRScrollBarDisplayPolicy}; /* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRScrollBarPlacement_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmTOP_LEFT:
    return (s_XmTOP_LEFT);
  case XmBOTTOM_LEFT:
    return (s_XmBOTTOM_LEFT);
  case XmTOP_RIGHT:
    return (s_XmTOP_RIGHT);
  case XmBOTTOM_RIGHT:
    return (s_XmBOTTOM_RIGHT);
  default:
    xlfail("Internal error in Cvt_XmRScrollBarPlacement_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRScrollBarPlacement(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmTOP_LEFT)
    result = XmTOP_LEFT;
  else if (lisp_val == s_XmBOTTOM_LEFT)
    result = XmBOTTOM_LEFT;
  else if (lisp_val == s_XmTOP_RIGHT)
    result = XmTOP_RIGHT;
  else if (lisp_val == s_XmBOTTOM_RIGHT)
    result = XmBOTTOM_RIGHT;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRScrollBarPlacement must be one of the following symbols [%s %s %s %s].",
	    (char*) getstring(getpname(s_XmTOP_LEFT)), 
	    (char*) getstring(getpname(s_XmBOTTOM_LEFT)), 
	    (char*) getstring(getpname(s_XmTOP_RIGHT)), 
	    (char*) getstring(getpname(s_XmBOTTOM_RIGHT)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRScrollBarPlacement = {
  XmRScrollBarPlacement, 
  SYMBOL,			/* enumerated type */
  Cvt_XmRScrollBarPlacement_to_LVAL, 
  Cvt_LVAL_to_XmRScrollBarPlacement}; /* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRDialogStyle_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmDIALOG_WORK_AREA:
    return (s_XmDIALOG_WORK_AREA);
  case XmDIALOG_MODELESS:
    return (s_XmDIALOG_MODELESS);
  case XmDIALOG_APPLICATION_MODAL:
    return (s_XmDIALOG_APPLICATION_MODAL);
  case XmDIALOG_SYSTEM_MODAL:
    return (s_XmDIALOG_SYSTEM_MODAL);
  default:
    xlfail("Internal error in Cvt_XmRDialogStyle_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRDialogStyle(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmDIALOG_WORK_AREA)
    result = XmDIALOG_WORK_AREA;
  else if (lisp_val == s_XmDIALOG_MODELESS)
    result = XmDIALOG_MODELESS;
  else if (lisp_val == s_XmDIALOG_APPLICATION_MODAL)
    result = XmDIALOG_APPLICATION_MODAL;
  else if (lisp_val == s_XmDIALOG_SYSTEM_MODAL)
    result = XmDIALOG_SYSTEM_MODAL;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRDialogStyle must be one of the following symbols [%s %s %s %s].",
	    (char*) getstring(getpname(s_XmDIALOG_WORK_AREA)), 
	    (char*) getstring(getpname(s_XmDIALOG_MODELESS)), 
	    (char*) getstring(getpname(s_XmDIALOG_APPLICATION_MODAL)), 
	    (char*) getstring(getpname(s_XmDIALOG_SYSTEM_MODAL)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRDialogStyle = {
  XmRDialogStyle,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRDialogStyle_to_LVAL,	
  Cvt_LVAL_to_XmRDialogStyle};	/* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRDefaultButtonType_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmDIALOG_OK_BUTTON:
    return (s_XmDIALOG_OK_BUTTON);
  case XmDIALOG_CANCEL_BUTTON:
    return (s_XmDIALOG_CANCEL_BUTTON);
  case XmDIALOG_HELP_BUTTON:
    return (s_XmDIALOG_HELP_BUTTON);
  default:
    xlfail("Internal error in Cvt_XmRDefaultButtonType_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRDefaultButtonType(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmDIALOG_OK_BUTTON)
    result = XmDIALOG_OK_BUTTON;
  else if (lisp_val == s_XmDIALOG_CANCEL_BUTTON)
    result = XmDIALOG_CANCEL_BUTTON;
  else if (lisp_val == s_XmDIALOG_HELP_BUTTON)
    result = XmDIALOG_HELP_BUTTON;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRDefaultButtonType must be one of the following symbols [%s %s %s].",
	    (char*) getstring(getpname(s_XmDIALOG_OK_BUTTON)), 
	    (char*) getstring(getpname(s_XmDIALOG_CANCEL_BUTTON)), 
	    (char*) getstring(getpname(s_XmDIALOG_HELP_BUTTON)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRDefaultButtonType = {
  XmRDefaultButtonType, 
  SYMBOL,			/* enumerated type */
  Cvt_XmRDefaultButtonType_to_LVAL, 
  Cvt_LVAL_to_XmRDefaultButtonType}; /* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRDialogType_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmDIALOG_ERROR:
    return (s_XmDIALOG_ERROR);
  case XmDIALOG_INFORMATION:
    return (s_XmDIALOG_INFORMATION);
  case XmDIALOG_MESSAGE:
    return (s_XmDIALOG_MESSAGE);
  case XmDIALOG_QUESTION:
    return (s_XmDIALOG_QUESTION);
  case XmDIALOG_WARNING:
    return (s_XmDIALOG_WARNING);
  case XmDIALOG_WORKING:
    return (s_XmDIALOG_WORKING);
  case XmDIALOG_PROMPT:
    return (s_XmDIALOG_PROMPT);
  case XmDIALOG_SELECTION:
    return (s_XmDIALOG_SELECTION);
  case XmDIALOG_COMMAND:
    return (s_XmDIALOG_COMMAND);
  default:
    xlfail("Internal error in Cvt_XmRDialogType_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRDialogType(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmDIALOG_ERROR)
    result = XmDIALOG_ERROR;
  else if (lisp_val == s_XmDIALOG_INFORMATION)
    result = XmDIALOG_INFORMATION;
  else if (lisp_val == s_XmDIALOG_MESSAGE)
    result = XmDIALOG_MESSAGE;
  else if (lisp_val == s_XmDIALOG_QUESTION)
    result = XmDIALOG_QUESTION;
  else if (lisp_val == s_XmDIALOG_WARNING)
    result = XmDIALOG_WARNING;
  else if (lisp_val == s_XmDIALOG_WORKING)
    result = XmDIALOG_WORKING;
  else if (lisp_val == s_XmDIALOG_PROMPT)
    result = XmDIALOG_PROMPT;
  else if (lisp_val == s_XmDIALOG_SELECTION)
    result = XmDIALOG_SELECTION;
  else if (lisp_val == s_XmDIALOG_COMMAND)
    result = XmDIALOG_COMMAND;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRDialogType must be one of the following symbols [%s %s %s %s %s %s %s %s %s].",
	    (char*) getstring(getpname(s_XmDIALOG_ERROR)), 
	    (char*) getstring(getpname(s_XmDIALOG_INFORMATION)), 
	    (char*) getstring(getpname(s_XmDIALOG_MESSAGE)), 
	    (char*) getstring(getpname(s_XmDIALOG_QUESTION)), 
	    (char*) getstring(getpname(s_XmDIALOG_WARNING)), 
	    (char*) getstring(getpname(s_XmDIALOG_WORKING)), 
	    (char*) getstring(getpname(s_XmDIALOG_PROMPT)), 
	    (char*) getstring(getpname(s_XmDIALOG_SELECTION)), 
	    (char*) getstring(getpname(s_XmDIALOG_COMMAND)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRDialogType = {
  XmRDialogType,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRDialogType_to_LVAL, 
  Cvt_LVAL_to_XmRDialogType};	/* sizeof (unsigned char) */



/********************************************************************************/

/*
 * Note: in Xm:RowColumn.c, XmNentryClass/XmCEntryClass resource is coded
 * as being of representation type XmRInt, when it's value is supposed to hold
 * a widget class pointer. So we create a fake, new representation type
 * XmRWidgetClass for this resource.
 */

static LVAL Cvt_XmRWidgetClass_to_LVAL(res_val)
     WidgetClass res_val;
{
  extern LVAL Wcls_WidgetClassID_To_WIDGETCLASSOBJ(); /* w_classes.c */
  return (Wcls_WidgetClassID_To_WIDGETCLASSOBJ(res_val));
}

static caddr_t Cvt_LVAL_to_XmRWidgetClass(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;		/* OBJECT */
     LVAL o_widget;
     LVAL lval_resname;
{
  extern WidgetClass Wcls_WIDGETCLASSOBJ_To_WidgetClassID(); /* w_classes.c */
  WidgetClass result;

  if (xlclass_p(lisp_val)
      && !(result = Wcls_WIDGETCLASSOBJ_To_WidgetClassID(lisp_val)))
    xlerror("Expected a 'Class' object that is a subclass of 'WIDGET_CLASS'.", lisp_val);

  return ((caddr_t) result);
}

static Resource_Class resclass_XmRWidgetClass = {
  "WidgetClass",	
  OBJECT,			/* must be a widget class object */
  Cvt_XmRWidgetClass_to_LVAL, 
  Cvt_LVAL_to_XmRWidgetClass};	/* sizeof(WidgetClass) == sizeof(* struct _WidgetClassRec) */


/********************************************************************************/

static LVAL Cvt_XmRDeleteResponse_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmDESTROY:
    return (s_XmDESTROY);
  case XmUNMAP:
    return (s_XmUNMAP);
  case XmDO_NOTHING:
    return (s_XmDO_NOTHING);
  default:
    xlfail("Internal error in Cvt_XmRDeleteResponse_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRDeleteResponse(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmDESTROY)
    result = XmDESTROY;
  else if (lisp_val == s_XmUNMAP)
    result = XmUNMAP;
  else if (lisp_val == s_XmDO_NOTHING)
    result = XmDO_NOTHING;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRDeleteResponse must be one of the following symbols [%s %s %s].",
	    (char*) getstring(getpname(s_XmDESTROY)), 
	    (char*) getstring(getpname(s_XmUNMAP)), 
	    (char*) getstring(getpname(s_XmDO_NOTHING)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRDeleteResponse = {
  XmRDeleteResponse,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRDeleteResponse_to_LVAL, 
  Cvt_LVAL_to_XmRDeleteResponse};	/* sizeof (unsigned char) */



/********************************************************************************/

static LVAL Cvt_XmRKeyboardFocusPolicy_to_LVAL(res_val)
     unsigned char res_val;
{
  switch (res_val) {
  case XmEXPLICIT:
    return (s_XmEXPLICIT);
  case XmPOINTER:
    return (s_XmPOINTER);
  default:
    xlfail("Internal error in Cvt_XmRKeyboardFocusPolicy_to_LVAL().");
  }
}

static caddr_t Cvt_LVAL_to_XmRKeyboardFocusPolicy(lisp_val, o_widget, lval_resname)
     LVAL lisp_val;
     LVAL o_widget;
     LVAL lval_resname;
{
  unsigned char result;
  if (lisp_val == s_XmEXPLICIT)
    result = XmEXPLICIT;
  else if (lisp_val == s_XmPOINTER)
    result = XmPOINTER;
  else {
    sprintf(temptext,
	    "In Widget Arglist, resource value of type XmRKeyboardFocusPolicy must be one of the following symbols [%s %s].",
	    (char*) getstring(getpname(s_XmEXPLICIT)), 
	    (char*) getstring(getpname(s_XmPOINTER)));
    xlerror(temptext, lisp_val);
  }
  return ((caddr_t) result);
}

static Resource_Class resclass_XmRKeyboardFocusPolicy = {
  XmRKeyboardFocusPolicy,	
  SYMBOL,			/* enumerated type */
  Cvt_XmRKeyboardFocusPolicy_to_LVAL, 
  Cvt_LVAL_to_XmRKeyboardFocusPolicy};	/* sizeof (unsigned char) */


/**************************************************************************/
/************      T A B L E     OF    R E S O U R C E S     **************/
/************                                                **************/
/**************************************************************************/

typedef struct _Resource_Instance {
  char*           printname;	/* lisp printname for the resource */
  Resource_Class* class;	/* pointer to Resource_Class structure -- the value of 'symbol' in lisp */
  char*           name;		/* holds the XmN* name */
  LVAL            symbol;	/* holds the lisp symbol assoc'd w/ this resource -- set in Wres_Init() */
} Resource_Instance;

static Resource_Instance resource_table[] = {
  {":XMN_TEXT_ACCELERATORS", &resclass_XmRAcceleratorTable, XmNtextAccelerators}, /* Xm:SelectionB.c */
  {":XMN_ACCELERATORS", &resclass_XmRAcceleratorTable, XmNaccelerators}, /* Xt:WidnowObj.c ==> Core */
  {":XMN_ALIGNMENT", &resclass_XmRAlignment, XmNalignment}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_MESSAGE_ALIGNMENT", &resclass_XmRAlignment, XmNmessageAlignment}, /* Xm:MessageB.c */
  {":XMN_ENTRY_ALIGNMENT", &resclass_XmRAlignment, XmNentryAlignment}, /* Xm:RowColumn.c*/
  {":XMN_ANCESTOR_SENSITIVE", &resclass_XmRBoolean, XmNancestorSensitive}, /* Xt:RectObj.c Xt:Shell.c*/
  {":XMN_INPUT", &resclass_XmRBoolean, XmNinput}, /* Xt:Vendor.c Xt:Shell.c -- actually of type XmRBool, but placed here anyways. BUG? */
  {":XMN_SENSITIVE", &resclass_XmRBoolean, XmNsensitive}, /* Xt:RectObj.c */
  {":XMN_ALLOW_SHELL_RESIZE", &resclass_XmRBoolean, XmNallowShellResize}, /* Xt:Shell.c Xm:MenuShell.c */
  {":XMN_SAVE_UNDER", &resclass_XmRBoolean, XmNsaveUnder}, /* Xt:Shell.c Xm:MenuShell.c */
  {":XMN_OVERRIDE_REDIRECT", &resclass_XmRBoolean, XmNoverrideRedirect}, /* Xt:Shell.c */
  {":XMN_WAIT_FOR_WM", &resclass_XmRBoolean, XmNwaitForWm}, /* Xt:Shell.c */
  {":XMN_TRANSIENT", &resclass_XmRBoolean, XmNtransient}, /* Xt:Shell.c */
  {":XMN_ICONIC", &resclass_XmRBoolean, XmNiconic}, /* Xt:Shell.c */
  {":XMN_MAPPED_WHEN_MANAGED", &resclass_XmRBoolean, XmNmappedWhenManaged}, /* Xt:WindowObj.c */
  {":XMN_ALLOW_OVERLAP", &resclass_XmRBoolean, XmNallowOverlap}, /* Xm:BulletinB.c */
  {":XMN_AUTO_UNMANAGE", &resclass_XmRBoolean, XmNautoUnmanage}, /* Xm:BulletinB.c & Xm:Command.c & Xm:FileSB.c */
  {":XMN_DEFAULT_POSITION", &resclass_XmRBoolean, XmNdefaultPosition}, /* Xm:BulletinB.c & Xm:Command.c */
  {":XMN_NO_RESIZE", &resclass_XmRBoolean, XmNnoResize}, /* Xm:BulletinB.c */
  {":XMN_PUSH_BUTTON_ENABLED", &resclass_XmRBoolean, XmNpushButtonEnabled}, /* Xm:DrawnB.c */
  {":XMN_LIST_UPDATED", &resclass_XmRBoolean, XmNlistUpdated}, /* Xm:FileSB.c */
  {":XMN_RUBBER_POSITIONING", &resclass_XmRBoolean, XmNrubberPositioning}, /* Xm:Form.c */
  {":XMN_RESIZABLE", &resclass_XmRBoolean, XmNresizable}, /* Xm:Form.c */
  {":XMN_TRAVERSAL_ON", &resclass_XmRBoolean, XmNtraversalOn}, /* Xm:Gadget.c Xm:Primitive.c Xm:Scale.c */
  {":XMN_HIGHLIGHT_ON_ENTER", &resclass_XmRBoolean, XmNhighlightOnEnter}, /* Xm:Gadget.c Xm:Primitive.c Xm:Scale.c */
  {":XMN_RECOMPUTE_SIZE", &resclass_XmRBoolean, XmNrecomputeSize}, /* Xm:Label.c & Xm:LabelG.c */
  {":XMN_AUTOMATIC_SELECTION", &resclass_XmRBoolean, XmNautomaticSelection}, /* Xm:List.c */
  {":XMN_SHOW_SEPARATOR", &resclass_XmRBoolean, XmNshowSeparator}, /* Xm:MainW.c */
  {":XMN_MINIMIZE_BUTTONS", &resclass_XmRBoolean, XmNminimizeButtons}, /* Xm:MessageB.c Xm:SelectionB.c */
  {":XMN_REFIGURE_MODE", &resclass_XmRBoolean, XmNrefigureMode}, /* Xm:PanedW.c */
  {":XMN_SEPARATOR_ON", &resclass_XmRBoolean, XmNseparatorOn}, /* Xm:PanedW.c */
  {":XMN_ALLOW_RESIZE", &resclass_XmRBoolean, XmNallowResize}, /* Xm:PanedW.c */
  {":XMN_SKIP_ADJUST", &resclass_XmRBoolean, XmNskipAdjust}, /* Xm:PanedW.c */
  {":XMN_FILL_ON_ARM", &resclass_XmRBoolean, XmNfillOnArm}, /* Xm:PushB.c & Xm:PushBG.c */
  {":XMN_RESIZE_WIDTH", &resclass_XmRBoolean, XmNresizeWidth}, /* Xm:RowColumn.c Xm:TextOut.c */
  {":XMN_RESIZE_HEIGHT", &resclass_XmRBoolean, XmNresizeHeight}, /* Xm:RowColumn.c Xm:TextOut.c */
  {":XMN_ADJUST_LAST", &resclass_XmRBoolean, XmNadjustLast}, /* Xm:RowColumn.c */
  {":XMN_IS_ALIGNED", &resclass_XmRBoolean, XmNisAligned}, /* Xm:RowColumn.c */
  {":XMN_ADJUST_MARGIN", &resclass_XmRBoolean, XmNadjustMargin}, /* Xm:RowColumn.c */
  {":XMN_RADIO_BEHAVIOR", &resclass_XmRBoolean, XmNradioBehavior}, /* Xm:RowColumn.c */
  {":XMN_RADIO_ALWAYS_ONE", &resclass_XmRBoolean, XmNradioAlwaysOne}, /* Xm:RowColumn.c */
  {":XMN_IS_HOMOGENEOUS", &resclass_XmRBoolean, XmNisHomogeneous}, /* Xm:RowColumn.c */
  {":XMN_POPUP_ENABLED", &resclass_XmRBoolean, XmNpopupEnabled}, /* Xm:RowColumn.c */
  {":XMN_SHOW_VALUE", &resclass_XmRBoolean, XmNshowValue}, /* Xm:Scale.c */
  {":XMN_SHOW_ARROWS", &resclass_XmRBoolean, XmNshowArrows}, /* Xm:ScrollBar.c */
  {":XMN_MUST_MATCH", &resclass_XmRBoolean, XmNmustMatch}, /* Xm:SelectionB.c */
  {":XMN_AUTO_SHOW_CURSOR_POSITION", &resclass_XmRBoolean, XmNautoShowCursorPosition}, /* Xm:Text.c */
  {":XMN_EDITABLE", &resclass_XmRBoolean, XmNeditable},	/* Xm:Text.c */
  {":XMN_PENDING_DELETE", &resclass_XmRBoolean, XmNpendingDelete}, /* Xm:TextIn.c */
  {":XMN_WORD_WRAP", &resclass_XmRBoolean, XmNwordWrap}, /* Xm:TextOut.c */
  {":XMN_SCROLL_VERTICAL", &resclass_XmRBoolean, XmNscrollVertical}, /* Xm:TextOut.c */
  {":XMN_SCROLL_HORIZONTAL", &resclass_XmRBoolean, XmNscrollHorizontal}, /* Xm:TextOut.c */
  {":XMN_SCROLL_LEFT_SIDE", &resclass_XmRBoolean, XmNscrollLeftSide}, /* Xm:TextOut.c */
  {":XMN_SCROLL_TOP_SIDE", &resclass_XmRBoolean, XmNscrollTopSide}, /* Xm:TextOut.c */
  {":XMN_CURSOR_POSITION_VISIBLE", &resclass_XmRBoolean, XmNcursorPositionVisible}, /* Xm:TextOut.c */
  {":XMN_VISIBLE_WHEN_OFF", &resclass_XmRBoolean, XmNvisibleWhenOff}, /* Xm:ToggleB.c Xm:ToggleBG.c */
  {":XMN_SET", &resclass_XmRBoolean, XmNset}, /* Xm:ToggleB.c Xm:ToggleBG.c */
  {":XMN_INDICATOR_ON", &resclass_XmRBoolean, XmNindicatorOn}, /* Xm:ToggleB.c Xm:ToggleBG.c */
  {":XMN_FILL_ON_SELECT", &resclass_XmRBoolean, XmNfillOnSelect}, /* Xm:ToggleB.c Xm:ToggleBG.c */
  {":XMN_DESTROY_CALLBACK", &resclass_XmRCallback, XmNdestroyCallback},	/* Xt:Object.c */
  {":XMN_POPUP_CALLBACK", &resclass_XmRCallback, XmNpopupCallback}, /* Xt:Shell.c */
  {":XMN_POPDOWN_CALLBACK", &resclass_XmRCallback, XmNpopdownCallback},	/* Xt:Shell.c */
  {":XMN_FOCUS_POLICY_CHANGED", &resclass_XmRCallback, XmNfocusPolicyChanged}, /* Xt:Vendor.c */
  {":XMN_ACTIVATE_CALLBACK", &resclass_XmRCallback, XmNactivateCallback}, /* Xm:ArrowB.c Xm:ArrowBG.c Xm:CascadeB.c Xm:CascadeBG.c Xm:DrawnB.c Xm:PushB.c Xm:PushBG.c Xm:Text.c */
  {":XMN_ARM_CALLBACK", &resclass_XmRCallback, XmNarmCallback},	/* Xm:ArrowB.c Xm:ArrowBG.c Xm:DrawnB.c Xm:PushB.c Xm:PushBG.c Xm:ToggleB.c Xm:ToggleBG.c*/
  {":XMN_DISARM_CALLBACK", &resclass_XmRCallback, XmNdisarmCallback}, /* Xm:ArrowB.c Xm:ArrowBG.c Xm:DrawnB.c Xm:PushB.c Xm:PushBG.c Xm:ToggleB.c Xm:ToggleBG.c */
  {":XMN_FOCUS_CALLBACK", &resclass_XmRCallback, XmNfocusCallback}, /* Xm:BulletinB.c Xm:Text.c */
  {":XMN_MAP_CALLBACK", &resclass_XmRCallback, XmNmapCallback},	/* Xm:BulletinB.c Xm:RowColumn.c */
  {":XMN_UNMAP_CALLBACK", &resclass_XmRCallback, XmNunmapCallback}, /* Xm:BulletinB.c Xm:RowColumn.c */
  {":XMN_CASCADING_CALLBACK", &resclass_XmRCallback, XmNcascadingCallback}, /* Xm:CascadeB.c Xm:CascadeBG.c */
  {":XMN_COMMAND_ENTERED_CALLBACK", &resclass_XmRCallback, XmNcommandEnteredCallback}, /* Xm:Command.c */
  {":XMN_COMMAND_CHANGED_CALLBACK", &resclass_XmRCallback, XmNcommandChangedCallback}, /* Xm:Command.c */
  {":XMN_RESIZE_CALLBACK", &resclass_XmRCallback, XmNresizeCallback}, /* Xm:DrawingA.c Xm:DrawnB.c */
  {":XMN_EXPOSE_CALLBACK", &resclass_XmRCallback, XmNexposeCallback}, /* Xm:DrawingA.c Xm:DrawnB.c */
  {":XMN_INPUT_CALLBACK", &resclass_XmRCallback, XmNinputCallback}, /* Xm:DrawingA.c */
  {":XMN_HELP_CALLBACK", &resclass_XmRCallback, XmNhelpCallback}, /* Xm:Gadget.c Xm:Manager.c Xm:Primitive.c */
  {":XMN_SINGLE_SELECTION_CALLBACK", &resclass_XmRCallback, XmNsingleSelectionCallback}, /* Xm:List.c */
  {":XMN_MULTIPLE_SELECTION_CALLBACK", &resclass_XmRCallback, XmNmultipleSelectionCallback}, /* Xm:List.c */
  {":XMN_EXTENDED_SELECTION_CALLBACK", &resclass_XmRCallback, XmNextendedSelectionCallback}, /* Xm:List.c */
  {":XMN_BROWSE_SELECTION_CALLBACK", &resclass_XmRCallback, XmNbrowseSelectionCallback}, /* Xm:List.c */
  {":XMN_DEFAULT_ACTION_CALLBACK", &resclass_XmRCallback, XmNdefaultActionCallback}, /* Xm:List.c */ 
  {":XMN_OK_CALLBACK", &resclass_XmRCallback, XmNokCallback}, /* Xm:MessageB.c Xm:SelectioB.c */
  {":XMN_CANCEL_CALLBACK", &resclass_XmRCallback, XmNcancelCallback}, /* Xm:MessageB.c Xm:SelectioB.c */
  {":XMN_ENTRY_CALLBACK", &resclass_XmRCallback, XmNentryCallback}, /* Xm:RowColumn.c */
  {":XMN_VALUE_CHANGED_CALLBACK", &resclass_XmRCallback, XmNvalueChangedCallback}, /* Xm:Scale.c Xm:ScrollBar.c Xm:Text.c Xm:ToggleB.c Xm:ToggleBG.c */
  {":XMN_DRAG_CALLBACK", &resclass_XmRCallback, XmNdragCallback}, /* Xm:Scale.c Xm:ScrollBar.c */
  {":XMN_INCREMENT_CALLBACK", &resclass_XmRCallback, XmNincrementCallback}, /* Xm:ScrollBar.c */
  {":XMN_DECREMENT_CALLBACK", &resclass_XmRCallback, XmNdecrementCallback}, /* Xm:ScrollBar.c */
  {":XMN_PAGE_INCREMENT_CALLBACK", &resclass_XmRCallback, XmNpageIncrementCallback}, /* Xm:ScrollBar.c */
  {":XMN_PAGE_DECREMENT_CALLBACK", &resclass_XmRCallback, XmNpageDecrementCallback}, /* Xm:ScrollBar.c */
  {":XMN_TO_TOP_CALLBACK", &resclass_XmRCallback, XmNtoTopCallback}, /* Xm:ScrollBar.c */
  {":XMN_TO_BOTTOM_CALLBACK", &resclass_XmRCallback, XmNtoBottomCallback}, /* Xm:ScrollBar.c */
  {":XMN_NO_MATCH_CALLBACK", &resclass_XmRCallback, XmNnoMatchCallback}, /* Xm:SelectioB.c */
  {":XMN_APPLY_CALLBACK", &resclass_XmRCallback, XmNapplyCallback}, /* Xm:SelectioB.c */
  {":XMN_LOSING_FOCUS_CALLBACK", &resclass_XmRCallback, XmNlosingFocusCallback}, /* Xm:Text.c */
  {":XMN_MODIFY_VERIFY_CALLBACK", &resclass_XmRCallback, XmNmodifyVerifyCallback}, /* Xm:Text.c */
  {":XMN_MOTION_VERIFY_CALLBACK", &resclass_XmRCallback, XmNmotionVerifyCallback}, /* Xm:Text.c */
  {":XMN_MNEMONIC", &resclass_XmRChar, XmNmnemonic}, /* Xm:Label.c Xm:LabelG.c Xm:RowColumn.c */
  {":XMN_WIDTH", &resclass_XmRDimension, XmNwidth}, /* Xt:RectObj.c */
  {":XMN_HEIGHT", &resclass_XmRDimension, XmNheight}, /* Xt:RectObj.c */
  {":XMN_BORDER_WIDTH", &resclass_XmRDimension, XmNborderWidth}, /* Xt:RectObj.c Xm:Gadget.c Xm:Manager.c Xm:Primitive.c Xm:Sash.c */
  {":XMN_SASH_WIDTH", &resclass_XmRDimension, XmNsashWidth}, /* Xm:PanedW.c */
  {":XMN_SASH_HEIGHT", &resclass_XmRDimension, XmNsashHeight}, /* Xm:PanedW.c */
  {":XMN_RCMARGIN_WIDTH", &resclass_XmRDimension, XmNmarginWidth}, /* Xm:RowColumn.c -- this is a special alias for r/c widget due to name conflict*/
  {":XMN_RCMARGIN_HEIGHT", &resclass_XmRDimension, XmNmarginHeight}, /* Xm:RowColumn.c -- this is a special alias for r/c widget due to name conflic*/
  {":XMN_SPACING", &resclass_XmRDimension, XmNspacing},	/* Xm:RowColumn.c */
  {":XMN_ENTRY_BORDER", &resclass_XmRDimension, XmNentryBorder}, /* Xm:RowColumn.c */
  {":XMN_SCALE_WIDTH", &resclass_XmRDimension, XmNscaleWidth}, /* Xm:Scale.c */
  {":XMN_SCALE_HEIGHT", &resclass_XmRDimension, XmNscaleHeight}, /* Xm:Scale.c */
  {":XMN_EDIT_MODE", &resclass_XmREditMode, XmNeditMode}, /* Xm:Text.c */
  {":XMN_BUTTON_FONT_LIST", &resclass_XmRFontList, XmNbuttonFontList}, /* Xm:BulletinB.c */
  {":XMN_LABEL_FONT_LIST", &resclass_XmRFontList, XmNlabelFontList}, /* Xm:BulletinB.c */
  {":XMN_TEXT_FONT_LIST", &resclass_XmRFontList, XmNtextFontList}, /* Xm:BulletinB.c */
  {":XMN_FONT_LIST", &resclass_XmRFontList, XmNfontList}, /* Xm:Label.c Xm:LabelG.c Xm:List.c Xm:Scale.c Xm:TextOut.c */
  {":XMN_INDICATOR_TYPE", &resclass_XmRIndicatorType, XmNindicatorType}, /* Xm:ToggleB.c Xm:ToggleBG.c */
  {":XMN_DEPTH", &resclass_XmRInt, XmNdepth}, /* Xt:Shell.c Xt:WindowObj.c */
  {":XMN_WM_TIMEOUT", &resclass_XmRInt, XmNwmTimeout}, /* Xt:Shell.c */
  {":XMN_MIN_WIDTH", &resclass_XmRInt, XmNminWidth}, /* Xt:Shell.c */
  {":XMN_MIN_HEIGHT", &resclass_XmRInt, XmNminHeight}, /* Xt:Shell.c */
  {":XMN_MAX_WIDTH", &resclass_XmRInt, XmNmaxWidth}, /* Xt:Shell.c */
  {":XMN_WIDTH_INC", &resclass_XmRInt, XmNwidthInc}, /* Xt:Shell.c */
  {":XMN_HEIGHT_INC", &resclass_XmRInt, XmNheightInc}, /* Xt:Shell.c */
  {":XMN_MIN_ASPECT_X", &resclass_XmRInt, XmNminAspectX}, /* Xt:Shell.c */
  {":XMN_MIN_ASPECT_Y", &resclass_XmRInt, XmNminAspectY}, /* Xt:Shell.c */
  {":XMN_MAX_ASPECT_X", &resclass_XmRInt, XmNmaxAspectX}, /* Xt:Shell.c */
  {":XMN_MAX_ASPECT_Y", &resclass_XmRInt, XmNmaxAspectY}, /* Xt:Shell.c */
  {":XMN_INITIAL_STATE", &resclass_XmRInt, XmNinitialState}, /* Xt:Shell.c */
  {":XMN_ICON_X", &resclass_XmRInt, XmNiconX}, /* Xt:Shell.c */
  {":XMN_ICON_Y", &resclass_XmRInt, XmNiconY}, /* Xt:Shell.c */
  {":XMN_ARGC", &resclass_XmRInt, XmNargc}, /* Xt:Shell.c */
  {":XMN_MWM_DECORATIONS", &resclass_XmRInt, XmNmwmDecorations}, /* Xt:Vendor.c */
  {":XMN_MWM_FUNCTIONS", &resclass_XmRInt, XmNmwmFunctions}, /* Xt:Vendor.c */
  {":XMN_MWM_INPUT_MODE", &resclass_XmRInt, XmNmwmInputMode}, /* Xt:Vendor.c */
  {":XMN_MAPPING_DELAY", &resclass_XmRInt, XmNmappingDelay}, /* Xm:CascadeB.c Xm:CascadeBG.c */
  {":XMN_HISTORY_ITEM_COUNT", &resclass_XmRInt, XmNhistoryItemCount}, /* Xm:Command.c */
  {":XMN_HISTORY_MAX_ITEMS", &resclass_XmRInt, XmNhistoryMaxItems}, /* Xm:Command.c */
  {":XMN_HISTORY_VISIBLE_ITEM_COUNT", &resclass_XmRInt, XmNhistoryVisibleItemCount}, /* Xm:Command.c */
  {":XMN_HORIZONTAL_SPACING", &resclass_XmRInt, XmNhorizontalSpacing}, /* Xm:Form.c */
  {":XMN_VERTICAL_SPACING", &resclass_XmRInt, XmNverticalSpacing}, /* Xm:Form.c */
  {":XMN_FRACTION_BASE", &resclass_XmRInt, XmNfractionBase}, /* Xm:Form.c */
  {":XMN_TOP_POSITION", &resclass_XmRInt, XmNtopPosition}, /* Xm:Form.c Xm:Text.c */
  {":XMN_BOTTOM_POSITION", &resclass_XmRInt, XmNbottomPosition}, /* Xm:Form.c */
  {":XMN_LEFT_POSITION", &resclass_XmRInt, XmNleftPosition}, /* Xm:Form.c */
  {":XMN_RIGHT_POSITION", &resclass_XmRInt, XmNrightPosition}, /* Xm:Form.c */
  {":XMN_TOP_OFFSET", &resclass_XmRInt, XmNtopOffset}, /* Xm:Form.c */
  {":XMN_BOTTOM_OFFSET", &resclass_XmRInt, XmNbottomOffset}, /* Xm:Form.c */
  {":XMN_LEFT_OFFSET", &resclass_XmRInt, XmNleftOffset}, /* Xm:Form.c */
  {":XMN_RIGHT_OFFSET", &resclass_XmRInt, XmNrightOffset}, /* Xm:Form.c */
  {":XMN_ITEM_COUNT", &resclass_XmRInt, XmNitemCount}, /* Xm:List.c */
  {":XMN_SELECTED_ITEM_COUNT", &resclass_XmRInt, XmNselectedItemCount},	/* Xm:List.c */
  {":XMN_VISIBLE_ITEM_COUNT", &resclass_XmRInt, XmNvisibleItemCount}, /* Xm:List.c */
  {":XMN_DOUBLE_CLICK_INTERVAL", &resclass_XmRInt, XmNdoubleClickInterval}, /* Xm:List.c */
  {":XMN_SASH_SHADOW_THICKNESS", &resclass_XmRInt, XmNsashShadowThickness}, /* Xm:PanedW.c */
  {":XMN_MINIMUM", &resclass_XmRInt, XmNminimum}, /* Xm:PanedW.c Xm:Scale.c Xm:ScrollBar.c */
  {":XMN_MAXIMUM", &resclass_XmRInt, XmNmaximum}, /* Xm:PanedW.c Xm:Scale.c Xm:ScrollBar.c */
  {":XMN_VALUE", &resclass_XmRInt, XmNvalue}, /* Xm:Scale.c Xm:ScrollBar.c */
  {":XMN_SLIDER_SIZE", &resclass_XmRInt, XmNsliderSize}, /* Xm:ScrollBar.c */
  {":XMN_INCREMENT", &resclass_XmRInt, XmNincrement}, /* Xm:ScrollBar.c */
  {":XMN_PAGE_INCREMENT", &resclass_XmRInt, XmNpageIncrement}, /* Xm:ScrollBar.c */
  {":XMN_INITIAL_DELAY", &resclass_XmRInt, XmNinitialDelay}, /* Xm:ScrollBar.c */
  {":XMN_REPEAT_DELAY", &resclass_XmRInt, XmNrepeatDelay}, /* Xm:ScrollBar.c */
  {":XMN_TEXT_COLUMNS", &resclass_XmRInt, XmNtextColumns}, /* Xm:SelectionB.c */
  {":XMN_LIST_ITEM_COUNT", &resclass_XmRInt, XmNlistItemCount},	/* Xm:SelectionB.c */
  {":XMN_LIST_VISIBLE_ITEM_COUNT", &resclass_XmRInt, XmNlistVisibleItemCount}, /* Xm:SelectionB.c */
  {":XMN_MAX_LENGTH", &resclass_XmRInt, XmNmaxLength}, /* Xm:Text.c */
  {":XMN_CURSOR_POSITION", &resclass_XmRInt, XmNcursorPosition}, /* Xm:Text.c */
  {":XMN_SELECTION_ARRAY_COUNT", &resclass_XmRInt, XmNselectionArrayCount}, /* Xm:TextIn.c */
  {":XMN_SELECT_THRESHOLD", &resclass_XmRInt, XmNselectThreshold}, /* Xm:TextIn.c */
  {":XMN_BLINK_RATE", &resclass_XmRInt, XmNblinkRate}, /* Xm:TextOut.c */
  {":XMN_LABEL_TYPE", &resclass_XmRLabelType, XmNlabelType}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_ORIENTATION", &resclass_XmROrientation, XmNorientation}, /* Xm:RowColumn.c Xm:scale.c Xm:ScrollBar.c Xm:SeparatoG.c Xm:Separator.c */
  {":XMN_PACKING", &resclass_XmRPacking, XmNpacking}, /* Xm:RowColumn.c */
  {":XMN_BORDER_COLOR", &resclass_XmRPixel, XmNborderColor}, /* Xt:WindowObj.c */
  {":XMN_FOREGROUND", &resclass_XmRPixel, XmNforeground}, /* Xm:Manager.c Xm:Primitive.c */
  {":XMN_BACKGROUND", &resclass_XmRPixel, XmNbackground}, /* Xt:WindowObj.c Xm:Manager.c Xm:Primitive.c */
  {":XMN_HIGHLIGHT_COLOR", &resclass_XmRPixel, XmNhighlightColor}, /* Xm:Manager.c Xm:Primitive.c */
  {":XMN_TOP_SHADOW_COLOR", &resclass_XmRPixel, XmNtopShadowColor}, /* Xm:Manager.c Xm:Primitive.c */
  {":XMN_BOTTOM_SHADOW_COLOR", &resclass_XmRPixel, XmNbottomShadowColor}, /* Xm:Manager.c Xm:Primitive.c */
  {":XMN_ARM_COLOR", &resclass_XmRPixel, XmNarmColor}, /* Xm:PushB.c Xm:PushBG.c */
  {":XMN_SELECT_COLOR", &resclass_XmRPixel, XmNselectColor}, /* Xm:ToggleB.c Xm:ToggleBG.c */
  {":XMN_ICON_PIXMAP", &resclass_XmRPixmap, XmNiconPixmap}, /* Xt:Shell.c */
  {":XMN_ICON_MASK", &resclass_XmRPixmap, XmNiconMask},	/* Xt:Shell.c */
  {":XMN_BACKGROUND_PIXMAP", &resclass_XmRPixmap, XmNbackgroundPixmap},	/* Xt:WindowObj.c Xm:Manager.c Xm:Primitive.c */
  {":XMN_BORDER_PIXMAP", &resclass_XmRPixmap, XmNborderPixmap},	/* Xt:WindowObj.c */
  {":XMN_LABEL_INSENSITIVE_PIXMAP", &resclass_XmRManForegroundPixmap, XmNlabelInsensitivePixmap}, /* Xm:Label.c Xm:LabelG.c -- XmRPixmap */
  {":XMN_SELECT_INSENSITIVE_PIXMAP", &resclass_XmRManForegroundPixmap, XmNselectInsensitivePixmap}, /* Xm/ToggleBG.c Xm/ToggleB.c -- XmRPixmap */
  {":XMN_SELECT_PIXMAP", &resclass_XmRManForegroundPixmap, XmNselectPixmap}, /* Xm/ToggleB.c -- XmRPrimForegroundPixmap; Xm/ToggleBG.c -- XmRPixmap */
  {":XMN_CASCADE_PIXMAP", &resclass_XmRManForegroundPixmap, XmNcascadePixmap}, /* Xm/CascadeB.c -- XmRPrimForegroundPixmap; Xm/CascadeBG.c -- XmRManForegroundPixmap */
  {":XMN_LABEL_PIXMAP", &resclass_XmRManForegroundPixmap, XmNlabelPixmap}, /* Xm/Label.c -- XmRPrimForegroundPixmap; Xm/LabelG.c -- XmRManForegroundPixmap */
  {":XMN_HIGHLIGHT_PIXMAP", &resclass_XmRManHighlightPixmap, XmNhighlightPixmap}, /* in Xm/Primitive.c -- XmRPrimHighlightPixmap; Xm/Manager.c -- XmRManHighlightPixmap */
  {":XMN_ARM_PIXMAP", &resclass_XmRManForegroundPixmap, XmNarmPixmap}, /* in Xm/PushB.c Xm/PushBG.c -- XmRPrimForegroundPixmap */
  {":XMN_SYMBOL_PIXMAP", &resclass_XmRManForegroundPixmap, XmNsymbolPixmap}, /* in Xm/MessageB.c -- XmRManForegroundPixmap */
  {":XMN_TOP_SHADOW_PIXMAP", &resclass_XmRManTopShadowPixmap, XmNtopShadowPixmap}, /* in Xm/Primitive.c -- XmRPrimTopShadowPixmap; Xm/Manager.c -- XmRManTopShadowPixmap */
  {":XMN_BOTTOM_SHADOW_PIXMAP", &resclass_XmRManBottomShadowPixmap, XmNbottomShadowPixmap}, /* in Xm/Primitive.c -- XmRPrimBottomShadowPixmap; Xm/Manager.c -- XmRManBottomShadowPixmap */
  {":XMN_COLORMAP", &resclass_XmRPointer, XmNcolormap},	/* Xt:Shell.c Xt:WindowObj.c */
  {":XMN_ARGV", &resclass_XmRPointer, XmNargv},	/* Xt:Shell.c */
  {":XMN_SCREEN", &resclass_XmRPointer, XmNscreen}, /* Xt:WindowObj.c */
  {":XMN_SOURCE", &resclass_XmRPointer, XmNsource}, /* Xm:Text.c */
  {":XMN_SELECTION_ARRAY", &resclass_XmRPointer, XmNselectionArray}, /* Xm:TextIn.c */
  {":XMN_X", &resclass_XmRPosition, XmNx}, /* Xt:RectObj.c */
  {":XMN_Y", &resclass_XmRPosition, XmNy}, /* Xt:RectObj.c */
  {":XMN_SASH_INDENT", &resclass_XmRPosition, XmNsashIndent}, /* Xm:PanedW.c */
  {":XMN_ROW_COLUMN_TYPE", &resclass_XmRRowColumnType, XmNrowColumnType}, /* Xm:RowColumn.c */
  {":XMN_SHADOW_THICKNESS", &resclass_XmRShort, XmNshadowThickness}, /* Xm:BulletinB.c Xm:CascadeB.c Xm:CascadeBG.c Xm:DrawnB.c Xm:Gadget.c Xm:Label.c Xm:LabelG.c Xm:Manager.c Xm:Primitive.c Xm:PushB.c Xm:PushBG.c Xm:ScrolledW.c */
  {":XMN_MARGIN_WIDTH", &resclass_XmRShort, XmNmarginWidth}, /* Xm:BulletinB.c Xm:DrawingA.c Xm:Frame.c Xm:Label.c Xm:LabelG.c Xm:PanedW.c Xm:Text.c */
  {":XMN_MARGIN_HEIGHT", &resclass_XmRShort, XmNmarginHeight}, /* Xm:BulletinB.c Xm:DrawingA.c Xm:Frame.c Xm:Label.c Xm:LabelG.c Xm:PanedW.c Xm:Text.c */
  {":XMN_HIGHLIGHT_THICKNESS", &resclass_XmRShort, XmNhighlightThickness}, /* Xm:Gadget.c Xm:Primitive.c Xm:Scale.c */
  {":XMN_MARGIN_LEFT", &resclass_XmRShort, XmNmarginLeft}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_MARGIN_RIGHT", &resclass_XmRShort, XmNmarginRight}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_MARGIN_TOP", &resclass_XmRShort, XmNmarginTop}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_MARGIN_BOTTOM", &resclass_XmRShort, XmNmarginBottom}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_LIST_SPACING", &resclass_XmRShort, XmNlistSpacing}, /* Xm:List.c */
  {":XMN_LIST_MARGIN_WIDTH", &resclass_XmRShort, XmNlistMarginWidth}, /* Xm:List.c */
  {":XMN_LIST_MARGIN_HEIGHT", &resclass_XmRShort, XmNlistMarginHeight},	/* Xm:List.c */
  {":XMN_MAIN_WINDOW_MARGIN_WIDTH", &resclass_XmRShort, XmNmainWindowMarginWidth}, /* Xm:MainW.c */
  {":XMN_MAIN_WINDOW_MARGIN_HEIGHT", &resclass_XmRShort, XmNmainWindowMarginHeight}, /* Xm:MainW.c */
  {":XMN_SHOW_AS_DEFAULT", &resclass_XmRShort, XmNshowAsDefault}, /* Xm:PushB.c Xm:PushBG.c */
  {":XMN_NUM_COLUMNS", &resclass_XmRShort, XmNnumColumns}, /* Xm:RowColumn.c */
  {":XMN_DECIMAL_POINTS", &resclass_XmRShort, XmNdecimalPoints}, /* Xm:Scale.c */
  {":XMN_SCROLLED_WINDOW_MARGIN_WIDTH", &resclass_XmRShort, XmNscrolledWindowMarginWidth}, /* Xm:ScrolledW.c */
  {":XMN_SCROLLED_WINDOW_MARGIN_HEIGHT", &resclass_XmRShort, XmNscrolledWindowMarginHeight}, /* Xm:ScrolledW.c */
  {":XMN_MARGIN", &resclass_XmRShort, XmNmargin}, /* Xm:Separator.c Xm:SeparatoG.c */
  {":XMN_COLUMNS", &resclass_XmRShort, XmNcolumns}, /* Xm:TextOut.c */
  {":XMN_ROWS", &resclass_XmRShort, XmNrows}, /* Xm:TextOut.c */
  {":XMN_ACCELERATOR", &resclass_XmRString, XmNaccelerator}, /* Xm:Label.c Xm:LabelG.c */
  {":XMN_MENU_ACCELERATOR", &resclass_XmRString, XmNmenuAccelerator}, /* Xm:RowColumn.c */
  {":XMN_STRING", &resclass_XmRString, XmNvalue}, /* Xm:Text.c -- note: this is alias for resource XmNvalue on the textedit widget -- this name already in use (as an int) for XmScale.c and XmScrollbar.c */
  {":XMN_GEOMETRY", &resclass_XmRString, XmNgeometry}, /* Xt:Shell.c */
  {":XMN_TITLE", &resclass_XmRString, XmNtitle}, /* Xt:Shell.c */
  {":XMN_ICON_NAME", &resclass_XmRString, XmNiconName},	/* Xt:Shell.c -- toplevelshell */
  {":XMN_MWM_MENU", &resclass_XmRString, XmNmwmMenu}, /* Xt:Vendor.c */
  {":XMN_STRING_DIRECTION", &resclass_XmRStringDirection, XmNstringDirection}, /* Xm:BulletinB.c Xm:Label.c Xm:LabelG.c Xm:List.c */
  {":XMN_TEXT_TRANSLATIONS", &resclass_XmRTranslationTable, XmNtextTranslations}, /* Xm/BulletinB.c */
  {":XMN_TRANSLATIONS", &resclass_XmRTranslationTable, XmNtranslations}, /* Xt/WindowObj.c */
  {":XMN_WHICH_BUTTON", &resclass_XmRWhichButton, XmNwhichButton}, /* Xm/RowColumn.c */
  {":XMN_ICON_WINDOW", &resclass_XmRWindow, XmNiconWindow}, /* Xt/Shell.c */
  {":XMN_WINDOW_GROUP", &resclass_XmRWindow, XmNwindowGroup}, /* Xt/Shell.c */
  {":XMN_SUB_MENU_ID", &resclass_XmRWidget, XmNsubMenuId}, /* Xm:CascadeB.c Xm:CascadeBG.c Xm:RowColumn.c*/
  {":XMN_MENU_HELP_WIDGET", &resclass_XmRWidget, XmNmenuHelpWidget}, /* Xm:RowColumn.c -- used only for XmMENU_BAR */
  {":XMN_MENU_HISTORY", &resclass_XmRWidget, XmNmenuHistory}, /* Xm:RowColumn.c -- last activated menu entry */
  {":XMN_DEFAULT_BUTTON", &resclass_XmRWidget, XmNdefaultButton}, /* Xm/BulletinB.c */
  {":XMN_CANCEL_BUTTON", &resclass_XmRWidget, XmNcancelButton},	/* Xm/BulletinB.c */
  {":XMN_TOP_WIDGET", &resclass_XmRWidget, XmNtopWidget}, /* Xm/Form.c */
  {":XMN_BOTTOM_WIDGET", &resclass_XmRWidget, XmNbottomWidget},	/* Xm/Form.c */
  {":XMN_LEFT_WIDGET", &resclass_XmRWidget, XmNleftWidget}, /* Xm/Form.c */
  {":XMN_RIGHT_WIDGET", &resclass_XmRWidget, XmNrightWidget}, /* Xm/Form.c */
  {":XMN_HORIZONTAL_SCROLL_BAR", &resclass_XmRWidget, XmNhorizontalScrollBar}, /* Xm/List.c Xm/ScrolledW.c */
  {":XMN_VERTICAL_SCROLL_BAR", &resclass_XmRWidget, XmNverticalScrollBar}, /* Xm/List.c Xm/ScrolledW.c */
  {":XMN_COMMAND_WINDOW", &resclass_XmRWidget, XmNcommandWindow}, /* Xm/MainW.c */
  {":XMN_MENU_BAR", &resclass_XmRWidget, XmNmenuBar}, /* Xm/MainW.c */
  {":XMN_WORK_WINDOW", &resclass_XmRWidget, XmNworkWindow}, /* Xm/ScrolledW.c */
  {":XMN_CLIP_WINDOW", &resclass_XmRWidget, XmNclipWindow}, /* Xm/ScrolledW.c */
  {":XMN_DIALOG_TITLE", &resclass_XmRXmString, XmNdialogTitle},	/* Xm/BulletinB.c */
  {":XMN_PROMPT_STRING", &resclass_XmRXmString, XmNpromptString}, /* Xm/Command.c */
  {":XMN_COMMAND", &resclass_XmRXmString, XmNcommand}, /* Xm/Command.c */
  {":XMN_TEXT_STRING", &resclass_XmRXmString, XmNtextString}, /* Xm/SelectioB.c */
  {":XMN_LABEL_STRING", &resclass_XmRXmString, XmNlabelString},	/* Xm/DrawnB.c Xm/RowColumn.c Xm/Label.c Xm/LabelG.c Xm/RowColumn.c */
  {":XMN_LIST_LABEL_STRING", &resclass_XmRXmString, XmNlistLabelString}, /* Xm/FileSB.c Xm/SelectioB.c */
  {":XMN_APPLY_LABEL_STRING", &resclass_XmRXmString, XmNapplyLabelString}, /* Xm/FileSB.c Xm/SelectioB.c */
  {":XMN_DIR_SPEC", &resclass_XmRXmString, XmNdirSpec},	/* Xm/FileSB.c */
  {":XMN_FILTER_LABEL_STRING", &resclass_XmRXmString, XmNfilterLabelString}, /* Xm/FileSB.c */
  {":XMN_DIR_MASK", &resclass_XmRXmString, XmNdirMask},	/* Xm/FileSB.c */
  {":XMN_ACCELERATOR_TEXT", &resclass_XmRXmString, XmNacceleratorText},	/* Xm/Label.c Xm/LabelG.c */
  {":XMN_MESSAGE_STRING", &resclass_XmRXmString, XmNmessageString}, /* Xm/MessageB.c */
  {":XMN_OK_LABEL_STRING", &resclass_XmRXmString, XmNokLabelString}, /* Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_CANCEL_LABEL_STRING", &resclass_XmRXmString, XmNcancelLabelString}, /* Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_HELP_LABEL_STRING", &resclass_XmRXmString, XmNhelpLabelString}, /* Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_TITLE_STRING", &resclass_XmRXmString, XmNtitleString},	/* Xm/Scale.c */
  {":XMN_SELECTION_LABEL_STRING", &resclass_XmRXmString, XmNselectionLabelString}, /* Xm/SelectioB.c */
  {":XMN_HISTORY_ITEMS", &resclass_XmRXmStringTable, XmNhistoryItems}, /* Xm/Command.c */
  {":XMN_ITEMS", &resclass_XmRXmStringTable, XmNitems},	/* Xm/List.c */
  {":XMN_SELECTED_ITEMS", &resclass_XmRXmStringTable, XmNselectedItems}, /* Xm/List.c */
  {":XMN_LIST_ITEMS", &resclass_XmRXmStringTable, XmNlistItems}, /* Xm/SelectioB.c */
  {":XMN_UNIT_TYPE", &resclass_XmRUnitType, XmNunitType}, /* Xm/Gadget.c Xm/Manager.c Xm/Primitive.c */
  {":XMN_SHELL_UNIT_TYPE", &resclass_XmRUnitType, XmNshellUnitType}, /* Xt/Vendor.c -- note: this is listed as XmRShellUnitType which is same as XmRUnitType */
  {":XMN_RESIZE_POLICY", &resclass_XmRResizePolicy, XmNresizePolicy}, /* Xm/BulletinB.c Xm/Command.c Xm/DrawingA.c */
  {":XMN_TOP_ATTACHMENT", &resclass_XmRAttachment, XmNtopAttachment}, /* Xm/Form.c */
  {":XMN_BOTTOM_ATTACHMENT", &resclass_XmRAttachment, XmNbottomAttachment}, /* Xm/Form.c */
  {":XMN_LEFT_ATTACHMENT", &resclass_XmRAttachment, XmNleftAttachment},	/* Xm/Form.c */
  {":XMN_RIGHT_ATTACHMENT", &resclass_XmRAttachment, XmNrightAttachment}, /* Xm/Form.c */
  {":XMN_SHADOW_TYPE", &resclass_XmRShadowType, XmNshadowType},	/* Xm/BulletinB.c Xm/DrawnB.c Xm/Frame.c */
  {":XMN_ARROW_DIRECTION", &resclass_XmRArrowDirection, XmNarrowDirection}, /* Xm/ArrowB.c Xm/ArrowBG.c */
  {":XMN_SEPARATOR_TYPE", &resclass_XmRSeparatorType, XmNseparatorType}, /* Xm/SeparatoG.c Xm/Separator.c */
  {":XMN_PROCESSING_DIRECTION", &resclass_XmRProcessingDirection, XmNprocessingDirection}, /* Xm/Scale.c Xm/ScrollBar.c */
  {":XMN_SELECTION_POLICY", &resclass_XmRSelectionPolicy, XmNselectionPolicy}, /* Xm/List.c */
  {":XMN_LIST_SIZE_POLICY", &resclass_XmRListSizePolicy, XmNlistSizePolicy}, /* Xm/List.c */
  {":XMN_SCROLLING_POLICY", &resclass_XmRScrollingPolicy, XmNscrollingPolicy}, /* Xm/ScrolledW.c */
  {":XMN_VISUAL_POLICY", &resclass_XmRVisualPolicy, XmNvisualPolicy}, /* Xm/ScrolledW.c */
  {":XMN_SCROLL_BAR_DISPLAY_POLICY", &resclass_XmRScrollBarDisplayPolicy, XmNscrollBarDisplayPolicy}, /* Xm/List.c Xm/ScrolledW.c */
  {":XMN_SCROLL_BAR_PLACEMENT", &resclass_XmRScrollBarPlacement, XmNscrollBarPlacement}, /* Xm/ScrolledW.c */
  {":XMN_DIALOG_STYLE", &resclass_XmRDialogStyle, XmNdialogStyle}, /* Xm/BulletinB.c */
  {":XMN_DEFAULT_BUTTON_TYPE", &resclass_XmRDefaultButtonType, XmNdefaultButtonType}, /* Xm/MessageB.c */
  {":XMN_DIALOG_TYPE", &resclass_XmRDialogType, XmNdialogType},	/* Xm/Command.c Xm/FileSB.c Xm/MessageB.c Xm/SelectioB.c */
  {":XMN_ENTRY_CLASS", &resclass_XmRWidgetClass, XmNentryClass}, /* Xm:RowColumn.c */
  {":XMN_DELETE_RESPONSE", &resclass_XmRDeleteResponse, XmNdeleteResponse}, /* X11/Vendor.c Xm/DialogS.c */
  {":XMN_KEYBOARD_FOCUS_POLICY", &resclass_XmRKeyboardFocusPolicy, XmNkeyboardFocusPolicy}, /* X11/Vendor.c */
  {NULL, NULL, NULL, NULL}	/* BY ALL MEANS NECESSARY -- end of table marker */
};

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

#define FREEABLES_SIZE_INCREMENT 50
static struct Argval_Deallocator_Pair {
  caddr_t pointer;
  void (*deallocator)();
} *freeables; /* initialized in Wres_Init() */
static int freeables_size;	/* maximum index based on size of freeables */
static int freeables_end_idx;	/* index of last elt in freeables */

/******************************************************************************
 * Wres_Free_C_Arglist_Data():
 *
 * This procedure frees any unneeded structures that were created by
 * Wres_Get_LispArglist(). It should be called only after the C arglist
 * returned by Wres_Get_LispArglist() is used by a widget creation routine
 * (see :isnew methods on widgets) or by XtSetValues().
 ******************************************************************************/
void Wres_Free_C_Arglist_Data()
{
  int i = 0;
  while (i < freeables_end_idx) {
    (*(freeables[i].deallocator))(freeables[i].pointer);
    i++;
  }
  freeables_end_idx = 0;	/* reset */
}


/******************************************************************************
 * Wres_Free_This_Later():
 *
 * Stores a pointer to an allocated chunk of memory, and a procedure to deallocate
 * that memory. This will get called upon calling Wres_Free_C_Arglist_Data().
 ******************************************************************************/
void Wres_Free_This_Later(pointer, deallocator)
     caddr_t pointer;
     void (*deallocator)();
{
  if (freeables_end_idx >= freeables_size) {
    freeables = (struct Argval_Deallocator_Pair *) XtRealloc(freeables, (unsigned) ((freeables_size + FREEABLES_SIZE_INCREMENT) * sizeof(struct Argval_Deallocator_Pair)));
    freeables_size += FREEABLES_SIZE_INCREMENT;
  }
  freeables[freeables_end_idx].pointer = pointer;
  freeables[freeables_end_idx].deallocator = deallocator;
  freeables_end_idx++;
}

/**************************************************************************/
/**************************************************************************/
/**************************************************************************/
/**************************************************************************/

/*****************************************************************************
 * Wres_Get_LispArglist():
 *
 * This routine will get all remaining arguments off of the argument stack and
 * treat them as a list of name/value pairs for arguments. It returns an ArgList
 * representing the Xtoolkit version of the arglist passed in, which MUST NOT
 * BE FREED BY THE CALLER.
 *
 * This subroutine will signal an error if
 * (1) an invalid resource name is used;
 * (2) the value associated with the resource name is of the wrong type
 *     (each resource_name expects a unique resource value, if the value
 *     is a string instead of a value of the expected type, then a resource
 *     converter is called to convert this value. That may too result in
 *     an error being signaled if the resource converter couldn't be found...)
 *
 * This routine will return with the argument stack empty, or it will signal
 * an error.
 ****************************************************************************/
#define XTARGLIST_SIZE_INCREMENT 20
static ArgList xt_arglist;	/* init'd in Wres_Init() */
static Cardinal xt_arglist_size; /* init'd in Wres_Init() */
ArgList Wres_Get_LispArglist(o_widget, widgetID, prepend_args, prepend_nargs, nargs)
     LVAL     o_widget;
     Widget   widgetID;		/* for calling XtConvert if type conv needed */
     ArgList  prepend_args;	/* arguments to be prepended to result */
     Cardinal prepend_nargs;	/* number of arguments to be prepended */
     Cardinal *nargs;		/* returns number of args in result */
{
  ArgList             xt_arg;
  Cardinal            lisp_nargs;
  LVAL                lval_resname, lval_resval;
  Resource_Instance  *resource;
  extern void         Wpm_Set_Pixmap_Reference(); /* w_pixmap.c */
    
  if ((xlargc & 1) != 0)	/* if there's an odd number of args on the stack */
    xlerror("Widget argument list must consist of pairs of resource-name & value.\n	An odd number of arguments were found, the first being:",
	    *xlargv);

  lisp_nargs = xlargc / 2;	/* the number of name/value pairs */
  *nargs = lisp_nargs + prepend_nargs;

  if (*nargs > xt_arglist_size) { /* make sure xt_arglist is big enough */
    xt_arglist = (ArgList) XtRealloc(xt_arglist, (unsigned) (*nargs * sizeof(Arg)));
    xt_arglist_size = *nargs;
  }
  xt_arg = xt_arglist;

  /* copy prepend_args into result ArgList */
  while (prepend_nargs--)
    *xt_arg++ = *prepend_args++;

  /* now append lisp_arglist into result ArgList (starting at xt_arg) */
  while (lisp_nargs--) {	/* while more name/value pairs to process */
    /* 
     * get resource name
     */
    lval_resname = nextarg();
    if (xtresource_p(lval_resname))
      resource = get_xtresource(lval_resname);
    else if (symbolp(lval_resname)) { /* hasn't been eVALuated yet... */
      lval_resname = getvalue(lval_resname); /* get the value */
      if (xtresource_p(lval_resname))
	resource = get_xtresource(lval_resname);
      else
	xlerror("In Widget Arglist: Invalid resource keyword in widget resource list.", lval_resname);
    }
    else
      xlerror("In Widget Arglist: Invalid resource name in widget resource list.", lval_resname);

    xt_arg->name = resource->name;

    /* 
     * get resource value.
     */
    lval_resval = nextarg();

    /* 
     * check to see if the lval_resval is of the type corresponding to resource.
     * And if it is, then call appropriate LVAL-->caddr_t procedure to convert
     * from a lisp argument to the C equivalent.
     */
    if ((lval_resval && (ntype(lval_resval) == resource->class->LVAL_type))
	|| (resource->class->LVAL_type == FREE)) /* FREE stands for any LVAL type (eg, for XmRBoolean, XmRXmStringTable) */
      xt_arg->value = (XtArgVal) (*(resource->class->LVAL_to_resource_val_converter))(lval_resval, o_widget, lval_resname);

    /* 
     * If the lisp value is a string, and we expect an XmString, override the
     * default XmRString-->XmRXmString conversion that would be done below
     * (via XtConvert()) since the Motif 1.0 built-in resource converter
     * _XmCvtStringToXmString() uses XmStringCreate(). We want to use
     * XmStringCreateLtoR() which will allow us to create XmStrings with
     * multiple lines by embedding '\n' characters inside our strings.
     * (From my anglocentric perspective, I consider the use of XmStringCreate()
     * by XtConvert() to be a bug.)
     *
     * Note also that in using _XmCvtStringToXmString(),  we cannot pass the
     * XmString on to Wres_Free_This_Later() because Motif 1.0 seems to keep
     * a cache of string<-->XmStrings. Thus once you free XmString("foo") once,
     * subsequent calls to XmString("foo") will return a bogus XmString.
     * This caching is no doubt the result of Motif's designers attempting to
     * hack around the built-in memory leaks in XtConvert(). I cannot verify
     * this right now because I only have sources to snapshot #11, and Motif 1.0
     * seems different w/r/t/ handling of XmString conversions. (Barf).
     */
    else if (stringp(lval_resval) && (resource->class == &resclass_XmRXmString)) {
      xt_arg->value = (XtArgVal) XmStringCreateLtoR((char*) getstring(lval_resval), XmSTRING_DEFAULT_CHARSET);
      Wres_Free_This_Later((caddr_t) xt_arg->value, XmStringFree); /* XmStrings are copied into widget upon create/setvalues, so this XmString may be freed later */
    }

    /* 
     * otherwise, if lval_resval is a string, we call XtConvert() to convert
     * the resource from a string representaion to the representation type 
     * specified in resource->class->XmR_type.
     */
    else if (stringp(lval_resval)) {
      XrmValue from, to;
      from.size = (unsigned int) getslength(lval_resval);
      from.addr = (caddr_t) getstring(lval_resval);
      XtConvert(widgetID,	/* this is the only thing that uses widgetID */
		XmRString, &from,
		resource->class->XmR_type, &to);
      if (to.addr == NULL) {	/* error if conversion failed */
	sprintf(temptext, 
		"In Widget Arglist, error in converting string-->%s.", 
		resource->class->XmR_type);
	xlerror(temptext, lval_resval);	
      }
      xt_arg->value = *((XtArgVal *) to.addr);

      /*
       * If converted XmRString-->XmR*Pixmap*, note that pixmaps are not copied
       * into the widget, but rather, we must use lisp references and lisp
       * garbage collection to free up unused Pixmap resources
       */
      if (resource->class->LVAL_type == XLTYPE_Pixmap) { /* note: can't do resource->class == &resclass_Pixmap since there are many different kinds of pixmap resource classes */
	/* if string-->pixmap conversion fails, XmGetPixmap() returns XmUNSPECIFIED_PIXMAP */
	if (xt_arg->value == (XtArgVal) XmUNSPECIFIED_PIXMAP) {
	  sprintf(temptext,
		  "In Widget Arglist, error in converting string-->%s.\n    XmGetPixmap() couldn't create a pixmap from given specification.", 
		  resource->class->XmR_type);
	  xlerror(temptext, lval_resval);
	}
#ifdef 0			/* commented out because this causes unexpected X Protocol Error -- see comments below */
	/* pixmap is referenced inside widget, so don't allow pixmap to be
	   garbage collected & XmDestroyPixmap()'d until it is no longer
	   referenced by anybody. See also Cvt_LVAL_to_Pixmap() above */
	Wpm_Set_Pixmap_Reference(cv_pixmap((Pixmap) xt_arg->value), o_widget, lval_resname);
#endif
      }

      /* NOTES ON FREEING SPACE ALLOCATED BY XtConvert() -- attempts at
	 plugging memory leaks inherent in Motif's braindamaged design....:
	 
	 (1) XtConvert(XmRString-->XmRFontList):
	 Don't free fontlist returned by XtConvert() even though fontlists
	 are copied into the widget's internal space. This is because the
	 converter _XmCvtStringToXmFontList() keeps around a static fontlist
	 pointer that gets returned by XtConvert(). Freeing this will cause
	 bogosity upon subsequent accesses to the XmRString-->XmRFontList converter.
	 
	 (2) XtConvert(XmRString-->XmRXmStringTable):
	 Although create/setvalues with a XmRXmStringTable resource will copy
	 the XmStringTable and it's consituent XmStrings into the widget's space,
	 implementation deficiencies in _XmCvtStringToXmStringTable() prevent us
	 from reliably freeing the XmStrings allocated by the converter. This
	 will result in a memory leak that can be avoided by not using
	 XmString-->XmRXmStringTable conversions -- specify a vector of XmStrings
	 in lisp instead. 
	 The problem is that the static array of XmStrings returned by
	 _XmCvtStringToXmStringTable() is not null terminated, and this
	 means that we have no way of knowing how many elements in the
	 stringtable were allocated. Thus, we don't know how many elements
	 can be freed.
	 The current implementation prevents this conversion from occuring,
	 since all XmStringTable arg processing is forced to go through
	 Cvt_LVAL_to_XmRXmStringTable() by virtue of the fact that we
	 specify type FREE. (see code above).
	 
	 (3) XtConvert(XmRString-->XmR*Pixmap*):
	 Pixmaps are not copied onto the widget. Instead, the CvtStringToPixmap()
	 routine ends up calling XmGetPixmap() and passes the pixmap pointer on
	 to the widget. Each call to XmGetPixmap() will increase the reference
	 count on a particular pixmap, and thus we should only call XmDestroyPixmap()
	 every time a pixmap reference is no longer needed.
	 
	 In therory, that should all work fine; In practice, I've been getting 
	 XProtocol errors ("BadDrawable, invalid Pixmap or Window parameter")
	 after using XmRString-->XmR*Pixmap* conversion along with code that forces
	 unreferenced pixmaps to be garbage collected -- this only happens if
	 you end up specifying the same pixmap conversion twice in a row
	 and a garbage collection occurs (which ends up calling XmDestroyPixmap()).
	 I still haven't figured out why this happens. Until I do, the code that
	 causes unused pixmaps to be deallocated has been commented out below so
	 as to prevent such problems. Unfortunatley, this means that pixmaps
	 specified in this way will still consume storage even if they aren't
	 referenced anywere.
	 
	 If you concerned about memory usage, use the XM_GET_PIXMAP primitive
	 to return a pixmap object as the value for a pixmap resource.
	 That primitve will end up forcing unused pixmaps to be destroyed
	 when they are no longer referenced anywhere.
	 
	 (4) Others? -- Accelerators, TranslationTables, Mnemonics, etc?
	 */
    }
    
    else {
      sprintf(temptext,
	      "In Widget Arglist: expected resource value of type %s.",
	      resource->class->XmR_type);
      xlerror(temptext, lval_resval);
    }

    xt_arg++;			/* increment index into <xt_arglist> */
  }

  return(xt_arglist);
}


/******************************************************************************
 * Wres_Get_XtGetValues_ArgList() // Wres_Set_Lisp_Vars_From_GetValues_ArgList()
 * 
 * Wres_Get_XtGetValues_ArgList() will get all remaining arguments off of the
 * argument stack and treat them as a list of name/symbol pairs for arguments
 * to WIDGET_CLASS method :get_values. It returns an ArgList representing
 * the Xtoolkit version of the arglist passed in, which MUST NOT BE FREED
 * BY THE CALLER. XtGetValues() should then be called on the arglist, and
 * then the symbols from the arglist get set to values retrieved from
 * XtGetValues() in proc Wres_Set_Lisp_Vars_From_GetValues_ArgList()
 * 
 *
 * Wres_Get_XtGetValues_ArgList() will signal an error if
 * (1) an invalid resource name is used;
 * (2) lack of C->Lisp converter, or error in convesion.
 *
 * Wres_Get_XtGetValues_ArgList() will return with the argument stack empty,
 * or it will signal an error.
 ******************************************************************************/

typedef struct _GetValues_Info {
  caddr_t data;			/* this MUST be the first field in this struct. 
				   struct _GetValues_Info is pointed to by
				   (XtArgVal).value and this location gets written
				   to by XtGetValues(). XtGetValues() ends up
				   writing data up to sizeof(long)==sizeof(caddr_t)
				   at the location pointed to by (XtArgVal).value.
				   That corresponds to this slot. */
  LVAL    place_for_result;	/* a 'setf' place form for telling
				   Wres_GetValues_ArgList_To_Lisp() where to put
				   getvalues result */
  LVAL    (*resource_val_to_LVAL_converter)(); /* a function that takes 'data' slot and returns it's LISP equivalent */
} GetValues_Info;

#define GETARGLIST_SIZE_INCREMENT 20
static ArgList getarglist;	/* init'd in Wres_Init() */
static Cardinal getarglist_size; /* init'd in Wres_Init() */

ArgList Wres_Get_GetValues_ArgList(numargs)
     Cardinal *numargs;
{
  ArgList             xt_arg;
  Cardinal            argcount;

  if ((xlargc & 1) != 0)	/* if there's an odd number of args on the stack */
    xlerror(":get_values argument list must consist of pairs of resource-name & symbol.\n	An odd number of arguments were found, the first being:",
	    *xlargv);
  
  *numargs = argcount = xlargc / 2; /* the number of name/value pairs */

  if (argcount > getarglist_size) { /* make sure getarglist is big enough */
    getarglist = (ArgList) XtRealloc(getarglist, (unsigned) (argcount * sizeof(Arg)));
    while (getarglist_size < argcount)
      getarglist[getarglist_size++].value = (XtArgVal) XtMalloc(sizeof(GetValues_Info)); /* note bogus jacking in of object larger than XtArgVal for use by Wres_Set_Lisp_Vars_From_GetValues_ArgList() */
    /* xt_arglist_size = argcount; */
  }

  for (xt_arg = getarglist;  (argcount--); xt_arg++) {		/* while more name/value pairs to process */
    Resource_Instance  *resource;
    GetValues_Info     *elt;
    LVAL                lval_resname;

    /* 
     * get resource name
     */
    lval_resname = nextarg();
    if (xtresource_p(lval_resname))
      resource = get_xtresource(lval_resname);
    else if (symbolp(lval_resname)) { /* hasn't been eVALuated yet... */
      lval_resname = getvalue(lval_resname); /* get the value */
      if (xtresource_p(lval_resname))
	resource = get_xtresource(lval_resname);
      else
	xlerror("Invalid resource keyword in widget :get_values resource list.", lval_resname);
    }
    else
      xlerror("Invalid resource name in widget :get_values resource list.", lval_resname);
    xt_arg->name = resource->name;

    elt = (GetValues_Info *) xt_arg->value;

    /* 
     * clear out 'data' since XtGetValues() won't clear entire word 
     * for resources shorter than a 'long'. This could lead to retrieval
     * of bogus data in lisp. (Not necessary if all coercions are correct,
     * but this will work around bugs in misdeclared motif resources...
     */
    elt->data = (caddr_t) 0L;

    /*
     * get symbol to set value to
     */
    elt->place_for_result = xlgetarg();	/* get an place to put val */
    if (!( null(elt->place_for_result) || symbolp(elt->place_for_result) || consp(elt->place_for_result) ))
      xlerror(":get_values expected a <place> form, either NIL, a symbol, or a place-form.", elt->place_for_result);
    
    /* 
     * Get the resource-->LVAL converter suggested by the resource-class.
     * This converter will be called after XtGetValues() is done by
     * calling Wres_Set_Lisp_Vars_From_GetValues_ArgList().
     */
    elt->resource_val_to_LVAL_converter = resource->class->resource_val_to_LVAL_converter;
  }
  return (getarglist);
}


LVAL Wres_GetValues_ArgList_To_Lisp(arglist, numargs)
     ArgList arglist;
     Cardinal numargs;
{
  GetValues_Info *elt;
  LVAL result,cur,next = NIL;
  xlstkcheck(2);
  xlsave(result);
  xlsave(next);
    
  while (numargs--) {
    elt = (GetValues_Info *) (*arglist++).value;

    /* Convert elt->data (set by XtGetValues) to a lisp value and set that
       value to elt->place_for_result. depending on the resource type, elt->data
       may be a pointer to more data (eg. a string) or an immediate value
       (eg, an integer). The converter pointed to by
       (*(elt->resource_val_to_LVAL_converter))() will know how to convert
       this data to a lisp value. */

    if (!(elt->place_for_result))	{ /* if NIL, then return the value as function's result */
      next = cons((*(elt->resource_val_to_LVAL_converter))(elt->data), NIL);
      if (result) {
	rplacd(cur, next);
	cur = next;
      }
      else
	result = cur = next;
    }
    else			/* check the place form -- stolen from xsetf() */
      if (symbolp(elt->place_for_result))
	xlsetvalue(elt->place_for_result, (*(elt->resource_val_to_LVAL_converter))(elt->data));
      else if (consp(elt->place_for_result))
	placeform(elt->place_for_result, (*(elt->resource_val_to_LVAL_converter))(elt->data));
      else
	xlfail("bad place form");
  }
  
  xlpopn(2);
  return (result);
}


/*****************************************************************************
 * This accesses the symbol value of a resource instance object (whose type
 * is opaque outside of this module). It is used in xlprint.c:xlprint().
 *
 * It is assumed that the parameter res's type is such that
 * ntype(res) == XLTYPE_XT_RESOURCE.
 ****************************************************************************/
LVAL Wres_Get_Symbol(res)
     LVAL res;
{
  return (get_xtresource(res)->symbol);
}

/*****************************************************************************
 * This accesses the XmN* name of a resource instance object (whose type
 * is opaque outside of this module). 
 *
 * It is assumed that the parameter res's type is such that
 * ntype(res) == XLTYPE_XT_RESOURCE.
 ****************************************************************************/
char* Wres_Get_Name(res)
     LVAL res;
{
  return (get_xtresource(res)->name);
}

/******************************************************************************
 * It is assumed that the parameter res's type is such that
 * ntype(res) == XLTYPE_XT_RESOURCE.
 ******************************************************************************/ 
Boolean Wres_Is_Callback_P(res)
     LVAL res;
{
  return (get_xtresource(res)->class == &resclass_XmRCallback);
}


/*****************************************************************************
 * This initializes the resource symbols in xlisp. It is called in main().
 ****************************************************************************/
Wres_Init()
{

  /* initialize resource symbols and values */
  {
    register Resource_Instance *resource;
    for (resource = &(resource_table[0]); resource->printname; resource++)
      setvalue((resource->symbol = xlenter(resource->printname)), 
	       cv_xtresource(resource));
  }

  /* set up storage space for Wres_Get_LispArglist() */
  xt_arglist = (ArgList) XtMalloc((unsigned) (XTARGLIST_SIZE_INCREMENT * sizeof(Arg)));
  xt_arglist_size = XTARGLIST_SIZE_INCREMENT;

  /* set up storage space used by Wres_Free_C_Arglist_Data() and Wres_Free_This_Later() */
  freeables_size = FREEABLES_SIZE_INCREMENT;
  freeables_end_idx = 0;
  freeables = (struct Argval_Deallocator_Pair *) XtMalloc((unsigned) (FREEABLES_SIZE_INCREMENT * sizeof(struct Argval_Deallocator_Pair)));

  /* set up storage space for Cvt_LVAL_to_XmRXmStringTable() */
  xm_stringtable_size = XMSTRINGTABLE_SIZE_INCREMENT;
  xm_stringtable = (XmString *) XtMalloc((unsigned) (XMSTRINGTABLE_SIZE_INCREMENT * sizeof(XmString)));

  /* set up storage space for Wres_Get_GetValues_ArgList() and Wres_Set_Lisp_Vars_From_GetValues_ArgList() */
  getarglist = (ArgList) XtMalloc((unsigned) (GETARGLIST_SIZE_INCREMENT * sizeof(Arg))); 
  getarglist_size = 0;
  while (getarglist_size < GETARGLIST_SIZE_INCREMENT)
    getarglist[getarglist_size++].value = (XtArgVal) XtMalloc(sizeof(GetValues_Info)); /* note bogus jacking in of object larger than XtArgVal for use by Wres_Set_Lisp_Vars_From_GetValues_ArgList() */
  /* getarglist_size = GETARGLIST_SIZE_INCREMENT */

  /* initialize keyword symbols... */
  s_XmALIGNMENT_BEGINNING	= xlenter(":ALIGNMENT_BEGINNING");
  s_XmALIGNMENT_CENTER		= xlenter(":ALIGNMENT_CENTER");
  s_XmALIGNMENT_END		= xlenter(":ALIGNMENT_END");
  s_XmN_OF_MANY			= xlenter(":N_OF_MANY");
  s_XmONE_OF_MANY		= xlenter(":ONE_OF_MANY");
  s_XmMULTI_LINE_EDIT		= xlenter(":MULTI_LINE_EDIT");
  s_XmSINGLE_LINE_EDIT		= xlenter(":SINGLE_LINE_EDIT");
  s_XmSTRING			= xlenter(":STRING");
  s_XmPIXMAP			= xlenter(":PIXMAP");
  s_XmNO_ORIENTATION		= xlenter(":NO_ORIENTATION");
  s_XmVERTICAL			= xlenter(":VERTICAL");
  s_XmHORIZONTAL		= xlenter(":HORIZONTAL");
  s_XmNO_PACKING		= xlenter(":NO_PACKING");
  s_XmPACK_TIGHT		= xlenter(":PACK_TIGHT");
  s_XmPACK_COLUMN		= xlenter(":PACK_COLUMN");
  s_XmPACK_NONE			= xlenter(":PACK_NONE");
  s_XmWORK_AREA			= xlenter(":WORK_AREA");
  s_XmMENU_BAR			= xlenter(":MENU_BAR");
  s_XmMENU_POPUP		= xlenter(":MENU_POPUP");
  s_XmMENU_PULLDOWN		= xlenter(":MENU_PULLDOWN");
  s_XmMENU_OPTION		= xlenter(":MENU_OPTION");
  s_XmSTRING_DIRECTION_L_TO_R	= xlenter(":STRING_DIRECTION_L_TO_R");
  s_XmSTRING_DIRECTION_R_TO_L	= xlenter(":STRING_DIRECTION_R_TO_L");
  s_Button1			= xlenter(":BUTTON1");
  s_Button2			= xlenter(":BUTTON2");
  s_Button3			= xlenter(":BUTTON3");
  s_Button4			= xlenter(":BUTTON4");
  s_Button5			= xlenter(":BUTTON5");
  s_XmPIXELS			= xlenter(":PIXELS");
  s_Xm100TH_MILLIMETERS		= xlenter(":100TH_MILLIMETERS");
  s_Xm1000TH_INCHES		= xlenter(":1000TH_INCHES");
  s_Xm100TH_POINTS		= xlenter(":100TH_POINTS");
  s_Xm100TH_FONT_UNITS		= xlenter(":100TH_FONT_UNITS");
  s_XmRESIZE_NONE		= xlenter(":RESIZE_NONE");
  s_XmRESIZE_GROW		= xlenter(":RESIZE_GROW");
  s_XmRESIZE_ANY		= xlenter(":RESIZE_ANY");
  s_XmATTACH_NONE		= xlenter(":ATTACH_NONE");
  s_XmATTACH_FORM		= xlenter(":ATTACH_FORM");
  s_XmATTACH_OPPOSITE_FORM 	= xlenter(":ATTACH_OPPOSITE_FORM ");
  s_XmATTACH_WIDGET		= xlenter(":ATTACH_WIDGET");
  s_XmATTACH_OPPOSITE_WIDGET	= xlenter(":ATTACH_OPPOSITE_WIDGET");
  s_XmATTACH_POSITION		= xlenter(":ATTACH_POSITION");
  s_XmATTACH_SELF		= xlenter(":ATTACH_SELF");
  s_XmSHADOW_ETCHED_IN		= xlenter(":SHADOW_ETCHED_IN");
  s_XmSHADOW_ETCHED_OUT		= xlenter(":SHADOW_ETCHED_OUT");
  s_XmSHADOW_IN			= xlenter(":SHADOW_IN");
  s_XmSHADOW_OUT		= xlenter(":SHADOW_OUT");
  s_XmARROW_UP			= xlenter(":ARROW_UP");
  s_XmARROW_DOWN		= xlenter(":ARROW_DOWN");
  s_XmARROW_LEFT		= xlenter(":ARROW_LEFT");
  s_XmARROW_RIGHT		= xlenter(":ARROW_RIGHT");
  s_XmNO_LINE			= xlenter(":NO_LINE");
  s_XmSINGLE_LINE		= xlenter(":SINGLE_LINE");
  s_XmDOUBLE_LINE		= xlenter(":DOUBLE_LINE");
  s_XmSINGLE_DASHED_LINE	= xlenter(":SINGLE_DASHED_LINE");
  s_XmDOUBLE_DASHED_LINE	= xlenter(":DOUBLE_DASHED_LINE");
  s_XmMAX_ON_TOP		= xlenter(":MAX_ON_TOP");
  s_XmMAX_ON_BOTTOM		= xlenter(":MAX_ON_BOTTOM");
  s_XmMAX_ON_LEFT		= xlenter(":MAX_ON_LEFT");
  s_XmMAX_ON_RIGHT		= xlenter(":MAX_ON_RIGHT");
  s_XmSINGLE_SELECT		= xlenter(":SINGLE_SELECT");
  s_XmMULTIPLE_SELECT		= xlenter(":MULTIPLE_SELECT");
  s_XmEXTENDED_SELECT		= xlenter(":EXTENDED_SELECT");
  s_XmBROWSE_SELECT		= xlenter(":BROWSE_SELECT");
  s_XmVARIABLE			= xlenter(":VARIABLE");
  s_XmCONSTANT			= xlenter(":CONSTANT");
  s_XmRESIZE_IF_POSSIBLE	= xlenter(":RESIZE_IF_POSSIBLE");
  s_XmAUTOMATIC			= xlenter(":AUTOMATIC");
  s_XmAPPLICATION_DEFINED	= xlenter(":APPLICATION_DEFINED");
  s_XmSTATIC			= xlenter(":STATIC");
  s_XmAS_NEEDED			= xlenter(":AS_NEEDED");
  s_XmTOP_LEFT			= xlenter(":TOP_LEFT");
  s_XmBOTTOM_LEFT		= xlenter(":BOTTOM_LEFT");
  s_XmTOP_RIGHT			= xlenter(":TOP_RIGHT");
  s_XmBOTTOM_RIGHT		= xlenter(":BOTTOM_RIGHT");
  s_XmDIALOG_WORK_AREA		= xlenter(":DIALOG_WORK_AREA");
  s_XmDIALOG_MODELESS		= xlenter(":DIALOG_MODELESS");
  s_XmDIALOG_APPLICATION_MODAL	= xlenter(":DIALOG_APPLICATION_MODAL");
  s_XmDIALOG_SYSTEM_MODAL	= xlenter(":DIALOG_SYSTEM_MODAL");
  s_XmDIALOG_ERROR		= xlenter(":DIALOG_ERROR");
  s_XmDIALOG_INFORMATION	= xlenter(":DIALOG_INFORMATION");
  s_XmDIALOG_MESSAGE		= xlenter(":DIALOG_MESSAGE");
  s_XmDIALOG_QUESTION		= xlenter(":DIALOG_QUESTION");
  s_XmDIALOG_WARNING		= xlenter(":DIALOG_WARNING");
  s_XmDIALOG_WORKING		= xlenter(":DIALOG_WORKING");
  s_XmDIALOG_PROMPT		= xlenter(":DIALOG_PROMPT");
  s_XmDIALOG_SELECTION		= xlenter(":DIALOG_SELECTION");
  s_XmDIALOG_COMMAND		= xlenter(":DIALOG_COMMAND");
  s_XmDESTROY                   = xlenter(":DESTROY");
  s_XmUNMAP                     = xlenter(":UNMAP");
  s_XmDO_NOTHING                = xlenter(":DO_NOTHING");
  s_XmEXPLICIT                  = xlenter(":EXPLICIT");
  s_XmPOINTER                   = xlenter(":POINTER");
  s_XmDIALOG_NONE		= xlenter(":DIALOG_NONE");
  s_XmDIALOG_APPLY_BUTTON	= xlenter(":DIALOG_APPLY_BUTTON");
  s_XmDIALOG_CANCEL_BUTTON	= xlenter(":DIALOG_CANCEL_BUTTON");
  s_XmDIALOG_DEFAULT_BUTTON	= xlenter(":DIALOG_DEFAULT_BUTTON");
  s_XmDIALOG_OK_BUTTON		= xlenter(":DIALOG_OK_BUTTON");
  s_XmDIALOG_FILTER_LABEL	= xlenter(":DIALOG_FILTER_LABEL");
  s_XmDIALOG_FILTER_TEXT	= xlenter(":DIALOG_FILTER_TEXT");
  s_XmDIALOG_HELP_BUTTON	= xlenter(":DIALOG_HELP_BUTTON");
  s_XmDIALOG_LIST		= xlenter(":DIALOG_LIST");
  s_XmDIALOG_HISTORY_LIST	= xlenter(":DIALOG_HISTORY_LIST");
  s_XmDIALOG_LIST_LABEL		= xlenter(":DIALOG_LIST_LABEL");
  s_XmDIALOG_MESSAGE_LABEL	= xlenter(":DIALOG_MESSAGE_LABEL");
  s_XmDIALOG_SELECTION_LABEL	= xlenter(":DIALOG_SELECTION_LABEL");
  s_XmDIALOG_PROMPT_LABEL	= xlenter(":DIALOG_PROMPT_LABEL");
  s_XmDIALOG_SYMBOL_LABEL	= xlenter(":DIALOG_SYMBOL_LABEL");
  s_XmDIALOG_TEXT		= xlenter(":DIALOG_TEXT");
  s_XmDIALOG_VALUE_TEXT		= xlenter(":DIALOG_VALUE_TEXT");
  s_XmDIALOG_COMMAND_TEXT	= xlenter(":DIALOG_COMMAND_TEXT");
  s_XmDIALOG_SEPARATOR		= xlenter(":DIALOG_SEPARATOR");
  s_XmDIALOG_FILE_SELECTION	= xlenter(":DIALOG_FILE_SELECTION");
}
