/* -*-C-*-
********************************************************************************
*
* File:         winterp.c
* RCS:          $Header: winterp.c,v 1.2 89/11/25 04:59:12 mayer Exp $
* Description:  WINTERP main() file.
* Author:       Niels Mayer, HPLabs
* Created:      Sat Jun 10 02:15:35 1989
* Modified:     Sat Nov 25 04:58:48 1989 (Niels Mayer) mayer@hplnpm
* Language:     C
* Package:      N/A
* Status:       X11r4 contrib tape release
*
* WINTERP 1.0 Copyright 1989 Hewlett-Packard Company (by Niels Mayer).
* XLISP version 2.1, Copyright (c) 1989, by David Betz.
*
* Permission to use, copy, modify, distribute, and sell this software and its
* documentation for any purpose is hereby granted without fee, provided that
* the above copyright notice appear in all copies and that both that
* copyright notice and this permission notice appear in supporting
* documentation, and that the name of Hewlett-Packard and David Betz not be
* used in advertising or publicity pertaining to distribution of the software
* without specific, written prior permission.  Hewlett-Packard and David Betz
* make no representations about the suitability of this software for any
* purpose. It is provided "as is" without express or implied warranty.
*
* HEWLETT-PACKARD AND DAVID BETZ DISCLAIM ALL WARRANTIES WITH REGARD TO THIS
* SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS,
* IN NO EVENT SHALL HEWLETT-PACKARD NOR DAVID BETZ BE LIABLE FOR ANY SPECIAL,
* INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
* LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
* OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
* PERFORMANCE OF THIS SOFTWARE.
*
* See ./winterp/COPYRIGHT for information on contacting the authors.
* 
* Please send modifications, improvements and bugfixes to mayer@hplabs.hp.com
* Post XLISP-specific questions/information to the newsgroup comp.lang.lisp.x
**
********************************************************************************
*/
static char rcs_identity[] = "@(#)$Header: winterp.c,v 1.2 89/11/25 04:59:12 mayer Exp $";

#include <stdio.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <X11/Intrinsic.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>
#include "winterp.h"
#include "user_prefs.h"
#include "xlisp/xlisp.h"
#include "w_funtab.h"

/* external/forward routines */
static XtInputCallbackProc Read_Eval_Print();
static int                 Initialize_Server_Socket();
static XtCallbackProc      Quit_Button_Callback();
extern FILE*               osaopen();
static XtErrorMsgHandler   Winterp_Xtoolkit_Error_Handler();
static XtErrorMsgHandler   Winterp_Xtoolkit_Warning_Handler();

/* external variables */
extern LVAL s_stdin,s_evalhook,s_applyhook;
extern LVAL s_1plus,s_2plus,s_3plus,s_1star,s_2star,s_3star,s_minus;
extern int xltrcindent;
extern int xldebug;
extern LVAL true;
extern FILE *tfp;

/* global variables */
jmp_buf		top_level;
CONTEXT		cntxt;
int		read_eval_print_just_called;
int		lisp_reader_hit_eof;
char*		progname = NULL;
static int	client_listen_socket = NULL;
Widget		toplevel_Wgt = NULL;
Display*	display;
Window		root_win;
Screen*		screen;
Colormap	colormap;
USER_PREFS_DATA user_prefs;	/* extern declared in user_prefs.h, really here */
char		temptext[BUFSIZ]; /* a temporary text buffer, for sprintf() */
Arg		_args[10];	/* for XtSetArg() macros in winterp.h */
int		_num_args;	/* for XtSetArg() macros in winterp.h */

/* 
 * Data on how user-customization resources are interpreted:
 * this must be kept up to date with data structure USER_PREFS_DATA_PTR 
 * in user_prefs.h
 *
 * My philosophy is to place all my intended defaults in 
 * /usr/lib/X11/app-defaults/Winterp, so that the program doesn't have to
 * be recompiled just to change a simple default. Thus I don't always have
 * the best defaults in this structure.
 */
