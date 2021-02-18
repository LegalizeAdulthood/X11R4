/*
 * Copyright (c) 1989 Stanford University
 *
 * Permission to use, copy, modify, distribute, and sell this software and its
 * documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Stanford not be used in advertising or
 * publicity pertaining to distribution of the software without specific,
 * written prior permission.  Stanford makes no representations about
 * the suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * STANFORD DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS.
 * IN NO EVENT SHALL STANFORD BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE,
 * DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION
 * WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

/*
 * C++ interface to Unix signals
 */

#ifndef signal_h
#define signal_h

#if defined(__cplusplus)
extern "C" {
#endif

/*
 * Use standard system definitions, but ignore forward function declarations
 * because they probably don't have argument prototypes.
 */

#define signal c_signal
#include "//usr/include/sys/signal.h"
#undef signal

typedef void (*SignalHandler)(...);

extern SignalHandler signal(int, SignalHandler);
extern int kill(int, int);

#define SignalBad ((SignalHandler)-1)
#define SignalDefault ((SignalHandler)0)
#define SignalIgnore ((SignalHandler)1)

/*
 * BSD-specific signal routines.  On hpux, some of them are declared
 * with long parameters.
 */

#ifdef hpux
extern long sigsetmask(long mask);
extern long sigblock(long mask);
extern long sigpause(long mask);
extern int sigvec(int sig, struct sigvec* v, struct sigvec* prev);
#else
extern int sigsetmask(int mask);
extern int sigblock(int mask);
extern int sigpause(int mask);
extern int sigvec(int sig, struct sigvec* v, struct sigvec* prev);
#endif

#if defined(__cplusplus)
}
#endif

#endif
