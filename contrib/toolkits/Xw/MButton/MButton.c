/*************************************<+>*************************************
 *****************************************************************************
 **
 **   File:        MButton.c
 **
 **   Project:     X Widgets
 **
 **   Description: Contains code for primitive widget class: MultiButton
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

/*
 * Include files & Static Routine Definitions
 */

#include <stdio.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <X11/Misc.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include "MButtonP.h"
#include "MButton.h"

#define MB_BW 2


static void Redisplay();
static void RedrawButtonFace();
static Boolean SetValues();
static void ClassInitialize();
static void Initialize();
static void Select();
static void Rotate();
static void Resize();
static Boolean RecomputeSize();
static void Destroy();
static int HandleLabelAllocation();


/*************************************<->*************************************
 *
 *
 *   Description:  default translation table for class: MultiButton
 *   -----------
 *
 *   Matches events with string descriptors for internal routines.
 *
 *************************************<->***********************************/

static char defaultTranslations[] =
   "<Btn1Down>:             select() \n\
    <Btn1Up>:               rotate() \n\
    <EnterWindow>:          enter() \n\
    <LeaveWindow>:          leave() \n\
    <KeyUp>0xFF60:          rotate() \n\
    <KeyDown>0xFF60:       select()";



/*************************************<->*************************************
 *
 *
 *   Description:  action list for class: MultiButton
 *   -----------
 *
 *   Matches string descriptors with internal routines.
 *   Note that Primitive will register additional event handlers
 *   for traversal.
 *
 *************************************<->***********************************/

static XtActionsRec actionsList[] =
{
  {"select", (XtActionProc) Select},
  {"rotate", (XtActionProc) Rotate},
  {"enter", (XtActionProc) _XwPrimitiveEnter},
  {"leave", (XtActionProc) _XwPrimitiveLeave},
};


/*  The resource list for MultiButton  */

static XtResource resources[] = 
{     
   {
     XtNlabels, XtCLabels, XtRLabels, sizeof (char **),
     XtOffset (XwMultiButtonWidget, multi_button.labels),
     XtRString, (caddr_t) NULL
   },

   {
     XtNnumLabels, XtCNumLabels, XtRInt, sizeof (int),
     XtOffset (XwMultiButtonWidget, multi_button.num_labels),
     XtRString, "0"
   },

   {
     XtNinvertOnSelect, XtCInvertOnSelect, XtRBoolean, sizeof (Boolean),
     XtOffset (XwMultiButtonWidget, multi_button.invert_on_select),
     XtRString, "True"
   },

};



/*************************************<->*************************************
 *
 *
 *   Description:  global class record for instances of class: MultiButton
 *   -----------
 *
 *   Defines default field settings for this class record.
 *
 *************************************<->***********************************/

XwMultiButtonClassRec XwmultiButtonClassRec = {
  {
/* core_class fields */	
    /* superclass	  */	(WidgetClass) &XwbuttonClassRec,
    /* class_name	  */	"MultiButton",
    /* widget_size	  */	sizeof(XwMultiButtonRec),
    /* class_initialize   */    ClassInitialize,
    /* class_part_init    */    NULL,				
    /* class_inited       */	FALSE,
    /* initialize	  */	Initialize,
    /* initialize_hook    */    NULL,
    /* realize		  */	_XwRealize,
    /* actions		  */	actionsList,
    /* num_actions	  */	XtNumber(actionsList),
    /* resources	  */	resources,
    /* num_resources	  */	XtNumber(resources),
    /* xrm_class	  */	NULLQUARK,
    /* compress_motion	  */	TRUE,
    /* compress_exposure  */	TRUE,
    /* compress_enterlv   */    TRUE,
    /* visible_interest	  */	FALSE,
    /* destroy		  */    Destroy,
    /* resize		  */	Resize,
    /* expose		  */	Redisplay,
    /* set_values	  */	SetValues,
    /* set_values_hook    */    NULL,
    /* set_values_almost  */    XtInheritSetValuesAlmost,
    /* get_values_hook  */	NULL,
    /* accept_focus	  */	NULL,
    /* version          */	XtVersion,
    /* callback_private */      NULL,
    /* tm_table         */      defaultTranslations,
    /* query_geometry   */	NULL, 
  }
};
WidgetClass XwmultiButtonWidgetClass = (WidgetClass)&XwmultiButtonClassRec;


