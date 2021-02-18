/*
 * Copyright 1989 FUJITSU LIMITED
 *
 * Permission to use, copy, modify, and distribute this software and its
 * documentation for any purpose and without fee is hereby granted, provided
 * that the above copyright notice appear in all copies and that both that
 * copyright notice and this permission notice appear in supporting
 * documentation, and that the name of FUJITSU not be used in advertising
 * or publicity pertaining to distribution of the software without specific,
 * written prior permission.  FUJITSU makes no representations about the
 * suitability of this software for any purpose.  It is provided "as is"
 * without express or implied warranty.
 *
 * FUJITSU DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING ALL
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN NO EVENT SHALL FUJITSU
 * BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN 
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * Author: Masahiko Narita, FUJITSU LIMITED
 * 	   Yoshitaka Kakuta, FUJITSU LIMITED
 * 	   Shuuichi Akagawa, FUJITSU LIMITED
 *         kakuta@sysrap.ks.fujitsu.co.jp
 */

/* $Header: XJplib.h,v 2.100 89/06/01 08:42:10 rws Exp $ */

/* code */
#define 	XJP_JIS7	0
#define 	XJP_JIS8	1
#define 	XJP_EUC		2
#define 	XJP_SJIS	3

/* character sets */
#define		XJP_ascii	"(B"
#define		XJP_romanX0201	"(J"
#define		XJP_kanaX0201	"(I"
#define		XJP_kanjiX0208	"$B"
#define		XJP_gaiji1byte	"(0"
#define		XJP_gaiji2byte	"$0"

/* display mode of kana-kanji conversion window */
#define		XJP_UNDER	0
#define		XJP_XY		1
#define		XJP_ROOT	2

/* attribute mask of kana-kanji conversion window */
#define		XJP_PWIDTH	0x00000001L
#define		XJP_PHEIGHT	0x00000002L
#define 	XJP_X		0x00000004L
#define		XJP_Y		0x00000008L
#define		XJP_WIDTH	0x00000010L
#define		XJP_HEIGHT	0x00000020L
#define		XJP_FG		0x00000040L
#define		XJP_BG		0x00000080L
#define		XJP_BP		0x00000100L
#define		XJP_FONTS	0x00000200L
#define		XJP_STATUS	0x00000400L

/* font structure */
typedef struct {
	char		*id;
	char		*fontname;
} XJp_fontInfo;

/* kana-kanji conversion window attributes */
typedef struct {
	long 		p_width;
	long		p_height;
	long		x;
	long		y;
	long		width;
	long		height;
	unsigned long	fg;
	unsigned long	bg;
	long		bp;
	long		status_x;
	long		status_y;
	long		status_width;
	long		status_height;
	XJp_fontInfo	*fonts;
	int		num_fonts;
}XJp_attributes;

typedef unsigned long	XJp_id;
extern	int 		XJp_errno;
