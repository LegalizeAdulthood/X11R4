	static char rcsid [] = "$Header: /u/04c/mnt/integ/saw/src/RCS/sxt.c,v 1.6 89/07/20 18:25:18 rnk Exp $";

/*--------------------------------------------------------------------------*\
|
|  Name:
|	sxt.c
|
|  Description:
|	This is the main program for Saw (new version of serpent_x).
|
|  Component of:
|	Saw
|
|  Related files:
|	lots, most notably sxt.h
|
|  Project:
|	SERPENT System
|	User Interface Prototyping
|	Software Engineering Institute
|	Carnegie Mellon University
|	Pittsburgh, PA  15213
|
|  Version:
|	1.0
|
|  Owner:  
|	Len Bass
|	ljb@sei.cmu.edu
|
|  Authors:
|	Kurt W. Hoyt
|	kwh@sei.cmu.edu
|
|	Erik J. Hardy
|	erik@sei.cmu.edu
|
|	Rick Kazman
|	rnk@sei.cmu.edu
|
|  Change History:
$Log:	sxt.c,v $
 * Revision 1.6  89/07/20  18:25:18  rnk
 * removed icon bitmap stuff from initialize_toolkit (it didn't work anyway)
 * along with creation of dummy widget (another no-op)
 * 
 * Revision 1.4  89/06/23  09:41:48  rnk
 * added defines and code to create a graphics context and cursor for
 * boundary box movement
 * 
 * Revision 1.3  89/06/16  13:13:43  rnk
 * added the routines sleep_10ms(count) and sleepx(), which are slight
 * modifications of the system sleep routines to allow sleeping in increments
 * of 10 microseconds.  Changed the saw main loop to sleep for 1/10 sec.
 * whenever there are no X events or interface messages pending, effectively
 * removing the busywait from saw.
 * 
 * Revision 1.2  89/06/13  12:06:36  rnk
 * added code to initialize x22_values_list
 * 
 * Revision 1.1  89/04/26  10:44:49  rnk
 * Initial revision
 * 
 * Revision 1.8  89/02/17  03:38:55  ejh
 * fixed x includes; new version
 * 
 * Revision 1.7  89/02/13  14:56:42  ejh
 * changed interrupt handler to print out nice message; new version #
 * 
 * Revision 1.6  89/02/10  12:52:45  ejh
 * fixed interrupt handling to be more friendly; added copyright notice
 * 
 * Revision 1.5  89/02/02  12:29:04  ejh
 * new version
 * 
 * Revision 1.4  89/02/01  11:55:40  ejh
 * new version number
 * 
 * Revision 1.3  89/01/26  16:44:58  ejh
 * changed the way we get the parent of each widget to conform to the way the
 * sun does it; put x22_VERSION here
 * 
 * Revision 1.2  88/11/14  17:22:46  ejh
 * renamed icon.bits to icon_bits.h
 * 
 * Revision 1.1  88/11/14  15:46:45  ejh
 * Initial revision
 * 
|	 3 Nov 88	project standards imposed	erik
|	15 Aug 88	moved creation of shell to here as part of init	erik
|	11 Mar 88	created		kwh
|
\*--------------------------------------------------------------------------*\
|
|   Distribution: Approved for public release; distribution is unlimited.
|
|   Copyright (c) 1989  by  Carnegie  Mellon  University, Pittsburgh, PA.  The
|  Software Engineering  Institute  (SEI) is a federally  funded  research and
|  development center established  and  operated by Carnegie Mellon University
|  (CMU).  Sponsored  by  the  U.S.  Department  of   Defense  under  contract
|  F19628-85-C-0003,  the  SEI  is  supported  by  the  services  and  defense
|  agencies, with the U.S. Air Force as the executive contracting agent.
|
|    Permission to use,  copy,  modify, or  distribute  this  software and its
|  documentation for  any purpose and without fee  is hereby granted, provided
|  that  the above copyright notice appear  in  all copies and that both  that
|  copyright  notice  and   this  permission  notice  appear   in   supporting
|  documentation.   Further,  the  names  Software  Engineering  Institute  or
|  Carnegie  Mellon  University  may  not be used in  advertising or publicity
|  pertaining to distribution of the software without specific, written  prior
|  permission.  CMU makes no claims  or representations  about the suitability
|  of this software for any purpose.  This software is provided "as is" and no
|  warranty,  express  or  implied,  is  made  by  the  SEI  or CMU, as to the
|  accuracy  and  functioning of the program and related program material, nor
|  shall   the  fact of  distribution   constitute  any   such  warranty.   No
|  responsibility is assumed by the SEI or CMU in connection herewith.
|
\*--------------------------------------------------------------------------*/

