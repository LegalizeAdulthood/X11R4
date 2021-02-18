/* @(#)selection.h 20.9 89/08/18 SMI	 */

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

#ifndef xview_selection_DEFINED
#define xview_selection_DEFINED

/*
 * **********************************************************************
 * 
 * Definitions and Macros **********************************************************************
 * 
 */

/*
 * PUBLIC #defines
 */

/*
 * sel_type values
 */
#define	SELTYPE_NULL		0
#define	SELTYPE_CHAR		1

/*
 * sel_itembytes values
 */
#define	SEL_UNKNOWNITEMS	-1	/* Don't know how many items */

/*
 * sel_pubflags values
 */
#define	SEL_PRIMARY		0X01	/* Primary selection */
#define	SEL_SECONDARY		0X02	/* Secondary selection */


/*
 * **********************************************************************
 * 
 * Typedefs, Enumerations, and Structures **********************************************************************
 * 
 */

struct selection {
    int             sel_type, sel_items, sel_itembytes, sel_pubflags;
    caddr_t         sel_privdata;
};

/*
 * **********************************************************************
 * 
 * Globals **********************************************************************
 * 
 */

/*
 * Public variables
 */

extern struct selection selnull;


#ifdef	cplus

/*
 * Public Functions
 */

/*
 * Create the selection
 */
void            selection_set(struct selection * sel, int (*sel_write) (), (*sel_clear) (),
			                      window);
/*
 * Fetch the selection
 */
void            selection_get(int (*sel_read) (), window);
/*
 * Clear the selection
 */
void            selection_clear(window);
/*
 * Write the bits of the selection
 */
void            sel_write(struct selection * sel, FILE * file);
/*
 * Read the bits of the selection
 */
void            sel_read(struct selection * sel, FILE * file);
/*
 * As the owner of the selection you should clear your hiliting because you
 * are no longer the selection owner.
 */
void            sel_clear(struct selection * sel, int window);
#endif	/* cplus */

#endif	/* ~xview_selection_DEFINED */
