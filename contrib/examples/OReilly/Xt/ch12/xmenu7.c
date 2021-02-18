/*
 * Copyright 1989 O'Reilly and Associates, Inc.
 * See ../Copyright for complete rights and liability information.
 */


#include <stdio.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/bitmaps/xlogo16>

/* works under R4 and later only */
#include <X11/Xaw/MenuButton.h>
#include <X11/Xaw/SimpleMenu.h>
#include <X11/Xaw/SmeBSB.h>
#include <X11/Xaw/SmeLine.h>

#define NUM_MENU_ITEMS 12

static String menu_entry_names[] = {
  "quit",
  "item1",
  "item2",
  "item3",
  "line",
  "item5",
  "item6",
  "item7",
  "blank",
  "menu1",
  "menu2",
  "menu3",
};

static Boolean status[NUM_MENU_ITEMS];
static Pixmap mark;

/* ARGSUSED */
static void
MenuSelect(w, pane_num, garbage)
Widget w;
int pane_num;	/* client_data */
caddr_t garbage;	/* call_data */
{
    Arg arglist[1];
    Cardinal num_args = 0;

    printf("Menu item %s has been selected.\n", XtName(w));

    if (pane_num == 0)			/* quit selected. */
        exit(0);
    
    if (status[pane_num]) 
        XtSetArg(arglist[num_args], XtNleftBitmap, None); 
    else 
        XtSetArg(arglist[num_args], XtNleftBitmap, mark);
    num_args++;
        XtSetValues(w, arglist, num_args);

    status[pane_num] = !status[pane_num];
}

void
main(argc, argv)
char ** argv;
int argc;
{
    Widget topLevel, menu, button, entry;
    int i;
    Arg arglist[1];

    topLevel = XtInitialize(argv[0], "XMenu7",  NULL, 0, 
                          (unsigned int*) &argc, argv);

    button = XtCreateManagedWidget("menuButton", 
                          menuButtonWidgetClass, topLevel,
                          arglist, (Cardinal) 0);
    menu = XtCreatePopupShell("menu", simpleMenuWidgetClass, 
                          button, NULL, 0);
  
    for (i = 0; i < NUM_MENU_ITEMS ; i++) {
        String item = menu_entry_names[i];


        if (i == 4)   /* use a line pane */
            entry = XtCreateManagedWidget(item,
                          smeLineObjectClass, menu,
                          NULL, 0);
        else if (i == 8) /* blank entry */
            entry = XtCreateManagedWidget(item, smeObjectClass, menu,
                          NULL, 0);
        else {
            entry = XtCreateManagedWidget(item, smeBSBObjectClass, 
                          menu, NULL, 0);

            XtAddCallback(entry, XtNcallback, MenuSelect, (caddr_t) i);
        }
    }

    mark = XCreateBitmapFromData(XtDisplay(topLevel),
			  RootWindowOfScreen(XtScreen(topLevel)),
			  xlogo16_bits, xlogo16_width, xlogo16_height);

    XtRealizeWidget(topLevel);
    XtMainLoop();
}
