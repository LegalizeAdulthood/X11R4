/*
 *	$Source: /site/mit/appdev1/roman/xcalendar/RCS/xcalendar.c,v $
 *	$Header: xcalendar.c,v 1.5 88/10/14 16:56:48 roman Locked $
 *      $Author: roman $
 *      $Locker: roman $
 *
 * 	Copyright (C) 1988 Massachusetts Institute of Technology	
 *	
 */

/*

   Permission to use, copy, modify, and distribute this
   software and its documentation for any purpose and without
   fee is hereby granted, provided that the above copyright
   notice appear in all copies and that both that copyright
   notice and this permission notice appear in supporting
   documentation, and that the name of M.I.T. not be used in
   advertising or publicity pertaining to distribution of the
   software without specific, written prior permission.
   M.I.T. makes no representations about the suitability of
   this software for any purpose.  It is provided "as is"
   without express or implied warranty.

*/


/** Author:  Roman J. Budzianowski - Project Athena, MIT **/
/** Version 1.4 beta **/

/**
  *  To do:
  *  1. Provide facility to browse through the calendar (next month , previous month)
  *  2. Make possible to remove all entries in a month
  *  3. Remove the limit on the size of the file (switch to DiskSource).
**/

#ifndef lint
static char *rcsid_calendar_c = "$Header: xcalendar.c,v 1.5 88/10/14 16:56:48 roman Locked $";
#endif lint

#include <stdio.h>
#include  <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Form.h>
#include <X11/Command.h>
#include <X11/Label.h>
#include <X11/AsciiText.h>
#include <X11/Shell.h>

#include "xcalendar.h"

Cardinal displayedMonth, displayedYear;

Widget    toplevel;
Display   *dsp;
int       debug = 0;
List      toplist, callist, daylist;	/* needed for handling geometry layout*/
void      FormGeometryHandler();
LList     *dayEntryList;

Cardinal  day,month,year;
Boolean   dayEntryState = False;
Arg       markedDayColors[2];
Arg       unmarkedDayColors[2];
void      ShowEntries();
Window    markedDaysKey;

XtActionsRec appActions[]= {
   {"ShowEntries", ShowEntries},
};

String defTranslations =
  "<Btn1Down>:ShowEntries()";

#define SetResOffset(n,field) \
  appResourceList[n].resource_offset = ((char *)&appResources.field - (char *)&appResources)


char	*smonth[13]= {
        NULL,	"January", "February", "March", "April",
	"May", "June", "July", "August",
	"September", "October", "November", "December",
};

Cardinal defFirstDay = 1;

AppResourcesRec appResources;	/* application specific resources */

/* add new resources to the end of the array */

XtResource appResourceList[] = {
   {"reverseVideoMark", "ReverseVideo", XtRBoolean, sizeof(Boolean),
      0,XtRString, "False"},
   {"setMarkForeground", "SetForeground", XtRBoolean, sizeof(Boolean),
      0,XtRString, "False"},
   {"markForeground", "MarkForeground", XtRPixel, sizeof(Pixel),
      0,XtRString,"Black"},
   {"setMarkBackground", "SetBackground", XtRBoolean, sizeof(Boolean),
      0,XtRString,"False"},
   {"markBackground", "MarkBackground", XtRPixel, sizeof(Pixel),
      0,XtRString,"White"},
   {"january","January",XtRString,sizeof(String),
      0,XtRString,"January"},
   {"february","February",XtRString,sizeof(String),
      0,XtRString,"February"},
   {"march","March",XtRString,sizeof(String),
      0,XtRString,"March"},
   {"april","April",XtRString,sizeof(String),
      0,XtRString,"April"},
   {"may","May",XtRString,sizeof(String),
      0,XtRString,"May"},
   {"june","June",XtRString,sizeof(String),
      0,XtRString,"June"},
   {"july","July",XtRString,sizeof(String),
      0,XtRString,"July"},
   {"august","August",XtRString,sizeof(String),
      0,XtRString,"August"},
   {"september","September",XtRString,sizeof(String),
      0,XtRString,"September"},
   {"october","October",XtRString,sizeof(String),
      0,XtRString,"October"},
   {"november","November",XtRString,sizeof(String),
      0,XtRString,"November"},
   {"december","December",XtRString,sizeof(String),
      0,XtRString,"December"},
   {"firstDay","FirstDay",XtRInt,sizeof(int),
      0,XtRInt,(caddr_t)&defFirstDay},
   {"markOnStartup","MarkOnStarup", XtRBoolean, sizeof(Boolean),
      0,XtRString,"False"},
   {"helpFile","HelpFile",XtRString,sizeof(String),
      0,XtRString,"/usr/lib/X11/xcalendar.hlp"},
   {"textBufferSize","TextBufferSize",XtRInt,sizeof(int),
      0,XtRString,"2048"},
};