static XtResource resources[] = {
  /*
   * The port number of the widget interpreter lisp server.
   */
  {"servicePort", "ServicePort",
     XmRInt, sizeof(int),
     XtOffset(USER_PREFS_DATA_PTR, service_port),
     XmRString, "23751"},
  /*
   * The service name of the widget interpreter lisp server.
   */
  {"serviceName", "ServiceName",
     XmRString, sizeof(String),
     XtOffset(USER_PREFS_DATA_PTR, service_name),
     XmRString, "widget_interp"},

  /*
   * The name of the file to load to initialize xlisp.
   */
  {"lispInitFile", "LispInitFile",
     XmRString, sizeof(String),
     XtOffset(USER_PREFS_DATA_PTR, lisp_init_file),
     XmRString, "init.lsp"},

  /*
   * The name of the file to output lisp transactions.
   */
  {"lispTranscriptFile", "LispTranscriptFile",
     XmRString, sizeof(String),
     XtOffset(USER_PREFS_DATA_PTR, lisp_transcript_file),
     XmRString, NULL}
};


/*
 * Setup an action table for winterp. Note that action procedure "Lisp"
 * is a special action procedure that calls the lisp evaluator on the
 * parameters of the action. A translation like
 * "Ctrl<Key>K: Lisp(quack 1 2 3)" will evaluate '(quack 1 2 3)'
 */
extern void Wtx_Winterp_Lisp_Action_Proc(); /* w_txlations.c */
static XtActionsRec winterp_action_table[] = {
  {"Lisp", Wtx_Winterp_Lisp_Action_Proc}
};


/*******************************************************************************
 * main - the main routine
 ******************************************************************************/
