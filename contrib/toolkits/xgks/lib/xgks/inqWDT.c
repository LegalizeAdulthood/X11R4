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
 * $Header: inqWDT.c,v 4.0 89/08/31 16:21:22 amy Exp $
 *
 * $Source: /andrew/Xgks/source/xgks.bld/src/RCS/inqWDT.c,v $
 *
 * $Log:	inqWDT.c,v $
 * Revision 4.0  89/08/31  16:21:22  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 3.11  89/06/05  10:10:48  bruce
 * DCR# d1:	Changed include file name from gks_implement.h
 * 		to gks_implem.h for AIX compiler.
 * PTR# c1176:	Replaced string function calls with macros that check for
 * 		NULL pointers before making the call.
 * 
 * Revision 3.10  89/03/15  14:02:17  bruce
 * PTR c2048:   Change the values returned for the number of available input devices
 * 		from 65K to 10 per input type.
 * 
 * Revision 3.9  89/02/04  15:24:28  amy
 * PTR c1147	Make global vars. and functions private, and static where possible.
 * 
 * Revision 3.8  88/12/08  10:00:48  amy
 * PTR c1130	GKSERROR calls:  changed function name parameter from a character
 * 		string to an enum. type value.
 * Changed spelling from color to colour.
 * 
 * Revision 3.7  88/09/21  09:04:03  amy
 * MIT	ginqwsclass:  changed error checking for valid workstation type.
 * 
 * Revision 3.6  88/08/17  07:42:41  amy
 * No additional changes in August tape version-- no changes made.
 * 
 * Revision 3.5  88/08/11  08:26:49  amy
 * PTR c1012  ginqwsconntype, inqwsdeferupdatest & ginqwsst:
 * Added call to VALID_WSID to check for error 20.
 * 
 * Revision 1.6  88/08/05  13:53:52  todd
 * No Changes Made.
 * 
 * Revision 1.5  88/07/28  15:18:03  owens
 * added check for error 20 for PTR c1012
 * 
 * Revision 1.4  88/07/28  14:37:11  bruce
 * PTR# c1081 + c1079    GKSERROR macros added to the functions
 * inqwsdeferupdatest() and inqwsst() to check for error codes 33, 35, and
 * 36 that relate to improper workstation categories
 * 
 * Revision 1.3  88/07/27  07:08:19  todd
 * No Changes Made (TRG)
 * 
 * Revision 1.2  88/07/26  17:52:10  owens
 * renamed/replaced VALID_WSID macro for PTR c1012 (DWO)
 * 
 * Revision 1.1  88/07/21  10:48:57  david
 * Initial revision
 *  
 *
 */

static char rcsid[]="$Header: inqWDT.c,v 4.0 89/08/31 16:21:22 amy Exp $";

#include "gks_implem.h"                 /* d1 */
#include <strings.h>

/*$F
 * INQUIRE LIST OF AVAILABLE WORKSTATION TYPES
 *
 * Note: The item wstypes->strings is allocated by GKS, and must be freed
 *	by the application after use.  However, the strings pointed to by
 *	wstypes->strings are static and should not be modified or freed.
 *
 * returns: 0, 8
 *
 * See Also: ANSI Standard p.145
 */

static char *WSTList[] = { "MI", "MO", "WISS", NULL };

ginqavailwstypes( wstypes )
	Gstrlist *wstypes;
{
/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqavailwstypes); /*c1147*/

	wstypes->number = 4;
	wstypes->strings = (Gchar **) malloc( sizeof( Gchar *) * 4);
	GKSERROR( (wstypes->strings == NULL), 300, errginqavailwstypes);
	wstypes->strings[0] = WSTList[0];
	wstypes->strings[1] = WSTList[1];
	wstypes->strings[2] = WSTList[2];
	wstypes->strings[3] = WSTList[3];

	return( OK );
}

