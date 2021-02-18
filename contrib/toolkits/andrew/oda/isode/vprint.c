/*
Copyright 1988, 1989 Marshall T. Rose

The ISODE is openly available but is NOT in the public domain. You are
allowed and encouraged to take this software and build commercial
products. However, as a condition of use, you are required to "hold
harmless" all contributors.
Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted,
provided that this notice and the reference to this notice appearing
in each software module be retained unaltered, and that the name of
any contributors shall not be used in advertising or publicity pertaining
to distribution of the software without specific written prior permission.
No contributor makes any representations about the suitability of this
software for any purpose. It is provided "as is" without express or
implied warranty.

ALL CONTRIBUTORS DISCLAIM ALL WARRANTIES WITH
REGARD TO THIS SOFTWARE, INCLUDING ALL IMPLIED
WARRANTIES OR MERCHANTIBILITY AND FITNESS. IN
NO EVENT SHALL ANY CONTRIBUTOR BE LIABLE FOR
ANY SPECIAL, INDIRECT, OR CONSEQUENTIAL DAMAGES
OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS
OF USE, DATA OR PROFITS, WHETHER IN ACTION OF
CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR
PERFORMANCE OF THIS SOFWTARE.

As used above, "contributor" includes, but is not limited to:

	The MITRE Corporation
	The Northrop Corporation
	University College London
	The University of Nottingham
	The Wollongong Group, Inc.
	Marshall T. Rose

In particular, the Northrop Coporation provided the initial
sponsorship for the ISODE and teh Wollongong Group, Inc.,
continued supporting this effort.
*/
/* vprint.c - pepy printer support */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/vprint.c,v 1.1 89/09/21 05:56:58 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/vprint.c,v 1.1 89/09/21 05:56:58 mss Exp $
 *
 *
 * $Log:	vprint.c,v $
 * Revision 1.1  89/09/21  05:56:58  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:22:34  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:04:34  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:24:41  mss
 * Initial revision
 * 
 * Revision 6.1  89/03/23  22:27:44  mrose
 * out-the-door
 * 
 * Revision 6.0  89/03/18  23:38:02  mrose
 * Release 5.0
 * 
 */

/*
 *				  NOTICE
 *
 *    Acquisition, use, and distribution of this module and related
 *    materials are subject to the restrictions of a license agreement.
 *    Consult the Preface in the User's Manual for the full terms of
 *    this agreement.
 *
 */


/* LINTLIBRARY */

#include <ctype.h>
#include <stdio.h>
#include <varargs.h>
#include "psap.h"
#include "logger.h"


int	fprintf ();

/*    DATA */

#define	VPRINT(s)	vprint1 (), vwrite ((s)), vprint2 ()

static int vlevel = 0;

static int didname = 0;
static int didvpop = 0;
static int didvpush = 0;
static int docomma = 0;

static char  *py_classlist[] = {
    "UNIVERSAL", "APPLICATION", "", "PRIVATE"
};

static char *vbp = NULL;
static char *vsp;

static IFP   vfnx = fprintf;
static FILE *vfp = stdout;


char   *oct2str (), *newbuf ();

/*    VPUSH/VPOP */

vpush () {
    if (didvpush)
	vwrite ("\n"), didvpush = 0;
    else
	if (!didname && docomma)
	    vwrite (",\n");

    if (didname)
	vwrite (" ");
    else
	if (vfp && vlevel > 0)
	    (*vfnx) (vfp, "%*s", vlevel * 3, "");
    vwrite ("{");
    vlevel++;

    didname = didvpop = docomma = 0, didvpush = 1;
}


vpop () {
    if (didname || docomma)
	vwrite ("\n");

    vlevel--;
    if (!didvpush && vfp && vlevel > 0)
	(*vfnx) (vfp, "%*s", vlevel * 3, "");
    vwrite ("}");
    if (vlevel == 0)
	vwrite ("\n");

    didname = didvpush = 0, didvpop = docomma = vlevel ? 1 : 0;
}

/*    VNAME/VTAG */

vname (name)
char   *name;
{
    if (didvpush)
	vwrite ("\n"), didvpush = 0;
    else
	if (docomma)
	    vwrite (",\n");

    if (vfp && vlevel > 0)
	(*vfnx) (vfp, "%*s", vlevel * 3, "");
    vwrite (name);

    didname = 1;
}


