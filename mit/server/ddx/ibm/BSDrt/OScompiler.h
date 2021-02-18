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
/***********************************************************
		Copyright IBM Corporation 1988

                      All Rights Reserved

Permission to use, copy, modify, and distribute this software and its 
documentation for any purpose and without fee is hereby granted, 
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in 
supporting documentation, and that the name of IBM not be
used in advertising or publicity pertaining to distribution of the
software without specific, written prior permission.  

IBM DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
IBM BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/
/***********************************************************
Copyright 1989 by the Massachusetts Institute of Technology

                     All rights reserved.

Permission to use, copy, modify, and distribute this software and its
documentation for any purpose and without fee is hereby granted,
provided that the above copyright notice appear in all copies and that
both that copyright notice and this permission notice appear in
supporting documentation, and that the name of the Massachusetts
Institute of Technology (M.I.T.) not be used in advertising or publicity
pertaining to distribution of the software without specific, written
prior permission.

M.I.T. DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
M.I.T. BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
SOFTWARE.

******************************************************************/

#ifndef __COMPILER_DEPENDENCIES__
#define __COMPILER_DEPENDENCIES__
/* $Header: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/BSDrt/RCS/OScompiler.h,v 1.2 89/08/28 09:48:59 probe Exp $ */
/* $Source: /afs/testers.athena.mit.edu/src/x11r4/src/mit/server/ddx/ibm/BSDrt/RCS/OScompiler.h,v $ */

/* 
 * This File Contains all compiler dependant macros for the various
 * different IBM machines and their compilers:
 */

/* Are "volatile", "signed" or "const" Valid Declaration Modifiers ?? */
#ifdef _ANSI_DECLS_
#undef _ANSI_DECLS_
#endif /* _ANSI_DECLS_ */

#if defined(UNIXCPP) && !defined(ANSICPP)
#define APPEND_STRING(a,b) a/**/b
#else
#if defined(ANSICPP)
#define APPEND_STRING(a,b) a##b
#else
#define APPEND_STRING(a,b) /**/a\
b/**/
#endif /* ANSICPP */
#endif /* UNIXCPP */

#ifdef __HIGHC__
/* The following defines are for MetaWare HighC (hc) compiler on IBM hardware */
pragma on(RECOGNIZE_LIBRARY) ;
#if HCVERSION < 21000 /* Must be before hc2.1a for this */
#define MOVE( src, dst, length ) _move( src, dst, length )
#else	/* i.e. hc 2.0x or 1.Xx */
#define MOVE( src, dst, length ) memcpy( dst, src, length)
#endif /* HCVERSION < 21000 */
#define MAX(a,b) _max(a,b)
#define MIN(a,b) _min(a,b)
#define ABS(x) _abs(x)
#define _ANSI_DECLS_ TRUE

#else /* Not metaware's hc */
/* The Release 3 server probably should not be built using
 * any compiler other than hc !
 */
#ifdef __GNUC__
#define _ANSI_DECLS_ TRUE
#define MOVE(src,dst,length) __builtin_memcpy(dst,src,length)
#define ABS(x) __builtin_abs(x)
#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#else

#ifdef SYSV
#define MOVE( src, dst, length ) memcpy( dst, src, length)
#else
#define MOVE( src, dst, length ) bcopy( src, dst, length )
#endif

#define MAX(a,b) (((a)>(b))?(a):(b))
#define MIN(a,b) (((a)<(b))?(a):(b))
#define ABS(x) (((x)>0)?(x):-(x))

#endif /* GNUC */
#endif /* __HIGHC__ */

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

#endif