#define	memoryPack

#include 	<sys/time.h>
#include	<signal.h>

/*#include	<X11/Xlib.h>     /* X, the X Toolkit and the Athena widgets */
#include	<X11/Intrinsic.h>
#include	<X11/IntrinsicP.h> 
#include	<X11/StringDefs.h>
#include	<X11/Core.h>
#include	<X11/CoreP.h>
#include	<X11/Composite.h>
#include	<X11/CompositeP.h>
#include	<X11/Shell.h>
#include	<X11/Label.h>
#include	<X11/Box.h>
#include	<X11/Command.h>
#include	<X11/Form.h>
#include	<X11/Cardinals.h>
#include 	<X11/cursorfont.h>

#include        "serpent.h"     /* Serpent externals */
#include	"sxt.h"
#include	"x22.h"
#include	"iid.h"
#include        "saw.h"   /* X shared data definition */

#include        "hashPack.h"    /* Hash table includes */

#include        "xht.h" /* hash tables interface */
#include        "xct.h" /* color table interface */
#include        "xcp.h" /* core part */
#include        "xfw.h" /* form widget */
#include        "xlw.h" /* label widget */
#include        "xcm.h" /* command widget */
#include        "xbw.h" /* box widget */
#include        "xsb.h" /* scrollbar widget */
#include        "xvp.h" /* viewport widget */
#include        "xvt.h" /* vertical paned widget */
#include        "xdw.h" /* dialog widget */
#include        "xtw.h" /* text widget */
#include	"xdb.h" /* debug interface */

/* Definitions for Graphics Context */
#define DRAW_WIDTH              0       /* use fastest hardware draw */
#define DRAW_VALUE              0xfd
#define DRAW_FUNC               GXxor
XGCValues xgc ;                         /* to create font GCs */
GC      DrawGC ;                        /* graphics context for drawing */
Cursor  ArrowCrossCursor ;

/* Definitions for sleep routine */
#define	mask(s)	(1<<((s)-1))
#define	setvec(vec, a) \
	vec.sv_handler = a; vec.sv_mask = vec.sv_onstack = 0

#define DEBUG1(statement) SERPENT_DEBUG1(xht_debug, statement)
#define DEBUG2(statement) SERPENT_DEBUG2(xht_debug, statement)
#define DEBUG3(statement) SERPENT_DEBUG3(xht_debug, statement)

int screen ;
Display	*display;

/*	process table types and variables	*/
typedef	void	(*sxt_process_proc)();

typedef struct {
	char			*record_name;
	iid_id_type		id;
	sxt_process_proc	process;
	}	sxt_process_table_record;

typedef struct {
	iid_id_type id;
	iid_id_type parent;
	char *type;
	isd_change_type	change;
} 	sxt_object_node;

LIST	sxt_object_list,
	sxt_temp_object_list;

static	char	*record_names[] =
	{"form_widget", "label_widget",     "command_widget",
	"text_widget",  "scrollbar_widget", "viewport_widget", "box_widget",
	"vpane_widget", "dialog_widget",    "text_object", "pixmap_object",
	NULL};

static	sxt_process_proc	process_procs[] =
	{xfw_process_changes, xlw_process_changes,
	xcm_process_changes, xtw_process_changes, xsb_process_changes,
	xvp_process_changes, xbw_process_changes, xvt_process_changes,
	xdw_process_changes, NULL};

#define NUM_RECORDS	9
#define HASH_SIZE	257
#define	x22_VERSION	1.12

HASH	process_table;


/**************************/
/**************************/

/* Modified version of the system sleep function to use smaller time	*/
/* slices (system granularity is 1 sec, this routine is 1/100 sec)	*/

static int ringring;

