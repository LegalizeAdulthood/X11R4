/*	@(#)ttysw.h 20.9 89/08/18 SMI	*/

/*
 *	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents 
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE 
 *	file for terms of the license.
 */

/*
 * A tty subwindow is a subwindow type that is used to provide a
 * terminal emulation for teletype based programs.
 *
 * The caller of ttysw_start typically waits for the child process to die
 * before exiting.
 *
 */

#ifndef xview_ttysw_DEFINED
#define xview_ttysw_DEFINED

/*
 ***********************************************************************
 *			Definitions and Macros
 ***********************************************************************
 */

/*
 * PRIVATE #defines 
 */

/* 
 * options - controlled by ttysw_getopt(), ttysw_setopt
 * The values of the #define's are wired into aliases, shell-scripts,
 * etc. and should not be changed!
 */
#define	TTYOPT_PAGEMODE			1
#define TTYOPT_SELSVC			3
#define TTYOPT_TEXT			4	/* TERMSW */

/*
 * styles for rendering boldface characters 
 */
#define TTYSW_BOLD_NONE			0x0
#define TTYSW_BOLD_OFFSET_X		0x1
#define TTYSW_BOLD_OFFSET_Y		0x2
#define TTYSW_BOLD_OFFSET_XY		0x4
#define TTYSW_BOLD_INVERT		0x8
#define TTYSW_BOLD_MAX			0x8

/*
 * Modes for invert and underline 
 */
#define TTYSW_ENABLE			0x0
#define TTYSW_DISABLE			0x1
#define TTYSW_SAME_AS_BOLD		0x2

/*
 ***********************************************************************
 *		Typedefs, Enumerations, and Structures
 ***********************************************************************
 */

typedef caddr_t	Ttysubwindow;

/*
 ***********************************************************************
 *				Globals
 ***********************************************************************
 */


#ifdef cplus
/*
 * C Library routines specifically related to ttysw subwindow functions.
 */

/*
 * PRIVATE functions 
 */

void	ttysw_done(caddr_t ttysw);
void	ttysw_setopt(caddr_t ttysw, int opt, int on);
int	ttysw_getopt(caddr_t ttysw, int opt);

/*
 * PUBLIC functions
 * for compatibility with pre-SunView 1 code
 */
void	ttysw_becomeconsole(caddr_t ttysw);
#endif cplus

#endif ~xview_ttysw_DEFINED
