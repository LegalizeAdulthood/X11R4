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
/* isoaddrs.h - ISODE addressing */

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/isoaddrs.h,v 1.1 89/09/21 05:22:54 mss Exp $
 *
 *
 * $Log:	isoaddrs.h,v $
 * Revision 1.1  89/09/21  05:22:54  mss
 * Initial revision
 * 
 * Revision 1.1  89/09/21  04:26:02  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:12:24  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:02:30  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:10:02  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:32:06  mrose
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


#ifndef	_ISOADDRS_
#define	_ISOADDRS_

#ifndef	_MANIFEST_
#include "manifest.h"
#endif
#ifndef	_GENERAL_
#include "general.h"
#endif

/*  */

#ifdef	NULLPE
typedef struct AEInfo {		/* "real" directory services! */
    PE	    aei_ap_title;
    PE	    aei_ae_qualifier;

    int	    aei_ap_id;
    int	    aei_ae_id;

    int	    aei_flags;
#define	AEI_NULL	0x00
#define	AEI_AP_ID	0x01
#define	AEI_AE_ID	0x02
}	AEInfo, *AEI;
#define	NULLAEI		((AEI) 0)
#define	AEIFREE(aei) { \
    if ((aei) -> aei_ap_title) \
	pe_free ((aei) -> aei_ap_title), \
	    (aei) -> aei_ap_title = NULLPE; \
    if ((aei) -> aei_ae_qualifier) \
	pe_free ((aei) -> aei_ae_qualifier), \
	    (aei) -> aei_ae_qualifier = NULLPE; \
}

AEI	str2aei ();

char   *sprintaei ();
#endif

/*  */

struct NSAPaddr {
    long     na_type;	/* this structure shouldn't have holes in it */
#define	NA_NSAP	0
#define	NA_TCP	1
#define	NA_X25	2
#define	NA_BRG	3

/* An interim approach to use of Network Addresses... */
#define	INTERIM_IDP		"5400728722"

#define	SUBNET_REALNS		(-1)
#define	SUBNET_INTL_X25		1
#define	SUBNET_JANET		2
#define	SUBNET_INTERNET		3
#define	SUBNET_MAX		4	/* number of subnets */
    long    na_subnet;			/* prefix for Interim NSAPaddrs */

    union {
	struct na_nsap {		/* real network service */
#define	NASIZE	64			/* 20 ought to do it */
	    char    na_nsap_address[NASIZE];
	    char    na_nsap_addrlen;
	}               un_na_nsap;

	struct na_tcp {			/* emulation via RFC1006 */
#define	NSAP_DOMAINLEN	63
	    char    na_tcp_domain[NSAP_DOMAINLEN + 1];

	    u_short na_tcp_port;	/* non-standard TCP port */
	    u_short na_tcp_tset;	/* transport set */
#define	NA_TSET_TCP	0x0001		/*   .. TCP */
#define	NA_TSET_UDP	0x0002	        /*   .. UDP */
	}               un_na_tcp;

	struct na_x25 {			/* X.25 (assume single subnet) */
#define	NSAP_DTELEN	36
	    char    na_x25_dte[NSAP_DTELEN + 1]; /* Numeric DTE + Link */
	    char    na_x25_dtelen;	/* number of digits used */

/* Conventionally, the PID sits at the first head bytes of user data and so
 * should probably not be mentioned specially. A macro might do it, if
 * necessary.
 */

#define	NPSIZE	4
	    char    na_x25_pid[NPSIZE];	/* X.25 protocol id */
	    char    na_x25_pidlen;	/*   .. */

#define	CUDFSIZE 16
	    char    na_x25_cudf[CUDFSIZE];/* call user data field */
	    char    na_x25_cudflen;	/* .. */
/*
 * X25 Facilities field. 
 */
#define	FACSIZE	6
	    char    na_x25_fac[FACSIZE];	/* X.25 facilities */
	    char    na_x25_faclen;		/*   .. */
	}               un_na_x25;
    }               na_un;

#define	na_address	na_un.un_na_nsap.na_nsap_address
#define	na_addrlen	na_un.un_na_nsap.na_nsap_addrlen

#define	na_domain	na_un.un_na_tcp.na_tcp_domain
#define	na_port		na_un.un_na_tcp.na_tcp_port
#define	na_tset		na_un.un_na_tcp.na_tcp_tset

#define	na_dte		na_un.un_na_x25.na_x25_dte
#define	na_dtelen	na_un.un_na_x25.na_x25_dtelen
#define	na_pid		na_un.un_na_x25.na_x25_pid
#define	na_pidlen	na_un.un_na_x25.na_x25_pidlen
#define	na_cudf		na_un.un_na_x25.na_x25_cudf
#define	na_cudflen	na_un.un_na_x25.na_x25_cudflen
#define	na_fac		na_un.un_na_x25.na_x25_fac
#define	na_faclen	na_un.un_na_x25.na_x25_faclen
};
#define	NULLNA			((struct NSAPaddr *) 0)