Arg markColors[2];
Cardinal nMark = 0;

static XtCallbackRec callbacks[] = {
   {NULL,NULL},
   {NULL,NULL}
};

void quit(w, closure, call_data)
     Widget w;
     caddr_t closure;
     caddr_t call_data;
{
   CloseEditors();
   exit(0);
}

char *helpText = 
"\n\
The full text of help is not available, because the help file\n\
couldn't be opened. Set the resource 'helpFile' to\n\
the full pathname of the xcalendar.hlp on your system.\n\
\n\
XCalendar is a simple interactive calendar program with a notebook\n\
capability. It is build on the X Toolkit and the Athena Widgets.\n\
\n\
You can start the program as:  xcalendar [ month [year]]\n\
\n\
CREDITS:\n\
      Version 1.4 beta\n\
      Author: Roman J. Budzianowski - MIT Project Athena\n\
      Copyright 1988 Massachusetts Institute of Technology\n\
\n\
";

void help(w, closure, call_data)
     Widget w;
     caddr_t closure;
     caddr_t call_data;
{
   static DayEditor help = NULL;
   char             *iconName = "xcalendar: help";
   Arg              args[1];

   if(!help){
      help = CreateDayEditor("helpWindow",XttextRead,scrollVertical, NULL);
      help->button = w;
      help->used   = True;
      help->saved = True;
      help->filename = appResources.helpFile;

      if(ReadFile(help) == False || EmptyBuffer(help)){
	 free(help->buffer);
	 (void)ChangeTextSource(help,helpText);
      }

      XtUnmanageChild(help->saveButton);
      XtUnmanageChild(help->clearEntry);
   }

   XtSetArg(args[0],XtNiconName,iconName);
   XtSetValues(help->shell,args,1);
  
   StartEditor(help);

   XtSetSensitive(help->button,False);

}

