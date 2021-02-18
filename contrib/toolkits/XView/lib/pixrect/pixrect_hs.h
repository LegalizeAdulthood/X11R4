/*	@(#)pixrect_hs.h 1.18 88/02/08 SMI	*/

/*
 * 	(c) Copyright 1989 Sun Microsystems, Inc. Sun design patents
 *	pending in the U.S. and foreign countries. See LEGAL NOTICE
 *	file for terms of the license.
 */

/*
 * Include this header file to get all pixrect related header files.
 * THIS VERSION IS FOR USE WITH XView ONLY! 
 */

#ifndef	pixrect_hs_DEFINED
#define	pixrect_hs_DEFINED

#ifndef sun

/*
 * Include files for non-Suns
 */

#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <pixrect/pr_line.h>
#include <pixrect/pr_planegroups.h>
#include <pixrect/pr_util.h>
#include <pixrect/traprop.h>

#include <pixrect/memvar.h>

#include <pixrect/pixfont.h>

#include <pixrect/rasterfile.h>

#else

/*
 * Include files for Suns
 */
#include <sys/types.h>
#include <pixrect/pixrect.h>
#include <pixrect/pr_dblbuf.h>
#include <pixrect/pr_line.h>
#include <pixrect/pr_planegroups.h>
#include <pixrect/pr_util.h>
#include <pixrect/traprop.h>

#ifdef mc68000
#include <pixrect/bw1var.h>
#include <pixrect/cg1var.h>
#endif mc68000

#include <pixrect/bw2var.h>
#include <pixrect/cg2var.h>
#include <pixrect/gp1var.h>
#include <pixrect/cg4var.h>
#include <pixrect/memvar.h>

#include <pixrect/pixfont.h>

#include <rasterfile.h>
#include <pixrect/pr_io.h>

#endif  sun
#endif	pixrect_hs_DEFINED
