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
 * gks_error.c - GKS functions  gerrorhand()
 *                              gerrorlog()
 *                              gemergencyclosegks()
 *
 */

#define GKS_ERROR_C
#include "gks_implem.h"

Gint gclosews(Gint ws_id);
Gint gclosegks(void);

/* the following tables of gks function names are added for c1130 */
/* when adding or deleting entries from this table, be sure to correct */
/* the range checking if statements located in the errorlog() function */
char    *procname[] =
   {"",
    "gopengks()",
    "gclosegks()",
    "gopenws()",
    "gclosews()",
    "gactivatews()",
    "gdeactivatews()",
    "gclearws()",
    "gredrawsegws()",
    "gupdatews()",
    "gsetdeferst()",
    "gmessage()",
    "gescredrawnotify()",
    "gescsetcolourmask()",
    "gescsetdcsize()",
    "gescstoreprimi()",
    "gescinqxattr()",
    "gpolyline()",
    "gpolymarker()",
    "gtext()",
    "gfillarea()",
    "gcellarray()",
    "ggdp()",
    "gsetlineind()",
    "gsetlinetype()",
    "gsetlinewidth()",
    "gsetlinecolourind()",
    "gsetmarkerind()",
    "gsetmarkertype()",
    "gsetmarkersize()",
    "gsetmarkercolourind()",
    "gsettextind()",
    "gsettextfontprec()",
    "gsetcharexpan()",
    "gsetcharspace()",
    "gsettextcolourind()",
    "gsetcharheight()",
    "gsetcharup()",
    "gsettextpath()",
    "gsettextalign()",
    "gsetfillind()",
    "gsetfillintstyle()",
    "gsetfillstyleind()",
    "gsetfillcolourind()",
    "gsetpatsize()",
    "gsetpatrefpt()",
    "gsetasf()",
    "gsetpickid()",
    "gsetlinerep()",
    "gsetmarkerrep()",
    "gsetpatrep()",
    "gsetfillrep()",
    "gsettextrep()",
    "gsetcolourrep()",
    "gsetwindow()",
    "gsetviewport()",
    "gsetviewportinputpri()",
    "gselntran()",
    "gsetclip()",
    "gsetwswindow()",
    "gsetwsviewport()",
    "gcreateseg()",
    "gcloseseg()",
    "grenameseg()",
    "gdelseg()",
    "gdelsegws()",
    "gassocsegws()",
    "gcopysegws()",
    "ginsertseg()",
    "gsetsegtran()",
    "gsetvis()",
    "gsethighlight()",
    "gsetsegpri()",
    "gsetdet()",
    "gsetsegattr()",
    "ginitloc()",
    "ginitstroke()",
    "ginitval()",
    "ginitchoice()",
    "ginitpick()",
    "ginitstring()",
    "gsetlocmode()",
    "gsetstrokemode()",
    "gsetvalmode()",
    "gsetchoicemode()",
    "gsetpickmode()",
    "gsetstringmode()",
    "greqloc()",
    "greqstroke()",
    "greqval()",
    "greqchoice()",
    "greqpick()",
    "greqstring()",
    "gsampleloc()",
    "gsamplestroke()",
    "gsampleval()",
    "gsamplechoice()",
    "gsamplepick()",
    "gsamplestring()",
    "gawaitevent()",
    "gflushevents()",
    "ggetloc()",
    "ggetstroke()",
    "ggetval()",
    "ggetchoice()",
    "ggetpick()",
    "ggetstring()",
    "gwritegksm()",
    "ggettypegksm()",
    "greadgksm()",
    "ggetgksm()",
    "ginterpret()",
    "gevaltran()",
    "gaccumtran()",
    "gemergencyclosegks()",
    "gerrorhand()",
    "gerrorlog()",
    "ginqactivews()",
    "ginqasf()",
    "ginqassocws()",
    "ginqavailgdp()",
    "ginqavailwstypes()",
    "ginqchoicest()",
    "ginqclip()",
    "ginqcolourfacil()",
    "ginqcolourindices()",
    "ginqcolourrep()",
    "ginqcurntrannum()",
    "ginqdefchoice()",
    "ginqdefdeferst()",
    "ginqdefloc()",
    "ginqdefpick()",
    "ginqdefstring()",
    "ginqdefstroke()",
    "ginqdefval()",
    "ginqdisplayspacesize()",
    "ginqdisplaysize()",
    "ginqfillfacil()",
    "ginqfillindices()",
    "ginqfillrep()",
    "ginqgdp()",
    "ginqindivattr()",
    "ginqinputoverflow()",
    "ginqlevelgks()",
    "ginqlinefacil()",
    "ginqlineindices()",
    "ginqlinerep()",
    "ginqlocst()",
    "ginqmarkerfacil()",
    "ginqmarkerindices()",
    "ginqmarkerrep()",
    "ginqmaxntrannum()",
    "ginqmaxwssttables()",
    "ginqmodsegattr()",
    "ginqmodwsattr()",
    "ginqmoreevents()",
    "ginqnameopenseg()",
    "ginqntrannum()",
    "ginqntran()",
    "ginqnumavailinput()",
    "ginqnumsegpri()",
    "ginqopenws()",
    "ginqopst()",
    "ginqpatfacil()",
    "ginqpatindices()",
    "ginqpatrep()",
    "ginqcurpickid()",
    "ginqpickst()",
    "ginqpixel()",
    "ginqpixelarray()",
    "ginqpixelarraydim()",
    "ginqpredcolourrep()",
    "ginqpredfillrep()",
    "ginqpredlinerep()",
    "ginqpredmarkerrep()",
    "ginqpredpatrep()",
    "ginqpredtextrep()",
    "ginqprimattr()",
    "ginqsegattr()",
    "ginqsegnames()",
    "ginqsegnamesws()",
    "ginqstringst()",
    "ginqstrokest()",
    "ginqtextextent()",
    "ginqtextfacil()",
    "ginqtextindices()",
    "ginqtextrep()",
    "ginqvalst()",
    "ginqwscategory()",
    "ginqwsclass()",
    "ginqwsconntype()",
    "ginqwsdeferupdatest()",
    "ginqwsmaxnum()",
    "ginqwsst()",
    "ginqwstran()",     /* c1130 */
    "gurec()",          /* c1130 */
    "gprec()"           /* c1130 */
   };

