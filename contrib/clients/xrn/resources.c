
#ifndef lint
static char XRNrcsid[] = "$Header: /users/ricks/xrn-11/R4/RCS/resources.c,v 1.15 89/11/17 12:51:52 ricks Rel $";
#endif

/*
 * xrn - an X-based NNTP news reader
 *
 * Copyright (c) 1988, 1989, Ellen M. Sentovich and Rick L. Spickelmier.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of the University of California not
 * be used in advertising or publicity pertaining to distribution of 
 * the software without specific, written prior permission.  The University
 * of California makes no representations about the suitability of this
 * software for any purpose.  It is provided "as is" without express or
 * implied warranty.
 *
 * THE UNIVERSITY OF CALIFORNIA DISCLAIMS ALL WARRANTIES WITH REGARD TO 
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS, IN NO EVENT SHALL THE UNIVERSITY OF CALIFORNIA BE LIABLE FOR
 * ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER
 * RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
 * CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * resources.c: routines for handling resource management
 */

#include "copyright.h"
#include <X11/Xos.h>
#include "utils.h"
#include "config.h"
#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include "avl.h"
#include "xthelper.h"
#include "resources.h"
#ifdef VMS
#define index strchr
#endif /* VMS */

/*
 * resources and command list
 */

/* extra name and class specifications */
#define XtCgeometry       "Geometry"
#ifdef VMS
#define XtNgeometry	  "geometry"
#endif
#define XtNsaveDir        "saveDir"
#define XtCsaveDir        "SaveDir"
#define XtNiconGeometry   "iconGeometry"
#define XtCiconGeometry   "IconGeometry"
#define XtNnewsrcFile     "newsrcFile"
#define XtCnewsrcFile     "NewsrcFile"
#define XtNsaveNewsrcFile "saveNewsrcFile"
#define XtCsaveNewsrcFile "SaveNewsrcFile"
#define XtNsignatureFile  "signatureFile"
#define XtCsignatureFile  "SignatureFile"
#define XtNnntpServer     "nntpServer"
#define XtCnntpServer     "NntpServer"
#define XtNtopLines       "topLines"
#define XtCtopLines       "TopLines"
#define XtNsaveMode       "saveMode"
#define XtCsaveMode       "SaveMode"
#define XtNleaveHeaders   "leaveHeaders"
#define XtCleaveHeaders   "LeaveHeaders"
#define XtNstripHeaders   "stripHeaders"
#define XtCstripHeaders   "StripHeaders"
#define XtNsavePostings   "savePostings"
#define XtCsavePostings   "SavePostings"
#define XtNdeadLetters    "deadLetters"
#define XtCdeadLetters    "DeadLetters"
#define XtNminLines       "minLines"
#define XtCminLines       "MinLines"
#define XtNmaxLines       "maxLines"
#define XtCmaxLines       "MaxLines"
#define XtNmailer         "mailer"
#define XtCmailer         "Mailer"
#define XtNtmpDir         "tmpDir"
#define XtCtmpDir         "TmpDir"
#define XtNsubjectRead    "subjectRead"
#define XtCsubjectRead    "SubjectRead"
#define XtNconfirm        "confirm"
#define XtCconfirm        "Confirm"
#define XtNuserBitmap     "userBitmap"
#define XtCuserBitmap     "UserBitmap"
#define XtNeditorCommand  "editorCommand"
#define XtCeditorCommand  "EditorCommand"
#define XtNincludeSep     "includeSep"
#define XtCincludeSep     "IncludeSep"
#define XtNincludeHeader  "includeHeader"
#define XtCincludeHeader  "IncludeHeader"
#define XtNsortedSubjects "sortedSubjects"
#define XtCsortedSubjects "SortedSubjects"
#define XtNorganization	  "organization"
#define XtCorganization	  "Organization"
#define XtNreplyTo	  "replyTo"
#define XtCreplyTo	  "ReplyTo"
#define XtNlineLength     "lineLength"
#define XtClineLength     "LineLength"
#define XtNbreakLength    "breakLength"
#define XtCbreakLength    "BreakLength"
#define XtNpageArticles	  "pageArticles"
#define XtCPageArticles	  "PageArticles"
#define XtNprintCommand	  "printCommand"
#define XtCPrintCommand	  "PrintCommand"

