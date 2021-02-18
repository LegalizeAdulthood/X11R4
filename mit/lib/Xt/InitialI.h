/* $XConsortium: InitialI.h,v 1.35 90/08/31 08:13:11 swick Exp $ */

/***********************************************************
Copyright 1987, 1988 by Digital Equipment Corporation, Maynard, Massachusetts,
and the Massachusetts Institute of Technology, Cambridge, Massachusetts.

                        All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the names of Digital or MIT not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

/****************************************************************
 *
 * Displays
 *
 ****************************************************************/

#include <sys/param.h>				/* to get MAXPATHLEN */
#ifndef MAXPATHLEN
#define MAXPATHLEN 256
#endif

#ifndef MAX
#define MAX(a,b) (((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a,b) (((a) < (b)) ? (a) : (b))
#endif

#include "fd.h"

typedef struct _TimerEventRec {
        struct timeval        te_timer_value;
	struct _TimerEventRec *te_next;
	Display		      *te_dpy;
	XtTimerCallbackProc   te_proc;
	XtAppContext	      app;
	XtPointer	      te_closure;
} TimerEventRec;

typedef struct _InputEvent {
	XtInputCallbackProc   ie_proc;
	XtPointer	      ie_closure;
	struct _InputEvent    *ie_next;
	struct _InputEvent    *ie_oq;
	XtAppContext	      app;
	int		      ie_source;
	XtInputMask	      ie_condition;
} InputEvent;

typedef struct _WorkProcRec {
	XtWorkProc proc;
	XtPointer closure;
	struct _WorkProcRec *next;
	XtAppContext app;
} WorkProcRec;


typedef struct 
{
  	Fd_set rmask;
	Fd_set wmask;
	Fd_set emask;
	int	nfds;
	int	count;
} FdStruct;

typedef struct _ProcessContextRec {
    XtAppContext	defaultAppContext;
    XtAppContext	appContextList;
    ConverterTable	globalConverterTable;
} ProcessContextRec, *ProcessContext;

typedef struct {
    char*	start;
    char*	current;
    int		bytes_remaining;
} Heap;

typedef struct _DestroyRec DestroyRec;

typedef struct _XtAppStruct {
    XtAppContext next;		/* link to next app in process context */
    ProcessContext process;	/* back pointer to our process context */
    CallbackList destroy_callbacks;
    Display **list;
    TimerEventRec *timerQueue;
    WorkProcRec *workQueue;
    InputEvent **input_list;
    InputEvent *outstandingQueue;
    XrmDatabase errorDB;
    XtErrorMsgHandler errorMsgHandler, warningMsgHandler;
    XtErrorHandler errorHandler, warningHandler;
    struct _ActionListRec *action_table;
    ConverterTable converterTable;
    unsigned long selectionTimeout;
    FdStruct fds;
    short count;			/* num of assigned entries in list */
    short max;				/* allocate size of list */
    short last;
    Boolean sync, being_destroyed, error_inited;
#ifndef NO_IDENTIFY_WINDOWS
    Boolean identify_windows;		/* debugging hack */
#endif
    Heap heap;
    String * fallback_resources;	/* Set by XtAppSetFallbackResources. */
    struct _ActionHookRec* action_hook_list;
    int destroy_list_size;		/* state data for 2-phase destroy */
    int destroy_count;
    int dispatch_level;
    DestroyRec* destroy_list;
    Widget in_phase2_destroy;
} XtAppStruct;

extern char* _XtHeapAlloc(
#if NeedFunctionPrototypes
    Heap*	/* heap */,
    Cardinal	/* size */
#endif
);

extern void _XtSetDefaultErrorHandlers(
#if NeedFunctionPrototypes
    XtErrorMsgHandler*	/* errMsg */,
    XtErrorMsgHandler*	/* warnMsg */,
    XtErrorHandler*	/* err */,
    XtErrorHandler*	/* warn */
#endif
);

extern void _XtSetDefaultSelectionTimeout(
#if NeedFunctionPrototypes
    unsigned long* /* timeout */
#endif
);

extern void _XtSetDefaultConverterTable(
#if NeedFunctionPrototypes
    ConverterTable* /* table */
#endif
);

extern void _XtFreeConverterTable(
#if NeedFunctionPrototypes
    ConverterTable /* table */
#endif
);

extern XtAppContext _XtDefaultAppContext(
#if NeedFunctionPrototypes
    void
#endif
);

extern ProcessContext _XtGetProcessContext(
#if NeedFunctionPrototypes
    void
#endif
);

extern void _XtDestroyAppContexts(
#if NeedFunctionPrototypes
    void
#endif
);

extern void _XtCloseDisplays(
#if NeedFunctionPrototypes
    void
#endif
);

extern int _XtAppDestroyCount;
extern int _XtDpyDestroyCount;

