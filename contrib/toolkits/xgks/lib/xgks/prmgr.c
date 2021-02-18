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
 * prmgr.c - primitive list manager
 *	These functions will manage the list of output primitive.
 *
 *   XgksNewPrimi() - will return a pointer to an empty primitive structure
 *
 *   XgksInsertPrimi(insert_pt, elm) - 
 *       OUT_PRIMI *insert_pt, *elm  insert elm-> into the the list pointed to by head, and WILL
 *				set elm->NULL after insertion.
 *       OUT_PRIMI **insert_pt	insertion point on the primitive list
 *
 *   XgksDeletePrimi (head) -
 *	OUT_PRIMI **head   delete ALL entries in the list pointed to by head, and will "FREE" all
 *			   memory associated with them. set head to NULL
 *   XgksProcessPrimi (primi)
 *      OUT_PRIMI *primi     process the primitive structure, decide if to append to 
 *  			     to open segment or should it be append to ws non segment
 *			     primitive list
 *
 *   XgksProcessClip () -     for all openedws and segmemt, build_clip().
 * 
 * $Header: prmgr.c,v 4.0 89/08/31 16:24:02 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/prmgr.c,v $
 *
 * $Log:	prmgr.c,v $
 * Revision 4.0  89/08/31  16:24:02  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.12  89/06/05  10:11:19  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * PTR# c1176:	Replaced string function calls with macros that check for
 * 		NULL pointers before making the call.
 * 
 * Revision 3.11  89/02/04  15:27:57  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.10  88/12/08  12:43:39  amy
 * PTR c1130	gerrorhand calls:  changed function name parameter from
 * 		character string to enum. type value.
 * Changed spelling from color to colour
 * 
 * Revision 3.9  88/09/23  08:52:19  amy
 * MIT	XgksInsertPrimi:  declare local functions.
 * 	XgksDeletePrimi:  declare local functions.
 * 	XgksPrimiDump:  #ifdef this routine.
 * 	XgksFreePrimiStruct:  define static void.
 * 	Delete assignment of INVALID to new->pid.
 * 
 * Revision 3.8  88/08/18  06:58:29  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 3.7  88/08/15  07:04:44  amy
 * Removed printf
 * 
 * Revision 3.6  88/08/15  06:55:36  amy
 * PTR c1001  XgksInsertMesgPrimi modified for fix to message problem.
 * 
 * Revision 1.3  88/08/08  15:38:22  bruce
 * PTR# c1001  - the function XgksInsertMesgPrimi() was added to handle the insertion of
 * message primitives into the non-segment primitive lists.  This function will add the
 * message primitive if there isn't one yet, or change the old one and move it to 
 * the end of the list.
 * 
 * Revision 1.2  88/07/26  17:53:35  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:50:01  david
 * Initial revision
 *  
 *
 */

static char *rcsid = "$Header: prmgr.c,v 4.0 89/08/31 16:24:02 amy Exp $";

#include <sys/file.h>
#include <strings.h>
#include "gks_implem.h"                 /* d1 */

/*
 *   XgksNewPrimi() - will return a pointer to an empty primitive structure
 */
OUT_PRIMI *
XgksNewPrimi() {
	
	OUT_PRIMI *new;

	if ( (new=(OUT_PRIMI *) malloc ((unsigned) sizeof(OUT_PRIMI))) != NULL )  {
		new->seg_cnt = 0;
		new->next = NULL; 
	}
	return (new);
}
/*
 *   XgksInsertMesgPrimi (ws, primi) -  function added in response to PTR c1001 
 *       WS_STATE_PTR  ws     points to the active workstation record
 *                            we need the message_pt pointer and if it's NULL, we
 *                            need the insert_pt
 *       OUT_PRIMI *primi     points to the message primitive to be inserted
 *                            This function checks to see if there's a message primitive
 *                            in the non-segment list.  If there's not, it inserts a message
 *                            primitive normally and sets a pointer to that primitive.
 *                            If there is a message, then it replaces the text in that 
 *                            primitive with the text for the new one, because there should
 *                            only be one message in the list.
 */
