/*      @(#)pkg.h 20.10 89/11/21 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/* 
 * 	Of interest to package implementors only.
 */

#ifndef xview_pkg_DEFINED
#define xview_pkg_DEFINED

/*
 ***********************************************************************
 *			Include Files
 ***********************************************************************
 */

/* Don't include notify.h - it is not actually used by very many modules, and
 * generates too many symbols in the libraries.
 */
#include <varargs.h>
#include <xview/attr.h>

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PRIVATE #defines (for package implementors only)
 */

/*
 * Conversion macros for package implementors (public <=> private)	
 */

/*
 * FIXME: put debugging checks here when implemented 
 */
#define XV_PRIVATE(private_type, public_type, obj) \
	((private_type *)((public_type *) (obj))->private_data)
#define XV_PUBLIC(obj) \
	((obj)->public_self)

/* set procs can return XV_SET_DONE if xv_super_set_avlist()
 * has been called.  This will end the set.  Note that
 * other possible set proc return values are XV_OK or an attribute.
 */
#define	XV_SET_DONE	((Xv_opaque) 2)

#define Sv1_public	extern /* SunView1 compatibility only; part of the 
				* client interface 
				*/
#define Xv_public	extern	/* Part of the client interface */
#define Xv_public_data		/* Part of the client interface */
#define Xv_private	extern	/* Should only be used by Sv toolkit */
#define Xv_private_data		/* Should only be used by Sv toolkit */
#define Pkg_private	extern	/* Should only be used by same pkg */


/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */

/*
 * PRIVATE functions for package implementors only
 */

Xv_private Xv_object	 xv_create_avlist( /* parent, pkg, avlist */ );
Xv_private Xv_opaque	 xv_set_avlist( /* object, avlist */ );
Xv_private Xv_opaque	 xv_super_set_avlist( /* object, pkg, avlist */ );
Xv_private int		 xv_destroy_status( /* object, status */ );
Xv_private int 		 xv_check_bad_attr( /* pkg, attr */ );

#endif ~xview_pkg_DEFINED