/*************************************<->*************************************
 *
 *  Select (w, event)  
 *
 *   Description:
 *   -----------
 *     Invert or change highlight (depending on setting of shadow_on
 *     flag.  Issue any select callbacks and give them the current
 *     state value.
 *
 *   Inputs:
 *   ------
 *     w           =   widget instance that was selected.
 *     event       =   event record
 * 
 *************************************<->***********************************/

static void Select (w,event)
Widget w;
XEvent *event;

{
   XwMultiButtonWidget mb = (XwMultiButtonWidget) w;

   mb->button.set = TRUE;

   RedrawButtonFace (w, event, FALSE);
   XFlush (XtDisplay(w));
   XtCallCallbacks (w, XtNselect, (caddr_t) mb->multi_button.state);

}


/*************************************<->*************************************
 *
 *  Rotate (w, event) 
 *
 *   Description:
 *   -----------
 *     Mark button as not set, rotate label (if there are any to
 *     rotate) Generate unselect callbacks and give them the new
 *     state.
 *
 *   Inputs:
 *   ------
 *     w           =   widget instance that was selected.
 *     event       =   event record
 * 
 *************************************<->***********************************/

static void Rotate(w,event)
Widget w;
XEvent *event;

{
   XwMultiButtonWidget mb = (XwMultiButtonWidget) w;
   int newState = (mb->multi_button.state+1) % mb->multi_button.num_labels;

   mb->button.set = FALSE;
   mb->button.label= mb->multi_button.labels[newState];
   mb->multi_button.state = newState;
   _XwSetTextWidthAndHeight(mb);
   Resize(w);
  
   RedrawButtonFace (w, event, FALSE);
   XFlush(XtDisplay(w));
   XtCallCallbacks (w, XtNrelease, (caddr_t) newState);
}


/*************************************<->*************************************
 *
 *  Initialize 
 *
 *   Description:
 *   -----------
 *    If the core height and width fields are set to 0, treat that as a flag
 *    and compute the optimum size for this button.  Then using what ever
 *    the core fields are set to, compute the text placement fields.
 *    Make sure that the label location field is properly set for the
 *    Resize call.
 *
 *
 *   Inputs:
 *   ------
 *     request		=	request widget, old data.
 *
 *     new		=	new widget, new data; cumulative effect
 *				of initialize procedures.
 *
 *************************************<->***********************************/
static void Initialize (request, new)
Widget request, new;

{
   XwMultiButtonWidget mb = (XwMultiButtonWidget) new;
   int maxWidth = 0;

   /******************************************************************** 
       Needed width:
        2 * highlight thickness +
        2 * internal width (padding between label and button) +
        Max(pixel width of labels)

      Needed height:
       2 * highlight thickness +
       2 * internal height (padding) +
       label height
    
    ************************************************************************/


    maxWidth = HandleLabelAllocation(mb);

   if (request->core.width == 0)  mb->core.width =  maxWidth +
	      2 * ( mb->button.internal_width +    /* white space */
		     mb->primitive.highlight_thickness + MB_BW);
    
   if (request->core.height == 0) mb->core.height = mb->button.label_height + 
       2 * (mb->button.internal_height + mb->primitive.highlight_thickness
            + MB_BW);
    
   Resize(new);
}


/*************************************<->*************************************
 *
 *  CvtLabelsToPointer
 *
 *   Description:
 *   -----------
 *     Convert a string containing button labels into an array of pointers
 *     to a  sequence of  labels.  Labels appear in the input string
 *     surrounded by double quotes.
 *
 *     NOTE that this routine will not handle more thatn 20 labels
 *     or more than 400 characters.
 *
 *
 *   Inputs:
 *   ------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 * 
 *   Outputs:
 *   -------
 *     xxxxxxxxxxxx = xxxxxxxxxxxxx
 *
 *   Procedures Called
 *   -----------------
 *
 *************************************<->***********************************/

#define MAXLABELS 20
#define MAXCHARS  400

static char storage[MAXCHARS];
static char * labels[MAXLABELS];
static char * labelsPtr = (char *) labels;


