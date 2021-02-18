/*
 * @(#)win_notify.h 20.7 89/08/18 SMI
 *
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * SunView related notification definitions (see also notify.h).
 */

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PUBLIC Functions 
 */

/*
 * Posting of client events to window notifier clients 
 */

extern	Notify_error 	win_post_id(/* client, id, when */);		
/* Notify_client client;
 * short id;
 * Notify_event_type when; 
 */

extern	Notify_error	win_post_id_and_arg(/* id, when, arg, copy_func, 
					    release_func */);	
/* Notify_client client;
 * short id;
 * Notify_event_type when;
 * Notify_arg arg;
 * Notify_copy copy_func;
 * Notify_release release_func;
 */

extern	Notify_error	win_post_event(/* client, event, when */);	
/* Notify_client client;
 * Event *event;
 * Notify_event_type when;
 */

extern	Notify_error	win_post_event_arg(/* event, when, arg, copy_func,
					   release_func */);
/* Notify_client client;
 * Event *event;
 * Notify_event_type when;
 * Notify_arg arg;
 * Notify_copy copy_func;
 * Notify_release release_func;
 */

/*
 * Utilities to call if posting with win_post_id_and_arg or win_post_event_arg
 */

extern	Notify_arg	win_copy_event(/* client, arg, event_ptr */);
/* Notify_client client;
 * Notify_arg arg;
 * Event **event_ptr;
 */

extern	void	win_free_event(/* client, arg, event */);			
/* Notify_client client;
 * Notify_arg arg;
 * Event *event;
 */
