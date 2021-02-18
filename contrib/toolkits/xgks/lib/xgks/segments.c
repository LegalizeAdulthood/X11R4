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
 * segments.c - functions for handling workstation segments.
 *		gcreateseg()
 *		gcloseseg()
 *		grenameseg()
 *		gdelseg()
 *		gdelsegws()
 *		gsetsegattr()   
 *		  Setting all segment attributes :
 *		  - transformation, visibility, highlight, prioirty, detectability
 *		gsetpickid()
 *		gredrawsegws()
 *		XgksInSeg ()                PTR#  c1032
 *		gassocsegws()
 *		gcopysegws()
 *		ginsertseg()
 *
 *		i_dynamic_mod_seg_attr()
 *		i_name_open_seg()
 *		i_seg_attr()
 *		i_set_assoc_ws()
 *		i_set_seg_names()
 *		i_set_seg_names_ws()
 *		i_num_seg_priorities()
 *
 * utility functions:
 *		XgksInitGksSegments()
 *		XgksAppendSegPrimi ()	
 *		XgksAppendSegClip ()
 *		XgksSetHighLight ()
 *  		XgksDelAssocWs ()
 *		XgksClearWs ()
 *		XgksUpdateWsSegList ()
 *		XgksReDrawSeg ()
 *		XgksShowPick ()
 *		XgksFindPickSeg ()
 *		XgksIrgNec ()
 *		XgksDeleteAllSeg ()
 *		XgksReDrawAssocWs ()
 *		XgksReDrawSegWs ()
 *		XgksDrawSegToWs ()
 *		XgksOutputSeg ()
 *		XgksSegPrimiTran ()
 *	
 *
 * for segment state list : there's only one segment state list, it's mantained and
 *			    defined only in this file. manipulating routines are :
 *	      	XgksInstallSeg (); 	
 *		XgksDeleteSeg (); this list is implemented as a hash table
 *		XgksNewSeg ();
 *		XgksFindSeg ();
 *
 * for workstation segment list : there're one of this lists associated with each ws.
 *			   List of segments are linked by the order of their 
 *			   creation.
 *			   Manipulating routines are :
 *		XgksInsertWsSeg ();
 *		XgksDeleteWsSeg ();
 *		XgksRenameWsSeg ();
 *		XgksNewWsSeg ();
 *
 *
 * NOTE : one should never, NEVER use the segment state list to do redraw !, the 
 * ws->seglist is there for this purpose. segment state list is a good and convenient
 * way of storing and retrieving segment attributes.
 *
 * $Header: segments.c,v 4.0 89/08/31 16:24:21 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/segments.c,v $
 *
 * $Log:	segments.c,v $
 * Revision 4.0  89/08/31  16:24:21  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.131  89/06/15  17:18:06  bruce
 * PTR# c1175:	Changed GKSERROR condition for valid segment names.
 * 
 * Revision 3.130  89/06/05  10:14:24  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * PTR# c1176:	Replaced string function calls with macros that check for
 * 		NULL pointers before making the call.
 * PTR# c1179:	Changed exp field to tx_exp or ch_exp for AIX compiler
 * 		Moved variable declarations illegal in AIX.
 * 
 * Revision 3.129  89/04/13  14:57:11  bruce
 * PTR# c1170:	The function XgksInSeg was modified to use a small stretching factor
 * 		when testing if a point is located inside a segment extent. This was
 * 		done to allow picking of single primitive segments (polylines and polymarkers)
 * 		which before this change could not be picked.
 * 
 * Revision 3.128  89/03/16  13:06:19  amy
 * XgksCleanUpWsSegList:  removed 'static' from definition and declarations.
 * 
 * Revision 3.127  89/03/16  12:50:50  amy
 * Rearranged order of new functions for finding picked seg.
 * Added back in tmp_pickid.
 * 
 * Revision 3.126  89/03/15  20:06:03  amy
 * PTR c1165	XgksCleanUpWsSegList:  adjust ws->seg_insertpt
 * 		when removing a segment from the ws segment list.
 * 
 * Revision 3.125  89/03/15  14:35:09  amy
 * PTR c1160	gsetpickid:  change check for valid pickid:  0 or greater 
 * 		is valid, negative values are invalid.
 * 
 * Revision 3.124  89/02/13  16:44:32  amy
 * PTR c1154	XgksInitGksSegments:  set pickid to 0.
 * 
 * Revision 3.123  89/02/04  15:28:15  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.122  88/12/29  14:59:37  amy
 * PTR c1142	Current pick id and current open segment name moved from
 * static vars in segments.c to global vars in the GKS state list (defined
 * in open_gks.c; declared extern in gkslist.h), where they belong (also so
 * metafile code can get at them).
 * 
 * Revision 3.121  88/12/29  13:10:58  amy
 * PTR c1141	XgksDelAllMoSeg:  function added to search through the
 * 		segment hash table and attempt to disassociate the MO ws from
 * 		all existing segments-- unassociated segments ignore the request.
 * 
 * Revision 3.120  88/12/29  13:05:28  amy
 * PTR c1139	gcreateseg, gdelseg, gdelsegws, gassocsegws, XgksRenameWsSeg:
 * 		do not call housekeeping functions dealing with the workstation
 * 		segment list or input device list on an MO workstation,
 * 		which does not have these lists.
 * 
 * Revision 3.119  88/12/28  17:25:43  amy
 * PTR c1137	XgksSegCopyMo:  calls XgksMoSetClipOnWs instead of XgksMoSetClip
 * 		to write out clipping indicator information to the specific
 * 		MO ws receiving the segment, instead of to all active MO.
 * 
 * Revision 3.118  88/12/28  16:07:20  amy
 * PTR c2012	grenameseg:  if the segment to be renamed is the currently
 * 		open one, update the GKS state list with the new name.
 * 
 * Revision 3.117  88/12/22  18:24:45  amy
 * PTR c1073	gredrawsegws:  added code to output segment info. to metafile if
 * 		MO open.
 * 		XgksDeleteWsSeg:  returns name of deleted segment, rather than
 * 		INVALID, if successful, per comment in function.
 * 		XgksCleanUpWsSeglist:  update pointer to the segment list in 
 * 		the workstation structure when the first segment in the list is
 * 		updated.
 * 
 * Revision 3.116  88/12/22  13:48:35  amy
 * PTR c1144	gassocsegws:  restore GKS state on metafile out workstation.
 * 		gcopysegws:  restore GKS state on metafile out workstation.
 * 		ginsertseg:  restore GKS state on metafile out workstation.
 * 		XgksSegTransProcessMo:  set asfs on every active MO ws; set
 * 		clipping rectangle on MO.
 * 		Set Pattern Size and Reference Point:  implement to write
 * 		to metafile out ws.
 * 
 * Revision 3.115  88/12/21  18:01:42  amy
 * PTR c1080	XgksUpdateWsSegList:  change test for relative priority of
 * 		segments.
 * 
 * Revision 3.114  88/12/20  15:11:08  amy
 * PTR c1032	XgksInSeg function added.
 * 		XgksShowPick modified to simple highlight segment.  (Used to
 * 		also find the segment to highlight.)
 * 		XgksFindPickSeg:  performance improvement.
 * 
 * Revision 3.113  88/12/08  12:56:05  amy
 * PTR c1130	GKSERROR and gerrorhand calls:  changed function name parameter
 * 		from character string to enum. type value.
 * Changed spelling of color to colour.
 * 
 * Revision 3.112  88/11/15  16:33:00  amy
 * New 4.3 C compiler--
 * 	ginsertseg:  Remove apparently meaningless definition and use of
 * 		tmp_pickid, to avoid compiler warning message.
 * 	XgksDistPline:  move definition
 * 		of variable DD to inside the ifdef code where it is used.
 * 
 * Revision 3.111  88/09/23  09:53:07  amy
 * Edited 'MIT' log entry.
 * 
 * Revision 3.110  88/09/23  09:34:25  amy
 * MIT	Include strings.h
 * 	Throughout:  declare local and global functions.
 * 	XgksSegCopyMo:  call XgksMoSetClip with address of correct field of 
 *	structure.
 * 	XgksMoSetFillIntStyleOnWs:  remove superfluous '38' parameter to call.
 * 	XgksDistFillarea:  remove unused parm 'ws' from definition and calls.
 * 
 * Revision 3.109  88/08/18  07:39:24  amy
 * AUG  XgksDistPline:  changed calculation of distance of pick input point and a 
 *	polyline primitive.
 *      Did not merge in changes to fix message (XgksClearWs).
 * 
 * Revision 3.108  88/08/15  07:05:54  amy
 * Removed printf
 * 
 * Revision 3.107  88/08/15  06:56:20  amy
 * PTR c1001  XgksClearWs modified for fix to message problem.
 * 
 * Revision 1.5  88/08/08  15:42:26  bruce
 * PTR# c1001 - code was added to clear the message pt in the workstation state table.
 * 
 * Revision 1.4  88/07/28  14:59:44  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.3  88/07/26  17:53:41  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.2  88/07/26  15:19:02  owens
 * no change
 * 
 * Revision 1.1  88/07/21  10:50:06  david
 * Initial revision
 *  
 *
 */

static char rcsid[]="$Header: segments.c,v 4.0 89/08/31 16:24:21 amy Exp $";

#include "gks_implem.h"                 /* d1 */
#include <strings.h>		/*MIT*/
#include <math.h>



#define MAXNUM 1.0e+20

/* definition for segment state list */
typedef struct seg_st {
	Gint		assoc_ws[MAX_ASSOC_WS];	/* Set of associated workstations */
	Gsegattr	segattr;		/* Segment attribute including segment name */
	OUT_PRIMI	primi_list;		/* First primitive is always a CLIP_REC */
	OUT_PRIMI	*primi_insert_pt;	/* Primitive list manger need this for efficient insertion */
	Glimit		bound;			/* Bounding box for all primitive */
	Gint		text_present;		/* Flag indicating if there's text primitive in segments */
	struct seg_st	*seg_next;		/* pointer to next */
} *SEG_STATE_PTR, SEG_STATE_ENTRY;

/* c1142:  segment information and state list moved to gks state list */

#define SHSIZE		128
#define SHASH(segname)	(segname & (SHSIZE-1))

/*
 * Macro --- SegTrans (ndcpt, tranpt, matric)
 *	
 * Gpoint *ndcpt;		the normalized device coordinate point passed in
 * Gpoint *transpt		the transformed ndc point return
 * Gfloat matrix[2][3]		the transformation matirx
 */
#define SegTrans(ndcpt, transpt, matrix) {   \
	(transpt)->x = (ndcpt)->x*(matrix[0][0])+(ndcpt)->y*(matrix[1][0])+(matrix[0][2]); \
	(transpt)->y = (ndcpt)->x*(matrix[0][1])+(ndcpt)->y*(matrix[1][1])+(matrix[1][2]); \
}

#define MAX(a,b)	((a)>(b) ? (a) : (b))
#define MIN(a,b)	((a)>(b) ? (b) : (a))

#define NOT_WISS(ws_id)	((ws_id) != xgks_state.wiss_id)			/* c1147 */

static SEG_STATE_PTR segtable[SHSIZE];	/* Hash table where segment state information are stored */

static SEG_STATE_PTR XgksFindSeg ();
static SEG_STATE_PTR XgksDeleteSeg ();
static SEG_STATE_PTR XgksNewSeg();
static WS_SEG_LIST  *XgksNewWsSeg();
static OUT_PRIMI    *XgksSegPrimiTran ();
static float XgksInFill();
static float XgksFindDistance();
static float XgksDistPline();
static float XgksDistPmark();
static float XgksDistFillarea();
static float XgksDistCellarray();
static float XgksDistText();

OUT_PRIMI *XgksDuplicatePrimi ();
/*
 * XgksInitGksSegments() - utility function to initialize GKS segment data.
 */
XgksInitGksSegments()
{
	Gint i;

	for (i=0; i<SHSIZE; i++)
		segtable[i] = (SEG_STATE_PTR) NULL;
	xgks_state.gks_open_seg	= INVALID;			/* c1147 */
	xgks_state.gks_pick_id	= 0;			  /* c1154 c1147 */
}
/*$F
 * gcreateseg(name) - CREATE SEGMENT
 *
 * Gint name;		Name of the new segment. 1..n   c1175
 *
 * returns: 0, 3, 120, 121, 300
 *
 * See Also: ANSI Standard p.111
 */
gcreateseg(name)
Gint name;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();		/*MIT*/
	static SEG_STATE_PTR XgksNewSeg();		/*MIT*/

	SEG_STATE_PTR  seg;
	Gint i, wscnt;

/* STEP 1: Check for errors */
/* check for proper state */
	GKSERROR ((xgks_state.gks_state != GWSAC) ,3, errgcreateseg) /* c1147 */

/* check for valid segment name */
	GKSERROR ((name<1) ,120, errgcreateseg)         /* c1175 */

/* check for name in use */
	GKSERROR ((XgksFindSeg(name) != NULL) ,121, errgcreateseg)

