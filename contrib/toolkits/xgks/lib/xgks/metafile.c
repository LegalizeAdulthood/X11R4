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
 * $Header: metafile.c,v 4.0 89/08/31 16:22:49 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/metafile.c,v $
 *
 * $Log:	metafile.c,v $
 * Revision 4.0  89/08/31  16:22:49  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.23  89/06/05  10:11:59  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * PTR# c1176:	Replaced string function calls with macros that check for
 * 		NULL pointers before making the call.
 * PTR# c1179:	Changed exp field to tx_exp or ch_exp for AIX compiler
 * 		Moved variable declarations illegal in AIX.
 * 
 * Revision 3.22  89/03/15  20:48:52  amy
 * XgksExecData:  removed definition of unused variable key.
 * 
 * Revision 3.21  89/02/04  15:26:14  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.20  89/01/19  11:31:39  amy
 * PTR c1068	Additional changes for reading in of message items.
 * 
 * Revision 3.19  88/12/29  13:22:07  amy
 * PTR c1138	ggetgksm, greadgksm:  comments explaining MI error handling
 * 		were added.
 * 		XgksExecData:  no longer calls non-WISS functions on WISS 
 * 		workstations.  Its cellarray handler used to assign the value
 * 		of dim.y to rowsize; now it uses the value of dim.x.  No longer
 * 		closes all MI workstations when passed an end-of-metafile item.
 * 
 * Revision 3.18  88/12/28  17:21:23  amy
 * PTR c1137	XgksMoDeferWs:  corrected to write out correct value for the
 * 		implicit regeneration mode.
 * 		XgksMDate and XgksMAuthor:  fixed abuse of the library
 * 		string-handling functions.
 * 		XgksMDate:  now returns the current date, instead of hard-coded
 * 		88/01/01.
 * 
 * Revision 3.17  88/12/22  18:28:13  amy
 * PTR c1073	XgksExecData:  call gcloseseg just once, not once for each
 * 		workstation.
 * 
 * Revision 3.16  88/12/22  14:28:23  amy
 * PTR c1140	Added function XgksMoActivateWs to write the GKS 
 * 		state list and clipping information to any active MO ws upon 
 * 		activation.  (See ANSI Appendix E.)  This function is called
 * 		gactivatews (in act_ws.c).
 * 
 * Revision 3.15  88/12/22  13:55:11  amy
 * PTR c1144	NOTSUPPORTED macro:  change to correctly reflect unsupported
 * 		functions.
 * 		Added procedures XgksMoSetPatSizeOnWs, XgksMoSetPatRefOnWs,
 * 		and XgksMoSetPatSize and XgksMoSetPatRef.
 * 
 * Revision 3.14  88/12/08  11:54:40  amy
 * PTR c1130	GKSERROR and gerrorhand calls:  changed function name parameter
 * 		from character string to enum. type value.
 * Changed spelling of color to colour.
 * Changed spelling of center to centre.
 * 
 * Revision 3.13  88/11/18  14:05:00  amy
 * PTR c1068	Added extensive error checking (I/O and format) to the reading
 * 	and interpreting of MI data.  Fixed greadgksm to accept a positive buffer
 * 	length less than the actual item data size (excess data is truncated,
 * 	as per the standard), and to skip the current item on a zero-length buffer.
 * 	Other similar corrections made to improve MI error handling and recovery.
 * 
 * Revision 3.12  88/10/28  14:22:43  amy
 * PTR c1118	gwritegksm:  Added check for error 160 (Item type not allowed for 
 * 		user items).
 * 
 * Revision 3.11  88/09/22  08:57:04  amy
 * Edited MIT entry in log.
 * 
 * Revision 3.10  88/09/22  08:53:07  amy
 * MIT	Include strings.h
 *    	XgksmRecSize:  change spelling to XgksMRecSize.
 * 	Calling functions declare all local functions they are in turn calling.
 * 	XgksmAuthor:  changed spelling to XgksMAuthor.
 * 	XgksmDate:  changed spelling to XgksMDate.
 * 	Delete unused automatic variables tp and tzp.
 * 
 * Revision 3.9  88/08/22  07:41:43  amy
 * Removed nested comment.
 * 
 * Revision 3.8  88/08/17  13:41:15  amy
 * AUG  gwritegksm:  fixed state check.
 *   greadgksm:  fixed data len. check.
 *     and changed parms in call to XgksInputData.
 *   ginterpret:  checks for error 167.
 *   XgksMoSetPatRep:  add space in printf.
 *   XgksValidGksMItem:  checks for user item.
 *   XgksmRecSize:  delete unused return(0).
 *   XgksInputData:  changed formal parm; now handles user items.
 *   Changed "defaulf" to "default."
 * 
 * Revision 3.7  88/08/11  08:33:38  amy
 * PTR c1012  ggetgksm, greadgksm & gwritegksm:  added call to VALID_WSID
 * to check for error 20.
 * 
 * Revision 1.3  88/07/28  14:32:19  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.2  88/07/26  17:52:54  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:49:31  david
 * Initial revision
 *  
 *
 */
 
static char *rcsid = "$Header: metafile.c,v 4.0 89/08/31 16:22:49 amy Exp $";

#include <sys/time.h>
#include <strings.h>		/*MIT*/
#include <pwd.h>
#include <math.h>
#include <ctype.h>						/* c1068 */
#include "gks_implem.h"                 /* d1 */
#include "metafile.h"

static Gint gksm_version = 1;

static struct passwd  *passwd;

static Gchar buffer[41];
static Gchar date[9];
static Gchar dummy[] = "dummy info."; /* 11 bytes dummy for this implementation */


/*
	NOTE - size of string includes the last NULL character 

*/


#define NOTSUPPORTED(type)	(type==6 || type==16 )     /* c1144 */

typedef struct {
	Gchar	std[5];		/* the string GKSM + NULL */
	Gchar   info[41];	/* author installation etc. + NULL */
	Gchar	date[9];	/* yy/mm/dd + NULL */
	Gint	ver;		/* version numer */
	Gint    h,t,l,i,r,f,ri;	/* misc info */
	Gchar   d1[12], d2[12]; /* dummy in this implementation */
} GKSM_HEADER;

static struct {
	Ggksmit 	CurItem;		/* Each opened ws have their current item type store here */
	GKSM_HEADER	HeaderInfo;
	Gint		ws_id;
	Gint		GksmEmpty;
} GksmInfo[MAX_OPEN_WS];


XgksInitGksM ()
{
	Gint i;

	for (i=0; i<MAX_OPEN_WS; i++) {
		GksmInfo[i].CurItem.type = INVALID;
		GksmInfo[i].CurItem.length = INVALID;
		GksmInfo[i].ws_id = INVALID;
		GksmInfo[i].GksmEmpty = TRUE;
	}
}

	
/* 
 * gwritegksm (ws_id, type, length, data) -- WRITE ITEM TO GKSM
 * Gint ws_id;		workstation identifier
 * Gint type;		item type;
 * Gint length;		item length;
 * Gchar *data;		item data record pointer;
 *
 * returns : 0, 5, 20, 30, 32, 160, 161
 *
 * See Also: ANSI Standard p.142
 */
gwritegksm (ws_id, type, length, data)
Gint ws_id, type, length;
Gchar *data;
{
	/*
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	WS_STATE_PTR ws;
	Gint i;

	GKSERROR ((xgks_state.gks_state != GWSAC && xgks_state.gks_state != GSGOP), 5, errgwritegksm) /*AUG*/ /*c1147*/

/* DWO 7/26/88  added new check for validity in the name of PTR c1012 */
        GKSERROR ((!VALID_WSID(ws_id)), 20, errgwritegksm)   /* c1012 */

/* DWO 7/26/88  used to return error 20, now returns 30 in the name of PTR c1012 */
/*              if it isn't open, it can't be active...                          */
	GKSERROR (((ws=OPEN_WSID(ws_id))==NULL), 30, errgwritegksm)  /* c1012 */

	GKSERROR ((ws->wsstate != GACTIVE), 30, errgwritegksm)

	GKSERROR ((WS_CAT(ws) != GMO), 32, errgwritegksm)

        GKSERROR ((type <= 100), 160, errgwritegksm)                 /* c1118 */

	GKSERROR ((length < 0), 161, errgwritegksm)		/* c1068 */

	fprintf(ws->mfp, "%d %d ", type, length);
	for (i=0; i<length; i++)
		fprintf(ws->mfp, "%c", data[i]);
	fprintf(ws->mfp, "\n");

	return (OK);
}

/* 
 * ggetgksm (ws_id, result) -- GET ITEM TYPE FROM GKSM
 * Gint ws_id;		workstation identifier
 * Ggksmit *result	output metafile item infomation 
 *
 * returns : 0, 7, 20, 25, 34, 162, 163
 *
 * See Also: greadgksm (for explanation of ws->filestat), 	 * c1138 * 
 *           ANSI Standard p.142				 * c1138 * 
 */
ggetgksm (ws_id, result)
Gint ws_id;
Ggksmit *result;
{
	/*
	 * declare local functions
	 */
	static XgksMFindKey();		/*MIT*/
	static XgksValidGksMItem();	/*MIT*/

	WS_STATE_PTR ws;
	Gint key;

	GKSERROR ((xgks_state.gks_state != GWSOP && xgks_state.gks_state != GWSAC && xgks_state.gks_state != GSGOP), 7, errggetgksm) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errggetgksm)   /* c1012 */

/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id))==NULL), 25, errggetgksm)  /* c1012 */

	GKSERROR ((WS_CAT(ws) != GMI), 34, errggetgksm)

	GKSERROR (((key=XgksMFindKey(ws_id))==INVALID), 34, errggetgksm)

	GKSERROR ((GksmInfo[key].GksmEmpty == TRUE), 162, errggetgksm)

        if (XgksValidGksMItem(GksmInfo[key].CurItem.type)       /* c1068 */
            == INVALID)                                   {     /* c1068 */
                ws->filestat = MF_ITEM_ERR;                     /* c1068 */
        }                                                       /* c1068 */

	*result = GksmInfo[key].CurItem;			/* c1068 */

	GKSERROR ((ws->filestat != METAFILE_OK), 163, errggetgksm) /* c1068 */

	return (OK);
}

