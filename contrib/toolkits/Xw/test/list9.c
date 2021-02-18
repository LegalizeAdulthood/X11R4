

/****************************************
*   Automatic test program generation   *
*       Tue Jul 26 14:42:49 1988        *
****************************************/

#include "list9.h"

void main (argc, argv)
int argc;
char **argv;
{
   extern XtArgVal getValue();
   extern void getOptions();
   extern void flushEvents();
   extern void myQuit();

   signal (SIGINT, myQuit);

   Shell1 = XtInitialize (argv[0], "list9.defs", NULL, 0, &argc, argv);
   XwRegisterConverters();
   display = XtDisplay(Shell1);
/*   XSynchronize(display, 1);   */
   getOptions(argc, argv, &delay, &trace);

   n = 0;
   XtSetArg(args[n], XtNwidth, 400);  n++;
   XtSetArg(args[n], XtNheight, 300);  n++;
   XtSetArg(args[n], XtNallowShellResize, True);  n++;
   XtSetValues(Shell1, args, n);
   XtRealizeWidget(Shell1);
   wait = True;
   while (wait) {
      XGetWindowAttributes(XtDisplay(Shell1), XtWindow(Shell1), &info);
      if (info.map_state == IsViewable) wait = False;
      else sleep(1);
   }
   if (trace) {
      printf("\nXtInitialize:  Shell1 TopLevelShell\n\n");
      printf("XtSetArg:  XtNwidth 400\n");
      printf("XtSetArg:  XtNheight 300\n");
      printf("XtSetArg:  XtNallowShellResize\n");
      printf("XtSetValues:  Shell1\n");
      printf("XtRealizeWidget:  Shell1\n\n");
   }
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNcolumns,
      getValue("4", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "columns", "4");
   RowCol1 = XtCreateManagedWidget("RowCol1", XwrowColWidgetClass,
      Shell1, args, n);
   if (trace) printf("XtCreateManagedWidget:  RowCol1 RowCol Shell1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "100");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   List1 = XtCreateManagedWidget("List1", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List1 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow1 = XtCreateManagedWidget("Arrow1", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow1 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow2 = XtCreateManagedWidget("Arrow2", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow2 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow3 = XtCreateManagedWidget("Arrow3", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow3 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow4 = XtCreateManagedWidget("Arrow4", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow4 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow5 = XtCreateManagedWidget("Arrow5", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow5 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow6 = XtCreateManagedWidget("Arrow6", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow6 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow7 = XtCreateManagedWidget("Arrow7", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow7 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow8 = XtCreateManagedWidget("Arrow8", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow8 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow9 = XtCreateManagedWidget("Arrow9", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow9 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow10 = XtCreateManagedWidget("Arrow10", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow10 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Orange", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Orange");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow11 = XtCreateManagedWidget("Arrow11", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow11 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Orange", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Orange");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow12 = XtCreateManagedWidget("Arrow12", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow12 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Violet", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Violet");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow13 = XtCreateManagedWidget("Arrow13", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow13 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "30");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNforeground,
      getValue("Violet", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Violet");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   Arrow14 = XtCreateManagedWidget("Arrow14", XwarrowWidgetClass,
      List1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow14 Arrow List1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("240", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "240");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Navy", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Navy");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionMethod,
      getValue("Multiple", "SelectionMethod"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionMethod", "Multiple");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   List2 = XtCreateManagedWidget("List2", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List2 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton1 = XtCreateManagedWidget("PushButton1", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton1 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton2 = XtCreateManagedWidget("PushButton2", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton2 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton3 = XtCreateManagedWidget("PushButton3", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton3 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton4 = XtCreateManagedWidget("PushButton4", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton4 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton5 = XtCreateManagedWidget("PushButton5", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton5 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Orange", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Orange");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton6 = XtCreateManagedWidget("PushButton6", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton6 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Violet", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Violet");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton7 = XtCreateManagedWidget("PushButton7", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton7 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Turquoise", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Turquoise");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton8 = XtCreateManagedWidget("PushButton8", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton8 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("CornflowerBlue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "CornflowerBlue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton9 = XtCreateManagedWidget("PushButton9", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton9 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("SteelBlue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "SteelBlue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton10 = XtCreateManagedWidget("PushButton10", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton10 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Maroon", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Maroon");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton11 = XtCreateManagedWidget("PushButton11", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton11 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton12 = XtCreateManagedWidget("PushButton12", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton12 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("ForestGreen", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "ForestGreen");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNtoggle,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "toggle", "True");
   PushButton13 = XtCreateManagedWidget("PushButton13", XwpushButtonWidgetClass,
      List2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton13 PushButton List2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("240", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "240");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionMethod,
      getValue("Multiple", "SelectionMethod"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionMethod", "Multiple");
   XtSetArg(args[n], XtNselectionStyle,
      getValue("Sticky", "SelectionStyle"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionStyle", "Sticky");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   List3 = XtCreateManagedWidget("List3", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List3 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle1 = XtCreateManagedWidget("Toggle1", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle1 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle2 = XtCreateManagedWidget("Toggle2", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle2 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle3 = XtCreateManagedWidget("Toggle3", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle3 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle4 = XtCreateManagedWidget("Toggle4", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle4 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle5 = XtCreateManagedWidget("Toggle5", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle5 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle6 = XtCreateManagedWidget("Toggle6", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle6 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle7 = XtCreateManagedWidget("Toggle7", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle7 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle8 = XtCreateManagedWidget("Toggle8", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle8 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle9 = XtCreateManagedWidget("Toggle9", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle9 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-25");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("left", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "left");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle10 = XtCreateManagedWidget("Toggle10", XwtoggleWidgetClass,
      List3, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle10 Toggle List3\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("160", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "160");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionMethod,
      getValue("Single", "SelectionMethod"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionMethod", "Single");
   XtSetArg(args[n], XtNselectionStyle,
      getValue("Sticky", "SelectionStyle"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionStyle", "Sticky");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   List4 = XtCreateManagedWidget("List4", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List4 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText1 = XtCreateManagedWidget("StaticText1", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText1 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText2 = XtCreateManagedWidget("StaticText2", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText2 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText3 = XtCreateManagedWidget("StaticText3", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText3 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText4 = XtCreateManagedWidget("StaticText4", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText4 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText5 = XtCreateManagedWidget("StaticText5", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText5 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText6 = XtCreateManagedWidget("StaticText6", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText6 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText7 = XtCreateManagedWidget("StaticText7", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText7 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText8 = XtCreateManagedWidget("StaticText8", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText8 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("turquoise", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "turquoise");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText9 = XtCreateManagedWidget("StaticText9", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText9 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("turquoise", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "turquoise");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText10 = XtCreateManagedWidget("StaticText10", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText10 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText11 = XtCreateManagedWidget("StaticText11", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText11 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText12 = XtCreateManagedWidget("StaticText12", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText12 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Purple", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Purple");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText13 = XtCreateManagedWidget("StaticText13", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText13 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Purple", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Purple");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText14 = XtCreateManagedWidget("StaticText14", XwstatictextWidgetClass,
      List4, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText14 StaticText List4\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("240", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "240");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   XtSetArg(args[n], XtNselectionBias,
      getValue("Row_Bias", "SelectionBias"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionBias", "Row_Bias");
   List5 = XtCreateManagedWidget("List5", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List5 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle11 = XtCreateManagedWidget("Toggle11", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle11 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle12 = XtCreateManagedWidget("Toggle12", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle12 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle13 = XtCreateManagedWidget("Toggle13", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle13 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle14 = XtCreateManagedWidget("Toggle14", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle14 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle15 = XtCreateManagedWidget("Toggle15", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle15 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle16 = XtCreateManagedWidget("Toggle16", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle16 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle17 = XtCreateManagedWidget("Toggle17", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle17 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle18 = XtCreateManagedWidget("Toggle18", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle18 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Orange", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Orange");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle19 = XtCreateManagedWidget("Toggle19", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle19 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Orange", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Orange");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vrb-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vrb-25");
   XtSetArg(args[n], XtNsquare,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "square", "False");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle20 = XtCreateManagedWidget("Toggle20", XwtoggleWidgetClass,
      List5, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle20 Toggle List5\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("160", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "160");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionMethod,
      getValue("Multiple", "SelectionMethod"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionMethod", "Multiple");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   XtSetArg(args[n], XtNselectionBias,
      getValue("Row_Bias", "SelectionBias"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionBias", "Row_Bias");
   List6 = XtCreateManagedWidget("List6", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List6 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow15 = XtCreateManagedWidget("Arrow15", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow15 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow16 = XtCreateManagedWidget("Arrow16", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow16 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow17 = XtCreateManagedWidget("Arrow17", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow17 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow18 = XtCreateManagedWidget("Arrow18", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow18 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow19 = XtCreateManagedWidget("Arrow19", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow19 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow20 = XtCreateManagedWidget("Arrow20", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow20 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow21 = XtCreateManagedWidget("Arrow21", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow21 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow22 = XtCreateManagedWidget("Arrow22", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow22 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow23 = XtCreateManagedWidget("Arrow23", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow23 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "50");
   XtSetArg(args[n], XtNheight,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "50");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_down", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_down");
   Arrow24 = XtCreateManagedWidget("Arrow24", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow24 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("240", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "240");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionMethod,
      getValue("Multiple", "SelectionMethod"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionMethod", "Multiple");
   XtSetArg(args[n], XtNselectionStyle,
      getValue("Sticky", "SelectionStyle"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionStyle", "Sticky");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   XtSetArg(args[n], XtNselectionBias,
      getValue("Row_Bias", "SelectionBias"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionBias", "Row_Bias");
   List7 = XtCreateManagedWidget("List7", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List7 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton17 = XtCreateManagedWidget("PushButton17", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton17 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton18 = XtCreateManagedWidget("PushButton18", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton18 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton19 = XtCreateManagedWidget("PushButton19", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton19 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton20 = XtCreateManagedWidget("PushButton20", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton20 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton21 = XtCreateManagedWidget("PushButton21", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton21 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton22 = XtCreateManagedWidget("PushButton22", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton22 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("SteelBlue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "SteelBlue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton23 = XtCreateManagedWidget("PushButton23", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton23 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("SteelBlue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "SteelBlue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton24 = XtCreateManagedWidget("PushButton24", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton24 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton25 = XtCreateManagedWidget("PushButton25", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton25 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-25", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-25");
   PushButton26 = XtCreateManagedWidget("PushButton26", XwpushButtonWidgetClass,
      List7, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton26 PushButton List7\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("160", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "160");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionStyle,
      getValue("Sticky", "SelectionStyle"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionStyle", "Sticky");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   XtSetArg(args[n], XtNselectionBias,
      getValue("Row_Bias", "SelectionBias"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionBias", "Row_Bias");
   List8 = XtCreateManagedWidget("List8", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List8 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText15 = XtCreateManagedWidget("StaticText15", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText15 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText16 = XtCreateManagedWidget("StaticText16", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText16 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("turquoise", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "turquoise");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText17 = XtCreateManagedWidget("StaticText17", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText17 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("turquoise", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "turquoise");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText18 = XtCreateManagedWidget("StaticText18", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText18 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Navy", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Navy");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText19 = XtCreateManagedWidget("StaticText19", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText19 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Navy", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Navy");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText20 = XtCreateManagedWidget("StaticText20", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText20 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText21 = XtCreateManagedWidget("StaticText21", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText21 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText22 = XtCreateManagedWidget("StaticText22", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText22 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText23 = XtCreateManagedWidget("StaticText23", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText23 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText24 = XtCreateManagedWidget("StaticText24", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText24 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText25 = XtCreateManagedWidget("StaticText25", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText25 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText26 = XtCreateManagedWidget("StaticText26", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText26 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText27 = XtCreateManagedWidget("StaticText27", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText27 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fg-20", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fg-20");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText28 = XtCreateManagedWidget("StaticText28", XwstatictextWidgetClass,
      List8, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText28 StaticText List8\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("160", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "160");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   XtSetArg(args[n], XtNselectionBias,
      getValue("Col_Bias", "SelectionBias"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionBias", "Col_Bias");
   List9 = XtCreateManagedWidget("List9", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List9 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow25 = XtCreateManagedWidget("Arrow25", XwarrowWidgetClass,
      List6, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow25 Arrow List6\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow26 = XtCreateManagedWidget("Arrow26", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow26 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow27 = XtCreateManagedWidget("Arrow27", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow27 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow28 = XtCreateManagedWidget("Arrow28", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow28 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow29 = XtCreateManagedWidget("Arrow29", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow29 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Turquoise", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Turquoise");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow30 = XtCreateManagedWidget("Arrow30", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow30 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Turquoise", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Turquoise");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow31 = XtCreateManagedWidget("Arrow31", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow31 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Orange", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Orange");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow32 = XtCreateManagedWidget("Arrow32", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow32 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Orange", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Orange");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow33 = XtCreateManagedWidget("Arrow33", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow33 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow34 = XtCreateManagedWidget("Arrow34", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow34 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow35 = XtCreateManagedWidget("Arrow35", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow35 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow36 = XtCreateManagedWidget("Arrow36", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow36 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow37 = XtCreateManagedWidget("Arrow37", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow37 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "70");
   XtSetArg(args[n], XtNheight,
      getValue("70", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "70");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_right", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_right");
   Arrow38 = XtCreateManagedWidget("Arrow38", XwarrowWidgetClass,
      List9, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow38 Arrow List9\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("160", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "160");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionMethod,
      getValue("Multiple", "SelectionMethod"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionMethod", "Multiple");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   XtSetArg(args[n], XtNselectionBias,
      getValue("Col_Bias", "SelectionBias"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionBias", "Col_Bias");
   List10 = XtCreateManagedWidget("List10", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List10 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText29 = XtCreateManagedWidget("StaticText29", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText29 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText30 = XtCreateManagedWidget("StaticText30", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText30 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText31 = XtCreateManagedWidget("StaticText31", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText31 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText32 = XtCreateManagedWidget("StaticText32", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText32 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText33 = XtCreateManagedWidget("StaticText33", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText33 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText34 = XtCreateManagedWidget("StaticText34", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText34 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText35 = XtCreateManagedWidget("StaticText35", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText35 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("FireBrick", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "FireBrick");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText36 = XtCreateManagedWidget("StaticText36", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText36 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("SteelBlue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "SteelBlue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText37 = XtCreateManagedWidget("StaticText37", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText37 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("SteelBlue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "SteelBlue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText38 = XtCreateManagedWidget("StaticText38", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText38 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("CornflowerBlue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "CornflowerBlue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText39 = XtCreateManagedWidget("StaticText39", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText39 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("CornflowerBlue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "CornflowerBlue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText40 = XtCreateManagedWidget("StaticText40", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText40 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Navy", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Navy");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText41 = XtCreateManagedWidget("StaticText41", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText41 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Navy", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Navy");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText42 = XtCreateManagedWidget("StaticText42", XwstatictextWidgetClass,
      List10, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText42 StaticText List10\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("160", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "160");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionMethod,
      getValue("Multiple", "SelectionMethod"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionMethod", "Multiple");
   XtSetArg(args[n], XtNselectionStyle,
      getValue("Sticky", "SelectionStyle"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionStyle", "Sticky");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   XtSetArg(args[n], XtNselectionBias,
      getValue("Col_Bias", "SelectionBias"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionBias", "Col_Bias");
   List11 = XtCreateManagedWidget("List11", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List11 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "90");
   XtSetArg(args[n], XtNheight,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "90");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_left", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_left");
   Arrow39 = XtCreateManagedWidget("Arrow39", XwarrowWidgetClass,
      List11, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow39 Arrow List11\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "90");
   XtSetArg(args[n], XtNheight,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "90");
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_left", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_left");
   Arrow40 = XtCreateManagedWidget("Arrow40", XwarrowWidgetClass,
      List11, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow40 Arrow List11\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "90");
   XtSetArg(args[n], XtNheight,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "90");
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_left", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_left");
   Arrow41 = XtCreateManagedWidget("Arrow41", XwarrowWidgetClass,
      List11, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow41 Arrow List11\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "90");
   XtSetArg(args[n], XtNheight,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "90");
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_left", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_left");
   Arrow42 = XtCreateManagedWidget("Arrow42", XwarrowWidgetClass,
      List11, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow42 Arrow List11\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "90");
   XtSetArg(args[n], XtNheight,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "90");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_left", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_left");
   Arrow43 = XtCreateManagedWidget("Arrow43", XwarrowWidgetClass,
      List11, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow43 Arrow List11\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "90");
   XtSetArg(args[n], XtNheight,
      getValue("90", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "90");
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNarrowDirection,
      getValue("arrow_left", "arrowDirection"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "arrowDirection", "arrow_left");
   Arrow44 = XtCreateManagedWidget("Arrow44", XwarrowWidgetClass,
      List11, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow44 Arrow List11\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("240", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "240");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNnumColumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "numColumns", "2");
   XtSetArg(args[n], XtNselectionStyle,
      getValue("Sticky", "SelectionStyle"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionStyle", "Sticky");
   XtSetArg(args[n], XtNelementHighlight,
      getValue("Invert", "ElementHighlight"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "elementHighlight", "Invert");
   XtSetArg(args[n], XtNselectionBias,
      getValue("Col_Bias", "SelectionBias"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectionBias", "Col_Bias");
   List12 = XtCreateManagedWidget("List12", XwlistWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  List12 List RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Orange", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Orange");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-31", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-31");
   XtSetArg(args[n], XtNselectColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Red");
   Toggle21 = XtCreateManagedWidget("Toggle21", XwtoggleWidgetClass,
      List12, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle21 Toggle List12\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Purple", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Purple");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-31", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-31");
   XtSetArg(args[n], XtNselectColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Red");
   Toggle22 = XtCreateManagedWidget("Toggle22", XwtoggleWidgetClass,
      List12, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle22 Toggle List12\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Red");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-31", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-31");
   XtSetArg(args[n], XtNselectColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Red");
   Toggle23 = XtCreateManagedWidget("Toggle23", XwtoggleWidgetClass,
      List12, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle23 Toggle List12\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Blue");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-31", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-31");
   XtSetArg(args[n], XtNselectColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Red");
   Toggle24 = XtCreateManagedWidget("Toggle24", XwtoggleWidgetClass,
      List12, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle24 Toggle List12\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Yellow");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-31", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-31");
   XtSetArg(args[n], XtNselectColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Red");
   Toggle25 = XtCreateManagedWidget("Toggle25", XwtoggleWidgetClass,
      List12, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle25 Toggle List12\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-31", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-31");
   XtSetArg(args[n], XtNselectColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Red");
   Toggle26 = XtCreateManagedWidget("Toggle26", XwtoggleWidgetClass,
      List12, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle26 Toggle List12\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-31", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-31");
   XtSetArg(args[n], XtNselectColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Blue");
   Toggle27 = XtCreateManagedWidget("Toggle27", XwtoggleWidgetClass,
      List12, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle27 Toggle List12\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNforeground,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "foreground", "Green");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNfont,
      getValue("vri-31", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "vri-31");
   XtSetArg(args[n], XtNselectColor,
      getValue("Turquoise", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "Turquoise");
   Toggle28 = XtCreateManagedWidget("Toggle28", XwtoggleWidgetClass,
      List12, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle28 Toggle List12\n\n");
   flushEvents(delay);

   if (trace) printf("XtMainLoop:\n\n");
   XtMainLoop();
}

static XtArgVal getValue(strPtr, resType)
char *strPtr, *resType;
{
   XrmValue from, to;
   XtArgVal toValue;

   if (strcmp(resType, XtRString) == 0) return((XtArgVal) strPtr);
   from.size = strlen(strPtr) + 1;
   if (from.size < sizeof(String)) from.size = sizeof(String);
   from.addr = strPtr;
   to.addr = NULL;
   XtConvert(Shell1, XtRString, &from, resType, &to);
   if (to.size == sizeof(XtArgVal))
      toValue =  *((XtArgVal *) to.addr);
   else if (to.size == sizeof(short))
      toValue = (XtArgVal) *((short *) to.addr);
   else if (to.size == sizeof(char))
      toValue = (XtArgVal) *((char *) to.addr);
   else if (to.size < sizeof(XtArgVal))
      bcopy ((char *)to.addr, (char *)&toValue, (int)to.size);
   else {  /* to.addr is ptr to data */
      toValue = (XtArgVal) XtMalloc(to.size);
      bcopy ((char *)to.addr, (char *)toValue, (int)to.size);
   }
   return((XtArgVal) toValue);
}

static void getOptions(argc, argv, delay, trace)
int argc;
char **argv;
int *delay;
Boolean *trace;
{
   register int c;
   extern char *optarg;
   while ((c = getopt(argc, argv, "s:p")) != EOF) 
      switch(c) {
         case 's':
            *delay = (atoi(optarg) < 1) ? 0 : atoi(optarg);
            break;
         case 'p':
            *trace = True;
            break;
         default:
            fprintf(stderr, "usage: %s [-s delay] [-p]\n", PROGNAME);
            fprintf(stderr, "The -s option sets the delay between actions.\n");
            fprintf(stderr, "The -p option enables trace output.\n");
            exit();
      }
}

static void flushEvents(delay)
int delay;
{
   XEvent event;
   while (XPending(display)) {
      XNextEvent(display, &event);
      XtDispatchEvent(&event);
   }
   sleep(delay);
}




void myQuit()
{
     printf("Begin exiting ... please standby ... \n");
     fflush(stdout);
     exit(0);
}