XgksInsertMesgPrimi (ws, primi)
WS_STATE_PTR ws;
OUT_PRIMI *primi;
{
Gchar *tmp;

    if (ws->message_pt == NULL)   /* If there is no message yet */
       {
       ws->bef_message = ws->primi_insert_pt;           /* set backptr to node bef message */
       XgksInsertPrimi (&(ws->primi_insert_pt), primi); /* insert the prim. normally */
       ws->message_pt = ws->primi_insert_pt;      /* then set the mesg ptr to the mesg prim */
       }
    else  /* Change the Message pointed to by message_pt  */
       {
       if ((tmp = (Gchar *)malloc ((unsigned)STRLEN(primi->primi.mesg.string) + 1)) == NULL)	/* c1176 */
          {
          gerrorhand (300, errXgksInsertMesgPrimi, xgks_state.gks_err_file); /*c1147*/
          return (NULL); 
          }
       STRCPY (tmp, primi->primi.mesg.string);  /* Make a copy of the new message */	/* c1176 */
       free (ws->message_pt->primi.mesg.string); /* Free the old message space */
       ws->message_pt->primi.mesg.string = tmp;  /* and set the message ptr to the new message */
       if (ws->primi_insert_pt != ws->message_pt)/* if mesg node needs to be moved, */
          {
          ws->bef_message->next = ws->message_pt->next; /* cut mesg node out of list */
          ws->primi_insert_pt->next = ws->message_pt;   /* make last node pt to mesg node */
          ws->bef_message = ws->primi_insert_pt;        /* reset bef_message ptr */
          ws->message_pt->next = NULL;                  /* end of list pts to NULL */
          ws->primi_insert_pt = ws->message_pt;         /* message node is insertion pt */
          }
       }
}
/*
 *   XgksInsertPrimi(head, insert_pt, elm) - 
 *       OUT_PRIMI *head, *elm  insert elm-> into the the list pointed to by head, 
 *				this is done by allocating new memory for the list and copy
 *			        content of elm over.
 *       OUT_PRIMI *insert_pt	insertion point on the primitive list
 */
XgksInsertPrimi (insert_pt, elm)
OUT_PRIMI **insert_pt, *elm;
{
	/*
	 * local function declares
	 */
	OUT_PRIMI *XgksDuplicatePrimi();	/*MIT*/

	(*insert_pt)->next = XgksDuplicatePrimi (elm);
	(*insert_pt) = (*insert_pt)->next;
}
/*
 *   XgksDeletePrimi (head) -
 *	OUT_PRIMI **head   delete ALL entries in the list pointed to by head, and will "FREE" all
 *			   memory associated with them. Set head to NULL
 */
XgksDeletePrimi (head, insert_pt)
OUT_PRIMI *head, **insert_pt;
{
	/*
	 * local function declares
	 */
	static void XgksFreePrimiStruct();	/*MIT*/

	OUT_PRIMI *cnt, *next;

	cnt = head->next;

	while (cnt != NULL) {
		next = cnt->next;
		XgksFreePrimiStruct ( cnt );
		free ((char *)cnt);
		cnt = next;
	}
	head->pid = CLIP_REC;
	head->seg_cnt = 0;
	head->pickid = INVALID;
	head->primi.clip.segment = FALSE;
	head->primi.clip.rec = xgks_state.cliprec.rec;		/*c1147*/
	(*insert_pt) = head;
	head->next = NULL;
}
/*
 * XgksProcessPrimi (primi)
 *   OUT_PRIMI *primi	  process the primitive structure, decide if to append to 
 *			  to open segment or should it be append to ws non segment
 *			  primitive list
 */
XgksProcessPrimi (primi)
OUT_PRIMI *primi;
{
	OUT_PRIMI *tmp;

	if (xgks_state.gks_state == GSGOP) { /* there's opened segment, append to it */ /*c1147*/
		if ( (tmp=XgksAppendSegPrimi(primi)) == NULL) 
			return;		/* this means do not darw out, eg segment invisible */
		XgksOutputToWs (tmp);
		return;
	} 
	XgksAppendWsPrimi(primi);	/* There's no open segments, append to active ws */
	XgksOutputToWs(primi);		/* Now draw to all active workstations */
}
/*
 *   XgksProcessClip () -     for all openedws and segmemt,change current CLIP primitive or
 *			   if current primitive is not CLIP, append a new one, not appending
 *			   new CLIP primitive to segment will not cause any actual corresponding
 *			   x-calls, they are merely there for redraw purpose
 */
XgksProcessClip(rec)
Glimit *rec;
{
	if (xgks_state.gks_state == GSGOP)  XgksAppendSegClip(rec) ;	/*c1147*/
	XgksAppendWsClip (rec); /* always update ws->primilist */
}

#ifdef notdef		/*MIT*/
/*
 * XgksPrimiDump (head)
 *		
 */
