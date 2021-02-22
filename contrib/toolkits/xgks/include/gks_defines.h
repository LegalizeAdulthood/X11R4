/*
 *              Copyright IBM Corporation 1989
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
 * Urbana, IL   61801
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
 *
 */

#ifndef  LOCAL_H
#define  LOCAL_H

#include <stdlib.h>
#include <string.h>

#include "xgks.h"                       /* Standard GKS C-language binding definitions */

/* strlen was defined as STRLEN so a NULL ptr is allowed AIX port */
#define STRLEN(a)       ((a == NULL) ? 0 : strlen(a))

/* strcmp was defined as STRCMP so a NULL ptr is allowed AIX port */
/* Note: STRCMP given a NULL pointer will return 1                */
/*                                                                */
#define STRCMP(a,b) (((a) != NULL && (b) != NULL) ? strcmp((a),(b)) : 1)

/* strcpy was defined as STRCPY so a NULL ptr is allowed AIX port */
#define STRCPY(d,s) {if ((s) != NULL) strcpy((d),(s));}

#define GKSERROR( c, n, m ) \
        do { \
        if (c) { gerrorhand( n, (Gint)m, xgks_state.gks_err_file); return( n ); } \
        } while (0);

#define MAX_SEG         32767           /* Largest segment name for 16-bit integers */
#define MAX_OPEN_WS     10              /* Hardwired limits */
#define MAX_ACTIVE_WS   10
#define MAX_ASSOC_WS    10

#define MAX_BUNDL_TBL   21              /* Legal bundle table range 1-20 
                                           entry 0 will be initialized however */

/* c1143:  Added following 4 defines for use w/ predefined representations */
#define PDF_FILL_BNDLS  5               /* # of predef fill rep bundles */
#define PDF_LINE_BNDLS  5               /* # of predef line rep bundles */
#define PDF_MARK_BNDLS  5               /* # of predef marker rep bundles */
#define PDF_TEXT_BNDLS  6               /* # of predef text rep bundles */

#define DEFINED_FONT_TYPE 9             /* number of defined font type XGKS system */

#ifndef TRUE
#define TRUE            1
#define FALSE           0
#endif

#define INVALID         -1

#define OK              0

/* Metafile error flag values */                                /* c1068 */
#define METAFILE_OK     0                                       /* c1068 */
#define MF_ITEM_ERR     163                                     /* c1068 */
#define MF_DATA_ERR     165                                     /* c1068 */
#define MF_FILE_ERR     1000                                    /* c1068 */

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

typedef struct seg_struct {             /* set of segments associated ws a ws */
        Gint    seg;                    /* segment name */
        struct seg_struct *next;
} WS_SEG_LIST;

typedef struct {
        Gfloat  height;                 /* Current character height */
        Gpoint  up;                     /* Current character up vector */
        Gfloat  chwidth;                /* Current character width */
        Gpoint  base;                   /* Current character base vector */
        Gtxpath path;                   /* Current text path */
        Gtxalign  align;                /* Crrent text alignment */
} CHATTR;                               /* Character attributes */

typedef struct {
        Gpoint  widthvec;               /* Current pattern width vector */
        Gpoint  heightvec;              /* Current pattern height vector */
        Gpoint  ptp;                    /* Current pattern reference point */
} PTATTR;                               /* Pattern attributes */

#endif