vtag (class, id)
int	class,
	id;
{
    register char *bp;
    static char buffer[BUFSIZ];

    if (didname)
	return;

    bp = buffer;
    *bp++ = '[';
    switch (class) {
	case PE_CLASS_UNIV: 
	case PE_CLASS_APPL: 
	case PE_CLASS_PRIV: 
	    (void) sprintf (bp, "%s ", py_classlist[class]);
	    bp += strlen (bp);
	    break;

	case PE_CLASS_CONT: 
	default:
	    break;
    }
    (void) sprintf (bp, "%d]", id);

    vname (buffer);
}

/*    VPRINT */

#ifndef	lint
vprint (va_alist)
va_dcl
{
    char    buffer[BUFSIZ];
    va_list ap;
    
    vprint1 ();

    va_start (ap);

    _asprintf (buffer, NULLCP, ap);
    
    va_end (ap);

    vwrite (buffer);

    vprint2 ();
}
#else
/* VARARGS */

vprint (fmt)
char   *fmt;
{
    vprintf (fmt);
}
#endif


static	vprint1 ()
{
    if (didvpush) {
	vwrite ("\n"), didvpush = 0;
	goto indent;
    }
    else
	if (didname)
	    vwrite (" ");
	else {
	    if (docomma)
		vwrite (",\n");
indent: ;
	    if (vfp && vlevel > 0)
		(*vfnx) (vfp, "%*s", vlevel * 3, "");
	}
}


static	vprint2 ()
{
    if (vlevel == 0)
	vwrite ("\n");

    didname = didvpop = 0, docomma = vlevel ? 1 : 0;
}

/*  */

static	vwrite (s)
char   *s;
{
    if (vfp)
	(*vfnx) (vfp, "%s", s);
    else {
	register char   c,
		       *cp;

	for (cp = s; *cp; )
	    *vbp++ = (c = *cp++) != '\n' ? c : ' ';
    }
}

/*    VSTRING */

vstring (pe)
register PE	pe;
{
    register PE	    p;

    switch (pe -> pe_form) {
	case PE_FORM_PRIM: 
	case PE_FORM_ICONS:
	    VPRINT (oct2str ((char *) pe -> pe_prim, (int) pe -> pe_len));
	    break;

	case PE_FORM_CONS: 
	    vpush ();
	    for (p = pe -> pe_cons; p; p = p -> pe_next)
		vstring (p);
	    vpop ();
	    break;
    }
}

/*  */

static char   *oct2str (s, len)
register char  *s;
register int	len;
{
    int     ia5ok;
    register int    k;
    register char  *bp,
                   *cp,
                   *dp,
		   *zp;

    ia5ok = 1, k = 0;
    for (dp = (cp = s) + len; cp < dp; cp++) {
	switch (*cp) {
	    case ' ': 
		continue;

	    case '"': 
		break;

	    case '\b':
	    case '\f':
	    case '\n':
	    case '\r':
	    case '\t':
	    case '\\':
		ia5ok = -1, k++;
		continue;

	    case '-': 
		if (cp > s && *(cp + 1) == '-')
		    break;
		continue;

	    default: 
		if (iscntrl (*cp) || isspace (*cp) || (*cp & 0x80))
		    break;
		continue;
	}
	ia5ok = 0;
	break;
    }

    switch (ia5ok) {
	case 1: 
	    zp = newbuf (len + 2);
	    (void) sprintf (zp, "\"%*.*s\"", len, len, s);
	    break;

	case -1: 
	    bp = zp = newbuf (len + k + 2);
	    *bp++ = '"';
	    for (cp = s; cp < dp; cp++)
		if (*cp >= ' ' && *cp != '\\')
		    *bp++ = *cp;
		else {
		    *bp++ = '\\';
		    switch (*cp) {
			case '\b':
			    *bp++ = 'b';
			    break;
			case '\f':
			    *bp++ = 'f';
			    break;
			case '\n':
			    *bp++ = 'n';
			    break;
			case '\r':
			    *bp++ = 'r';
			    break;
			case '\t':
			    *bp++ = 't';
			    break;

			case '\\':
			    *bp++ =  '\\';
			    break;
		    }
		}
	    (void) sprintf (bp, "\"");
	    break;

	case 0: 
	default: 
	    bp = zp = newbuf (len * 2 + 3);
	    *bp++ = '\'';
	    for (cp = s; cp < dp; cp++) {
		(void) sprintf (bp, "%02x", *cp & 0xff);
		bp += strlen (bp);
	    }
	    (void) sprintf (bp, "'H");
	    break;
    }

    return zp;
}