/* STEP 2: Allocate memory for segment state list entry */
	GKSERROR (((seg=XgksNewSeg()) == NULL) ,300, errgcreateseg)

	if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (81, name);

	xgks_state.gks_open_seg = seg->segattr.seg = name;	/* c1147 */

	/* install it in segment state list */
	XgksInstallSeg (seg);

	/* Output assocaite this segment with all avtive ws and
	   insert it into ws->seglist */
	i = 0;
	for (wscnt=0; wscnt < MAX_ACTIVE_WS; wscnt++)  
		if (xgks_state.activews[wscnt].ws_id != INVALID) {	/* c1147 */
			seg->assoc_ws[i++] = xgks_state.activews[wscnt].ws_id; /* c1147 */
			if (xgks_state.activews[wscnt].ws->ewstype != MO) { /* c1139 */ /* c1147 */
				XgksInsertWsSeg (xgks_state.activews[wscnt].ws, name); /* c1147 */
				UPDATE_SEG_CNT(xgks_state.activews[wscnt].ws->primi_insert_pt); /* c1147 */
			}					 /* c1139 */
		}

	for(; i < MAX_ASSOC_WS; i++)
		seg->assoc_ws[i] = INVALID;

/* STEP 3: Change operating state */
	xgks_state.gks_state = GSGOP;				/* c1147 */

	return(0);
}
/*$F
 * gcloseseg() - CLOSE SEGMENT
 *
 * returns: 0, 4
 *
 * See Also: ANSI Standard p.111
 */
gcloseseg()
{
/* STEP 1: check for errors */
	GKSERROR ((xgks_state.gks_state != GSGOP) ,4, errgcloseseg)	/* c1147 */

/* STEP 2: INVALIDate gks_open_seg */
	xgks_state.gks_open_seg = INVALID;			/* c1147 */

/* STEP 3: change state */
	xgks_state.gks_state = GWSAC;				/* c1147 */

	if (MO_OPENED == TRUE) XgksMoCloseSeg ();

	return(0);
}
/*$F
 * grenameseg(old, new) - RENAME SEGMENT
 *
 * Gint old,			name of an existing segment
 *	new;			new name for the segment.
 *
 * returns: 0 = OK, or one of 7, 120, 121, 122
 *
 * See also: ANSI standard p.111
 */
grenameseg(old, new)
Gint old, new;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();		/*MIT*/
	static SEG_STATE_PTR XgksDeleteSeg ();		/*MIT*/

	SEG_STATE_PTR seg;
	Gint i;

/* STEP 1: check for errors */
/* check for proper state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgrenameseg) /* c1147 */

/* check for invalid name */
	GKSERROR (( (old<1) || (new<1)),120, errgrenameseg)     /* c1175 */

/* check for new name already in use */
	GKSERROR ((XgksFindSeg(new) != NULL) ,121, errgrenameseg)

/* check for existance of old segment by delete it from segment state list */
	GKSERROR (((seg=XgksDeleteSeg(old)) == NULL) ,122, errgrenameseg)

	if (MO_OPENED == TRUE) XgksMoRenameSeg (old, new);

/* STEP 2: change the segments name and re-install it into segment state list */
	seg->segattr.seg = new;
/* If segment being renamed is the currently open one, update gks state list. */
	if (xgks_state.gks_open_seg == old)		/*c2012*/	/* c1147 */
		xgks_state.gks_open_seg = new;		/*c2012*/	/* c1147 */
	XgksInstallSeg(seg);

/* Now do the renaming in all associated ws->seglist */
	for (i=0; seg->assoc_ws[i] != INVALID; i++)
/* DWO 7/26/88  changed macro name from VALID_WSID */
		XgksRenameWsSeg(OPEN_WSID(seg->assoc_ws[i]), old, new);  /* c1012 */

	return(0);
}
/*$F
 * gdelseg(name)	- DELETE SEGMENT
 *
 * Gint name;		Name of segment to delete from all workstations.
 *
 * returns 0, 7, 120, 122, 125
 *
 * See Also: ANSI Standard p.112
 */
gdelseg(name)
Gint name;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksDeleteSeg ();		/*MIT*/

	WS_STATE_PTR ws;
	SEG_STATE_PTR seg;
	Gint wscnt;

/* STEP 1: check for errors. */
/* check for proper state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgdelseg) /* c1147 */

/* check for invalid name */
	GKSERROR ((name < 1) ,120, errgdelseg)  /* c1175 */

/* check for segment currently open */
	GKSERROR ((xgks_state.gks_state == GSGOP && xgks_state.gks_open_seg == name) ,125, errgdelseg) /* c1147 */

/* try to delete the segment from segment state list in the mean time
   check for existance of segment */
	GKSERROR (((seg=XgksDeleteSeg(name)) == NULL) ,122, errgdelseg)

	if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (84, name);

/* Now delete the segment from ws segment list and redraw all associated workstations */
	for (wscnt=0; seg->assoc_ws[wscnt] != INVALID; wscnt++) {
/* DWO 7/26/88  changed macro name from VALID_WSID */
		ws = OPEN_WSID(seg->assoc_ws[wscnt]);  /* c1012 */
		if (ws->ewstype != MO) {			/* c1139 */
			XgksDeleteWsSeg (ws, name);
			REDRAWWS(ws);
		}						/* c1139 */
	}

	XgksDeletePrimi(&(seg->primi_list), &(seg->primi_insert_pt));  /* remove ALL assoc primitive */
	free (seg);		/* Free up the memory */
	return(0);
}
/*$F
 * gdelsegws(ws_id, name) - DELETE SEGMENT FROM WORKSTATION
 *
 * Gint	ws_id;			workstation to delete segment from.
 * Gint name;			segment to delete.
 *
 * returns 0, 7, 20, 25, 33, 35, 120, 123, 125
 *
 * See Also: ANSI Standard p.112
 */
gdelsegws(ws_id, name)
Gint ws_id;
Gint name;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();	/*MIT*/

	WS_STATE_PTR ws;
	SEG_STATE_PTR seg;

/* STEP 1: check for errors. */
/* check for proper state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgdelsegws) /* c1147 */

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgdelsegws)   /* c1012 */

/* check for invalid workstation identifier */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws = OPEN_WSID(ws_id)) == NULL) ,25, errgdelsegws)  /* c1012 */

/* check workstation category */
	GKSERROR ((WS_CAT(ws) == GMI) ,33, errgdelsegws)

	GKSERROR ((WS_CAT(ws) == GINPUT) ,35, errgdelsegws)

/* check for invalid name */
	GKSERROR ((name < 1) ,120, errgdelsegws)        /* c1175 */

/* check for existance of segment */
	GKSERROR (( (seg=XgksFindSeg(name)) == NULL ) ,123, errgdelsegws)

/* check for segment currently open */
	GKSERROR ((xgks_state.gks_state == GSGOP && xgks_state.gks_open_seg == name) ,125, errgdelsegws) /* c1147 */

/* delete the segment from the assoc_ws array */
	GKSERROR ( (XgksDelAssocWs(seg, ws->ws_id) == INVALID), 123, errgdelsegws)

	if (ws->ewstype != MO) XgksDeleteWsSeg (ws, name);	/* c1139 */

	if (ws->ewstype == MO) XgksMoSetGraphicAttrOnWs (ws, 84, name);

	REDRAWWS(ws);

	return(0);
}
/*$F
 * gsetsegattr (name segattr) - SET SEGMENT ATTRIBUTES
 *
 * Gint name;			segment whos attribute is to be changed.
 * Gsegattr	*segattr;	Pointer to segment attribute structure
 *
 * returns 0,
 *      transformation : 7, 120, 122 (ANSI standard p115)
 *	visibility     : .. same ..  (ANSI standard p116)
 *	highlighting   : .. same ..  (ANSI standard p116)
 *	detectability  : .. same ..  (ANSI standard p117)
 *	priority       : 7, 120, 122, 126 (ANSI standard p117)
 */
gsetsegattr (name, segattr)
Gint name;
Gsegattr *segattr;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();		/*MIT*/
	static void XgksCheckSegAttr ();		/* c1137 */

	SEG_STATE_PTR seg;
	WS_STATE_PTR  ws;
	Gint	Redraw, Hilighted, wscnt;
	Gsegattr old;
	
/* STEP 1: check for errors. */
/* check for proper state */
	GKSERROR ((xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP) ,7, errgsetsegattr) /* c1147 */

/* check for invalid name */
	GKSERROR ((name < 1) ,120, errgsetsegattr)      /* c1175 */

/* check for existance osegment */
	GKSERROR (((seg = XgksFindSeg(name))==NULL) ,122, errgsetsegattr)

/* check for valid priority */
	GKSERROR( (segattr->pri>1.0 || segattr->pri<0.0), 126, errgsetsegattr);

	if (MO_OPENED == TRUE) XgksCheckSegAttr (&(seg->segattr), segattr);

/* STEP 2: Go ahead and change the attribute to the segment in state list */
	old = seg->segattr;
	segattr->seg = seg->segattr.seg;	/* ingore input seg name */
	seg->segattr = (*segattr);

/* See if priority is changed */
/* NOTE : no redraw or anything like that will happen here, just re-arranging the assoc ws-seg_list */

/*  MORE IMPORTANT NOTE :: --> Rearrangement of the ws->seglist cannot occur 
			       until right before the GKS-generated redraw, else
			       a x-initiated redraw will leave ws in a inconsistant state */
	if (old.pri != seg->segattr.pri) {
		wscnt = 0;
		while (wscnt<MAX_ASSOC_WS && seg->assoc_ws[wscnt] != INVALID) {
/* DWO 7/26/88  changed macro name from VALID_WSID */
			ws = OPEN_WSID(seg->assoc_ws[wscnt]);  /* c1012 */
			ws->seg_list_dirty = TRUE;
			wscnt++;
		}
	}
/* See if redraw is necessary -- Will take care of highlight */
	Redraw = FALSE;
	if (XgksIrgNec(&(old), &(seg->segattr)) == TRUE) {
		XgksReDrawAssocWs(seg);
		Redraw = TRUE;
	}

	if (old.hilight == seg->segattr.hilight) Hilighted = FALSE;
	else Hilighted = TRUE;

/* If making VISIBLE, draw it out !! and check if hilighting is necessary */
	if (old.vis == GINVISIBLE && seg->segattr.vis == GVISIBLE) {
		wscnt = 0;
		while (wscnt<MAX_ASSOC_WS && seg->assoc_ws[wscnt]!=INVALID) {
			if (NOT_WISS(seg->assoc_ws[wscnt])) {
/* DWO 7/26/88  changed macro name from VALID_WSID */
				ws = OPEN_WSID(seg->assoc_ws[wscnt]);  /* c1012 */
				if (ws->wsdus.irgmode != GALLOWED) {
					XgksOutputSeg (ws, seg);
			        	if (seg->segattr.hilight == GHIGHLIGHTED) 
				    		XgksSetHighLight(ws, seg);
				}
			}
			wscnt++;
		}
	} else if (Hilighted == TRUE && seg->segattr.vis == GVISIBLE) {
			wscnt = 0;
			while (wscnt<MAX_ASSOC_WS && seg->assoc_ws[wscnt]!=INVALID) {
			     if (NOT_WISS(seg->assoc_ws[wscnt])) {
/* DWO 7/26/88  changed macro name from VALID_WSID */
			     	ws = OPEN_WSID(seg->assoc_ws[wscnt]);  /* c1012 */
			     	if ( (Redraw == FALSE) || (Redraw == TRUE && ws->wsdus.irgmode != GALLOWED) ) 
			     		XgksSetHighLight(ws, seg);
			    }
			    wscnt++;
			}
		}
	return(0);
}
/*$F
 * gsetpickid(pick_id) - SET PICK IDENTIFER
 *
 * Gint pick_id;		new pick identifier.
 *
 * returns: 0 = OK, or one of 8, 97
 *
 * See Also: ANSI Standard p.99
 */
gsetpickid(pick_id)
Gint pick_id;
{
/* STEP 1: check for errors */
/* gks in proper state? */
	GKSERROR ((xgks_state.gks_state == GGKCL) ,8, errgsetpickid)	/* c1147 */

/* valid pick id? */
	GKSERROR ((pick_id < 0) ,97, errgsetpickid)	/* c1160 */

/* STEP 2: change the current pick id */
	xgks_state.gks_pick_id = pick_id;				/* c1147 */

	if (MO_OPENED == TRUE) XgksMoSetGraphicAttr (44, pick_id);

	return(0);
}
/*$F
 * gassocsegws(ws_id, seg_id) ASSOCIATE SEGMENT WITH WORKSTATION
 *
 * Gint  ws_id;
 * Gint seg_id;
 *
 * returns : 0 = OK, or one of 6, 20, 25, 27, 33, 35, 120, 124
 *
 * See also: ANSI standard p.113
 */
gassocsegws (ws_id, seg_id)
Gint ws_id, seg_id;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();	/*MIT*/
	static void XgksSegAttrMo ();			/* c1147 */
	static void XgksSegProcessMo ();		/* c1147 */
	static void XgksRestoreMoGksStateOnWs ();	/* c1147 */

	WS_STATE_PTR ws, wis;
	SEG_STATE_PTR seg;
	WS_SEG_LIST *WsSeg;
	Gint i;

/* check for operating state */
	GKSERROR ( (xgks_state.gks_state != GWSOP && xgks_state.gks_state != GWSAC), 6, errgassocsegws) /* c1147 */

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgassocsegws)   /* c1012 */

/* Check if ws_id is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR ( ((ws=OPEN_WSID(ws_id))==NULL), 25, errgassocsegws)  /* c1012 */

/* Check if WISS ws is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR ( ((xgks_state.wiss_id == INVALID) || ((wis=OPEN_WSID(xgks_state.wiss_id)) == NULL)), 27, errgassocsegws); /* c1012 *//* c1147 */

/* check for valid ws category */
	GKSERROR ( (WS_CAT(ws) == GMI), 33, errgassocsegws)
	GKSERROR ( (WS_CAT(ws) == GINPUT), 35, errgassocsegws)
 
