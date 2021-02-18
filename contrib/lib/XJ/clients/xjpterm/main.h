
/*
 *	$XConsortium: main.h,v 1.3 88/09/06 17:08:12 jim Exp $
 *	$Header: main.h,v 1.1 89/05/30 14:53:48 kagotani Rel $
 */


#include <X11/copyright.h>

/*
 *	Copyright 1989 OMRON TATEISI ELECTRONICS CO.
 *
 *	Original Source: ./client/kterm/main.h
 *
 *	Modified by OMRON TATEISI ELECTRONICS CO., to handle XJp-library.
 *
 *	Author:	Seiji Kuwari	OMRON TATEISI ELECTRONICS CO.
 *				kuwa@frf.omron.junet
 *				uunet!nff.ncl.omron.co.jp!kuwa
 */				

/*
 * Copyright 1987 by Digital Equipment Corporation, Maynard, Massachusetts.
 *
 *                         All Rights Reserved
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted,
 * provided that the above copyright notice appear in all copies and that
 * both that copyright notice and this permission notice appear in
 * supporting documentation, and that the name of Digital Equipment
 * Corporation not be used in advertising or publicity pertaining to
 * distribution of the software without specific, written prior permission.
 *
 *
 * DIGITAL DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
 * ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL
 * DIGITAL BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR
 * ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION,
 * ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS
 * SOFTWARE.
 */

/* @(#)main.h\tX10/6.6\t11/10/86 */

#ifdef	KANJI
#define	DEFBOLDFONT		" "
#define	DEFBORDER		2
#define	DEFBORDERWIDTH		1
#define	DEFFONT			"a14"
#define	DEFKANAFONT		"kana14"
#define	DEFKANABOLDFONT		" "
#define	DEFKANJIFONT		"k14"
#define	DEFKANJIBOLDFONT	" "
#if defined(FACOM) || defined(EWS) || defined(UX386) || defined(SUN)
#define DEFKANJICODE		"euc"
#endif
#else	KANJI
#define	DEFBOLDFONT		"fixed"
#define	DEFBORDER		2
#define	DEFBORDERWIDTH		2
#define	DEFFONT			"fixed"
#endif	KANJI