static void CvtLabelsToPointer(args, num_args, fromVal, toVal)
       XrmValue * args;
       int * num_args;
       XrmValue * fromVal;
       XrmValue * toVal;
{

       char * instr = (char *) (fromVal->addr);
       char * str_pos = storage;
       int i;

   /* Fail Safe: in case we get garbage return NULL */

       (*toVal).size = sizeof (char **);
       (*toVal).addr = (char *) &labelsPtr;
       for (i=0; i<MAXLABELS; i++) labels[i]=NULL;

       i=0;

      /* We'll only look for MAXLABELS */
       while (i < MAXLABELS)
	 {
           /* Find beginning of label or end of input */
           while(*instr != '\0' && *instr != '"') instr++;

           if (*instr == '\0')    return;

           instr++;
           labels[i] = str_pos;

           /* Move string into storage space*/
           while (*instr != '"' && *instr != '\0')
              *str_pos++ = *instr++;
           
           if (*instr == '\0')
               XtError("Improper definition for MultiButton labels resource.");

          /* Append null to end of string, step beyond '"' marking end
           * of this label, increment "i" our label counter.
           */
           *str_pos++ = '\0';   
           instr++;         
           i++;

         }

}



/*************************************<->*************************************
 *
 *  ClassInitialize
 *
 *   Description:
 *   -----------
 *    Set fields in primitive class part of our class record so that
 *    the traversal code can invoke our button select/unselect procedures.
 *    Register specialized resource converter for this widget class.
 *
 *************************************<->***********************************/

static void ClassInitialize()
{
   XwmultiButtonClassRec.primitive_class.select_proc = (XtWidgetProc) Select;
   XwmultiButtonClassRec.primitive_class.release_proc = (XtWidgetProc) Rotate;
   XtAddConverter(XtRString, XtRLabels, CvtLabelsToPointer, NULL, 0);
}



/*************************************<->*************************************
 *
 *  Redisplay (w, event)
 *
 *   Description:
 *   -----------
 *     Cause the widget, identified by w, to be redisplayed.
 *
 *
 *   Inputs:
 *   ------
 *     w = widget to be redisplayed;
 *     event = event structure identifying need for redisplay on this
 *             widget.
 * 
 *************************************<->***********************************/

static void Redisplay (w, event)
Widget w;
XEvent *event;

{
    RedrawButtonFace (w, event, TRUE);
}


static void RedrawButtonFace (w, event, all)
XwMultiButtonWidget w;
XEvent *event;
Boolean all;

{
   register XwMultiButtonWidget mb = (XwMultiButtonWidget) w;
   int available_height, available_width;
   Boolean clipHeight, clipWidth;


   /* COMPUTE SPACE AVAILABLE FOR DRAWING LABEL */

   available_width = Max(0,mb->core.width - 2*(mb->button.internal_width +
				mb->primitive.highlight_thickness + MB_BW));

   available_height = Max(0, mb->core.height - 2*(mb->button.internal_height +
				mb->primitive.highlight_thickness + MB_BW));

   
   /* SEE IF WE NEED TO CLIP THIS LABEL ON TOP AND/OR BOTTOM */

   if (mb->button.label_width > available_width)
       clipWidth = True;
   else 
       clipWidth = False;


   if (mb->button.label_height > available_height)
       clipHeight = True;
   else 
       clipHeight = False;



   /* COMPUTE & DRAW MULTIBUTTON */



   /* COMPUTE x LOCATION FOR STRING & DRAW STRING                    */


   if (mb->button.label_len > 0)
   {
    XFillRectangle (XtDisplay(w), XtWindow(w),
		         ((mb->button.set && mb->multi_button.invert_on_select)
                                     ? mb->button.normal_GC
		                     : mb->button.inverse_GC),
                      w -> primitive.highlight_thickness + 1 + MB_BW,
                      w -> primitive.highlight_thickness + 1 + MB_BW,
                      w->core.width- 2 * (w->primitive.highlight_thickness
                                          + 1 + MB_BW),
                      w->core.height-2 * (w->primitive.highlight_thickness
                                          + 1 + MB_BW));

      XDrawString(XtDisplay(w), XtWindow(w),
		   ((mb->button.set && mb->multi_button.invert_on_select)
                                     ? mb->button.inverse_GC
		                     : mb->button.normal_GC),
                   ((mb->core.width + 1 - mb->button.label_width) / 2),
	            mb->button.label_y,  mb->button.label, 
		    mb->button.label_len);


     if (clipWidth)
       {
         XClearArea (XtDisplay(w), XtWindow(w), 0,0,
		     (mb->primitive.highlight_thickness + MB_BW +
		       mb->button.internal_width), mb->core.height, FALSE);

         XClearArea (XtDisplay(w), XtWindow(w), 
		     (mb->core.width - mb->primitive.highlight_thickness -
		       MB_BW - mb->button.internal_width),0,
		     (mb->primitive.highlight_thickness + MB_BW +
		       mb->button.internal_width), mb->core.height, FALSE);
       }
   
     if (clipHeight)
       {
         XClearArea (XtDisplay(w), XtWindow(w), 0,0, mb->core.width, 
		     (mb->primitive.highlight_thickness + MB_BW +
		       mb->button.internal_height), FALSE);
         XClearArea (XtDisplay(w), XtWindow(w), 0,
		     (mb->core.height - mb->primitive.highlight_thickness -
		       MB_BW - mb->button.internal_height), mb->core.width,
		     (mb->primitive.highlight_thickness + MB_BW +
		       mb->button.internal_height), FALSE);
       }
   }



   _XwDrawBox (XtDisplay (w), XtWindow (w), 
                  w -> button.normal_GC, MB_BW,
                  w -> primitive.highlight_thickness,
                  w -> primitive.highlight_thickness,
                  w->core.width - 2 * w->primitive.highlight_thickness,
                  w->core.height - 2 * w->primitive.highlight_thickness);


   /*  
    * Draw traversal/enter highlight if actual exposure or
    * if we had to clip text area
    */

   if (all || clipWidth || clipHeight) 
   {
      if (mb->primitive.highlighted)
         _XwHighlightBorder(w);
      else if (mb->primitive.display_highlighted) 
         _XwUnhighlightBorder(w);
   }
}