/* check for valid seg_id */
	GKSERROR ( (seg_id<1), 120, errgassocsegws)     /* c1175 */

/* Check if Segment is on WISS */
	WsSeg = wis->seglist;
	while (WsSeg != NULL) {
		if (WsSeg->seg == seg_id) break;
		WsSeg = WsSeg->next;
	}
	GKSERROR ( (WsSeg == NULL),  124, errgassocsegws);

/* Locate the desired segment from segment state list */
	seg = XgksFindSeg (WsSeg->seg);

/* Insert the new ws_id into seg assoc_ws array if the 
   ws_id is not already in there */
	i = 0;
	while (seg->assoc_ws[i] != INVALID) {
	    if (seg->assoc_ws[i] == ws_id) 
		return(0);		/* <---- NOTICE the return here !! */
	    i++;
	}
	seg->assoc_ws[i] = ws_id;

/* Now insert the segment into ws->seglist if ws is not MO */	/* c1139 */
	if (ws->ewstype != MO) {				/* c1139 */
		XgksInsertWsSeg (ws, seg_id);
		UPDATE_SEG_CNT(ws->primi_insert_pt);
	}							/* c1139 */

/* if destination ws is a MO, do what should be done, and return */
	else {							/* c1139 */
		XgksMoSetGraphicAttrOnWs (ws, 81, seg_id);
		XgksSegAttrMo (ws, seg);
		XgksRestoreMoGksStateOnWs (ws);       /* c1144 */
		XgksSegProcessMo (ws, seg);
		XgksMoCloseSegOnWs (ws);
		return (OK);
	}

/* Now if irgmode is not suppressed go ahead and redraw whole ws
   else set ws->wsdu.nframe == GYES as necessary */
	if (ws->wsdus.irgmode == GSUPPRESSED) {
	/* Now output the segment onto the destinatin ws (if it's visible) */
		if (seg->segattr.vis == GVISIBLE) {
			XgksIDevDisable (ws);
			XgksReDrawSeg ( ws, seg->segattr.seg);
			XgksIDevEnable (ws);
			ws->wsdus.nframe = GYES;
		}
	} else REDRAWWS(ws);
	
	return (0);
}
/*$F
 * gcopysegws(ws_id, seg_id) COPY SEGMENT TO WORKSTATION
 *
 * Gint  ws_id;
 * Gint seg_id;
 *
 * returns : 0 = OK, or one of 6, 20, 25, 27, 33, 35, 36, 120, 124
 *
 * See also: ANSI standard p.113
 */
gcopysegws (ws_id, seg_id)
Gint ws_id, seg_id;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();		/*MIT*/
	static OUT_PRIMI    *XgksSegPrimiTran ();	/*MIT*/
	static void XgksSegCopyMo ();			/* c1147 */
	static void XgksRestoreMoGksStateOnWs ();	/* c1147 */

	WS_STATE_PTR ws, wis;
	SEG_STATE_PTR seg;
	WS_SEG_LIST *WsSeg;
	OUT_PRIMI *clip, *primi, *tran;
	Glimit	wsclip;

/* check for operating state */
	GKSERROR ( (xgks_state.gks_state != GWSOP && xgks_state.gks_state != GWSAC), 6, errgcopysegws) /* c1147 */

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgcopysegws)   /* c1012 */

/* Check if ws_id is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR ( ((ws=OPEN_WSID(ws_id))==NULL), 25, errgcopysegws)  /* c1012 */

/* Check if WISS ws is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR ( ((xgks_state.wiss_id == INVALID) || ((wis=OPEN_WSID(xgks_state.wiss_id)) == NULL)), 27, errgcopysegws); /* c1012 *//* c1147 */

/* check for valid ws category */
	GKSERROR ( (WS_CAT(ws) == GMI), 33, errgcopysegws)
	GKSERROR ( (WS_CAT(ws) == GINPUT), 35, errgcopysegws)
	GKSERROR ( (WS_CAT(ws) == GWISS), 36, errgcopysegws)

/* check for valid seg_id */
	GKSERROR ( (seg_id<1), 120, errgcopysegws)      /* c1175 */

/* Check if Segment is on WISS */
	WsSeg = wis->seglist;
	while (WsSeg != NULL) {
		if (WsSeg->seg == seg_id) break;
		WsSeg = WsSeg->next;
	}
	GKSERROR ( (WsSeg == NULL),  124, errgcopysegws);

/* if ws is of type MO, do what should be done */
	if (ws->ewstype == MO)  {
		XgksRestoreMoGksStateOnWs (ws);			/* c1144 */
		XgksSegCopyMo (ws, XgksFindSeg(seg_id));
	}

	wsclip = ws->clip;	/* Save current workstation clip */
	XgksIDevDisable (ws);	/* Disable all input devices */

/* Get ready to output all primitives in segment to ws */
	seg = XgksFindSeg (seg_id);
	primi = &(seg->primi_list);
	while (primi != NULL) {
		if (primi->pid == CLIP_REC) {
			if (ws->primi_insert_pt->pid == CLIP_REC) {
				XgksWsWinInterset(ws,&(primi->primi.clip.rec),&(ws->primi_insert_pt->primi.clip.rec));
			} else {
				clip = XgksNewPrimi ();
			        clip->pid = CLIP_REC;
				XgksWsWinInterset(ws, &(primi->primi.clip.rec), &(clip->primi.clip.rec));
                        	XgksInsertPrimi(&(ws->primi_insert_pt), clip); 
			}
		} else {
			tran =  XgksSegPrimiTran (primi, seg->segattr.segtran);
			XgksInsertPrimi ( &(ws->primi_insert_pt), tran);
			XgksReDrawWs (ws, tran);
		}
		primi = primi->next;
	}
	XgksIDevEnable (ws);	/* Disable all input devices */
	ws->clip = wsclip;	/* Restore the old ws->clip */
	xXgksUpdateClip(ws);	/* imform x about the clip-rec change */ /* c1147 */

	return (0);
}
/*$F
 * ginsertseg(seg_id, segtran) - INSERT SEGMENT
 *
 * Gint seg_id;
 * Gfloat segtran[2][3];
 *
 * returns : 0 = OK, or one of 5, 27, 120, 124, 125
 *
 * See also: ANSI standard p.114
 */
ginsertseg (seg_id, segtran)
Gint seg_id;
Gfloat segtran[2][3];
{
	/*
	 * declare global functions
	 */
	OUT_PRIMI *XgksDuplicatePrimi();		/*MIT*/
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();		/*MIT*/
	static OUT_PRIMI    *XgksSegPrimiTran ();	/*MIT*/
	static void XgksRestoreMoGksState ();		/* c1147 */
	static void XgksSegTransProcessMo ();		/* c1147 */

	WS_STATE_PTR wis;
	WS_SEG_LIST *WsSeg;
	SEG_STATE_PTR seg;
	OUT_PRIMI  *primi, *tran, *tran2;
	Gint   tmp_pickid = 0;		/* MWW: added to suppress warning */

/* check for operating state */
	GKSERROR ( (xgks_state.gks_state != GWSAC && xgks_state.gks_state != GSGOP), 5, errginsertseg) /* c1147 */

/* Check if WISS ws is opened */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR ( ((xgks_state.wiss_id == INVALID) || ((wis=OPEN_WSID(xgks_state.wiss_id)) == NULL)), 27, errginsertseg); /* c1012 *//* c1147 */

/* check for valid seg_id */
	GKSERROR ( (seg_id<1), 120, errginsertseg)      /* c1175 */

/* check if seg_id is currently opened */
	GKSERROR ( (xgks_state.gks_state == GSGOP && xgks_state.gks_open_seg == seg_id), 125, errginsertseg) /* c1147 */

/* Check if Segment is on WISS */
	WsSeg = wis->seglist;
	while (WsSeg != NULL) {
		if (WsSeg->seg == seg_id) break;
		WsSeg = WsSeg->next;
	}
	GKSERROR ( (WsSeg == NULL),  124, errginsertseg);

/* if MO opened, note what's going on in to MO */
	if (MO_OPENED == TRUE)	 {
		XgksRestoreMoGksState ();				/* c1144 */
		XgksSegTransProcessMo (XgksFindSeg(seg_id), segtran);	/* c1144 */
	}

	if (xgks_state.gks_state == GSGOP)  tmp_pickid = xgks_state.gks_pick_id; /*c1147*/

/* Get ready to output all primitives in segment to ws */
	seg = XgksFindSeg (seg_id);
	primi = &(seg->primi_list);
	while (primi != NULL) {
		if (primi->pid != CLIP_REC) {			/* Ignore all clipings use, current
								   gkslist clip */
			if (xgks_state.gks_state == GSGOP) {		/* c1147 */
				xgks_state.gks_pick_id = primi->pickid; /* c1147 */
				tran2 = XgksDuplicatePrimi(XgksSegPrimiTran(primi, segtran));
				if ((tran = XgksAppendSegPrimi (tran2)) != NULL)
					XgksOutputToWs (tran);
			} else {
				tran = XgksSegPrimiTran (primi, segtran);
				XgksAppendWsPrimi (tran);
				XgksOutputToWs (tran);
			}
		}
		primi = primi->next;
	}

	if (xgks_state.gks_state == GSGOP)   xgks_state.gks_pick_id = tmp_pickid; /*c1147*/

	return (0);
}
/*$F
 * gredrawsegws(ws_id) - REDRAW ALL SEGMENTS ON WORKSTATION
 *
 * Gint ws_id;
 *
 * returns: 0 = OK, or one of 7, 20, 25, 33, 35, 36
 *
 * See also: ANSI standard p.77
 */
gredrawsegws(ws_id)
Gint ws_id;
{
	WS_STATE_PTR ws;

/* check proper gks operating state */
	GKSERROR ((xgks_state.gks_state==GGKOP || xgks_state.gks_state==GGKCL) ,7, errgredrawsegws) /* c1147 */

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgredrawsegws)   /* c1012 */

/* check valid ws_id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id))==NULL) ,25, errgredrawsegws)  /* c1012 */

/* check for valid workstation category */
	GKSERROR ((WS_CAT(ws) == GMI) ,33, errgredrawsegws)

	GKSERROR ((WS_CAT(ws) == GINPUT) ,35, errgredrawsegws)

	GKSERROR ((WS_CAT(ws) == GWISS) ,36, errgredrawsegws)

/* If MO, output info to file */
	if (ws->ewstype == MO) {				/* c1073 */
		XgksMoReDrawAllSeg (ws);			/* c1073 */
	}							/* c1073 */
	else {							/* c1073 */
		XgksReDrawSegWs (ws);				/* c1073 */
	}

/* if necessary call user defined redraw notifying function */
	if (ws->redrawfuncp != NULL)
		(*(ws->redrawfuncp))( ws->ws_id, GRD_GKS );

	return(OK);
}
/* 
 * SEG_STATE_PTR
 * XgksFindSeg (name)
 * Gint name;		 - the target segment name;
 *
 * Tries to find the <name> segment in the segment table, if found return a pointer to 
 * to the segment state, else return NULL
 *
 */
static SEG_STATE_PTR XgksFindSeg (name)
Gint name;
{
	SEG_STATE_PTR next;

	next = segtable[SHASH(name)];
	while (next != NULL) {
		if (next->segattr.seg == name) return(next);
		next = next->seg_next;
	}
	return (NULL);
}
/*
 *   XgksNewSeg() - will return a pointer to an empty segment structure
 */
static SEG_STATE_PTR XgksNewSeg()
{
	SEG_STATE_PTR new;
	Gint i;

	if ( (new = (SEG_STATE_PTR)  malloc ( (unsigned) sizeof(SEG_STATE_ENTRY) )) != NULL )  {
		new->primi_list.pid = CLIP_REC;
		new->primi_list.primi.clip.segment = TRUE;
		new->primi_list.primi.clip.rec = xgks_state.cliprec.rec; /*c1147*/
		new->primi_list.next = NULL;
		new->primi_insert_pt = &(new->primi_list);
		new->segattr.seg = INVALID;
		new->segattr.vis = GVISIBLE;
		new->segattr.hilight = GNORMAL;
		new->segattr.pri = 0.0;
		new->segattr.det = GUNDETECTABLE;
		new->text_present = FALSE;
		new->seg_next = NULL;
		new->bound.xmax = new->bound.ymax = 0.0;
		new->bound.xmin = new->bound.ymin = 1.0;
		for (i=0; i<3; i++)
			new->segattr.segtran[0][i] = new->segattr.segtran[1][i] = 0.0;
		new->segattr.segtran[0][0] = new->segattr.segtran[1][1] = 1.0;
		for (i=0; i<MAX_ASSOC_WS; i++) new->assoc_ws[i] = INVALID;
	}
	return (new);
}
/*
 *   XgksInstallSeg(seg) - 
 *       SEG_STATE_PTR  seg  install seg-> into the segment state hash table
 */
static XgksInstallSeg (seg)
SEG_STATE_PTR seg;
{
	seg->seg_next = segtable[SHASH(seg->segattr.seg)];
	segtable[SHASH(seg->segattr.seg)] = seg;
}
/*
 *   XgksDeleteSeg (name) -  remove the entry from the table  
 *			  returns INVALID if name is undefined. Will return
 *		      	  pointer to the segment as a entry independent 
 *			  from the hash table
 */