/* 
 * greadgksm (ws_id, length, result) -- READ ITEM FROM GKSM
 * Gint ws_id;		workstation identifier
 * Gint length;		maximium item data record length
 * Gchar *record;	Output metafile item data record
 *
 * returns : 0, 7, 20, 25, 34, 162, 163, 165, 166
 *
 * See Also: ANSI Standard p.143
 *
 * c1138:  the following explanation was added
 *
 * The filestat field has been added to the workstation state structure to
 * retain MI error information between calls to ggetgksm and greadgksm.  The
 * field has one of four possible integer values (defined in metafile.h):
 *    METAFILE_OK -- no errors so far reading from metafile
 *    MF_DATA_ERR -- type and length of latest item read (current item) are
 *                   ok, but XgksReadData couldn't read the data (eg. non-
 *                   numeric characters found when trying to read an integer)
 *                   The item may be skipped (via greadgksm w/ length 0) and
 *                   MI processing can continue.
 *    MF_ITEM_ERR -- something more serious than a data error found in latest
 *                   item; eg. type invalid, length invalid, data read ter-
 *                   minated prematurely.  This error condition can be detected
 *                   while going on to the next item, so the current item is
 *                   returned correctly, but subsequent attempts to get/read
 *                   will fail.  Since the exact cause of the error is unknown,
 *                   this is not a recoverable condition.
 *    MF_FILE_ERR -- the system reported an I/O error during a read attempt.
 *                   This error is not recoverable.
 * The first function to detect the error will report it, while attempting to
 * process the item it applies to.  In other words, if greadgksm encounters a
 * file error while trying to go on to the next item after successfully reading
 * the current item, the error will not be reported until the next get/read
 * call.  After a fatal error has been reported (via GKS error 163, item is
 * invalid), subsequent get/read attempts will return error 162, no items left
 * in MI, since the error is unrecoverable and no more reading is allowed.
 */
greadgksm (ws_id, length, record)
Gint ws_id, length;
Gchar *record;
{
	/*
	 * declare local functions
	 */
	static XgksMFindKey();		/*MIT*/
	static XgksValidGksMItem();	/*MIT*/
	static Gint XgksGetNextMi();	/*MIT*/			/* c1068 */
	static Gint XgksInputData();	/*MIT*/			/* c1068 */

	WS_STATE_PTR ws;
	Gint key;						/* c1068 */
	Gchar *intrec;						/* c1068 */

	GKSERROR ((xgks_state.gks_state != GWSOP && xgks_state.gks_state != GWSAC && xgks_state.gks_state != GSGOP), 7, errgreadgksm) /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errgreadgksm)   /* c1012 */

/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR (((ws=OPEN_WSID(ws_id))==NULL), 25, errgreadgksm) /* c1012 */

	GKSERROR ((WS_CAT(ws) != GMI), 34, errgreadgksm)

	GKSERROR (((key=XgksMFindKey(ws_id))==INVALID), 34, errgreadgksm)

	if (GksmInfo[key].CurItem.type == 0)			/* c1068 */
	{							/* c1068 */
		GksmInfo[key].GksmEmpty = TRUE;			/* c1068 */
	}							/* c1068 */

	GKSERROR ((GksmInfo[key].GksmEmpty == TRUE), 162, errgreadgksm)

	if (ws->filestat == MF_FILE_ERR) {			/* c1068 */
		GksmInfo[key].GksmEmpty = TRUE;			/* c1068 */
		gerrorhand( 162, errgreadgksm, xgks_state.gks_err_file ); /* c1068 */ /*c1147*/
		return( 162 );					/* c1068 */
	}							/* c1068 */

	if (XgksValidGksMItem(GksmInfo[key].CurItem.type)	/* c1068 */
	    == INVALID)					  {	/* c1068 */
		ws->filestat = MF_ITEM_ERR;			/* c1068 */
	}							/* c1068 */

	GKSERROR (((ws->filestat == MF_ITEM_ERR) && (length != 0)), MF_ITEM_ERR,                   errgreadgksm)					/* c1068 */

	GKSERROR (((ws->filestat == MF_DATA_ERR) && (length != 0)), MF_DATA_ERR,                   errgreadgksm)					/* c1068 */

	GKSERROR ((length < 0), 166, errgreadgksm)		/* c1068 */

	if (length > 0) {					/* c1068 */
		intrec = (Gchar *)malloc( GksmInfo[key].CurItem.length );									/* c1068 */
		GKSERROR ((intrec == NULL), 300, errgreadgksm)	/* c1068 */
		ws->filestat =					/* c1068 */
		XgksInputData (ws->mfp, key, intrec);		/* c1068 */
		bcopy( intrec, record, length );		/* c1068 */
		free( intrec );					/* c1068 */
		if (feof( ws->mfp )) {				/* c1068 */
			GksmInfo[key].GksmEmpty = TRUE;		/* c1068 */
			gerrorhand( 162, errgreadgksm, xgks_state.gks_err_file ); /* c1068 */ /*c1147*/
			return( 162 );				/* c1068 */
		}
		GKSERROR ((ws->filestat == MF_ITEM_ERR) ||						  (ws->filestat == MF_FILE_ERR), 163, errgreadgksm)									/* c1068 */
		GKSERROR ((ws->filestat == MF_DATA_ERR), 165, errgreadgksm)									/* c1068 */
	}							/* c1068 */
	else							/* c1068 */
	{							/* c1068 */
		/* skip to end of current item */		/* c1068 */
		fscanf( ws->mfp, "%*[^\n]" );			/* c1068 */
	}							/* c1068 */

	ws->filestat =						/* c1068 */
	XgksGetNextMi (ws->mfp, key);

	return (OK);
}

/* 
 * ginterpret (recInfo, data)
 * Ggksmit	*recInfo;	item type and length pointer
 * Gchar 	*data		item data recoird pointer
 *
 * returns : 0, 7, 161, 163, 164, 165, 167, 168
 *
 * See Also: ANSI Standard p.143
 */
ginterpret (recInfo, data)
Ggksmit	*recInfo;
Gchar *data;
{
	/*
	 * declare local functions
	 */
	static XgksValidGksMItem();	/*MIT*/
	static XgksMRecSize();		/*MIT*/
	static XgksExecData();		/*MIT*/

	GKSERROR ((xgks_state.gks_state != GWSOP && xgks_state.gks_state != GWSAC && xgks_state.gks_state != GSGOP), 7, errginterpret) /*c1147*/

	GKSERROR ((recInfo == NULL), 163, errginterpret )	/* c1068 */

	GKSERROR (((recInfo->length > 0)&&(data == NULL)), 165, errginterpret )									/* c1068 */

	GKSERROR ((recInfo->length<XgksMRecSize(recInfo->type)), 161, errginterpret)

        GKSERROR ((XgksValidGksMItem(recInfo->type)==INVALID), 164, errginterpret
)

        /* c1068:  Can't check for 165 in ginterpret due to file format. */
        /* c1068:  Can't really check for 163, either.                   */

	GKSERROR ( (recInfo->type >100), 167, errginterpret)	/* AUG */

	GKSERROR ((NOTSUPPORTED(recInfo->type)), 168, errginterpret)

	GKSERROR ((XgksExecData (recInfo->type, data) != 0), 164, errginterpret)									/* c1068 */

	return (OK);
}

/*
 * XgksMiOpenWs -- Scan in Header, and the first item 
 */
int XgksMiOpenWs ( ws )						/* c1068 */
WS_STATE_PTR ws;
{
	/*
	 * declare local functions
	 */
	static XgksMInstall();		/*MIT*/
	static Gint XgksGetNextMi();	/*MIT*/			/* c1068 */
	static Gint XgksFeoln();				/* c1068 */

	Gint key,i,readct;					/* c1068 */

	clearerr( ws->mfp );					/* c1068 */
	key = XgksMInstall (ws->ws_id);
	for (i=0; i<4; i++) {					/* c1068 */
		READCHR (ws->mfp, GksmInfo[key].HeaderInfo.std[i]);										/* c1068 */
		if ((ferror( ws->mfp )) ||			/* c1068 */
		    (GksmInfo[key].HeaderInfo.std[i] == '\n'))	/* c1068 */
			return( 1 );				/* c1068 */
	}							/* c1068 */
	GksmInfo[key].HeaderInfo.std[4] = NULL;
	for (i=0; i<40; i++) {					/* c1068 */
		READCHR(ws->mfp, GksmInfo[key].HeaderInfo.info[i]);										/* c1068 */
		if ((ferror( ws->mfp )) ||			/* c1068 */
		    (GksmInfo[key].HeaderInfo.info[i] == '\n'))	/* c1068 */
			return( 1 );				/* c1068 */
	}							/* c1068 */
	GksmInfo[key].HeaderInfo.info[40] = NULL;
	for (i=0; i<8; i++) {					/* c1068 */
		READCHR(ws->mfp, GksmInfo[key].HeaderInfo.date[i]);										/* c1068 */
		if ((ferror( ws->mfp )) ||			/* c1068 */
		    (GksmInfo[key].HeaderInfo.date[i] == '\n'))	/* c1068 */
			return( 1 );				/* c1068 */
	}							/* c1068 */
	GksmInfo[key].HeaderInfo.date[8] = NULL;
	readct =						/* c1068 */
	READINT (ws->mfp, GksmInfo[key].HeaderInfo.ver);	/* c1068 */
	if (readct != 1) return( 1 );				/* c1068 */
	if (XgksFeoln( ws->mfp )) return( 1 );			/* c1068 */
	readct =						/* c1068 */
	READINT (ws->mfp, GksmInfo[key].HeaderInfo.h);		/* c1068 */
	if (readct != 1) return( 1 );				/* c1068 */
	if (XgksFeoln( ws->mfp )) return( 1 );			/* c1068 */
	readct =						/* c1068 */
	READINT (ws->mfp, GksmInfo[key].HeaderInfo.t);		/* c1068 */
	if (readct != 1) return( 1 );				/* c1068 */
	if (XgksFeoln( ws->mfp )) return( 1 );			/* c1068 */
	readct =						/* c1068 */
	READINT (ws->mfp, GksmInfo[key].HeaderInfo.l);		/* c1068 */
	if (readct != 1) return( 1 );				/* c1068 */
	if (XgksFeoln( ws->mfp )) return( 1 );			/* c1068 */
	readct =						/* c1068 */
	READINT (ws->mfp, GksmInfo[key].HeaderInfo.i);		/* c1068 */
	if (readct != 1) return( 1 );				/* c1068 */
	if (XgksFeoln( ws->mfp )) return( 1 );			/* c1068 */
	readct =						/* c1068 */
	READINT (ws->mfp, GksmInfo[key].HeaderInfo.r);		/* c1068 */
	if (readct != 1) return( 1 );				/* c1068 */
	if (XgksFeoln( ws->mfp )) return( 1 );			/* c1068 */
	readct =						/* c1068 */
	READINT (ws->mfp, GksmInfo[key].HeaderInfo.f);		/* c1068 */
	if (readct != 1) return( 1 );				/* c1068 */
	if (XgksFeoln( ws->mfp )) return( 1 );			/* c1068 */
	readct =						/* c1068 */
	READINT (ws->mfp, GksmInfo[key].HeaderInfo.ri);		/* c1068 */
	if (readct != 1) return( 1 );				/* c1068 */
	for (i=0; i<11; i++) {					/* c1068 */
		if (XgksFeoln( ws->mfp )) return( 1 );		/* c1068 */
		READCHR(ws->mfp, GksmInfo[key].HeaderInfo.d1[i]);										/* c1068 */
		if ((ferror( ws->mfp )) ||			/* c1068 */
		    (GksmInfo[key].HeaderInfo.d1[i] == '\n'))	/* c1068 */
			return( 1 );				/* c1068 */
	}							/* c1068 */
	GksmInfo[key].HeaderInfo.d1[11] = NULL;
	for (i=0; i<11; i++) {					/* c1068 */
		READCHR(ws->mfp, GksmInfo[key].HeaderInfo.d2[i]);										/* c1068 */
		if ((ferror( ws->mfp )) ||			/* c1068 */
		    (GksmInfo[key].HeaderInfo.std[i] == '\n'))	/* c1068 */
			return( 1 );				/* c1068 */
	}							/* c1068 */
	GksmInfo[key].HeaderInfo.d2[11] = NULL;

	ws->filestat = METAFILE_OK;				/* c1068 */

	ws->filestat =						/* c1068 */
	XgksGetNextMi (ws->mfp, key);				/* c1068 */

	if (ws->filestat != METAFILE_OK) return( 1 );		/* c1068 */

	return( 0 );
}