struct TSAPaddr {
#define	NTADDR	4
    struct NSAPaddr ta_addrs[NTADDR];	/* choice of network addresses */
    int     ta_naddr;

#define	TSSIZE	64
    int	    ta_selectlen;

    union {				/* TSAP selector */
	char    ta_un_selector[TSSIZE];

	u_short ta_un_port;
    }               un_ta;
#define	ta_selector	un_ta.ta_un_selector
#define	ta_port		un_ta.ta_un_port
};
#define	NULLTA			((struct TSAPaddr *) 0)


struct SSAPaddr {
    struct TSAPaddr sa_addr;		/* transport address */

#define	SSSIZE	64
    int	    sa_selectlen;

    union {				/* SSAP selector */
	char    sa_un_selector[SSSIZE];

	u_short sa_un_port;
    }               un_sa;
#define	sa_selector	un_sa.sa_un_selector
#define	sa_port		un_sa.sa_un_port
};
#define	NULLSA			((struct SSAPaddr *) 0)


struct PSAPaddr {
    struct SSAPaddr pa_addr;		/* session address */

#define	PSSIZE	64
    int	    pa_selectlen;

    union {				/* PSAP selector */
	char    pa_un_selector[PSSIZE];

	u_short pa_un_port;
    }               un_pa;
#define	pa_selector	un_pa.pa_un_selector
#define	pa_port		un_pa.pa_un_port
};
#define	NULLPA			((struct PSAPaddr *) 0)

struct PSAPaddr *aei2addr ();	/* application entity title to PSAPaddr */

/*  */



#ifdef	NULLPE
struct ns_query {
    int	    ns_id;

    char   *ns_name;
    char   *ns_attribute;
};

struct ns_response {
    int	    ns_id;

    PE	    ns_name;
    PE	    ns_value;
};

char   *alias2name ();
PE	name2value ();

extern PE    (*acsap_lookup) ();
#endif


#ifdef	NULLOID
struct isoentity {		/* for stub directory service */
    OIDentifier ie_identifier;
    char    *ie_descriptor;

    struct PSAPaddr ie_addr;
};

int	setisoentity (), endisoentity ();

struct isoentity *getisoentity ();

AEI	oid2aei ();
#endif


				/* old-style */
struct PSAPaddr *is2paddr ();	/* service entry to PSAPaddr */
struct SSAPaddr *is2saddr ();	/* service entry to SSAPaddr */
struct TSAPaddr *is2taddr ();	/* service entry to TSAPaddr */

/*  */

struct PSAPaddr *str2paddr ();  /* string encoding to PSAPaddr */
struct SSAPaddr *str2saddr ();  /* string encoding to SSAPaddr */
struct TSAPaddr *str2taddr ();  /* string encoding to TSAPaddr */

#define	paddr2str(pa,na)	_paddr2str ((pa), (na), 0)

char   *_paddr2str ();		/* PSAPaddr to string encoding */
char   *saddr2str ();		/* SSAPaddr to string encoding */
char   *taddr2str ();		/* TSAPaddr to string encoding */

struct NSAPaddr *na2norm ();	/* normalize NSAPaddr */

char   *na2str ();		/* pretty-print NSAPaddr */

/*  */

int	isodeserver ();		/* generic server dispatch */

/*  */

/* all of this really should be in "isoqos.h" ... */
   
struct QOStype {
				/* transport QOS */
    int	    qos_reliability;	/* "reliability" element */
#define	HIGH_QUALITY	0
#define	LOW_QUALITY	1

				/* session QOS */
    int	    qos_sversion;	/* session version required */
    int	    qos_extended;	/* extended control */
};
#define	NULLQOS	((struct QOStype *) 0)

#endif
