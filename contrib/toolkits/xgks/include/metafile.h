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
 * $Header: metafile.h,v 4.0 89/08/31 16:35:09 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/include/RCS/metafile.h,v $
 *
 * $Log:	metafile.h,v $
 * Revision 4.0  89/08/31  16:35:09  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.7  89/06/05  10:30:38  bruce
 * PTR# c1179:	Changed character expansion variable to new name for AIX.
 * 
 * Revision 3.6  88/12/22  14:14:22  amy
 * PTR c1144	Added typedefs for pattern size and pattern ref. point.
 * 
 * Revision 3.5  88/12/07  15:01:55  amy
 * Changed spellings from color to colour.
 * 
 * Revision 3.4  88/11/18  14:14:21  amy
 * PTR c1068	Rewrote READINT, READTPF, and READCHR macros to allow an error-
 * 	indicating return value.
 * 
 * Revision 3.3  88/08/19  13:21:21  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 1.1  88/07/21  10:47:24  david
 * Initial revision
 *  
 *
 */

/*
 * Metafiles
 *
 * $Header: metafile.h,v 4.0 89/08/31 16:35:09 amy Exp $
 */

#ifndef META_H
#define META_H

#include "xgks.h"

typedef	Gchar		*CHARPTR;
typedef Gint		*INTPTR;
typedef Gpoint		*POINTPTR;

#define INT_SIZE	sizeof(Gint)
#define FLOAT_SIZE	sizeof(Gfloat)
#define POINT_SIZE	sizeof(Gpoint)
#define IPOINT_SIZE	sizeof(Gipoint)
#define LIMIT_SIZE	sizeof(Glimit)
#define CHARPTR_SIZE	sizeof(CHARPTR)
#define INTPTR_SIZE	sizeof(INTPTR)
#define POINTPTR_SIZE	sizeof(POINTPTR)

#define READINT(fptr, i)   fscanf((fptr), " %d%*1[ ]", (int *)(&(i)))										/* c1068 */

#define READFTP(fptr, ftp)   fscanf((fptr), " %f%*1[ ]", (float *)&(ftp))									/* c1068 */

#define READCHR(fptr, chr)   fscanf((fptr), "%c", (char *)&(chr))										/* c1068 */

/* Suitable for Item type :

	0  - END ITEM
	2  - REDRAW ALL SEGMENTS ON WORKSTATION
	82 - CLOSE SEGMENT
	
	XGKSM0
*/

/* Suitable for Item type :
	
	1  - CLEAR WORKSTAION
	3  - UPDATE WORKSTAION
	21 - POLYLINE INDEX
	22 - LINETYPE 
	24 - POLYLINE COLOUR INDEX
	25 - POLYMARKER INDEX
	26 - MARKER TYPE
	28 - POLYMARKER COLOUR INDEX
	29 - TEXT INDEX
	33 - TEXT COLOUR INDEX
	35 - TEXT PATH
	37 - FILL AREA INDEX
	38 - FILL AREA INTERIOR STYLE
	39 - FILL AREA STYLE INDEX
	40 - FILL AREA COLOUR INDEX
	44 - PICK IDENTIFIER
	81 - CREATE SEGMENT
	84 - DELETE SEGMENT
*/
typedef struct {
	Gint	flag;
} XGKSMONE;



/* Suitable for Item Type :
	
	4  - DEFERRAL STATE
	30 - TEXT FONT AND PRECISION
	36 - TEXT ALIGNMENT
	83 - RENAME SEGMENT
	92 - SET SEGMENT VISIBILITY
	93 - SET SEGMENT HIGHLIGHT
	95 - SET SEGMENT DETECTABLILITY
*/
typedef struct {
	Gint	item1, item2;
} XGKSMTWO;



/* Suitable for MESSAGE :
	
	5 - XgksMoMessage
*/
typedef struct {
	Gint	strlen;
	Gchar	*string;
} XGKSMMESG;



/* Suitable for item type :
	
	11 - POLYLINE
	12 - POLYMARKER
	14 - FILL AREA
*/
typedef struct {
	Gint	num_pts;
	Gpoint  *pts;
} XGKSMGRAPH;



/* Suitable for TEXT
	
	13 - XgksMoText
*/
typedef struct {
	Gpoint	location;
	Gint	strlen;
	Gchar	*string;
} XGKSMTEXT;


/* Suitablr for Cell Array
	
	15 - XgksMoCellArray
*/
typedef struct {
	Gpoint	ll, ur, lr;
	Gipoint dim;
	Gint	*colour;
} XGKSMCELLARRAY;


/* Suitable for item type :

	23 - LINE WIDTH SCALE FACTOR
	27 - MARKER SIZE SCALE FACTOR
	31 - CHARACTER EXPANSION FACTOR
	32 - CHARACTER SPACING
*/
typedef struct {
	Gfloat	size;
} XGKSMSIZE;



/* Suitable for CHARACTER VECTRO
	
	34 - XgksMoSetCharVec
*/
typedef struct {
	Gpoint  up, base;
} XGKSMCHARVEC;



/* Suitable for ASPECT SOURCE FALGS

	43 - XgksMoSetAsf
*/
typedef struct {
	Gint	asf[13];
} XGKSMASF;



/* Suitable fot item type :
	
	51 - POLYLINE REPRESENTATION 
	52 - POLYMARKER REPRESENTATION
*/
typedef struct {
	Gint	idx,style, colour;
	Gfloat	size;
} XGKSMLMREP;



/* Suitable for : TEXT REPRESENTATION

	53 - XgksMoSetTextRep
*/
typedef struct {
	Gint	idx, font, prec, colour;
	Gfloat	tx_exp, space;		/* c1179 */
} XGKSMTEXTREP;



/* Suitable for FILL AREA REPRESENTATION

	54 - XgksMoSetFillRep
*/
typedef struct {
	Gint	idx, intstyle, style, colour;
}XGKSMFILLREP ;



/* Suitable for PATTERN REPRESENTATION

	55 - XgksMoSegPatRep
*/
typedef struct {
	Gint 	idx;
	Gipoint size;
	Gint	*array;
} XGKSMPATREP;

/* Suitable For COLOUR REPRESENTATION

	56 - XgksMoSetColourRep
*/
typedef struct {
	Gint	idx;
	Gfloat	red, green, blue;
} XGKSMCOLOURREP;



/* Suitable for item type :
	
	61 - CLIPPING RECTANGLE
	71 - WORKSTATION WINDOW
	72 - WORKSTATION VIEWPORT
*/
typedef struct {
	Glimit	rect;
} XGKSMLIMIT;



/* Suitable for SET SEGMENT TRANSFORMATION

	91 - XgksMoSegSegTrans
*/
typedef struct {
	Gint	name;
	Gfloat	matrix[2][3];
} XGKSMSEGTRAN;



/* Suitable for SET SEGMENT PRIORITY

	94 - XgksMoSegSegPri
*/
typedef struct {
	Gint	name;
	Gfloat	pri;
} XGKSMSEGPRI;

/* Suitable for SET PATTERN SIZE

        41 - XgksMoSetPatSiz
*/
typedef struct {
        Gpoint  wid;
        Gpoint  hgt;
} XGKSMPATSIZ;					/*c1144*/


/* Suitable for SET PATTERN REFERENCE PT

        42 - XgksMoSetPatRef
*/
typedef struct {
        Gpoint  ref;
} XGKSMPATREF;					/*c1144*/

#endif
