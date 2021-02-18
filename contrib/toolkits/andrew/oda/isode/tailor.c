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
/* tailor.c - ISODE tailoring */

#ifndef	lint
static char *rcsid = "$Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/tailor.c,v 1.1 89/09/21 05:56:35 mss Exp $";
#endif

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/tailor.c,v 1.1 89/09/21 05:56:35 mss Exp $
 *
 *
 * $Log:	tailor.c,v $
 * Revision 1.1  89/09/21  05:56:35  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:22:04  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:04:27  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  15:36:47  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:25:54  mrose
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
#include "general.h"
#include "manifest.h"
#include "isoaddrs.h"
#include "tailor.h"


#ifndef	ETCPATH
#define	ETCPATH	"/etc/"
#endif

#ifndef	LOGPATH
#define	LOGPATH		"/usr/tmp/"
#endif

/*    DATA */

struct pair {
    char   *p_name;
    int	    p_value;
};


static char   *isotailor = "isotailor";


char   *isodename = "";
char   *isodepath = ETCPATH;

char   *isodeconfig = ETCPATH;
int	isodeconlen = sizeof (ETCPATH) - 1;

char   *isodelogs = LOGPATH;


static struct pair ll_pairs[] = {
    "none", LLOG_NONE,
    "fatal", LLOG_FATAL,
    "exceptions", LLOG_EXCEPTIONS,
    "notice", LLOG_NOTICE,
    "pdus", LLOG_PDUS,
    "trace", LLOG_TRACE,
    "debug", LLOG_DEBUG,
    "all", LLOG_ALL,

    NULL
};

