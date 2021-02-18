

/****************************************
*   Automatic test program generation   *
*       Thu Jul 14 11:04:00 1988        *
****************************************/

#include "vpw13.h"

void main (argc, argv)
int argc;
char **argv;
{
   extern XtArgVal getValue();
   extern void getOptions();
   extern void flushEvents();
   extern void myQuit();

   signal (SIGINT, myQuit);

   Shell1 = XtInitialize (argv[0], "vpw13.defs", NULL, 0, &argc, argv);
   XwRegisterConverters();
   display = XtDisplay(Shell1);
/*   XSynchronize(display, 1);   */
   getOptions(argc, argv, &delay, &trace);

   n = 0;
   XtSetArg(args[n], XtNwidth, 300);  n++;
   XtSetArg(args[n], XtNheight, 300);  n++;
   XtSetArg(args[n], XtNallowShellResize, True);  n++;
   XtSetValues(Shell1, args, n);

   n = 0;
   VPanedWindow1 = XtCreateManagedWidget("VPanedWindow1", XwvPanedWidgetClass,
      Shell1, args, n);
   if (trace) printf("XtCreateManagedWidget:  VPanedWindow1 VPanedWindow Shell1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhSpace,
      getValue("8", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "hSpace", "8");
   XtSetArg(args[n], XtNvSpace,
      getValue("8", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "vSpace", "8");
   XtSetArg(args[n], XtNcolumns,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "columns", "3");
   XtSetArg(args[n], XtNallowResize,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "allowResize", "True");
   RowCol1 = XtCreateManagedWidget("RowCol1", XwrowColWidgetClass,
      VPanedWindow1, args, n);
   if (trace) printf("XtCreateManagedWidget:  RowCol1 RowCol VPanedWindow1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhSpace,
      getValue("8", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "hSpace", "8");
   XtSetArg(args[n], XtNvSpace,
      getValue("8", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "vSpace", "8");
   XtSetArg(args[n], XtNcolumns,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "columns", "3");
   XtSetArg(args[n], XtNallowResize,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "allowResize", "True");
   XtSetArg(args[n], XtNmin,
      getValue("30", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "min", "30");
   RowCol2 = XtCreateManagedWidget("RowCol2", XwrowColWidgetClass,
      VPanedWindow1, args, n);
   if (trace) printf("XtCreateManagedWidget:  RowCol2 RowCol VPanedWindow1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNhSpace,
      getValue("8", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "hSpace", "8");
   XtSetArg(args[n], XtNvSpace,
      getValue("8", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "vSpace", "8");
   XtSetArg(args[n], XtNcolumns,
      getValue("3", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "columns", "3");
   XtSetArg(args[n], XtNallowResize,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "allowResize", "True");
   XtSetArg(args[n], XtNmax,
      getValue("50", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "max", "50");
   RowCol3 = XtCreateManagedWidget("RowCol3", XwrowColWidgetClass,
      VPanedWindow1, args, n);
   if (trace) printf("XtCreateManagedWidget:  RowCol3 RowCol VPanedWindow1\n\n");
   flushEvents(delay);

   n = 0;
   PushButton1 = XtCreateManagedWidget("PushButton1", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton1 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   PushButton2 = XtCreateManagedWidget("PushButton2", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton2 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   PushButton3 = XtCreateManagedWidget("PushButton3", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton3 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   PushButton4 = XtCreateManagedWidget("PushButton4", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton4 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   PushButton5 = XtCreateManagedWidget("PushButton5", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton5 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   PushButton6 = XtCreateManagedWidget("PushButton6", XwpushButtonWidgetClass,
      RowCol1, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton6 PushButton RowCol1\n\n");
   flushEvents(delay);

   n = 0;
   PushButton7 = XtCreateManagedWidget("PushButton7", XwpushButtonWidgetClass,
      RowCol2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton7 PushButton RowCol2\n\n");
   flushEvents(delay);

   n = 0;
   PushButton8 = XtCreateManagedWidget("PushButton8", XwpushButtonWidgetClass,
      RowCol2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton8 PushButton RowCol2\n\n");
   flushEvents(delay);

   n = 0;
   PushButton9 = XtCreateManagedWidget("PushButton9", XwpushButtonWidgetClass,
      RowCol2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton9 PushButton RowCol2\n\n");
   flushEvents(delay);

   n = 0;
   PushButton10 = XtCreateManagedWidget("PushButton10", XwpushButtonWidgetClass,
      RowCol2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton10 PushButton RowCol2\n\n");
   flushEvents(delay);

   n = 0;
   PushButton11 = XtCreateManagedWidget("PushButton11", XwpushButtonWidgetClass,
      RowCol2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton11 PushButton RowCol2\n\n");
   flushEvents(delay);

   n = 0;
   PushButton12 = XtCreateManagedWidget("PushButton12", XwpushButtonWidgetClass,
      RowCol2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton12 PushButton RowCol2\n\n");
   flushEvents(delay);

   n = 0;
   PushButton13 = XtCreateManagedWidget("PushButton13", XwpushButtonWidgetClass,
      RowCol2, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton13 PushButton RowCol2\n\n");
   flushEvents(delay);

   n = 0;
   PushButton14 = XtCreateManagedWidget("PushButton14", XwpushButtonWidgetClass,
      RowCol3, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton14 PushButton RowCol3\n\n");
   flushEvents(delay);

   n = 0;
   PushButton15 = XtCreateManagedWidget("PushButton15", XwpushButtonWidgetClass,
      RowCol3, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton15 PushButton RowCol3\n\n");
   flushEvents(delay);

   n = 0;
   PushButton16 = XtCreateManagedWidget("PushButton16", XwpushButtonWidgetClass,
      RowCol3, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton16 PushButton RowCol3\n\n");
   flushEvents(delay);

   n = 0;
   PushButton17 = XtCreateManagedWidget("PushButton17", XwpushButtonWidgetClass,
      RowCol3, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton17 PushButton RowCol3\n\n");
   flushEvents(delay);

   n = 0;
   PushButton18 = XtCreateManagedWidget("PushButton18", XwpushButtonWidgetClass,
      RowCol3, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton18 PushButton RowCol3\n\n");
   flushEvents(delay);

   n = 0;
   PushButton19 = XtCreateManagedWidget("PushButton19", XwpushButtonWidgetClass,
      RowCol3, args, n);
   if (trace) printf("XtCreateManagedWidget:  PushButton19 PushButton RowCol3\n\n");
   flushEvents(delay);

   XtRealizeWidget(Shell1);
   if (trace) {
      printf("\nXtInitialize:  Shell1 TopLevelShell\n\n");
      printf("XtSetArg:  XtNwidth 400\n");
      printf("XtSetArg:  XtNheight 300\n");
      printf("XtSetArg:  XtNallowShellResize\n");
      printf("XtSetValues:  Shell1\n");
      printf("XtRealizeWidget:  Shell1\n\n");
   }
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
     printf("Begin exiting ... please standby ... n");
     fflush(stdout);
     exit(0);
}