main(argc,argv)
  int argc; char *argv[];
{
  XEvent event;
  progname = argv[0];

  /* sanity check to ensure that the number of pointers to funtab entries in
     w_funtab.h corresnpond to the number of entries in w_funtab.c:funtab[]. */
  if (LAST_FUNTAB_POINTER_USED_BY_libWinterp != INDEX_OF_LAST_FUNTAB_ENTRY_USED_BY_libWinterp) {
    fprintf(stderr,"%s: Fatal error: w_funtab.c out of sync with w_funtab.h\n", progname);
    exit(1);
  }

  /* 
   * Initialize the toolkit -- we need toplevel_Wgt so that we can
   * have around a "default" set of X structures (colors, graphics contexts
   * etc) for use by XtConvert()... this is a kludge. Nothing seems
   * to work right unless you create this initial window.
   */
  toplevel_Wgt = XtInitialize(argv[0], "Winterp", NULL, 0, &argc, argv);

  /*
   * place resources in user_prefs global struct.
   */
  XtGetApplicationResources(toplevel_Wgt, &user_prefs, 
			    resources, XtNumber(resources), NULL, 0);

  /*
   * Setup action table for accelerators and translations.
   */
  XtAddActions(winterp_action_table, XtNumber(winterp_action_table));

  /* 
   * This is bogus but necessary... see comments for XtInitialize()...
   * If you don't like the "quit_button" window around, just set up
   * Xdefaults so it appears offscreen.
   */
  { 
    Widget quit_button_Wgt = XtCreateManagedWidget("quit_button", 
						   xmPushButtonWidgetClass,
						   toplevel_Wgt, NULL, 0);
    XtAddCallback(quit_button_Wgt, XmNactivateCallback, Quit_Button_Callback, NULL);
  }

  XtRealizeWidget(toplevel_Wgt);

  /*
   * initialize some global variables used throughout this program.
   * NOTE: if winterp ever gets changed to use application contexts enabling
   * multiple displays, screens, etc, then we'll have to make some major changes
   * here, and to any primitives that use these values.
   */
  display = XtDisplay(toplevel_Wgt); 
  root_win = DefaultRootWindow(display);
  screen = XtScreen(toplevel_Wgt);
  colormap = XDefaultColormapOfScreen(screen);

  /*
   * get a socket to listen on. when it's selected, call Read_Eval_Print()
   * to open a connection socket, process the client request, and close the sockt
   */
  client_listen_socket = Initialize_Server_Socket();
  XtAddInput(client_listen_socket, XtInputReadMask, Read_Eval_Print, NULL);

  /*
   * Setup Xtoolkit warning and error handlers so that errors inside
   * the Xtoolkit will call xlerror().
   */
  XtSetErrorHandler(Winterp_Xtoolkit_Error_Handler);
  XtSetWarningHandler(Winterp_Xtoolkit_Warning_Handler);

  /* 
   * Startup XLISP
   */
  osinit("Initializing ...");

  /* setup initialization error handler */
  xlbegin(&cntxt,CF_TOPLEVEL|CF_CLEANUP|CF_BRKLEVEL,(LVAL)1);
  if (setjmp(cntxt.c_jmpbuf))
    xlfatal("fatal initialization error");
  if (setjmp(top_level))
    xlfatal("RESTORE not allowed during initialization");

  /* initialize xlisp */
  xlinit();
  Wso_Init();			/* w_savedobjs.c */
  Wres_Init();			/* w_resources.c */
  Wcb_Init();			/* w_callbacks.c */
  Wto_Init();			/* w_timeouts.c */
  Wtx_Init();			/* w_txlations.c */
  Weh_Init();			/* w_evnthndlr.c */

  /* 
   * The following create interfaces to all the motif widget-classes via
   * xlisp classes, by calling Wcls_Create_Subclass_Of_WIDGET_CLASS()
   * with o_WIDGET_CLASS (def'd in Wc_WIDGET_Init()) as their superclass.
   * Methods on the specific widget classes correspond to 
   * special operations pertaining to that class, and not to others. 
   * These derived classes may override the 'Widget_Class' :isnew method 
   * for cases where motif "convenience" functions are used to create the 
   * widget. Additionally, since different classes generate different callback
   * structures, certain widgetclasses may override the metaclass' :set_callback
   * and :add_callback methods so as to allow dereferencing of the appropriate
   * callback structure elements.
   */
  Wc_WIDGET_Init();		/* WIDGET_CLASS metaclass */
  Wc_SHELL_Init();		/* SHELL and POPUP_SHELL metaclasses */
  Wc_ArrowB_Init();
  Wc_BulletinB_Init();
  Wc_CascadeB_Init();
  Wc_Command_Init();
  Wc_DrawingA_Init();
  Wc_DrawnB_Init();
  Wc_FileSB_Init();
  Wc_Form_Init();
  Wc_Frame_Init();
  Wc_Label_Init();
  Wc_List_Init();
  Wc_MainW_Init();
  Wc_MessageB_Init();
  Wc_PanewW_Init();
  Wc_PushB_Init();
  Wc_RowColumn_Init();
  Wc_Scale_Init();
  Wc_ScrollBar_Init();
  Wc_ScrolledW_Init();
  Wc_SelectioB_Init();
  Wc_Separator_Init();
  Wc_Text_Init();
  Wc_ToggleB_Init();
  xlend(&cntxt);

  /* reset the error handler */
  xlbegin(&cntxt,CF_TOPLEVEL|CF_CLEANUP|CF_BRKLEVEL,true);

  /* open the transcript file */
  if (user_prefs.lisp_transcript_file && (tfp = osaopen(user_prefs.lisp_transcript_file,"w")) == NULL) {
    sprintf(temptext,"error: can't open transcript file: %s",user_prefs.lisp_transcript_file);
    stdputstr(temptext);
  }

  /* load "init.lsp" */
  if (setjmp(cntxt.c_jmpbuf) == 0)
    xlload(user_prefs.lisp_init_file,TRUE,FALSE);

  /* setup longjmp target for restore */
  if (setjmp(top_level))
    xlbegin(&cntxt,CF_TOPLEVEL|CF_CLEANUP|CF_BRKLEVEL,true);
  
  printf("================================================================================\n");
  printf("WINTERP -- Widget INTERPreter by Niels P. Mayer (mayer@hplabs.hp.com).\n");
  printf("	WINTERP version 1.0, Copyright (c) 1989 Hewlett-Packard Company\n");
  printf("	XLISP version 2.1, Copyright (c) 1989, by David Betz\n");
  printf("\n");
  printf("XLisp server ready for input on port %d using service %s\n",
	 user_prefs.service_port, user_prefs.service_name);
  printf("================================================================================\n");

  read_eval_print_just_called = TRUE; /* special initial cond */
  lisp_reader_hit_eof = FALSE;

  /* Process X Events and Lisp client requests forever */
  for (;;) {
    /* 
     * We need to setup a new error return only after each time that an XLISP 
     * evaluation occurs. Therefore, we check for read_eval_print_just_called 
     * (which is set by Read_Eval_Print()) and then clear it once the setjmp() 
     * has been done. This avoids setting up an error return for each X event
     * being processed in this loop. 
     */
    if (read_eval_print_just_called) {
      read_eval_print_just_called = FALSE;
      if (lisp_reader_hit_eof) 
	break;
      if (setjmp(cntxt.c_jmpbuf)) { /* longjmp target for error return */
	setvalue(s_evalhook, NIL);
	setvalue(s_applyhook, NIL);
	xltrcindent = 0;
	xldebug = 0;
        xlflush();		/* needed if using (read)/(read-line) from stdin */ 
      }
      stdputstr("XLisp> ");	/* print a "prompt" */
      fflush(stdout); fflush(stderr); /* otherwise output won't happen while blocked in XtNextEvent() */
    }

    /*
     * XtNextEvent() waits for Xevents, and while it is waiting, it will
     * process inputs added via XtAddInput() or XtAddWorkProc(). Lisp 
     * server input will cause Read_Eval_Print() to get called, and that
     * procedure sets the globals lisp_reader_hit_eof and 
     * read_eval_print_just_called. Read_Eval_Print() sends a bogus 
     * XAnyEvent (event.type == 0) so as to force XtNextEvent() to return; 
     * otherwise it would only return if a lisp evaluation caused X events 
     * to be generated, which means that XLISP error returns for non-X 
     * evaluations wouldn't get set up properly.
     *
     * XtDispatchEvent() will dispatch the actions from the events gathered
     * by XtNextEvent(). Note that XtDispatchEvent() ignores the aforementioned
     * bogus events: "if (event->type == 0) return;"
     */
    XtNextEvent(&event);
    XtDispatchEvent(&event);
  }
  wrapup();			/* this is also called if we exit by hitting
				   the quit_button, or by evaling expr (quit) */
}


