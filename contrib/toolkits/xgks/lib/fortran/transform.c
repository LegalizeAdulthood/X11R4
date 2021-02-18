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
 * GKS Transformation functions :
 *	gswn_
 *	gsvp_
 *	gsvpip_
 *	gselnt_
 *	gsclip_
 *	gswkwn_
 *	gswkvp_
 *        
 * David Berkowitz
 * Bruce Haimowitz
 * TCS Development
 * Cambridge MA
 *
 * August 31 1988
 *
 * $Header: transform.c,v 4.0 89/08/31 18:37:29 amy Exp $ 
 *
 * $Source: /andrew/Xgks/source/xgks.bld/fortran/RCS/transform.c,v $
 * 
 * $Log:	transform.c,v $
 * Revision 4.0  89/08/31  18:37:29  amy
 * Changed IBM copyright for MIT distribution.
 * 
 * Revision 1.15  89/06/26  16:09:45  amy
 * DCR d1	Declare errfp external for use in error macros.
 * 
 * Revision 1.14  89/02/13  15:37:48  amy
 *  
 * 
 * Revision 1.13  89/01/06  09:38:18  todd
 * Added switch for clipswitch.
 * 
 * Revision 1.12  88/12/27  14:35:43  todd
 * Correct enumerated type in set clip.
 * 
 * Revision 1.11  88/12/05  15:20:44  owens
 * changed errfp to errfpp
 * 
 * Revision 1.9  88/12/05  14:27:19  bhaim
 * 
 * 
 * Revision 1.8  88/11/10  10:23:40  bhaim
 * Error handling
 * 
 * Revision 1.7  88/10/11  17:04:25  todd
 * No changes.
 * 
 * Revision 1.6  88/10/11  14:31:40  todd
 * Added line to initialize global_errnum.
 * 
 * Revision 1.5  88/10/03  15:31:24  mike
 * Minor corrections made after Amy\'s review
 * 
 * Revision 1.4  88/09/27  16:22:07  mike
 * Fixed Code Review problems: prologues, casts, variable names, and range checks
 * 
 * Revision 1.3  88/09/27  11:20:28  todd
 * Added includes.
 * 
 * Revision 1.2  88/09/27  10:03:55  todd
 * Added missing functions.
 * 
 * Revision 1.1  88/09/27  08:17:34  todd
 * Initial revision
 * 
 */

  static char *rcsid = "$Header: transform.c,v 4.0 89/08/31 18:37:29 amy Exp $";

#include "fortxgks.h"
#include <xgks.h>

extern FILE *errfp;             /* d1 */

/*$F
 * gswn - Set Window
 *
 * int *tnr - pointer to transformation number
 * float *xmin,
 *       *xmax,
 *       *ymin,
 *       *ymax - pointers to window limits in world coordinates
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.107
 */
gswn_ (tnr, xmin, xmax, ymin, ymax)
int *tnr;
float *xmin, *xmax, *ymin, *ymax;

{
Glimit window;


debug ( ("Set Window %d (%7.2f, %7.2f) (%7.2f, %7.2f) \n", *tnr, *xmin, *ymin, *xmax, *ymax) );

window.xmin = (Gfloat)*xmin;
window.xmax = (Gfloat)*xmax;
window.ymin = (Gfloat)*ymin;
window.ymax = (Gfloat)*ymax;

gsetwindow ((Gint)*tnr, &window);


}

/*$F
 * gsvp - Set Viewport
 *
 * int *tnr - pointer to transformation number
 * float *xmin,
 *       *xmax,
 *       *ymin,
 *       *ymax - pointers to viewport limits in normalized device coordinates
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.107
 */
gsvp_ (tnr, xmin, xmax, ymin, ymax)
int *tnr;
float *xmin, *xmax, *ymin, *ymax;

