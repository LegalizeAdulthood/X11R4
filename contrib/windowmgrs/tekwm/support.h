#ifndef LINT
#ifdef RCS_ID
static char *rcsid=  "$Header: support.h,v 5.8 89/11/22 15:16:12 glennw Exp $";
#endif /* RCS_ID */
#endif /* LINT */

/*
 * Copyright 1987, 1988 by Digital Equipment Corporation, Maynard,
 * Massachusetts, and the Massachusetts Institute of Technology, Cambridge,
 * Massachusetts.  Copyright 1987 by Jordan Hubbard.  Copyright 1987, 1988 by
 * Ardent Computer Corporation, Sunnyvale, Ca.  Copyright 1989 by Tektronix,
 * Inc., Beaverton, Oregon.
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the names of Ardent Computer Corporation, Jordan
 * Hubbard, Digital Equipment, Tektronix or M.I.T.  not be used in
 * advertising or publicity pertaining to distribution of the software
 * without specific, written prior permission.  Ardent Computer Corporation,
 * Jordan Hubbard, Digital Equipment, Tektronix and M.I.T.  make no
 * representations about the suitability of this software for any purpose.
 * It is provided "as is" without express or implied warranty.
 *
 * ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL EQUIPMENT, TEKTRONIX,
 * AND M.I.T.  DISCLAIM ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
 * INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO
 * EVENT SHALL ARDENT COMPUTER CORPORATION, JORDAN HUBBARD, DIGITAL
 * EQUIPMENT, TEKTRONIX, OR M.I.T.  BE LIABLE FOR ANY SPECIAL, INDIRECT OR
 * CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
 * USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
 * OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 *
 * Author:  Glenn Widener, Tektronix, Inc.
 *          P.O. Box 1000
 *          Wilsonville, OR, 97070
 *          glennw@orca.wv.tek.com
 */

#undef STACK_RTNAMES

#ifdef TRACE
#define Entry(woo) { printf("Pushing to %s\n", woo); push_rtn(woo); }
#define Leave(woo) { printf("Pop from %s with %x\n", curr_rtn(), woo); pop_rtn(); return(woo); }
#define Leave_void { printf("Pop from %s (void)\n", curr_rtn()); pop_rtn(); return; }
#define Trace_on _rtn_trace = 1;
#define Trace_off _rtn_trace = 0;
extern char *curr_rtn();
extern int _rtn_level;
extern int _rtn_trace;
#else
#ifdef SAVERTNAME
#define Entry(woo) push_rtn(woo);
#define Leave(woo) { pop_rtn(); return(woo); }
#define Leave_void { pop_rtn(); return; }
#define Trace_on _rtn_trace = 1;
#define Trace_off _rtn_trace = 0;
extern char *curr_rtn();
extern int _rtn_level;
extern int _rtn_trace;
#else
#define Entry(s)
#define Leave(s) return(s);
#define Leave_void return;
#define Trace_on
#define Trace_off
#define Curr_rtn()
#endif /* SAVERTNAME */
#endif /* TRACE */