extern int _XtwaitForSomething(
#if NeedFunctionPrototypes
#if NeedWidePrototypes
    /* Boolean */ int	/* ignoreTimers */,
    /* Boolean */ int	/* ignoreInputs */,
    /* Boolean */ int	/* ignoreEvents */,
    /* Boolean */ int	/* block */,
#else
    Boolean 		/* ignoreTimers */,
    Boolean 		/* ignoreInputs */,
    Boolean 		/* ignoreEvents */,
    Boolean 		/* block */,
#endif /*NeedWidePrototypes*/
    unsigned long*	/* howlong */,
    XtAppContext 	/* app */
#endif
);

typedef struct {		/* support for XtGetGC */
    Screen* screen;		/* root to which drawables apply */
    Drawable* drawables;	/* list of drawables, indexed by depth */
    Cardinal drawable_count;	/* num entries in above list */
} ScreenDrawablesRec, *ScreenDrawables;

typedef struct _XtPerDisplayStruct {
    CallbackList destroy_callbacks;
    Region region;
    XtCaseProc defaultCaseConverter;
    XtKeyProc defaultKeycodeTranslator;
    XtAppContext appContext;
    KeySym *keysyms;                   /* keycode to keysym table */
    int keysyms_per_keycode;           /* number of keysyms for each keycode*/
    int min_keycode, max_keycode;      /* range of keycodes */
    KeySym *modKeysyms;                /* keysym values for modToKeysysm */
    ModToKeysymTable *modsToKeysyms;   /* modifiers to Keysysms index table*/
    unsigned char isModifier[32];      /* key-is-modifier-p bit table */
    KeySym lock_meaning;	       /* Lock modifier meaning */
    Modifiers mode_switch;	       /* keyboard group modifiers */
    Boolean being_destroyed;
    Boolean rv;			       /* reverse_video resource */
    XrmName name;		       /* resolved app name */
    XrmClass class;		       /* application class */
    Heap heap;
    struct _GCrec *GClist;	       /* support for XtGetGC */
    ScreenDrawables drawable_tab;      /* ditto for XtGetGC */
    String language;		       /* XPG language string */
    Atom xa_wm_colormap_windows;       /* the WM_COLORMAP_WINDOWS atom.
					  this is currently only used in 
					  XtSetColormapWindows. */
    Time last_timestamp;	       /* from last event dispatched */
    int multi_click_time;	       /* for XtSetMultiClickTime */
    struct _TMContext* tm_context;     /* for XtGetActionKeysym */
    CallbackList mapping_callbacks;    /* special case for TM */
    XtPerDisplayInputRec pdi;	       /* state for modal grabs & kbd focus */
} XtPerDisplayStruct, *XtPerDisplay;

typedef struct _PerDisplayTable {
	Display *dpy;
	XtPerDisplayStruct perDpy;
	struct _PerDisplayTable *next;
} PerDisplayTable, *PerDisplayTablePtr;

extern PerDisplayTablePtr _XtperDisplayList;

extern XtPerDisplay _XtSortPerDisplayList(
#if NeedFunctionPrototypes
    Display* /* dpy */
#endif
);

/*
extern XtPerDisplay _XtGetPerDisplay( Display* );
extern XtPerDisplayInputRec* _XtGetPerDisplayInput( Display* );
*/

#ifdef DEBUG
#define _XtGetPerDisplay(display) \
    ((_XtperDisplayList != NULL && (_XtperDisplayList->dpy == (display))) \
     ? &_XtperDisplayList->perDpy \
     : _XtSortPerDisplayList(display))
#define _XtGetPerDisplayInput(display) \
    ((_XtperDisplayList != NULL && (_XtperDisplayList->dpy == (display))) \
     ? &_XtperDisplayList->perDpy.pdi \
     : &_XtSortPerDisplayList(display)->pdi)
#else
#define _XtGetPerDisplay(display) \
    ((_XtperDisplayList->dpy == (display)) \
     ? &_XtperDisplayList->perDpy \
     : _XtSortPerDisplayList(display))
#define _XtGetPerDisplayInput(display) \
    ((_XtperDisplayList->dpy == (display)) \
     ? &_XtperDisplayList->perDpy.pdi \
     : &_XtSortPerDisplayList(display)->pdi)
#endif /*DEBUG*/

extern void _XtDisplayInitialize(
#if NeedFunctionPrototypes
    Display*		/* dpy */,
    XtPerDisplay	/* pd */,
    String		/* name */,
    String		/* classname */,
    XrmOptionDescRec*	/* urlist */,
    Cardinal 		/* num_urs */,
    Cardinal*		/* argc */,
    char** 		/* argv */
#endif
);

extern void _XtCacheFlushTag(
#if NeedFunctionPrototypes
    XtAppContext /* app */,
    XtPointer	 /* tag */
#endif
);

extern void _XtFreeActions(
#if NeedFunctionPrototypes
    struct _ActionListRec* /* action_table */
#endif
);

extern void _XtDoPhase2Destroy(
#if NeedFunctionPrototypes
    XtAppContext /* app */,
    int		 /* dispatch_level */
#endif
);

#define _XtSafeToDestroy(app) ((app)->dispatch_level == 0)