/*$F
 * ginqdisplayspacesize(ws_type, dspsz) - INQUIRE MAXIMUM DISPLAY SURFACE SIZE
 * 
 * Gchar *ws_type;		asking about this type of workstation.
 * Gdspsize *dspsz;		Out display space size structure
 *
 * errors 0, 8, 22, 23, 31, 33, 36
 *
 * See Also: ANSI Standard p.171
 */
ginqdisplayspacesize(ws_type, dspsz)
	Gchar *ws_type;
	Gdspsize  *dspsz;
{
	EWSTYPE ewstype;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqdisplayspacesize); /*c1147*/

/* check for valid workstation type */
	ewstype = XgksWsTypeToEnum(ws_type);
	GKSERROR( (ewstype == WST_INVALID), 22, errginqdisplayspacesize);
	GKSERROR( (ewstype == MO), 31, errginqdisplayspacesize);
	GKSERROR( (ewstype == MI), 33, errginqdisplayspacesize);
	GKSERROR( (ewstype == WISS), 36, errginqdisplayspacesize);

/* set up the return values, all X workstation look the same. */
	dspsz->units = GDC_OTHER;
	dspsz->device.x = 1280.0;
	dspsz->device.y = 1024.0;
	dspsz->raster.x = 1280;
	dspsz->raster.y = 1024;

	return( OK );
}

/*$F
 * ginqwscategory(ws_type, cat) - INQUIRE WORKSTATION CATEGORY
 *
 * Gchar *ws_type;		asking about this type of workstation.
 * Gwscat *cat;			workstation category GOUTPUT|GINPUT|GOUTIN|GWISS|GMO|GMI.
 *
 * errors 0, 8, 22, 23
 *
 * See Also: ANSI Standard p.170
 */
ginqwscategory(ws_type, cat)
Gchar *ws_type;
Gwscat *cat;
{
	EWSTYPE ewstype;
/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqwscategory); /*c1147*/

/* check for valid workstation type */
	ewstype = XgksWsTypeToEnum( ws_type );

/* set up the return values */
	switch(ewstype) {
	case WISS:
		*cat = GWISS;
		break;
	case MI:
		*cat = GMI;
		break;
	case MO:
		*cat = GMO;
		break;
	case X_WIN:
		*cat = GOUTIN;
		break;
	default:
		GKSERROR( (ewstype == WST_INVALID), 22, errginqwscategory);
		break;
	}
	return( OK );
}

/*$F
 * ginqwsclass(ws_type, class) - INQUIRE WORKSTATION CLASSIFICATION
 *
 * Gchar *ws_type;		asking about this type of workstation.
 * Gwsclass *class;			workstation class, VECTOR | RASTER | OTHER.
 *
 * errors 0, 22, 23, 31, 33, 35, 36
 *
 * See Also: ANSI Standard p.171
 */

ginqwsclass(ws_type, class)
	Gchar *ws_type;
	Gwsclass *class;
{
	EWSTYPE ewstype;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqwsclass); /*c1147*/

/* check for valid workstation type */
	ewstype = XgksWsTypeToEnum( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqwsclass); /*MIT*/
	GKSERROR( (ewstype == WISS || ewstype == MO || ewstype == MI), 39,
		 errginqwsclass);  /*MIT*/

/* set up the return values */
	if (ewstype == X_WIN)		/*MIT*/
	  *class = GRASTER;		/*MIT*/
	return( OK );
}

/*$F
 * ginqmodwsattr(ws_type, dyn) -
 *		INQUIRE DYNAMIC MODIFICATION OF WORKSTATION ATTRIBUTES
 *
 * Gchar *ws_type;		asking about this type of workstation.
 * Gmodws *dyn;                 OUT structure of dynamic modification indications
 *
 * errors 0, 8, 22, 23, 39
 *
 * See Also: ANSI Standard p.172
 */

