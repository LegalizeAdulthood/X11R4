#include <X11/Xlib.h>
#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/Xatom.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <Xw/Xw.h>
#include <Xw/XwP.h>
#include <Xw/MenuBtn.h>
#include <Xw/Cascade.h>
#include <Xw/PopupMgr.h>
#include <Xw/PButton.h>
#include <Xw/RCManager.h>
#include <Xw/BBoard.h>
#include <Xw/Form.h>
#include <Xw/Panel.h>
#include <Xw/MenuSep.h>

Widget toplevel, bboard;
Widget mmgrshell1, mmgrshell3;
Widget mmgr1, mmgr3;
Widget paneshell3a, paneshell3b, paneshell3c, paneshell3d;
Widget paneshell3e, paneshell3f;
Widget menupane3a, menupane3b, menupane3c, menupane3d, menupane3e;
Widget menupane3f, menupane4a;
Widget button3a1, button3a2, button3a3;
Widget button3a4, button3a5, button3a6;
Widget button3a7, button3a8, button3a9;
Widget button3a10, button3a11, button3a12;
Widget random1, junkr;
Widget exitBtn, nullBtn, travBtn;
Widget shells[30];
Widget panes[30];
Widget btns[110];
XImage * newImage;

char MarkData[] =
  {
   0x00, 0x00, 0x00, 0x00, 0xFC, 0x3F, 0x0C, 0x30, 
   0x14, 0x28, 0x24, 0x24, 0x44, 0x22, 0x84, 0x21, 
   0x84, 0x21, 0x44, 0x22, 0x24, 0x24, 0x14, 0x28, 
   0x0C, 0x30, 0xFC, 0x3F, 0x00, 0x00, 0x00, 0x00, 
};


static Arg mmgr3Args [] = {
   {XtNmenuUnpost, (XtArgVal) "Shift<Key>Menu"},
   {XtNkbdSelect, (XtArgVal) NULL},
   {XtNmenuSelect, (XtArgVal) NULL}
};

static Arg bboardArgs [] = {
   {XtNheight, (XtArgVal) 250},
   {XtNwidth, (XtArgVal) 150}
};



/****** Menu Pane ArgLists ******/


static Arg menupane3aArgs [] = {
   {XtNtitleString, (XtArgVal) "Options"},
   {XtNtitleShowing, (XtArgVal) False},
   {XtNattachTo, (XtArgVal) "mmgr3"}
};

static Arg menupane3bArgs [] = {
   {XtNhighlightThickness, (XtArgVal) 0},
   {XtNtitleString, (XtArgVal) ""},
   {XtNtitleShowing, (XtArgVal) False}
};

static Arg menupane3cArgs [] = {
   {XtNtitlePosition, (XtArgVal) XwBOTH},
   {XtNtitleImage, (XtArgVal) NULL},
   {XtNtitleType, (XtArgVal) XwIMAGE}
};

static Arg menupane3dArgs [] = {
   {XtNtitlePosition, (XtArgVal) XwBOTTOM},
   {XtNtitleImage, (XtArgVal) NULL},
   {XtNtitleType, (XtArgVal) XwIMAGE}
};

static Arg menupane3eArgs [] = {
   {XtNtitlePosition, (XtArgVal) XwTOP},
   {XtNtitleImage, (XtArgVal) NULL},
   {XtNtitleType, (XtArgVal) XwIMAGE}
};

static Arg menupane3fArgs [] = {
   {XtNtitlePosition, (XtArgVal) XwTOP},
   {XtNtitleImage, (XtArgVal) NULL},
   {XtNtitleType, (XtArgVal) XwIMAGE}
};

static Arg menupane4aArgs [] = {
   {XtNtitleString, (XtArgVal) NULL},
   {XtNx, (XtArgVal) 10},
   {XtNy, (XtArgVal) 100},
};


/****** Menu Buttons (pane 3a) ArgLists ******/

