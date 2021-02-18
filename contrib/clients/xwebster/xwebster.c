/* -*-C-*-
********************************************************************************
*
* File:         xwebster.c
* RCS:          $Header: xwebster.c,v 1.12 89/03/06 03:43:31 mayer Exp $
* Description:  X11 + HP-Xwidgets interface to a webster dictionary server
* Author:       Niels Mayer, HPLabs
* Created:      Wed Aug 31 14:09:08 1988
* Modified:     Mon Mar  6 03:43:14 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       G-Job
*
* xwebster - dictionary browser
*
* Copyright 1988 Hewlett-Packard Company
*
* Permission to use, copy, modify, and distribute this software and its
* documentation for any purpose and without fee is hereby granted, provided
* that the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of HP not be used in advertising or
* publicity pertaining to distribution of the software without specific,
* written prior permission.  HP makes no representations about the
* suitability of this software for any purpose.  It is provided "as is"
* without express or implied warranty.
*
* Please send any improvements, bug fixes, useful modifications, and comments
* to mayer@hplabs.hp.com.
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: xwebster.c,v 1.12 89/03/06 03:43:31 mayer Exp $";
char xwebster_version[] = "Xwebster, version 1.10";

/* 
 * Some aspects of this program were inspired by the first
 * C-language translation of the webster client program, webster.c, done by:
 *                   "David A. Curry
 *                    Purdue University
 *                    Engineering Computer Network
 *                    April, 1986"
 * The only copyright notice appearing in the webster.c source is:
 *   "Webster's 7th Collegiate Dictionary, Copyright (C) 1963 by Merriam-Webster,
 *    Inc.  No part of this information may be copied or reprinted without the
 *    express written consent of the publisher."
 */

#include "xwebster.h"
#include "user_prefs.h"
#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <memory.h>
#include <Xw/VPW.h>
#include <X11/cursorfont.h>

static XtInputCallbackProc Webster_Get_Server_Output();
static void Process_Webster_Line();
static int Webster_Connect_To_Server();
static void Webster_Reconnect();
static XtInputCallbackProc Webster_Handle_Server_Exception();
static void Webster_Disconnect();

int              webster_Socket = NULL;
static XtInputId webster_xinput;
static XtInputId webster_xexcept;
static Widget    toplevel_Wgt = NULL;
Display*         display;
USER_PREFS_DATA  user_prefs; /* extern declared in user_prefs.h, really here */

/* 
 * Data on how user-customization resources are interpreted:
 * this must be kept up to date with data structure USER_PREFS_DATA_PTR 
 * in user_prefs.h
 *
 * My philosophy is to place all my intended defaults in 
 * APPDEFAULTSDIR/Xwebster, so that the program doesn't have to
 * be recompiled just to change a simple default. Thus I don't always have
 * the best defaults in this structure.
 */
static XtResource resources[] = {
  {"helpText", "HelpText",
     XtRString, sizeof(String),
     XtOffset(USER_PREFS_DATA_PTR, help_text),
     XtRString, "Warning -- Someone forgot to install APPDEFAULTSDIR/Xwebster"},
  {"hostAddrList", "HostAddrList",
     XtRString, sizeof(String),
     XtOffset(USER_PREFS_DATA_PTR, host_addr_list),
     XtRString, "10.0.0.51 26.0.0.73"},
  {"hostPort", "HostPort",
     XtRInt, sizeof(int),
     XtOffset(USER_PREFS_DATA_PTR, host_port),
     XtRString, "103"},
  {"hostServiceName", "HostServiceName",
     XtRString, sizeof(String),
     XtOffset(USER_PREFS_DATA_PTR, host_service_name),
     XtRString, "webster"},
  {"wordInputWidgetWidth", "WordInputWidgetWidth",
     XtRInt, sizeof(int),
     XtOffset(USER_PREFS_DATA_PTR, word_input_Wgt_width),
     XtRString, "30"},
  {"useEditorAsDisplay", "UseEditorAsDisplay", 
     XtRBoolean, sizeof(Boolean),
     XtOffset(USER_PREFS_DATA_PTR, use_editor_as_display),
     XtRString, "FALSE"},  
  {"displayWidth", "DisplayWidth",
     XtRInt, sizeof(int),
     XtOffset(USER_PREFS_DATA_PTR, display_width),
     XtRString, "85"},
  {"displayHeight", "DisplayHeight",
     XtRInt, sizeof(int),
     XtOffset(USER_PREFS_DATA_PTR, display_height),
     XtRString, "10"},
  {"numWordsBeforeRefresh", "NumWordsBeforeRefresh",
     XtRInt, sizeof(int),
     XtOffset(USER_PREFS_DATA_PTR, num_words_before_refresh),
     XtRString, "20"}
};