XgksPrimiDump (head)
OUT_PRIMI *head;
{
	fprintf(stderr, "pids .. ");
	while (head != NULL) {
	  fprintf(stderr, "%d ", head->pid);
	  head = head->next;
	}
	fprintf(stderr, "\n");
}
#endif		/*MIT*/

/* 
 * XgksUpdatePrimiBound (primi, bound) -- locate a bounding box for the primitive 
 *
 * NOTE : everything is in NDC space
 *
 */
XgksUpdatePrimiBound (primi, bound)
OUT_PRIMI *primi;
Glimit *bound;
{
	
	Gpoint *ndc_pt;
	Gint cnt, num_pts;

    	switch (primi->pid) {
	case PLINE: 
		num_pts = primi->primi.pline.num_pts;
		ndc_pt  = primi->primi.pline.pts;
    		for (cnt=0; cnt<num_pts; cnt++) 
       	    	    XgksMiniMax (bound, ndc_pt++);
		break;
	case PMARK:
		num_pts =  primi->primi.pmark.num_pts;
		ndc_pt  = primi->primi.pmark.location;
    		for (cnt=0; cnt<num_pts; cnt++) 
       	    	    XgksMiniMax (bound, ndc_pt++);
		break;
	case FILL_AREA:
		num_pts =  primi->primi.fill_area.num_pts;
		ndc_pt  = primi->primi.fill_area.pts;
    		for (cnt=0; cnt<num_pts; cnt++) 
       	    	    XgksMiniMax (bound, ndc_pt++);
		break;
	case CELL_ARRAY: 
		XgksMiniMax (bound, &(primi->primi.cell_array.ll));
		XgksMiniMax (bound, &(primi->primi.cell_array.lr));
		XgksMiniMax (bound, &(primi->primi.cell_array.ur));
		XgksMiniMax (bound, &(primi->primi.cell_array.ul));
		break;
	case CLIP_REC  : break;
	case MESG      : break;
	case TEXT      : break;
	case GDP       : break;
   }
}
/*
 * XgksMiniMax (bound, pt) -- setting values in bound.xmax if pt.x > bound->xmamx 
 *					    bound.xmin if pt.x < bound->xmin 
 *					    bound.ymax if pt.y > bound->ymax
 *					    bound.ymin if pt.y < bound->ymin
 */
XgksMiniMax (bound, pt)
Glimit *bound;
Gpoint *pt;
{
   if (pt->x > bound->xmax) bound->xmax = pt->x;
   if (pt->x < bound->xmin) bound->xmin = pt->x;
   if (pt->y > bound->ymax) bound->ymax = pt->y;
   if (pt->y < bound->ymin) bound->ymin = pt->y;
}
/*
 * XgksFreePrimiStruct ( primi ) -- Free the memory associated with primitive sub-structure
 *
 */
static void XgksFreePrimiStruct (primi)		/*MIT*/
OUT_PRIMI *primi;
{
    switch (primi->pid) {
	case PLINE: 
		if (primi->primi.pline.num_pts > 0)
		  free (primi->primi.pline.pts);
		break;
	case PMARK:
		if (primi->primi.pmark.num_pts > 0)
		  free(primi->primi.pmark.location);
		break;
	case FILL_AREA:
		if (primi->primi.fill_area.num_pts > 0)
		  free(primi->primi.fill_area.pts);
		break;
	case CLIP_REC  : break;
	case TEXT      : 
		free(primi->primi.text.location);
		free(primi->primi.text.string);
		break;
	case MESG      : 
		free(primi->primi.mesg.string);
		break;
	case CELL_ARRAY: 
		free(primi->primi.cell_array.colour);
		break;
	case GDP       : break;
    }                                        
}
/*
 * XgksDuplicatePrimi -- Input a primitive structure and output a duplicate one
 *			 with real memory.
 */
