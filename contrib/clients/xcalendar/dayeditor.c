/*
 *	$Source: /site/mit/appdev1/roman/xcalendar/RCS/dayeditor.c,v $
 *	$Header: dayeditor.c,v 1.5 88/10/14 16:57:14 roman Locked $
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

/** Written by Roman J. Budzianowski - Project Athena, MIT **/

#ifndef lint
static char *rcsid_dayeditor_c = "$Header: dayeditor.c,v 1.5 88/10/14 16:57:14 roman Locked $";
#endif lint

#include <stdio.h>
#include <X11/Xos.h>
#include <X11/Xlib.h>
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Form.h>
#include <X11/Command.h>
#include <X11/Label.h>
#include <X11/Box.h>
#include <X11/Shell.h>
#include <X11/AsciiText.h>
#include <X11/VPaned.h>

#include	<sys/dir.h>

#include "xcalendar.h"

static XtCallbackRec callbacks[] = {
   {NULL,NULL},
   {NULL,NULL}
};

List            editorList  = NULL;
Boolean         initialized = False;
char            *homedir    = NULL;
char            *calendarDir;

extern Cardinal displayedMonth, displayedYear;
void SaveButtonHandler();

Boolean InitEditors()
{
   char                 *getenv();
   DIR                  *dirp;
   struct direct	*dp;
   char                 yearStr[5];
   Cardinal             firstDay;

   sprintf(yearStr,"%d",displayedYear);
   firstDay = FirstDay(month,year);

   /* open the ~/Calendar directory - create if necessary */
   homedir = getenv("HOME");

   calendarDir = XtMalloc(strlen(homedir) + 10);
   strcpy(calendarDir,homedir);
   strcat(strcat(calendarDir,"/"),"Calendar");;

   if	((dirp =(DIR *) opendir(homedir)) == NULL) {
      perror("xcalendar:opendir:");
      fprintf(stderr,"cannot open home directoryr(%s).\n",homedir);
      return False;
   }
   
   while	((dp = readdir(dirp)) != NULL)
     if (strcmp(dp->d_name,"Calendar") == 0 ) /* found */
       break;

   if(dp == NULL){		/* not found */
      /* it should be a popup asking the user ...*/
      fprintf(stderr,"%s doesn't exist : Creating...",calendarDir);

      if( mkdir(calendarDir,0700) == -1 ){
	 XBell( XtDisplay(toplevel) ,100);
	 fprintf(stderr,"Couldn't create %s directory.\n");
	 perror("xcalendar:mkdir:");
	 fflush(stderr);
	 return False;
      }
   }
    
   /* it's there */

   closedir(dirp);

   chdir(calendarDir);

   dirp = (DIR *) opendir(calendarDir);

   while	((dp = readdir(dirp)) != NULL){
      char *s;
      /* find entries from the displayedMonth and store them in a list */

      s = (char *)index(dp->d_name,*smonth[displayedMonth]);

      if (s != NULL && !strncmp(s+3,yearStr,4) && !strncmp(s,smonth[displayedMonth],3))
	MarkDayEntry(GetWidgetFromList(daylist,atoi(dp->d_name + 2) + firstDay -1),True);
   }
 
   if(!editorList)
     editorList = CreateList(5,sizeof(DayEditor));

   initialized = True;
   return True;
}

void EditDayEntry(w, closure, call_data)
       Widget w;
       caddr_t closure;
       caddr_t call_data;
{
  int i;
  DayEditor editor = NULL;
  Arg       args[1];
  char      title[60];
  Boolean   success;
  char      mon[4];

  if(!initialized){
     success = InitEditors();
     if(!success)
       return;
  }

  /* find free editor */

  for(i=1;i <= ListLength(editorList); i++){
    editor = GetEditorFromList(editorList,i);
    if(editor->used)
      editor = NULL;
    else
      break;
 }

  if(editor == NULL){
     editor = CreateDayEditor("dayEditor",XttextEdit, wordBreak, NULL);
     PushEditorOnList(editorList,editor);
  }

  editor->button = w;
  editor->day    = (int)closure;
  editor->month  = displayedMonth;
  editor->year   = displayedYear;
  editor->saved  = True;

  editor->filename = XtMalloc(12);
  strncpy(mon,smonth[editor->month],3);
  mon[3]='\0';

  sprintf(editor->filename,"xc%d%s%d",editor->day,mon,editor->year);

  if(!ReadFile(editor))
    return;

  editor->used = True; 

  sprintf(title," %d %s %d ",
	  editor->day, smonth[editor->month], editor->year);

  XtSetArg(args[0],XtNlabel,title);
  XtSetValues(editor->title,args,1);

  XtSetArg(args[0],XtNiconName,title);
  XtSetValues(editor->shell,args,1);

  XtSetSensitive(editor->saveButton,False);

  XtAddEventHandler(editor->editor,KeyPressMask,False,SaveButtonHandler,(caddr_t)editor);

  if(EmptyBuffer(editor))
    XtSetSensitive(editor->clearEntry,False);
  else
    XtSetSensitive(editor->clearEntry,True);

  StartEditor(editor);

  XtSetSensitive(editor->button,False);

}