/******************************************************************************
 *
 ******************************************************************************/
main(argc, argv)
     int             argc;
     char          **argv;
{
  /* 
   * Initialize the toolkit, place xwebster-defined resources in user_prefs.
   */
  toplevel_Wgt = XtInitialize(argv[0], "Xwebster", NULL, 0, &argc, argv);
  XwRegisterConverters();	/* Xw WORKAROUND-KLUDGE -- needed for XtRTileType converter, which should be registered but isn't */
  XtGetApplicationResources(toplevel_Wgt, &user_prefs, 
			    resources, XtNumber(resources), NULL, 0);
  {
    Widget panel_Wgt = XtCreateManagedWidget("panel", XwvPanedWidgetClass, 
					     toplevel_Wgt, NULL, 0);
    Controlpanel_Titlebar_Init(panel_Wgt);
    Wordlist_Init(panel_Wgt);
    Controlpanel_Init(panel_Wgt);
    Display_Def_Init(panel_Wgt);
  }
  XtRealizeWidget(toplevel_Wgt);
  display = XtDisplay(toplevel_Wgt);
  XDefineCursor(display, XtWindow(toplevel_Wgt), 
		XCreateFontCursor(display, XC_plus));
  XtMainLoop();
}

/******************************************************************************
 * This procedure is called indirectly, via XtAddInput() callback from 
 * XtMainLoop() whenever new input appears on webster_Socket. This callback
 * is setup in procedure Webster_Reconnect().
 * This procedure will read all the data from webster_Socket, and then call 
 * Process_Webster_Line() for each line of input received. If an incomplete 
 * line of text is received, then this procedure will buffer that line until
 * the next time it gets called from the callback.
 ******************************************************************************/
static XtInputCallbackProc Webster_Get_Server_Output(client_data, source_fildes, id)
     caddr_t   client_data;
     int       source_fildes;
     XtInputId id;
{
  unsigned nbytes;
  char readbuf[BUFSIZ];
  static char buf[2*BUFSIZ];	/* overkill -- buf shouldn't get longer than BUFSIZ+
				   the size of any remaining line (max 80). */
  static char linebuf[BUFSIZ];
  static int buf_end_idx = 0;	/* init value only on first call */
  int prev_buf_end_idx;
  int prev_buf_idx;
  register int buf_idx;
  register int linebuf_idx;
  
  if (webster_Socket == NULL)
    Webster_Reconnect();
  if (webster_Socket == NULL)
    return;
  if ((nbytes = read(webster_Socket, readbuf, BUFSIZ)) <= 0) {
    Webster_Disconnect();
    return;
  }
  memcpy(&(buf[buf_end_idx]), readbuf, nbytes);	/* append new input to buf */
  buf_end_idx += nbytes;
  buf[buf_end_idx] = '\000';	/* NULL marks end of buf */
  prev_buf_end_idx = buf_end_idx;

  buf_idx = 0; 
  while (1) {			/* process lines till none left: exits w/ break*/
    linebuf_idx = 0;
    prev_buf_idx = buf_idx;
    /** try to copy a line of characters to linebuf, quitting on EOF or \000 **/
    while (((buf[buf_idx] & 0177) != '\n')
	   && (buf[buf_idx] != '\000')
	   && (buf[buf_idx] != '\200'))	/* webster server's <EOF> char */
      linebuf[linebuf_idx++] = buf[buf_idx++] & 0177;

    if ((buf[buf_idx] & 0177) == '\n') { /* copy to linebuf stopped at LF */
      linebuf[linebuf_idx-1] = '\n'; /* overwrite CR with LF */
      linebuf[linebuf_idx] = '\000'; /* NULL terminate linebuf */
      Process_Webster_Line(linebuf); /* process a line of input */
      buf_idx++;		/* skip over \n in buf */
      buf_end_idx -= linebuf_idx + 1; /* shorten by len(linebuf)+len(\n) */
    }
    else if (buf[buf_idx] == '\200') { /* copy to linebuf stopped at EOF */
      Process_Webster_Line(NULL); /* signal special case of EOF */
      buf_idx++;		/* skip over \200 in buf */
      buf_end_idx -= linebuf_idx + 1; /* shorten by len(linebuf)+len(\200) */
    }
    else {			/* copy to linebuf stopped at \000 (end of buf) */
      /** IF characters were transferred from buf to linebuf... **/
      if (prev_buf_end_idx != buf_end_idx)
	/** ...THEN shiftLeft characters that weren't transferred. **/
	memccpy(&(buf[0]), &(buf[prev_buf_idx]), '\000', 2*BUFSIZ); 
      break;			/*@@@---EXIT THE WHILE LOOP---@@@*/
      /* Buf_end_idx is now really end of buf: on the next call to this proc,
	 new input will append after remaining unprocessed input in buf.*/
    }
  }
}