sleep_10ms(n)
	unsigned n;
{
	int sleepx(), omask;
	struct itimerval itv, oitv;
	register struct itimerval *itp = &itv;
	struct sigvec vec, ovec;

	if (n == 0) return;
	timerclear(&itp->it_interval);
	timerclear(&itp->it_value);
	if (setitimer(ITIMER_REAL, itp, &oitv) < 0) return;
	setvec(ovec, SIG_DFL);
	omask = sigblock(0);
	    /* Multiply millisecond time by 10,000 to get 10 ms chunks */
	itp->it_value.tv_usec = n * 10000 ;
	if (timerisset(&oitv.it_value))	
	{
		if (timercmp(&oitv.it_value, &itp->it_value, >))
			oitv.it_value.tv_sec -= itp->it_value.tv_sec;
		else 
		{
			itp->it_value = oitv.it_value;
			/*
			 * This is a hack, but we must have time to
			 * return from the setitimer after the alarm
			 * or else it'll be restarted.  And, anyway,
			 * sleep never did anything more than this before.
			 */
			oitv.it_value.tv_sec = 1;
			oitv.it_value.tv_usec = 0;
		}
	}
	setvec(vec, sleepx);
	(void) sigvec(SIGALRM, &vec, &ovec);
	ringring = 0;
	(void) setitimer(ITIMER_REAL, itp, (struct itimerval *)0);
	while (!ringring) sigpause(omask &~ mask(SIGALRM));
	(void) sigvec(SIGALRM, &ovec, (struct sigvec *)0);
	(void) setitimer(ITIMER_REAL, &oitv, (struct itimerval *)0);
}

static sleepx()
{
	ringring = 1;
}


int	sxt_match_name (id, record)
char	*id;
sxt_process_table_record	*record;
{
    if (xht_debug >= 2)
    {
	fprintf (stderr, "sxt_match_name: Entering.\n");
	fprintf (stderr, "sxt_match_name: id = %s\n", id);
	fprintf (stderr, "sxt_match_name: record = %s\n",
	    record->record_name);
	fprintf (stderr, "sxt_match_name: Leaving.\n");
    }

    return (strcmp (id, record->record_name) == 0);
}


void	sxt_make_process_table ()
{
    sxt_process_table_record	*temp;
    int				i;

    if (xht_debug >= 2)
	fprintf (stderr, "sxt_make_process_table: Entering.\n");

    process_table = make_hashtable (HASH_SIZE, xht_hash_string,
	sxt_match_name);

    for (i = 0; i < NUM_RECORDS; i++)
    {
	temp = (sxt_process_table_record *) make_node (sizeof
	    (sxt_process_table_record));
	temp->record_name = record_names [i];
	temp->process = process_procs [i];

	add_to_hashtable (process_table, temp, temp->record_name);

	if (xht_debug >= 2)
	{
	    fprintf (stderr, "sxt_make_process_table: name = %s\n",
		record_names [i]);
	    fprintf (stderr, "sxt_make_process_table: process = %x\n",
		process_procs [i]);
	}
    }
}

static XtActionsRec actions[] = 
{    
     {"x22_child_move", 	x22_child_move},
     {"x22_child_resize", 	x22_child_resize},
     {"x22_select_point", 	x22_select_point},
} ;


void	sxt_initialize_toolkit (argc, argv)
int	argc;
char	**argv;

{
    Widget	toplevel,
		dummy_widget; 
    Arg		args [5];
    int		nargs;

    toplevel = XtInitialize(NULL, "saw", NULL, 0, &argc, argv);

    display = XtDisplay(toplevel);
    screen  = DefaultScreen(display);

    nargs = 0 ;
    XtSetArg (args [nargs], XtNallowShellResize, TRUE); nargs++ ;
    XtSetValues (toplevel, args, nargs);
 
    XtAddActions(actions, XtNumber(actions)) ;

    x22_register_new_shell (toplevel);
}


void	sxt_list_objects	(dummy, node)
int	*dummy;
sxt_object_node	*node;
{
    xpw_DEBUG2 (fprintf (stderr,
	"sxt_list_objects: type = %s, id = %d, parent = %d\n", 
	(char *)(node->type), node->id, node->parent));
}


void	sxt_execution	(transaction, node)
isd_trans	transaction;
sxt_object_node	*node;
{
    sxt_process_table_record	*hnode;
    sxt_process_proc		process_element_changes;

    hnode = (sxt_process_table_record *) get_from_hashtable (process_table,
	node->type);
    process_element_changes = hnode->process;

    if (process_element_changes != NULL)
	(*process_element_changes) (node->change, transaction, node->id);

    return;
}


typedef struct  {
        Widget  widget;
        Widget  parent;
}       x22_parent_node;