/*  */

char   *bit2str (pe, s)
PE	pe;
char   *s;
{
    int     ia5ok;
    register int    hit,
		    i,
                    j,
                    k;
    register char  *bp,
                   *cp,
		   *zp;

    j = pe -> pe_nbits;
    if ((cp = s) && *++cp) {
	ia5ok = 1, hit = 0;
	for (i = 0; i < j;)
	    if (bit_test (pe, i++) == 1) {
		do {
		    if (!(k = *cp++ & 0xff))
			break;
		    if (k == i) {
			hit += hit ? 2 : 1;
			for (; *cp > ' '; cp++)
			    hit++;
		    }
		    else
			for (; *cp > ' '; cp++)
			    continue;
		} while (k != 0 && k < i);
		if (k == 0 || k > i) {
		    ia5ok = 0;
		    break;
		}
	    }
    }
    else
	ia5ok = 0;

    if (ia5ok) {
	bp = zp = newbuf (hit + 3);
	*bp++ = '{';

	cp = s, cp++;
	for (i = hit = 0; i < j;)
	    if (bit_test (pe, i++) == 1) {
		do {
		    if (!(k = *cp++ & 0xff))
			break;
		    if (k == i) {
			if (hit)
			    *bp++ = ',';
			*bp++ = ' ';
			for (; *cp > ' '; cp++)
			    *bp++ = *cp;
		    }
		    else
			for (; *cp > ' '; cp++)
			    continue;
		} while (k != 0 && k < i);
		if (k == 0 || k > i)
		    break;
		hit++;
	    }

	(void) sprintf (bp, "%s}", hit ? " " : "");
    }
    else {
	bp = zp = newbuf (j + 3);
	*bp++ = '\'';
	for (i = 0; i < j; i++)
	    *bp++ = bit_test (pe, i) ? '1' : '0';
	(void) sprintf (bp, "'B");
    }

    return zp;
}

/*  */

vunknown (pe)
register PE	pe;
{
    int     i;
#ifdef	notyet
    double  j;
#endif
    OID	    oid;
    register PE	    p;

    switch (pe -> pe_form) {
	case PE_FORM_PRIM: 
	    switch (PE_ID (pe -> pe_class, pe -> pe_id)) {
		case PE_ID (PE_CLASS_UNIV, PE_PRIM_BOOL): 
		    if ((i = prim2flag (pe)) == NOTOK)
			goto bad_pe;
		    VPRINT (i ? "TRUE" : "FALSE");
		    break;

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_INT): 
		case PE_ID (PE_CLASS_UNIV, PE_PRIM_ENUM): 
		    if ((i = prim2num (pe)) == NOTOK
			    && pe -> pe_errno != PE_ERR_NONE)
			goto bad_pe;
		    vprint ("%d", i);
		    break;

#ifdef	notyet
		case PE_ID (PE_CLASS_UNIV, PE_PRIM_REAL): 
		    if ((j = prim2real (pe)) == NOTOK
			    && pe -> pe_errno != PE_ERR_NONE)
			goto bad_pe;
		    vprint ("%g", j);
		    break;
#endif

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_BITS): 
		    if ((p = prim2bit (pe)) == NULLPE)
			goto bad_pe;
		    VPRINT (bit2str (p, "\020"));
		    break;

		default: 
	    bad_pe: ;
		    vtag ((int) pe -> pe_class, (int) pe -> pe_id);
		/* fall */

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_OCTS): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_IA5S): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_NUMS): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_PRTS): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_T61S): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_VTXS): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_GENT): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_UTCT): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_GFXS): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_VISS): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_GENS): 
		case PE_ID (PE_CLASS_UNIV, PE_DEFN_CHRS): 
		case PE_ID (PE_CLASS_UNIV, PE_PRIM_ODE): 
		    vstring (pe);
		    break;

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_NULL):
		    VPRINT ("NULL");
		    break;

		case PE_ID (PE_CLASS_UNIV, PE_PRIM_OID): 
		    if ((oid = prim2oid (pe)) == NULLOID)
			goto bad_pe;
		    VPRINT (oid2ode (oid));
		    break;
	    }
	    break;

	case PE_FORM_CONS: 
	    switch (PE_ID (pe -> pe_class, pe -> pe_id)) {
		case PE_ID (PE_CLASS_UNIV, PE_CONS_SEQ):
		case PE_ID (PE_CLASS_UNIV, PE_CONS_SET):
		    break;

		case PE_ID (PE_CLASS_UNIV, PE_CONS_EXTN):
		    (void) print_UNIV_EXTERNAL (pe, 1, NULLIP, NULLVP,
						NULLCP);
		    return;

		default:
		    vtag ((int) pe -> pe_class, (int) pe -> pe_id);
		    break;
	    }
	    vpush ();
	    for (p = pe -> pe_cons; p; p = p -> pe_next)
		vunknown (p);
	    vpop ();
	    break;

	case PE_FORM_ICONS:
	    vtag ((int) pe -> pe_class, (int) pe -> pe_id);
	    vstring (pe);
	    break;
    }
}

