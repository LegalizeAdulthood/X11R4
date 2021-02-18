/*
 *	WStr.h -- header file for Wide-Character String Library
 */

/*
 * Copyright (c) 1989  Software Research Associates, Inc.
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of Software Research Associates not be
 * used in advertising or publicity pertaining to distribution of the
 * software without specific, written prior permission.  Software Research
 * Associates makes no representations about the suitability of this software
 * for any purpose.  It is provided "as is" without express or implied
 * warranty.
 *
 * Author:  Makoto Ishisone, Software Research Associates, Inc., Japan
 *		ishisone@sra.co.jp
 */

/* $Header: /private/usr/mmb/ishisone/job/x11/xlocal/RCS/WStr.h,v 1.5 89/09/12 10:13:31 ishisone Rel $ */

#ifndef _WStr_h
#define _WStr_h

#ifndef WCHAR_T
#define WCHAR_T
typedef unsigned short	wchar_t;
#endif

/* for backward compatibility... */
#define convWStoLatin1	convJWStoLatin1
#define convLatin1toWS	convLatin1toJWS
#define convWStoJIS	convJWStoJIS
#define convJIStoWS	convJIStoJWS
#define convWStoEUC	convJWStoEUC
#define convEUCtoWS	convEUCtoJWS
#define convWStoSJIS	convJWStoSJIS
#define convSJIStoWS	convSJIStoJWS

#if __STDC__ == 1
extern int convJWStoLatin1(wchar_t *, unsigned char *);
extern int convLatin1toJWS(unsigned char *, wchar_t *);
extern int convJWStoJIS(wchar_t *, unsigned char *);
extern int convJIStoJWS(unsigned char *, wchar_t *);
extern int convJWStoEUC(wchar_t *, unsigned char *);
extern int convEUCtoJWS(unsigned char *, wchar_t *);
extern int convJWStoSJIS(wchar_t *, unsigned char *);
extern int convSJIStoJWS(unsigned char *, wchar_t *);
extern wchar_t *wstrcat(wchar_t *, wchar_t *);
extern wchar_t *wstrncat(wchar_t *, wchar_t *, int);
extern int wstrcmp(wchar_t *, wchar_t *);
extern int wstrncmp(wchar_t *, wchar_t *, int);
extern wchar_t *wstrcpy(wchar_t *, wchar_t *);
extern wchar_t *wstrncpy(wchar_t *, wchar_t *, int);
extern int wstrlen(wchar_t *);
extern wchar_t *windex(wchar_t *, wchar_t);
extern wchar_t *wrindex(wchar_t *, wchar_t);
#else
extern int convJWStoLatin1();
extern int convLatin1toJWS();
extern int convJWStoJIS();
extern int convJIStoJWS();
extern int convJWStoEUC();
extern int convEUCtoJWS();
extern int convJWStoSJIS();
extern int convSJIStoJWS();
extern wchar_t *wstrcat();
extern wchar_t *wstrncat();
extern int wstrcmp();
extern int wstrncmp();
extern wchar_t *wstrcpy();
extern wchar_t *wstrncpy();
extern int wstrlen();
extern wchar_t *windex();
extern wchar_t *wrindex();
#endif

#endif