/*
 * Do something about this header !!
 */
XgksMoOpenWs ( ws )
WS_STATE_PTR ws;
{
	/*
	 * declare local functions
	 */
	static Gchar *XgksMAuthor();	/*MIT*/
	static Gchar *XgksMDate();	/*MIT*/

	fprintf (ws->mfp, "%s%s%s\n", "GKSM", XgksMAuthor(), XgksMDate()); /*MIT*/
	fprintf (ws->mfp, "%d 0 %d %d %d %d 1 1",gksm_version, INT_SIZE, INT_SIZE, INT_SIZE, FLOAT_SIZE);
	fprintf (ws->mfp, "%s%s\n", dummy, dummy);
}

XgksMiCloseWs ( ws )
WS_STATE_PTR ws;
{
	/*
	 * declare local functions
	 */
	static void XgksMDelete();	/*MIT*/

	XgksMDelete(ws->ws_id);
	fclose (ws->mfp);
}

/*
 *  XgksMoCloseWs -- pad a metafile EOF and close the file
 */
XgksMoCloseWs ( ws )
WS_STATE_PTR ws;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf (ws->mfp, "0 %d\n",XgksMRecSize(0));
	fclose  (ws->mfp);
}

XgksMoClearWs ( ws, flag )
WS_STATE_PTR ws;
Gclrflag flag;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf (ws->mfp, "1 %d %d\n", XgksMRecSize(1), (flag == GCONDITIONALLY ? 0 : 1));
}

XgksMoReDrawAllSeg ( ws ) 
WS_STATE_PTR ws;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "2 %d\n", XgksMRecSize(2));
}

XgksMoUpdateWs ( ws, regenflag )
WS_STATE_PTR ws;
Gregen regenflag;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "3 %d %d\n", XgksMRecSize(3), (regenflag == GPERFORM ? 0 : 1));
}

XgksMoDeferWs (ws, defer_mode, regen_mode)
WS_STATE_PTR ws;
Gdefmode defer_mode;
Girgmode regen_mode;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint defer, regen;

	if (defer_mode == GASAP) defer = 0;
	else if (defer_mode == GBNIG) defer = 1;
	else if (defer_mode == GBNIL) defer = 2;
	else  defer = 3;

	if (regen_mode == GSUPPRESSED) regen = 0;		/* c1137 */
	else regen = 1;

	fprintf(ws->mfp, "4 %d %d %d\n", XgksMRecSize(4), defer, regen);
}

XgksMoMessage (ws, string)
WS_STATE_PTR ws;
Gchar *string;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	 fprintf(ws->mfp, "5 %d %d %s\n", STRLEN(string)+1+XgksMRecSize(5), STRLEN(string), string);/* c1176 */
}

/*
 * This routine is suitable for 
 *
 *	POLYLINE    -- code == 11,
 *	POLYMARKER  -- code == 12,
 *	FILLAREA    -- code == 14
 *	
 *	to do their metafile output when destination ws is know
 */
XgksMoGraphicOutputToWs (ws, code, num_pt, pos)
WS_STATE_PTR ws;
Gint code;
Gint num_pt;
Gpoint *pos;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint	 i;

	fprintf(ws->mfp, "%d %d %d ",code, (num_pt*POINT_SIZE)+XgksMRecSize(code), num_pt);
	for (i=0; i<num_pt; i++) {
		fprintf(ws->mfp, "%f %f ", pos->x, pos->y);
		pos++;
	}
	fprintf(ws->mfp, "\n");
}
/*
 * This routine is suitable for 
 *
 *	POLYLINE    -- code == 11,
 *	POLYMARKER  -- code == 12,
 *	FILLAREA    -- code == 14
 *	
 *	to do their metafile output when destination ws is unknown
 */
XgksMoGraphicOutput (code, num_pt, pos)
Gint code;
Gint num_pt;
Gpoint *pos;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint	 i, cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) {
				  fprintf(xgks_state.activews[cnt].ws->mfp, "%d %d %d ",code, 
					(num_pt*POINT_SIZE)+XgksMRecSize(code), num_pt);
				  for (i=0; i<num_pt; i++) {
					fprintf(xgks_state.activews[cnt].ws->mfp, "%f %f ", pos->x, pos->y);
					pos++;
				  }
				  fprintf(xgks_state.activews[cnt].ws->mfp, "\n");
			}
		}
}

XgksMoTextToWs (ws, at, string)
WS_STATE_PTR ws;
Gpoint *at;
Gchar *string;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint length;

 	length = STRLEN (string);	/* c1176 */
	fprintf(ws->mfp, "13 %d %f %f %d %s\n", 
			length+1+XgksMRecSize(13), at->x, at->y, length, string);
}

XgksMoText (at, string)
Gpoint *at;
Gchar *string;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt,length;

 	length = STRLEN (string);	/* c1176 */
        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO)  
				fprintf(xgks_state.activews[cnt].ws->mfp, "13 %d %f %f %d %s\n", 
					length+1+XgksMRecSize(13), at->x, at->y, length, string);
	}
}

XgksMoCellArrayToWs (ws, ll, ur, lr, row, colour, dim)
WS_STATE_PTR ws;
Gpoint *ll, *ur, *lr;
Gint   row, *colour; 						/* c1137 */
Gipoint *dim;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint  i,j, size;

	size = dim->x * dim->y;
	fprintf(ws->mfp, "15 %d %f %f %f %f %f %f %d %d ",
		  XgksMRecSize(15)+(size*INT_SIZE), 
		  ll->x, ll->y, ur->x, ur->y, lr->x, lr->y, dim->x, dim->y);
	for (i=0; i<dim->y; i++)
		for (j=0; j<row; j++)
			if (j<dim->x) fprintf(ws->mfp, "%d ", colour[i*row+j]);
	fprintf(ws->mfp, "\n");
}

XgksMoCellArray (ll, ur, lr, row, colour, dim)
Gpoint *ll, *ur, *lr;
Gint   *colour, row; 
Gipoint *dim;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt, i, j, size;

	size = dim->x * dim->y;
        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO)   {
				fprintf(xgks_state.activews[cnt].ws->mfp, "15 %d %f %f %f %f %f %f %d %d ",
				  XgksMRecSize(15)+(size*INT_SIZE), 
				  ll->x, ll->y, ur->x, ur->y, lr->x, lr->y, dim->x, dim->y);
				for (i=0; i<dim->y; i++)
					for (j=0; j<row; j++)
						if (j<dim->x) fprintf(xgks_state.activews[cnt].ws->mfp, "%d ", colour[i*row+j]);
				fprintf(xgks_state.activews[cnt].ws->mfp, "\n");
			}
	}
}

XgksMoSetGraphicSizeOnWs (ws, code, size)
WS_STATE_PTR ws;
Gint code;
Gfloat size;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "%d %d %f\n",code, XgksMRecSize(code), size);
}

XgksMoSetGraphicSize (code, size)
Gint code;
Gfloat size;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "%d %d %f\n",code, XgksMRecSize(code), size);
        }
}

XgksMoCloseSegOnWs (ws)
WS_STATE_PTR ws;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "%d %d\n",82, XgksMRecSize(82));
}

XgksMoSetGraphicAttrOnWs (ws, code, attr)
WS_STATE_PTR ws;
Gint code;
Gint attr;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "%d %d %d\n",code, XgksMRecSize(code), attr);
}

XgksMoSetGraphicAttr (code, attr)
Gint code;
Gint attr;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "%d %d %d\n",code, XgksMRecSize(code), attr);
        }
}

XgksMoSetTextFPOnWs (ws, txfp)
WS_STATE_PTR ws;
Gtxfp *txfp;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint prec;

	if (txfp->prec == GSTRING) prec = 0;
	else if (txfp->prec == GCHAR) prec = 1;
	else prec = 2;

	fprintf(ws->mfp, "30 %d %d %d\n", XgksMRecSize(30),txfp->font, prec);
}