/* when adding or deleting entries from this table, be sure to correct */
/* the range checking if statements located in the errorlog() function */
char    *procname1000[] =
   {"XgksChoUpdatePrompt()",
    "XgksEnqueueEvent()",
    "XgksLocUpdatePrompt()",
    "XgksStrUpdatePrompt()",
    "XgksPicUpdatePrompt()",
    "XgksInsertMesgPrimi()",
    "XgksDuplicatePrimi()",
    "XgksSegPrimiTran()",
    "XgksStkUpdatePrompt()",
    "XgksValUpdatePrompt()",
    "XgksDistFillarea()",
    "XgksInitWssFillArea()",
    "XgksDistCellarray()",
    "XgksExecData()",
    "xPolyMarker()",
    "xFillArea()",
    "xInqPixelarray()"
   };

/*$F
 * gemergencyclosegks() - EMERGENCY CLOSE GKS
 *
 * returns: 0 (always)
 *
 * See also: Ansi standard p. 193
 */

Gint gemergencyclosegks(void)
{
        int i;

        if (xgks_state.gks_state == GGKCL) /* already closed, nothing to do *//*c1147*/
                return( OK );

        if (xgks_state.gks_state == GSGOP) /* close any open segment */ /*c1147*/
                gcloseseg();

        if (xgks_state.gks_state != GGKCL && xgks_state.gks_state != GGKOP) { /* any workstations open? */ /*c1147*/
                for(i=0; i<MAX_OPEN_WS; i++)
                        if ( xgks_state.openedws[i].ws_id != INVALID ) { /*c1147*/
                          if (xgks_state.openedws[i].ws->wsstate == GACTIVE) /*c1147*/
                             gdeactivatews( xgks_state.openedws[i].ws_id ); /*c1147*/
                        gclosews( xgks_state.openedws[i].ws_id ); /*c1147*/
                        }
        }
        gclosegks();

        return( OK );
}


/*$F
 *  ERROR LOGGING
 *
 *      Gint errnum;            number for the error that was detected.
 *      Gint funcname;          name of function that detected the error.
 *      Gfile *perrfile;        file where error message is to be printed.
 *
 */

Gint gerrorlog(Gint errnum, Gint funcname, Gfile *perrfile)
{
        Gchar *fname;           /* c1130  */

        /* look up function name - this is printed now instead of the function number */
        /* all fprintf statements reflect this change   *** c1130 ***/
        if ((funcname < 197) && (funcname > 0))
           fname = procname[funcname];
        else
           if ((funcname > 999) && (funcname < 1017))
               fname = procname1000[funcname - 1000];
           else
               fname = "Invalid function name/number";

        if (perrfile == NULL)
                perrfile = stderr;      /* c1130 */

        if (errnum < 0)
                errnum = 0;     /* AUG */

        if (errnum < 201)
                if (GKSErrorMessages0to200[errnum] == (char *)NULL)
                   fprintf(perrfile, "%s %d <Undefined Error>\n", fname, errnum);
                else
                   fprintf(perrfile, "%s %4d %s\n", fname, errnum, GKSErrorMessages0to200[errnum]);
        else
        if ((errnum > 299) && (errnum < 310))
                if (GKSErrorMessages300to309[errnum-300] == (char *)NULL)
                        fprintf(perrfile, "%s %d <Undefined Error>\n", fname, errnum);
                else
                        fprintf(perrfile, "%s %4d %s\n", fname, errnum, GKSErrorMessages300to309[errnum-300]);
        else
        if ((errnum > 1999) && (errnum < 2004)) /* c1130 */
                if (GKSErrorMessages2000to2000[errnum-2000] == (char *)NULL)
                        fprintf(perrfile, "%s %d <Undefined Error>\n", fname, errnum);
                else
                        fprintf(perrfile, "%s %4d %s\n", fname, errnum, GKSErrorMessages2000to2000[errnum-2000]);
        else
                fprintf(perrfile, "%s %d <Undefined Error>\n", fname, errnum);

        return( OK );
}
