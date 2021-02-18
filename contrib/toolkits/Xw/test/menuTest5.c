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
#include <Xw/SText.h>
#include <Xw/MenuSep.h>

Widget toplevel, bboard;
Widget mmgrshell1;
Widget mmgr1;
Widget random1, junkr;
Widget exitBtn;
Widget shells[30];
Widget panes[30];
Widget btns[110];
Arg args[30];

static Arg mmgr3Args [] = {
   {XtNmenuUnpost, (XtArgVal) "Shift<Key>Menu"},
   {XtNkbdSelect, (XtArgVal) NULL},
   {XtNmenuSelect, (XtArgVal) NULL}
};

static Arg bboardArgs [] = {
   {XtNheight, (XtArgVal) 250},
   {XtNwidth, (XtArgVal) 150}
};

static Arg btnArgs [] = {
   {XtNkbdAccelerator, (XtArgVal) "Ctrl<Key>a"},
};


static Arg exitBtnArgs [] = {
   {XtNlabel, (XtArgVal) "Exit"},
   {XtNx, (XtArgVal) 100},
   {XtNy, (XtArgVal) 50}
};

static Arg junkrArgs [] = {
   {XtNstring, (XtArgVal) "Use Shift<Btn1> to select Exit Button"},
   {XtNx, (XtArgVal) 10},
   {XtNy, (XtArgVal) 10}
};

/*******************************/

void RunTests()

{

   /********************************************/
   /*
    * PopupMgr (4):
    * Clear associateChildren flag while post string, post accelerator,
    * and kbd select accelerators are active.
    */
   XtSetArg (args[0], XtNassociateChildren, False);
   XtSetValues (mmgr1, args, XtNumber(args));

   /********************************************/
   /*
    * PopupMgr (21):
    * With no top level pane, detach/attach a pane.
    */
   XtSetArg (args[0], XtNattachTo, NULL);
   XtSetValues (panes[0], args, XtNumber(args));
   XtSetArg (args[0], XtNattachTo, NULL);
   XtSetValues (panes[3], args, XtNumber(args));
   XtSetArg (args[0], XtNattachTo, "btn4");
   XtSetValues (panes[3], args, XtNumber(args));
   XtSetArg (args[0], XtNattachTo, "mmgr1");
   XtSetValues (panes[0], args, XtNumber(args));

   /********************************************/
   /*
    * PopupMgr (22):
    * Attach a pane to another pane which has 'mapped when managed' false.
    */
   XtSetArg (args[0], XtNattachTo, NULL);
   XtSetValues (panes[3], args, XtNumber(args));
   XtSetArg (args[0], XtNmappedWhenManaged, False);
   XtSetValues (panes[2], args, XtNumber(args));
   XtSetArg (args[0], XtNattachTo, "btn4");
   XtSetValues (panes[3], args, XtNumber(args));
   XtSetArg (args[0], XtNmappedWhenManaged, True);
   XtSetValues (panes[2], args, XtNumber(args));

   /********************************************/
   /*
    * PopupMgr (24):
    * Unmanage a menubtn which has a select accelerator.
    */
   XtUnmanageChild (btns[3]);

   /********************************************/
   /*
    * PopupMgr (3):
    * Create a menu mgr with associateChildren True, a post string
    * and a post accelerator; the associated widget must be realized.
    */
   XtDestroyWidget (mmgrshell1);
   CreateMenu();
}

/*******************************/

void Quit()

{
   RunTests();
   exit (0);
}
   
/*******************************/
   
CreateMenu()
{
   mmgrshell1 = XtCreatePopupShell("mgrshell1", shellWidgetClass, 
                                bboard, NULL, 0);

   XtSetArg (args[0], XtNpostAccelerator, "Ctrl<Key>Menu");
   XtSetArg (args[1], XtNmenuPost, "<Btn1Down>");
   XtSetArg (args[2], XtNassociateChildren, True);
   mmgr1 = XtCreateManagedWidget("mmgr1", XwpopupmgrWidgetClass, 
                                  mmgrshell1, args, XtNumber(args));

   shells[0] = XtCreatePopupShell("paneshell1", shellWidgetClass, mmgr1, 
                                   NULL, 0);
   XtSetArg (args[0], XtNattachTo, "mmgr1");
   panes[0] = XtCreateManagedWidget("pane1", XwcascadeWidgetClass, 
                                   shells[0], args, 1);

   shells[1] = XtCreatePopupShell("paneshell2", shellWidgetClass, mmgr1, 
                                   NULL, 0);
   XtSetArg (args[0], XtNattachTo, "btn1");
   panes[1] = XtCreateManagedWidget("pane2", XwcascadeWidgetClass, 
                                   shells[1], args, 1);

   shells[2] = XtCreatePopupShell("paneshell3", shellWidgetClass, mmgr1, 
                                   NULL, 0);
   XtSetArg (args[0], XtNattachTo, "btn3");
   panes[2] = XtCreateManagedWidget("pane3", XwcascadeWidgetClass, 
                                   shells[2], args, 1);

   shells[3] = XtCreatePopupShell("paneshell4", shellWidgetClass, mmgr1, 
                                   NULL, 0);
   XtSetArg (args[0], XtNattachTo, "btn4");
   panes[3] = XtCreateManagedWidget("pane4", XwcascadeWidgetClass, 
                                   shells[3], args, 1);

   btns[0] = XtCreateManagedWidget("btn0", XwmenubuttonWidgetClass, 
                                   panes[0], btnArgs, XtNumber(btnArgs));
   btns[1] = XtCreateManagedWidget("btn1", XwmenubuttonWidgetClass, 
                                   panes[0], NULL, 0);
   btns[2] = XtCreateWidget("btn2", XwmenubuttonWidgetClass, 
                                   panes[0], NULL, 0);
   XtSetArg (args[0], XtNkbdAccelerator, "Ctrl<Key>b");
   btns[3] = XtCreateManagedWidget("btn3", XwmenubuttonWidgetClass, 
                                   panes[1], args, XtNumber(args));
   btns[4] = XtCreateManagedWidget("btn4", XwmenubuttonWidgetClass, 
                                   panes[2], NULL, 0);
   btns[5] = XtCreateManagedWidget("btn5", XwmenubuttonWidgetClass, 
                                   panes[3], NULL, 0);
}

void 
main(argc, argv)
    int argc;
    char **argv;

{
   int i;

   /*******************************/
   /* Create the top level widget */
   /*******************************/

   toplevel = XtInitialize (argv[0], "menuTest5", NULL, 0, &argc, argv);

   bboard = XtCreateManagedWidget("bboard", XwbulletinWidgetClass,
                                   toplevel, bboardArgs, XtNumber(bboardArgs));

   /********************************************/

   exitBtn = XtCreateManagedWidget ("exitBtn", 
               XwpushButtonWidgetClass, bboard, exitBtnArgs, 
               XtNumber (exitBtnArgs));
   XtAddCallback (exitBtn, XtNrelease, Quit, NULL);

   junkr = XtCreateManagedWidget ("junkr", 
               XwstatictextWidgetClass, bboard, junkrArgs, 
               XtNumber (junkrArgs));

   /********************************************/
   /*
    * PopupMgr (9):
    * Create a popup, with a post accelerator, select accelerators
    * and associateChildren True; associated widget is unrealized.
    * Afterwards, realize the associated widget.
    */
   CreateMenu();

   /********************************************************/
   /* Realize the widget tree, and start processing events */
   /********************************************************/


   XtRealizeWidget (toplevel);

   XtMainLoop();
}