static SEG_STATE_PTR XgksDeleteSeg (name)
Gint name;
{
	SEG_STATE_PTR  pre, cnt;

	if ( (pre = segtable[SHASH(name)]) == NULL )
		return (NULL);

	if (pre->segattr.seg == name) {
		segtable[SHASH(name)] = pre->seg_next;
		pre->seg_next = NULL;
		return (pre);
	}

	while ( (cnt=pre->seg_next) != NULL) {
		if (cnt->segattr.seg == name) {
			pre->seg_next = cnt->seg_next;
			cnt->seg_next = NULL;
			return(cnt);
		}
		pre = cnt;
	}
	return (NULL);
}
/*
 * XgksNewWsSeg()	allocate memory for workstation segment list return pointer to
 *		the entry
 */
static WS_SEG_LIST *XgksNewWsSeg()
{
	WS_SEG_LIST *new;

	if ((new = (WS_SEG_LIST *) malloc ((unsigned) sizeof(WS_SEG_LIST))) != NULL)
		new->next = NULL;
	return (new);
}
/*
 * XgksInsertWsSeg(ws, seg_id)	build a ws_seg structure insert it to the end of 
 * ws->seglist according to seg_id's priority.
 *
 * The ws->seglist is a priority queue ordered from lowest to highest priority.
 * The lowest possible priority is 0. The highest priority is 1.	c1032
 *
 */
static XgksInsertWsSeg (ws, seg_id)
WS_STATE_PTR ws;
Gint	seg_id;
{
	/*
	 * declare local functions
	 */
	static WS_SEG_LIST  *XgksNewWsSeg();	/*MIT*/

	if (ws->seglist == NULL) {
		ws->seg_insertpt = XgksNewWsSeg();
		ws->seglist = ws->seg_insertpt;
	} else {
		ws->seg_insertpt->next = XgksNewWsSeg();
		ws->seg_insertpt = ws->seg_insertpt->next;
	}
	ws->seg_insertpt->seg = seg_id;

	/* mess up of segment priority */
	ws->seg_list_dirty = TRUE;
}
/*
 *   XgksDeleteWsSeg (ws, seg_id) -  delete the entry in the list with sement name = name
 *			          returns : 
 *				     INVALID if undefined
 *			             else return the name of the deleted segment
 */
static XgksDeleteWsSeg (ws, seg_id)
WS_STATE_PTR ws;
Gint seg_id;
{
	WS_SEG_LIST  *cnt;

	cnt = ws->seglist;
	while (cnt != NULL ) {
		if (cnt->seg == seg_id) {
			cnt->seg = INVALID;
			return (seg_id);			/* c1073 */
		}
		cnt = cnt->next;
	}
	return (INVALID);
}
/*
 * XgksRenameWsSeg (ws, old, new) - rename the segment name in ws->seglist
 *
 */
static XgksRenameWsSeg (ws, old, new)
WS_STATE_PTR ws;
Gint old, new;
{
	WS_SEG_LIST *ptr;

	if (ws->ewstype == MO) return;				/* c1139 */

	ptr = ws->seglist;
	while (ptr != NULL) {
		if (ptr->seg == old) {
		   ptr->seg = new;
		   return;
		}
		ptr = ptr->next;
	}
}
/* 
 * XgksAppendSegPrimi (primi)
 *  OUT_PRIMI *primi	   append this entry to current open segment, built the input 
 *			   primitive into transformed ndc world, return pointer to the
 *			   transformed primitive
 *			
 * After appending, check current segment attribute and decide if this segment ..
 *   should show on output -- pointer
 * or maybe it's invisible -- NULL
 *
 */
OUT_PRIMI *XgksAppendSegPrimi (primi)
OUT_PRIMI *primi;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();		/*MIT*/
	static OUT_PRIMI    *XgksSegPrimiTran ();	/*MIT*/

    SEG_STATE_PTR seg;

    seg = XgksFindSeg(xgks_state.gks_open_seg);		/* c1147 */
    primi->pickid = xgks_state.gks_pick_id;		/* c1147 */
    XgksInsertPrimi (&(seg->primi_insert_pt), primi);
    XgksUpdatePrimiBound (primi, &(seg->bound));
    if (primi->pid == TEXT) seg->text_present = TRUE;

/* No need to calculate transformation if it will no show up */
    if (seg->segattr.vis == GINVISIBLE) return (NULL);   

    return (XgksSegPrimiTran(primi, seg->segattr.segtran));
}
/*
 * XgksIrgNec (old, new)  - decide if implicit regeneration is necessary
 *  Gsegattr *old, *new   depending on the values of:
 *
 *    ->segtran : if different return TRUE
 *    ->vis: if old != new     return TRUE
 *    ->pri: if different      return TRUE
 */
static XgksIrgNec (old, new)
Gsegattr *old, *new;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();	/*MIT*/

	Gint i;

	if (old->pri != new->pri) return (TRUE);

	if (old->vis != new->vis) return (TRUE);
			
	for (i=0; i<3; i++) 
	   if ( (old->segtran[0][i] != new->segtran[0][i]) || (old->segtran[1][i] != new->segtran[1][i]) )
		return(TRUE);

	return (FALSE);
}
/*
 * XgksDeleteAllSeg (ws)	delete all segments from the workstation 
 *
 */
XgksDeleteAllSeg (ws)
WS_STATE_PTR ws;
{
	Girgmode tmp;

	tmp = ws->wsdus.irgmode;
	ws->wsdus.irgmode = GSUPPRESSED;	/* Avoid redraw after each deletion */
	while (ws->seglist != NULL)  {
		ws->seg_insertpt = ws->seglist->next;
		if (ws->seglist->seg != INVALID) {
			XgksDelAssocWs (XgksFindSeg(ws->seglist->seg), ws->ws_id);
			free( (char *) (ws->seglist));
		}
		ws->seglist = ws->seg_insertpt;
	}
	ws->wsdus.irgmode = tmp;		/* restore the old irgmode */
	REDRAWWS(ws);
}

/* c1141:  This function added for MO's */
/*								 * c1141 *
 * XgksDelAllMoSeg( ws )        delete the MO ws from all assoc	 * c1141 *
 *                              seg				 * c1141 *
 * 								 * c1141 *
 * Note that MO ws have no segment list, so we have to search	 * c1141 *
 *    all seg							 * c1141 *
 *								 * c1141 *
 */								/* c1141 */
void XgksDelAllMoSeg( ws )					/* c1141 */
	WS_STATE_PTR ws;					/* c1141 */
{								/* c1141 */
	Gint i;							/* c1141 */
	SEG_STATE_PTR seg;					/* c1141 */
								/* c1141 */
	for (i = 0; i < SHSIZE; i++) {				/* c1141 */
		seg = segtable[i];				/* c1141 */
		while (seg != NULL) {				/* c1141 */
			XgksDelAssocWs( seg, ws->ws_id );	/* c1141 */
			seg = seg->seg_next;			/* c1141 */
		}						/* c1141 */
	}							/* c1141 */
}

/* 
 * XgksReDrawAssocWs (seg)	- generate redraw signal to all the ws assoc.
 *				  with seg->
 *
 *	NOTE : ... This  function will not redraw on WISS (that is if WISS, skip )
 *
 */
static XgksReDrawAssocWs(seg)
SEG_STATE_PTR seg;
{
	Gint i;
	WS_STATE_PTR ws;

	for (i=0; seg->assoc_ws[i]!=INVALID && i<MAX_ASSOC_WS; i++) {
		if (NOT_WISS(seg->assoc_ws[i])) {
/* DWO 7/26/88  changed macro name from VALID_WSID */
			ws = OPEN_WSID(seg->assoc_ws[i]);  /* c1012 */
			REDRAWWS(ws);
		}
	}
}
/*
 * XgksDrawSegToWs(ws) 		- draw out all ws assoc segments to ws
 *	
 *				  Routine will do UPDATE_SEG_CNT for every segment !!
 *				  Routine will also Disable and Enable before and after
 *					segment redraws
 */
static								/* c1147 */
void								/* c1147 */
XgksDrawSegToWs (ws)
WS_STATE_PTR ws;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();	/*MIT*/

	SEG_STATE_PTR seg;
	WS_SEG_LIST *seglist;

	XgksIDevDisable(ws);
	seglist = ws->seglist;
	while (seglist != NULL) {
	    if (seglist->seg != INVALID) {
	    	seg = XgksFindSeg(seglist->seg);
	    	UPDATE_SEG_CNT (ws->primi_insert_pt);
	    	if (seg->segattr.vis == GVISIBLE)  XgksReDrawSeg (ws, seg->segattr.seg);
	    }
	    seglist = seglist->next;
	}
	XgksIDevEnable(ws);
}
/*
 * XgksOutputSeg (ws, seg)	Output all primitive in the seg->primi_list to ws
 *
 */
static XgksOutputSeg (ws, seg)
WS_STATE_PTR ws;
SEG_STATE_PTR seg;
{
	/*
	 * declare local functions
	 */
	static OUT_PRIMI    *XgksSegPrimiTran ();	/*MIT*/

	OUT_PRIMI *primi;
	Glimit tmp_clip;

	if (seg->segattr.vis == GINVISIBLE) return;
	primi = &(seg->primi_list);
	tmp_clip = ws->clip;	/* save the current clip region */
	while (primi != NULL) {
		XgksDrawToWs (ws, XgksSegPrimiTran(primi, seg->segattr.segtran));
		primi = primi->next;
	}
	ws->clip = tmp_clip;	/* restore clip ws-clip region */
	xXgksUpdateClip(ws);						/* c1147 */
}
/*
 * XgksSegPrimiTran (primi, matrix) -- build a transformed ndc primitive structure
 *				     and return a pointer to the newly constructed primitive
 *
 */
static OUT_PRIMI *XgksSegPrimiTran (primi, matrix)
OUT_PRIMI *primi;
Gfloat matrix[2][3];
{
    Gint  cnt, num_pts;
    Gpoint *ndc_pt, up_pt, base_pt;
    static OUT_PRIMI trans_primi;	/* transformed primi */
    static Gpoint *trans_pt = NULL;
    static Gchar  *string = NULL;
    static Gint   *colour = NULL;

    trans_primi = *primi;

#define ALLOC_CHECK(c)	{\
	if (c) { gerrorhand(300, errXgksSegPrimiTran, xgks_state.gks_err_file); return (NULL); }\
			}

    if (trans_pt != NULL)  {
	free (trans_pt);
	trans_pt = NULL;
    }

    if (string != NULL) {
	free (string);
	string = NULL;
    }

    if (colour != NULL) {
	free (colour);
	colour = NULL;
    }

    switch (primi->pid) {
	case PLINE: 
		num_pts = trans_primi.primi.pline.num_pts = primi->primi.pline.num_pts;
		ALLOC_CHECK((trans_pt= (Gpoint *) malloc ( (unsigned) num_pts*sizeof(Gpoint)))==NULL);
		ndc_pt  = primi->primi.pline.pts;
    		for (cnt=0; cnt<num_pts; cnt++) {
       		    SegTrans(ndc_pt, &(trans_pt[cnt]), matrix);
		    ndc_pt++;
		}
		trans_primi.primi.pline.pts = trans_pt;
		break;
	case PMARK:
		num_pts = trans_primi.primi.pmark.num_pts = primi->primi.pmark.num_pts;
		ALLOC_CHECK((trans_pt= (Gpoint *) malloc ( (unsigned) num_pts*sizeof(Gpoint)))==NULL);
		ndc_pt  = primi->primi.pmark.location;
    		for (cnt=0; cnt<num_pts; cnt++)  {
       		    SegTrans(ndc_pt, &(trans_pt[cnt]), matrix);
		    ndc_pt++;
		}
		trans_primi.primi.pmark.location = trans_pt;
		break;
	case FILL_AREA:
		num_pts = trans_primi.primi.fill_area.num_pts = primi->primi.fill_area.num_pts;
		ALLOC_CHECK((trans_pt= (Gpoint *) malloc ( (unsigned) num_pts*sizeof(Gpoint)))==NULL);
		ndc_pt  = primi->primi.fill_area.pts;
    		for (cnt=0; cnt<num_pts; cnt++) {
       		    SegTrans(ndc_pt, &(trans_pt[cnt]), matrix);
		    ndc_pt++;
		}
		trans_primi.primi.fill_area.pts = trans_pt;
		break;
	case CLIP_REC  : 
		trans_primi.primi.clip.rec = primi->primi.clip.rec;
		trans_primi.primi.clip.segment = primi->primi.clip.segment;
		break;
	case TEXT      : 
		ALLOC_CHECK((trans_pt= (Gpoint *) malloc ( (unsigned) sizeof(Gpoint)))==NULL);
	   /* SegTran on starting location of string */
		ndc_pt  = primi->primi.text.location;
       		SegTrans(ndc_pt, trans_pt, matrix);
		trans_primi.primi.text.location = trans_pt;

		up_pt = primi->primi.text.up_vec;
		base_pt = primi->primi.text.base_vec;
	   /* SegTran on up/base vector of string, NO TRANSLATION can be done on vectors */
       		SegTrans(&up_pt, &(trans_primi.primi.text.up_vec), matrix);
		trans_primi.primi.text.up_vec.x -= matrix[0][2];
		trans_primi.primi.text.up_vec.y -= matrix[1][2];
       		SegTrans(&base_pt, &(trans_primi.primi.text.base_vec), matrix);
		trans_primi.primi.text.base_vec.x -= matrix[0][2];
		trans_primi.primi.text.base_vec.y -= matrix[1][2];

	        ALLOC_CHECK((string = (Gchar *)malloc ((unsigned) STRLEN(primi->primi.text.string) + 1))==NULL);	/* c1176 */
		STRCPY (string, primi->primi.text.string);	/* c1176 */
		trans_primi.primi.text.string = string;
		break;

	case CELL_ARRAY: 
		cnt = primi->primi.cell_array.rowsize * primi->primi.cell_array.dim.y;
		ALLOC_CHECK((colour=(Gint *)malloc( cnt * sizeof(Gint)))==NULL);
		cnt--;
		while (cnt>=0) {
			trans_primi.primi.cell_array.colour[cnt] = primi->primi.cell_array.colour[cnt];
			cnt--;
		}
		SegTrans(&(primi->primi.cell_array.ll), &(trans_primi.primi.cell_array.ll), matrix);
		SegTrans(&(primi->primi.cell_array.lr), &(trans_primi.primi.cell_array.lr), matrix);
		SegTrans(&(primi->primi.cell_array.ur), &(trans_primi.primi.cell_array.ur), matrix);
		SegTrans(&(primi->primi.cell_array.ul), &(trans_primi.primi.cell_array.ul), matrix);
		trans_primi.primi.cell_array.dim = primi->primi.cell_array.dim;
		trans_primi.primi.cell_array.rowsize = primi->primi.cell_array.rowsize;
		break;
	case GDP       : break;
    }                                        
    return (&trans_primi);

}

