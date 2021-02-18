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
 * $XConsortium: X10int.h,v 6.2 88/10/11 18:36:21 jim Exp $ 
 * $Locker:  $ 
 */
#include <X11/copyright.h>

#include <X11/Xos.h>
#include <errno.h>
#include <sys/socket.h>

#include <signal.h>
#include <netinet/in.h>
#ifdef UNIXCONN
#include <sys/un.h>
#endif
#include <setjmp.h>
#ifndef SYSV
#include <net/if.h>
#endif /* SYSV */
#include <sys/param.h>
#ifdef DNETCONN
#include <netdnet/dn.h>
#include <netdnet/dnetdb.h>
#endif

#define XFROMUSER
#define X_TCP_LI_PORT	5800		/* add display number */
#define X_TCP_BI_PORT	5900		/* add display number */

#if defined(XFROMUSER) || (!defined(XFROMINIT) && defined(sun))
#define X_UNIX_PATH	"/tmp/X"	/* concat display number */
#else
#define X_UNIX_PATH	"/dev/X"	/* concat display number */
#endif

/*
 * Defines used by dispatch and the X10 io routines.
 */
#define maxsocks (NOFILE - 1)		/* maximum open sockets */
#define mskcnt ((maxsocks + 31) / 32)	/* size of bit array */

#if (mskcnt==1)
#define copybits(src, dst) dst[0] = src[0]
#define clearbits(buf) buf[0] = 0
#endif
#if (mskcnt==2)
#define copybits(src, dst) dst[0] = src[0]; dst[1] = src[1]
#define clearbits(buf) buf[0] = 0; buf[1] = 0
#endif
#if (mskcnt==3)
#define copybits(src, dst) dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
#define clearbits(buf) buf[0] = 0; buf[1] = 0; buf[2] = 0
#endif
#if (mskcnt>3)
#define copybits(src, dst) bcopy((caddr_t) src, (caddr_t) dst, sizeof (src))
#define clearbits(buf) bzero((caddr_t) buf, sizeof (buf))
#endif
#if (mskcnt==1)
#define bitmask(i) (1 << (i))
#define maskidx(i) 0
#define singlebit(buf, i) maskword(buf, i) = bitmask(i)
#endif
#if (mskcnt>1)
#define bitmask(i) (1 << ((i) & 31))
#define maskidx(i) ((i) >> 5)
#define singlebit(buf, i) clearbits(buf); bitset(buf, i)
#endif

#define maskword(buf, i) buf[maskidx(i)]
#define bitset(buf, i) maskword(buf, i) |= bitmask(i)
#define bitclear(buf, i) maskword(buf, i) &= ~bitmask(i)