void	sxt_realize (dummy, cur_widget)
int	*dummy;
x22_parent_node	*cur_widget;
{
    /* Look at widgets with dummy parents and realize	*/
    /* any of them which are not already realized.	*/

    if (!XtIsRealized (cur_widget->parent)) 
        XtRealizeWidget (cur_widget->parent);

    return;
}


boolean	serpent_inited = FALSE;

void	sxt_interrupt_handler ()
{
    if (serpent_inited)
	serpent_cleanup ();
    fprintf (stderr, "\nSaw exiting\n");
    exit (0);
}


/**********/
/*  Main  */
/**********/

main	(argc, argv)
int	argc;
char	**argv;

{
    /* Interface Variables */
    isc_status_codes	status;
    isd_trans		transaction;
    iid_id_type		id, *sparent;
    isd_process		me;
    int			i, k;
    string		element_name;
    isd_change_type	change;

    /* X Toolkit Variables */
    XEvent	my_event;

    /* Miscellaneous variables */
    sxt_process_proc		process_element_changes;
    sxt_process_table_record	*node;
    sxt_object_node	*snode;
    NODE	object_head, object_node, object_one, addr ;

    u22_process_name = "saw" ;
    fprintf(stderr, "Welcome to Saw, version %4.2f\n", x22_VERSION);

    signal(SIGINT, sxt_interrupt_handler);
    signal(SIGTERM, sxt_interrupt_handler);

    isd_debug = 0;
    idd_debug = 0;
    iid_debug = 0;
    ipc_debug = 0;
    ids_debug = 0;
    ism_debug = 0;
    isc_debug = 0;
    ubs_debug = 0;

    xdb_parse_debug(argc, argv);

    serpent_init(MAIL_BOX, ILL_FILE);

    if (status != ok)
    {
	fprintf (stderr, "saw.main: serpent_init failed\n");
	exit (1);
    }
    check_status ("sxt: serpent_init failed\n");

    xht_make_hash_tables ();
    sxt_make_process_table ();

    /* initialize the X toolkit */
    sxt_initialize_toolkit(argc, argv); 
    xct_make_color_table(display) ;

    /* Create the cursor and the graphics context for move/resize */
    ArrowCrossCursor = XCreateFontCursor(display, XC_fleur);

    xgc.line_width = DRAW_WIDTH ;
    xgc.foreground = DRAW_VALUE;
    xgc.function = DRAW_FUNC;
    xgc.subwindow_mode = IncludeInferiors;
    DrawGC = XCreateGC(display, RootWindow(display, screen),
        (GCLineWidth | GCForeground | GCFunction | GCSubwindowMode), &xgc);

    /* Initialize lists for keeping track of saw objects */
    sxt_object_list = make_list() ;
    x22_parent_list = make_list() ;
    x22_values_list = make_list() ;

    xpw_start_print() ;
    x22_transaction_started = FALSE ;
    fprintf (stderr, "Saw: ready\n") ;

    while (TRUE)
    {
	status = not_available;
	while (status == not_available)
	{
	    while (XtPending())
	    {
		XtAppNextEvent (_XtDefaultAppContext(), &my_event);
		if (xht_debug >= 3)
		    fprintf (stderr, "saw.main: Event is %s.\n",
			event_names [my_event.type]);
		XtDispatchEvent (&my_event);
	    } 

	    transaction = isd_get_transaction_no_wait();
	    status = get_status ();
	    if (status == not_available && !XtPending())
		sleep_10ms(10) ;
	}

	if (status != ok)
	{
	    fprintf (stderr, "saw.main: isd_get_transaction of %d.\n",
		status);
	    serpent_cleanup ();
	    fprintf (stderr, "saw.main: Exiting.\n");
	    fflush (stderr);
	    exit (1);
	}

	if (xht_debug >= 3)
	    fprintf (stderr, "saw.main: Got a transaction.\n");

	/* Get the id for the first element in the transaction and 	*/
	/* process the changed element. 				*/

	id = isd_get_first_changed_element (transaction);

	/* Get the id of the remaining changed elements. */

	if (id != iid_null_id)
	{
	    while (id != iid_null_id)
	    {
		element_name = isd_get_element_name (transaction, id);
		if (strcmp (element_name, "point") != 0 &&
		    strcmp (element_name, "debug_object") != 0)
		{
		    addr = make_node (isd_get_length (element_name));
		    addr = (NODE) isd_get_shared_data (transaction, id, NULL);
		    snode = (sxt_object_node *) make_node (sizeof
			(sxt_object_node));
		    snode->id = id;
		    i = 0;
		    while ((strcmp (record_names [i], element_name))!= 0)
			++i;
		    if (i < 10)
			sparent = (iid_id_type *) isd_get_shared_data
			    (transaction, id, "parent");
		    else
			sparent = (iid_id_type *) isd_get_shared_data
			    (transaction, id, "widget");
		    if (sparent == NULL)
			snode -> parent = NULL;
		    else
			snode -> parent = *sparent;
		    snode->type = (char *) make_node (strlen (element_name) +
			1);
		    strcpy (snode->type, element_name);
		    snode->change = isd_get_change_type (transaction, id);
		    add_to_head (sxt_object_list, snode);
		}

		id = isd_get_next_changed_element (transaction);
	    }

	    /* print the list in reverse order that they were found */

	    xpw_print (before sorting, NULL);
	    for_all_list (sxt_object_list, NULL, sxt_list_objects);

	    /* this is the sorting algorithm: first, put all removes at */
	    /* the head of the list 					*/

	    sxt_temp_object_list = make_list ();
	    object_head = get_list_head (sxt_object_list);
	    while (object_head != NULL)
	    {
		if (((sxt_object_node *) object_head) -> change == isd_remove)
		{
		    object_node = get_list_next (sxt_object_list,object_head);
		    (void) add_to_tail (sxt_temp_object_list,
			delete_list_node (sxt_object_list, object_head));
		    object_head = object_node;
		}
		else
		    object_head = get_list_next (sxt_object_list,object_head);
	    }

	    /* then, put all form widgets after the removes */

	    object_head = get_list_head (sxt_object_list);
	    while (object_head != NULL)
	    {
		if (strcmp (((sxt_object_node *) object_head) -> type,
		    "form_widget") == 0)
		{
		    object_node = get_list_next (sxt_object_list,object_head);
		    (void) add_to_tail (sxt_temp_object_list,
			delete_list_node (sxt_object_list, object_head));
		    object_head = object_node;
		}
		else
		    object_head = get_list_next (sxt_object_list,object_head);
	    }


	    /* append whatever's left and rename */

	    (void) append_list (sxt_temp_object_list, sxt_object_list);
	    sxt_object_list = sxt_temp_object_list;

	    /* then, if any "upper" node in the list has a parent which	*/
	    /* is the same as the id in a "lower" node, move the "upper"*/
	    /* node to the tail, but only if it's not a remove 		*/

	    object_head = get_list_head (sxt_object_list);
	    object_node = get_list_next (sxt_object_list, object_head);

	    while (object_head != get_list_tail (sxt_object_list))
	    {
		object_node = get_list_next (sxt_object_list, object_head);
		while (object_node != NULL)
		{
		    if (((sxt_object_node *) object_head)->parent ==
			((sxt_object_node *) object_node)->id &&
			((sxt_object_node *) object_head)->change !=
			  isd_remove)
		    {
			object_one = object_head;
			(void) add_to_tail (sxt_object_list, object_head);
			object_head = get_list_next (sxt_object_list,
			    object_head);
			(void) delete_list_node (sxt_object_list, object_one);
			object_node = get_list_next (sxt_object_list,
			    object_head);
		    }
		    else
			object_node = get_list_next (sxt_object_list,
			    object_node);
		}
		object_head = get_list_next (sxt_object_list, object_head);
	    }

	    xpw_print (after sorting, NULL);
	    for_all_list (sxt_object_list, NULL, sxt_list_objects);
	    for_all_list (sxt_object_list, transaction, sxt_execution);

	    /* This will realize any unrealized widgets */
	    for_all_list (x22_parent_list, NULL, sxt_realize); 

	    flush_list (sxt_object_list, AND_NODES);
	    xpw_print (done transaction, NULL);

	}	/* end if (id != iid_null_id) */

	/* Purge transaction. */

	if (xht_debug >= 3) 
	    fprintf(stderr, "saw.main: Purging transaction.\n");
	isd_purge_transaction(transaction) ;

	if (x22_transaction_started)
	{
	    isd_commit_transaction(x22_trans);
	    x22_transaction_started = FALSE;
	    if (xht_debug > 1)
		fprintf (stderr, "saw.main: sent transaction\n");
	}

    }	/* end while */

    /* this point is never reached, but if it were, */
    /* we should do a serpent_cleanup() and exit(0) */

}