/*
 * XgksAppendSegClip () -- check if current gks_open_seg->primi_insertpt->pid == CLIP_REC
 *			 if yes, change it's current value to clip->rec else
 *			 build one with value = clip->rec append to list
 */
XgksAppendSegClip ()
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();	/*MIT*/

	SEG_STATE_PTR seg;
	OUT_PRIMI *clip;

	seg = XgksFindSeg(xgks_state.gks_open_seg);		/* c1147 */
	if (seg->primi_insert_pt->pid == CLIP_REC) {
		seg->primi_insert_pt->primi.clip.rec = xgks_state.cliprec.rec; /*c1147*/
		return;
	} else { 
		clip = XgksNewPrimi();
		clip->pid = CLIP_REC;
		clip->primi.clip.segment = TRUE;
		clip->primi.clip.rec = xgks_state.cliprec.rec;		/*c1147*/
    		XgksInsertPrimi (&(seg->primi_insert_pt), clip);
	}
}
/*
 * XgksSetHighLight (ws, seg) -- setting/unsetting segment highlight on specified 
 * 				 workstations
 *
 */
static XgksSetHighLight (ws, seg)
WS_STATE_PTR ws;
SEG_STATE_PTR seg;
{
	/* declare local functions */				/* c1147 */
	static void XgksProcessLocalBound ();			/* c1147 */

	Gint i;
	Gpoint ndc[4], trans[5];
	Gfloat xfact, yfact;
	Glimit localbound;

	XgksProcessLocalBound(ws, seg, &localbound);	

/* figuring out the stretching factor for the bounding box */
	xfact = 0.01 * (ws->wsti.current.w.xmax - ws->wsti.current.w.xmin);
	yfact = 0.01 * (ws->wsti.current.w.ymax - ws->wsti.current.w.ymin);
	
/* now ndc values are stretched bounding box */
	ndc[0].x = localbound.xmin - xfact;
	ndc[0].y = localbound.ymin - yfact;
	ndc[1].x = localbound.xmin - xfact;
	ndc[1].y = localbound.ymax + yfact;
	ndc[2].x = localbound.xmax + xfact;
	ndc[2].y = localbound.ymax + yfact;
	ndc[3].x = localbound.xmax + xfact;
	ndc[3].y = localbound.ymin - yfact;

/* pass the bounding box through segtran */
	for (i=0; i<4; i++)
   	     SegTrans (&(ndc[i]), &(trans[i]), seg->segattr.segtran);
	trans[4] = trans[0];	/* for xpolyline to do drawing */

	XgksIDevDisable (ws);
	xXgksHighLight (ws, trans);					/* c1147 */
	XgksIDevEnable (ws);
}
/*
 * XgksSegdump() - ....
 */
#ifdef SEGMENTDEBUG						/* c1147 */
XgksSegDump (seg)
SEG_STATE_PTR seg;
{
	fprintf(stderr,"name=%d, vis=%d, hi=%d, det=%d, pri=%f\n", seg->segattr.seg, seg->segattr.vis,
		 seg->segattr.hilight, seg->segattr.det, seg->segattr.pri);
	fprintf(stderr,"trans= %f %f %f \n", seg->segattr.segtran[0][0],
		 seg->segattr.segtran[0][1],seg->segattr.segtran[0][2]);
	fprintf(stderr,"       %f %f %f \n", seg->segattr.segtran[1][0],
		 seg->segattr.segtran[1][1],seg->segattr.segtran[1][2]);
}
#endif								/* c1147 */
/*
 * XgksDelAssocWs (seg, ws_id) 	- delete ws_id from segment state return OK if
 *		 	          successful, else INVALID, will free the segment
 *				  state list if after deletion, there's no more ws
 *				  assoc with the segment
 *
 *  Will redraw the workstation if necessary 
 */
static XgksDelAssocWs (seg, ws_id)
SEG_STATE_PTR seg;
Gint ws_id;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksDeleteSeg ();		/*MIT*/

	Gint i;
/* First find the segment in the assoc_ws array */
	for(i=0; seg->assoc_ws[i] != ws_id && seg->assoc_ws[i] != INVALID; i++)
		continue; 
	/* not on this workstation*/
	if (seg->assoc_ws[i] == INVALID) return (INVALID);

/* Remove this workstation by squeeze the assoc_ws array */
	while( i < MAX_ASSOC_WS) {
		if (i == (MAX_ASSOC_WS-1)) {
			seg->assoc_ws[i] = INVALID; 
			break;
		}
		if ((seg->assoc_ws[i] = seg->assoc_ws[i+1]) == INVALID) break;  
		i++;
	}

/* if no more workstation associated with the segment delete it from data structures */
	if (seg->assoc_ws[0] == INVALID) {
		seg = XgksDeleteSeg (seg->segattr.seg); 
		free ((char *)seg);
	} 
	return (0);
}
/*
 * XgksClearWs (ws)   -- Clear the workstation WITHOUT any checking, and WILL DELETE 
 *		         all non-segment primitives.
 *			 Will also unpend_pending_transformations.
 */
static								/* c1147 */
void								/* c1147 */
XgksClearWs (ws)
WS_STATE_PTR ws;
{
	/* declare local functions */				/* c1147 */
	void XgksCleanUpWsSegList ();			/* c1147 */

/* Free up ALL non-segment primitive associcated with this ws */
	XgksDeletePrimi (&(ws->primi_list), &(ws->primi_insert_pt)); 

/* Clear message ptr    PTR# c1001 */
        ws->message_pt = NULL;

/* Now delete All the ws->seglist with name = INVALID */
	XgksCleanUpWsSegList (ws);

/* if display surface is not empty, clear display */
	if (ws->wsdus.dspsurf == GNOTEMPTY) xXgksClearWs(ws);		/* c1147 */

/* Display now empty, and no new fram on display */
	ws->wsdus.dspsurf = GEMPTY;
	ws->wsdus.nframe = GNO;

/* unpend pending transformations */
	if (ws->wsti.wstus == GPENDING) XgksUnpendPendingTrans(ws);

	return;							/* c1147 */
}
/*
 * XgksUpdateWsSegList (wsg)  -- re-insert the whole ws->seglist according to 
 *				 each segment's priority
 *		
 *	Reorder list from low priority to high priority
 *
 *	The way priority works:
 *
 * 	During redraw ... just redraw blindly from the front, BIGGEST will 
 *	get drawn last, thus highest prority.
 */

static								/* c1147 */
void								/* c1147 */
XgksUpdateWsSegList (ws)
WS_STATE_PTR ws;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();		/*MIT*/
	void XgksCleanUpWsSegList ();		/* c1147 */

	WS_SEG_LIST *old, *cnt, *pre;
	SEG_STATE_PTR workSeg, oldSeg;

	/* segment list is clean again */
	ws->seg_list_dirty = FALSE;

	/* First clean up the list by deleting all segment with INVALID name */
	XgksCleanUpWsSegList (ws);

	/* Do the rearrangment by construct the list */

	/* if there's only one segment in the list 
	   then we are done */
	if (ws->seglist == ws->seg_insertpt) return;

	old = ws->seglist->next;
	ws->seg_insertpt = ws->seglist;
	ws->seg_insertpt->next = NULL;

	while (old != NULL) {
		oldSeg  = XgksFindSeg (old->seg);
		cnt = ws->seglist;	
		workSeg = XgksFindSeg (cnt->seg);
		pre = NULL;
		while ((workSeg->segattr.pri <= oldSeg->segattr.pri) && (cnt != NULL)) { /*c1032*/ /*c1080*/
			pre = cnt;
			cnt = cnt->next;
			if (cnt != NULL) workSeg = XgksFindSeg (cnt->seg);
			else break;
		}
		if (cnt == NULL) { 		/* At the end of the ws->seglist */
			pre->next = old;
			ws->seg_insertpt = old;
			old = old->next;
			ws->seg_insertpt->next = NULL;
		} else if (pre == NULL) {	/* Begingin of the ws->seglist */
			ws->seglist = old;
			old = old->next;
			ws->seglist->next = cnt;
		} else {
			pre->next = old;
			old = old->next;
			pre->next->next = cnt;
		}
	}
}
/*
 * XgksReDrawSeg(ws, seg_id)     -- This is the x-initiated re-draw segment path, this 
 *				function should only be called be re-draw initialted 
 *				
 */
XgksReDrawSeg(ws, seg_id)
WS_STATE_PTR ws;
Gint seg_id;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();		/*MIT*/
	static OUT_PRIMI    *XgksSegPrimiTran ();	/*MIT*/

	SEG_STATE_PTR seg;
	OUT_PRIMI *primi;
	Glimit tmp_clip;

	if (seg_id != INVALID) {

		seg = XgksFindSeg (seg_id);
		if (seg->segattr.vis == GINVISIBLE) return;
		primi = &(seg->primi_list);
		tmp_clip = ws->clip;	/* save the current clip region */
		while (primi != NULL) {
			XgksReDrawWs (ws, XgksSegPrimiTran(primi, seg->segattr.segtran));
			primi = primi->next;
		}
		ws->clip = tmp_clip;	/* restore clip ws-clip region */
		xXgksUpdateClip(ws);					/* c1147 */
		if (seg->segattr.hilight == GHIGHLIGHTED) XgksSetHighLight(ws, seg);
	}
}

/*
 * float XgksInFill(seg, pt, pe, m) - distance between point *pt and fillarea *pe.
 * SEG_STATE_PTR seg;		segment state list pointer.
 * Gpoint *pe, *pt;		point and fillarea pointers.
 * Gint m;			point number of fillarea *pe.
 */
static float
XgksInFill(seg, pt, pe, m)
	SEG_STATE_PTR seg;
	Gpoint *pe, *pt;
	Gint m;
{
	float s, s1, s2, x, y, X, Y, x1, y1, x2, y2;
	float DX, DY, DY1, DY2, DD, DD1, DD2, DD3, DD4, mini, dist;
	Gpoint *ppe, p;
	Gint i, num;

	num = 0;
	mini = MAXNUM;
	ppe = pe;
	ppe++;
	x = pt->x;  	y = pt->y;
	for (i=0; i < m-1; i++, pe++, ppe++) {
		SegTrans( pe, &p, seg->segattr.segtran );
		x1 = p.x;
		y1 = p.y;
		SegTrans( ppe, &p, seg->segattr.segtran );
		x2 = p.x;
		y2 = p.y;
		if (y1 == y2) { X = x; Y = y1; DD4 = MAXNUM; }
		else if (x1 == x2) { X = x1; Y = y; DD4 = x1; }
		  else { s = (y1-y2)/(x1-x2);
			 s1 = 1/s;
			 s2 = s*s + 1.0;
			 X = (s*x1-y1+y+s1*x)*s/s2;
			 Y = (y1-y-s*x1-s1*x)/s2 + y + s1*x;
			 DD4 = x1+(y-y1)*s1;
		  }
		DX = (float)sqrt( (double)( (X-x)*(X-x) + (Y-y)*(Y-y) ) );
		DY = (float)sqrt( (double)( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) ) );
		DY1 = (float)sqrt( (double)( (X-x1)*(X-x1) + (Y-y1)*(Y-y1) ) );
		DY2 = (float)sqrt( (double)( (X-x2)*(X-x2) + (Y-y2)*(Y-y2) ) );
		dist = ( DY1 < DY2 ) ? DY1 : DY2 ;
		DD = DY1 + DY2;
		dist = ( DY >= DD ) ? DX : ( DX+dist ) ;
		DD1 = fabs(y1-y2);	DD2 = fabs(y-y1)+fabs(y-y2);
		DD3 = x;
		num = ( DD1 >= DD2 && DD3 >= DD4 ) ? (num+1) : num ;
		mini = ( dist < mini ) ? dist : mini ; 
	}
	if ( num%2 != 0 ) return(0.0);
	return(mini);
}

/*
 * XgksShowPick -- bound or unbound the picked segment for 1 second
 */
XgksShowPick(ws, seg)
WS_STATE_PTR ws;
SEG_STATE_PTR seg;
{
        /*
         * declare local functions
         */
        static XgksSetHighLight ();		/*c1032*/

            XgksSetHighLight (ws, seg);
            sleep(1);
            XgksSetHighLight (ws, seg);
}

/* 
 * Utility function added for ptr c1032.
 * Function will return 1 if pt is in the bound pointed to by pe.
 * 
 * int XgksInSeg(seg, pt, pe, m) - is point *pt in segment *pe.
 * SEG_STATE_PTR seg;           segment state list pointer.
 * Gpoint *pe, *pt;             point and bound pointers.
 * Gint m;                      number of points in segment bound 
 */
