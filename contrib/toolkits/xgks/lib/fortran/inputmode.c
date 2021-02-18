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
 * FORTRAN to C binding for XGKS
 *
 * GKS Set Input Mode functions :
 *	gslcm_
 *	gsskm_
 *	gsvlm_
 *	gschm_
 *	gspkm_
 *	gsstm_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: inputmode.c,v 4.0 89/08/31 18:31:06 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/inputmode.c,v $
 * 
 * $Log:	inputmode.c,v $
 * Revision 4.0  89/08/31  18:31:06  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.7  89/06/26  15:22:12  amy
 * DCR d1	Declare errfp external so that
 * 		error macros may reference it.
 * 
 * Revision 1.6  88/12/05  15:19:03  owens
 * changed errfp to errfpp
 * 
 * Revision 1.4  88/12/05  14:25:27  bhaim
 * Changes for Code II
 * 
 * Revision 1.3  88/10/11  17:00:50  todd
 * Added Global_errnum initialization.
 * 
 * Revision 1.2  88/09/27  16:22:13  mike
 * Fixed Code Review problems: prologues, casts, variable names, and range checks
 * 
 * Revision 1.1  88/09/27  08:17:02  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: inputmode.c,v 4.0 89/08/31 18:31:06 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

/* errfp is needed for error macros d1 */
extern FILE *errfp;              /* d1 */

/*$F
 *
 * gslcm - Set Locator Mode
 *
 * int *wkid - pointer to workstation id
 * int *lcdnr - pointer to locator device number
 * int *mode - pointer to operating mode
 * int *esw - pointer to echo switch
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.128
 */
gslcm_ (wkid, lcdnr, mode, esw)
int *wkid;
int *lcdnr;
int *mode;
int *esw;

{

debug ( ("Set Locator Mode %d  %d %d %d \n", *wkid, *lcdnr, *mode, *esw) );

OPERATINGMODE (*mode, errgsetlocmode);
ECHOSWITCH (*esw, errgsetlocmode);

gsetlocmode ((Gint)*wkid, 
             (Gint)*lcdnr, 
             (Gimode)*mode, 
             (Gesw)((*esw) ? FORT_GNECHO : FORT_GECHO)); 


}

/*$F
 * gsskm - Set Stroke Mode
 *
 * int *wkid - pointer to workstation id
 * int *skdnr - pointer to stroke device number
 * int *mode - pointer to operating mode
 * int *esw - pointer to echo switch
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.129
 */
gsskm_ (wkid, skdnr, mode, esw)
int *wkid;
int *skdnr;
int *mode;
int *esw;

{

debug ( ("Set Stroke Mode %d %d %d %d \n", *wkid, *skdnr, *mode, *esw) );

OPERATINGMODE (*mode, errgsetstrokemode);
ECHOSWITCH (*esw, errgsetstrokemode);

gsetstrokemode ((Gint)*wkid,
                (Gint)*skdnr, 
                (Gimode)*mode, 
                (Gesw)((*esw) ? FORT_GNECHO : FORT_GECHO));


}

/*$F
 *
 * gsvlm - Set Valuator Mode
 *
 * int *wkid - pointer to workstation id
 * int *lcdnr - pointer to valuator device number
 * int *mode - pointer to operating mode
 * int *esw - pointer to echo switch
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.128
 */
gsvlm_ (wkid, vldnr, mode, esw)
int *wkid;
int *vldnr;
int *mode;
int *esw;

{

debug ( ("Set Valuator Mode %d %d %d %d \n", *wkid, *vldnr, *mode, *esw) );

OPERATINGMODE (*mode, errgsetvalmode);
ECHOSWITCH (*esw, errgsetvalmode);

gsetvalmode ((Gint)*wkid, 
             (Gint)*vldnr, 
             (Gimode)*mode,
             (Gesw)((*esw) ? FORT_GNECHO : FORT_GECHO));


}

/*$F
 *
 * gschm - Set Choice Mode
 *
 * int *wkid - pointer to workstation id
 * int *lcdnr - pointer to choice device number
 * int *mode - pointer to operating mode
 * int *esw - pointer to echo switch
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.130
 */
gschm_ (wkid, chdnr, mode, esw)
int *wkid;
int *chdnr;
int *mode;
int *esw;

{

debug ( ("Set Choice Mode %d %d %d %d \n", *wkid, *chdnr, *mode, *esw) );

OPERATINGMODE (*mode, errgsetchoicemode);
ECHOSWITCH (*esw, errgsetchoicemode);

gsetchoicemode ((Gint)*wkid, 
                (Gint)*chdnr, 
                (Gimode)*mode, 
                (Gesw)((*esw) ? FORT_GNECHO : FORT_GECHO));


}

/*$F
 *
 * gspkm - Set Pick Mode
 *
 * int *wkid - pointer to workstation id
 * int *lcdnr - pointer to pick device number
 * int *mode - pointer to operating mode
 * int *esw - pointer to echo switch
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.128
 */
gspkm_ (wkid, pkdnr, mode, esw)
int *wkid;
int *pkdnr;
int *mode;
int *esw;

{

debug ( ("Set Pick Mode %d %d %d %d \n", *wkid, *pkdnr, *mode, *esw ));

OPERATINGMODE (*mode, errgsetpickmode);
ECHOSWITCH (*esw, errgsetpickmode);

gsetpickmode ((Gint)*wkid, 
              (Gint)*pkdnr, 
              (Gimode)*mode,
              (Gesw)((*esw) ? FORT_GNECHO : FORT_GECHO));


}

/*$F
 *
 * gsstm - Set String Mode
 *
 * int *wkid - pointer to workstation id
 * int *lcdnr - pointer to string device number
 * int *mode - pointer to operating mode
 * int *esw - pointer to echo switch
 *
 * Returns: 2000, in addition to ANSI standard errors for this function.
 *
 * See also: ANSI standard p.128
 */
gsstm_ (wkid, stdnr, mode, esw)
int *wkid;
int *stdnr;
int *mode;
int *esw;

{

debug ( ("Set String Mode %d %d %d %d \n", *wkid, *stdnr, *mode, *esw) );

OPERATINGMODE (*mode, errgsetstringmode);
ECHOSWITCH (*esw, errgsetstringmode);

gsetstringmode ((Gint)*wkid, 
                (Gint)*stdnr, 
                (Gimode)*mode, 
                (Gesw)((*esw) ? FORT_GNECHO : FORT_GECHO));


}