/*    VPUSHFP/VPOPFP */

vpushfp (fp, pe, s, rw)
FILE   *fp;
PE	pe;
char   *s;
int	rw;
{
    vpushpp ((caddr_t) fp, fprintf, pe, s, rw);
}

vsetfp (fp, s)
FILE	* fp;
char	* s;
{
    vfp = fp;
    vfnx = fprintf;

    if(s != NULLCP)
	(*vfnx) (vfp, "%s\n", s);

    vlevel = didname = didvpush = didvpop = docomma = 0;
}

vpopfp ()
{
    (*vfnx) (vfp, "-------\n");
    (void) fflush (vfp);

    vpopp ();
}

/*    VPUSHSTR/VPOPSTR */

vpushstr (cp)
char   *cp;
{
    vfp = NULL;
    vbp = vsp = cp;

    vlevel = didname = didvpush = didvpop = docomma = 0;
}


vpopstr ()
{
    while (--vbp >= vsp)
	if (*vbp != ' ')
	    break;
    *++vbp = NULL;

    vfp = stdout;
}

/*    VPUSHPP */

vpushpp (pv, pfnx, pe, text, rw)
caddr_t pv;
IFP	pfnx;
register PE	pe;
char   *text;
int	rw;
{
    vfp = (FILE *) pv, vfnx = pfnx;

    (*vfnx) (vfp, "%s %s", rw ? "read" : "wrote", text ? text : "pdu");
    if (pe -> pe_context != PE_DFLT_CTX)
	(*vfnx) (vfp, ", context %d", pe -> pe_context);
    (*vfnx) (vfp, "\n");

    vlevel = didname = didvpush = didvpop = docomma = 0;
}


vpopp ()
{
    vfp = stdout, vfnx = fprintf;
}

/*    VPDU */

_vpdu (lp, fnx, pe, text, rw)
register LLog *lp;
IFP	fnx;
register PE pe;
char   *text;
int	rw;
{
    register char   *bp;
    char   buffer[BUFSIZ];

    vfp = (FILE *) lp, vfnx = ll_printf;

    bp = buffer;
    (void) sprintf (bp, "%s %s", rw ? "read" : "wrote",
		    text ? text : "pdu");
    bp += strlen (bp);
    if (pe -> pe_context != PE_DFLT_CTX) {
	(void) sprintf (bp, ", context %d", pe -> pe_context);
	bp += strlen (bp);
    }
    LLOG (lp, LLOG_ALL, ("%s", buffer));

    vlevel = didname = didvpush = didvpop = docomma = 0;

    (void) (*fnx) (pe, 1, NULLIP, NULLVP, NULLCP);

    (void) ll_printf (lp, "-------\n");

    (void) ll_sync (lp);

    vfp = stdout, vfnx = fprintf;
}

/*    MISC */

static char *newbuf (i)
int	i;
{
    static unsigned int len = 0;
    static char *bp = NULL;

    if (i++ < len)
	return bp;

    if (bp)
	free (bp);
    if ((bp = malloc ((unsigned int) i)))
	len = i;
    else
	len = 0;

    return bp;
}