/*************************************<->*************************************
 *
 *  SetValues(current, request, new)
 *
 *   Description:
 *   -----------
 *     This is the set values procedure for the multi_button class.  It is
 *     called last (the set values rtnes for its superclasses are called
 *     first).
 *
 *
 *   Inputs:
 *   ------
 *    current = original widget;
 *    request = copy of current (?);
 *    new = copy of request which reflects changes made to it by
 *          set values procedures of its superclasses;
 *
 *************************************<->***********************************/

static Boolean SetValues(current, request, new)
Widget current, request, new;

{
    XwMultiButtonWidget curmb = (XwMultiButtonWidget) current;
    XwMultiButtonWidget newmb = (XwMultiButtonWidget) new;
    Boolean  flag = FALSE;    /* our return value */
    int maxWidth = -1;
    XFontStruct  *fs = newmb->button.font;
    int i;
    

   /* Validate fields unique to MultiButton */    

    if (curmb->multi_button.num_labels != newmb->multi_button.num_labels)
      {
        /* Reset state to 0 */
        newmb->multi_button.state = 0;

       /* Don't allow new value to be illegal */
        if (newmb->multi_button.num_labels < 1)
                 newmb->multi_button.num_labels = 1;

      }


    if (curmb->multi_button.labels != newmb->multi_button.labels)
      {
       Destroy(curmb);   /* free up pointers and strings */
       maxWidth = HandleLabelAllocation(newmb);
       flag = TRUE;
      }
    
    /**********************************************************************
     * Calculate the window size:  The assumption here is that if
     * the width and height are the same in the new and current instance
     * record that those fields were not changed with set values.  Therefore
     * its okay to recompute the necessary width and height.  However, if
     * the new and current do have different width/heights then leave them
     * alone because that's what the user wants.  Also, use the
     * RecomputeSize procedure (defined below) to test if we should
     * recompute the size.
     *********************************************************************/
    if ((curmb->core.width == newmb->core.width) &&
         (RecomputeSize(current, new)))
     {
       if (maxWidth < 0)
          for (i=0; i<newmb->multi_button.num_labels; i++)
              maxWidth = Max(maxWidth, 
			     XTextWidth(fs, newmb->multi_button.labels[i]));

	newmb->core.width = maxWidth +  2*(newmb->button.internal_width +
		   newmb->primitive.highlight_thickness + MB_BW);


	flag = TRUE;
     }

    if ((curmb->core.height == newmb->core.height) &&
         (RecomputeSize(current, new)))
     {
	newmb->core.height =
	    newmb->button.label_height + 2*(newmb->button.internal_height +
     		    newmb->primitive.highlight_thickness + MB_BW);


	flag = TRUE;
     }

   return(flag);
}




/*************************************<->*************************************
 *
 *  Resize(w)
 *
 *   Description:
 *   -----------
 *     Recompute location of button text (center text in the button
 *     face).
 *
 *   Inputs:
 *   ------
 *     w  = widget to be resized.
 * 
 *
 *************************************<->***********************************/