XgksMoSetTextFP (txfp)
Gtxfp *txfp;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt,prec;

	if (txfp->prec == GSTRING) prec = 0;
	else if (txfp->prec == GCHAR) prec = 1;
	else prec = 2;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "30 %d %d %d\n", XgksMRecSize(30),txfp->font, prec);
        }
}

XgksMoSetCharUpOnWs (ws)
WS_STATE_PTR ws;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gpoint ndc_up, ndc_base;

	XgksComputeVec (&ndc_up, &ndc_base);
	fprintf(ws->mfp, "34 %d %f %f %f %f\n", XgksMRecSize(34), ndc_up.x, ndc_up.y, ndc_base.x, ndc_base.y);
}

XgksMoSetCharUp ()
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;
	Gpoint ndc_up, ndc_base;

	XgksComputeVec (&ndc_up, &ndc_base);
        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "34 %d %f %f %f %f\n", 
					XgksMRecSize(34), ndc_up.x, ndc_up.y, ndc_base.x, ndc_base.y);
        }
}

XgksMoSetTextPathOnWs (ws, path)
WS_STATE_PTR ws;
Gtxpath path;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint txpath;

	if (path == GTP_RIGHT) txpath = 0;
	else if (path == GTP_LEFT) txpath = 1;
	else if (path == GTP_UP) txpath = 2;
	else txpath = 3;

   	fprintf(ws->mfp, "35 %d %d\n", XgksMRecSize(35), txpath);
}

XgksMoSetTextPath (path)
Gtxpath path;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint txpath, cnt;

	if (path == GTP_RIGHT) txpath = 0;
	else if (path == GTP_LEFT) txpath = 1;
	else if (path == GTP_UP) txpath = 2;
	else txpath = 3;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "35 %d %d\n", XgksMRecSize(35), txpath);
        }
}

XgksMoSetTextAlignOnWs (ws, align)
WS_STATE_PTR ws;
Gtxalign *align;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint hor, ver;

	if (align->hor == GTH_NORMAL) hor = 0;
	else if (align->hor == GTH_LEFT) hor = 1;
	else if (align->hor == GTH_CENTRE) hor = 2;
	else hor = 3;

	if (align->ver == GTV_NORMAL) ver = 0;
	else if (align->ver == GTV_TOP) ver = 1;
	else if (align->ver == GTV_CAP) ver = 2;
	else if (align->ver == GTV_HALF) ver = 3;
	else if (align->ver == GTV_BASE) ver = 4;
	else ver = 5;

	fprintf(ws->mfp, "36 %d %d %d\n",XgksMRecSize(36), hor, ver);
}

XgksMoSetTextAlign (align)
Gtxalign *align;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint hor, ver, cnt;

	if (align->hor == GTH_NORMAL) hor = 0;
	else if (align->hor == GTH_LEFT) hor = 1;
	else if (align->hor == GTH_CENTRE) hor = 2;
	else hor = 3;

	if (align->ver == GTV_NORMAL) ver = 0;
	else if (align->ver == GTV_TOP) ver = 1;
	else if (align->ver == GTV_CAP) ver = 2;
	else if (align->ver == GTV_HALF) ver = 3;
	else if (align->ver == GTV_BASE) ver = 4;
	else ver = 5;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "36 %d %d %d\n",XgksMRecSize(36), hor, ver);
        }
}

XgksMoSetFillIntStyleOnWs (ws, style)
WS_STATE_PTR ws;
Gflinter style;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint intstyle;

	if (style == GHOLLOW) intstyle = 0;
	else if (style == GSOLID) intstyle = 1;
	else if (style == GPATTERN) intstyle = 2;
	else intstyle = 3;

	fprintf(ws->mfp, "38 %d %d\n", XgksMRecSize(38), intstyle);
}

XgksMoSetFillIntStyle (style)
Gflinter style;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();

	Gint intstyle, cnt;

	if (style == GHOLLOW) intstyle = 0;
	else if (style == GSOLID) intstyle = 1;
	else if (style == GPATTERN) intstyle = 2;
	else intstyle = 3;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "38 %d %d\n", XgksMRecSize(38), intstyle);
        }
}

XgksMoSetPatSizeOnWs ( ws )        /* Proc added for c1144 */
WS_STATE_PTR ws;
{
  fprintf(ws->mfp, "41 %d %f %f %f %f\n", XgksMRecSize(41),
    xgks_state.gks_ptattr.widthvec.x, xgks_state.gks_ptattr.widthvec.y,		
    xgks_state.gks_ptattr.heightvec.x, xgks_state.gks_ptattr.heightvec.y); /*c1147*/
}

XgksMoSetPatRefOnWs ( ws )        /* Proc added for c1144 */
WS_STATE_PTR ws;
{
  fprintf(ws->mfp, "42 %d %f %f\n", XgksMRecSize(42),
    xgks_state.gks_ptattr.ptp.x, xgks_state.gks_ptattr.ptp.y);		/*c1147*/
}

XgksMoSetPatSize ()        /* Proc added for c1144 */
{
Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			  fprintf(xgks_state.activews[cnt].ws->mfp, "41 %d %f %f %f %f\n", XgksMRecSize(41),
			    xgks_state.gks_ptattr.widthvec.x, xgks_state.gks_ptattr.widthvec.y,
			    xgks_state.gks_ptattr.heightvec.x, xgks_state.gks_ptattr.heightvec.y);				/*c1147*/
	}
}

XgksMoSetPatRef ()        /* Proc added for c1144 */
{
Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			  fprintf(xgks_state.activews[cnt].ws->mfp, "42 %d %f %f\n", XgksMRecSize(42),
			    xgks_state.gks_ptattr.ptp.x, xgks_state.gks_ptattr.ptp.y); /*c1147*/
	}
}

XgksMoSetAsfOnWs (ws)
WS_STATE_PTR ws;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

   	fprintf(ws->mfp, "43 %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n", XgksMRecSize(43),
	  (xgks_state.gks_lnattr.type == GBUNDLED ? 0 : 1),		/*c1147*/
	  (xgks_state.gks_lnattr.width == GBUNDLED ? 0 : 1),		/*c1147*/
	  (xgks_state.gks_lnattr.colour == GBUNDLED ? 0 : 1),		/*c1147*/
	  (xgks_state.gks_mkattr.type == GBUNDLED ? 0 : 1),
	  (xgks_state.gks_mkattr.size == GBUNDLED ? 0 : 1),
	  (xgks_state.gks_mkattr.colour == GBUNDLED ? 0 : 1),
	  (xgks_state.gks_txattr.fp == GBUNDLED ? 0 : 1),
	  (xgks_state.gks_txattr.tx_exp == GBUNDLED ? 0 : 1),	/* c1179 */
	  (xgks_state.gks_txattr.space == GBUNDLED ? 0 : 1),
	  (xgks_state.gks_txattr.colour == GBUNDLED ? 0 : 1),
	  (xgks_state.gks_flattr.inter == GBUNDLED ? 0 : 1),
	  (xgks_state.gks_flattr.style == GBUNDLED ? 0 : 1),
	  (xgks_state.gks_flattr.colour == GBUNDLED ? 0 : 1));
}

XgksMoSetAsf ()
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO)  
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "43 %d %d %d %d %d %d %d %d %d %d %d %d %d %d\n",
				  XgksMRecSize(43),
	  			  (xgks_state.gks_lnattr.type == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_lnattr.width == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_lnattr.colour == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_mkattr.type == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_mkattr.size == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_mkattr.colour == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_txattr.fp == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_txattr.tx_exp == GBUNDLED ? 0 : 1),	/* c1179 */
	  			  (xgks_state.gks_txattr.space == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_txattr.colour == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_flattr.inter == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_flattr.style == GBUNDLED ? 0 : 1),
	  			  (xgks_state.gks_flattr.colour == GBUNDLED ? 0 : 1));
        }
}

XgksMoSetLineMarkRep (ws, code, idx, type, size, colour)
WS_STATE_PTR ws;
Gint code, idx, type, colour;
Gfloat size;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "%d %d %d %d %f %d\n", code, XgksMRecSize(code), idx, type, size, colour);
}

XgksMoSetTextRep (ws, idx, rep)
WS_STATE_PTR ws;
Gint idx;
Gtxbundl *rep;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint prec;

	if (rep->fp.prec == GSTRING) prec = 0;
	else if (rep->fp.prec == GCHAR) prec = 1;
	else prec = 2;

	fprintf (ws->mfp, "53 %d %d %d %d %f %f %d\n", XgksMRecSize(53),
		idx, rep->fp.font, prec, rep->ch_exp, rep->space, rep->colour);	/* c1179 */
}

XgksMoSetFillRep (ws, idx, rep)
WS_STATE_PTR ws;
Gint idx;
Gflbundl *rep;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint intstyle;

	if (rep->inter == GHOLLOW) intstyle = 0;
	else if (rep->inter == GSOLID) intstyle = 1;
	else if (rep->inter == GPATTERN) intstyle = 2;
	else intstyle = 3;

	fprintf(ws->mfp, "54 %d %d %d %d %d\n", XgksMRecSize(54), idx, intstyle, rep->style, rep->colour);
}

XgksMoSetPatRep (ws, idx, rep)
WS_STATE_PTR ws;
Gint idx;
Gptbundl *rep;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint size, i;

	size = rep->size.x * rep->size.y;

	fprintf(ws->mfp, "55 %d %d %d %d ", (size*INT_SIZE)+XgksMRecSize(55), idx, rep->size.x, rep->size.y);  /*AUG*/
	
	for (i=0; i<size; i++)
		fprintf(ws->mfp, "%d ",rep->array[i]);

	fprintf(ws->mfp, "\n");
}

XgksMoSetColourRep (ws, idx, rep)
WS_STATE_PTR ws;
Gint idx;
Gcobundl *rep;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "56 %d %d %f %f %f\n", XgksMRecSize(56), idx, rep->red, rep->green, rep->blue);
}

XgksMoSetClipOnWs (ws, rect)
WS_STATE_PTR ws;
Glimit *rect;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "61 %d %f %f %f %f\n", XgksMRecSize(61), rect->xmin, rect->xmax, rect->ymin, rect->ymax);
}