#define XtNaddBindings	  "addBindings"
#define XtCAddBindings	  "AddBindings"
#define XtNngBindings	  "ngBindings"
#define XtCNgBindings	  "NgBindings"
#define XtNartBindings	  "artBindings"
#define XtCArtBindings	  "ArtBindings"
#define XtNallBindings	  "allBindings"
#define XtCAllBindings	  "AllBindings"

#define XtNaddButtonList  "addButtonList"
#define XtCAddButtonList  "AddButtonList"
#define XtNngButtonList   "ngButtonList"
#define XtCNgButtonList   "NgButtonList"
#define XtNallButtonList  "allButtonList"
#define XtCAllButtonList  "AllButtonList"
#define XtNartButtonList  "artButtonList"
#define XtCArtButtonList  "ArtButtonList"
#define XtNartSpecButtonList  "artSpecButtonList"
#define XtCArtSpecButtonList  "ArtSpecButtonList"

#define XtNupdateNewsrc   "updateNewsrc"
#define XtCupdateNewsrc   "UpdateNewsrc"
#define XtNkillFiles      "killFiles"
#define XtCkillFiles      "KillFiles"


static Boolean defaultFalse = False;
static Boolean defaultTrue  = True;


app_resourceRec app_resources;

/*
 * resources 'xrn' needs to get, rather than ones that the individual
 * widgets will handle
 */