ginqmodwsattr(ws_type, dyn)
	Gchar *ws_type;
	Gmodws *dyn;
{
	EWSTYPE ewstype;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqmodwsattr); /*c1147*/

/* check for valid workstation type */
	ewstype = XgksWsTypeToEnum( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqmodwsattr);
	GKSERROR( (ewstype != X_WIN), 39, errginqmodwsattr);

/* set up the return values */
	dyn->line = GIMM;
	dyn->mark = GIMM;
	dyn->text = GIMM;
	dyn->fill = GIMM;
	dyn->pat  = GIMM;
	dyn->colour= GIMM;
	dyn->wstran = GIRG;
	return( OK );
}

/*$F
 * ginqdefdeferst(ws_type, def) - INQUIRE DEFUALT DEFERRAL STATE VALUES
 *
 * Gchar *ws_type;		asking about this type of workstation.
 * Gdefer *def;                 OUT default deferral state values structure
 *
 * errors 0, 8, 22, 23, 39
 *
 * See Also: ANSI Standard p.173
 */

ginqdefdeferst(ws_type, def)
	Gchar *ws_type;
	Gdefer *def;
{
	EWSTYPE ewstype;
/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqdefdeferst); /*c1147*/

/* check for valid workstation type */
	ewstype = XgksWsTypeToEnum( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqdefdeferst);
	GKSERROR( (ewstype != X_WIN), 39, errginqdefdeferst);

/* set up the return values */
	def->defmode = GASAP;
	def->irgmode = GALLOWED;

	return( OK );
}

/*$F
 * ginqmaxwssttables(ws_type, tables) -
 *		INQUIRE MAXIMUM LENGTH OF WORKSTATION STATE TABLES
 *
 * Gchar *ws_type;		asking about this type of workstation.
 * Gwstables *tables;           OUT maximum length of workstation state tables
 *
 * errors 0, 8, 22, 23, 39
 *
 * See Also: ANSI Standard p.182
 */

ginqmaxwssttables(ws_type, tables)
	Gchar *ws_type;
	Gwstables *tables;
{
	EWSTYPE ewstype;
/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqmaxwssttables); /*c1147*/

/* check for valid workstation type */
	ewstype = XgksWsTypeToEnum( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqmaxwssttables);
	GKSERROR( (ewstype != X_WIN), 39, errginqmaxwssttables);

/* set up the return values */
	tables->line = 20;
	tables->mark = 20;
	tables->text = 20;
	tables->fill = 20;
	tables->pat  = 20;
	tables->colour= XgksMaxColours( ws_type );

	GKSERROR( tables->colour < 0, 22, errginqmaxwssttables);

	return( OK );
}

/*$F
 * ginqnumavailinput(ws_type, num)- INQUIRE NUMBER OF AVAILABLE LOGICAL INPUT DEVICES
 *
 * Gchar *ws_type;		Workstation type.
 * Gnumdev *num;                OUT number of input devices
 *
 * Errors: 0, 8, 22, 23, 38
 *
 * See Also: ANSI Standard p.184
 */

ginqnumavailinput(ws_type, num)
	Gchar *ws_type;
	Gnumdev *num;
{
	EWSTYPE ewstype;
/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL), 8, errginqnumavailinput); /*c1147*/

/* check for valid workstation type */
	ewstype = XgksWsTypeToEnum( ws_type );
	GKSERROR( (ewstype == WST_INVALID), 22, errginqnumavailinput);
	GKSERROR( (ewstype != X_WIN), 38, errginqnumavailinput);

/* set up the return values */
        num->locator = 10;      /* c2048 */
        num->stroke  = 10;      /* c2048 */
        num->valuator= 10;      /* c2048 */
        num->choice  = 10;      /* c2048 */
        num->pick    = 10;      /* c2048 */
        num->string  = 10;      /* c2048 */

	return( OK );
}

/*$F
 * Inquiry Functions for Workstation State List
 */