/******************************************************************************
 * This is a "state machine" procedure that is called for every line of text
 * received from the webster server. The state of the webster program is
 * contained in the static variable webster_state which lets this procedure
 * handle the current line of text in a way that depends on what the previous
 * line of text was. For more info on the webster protocol that is embodied in
 * this procedure, do "/usr/bin/telnet 10.0.0.51 103" and then type "HELP<cr>".
 ******************************************************************************/
#define WEBSTER_READY 0
#define GET_WORDLIST 1
#define GET_XREFS 2
#define GET_DEFINITION 3
/*-----------------------------------------------------------------------------*/
static void Process_Webster_Line(line)
     char* line;
{
  static int num_xrefs;
  static int webster_state = WEBSTER_READY;
  static int current_wordlist_is_xrefs = FALSE;

  switch(webster_state) {
  case WEBSTER_READY:
    if (line == NULL) {
      Wordlist_Reset();
      Controlpanel_Reactivate(); /* ready for user input */
    } 
    else if (strncmp(line, "AMBIGUOUS ", 10) == 0) { /* returned by COMPLETE */
      int num_ambiguities = 0;
      sscanf(line, "AMBIGUOUS %d\n", &num_ambiguities);
      sprintf(temptext, "Ambiguous! Matches %d other words.", num_ambiguities);
      Controlpanel_Titlebar_Set_Label(temptext);
      XBell(display, 100);
      Wordlist_Reset();
      Controlpanel_Reactivate(); /* ready for user input */
      /* webster_state = WEBSTER_READY; */
    }
    else if (strncmp(line, "COMPLETION ", 11) == 0) { /* returned by COMPLETE */
      sscanf(line, "COMPLETION %s\n", temptext);
      Controlpanel_Set_Input_Word(temptext);
      Controlpanel_Titlebar_Set_Label("\000");
      Wordlist_Reset();
      Controlpanel_Reactivate(); /* ready for user input */
      /* webster_state = WEBSTER_READY; */
    }
    else if (strncmp(line, "MATCHS 0\n", 9) == 0) { /* returned by ENDINGS */
      XBell(display, 100);
      Controlpanel_Titlebar_Set_Label("No matching words!");
      Wordlist_Reset();
      Controlpanel_Reactivate(); /* ready for user input */
      /* webster_state = WEBSTER_READY; */
    }      
    else if (strncmp(line, "MATCHS\n", 7) == 0) { /* returned by ENDINGS */
      Controlpanel_Titlebar_Set_Label("Choose a word:");
      Wordlist_Reset();
      webster_state = GET_WORDLIST; current_wordlist_is_xrefs = FALSE;
    }
    else if (strncmp(line, "SPELLING 0\n", 11) == 0) { /* returned by DEFINE and SPELL */
      Controlpanel_Titlebar_Set_Label("No such word!");
      XBell(display, 100);
      Wordlist_Reset();
      Controlpanel_Reactivate(); /* ready for user input */
      /* webster_state = WEBSTER_READY; */
    }
    else if (strncmp(line, "SPELLING\n", 9) == 0) { /* returned by DEFINE */
      Controlpanel_Titlebar_Set_Label("Word not in dictionary. Do you mean:");
      Wordlist_Reset();
      webster_state = GET_WORDLIST; current_wordlist_is_xrefs = FALSE;
    }
    else if (strncmp(line, "WILD 0\n", 7) == 0) { /* returned by DEFINE */
      Controlpanel_Titlebar_Set_Label("No Match!");
      XBell(display, 100);
      Wordlist_Reset();
      Controlpanel_Reactivate();
      /* webster_state = WEBSTER_READY; */
    } 
    else if (strncmp(line, "WILD\n", 5) == 0) {	/* returned by DEFINE */
      Controlpanel_Titlebar_Set_Label("Choose a word:");
      Wordlist_Reset();
      webster_state = GET_WORDLIST; current_wordlist_is_xrefs = FALSE;
    }      
    else if (strncmp(line, "DEFINITION ", 11) == 0) { /* returned by DEFINE */
      sscanf(line, "DEFINITION %d\n", &num_xrefs);
      if (num_xrefs == 0) {
	/** don't clear the wordlist if last define req came from the wordlist **/
	if (Controlpanel_Cur_Word_Is_From_TextEdit()) {
	  Wordlist_Reset();
	  Controlpanel_Titlebar_Set_Label("\000");
	}
	webster_state = GET_DEFINITION;
      }
      else {
	/** don't clear old xrefs if new set of xrefs gend from a word in old xrefs **/
	if (Controlpanel_Cur_Word_Is_From_TextEdit() || !current_wordlist_is_xrefs)
	  Wordlist_Reset();
	Controlpanel_Titlebar_Set_Label("Cross References:");
	webster_state = GET_XREFS; current_wordlist_is_xrefs = TRUE;
      }
    }
    else if (strncmp(line, "ERROR RECOVERABLE", 17) == 0) {
      Controlpanel_Titlebar_Set_Label("Error:");
      Display_Def_Text_Reset();
      Display_Def_Text_Append(line);
      Display_Def_Refresh();
      XBell(display, 100); XBell(display, 100);
      Wordlist_Reset();
      Controlpanel_Reactivate();
      /* webster_state = WEBSTER_READY; */
    }
    else if (strncmp(line, "ERROR FATAL", 11) == 0) {
      XBell(display, 100); XBell(display, 100);
      XBell(display, 100); XBell(display, 100);
      Display_Def_Text_Reset();
      Display_Def_Text_Append(line);
      Display_Def_Refresh();
      Webster_Disconnect();
      Controlpanel_Reactivate();
    }
    else {
      sprintf(temptext, "unknown server message: %s", line);
      Controlpanel_Titlebar_Set_Label(temptext); 
      XBell(display, 100); XBell(display, 100);
      Wordlist_Reset();
      Controlpanel_Reactivate();
      /* webster_state = WEBSTER_READY; */
    }
    break;
  case GET_WORDLIST:		/* get words, one per line, till EOF reached */
    if (line == NULL) {		/* EOF reached */
      Wordlist_Show();
      Controlpanel_Reactivate(); /* ready for user input */
      webster_state = WEBSTER_READY;
    }
    else {
      sscanf(line, "%*d %[^\n]", temptext);
      Wordlist_Add(temptext);
      /* webster_state = GET_WORDLIST; */
    }
    break;
  case GET_XREFS:
    if (line == NULL) {
      Display_Def_Refresh();	/* show the definition */
      Controlpanel_Reactivate();
      webster_state = WEBSTER_READY;
      break;
    }
    sscanf(line, "%*d %[^\n]", temptext);
    Wordlist_Add(temptext);
    num_xrefs--;
    if (num_xrefs == 0) {
      Wordlist_Show();
      webster_state = GET_DEFINITION;
    }
    /* else
       webster_state = GET_XREFS; */
    break;
  case GET_DEFINITION:
    if (line == NULL) {		/* EOF reached */
      Controlpanel_Set_Input_Word(NULL);
      Display_Def_Refresh();	/* show the definition */
      Controlpanel_Reactivate(); /* ready for user input */
      webster_state = WEBSTER_READY;
    }
    else {
      Display_Def_Text_Append(line);
      /* webster_state = GET_DEFINITION; */
    }
    break;
  default:
    XBell(display, 100);
    Controlpanel_Titlebar_Set_Label("error -- xwebster programmer goofed");
    Controlpanel_Reactivate();
    webster_state = WEBSTER_READY;
    break;
  }
}