void SaveButtonHandler(w,data,event)
     Widget w;
     caddr_t data;
     XEvent *event;
{
   DayEditor editor = (DayEditor)data;
   char temp;

   if(XLookupString((XKeyEvent *)event,&temp,1,NULL,NULL) == 0)
     return;
   XtSetSensitive(editor->saveButton,True);
   editor->saved=False;
   XtSetSensitive(editor->clearEntry,True);
   XtRemoveEventHandler(editor->editor,KeyPressMask,False,SaveButtonHandler,data);

}

void SaveText(w,closure,call_data)
     Widget w;
     caddr_t closure;
     caddr_t call_data;
{
   DayEditor editor = (DayEditor)closure;
   int saveStatus;

   if(editor->filename == NULL)
     return;

   if((saveStatus=write_to_file(editor->filename,editor->buffer,TextSize(editor))) == -1 )
     return;

   if(saveStatus == 0){
      MarkDayEntry(editor->button,False);
      XtSetSensitive(editor->clearEntry,False);
   }
   else
     MarkDayEntry(editor->button,True);
   
   XtSetSensitive(editor->saveButton,False);
   
   XtAddEventHandler(editor->editor,KeyPressMask,False,SaveButtonHandler,(caddr_t)editor);
   editor->saved = True;
}

void ExitEditor(w,closure,call_data)
     Widget w;
     caddr_t closure;
     caddr_t call_data;
{
   DayEditor editor = (DayEditor)closure;

   if(editor->saved == False)
     SaveText(w,closure,call_data);

   XtRemoveEventHandler(editor->editor,KeyPressMask,False,SaveButtonHandler,(caddr_t)editor);
   
   editor->used = False;

   XtPopdown(editor->shell);
   
   XtSetSensitive(editor->button,True);

}

void ClearEntry(w,closure,call_data)
     Widget w;
     caddr_t closure;
     caddr_t call_data;
{
   DayEditor    editor = (DayEditor)closure;

   bzero(editor->buffer,appResources.textBufferSize);

   ChangeTextSource(editor,editor->buffer);

   SaveText(w,closure,call_data);

}


DayEditor CreateDayEditor(name,mode,options,initialtext)
     char           *name;
     XtTextEditType mode;
     char           *initialtext;
     int            options;
{
   DayEditor editor = (DayEditor)XtCalloc(1,sizeof(DayEditorRec));
   Arg       args[7];
   Widget    frame,title,text,controls;

   editor->shell = XtCreatePopupShell(name,topLevelShellWidgetClass, toplevel, NULL, 0);

   frame = XtCreateWidget("editorFrame", vPanedWidgetClass, editor->shell, args, 0);

    /** create title **/

   XtSetArg(args[0],XtNskipAdjust,True);

   title = XtCreateManagedWidget("editorTitle", labelWidgetClass, frame, args, 1);

   editor->title = title;

   editor->mode = mode;
   editor->buffer = XtCalloc(appResources.textBufferSize, sizeof(char));
   if(initialtext) strcpy(editor->buffer,initialtext);

   editor->bufSize = appResources.textBufferSize;

   XtSetArg(args[0],XtNstring, editor->buffer);
   XtSetArg(args[1],XtNeditType,mode);
   XtSetArg(args[2],XtNlength,appResources.textBufferSize);
   XtSetArg(args[3],XtNtextOptions, options);

   text = XtCreateManagedWidget("editor", asciiStringWidgetClass, frame, args, 4);
   editor->editor = text;

   XtSetArg(args[0],XtNfromVert,text); 
   XtSetArg(args[1],XtNskipAdjust,True);
   controls = XtCreateManagedWidget("editorControls", boxWidgetClass, frame, args,2);

   callbacks[0].callback = ExitEditor;
   callbacks[0].closure  = (caddr_t)editor;
   XtSetArg(args[0],XtNcallback,callbacks);

   XtCreateManagedWidget("doneButton", commandWidgetClass, controls, args, 1);

   callbacks[0].callback = SaveText;
   callbacks[0].closure  = (caddr_t)editor;
   XtSetArg(args[0],XtNcallback,callbacks);

   editor->saveButton = XtCreateManagedWidget("saveButton", commandWidgetClass, controls, args, 1);

   callbacks[0].callback = ClearEntry;
   callbacks[0].closure  = (caddr_t)editor;
   XtSetArg(args[0],XtNcallback,callbacks);

   editor->clearEntry = XtCreateManagedWidget("clearEntry", commandWidgetClass, controls, args, 1);

   XtManageChild(frame); 
   
   XtRealizeWidget(editor->shell);

   return editor;

}