OUT_PRIMI *XgksDuplicatePrimi (primi)
OUT_PRIMI *primi;
{
    Gint num_pts, cnt;
    Gpoint *new_pts, *old_pts;
    OUT_PRIMI *new_primi;

    if ((new_primi = XgksNewPrimi ()) == NULL) {
	gerrorhand (300, errXgksDuplicatePrimi, xgks_state.gks_err_file); /*c1147*/
	return (NULL);
    }

    *new_primi = *primi;

    switch (primi->pid) {
	case PLINE: 
		num_pts = new_primi->primi.pline.num_pts = primi->primi.pline.num_pts;
		if ((new_pts = (Gpoint *) malloc ((unsigned) num_pts*sizeof(Gpoint)))==NULL) { 
			gerrorhand (300, errXgksDuplicatePrimi, xgks_state.gks_err_file); /*c1147*/
			return (NULL);
    		}
		new_primi->primi.pline.pts = new_pts;
		old_pts  = primi->primi.pline.pts;
    		for (cnt=0; cnt<num_pts; cnt++, new_pts++, old_pts++) 
		    *new_pts = *old_pts;
		break;
	case PMARK:
		num_pts = new_primi->primi.pmark.num_pts = primi->primi.pmark.num_pts;
		if ((new_pts = (Gpoint *) malloc ((unsigned) num_pts*sizeof(Gpoint)))==NULL) { 
			gerrorhand (300, errXgksDuplicatePrimi, xgks_state.gks_err_file); /*c1147*/
			return (NULL);
    		}
		new_primi->primi.pmark.location = new_pts;
		old_pts = primi->primi.pmark.location;
    		for (cnt=0; cnt<num_pts; cnt++, new_pts++, old_pts++) 
		    *new_pts = *old_pts;
		break;
	case FILL_AREA:
		num_pts = new_primi->primi.fill_area.num_pts = primi->primi.fill_area.num_pts;
		if ((new_pts = (Gpoint *) malloc ((unsigned) num_pts*sizeof(Gpoint)))==NULL) { 
			gerrorhand (300, errXgksDuplicatePrimi, xgks_state.gks_err_file); /*c1147*/
			return (NULL);
    		}
		new_primi->primi.fill_area.pts = new_pts;
		old_pts = primi->primi.fill_area.pts;
    		for (cnt=0; cnt<num_pts; cnt++, new_pts++, old_pts++) 
		    *new_pts = *old_pts;
		break;
	case CLIP_REC  : 
		new_primi->primi.clip.rec = primi->primi.clip.rec;
		new_primi->primi.clip.segment = primi->primi.clip.segment;
		break;
	case TEXT      : 
		if ((new_pts = (Gpoint *) malloc ((unsigned)sizeof(Gpoint)))==NULL) { 
			gerrorhand (300, errXgksDuplicatePrimi, xgks_state.gks_err_file); /*c1147*/
			return (NULL);
    		}
		new_primi->primi.text.location = new_pts;
		old_pts = primi->primi.text.location;
       		*new_pts = *old_pts;
		if ((new_primi->primi.text.string= (Gchar *)malloc ((unsigned) STRLEN(primi->primi.text.string) + 1))==NULL) {	/* c1176 */
			gerrorhand(300, errXgksDuplicatePrimi, xgks_state.gks_err_file); /*c1147*/
			return (NULL);
		}
		STRCPY ((new_primi->primi.text.string), primi->primi.text.string);	/* c1176 */
		break;
	case MESG      : 
		if ((new_primi->primi.mesg.string= (Gchar *)malloc ((unsigned) STRLEN(primi->primi.mesg.string) + 1))==NULL) {	/* c1176 */
			gerrorhand(300, errXgksDuplicatePrimi, xgks_state.gks_err_file); /*c1147*/
			return (NULL);
		}
		STRCPY ((new_primi->primi.mesg.string), primi->primi.mesg.string);	/* c1176 */
		break;
	case CELL_ARRAY: 
		cnt = primi->primi.cell_array.rowsize * primi->primi.cell_array.dim.y;
		if ((new_primi->primi.cell_array.colour=(Gint *)malloc(cnt * sizeof(Gint))) == NULL) {
			gerrorhand(300, errXgksDuplicatePrimi, xgks_state.gks_err_file); /*c1147*/
			return (NULL);
		}
		cnt--;
		while (cnt>=0) {
			new_primi->primi.cell_array.colour[cnt] = primi->primi.cell_array.colour[cnt];
			cnt--;
		}
		new_primi->primi.cell_array.ll = primi->primi.cell_array.ll;
		new_primi->primi.cell_array.lr = primi->primi.cell_array.lr;
		new_primi->primi.cell_array.ur = primi->primi.cell_array.ur;
		new_primi->primi.cell_array.lr = primi->primi.cell_array.lr;
		new_primi->primi.cell_array.dim = primi->primi.cell_array.dim;
		new_primi->primi.cell_array.rowsize = primi->primi.cell_array.rowsize;
		break;
	case GDP       : break;
    }                                        
	
   return (new_primi);
}