{
Glimit viewport;


debug ( ("Set Viewport %d (%7.2f, %7.2f) (%7.2f, %7.2f) \n", *tnr, *xmin, *ymin, *xmax, *ymax) );

viewport.xmin = (Gfloat)*xmin;
viewport.xmax = (Gfloat)*xmax;
viewport.ymin = (Gfloat)*ymin;
viewport.ymax = (Gfloat)*ymax;

gsetviewport ((Gint)*tnr, &viewport);


}

/*$F
 * gsvpip - Set Viewport Input Priority
 *
 * int *tnr - pointer to transformation number
 * int *rtnr - pointer to reference transformation number
 * int *relpri - pointer to relative priority
 *
 * Returns: 2000, and ANSI standard errors for this function.
 *
 * See also: ANSI standard p.107
 */
gsvpip_ (tnr, rtnr, relpri)
int *tnr;
int *rtnr;
int *relpri;
{

debug ( ("Set Viewport Input Priority  %d %d %d \n", *tnr, *rtnr, *relpri) );

RELATIVEINPUTPRIORITY (*relpri, errgsetviewportinputpri);

gsetviewportinputpri ((Gint)*tnr, (Gint)*rtnr, (Gvpri)*relpri);


}

/*$F
 * gselnt - Select Normalization Transformation
 *
 * int *tnr - pointer to transformation number
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.108
 */
gselnt_ (tnr)
int *tnr;
{

debug ( ("Select Normalization Transformation %d \n", *tnr) ); 

gselntran ((Gint)*tnr);


}

/*$F
 * gsclip - Set Clipping Indicator
 *
 * int *clsw - pointer to clipping indicator
 *
 * Returns: 2000, and ANSI standard errors for this function.
 *
 * See also: ANSI standard p.108
 */
gsclip_ (clsw)
int *clsw;

{
Gclip clipswitch;

debug ( ("Set Clipping Indicator %d \n", *clsw) );
CLIPPINGINDICATOR (*clsw, errgsetclip);

switch(*clsw)
{
 case FORT_GCLIP: clipswitch = GCLIP;
 break;
 case FORT_GNCLIP: clipswitch = GNOCLIP;
 break;
 default:
      clipswitch = NULL;             /* suppress compiler warning */
      fprintf(stderr,"error: unknown clipping indicator");
}
gsetclip (clipswitch);

}

/*$F
 * gswkwn - Set Workstation Window
 *
 * int *tnr - pointer to workstation identifier
 * float *xmin,
 *       *xmax,
 *       *ymin,
 *       *ymax - pointers to window limits in normalized device coordinates
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.109
 */
gswkwn_ (wkid, xmin, xmax, ymin, ymax)
int *wkid;
float *xmin, *xmax, *ymin, *ymax;
{
Glimit window;


debug ( ("Set Workstation Window %d (%7.2f, %7.2f) (%7.2f, %7.2f) \n", *wkid, *xmin, *ymin, *xmax, *ymax) );

window.xmin = (Gfloat)*xmin;
window.xmax = (Gfloat)*xmax;
window.ymin = (Gfloat)*ymin;
window.ymax = (Gfloat)*ymax;

gsetwswindow ((Gint)*wkid, &window);

}

/*$F
 * gswkvp - Set Workstation Viewport
 *
 * int *wkid - pointer to workstation identifier
 * float *xmin,
 *       *xmax,
 *       *ymin,
 *       *ymax - pointers to viewport limits in device coordinates
 *
 * Returns: ANSI standard errors for this function.
 *
 * See also: ANSI standard p.109
 */
gswkvp_ (wkid, xmin, xmax, ymin, ymax)
int *wkid;
float *xmin, *xmax, *ymin, *ymax;
{
Glimit viewport;


debug ( ("Set Workstation Viewport %d (%7.2f, %7.2f) (%7.2f, %7.2f) \n", *wkid, *xmin, *ymin, *xmax, *ymax) );

viewport.xmin = (Gfloat)*xmin;
viewport.xmax = (Gfloat)*xmax;
viewport.ymin = (Gfloat)*ymin;
viewport.ymax = (Gfloat)*ymax;

gsetwsviewport ((Gint)*wkid, &viewport);


}