void main(argc, argv)
    int argc;
    char **argv;
{
    Widget      frame, calendar, controls;
    Widget      button;
    Widget      CreateCalendarFrame();
    Cardinal    numberOfDays,firstDay;
    Arg         args[4];
    char        title[80];
    char        iconName[80];
    int         i, resNo = 0;

    toplevel = XtInitialize(NULL, "XCalendar", NULL, 0, &argc, argv);

    dsp = XtDisplay(toplevel);

/* get application specific resources*/

    /* names of months - default in english */
    appResources.months = &smonth[1];

/*  some compilers don't like XtOffset  */

    SetResOffset(resNo,reverseVideo);
    if(XtScreen(toplevel)->root_visual->class == StaticGray)
      appResourceList[resNo].default_addr = "True";
    resNo++;
    SetResOffset(resNo++,setForeground);
    SetResOffset(resNo++,markForeground);
    SetResOffset(resNo++,setBackground);
    SetResOffset(resNo++,markBackground);

    for(i=0;i<12;i++,resNo++)
      SetResOffset(resNo,months[i]);

    SetResOffset(resNo++,firstDay);
    SetResOffset(resNo++,markOnStartup);
    SetResOffset(resNo++,helpFile);
    SetResOffset(resNo++,textBufferSize);

    XtGetApplicationResources(toplevel,&appResources,appResourceList,
			      XtNumber(appResourceList),NULL,0);

    if(appResources.setBackground){
       XtSetArg(markColors[nMark],XtNbackground,appResources.markBackground);nMark++;
    }
    if(appResources.setForeground){
       XtSetArg(markColors[nMark],XtNforeground,appResources.markForeground);nMark++;
    }


    day   = current_day();
    month = current_month();
    year  = current_year(); 

    if(argc >= 2)
      month = atoi(argv[1]);

    if(argc == 3)
      year = atoi(argv[2]);

    displayedMonth = month;
    displayedYear  = year;

    sprintf(iconName,"xcalendar: %s %d",smonth[displayedMonth], displayedYear);

    XtSetArg(args[0],XtNiconName,iconName);
    XtSetValues(toplevel,args,1);

    if(month != current_month())
      sprintf(title,"%s %d", smonth[month], year);
    else
      sprintf(title,"%d %s %d", day, smonth[month], year);

    firstDay     = FirstDay(month,year);
    numberOfDays = NumberOfDays(month,year);

    /*
     * Create the top level form widget
     */

    frame = XtCreateWidget("xcalendar",formWidgetClass, toplevel, NULL, 0);

    markedDaysKey = XtWindow(frame);

    /* list of widgets in the frame widget */
    toplist = CreateList(2,sizeof(Widget));

    /** create control panel **/

    XtSetArg(args[0],XtNresizable,True);
    controls = XtCreateManagedWidget("controls", formWidgetClass, frame, args, 1);

    PushWidgetOnList(toplist,controls);

    /* quit button */

    callbacks[0].callback = quit;
    XtSetArg(args[0],XtNcallback,callbacks);
    XtSetArg(args[1],XtNfromHoriz,NULL);
    XtSetArg(args[2],XtNleft,XtChainLeft);
    XtSetArg(args[3],XtNright,XtChainLeft);

    button = XtCreateManagedWidget("quitButton", commandWidgetClass, controls, args, 4);

    /* date label - maybe put xclock here */

    XtSetArg(args[0],XtNlabel,title);
    XtSetArg(args[1],XtNfromHoriz,button);
    XtSetArg(args[2],XtNleft,XtChainLeft);
    XtSetArg(args[3],XtNright,XtChainRight);

    button = XtCreateManagedWidget("date", labelWidgetClass, controls, args, 4);

    XtAddActions(appActions,1);	/* register actions */
    
    XtAugmentTranslations(button,XtParseTranslationTable(defTranslations));
     
    /* help button */

    callbacks[0].callback = help;
    XtSetArg(args[0],XtNcallback,callbacks);
    XtSetArg(args[1],XtNfromHoriz,button);
    XtSetArg(args[2],XtNleft,XtChainRight);
    XtSetArg(args[3],XtNright,XtChainRight);

    button = XtCreateManagedWidget("helpButton", commandWidgetClass, controls, args, 4);
    
/** end of control panel code **/

/** create calendar form **/

    XtSetArg(args[0],XtNfromVert,controls);
    XtSetArg(args[1],XtNresizable,True);
    calendar = CreateCalendarFrame(frame,args,2,numberOfDays,firstDay);
    PushWidgetOnList(toplist,calendar);

/** end of top level form **/

    XtManageChild(frame);

    /*
     * Create the windows
     */
    XtRealizeWidget(toplevel);

/** 
    now we have to fix the geometry because the form widget doesn't do
    what we want, that is to make all widgets of the same width

    XtAddRawEventHandler should be enough but it doesn't work in R2
**/

    XtAddEventHandler(toplevel,StructureNotifyMask,True,
			 FormGeometryHandler,NULL);

/** mark the entries **/

    if(appResources.markOnStartup == True)
      ShowEntries(toplevel,NULL,NULL,0);

    /*
     * Now process the events.
     */
    XtMainLoop();
}


/* This solution has one potential problem: its success depends on the sequence the 
   handlers are called. It works now, when the original hanler is called first,
   so we come here after the toplevel has already handled MapNotify */

void FormGeometryHandler(w,data,event)
     Widget w;
     caddr_t data;
     XEvent *event;
{
   if(event->type==MapNotify){
      FixFormGeometry(callist,XtNwidth);
      FreeList(callist);

      FixFormGeometry(toplist,XtNwidth); 
      FreeList(toplist);

      XtRemoveEventHandler(w,XtAllEvents,True,FormGeometryHandler,data);
   }
}      

FixFormGeometry(widgetlist,what)
     List widgetlist;
     char *what;
{
   Dimension borderWidth;
   Dimension value, max = 0;
   Arg args[4];
   int i;

   if(strcmp(what,XtNwidth) && strcmp(what,XtNheight))
     return;

   XtSetArg(args[0],XtNborderWidth,&borderWidth);
   XtSetArg(args[1],what,&value);

    for(i=1;i<=ListLength(widgetlist);i++){
       XtGetValues(GetWidgetFromList(widgetlist,i),args,2);
       value += 2*borderWidth;
       if(max < value) max = value;
    }

    for(i=1;i<=ListLength(widgetlist);i++){
       XtSetArg(args[0],XtNborderWidth,&borderWidth);
       XtGetValues(GetWidgetFromList(widgetlist,i),args,1);
       value  = max - 2*borderWidth;

       XtSetArg(args[0],XtNwidth,value);
       XtSetValues(GetWidgetFromList(widgetlist,i),args,1); 
    }
}

