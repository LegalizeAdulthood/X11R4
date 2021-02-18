

/****************************************
*   Automatic test program generation   *
*       Thu Jul  7 12:46:43 1988        *
****************************************/

#include "bull2.h"

void main (argc, argv)
int argc;
char **argv;
{
   extern XtArgVal getValue();
   extern void getOptions();
   extern void flushEvents();
   extern void myQuit();

   signal (SIGINT, myQuit);

   Shell1 = XtInitialize (argv[0], "bull2.defs", NULL, 0, &argc, argv);
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
   XtSetArg(args[n], XtNwidth,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "300");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   BulletinBoard1 = XtCreateManagedWidget("BulletinBoard1", XwbulletinWidgetClass,
      Shell1, args, n);
   if (trace) printf("XtCreateManagedWidget:  BulletinBoard1 BulletinBoard Shell1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "300");
   XtSetArg(args[n], XtNheight,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "250");
   XtSetValues(BulletinBoard1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "BulletinBoard1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNwidth,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "250");
   XtSetArg(args[n], XtNheight,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "250");
   XtSetValues(BulletinBoard1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "BulletinBoard1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalOn,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalOn", "True");
   XtSetArg(args[n], XtNwidth,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "250");
   XtSetArg(args[n], XtNheight,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "250");
   XtSetValues(BulletinBoard1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "BulletinBoard1");
   flushEvents(delay);

   n = 0;
   Toggle1 = XtCreateManagedWidget("Toggle1", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle1 Toggle BulletinBoard1\n\n");
   flushEvents(delay);

   n = 0;
   Toggle2 = XtCreateManagedWidget("Toggle2", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle2 Toggle BulletinBoard1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "200");
   XtSetArg(args[n], XtNy,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "200");
   XtSetValues(Toggle2, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle2");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNy,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "200");
   Toggle3 = XtCreateManagedWidget("Toggle3", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle3 Toggle BulletinBoard1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "200");
   Toggle4 = XtCreateManagedWidget("Toggle4", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle4 Toggle BulletinBoard1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "300");
   XtSetArg(args[n], XtNy,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "100");
   Toggle5 = XtCreateManagedWidget("Toggle5", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle5 Toggle BulletinBoard1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalOn,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalOn", "True");
   XtSetArg(args[n], XtNlayout,
      getValue("ignore", "Layout"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "layout", "ignore");
   XtSetArg(args[n], XtNwidth,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "250");
   XtSetArg(args[n], XtNheight,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "250");
   XtSetValues(BulletinBoard1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "BulletinBoard1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "300");
   XtSetArg(args[n], XtNy,
      getValue("150", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "150");
   Toggle6 = XtCreateManagedWidget("Toggle6", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle6 Toggle BulletinBoard1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalOn,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalOn", "True");
   XtSetArg(args[n], XtNlayout,
      getValue("maximize", "Layout"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "layout", "maximize");
   XtSetArg(args[n], XtNwidth,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "250");
   XtSetArg(args[n], XtNheight,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "250");
   XtSetValues(BulletinBoard1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "BulletinBoard1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("350", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "350");
   XtSetArg(args[n], XtNy,
      getValue("125", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "125");
   Toggle7 = XtCreateManagedWidget("Toggle7", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle7 Toggle BulletinBoard1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("350", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "350");
   XtSetArg(args[n], XtNy,
      getValue("125", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "125");
   XtSetArg(args[n], XtNwidth,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "200");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetValues(Toggle2, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle2");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("350", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "350");
   XtSetArg(args[n], XtNy,
      getValue("125", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "125");
   XtSetArg(args[n], XtNwidth,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "200");
   XtSetArg(args[n], XtNheight,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "200");
   XtSetArg(args[n], XtNborderWidth,
      getValue("1", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "1");
   XtSetValues(Toggle2, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle2");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("350", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "350");
   XtSetArg(args[n], XtNy,
      getValue("125", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "125");
   XtSetArg(args[n], XtNwidth,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "200");
   XtSetArg(args[n], XtNheight,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "100");
   XtSetArg(args[n], XtNborderWidth,
      getValue("1", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "1");
   XtSetValues(Toggle2, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle2");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "200");
   XtSetArg(args[n], XtNy,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "200");
   XtSetArg(args[n], XtNwidth,
      getValue("200", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "200");
   XtSetArg(args[n], XtNheight,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "100");
   XtSetArg(args[n], XtNborderWidth,
      getValue("1", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "1");
   XtSetValues(Toggle2, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle2");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNtraversalOn,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalOn", "True");
   XtSetArg(args[n], XtNlayout,
      getValue("minimize", "Layout"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "layout", "minimize");
   XtSetArg(args[n], XtNwidth,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "250");
   XtSetArg(args[n], XtNheight,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "250");
   XtSetValues(BulletinBoard1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "BulletinBoard1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "300");
   XtSetArg(args[n], XtNy,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "300");
   XtSetArg(args[n], XtNwidth,
      getValue("500", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "500");
   XtSetArg(args[n], XtNheight,
      getValue("20", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "20");
   XtSetArg(args[n], XtNborderWidth,
      getValue("1", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "1");
   Toggle8 = XtCreateManagedWidget("Toggle8", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle8 Toggle BulletinBoard1\n\n");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "300");
   XtSetArg(args[n], XtNy,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "300");
   XtSetArg(args[n], XtNwidth,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "100");
   XtSetArg(args[n], XtNheight,
      getValue("21", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "21");
   XtSetArg(args[n], XtNborderWidth,
      getValue("1", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "1");
   XtSetValues(Toggle8, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle8");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "300");
   XtSetArg(args[n], XtNy,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "300");
   XtSetArg(args[n], XtNwidth,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "100");
   XtSetArg(args[n], XtNheight,
      getValue("21", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "21");
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetValues(Toggle8, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle8");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("300", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "300");
   XtSetArg(args[n], XtNy,
      getValue("310", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "310");
   XtSetArg(args[n], XtNwidth,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "100");
   XtSetArg(args[n], XtNheight,
      getValue("21", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "21");
   XtSetArg(args[n], XtNborderWidth,
      getValue("0", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "0");
   XtSetValues(Toggle8, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "Toggle8");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNborderWidth,
      getValue("10", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "borderWidth", "10");
   XtSetArg(args[n], XtNtraversalOn,
      getValue("True", "Boolean"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "traversalOn", "True");
   XtSetArg(args[n], XtNlayout,
      getValue("minimize", "Layout"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "layout", "minimize");
   XtSetArg(args[n], XtNwidth,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "250");
   XtSetArg(args[n], XtNheight,
      getValue("250", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "250");
   XtSetValues(BulletinBoard1, args, n);
   if (trace) printf("XtSetValues:  %s\n\n", "BulletinBoard1");
   flushEvents(delay);

   n = 0;
   XtSetArg(args[n], XtNx,
      getValue("350", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "x", "350");
   XtSetArg(args[n], XtNy,
      getValue("350", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "y", "350");
   XtSetArg(args[n], XtNwidth,
      getValue("100", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "width", "100");
   XtSetArg(args[n], XtNheight,
      getValue("21", "Int"));  n++;
   if (trace) printf("XtSetArg:  XtN%s %s\n", "height", "21");
   Toggle9 = XtCreateManagedWidget("Toggle9", XwtoggleWidgetClass,
      BulletinBoard1, args, n);
   if (trace) printf("XtCreateManagedWidget:  Toggle9 Toggle BulletinBoard1\n\n");
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