/*$F
 * ginqwsconntype(ws_id, ct) - INQUIRE WORKSTATION CONNECTION AND TYPE
 *
 * Gint ws_id;			workstation identifier.
 * Gwsct *ct;                   OUT connection identifier and workstation type
 *	ct->conn & ct->type are malloc'ed by GKS and should be free'ed by the user.
 *
 * errors 0, 7, 20, 25
 *
 * See Also: ANSI Standard p.153
 */

ginqwsconntype(ws_id, ct)
	Gint ws_id;
	Gwsct *ct;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqwsconntype); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqwsconntype)   /* c1012 */

/* check for open workstation id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws=OPEN_WSID(ws_id)) == NULL), 25, errginqwsconntype);  /* c1012 */

/* set up the return values */
	ct->conn = (Gchar *)malloc(STRLEN(ws->conn)+1);		/* c1176 */
	GKSERROR( (ct->conn == NULL), 300, errginqwsconntype);
	STRCPY(ct->conn, ws->conn);		/* c1176 */

	ct->type = (Gchar *)malloc(STRLEN(ws->wstype)+1);	/* c1176 */
	GKSERROR( (ct->type == NULL), 300, errginqwsconntype);
	STRCPY(ct->type, ws->wstype);	/* c1176 */

	return( OK );
}

/*$F
 * ginqwsst(ws_id, state) - INQUIRE WORKSTATION STATE
 *
 * Gint ws_id;			workstation identifier.
 * Gwsstate *state;		current workstation state INACTIVE|ACTIVE
 *
 * errors 0, 7, 20, 25, 33, 35
 *
 * See Also: ANSI Standard p.153
 */

ginqwsst(ws_id, state)
	Gint ws_id;
	Gwsstate *state;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqwsst); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqwsst)   /* c1012 */

/* check for open workstation id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws=OPEN_WSID(ws_id)) == NULL), 25, errginqwsst);  /* c1012 */

/* check for valid workstation category                                 c1081 */
        GKSERROR ( (WS_CAT(ws)==GMI), 33, errginqwsst);    /* c1079 */

        GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errginqwsst); /* c1081 */

/* set up the return values */
	*state = ws->wsstate;

	return( OK );
}

/*$F
 * ginqwsdeferupdatest(ws_id, du) - INQUIRE WORKSTATION DEFERRAL AND UPDATE STATES
 *
 * Gint ws_id;			workstation identifier.
 * Gwsdus *du;			deferral mode.
 *
 * errors 0, 7, 20, 25, 33, 35, 36
 *
 * See Also: ANSI Standard p.154
 */

ginqwsdeferupdatest(ws_id, du)
	Gint ws_id;
	Gwsdus *du;
{
	WS_STATE_PTR ws;

/* check for proper operating state */
	GKSERROR( (xgks_state.gks_state == GGKCL || xgks_state.gks_state == GGKOP), 7, errginqwsdeferupdatest); /*c1147*/

/* check for invalid workstation id */
/* DWO 7/28/88  added check to differentiate between */
/*              error 20 and error 25 for PTR c1012  */
        GKSERROR ( (!VALID_WSID(ws_id)), 20, errginqwsdeferupdatest)   /* c1012 */

/* check for open workstation id */
/* DWO 7/26/88  changed macro name from VALID_WSID */
	GKSERROR( ((ws=OPEN_WSID(ws_id)) == NULL), 25, errginqwsdeferupdatest);  /* c1012 */

/* check for valid workstation category                                 c1081 */
        GKSERROR ( (WS_CAT(ws)==GMI), 33, errginqwsdeferupdatest);    /* c1079 */

        GKSERROR ( (WS_CAT(ws)==GINPUT), 35, errginqwsdeferupdatest); /* c1081 */

        GKSERROR ( (WS_CAT(ws)==GWISS), 36, errginqwsdeferupdatest);  /* c1081 */

/* set up the return values */
	*du = ws->wsdus;

	return( OK );
}