static Arg button3a1Args [] = {
   {XtNlabel, (XtArgVal) "option1"},
};

static Arg button3a2Args [] = {
   {XtNlabel, (XtArgVal) "option2"},
};

static Arg button3a3Args [] = {
   {XtNlabel, (XtArgVal) "option3"},
};

static Arg button3a4Args [] = {
   {XtNlabel, (XtArgVal) "option4"},
};

static Arg button3a5Args [] = {
   {XtNlabel, (XtArgVal) "option5"},
};

static Arg button3a6Args [] = {
   {XtNlabel, (XtArgVal) "option6"},
};

static Arg button3a7Args [] = {
   {XtNlabel, (XtArgVal) "option7"},
};

static Arg button3a8Args [] = {
   {XtNlabel, (XtArgVal) "option8"},
};

static Arg button3a9Args [] = {
   {XtNlabel, (XtArgVal) "option9"},
};

static Arg button3a10Args [] = {
   {XtNlabel, (XtArgVal) "option10"},
};

static Arg button3a11Args [] = {
   {XtNlabel, (XtArgVal) "option11"},
};

static Arg button3a12Args [] = {
   {XtNlabel, (XtArgVal) "option12"},
};


static Arg exitBtnArgs [] = {
   {XtNlabel, (XtArgVal) "Exit"},
   {XtNx, (XtArgVal) 10},
   {XtNy, (XtArgVal) 10}
};

static Arg nullBtnArgs [] = {
   {XtNlabel, (XtArgVal) "Null Ref Post"},
   {XtNx, (XtArgVal) 10},
   {XtNy, (XtArgVal) 40}
};

static Arg travBtnArgs [] = {
   {XtNlabel, (XtArgVal) "Traversal Post"},
   {XtNx, (XtArgVal) 10},
   {XtNy, (XtArgVal) 70}
};


/*******************************/

void Quit()

{
   exit (0);
}
   
/*******************************/

void NullRef()

{
   /*
    * PopupMgr (28):
    * Manually post a menupane, specifying no reference widget
    */
   XwPostPopup (mmgr1, NULL, 10, 10);
}

/*******************************/
   
void TraversalPost()

{
   /*
    * PopupMgr (29):
    * Manually post a menupane, specifying no reference widget
    */
   Arg args[1];

   XtSetArg (args[0], XtNtraversalOn, True);
   XtSetValues (mmgr1, args, 1);
   XwPostPopup (mmgr1, NULL, 10, 10);
}

/*******************************/

void TraversalOff()

{
   Arg args[1];

   XtSetArg (args[0], XtNtraversalOn, False);
   XtSetValues (mmgr1, args, 1);
}

/*******************************/


void MakeImage()

{
      newImage = XCreateImage (XtDisplay(toplevel), CopyFromParent, 
                       1, XYBitmap, 0, MarkData, 16, 16, 8, 2);

      newImage->byte_order = MSBFirst;
      newImage->bitmap_bit_order = LSBFirst;
      newImage->bitmap_unit = 8;
}

/*******************************/

RunTests ()

