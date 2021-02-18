/*
 * Copyright IBM Corporation 1987,1988,1989
 *
 * All Rights Reserved
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
*/
/* "@(#)compiler.h	3.1 88/09/22 09:31:34" */
#ifndef __COMPILER_DEPENDANCIES__
#define __COMPILER_DEPENDANCIES__
/* $Header: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/OScompiler.h,v 20.0 88/12/04 15:20:45 paul Exp $ */
/* $Source: /andrew/X11/r3src/server/ddx/ibm/AIX/RCS/OScompiler.h,v $ */

/* This File Contains all compiler dependant macros for the various
 * different IBM machines and their compilers:
 * rt's & PS/2's
 */

/* Are "volatile", "signed" or "const" Valid Declaration Modifiers ?? */
#ifdef _ANSI_DECLS_
#undef _ANSI_DECLS_
#endif

#if defined(UNIXCPP) && !defined(ANSICPP)
#define APPEND_STRING(a,b) a/**/b
#else
#if defined(ANSICPP)
#define APPEND_STRING(a,b) a##b
#else
#define APPEND_STRING(a,b) /**/a\
b/**/
#endif
#endif

#ifdef __HIGHC__
/* The following defines are for MetaWare HighC (hc) compiler on IBM hardware */
pragma on(RECOGNIZE_LIBRARY) ;
#if defined(OLDHC) /* i.e. hc 1.4 */
#define MOVE( src, dst, length ) _move( src, dst, length )
#else	/* i.e. hc 2.x */
#define MOVE( src, dst, length ) memcpy( dst, src, length)
#endif
#define MAX(a,b) _max(a,b)
#define MIN(a,b) _min(a,b)
#define ABS(x) _abs(x)
#define _ANSI_DECLS_ TRUE

#else /* Not metaware's hc */

#ifdef SYSV
#define MOVE( src, dst, length ) memcpy( dst, src, length)
#else
#define MOVE( src, dst, length ) bcopy( src, dst, length )
#endif

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define ABS(x) (((x)>0)?(x):-(x))

#endif /* Not metaware's hc */

/* Are "volatile" & "const" Valid Declaration Modifiers ?? */
#if !defined(_ANSI_DECLS_) || defined(lint)
/* So that lint doesn't complain about constructs it doesn't understand */
#ifdef volatile
#undef volatile
#endif
#define volatile /**/
#ifdef const
#undef const
#endif
#define const	/**/
#ifdef signed
#undef signed
#endif
#define signed	/**/
#ifdef _ANSI_DECLS_
#undef _ANSI_DECLS_
#endif
#endif

#if defined(AIXrt)
#if defined(volatile)
#undef volatile
#endif
#endif

#endif