XgksMoSetClip (rect)
Glimit *rect;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "61 %d %f %f %f %f\n", 
					XgksMRecSize(61), rect->xmin, rect->xmax, rect->ymin, rect->ymax);
        }
}

XgksMoSetLimit (ws, code, rect)
WS_STATE_PTR ws;
Gint code;
Glimit *rect;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "%d %d %f %f %f %f\n", code, XgksMRecSize(code), 
		rect->xmin, rect->xmax, rect->ymin, rect->ymax);
}

XgksMoCloseSeg ()
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "82 %d\n", XgksMRecSize(82));
        }
}

XgksMoRenameSeg (old, new)
Gint old, new;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "83 %d %d %d\n", XgksMRecSize(83), old, new);
        }
}

XgksMoSetSegTransOnWs (ws, name, matrix)
WS_STATE_PTR ws;
Gint name;
Gfloat matrix[2][3];
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "91 %d %d %f %f %f %f %f %f \n", XgksMRecSize(91), name, 
		 matrix[0][0], matrix[0][1], matrix[0][2], 
		 matrix[1][0], matrix[1][1], matrix[1][2]);
}

XgksMoSetSegTrans (name, matrix)
Gint name;
Gfloat matrix[2][3];
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "91 %d %d %f %f %f %f %f %f \n",
				 XgksMRecSize(91), name, 
				 matrix[0][0], matrix[0][1], matrix[0][2], 
				 matrix[1][0], matrix[1][1], matrix[1][2]);
        }
}

XgksMoSetSegAttrOnWs (ws, name, code, attr)
WS_STATE_PTR ws;
Gint name, code, attr;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "%d %d %d %d\n", code, XgksMRecSize(code), name, attr);
}

XgksMoSetSegVis (name, vis)
Gint name;
Gsegvis vis;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "92 %d %d %d\n", 
					XgksMRecSize(92), name, (vis == GVISIBLE ? 0 : 1));
        }
}

XgksMoSetSegHiLight (name, hilight)
Gint name;
Gseghi hilight;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "93 %d %d %d\n",
					XgksMRecSize(93), name, (hilight == GNORMAL ? 0 : 1));
        }
}

XgksMoSetSegPriOnWs (ws, name, pri)
WS_STATE_PTR ws;
Gint name;
Gfloat pri;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	fprintf(ws->mfp, "94 %d %d %f\n", XgksMRecSize(94), name, pri);
}

XgksMoSetSegPri (name, pri)
Gint name;
Gfloat pri;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "94 %d %d %f\n",
					XgksMRecSize(94), name, pri);
        }
}