static void Resize(w)
    Widget w;
{
    XwMultiButtonWidget mb = (XwMultiButtonWidget) w;

    mb->button.label_x = (mb->core.width + 1 - mb->button.label_width) / 2;

    mb->button.label_y =
       (mb->core.height - mb->button.label_height) / 2
	+ mb->button.font->max_bounds.ascent;
}



/*************************************<->*************************************
 *  Boolean
 *  RecomputeSize(current, new)
 *
 *   Description:
 *   -----------
 *     Used during SetValues.
 *
 *     If the font has changed OR the label has changed OR
 *     the internal spacing has changed OR the highlight
 *     thickness has changed AND the recompute flag is TRUE
 *     (in the new widget) return TRUE, else return FALSE.
 *
 *
 *   Inputs:
 *   ------
 *    current = current version of widget
 *    new = new version of widget
 * 
 *   Outputs:
 *   -------
 *     TRUE if resize is needed and okay, FALSE otherwise.
 *
 *************************************<->***********************************/
static  Boolean RecomputeSize(current, new)
  XwButtonWidget current, new;
{
  if (((new->button.font != current->button.font) ||
     (new->button.label != current->button.label) ||
     (new->primitive.highlight_thickness != 
         current->primitive.highlight_thickness) ||
     (new->button.internal_height != current->button.internal_height) ||
     (new->button.internal_width != current->button.internal_width)) &&
     (new->primitive.recompute_size == TRUE))
         return(TRUE);
  else
         return(FALSE);
}



/*************************************<->*************************************
 *
 *  Destroy (mb)
 *
 *   Description:
 *   -----------
 *     Free up the memory allocated especially for the 
 *     multibutton part of the widget instance record.
 *
 *
 *   Inputs:
 *   ------
 *     mb = multibutton widget.
 * 
 *************************************<->***********************************/
static void Destroy(mb)
  XwMultiButtonWidget mb;
{
   int i;

   /* Free each of the labels */
   for (i=0; i < mb->multi_button.num_labels; i++)
     XtFree(mb->multi_button.labels[i]);

  /* Free the array of pointers to the labels */
   XtFree((char *)mb->multi_button.labels);

}


/*************************************<->*************************************
 *  int
 *  HandleLabelAllocation(mb)
 *
 *   Description:
 *   -----------
 *     If no labels have been provided, use the button name as 
 *     the single label.  Allocate an array of pointers to strings
 *     to hold pointer to button labels; then allocate space for
 *     each button label and copy label to this space.  Compute
 *     the widest (in pixels) label and return this to figure to
 *     the caller (it will be used to compute the optimum width 
 *     for the button).  Set the multibutton state to 0 and put
 *     a pointer to the corresponding label into button.label.
 *
 *
 *   Inputs:
 *   ------
 *     mb = multibutton whose labels are to be allocated.
 * 
 *   Outputs:
 *   -------
 *     returns width (in pixels) of widest label.
 *
 *************************************<->***********************************/
static int HandleLabelAllocation(mb)
   XwMultiButtonWidget mb;
{
   String * labels;
   int i;
   int maxWidth = 0;
   XFontStruct  *fs = mb->button.font;

   /* If user has not given us any labels then try to use
    * widget's name as a label (of course this too could be NULL but
    * it still shouldn't hurt us).
    */
   if (mb->multi_button.labels == NULL)
       mb->multi_button.labels = &(mb->core.name);

   /* There is ALWAYS one label, even if that label is NULL (i.e., the
    * application provides no names and the button's name is NULL).
    */
   if (mb->multi_button.num_labels < 1)
       mb->multi_button.num_labels = 1;

   /* Allocate array of label pointers */
   labels = (String *) XtMalloc(sizeof(char *)* mb->multi_button.num_labels);


   /* Allocate space for each label and copy pointer to new string
    * into array allocated above. Also compute, in pixels, width of
    * widest label.
    */
   for (i=0; i< mb->multi_button.num_labels; i++)
     {
       labels[i]= (char *)strcpy( 
			      XtMalloc((unsigned)mb->multi_button.labels[i] + 1),
			       mb->multi_button.labels[i]);
       maxWidth = Max(maxWidth, XTextWidth(fs,labels[i], XwStrlen(labels[i])));
     }


   /* Now put correct label, set state and compute label location. */
   mb->multi_button.labels = labels;
   mb->button.label = labels[0];
   mb->multi_button.state = 0;
   _XwSetTextWidthAndHeight(mb);

  /* Return width (in pixels) of widest label */  
   return(maxWidth);
 }
