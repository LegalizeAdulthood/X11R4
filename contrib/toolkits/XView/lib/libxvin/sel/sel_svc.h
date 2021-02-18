/*      @(#)sel_svc.h 20.17 89/11/09		*/

#ifndef	xview_selection_svc_DEFINED
#define	xview_selection_svc_DEFINED

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

#include <xview/pkg.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <X11/Xatom.h>
#include <rpc/rpc.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PUBLIC #defines 
 */

#ifdef TRUE
#undef	TRUE
#define	TRUE	1
#endif

#ifdef FALSE
#undef	FALSE
#define	FALSE	0
#endif

#define SELN_REPORT(event)	seln_report_event(0, event)

/*
 * routines & data for keeping track of the state of the function keys
 */

#define selection_function_pending(server)	\
				     server_get_seln_function_pending(server)
#define seln_function_clear(server)		\
				     server_set_seln_function_pending(server, 0)

/*
 * possibly useful predicates	
 */
#define		seln_holder_is_me(client, holder)	\
			(seln_holder_same_client(client, holder))

/*
 * PRIVATE #defines 
 */

#define SELN_FUNCTION_WORD_COUNT 8      /*  256 bits should last a while  */

#define SELN_RPC_BUFSIZE	2048
#define SELN_BUFSIZE  (SELN_RPC_BUFSIZE				\
			    -	128				\
			    - sizeof(Seln_replier_data *)	\
			    - sizeof(Seln_requester)		\
			    - sizeof(char *)			\
			    - sizeof(Seln_rank)			\
			    - sizeof(Seln_result)		\
			    - sizeof(unsigned))

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

/*
 * Seln_client:	opaque handle returned to client from create    
 */
typedef char *Seln_client;

/*
 * PUBLIC enums 
 */

/*
 * Seln_result:	Standard return codes	
 */
typedef enum	{
    SELN_FAILED, SELN_SUCCESS,		/*     the basic all-around uses  */
    SELN_NON_EXIST, SELN_DIDNT_HAVE, SELN_WRONG_RANK,	/* special cases  */
    SELN_CONTINUED, SELN_CANCEL, SELN_UNRECOGNIZED,
    SELN_OVER 
}	Seln_result;

/*
 * Seln_rank:	Selection identifiers	
 */
typedef enum	{
	SELN_UNKNOWN	= 0, 
	SELN_CARET	= 1, 
	SELN_PRIMARY	= 2,
	SELN_SECONDARY	= 3, 
	SELN_SHELF 	= 4, 
	SELN_UNSPECIFIED = 5
}	Seln_rank;

/*
 *	Seln_function:	Modes which affect rank of selection,
 *	controlled by function-keys or their programmatic equivalents
 */
typedef enum	{
	    SELN_FN_ERROR = 0,
    SELN_FN_STOP = 1,  SELN_FN_AGAIN = 2,
    SELN_FN_PROPS = 3, SELN_FN_UNDO  = 4,
    SELN_FN_FRONT = 5, SELN_FN_PUT   = 6,
    SELN_FN_OPEN  = 7,  SELN_FN_GET  = 8,
    SELN_FN_FIND  = 9,  SELN_FN_DELETE = 10
}	Seln_function;

#define SELN_FN_COPY	SELN_FN_PUT
#define SELN_FN_PASTE	SELN_FN_GET
#define SELN_FN_CUT		SELN_FN_DELETE

/*
 *	Seln_state:	States a selection (or its holder) may be in
 */
typedef enum	{
    SELN_NONE, SELN_EXISTS, SELN_FILE
}	Seln_state;

/*
 *	Seln_response:	possible appropriate responses to a Seln_function_buffer
 */
typedef enum	{
    SELN_IGNORE, SELN_REQUEST, SELN_FIND, SELN_SHELVE, SELN_DELETE
}	Seln_response;

/*
 * PUBLIC structs 
 */

/*
 * Seln_access for SunView 1 compatibility
 * (cannot be moved below because Seln_holder depends upon it)
 */
typedef struct {
    int                 pid;
    int                 program;
    struct sockaddr_in  tcp_address;
    struct sockaddr_in  udp_address;
    char               *client;
}	Seln_access;

typedef struct	{
    Seln_rank           rank;
    Seln_state          state;
    Seln_access         access;
}	Seln_holder;

typedef struct	{
    Seln_holder         caret;
    Seln_holder         primary;
    Seln_holder         secondary;
    Seln_holder         shelf;
}	Seln_holders_all;

typedef struct	{
    Seln_function       function;
    Seln_rank           addressee_rank;
    Seln_holder         caret;
    Seln_holder         primary;
    Seln_holder         secondary;
    Seln_holder         shelf;
}	Seln_function_buffer;

typedef struct	{
    char		*client_data;
    Seln_rank            rank;
    char		*context;
    char	       **request_pointer;
    char	       **response_pointer;

}	Seln_replier_data;

typedef struct	{
    Seln_result	       (*consume)();
    char		*context;
}	Seln_requester;

typedef struct {
    Seln_replier_data	*replier;
    Seln_requester	 requester;
    char		*addressee;
    Seln_rank            rank;
    Seln_result	         status;
    unsigned             buf_size;
    char                 data[SELN_BUFSIZE];
}	Seln_request;

/*
 * PUBLIC structs 
 * For SunView 1 compatibility 
 */

typedef struct {
    Seln_rank           rank;
    char               *pathname;
}	Seln_file_info;

typedef struct	{
    Seln_holder         holder;
    Seln_function       function;
    int                 down;
}	Seln_inform_args;

typedef struct {
	unsigned	data[SELN_FUNCTION_WORD_COUNT];
}	Seln_functions_state;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PUBLIC variables 
 */

/*
 * null structs for initialization & easy reference	
 */
extern Seln_function_buffer
			seln_null_function;
extern Seln_holder	seln_null_holder;
extern Seln_request     seln_null_request;

/*
 * PUBLIC functions 
 */

Xv_public    Seln_client         seln_create(), selection_create();
Xv_public    Seln_function_buffer
    				seln_inform(), selection_inform();
Xv_public    Seln_holder     	seln_inquire(), selection_inquire();
Xv_public    Seln_holders_all 	seln_inquire_all(), selection_inquire_all();
Xv_public    Seln_rank       	seln_acquire(), selection_acquire();
Xv_public    Seln_request      *seln_ask(), *selection_ask();
Xv_public    Seln_response	seln_figure_response(), selection_figure_response();
Xv_public    Seln_result     	seln_done(), selection_done(),
	    			seln_hold_file(), selection_hold_file(),
	    			seln_query(), selection_query(),
	    			seln_request(), selection_request();
Xv_public    void            	seln_report_event(), selection_report_event(),
	    			seln_yield_all(), selection_yield_all();
Xv_public    void            	seln_destroy(), selection_destroy();
Xv_public    int		seln_holder_same_client(),
	    	 		seln_holder_same_process(),
	    			seln_secondary_made(),
	    			seln_secondary_exists();
Xv_public    void		seln_init_request(), selection_init_request(),
	    			seln_clear_functions(),
    				selection_clear_functions(), seln_use_timeout(),
				selection_use_timeout();

#endif ~xview_selection_svc_DEFINED