static int
XgksInSeg(seg, pt, pe, m)
        SEG_STATE_PTR seg;
        Gpoint *pe, *pt;
        Gint m;
{
        Gpoint limits[4];
        Gint i;

	/* transform points to NDC */
        for (i=0; i < m; i++, pe++)
         SegTrans( pe, &limits[i], seg->segattr.segtran );   

	/* test if pt is within the segment bound  + or - 0.01 c1170  */
        if (( pt->x >= limits[0].x - 0.01  && pt->x >= limits[3].x - 0.01 ) && /* c1170 */
            ( pt->x <= limits[1].x + 0.01  && pt->x <= limits[2].x + 0.01 ) && /* c1170 */
            ( pt->y <= limits[3].y + 0.01  && pt->y <= limits[2].y + 0.01 ) && /* c1170 */
            ( pt->y >= limits[0].y - 0.01  && pt->y >= limits[1].y - 0.01 ))   /* c1170 */
          return(1);
        else
          return(0);
}

/*
 * XgksFindPickSeg - based on the value of ndc-point, locate a segment
 *		    
 * returns GP_OK     -- for pick_ok
 *	   GP_NOPICK -- for nothing is picked
 *	   GP_NONE   -- for what ?
 *
 *
 *        BIGGER->BIGGER->SAME->SAME->SMALLER->SAME->SMALLEST=
 *
 * 	During redraw ... just redraw blindly from the front, SMALLEST will 
 *	get drawn last, thus highest priority.
 *	
 *	The way pick works, ... 
 *
 *	return_seg_id <- INVALID
 *	While (not end of ws_assoc_seg_list)
 *	  if (within or equal bound box) set return_seg_id
 *
 *	if seg_id == INVALID return GP_NOPICK
 *	else pick_id <- first non_clip primitive pick_id
 *
 *      10/28/88 TRG   PTR c1032
 *
 *      This function was modified to improve performance.
 *      The findpickid is used as a flag to prevent unnecessary
 * 	calculation of the pickid. If the device is in sample
 * 	mode the pickid is not calculated on every click. It 
 *	is calculated only when an actual sample is done.
 *
 */
Gpstat XgksFindPickSeg (ws, ndcpt, response, idev, findpickid)	/*c1032*/
WS_STATE_PTR ws;
Gpoint *ndcpt;
Gpick *response;
INPUT_DEV *idev;
Gint findpickid;
{
	/*
	 * declare local functions
	 */
	static int XgksInSeg();                 /*MIT*/	/*c1032 */
	static float XgksFindDistance();	/*MIT*/
	static SEG_STATE_PTR XgksFindSeg ();	/*MIT*/
	static void XgksProcessLocalBound ();	/* c1147 */	

	SEG_STATE_PTR seg, foundseg = NULL;	/*c1032*/ /* New 4.3 Compiler */
	WS_SEG_LIST *WsSeg=ws->seglist;
	Gpoint segpt[5];
	Gfloat highest;				/*c1032*/
	Glimit localbound;
	Gint pickid;

	highest = -1;				/*c1032*/
	response->seg = INVALID;
	response->pickid = INVALID;
	while (WsSeg != NULL) {
		seg = XgksFindSeg(WsSeg->seg);
		if (seg->segattr.det == GDETECTABLE && seg->segattr.vis==GVISIBLE && seg->segattr.pri >= highest) {  /*c1032*/
                        XgksProcessLocalBound (ws, seg, &localbound);  /* if text is present up date bound  c1032 */
			segpt[0].x = localbound.xmin; segpt[0].y = localbound.ymin;  
			segpt[1].x = localbound.xmax; segpt[1].y = localbound.ymin;  
			segpt[2].x = localbound.xmax; segpt[2].y = localbound.ymax;  
			segpt[3].x = localbound.xmin; segpt[3].y = localbound.ymax;  
			segpt[4].x = localbound.xmin; segpt[4].y = localbound.ymin;  

			if ( XgksInSeg(seg, ndcpt, &segpt[0], 4)) {	/*c1032*/
                                highest = seg->segattr.pri;		/*c1032*/
                                foundseg = seg;				/*c1032*/
			        response->seg = WsSeg->seg;		
			}
		}
		WsSeg = WsSeg->next;
	} /* End While */

/* If no segment is satisfied just return NO_PICK */
	if (response->seg == INVALID) return ((response->status=GP_NOPICK));
	idev->active = FALSE;  /* prevent another pick from interrupting  c1032*/
        /* if findpickid == 2 then the segment has already been 
	   highlighted so don't rehighlight just find the pickid.
	   This only happens in sample mode.           		          c1032*/
        if ((findpickid != 2) && (idev->data.pic.initst.esw == GECHO))
		XgksShowPick(ws, foundseg);				/*c1032*/

	if (findpickid)  /* is it necessary to find the pick id ? */	/*c1032*/
         {
          XgksFindDistance(ws, foundseg, &pickid, ndcpt);  /* Find Pick Id  c1032*/
          response->pickid = pickid;					/*c1032*/
         }
        idev->active = TRUE;    /* allow more picks */			/*c1032*/
	
	return ((response->status=GP_OK));
}
/*
 * XgksProcessLocalBound -- if there's text-primitive in segment than update seg->bound into
 * 			    localbound, else simply return seg->bound
 */
static								/* c1147 */
void								/* c1147 */
XgksProcessLocalBound (ws, seg, localbound)
WS_STATE_PTR ws;
SEG_STATE_PTR seg;
Glimit *localbound;
{
	OUT_PRIMI *primi;
	Gpoint ndc_points[5];

	*localbound = seg->bound;

	if (seg->text_present == FALSE) return;
	
	primi = seg->primi_list.next;
	while (primi != NULL) {
		if (primi->pid == TEXT) {
			xXgksInqTextExtent (ws, &(primi->primi), ndc_points); /* c1147 */
			XgksMiniMax (localbound, &(ndc_points[1]));
			XgksMiniMax (localbound, &(ndc_points[2]));
			XgksMiniMax (localbound, &(ndc_points[3]));
			XgksMiniMax (localbound, &(ndc_points[4]));
		}
		primi = primi->next;
	}
}

/*
 * XgksFindDistance(ws, seg, pid, pt) - find the distance between the pick input point and the segment
 * WS_STATE_PTR ws;		workstation list pointer.
 * SEG_STATE_PTR seg;		segment idetify.
 * Gint *pid;			pick identifier of the picked primitive.
 * Gpoint *pt;			pick input point.
 */
static float
XgksFindDistance(ws, seg, pid, pt)
	WS_STATE_PTR ws;
	SEG_STATE_PTR seg;
	Gint *pid;
	Gpoint *pt;
{
	/*
	 * declare local functions
	 */
	static float XgksDistFillarea();	/*MIT*/
	static float XgksDistCellarray();	/*MIT*/
	static float XgksDistPline();		/*MIT*/
	static float XgksDistPmark();		/*MIT*/
	static float XgksDistText();		/*MIT*/

	OUT_PRIMI *primi;
	float dist_primi, mini_primi;


	primi = &(seg->primi_list);
	
	mini_primi = MAXNUM;
	while(primi != NULL) {
		dist_primi = MAXNUM;
		switch(primi->pid) {
		case PLINE	: dist_primi = XgksDistPline(ws, seg, primi, pt);
#ifdef SEGMENTDEBUG
printf("seg= %d, line dist= %f\n",seg->segattr.seg,dist_primi);
#endif
				  break;
		case PMARK	: dist_primi = XgksDistPmark(ws, seg, primi, pt);
#ifdef SEGMENTDEBUG
printf("seg= %d, mark dist= %f\n",seg->segattr.seg,dist_primi);
#endif
				  break;
		case TEXT	: dist_primi = XgksDistText(ws, seg, primi, pt);
#ifdef SEGMENTDEBUG
printf("seg= %d, text dist= %f\n",seg->segattr.seg,dist_primi);
#endif
				  break;
		case FILL_AREA	: dist_primi = XgksDistFillarea(seg, primi, pt); /*MIT*/
#ifdef SEGMENTDEBUG
printf("seg= %d, fill dist= %f\n",seg->segattr.seg,dist_primi);
#endif
				  break;
		case CELL_ARRAY	: dist_primi = XgksDistCellarray(seg, primi, pt);
#ifdef SEGMENTDEBUG
printf("seg= %d, cellarray dist= %f\n",seg->segattr.seg,dist_primi);
#endif
				  break;
		case GDP	: /* gdp */
		case CLIP_REC	: 
		case MESG	: 
		default		: break;
		}
		if (dist_primi < mini_primi) { mini_primi = dist_primi; *pid = primi->pickid; } 
		if ( mini_primi == 0.0 ) return(0.0);
		primi = primi->next;	
	}
	return(mini_primi);
}

/*
 * float XgksDistPline(ws, seg, primi, pt) - Find the distance between pick input point and a polyline primitive
 * WS_STATE_PTR ws;		workstation state list pointer.
 * SEG_STATE_PTR seg;		segment state list pointer.
 * OUT_PRIMI *primi;		output primitive pointer.
 * Gpoint *pt;			pick input point.
 *
 */

static float
XgksDistPline(ws, seg, primi, pt)
	WS_STATE_PTR ws;
	SEG_STATE_PTR seg;
	OUT_PRIMI *primi;
	Gpoint *pt;
{
	PLINE_ST *line;
	float s, s1, s2, x, y, X, Y, x1, y1, x2, y2;
	float DX, DY, DY1, DY2, mini, dist, width;
#ifdef notdef				/* c1179 */  
        float DD;                  /* AUG */
#endif
	Gpoint pe, p;
	Gint i;

	XPoint xpe;


	mini = (float)MAXNUM;
	line = &(primi->primi.pline);
	x = pt->x;
	y = pt->y;
	for (i=0; i < line->num_pts-1; i++) {
		SegTrans( &(line->pts[i]), &p, seg->segattr.segtran);
		x1 = p.x;
		y1 = p.y;
		SegTrans( &(line->pts[i+1]), &p, seg->segattr.segtran);
		x2 = p.x;
		y2 = p.y;
		if (y1 == y2) { X = x; Y = y1; }	/* Horizontal line  AUG */
		else if (x1 == x2) { X = x1; Y = y; }	/* Vertical line    AUG */
		  else { s = (y1-y2)/(x1-x2);
			 s1 = 1/s;
			 s2 = s*s + 1.0;
			 X = (s*x1-y1+y+s1*x)*s/s2;
			 Y = (y1-y-s*x1-s1*x)/s2 + y + s1*x;
		  }
/* The line (x,y)(X,Y) is perpendicular to (x1,y1)(x2,y2)		AUG
 * and intersects (x1,y1)(x2,y2) at (X,Y)				AUG
 * Note: (X,Y) is not necessarily between (x1,y1) and (x2,y2)		AUG
 */
/* DX = Perpendicular distance   AUG */
		DX = (float)sqrt( (double)( (X-x)*(X-x) + (Y-y)*(Y-y) ) );

/* DY = line length 		 AUG */
		DY = (float)sqrt( (double)( (x1-x2)*(x1-x2) + (y1-y2)*(y1-y2) ) );

/* DY1 = distance from (X,Y) to (x1,y1) endpoint    AUG */
		DY1 = (float)sqrt( (double)( (X-x1)*(X-x1) + (Y-y1)*(Y-y1) ) );

/* DY2 = distance from (X,Y) to (x2,y2) endpoint    AUG */
		DY2 = (float)sqrt( (double)( (X-x2)*(X-x2) + (Y-y2)*(Y-y2) ) );

		if ( (dist = DY1+DY2) > DY )	/* (X,Y) is not between (x1,y1)(x2,y2)  AUG*/
			dist = sqrt( (DX+dist)*(DX+dist) );	/* AUG */
		else {						/* AUG */
			pe.x = DX;
			pe.y = 0.0;
			NdcToX(ws, &pe, &xpe);
			if (xpe.x < 0) xpe.x = -xpe.x;
			width = ( line->plnattr.width == GINDIVIDUAL ) ?
				(Gint)( line->plnattr.bundl.width) :
				(Gint)( ws->lnbundl_table[line->plnattr.line].width ) ; 
			width = width/2;
			dist = ( width >= xpe.x ) ? 0.0 : DX ;
		}
			
#ifdef notdef		/* AUG  c1179 */
/* minimum distance of intersect to endpoint */
		dist = ( DY1 < DY2 ) ? DY1 : DY2 ;
		DD = DY1 + DY2;
		dist = ( DY >= DD ) ? DX : sqrt( (DX+dist)*(DX+dist) ) ;
		if ( DX == dist ) {
			pe.x = DX;
			pe.y = 0.0;
			NdcToX(ws, &pe, &xpe);
			if (xpe.x < 0) xpe.x = -xpe.x;
			width = ( line->plnattr.width == GINDIVIDUAL ) ?
				(Gint)( line->plnattr.bundl.width) :
				(Gint)( ws->lnbundl_table[line->plnattr.line].width ) ; 
			width = width/2;
			mini = ( width >= xpe.x ) ? 0.0 : mini ;
		}
#endif	/* AUG */
		mini = ( dist < mini ) ? dist : mini ;	/* AUG */
		if ( mini == 0.0 ) return(0.0);
	}
	return(mini);
}
	
/*
 * float XgksDistFillarea(seg, primi, pt) - Find the distance between pick input point and fillarea primitive
 * SEG_STATE_PTR seg;		segment state list pointer.
 * OUT_PRIMI *primi;		output primitive pointer.
 * Gpoint *pt;			pick input point.
 *
 */