Boolean ReadFile(editor)
       DayEditor editor;
{
   int size;

   bzero(editor->buffer,editor->bufSize);

   size = read_file(editor->filename, editor->buffer, editor->bufSize);

   if(size == -1)
     return False;
   else if( size > editor->bufSize){
      editor->buffer = XtRealloc(editor->buffer,size + 1);
      editor->bufSize = size + 1;
      size = read_file(editor->filename, editor->buffer, editor->bufSize);
      if(size == -1)
	return False;
   }

   ChangeTextSource(editor,editor->buffer);

   return True;
}

void ChangeTextSource(editor,newtext)
     DayEditor editor;
     char      *newtext;
{
   Arg          args[3];
   XtTextSource old = XtTextGetSource(editor->editor);

   XtSetArg(args[0],XtNstring, newtext);
   XtSetArg(args[1],XtNlength, editor->bufSize);
   XtSetArg(args[2],XtNeditType,editor->mode);

   XtTextSetSource(editor->editor,
		   XtStringSourceCreate(editor->editor, args, 3),(XtTextPosition)0);
		   
   XtStringSourceDestroy(old);
}

void StartEditor(editor)
       DayEditor editor;
{

   XtPopup(editor->shell ,XtGrabNone);

}

void CloseEditors()
{
   int i;
   DayEditor editor;

   if(editorList == NULL)
     return;

   for(i=1; i <= ListLength(editorList) ; i++){
      editor = GetEditorFromList(editorList,i);
      if(editor->saved == False)
	SaveText(NULL,(caddr_t)editor,NULL);
   }
}


#include <sys/stat.h>
#include <sys/file.h>
#include <sys/errno.h>

extern int errno;

int read_file(filename,buffer,size)
     char *filename;
     char *buffer;
     int  size;
{
   int fd, len;
   struct stat sb;

   if((fd = open(filename, O_RDONLY, 0666)) < 0) {
      if(errno == ENOENT)
	return 0;
      else{
	 perror("xcalendar:open");
	 return(-1);
      }
   }
   
   if(fstat(fd, &sb) != 0) {
      close(fd);
      perror("xcalendar:fstat:");
      return(-1);
   }

   if(sb.st_size == 0){
      unlink(filename);
      return 0;
   }

   len = (size < sb.st_size)? size : sb.st_size;

   if(read(fd, buffer, len) < len) {
      close(fd);
      fprintf(stderr, "Couldn't read all file %s\n",filename);
      perror("xcalendar:read:");
      return(-1);
   }

   close(fd);

   return sb.st_size;

}

int write_to_file(filename,buffer,len)
     char *filename;
     char *buffer;
     int  len;
{
   int fd;

   if((fd = open(filename, O_WRONLY | O_TRUNC | O_CREAT , 0666)) < 0) {
      perror("xcalendar:open");
      return(-1);
   }
   
   if(len == 0){
      unlink(filename);
      return 0;
   }

   if(write(fd, buffer, len)  < len) {
      fprintf(stderr, "Sorry couldn't write all file %s\n", filename);
      perror("xcalendar:write:");
      close(fd);
      return(-1);
   }

   close(fd);

   return len;
}

