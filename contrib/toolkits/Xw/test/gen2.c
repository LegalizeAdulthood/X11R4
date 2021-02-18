

/****************************************
*   Automatic test program generation   *
*       Thu Jul 14 11:35:36 1988        *
****************************************/

#include "gen2.h"

void main (argc, argv)
int argc;
char **argv;
{
   extern XtArgVal getValue();
   extern void getOptions();
   extern void flushEvents();
   extern void myQuit();

   signal (SIGINT, myQuit);

   Shell1 = XtInitialize (argv[0], "gen2.defs", NULL, 0, &argc, argv);
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
   XtSetArg(args[n], XtNtraversalOn,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalOn", "True");
   XtSetArg(args[n], XtNcolumns,
      getValue("4", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "columns", "4");
   XtSetValues(RowCol1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "RowCol1");
   flushEvents(delay);

   n = 0;
   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "10");
   Arrow1 = XtCreateManagedWidget("Arrow1", XwarrowWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Arrow1 Arrow RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("15", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "15");
   XtSetValues(Arrow1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Arrow1");
   flushEvents(delay);

   n = 0;
   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "10");
   PushButton1 = XtCreateManagedWidget("PushButton1", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton1 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("15", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "15");
   XtSetArg(args[n], XtNborderWidth,
      getValue("1", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "1");
   XtSetValues(PushButton1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "PushButton1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNsliderMin,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "sliderMin", "10");
   ScrollBar1 = XtCreateManagedWidget("ScrollBar1", XwscrollbarWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  ScrollBar1 ScrollBar RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNsliderMin,
      getValue("15", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "sliderMin", "15");
   XtSetValues(ScrollBar1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "ScrollBar1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNsliderMin,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "sliderMin", "0");
   XtSetValues(ScrollBar1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "ScrollBar1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "10");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   StaticText1 = XtCreateManagedWidget("StaticText1", XwstatictextWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  StaticText1 StaticText RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("15", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "15");
   XtSetArg(args[n], XtNstring,
      getValue("static", "String"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "string", "static");
   XtSetValues(StaticText1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "StaticText1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("5", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "5");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   TextEdit1 = XtCreateManagedWidget("TextEdit1", XwtexteditWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  TextEdit1 TextEdit RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("5", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "5");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetValues(TextEdit1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "TextEdit1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "10");
   XtSetArg(args[n], XtNheight,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "30");
   XtSetValues(TextEdit1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "TextEdit1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "10");
   Toggle1 = XtCreateManagedWidget("Toggle1", XwtoggleWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle1 Toggle RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("15", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "15");
   XtSetValues(Toggle1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "10");
   XtSetValues(Toggle1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("15", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "15");
   XtSetValues(Toggle1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "10");
   Valuator1 = XtCreateManagedWidget("Valuator1", XwvaluatorWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Valuator1 Valuator RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("15", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "15");
   XtSetValues(Valuator1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Valuator1");
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