XgksMoSetSegDet (name, det)
Gint name;
Gsegdet det;
{
	/* 
	 * declare local functions
	 */
	static XgksMRecSize();		/*MIT*/

	Gint cnt;

        for(cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {           /* try to find MO */
                if (xgks_state.activews[cnt].ws_id != INVALID) 
			if (xgks_state.activews[cnt].ws->ewstype == MO) 
			   	fprintf(xgks_state.activews[cnt].ws->mfp, "95 %d %d %d\n", 
					XgksMRecSize(95), name, (det == GUNDETECTABLE ? 0 : 1));
        }
}

/* c1140:  following function added to do write initial MO gks attributes */
XgksMoActivateWs (ws)						/* c1140 */
WS_STATE_PTR ws;
{
	XgksMoSetClipOnWs( ws, &xgks_state.cliprec.rec );

	XgksSetLineAttrMo( ws, &xgks_state.gks_lnattr );
	XgksSetMarkAttrMo( ws, &xgks_state.gks_mkattr );
	XgksSetTextAttrMo( ws, &xgks_state.gks_txattr, &xgks_state.gks_chattr ); /*c1147*/
	XgksSetFillPatAttrMo( ws, &xgks_state.gks_flattr, &xgks_state.gks_ptattr ); /*c1147*/

	XgksMoSetPatSizeOnWs ( ws );	/* c1144 */
	XgksMoSetPatRefOnWs ( ws );	/* c1144 */
	XgksMoSetAsfOnWs( ws );
	XgksMoSetGraphicAttrOnWs( ws, 44, xgks_state.gks_pick_id );	/*c1147*/
}

static Gint XgksMInstall (ws_id)				/* c1068 */
Gint ws_id;
{
	Gint i;

	for (i=0; i<MAX_OPEN_WS; i++) 
		if (GksmInfo[i].ws_id == INVALID) {
                	GksmInfo[i].ws_id = ws_id;
                	return(i);
        	}
	return (INVALID);
}

static void XgksMDelete (ws_id)		/*MIT*/
Gint ws_id;
{
	Gint i;

	for (i=0; i< MAX_OPEN_WS; i++) 
		if (GksmInfo[i].ws_id == ws_id) {
			GksmInfo[i].CurItem.type = INVALID;
			GksmInfo[i].CurItem.length = INVALID;
			GksmInfo[i].ws_id = INVALID;
			GksmInfo[i].GksmEmpty = TRUE;
			return ;
		}
}

static Gint XgksMFindKey (ws_id)				/* c1068 */
Gint ws_id;
{
	Gint i;

	for (i=0; i< MAX_OPEN_WS; i++) 
		if (GksmInfo[i].ws_id == ws_id) return(i);
	return (INVALID);
}

static XgksValidGksMItem (type)
Gint type;
{
	if (type>= 0 && type<= 6) return (OK);
	if (type>=11 && type<=16) return (OK);
	if (type>=21 && type<=44) return (OK);
	if (type>=51 && type<=56) return (OK);
	if (type==61) return (OK);
	if (type==71 || type==72) return (OK);
	if (type>=81 && type<=84) return (OK);
	if (type>=91 && type<=95) return (OK);
	if (type>100) return (OK);			/* AUG */
	return(INVALID);
}

static XgksMRecSize (type)
Gint type;
{
	switch (type) {

	case 0  :
	case 2  :
	case 82 : return (0);

	case 1  :
        case 3  :
        case 21 :
        case 22 :
        case 24 :
        case 25 :
        case 26 :
        case 28 :
        case 29 :
        case 33 :
        case 35 :
        case 37 :
        case 38 :
        case 39 :
        case 40 :
        case 44 :
        case 81 :
        case 84 : return (sizeof(XGKSMONE));
		  
 	case 4  :
        case 30 :
        case 36 :
        case 83 :
        case 92 :
        case 93 :
        case 95 : return (sizeof(XGKSMTWO));

	case 5  : return (sizeof(XGKSMMESG));

 	case 11 :
        case 12 :
        case 14 : return(sizeof(XGKSMGRAPH));

	case 13 : return(sizeof(XGKSMTEXT));

	case 15 : return(sizeof(XGKSMCELLARRAY));

	case 23 :
        case 27 :
        case 31 :
        case 32 : return(sizeof(XGKSMSIZE));

	case 34 : return(sizeof(XGKSMCHARVEC));

	case 43 : return(sizeof(XGKSMASF));

	case 41 : return(sizeof(XGKSMPATSIZ));    /* c1144 */

	case 42 : return(sizeof(XGKSMPATREF));    /* c1144 */

	case 51 :
        case 52 : return(sizeof(XGKSMLMREP));

	case 53 : return(sizeof(XGKSMTEXTREP));

	case 54 : return(sizeof(XGKSMFILLREP));

 	case 55 : return(sizeof(XGKSMPATREP));

	case 56 : return(sizeof(XGKSMCOLOURREP));

	case 61 :
        case 71 :
        case 72 : return(sizeof(XGKSMLIMIT));

	case 91 : return(sizeof(XGKSMSEGTRAN));

	case 94 : return(sizeof(XGKSMSEGPRI));

	default : return(INVALID);
	}
}

static Gint XgksInputData (fp, key, record)			/* c1068 */
Gfile *fp;
Gint key;						/* AUG */
Gchar *record;
{
	/* declare local functions */				/* c1068 */
	static Gint XgksFeoln();				/* c1068 */

	XGKSMONE	*ptr1;
	XGKSMTWO	*ptr2;
	XGKSMMESG	*msg;
	XGKSMGRAPH	*graph;
	XGKSMTEXT	*text;
	XGKSMSIZE	*size;
	XGKSMCHARVEC	*vec;
	XGKSMASF	*asf;
	XGKSMLMREP	*lmrep;
	XGKSMTEXTREP	*txrep;
	XGKSMFILLREP	*flrep;
	XGKSMPATREP	*patrep;
	XGKSMCOLOURREP	*corep;
	XGKSMLIMIT	*limit;
	XGKSMSEGTRAN	*tran;
	XGKSMSEGPRI	*pri;
	XGKSMCELLARRAY	*cell;
	XGKSMPATREF     *patref;	/* c1144 */
	XGKSMPATSIZ     *patsiz;	/* c1144 */
	Gint   i,j;
	Gint type;							/* AUG */
	Gint readcnt = 1;					/* c1068 */

        
        type=GksmInfo[key].CurItem.type;				/* AUG */
	clearerr( fp );						/* c1068 */

        if(type>100) { 						/* c1068 */
		for(i=0;i<GksmInfo[key].CurItem.length;i++) {	/* c1068 */
			READCHR(fp, *(record+i));		/* c1068 */
			if (ferror( fp )) {			/* c1068 */
				return( MF_FILE_ERR );		/* c1068 */
			}					/* c1068 */
			if (*(record + i) == '\n') {		/* c1068 */
				return( MF_ITEM_ERR );		/* c1068 */
			}					/* c1068 */
		}						/* c1068 */
		return( METAFILE_OK );				/* c1068 */
	}							/* c1068 */

	switch (type) {

	case 0  :
	case 2  :
	case 82 : *record = NULL;
		  break;

	case 1  :
        case 3  :
        case 21 :
        case 22 :
        case 24 :
        case 25 :
        case 26 :
        case 28 :
        case 29 :
        case 33 :
        case 35 :
        case 37 :
        case 38 :
        case 39 :
        case 40 :
        case 44 :
        case 81 :
        case 84 : ptr1 = (XGKSMONE *)record;
		  if (XgksFeoln( fp ))				/* c1068 */
		  {						/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, ptr1->flag);		/* c1068 */
		  break;

 	case 4  :
        case 30 :
        case 36 :
        case 83 :
        case 92 :
        case 93 :
        case 95 : ptr2 = (XGKSMTWO *)record;
		  if (XgksFeoln( fp ))				/* c1068 */
		  {						/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, ptr2->item1);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp ))				/* c1068 */
		  {						/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, ptr2->item2);		/* c1068 */
		  break;

	case 5  : msg = (XGKSMMESG *)record;
		  msg->string = &(record[sizeof(XGKSMMESG)]);
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, msg->strlen);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READCHR (fp, msg->string[0]);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (msg->string[0] == '\n') {			/* c1068 */
	  		readcnt = 0;				/* c1068 */
			break;					/* c1068 */
		  }						/* c1068 */
		  for (i=1; i<msg->strlen; i++) {		/* c1068 */
			readcnt = READCHR (fp, msg->string[i]);	/* c1068 */
		  	if (!readcnt) break;			/* c1068 */
			if (msg->string[i] == '\n') {		/* c1068 */
		  		readcnt = 0;			/* c1068 */
				break;				/* c1068 */
			}					/* c1068 */
		  }						/* c1068 */
		  msg->string[i] = NULL;
		  break;

 	case 11 :
        case 12 :
        case 14 : graph = (XGKSMGRAPH *)record;
		  graph->pts = (Gpoint *) &(record[sizeof(XGKSMGRAPH)]);
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, graph->num_pts);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  for (i=0; i<graph->num_pts; i++) {		/* c1068 */
		  	  if (XgksFeoln( fp )) {		/* c1068 */
		  		readcnt = 0;			/* c1068 */
		  		break;				/* c1068 */
		  	  }					/* c1068 */
			  readcnt = READFTP (fp, graph->pts[i].x);										/* c1068 */
			  if (!readcnt) break;			/* c1068 */
		  	  if (XgksFeoln( fp )) {		/* c1068 */
		  		readcnt = 0;			/* c1068 */
		  		break;				/* c1068 */
		  	  }					/* c1068 */
			  readcnt = READFTP (fp, graph->pts[i].y);										/* c1068 */
			  if (!readcnt) break;			/* c1068 */
		  }						/* c1068 */
		  break;

	case 13 : text = (XGKSMTEXT *)record;
                  text->string = &(record[sizeof(XGKSMTEXT)]);
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
                  readcnt = READFTP (fp, text->location.x);	/* c1068 */
                  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
                  readcnt = READFTP (fp, text->location.y);	/* c1068 */
                  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
                  readcnt = READINT (fp, text->strlen);		/* c1068 */
                  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READCHR (fp, text->string[0]);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
                  if (text->string[0] == '\n') {		/* c1068 */
			  readcnt = 0;				/* c1068 */
			  break;				/* c1068 */
		  }						/* c1068 */
                  for (i=1; i<text->strlen; i++) {		/* c1068 */
                          readcnt = READCHR(fp, text->string[i]);										/* c1068 */
			  if (text->string[i] == '\n') {	/* c1068 */
				  readcnt = 0;			/* c1068 */
				  break;			/* c1068 */
			  }					/* c1068 */
			  if (!readcnt) break;			/* c1068 */
		  }						/* c1068 */
                  text->string[i] = NULL;
                  break;

	case 15 : cell = (XGKSMCELLARRAY *)record;
		  cell->colour = (Gint *) &(record[sizeof(XGKSMCELLARRAY)]);
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, cell->ll.x);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, cell->ll.y);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, cell->ur.x);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, cell->ur.y);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, cell->lr.x);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, cell->lr.y);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, cell->dim.x);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, cell->dim.y);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  j = cell->dim.x * cell->dim.y;
		  for (i=0; i<j; i++) {				/* c1068 */
		  	if (XgksFeoln( fp )) {			/* c1068 */
		  		readcnt = 0;			/* c1068 */
		  		break;				/* c1068 */
		  	}					/* c1068 */
			readcnt = READINT(fp, cell->colour[i]);	/* c1068 */
			if (!readcnt) break;			/* c1068 */
		  }						/* c1068 */
		  break;

	case 23 :
        case 27 :
        case 31 :
        case 32 : size = (XGKSMSIZE *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, size->size);		/* c1068 */
		  break;

	case 34 : vec = (XGKSMCHARVEC *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, vec->up.x);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, vec->up.y);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, vec->base.x);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, vec->base.y);		/* c1068 */
		  break;

	case 41 : patsiz = (XGKSMPATSIZ *)record;               /* c1144 */
		  if (XgksFeoln( fp )) {			/* c1144 */
		  	readcnt = 0;				/* c1144 */
		  	break;					/* c1144 */
		  }						/* c1144 */
		  readcnt = READFTP (fp, patsiz->wid.x);	/* c1144 */
		  if (!readcnt) break;				/* c1144 */
		  if (XgksFeoln( fp )) {			/* c1144 */
		  	readcnt = 0;				/* c1144 */
		  	break;					/* c1144 */
		  }						/* c1144 */
		  readcnt = READFTP (fp, patsiz->wid.y);	/* c1144 */
		  if (!readcnt) break;				/* c1144 */
		  if (XgksFeoln( fp )) {			/* c1144 */
		  	readcnt = 0;				/* c1144 */
		  	break;					/* c1144 */
		  }
		  readcnt = READFTP (fp, patsiz->hgt.x);	/* c1144 */
		  if (!readcnt) break;				/* c1144 */
		  if (XgksFeoln( fp )) {			/* c1144 */
		  	readcnt = 0;				/* c1144 */
		  	break;					/* c1144 */
		  }						/* c1144 */
		  readcnt = READFTP (fp, patsiz->hgt.y);	/* c1144 */
		  break;					/* c1144 */

	case 42 : patref = (XGKSMPATREF *)record;    		/* c1144 */
		  if (XgksFeoln( fp )) {			/* c1144 */
		  	readcnt = 0;				/* c1144 */
		  	break;					/* c1144 */
		  }						/* c1144 */
		  readcnt = READFTP (fp, patref->ref.x);	/* c1144 */
		  if (!readcnt) break;				/* c1144 */
		  if (XgksFeoln( fp )) {			/* c1144 */
		  	readcnt = 0;				/* c1144 */
		  	break;					/* c1144 */
		  }						/* c1144 */
		  readcnt = READFTP (fp, patref->ref.y);	/* c1144 */
		  break;

	case 43 : asf = (XGKSMASF *)record;
		  for (i=0; i<13; i++) {			/* c1068 */
		  	if (XgksFeoln( fp )) {			/* c1068 */
		  		readcnt = 0;			/* c1068 */
		  		break;				/* c1068 */
		  	}					/* c1068 */
			readcnt = READINT (fp, asf->asf[i]);	/* c1068 */
			if (!readcnt) break;			/* c1068 */
		  }						/* c1068 */
		  break;

	case 51 :
        case 52 : lmrep = (XGKSMLMREP *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, lmrep->idx);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, lmrep->style);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, lmrep->size);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, lmrep->colour);		/* c1068 */
		  break;

	case 53 : txrep = (XGKSMTEXTREP *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, txrep->idx);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, txrep->font);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, txrep->prec);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, txrep->tx_exp);  /* c1179 c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, txrep->space);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, txrep->colour);		/* c1068 */
		  break;

	case 54 : flrep = (XGKSMFILLREP *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, flrep->idx);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, flrep->intstyle);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, flrep->style);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, flrep->colour);		/* c1068 */
		  break;

 	case 55 : patrep = (XGKSMPATREP *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, patrep->idx);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, patrep->size.x);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, patrep->size.y);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  patrep->array = (Gint *) &(record[sizeof(XGKSMPATREP)]);
                  j = patrep->size.x * patrep->size.y;
                  for (i=0; i<j ;i++) {				/* c1068 */
		  	  if (XgksFeoln( fp )) {		/* c1068 */
		  		readcnt = 0;			/* c1068 */
		  		break;				/* c1068 */
		  	  }					/* c1068 */
                	  readcnt = READINT(fp, patrep->array[i]);										/* c1068 */
                	  if (!readcnt) break;			/* c1068 */
		  }						/* c1068 */
                  break;

	case 56 : corep = (XGKSMCOLOURREP *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, corep->idx);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, corep->red);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, corep->green);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, corep->blue);		/* c1068 */
		  break;

	case 61 :
        case 71 :
        case 72 : limit = (XGKSMLIMIT *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, limit->rect.xmin);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, limit->rect.xmax);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, limit->rect.ymin);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, limit->rect.ymax);	/* c1068 */
		  break;

	case 91 : tran = (XGKSMSEGTRAN *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, tran->name);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, tran->matrix[0][0]);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, tran->matrix[0][1]);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, tran->matrix[0][2]);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, tran->matrix[1][0]);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, tran->matrix[1][1]);	/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, tran->matrix[1][2]);	/* c1068 */
		  break;

	case 94 : pri = (XGKSMSEGPRI *)record;
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READINT (fp, pri->name);		/* c1068 */
		  if (!readcnt) break;				/* c1068 */
		  if (XgksFeoln( fp )) {			/* c1068 */
		  	readcnt = 0;				/* c1068 */
		  	break;					/* c1068 */
		  }						/* c1068 */
		  readcnt = READFTP (fp, pri->pri);		/* c1068 */
		  break;

	default : readcnt = 0;					/* c1068 */
		  break;
	}

	if (!readcnt) {						/* c1068 */
		if (ferror( fp )) {				/* c1068 */
			return( MF_FILE_ERR );			/* c1068 */
		}						/* c1068 */
		else {						/* c1068 */
			return( MF_DATA_ERR );			/* c1068 */
		}						/* c1068 */
	}							/* c1068 */
	else {							/* c1068 */
		return( METAFILE_OK );				/* c1068 */
	}							/* c1068 */

}