{
   Arg args[10];

   /*
    * Cascade (2):
    * Do a setvalue (h & w) on an empty menupane which has
    *  0 highlight thickness.
    */
   XtSetArg (args[0], XtNheight, 100);
   XtSetArg (args[1], XtNwidth, 100);
   XtSetValues (menupane3b, args, 2);

   /*
    * MenuMgr (5):
    * Set post string to null 
    */
   XtSetArg (args[0], XtNmenuPost, NULL);
   XtSetValues (mmgr3, args, 1);

   /*
    * MenuMgr (6):
    * Set select string to valid value, and then null 
    */
   XtSetArg (args[0], XtNmenuSelect, "<Btn3Down>");
   XtSetValues (mmgr3, args, 1);
   XtSetArg (args[0], XtNmenuSelect, NULL);
   XtSetValues (mmgr3, args, 1);

   /*
    * MenuPane (4, 6):
    * Do a setvalues on a menupane which is not part of a menu mgr;
    * also test setting title string to NULL.
    */
   XtSetArg (args[0], XtNtitleString, NULL);
   XtSetValues (menupane4a, args, 1);

   /*
    * MenuPane (5):
    * Test changing a title string to a different string.
    */
   XtSetArg (args[0], XtNtitleString, "abc");
   XtSetValues (menupane4a, args, 1);
   XtSetArg (args[0], XtNtitleString, "12345");
   XtSetValues (menupane4a, args, 1);

   /*
    * MenuBtn (1):
    * Change a select accelerator from a valid one to another valid one.
    */
   XtSetArg (args[0], XtNkbdAccelerator, "Ctrl<Key>a");
   XtSetValues (button3a1, args, 1);
   XtSetArg (args[0], XtNkbdAccelerator, "Ctrl<Key>b");
   XtSetValues (button3a1, args, 1);

   /*
    * MenuBtn (2, 3, 4):
    * Use setvalues to set a NULL mark and cascade image, and also
    * set a negative height and width.
    */
   XtSetArg (args[0], XtNmarkImage, NULL);
   XtSetArg (args[1], XtNcascadeImage, NULL);
   XtSetArg (args[2], XtNheight, -3);
   XtSetArg (args[3], XtNwidth, -3);
   XtSetValues (button3a1, args, 4);
}
/*******************************/
   
