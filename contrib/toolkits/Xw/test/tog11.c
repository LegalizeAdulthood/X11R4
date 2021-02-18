

/****************************************
*   Automatic test program generation   *
*       Thu Aug  4 15:38:50 1988        *
****************************************/

#include "tog11.h"

void main (argc, argv)
int argc;
char **argv;
{
   extern XtArgVal getValue();
   extern void getOptions();
   extern void flushEvents();
   extern void myQuit();

   signal(SIGINT, myQuit);

   Shell1 = XtInitialize (argv[0], "tog11.defs", NULL, 0, &argc, argv);
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
   XtSetArg(args[n], XtNtraversalOn,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalOn", "True");
   XtSetArg(args[n], XtNcolumns,
      getValue("2", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "columns", "2");
   RowCol1 = XtCreateManagedWidget("RowCol1", XwrowColWidgetClass,
      Shell1, args, n);
   if (trace) printf("XtCreateManagedWidget:  RowCol1 RowCol Shell1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Blue");
   XtSetArg(args[n], XtNselectColor,
      getValue("blue", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "blue");
   Toggle1 = XtCreateManagedWidget("Toggle1", XwtoggleWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle1 Toggle RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Red");
   XtSetArg(args[n], XtNselectColor,
      getValue("red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "red");
   Toggle2 = XtCreateManagedWidget("Toggle2", XwtoggleWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle2 Toggle RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNselectColor,
      getValue("red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "red");
   Toggle3 = XtCreateManagedWidget("Toggle3", XwtoggleWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle3 Toggle RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNselectColor,
      getValue("red", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "red");
   Toggle4 = XtCreateManagedWidget("Toggle4", XwtoggleWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle4 Toggle RowCol1\n\n");
   flushEvents(delay);


   n = 0;
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Green");
   XtSetArg(args[n], XtNselectColor,
      getValue("green", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "green");
   XtSetValues(Toggle3, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle3");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Yellow");
   XtSetArg(args[n], XtNselectColor,
      getValue("yellow", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "yellow");
   XtSetValues(Toggle3, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle3");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalType,
      getValue("highlight_traversal", "TraversalType"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalType", "highlight_traversal");
   XtSetArg(args[n], XtNhighlightThickness,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightThickness", "3");
   XtSetArg(args[n], XtNhighlightColor,
      getValue("Navy", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "highlightColor", "Navy");
   XtSetArg(args[n], XtNlabelLocation,
      getValue("center", "LabelLocation"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "labelLocation", "center");
   XtSetArg(args[n], XtNselectColor,
      getValue("violet", "Pixel"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "selectColor", "violet");
   Toggle5 = XtCreateManagedWidget("Toggle5", XwtoggleWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle5 Toggle RowCol1\n\n");
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

