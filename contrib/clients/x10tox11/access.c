/*
 * Copyright 1987 by Tektronix Inc, Beaverton, Oregon.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Tektronix not be used
 * in advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.
 *
 *
 * TEKTRONIX DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * TEKTRONIX BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */
/* 
 * $XConsortium: access.c,v 1.1 88/10/11 18:11:47 jim Exp $ 
 * $Locker:  $ 
 */
#include "resource.h"
#include "X10int.h"

static XHostAddress	*validHosts = NULL;
static int		nHosts;

/* Check for valid address family, and for local host if client modification.
 * Return address length.
 */

StaticFunction X10toX11Family(family)
	short	family;
{
	switch(family) {
	case X10AF_INET:	return (FamilyInternet);
	case X10AF_DECnet:	return (FamilyDECnet);
	default:		return (-1);
	}
}

StaticFunction CheckFamily (family)
	short family;
{
	int	length;

	switch (family) {
	case X10AF_INET:
	    length = sizeof (struct in_addr);
	    break;
#ifdef DNETCONN
	case X10AF_DECnet:
	    length = sizeof (struct dn_naddr);
	    break;
#endif
	default:
	    Xstatus = X10BadValue;
	    return (-1);
	}
	return (length);
}

/* Reset access control list to initial hosts */

void ResetHosts (client)
	int	client;
{
	Bool	enabled;

	if (nHosts)
	    free (validHosts);
	validHosts = XListHosts (clist[ client ]->dpy, &nHosts, &enabled);
}

/* Add a host to the access control list */

void AddHost (client, family, addr)
	int client;
	short family;
	caddr_t addr;
{
	XHostAddress	newHost;
	int length;

	if ((length = CheckFamily (family)) < 0)
	    return;

	newHost.family = X10toX11Family(family);
	newHost.length = length;
	newHost.address = addr;

	XAddHost(clist[ client ]->dpy, &newHost);
	ResetHosts(client);
}

/* Remove a host from the access control list */

void RemoveHost (client, family, addr)
	int client;
	short family;
	caddr_t addr;
{
	XHostAddress	newHost;
	int length;

	if ((length = CheckFamily (family)) < 0)
	    return;

	newHost.family = X10toX11Family(family);
	newHost.length = length;
	newHost.address = addr;

	XRemoveHost(clist[ client ]->dpy, &newHost);
	ResetHosts(client);
}

/* Get all hosts in the access control list */

GetHosts (client, family, data)
	int	client;
	short	family;
	char	**data;
{
	int length;
	register int n;
	register caddr_t ptr;
	register XHostAddress *host;

	ResetHosts(client);

	if ((length = CheckFamily (family)) < 0)
	    return (-1);
	family = X10toX11Family(family);
	n = 0;
	host = &validHosts[ nHosts ];
	while (--host >= validHosts) {
	    if (host->family == family && host->length == length)
		n += length;
	}
	if (n) {
	    *data = ptr = StaticAlloc (n);
	    host = &validHosts[ nHosts ];
	    while (--host >= validHosts) {
		if (host->family == family && host->length == length) {
		    bcopy (host->address, ptr, length);
		    ptr += length;
		}
	    }
	}
	return (n);
}

StaticFunction ConvertAddress (saddr, length, addr)
	register struct sockaddr *saddr;
	int *length;
	caddr_t *addr;
{
	if (length == 0)
	    return (-1);
	switch (saddr->sa_family) {
	case AF_UNSPEC:
	case AF_UNIX:
	    return (0);
	case AF_INET:
	    *length = sizeof (struct in_addr);
	    *addr = (caddr_t) &(((struct sockaddr_in *) saddr)->sin_addr);
	    return (FamilyInternet);
#ifdef DNETCONN
	case AF_DECnet:
	    *length = sizeof (struct dn_naddr);
	    *addr = (caddr_t) &(((struct sockaddr_dn *) saddr)->sdn_add);
	    return (FamilyDECnet);
#endif
	default:
	    break;
	}
	return (-1);
}

/* Check if a host is in the access control list */

Bool ValidHost (saddr, length)
	struct sockaddr *saddr;
	int length;
{
	short		family;
	caddr_t		addr;
	XHostAddress	*host;
	Bool		triedServer = False;

	if ((family = ConvertAddress (saddr, &length, &addr)) < 0)
	    return (False);
	if (family == 0)
	    return (True);

	for (;;) {
	    host = &validHosts[ nHosts ];
	    while (--host >= validHosts) {
		if (family == host->family && length == host->length)
		    if (bcmp (addr, host->address, length))
			return(True);
	    }

	    /*
	     * If what we have in memory says "no" try the server again
	     * just in case things have changed.
	     */
	    if (triedServer)
		break;
	    else {
		ResetHosts(PrimaryClient);
		triedServer = True;
	    }
	}
	return (False);
}