void 
main(argc, argv)
    int argc;
    char **argv;
{
   Widget children[60];
   int count = 0;
   Arg args[30];
   int i;
   char shellName[100];
   char paneName[100];
   char btnName[100];
   char accelerator[100];
   char attach[100];

   /*******************************/
   /* Create the top level widget */
   /*******************************/

   toplevel = XtInitialize (argv[0], "menuTest1", NULL, 0, &argc, argv);

   bboard = XtCreateManagedWidget("bboard", XwbulletinWidgetClass,
                                   toplevel, bboardArgs, XtNumber(bboardArgs));

   MakeImage();

   /*************************************************************/
   /* Create two menu managers, one for each of the pushbuttons */
   /*************************************************************/

   mmgrshell3 = XtCreatePopupShell("mgrshell3", shellWidgetClass, 
                                bboard, NULL, 0);

   /* MenuMgr (2, 3, 4) */
   /* PopupMgr (2) */
   mmgr3 = XtCreateManagedWidget("mmgr3", XwpopupmgrWidgetClass, 
                                  mmgrshell3, mmgr3Args, XtNumber(mmgr3Args));


   /**********************************************************************/
   /* Create 1 menupane, which will be used to build the menu hierarchy  */
   /* for the third menu system.                                         */
   /**********************************************************************/

   paneshell3a = XtCreatePopupShell("paneshell3a", shellWidgetClass, mmgr3, 
                                   NULL, 0);
   menupane3a = XtCreateManagedWidget("menupane3a", XwcascadeWidgetClass, 
                                   paneshell3a, menupane3aArgs, 
                                   XtNumber(menupane3aArgs));

   paneshell3b = XtCreatePopupShell("paneshell3b", shellWidgetClass, mmgr3, 
                                   NULL, 0);
   menupane3b = XtCreateManagedWidget("menupane3b", XwcascadeWidgetClass, 
                                   paneshell3b, menupane3bArgs, 
                                   XtNumber(menupane3bArgs));

   /* Cascade (4) */
   paneshell3c = XtCreatePopupShell("paneshell3c", shellWidgetClass, mmgr3, 
                                   NULL, 0);
   menupane3cArgs[1].value = (XtArgVal)newImage;
   menupane3c = XtCreateManagedWidget("menupane3c", XwcascadeWidgetClass, 
                                   paneshell3c, menupane3cArgs, 
                                   XtNumber(menupane3cArgs));

   /* Cascade (5) */
   paneshell3d = XtCreatePopupShell("paneshell3d", shellWidgetClass, mmgr3, 
                                   NULL, 0);
   menupane3dArgs[1].value = (XtArgVal)newImage;
   menupane3d = XtCreateManagedWidget("menupane3d", XwcascadeWidgetClass, 
                                   paneshell3d, menupane3dArgs, 
                                   XtNumber(menupane3dArgs));

   /* Cascade (6) */
   paneshell3e = XtCreatePopupShell("paneshell3e", shellWidgetClass, mmgr3, 
                                   NULL, 0);
   menupane3eArgs[1].value = (XtArgVal)newImage;
   menupane3e = XtCreateManagedWidget("menupane3e", XwcascadeWidgetClass, 
                                   paneshell3e, menupane3eArgs, 
                                   XtNumber(menupane3eArgs));

   /* Cascade (7) */
   paneshell3f = XtCreatePopupShell("paneshell3f", shellWidgetClass, mmgr3, 
                                   NULL, 0);
   menupane3f = XtCreateManagedWidget("menupane3f", XwcascadeWidgetClass, 
                                   paneshell3f, menupane3fArgs, 
                                   XtNumber(menupane3fArgs));

   menupane4a = XtCreateManagedWidget("menupane4a", XwcascadeWidgetClass, 
                                   bboard, menupane4aArgs, 
                                   XtNumber(menupane4aArgs));


   /*****************************************/
   /* Create the menubuttons for menupane3a */
   /*****************************************/

   /* MenuPane (7) */
   children[0] = button3a1 = XtCreateWidget ("option1", 
               XwmenubuttonWidgetClass, menupane3a, button3a1Args, 
               XtNumber (button3a1Args));

   children[1] = button3a2 = XtCreateWidget ("option2", 
               XwmenubuttonWidgetClass, menupane3a, button3a2Args, 
               XtNumber (button3a2Args));

   children[2] = button3a3 = XtCreateWidget ("option3", 
               XwmenubuttonWidgetClass, menupane3a, button3a3Args, 
               XtNumber (button3a3Args));

   /* Cascade (1) */
   children[3] = random1 = XtCreateWidget ("random1", 
               XwpushButtonWidgetClass, menupane3a, button3a3Args, 
               XtNumber (button3a3Args));

   children[4] = button3a4 = XtCreateWidget ("option4", 
               XwmenubuttonWidgetClass, menupane3a, button3a4Args, 
               XtNumber (button3a4Args));

   children[5] = button3a5 = XtCreateWidget ("option5", 
               XwmenubuttonWidgetClass, menupane3a, button3a5Args, 
               XtNumber (button3a5Args));

   children[6] = button3a6 = XtCreateWidget ("option6", 
               XwmenubuttonWidgetClass, menupane3a, button3a6Args, 
               XtNumber (button3a6Args));

   children[7] = button3a7 = XtCreateWidget ("option7", 
               XwpushButtonWidgetClass, menupane3a, button3a7Args, 
               XtNumber (button3a7Args));

   children[8] = button3a8 = XtCreateWidget ("option8", 
               XwmenubuttonWidgetClass, menupane3a, button3a8Args, 
               XtNumber (button3a8Args));

   children[9] = button3a9 = XtCreateWidget ("option9", 
               XwmenubuttonWidgetClass, menupane3a, button3a9Args, 
               XtNumber (button3a9Args));

   children[10] = button3a10 = XtCreateWidget ("option10", 
               XwmenubuttonWidgetClass, menupane3a, button3a10Args, 
               XtNumber (button3a10Args));

   children[11] = button3a11 = XtCreateWidget ("option11", 
               XwpushButtonWidgetClass, menupane3a, button3a11Args, 
               XtNumber (button3a11Args));
   XtManageChildren (children, 12);

   /********************************************/

   exitBtn = XtCreateManagedWidget ("exitBtn", 
               XwpushButtonWidgetClass, bboard, exitBtnArgs, 
               XtNumber (exitBtnArgs));
   XtAddCallback (exitBtn, XtNrelease, Quit, NULL);

   nullBtn = XtCreateManagedWidget ("nullBtn", 
               XwpushButtonWidgetClass, bboard, nullBtnArgs, 
               XtNumber (nullBtnArgs));
   XtAddCallback (nullBtn, XtNrelease, NullRef, NULL);

   travBtn = XtCreateManagedWidget ("travBtn", 
               XwpushButtonWidgetClass, bboard, travBtnArgs, 
               XtNumber (travBtnArgs));
   XtAddCallback (travBtn, XtNselect, TraversalPost, NULL);

   /********************************************/

   /*
    * PopupMgr (6):
    * Delete a popup mgr, before the associated widget has been realized.
    * Use an unmanaged widget, to bypass the 2 part destroy shortcomings.
    */
   junkr = XtCreateWidget ("junkr", XwpushButtonWidgetClass, bboard, NULL, 0);
   mmgrshell1 = XtCreatePopupShell("mgrshell1", shellWidgetClass, 
                                junkr, NULL, 0);

   mmgr1 = XtCreateManagedWidget("mmgr1", XwpopupmgrWidgetClass, 
                                  mmgrshell1, NULL, 0);
   XtDestroyWidget (mmgrshell1);

   /********************************************/
   
   /*
    * PopupMgr (19):
    * Create 30 attached menupanes (with no buttons), so that they all
    * end up on the attach list.
    */
   mmgrshell1 = XtCreatePopupShell("mgrshell1", shellWidgetClass, 
                                menupane4a, NULL, 0);

   mmgr1 = XtCreateManagedWidget("mmgr1", XwpopupmgrWidgetClass, 
                                  mmgrshell1, NULL, 0);
   XtSetArg (args[0], XtNmenuSelect, "<Btn1Down>");
   XtSetValues (mmgr1, args, 1);

   for (i = 0; i < 30; i++)
   {
      sprintf (attach, "btn%d", i);
      XtSetArg (args[0], XtNattachTo, attach);
      sprintf (shellName, "shell%d", i);
      sprintf (paneName, "pane%d", i);
      shells[i] = XtCreatePopupShell(shellName, shellWidgetClass, mmgr1, 
                                      NULL, 0);
      panes[i] = XtCreateManagedWidget(paneName, XwcascadeWidgetClass, 
                                      shells[i], args, 1);
   }

   for (i = 1; i < 30; i++)
      XtDestroyWidget (shells[i]);

   /********************************************/
   
   XtSetArg (args[0], XtNattachTo, "mmgr1");
   XtSetValues (panes[0], args, 1);
   
   for (i = 0; i < 10; i++)
   {
      sprintf (accelerator, "<Key>%c", 'a' + i);
      XtSetArg (args[0], XtNkbdAccelerator, accelerator);
      sprintf (btnName, "btn%d", i);
      btns[i] = XtCreateManagedWidget(btnName, XwmenubuttonWidgetClass, 
                                      panes[0], args, 1);
      XtAddCallback (btns[i], XtNselect, TraversalOff, NULL);
   }

   /********************************************/
   
   /*
    * PopupMgr (26):
    * Attempt to manually post, specifying a NULL menu mgr.
    */
   XwPostPopup (NULL, NULL, 10, 10);

   /********************************************/
   
   /*
    * PopupMgr (27):
    * Attempt to manually post, specifying a NULL menu mgr.
    */
   XtSetSensitive (mmgr1, FALSE);
   XwPostPopup (mmgr1, NULL, 10, 10);
   XtSetSensitive (mmgr1, TRUE);

   /********************************************/


   /********************************************************/
   /* Realize the widget tree, and start processing events */
   /********************************************************/


   XtRealizeWidget (toplevel);

   RunTests();

   XtMainLoop();
}