static char *compatdebug = "none";
LLog _compat_log = {
    "%d.log", NULLCP, "compat", LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *compat_log = &_compat_log;

static char *addrdebug = "none";
LLog _addr_log = {
    "%d.log", "addr", NULLCP, LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *addr_log = &_addr_log;

static char *tsapdebug = "none";
LLog _tsap_log = {
    "%d.tpkt", "tsap", NULLCP, LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *tsap_log = &_tsap_log;

static char *ssapdebug = "none";
LLog _ssap_log = {
    "%d.spkt", "ssap", NULLCP, LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *ssap_log = &_ssap_log;

static char *psapdebug = "none";
LLog _psap_log = {
    "%d.pe", NULLCP, "psap", LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *psap_log = &_psap_log;

static char *psap2debug = "none";
LLog _psap2_log = {
    "%d.ppkt", "psap2", NULLCP, LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *psap2_log = &_psap2_log;

static char *acsapdebug = "none";
LLog _acsap_log = {
    "%d.acpkt", "acsap", NULLCP, LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *acsap_log = &_acsap_log;

static char *rtsapdebug = "none";
LLog _rtsap_log = {
    "%d.rtpkt", "rtsap", NULLCP, LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *rtsap_log = &_rtsap_log;

static char *rosapdebug = "none";
LLog _rosap_log = {
    "%d.ropkt", "rosap", NULLCP, LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *rosap_log = &_rosap_log;


static struct pair ts_pairs[] = {
    "none", TS_NONE,
    "tcp", TS_TCP,
    "x25", TS_X25,
    "cons", TS_CONS,
    "bridge", TS_BRG,
    "tp4", TS_TP4,
    "all", TS_ALL,

    NULL
};

static char *_ts_stacks = "all";
int	ts_stacks = TS_ALL;

static struct pair ts_comms[] = {
    "intl-x25", SUBNET_INTL_X25,
    "janet", SUBNET_JANET,
    "internet", SUBNET_INTERNET,
    "realns", SUBNET_REALNS,

    NULL
};

static char *_ts_communities = "all";
int    ts_communities[SUBNET_MAX + 1];

#ifdef  X25
char    *x25_local_dte = "";

static char    *x25_intl_zero_str = "off";
char     x25_intl_zero = 0;

static char     *x25_strip_dnic_str = "off";
char    x25_strip_dnic = 0;

char    *x25_dnic_prefix = "";

		/* 0 = don't request/allow reverse charging
		 * 1 = reverse charge request/allowed
		 */
static char *reverse_charge_default = "0";
u_char reverse_charge = 0;

		/* 0= default recv packet size.
		 * 16, 32, 64, 128,
		 * 256, 512, 1024
		 * ( octets in decimal )
		 */
static char *recvpktsize_default = "0";
u_short recvpktsize = 0;

		/* same as above, but for send packet size */
static char *sendpktsize_default = "0";
u_short sendpktsize = 0;

		/* 0= default recv window size.
		 * 7, 127 ( in decimal )
		 */
static char *recvwndsize_default = "0";
u_char recvwndsize = 0;

		/* same as above, but for send window size */
static char *sendwndsize_default = "0";
u_char sendwndsize = 0;

		/* 0= default recv throughtput.
		 * 3= 75    8= 2400
		 * 4= 150   9= 4800
		 * 5= 300  10= 9600
		 * 6= 600  11= 19200
		 * 7= 1200 12= 48000
		 * ( bps in decimal )
		 */
static char *recvthruput_default = "0";
u_char recvthruput = 0;

		/* same as above, but for send throughput */
		/* 1 = use closed user group in cug_index */
static char *sendthruput_default = "0";
u_char sendthruput = 0;

		/* 0 = no closed user group. */
		/* 1 = use closed user group in cug_index */
static char *cug_req_default = "0";
u_char cug_req = 0;

		/* valid when cug_req= 1.
		 * 0x00 ~ 0x99 ( closed user group in BCD)
		 */
static char *cug_index_default = "0";
u_char     cug_index = 0;

		/* 0= don't use fast select
		 * 1= clear is fast select response
		 * 2= clear or call accepted
		 *    is fast select response
		 */
static char *fast_select_type_default = "0";
u_char fast_select_type = 0;

		/* 0= no RPOA transit request
		 * 1= use RPOA transit request in rpoa
		 */
static char *rpoa_req_default = "0";
u_char rpoa_req = 0;

		/* valid when rpoa_req= 1 */
		/* 0x0000 ~ 0x9999 (RPOA transit group in BCD) */
static char *rpoa_default = "0";
u_short rpoa = 0;

static char *x25debug = "none";
static LLog _x25_log = {
    "x25log", "x25", NULLCP, LLOG_NONE, LLOG_NONE, -1,
    LLOGCLS | LLOGCRT | LLOGZER, NOTOK
};
LLog *x25_log = &_x25_log;

#ifdef CAMTEC_CCL
static char *x25_outgoing_port_str = "A";
char    x25_outgoing_port = 'A';
#endif
#endif


#ifdef	BRIDGE_X25
char    *x25_bridge_host = "x25bridge";
char    *x25_bridge_addr = "000021000018";
char	*x25_bridge_listen = "";
char	*x25_bridge_pid = "";
char	*x25_bridge_discrim = "0000";
#endif


#if	defined (BRIDGE_X25) || defined (X25)
static char *x25_bridge_port_default = "146";
u_short x25_bridge_port = 0;
#endif


#ifdef CONS
#ifdef CAMTEC_CCL
#ifdef CONS_IS_YBTS
char    cons_is_ybts = 1;
/*
 * YBTS address strings for listening and calling addresses
 */
char    *cons_local_address = "ISODE";
char    *cons_listen_str = "ISODE";
#else
char    cons_is_ybts = 0;
/*
 * NSAP addresses for calling and listening address strings
 */
char    *cons_local_address = "0987654321";
char    *cons_listen_str = "2345678901";
#endif
#endif
#endif




char	ns_enabled = 0;
static char *usens = "off";

char   *ns_address = "";

/*  */

static struct bind {
    char   *b_key;

    char  **b_value;
    int	    b_dynamic;
}	binds[] = {
    "localname",	&isodename,			0,
    "localpath",	&isodepath,			0,
    "logpath",		&isodelogs,			0,

    "compatlevel",	&compatdebug,			0,
    "compatfile",	&_compat_log.ll_file,		0,

    "addrlevel",	&addrdebug,			0,
    "addrfile",		&_addr_log.ll_file,		0,

    "tsaplevel",	&tsapdebug,			0,
    "tsapfile",		&_tsap_log.ll_file,		0,

    "ssaplevel",	&ssapdebug,			0,
    "ssapfile",		&_ssap_log.ll_file,		0,

    "psaplevel",	&psapdebug,			0,
    "psapfile",		&_psap_log.ll_file,		0,

    "psap2level",	&psap2debug,			0,
    "psap2file",	&_psap2_log.ll_file,		0,

    "acsaplevel",	&acsapdebug,			0,
    "acsapfile",	&_acsap_log.ll_file,		0,

    "rtsaplevel",	&rtsapdebug,			0,
    "rtsapfile",	&_rtsap_log.ll_file,		0,

    "rosaplevel",	&rosapdebug,			0,
    "rosapfile",	&_rosap_log.ll_file,		0,

    "ts_stacks",	&_ts_stacks,			0,
    "ts_communities",	&_ts_communities,		0,

#ifdef  X25
    "x25_local_dte",	&x25_local_dte,			0,
    "x25_dnic_prefix",	&x25_dnic_prefix,		0,
    "x25_intl_zero",	&x25_intl_zero_str,		0,
    "x25_strip_dnic",	&x25_strip_dnic_str,		0,

    "reverse_charge",   &reverse_charge_default,	0,
    "recvpktsize",      &recvpktsize_default,		0,
    "sendpktsize",      &sendpktsize_default,		0,
    "recvwndsize",      &recvwndsize_default,		0,
    "sendwndsize",      &sendwndsize_default,		0,
    "recvthruput",      &recvthruput_default,		0,
    "sendthruput",      &sendthruput_default,		0,
    "cug_req",          &cug_req_default,		0,
    "cug_index",        &cug_index_default,		0,
    "fast_select_type", &fast_select_type_default,	0,
    "rpoa_req",         &rpoa_req_default,		0,
    "rpoa",             &rpoa_default,			0,

    "x25level",		&x25debug,			0,
    "x25file",		&_x25_log.ll_file,		0,

#ifdef CAMTEC_CCL
    "x25_outgoing_port",        &x25_outgoing_port_str,	0,
#endif
#endif

#ifdef	BRIDGE_X25
    "x25_bridge_host",		&x25_bridge_host,	0,
    "x25_bridge_addr",		&x25_bridge_addr,	0,
    "x25_bridge_listen",	&x25_bridge_listen,	0,
    "x25_bridge_pid",		&x25_bridge_pid,	0,
    "x25_bridge_discrim",	&x25_bridge_discrim,	0,
#endif

#if	defined (BRIDGE_X25) || defined (X25)
    "x25_bridge_port",		&x25_bridge_port_default,0,
#endif

#ifdef CONS
    "cons_local_address",       &cons_local_address,	0,
    "cons_listen_str",          &cons_listen_str,	0,
#endif


    "ns_enable",		&usens,			0,
    "ns_address",		&ns_address,		0,

    NULL
};


int	tailor_read ();
char   *tailor_value ();

/*  */

char  *isodesetailor (file)
char  *file;
{
    char   *ofile = isotailor;

    if ((isotailor = file) == NULLCP)
	isotailor = "isotailor";

    return ofile;
}

/*  */

void	isodetailor (myname, wantuser)
char   *myname;
int	wantuser;
{
    register char *hp,
		  *mp;
    char   buffer[BUFSIZ];
    static int  inited = 0;

    if (inited)
	return;
    inited = 1;

    tailor_read (isodefile (isotailor));

    if (wantuser) {
	if ((hp = getenv ("HOME")) == NULL)
	    hp = ".";
	if (myname) {
	    if (mp = rindex (myname, '/'))
		mp++;
	    if (mp == NULL || *mp == NULL)
		mp = myname;
	}
	else
	    mp = "isode";
	(void) sprintf (buffer, "%s/.%s_tailor", hp, mp);
	tailor_read (buffer);
    }

    isodexport ();

    ll_hdinit (compat_log, myname);
    ll_hdinit (addr_log, myname);
    ll_hdinit (tsap_log, myname);
    ll_hdinit (ssap_log, myname);
    ll_hdinit (psap_log, myname);
    ll_hdinit (psap2_log, myname);
    ll_hdinit (acsap_log, myname);
    ll_hdinit (rtsap_log, myname);
    ll_hdinit (rosap_log, myname);
#ifdef X25
    ll_hdinit (x25_log, myname);
#endif
}

/*  */

static int  tailor_read (file)
char   *file;
{
    register char  *bp,
                   *cp;
    char    buffer[BUFSIZ];
    register FILE *fp;

    if (fp = fopen (file, "r")) {
	while (fgets (buffer, sizeof buffer, fp)) {
	    if ((cp = index (buffer, '\n')) == NULL) {
		fprintf (stderr, "%s: line too long\n", file);
		break;
	    }
	    *cp = NULL;
	    if (*buffer == '#' || *buffer == NULL)
		continue;
	    if ((bp = index (buffer, ':')) == NULL) {
		fprintf (stderr, "%s: invalid syntax in \"%s\"\n",
			 file, buffer);
		break;
	    }
	    for (cp = bp - 1; cp >= buffer; cp--)
		if (isspace (*cp))
		    *cp = NULL;
		else
		    break;

	    *bp++ = NULL;
	    while (isspace (*bp))
		*bp++ = NULL;

	    if ((cp = tailor_value (bp))
		     && isodesetvar (buffer, cp, 1) == NOTOK)
		free (cp);
	}

	(void) fclose (fp);
    }
}

/*  */

int	isodesetvar (name, value, dynamic)
char   *name,
       *value;
int	dynamic;
{
    register struct bind   *b;

    for (b = binds; b -> b_key; b++)
	if (strcmp (b -> b_key, name) == 0) {
	    if (b -> b_dynamic && *b -> b_value)
		free (*b -> b_value);
	    *b -> b_value = value, b -> b_dynamic = dynamic;
	    return OK;
	}

    return NOTOK;
}

/*  */

void	isodexport ()
{
    compat_log -> ll_events = events_value (ll_pairs, compatdebug, "compatlevel");
    addr_log -> ll_events = events_value (ll_pairs, addrdebug, "addrlevel");
    tsap_log -> ll_events = events_value (ll_pairs, tsapdebug, "tsaplevel");
    ssap_log -> ll_events = events_value (ll_pairs, ssapdebug, "ssaplevel");
    psap_log -> ll_events = events_value (ll_pairs, psapdebug, "psaplevel");
    psap2_log -> ll_events = events_value (ll_pairs, psap2debug, "psap2level");
    acsap_log -> ll_events = events_value (ll_pairs, acsapdebug, "acsaplevel");
    rtsap_log -> ll_events = events_value (ll_pairs, rtsapdebug, "rtsaplevel");
    rosap_log -> ll_events = events_value (ll_pairs, rosapdebug, "rosaplevel");

    ts_stacks = events_value (ts_pairs, _ts_stacks, "ts_stacks");

    {
	register int   i,
		      *ip,
		      *jp;
	register char *cp,
		     **ap;
	register struct pair *pp;
	char    buffer[BUFSIZ],
	       *vec[NVEC + NSLACK + 1];

	(void) strcpy (buffer, _ts_communities);
	(void) str2vec (buffer, ap = vec);
	ip = ts_communities;
	while (cp = *ap++) {
	    if (strcmp (cp, "none") == 0) {
		ip = ts_communities;
		break;
	    }

	    if (strcmp (cp, "all") == 0) {
		for (i = 1; i <= SUBNET_MAX; i++) {
		    for (jp = ts_communities; jp < ip; jp++)
			if (*jp == i)
			    break;
		    if (jp >= ip)
			*ip++ = i;
		}
		break;
	    }

	    for (pp = ts_comms; pp -> p_name; pp++)
		if (strcmp (pp -> p_name, cp) == 0)
		    break;
	    if (pp -> p_name) {
		for (jp = ts_communities; jp < ip; jp++)
		    if (*jp == pp -> p_value)
			break;
		if (jp >= ip)
		    *ip++ = pp -> p_value;
	    }
	    else
		fprintf (stderr,
			 "unknown value \"%s\" for variable ts_communites\n",
			 cp);
	}
	*ip = NULL;
    }

#ifdef	X25
    reverse_charge = (u_char) atoi (reverse_charge_default);
    recvpktsize = (u_short) atoi (recvpktsize_default);
    sendpktsize = (u_short) atoi (sendpktsize_default);
    recvwndsize = (u_char) atoi (recvwndsize_default);
    sendwndsize = (u_char) atoi (sendwndsize_default);
    recvthruput = (u_char) atoi (recvthruput_default);
    sendthruput = (u_char) atoi (sendthruput_default);
    cug_req = (u_char) atoi (cug_req_default);
    cug_index = (u_char) atoi (cug_index_default);
    fast_select_type = (u_char) atoi (fast_select_type_default);
    rpoa_req = atoi (rpoa_req_default);
    rpoa = (u_short) atoi (rpoa_default);

#ifdef	X25
    x25_log -> ll_events = events_value (ll_pairs, x25debug, "x25level");
#endif

#ifdef	CAMTEC_CCL
    x25_outgoing_port = *x25_outgoing_port_str;
#endif

    x25_intl_zero = !strcmp (x25_intl_zero_str, "on");
    x25_strip_dnic =  !strcmp (x25_strip_dnic_str, "on");
#endif

#if	defined (BRIDGE_X25) || defined (X25)
    x25_bridge_port = htons ((u_short) atoi (x25_bridge_port_default));
#endif

    ns_enabled = !strcmp (usens, "on");
}

/*  */

#define	QUOTE	'\\'


static char *tailor_value (s)
register char   *s;
{
    register int    i,
                    r;
    register char  *bp;
    char    buffer[BUFSIZ];

    for (bp = buffer; *s; bp++, s++)
	if (*s != QUOTE)
	    *bp = *s;
	else
	    switch (*++s) {
		case '0':
		    *bp = '\0';
		    break;
		case 'b':
		    *bp = '\b';
		    break;
		case 'f':
		    *bp = '\f';
		    break;
		case 'n':
		    *bp = '\n';
		    break;
		case 'r':
		    *bp = '\r';
		    break;
		case 't':
		    *bp = '\t';
		    break;

		case NULL: s--;
		case QUOTE: 
		    *bp = QUOTE;
		    break;

		default: 
		    if (!isdigit (*s)) {
			*bp++ = QUOTE;
			*bp = *s;
			break;
		    }
		    r = *s != '0' ? 10 : 8;
		    for (i = 0; isdigit (*s); s++)
			i = i * r + *s - '0';
		    s--;
		    *bp = toascii (i);
		    break;
	    }
    *bp = NULL;

    if ((bp = malloc ((unsigned) (strlen (buffer) + 1))) != NULL)
	(void) strcpy (bp, buffer);

    return bp;
}

/*  */

static int  events_value (pairs, s, var)
struct pair *pairs;
char   *s,
       *var;
{
    int     value;
    register char  *cp,
                  **ap;
    register struct pair   *pp;
    char    buffer[BUFSIZ],
	   *vec[NVEC + NSLACK + 1];

    value = 0;
    (void) strcpy (buffer, s);
    (void) str2vec (buffer, ap = vec);
    while (cp = *ap++) {
	for (pp = pairs; pp -> p_name; pp++) 
	    if (strcmp (pp -> p_name, cp) == 0) {
		value |= pp -> p_value;
		break;
	    }
	if (!pp -> p_name)
	    fprintf (stderr, "unknown value \"%s\" for variable %s\n",
		     cp, var);
    }

    return value;
}