static float
XgksDistFillarea(seg, primi, pt)	/*MIT*/
	SEG_STATE_PTR seg;
	OUT_PRIMI *primi;
	Gpoint *pt;
{
	/*
	 * declare local functions
	 */
	static float XgksInFill();	/*MIT*/

	FILL_AREA_ST *fill;
	float mini;
	Gpoint *pe;
	Gint i, n;


	fill = &(primi->primi.fill_area);
	pe = (Gpoint *) malloc(sizeof(fill->pts[0]) * (fill->num_pts+1));
	GKSERROR( (pe == NULL), 300, errXgksDistFillarea );
	n = fill->num_pts;
	for (i=0; i < n; i++) *(pe+i) = fill->pts[i];
	if (fill->pts[0].x != fill->pts[n-1].x || fill->pts[0].y != fill->pts[n-1].y) {	
		*(pe+n) = *pe;		 n += 1;
	}
	mini = XgksInFill(seg, pt, pe, n);
	free(pe);
	return(mini);
}
/*
 * float XgksDistPmark(ws, seg, primi, pt) - Find the distance between pick input point and polymarker primitive
 * WS_STATE_PTR ws;		workstation state list pointer.
 * SEG_STATE_PTR seg;		segment state list pointer.
 * OUT_PRIMI *primi;		output primitive pointer.
 * Gpoint *pt;			pick input point.
 *
 */

static float
XgksDistPmark(ws, seg, primi, pt)
	WS_STATE_PTR ws;
	SEG_STATE_PTR seg;
	OUT_PRIMI *primi;
	Gpoint *pt;
{
	PMARK_ST *mark;
	float x, y, x1, y1;
	float mini, dist;
	Gpoint pe, p;
	Gint i, size;

	XPoint xpe;

	mini = MAXNUM;
	mark = &(primi->primi.pmark);
	x = pt->x;
	y = pt->y;
	for (i=0; i < mark->num_pts; i++) {
		SegTrans( &(mark->location[i]), &p, seg->segattr.segtran );
		x1 = p.x;
		y1 = p.y;
		dist = (float)sqrt( (double)( (x1-x)*(x1-x) + (y1-y)*(y1-y) ) );
		mini = ( dist < mini ) ? dist : mini ; 
		pe.x = dist;
		pe.y = 0.0;
		NdcToX(ws, &pe, &xpe);
		size = ( mark->mkattr.size == GINDIVIDUAL ) ?
			(Gint)( mark->mkattr.bundl.size) :
			(Gint)( ws->mkbundl_table[mark->mkattr.mark].size ) ; 
		mini = ( size >= xpe.x ) ? 0.0 : mini ;
		if ( mini == 0.0 ) return(0.0);
	}
	return(mini);
}
/*
 * float XgksDistText(ws, seg, primi, pt) - Find the distance between pick input point and text primitive
 * WS_STATE_PTR ws;		workstation state list pointer.
 * SEG_STATE_PTR seg;		segment state list pointer.
 * OUT_PRIMI *primi;		output primitive pointer.
 * Gpoint *pt;			pick input point.
 *
 */

static float
XgksDistText(ws, seg, primi, pt)
	WS_STATE_PTR ws;
	SEG_STATE_PTR seg;
	OUT_PRIMI *primi;
	Gpoint *pt;
{
	/*
	 * declare local functions
	 */
	static float XgksInFill();	/*MIT*/

	Gpoint points[5], ptn[5];
	Gfloat mini;


	xXgksInqTextExtent(ws, &(primi->primi), points);		/* c1147 */
	ptn[0] = points[1];	ptn[1] = points[2];
	ptn[2] = points[3];	ptn[3] = points[4];
	ptn[4] = points[1];
	mini = XgksInFill(seg, pt, &ptn[0], 5);
	return(mini);
}
/*
 * float XgksDistCellarray(seg, primi, pt) - Find the distance between pick input point and fillarea primitive
 * SEG_STATE_PTR seg;		segment state list pointer.
 * OUT_PRIMI *primi;		output primitive pointer.
 * Gpoint *pt;			pick input point.
 *
 */

static float
XgksDistCellarray(seg, primi, pt)
	SEG_STATE_PTR seg;
	OUT_PRIMI *primi;
	Gpoint *pt;
{
	/*
	 * declare local functions
	 */
	static float XgksInFill();		/*MIT*/

	CELL_ARRAY_ST *cell;
	float mini;
	Gpoint *pe;

	cell = &(primi->primi.cell_array);
	pe = (Gpoint *) malloc(sizeof(Gpoint) * 5);
	GKSERROR( (pe == NULL), 300, errXgksDistCellarray );
	*(pe) = cell->ll;	 *(pe+1) = cell->lr;
	*(pe+2) = cell->ur;	 *(pe+3) = cell->ul;
	*(pe+4) = cell->ll;
	mini = XgksInFill(seg, pt, pe, 5);
	free(pe);
	return(mini);
}
/*
 * Do gredrawsegws() - with no error checking and no MO output
 *
 */
XgksReDrawSegWs (ws)
WS_STATE_PTR ws;
{
	/* declare local functions */				/* c1147 */
	static void XgksDrawSegToWs ();				/* c1147 */
	static void XgksClearWs ();				/* c1147 */
	static void XgksUpdateWsSegList ();			/* c1147 */

	XgksIDevDisable( ws );

	XgksClearWs(ws);
	/* Now workstaiton had been cleared, update segment list 
	   (according to priority) and redraw all segments */
	if (ws->seg_list_dirty == TRUE) XgksUpdateWsSegList (ws);
	XgksDrawSegToWs (ws);	/* Now redraw all segments */

	XgksIDevEnable( ws );
}

static								/* c1147 */
void								/* c1147 */
XgksCheckSegAttr (old, new)
Gsegattr *old, *new;
{
	Gint tranDiff, i;

	tranDiff = FALSE;

	for (i=0; i<3; i++)  
	   if ( (old->segtran[0][i] != new->segtran[0][i]) || (old->segtran[1][i] != new->segtran[1][i]) ) {
		tranDiff = TRUE;
		i=3;
	   }
	if (tranDiff == TRUE) XgksMoSetSegTrans (old->seg, new->segtran);

	if (old->vis != new->vis) XgksMoSetSegVis (old->seg, new->vis);

	if (old->hilight != new->hilight) XgksMoSetSegHiLight (old->seg, new->hilight);
			
	if (old->pri != new->pri) XgksMoSetSegPri (old->seg, new->pri);

	if (old->det != new->det) XgksMoSetSegDet (old->seg, new->det);
}

void								/* c1147 */
XgksCleanUpWsSegList (ws)
WS_STATE_PTR ws;
{
	WS_SEG_LIST	*cnt, *pre;

	cnt = pre = ws->seglist;
	while (cnt != NULL) {
		if (cnt->seg == INVALID) {
			if (cnt == pre) {
				cnt = cnt->next;
				/* c1073:  change ws seg ptr if deleting */
				/*    head of list                       */
				if (pre == ws->seglist) {	/* c1073 */
					ws->seglist = cnt;	/* c1073 */
				}				/* c1073 */

				/* if we just deleted the segment */ /* c1165 */
                                /* pointed to by seg_insertpt,    */ /* c1165 */
                                /* then, we better reset the      */ /* c1165 */
                                /* insert point.     (DWO)        */ /* c1165 */
                                if (pre == ws->seg_insertpt)         /* c1165 */
                                        ws->seg_insertpt = cnt;      /* c1165 */

				free (pre);
				pre = cnt;
			} else {
				pre->next = cnt->next;

				/* if we just deleted the segment */ /* c1165 */
                                /* pointed to by seg_insertpt,    */ /* c1165 */
                                /* then, we better reset the      */ /* c1165 */
                                /* insert point.     (DWO)        */ /* c1165 */
                                if (cnt == ws->seg_insertpt)         /* c1165 */
                                        ws->seg_insertpt = pre;      /* c1165 */

				free (cnt);
				cnt = pre->next;
			}
		} else {
			pre = cnt;
			cnt = cnt->next;
		}
	}
}

static								/* c1147 */
void								/* c1147 */
XgksSegTransProcessMo (seg, matrix)
SEG_STATE_PTR seg;
Gfloat matrix[2][3];
{
	/*
	 * declare local functions
	 */
	static OUT_PRIMI    *XgksSegPrimiTran ();	/*MIT*/
	static void XgksOutPrimiToMo ();		/* c1147 */

	Gint   wscnt;             /* c1144  */
	OUT_PRIMI *primi;

	for (wscnt=0; wscnt<MAX_ACTIVE_WS; wscnt++) {
		if ( (xgks_state.activews[wscnt].ws_id != INVALID) && (xgks_state.activews[wscnt].ws->ewstype == MO) )  {			/* c1147 */
			XgksMoSetAsfOnWs (xgks_state.activews[wscnt].ws); /* c1144 */ /* c1147 */
			primi = seg->primi_list.next;
			while (primi != NULL) {
				XgksOutPrimiToMo (xgks_state.activews[wscnt].ws, XgksSegPrimiTran(primi, matrix)); /* c1147 */
				primi = primi->next;
			}
		}
	}
	XgksMoSetClip (&xgks_state.cliprec.rec);      /*  c1144   */ /*c1147*/
}

static								/* c1147 */
void								/* c1147 */
XgksSegCopyMo (ws, seg)
WS_STATE_PTR ws;
SEG_STATE_PTR seg;
{
	/*
	 * declare local functions
	 */
	static OUT_PRIMI    *XgksSegPrimiTran ();	/*MIT*/
	static void XgksOutPrimiToMo ();		/* c1147 */

	OUT_PRIMI *primi = seg->primi_list.next;

	XgksMoSetPatRefOnWs (ws);   /*  PTR# c1144  */
	XgksMoSetPatSizeOnWs (ws);  /*  PTR# c1144  */
	XgksMoSetAsfOnWs (ws);
	while (primi != NULL) {
		XgksOutPrimiToMo (ws, XgksSegPrimiTran(primi, seg->segattr.segtran));
		primi = primi->next;
	}
	XgksMoSetClipOnWs (ws, &xgks_state.cliprec.rec); /*MIT*/ /* c1137 */ /*c1147*/
}

static								/* c1147 */
void								/* c1147 */
XgksSegProcessMo (ws, seg)
WS_STATE_PTR ws;
SEG_STATE_PTR seg;
{
	/* declare local functions */				/* c1147 */
	static void XgksOutPrimiToMo ();			/* c1147 */

	OUT_PRIMI  *primi = seg->primi_list.next;

	XgksMoSetAsfOnWs (ws);
	while (primi != NULL) {
		XgksOutPrimiToMo (ws, primi);
		primi = primi->next;
	}
	XgksMoSetClipOnWs (ws, &xgks_state.cliprec.rec); /* c1137 */ /*c1147*/
}

static								/* c1147 */
void								/* c1147 */
XgksOutPrimiToMo (ws, primi)
WS_STATE_PTR ws;
OUT_PRIMI *primi;
{
	switch (primi->pid) {
		case PLINE	: XgksMoSetGraphicAttrOnWs (ws, 44, primi->pickid);
				  XgksSetLineAttrMo (ws, &(primi->primi.pline.plnattr));
				  XgksMoGraphicOutputToWs 
					(ws, 11, primi->primi.pline.num_pts, primi->primi.pline.pts);
				  break;
		case PMARK	: XgksMoSetGraphicAttrOnWs (ws, 44, primi->pickid);
				  XgksSetMarkAttrMo (ws, &(primi->primi.pmark.mkattr));
				  XgksMoGraphicOutputToWs 
				      	(ws, 12, primi->primi.pmark.num_pts, primi->primi.pmark.location);
				  break;
		case TEXT	: XgksMoSetGraphicAttrOnWs (ws, 44, primi->pickid);
				  XgksSetTextAttrMo (ws, &(primi->primi.text.txattr), &(primi->primi.text.chattr));
				  XgksMoTextToWs (ws, primi->primi.text.location, primi->primi.text.string);
				  break;
		case FILL_AREA	: XgksMoSetGraphicAttrOnWs (ws, 44, primi->pickid);
				  XgksSetFillPatAttrMo (ws, 
					&(primi->primi.fill_area.flattr), (primi->primi.fill_area.ptattr));
				  XgksMoGraphicOutputToWs
					(ws, 14, primi->primi.fill_area.num_pts, primi->primi.fill_area.pts);
				  break;
		case CELL_ARRAY	: XgksMoSetGraphicAttrOnWs (ws, 44, primi->pickid);
				  XgksMoCellArrayToWs 
					(ws, &(primi->primi.cell_array.ll), &(primi->primi.cell_array.ur), 
					 &(primi->primi.cell_array.lr), primi->primi.cell_array.rowsize, 
					 primi->primi.cell_array.colour, &(primi->primi.cell_array.dim));
				  break;
		case GDP	: /* gdp */
		case CLIP_REC	: 
		case MESG	: 
		default		: break;
	}
}

