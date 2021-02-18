/*      @(#)filter.h 20.8 89/08/17 SMI      */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_filter_DEFINED
#define xview_filter_DEFINED


/*
 ***********************************************************************
 *		Typedefs, enumerations, and structs
 ***********************************************************************
 */

struct filter_rec {
	char           *key_name;
	int             key_num;
	char           *class;
	char          **call;
};

/*
 ***********************************************************************
 *			Globals
 ***********************************************************************
 */

/*
 * Private Functions 
 */

struct filter_rec **xv_parse_filter_table();
/* xv_parse_filter_table(in)
 *	STREAM         *in;
 */

void xv_free_filter_table();
/* xv_free_filter_table(table)
 *	struct filter_rec **table;
 */

#endif ~xview_filter_DEFINED