/******************************************************************************
 * returns 0 if it couldn't send, else 1.
 ******************************************************************************/
int Webster_Send(buf)
char *buf;
{
  extern int sys_nerr;
  extern char *sys_errlist[];
  extern int errno;

  if (webster_Socket == NULL)
    Webster_Reconnect();
  if (webster_Socket == NULL)
    return(0);
  if (send(webster_Socket, buf, strlen(buf), 0) < 0) {
    Display_Def_Text_Reset();
    if (errno < sys_nerr)
      (void) sprintf(temptext, "Error sending to webster server: %s\n", sys_errlist[errno]);
    else
      (void) strcpy(temptext, "Error sending to webster server: unknown error\n");
    Display_Def_Text_Append(temptext);
    Display_Def_Refresh();
    return(0);
  }
  return(1);
}

/******************************************************************************
 * originally from webster.c, but by now, highly modified.
 ******************************************************************************/
static int Webster_Connect_To_Server(host_addr, host_port, host_service_name)
  char* host_addr;
  int host_port;
  char* host_service_name;
{
  struct sockaddr_in sin;
  register int s;
  register struct servent *sp;

  /* Look up the host in the host file. */
  memset( (char*) &sin, '\000', sizeof(struct sockaddr_in));
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = inet_addr(host_addr);
  if (sin.sin_addr.s_addr == -1) {
    fprintf(stderr, "xwebster: %s: unknown host.\n", host_addr);
    return(-1);
  }

  if ((sp = getservbyname(host_service_name, "tcp")) == NULL)
    sin.sin_port = htons(host_port);
  else
    sin.sin_port = sp->s_port;

  /* Get a TCP socket. */
  if ((s = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    return(-1);
  }

  /* Try to connect. */
  if (connect(s, &sin, sizeof(struct sockaddr_in)) < 0) {
    return(-1);
  }
  return(s);
}

/******************************************************************************
 * Try to connect up to a webster server. Xwebster will try each host address
 * in the Xdefault hostAddrList successively until it connects succesfully
 * or until it runs out of hosts to try to connect.
 ******************************************************************************/
static void Webster_Reconnect()
{
  extern int   sys_nerr;
  extern char* sys_errlist[];
  extern int   errno;
  extern char* strtok();	/* if you don't have this, get the source from 
				   Henry Spencer's public domain strings package */

  char* host_addr = strtok(user_prefs.host_addr_list, " \t");
  Display_Def_Text_Reset();
  do {
    sprintf(temptext, "Trying webster server at address %s ...\n", host_addr);
    Display_Def_Text_Append(temptext);
    Display_Def_Refresh();
    XSync(display, FALSE);
    while (XtPending()) {	/* since we can't get back to XtMainLoop yet-*/
      XEvent event;		/*-we process the events here... (yech) */
      XtNextEvent(&event);
      XtDispatchEvent(&event);
    }
    webster_Socket = Webster_Connect_To_Server(host_addr, user_prefs.host_port, 
					       user_prefs.host_service_name);
    if (webster_Socket < 0) {
      Display_Def_Text_Reset();
      (void) sprintf(temptext, "Problem with server host %s\n", host_addr);
      Display_Def_Text_Append(temptext);
      if (errno < sys_nerr)
	(void) sprintf(temptext, "webster server error: %s\n", sys_errlist[errno]);
      else
	(void) strcpy(temptext, "webster server error: unknown error\n");
      Display_Def_Text_Append(temptext);
    }
  } while ((webster_Socket < 0) && ((host_addr = strtok(NULL, " \t")) != NULL));
	
  if (webster_Socket < 0) {
    Controlpanel_Titlebar_Set_Label("Error:");
    Display_Def_Text_Reset();
    Display_Def_Text_Append("Couldn't access a webster host. Be sure that the following Xdefaults\n");
    Display_Def_Text_Append("have been set correctly for accessing your Webster server:\n");
    Display_Def_Text_Append("*hostAddrList *hostPort *hostServiceName\n");
    sprintf(temptext, "See %s/Xwebster for details.\n", APPDEFAULTSDIR);
    Display_Def_Text_Append(temptext);
    Display_Def_Refresh();
    webster_Socket = NULL;  /* indicates we still don't have server connection */
  } 
  else {
    Controlpanel_Titlebar_Set_Label(xwebster_version);
    Display_Def_Text_Reset();
    webster_xinput = XtAddInput(webster_Socket, XtInputReadMask,
				Webster_Get_Server_Output, NULL);
    webster_xexcept = XtAddInput(webster_Socket, XtInputExceptMask,
				 Webster_Handle_Server_Exception, NULL);
  }
}

/******************************************************************************
 * This procedure is called indirectly, via XtAddInput() callback from 
 * XtMainLoop() whenever an exception occurs on webster_Socket. This callback
 * is setup in procedure Webster_Reconnect().
 ******************************************************************************/
static XtInputCallbackProc Webster_Handle_Server_Exception(client_data, source_fildes, id)
     caddr_t   client_data;
     int       source_fildes;
     XtInputId id;
{
  Webster_Disconnect();
}

/******************************************************************************
 *
 ******************************************************************************/
static void Webster_Disconnect()
{
  extern int   sys_nerr;
  extern char* sys_errlist[];
  extern int   errno;

  Controlpanel_Titlebar_Set_Label("Webster Server Error:");
  Display_Def_Text_Reset();
  if (errno < sys_nerr)
    (void) sprintf(temptext, "Webster server error: %s\n", sys_errlist[errno]);
  else
    (void) strcpy(temptext, "Webster server error: unknown error\n");
  Display_Def_Text_Append(temptext);
  Display_Def_Text_Append("Webster server connection closed.\n");
  Display_Def_Refresh();
  XtRemoveInput(webster_xinput);
  XtRemoveInput(webster_xexcept);
  close(webster_Socket);
  webster_Socket = NULL;
  /* No point reconnecting - we'll wait till we try to do something. */
}