static Gint XgksExecData (type, record)
Gint type;
Gchar *record;
{
	XGKSMONE	*ptr1;
	XGKSMTWO	*ptr2;
	XGKSMMESG	*msg;
	XGKSMGRAPH	*graph;
	XGKSMTEXT	*text;
	XGKSMSIZE	*size;
	XGKSMCHARVEC	*vec;
	XGKSMASF	*asf;
	XGKSMLMREP	*lmrep;
	XGKSMTEXTREP	*txrep;
	XGKSMFILLREP	*flrep;
	XGKSMPATREP	*patrep;
	XGKSMCOLOURREP	*corep;
	XGKSMLIMIT	*limit;
	XGKSMSEGTRAN	*tran;
	XGKSMSEGPRI	*pri;
	XGKSMCELLARRAY  *cell;
	XGKSMPATREF     *patref;	/* c1144 */
	XGKSMPATSIZ     *patsiz;	/* c1144 */
	OUT_PRIMI	*primi;
	Gtxpath		path;
	Gflinter	inter;
	Gdefmode	defmode;
	Gtxfp		txfp;
	Gsegattr	segattr;
	Gtxalign	txalign;
	Glnbundl	lnrep;
	Gmkbundl	mkrep;
	Gtxbundl	textrep;
	Gflbundl	fillrep;
	Gptbundl	ptrep;
	Gcobundl	colourrep;
	Gasfs		asfs;
	Gint  	        cnt, i,j;
	Gpoint		*pts;
	Gpoint          siz;	/* c1144 */
	Gfloat		height;

	switch (type) {

	case 0  : break;					/* c1138 */

	case 2  : for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gredrawsegws (xgks_state.activews[cnt].ws_id);
				}
			}
        	  }
		  break;
	case 82 : /* PTR c1073:  only need to call gcloseseg() once, */
		  /*    not for each workstation                     */
                  gcloseseg();
		  break;

	case 1  : ptr1 = (XGKSMONE *)record;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
                		gclearws (xgks_state.activews[cnt].ws_id, (ptr1->flag == 0 ? GCONDITIONALLY : GALWAYS));
			}
        	  }
		  break;
        case 3  : ptr1 = (XGKSMONE *)record;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gupdatews (xgks_state.activews[cnt].ws_id, (ptr1->flag == 0 ? GPERFORM : GPOSTPONE)); 
				}
			}
        	  }
		  break;
        case 21 : ptr1 = (XGKSMONE *)record;
		  gsetlineind (ptr1->flag);
		  break;
        case 22 : ptr1 = (XGKSMONE *)record;
		  gsetlinetype (ptr1->flag);
		  break;
        case 24 : ptr1 = (XGKSMONE *)record;
		  gsetlinecolourind (ptr1->flag);
		  break;
        case 25 : ptr1 = (XGKSMONE *)record;
		  gsetmarkerind (ptr1->flag);
		  break;
        case 26 : ptr1 = (XGKSMONE *)record;
		  gsetmarkertype (ptr1->flag);
		  break;
        case 28 : ptr1 = (XGKSMONE *)record;
		  gsetmarkercolourind (ptr1->flag);
		  break;
        case 29 : ptr1 = (XGKSMONE *)record;
		  gsettextind (ptr1->flag);
		  break;
        case 33 : ptr1 = (XGKSMONE *)record;
		  gsettextcolourind (ptr1->flag);
		  break;
        case 35 : ptr1 = (XGKSMONE *)record;
		  if (ptr1->flag == 0) path = GTP_RIGHT;
		  else if (ptr1->flag == 1) path = GTP_LEFT;
		  else if (ptr1->flag == 2) path = GTP_UP;
		  else path = GTP_DOWN;
		  gsettextpath (path);
		  break;
        case 37 : ptr1 = (XGKSMONE *)record;
		  gsetfillind (ptr1->flag);
		  break;
        case 38 : ptr1 = (XGKSMONE *)record;
		  if (ptr1->flag == 0) inter = GHOLLOW;
		  else if (ptr1->flag == 1) inter = GSOLID;
		  else if (ptr1->flag == 2) inter = GPATTERN;
		  else inter = GHATCH;
		  gsetfillintstyle (inter);
		  break;
        case 39 : ptr1 = (XGKSMONE *)record;
		  gsetfillstyleind (ptr1->flag);
		  break;
        case 40 : ptr1 = (XGKSMONE *)record;
		  gsetfillcolourind (ptr1->flag);
		  break;
	case 41 : patsiz = (XGKSMPATSIZ *)record;    	/* c1144 */
		  siz.x = patsiz->wid.x;		/* c1144 */
		  siz.y = patsiz->hgt.y;		/* c1144 */
		  gsetpatsize (&siz);			/* c1144 */
		  break;				/* c1144 */
	case 42 : patref = (XGKSMPATREF *)record;    	/* c1144 */
		  gsetpatrefpt (patref);		/* c1144 */
		  break;				/* c1144 */
        case 44 : ptr1 = (XGKSMONE *)record;
		  gsetpickid (ptr1->flag);
		  break;
        case 81 : ptr1 = (XGKSMONE *)record;
		  gcreateseg (ptr1->flag);
		  break;
        case 84 : ptr1 = (XGKSMONE *)record;
		  gdelseg (ptr1->flag);
		  break;

 	case 4  : ptr2 = (XGKSMTWO *)record;
		  if (ptr2->item1 == 0) defmode = GASAP;
		  else if (ptr2->item1 == 1) defmode = GBNIG;
		  else if (ptr2->item1 == 2) defmode = GBNIL;
		  else  defmode = GASTI;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
		  			gsetdeferst (xgks_state.activews[cnt].ws_id, defmode,(ptr2->item2==0 ? GALLOWED : GSUPPRESSED));
				}
			}
        	  }
		  break;
        case 30 : ptr2 = (XGKSMTWO *)record;
		  txfp.font = ptr2->item1;
		  if (ptr2->item2 == 0) txfp.prec = GSTRING;
		  else if (ptr2->item2 == 1) txfp.prec = GCHAR;
		  else txfp.prec = GSTROKE;
		  gsettextfontprec (&txfp);
		  break;
        case 36 : ptr2 = (XGKSMTWO *)record;
		  if (ptr2->item1 == 0) txalign.hor = GTH_NORMAL;
		  else if (ptr2->item1 == 1) txalign.hor = GTH_LEFT;
		  else if (ptr2->item1 == 2) txalign.hor = GTH_CENTRE;
		  else  txalign.hor = GTH_RIGHT;
		  if (ptr2->item2 == 0) txalign.ver = GTV_NORMAL;
		  else if (ptr2->item2 == 1) txalign.ver = GTV_TOP;
		  else if (ptr2->item2 == 2) txalign.ver = GTV_CAP;
		  else if (ptr2->item2 == 3) txalign.ver = GTV_HALF;
		  else if (ptr2->item2 == 4) txalign.ver = GTV_BASE;
		  else txalign.ver = GTV_BOTTOM;
		  gsettextalign (&txalign);
		  break;
        case 83 : ptr2 = (XGKSMTWO *)record;
		  grenameseg (ptr2->item1, ptr2->item2);
		  break;
        case 92 : ptr2 = (XGKSMTWO *)record;
		  segattr.seg = ptr2->item1;
		  ginqsegattr (&segattr);
		  segattr.vis = (ptr2->item2 == 0 ? GVISIBLE : GINVISIBLE);
		  gsetsegattr (ptr2->item1, &segattr);
		  break;
        case 93 : ptr2 = (XGKSMTWO *)record;
		  segattr.seg = ptr2->item1;
		  ginqsegattr (&segattr);
		  segattr.hilight = (ptr2->item2 == 0 ? GNORMAL : GHIGHLIGHTED);
		  gsetsegattr (ptr2->item1, &segattr);
		  break;
        case 95 : ptr2 = (XGKSMTWO *)record;
		  segattr.seg = ptr2->item1;
		  ginqsegattr (&segattr);
		  segattr.det = (ptr2->item2 == 0 ? GUNDETECTABLE : GDETECTABLE);
		  gsetsegattr (ptr2->item1, &segattr);
		  break;

	case 5  : msg = (XGKSMMESG *)record;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gmessage (xgks_state.activews[cnt].ws_id, msg->string);
				}
			}
        	  }
		  break;

 	case 11 : graph = (XGKSMGRAPH *)record;
		  GKSERROR (((pts = (Gpoint *) malloc (graph->num_pts*sizeof(Gpoint)))==NULL), 300, errXgksExecData)
		  for (i=0; i<graph->num_pts; i++)
			NdcToWc (&(graph->pts[i]), &(pts[i]));
               	  gpolyline (graph->num_pts, pts);
		  free (pts);
		  break;
        case 12 : graph = (XGKSMGRAPH *)record;
		  GKSERROR (((pts = (Gpoint *) malloc (graph->num_pts*sizeof(Gpoint)))==NULL), 300, errXgksExecData)
		  for (i=0; i<graph->num_pts; i++)
			NdcToWc (&(graph->pts[i]), &(pts[i]));
		  gpolymarker (graph->num_pts, pts);
		  free (pts);
		  break;
        case 14 : graph = (XGKSMGRAPH *)record;
		  GKSERROR (((pts = (Gpoint *) malloc (graph->num_pts*sizeof(Gpoint)))==NULL), 300, errXgksExecData)
		  for (i=0; i<graph->num_pts; i++)
			NdcToWc (&(graph->pts[i]), &(pts[i]));
		  gfillarea (graph->num_pts, pts);
		  free (pts);
		  break;

	case 13 : text = (XGKSMTEXT *)record;
		  GKSERROR (((pts = (Gpoint *) malloc (sizeof(Gpoint)))==NULL), 300, errXgksExecData)
		  NdcToWc (&(text->location), pts);
                  gtext (pts, text->string);
		  free (pts);
                  break;

	case 15 : cell = (XGKSMCELLARRAY *)record;
		  GKSERROR (((primi = XgksNewPrimi())==NULL), 300, errXgksExecData)
		  primi->pid = CELL_ARRAY;
		  primi->primi.cell_array.dim = cell->dim;
		  /* rowsize is equal to cell->dim.x */		/* c1138 */
                  primi->primi.cell_array.rowsize = cell->dim.x; /* c1138 */
		  j = cell->dim.x * cell->dim.y;
		  GKSERROR (((primi->primi.cell_array.colour=(Gint *)malloc(j*sizeof(Gint)))==NULL),300, errXgksExecData)
		  primi->primi.cell_array.ll = cell->ll;
		  primi->primi.cell_array.ur = cell->ur;
		  primi->primi.cell_array.lr = cell->lr;
		  primi->primi.cell_array.ul.x = cell->ll.x + (cell->ur.x - cell->lr.x);
		  primi->primi.cell_array.ul.y = cell->ll.y + (cell->ur.y - cell->lr.y);
		  for (i=0; i<j; i++)
			primi->primi.cell_array.colour[i] = cell->colour[i];
		  XgksProcessPrimi (primi);
		  if (MO_OPENED == TRUE)
			XgksMoCellArray (&(primi->primi.cell_array.ll), &(primi->primi.cell_array.ur),
                        &(primi->primi.cell_array.lr), primi->primi.cell_array.rowsize, primi->primi.cell_array.colour,
                        &(primi->primi.cell_array.dim));
		  free (primi->primi.cell_array.colour);
		  free (primi);
		  break;

	case 23 : size = (XGKSMSIZE *)record;
		  gsetlinewidth (size->size);
		  break;
        case 27 : size = (XGKSMSIZE *)record;
		  gsetmarkersize (size->size);
		  break;
        case 31 : size = (XGKSMSIZE *)record;
		  gsetcharexpan (size->size);
		  break;
        case 32 : size = (XGKSMSIZE *)record;
		  gsetcharspace (size->size);
		  break;

	case 34 : vec = (XGKSMCHARVEC *)record;
		  gsetcharup (&(vec->up));
		  NdcToWc (&(vec->up), &(vec->base))
		  height = sqrt((vec->base.x*vec->base.x)+(vec->base.y*vec->base.y));
		  gsetcharheight (height);
		  break;

	case 43 : asf = (XGKSMASF *)record;
		  asfs.ln_type  = (asf->asf[0]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.ln_width = (asf->asf[1]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.ln_colour = (asf->asf[2]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.mk_type  = (asf->asf[3]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.mk_size  = (asf->asf[4]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.mk_colour = (asf->asf[5]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.tx_fp    = (asf->asf[6]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.tx_exp   = (asf->asf[7]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.tx_space = (asf->asf[8]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.tx_colour = (asf->asf[9]  == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.fl_inter = (asf->asf[10] == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.fl_style = (asf->asf[11] == 0 ? GBUNDLED : GINDIVIDUAL);
		  asfs.fl_colour = (asf->asf[12] == 0 ? GBUNDLED : GINDIVIDUAL);
		  gsetasf (&asfs);
		  break;

	case 51 : lmrep = (XGKSMLMREP *)record;
		  lnrep.type  = lmrep->style;
		  lnrep.width = lmrep->size;
		  lnrep.colour = lmrep->colour;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gsetlinerep (xgks_state.activews[cnt].ws_id, lmrep->idx, &lnrep);
				}
			}
        	  }
		  break;
        case 52 : lmrep = (XGKSMLMREP *)record;
		  mkrep.type  = lmrep->style;
		  mkrep.size  = lmrep->size;
		  mkrep.colour = lmrep->colour;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gsetmarkerrep (xgks_state.activews[cnt].ws_id, lmrep->idx, &mkrep);
				}
			}
        	  }
		  break;

	case 53 : txrep = (XGKSMTEXTREP *)record;
		  textrep.fp.font = txrep->font;
		  textrep.ch_exp  = txrep->tx_exp;	/* c1179 */
		  textrep.space	  = txrep->space;
		  textrep.colour   = txrep->colour;
		  if (txrep->prec == 0) textrep.fp.prec = GSTRING;
		  else if (txrep->prec == 1) textrep.fp.prec = GCHAR;
		  else textrep.fp.prec = GSTROKE;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gsettextrep (xgks_state.activews[cnt].ws_id, txrep->idx, &textrep);
				}
			}
        	  }
		  break;

	case 54 : flrep = (XGKSMFILLREP *)record;
		  fillrep.style = flrep->style;
		  fillrep.colour = flrep->colour;
		  if (flrep->intstyle == 0) fillrep.inter = GHOLLOW;
		  else if (flrep->intstyle == 1) fillrep.inter = GSOLID;
		  else if (flrep->intstyle == 2) fillrep.inter = GPATTERN;
		  else fillrep.inter = GHATCH;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gsetfillrep (xgks_state.activews[cnt].ws_id, flrep->idx, &fillrep);
				}
			}
        	  }
		  break;

 	case 55 : patrep = (XGKSMPATREP *)record;
		  ptrep.size.x = patrep->size.x;
		  ptrep.size.y = patrep->size.y;
		  j = ptrep.size.x * ptrep.size.y;
		  GKSERROR (((ptrep.array = (Gint *) malloc (j*sizeof(Gint)))==NULL), 300, errXgksExecData)
                  for (i=0; i<j ;i++) 
                	  ptrep.array[i] = patrep->array[i];
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
		  			gsetpatrep (xgks_state.activews[cnt].ws_id, patrep->idx, &ptrep);
				}
			}
        	  }
		  break;

	case 56 : corep = (XGKSMCOLOURREP *)record;
		  colourrep.red = corep->red;
		  colourrep.green = corep->green;
		  colourrep.blue = corep->blue;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gsetcolourrep (xgks_state.activews[cnt].ws_id, corep->idx, &colourrep);
				}
			}
        	  }
		  break;

	case 61 : limit = (XGKSMLIMIT *)record;
		  xgks_state.cliprec.rec = limit->rect;
		  break;
        case 71 : limit = (XGKSMLIMIT *)record;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gsetwswindow (xgks_state.activews[cnt].ws_id, &(limit->rect));
				}
			}
        	  }
		  break;
        case 72 : limit = (XGKSMLIMIT *)record;
		  for (cnt=0; cnt<MAX_ACTIVE_WS; cnt++) {          /* Do it on all active ws */
                	if (xgks_state.activews[cnt].ws_id != INVALID) {
				/* c1138:  don't do this on WISS */
				if (xgks_state.activews[cnt].ws->ewstype != WISS) {
                			gsetwsviewport (xgks_state.activews[cnt].ws_id, &(limit->rect));
				}
			}
        	  }
		  break;

	case 91 : tran = (XGKSMSEGTRAN *)record;
		  segattr.seg = tran->name;
		  ginqsegattr (&segattr);
		  segattr.segtran[0][0] = tran->matrix[0][0];
		  segattr.segtran[0][1] = tran->matrix[0][1];
		  segattr.segtran[0][2] = tran->matrix[0][2];
		  segattr.segtran[1][0] = tran->matrix[1][0];
		  segattr.segtran[1][1] = tran->matrix[1][1];
		  segattr.segtran[1][2] = tran->matrix[1][2];
		  gsetsegattr (tran->name, &segattr);
		  break;

	case 94 : pri = (XGKSMSEGPRI *)record;
		  segattr.seg = pri->name;
		  ginqsegattr (&segattr);
		  segattr.pri = pri->pri;
		  gsetsegattr (pri->name, &segattr);
		  break;

	default : return(1);					/* c1068 */
	}
	return(0);
}

