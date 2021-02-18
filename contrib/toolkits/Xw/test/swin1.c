

/****************************************
*   Automatic test program generation   *
*       Mon Jul 11 14:21:55 1988        *
****************************************/

#include "swin1.h"

void main (argc, argv)
int argc;
char **argv;
{
   extern XtArgVal getValue();
   extern void getOptions();
   extern void flushEvents();
   extern void myQuit();

   signal (SIGINT, myQuit);

   Shell1 = XtInitialize (argv[0], "swin1.defs", NULL, 0, &argc, argv);
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
   Form1 = XtCreateManagedWidget("Form1", XwformWidgetClass,
      Shell1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Form1 Form Shell1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("Static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "Static");
   XtSetArg(args[n], XtNxRefName,
      getValue("Form1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "Form1");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "True");
   StaticText1 = XtCreateManagedWidget("StaticText1", XwstatictextWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText1 StaticText Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNseparatorType,
      getValue("no_line", "SeparatorType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "separatorType", "no_line");
   XtSetArg(args[n], XtNxRefName,
      getValue("Form1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "Form1");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "True");
   XtSetArg(args[n], XtNyRefName,
      getValue("StaticText1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "StaticText1");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   MenuSep1 = XtCreateManagedWidget("MenuSep1", XwmenuSepWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  MenuSep1 MenuSep Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetArg(args[n], XtNlayoutType,
      getValue("maximum_columns", "LayoutType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "layoutType", "maximum_columns");
   XtSetArg(args[n], XtNsingleRow,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "singleRow", "True");
   XtSetArg(args[n], XtNxRefName,
      getValue("Form1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "Form1");
   XtSetArg(args[n], XtNxOffset,
      getValue("4", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xOffset", "4");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "True");
   XtSetArg(args[n], XtNxAttachOffset,
      getValue("4", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachOffset", "4");
   XtSetArg(args[n], XtNyRefName,
      getValue("MenuSep1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "MenuSep1");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   RowCol1 = XtCreateManagedWidget("RowCol1", XwrowColWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  RowCol1 RowCol Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabel,
      getValue("Area", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "label", "Area");
   PushButton1 = XtCreateManagedWidget("PushButton1", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton1 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetValues(RowCol1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "RowCol1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabel,
      getValue("File", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "label", "File");
   PushButton2 = XtCreateManagedWidget("PushButton2", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton2 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabel,
      getValue("Options", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "label", "Options");
   PushButton3 = XtCreateManagedWidget("PushButton3", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton3 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabel,
      getValue("Show", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "label", "Show");
   PushButton4 = XtCreateManagedWidget("PushButton4", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton4 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNlabel,
      getValue("History", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "label", "History");
   PushButton5 = XtCreateManagedWidget("PushButton5", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton5 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNseparatorType,
      getValue("no_line", "SeparatorType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "separatorType", "no_line");
   XtSetArg(args[n], XtNxRefName,
      getValue("Form1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "Form1");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "True");
   XtSetArg(args[n], XtNyRefName,
      getValue("RowCol1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "RowCol1");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   MenuSep2 = XtCreateManagedWidget("MenuSep2", XwmenuSepWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  MenuSep2 MenuSep Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetArg(args[n], XtNgravity,
      getValue("WestGravity", "Gravity"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "gravity", "WestGravity");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("Work Area:", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "Work Area:");
   XtSetArg(args[n], XtNxRefName,
      getValue("Form1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "Form1");
   XtSetArg(args[n], XtNxOffset,
      getValue("8", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xOffset", "8");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "False");
   XtSetArg(args[n], XtNyRefName,
      getValue("MenuSep2", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "MenuSep2");
   XtSetArg(args[n], XtNyOffset,
      getValue("4", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yOffset", "4");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   StaticText2 = XtCreateManagedWidget("StaticText2", XwstatictextWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText2 StaticText Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNheight,
      getValue("18", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "18");
   XtSetArg(args[n], XtNwidth,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "300");
   XtSetArg(args[n], XtNxRefName,
      getValue("StaticText2", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "StaticText2");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "True");
   XtSetArg(args[n], XtNxAttachOffset,
      getValue("4", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachOffset", "4");
   XtSetArg(args[n], XtNyRefName,
      getValue("MenuSep2", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "MenuSep2");
   XtSetArg(args[n], XtNyOffset,
      getValue("6", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yOffset", "6");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   TextEdit1 = XtCreateManagedWidget("TextEdit1", XwtexteditWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  TextEdit1 TextEdit Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetArg(args[n], XtNgravity,
      getValue("WestGravity", "Gravity"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "gravity", "WestGravity");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("Selection:", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "Selection:");
   XtSetArg(args[n], XtNxRefName,
      getValue("Form1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "Form1");
   XtSetArg(args[n], XtNxOffset,
      getValue("8", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xOffset", "8");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNyRefName,
      getValue("StaticText2", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "StaticText2");
   XtSetArg(args[n], XtNyOffset,
      getValue("4", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yOffset", "4");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   StaticText3 = XtCreateManagedWidget("StaticText3", XwstatictextWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText3 StaticText Form1\n\n");
   flushEvents(delay);


   n = 0;
   XtSetArg(args[n], XtNheight,
      getValue("18", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "18");
   XtSetArg(args[n], XtNwidth,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "300");
   XtSetArg(args[n], XtNxRefName,
      getValue("StaticText3", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "StaticText3");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "True");
   XtSetArg(args[n], XtNxAttachOffset,
      getValue("4", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachOffset", "4");
   XtSetArg(args[n], XtNyRefName,
      getValue("TextEdit1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "TextEdit1");
   XtSetArg(args[n], XtNyOffset,
      getValue("6", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yOffset", "6");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   TextEdit2 = XtCreateManagedWidget("TextEdit2", XwtexteditWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  TextEdit2 TextEdit Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNsashIndent,
      getValue("-50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "sashIndent", "-50");
   XtSetArg(args[n], XtNframed,
      getValue("False", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "framed", "False");
   XtSetArg(args[n], XtNxRefName,
      getValue("Form1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "Form1");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "True");
   XtSetArg(args[n], XtNyRefName,
      getValue("StaticText3", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "StaticText3");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   VPanedWindow1 = XtCreateManagedWidget("VPanedWindow1", XwvPanedWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  VPanedWindow1 VPanedWindow Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("400", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "400");
   XtSetArg(args[n], XtNheight,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "150");
   XtSetArg(args[n], XtNlayout,
      getValue("maximize", "Layout"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "layout", "maximize");
   XtSetArg(args[n], XtNinitialX,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "initialX", "0");
   XtSetArg(args[n], XtNallowResize,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "allowResize", "True");
   XtSetArg(args[n], XtNallowResize,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "allowResize", "True");
   ScrolledWindow1 = XtCreateManagedWidget("ScrolledWindow1", XwswindowWidgetClass,
      VPanedWindow1, args, n);
   if (trace) printf("XtCreateManagedWidget:  ScrolledWindow1 ScrolledWindow VPanedWindow1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNheight,
      getValue("400", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "400");
   XtSetArg(args[n], XtNwidth,
      getValue("390", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "390");
   TextEdit3 = XtCreateManagedWidget("TextEdit3", XwtexteditWidgetClass,
      ScrolledWindow1, args, n);
   if (trace) printf("XtCreateManagedWidget:  TextEdit3 TextEdit ScrolledWindow1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetArg(args[n], XtNwidth,
      getValue("400", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "400");
   Form2 = XtCreateManagedWidget("Form2", XwformWidgetClass,
      VPanedWindow1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Form1 Form VPanedWidnow1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetArg(args[n], XtNhSpace,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "hSpace", "10");
   XtSetArg(args[n], XtNvSpace,
      getValue("5", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "vSpace", "5");
   XtSetArg(args[n], XtNgravity,
      getValue("WestGravity", "Gravity"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "gravity", "WestGravity");
   XtSetArg(args[n], XtNfont,
      getValue("fixed", "FontStruct"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "font", "fixed");
   XtSetArg(args[n], XtNstring,
      getValue("File:", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "File:");
   XtSetArg(args[n], XtNallowResize,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "allowResize", "True");
   StaticText4 = XtCreateManagedWidget("StaticText4", XwstatictextWidgetClass,
      Form2, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText4 StaticText Form2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNheight,
      getValue("18", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "18");
   XtSetArg(args[n], XtNwidth,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "100");
   XtSetArg(args[n], XtNxRefName,
      getValue("StaticText4", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "StaticText4");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNyRefName,
      getValue("Form2", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "Form2");
   XtSetArg(args[n], XtNyOffset,
      getValue("6", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yOffset", "6");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   TextEdit4 = XtCreateManagedWidget("TextEdit4", XwtexteditWidgetClass,
      Form2, args, n);
   if (trace) printf("XtCreateManagedWidget:  TextEdit4 TextEdit Form2\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("400", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "400");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNlayout,
      getValue("maximize", "Layout"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "layout", "maximize");
   XtSetArg(args[n], XtNxRefName,
      getValue("Form1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xRefName", "Form1");
   XtSetArg(args[n], XtNxAddWidth,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAddWidth", "True");
   XtSetArg(args[n], XtNxResizable,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xResizable", "True");
   XtSetArg(args[n], XtNxAttachRight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "xAttachRight", "True");
   XtSetArg(args[n], XtNyRefName,
      getValue("VPanedWindow1", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yRefName", "VPanedWindow1");
   XtSetArg(args[n], XtNyAddHeight,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "yAddHeight", "True");
   ScrolledWindow2 = XtCreateManagedWidget("ScrolledWindow2", XwswindowWidgetClass,
      Form1, args, n);
   if (trace) printf("XtCreateManagedWidget:  ScrolledWindow2 ScrolledWindow Form1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNheight,
      getValue("700", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "700");
   XtSetArg(args[n], XtNwidth,
      getValue("390", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "390");
   TextEdit5 = XtCreateManagedWidget("TextEdit5", XwtexteditWidgetClass,
      ScrolledWindow2, args, n);
   if (trace) printf("XtCreateManagedWidget:  TextEdit5 TextEdit ScrolledWindow2\n\n");
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