void ShowEntries(w,event,params,numb)
     Widget w;
     XEvent *event;
     String *params;
     Cardinal *numb;
{
   LList *listp;
   Arg colors[2];
   Arg *args = colors;
   Cardinal n = 2;

   if(!(initialized || InitEditors()))
     return;

   listp = dayEntryList;

   if(dayEntryList)
     do{
	if(dayEntryState){
	   XtSetArg(colors[0],XtNbackground,DayBackground(listp));
	   XtSetArg(colors[1],XtNforeground,DayForeground(listp));
	}
	else
	  {
	     if(appResources.reverseVideo){
		XtSetArg(colors[0],XtNbackground,DayForeground(listp));
		XtSetArg(colors[1],XtNforeground,DayBackground(listp));
	     }
	     else{
		args = markColors; n = nMark;
	     }
	  }
	XtSetValues((Widget)listp->element,args,n);
	listp = listp->next;

     }while(listp!=dayEntryList);

   dayEntryState = !dayEntryState;

}

MarkDayEntry(w,doFlag)
     Widget w;
     Boolean   doFlag;
{
   Arg colors[2];
   LList *lp;

   if(doFlag == True){
      if(lookup(w,markedDaysKey,dayEntryList) != NULL) return; /* it's already on the list */
      dayEntryList = (LList *)pput(w,markedDaysKey,dayEntryList);
      dayEntryList->data = XtMalloc(sizeof(MarkColors));
      XtSetArg(colors[0],XtNbackground,&DayBackground(dayEntryList));
      XtSetArg(colors[1],XtNforeground,&DayForeground(dayEntryList));
      XtGetValues(w,colors,2);

      if(dayEntryState == True){
	 if(appResources.reverseVideo){
	    XtSetArg(colors[0],XtNbackground,DayForeground(dayEntryList));
	    XtSetArg(colors[1],XtNforeground,DayBackground(dayEntryList));
	    XtSetValues(w,colors,2);
	 }
	 else
	   XtSetValues(w,markColors,nMark);
      }
   }
   else{
      if((lp=(LList *)lookup(w,markedDaysKey,dayEntryList)) == NULL) 
	return; /* it's not on the list */
      if(dayEntryState == True){
	 dayEntryList = lp;
	 XtSetArg(colors[0],XtNbackground,DayBackground(dayEntryList));
	 XtSetArg(colors[1],XtNforeground,DayForeground(dayEntryList));
	 XtSetValues(w,colors,2);
      }
      XtFree(dayEntryList->data);
      dayEntryList = (LList *)premove(w,markedDaysKey,dayEntryList);
   }
}

Widget CreateCalendarFrame(parent,args,nargs,numDays,firstDay)
     Widget     parent;
     Cardinal   nargs;
     ArgList    args;
     Cardinal   numDays,firstDay;
{
   Widget calendar, CreateDayLabels(), CreateDayNumbers();
   Widget widget;
   Arg    bargs[2];

   callist = CreateList(2,sizeof(Widget));

   calendar = XtCreateManagedWidget("calendar", formWidgetClass, parent, args, nargs);

   XtSetArg(bargs[0],XtNfromVert,NULL);
   widget = CreateDayLabels(calendar,bargs,1);
   PushWidgetOnList(callist,widget);

   XtSetArg(bargs[0],XtNfromVert,widget);
   XtSetArg(bargs[1],XtNresizable,True);
   widget = CreateDayNumbers(calendar,bargs,2,numDays,firstDay);
   PushWidgetOnList(callist,widget);

   return calendar;

}