/******************************************************************************
 * initialize server, returning a socket that can be listened on.
 ******************************************************************************/
static int Initialize_Server_Socket()
{
  int                ls;	/* socket descriptor */
  struct servent    *sp;	/* pointer to service information */
  struct sockaddr_in myaddr_in;	/* for local socket address */

  /* clear out address structure */
  memset ((char *)&myaddr_in, 0, sizeof(struct sockaddr_in));
  
  /* Set up address structure for the listen socket. */
  myaddr_in.sin_family = AF_INET;
  myaddr_in.sin_addr.s_addr = INADDR_ANY;
  
  /* Find the information for the server to get the needed port number. */
  if (user_prefs.service_port != NULL)
    myaddr_in.sin_port = htons(user_prefs.service_port);
  else {
    if ((sp = getservbyname(user_prefs.service_name, "tcp")) == NULL) {
      fprintf(stderr,"%s: unable to get servicename from /etc/services or from YP\n",
	      progname);
      exit(1);
    }
    myaddr_in.sin_port = sp->s_port;
  }
  
  /* Create the listen socket. */
  if ((ls = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror(progname);
    fprintf(stderr, "%s: unable to create socket\n", progname);
    exit(1);
  }
  
  /* Bind the listen address to the socket. */
  if (bind(ls, &myaddr_in, sizeof(struct sockaddr_in)) == -1) {
    perror(progname);
    fprintf(stderr, "%s: unable to bind socket\n", progname);
    exit(1);
  }

  /* Initiate the listen on the socket so remote users
   * can connect.  The listen backlog is set to 5, which
   * is the largest currently supported.
   */
  if (listen(ls,5) == -1) {
    perror(progname);
    fprintf(stderr, "%s: unable to listen\n", progname);
    exit(1);
  }
  
  setpgrp();
  return(ls);
}


/******************************************************************************
 * This procedure is called (indirectly, via XtAddInput() callback) from 
 * XtNextEvent() in main() and from XtNextEvent() in 
 * xldbug.c:breakloop(). This callback will be called whenever new input 
 * appears on client_listen_socket indicating that a new connection has been 
 * requested and that another s-expression is ready to be evaluated by Xlisp. 
 * This procedure will accept that connection and read all the data from the 
 * client and send it off to the XLisp reader, and the Xlisp evaluator. 
 * The results of the evaluation are printed.
 ******************************************************************************/
static XtInputCallbackProc Read_Eval_Print(client_data, source_fildes, id)
     caddr_t    client_data;
     int*       source_fildes;
     XtInputId* id;
{
  int rdsock;
  static char rdbuf[BUFSIZ];

  /* 
   * set this global flag so that main() and breakloop() will set up an error 
   * handler for the next call to the lisp evaluator.
   */
  read_eval_print_just_called = TRUE; 
  { 
    /*
     * Accept the request on client_listen_socket, and open a socket for
     * reading rdsock.
     */
    long linger = 1L;
    int addrlen = sizeof(struct sockaddr_in);
    struct sockaddr_in peeraddr_in; /* for peer socket address */
    memset((char *)&peeraddr_in, 0, sizeof(struct sockaddr_in));
    if ((rdsock = accept(client_listen_socket, &peeraddr_in, &addrlen)) == -1) {
      perror(progname);
      sprintf(temptext,"%s: unable to accept\n",progname);
      xlfatal(temptext);		/* CLEANUP & EXIT */
    }
    if (setsockopt(rdsock, SOL_SOCKET, SO_LINGER, (char *) &linger, sizeof(long)) == -1) {
      perror(progname);
      sprintf(temptext, "%s: unable to setsockopt\n", progname);
      xlfatal(temptext);		/* CLEANUP & EXIT */
    }
  }
  {
    /*
     * Read the sexpression from the socket -- note assumption that entire
     * sexpression is sent in one "packet" and then the socket is closed.
     */
    int len, i;
    LVAL sexp_stream, new_elt, last_elt = NIL;
    xlsave1(sexp_stream);	/* protect from gc */
    sexp_stream = newustream();	/* note - stream obj has ptrs for head and tail*/

    while (len = recv(rdsock, rdbuf, BUFSIZ, 0)) { /* read len characters into rdbuf */
      if (len < 0) {
	perror(progname);
	sprintf(temptext, "%s: unable to recv\n",progname);
	xlfatal(temptext);		/* CLEANUP & EXIT */
      }

      /* foreach character received, stuff it into an xlisp unnamed stream */
      for (i = 0; i < len; i++) {
	new_elt = cons(cvchar(rdbuf[i]), NIL);
	if (last_elt) {		/* if we've already created the head of the stream */
	  rplacd(last_elt, new_elt); /* add new_elt to the tail of the list */
	  last_elt = new_elt;	/* increment last_elt pointer */
	}
	else {			/* else create the head of the stream */
	  sethead(sexp_stream, new_elt);
	  last_elt = new_elt;
	}
      }
    }
    if (last_elt)
      settail(sexp_stream, last_elt);	/* streams are cdr-coded -- give ptr to tail */
    else			
      sexp_stream = NIL;	/* loop never executed, no characters read. */
    lisp_reader_hit_eof = !(Read_From_Stream_Eval_And_Print(sexp_stream));
    xlpop(/*sexp_stream*/);
  }
  close(rdsock);		/* we've finished reading from the socket */

  /* TODO -- 
     (1) make the client program, wl, wait until the evaluation is done. This will
     ensure that we don't get into a "race condition" with gnumeacs' winterp-mode --
     It is possible that winterp will still be reading winterp-mode's tempfile
     as gnuemacs writes another copy of this file. This can happen when a user
     is giving the gnuemacs winterp-send-defun command faster than winterp can
     read the files being sent to it.
     
     (2) send the results of the evaluation back to the client program wl, 
     have it print the results on stdout. Furthermore, if the form sent to
     winterp by wl results in a lisp error, wl should return a nonzero exitstatus. 
     
     This would be trivial, except that we'd want to send stdout and stderr
     back as well. If we were to use only the xlisp xlio.c routiunes for printing
     We could conceivably set the lisp symbols *standard-output* *debug-output*
     and *trace-output* so that they print to a stream, and just shove these
     streams back at the client.
  */

  /*
   * HACK CAUSED BY LAME IMPLEMENTATION OF XtMainLoop/XtNextEvent:
   * This creates a bogus event so as to force XtNextEvent to return, even if
   * the lisp evaluation didn't result in any new events being generated. 
   * The problem was that XtAddInput callbacks were being handled entirely 
   * within XtNextEvent(). Thus, once this procedure exited, XtNextEvent() 
   * would block waiting for a "real event", and never exit until an XEvent 
   * occured. XLISP requires that a new setjmp/longjmp error return be setup 
   * before each new lisp evaluation, and that couldn't happen unless 
   * XtNextEvent exited and allowed a new execution context to be created.
   *
   * Although I could do a call to XEventsQueued(display, QueuedAfterFlush)
   * in order to determine whether a bogus event needs to be sent, my hunch
   * is that the extra XFlush() caused by that operation would be more 
   * inefficient than processing/discarding the extra bogus event each time
   * a sexp is sent to the lisp server.
   */
  {
    XEvent bogus_event;
    bogus_event.type = 0;	/* XAnyEvent type --> ignored by XtDispatchEvent() */
    bogus_event.xany.display = display;
    bogus_event.xany.window  = XtWindow(toplevel_Wgt);;
    XPutBackEvent(display, &bogus_event);
  }
}

/*******************************************************************************
 * This fn reads from it's input, which is assumed to be a xlisp stream.
 * returns false if EOF hit during read.
 ******************************************************************************/
int Read_From_Stream_Eval_And_Print(sexp_stream)
     LVAL sexp_stream;		/* make sure this is a stream, and not other LVAL */
{
  LVAL rep_expr;
  int read_result;

  xlprot1(sexp_stream);		/* protect against GC */
    
  /* Read Evaluate and Print the expression in sexp_stream */
  if ((read_result = xlread(sexp_stream, &rep_expr, FALSE))) {

    /* save the last expression returned by the reader */
    setvalue(s_3plus, getvalue(s_2plus));
    setvalue(s_2plus, getvalue(s_1plus));
    setvalue(s_1plus, getvalue(s_minus));
    setvalue(s_minus, rep_expr);

    /* evaluate the expression returned by the reader */
    rep_expr = xleval(rep_expr);

    /* save the last expression returned by the evaluator */
    setvalue(s_3star,getvalue(s_2star));
    setvalue(s_2star,getvalue(s_1star));
    setvalue(s_1star,rep_expr);

    if (xldebug)		/* print eval results */
      dbgprint(rep_expr);
    else
      stdprint(rep_expr);
  }

  else {			/* if reader hit EOF, just print a new line */
    if (xldebug)
      dbgputstr("\n");   
    else
      stdputstr("\n");
  }
  xlpop(/*sexp_stream*/);
  return (read_result);		/* return FALSE if hit EOF */
}


/*******************************************************************************
 * xlfatal - print a fatal error message and exit
 ******************************************************************************/
xlfatal(msg)
  char *msg;
{
  close(client_listen_socket);
  fprintf(stderr, "%s -- error: %s\n", progname, msg);
  if (tfp)
    fclose(tfp);
  exit(1);
}


/*******************************************************************************
 * wrapup - clean up and exit to the operating system. 
 * This is also called in xlsys.c:xexit().
 ******************************************************************************/
wrapup()
{
  stdputstr("\n");
  close(client_listen_socket);
  if (tfp)
    fclose(tfp);
  exit(0);
}


/*******************************************************************************
 *
 ******************************************************************************/
static XtCallbackProc Quit_Button_Callback(widget, client_data, call_data)
     Widget widget;
     caddr_t client_data;
     caddr_t call_data;
{
  wrapup();
}


/*******************************************************************************
 * This handles fatal errors from the Xtoolkit. According to the Xtoolkit
 * docs, such a handler should terminate the application. In this case,
 * however, we suggest to the user that the application be terminated, but
 * don't actually do it. This may allow the user to figure out what went 
 * wrong by poking around inside the lisp environment.
 *
 * This is set up in main() via XtSetErrorHandler(). Note that the default
 * error handler is _XtDefaultError().
 ******************************************************************************/
static XtErrorMsgHandler Winterp_Xtoolkit_Error_Handler(message)
     String message;
{
  sprintf(temptext, "X Toolkit Fatal Error -- PLEASE QUIT AND RESTART THIS APPLICATION -- %s\n", message);
  xlfail(temptext);
}


/*******************************************************************************
 * This handles nonfatal errors from the Xtoolkit.
 *
 * This is set up in main() via XtSetWarningHandler(). Note that the default
 * error handler is _XtDefaultWarning().
 ******************************************************************************/
static XtErrorMsgHandler Winterp_Xtoolkit_Warning_Handler(message)
     String message;
{
  sprintf(temptext, "X Toolkit Warning: %s\n", message);
  xlfail(temptext);
}
