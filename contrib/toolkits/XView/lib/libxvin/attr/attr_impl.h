/*      @(#)attr_impl.h 20.7 89/08/17 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef _attr_impl_h_already_included
#define	_attr_impl_h_already_included

#include <varargs.h>
#include <xview/attr.h>

/* NON_PORTABLE means that the var-args list is treated
 * as an avlist.  This is known to work for Sun1/2/3/rise.
 * If the implementation of varargs.h does not have va_arg()
 * equivalent to an array access (e.g. *avlist++), then
 * NON_PORTABLE should NOT be defined.
 */
#define	NON_PORTABLE

/* size of an attribute */
#define	ATTR_SIZE	(sizeof(Xv_opaque))

/* package private routines */
extern Attr_avlist	attr_copy_avlist();
extern void		attr_check_pkg();
extern int		attr_count_avlist();
extern Attr_avlist	attr_copy_valist();

#endif _attr_impl_h_already_included