static XtResource resources[] = {
   {XtNgeometry, XtCgeometry, XtRString,  sizeof(char *),
      XtOffset(app_res,geometry), XtRString, (caddr_t) NULL},
   
   {XtNiconGeometry,  XtCiconGeometry,  XtRString,  sizeof(char *),
      XtOffset(app_res,iconGeometry), XtRString, (caddr_t) NULL},
   
   {XtNsaveDir,  XtCsaveDir,  XtRString, sizeof(char *),
      XtOffset(app_res,saveDir), XtRString, (caddr_t) SAVEDIR},
   
   {XtNnewsrcFile, XtCnewsrcFile, XtRString, sizeof(char *),
      XtOffset(app_res,newsrcFile), XtRString, (caddr_t) NEWSRCFILE},
   
   {XtNsaveNewsrcFile, XtCsaveNewsrcFile, XtRString, sizeof(char *),
      XtOffset(app_res,saveNewsrcFile), XtRString, (caddr_t) SAVENEWSRCFILE},
   
   {XtNsignatureFile, XtCsignatureFile, XtRString, sizeof(char *),
      XtOffset(app_res,signatureFile), XtRString, (caddr_t) SIGNATUREFILE},
   
   {XtNnntpServer, XtCnntpServer, XtRString, sizeof(char *),
      XtOffset(app_res,nntpServer), XtRString, (caddr_t) NULL},
   
   {XtNtopLines, XtCtopLines, XtRInt, sizeof(int),
      XtOffset(app_res,topLines), XtRString, (caddr_t) TOPLINES},
   
   {XtNsaveMode, XtCsaveMode, XtRString, sizeof(char *),
      XtOffset(app_res,strSaveMode), XtRString, (caddr_t) SAVEMODE},
   
   {XtNleaveHeaders, XtCleaveHeaders, XtRString, sizeof(char *),
      XtOffset(app_res,leaveHeaders), XtRString, (caddr_t) NULL},
   
   {XtNstripHeaders, XtCstripHeaders, XtRString, sizeof(char *),
      XtOffset(app_res,stripHeaders), XtRString, (caddr_t) NULL},
   
   {XtNsavePostings, XtCsavePostings, XtRString, sizeof(char *),
      XtOffset(app_res,savePostings), XtRString, (caddr_t) SAVEPOSTINGS},
   
   {XtNdeadLetters, XtCdeadLetters, XtRString, sizeof(char *),
      XtOffset(app_res,deadLetters), XtRString, (caddr_t) DEADLETTER},

   {XtNminLines, XtCminLines, XtRInt, sizeof(int),
      XtOffset(app_res,minLines), XtRString, (caddr_t) MINLINES},

   {XtNmaxLines, XtCmaxLines, XtRInt, sizeof(int),
      XtOffset(app_res,maxLines), XtRString, (caddr_t) MAXLINES},
   
   {XtNmailer, XtCmailer, XtRString, sizeof(char *),
      XtOffset(app_res,mailer), XtRString, (caddr_t) SENDMAIL},
   
   {XtNsubjectRead, XtCsubjectRead, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,subjectRead), XtRBoolean, (caddr_t) &defaultFalse},
   
   {XtNtmpDir, XtCtmpDir, XtRString, sizeof(char *),
      XtOffset(app_res,tmpDir), XtRString, (caddr_t) NULL},

   {XtNconfirm, XtCconfirm, XtRString, sizeof(char *),
      XtOffset(app_res,confirm), XtRString, (caddr_t) NULL},

   {XtNkillFiles, XtCkillFiles, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,killFiles), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNuserBitmap, XtCuserBitmap, XtRString, sizeof(char *),
      XtOffset(app_res,userBitmap), XtRString, (caddr_t) NULL},
   
   {XtNeditorCommand, XtCeditorCommand, XtRString, sizeof(char *),
      XtOffset(app_res,editorCommand), XtRString, (caddr_t) NULL},

   {XtNorganization, XtCorganization, XtRString, sizeof(char *),
      XtOffset(app_res,organization), XtRString, (caddr_t) NULL},
   
   {XtNreplyTo, XtCreplyTo, XtRString, sizeof(char *),
      XtOffset(app_res,replyTo), XtRString, (caddr_t) NULL},
   
   {XtNincludeSep, XtCincludeSep, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,includeSep), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNincludeHeader, XtCincludeHeader, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,includeHeader), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNsortedSubjects, XtCsortedSubjects, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,sortedSubjects), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNupdateNewsrc, XtCupdateNewsrc, XtRBoolean, sizeof(Boolean),
     XtOffset(app_res,updateNewsrc), XtRBoolean, (caddr_t) &defaultFalse},

   {XtNlineLength, XtClineLength, XtRInt, sizeof(int),
      XtOffset(app_res,lineLength), XtRString, (caddr_t) LINELENGTH},
   
   {XtNbreakLength, XtCbreakLength, XtRInt, sizeof(int),
      XtOffset(app_res,breakLength), XtRString, (caddr_t) BREAKLENGTH},

   {XtNpageArticles, XtCPageArticles, XtRBoolean, sizeof(Boolean),
      XtOffset(app_res,pageArticles), XtRBoolean, (caddr_t) &defaultTrue},

   {XtNaddButtonList, XtCAddButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,addButtonList), XtRString, (caddr_t) NULL},

   {XtNngButtonList, XtCNgButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,ngButtonList), XtRString, (caddr_t) NULL},

   {XtNallButtonList, XtCAllButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,allButtonList), XtRString, (caddr_t) NULL},

   {XtNartButtonList, XtCArtButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,artButtonList), XtRString, (caddr_t) NULL},

   {XtNartSpecButtonList, XtCArtSpecButtonList, XtRString, sizeof(char *),
      XtOffset(app_res,artSpecButtonList), XtRString, (caddr_t) NULL},

   {XtNprintCommand, XtCPrintCommand, XtRString, sizeof(char *),
      XtOffset(app_res,printCommand), XtRString, (caddr_t) PRINTCOMMAND},

   {XtNngBindings, XtCNgBindings, XtRString, sizeof(char *),
      XtOffset(app_res,ngBindings), XtRString, (caddr_t) NULL},

   {XtNartBindings, XtCArtBindings, XtRString, sizeof(char *),
      XtOffset(app_res,artBindings), XtRString, (caddr_t) NULL},

   {XtNallBindings, XtCAllBindings, XtRString, sizeof(char *),
      XtOffset(app_res,allBindings), XtRString, (caddr_t) NULL},

   {XtNaddBindings, XtCAddBindings, XtRString, sizeof(char *),
      XtOffset(app_res,addBindings), XtRString, (caddr_t) NULL},
};


/*
 * allowed command line options
 */