static Gint XgksGetNextMi (fp, key)		/*MIT*/		/* c1068 */
Gfile *fp;
Gint key;
{

	/* declare local functions */				/* c1068 */
	static Gint XgksFeoln();				/* c1068 */

	fscanf( fp, " " );					/* c1068 */
	if (feof(fp)) {
		GksmInfo[key].GksmEmpty = TRUE;
		GksmInfo[key].CurItem.type = INVALID;
		GksmInfo[key].CurItem.length = INVALID;
		return( METAFILE_OK );				/* c1068 */
	}
	if (READINT (fp, GksmInfo[key].CurItem.type) != 1)	/* c1068 */
		return( MF_FILE_ERR );				/* c1068 */
	if (feof(fp)) {						/* c1068 */
		GksmInfo[key].GksmEmpty = TRUE;			/* c1068 */
		GksmInfo[key].CurItem.type = INVALID;		/* c1068 */
		GksmInfo[key].CurItem.length = INVALID;		/* c1068 */
		return( MF_FILE_ERR );				/* c1068 */
	}							/* c1068 */
	if (XgksFeoln( fp ))					/* c1068 */
		return( MF_FILE_ERR );				/* c1068 */
	if (READINT (fp, GksmInfo[key].CurItem.length) != 1)	/* c1068 */
		return( MF_FILE_ERR );				/* c1068 */
	GksmInfo[key].GksmEmpty = FALSE;			/* c1068 */
	return( METAFILE_OK );					/* c1068 */

}

static Gchar *XgksMAuthor () {		/*MIT*/
	
	Gint i;

	passwd = getpwuid (getuid());
	strncat (buffer, passwd->pw_name, 15);
	for (i=STRLEN(passwd->pw_name); i<15; i++)	/* c1176 */
		buffer[i] = ' ';
	strcat (&(buffer[15]), " at :");
	gethostname (&(buffer[20]), 20);
	for (i=STRLEN(buffer); i<40; i++)	/* c1176 */
		buffer[i] = ' ';
	buffer[40] = NULL;
	return (buffer);
}

static Gchar *XgksMDate()		/*MIT*/
{
	struct timeval	 gmt_time;				/* c1137 */
	struct timezone	 our_tmzn;				/* c1137 */
	long		 clock;					/* c1137 */
	struct tm	*our_time;				/* c1137 */

	/* go thru agonizing UNIX stuff to get date */		/* c1137 */
	gettimeofday (&gmt_time, &our_tmzn);			/* c1137 */
	clock = gmt_time.tv_sec;				/* c1137 */
	our_time = localtime (&clock);				/* c1137 */
	/* build date string for metafile header */		/* c1137 */
	sprintf (date, "%02d/%02d/%02d",			/* c1137 */
		/* if year > 2000, truncate to 2 digits */	/* c1137 */
		(our_time->tm_year < 100 ?			/* c1137 */
			our_time->tm_year :			/* c1137 */
			our_time->tm_year - 100),		/* c1137 */
		/* month goes 0-11 so add 1 */			/* c1137 */
		our_time->tm_mon+1,				/* c1137 */
		/* day is fine, anyway */			/* c1137 */
		our_time->tm_mday);				/* c1137 */

	return (date);
}

static Gint XgksFeoln( fp )					/* c1068 */
	FILE *fp;						/* c1068 */
{								/* c1068 */
	int i;							/* c1068 */
								/* c1068 */
	if ((i = getc( fp )) == EOF) return( TRUE );		/* c1068 */
	ungetc( i, fp );					/* c1068 */
	if (i == '\n') return( TRUE );				/* c1068 */
	else return( FALSE );					/* c1068 */
}								/* c1068 */
