/*
 *		Copyright IBM Corporation 1989
 *
 *                      All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of IBM not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.
 *
 * IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 *
 *
 * University of Illinois at Urbana-Champaign
 * Department of Computer Science
 * 1304 W. Springfield Ave.
 * Urbana, IL	61801
 *
 * (C) Copyright 1987, 1988 by The University of Illinois Board of Trustees.
 * All rights reserved.
 *
 * Tool: X 11 Graphical Kernel System
 * Author: Gregory Scott Rogers
 * Author: Sung Hsien Ching Kelvin
 * Author: Yu Pan
 * 
 * Define file for : Implementation dependent constants
 *
 * $Header: gks_defines.h,v 4.0 89/08/31 16:34:11 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/gks_defines.h,v $
 *
 * $Log:	gks_defines.h,v $
 * Revision 4.0  89/08/31  16:34:11  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.7  89/06/05  10:28:33  bruce
 * PTR# c1176:	Added string macros to check for NULL pointers before making
 * 		calls to the string functions.
 * 
 * Revision 3.6  89/02/04  15:32:31  amy
 * PTR c1147	Make global vars. and functions private, or static where possible.
 * 
 * Revision 3.5  88/12/28  14:52:56  amy
 * PTR c1143	Added #define'd constants for the number of different predefined
 * fillarea, polyline, polymarker, and text representations (5, 5, 5, and 6).
 * 
 * Revision 3.4  88/12/06  16:25:27  amy
 * PTR c1130	GKSERROR:  cast function name parameter to gerrorhand call to
 * 		a Gint.
 * 
 * Revision 3.3  88/11/18  14:13:20  amy
 * PTR c1068	Added defined constants METAFILE_OK, MF_ITEM_ERR, MF_DATA_ERR,
 * 	and MF_FILE_ERR for error-handling in metafile.c
 * 
 * Revision 3.2  88/08/19  13:12:59  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:47:12  david
 * Initial revision
 *  
 *
 */

#ifndef  LOCAL_H
#define  LOCAL_H

#include "xgks.h"			/* Standard GKS C-language binding definitions */

/* strlen was defined as STRLEN so a NULL ptr is allowed AIX port */
#define STRLEN(a)       ((a == NULL) ? 0 : strlen(a))   /* c1176 */

/* strcmp was defined as STRCMP so a NULL ptr is allowed AIX port */
/* Note: STRCMP given a NULL pointer will return 1                */
/*                                                                */
#define STRCMP(a,b) (((a) != NULL && (b) != NULL) ? strcmp((a),(b)) : 1)
							/* c1176 */

/* strcpy was defined as STRCPY so a NULL ptr is allowed AIX port */
#define STRCPY(d,s) {if ((s) != NULL) strcpy((d),(s));}

#define GKSERROR( c, n, m ) \
	if (c) { gerrorhand( n, (Gint)m, xgks_state.gks_err_file); return( n ); } /*c1147*/

#define MAX_SEG		32767 		/* Largest segment name for 16-bit integers */
#define MAX_OPEN_WS	10		/* Hardwired limits */
#define MAX_ACTIVE_WS	10
#define	MAX_ASSOC_WS	10

#define MAX_BUNDL_TBL	21  		/* Legal bundle table range 1-20 
					   entry 0 will be initialized however */

/* c1143:  Added following 4 defines for use w/ predefined representations */
#define PDF_FILL_BNDLS	5		/* # of predef fill rep bundles */
#define PDF_LINE_BNDLS	5		/* # of predef line rep bundles */
#define PDF_MARK_BNDLS	5		/* # of predef marker rep bundles */
#define PDF_TEXT_BNDLS	6		/* # of predef text rep bundles */

#define DEFINED_FONT_TYPE 9		/* number of defined font type XGKS system */

#ifndef TRUE
#define TRUE		1
#define FALSE		0
#endif

#define INVALID		-1

#define OK		0

/* Metafile error flag values */				/* c1068 */
#define	METAFILE_OK	0					/* c1068 */
#define	MF_ITEM_ERR	163					/* c1068 */
#define	MF_DATA_ERR	165					/* c1068 */
#define	MF_FILE_ERR	1000					/* c1068 */

/* PID - Primitive identifier */
typedef enum {
	PLINE,
	PMARK,
	TEXT,
	FILL_AREA,
	CELL_ARRAY,
	CLIP_REC,
	MESG,
	GDP
} PID;

/* ERROR_ST - GKS error state */
typedef enum {
	ERR_ON,
	ERR_OFF
} ERROR_ST;


typedef enum {
	VAL_HORIZ,
	VAL_VERT
} VAL_AXIS;

typedef struct seg_struct {		/* set of segments associated ws a ws */
	Gint	seg;			/* segment name */
	struct seg_struct *next;
} WS_SEG_LIST;

typedef struct {
	Gfloat	height;			/* Current character height */
	Gpoint	up;			/* Current character up vector */
	Gfloat	chwidth;		/* Current character width */
	Gpoint	base;			/* Current character base vector */
	Gtxpath	path;			/* Current text path */
	Gtxalign  align;		/* Crrent text alignment */
} CHATTR;				/* Character attributes */

typedef struct {
	Gpoint	widthvec;		/* Current pattern width vector */
	Gpoint	heightvec;		/* Current pattern height vector */
	Gpoint	ptp;			/* Current pattern reference point */
} PTATTR;				/* Pattern attributes */

extern unsigned char *malloc();
#endif
