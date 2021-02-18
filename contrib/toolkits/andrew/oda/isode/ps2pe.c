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
/* ps2pe.c - presentation stream to presentation element */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ps2pe.c,v 1.1 89/09/21 05:51:50 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/ps2pe.c,v 1.1 89/09/21 05:51:50 mss Exp $
 *
 *
 * $Log:	ps2pe.c,v $
 * Revision 1.1  89/09/21  05:51:50  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:56:46  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:17:28  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:03:27  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:29:01  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:39:00  mrose
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

#include <stdio.h>
#include "psap.h"
#include "tailor.h"

/*  */

PE	ps2pe_aux (ps, top)
register PS	ps;
int	top;
{
    register PElementLen len;
    PElementClass   class;
    PElementForm    form;
    PElementID	    id;
    register PE	    pe;

    if (top && ps_prime (ps) == NOTOK)
	return NULLPE;
    
    if (ps_read_id (ps, top, &class, &form, &id) == NOTOK)
	return NULLPE;
    if ((pe = pe_alloc (class, form, id)) == NULLPE)
	return ps_seterr (ps, PS_ERR_NMEM, NULLPE);
    if (ps_read_len (ps, &pe -> pe_len) == NOTOK)
	goto you_lose;

    len = pe -> pe_len;
    switch (pe -> pe_form) {
	case PE_FORM_PRIM: 
	    if (len == PE_LEN_INDF) {
		(void) ps_seterr (ps, PS_ERR_INDF, NULLPE);
	        goto you_lose;
	    }
	    if (len > 0) {
		if (ps -> ps_inline) {	/* "ultra-efficiency"... */
		    if (ps -> ps_base == NULLCP || ps -> ps_cnt < len) {
			(void) ps_seterr (ps, PS_ERR_XXX, NULLPE);
			goto you_lose;
		    }
		    pe -> pe_inline = 1;
		    pe -> pe_prim = (PElementData) ps -> ps_ptr;
		    ps -> ps_ptr += len, ps -> ps_cnt -= len;
		    ps -> ps_byteno += len;
		}
		else {
		    if ((pe -> pe_prim = PEDalloc (len)) == NULLPED) {
			(void) ps_seterr (ps, PS_ERR_NMEM, NULLPE);
			goto you_lose;
		    }
		    if (ps_read (ps, pe -> pe_prim, len) == NOTOK) {
#ifdef	DEBUG
			SLOG (psap_log, LLOG_DEBUG, NULLCP,
			      ("error reading primitive, %d bytes: %s",
			       len, ps_error (ps -> ps_errno)));
#endif
			goto you_lose;
		    }
		}
	    }
	    break;

	case PE_FORM_CONS: 
	    if (len != 0 && ps_read_cons (ps, &pe -> pe_cons, len) == NOTOK)
		goto you_lose;
	    break;
    }

    return pe;

you_lose: ;
#ifdef	DEBUG
    if (psap_log -> ll_events & LLOG_DEBUG) {
	LLOG (psap_log, LLOG_PDUS, ("PE read thus far"));
	pe2text (psap_log, pe, 1, NOTOK);
    }
#endif

    pe_free (pe);
    return NULLPE;
}

/*  */

static int pe_id_overshift = PE_ID_MASK << (PE_ID_BITS - (PE_ID_SHIFT + 1));


