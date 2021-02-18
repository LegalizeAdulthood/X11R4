/**************************************************************
  This is a simple program that creates a bulletin board manager
  which contains a static text label and a multibutton.
  
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
***************************************************************/

#include <stdio.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <Xw/Xw.h>
#include <Xw/SText.h>
#include <Xw/BBoard.h>
#include "MButton.h"
#include <Xw/PButton.h>
#include <X11/Xresource.h>

Widget bbox, colorbox, mbutton;
Widget toplevel, outer_box, pbutton;
Arg myArgs[10];
Pixel color;
static char * moreLabels[] =
      { "Truly Red", "pyBlack", "A Strange Green"};


Pixel CvtStringToPixel(src_string)
    char *  src_string;
{ 
    XColor aColor, bColor;   

    XAllocNamedColor(XtDisplay(toplevel),
		  DefaultColormapOfScreen(XtScreen(toplevel)), 
		   src_string, &aColor, &bColor);

    return(aColor.pixel);
}




/*  Set up callbacks for buttons */



void ToggleColor(w, closure, call_data)
    Widget      w;
    caddr_t     closure;
    caddr_t     call_data;
{
    int state = (int) call_data;

    switch(state)
      {
      case 0 :  color = CvtStringToPixel("red");
	        break;
      case 1:   color = CvtStringToPixel("black");
	        break;
      case 2:   color = CvtStringToPixel("green");
               break;
      default: color = CvtStringToPixel("orange");
       }
   XtSetArg(myArgs[0], XtNforeground, (XtArgVal) color);
   XtSetValues(colorbox, (ArgList)myArgs, 1);
}

static XtCallbackRec colorCallback[] =
{
   { ToggleColor, (caddr_t) NULL },
   { NULL,         (caddr_t) NULL },
};


void Switch(w, closure, call_data)
    Widget      w;
    caddr_t     closure;
    caddr_t     call_data;
{
   XtSetArg(myArgs[0], XtNlabels, moreLabels);
   XtSetArg(myArgs[1], XtNnumLabels, 3);
   XtSetValues(mbutton, (ArgList)myArgs, 2);
}

static XtCallbackRec switchCallback[] =
{
   { Switch, (caddr_t) NULL },
   { NULL,         (caddr_t) NULL },
};



void main(argc, argv)
    unsigned int argc;
    char **argv;
{
    static char * labels[] =
      { "RED", "PAINT IT BLACK, YOU DEVIL", "GREEN", "ORANGE" };


    toplevel = XtInitialize(
	argv[0], "XTest1", NULL, 0, &argc, argv);
	
    XtSetArg(myArgs[0],XtNlayout, XwIGNORE);
    XtSetArg(myArgs[1],XtNwidth, 500);
    XtSetArg(myArgs[2],XtNheight, 300);
    outer_box = XtCreateManagedWidget
                ("bb1", XwbulletinWidgetClass, toplevel,
			  (ArgList)myArgs, 3);


    XtSetArg(myArgs[0],XtNwidth, 300);
    XtSetArg(myArgs[1],XtNalignment, XwALIGN_CENTER);
    XtSetArg(myArgs[2],XtNstring, 
    		(XtArgVal) "One Of Many Button Box");
    colorbox = XtCreateManagedWidget
               ("text",XwstatictextWidgetClass, outer_box,
			  (ArgList)myArgs, 3);

    XtSetArg(myArgs[0],XtNx, 10);
    XtSetArg(myArgs[1],XtNy, 50);
    XtSetArg(myArgs[2], XtNselect, (XtArgVal) switchCallback);
    pbutton = XtCreateManagedWidget
        ("Change Strings", XwpushButtonWidgetClass, outer_box, (ArgList)myArgs, 3);

    XtSetArg(myArgs[0],XtNx, 10);
    XtSetArg(myArgs[1],XtNy, 100);
    XtSetArg(myArgs[2],XtNselect, (XtArgVal) colorCallback);
    XtSetArg(myArgs[3],XtNinvertOnSelect, FALSE);
    mbutton = XtCreateManagedWidget
               ("MultiButton!", XwmultiButtonWidgetClass, outer_box,
                          (ArgList)myArgs, 4);


        
    XtRealizeWidget(toplevel);
    XtMainLoop();
}