#ifndef VMS
static XrmOptionDescRec optionList[] = {
    {"-iconGeometry",   XtNiconGeometry,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-nntpServer",     XtNnntpServer,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-newsrcFile",     XtNnewsrcFile,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-saveNewsrcFile", XtNsaveNewsrcFile, XrmoptionSepArg,  (caddr_t) NULL},
    {"-signatureFile",  XtNsignatureFile,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-saveDir",        XtNsaveDir,        XrmoptionSepArg,  (caddr_t) NULL},
    {"-saveMode",       XtNsaveMode,       XrmoptionSepArg,  (caddr_t) NULL},
    {"-leaveHeaders",   XtNleaveHeaders,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-stripHeaders",   XtNstripHeaders,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-topLines",       XtNtopLines,       XrmoptionSepArg,  (caddr_t) NULL},
    {"-savePostings",   XtNsavePostings,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-deadLetters",    XtNdeadLetters,    XrmoptionSepArg,  (caddr_t) NULL},
    {"-minLines",       XtNminLines,       XrmoptionSepArg,  (caddr_t) NULL},
    {"-maxLines",       XtNmaxLines,       XrmoptionSepArg,  (caddr_t) NULL},
    {"-mailer",         XtNmailer,         XrmoptionSepArg,  (caddr_t) NULL},
    {"-tmpDir",         XtNtmpDir,         XrmoptionSepArg,  (caddr_t) NULL},
    {"-subjectRead",    XtNsubjectRead,    XrmoptionNoArg,   (caddr_t) "off"},
    {"+subjectRead",    XtNsubjectRead,    XrmoptionNoArg,   (caddr_t) "on"},
    {"-confirm",        XtNconfirm,        XrmoptionSepArg,  (caddr_t) NULL},
    {"-killFiles",      XtNkillFiles,      XrmoptionNoArg,   (caddr_t) "off"},
    {"+killFiles",      XtNkillFiles,      XrmoptionNoArg,   (caddr_t) "on"},
    {"-userBitmap",     XtNuserBitmap,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-editorCommand",  XtNeditorCommand,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-includeSep",     XtNincludeSep,     XrmoptionNoArg,   (caddr_t) "off"},
    {"+includeSep",     XtNincludeSep,     XrmoptionNoArg,   (caddr_t) "on"},
    {"-includeHeader",  XtNincludeHeader,  XrmoptionNoArg,   (caddr_t) "off"},
    {"+includeHeader",  XtNincludeHeader,  XrmoptionNoArg,   (caddr_t) "on"},
    {"-sortedSubjects", XtNsortedSubjects, XrmoptionNoArg,   (caddr_t) "off"},
    {"+sortedSubjects", XtNsortedSubjects, XrmoptionNoArg,   (caddr_t) "on"},
    {"-updateNewsrc",   XtNupdateNewsrc,   XrmoptionNoArg,   (caddr_t) "off"},
    {"+updateNewsrc",   XtNupdateNewsrc,   XrmoptionNoArg,   (caddr_t) "on"},
    {"-lineLength",     XtNlineLength,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-breakLength",    XtNbreakLength,    XrmoptionSepArg,  (caddr_t) NULL},
    {"-pageArticles",	XtNpageArticles,   XrmoptionNoArg,   (caddr_t) "off"},
    {"+pageArticles",	XtNpageArticles,   XrmoptionNoArg,   (caddr_t) "on"},
    {"-organization",   XtNorganization,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-replyTo",        XtNreplyTo,        XrmoptionSepArg,  (caddr_t) NULL},
    {"-addButtonList",  XtNaddButtonList,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-ngButtonList",   XtNngButtonList,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-allButtonList",  XtNallButtonList,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-artButtonList",  XtNartButtonList,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-artSpecButtonList",  XtNartSpecButtonList,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-printCommand",   XtNprintCommand,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-ngBindings",     XtNngBindings,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-artBindings",    XtNartBindings,    XrmoptionSepArg,  (caddr_t) NULL},
    {"-addBindings",    XtNaddBindings,    XrmoptionSepArg,  (caddr_t) NULL},
    {"-allBindings",    XtNallBindings,    XrmoptionSepArg,  (caddr_t) NULL},
};    
#else
static XrmOptionDescRec optionList[] = {
    {"-icongeometry",   XtNiconGeometry,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-nntpserver",     XtNnntpServer,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-newsrcfile",     XtNnewsrcFile,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-savenewsrcfile", XtNsaveNewsrcFile, XrmoptionSepArg,  (caddr_t) NULL},
    {"-signaturefile",  XtNsignatureFile,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-savedir",        XtNsaveDir,        XrmoptionSepArg,  (caddr_t) NULL},
    {"-savemode",       XtNsaveMode,       XrmoptionSepArg,  (caddr_t) NULL},
    {"-leaveheaders",   XtNleaveHeaders,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-stripheaders",   XtNstripHeaders,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-toplines",       XtNtopLines,       XrmoptionSepArg,  (caddr_t) NULL},
    {"-savepostings",   XtNsavePostings,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-deadletters",    XtNdeadLetters,    XrmoptionSepArg,  (caddr_t) NULL},
    {"-minlines",       XtNminLines,       XrmoptionSepArg,  (caddr_t) NULL},
    {"-maxlines",       XtNmaxLines,       XrmoptionSepArg,  (caddr_t) NULL},
    {"-mailer",         XtNmailer,         XrmoptionSepArg,  (caddr_t) NULL},
    {"-tmpdir",         XtNtmpDir,         XrmoptionSepArg,  (caddr_t) NULL},
    {"-subjectread",    XtNsubjectRead,    XrmoptionNoArg,   (caddr_t) "off"},
    {"+subjectread",    XtNsubjectRead,    XrmoptionNoArg,   (caddr_t) "on"},
    {"-confirm",        XtNconfirm,        XrmoptionSepArg,  (caddr_t) NULL},
    {"-killfiles",      XtNkillFiles,      XrmoptionNoArg,   (caddr_t) "off"},
    {"+killfiles",      XtNkillFiles,      XrmoptionNoArg,   (caddr_t) "on"},
    {"-userbitmap",     XtNuserBitmap,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-editorcommand",  XtNeditorCommand,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-includesep",     XtNincludeSep,     XrmoptionNoArg,   (caddr_t) "off"},
    {"+includesep",     XtNincludeSep,     XrmoptionNoArg,   (caddr_t) "on"},
    {"-linelength",     XtNlineLength,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-breaklength",    XtNbreakLength,    XrmoptionSepArg,  (caddr_t) NULL},
    {"-pagearticles",	XtNpageArticles,   XrmoptionNoArg,   (caddr_t) "off"},
    {"+pagearticles",	XtNpageArticles,   XrmoptionNoArg,   (caddr_t) "on"},
    {"-organization",   XtNorganization,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-replyto",        XtNreplyTo,        XrmoptionSepArg,  (caddr_t) NULL},
    {"-addbuttonlist",  XtNaddButtonList,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-ngbuttonlist",   XtNngButtonList,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-allbuttonlist",  XtNallButtonList,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-artbuttonlist",  XtNartButtonList,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-artspecbuttonlist",  XtNartSpecButtonList,  XrmoptionSepArg,  (caddr_t) NULL},
    {"-printcommand",   XtNprintCommand,   XrmoptionSepArg,  (caddr_t) NULL},
    {"-ngbindings",     XtNngBindings,     XrmoptionSepArg,  (caddr_t) NULL},
    {"-artbindings",    XtNartBindings,    XrmoptionSepArg,  (caddr_t) NULL},
    {"-addbindings",    XtNaddBindings,    XrmoptionSepArg,  (caddr_t) NULL},
    {"-allbindings",    XtNallBindings,    XrmoptionSepArg,  (caddr_t) NULL},
};    
#endif

static void
usage(ac, av)
int ac;
char **av;  /* program name */
/*
 * print out the usage message
 *
 *   returns: void
 *   
 */
{
    int i;

    (void) printf("Unknown options:");
    for (i = 1; i <= ac - 1; i++) {
	if (index(av[i], ':') == NIL(char)) {
	    printf(" %s", av[i]);
	}
    }
    printf("\n");
    printf("usage: %s [options] [-display host:display]\n", av[0]);
    printf("\t-geometry WxH+X+Y\tSize and position of window\n");
    printf("\t-iconGeometry +X+Y\tPosition of icon\n");
    printf("\t-fg color\t\tText color\n");
    printf("\t-bg color\t\tBackground color\n");
    printf("\t-nntpServer name\tNNTP server\n");
    printf("\t-newsrcFile file\t.newsrc filename\n");
    printf("\t-saveNewsrcFile file\tsaved .newsrc filename\n");
    printf("\t-saveDir directory\tDirectory for saving files\n");
    printf("\t-signatureFile file\tSignature file for posting\n");
    printf("\t-topLines number\tNumber of lines used by the top window\n");
    printf("\t-saveMode mode\t\tMethod of saving articles\n");
    printf("\t-leaveHeaders list\tHeaders to leave\n");
    printf("\t-stripHeaders list\tHeaders to strip\n");
    printf("\t-deadLetters file\tFile to store failed postings/messages\n");
    printf("\t-savePostings file\tFile to save postings/messages\n");
    printf("\t-minLines number\tMinimum number of lines above cursor\n");
    printf("\t-maxLines number\tMaximum number of lines above cursor\n");
    printf("\t-mailer\t\t\tMailer to use\n");
    printf("\t-editorCommand\t\tEditor to use (defaults to the toolkit editor)\n");
    printf("\t+/-includeHeader\tInclude original article's header\n");
    printf("\t+/-includeSep\tput '>' in front of included lines\n");
    printf("\t+/-updateNewsrc\tupdate the .newsrc file each time a group is exited\n");
    printf("\t+/-sortedSubjects\tsort or do not sort the subjects\n");
    printf("\t-tmpDir\t\t\tTemporary article directory\n");
    printf("\t+/-subjectRead\t\tSwap subject next and next unread buttons\n");
    printf("\t-confirm\t\tTurn on/off confirmation boxes\n");
    printf("\t+/-killFiles\t\tTurn on/off the use of kill files\n");
    printf("\t-userBitmap\t\tUse the users bitmap as the icon\n");
    printf("\t-lineLength\t\tlength of lines for article postings\n");
    printf("\t-breakLength\t\tlength of line at which line wrap goes into effect\n");
    printf("\t-organization\t\tname of your organization\n");
    printf("\t-replyTo\t\tvalue used for the Reply-To field\n");
    printf("\t-addButtonList\tlist of Add mode buttons\n");
    printf("\t-ngButtonList\t\tlist of Newsgroup mode buttons\n");
    printf("\t-allButtonList\t\tlist of All mode buttons\n");
    printf("\t-artButtonList\t\tlist of Article mode buttons (top box)\n");
    printf("\t-artSpecButtonList\t\tlist of Article mode buttons (bottom box)\n");
    printf("\t-addBindings\t\tAdd mode bindings\n");
    printf("\t-ngBindings\t\tNewsgroup mode bindings\n");
    printf("\t-artBindings\t\tArticle mode bindings\n");
    printf("\t-allBindings\t\tAll mode bindings\n");

    ehErrorExitXRN("");
}


Widget
Initialize(argc, argv)
int argc;
char **argv;
/*
 * initialize the toolkit, parse the command line, and handle the Xdefaults
 *
 *   returns: top level widget
 *
 */
{
#ifndef XDBBUG
    extern XrmDatabase XtDatabase();
#endif /* XDBBUG */
    XrmDatabase rdb;
    Widget widget;
    char *ptr;
    static Arg shell_args[] = {
	{XtNinput, (XtArgVal)True},
    };


    /* set up the program name */
#ifndef VMS
    if ((ptr = rindex(argv[0], '/')) == NIL(char)) {
	(void) strncpy(app_resources.progName, argv[0],
		       sizeof(app_resources.progName));
    } else {
	(void) strncpy(app_resources.progName, ++ptr,
		       sizeof(app_resources.progName));
    }
#else
    (void) strcpy(app_resources.progName, "xrn");
#endif

    widget = XtInitialize(app_resources.progName, "XRn",
			  optionList, XtNumber(optionList), &argc, argv);

    /* check and set the lock file */
    checkLock();

    if (argc > 1) {
	usage(argc, argv);
    }

    XtSetValues(widget, shell_args, XtNumber(shell_args));

    rdb = XtDatabase(XtDisplay(widget));

    /* get the resources needed by xrn itself */
    XtGetApplicationResources(widget, (caddr_t) &app_resources,
			      resources, XtNumber(resources), 0, 0);

    /* article saving mode */

    app_resources.saveMode = 0;
    if (utSubstring(app_resources.strSaveMode, "mailbox") == 1) {
	app_resources.saveMode |= MAILBOX_SAVE;
    } else {
	app_resources.saveMode |= NORMAL_SAVE;
    }
    if (utSubstring(app_resources.strSaveMode, "noheaders") == 1) {
	app_resources.saveMode |= NOHEADERS_SAVE;
    } else {
	app_resources.saveMode |= HEADERS_SAVE;
    }

    if (utSubstring(app_resources.strSaveMode, "subdirs") == 1) {
	app_resources.saveMode |= SUBDIRS_SAVE;
    } else if (utSubstring(app_resources.strSaveMode, "onedir") == 1) {
	app_resources.saveMode |= ONEDIR_SAVE;
    } else {
	app_resources.saveMode |= SAVE_DIR_DEFAULT;
    }

    /* header stripping mode */

    /* STRIP_HEADERS with a NIL table will leave all headers (nothing to strip) */
    app_resources.headerTree = avl_init_table(strcmp);
    app_resources.headerMode = STRIP_HEADERS;

    if ((app_resources.leaveHeaders != NIL(char)) &&
	(app_resources.stripHeaders != NIL(char))) {
	ehErrorExitXRN("Only one of 'stripHeaders, leaveHeaders' allowed\n");
     }  

    if (app_resources.leaveHeaders != NIL(char)) {
	char *ptr, *token;
	
	app_resources.headerMode = LEAVE_HEADERS;
	ptr = app_resources.leaveHeaders;
	while ((token = strtok(ptr, ", \t\n")) != NIL(char)) {
	    utDowncase(token);
	    (void) avl_insert(app_resources.headerTree, token, (char *) 1);
	    ptr = NIL(char);
	}
	
    } else if (app_resources.stripHeaders != NIL(char)) {
	char *ptr, *token;
	
	app_resources.headerMode = STRIP_HEADERS;
	ptr = app_resources.stripHeaders;
	while ((token = strtok(ptr, ", \t\n")) != NIL(char)) {
	    utDowncase(token);
	    (void) avl_insert(app_resources.headerTree, token, (char *) 1);
	    ptr = NIL(char);
	}
    }

    /* confirm boxes */

    app_resources.confirmMode = 0;

    if (app_resources.confirm != NIL(char)) {
	char *ptr, *token;

	ptr = app_resources.confirm;
	while ((token = strtok(ptr, ", \t\n")) != NIL(char)) {
	    if (strcmp(token, "ngQuit") == 0) {
		app_resources.confirmMode |= NG_QUIT;
	    } else if (strcmp(token, "ngExit") == 0) {
		app_resources.confirmMode |= NG_EXIT;
	    } else if (strcmp(token, "ngCatchUp") == 0) {
		app_resources.confirmMode |= NG_CATCHUP;
	    } else if (strcmp(token, "artCatchUp") == 0) {
		app_resources.confirmMode |= ART_CATCHUP;
	    } else if (strcmp(token, "ngUnsub") == 0) {
		app_resources.confirmMode |= NG_UNSUBSCRIBE;
	    } else if (strcmp(token, "artUnsub") == 0) {
		app_resources.confirmMode |= ART_UNSUBSCRIBE;
	    } else {
		mesgPane("unknown confirm button: %s", token);
	    }
	    ptr = NIL(char);
	}
    }

    /* temporary directory */

    if (app_resources.tmpDir == NIL(char)) {
	char *ptr = getenv("TMPDIR");

	if (ptr == NIL(char)) {
	    app_resources.tmpDir = XtNewString(TEMPORARY_DIRECTORY);
	} else {
	    app_resources.tmpDir = XtNewString(ptr);
	}
    }

    return widget;
}