static int  ps_read_id (ps, top, class, form, id)
register PS	ps;
int	top;
register PElementClass *class;
register PElementForm *form;
register PElementID *id;
{
    byte    c,
	    d;
    register PElementID j;

    if (ps_read (ps, &c, 1) == NOTOK) {
	if (top && ps -> ps_errno == PS_ERR_EOF)
	    ps -> ps_errno = PS_ERR_NONE;
	else {
#ifdef	DEBUG
	SLOG (psap_log, LLOG_DEBUG, NULLCP,
	      ("error reading initial octet: %s", ps_error (ps -> ps_errno)));
#endif
	}
	    
	return NOTOK;
    }

    *class = (c & PE_CLASS_MASK) >> PE_CLASS_SHIFT;
    *form = (c & PE_FORM_MASK) >> PE_FORM_SHIFT;
    j = (c & PE_CODE_MASK);

    if (j == PE_ID_XTND)
	for (j = 0;; j <<= PE_ID_SHIFT) {
	    if (ps_read (ps, &d, 1) == NOTOK) {
		if (ps -> ps_errno == PS_ERR_EOF)
		    ps -> ps_errno = PS_ERR_EOFID;
		return NOTOK;
	    }

	    j |= d & PE_ID_MASK;
	    if (!(d & PE_ID_MORE))
		break;
	    if (j & pe_id_overshift)
		return ps_seterr (ps, PS_ERR_OVERID, NOTOK);
	}
    *id = j;
    DLOG (psap_log, LLOG_DEBUG, ("class=%d form=%d id=%d",*class, *form, *id));

    return OK;
}

/*  */

static int  ps_read_len (ps, len)
register PS	ps;
register PElementLen   *len;
{
    register int    i;
    register PElementLen j;
    byte    c;

    if (ps_read (ps, &c, 1) == NOTOK) {
#ifdef	DEBUG
	SLOG (psap_log, LLOG_DEBUG, NULLCP,
	      ("error reading initial length octet: %s",
	       ps_error (ps -> ps_errno)));
#endif

	return NOTOK;
    }

    if ((i = c) & PE_LEN_XTND) {
	if ((i &= PE_LEN_MASK) > sizeof (PElementLen))
	    return ps_seterr (ps, PS_ERR_OVERLEN, NOTOK);

	if (i) {
	    for (j = 0; i-- > 0;) {
		if (ps_read (ps, &c, 1) == NOTOK) {
		    if (ps -> ps_errno == PS_ERR_EOF)
			ps -> ps_errno = PS_ERR_EOFLEN;
		    return NOTOK;
		}

		j = (j << 8) | (c & 0xff);
	    }
	    *len = j;
	}
	else
	    *len = PE_LEN_INDF;
    }
    else
	*len = i;
#ifdef	DEBUG
    SLOG (psap_log, LLOG_DEBUG, NULLCP, ("len=%d", *len));
#endif

    return OK;
}

/*  */

static int  ps_read_cons (ps, pe, len)
register PS	ps;
register PE    *pe;
register PElementLen len;
{
    register int    cc;
    register PE	    p,
		    q;

    cc = ps -> ps_byteno + len;

    if ((p = ps2pe_aux (ps, 0)) == NULLPE) {
no_cons: ;
#ifdef	DEBUG
	if (len == PE_LEN_INDF)
	    LLOG (psap_log, LLOG_DEBUG,
		  ("error building indefinite constructor, %s",
		   ps_error (ps -> ps_errno)));
	else
	    LLOG (psap_log, LLOG_DEBUG,
		  ("error building constructor, stream at %d, wanted %d: %s",
		   ps -> ps_byteno, cc, ps_error (ps -> ps_errno)));
#endif

	return NOTOK;
    }
    *pe = p;

    if (len == PE_LEN_INDF) {
	if (p -> pe_class == PE_CLASS_UNIV && p -> pe_id == PE_UNIV_EOC) {
	    pe_free (p);
	    *pe = NULLPE;
	    return OK;
	}
	for (q = p; p = ps2pe_aux (ps, 0); q = q -> pe_next = p) {
	    if (p -> pe_class == PE_CLASS_UNIV && p -> pe_id == PE_UNIV_EOC) {
		pe_free (p);
		return OK;
	    }
	}

	goto no_cons;
    }

    for (q = p;; q = q -> pe_next = p) {
	if (cc < ps -> ps_byteno)
	    return ps_seterr (ps, PS_ERR_LEN, NOTOK);
	if (cc == ps -> ps_byteno)
	    return OK;
	if ((p = ps2pe_aux (ps, 0)) == NULLPE)
	    goto no_cons;
    }
}
