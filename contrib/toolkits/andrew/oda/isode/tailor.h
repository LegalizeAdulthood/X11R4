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
/* tailor.h - ISODE tailoring */

/* 
 * $Header: /afs/andrew.cmu.edu/itc/sm/releases/X.V11R4/andrew/oda/isode/RCS/tailor.h,v 1.1 89/09/21 05:56:45 mss Exp $
 *
 *
 * $Log:	tailor.h,v $
 * Revision 1.1  89/09/21  05:56:45  mss
 * Initial revision
 * 
 * Revision 1.2  89/09/11  16:22:19  mss
 * added copyright
 * 
 * Revision 1.1  89/07/27  13:04:32  mss
 * Initial revision
 * 
 * Revision 1.1  89/05/16  17:10:44  mss
 * Initial revision
 * 
 * Revision 6.0  89/03/18  23:32:26  mrose
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


#ifndef	_TAILOR_
#define	_TAILOR_

#ifndef	_LOGGER_
#include "logger.h"
#endif

extern char    *isodename;
extern char    *isodepath;

extern char    *isodeconfig;
extern int	isodeconlen;

extern	char   *isodelogs;

extern LLog *compat_log, _compat_log;
extern LLog *addr_log, _addr_log;
extern LLog *tsap_log, _tsap_log;
extern LLog *ssap_log, _ssap_log;
extern LLog *psap_log, _psap_log;
extern LLog *psap2_log, _psap2_log;
extern LLog *acsap_log, _acsap_log;
extern LLog *rtsap_log, _rtsap_log;
extern LLog *rosap_log, _rosap_log;

extern int    ts_stacks;
#define	TS_NONE	0
#define	TS_TCP	0x01
#define	TS_X25	0x02
#define	TS_CONS	0x04
#define	TS_BRG	0x08
#define	TS_TP4	0x10
#define	TS_ALL	0xff

extern int    ts_communities[];

#ifdef	X25
extern char    *x25_local_dte;

extern char     x25_intl_zero;
extern char     x25_strip_dnic;
extern char    *x25_dnic_prefix;

extern u_char   reverse_charge;
extern u_short  recvpktsize;
extern u_short  sendpktsize;
extern u_char   recvwndsize;
extern u_char   sendwndsize;
extern u_char   recvthruput;
extern u_char   sendthruput;
extern u_char   cug_req;
extern u_char   cug_index;
extern u_char   fast_select_type;
extern u_char   rpoa_req;
extern u_short  rpoa;

extern LLog *x25_log, _x25_log;

#ifdef	CAMTEC_CCL
extern char     x25_outgoing_port;
#endif
#endif

#ifdef	BRIDGE_X25
extern char    *x25_bridge_host;
extern char    *x25_bridge_addr;
extern char    *x25_bridge_listen;
extern char    *x25_bridge_pid;
extern char    *x25_bridge_discrim;
#endif

#if	defined (BRIDGE_X25) || defined (X25)
extern u_short  x25_bridge_port;
#endif

#ifdef CONS
#ifdef CAMTEC_CCL
extern char    *cons_local_address;
extern char    cons_is_ybts;
extern char    *cons_listen_str;
#endif
#endif


extern char	ns_enabled;
extern char    *ns_address;


void	isodetailor ();
int	isodesetvar ();
void	isodexport ();

#define	isodefile(file)	_isodefile (isodepath, (file))
char   *_isodefile ();

char   *getlocalhost ();

#endif