static								/* c1147 */
void								/* c1147 */
XgksRestoreMoGksState ()
{
	WS_STATE_PTR  ws;    /* c1144 */
	Gint cnt;

	for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {
		ws = xgks_state.activews[cnt].ws;		/* c1144 *//* c1147 */
		if (xgks_state.activews[cnt].ws_id != INVALID && xgks_state.activews[cnt].ws->ewstype == MO) {			/* c1147 */
			XgksSetLineAttrMo (ws, &(xgks_state.gks_lnattr)); /* c1144 *//* c1147 */
			XgksSetMarkAttrMo (ws, &(xgks_state.gks_mkattr));/* c1144 *//* c1147 */
			XgksSetTextAttrMo (ws, &(xgks_state.gks_txattr),  &(xgks_state.gks_chattr)); /* c1144 */ /* c1147 */
			XgksSetFillPatAttrMo (ws, &(xgks_state.gks_flattr), &(xgks_state.gks_ptattr));	/* c1144 */ /* c1147 */
			XgksMoSetPatRefOnWs (ws);    /* c1144 */
			XgksMoSetPatSizeOnWs (ws);   /* c1144 */

		}
	}
}

static								/* c1147 */
void								/* c1147 */
XgksRestoreMoGksStateOnWs (ws)    /*  Proc added for c1144   */
WS_STATE_PTR  ws;                               /* c1144 */
{
	if (ws->ws_id != INVALID && ws->ewstype == MO) {
		XgksSetLineAttrMo (ws, &(xgks_state.gks_lnattr)); /* c1144 *//* c1147 */
		XgksSetMarkAttrMo (ws, &(xgks_state.gks_mkattr)); /* c1144 */ /* c1147 */
		XgksSetTextAttrMo (ws, &(xgks_state.gks_txattr), &(xgks_state.gks_chattr));	/* c1144 */ /* c1147 */
		XgksSetFillPatAttrMo (ws, &(xgks_state.gks_flattr), &(xgks_state.gks_ptattr));	/* c1144 */ /* c1147 */
		XgksMoSetPatRefOnWs (ws);    /* c1144 */
		XgksMoSetPatSizeOnWs (ws);   /* c1144 */
	}
}
XgksSetLineAttrMo (ws, lnattr)
WS_STATE_PTR ws;
Glnattr *lnattr;
{
	XgksMoSetGraphicAttrOnWs (ws, 21, lnattr->line);
	XgksMoSetGraphicAttrOnWs (ws, 22, lnattr->bundl.type);
	XgksMoSetGraphicSizeOnWs (ws, 23, lnattr->bundl.width);
	XgksMoSetGraphicAttrOnWs (ws, 24, lnattr->bundl.colour);
}

XgksSetMarkAttrMo (ws, mkattr)
WS_STATE_PTR ws;
Gmkattr *mkattr;
{
	XgksMoSetGraphicAttrOnWs (ws, 25, mkattr->mark);
	XgksMoSetGraphicAttrOnWs (ws, 26, mkattr->bundl.type);
	XgksMoSetGraphicSizeOnWs (ws, 27, mkattr->bundl.size);
	XgksMoSetGraphicAttrOnWs (ws, 28, mkattr->bundl.colour);
}

XgksSetTextAttrMo (ws, txattr, chattr)
WS_STATE_PTR ws;
Gtxattr *txattr;
CHATTR *chattr;
{
	CHATTR tmp;

	XgksMoSetGraphicAttrOnWs (ws, 29, txattr->text);
	XgksMoSetTextFPOnWs      (ws,   &(txattr->bundl.fp));
	XgksMoSetGraphicSizeOnWs (ws, 31, txattr->bundl.ch_exp);	/* c1179 */
	XgksMoSetGraphicSizeOnWs (ws, 32, txattr->bundl.space);
	XgksMoSetGraphicAttrOnWs (ws, 33, txattr->bundl.colour);

	tmp = xgks_state.gks_chattr;				/* c1147 */
	xgks_state.gks_chattr = *chattr;			/* c1147 */
	XgksMoSetCharUpOnWs      (ws);
	XgksMoSetTextPathOnWs    (ws,    chattr->path);		/* c1137 */
	XgksMoSetTextAlignOnWs   (ws,    &(chattr->align));
	xgks_state.gks_chattr = tmp;				/* c1147 */
}

XgksSetFillPatAttrMo (ws, flattr, ptattr)
WS_STATE_PTR ws;
Gflattr *flattr;
PTATTR  *ptattr;
{
	XgksMoSetGraphicAttrOnWs (ws, 37, flattr->fill);
	XgksMoSetFillIntStyleOnWs(ws, flattr->bundl.inter);	/*MIT*/
	XgksMoSetGraphicAttrOnWs (ws, 39, flattr->bundl.style);
	XgksMoSetGraphicAttrOnWs (ws, 40, flattr->bundl.colour);
	/* 41 & 42 not supported (patterns) */
}

static								/* c1147 */
void								/* c1147 */
XgksSegAttrMo (ws, seg)
WS_STATE_PTR ws;
SEG_STATE_PTR seg;
{
	XgksMoSetSegTransOnWs  (ws, seg->segattr.seg, seg->segattr.segtran);
	XgksMoSetSegAttrOnWs   (ws, seg->segattr.seg, 92, (seg->segattr.vis == GVISIBLE ? 0 : 1));
	XgksMoSetSegAttrOnWs   (ws, seg->segattr.seg, 93, (seg->segattr.hilight == GNORMAL ? 0 : 1));
	XgksMoSetSegPriOnWs    (ws, seg->segattr.seg, seg->segattr.pri);
	XgksMoSetSegAttrOnWs   (ws, seg->segattr.seg, 95, (seg->segattr.det == GUNDETECTABLE ? 0 : 1));
}

/* Inquiry functions for segment */

/*$F
 * ginqmodsegattr(ws_type, dyn)
 *	- INQUIRE DYNAMIC MODIFICATION OF SEGMENT ATTRIBUTES
 *
 * Gchar *ws_type;		type of workstation the inquiry is about.
 * Gmodseg *dyn;                OUT dynamic modification of segment attributes 
 *
 * errors 0, 8, 22, 23, 39
 *
 * See Also: ANSI Standard p.184
 */
ginqmodsegattr(ws_type, dyn)
Gchar *ws_type; 
Gmodseg *dyn; {

	EWSTYPE ewstype;

/* STEP 1: check for errors */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqmodsegattr); /* c1147 */

/* check for valid workstation */
	ewstype = XgksWsTypeToEnum ( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqmodsegattr);

/* check for proper workstation category */
	GKSERROR( (ewstype != X_WIN), 39, errginqmodsegattr); 

/* STEP 2: set up the return values. */
	dyn->transform = GIRG;
	dyn->appear    = GIMM;
	dyn->disappear = GIRG;
	dyn->highlight = GIMM;
	dyn->priority  = GIRG;
	dyn->addition  = GIMM;
	dyn->deletion  = GIRG;

	return(OK);
}
/*$F
 * ginqnameopenseg(seg) - INQUIRE NAME OF OPEN SEGMENT
 *
 * Gint *seg;		name of the currently open segment.
 *
 * errors 0, 4
 *
 * See Also: ANSI Standard p.151
 */
ginqnameopenseg(seg)
Gint *seg;
{
/* STEP 1: check for errors */
	GKSERROR( (xgks_state.gks_state != GSGOP), 4, errginqnameopenseg); /* c1147 */

/* STEP 2: set up the return values */
	*seg = xgks_state.gks_open_seg;				/* c1147 */

	return (OK);
}
/*$F
 * ginqsegattr(segattr) - INQUIRE SEGMENT ATTRIBUTES
 *
 * Gsegattr *segattr;              OUT segment attributes 
 *
 * errors 0, 7, 120, 122
 *
 * See Also: ANSI Standard p.189
 */
ginqsegattr(segattr)
Gsegattr *segattr;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();	/*MIT*/

	SEG_STATE_PTR seg;

/* STEP 1: check for errors */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqsegattr); /* c1147 */

/* check for valid segment name */
	GKSERROR( (segattr->seg < 1), 120, errginqsegattr);     /* c1175 */

/* check for segment existance */
	GKSERROR( ((seg=XgksFindSeg(segattr->seg)) == NULL), 122, errginqsegattr);

	*segattr = seg->segattr;

	return(OK);
}
/*$F
 * ginqassocws(seg, asswk) - INQUIRE SET OF ASSOCIATED WORKSTATIONS
 *
 * Gint seg;                segment name
 * Gintlist *asswk;         OUT set of associated workstation identifiers
 *
 *	On input, num contains the maximum size for the returned set.
 *
 * errors 0, 7, 120, 122
 *
 * See Also: ANSI Standard p.189
 */
ginqassocws(seg, asswk)
Gint seg;
Gintlist *asswk;
{
	/*
	 * declare local functions
	 */
	static SEG_STATE_PTR XgksFindSeg ();	/*MIT*/

	SEG_STATE_PTR segp;
	Gint i;

/* STEP 1: check for errors */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqassocws); /* c1147 */

/* check for valid segment name */
	GKSERROR( (seg < 1), 120, errginqassocws);      /* c1175 */

/* check for segment existance */
	GKSERROR( ((segp=XgksFindSeg(seg)) == NULL), 122, errginqassocws);

/* STEP 2: set up the return values */
	for (i=0; segp->assoc_ws[i] != INVALID; i++) ;
	asswk->number = i;
	GKSERROR (((asswk->integers = (Gint *)malloc( i * sizeof(Gint)))==NULL), 300, errginqassocws)
	for (i=0; i < asswk->number; i++)
		asswk->integers[i] = segp->assoc_ws[i];
	
	return (OK);
}
/*$F
 * ginqsegnames(segs) - INQUIRE SET OF SEGMENT NAMES IN USE
 *
 * Gintlist *segs;               OUT set of segment names in use 
 *
 *	On input, num contains the maximum number of names that can be returned.
 *	The names are not returned in any particular order.
 *
 * errors 0, 7
 *
 * See Also: ANSI Standard p.152
 */

ginqsegnames(segs)
Gintlist *segs;
{
	SEG_STATE_PTR seg;
	Gint cnt=0, num_seg=0;

/* STEP 1: check for errors */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqsegnames); /* c1147 */

/* STEP 2: set up the return values. */
	while (cnt < SHSIZE) {
		if ((seg=segtable[cnt]) != NULL) {
			num_seg++;
			seg = seg->seg_next;
			while (seg != NULL) {
				num_seg++;
				seg = seg->seg_next;
			}
		}
		cnt++;
	}
	segs->number = num_seg;
	GKSERROR (((segs->integers = (Gint *)malloc( num_seg * sizeof(Gint)))==NULL), 300, errginqsegnames)
	cnt = num_seg = 0;
	while (cnt < SHSIZE) {
		if ((seg=segtable[cnt]) != NULL) {
			segs->integers[num_seg] = seg->segattr.seg;
			num_seg++;
			seg = seg->seg_next;
			while (seg != NULL) {
				segs->integers[num_seg] = seg->segattr.seg;
				num_seg++;
				seg = seg->seg_next;
			}
		}
		cnt++;
	}

	return(OK);
}

/*$F
 * ginqsegnamesws(ws_id, segs) - INQUIRE SET OF SEGMENT NAMES ON WORKSTATION
 *
 * Gint ws_id;			workstation identifier
 * Gintlist *segs;               OUT set of stored segments for this workstation
 *
 * errors 0, 7, 20, 25, 33, 35
 *
 * See Also: ANSI Standard p.166
 */
ginqsegnamesws(ws_id, segs)
Gint ws_id;
Gintlist *segs;
{
	WS_STATE_PTR ws;
	WS_SEG_LIST *seg; 
	Gint i;

/* STEP 1: check for errors */
/* gks in proper state? */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqsegnamesws); /* c1147 */

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqsegnamesws)   /* c1012 */

/* workstation open? */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws=OPEN_WSID(ws_id)) == NULL), 25, errginqsegnamesws);  /* c1012 */

/* check the workstation category */
	GKSERROR( (WS_CAT(ws) == GMI), 33, errginqsegnamesws);

/* STEP 2: ask the workstation */
	for (i=0, seg=ws->seglist; seg != NULL; seg=seg->next, i++) ;
	segs->number = i;
	GKSERROR(((segs->integers = (Gint *)malloc( i * sizeof(int)))==NULL), 300, errginqsegnamesws)
	for (i=0, seg=ws->seglist; seg != NULL; seg=seg->next, i++)
		segs->integers[i] = seg->seg;
	return (OK);
}
/*$F
 * ginqnumsegpri(ws_type, numpri) - INQUIRE NUMBER OF SEGMENT PRIORITIES SUPPORTED
 * 
 * Gchar *ws_type;		type of workstation the inquiry is about.
 * Gint *numpri;                OUT number of segment priorities supported
 *
 * errors 0, 8, 22, 23, 39
 *
 * See Also: ANSI Standard p.183
 */
ginqnumsegpri(ws_type, numpri)
Gchar *ws_type;
Gint *numpri;
{
	EWSTYPE ewstype;

/* STEP 1: check for errors */
/* proper state? */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqnumsegpri); /* c1147 */

/* valid workstation type */
	ewstype = XgksWsTypeToEnum ( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqnumsegpri);

/* valid workstation category */
	GKSERROR( (ewstype != X_WIN && ewstype != MO), 39, errginqnumsegpri);

/* STEP 2: set up the return values */
	*numpri = 0;
	return (OK);
}
/*$F
 * gincurpickid(pickid) - INQUIRE CURRENT PICK IDENTIFIER VALUE
 * 
 * Gint *pickid		 OUT current pick identifier
 *
 * errors 0, 8
 *
 * See Also: ANSI Standard p.148
 */
ginqcurpickid(pickid)
Gint *pickid;
{
/* STEP 1: check for errors */
/* proper state? */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqcurpickid); /* c1147 */

/* STEP 2: set up the return values */
	*pickid = xgks_state.gks_pick_id;				/* c1147 */
	return (OK);
}