Widget CreateDayNumbers(parent,args,nargs,numDays,firstDay)
     Widget     parent;
     ArgList    args;
     Cardinal   nargs;
     Cardinal   numDays,firstDay;
{
   Widget   daynumbers;
   Widget   prevButton=NULL;
   Cardinal numButtons;
   Arg      bargs[6];
   int      i;
   char     buttonText[4];
   char     buttonName[8];
   void     EditDayEntry();

   /* end of declarations */

   if(firstDay + numDays -1 > 35)
     numButtons = 42;
   else
     numButtons = 35;

   daylist = CreateList(numButtons,sizeof(Widget));

   daynumbers = XtCreateManagedWidget("daynumbers", formWidgetClass, parent, args, nargs);

   callbacks[0].callback    = EditDayEntry;

   XtSetArg(bargs[0],XtNcallback,callbacks);

   for(i=1; i <= numButtons; i++){

      if(i < firstDay || i > firstDay + numDays -1){
	 XtSetArg(bargs[1],XtNmappedWhenManaged,False);
	 callbacks[0].closure     = NULL;
      }	 
      else{
	 XtSetArg(bargs[1],XtNmappedWhenManaged,True);
	 callbacks[0].closure = (caddr_t)(i - firstDay +1);
      }
      
      if( (i-1)%7 == 0 )
	XtSetArg(bargs[2],XtNfromHoriz,NULL);
      else
	XtSetArg(bargs[2],XtNfromHoriz,prevButton);

      if( i <= 7 )
	XtSetArg(bargs[3],XtNfromVert,NULL);
      else
	XtSetArg(bargs[3],XtNfromVert,GetWidgetFromList(daylist,i-7));

      sprintf(buttonText,"%3.d",i - firstDay + 1);
      XtSetArg(bargs[4],XtNlabel,buttonText);

/**       sprintf(buttonName,"%d",(i-1)%7 + 1);  **/
      sprintf(buttonName,"%d",i);

      prevButton = XtCreateManagedWidget(buttonName,commandWidgetClass, daynumbers, bargs, 5);

      (void)PushWidgetOnList(daylist,prevButton);
   }
   
   return daynumbers;

}

Widget CreateDayLabels(parent,args,nargs)
     Widget    parent;
     ArgList   args;
     Cardinal  nargs;
{
   Widget  prevButton=NULL, daynames;
   Arg     bargs[3];
   int     i,day;
   static char *names[8]=
     {NULL,"MON","TUE","WED","THU","FRI","SAT","SUN"};

   /* end of declarations */

   daynames = XtCreateManagedWidget("daynames", formWidgetClass, parent, args, nargs);

   for(i=1; i <= 7; i++){
      XtSetArg(bargs[0],XtNfromHoriz,prevButton);
      day = (i+appResources.firstDay -1)%7;
      day = day > 0 ? day : 7;
      prevButton = XtCreateManagedWidget(names[day],
				  labelWidgetClass, daynames, bargs, 1);
   }

   return daynames;

}

#include <sys/time.h>

static struct tm *today = NULL;;

struct tm *gettime()
{
   struct timeval t;
   struct tm *localtime();

   gettimeofday(&t,NULL);
   today = localtime(&t.tv_sec);
   return today;
}

int current_month()
{
   if(today == NULL)
     (void)gettime();
   return(today->tm_mon + 1);
}

int current_day()
{
   if(today == NULL)
     (void)gettime();
   return(today->tm_mday);
}   

int current_year()
{
   if(today == NULL)
     (void)gettime();
   return(today->tm_year + 1900);
}   

/**/

/* taken from cal.c */

char	mon[] = {
   0,
   31, 29, 31, 30,
   31, 30, 31, 31,
   30, 31, 30, 31,
};

static int calInit = 0;

NumberOfDays(m,y)
     int m,y;
{
   if(calInit != y)
     FirstDay(m,y);
   return mon[m];
}

/* should be called first */
FirstDay(m, y)
     int m,y;
{
   register d, i;
   
   calInit = y;
   d = jan1(y);
   mon[2] = 29;
   mon[9] = 30;
   
   switch((jan1(y+1)+7-d)%7) {
      
      /*
       *	non-leap year
       */
    case 1:
      mon[2] = 28;
      break;
      
      /*
       *	1752
       */
    default:
      mon[9] = 19;
      break;

      /*
       *	leap year
       */
    case 2:
      ;
   }
   
   for(i=1; i<m; i++)
     d += mon[i];
   d %= 7;
   
   d = d>0 ? d : 7;		/* returns 1-7, not 0-6 */

   d = d + 1 - appResources.firstDay;
   return d>0 ? d : 7+d;

}
/*
 *	return day of the week
 *	of jan 1 of given year
 */

jan1(yr)
{
	register y, d;

/*
 *	normal gregorian calendar
 *	one extra day per four years
 */

	y = yr;
	d = 4+y+(y+3)/4;

/*
 *	julian calendar
 *	regular gregorian
 *	less three days per 400
 */

	if(y > 1800) {
		d -= (y-1701)/100;
		d += (y-1601)/400;
	}

/*
 *	great calendar changeover instant
 */

	if(y > 1752)
		d += 3;

	return(d%7);
}
